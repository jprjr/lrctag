#include "lyricsourcefactory.h"

#include <spdlog/spdlog.h>
#include <map>

#include "nulllyricsource.h"
#include "filelyricsource.h"
#include "taglyricsource.h"
#include "lrcliblyricsource.h"

namespace LrcTag {

    static LyricSource* LrcLibProviderLoader(const Config& conf, const Container* c) {
        return new LrcLibLyricSource(conf, c);
    }

    static LyricSource* TagProviderLoader(const Config& conf, const Container* c) {
        return new TagLyricSource(conf, c);
    }

    static LyricSource* FileProviderLoader(const Config& conf, const Container* c) {
        (void)conf;
        return new FileLyricSource(c->path(), c->audioProperties()->lengthInMilliseconds());
    }

    static LyricSource* NullProviderLoader(const Config& conf, const Container* c) {
        (void)conf;
        (void)c;
        return new NullLyricSource();
    }

    static const std::map<std::string, LyricSource* (*)(const Config&, const Container*)> LyricSourceFactoryMap = {
        { "lrclib", LrcLibProviderLoader  },
        { "tag",    TagProviderLoader  },
        { "file",   FileProviderLoader },
        { "null",   NullProviderLoader },
    };

    LyricSourceFactory::LyricSourceFactory(const Config& config) : m_config(config) {
        for(auto it = config.sources.cbegin(); it != config.sources.cend(); ++it) {

            spdlog::debug("Enabling lyrics source {}", *it);
            m_providers.push_back(std::pair(*it, LyricSourceFactoryMap.at(*it)));
        }
    }

}
