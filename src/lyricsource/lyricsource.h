#ifndef LRCTAG_LYRICSOURCE_INCLUDE_GUARD
#define LRCTAG_LYRICSOURCE_INCLUDE_GUARD

#include "../synchedlyric.h"
#include <unicode/unistr.h>
#include <vector>

namespace LrcTag {
    class LyricSource {
        public:
            virtual ~LyricSource() { }

            virtual bool hasSynchronizedLyrics() = 0;
            virtual bool hasUnsynchronizedLyrics() = 0;

            virtual icu::UnicodeString getUnsynchronizedLyrics() = 0;
            virtual std::vector<SynchedLyric> getSynchronizedLyrics() = 0;
    };
}

#endif

