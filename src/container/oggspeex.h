#ifndef LRCTAG_CONTAINER_OGG_SPEEX_INCLUDE_GUARD
#define LRCTAG_CONTAINER_OGG_SPEEX_INCLUDE_GUARD

#include "ogg.h"

#include <speexfile.h>

namespace LrcTag {
    class OggSpeexContainer: public OggContainer {
        public:
            OggSpeexContainer(const Config& config, const std::filesystem::path& path, TagLib::FileStream* fs)
            : OggContainer(config, path, static_cast<TagLib::File*>(&m_file)), m_file(fs) {
                OggContainer::setTag();
            }
    
            ~OggSpeexContainer() override {
            }
        
        private:
            TagLib::Ogg::Speex::File m_file;
    };
}

#endif

