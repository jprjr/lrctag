#ifndef LRCTAG_CONTAINER_FLAC_INCLUDE_GUARD
#define LRCTAG_CONTAINER_FLAC_INCLUDE_GUARD

#include "container.h"
#include "../taghandler/xiphcomment.h"

#include <flacfile.h>

namespace LrcTag {
    class FLACContainer: public ContainerBase {
        public:
            FLACContainer(const std::filesystem::path& path, TagLib::FileStream* fs)
            : ContainerBase(path), m_file(fs), m_tag(m_file.xiphComment(true)) {
            }
    
            ~FLACContainer() override {
            }
    
            XiphTagHandler* tag() override {
                return &m_tag;
            }

            const XiphTagHandler* tag() const override {
                return &m_tag;
            }

            TagLib::AudioProperties* audioProperties() const override {
                return m_file.audioProperties();
            }

            void save() override {
                m_file.save();
            }
        
        private:
            TagLib::FLAC::File m_file;
            XiphTagHandler m_tag;
    };
}

#endif

