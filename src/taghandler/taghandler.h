#ifndef LRCTAG_TAGHANDLER_INCLUDE_GUARD
#define LRCTAG_TAGHANDLER_INCLUDE_GUARD

#include <tag.h>
#include <unicode/unistr.h>

#include "../synchedlyric.h"
#include "../config.h"

namespace LrcTag {
class TagHandler {
    public:
        TagHandler(TagLib::Tag* tag)
        : m_tag(tag) { }

        virtual ~TagHandler() = default;

        virtual bool supportsSynchronizedLyrics(const Config&) const {
            return false;
        }

        virtual bool supportsUnsynchronizedLyrics(const Config&) const {
            return false;
        }

        virtual bool hasSynchronizedLyrics(const Config&) const {
            return false;
        }

        virtual bool hasUnsynchronizedLyrics(const Config&) const {
            return false;
        }

        virtual std::vector<SynchedLyric> getSynchronizedLyrics(const Config&, unsigned int) const = 0;
        virtual icu::UnicodeString getUnsynchronizedLyrics(const Config&) const = 0;

        virtual bool setSynchronizedLyrics(const Config&, const std::vector<SynchedLyric>&) = 0;

        virtual bool setUnsynchronizedLyrics(const Config&, const icu::UnicodeString&) = 0;

        virtual icu::UnicodeString title() const {
            return icu::UnicodeString::fromUTF8(m_tag->title().to8Bit(true));
        }

        virtual icu::UnicodeString artist() const {
            return icu::UnicodeString::fromUTF8(m_tag->artist().to8Bit(true));
        }

        virtual icu::UnicodeString album() const {
            return icu::UnicodeString::fromUTF8(m_tag->album().to8Bit(true));
        }

    private:
        TagLib::Tag* m_tag;
};
}

#endif

