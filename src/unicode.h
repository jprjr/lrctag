#ifndef LRCTAG_UNICODE_INCLUDE_GUARD
#define LRCTAG_UNICODE_INCLUDE_GUARD

#include <unicode/normalizer2.h>
#include <unicode/unistr.h>
#include <string>

class UnicodeNormalizer {
    private:
        static const icu::Normalizer2* nfkc;
    public:
        static bool init() {
            UErrorCode ec;
            nfkc = icu::Normalizer2::getNFKCInstance(ec);
            return U_SUCCESS(ec);
        }

        static icu::UnicodeString toNFKC(const icu::UnicodeString str, UErrorCode &errorCode) {
            return nfkc->normalize(str, errorCode);
        }
};

#endif
