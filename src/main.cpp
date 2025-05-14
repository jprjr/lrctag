#include <cstdio>

#include <spdlog/spdlog.h>
#include <spdlog/cfg/env.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <vector>
#include <filesystem>
#include <set>
#include <thread>
#include <atomic>
#include <algorithm>

#include <unicode/uclean.h>
#include <unicode/ustring.h>

#include <curl/curl.h>

#include "config.h"
#include "worker.h"
#include "debuglistener.h"
#include "utils.h"
#include "lrc.h"
#include "scanner.h"

#include "lyricsource/lyricsourcefactory.h"

static void setup_logger(void) {
    auto sink = std::make_shared<spdlog::sinks::stderr_color_sink_mt>();
    spdlog::set_default_logger(std::make_shared<spdlog::logger>("main thread", std::move(sink)));
    spdlog::cfg::load_env_levels("LRCTAG_LOGLEVEL");
    LRCTAGDebugListener::setLogger(spdlog::default_logger());
}

static bool init_unicode(void) {
    UErrorCode code = U_ZERO_ERROR;
    u_init(&code);
    if(U_FAILURE(code)) return false;
    return true;
}

static LRCTAGDebugListener debugListener;

int main(int argc, const char* argv[]) {
    LrcTag::Config c;
    setup_logger();

    if(curl_global_init(CURL_GLOBAL_ALL) != 0) {
        spdlog::critical("error initializing libcurl");
        return 1;
    }

    if(!init_unicode()) {
        spdlog::critical("error initializing libicu");
        return 1;
    }

    if(!LrcTag::String::initNFKC()) {
        spdlog::critical("error creating NFKC instance");
        return 1;
    }

    if(!LrcTag::Regex::compilePatterns()) {
        spdlog::critical("error compiling regex patterns");
        return 1;
    }

    if(!LrcTag::LRC::compilePatterns()) {
        spdlog::critical("error compiling LRC parser regex patterns");
        return 1;
    }

    if(!c.argparse(argc, argv)) {
        spdlog::trace("main return 1");
        return 1;
    }

    if(c.synchronized_lyrics || c.unsynchronized_lyrics) {
        LrcTag::LyricSourceFactory lsf(c);
        LrcTag::LyricDestFactory lsd(c);
        LrcTag::FsScanner fs(c);

        fs.scanFiles();

        std::vector<std::filesystem::path> files = fs.getFiles();

        spdlog::debug("scanning {} files", files.size());

        std::vector<std::thread> threads;
        std::atomic<unsigned long long> worker_id(0);
        std::atomic<unsigned long long> index(0);

        TagLib::setDebugListener(&debugListener);

        for(int i = 0; i < c.threads; ++i) {
            threads.emplace_back([&]() {
                LrcTag::Worker w(worker_id.fetch_add(1, std::memory_order_relaxed));
                w.work(lsf, lsd, index, files);
            });
        }

        for(auto it = threads.begin(); it != threads.end(); ++it) {
            it->join();
        }

        std::sort(files.begin(), files.end(), [](const std::filesystem::path& a, const std::filesystem::path& b) {
            return a < b;
        });
    }

    spdlog::trace("main return 0");

    LrcTag::Regex::releasePatterns();
    LrcTag::LRC::releasePatterns();
    return 0;
}
