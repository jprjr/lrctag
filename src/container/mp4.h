#ifndef LRCTAG_CONTAINER_MP4_INCLUDE_GUARD
#define LRCTAG_CONTAINER_MP4_INCLUDE_GUARD

#include "container.h"
#include "../taghandler/mp4.h"

#include <mp4file.h>

namespace LrcTag {
    class MP4Container: public ContainerBase {
        private:
            TagLib::MP4::File m_file;
            MP4TagHandler m_tag;

        public:
            MP4Container(const Config& config, const std::filesystem::path& path, TagLib::FileStream* fs)
            : ContainerBase(config, path), m_file(fs), m_tag(config, m_file.hasMP4Tag() ? m_file.tag() : NULL) {
            }
    
            ~MP4Container() override {
            }
    
            MP4TagHandler* tag() override {
                return &m_tag;
            }

            const MP4TagHandler* tag() const override {
                return &m_tag;
            }

            TagLib::AudioProperties* audioProperties() const override {
                return m_file.audioProperties();
            }

            void save() override {
                m_file.save();
            }
    };
}

#endif


