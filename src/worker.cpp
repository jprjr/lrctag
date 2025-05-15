#include "worker.h"

#include "debuglistener.h"
#include "container/containerfactory.h"
#include "lyricsource/filelyricsource.h"

#include <thread>
#include <chrono>

#include <cstdio>

namespace LrcTag {

    void Worker::report(std::vector<Report>& reports) {
        LRCTAGDebugListener::setLogger(m_logger);

        unsigned long long i;
        auto max = m_paths.size();

        while( (i = m_index.fetch_add(1, std::memory_order_relaxed)) < max) {
            m_logger->trace("working on file #{}", i);
            const std::filesystem::path& p = m_paths.at(i);
            Container* c = ContainerFactory::fromPath(p);
            Report& r = reports.at(i);
            /* we don't need to worry about having the synched lyrics
             * length correct - so we also don't need to worry about
             * whether the audio file is known container type */
            FileLyricSource fls(p, 0);
            r.has_synched_file = fls.hasSynchronizedLyrics() ? Report::Yes : Report::No;
            r.has_unsynched_file = fls.hasUnsynchronizedLyrics() ? Report::Yes : Report::No;

            if(c != NULL) {
                const auto tag = c->tag();

                if(tag->supportsSynchronizedLyrics(m_config)) {
                    if(tag->hasSynchronizedLyrics(m_config)) {
                        r.has_synched_tag = Report::Yes;
                    } else {
                        r.has_synched_tag = Report::No;
                    }

                } else {
                    r.has_synched_tag = Report::NA;
                }

                if(tag->supportsUnsynchronizedLyrics(m_config)) {
                    if(tag->hasUnsynchronizedLyrics(m_config)) {
                        r.has_unsynched_tag = Report::Yes;
                    } else {
                        r.has_unsynched_tag = Report::No;
                    }

                } else {
                    r.has_unsynched_tag = Report::NA;
                }

                delete c;
            }
        }
        m_logger->trace("thread terminating", i);
    }

    void Worker::work(const LyricSourceFactory& lsf, const LyricDestFactory& lsd) {
        LRCTAGDebugListener::setLogger(m_logger);

        unsigned long long i;
        auto max = m_paths.size();

        while( (i = m_index.fetch_add(1, std::memory_order_relaxed)) < max) {
            m_logger->trace("working on file #{}", i);
            const std::filesystem::path& p = m_paths.at(i);
            process(lsf, lsd, p);
        }
        m_logger->trace("thread terminating", i);
    }

    void Worker::process(const LyricSourceFactory& lsf, const LyricDestFactory& lsd, const std::filesystem::path& p) {
        m_logger->debug("processing {}", p.string());
    
        Container* c = ContainerFactory::fromPath(p);
        if(c == nullptr) {
            m_logger->warn("{}: unknown file type", p.string());
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
                m_logger->debug("{}: trying unsynchronized lyrics source '{}'", p.string(), lsf.name(i));
                if(ls->hasUnsynchronizedLyrics()) {
                    ul = ls->getUnsynchronizedLyrics();
                    if(ul.length() > 0) {
                        find_unsynched = false;
                        found_unsynched = true;
                        m_logger->debug("{}: found unsynched lyrics via source '{}'", p.string(), lsf.name(i));
                    }
                }
            }

            if(find_synched) {
                m_logger->debug("{}: trying synchronized lyrics source '{}'", p.string(), lsf.name(i));
                if(ls->hasSynchronizedLyrics()) {
                    sl = ls->getSynchronizedLyrics();
                    if(sl.size() > 0) {
                        find_synched = false;
                        found_synched = true;
                        m_logger->debug("{}: found synched lyrics via source '{}'", p.string(), lsf.name(i));
                    }
                }
            }

            delete ls;
        }

        for(auto it = destinations.begin(); it != destinations.end(); ++it) {
            LyricDest* d = it->second;
            if(found_unsynched) {
                if(d->needsUnsynchronizedLyrics()) {
                    m_logger->debug("{}: saving to unsynched lyrics destination '{}'", p.string(), it->first);
                    d->saveUnsynchronizedLyrics(ul);
                }
            }
            if(found_synched) {
                if(d->needsSynchronizedLyrics()) {
                    m_logger->debug("{}: saving to synched lyrics destination '{}'", p.string(), it->first);
                    d->saveSynchronizedLyrics(sl);
                }
            }
            delete d;
        }
    
        delete c;
    }

}

