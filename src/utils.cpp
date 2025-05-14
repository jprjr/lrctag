#include "utils.h"

#include <fstream>
#include <sstream>
#include <unicode/ucsdet.h>
#include <unicode/schriter.h>
#include <spdlog/spdlog.h>

#include "debuglistener.h"

#include <iostream>

static const icu::UnicodeString BASE10_CHARS = icu::UnicodeString::fromUTF8("0123456789");

namespace LrcTag {

    const icu::Normalizer2* String::nfkc = NULL;

    const icu::UnicodeString Regex::WHITESPACE_START_PATTERN = icu::UnicodeString::fromUTF8("^\\s+");
    const icu::UnicodeString Regex::WHITESPACE_END_PATTERN = icu::UnicodeString::fromUTF8("\\s+$");
    const icu::UnicodeString Regex::EOL_PATTERN = icu::UnicodeString::fromUTF8("\\n");
    const icu::UnicodeString Regex::OTHER_EOL_PATTERN = icu::UnicodeString::fromUTF8("\\r\\n?");

    icu::RegexPattern* Regex::WHITESPACE_START = NULL;
    icu::RegexPattern* Regex::WHITESPACE_END = NULL;
    icu::RegexPattern* Regex::EOL = NULL;
    icu::RegexPattern* Regex::OTHER_EOL = NULL;

    bool Regex::compilePatterns() {
        UParseError pe;
        UErrorCode ec = U_ZERO_ERROR;

        WHITESPACE_START = icu::RegexPattern::compile(WHITESPACE_START_PATTERN,
          pe, ec);

        if(U_FAILURE(ec)) {
            spdlog::critical("failed to compile WHITESPACE_START_PATTERN: {}", u_errorName(ec));
            return false;
        }

        WHITESPACE_END = icu::RegexPattern::compile(WHITESPACE_END_PATTERN,
          pe, ec);

        if(U_FAILURE(ec)) {
            spdlog::critical("failed to compile WHITESPACE_END_PATTERN: {}", u_errorName(ec));
            return false;
        }

        EOL = icu::RegexPattern::compile(EOL_PATTERN,
          pe, ec);

        if(U_FAILURE(ec)) {
            spdlog::critical("failed to compile EOL_PATTERN: {}", u_errorName(ec));
            return false;
        }

        OTHER_EOL = icu::RegexPattern::compile(OTHER_EOL_PATTERN,
          pe, ec);

        if(U_FAILURE(ec)) {
            spdlog::critical("failed to compile OTHER_EOL_PATTERN: {}", u_errorName(ec));
            return false;
        }

        return true;
    }

    void Regex::releasePatterns() {
        delete WHITESPACE_START;
        delete WHITESPACE_END;
        delete EOL;
        delete OTHER_EOL;
    }

    icu::UnicodeString String::remove_bom(const icu::UnicodeString& src) {
        int32_t index = 0;
        if(src.charAt(0) == 0xfeff) {
            index++;
        }
        return icu::UnicodeString(src, index);
    }

    icu::UnicodeString String::trimStart(const icu::UnicodeString& src, UErrorCode& ec) {
        auto logger = LRCTAGDebugListener::getLogger();
        icu::RegexMatcher* matcher = Regex::WHITESPACE_START->matcher(src, ec);
        if(U_FAILURE(ec)) {
            logger->error("failed to create WHITESPACE_START matcher: {}", u_errorName(ec));
            return src;
        }

        icu::UnicodeString ret = matcher->replaceFirst(icu::UnicodeString(), ec);
        delete matcher;

        if(U_FAILURE(ec)) {
            logger->error("failed to execute WHITESPACE_START matcher: {}", u_errorName(ec));
            return src;
        }
        return ret;
    }

    icu::UnicodeString String::trimEnd(const icu::UnicodeString& src, UErrorCode& ec) {
        auto logger = LRCTAGDebugListener::getLogger();
        icu::RegexMatcher* matcher = Regex::WHITESPACE_END->matcher(src, ec);
        if(U_FAILURE(ec)) {
            logger->error("failed to create WHITESPACE_END matcher: {}", u_errorName(ec));
            return src;
        }

        icu::UnicodeString ret = matcher->replaceFirst(icu::UnicodeString(), ec);
        delete matcher;

        if(U_FAILURE(ec)) {
            logger->error("failed to execute WHITESPACE_END matcher: {}", u_errorName(ec));
            return src;
        }

        return ret;
    }

    icu::UnicodeString String::trim(const icu::UnicodeString& src, UErrorCode& ec) {
        icu::UnicodeString ret = remove_bom(src);

        ec = U_ZERO_ERROR;
        ret = trimStart(ret, ec);
        if(U_FAILURE(ec)) return src;
        ret = trimEnd(ret, ec);
        if(U_FAILURE(ec)) return src;

        return ret;
    }

    icu::UnicodeString String::norm_eol(const icu::UnicodeString& src, UErrorCode& ec) {
        auto logger = LRCTAGDebugListener::getLogger();
        icu::RegexMatcher* matcher = Regex::OTHER_EOL->matcher(src, ec);
        if(U_FAILURE(ec)) {
            logger->error("failed to create end-of-line matcher: {}", u_errorName(ec));
            return src;
        }

        icu::UnicodeString ret = matcher->replaceAll(icu::UnicodeString::fromUTF8("\n"), ec);
        delete matcher;

        if(U_FAILURE(ec)) {
            logger->error("failed to execute end-of-line matcher: {}", u_errorName(ec));
            return src;
        }

        return ret;
    }

