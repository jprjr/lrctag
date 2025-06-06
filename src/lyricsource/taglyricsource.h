#ifndef LRCTAG_TAG_LYRICSOURCE_INCLUDE_GUARD
#define LRCTAG_TAG_LYRICSOURCE_INCLUDE_GUARD

#include "lyricsource.h"
#include "../config.h"
#include "../container/container.h"

namespace LrcTag {
    class TagLyricSource: public LyricSource {
        const Config& m_config;
        const Container* m_container;

        public:
            TagLyricSource(const Config& conf, const Container* container)
            : m_config(conf), m_container(container) { }
            ~TagLyricSource() override { }

            bool hasSynchronizedLyrics() override {
                const auto tag = m_container->tag();
                return tag->supportsSynchronizedLyrics() &&
                  tag->hasSynchronizedLyrics();
            }

            bool hasUnsynchronizedLyrics() override {
                const auto tag = m_container->tag();
                return tag->supportsUnsynchronizedLyrics() &&
                  tag->hasUnsynchronizedLyrics();
            }

            icu::UnicodeString getUnsynchronizedLyrics() override {
                const auto tag = m_container->tag();
                return tag->getUnsynchronizedLyrics();
            }

            std::vector<SynchedLyric> getSynchronizedLyrics() override {
                const auto tag = m_container->tag();
                return tag->getSynchronizedLyrics(m_container->audioProperties()->lengthInMilliseconds());
            }
    };
}

#endif

