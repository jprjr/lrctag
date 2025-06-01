#ifndef LRCTAG_CONTAINER_OGG_OPUS_INCLUDE_GUARD
#define LRCTAG_CONTAINER_OGG_OPUS_INCLUDE_GUARD

#include "ogg.h"

#include <opusfile.h>

namespace LrcTag {
    class OggOpusContainer: public OggContainer {
        public:
            OggOpusContainer(const Config& config, const std::filesystem::path& path, TagLib::FileStream* fs)
            : OggContainer(config, path, static_cast<TagLib::File*>(&m_file)), m_file(fs) {
                OggContainer::setTag();
            }
    
            ~OggOpusContainer() override { }
        
        private:
            TagLib::Ogg::Opus::File m_file;
    };
}

#endif
