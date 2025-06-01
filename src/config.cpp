#include "config.h"

#include <set>

#include <spdlog/spdlog.h>
#include <argparse/argparse.hpp>
#include <unicode/unistr.h>

#include <thread>

#include "utils.h"

namespace LrcTag {
    bool Config::argparse(int argc, const char* argv[]) {
        bool success = true;
    
        argparse::ArgumentParser program("lrctag");
    
        program.add_argument("--no-synchronized-lyrics")
          .help("Disable synchronized lyrics.")
          .flag();

        program.add_argument("--no-unsynchronized-lyrics")
          .help("Disable unsynchronized lyrics.")
          .flag();
    
        program.add_argument("--recursive")
          .help("Load files recursively.")
          .flag();
    
        program.add_argument("--extension")
          .help("Treat <ext> as an audio file.")
          .default_value<std::vector<std::string> >({ ".flac", ".mp3", ".ogg", ".opus", ".oga", ".mp4", ".m4a", ".mpc", ".tta", ".wv", ".ape" })
          .append();

        program.add_argument("--source")
          .help("Enable lyrics source.")
          .default_value<std::vector<std::string> >({"file","lrclib"})
          .append();

        program.add_argument("--destination")
          .help("Enable lyrics destination.")
          .default_value<std::vector<std::string> >({ "tag", "file" })
          .append();
    
        program.add_argument("--threads")
          .help("Use specified number of threads.")
          .default_value<int>(0)
          .scan<'i',int>();

        program.add_argument("--report")
          .help("Output CSV of tag and file lyrics status")
          .flag();


        program.add_argument("--overwrite-tags")
          .help("Overwrite existing lyrics tags.")
          .flag();

        program.add_argument("--overwrite-files")
          .help("Overwrite existing lyrics files.")
          .flag();

        auto &mp3_strip_id3v1_group = program.add_mutually_exclusive_group();
        mp3_strip_id3v1_group.add_argument("--mp3-strip-id3v1")
          .help("Strip ID3v1 tags from MP3 files (default).")
          .flag();
        mp3_strip_id3v1_group.add_argument("--no-mp3-strip-id3v1")
          .help("Do not strip ID3v1 tags from MP3 files.")
          .flag();

        auto &mp3_strip_ape_group = program.add_mutually_exclusive_group();
        mp3_strip_ape_group.add_argument("--mp3-strip-ape")
          .help("Strip APE tags from MP3 files (default).")
          .flag();
        mp3_strip_ape_group.add_argument("--no-mp3-strip-ape")
          .help("Do not strip APE tags from MP3 files.")
          .flag();

        auto &flac_strip_id3v1_group = program.add_mutually_exclusive_group();
        flac_strip_id3v1_group.add_argument("--flac-strip-id3v1")
          .help("Strip ID3v1 tags from FLAC files (default).")
          .flag();
        flac_strip_id3v1_group.add_argument("--no-flac-strip-id3v1")
          .help("Do not strip ID3v1 tags from FLAC files.")
          .flag();

        auto &flac_strip_id3v2_group = program.add_mutually_exclusive_group();
        flac_strip_id3v2_group.add_argument("--flac-strip-id3v2")
          .help("Strip ID3v2 tags from FLAC files (default).")
          .flag();
        flac_strip_id3v2_group.add_argument("--no-flac-strip-id3v2")
          .help("Do not strip ID3v2 tags from FLAC files.")
          .flag();

        auto &trueaudio_strip_id3v1_group = program.add_mutually_exclusive_group();
        trueaudio_strip_id3v1_group.add_argument("--trueaudio-strip-id3v1")
          .help("Strip ID3v1 tags from TrueAudio files (default).")
          .flag();
        trueaudio_strip_id3v1_group.add_argument("--no-trueaudio-strip-id3v1")
          .help("Do not strip ID3v1 tags from TrueAudio files.")
          .flag();

        auto &trueaudio_tag_group = program.add_mutually_exclusive_group();
        trueaudio_tag_group.add_argument("--trueaudio-ape")
          .help("Store APE tags in TrueAudio (default).")
          .flag();
        trueaudio_tag_group.add_argument("--trueaudio-id3v2")
          .help("Store ID3v2 tags in TrueAudio files.")
          .flag();

        auto &ape_strip_id3v1_group = program.add_mutually_exclusive_group();
        ape_strip_id3v1_group.add_argument("--ape-strip-id3v1")
          .help("Strip ID3v1 tags from APE files (default).")
          .flag();
        ape_strip_id3v1_group.add_argument("--no-ape-strip-id3v1")
          .help("Do not strip ID3v1 tags from APE files.")
          .flag();

        auto &wavpack_strip_id3v1_group = program.add_mutually_exclusive_group();
        wavpack_strip_id3v1_group.add_argument("--wavpack-strip-id3v1")
          .help("Strip ID3v1 tags from WavPack files (default).")
          .flag();
        wavpack_strip_id3v1_group.add_argument("--no-wavpack-strip-id3v1")
          .help("Do not strip ID3v1 tags from WavPack files.")
          .flag();

        auto &mpc_strip_id3v1_group = program.add_mutually_exclusive_group();
        mpc_strip_id3v1_group.add_argument("--mpc-strip-id3v1")
          .help("Strip ID3v1 tags from Musepack files (default).")
          .flag();
        mpc_strip_id3v1_group.add_argument("--no-mpc-strip-id3v1")
          .help("Do not strip ID3v1 tags from Musepack files.")
          .flag();

        auto &mpc_strip_id3v2_group = program.add_mutually_exclusive_group();
        mpc_strip_id3v2_group.add_argument("--mpc-strip-id3v2")
          .help("Strip ID3v2 tags from Musepack files (default).")
          .flag();
        mpc_strip_id3v2_group.add_argument("--no-mpc-strip-id3v2")
          .help("Do not strip ID3v2 tags from Musepack files.")
          .flag();

        auto &vc_unsynch_group = program.add_mutually_exclusive_group();

        vc_unsynch_group.add_argument("--vorbiscomment-synchronized")
          .help("Set field to use for vorbiscomment synched lyrics (FLAC, Opus, etc)")
          .default_value(std::string{"LYRICS"});
        vc_unsynch_group.add_argument("--no-vorbiscomment-synchronized")
          .help("Disable synchronized lyrics in vorbiscomment files.")
          .flag();

        auto &vc_synch_group = program.add_mutually_exclusive_group();

        vc_synch_group.add_argument("--vorbiscomment-unsynchronized")
          .help("Set field to use for vorbiscomment unsynched lyrics (FLAC, Opus, etc)")
          .default_value(std::string{"UNSYNCEDLYRICS"});
        vc_synch_group.add_argument("--no-vorbiscomment-unsynchronized")
          .help("Disable synchronized lyrics in vorbiscomment files.")
          .flag();

        auto &ape_unsynch_group = program.add_mutually_exclusive_group();

        ape_unsynch_group.add_argument("--ape-synchronized")
          .help("Set tag name to use for synched lyrics in APE tags")
          .default_value(std::string{"SyncedLyrics"});
        ape_unsynch_group.add_argument("--no-ape-synchronized")
          .help("Disable synchronized lyrics in APE tags.")
          .flag();

        auto &ape_synch_group = program.add_mutually_exclusive_group();
        ape_synch_group.add_argument("--ape-unsynchronized")
          .help("Set tag name to use for unsynched lyrics in APE tags")
          .default_value(std::string{"Lyrics"});
        ape_synch_group.add_argument("--no-ape-unsynchronized")
          .help("Disable unsynchronized lyrics in APE tags.")
          .flag();

        auto &mp4_synch_group = program.add_mutually_exclusive_group();
        mp4_synch_group.add_argument("--mp4-unsynchronized")
          .help("Store unsynchronized lyrics in MP4 tags (default).")
          .flag();
        mp4_synch_group.add_argument("--mp4-synchronized")
          .help("Store synchronized lyrics in MP4 tags.")
          .flag();

        auto &id3v2_version_group = program.add_mutually_exclusive_group();
        id3v2_version_group.add_argument("--id3v2.4")
          .help("Save ID3v2.4 tags (default)")
          .flag();
        id3v2_version_group.add_argument("--id3v2.3")
          .help("Save ID3v2.3 tags")
          .flag();
    
        program.add_argument("filename")
          .nargs(argparse::nargs_pattern::at_least_one);
    
    
        try {
            program.parse_args(argc, argv);
        } catch (const std::exception& err) {
            std::cerr << err.what() << std::endl;
            std::cerr << program;
            success = false;
        }
    
        if(!success) return success;
    
        files = program.get<std::vector<std::string> >("filename");
        /* I don't think this should happen but, just in case */
        if(files.size() == 0) return false;
    
        recursive = program.get<bool>("--recursive");
        reportmode = program.get<bool>("--report");

        std::vector<std::string> raw_audio_extensions = program.get<std::vector<std::string> >("--extension");
        for(auto it = raw_audio_extensions.cbegin(); it != raw_audio_extensions.cend(); ++it) {
            if(audio_extensions.count(*it) == 0) {
               audio_extensions.insert(*it);
            }
        }
    
        threads = program.get<int>("--threads");
        if(threads <= 0) {
            threads = std::thread::hardware_concurrency();
            if(threads == 0) {
                spdlog::warn("unable to determine number of cores, defaulting to 1 thread");
                threads = 1;
            }
        }

        if(program["--no-mp3-strip-id3v1"] == true) {
            mp3_strip_id3v1 = false;
        }

        if(program["--no-mp3-strip-ape"] == true) {
            mp3_strip_ape = false;
        }

        if(program["--no-flac-strip-id3v1"] == true) {
            flac_strip_id3v1 = false;
        }

        if(program["--no-flac-strip-id3v2"] == true) {
            flac_strip_id3v2 = false;
        }

        if(program["--no-trueaudio-strip-id3v1"] == true) {
            trueaudio_strip_id3v1 = false;
        }

        if(program["--no-ape-strip-id3v1"] == true) {
            ape_strip_id3v1 = false;
        }

        if(program["--no-wavpack-strip-id3v1"] == true) {
            wavpack_strip_id3v1 = false;
        }

        if(program["--no-mpc-strip-id3v1"] == true) {
            mpc_strip_id3v1 = false;
        }

        if(program["--no-mpc-strip-id3v2"] == true) {
            mpc_strip_id3v2 = false;
        }

        if(program["--no-vorbiscomment-synchronized"] == true) {
            vc_synched_tagname = TagLib::String();
        } else {
            vc_synched_tagname = TagLib::String(program.get<std::string>("--vorbiscomment-synchronized"), TagLib::String::UTF8);
        }

        if(program["--no-vorbiscomment-unsynchronized"] == true) {
            vc_unsynched_tagname = TagLib::String();
        } else {
            vc_unsynched_tagname = TagLib::String(program.get<std::string>("--vorbiscomment-unsynchronized"), TagLib::String::UTF8);
        }

        if(program["--no-ape-synchronized"] == true) {
            ape_synched_tagname = TagLib::String();
        } else {
            ape_synched_tagname = TagLib::String(program.get<std::string>("--ape-synchronized"), TagLib::String::UTF8);
        }

        if(program["--no-ape-unsynchronized"] == true) {
            ape_unsynched_tagname = TagLib::String();
        } else {
            ape_unsynched_tagname = TagLib::String(program.get<std::string>("--ape-unsynchronized"), TagLib::String::UTF8);
        }

        if(program["--id3v2.3"] == true) {
            id3v2_version = TagLib::ID3v2::v3;
        }

        if(program["--trueaudio-id3v2"] == true) {
            trueaudio_ape = false;
        }

        if(program["--mp4-synchronized"] == true) {
            mp4_unsynch = false;
        }

        if(!reportmode) {
            std::set<std::string> sources_set;
            std::vector<std::string> raw_sources = program.get<std::vector<std::string> >("--source");
            for(auto it = raw_sources.cbegin(); it != raw_sources.cend(); ++it) {
                if(sources_set.count(*it) == 0) {
                    sources_set.insert(*it);
                    sources.push_back(*it);
                }
            }

            std::set<std::string> dests_set;
            std::vector<std::string> raw_dests = program.get<std::vector<std::string> >("--destination");
            for(auto it = raw_dests.cbegin(); it != raw_dests.cend(); ++it) {
                if(dests_set.count(*it) == 0) {
                    dests_set.insert(*it);
                    destinations.push_back(*it);
                }
            }

            if(program["--overwrite-tags"] == true) {
                overwrite_tags = true;
            }
    
            if(program["--overwrite-files"] == true) {
                overwrite_files = true;
            }

            if(program["--no-synchronized-lyrics"] == true) {
                synchronized_lyrics = false;
            }

            if(program["--no-unsynchronized-lyrics"] == true) {
                unsynchronized_lyrics = false;
            }
    
        }
    
        return true;
    }
}

