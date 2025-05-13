#include "filelyricdest.h"

#include <mutex>
#include <set>

#include "../debuglistener.h"
#include "../lrc.h"
#include "../utils.h"

static std::set<std::filesystem::path> txt_paths;
static std::set<std::filesystem::path> lrc_paths;
static std::mutex txt_mutex;
static std::mutex lrc_mutex;

namespace LrcTag {

    FileLyricDest::FileLyricDest(const Config& config, const std::filesystem::path& p) : m_config(config) {
        m_path_lrc = (p.parent_path() / p.stem()) += std::filesystem::path(".lrc");
        m_path_txt = (p.parent_path() / p.stem()) += std::filesystem::path(".txt");
    }

    bool FileLyricDest::shouldWriteLRC() {
        bool should_write = false;
        std::lock_guard<std::mutex> guard(lrc_mutex);
        if(lrc_paths.count(m_path_lrc) == 0) {
            should_write = true;
            lrc_paths.insert(m_path_lrc);
        }
        return should_write;
    }

    bool FileLyricDest::shouldWriteTXT() {
        bool should_write = false;
        std::lock_guard<std::mutex> guard(txt_mutex);
        if(lrc_paths.count(m_path_txt) == 0) {
            should_write = true;
            txt_paths.insert(m_path_txt);
        }
        return should_write;
    }

    bool FileLyricDest::needsSynchronizedLyrics() {
        return m_config.synchronized_lyrics && (m_config.overwrite_files || (!(std::filesystem::exists(m_path_lrc))));
    }

    bool FileLyricDest::needsUnsynchronizedLyrics() {
        return m_config.unsynchronized_lyrics && (m_config.overwrite_files || (!(std::filesystem::exists(m_path_txt))));
    }

    bool FileLyricDest::saveSynchronizedLyrics(const std::vector<SynchedLyric>& sl) {
        auto logger = LRCTAGDebugListener::getLogger();
        if(shouldWriteLRC()) {
            if(needsSynchronizedLyrics()) {
                logger->debug("saving synchronized lyrics to {}", m_path_lrc.string());
                File::dump(m_path_lrc, LRC::format(sl));
            }
        }
        else {
            logger->debug("did not get lock for {}, not writing", m_path_lrc.string());
        }
        /* we just assume saving worked */
        return true;
    }

    bool FileLyricDest::saveUnsynchronizedLyrics(const icu::UnicodeString& ul) {
        auto logger = LRCTAGDebugListener::getLogger();
        logger->debug("evaluating whether to write unsych file");
        if(shouldWriteTXT()) {
            if(needsUnsynchronizedLyrics()) {
                logger->debug("saving unsynchronized lyrics to {}", m_path_txt.string());
                File::dump(m_path_txt, ul);
            } else {
                logger->debug("not saving unsychronized lyrics - needs returned false");
            }
        }
        else {
            logger->debug("did not get lock for {}, not writing", m_path_txt.string());
        }
        /* we just assume saving worked */
        return true;
    }

}
