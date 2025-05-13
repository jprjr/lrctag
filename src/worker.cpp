#include "worker.h"

#include "utils.h"
#include "unicode.h"
#include "debuglistener.h"
#include "container/containerfactory.h"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <thread>
#include <chrono>

#include <cstdio>

namespace LrcTag {
    void Worker::work(const LyricSourceFactory& lsf, const LyricDestFactory& lsd, std::atomic<unsigned long long>& index, std::vector<LrcTag::Result>& results) {
        unsigned long long i;
        auto max = results.size();

        std::string thread_name = lrctag_string_format("thread %d", m_id + 1);
        auto logger = spdlog::stderr_color_mt(thread_name);
        LRCTAGDebugListener::setLogger(logger);

        while( (i = index.fetch_add(1, std::memory_order_relaxed)) < max) {
            logger->trace("working on file #{}", i);
            LrcTag::Result& r = results[i];
            process(logger, lsf, lsd, r);
        }
        logger->trace("thread terminating", i);
    }

    void Worker::process(const std::shared_ptr<spdlog::logger> logger, const LyricSourceFactory& lsf, const LyricDestFactory& lsd, LrcTag::Result& result) {
        logger->debug("processing {}", result.path.string());
    
        Container* c = ContainerFactory::fromPath(result.path);
        if(c == nullptr) {
            logger->warn("{}: unknown file type", result.path.string());
            return;
        }

        bool find_unsynched = false;
        bool find_synched = false;
        bool found_unsynched = false;
        bool found_synched = false;

        std::vector<std::pair<std::string, LyricDest*> > destinations;
        for(std::size_t i = 0; i < lsd.size(); ++i) {
            LyricDest* d = lsd.create(i, c);
            destinations.push_back(std::pair(lsd.name(i), d));
            if(d->needsSynchronizedLyrics()) find_synched = true;
            if(d->needsUnsynchronizedLyrics()) find_unsynched = true;
        }

        icu::UnicodeString ul;
        std::vector<SynchedLyric> sl;

        for(std::size_t i = 0; i < lsf.size(); ++i) {
            if(find_unsynched == false && find_synched == false) break;

            LyricSource* ls = lsf.create(i, c);

            if(find_unsynched) {
                logger->debug("{}: trying unsynchronized lyrics source '{}'", result.path.string(), lsf.name(i));
                if(ls->hasUnsynchronizedLyrics()) {
                    ul = ls->getUnsynchronizedLyrics();
                    if(ul.length() > 0) {
                        find_unsynched = false;
                        found_unsynched = true;
                        logger->debug("{}: found unsynched lyrics via source '{}'", result.path.string(), lsf.name(i));
                    }
                }
            }

            if(find_synched) {
                logger->debug("{}: trying synchronized lyrics source '{}'", result.path.string(), lsf.name(i));
                if(ls->hasSynchronizedLyrics()) {
                    sl = ls->getSynchronizedLyrics();
                    if(sl.size() > 0) {
                        find_synched = false;
                        found_synched = true;
                        logger->debug("{}: found synched lyrics via source '{}'", result.path.string(), lsf.name(i));
                    }
                }
            }

            delete ls;
        }

        for(auto it = destinations.begin(); it != destinations.end(); ++it) {
            LyricDest* d = it->second;
            if(found_unsynched) {
                logger->debug("{}: saving to unsynched lyrics destination '{}'", result.path.string(), it->first);
                d->saveUnsynchronizedLyrics(ul);
            }
            if(found_synched) {
                logger->debug("{}: saving to synched lyrics destination '{}'", result.path.string(), it->first);
                d->saveSynchronizedLyrics(sl);
            }
            delete d;
        }
    
        delete c;
        std::this_thread::sleep_for(std::chrono::milliseconds(3));
    }

}

