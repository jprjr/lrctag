#ifndef LRCTAG_CONTAINER_OGG_OPUS_INCLUDE_GUARD
#define LRCTAG_CONTAINER_OGG_OPUS_INCLUDE_GUARD

#include "ogg.h"

#include <opusfile.h>

namespace LrcTag {
    class OggOpusContainer: public OggContainer {
        public:
            OggOpusContainer(const std::filesystem::path& path, TagLib::FileStream* fs)
            : OggContainer(path), m_file(fs) {
                OggContainer::setFile(&m_file);
            }
    
            ~OggOpusContainer() override { }
        
        private:
            TagLib::Ogg::Opus::File m_file;
    };
}

#endif
