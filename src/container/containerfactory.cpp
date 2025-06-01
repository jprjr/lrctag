#include "containerfactory.h"

#include <tfilestream.h>

#include "oggvorbis.h"
#include "oggopus.h"
#include "oggspeex.h"
#include "oggflac.h"
#include "flac.h"
#include "mpeg.h"
#include "ape.h"
#include "mp4.h"
#include "wavpack.h"
#include "trueaudio.h"
#include "mpc.h"

namespace LrcTag {
    Container* ContainerFactory::fromPath(const std::filesystem::path& path) {
        TagLib::FileStream* fs = new TagLib::FileStream(path.c_str(), m_config.reportmode);
        if(!fs->isOpen()) {
            delete fs;
            return nullptr;
        }
    
        if(TagLib::Ogg::Vorbis::File::isSupported(fs)) {
            return new OggVorbisContainer(m_config, path, fs);
        }
        else if(TagLib::Ogg::Opus::File::isSupported(fs)) {
            return new OggOpusContainer(m_config, path, fs);
        }
        else if(TagLib::Ogg::FLAC::File::isSupported(fs)) {
            return new OggFLACContainer(m_config, path, fs);
        }
        else if(TagLib::Ogg::Speex::File::isSupported(fs)) {
            return new OggSpeexContainer(m_config, path, fs);
        }
        else if(TagLib::FLAC::File::isSupported(fs)) {
            return new FLACContainer(m_config, path, fs);
        }
        else if(TagLib::MPEG::File::isSupported(fs)) {
            return new MPEGContainer(m_config, path, fs);
        }
        else if(TagLib::MP4::File::isSupported(fs)) {
            return new MP4Container(m_config, path, fs);
        }
        else if(TagLib::APE::File::isSupported(fs)) {
            return new APEContainer(m_config, path, fs);
        }
        else if(TagLib::WavPack::File::isSupported(fs)) {
            return new WavPackContainer(m_config, path, fs);
        }
        else if(TagLib::TrueAudio::File::isSupported(fs)) {
            return new TrueAudioContainer(m_config, path, fs);
        }
        else if(TagLib::MPC::File::isSupported(fs)) {
            return new MPCContainer(m_config, path, fs);
        }

        delete fs;
        return nullptr;
    }
}
