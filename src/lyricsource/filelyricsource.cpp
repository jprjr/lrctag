#include "filelyricsource.h"


#include <spdlog/spdlog.h>
#include <unicode/unistr.h>
#include <unicode/ucsdet.h>

#include "../debuglistener.h"
#include "../utils.h"
#include "../lrc.h"

namespace LrcTag {

    icu::UnicodeString FileLyricSource::loadFile(const std::filesystem::path& p) {
        icu::UnicodeString ret;
        icu::UnicodeString tmp;
        UErrorCode ec = U_ZERO_ERROR;
        int f_ec = 0;

        auto logger = LRCTAGDebugListener::getLogger();

        std::string s = File::slurp(p, f_ec);
        if(f_ec != 0) {
            logger->warn("{}: unable to open for reading", p.string());
            goto cleanup;
        }

        if(s.length() == 0) {
            logger->warn("{}: unable to read", p.string());
            goto cleanup;
        }

        logger->trace("{}: raw bytes length = {}", p.string(), s.length());

        tmp = String::decode(s, ec);
        if(tmp.length() == 0) goto cleanup;

        logger->trace("{}: decoded length = {}", p.string(), tmp.length());

        tmp = String::trim(tmp, ec);
        if(U_FAILURE(ec)) {
            logger->error("{}: unable to trim whitespace ({})", p.string(), u_errorName(ec));
            goto cleanup;
        }
        logger->trace("{}: trimmed length = {}", p.string(), tmp.length());

        tmp = String::norm_eol(tmp, ec);
        if(U_FAILURE(ec)) {
            logger->error("{}: unable to normalize line endings ({})", p.string(), u_errorName(ec));
            goto cleanup;
        }

        logger->trace("{}: eol-normalized length = {}", p.string(), tmp.length());

        ret = tmp;

        cleanup:
        return ret;
    }

    void FileLyricSource::loadUnsynchedLyrics() {
        if(m_unsynched_lyrics_attempted == false) {
            m_unsynched_lyrics_attempted = true;
            auto logger = LRCTAGDebugListener::getLogger();
            logger->debug("loading unsynched file: {}", m_path_txt.string());
            m_unsynched_lyrics = loadFile(m_path_txt);
        }
    }

    void FileLyricSource::loadSynchedLyrics() {
        if(m_synched_lyrics_attempted == false) {
            m_synched_lyrics_attempted = true;
            auto logger = LRCTAGDebugListener::getLogger();
            logger->debug("loading synched file: {}", m_path_lrc.string());
            m_synched_lyrics = LRC::parse(loadFile(m_path_lrc), m_ms);
        }
    }

    FileLyricSource::FileLyricSource(const std::filesystem::path& p, unsigned int song_ms) {
        m_path_lrc = (p.parent_path() / p.stem()) += std::filesystem::path(".lrc");
        m_path_txt = (p.parent_path() / p.stem()) += std::filesystem::path(".txt");
        m_unsynched_lyrics_attempted = false;
        m_synched_lyrics_attempted = false;
        m_ms = song_ms;
    }

}

