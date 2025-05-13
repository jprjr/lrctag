#ifndef LRCTAG_TAGHANDLER_XIPHCOMMENT_INCLUDE_GUARD
#define LRCTAG_TAGHANDLER_XIPHCOMMENT_INCLUDE_GUARD

#include "taghandler.h"
#include "../lrc.h"
#include "../utils.h"
#include <xiphcomment.h>

#include "../debuglistener.h"

namespace LrcTag {
class XiphTagHandler: public TagHandler {
    public:
        XiphTagHandler() 
        : TagHandler(NULL), m_xc(NULL) { }

        XiphTagHandler(TagLib::Ogg::XiphComment* xc)
        : TagHandler(static_cast<TagLib::Tag*>(xc)), m_xc(xc) { }

        ~XiphTagHandler() { }

        bool supportsSynchronizedLyrics(const Config& c) const override {
            return c.synchronized_lyrics && c.vc_synched_tagname.size() > 0;
        }

        bool supportsUnsynchronizedLyrics(const Config& c) const override {
            return c.unsynchronized_lyrics && c.vc_unsynched_tagname.size() > 0;
        }

        bool hasSynchronizedLyrics(const Config& c) const override {
            return c.synchronized_lyrics && c.vc_synched_tagname.size() > 0 && m_xc->contains(c.vc_synched_tagname);
        }

        bool hasUnsynchronizedLyrics(const Config& c) const override {
            return c.unsynchronized_lyrics && c.vc_unsynched_tagname.size() > 0 && m_xc->contains(c.vc_unsynched_tagname);
        }

        std::vector<SynchedLyric> getSynchronizedLyrics(const Config& c, unsigned int len) const override {
            return LRC::parse(icu::UnicodeString::fromUTF8(m_xc->fieldListMap()[c.vc_synched_tagname][0].to8Bit(true)), len);
        }

        icu::UnicodeString getUnsynchronizedLyrics(const Config& c) const override {
            return icu::UnicodeString::fromUTF8(m_xc->fieldListMap()[c.vc_synched_tagname][0].to8Bit(true));
        }

        bool setSynchronizedLyrics(const Config& c, const std::vector<SynchedLyric>& lyrics) override {
            if(c.synchronized_lyrics && c.vc_synched_tagname.size() > 0) {
                m_xc->addField(c.vc_synched_tagname, TagLib::String(
                  String::toUTF8(
                    LRC::format(lyrics)
                  ), TagLib::String::UTF8
                ), true);
            }
            return true;
        }

        bool setUnsynchronizedLyrics(const Config& c, const icu::UnicodeString& lyrics) override {
            if(c.unsynchronized_lyrics && c.vc_unsynched_tagname.size() > 0) {
                m_xc->addField(c.vc_unsynched_tagname, TagLib::String(
                  String::toUTF8(lyrics), TagLib::String::UTF8
                ), true);
            }
            return true;
        }

    private:
        TagLib::Ogg::XiphComment* m_xc;
};
}

#endif
