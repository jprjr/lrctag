#include "containerfactory.h"

#include <tfilestream.h>

#include "oggvorbis.h"
#include "oggopus.h"
#include "oggspeex.h"
#include "oggflac.h"
#include "flac.h"

namespace LrcTag {
    Container* ContainerFactory::fromPath(const std::filesystem::path& path) {
        TagLib::FileStream* fs = new TagLib::FileStream(path.c_str());
        if(!fs->isOpen()) {
            delete fs;
            return nullptr;
        }
    
        if(TagLib::Ogg::Vorbis::File::isSupported(fs)) {
            return new OggVorbisContainer(path, fs);
        }
        else if(TagLib::Ogg::Opus::File::isSupported(fs)) {
            return new OggOpusContainer(path, fs);
        }
        else if(TagLib::Ogg::FLAC::File::isSupported(fs)) {
            return new OggFLACContainer(path, fs);
        }
        else if(TagLib::Ogg::Speex::File::isSupported(fs)) {
            return new OggSpeexContainer(path, fs);
        }
        else if(TagLib::FLAC::File::isSupported(fs)) {
            return new FLACContainer(path, fs);
        }

        delete fs;
        return nullptr;
    }
}
