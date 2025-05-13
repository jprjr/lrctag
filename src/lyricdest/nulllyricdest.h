#ifndef LRCTAG_NULL_LYRICDEST_INCLUDE_GUARD
#define LRCTAG_NULL_LYRICDEST_INCLUDE_GUARD

#include "lyricdest.h"

namespace LrcTag {
    class NullLyricDest: public LyricDest {
        public:
            constexpr NullLyricDest() { }
            ~NullLyricDest() override { }

            bool needsSynchronizedLyrics() override {
                return true;
            }

            bool needsUnsynchronizedLyrics() override {
                return true;
            }

            bool saveSynchronizedLyrics(const std::vector<SynchedLyric>& sl) override {
                return true;
            }

            bool saveUnsynchronizedLyrics(const icu::UnicodeString& ul) override {
                return true;
            }
    };
}

#endif

