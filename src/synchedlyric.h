#ifndef LRCTAG_SYNCHEDLYRIC_INCLUDE_GUARD
#define LRCTAG_SYNCHEDLYRIC_INCLUDE_GUARD

#include <unicode/unistr.h>

namespace LrcTag {

    class SynchedLyric {
        public:
            const icu::UnicodeString m_txt;
            unsigned int m_start;
            unsigned int m_end;

            SynchedLyric(const icu::UnicodeString& txt, unsigned int start, unsigned int end)
            : m_txt(txt), m_start(start), m_end(end) { }
            ~SynchedLyric() { }
    };
}

#endif
