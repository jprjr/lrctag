#ifndef LRCTAG_LRC_INCLUDE_GUARD
#define LRCTAG_LRC_INCLUDE_GUARD

#include <vector>
#include "synchedlyric.h"

#include <unicode/regex.h>

namespace LrcTag {
    class LRC {
        private:
            static const icu::UnicodeString LINE_PATTERN;
            static icu::RegexPattern* LINE;

        public:
            static std::vector<SynchedLyric> parse(const icu::UnicodeString& src, unsigned int end_ms);
            static icu::UnicodeString format(const std::vector<SynchedLyric>& src);

            static bool compilePatterns();
            static void releasePatterns();

    };
}

#endif
