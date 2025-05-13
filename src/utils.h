#ifndef LRCTAG_UTILS_INCLUDE_GUARD
#define LRCTAG_UTILS_INCLUDE_GUARD

#include <memory>
#include <string>
#include <stdexcept>
#include <vector>
#include <filesystem>

#include <unicode/regex.h>
#include <unicode/normalizer2.h>

template<typename ... Args>
std::string lrctag_string_format(const std::string& format, Args ... args) {
    int l = std::snprintf(nullptr, 0, format.c_str(), args ...) + 1;
    if(l <= 0) {
        throw std::runtime_error("Error during formatting.");
    }
    size_t size = static_cast<size_t>(l);
    std::unique_ptr<char[]> buf(new char[size]);
    std::snprintf(buf.get(), size, format.c_str(), args ...);
    return std::string(buf.get(), buf.get() + size - 1);
}

namespace LrcTag {
    class Regex {
        private:
            static const icu::UnicodeString WHITESPACE_START_PATTERN;
            static const icu::UnicodeString WHITESPACE_END_PATTERN;
            static const icu::UnicodeString EOL_PATTERN;
            static const icu::UnicodeString OTHER_EOL_PATTERN;

        public:
            static icu::RegexPattern* WHITESPACE_START;
            static icu::RegexPattern* WHITESPACE_END;
            static icu::RegexPattern* EOL;
            static icu::RegexPattern* OTHER_EOL;

            static bool compilePatterns();
            static void releasePatterns();
    };

    class String {
        private:
            static const icu::Normalizer2* nfkc;

        public:

            static std::vector<icu::UnicodeString> split(const icu::UnicodeString& src, const icu::RegexPattern* pattern, UErrorCode& ec);

            static icu::UnicodeString remove_bom(const icu::UnicodeString& src);
            static icu::UnicodeString trimStart(const icu::UnicodeString& src, UErrorCode& ec);
            static icu::UnicodeString trimEnd(const icu::UnicodeString& src, UErrorCode& ec);

            /* calls remove_bom, trimStart, trimEnd all in one go */
            static icu::UnicodeString trim(const icu::UnicodeString& src, UErrorCode& ec);

            static icu::UnicodeString norm_eol(const icu::UnicodeString& src, UErrorCode& ec);

            static icu::UnicodeString decode(const std::string& src, UErrorCode& ec);

            static int32_t parseInt(const icu::UnicodeString& src, int32_t base);

            static icu::UnicodeString formatInt(unsigned int val, unsigned int min_width);

            static inline std::string toUTF8(const icu::UnicodeString& u) {
                std::string r;
                return u.toUTF8String(r);
            }

            static bool initNFKC() {
                UErrorCode ec;
                nfkc = icu::Normalizer2::getNFKCInstance(ec);
                return U_SUCCESS(ec);
            }

            static inline icu::UnicodeString toNFKC(const icu::UnicodeString& str, UErrorCode& ec) {
                return nfkc->normalize(str, ec);
            }
    };

    class File {
        public:
            static std::string slurp(const std::filesystem::path& p, int& ec);
            static bool dump(const std::filesystem::path& p, const icu::UnicodeString& str);
    };
}

#endif
