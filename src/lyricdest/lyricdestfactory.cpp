#include "lyricdestfactory.h"

#include <spdlog/spdlog.h>
#include <map>

#include "nulllyricdest.h"
#include "taglyricdest.h"
#include "filelyricdest.h"

namespace LrcTag {

    static LyricDest* NullDestLoader(const Config& conf, Container* c) {
        (void)conf;
        (void)c;
        return new NullLyricDest();
    }

    static LyricDest* TagDestLoader(const Config& conf, Container* c) {
        return new TagLyricDest(conf, c);
    }

    static LyricDest* FileDestLoader(const Config& conf, Container* c) {
        return new FileLyricDest(conf, c->path());
    }

    static const std::map<std::string, LyricDest* (*)(const Config&, Container*)> LyricDestFactoryMap = {
        { "file", FileDestLoader },
        { "tag", TagDestLoader },
        { "null", NullDestLoader },
    };

    LyricDestFactory::LyricDestFactory(const Config& conf) : m_config(conf) {
        for(auto it = conf.destinations.cbegin(); it != conf.destinations.cend(); ++it) {

            spdlog::debug("Enabling lyrics destination {}", *it);
            m_providers.push_back(std::pair(*it, LyricDestFactoryMap.at(*it)));
        }
    }

}

