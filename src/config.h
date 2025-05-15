#ifndef LRCTAG_CONFIG_INCLUDE_GUARD
#define LRCTAG_CONFIG_INCLUDE_GUARD

#include <vector>
#include <set>
#include <string>
#include <tstring.h>

namespace LrcTag {
    class Config {
        public:
            std::vector<std::string> files;
            std::set<std::string> audio_extensions;
            std::vector<std::string> sources;
            std::vector<std::string> destinations;
            int threads;
            bool recursive;
            bool unsynchronized_lyrics;
            bool synchronized_lyrics;
            bool overwrite_tags;
            bool overwrite_files;
            bool reportmode;
            TagLib::String vc_synched_tagname;
            TagLib::String vc_unsynched_tagname;
            std::string lrclib_endpoint;

            Config()
            :
              files{},
              audio_extensions{},
              sources{},
              destinations{},
              threads(0),
              recursive(false),
              unsynchronized_lyrics(true),
              synchronized_lyrics(true),
              overwrite_tags(false),
              overwrite_files(false),
              reportmode(false),
              vc_synched_tagname("LYRICS", TagLib::String::UTF8),
              vc_unsynched_tagname("UNSYNCEDLYRICS", TagLib::String::UTF8),
              lrclib_endpoint("https://lrclib.net/api/get")
              { }
            ~Config() { }

            bool argparse(int argc, const char* argv[]);
    };

}

#endif
