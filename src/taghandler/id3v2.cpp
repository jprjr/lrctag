#include "id3v2.h"

namespace LrcTag {

    TagLib::ID3v2::SynchronizedLyricsFrame* ID3v2TagHandler::findSynchedLyric() const {
        TagLib::ID3v2::SynchronizedLyricsFrame* ret = NULL;
        TagLib::ID3v2::SynchronizedLyricsFrame* sylt = NULL;

        if(m_tag == NULL) return NULL;

        auto frames = m_tag->frameList("SYLT");

        for(auto it = frames.cbegin(); it != frames.cend(); ++it) {
            sylt = static_cast<TagLib::ID3v2::SynchronizedLyricsFrame*>(*it);
            if(sylt->type() == TagLib::ID3v2::SynchronizedLyricsFrame::Lyrics) {
                ret = sylt;
                break;
            }
        }

        return ret;
    }

    TagLib::ID3v2::UnsynchronizedLyricsFrame* ID3v2TagHandler::findUnsynchedLyric() const {
        if(m_tag == NULL) return NULL;
        auto frames = m_tag->frameList("USLT");
        if(frames.isEmpty()) return NULL;
        return static_cast<TagLib::ID3v2::UnsynchronizedLyricsFrame*>(frames.front());
    }

    std::vector<SynchedLyric> ID3v2TagHandler::getSynchronizedLyrics(unsigned int len) const {
        std::vector<SynchedLyric> sl;
        UErrorCode ec = U_ZERO_ERROR;
        icu::RegexMatcher* nl_start = NULL;
        icu::RegexMatcher* nl_end = NULL;
        bool prev_newline = false;

        if(m_sylt == NULL) return sl;
    
        nl_start = Regex::NEWLINE_START->matcher(ec);
    
        if(U_FAILURE(ec)) {
            return sl;
        }
    
        nl_end = Regex::NEWLINE_END->matcher(ec);
    
        if(U_FAILURE(ec)) {
            return sl;
        }
    
        auto src = m_sylt->synchedText();
        for(auto it = src.cbegin(); it != src.cend(); ++it) {
    
            icu::UnicodeString line = icu::UnicodeString::fromUTF8(it->text.to8Bit(true));
            nl_start->reset(line);
            nl_end->reset(line);
    
            bool start_match = nl_start->find(ec);
            if(U_FAILURE(ec)) {
                sl.clear();
                break;
            }
    
            bool end_match = nl_end->find(ec);
            if(U_FAILURE(ec)) {
                sl.clear();
                break;
            }
    
            icu::UnicodeString trimmed = nl_start->replaceFirst(icu::UnicodeString(), ec);
            if(U_FAILURE(ec)) {
                sl.clear();
                break;
            }
            nl_end->reset(trimmed);
            trimmed = nl_end->replaceFirst(icu::UnicodeString(), ec);
            if(U_FAILURE(ec)) {
                sl.clear();
                break;
            }

            /* we only remove leading/trailing whitespace when lyric begins/ends
             * with a newline - because in theory, we could have lyrics like:
             *   <00:00.00> \nStart of a line
             *   <00:01.00> a word
             *   <00:02.00> \nStart of another line
             *
             *  in this case, we'd merge lines 1 and 2 together into a single line,
             *  and would want to preserve any spaces */

            if(start_match) {
                /* remove leading whitespace */
                trimmed = String::trimStart(trimmed, ec);
                if(U_FAILURE(ec)) {
                    sl.clear();
                    break;
                }
            }

            if(end_match) {
                /* remove trailing whitespace */
                trimmed = String::trimEnd(trimmed, ec);
                if(U_FAILURE(ec)) {
                    sl.clear();
                    break;
                }
            }
    
            if(start_match || prev_newline) {
                /* if we start with a newline, or our previous entry
                 * ended with a newline - we should set the previous entry
                 * end time to this entry start time. */
                if(sl.size() > 0) {
                    SynchedLyric& prev = sl.back();
                    if(prev.m_end == 0) {
                        prev.m_end = it->time;
                    }
                }
            }

            if(trimmed.length() > 0) {
                if(sl.size() == 0 || start_match || prev_newline) {
                    /* if sl.size() == 0 - this is our first line so
                     * we should add it - no matter whether we've seen any newlines or not.
                     *
                     * Alternatively - if this is a line that began with a newline,
                     * or the previous line ended in a newline - then we also start
                     * a new line entry. */
                    sl.emplace_back(trimmed, it->time, 0);
                } else {
                    /* this is a continutation of a previous entry */
                    /* TODO actually text this out, so far everything I've
                     * seen with synched lyrics in MP3 assumes 1 entry = 1 line */
                    SynchedLyric& prev = sl.back();
                    prev.m_txt += trimmed;
                }
            }

            /* record whether this line had a newline or not as we process the next line */
            prev_newline = end_match;
        }
    
        delete nl_start;
        delete nl_end;

        if(sl.size() > 0) {
            SynchedLyric& fin = sl.back();
            if(fin.m_end == 0) {
                fin.m_end = len;
            }
        }
    
        return sl;
    }

    bool ID3v2TagHandler::setSynchronizedLyrics(const std::vector<SynchedLyric>& lyrics) {
        if(!TagHandler::m_config.synchronized_lyrics) return false;
        if(m_tag == NULL) return false;

        TagLib::ID3v2::SynchronizedLyricsFrame::SynchedTextList sl_list;
        TagLib::ID3v2::SynchronizedLyricsFrame* sl_frame = NULL;

        while( (sl_frame = findSynchedLyric()) != NULL) {
            m_tag->removeFrame(sl_frame);
        }

        sl_frame = new TagLib::ID3v2::SynchronizedLyricsFrame(TagLib::String::UTF8);

        sl_frame->setTimestampFormat(TagLib::ID3v2::SynchronizedLyricsFrame::AbsoluteMilliseconds);
        sl_frame->setType(TagLib::ID3v2::SynchronizedLyricsFrame::Lyrics);

        for(std::size_t i = 0; i < lyrics.size(); ) {
            sl_list.append(TagLib::ID3v2::SynchronizedLyricsFrame::SynchedText(lyrics[i].m_start, TagLib::String("\n ", TagLib::String::UTF8) + TagLib::String(String::toUTF8(lyrics[i].m_txt), TagLib::String::UTF8)));

            i++;

            if((i == lyrics.size()) || (lyrics[i-1].m_end != lyrics[i].m_start)) {
                sl_list.append(TagLib::ID3v2::SynchronizedLyricsFrame::SynchedText(lyrics[i-1].m_end, TagLib::String("\n ", TagLib::String::UTF8)));
            }
        }
        sl_frame->setSynchedText(sl_list);
        m_tag->addFrame(sl_frame);

        return true;

    }

    icu::UnicodeString ID3v2TagHandler::getUnsynchronizedLyrics() const {
        if(m_uslt == NULL) return icu::UnicodeString();

        return icu::UnicodeString::fromUTF8(
          m_uslt->text().to8Bit(true));
    }

    bool ID3v2TagHandler::setUnsynchronizedLyrics(const icu::UnicodeString& lyrics) {
        if(!TagHandler::m_config.synchronized_lyrics) return false;
        if(m_tag == NULL) return false;

        TagLib::ID3v2::UnsynchronizedLyricsFrame* ul_frame = new TagLib::ID3v2::UnsynchronizedLyricsFrame(TagLib::String::UTF8);

        ul_frame->setText(TagLib::String(String::toUTF8(lyrics), TagLib::String::UTF8));

        m_tag->removeFrames("USLT");
        m_tag->addFrame(ul_frame);

        return true;
    }
    
}
