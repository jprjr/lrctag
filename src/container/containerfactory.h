#ifndef LRCTAG_CONTAINERFACTORY_INCLUDE_GUARD
#define LRCTAG_CONTAINERFACTORY_INCLUDE_GUARD

#include <filesystem>
#include "container.h"

namespace LrcTag {
    class ContainerFactory {
        public:
            /* returns nullptr if file is not supported */
            static Container* fromPath(const std::filesystem::path& path);
    };
}

#endif
