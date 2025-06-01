#ifndef LRCTAG_TAGHANDLER_MP4_INCLUDE_GUARD
#define LRCTAG_TAGHANDLER_MP4_INCLUDE_GUARD

#include "taghandler.h"
#include "../lrc.h"

#include <mp4tag.h>

namespace LrcTag {
    class MP4TagHandler: public TagHandler {
        private:
            TagLib::MP4::Tag* m_tag;

        public:
            MP4TagHandler(const Config& config, TagLib::MP4::Tag* tag)
                : TagHandler(config, static_cast<TagLib::Tag*>(tag)), m_tag(tag) {
            }

            ~MP4TagHandler() { }

            bool hasTag() const {
                return m_tag != NULL;
            }

            void setTag(TagLib::MP4::Tag* tag) {
                m_tag = tag;
                TagHandler::setTag(static_cast<TagLib::Tag*>(m_tag));
            }

            bool supportsSynchronizedLyrics() const override {
                return TagHandler::m_config.synchronized_lyrics && !TagHandler::m_config.mp4_unsynch;
            }

            bool supportsUnsynchronizedLyrics() const override {
                return TagHandler::m_config.synchronized_lyrics && TagHandler::m_config.mp4_unsynch;
            }

            bool hasSynchronizedLyrics() const override {
                if(m_tag == NULL) return false;

                return TagHandler::m_config.unsynchronized_lyrics &&
                  (!TagHandler::m_config.mp4_unsynch) &&
                  m_tag->contains(TagLib::String("\251lyr", TagLib::String::Latin1));
            }

            bool hasUnsynchronizedLyrics() const override {
                if(m_tag == NULL) return false;

                return TagHandler::m_config.unsynchronized_lyrics &&
                  TagHandler::m_config.mp4_unsynch &&
                  m_tag->contains(TagLib::String("\251lyr", TagLib::String::Latin1));
            }

            std::vector<SynchedLyric> getSynchronizedLyrics(unsigned int len) const override {
                std::vector<SynchedLyric> ret;

                if(m_tag == NULL) return ret;
                if(TagHandler::m_config.synchronized_lyrics == false) return ret;
                if(TagHandler::m_config.mp4_unsynch) return ret;

                return LRC::parse(
                    icu::UnicodeString::fromUTF8(m_tag->item(TagLib::String("\251lyr", TagLib::String::Latin1)).toStringList().toString().to8Bit(true)),
                    len);
            }

            icu::UnicodeString getUnsynchronizedLyrics() const override {
                if(m_tag == NULL) return icu::UnicodeString();
                if(TagHandler::m_config.unsynchronized_lyrics == false) return icu::UnicodeString();
                if(!TagHandler::m_config.mp4_unsynch) return icu::UnicodeString();

                return icu::UnicodeString::fromUTF8(m_tag->item(TagLib::String("\251lyr", TagLib::String::Latin1)).toStringList().toString().to8Bit(true));
            }

            bool setSynchronizedLyrics(const std::vector<SynchedLyric>& lyrics) override {
                if(m_tag == NULL) return false;
                if(TagHandler::m_config.synchronized_lyrics == false) return false;
                if(TagHandler::m_config.mp4_unsynch == true) return false;

                m_tag->setItem(
                  TagLib::String("\251lyr", TagLib::String::Latin1),
                  TagLib::StringList(TagLib::String(String::toUTF8(LRC::format(lyrics)), TagLib::String::UTF8)));
                return true;
            }

            bool setUnsynchronizedLyrics(const icu::UnicodeString& lyrics) override {
                if(m_tag == NULL) return false;
                if(TagHandler::m_config.unsynchronized_lyrics == false) return false;
                if(TagHandler::m_config.mp4_unsynch == false) return false;

                m_tag->setItem(
                  TagLib::String("\251lyr", TagLib::String::Latin1),
                  TagLib::StringList(TagLib::String(String::toUTF8(lyrics), TagLib::String::UTF8)));
                return true;
            }
    };
}

#endif