    icu::UnicodeString String::decode(const std::string& src, UErrorCode& ec) {
        icu::UnicodeString ret;
        icu::UnicodeString tmp;
        UCharsetDetector* detector = NULL;
        const UCharsetMatch*m = NULL;
        char16_t* buf = NULL;
        int32_t ulen = 0;
        auto logger = LRCTAGDebugListener::getLogger();

        ec = U_ZERO_ERROR;
        detector = ucsdet_open(&ec);
        if(U_FAILURE(ec)) {
            logger->error("error creating encoding detector ({})", u_errorName(ec));
            goto cleanup;
        }

        ucsdet_setText(detector, src.c_str(), src.length(), &ec);
        if(U_FAILURE(ec)) {
            logger->error("error setting detector source ({})", u_errorName(ec));
            goto cleanup;
        }

        m = ucsdet_detect(detector, &ec);
        if(U_FAILURE(ec)) {
            logger->error("error detecting character encoding ({})", u_errorName(ec));
            goto cleanup;
        }
        if(m == NULL) {
            logger->error("error detecting character encoding (unknown)");
            goto cleanup;
        }

        ulen = ucsdet_getUChars(m, NULL, 0, &ec);
        if(U_FAILURE(ec) && !(ec == U_BUFFER_OVERFLOW_ERROR)) {
            logger->error("error getting decoded string length ({})", u_errorName(ec));
            goto cleanup;
        }
        ec = U_ZERO_ERROR;
        if(ulen <= 0) {
            logger->error("error getting decoded string length (unknown)");
            goto cleanup;
        }

        buf = tmp.getBuffer(ulen);
        ucsdet_getUChars(m, buf, ulen, &ec);
        if(U_FAILURE(ec)) {
            logger->error("error copying uchars ({})", u_errorName(ec));
            goto cleanup;
        }
        tmp.releaseBuffer(ulen);

        ret = tmp;

        cleanup:
        if(detector != NULL) ucsdet_close(detector);

        return ret;
    }

    std::vector<icu::UnicodeString> String::split(const icu::UnicodeString& src, const icu::RegexPattern* pattern, UErrorCode& ec) {
        std::vector<icu::UnicodeString> matches;
        std::vector<icu::UnicodeString> tmp;
        int64_t pos = 0;
        int64_t end = 0;
        auto logger = LRCTAGDebugListener::getLogger();
        icu::RegexMatcher* matcher = pattern->matcher(src, ec);
        if(U_FAILURE(ec)) {
            logger->error("failed to create matcher: {}", u_errorName(ec));
            goto cleanup;
        }

        while(matcher->find(pos, ec)) {
            if(U_FAILURE(ec)) {
                logger->error("failure while searching for split pattern: {}", u_errorName(ec));
                goto cleanup;
            }
            end = matcher->start64(ec);
            if(U_FAILURE(ec)) {
                logger->error("failure while finding pattern match start: {}", u_errorName(ec));
                goto cleanup;
            }
            logger->trace("string split match found, pos = {}, end = {}", pos, end);
            tmp.emplace_back(src, pos, end - pos);
            pos = matcher->end64(ec);
            if(U_FAILURE(ec)) {
                logger->error("failure while finding pattern match end: {}", u_errorName(ec));
                goto cleanup;
            }
        }

        if(pos < src.length()) {
            tmp.emplace_back(src, pos, src.length() - pos);
        }
        matches = tmp;

        cleanup:
        return matches;
    }

    int32_t String::parseInt(const icu::UnicodeString& src, int32_t base) {
        int32_t val = 0;
        int32_t t = 0;
        icu::StringCharacterIterator iterator(src);

        for(iterator.setToStart(); iterator.hasNext(); iterator.next()) {
            t = u_charDigitValue(iterator.current32());
            if(t == -1) return -1;
            val *= base;
            val += t;
        }

        return val;
    }

    icu::UnicodeString String::formatInt(unsigned int val, unsigned int min_width) {
        int32_t pos = 0;
        int32_t len = 1;
        unsigned int v = val;
        icu::UnicodeString ret;
        char16_t* buf = NULL;

        while(v > 10) {
            v /= 10;
            len++;
        }
        if(len < min_width) len = min_width;

        pos = len;
        buf = ret.getBuffer(len);

        while(val) {
            buf[--pos] = BASE10_CHARS.charAt(val % 10);
            val /= 10;
        }
        while(pos--) {
            buf[pos] = BASE10_CHARS.charAt(0);
        }

        ret.releaseBuffer(len);
        return ret;
    }

    std::string File::slurp(const std::filesystem::path& p, int& ec) {
        std::stringstream ss;
        std::string s;
        ec = 0;

        std::ifstream ifs(p, std::ios::binary | std::ios::in);

        if(!ifs.is_open()) {
            ec = 1;
            return s;
        }

        ss << ifs.rdbuf();
        s = ss.str();

        return s;
    }

    bool File::dump(const std::filesystem::path& p, const icu::UnicodeString& str) {
        std::ofstream ofs(p, std::ios::out | std::ios::trunc);
        if(!ofs.is_open()) return false;
        std::string txt = String::toUTF8(str);
        ofs.write(txt.c_str(), txt.length());
        ofs.write("\n", 1);
        ofs.close();
        return true;
    }

}
