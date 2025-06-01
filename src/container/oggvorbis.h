#ifndef LRCTAG_CONTAINER_OGG_VORBIS_INCLUDE_GUARD
#define LRCTAG_CONTAINER_OGG_VORBIS_INCLUDE_GUARD

#include "ogg.h"

#include <vorbisfile.h>

namespace LrcTag {
    class OggVorbisContainer: public OggContainer {
        public:
            OggVorbisContainer(const Config& config, const std::filesystem::path& path, TagLib::FileStream* fs)
            : OggContainer(config, path, static_cast<TagLib::File*>(&m_file)), m_file(fs) {
                OggContainer::setTag();
            }
    
            ~OggVorbisContainer() override { }
        
        private:
            TagLib::Ogg::Vorbis::File m_file;
    };
}

#endif
