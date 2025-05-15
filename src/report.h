#ifndef LRCTAG_REPORT_INCLUDE_GUARD
#define LRCTAG_REPORT_INCLUDE_GUARD

#include <filesystem>

namespace LrcTag {
    class Report {
        public:
            enum NoYesNA {
                Unknown = -1,
                No = 0,
                Yes = 1,
                NA = 2
            };

            NoYesNA has_unsynched_tag;
            NoYesNA has_synched_tag;
            NoYesNA has_unsynched_file;
            NoYesNA has_synched_file;

            constexpr Report()
            :
              has_unsynched_tag(Unknown), has_synched_tag(Unknown),
              has_unsynched_file(Unknown), has_synched_file(Unknown) { }
            ~Report() { }

            static inline std::string header() {
                return "unsynched_lyrics_tag,synched_lyrics_tag,unsynched_lyrics_file,synched_lyrics_file";
            }

            static constexpr const char* NoYesString(NoYesNA val) {
                switch(val) {
                    case Unknown: return "Unknown";
                    case No: return "No";
                    case Yes: return "Yes";
                    case NA: return "NA";
                }
                return "";
            }

            std::string string() const {
                return std::string(NoYesString(has_unsynched_tag)) +
                  std::string(",") +
                  std::string(NoYesString(has_synched_tag)) +
                  std::string(",") +
                  std::string(NoYesString(has_unsynched_file)) +
                  std::string(",") +
                  std::string(NoYesString(has_synched_file));
            }

    };
}

#endif
