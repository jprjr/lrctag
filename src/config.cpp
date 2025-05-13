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
          .default_value<std::vector<std::string> >({ ".flac", ".mp3", ".ogg", ".opus", ".oga", ".mp4", ".m4a" })
          .append();

        program.add_argument("--source")
          .help("Enable lyrics source.")
          .default_value<std::vector<std::string> >({"file"})
          .append();

        program.add_argument("--destination")
          .help("Enable lyrics destination.")
          .default_value<std::vector<std::string> >({ "tag", "file" })
          .append();
    
        program.add_argument("--threads")
          .help("Use specified number of threads.")
          .default_value<int>(0)
          .scan<'i',int>();

        program.add_argument("--overwrite-tags")
          .help("Overwrite existing lyrics tags.")
          .flag();

        program.add_argument("--overwrite-files")
          .help("Overwrite existing lyrics files.")
          .flag();

        auto &vc_unsynch_group = program.add_mutually_exclusive_group();

        vc_unsynch_group.add_argument("--vorbiscomment-synchronized-lyrics-tag")
          .help("Set tag to use for vorbiscomment synched lyrics (FLAC, Opus, etc)")
          .default_value(std::string{"LYRICS"});
        vc_unsynch_group.add_argument("--no-vorbiscomment-synchronized-lyrics")
          .help("Disable synchronized lyrics in vorbiscomment files.")
          .flag();

        auto &vc_synch_group = program.add_mutually_exclusive_group();

        vc_synch_group.add_argument("--vorbiscomment-unsynchronized-lyrics-tag")
          .help("Set tag to use for vorbiscomment unsynched lyrics (FLAC, Opus, etc)")
          .default_value(std::string{"UNSYNCEDLYRICS"});
        vc_synch_group.add_argument("--no-vorbiscomment-unsynchronized-lyrics")
          .help("Disable synchronized lyrics in vorbiscomment files.")
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

        std::vector<std::string> raw_audio_extensions = program.get<std::vector<std::string> >("--extension");
        for(auto it = raw_audio_extensions.cbegin(); it != raw_audio_extensions.cend(); ++it) {
            if(audio_extensions.count(*it) == 0) {
               audio_extensions.insert(*it);
            }
        }

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
    
        if(program["--no-synchronized-lyrics"] == true) {
            synchronized_lyrics = false;
        }

        if(program["--no-unsynchronized-lyrics"] == true) {
            unsynchronized_lyrics = false;
        }

        if(program["--overwrite-tags"] == true) {
            overwrite_tags = true;
        }

        if(program["--overwrite-files"] == true) {
            overwrite_files = true;
        }
    
        threads = program.get<int>("--threads");
        if(threads <= 0) {
            threads = std::thread::hardware_concurrency();
            if(threads == 0) {
                spdlog::warn("unable to determine number of cores, defaulting to 1 thread");
                threads = 1;
            }
        }

        if(program["--no-vorbiscomment-synchronized-lyrics"] == true) {
            vc_synched_tagname = TagLib::String();
        } else {
            vc_synched_tagname = TagLib::String(program.get<std::string>("--vorbiscomment-synchronized-lyrics-tag"), TagLib::String::UTF8);
        }

        if(program["--no-vorbiscomment-unsynchronized-lyrics"] == true) {
            vc_unsynched_tagname = TagLib::String();
        } else {
            vc_unsynched_tagname = TagLib::String(program.get<std::string>("--vorbiscomment-unsynchronized-lyrics-tag"), TagLib::String::UTF8);
        }
    
        return true;
    }
}

