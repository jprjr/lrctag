#ifndef LRCTAG_LYRIC_DEST_FACTORY_INCLUDE_GUARD
#define LRCTAG_LYRIC_DEST_FACTORY_INCLUDE_GUARD

#include <vector>
#include <string>

#include "lyricdest.h"
#include "../container/container.h"
#include "../config.h"

namespace LrcTag {
    class LyricDestFactory {
        private:
            const Config& m_config;
            std::vector<std::pair<std::string, LyricDest* (*)(const Config&, Container*)> > m_providers;
        public:
            LyricDestFactory(const Config& conf);
            ~LyricDestFactory() { }

            std::size_t size() const {
                return m_providers.size();
            }

            LyricDest* create(size_t index, Container* c) const {
                return (m_providers.at(index).second)(m_config, c);
            }

            std::string name(size_t index) const {
                return m_providers.at(index).first;
            }

    };
}

#endif
