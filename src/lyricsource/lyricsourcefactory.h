#ifndef LRCTAG_LYRIC_SOURCE_FACTORY_INCLUDE_GUARD
#define LRCTAG_LYRIC_SOURCE_FACTORY_INCLUDE_GUARD

#include <vector>
#include <string>

#include "lyricsource.h"
#include "../container/container.h"
#include "../config.h"

namespace LrcTag {
    class LyricSourceFactory {
        private:
            const Config& m_config;
            std::vector<std::pair<std::string, LyricSource* (*)(const Config&, const Container* c)> > m_providers;

        public:
            LyricSourceFactory(const Config& conf);
            ~LyricSourceFactory() { }

            std::size_t size() const {
                return m_providers.size();
            }

            LyricSource* create(size_t index, Container* c) const {
                return (m_providers.at(index).second)(m_config, c);
            }

            std::string name(size_t index) const {
                return m_providers.at(index).first;
            }
    };
}

#endif
