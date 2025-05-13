#ifndef LRCTAG_RESULT_INCLUDE_GUARD
#define LRCTAG_RESULT_INCLUDE_GUARD

#include "config.h"

#include <filesystem>
#include <vector>

namespace LrcTag {
    class Result {
        public:
            std::filesystem::path path;
            bool existing_synchronized_tag;
            bool existing_unsynchronized_tag;
            bool existing_synchronized_file;
            bool existing_unsynchronized_file;
            bool updated_synchronized_tag;
            bool updated_unsynchronized_tag;
            bool updated_synchronized_file;
            bool updated_unsynchronized_file;

            Result()
              :
                existing_synchronized_tag(false),
                existing_unsynchronized_tag(false),
                existing_synchronized_file(false),
                existing_unsynchronized_file(false),
                updated_synchronized_tag(false),
                updated_unsynchronized_tag(false),
                updated_synchronized_file(false),
                updated_unsynchronized_file(false) { }

            Result(std::filesystem::path p)
              : 
                path(p),
                existing_synchronized_tag(false),
                existing_unsynchronized_tag(false),
                existing_synchronized_file(false),
                existing_unsynchronized_file(false),
                updated_synchronized_tag(false),
                updated_unsynchronized_tag(false),
                updated_synchronized_file(false),
                updated_unsynchronized_file(false) { }

            Result(const Result& r) {
                path = r.path;
                existing_synchronized_tag = r.existing_synchronized_tag;
                existing_unsynchronized_tag = r.existing_unsynchronized_tag;
                existing_synchronized_file = r.existing_synchronized_file;
                existing_unsynchronized_file = r.existing_unsynchronized_file;
                updated_synchronized_tag = r.updated_synchronized_tag;
                updated_unsynchronized_tag = r.updated_unsynchronized_tag;
                updated_synchronized_file = r.updated_synchronized_file;
                updated_unsynchronized_file = r.updated_unsynchronized_file;
            }

            ~Result() { }

            static std::vector<Result> loadFiles(const Config& c);
    };
}

#endif
    

