#ifndef LRCTAG_TAGHANDLER_ID3V2_INCLUDE_GUARD
#define LRCTAG_TAGHANDLER_ID3V2_INCLUDE_GUARD

#include "taghandler.h"
#include "../utils.h"

#include <id3v2tag.h>
#include <synchronizedlyricsframe.h>
#include <unsynchronizedlyricsframe.h>

namespace LrcTag {
    class ID3v2TagHandler: public TagHandler {
        private:
            TagLib::ID3v2::Tag* m_tag;
            TagLib::ID3v2::SynchronizedLyricsFrame* m_sylt;
            TagLib::ID3v2::UnsynchronizedLyricsFrame* m_uslt;

            TagLib::ID3v2::SynchronizedLyricsFrame* findSynchedLyric() const;
            TagLib::ID3v2::UnsynchronizedLyricsFrame* findUnsynchedLyric() const;
            
        public:
            ID3v2TagHandler(const Config& config, TagLib::ID3v2::Tag* tag)
                : TagHandler(config, static_cast<TagLib::Tag*>(tag)), m_tag(tag) {
                    m_sylt = findSynchedLyric();
                    m_uslt = findUnsynchedLyric();
            }
            ~ID3v2TagHandler() { }

            bool hasTag() const {
                return m_tag != NULL;
            }

            void setTag(TagLib::ID3v2::Tag* tag) {
                m_tag = tag;
                m_sylt = findSynchedLyric();
                m_uslt = findUnsynchedLyric();
                TagHandler::setTag(static_cast<TagLib::Tag*>(m_tag));
            }

            bool supportsSynchronizedLyrics() const override {
                return TagHandler::m_config.synchronized_lyrics;
            }

            bool supportsUnsynchronizedLyrics() const override {
                return TagHandler::m_config.unsynchronized_lyrics;
            }

            bool hasSynchronizedLyrics() const override {
                return TagHandler::m_config.synchronized_lyrics && m_sylt != NULL;
            }

            bool hasUnsynchronizedLyrics() const override {
                return TagHandler::m_config.unsynchronized_lyrics && m_uslt != NULL;
            }

            std::vector<SynchedLyric> getSynchronizedLyrics(unsigned int len) const override;

            icu::UnicodeString getUnsynchronizedLyrics() const override;

            bool setSynchronizedLyrics(const std::vector<SynchedLyric>& lyrics) override;

            bool setUnsynchronizedLyrics(const icu::UnicodeString& lyrics) override;

    };
}

#endif
