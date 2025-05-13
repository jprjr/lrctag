#ifndef LRCTAG_LYRICDEST_INCLUDE_GUARD
#define LRCTAG_LYRICDEST_INCLUDE_GUARD

/* represents possible destinations to save lyrics, like local files or tags */

#include "../synchedlyric.h"
#include "../config.h"
#include <unicode/unistr.h>
#include <vector>

namespace LrcTag {
    class LyricDest {
        public:
            virtual ~LyricDest() { }

            virtual bool needsSynchronizedLyrics() = 0;
            virtual bool needsUnsynchronizedLyrics() = 0;
            virtual bool saveSynchronizedLyrics(const std::vector<SynchedLyric>&) = 0;
            virtual bool saveUnsynchronizedLyrics(const icu::UnicodeString&) = 0;
    };
}

#endif
