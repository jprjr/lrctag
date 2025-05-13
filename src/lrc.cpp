#include "lrc.h"
#include "debuglistener.h"

#include "utils.h"

#include <spdlog/spdlog.h>

static const icu::UnicodeString BRACKET_LEFT  = icu::UnicodeString::fromUTF8("[");
static const icu::UnicodeString BRACKET_RIGHT = icu::UnicodeString::fromUTF8("]");
static const icu::UnicodeString SPACE    = icu::UnicodeString::fromUTF8(" ");
static const icu::UnicodeString COLON    = icu::UnicodeString::fromUTF8(":");
static const icu::UnicodeString DOT      = icu::UnicodeString::fromUTF8(".");
static const icu::UnicodeString NEWLINE  = icu::UnicodeString::fromUTF8("\n");

namespace LrcTag {

    const icu::UnicodeString LRC::LINE_PATTERN = icu::UnicodeString::fromUTF8("\\[([0-9]{2}):([0-9]{2})\\.([0-9]{2})\\][\\t ]*([^\\n]*)(?:\\n|$)");

    icu::RegexPattern *LRC::LINE = NULL;

    bool LRC::compilePatterns() {
        UParseError pe;
        UErrorCode ec = U_ZERO_ERROR;

        LINE = icu::RegexPattern::compile(LINE_PATTERN,
          pe, ec);
        if(U_FAILURE(ec)) {
            spdlog::critical("failed to compile LINE_PATTERN: {}", u_errorName(ec));
            return false;
        }
        return true;
    }

    void LRC::releasePatterns() {
        delete LINE;
    }

    std::vector<SynchedLyric> LRC::parse(const icu::UnicodeString& src, unsigned int end_ms) {
        std::vector<SynchedLyric> lyrics;
        unsigned int start_ms = 0;
        UErrorCode ec = U_ZERO_ERROR;
        icu::RegexMatcher* matcher = NULL;
        auto logger = LRCTAGDebugListener::getLogger();

        matcher = LINE->matcher(src, ec);

        if(U_FAILURE(ec)) {
            logger->critical("error creating matcher: {}", u_errorName(ec));
            return lyrics;
        }

        while(matcher->find(ec)) {
            ec = U_ZERO_ERROR;
            icu::UnicodeString mm = matcher->group(1, ec);
            if(U_FAILURE(ec)) break;

            ec = U_ZERO_ERROR;
            icu::UnicodeString ss = matcher->group(2, ec);
            if(U_FAILURE(ec)) break;

            ec = U_ZERO_ERROR;
            icu::UnicodeString ms = matcher->group(3, ec);
            if(U_FAILURE(ec)) break;

            ec = U_ZERO_ERROR;
            icu::UnicodeString l = matcher->group(4, ec);
            if(U_FAILURE(ec)) break;

            start_ms = (String::parseInt(mm,10) * 60000) +
                       (String::parseInt(ss,10) * 1000) +
                       (String::parseInt(ms,10) * 10);

            if(lyrics.size() > 0) {
                SynchedLyric& prev = lyrics.back();
                if(prev.m_end == 0) {
                    prev.m_end = start_ms;
                }
            }
            if(l.length() > 0) {
                lyrics.emplace_back(l, start_ms, 0);
            }
        }

        delete matcher;

        if(U_FAILURE(ec)) {
            logger->critical("error matching LRC regex: {}", u_errorName(ec));
            lyrics.clear();
        }

        if(lyrics.size() > 0) {
            SynchedLyric& prev = lyrics.back();
            if(prev.m_end == 0) {
                prev.m_end = end_ms;
            }
            for(auto it = lyrics.cbegin(); it != lyrics.cend(); ++it) {
                logger->trace("{} -> {}: {}",
                  it->m_start, it->m_end, String::toUTF8(it->m_txt));
            }
        }
    
        return lyrics;
    }

    icu::UnicodeString LRC::format(const std::vector<SynchedLyric>& src) {
        auto logger = LRCTAGDebugListener::getLogger();
        icu::UnicodeString ret;

        for(std::size_t i = 0; i < src.size(); ) {
            icu::UnicodeString mm = String::formatInt(src[i].m_start / 60000, 2);
            icu::UnicodeString ss = String::formatInt((src[i].m_start % 60000) / 1000, 2);
            icu::UnicodeString ms = String::formatInt((src[i].m_start % 1000) / 10, 2);

            ret += BRACKET_LEFT + mm + COLON + ss + DOT + ms + BRACKET_RIGHT + SPACE + src[i].m_txt + NEWLINE;

            i++;
            if((i == src.size()) || (src[i-1].m_end != src[i].m_start)) {
                ret += BRACKET_LEFT
                  + String::formatInt(src[i-1].m_end / 60000, 2)
                  + COLON
                  + String::formatInt((src[i-1].m_end % 60000 / 1000), 2)
                  + DOT
                  + String::formatInt((src[i-1].m_end % 1000 / 10), 2)
                  + BRACKET_RIGHT;
                if(i != src.size()) ret += NEWLINE;
            }
        }

        logger->trace("{}", String::toUTF8(ret));
        return ret;
    }

}
