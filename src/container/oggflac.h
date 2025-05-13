#ifndef LRCTAG_CONTAINER_OGG_FLAC_INCLUDE_GUARD
#define LRCTAG_CONTAINER_OGG_FLAC_INCLUDE_GUARD

#include "ogg.h"

#include <oggflacfile.h>

namespace LrcTag {
    class OggFLACContainer: public OggContainer {
        public:
            OggFLACContainer(const std::filesystem::path& path, TagLib::FileStream* fs)
            : OggContainer(path), m_file(fs) {
                OggContainer::setFile(&m_file);
            }

    
            ~OggFLACContainer() override {
            }
        
        private:
            TagLib::Ogg::FLAC::File m_file;
    };
}

#endif
