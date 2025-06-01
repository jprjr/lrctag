#ifndef LRCTAG_TAGHANDLER_APE_INCLUDE_GUARD
#define LRCTAG_TAGHANDLER_APE_INCLUDE_GUARD

#include "taghandler.h"
#include "../lrc.h"

#include <apetag.h>

namespace LrcTag {
    class APETagHandler: public TagHandler {
        private:
            TagLib::APE::Tag* m_tag;

        public:
            APETagHandler(const Config& config, TagLib::APE::Tag* tag)
                : TagHandler(config, static_cast<TagLib::Tag*>(tag)), m_tag(tag) {
            }

            ~APETagHandler() { }

            bool hasTag() const {
                return m_tag != NULL;
            }

            void setTag(TagLib::APE::Tag* tag) {
                m_tag = tag;
                TagHandler::setTag(static_cast<TagLib::Tag*>(m_tag));
            }

            bool supportsSynchronizedLyrics() const override {
                return TagHandler::m_config.synchronized_lyrics;
            }

            bool supportsUnsynchronizedLyrics() const override {
                return TagHandler::m_config.unsynchronized_lyrics;
            }

            bool hasSynchronizedLyrics() const override {
                if(m_tag == NULL) return false;
                if(TagHandler::m_config.ape_synched_tagname.size() == 0) return false;

                return TagHandler::m_config.synchronized_lyrics &&
                  m_tag->itemListMap().contains(TagHandler::m_config.ape_synched_tagname.upper());
            }

            bool hasUnsynchronizedLyrics() const override {
                if(m_tag == NULL) return false;
                if(TagHandler::m_config.ape_unsynched_tagname.size() == 0) return false;

                return TagHandler::m_config.unsynchronized_lyrics &&
                  m_tag->itemListMap().contains(TagHandler::m_config.ape_unsynched_tagname.upper());
            }

            std::vector<SynchedLyric> getSynchronizedLyrics(unsigned int len) const override {
                if(m_tag == NULL) return std::vector<SynchedLyric>();
                if(TagHandler::m_config.synchronized_lyrics == false) return std::vector<SynchedLyric>();
                if(TagHandler::m_config.ape_synched_tagname.size() == 0) return std::vector<SynchedLyric>();

                return LRC::parse(icu::UnicodeString::fromUTF8(m_tag->itemListMap().value(TagHandler::m_config.ape_synched_tagname.upper()).toString().to8Bit(true)), len);
            }

            icu::UnicodeString getUnsynchronizedLyrics() const override {
                if(m_tag == NULL) return icu::UnicodeString();
                if(TagHandler::m_config.unsynchronized_lyrics == false) return icu::UnicodeString();
                if(TagHandler::m_config.ape_unsynched_tagname.size() == 0) return icu::UnicodeString();

                return icu::UnicodeString::fromUTF8(m_tag->itemListMap().value(TagHandler::m_config.ape_unsynched_tagname.upper()).toString().to8Bit(true));
            }

            bool setSynchronizedLyrics(const std::vector<SynchedLyric>& lyrics) override {
                if(m_tag == NULL) return false;
                if(TagHandler::m_config.synchronized_lyrics == false) return false;
                if(TagHandler::m_config.ape_synched_tagname.size() == 0) return false;

                m_tag->addValue(TagHandler::m_config.ape_synched_tagname,
                    TagLib::String(String::toUTF8(LRC::format(lyrics)), TagLib::String::UTF8), true);
                return true;
            }

            bool setUnsynchronizedLyrics(const icu::UnicodeString& lyrics) override {
                if(m_tag == NULL) return false;
                if(TagHandler::m_config.unsynchronized_lyrics == false) return false;
                if(TagHandler::m_config.ape_unsynched_tagname.size() == 0) return false;

                m_tag->addValue(TagHandler::m_config.ape_unsynched_tagname,
                    TagLib::String(String::toUTF8(lyrics), TagLib::String::UTF8), true);
                return true;
            }
    };
}

#endif
