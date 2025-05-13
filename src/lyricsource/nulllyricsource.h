#ifndef LRCTAG_NULL_LYRICSOURCE_INCLUDE_GUARD
#define LRCTAG_NULL_LYRICSOURCE_INCLUDE_GUARD

#include "lyricsource.h"

namespace LrcTag {
    class NullLyricSource: public LyricSource {
        public:
            constexpr NullLyricSource() { }
            ~NullLyricSource() override { }

            bool hasSynchronizedLyrics() override {
                return false;
            }

            bool hasUnsynchronizedLyrics() override {
                return false;
            }

            icu::UnicodeString getUnsynchronizedLyrics() override {
                return icu::UnicodeString();
            }

            std::vector<SynchedLyric> getSynchronizedLyrics() override {
                return std::vector<SynchedLyric>();
            }

    };
}

#endif

