#ifndef LRCTAG_TAGHANDLER_INCLUDE_GUARD
#define LRCTAG_TAGHANDLER_INCLUDE_GUARD

#include <tag.h>
#include <unicode/unistr.h>

#include "../synchedlyric.h"
#include "../config.h"

namespace LrcTag {
class TagHandler {
    protected:
        const Config& m_config;
        void setTag(TagLib::Tag* tag);
    private:
        TagLib::Tag* m_tag;
    public:
        TagHandler(const Config& config, TagLib::Tag* tag)
        : m_config(config), m_tag(tag) { }

        virtual ~TagHandler() = default;

        virtual bool supportsSynchronizedLyrics() const {
            return false;
        }

        virtual bool supportsUnsynchronizedLyrics() const {
            return false;
        }

        virtual bool hasSynchronizedLyrics() const {
            return false;
        }

        virtual bool hasUnsynchronizedLyrics() const {
            return false;
        }

        virtual std::vector<SynchedLyric> getSynchronizedLyrics(unsigned int) const = 0;
        virtual icu::UnicodeString getUnsynchronizedLyrics() const = 0;

        virtual bool setSynchronizedLyrics(const std::vector<SynchedLyric>&) = 0;

        virtual bool setUnsynchronizedLyrics(const icu::UnicodeString&) = 0;

        template<typename T>
        T title() const;

        template<typename T>
        T artist() const;

        template<typename T>
        T album() const;

};
}

#endif

