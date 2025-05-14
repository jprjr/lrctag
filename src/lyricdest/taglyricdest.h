#ifndef LRCTAG_TAG_LYRICDEST_INCLUDE_GUARD
#define LRCTAG_TAG_LYRICDEST_INCLUDE_GUARD

#include "lyricdest.h"
#include "../container/container.h"

namespace LrcTag {
    class TagLyricDest: public LyricDest {
        private:
            const Config& m_config;
            Container* m_container;
            bool m_changed;

        public:
            TagLyricDest(const Config& config, Container* c)
            : m_config(config), m_container(c), m_changed(false) { }

            ~TagLyricDest() override {
                if(m_changed) m_container->save();
            }

            bool needsSynchronizedLyrics() override {
                auto tag = m_container->tag();
                return tag->supportsSynchronizedLyrics(m_config) &&
                  (m_config.overwrite_tags || !(tag->hasSynchronizedLyrics(m_config)));
            }

            bool needsUnsynchronizedLyrics() override {
                auto tag = m_container->tag();
                return tag->supportsUnsynchronizedLyrics(m_config) &&
                  (m_config.overwrite_tags || !(tag->hasUnsynchronizedLyrics(m_config)));
            }

            bool saveSynchronizedLyrics(const std::vector<SynchedLyric>& sl) override {
                auto tag = m_container->tag();
                tag->setSynchronizedLyrics(m_config, sl);
                m_changed = true;
                return true;
            }

            bool saveUnsynchronizedLyrics(const icu::UnicodeString& ul) override {
                auto tag = m_container->tag();
                tag->setUnsynchronizedLyrics(m_config, ul);
                m_changed = true;
                return true;
            }

    };
}

#endif
