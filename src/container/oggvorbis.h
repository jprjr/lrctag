#ifndef LRCTAG_CONTAINER_OGG_VORBIS_INCLUDE_GUARD
#define LRCTAG_CONTAINER_OGG_VORBIS_INCLUDE_GUARD

#include "ogg.h"

#include <vorbisfile.h>

namespace LrcTag {
    class OggVorbisContainer: public OggContainer {
        public:
            OggVorbisContainer(const std::filesystem::path& path, TagLib::FileStream* fs)
            : OggContainer(path), m_file(fs) {
                OggContainer::setFile(&m_file);
            }
    
            ~OggVorbisContainer() override { }
        
        private:
            TagLib::Ogg::Vorbis::File m_file;
    };
}

#endif
