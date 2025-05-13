#ifndef LRCTAG_CONTAINER_OGG_SPEEX_INCLUDE_GUARD
#define LRCTAG_CONTAINER_OGG_SPEEX_INCLUDE_GUARD

#include "ogg.h"

#include <speexfile.h>

namespace LrcTag {
    class OggSpeexContainer: public OggContainer {
        public:
            OggSpeexContainer(const std::filesystem::path& path, TagLib::FileStream* fs)
            : OggContainer(path), m_file(fs) {
                OggContainer::setFile(&m_file);
            }
    
            ~OggSpeexContainer() override {
            }
        
        private:
            TagLib::Ogg::Speex::File m_file;
    };
}

#endif

