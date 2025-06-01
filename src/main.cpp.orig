#include <spdlog/spdlog.h>
#include <spdlog/cfg/env.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <iostream>
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
#include "report.h"
#include "csv.h"
#include "progress.h"

#include "lyricsource/lyricsourcefactory.h"

static LRCTAGDebugListener debugListener;

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
    TagLib::setDebugListener(&debugListener);

    std::vector<std::filesystem::path> files = LrcTag::FsScanner(c).getFiles();
    spdlog::debug("scanning {} files", files.size());

    std::vector<LrcTag::Worker> workers;
    std::vector<std::thread> threads;
    std::atomic<unsigned long long> index(0);
    LrcTag::Progress progress(c, files.size());

    for(int i = 0; i < c.threads; ++i) {
        workers.emplace_back(c, i, index, files, progress);
    }

    if(c.reportmode) {
        std::vector<LrcTag::Report> reports;
        reports.resize(files.size());
        std::sort(files.begin(), files.end());

        for(int i = 0; i < c.threads; ++i) {
            threads.emplace_back(&LrcTag::Worker::report,
              workers.at(i), std::ref(reports));
        }

        for(auto it = threads.begin(); it != threads.end(); ++it) {
            it->join();
        }

        const std::filesystem::path* p = files.data();
        const LrcTag::Report* r = reports.data();

        std::cout << "path,";
        std::cout << LrcTag::Report::header() << std::endl;
        for(unsigned long i = 0; i < files.size(); ++i) {
            LrcTag::CSV::escape(std::cout, p[i].string());
            std::cout << "," << r[i].string() << std::endl;
        }
    } else if(c.synchronized_lyrics || c.unsynchronized_lyrics) {
        LrcTag::LyricSourceFactory lsf(c);
        LrcTag::LyricDestFactory lsd(c);

        for(int i = 0; i < c.threads; ++i) {
            threads.emplace_back(&LrcTag::Worker::work,
              workers.at(i), std::ref(lsf), std::ref(lsd));
        }

        for(auto it = threads.begin(); it != threads.end(); ++it) {
            it->join();
        }
    }

    spdlog::trace("main return 0");

    LrcTag::Regex::releasePatterns();
    LrcTag::LRC::releasePatterns();
    return 0;
}
