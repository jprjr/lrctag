#ifndef LRCTAG_CONTAINER_INCLUDE_GUARD
#define LRCTAG_CONTAINER_INCLUDE_GUARD

/* a wrapper for taglib files with container-specific methods
 * for setting lyrics */

#include <filesystem>

#include <tfile.h>
#include <audioproperties.h>
#include "../taghandler/taghandler.h"

namespace LrcTag {
    class Container {
        public:
            virtual ~Container() { }

            virtual const std::filesystem::path& path() const = 0;
            virtual TagHandler* tag() = 0;
            virtual const TagHandler* tag() const = 0;
            virtual TagLib::AudioProperties* audioProperties() const = 0;
            virtual void save() = 0;
    };

    class ContainerBase: public Container {
        private:
            const std::filesystem::path m_path;
        public:
            ContainerBase(const std::filesystem::path& path)
            : m_path(path) { }
            ~ContainerBase() override { }

            const std::filesystem::path& path() const override {
                return m_path;
            }
    };
}

#endif
