#ifndef LRCTAG_CONTAINERFACTORY_INCLUDE_GUARD
#define LRCTAG_CONTAINERFACTORY_INCLUDE_GUARD

#include <filesystem>
#include "container.h"
#include "../config.h"

namespace LrcTag {
    class ContainerFactory {
        private:
            const Config& m_config;
        public:
            ContainerFactory(const Config& config)
                : m_config(config) { }
            ~ContainerFactory() { }

            /* returns nullptr if file is not supported */
            Container* fromPath(const std::filesystem::path& path);
    };
}

#endif
