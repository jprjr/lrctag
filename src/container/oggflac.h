#ifndef LRCTAG_CONTAINER_OGG_FLAC_INCLUDE_GUARD
#define LRCTAG_CONTAINER_OGG_FLAC_INCLUDE_GUARD

#include "ogg.h"

#include <oggflacfile.h>

namespace LrcTag {
    class OggFLACContainer: public OggContainer {
        public:
            OggFLACContainer(const Config& config, const std::filesystem::path& path, TagLib::FileStream* fs)
            : OggContainer(config, path, static_cast<TagLib::File*>(&m_file)), m_file(fs) {
                OggContainer::setTag();
            }

    
            ~OggFLACContainer() override {
            }
        
        private:
            TagLib::Ogg::FLAC::File m_file;
    };
}

#endif
