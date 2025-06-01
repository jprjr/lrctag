#ifndef LRCTAG_CONFIG_INCLUDE_GUARD
#define LRCTAG_CONFIG_INCLUDE_GUARD

#include <vector>
#include <set>
#include <string>
#include <tstring.h>
#include <mpegfile.h>

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
            bool flac_strip_id3v1;
            bool flac_strip_id3v2;
            bool mp3_strip_id3v1;
            bool mp3_strip_ape;
            bool trueaudio_strip_id3v1;
            bool trueaudio_ape;
            bool ape_strip_id3v1;
            bool wavpack_strip_id3v1;
            bool mp4_unsynch;
            bool mpc_strip_id3v1;
            bool mpc_strip_id3v2;
            bool mpc_strip_ape;
            TagLib::String ape_synched_tagname;
            TagLib::String ape_unsynched_tagname;
            TagLib::ID3v2::Version id3v2_version;

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
              lrclib_endpoint("https://lrclib.net/api/get"),
              flac_strip_id3v1(true),
              flac_strip_id3v2(true),
              mp3_strip_id3v1(true),
              mp3_strip_ape(true),
              trueaudio_strip_id3v1(true),
              trueaudio_ape(true),
              ape_strip_id3v1(true),
              wavpack_strip_id3v1(true),
              mp4_unsynch(true),
              mpc_strip_id3v1(true),
              mpc_strip_id3v2(true),
              mpc_strip_ape(true),
              ape_synched_tagname("SyncedLyrics", TagLib::String::UTF8),
              ape_unsynched_tagname("Lyrics", TagLib::String::UTF8),
              id3v2_version(TagLib::ID3v2::v4)
              { }
            ~Config() { }

            bool argparse(int argc, const char* argv[]);
    };

}

#endif
