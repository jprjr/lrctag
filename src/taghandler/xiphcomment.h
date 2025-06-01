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
        XiphTagHandler(const Config& config, TagLib::Ogg::XiphComment* xc)
        : TagHandler(config, static_cast<TagLib::Tag*>(xc)), m_xc(xc) { }

        ~XiphTagHandler() { }

        bool hasTag() const {
            return m_xc != NULL;
        }

        void setTag(TagLib::Ogg::XiphComment* xc) {
            m_xc = xc;
            TagHandler::setTag(static_cast<TagLib::Tag*>(m_xc));
        }

        bool supportsSynchronizedLyrics() const override {
            return TagHandler::m_config.synchronized_lyrics && TagHandler::m_config.vc_synched_tagname.size() > 0;
        }

        bool supportsUnsynchronizedLyrics() const override {
            return TagHandler::m_config.unsynchronized_lyrics && TagHandler::m_config.vc_unsynched_tagname.size() > 0;
        }

        bool hasSynchronizedLyrics() const override {
            return TagHandler::m_config.synchronized_lyrics && TagHandler::m_config.vc_synched_tagname.size() > 0 && m_xc->contains(TagHandler::m_config.vc_synched_tagname.upper());
        }

        bool hasUnsynchronizedLyrics() const override {
            return TagHandler::m_config.unsynchronized_lyrics && TagHandler::m_config.vc_unsynched_tagname.size() > 0 && m_xc->contains(TagHandler::m_config.vc_unsynched_tagname.upper());
        }

        std::vector<SynchedLyric> getSynchronizedLyrics(unsigned int len) const override {
            return LRC::parse(icu::UnicodeString::fromUTF8(m_xc->fieldListMap()[TagHandler::m_config.vc_synched_tagname.upper()][0].to8Bit(true)), len);
        }

        icu::UnicodeString getUnsynchronizedLyrics() const override {
            return icu::UnicodeString::fromUTF8(m_xc->fieldListMap()[TagHandler::m_config.vc_synched_tagname.upper()][0].to8Bit(true));
        }

        bool setSynchronizedLyrics(const std::vector<SynchedLyric>& lyrics) override {
            if(TagHandler::m_config.synchronized_lyrics && TagHandler::m_config.vc_synched_tagname.size() > 0) {
                m_xc->addField(TagHandler::m_config.vc_synched_tagname, TagLib::String(
                  String::toUTF8(
                    LRC::format(lyrics)
                  ), TagLib::String::UTF8
                ), true);
            }
            return true;
        }

        bool setUnsynchronizedLyrics(const icu::UnicodeString& lyrics) override {
            if(TagHandler::m_config.unsynchronized_lyrics && TagHandler::m_config.vc_unsynched_tagname.size() > 0) {
                m_xc->addField(TagHandler::m_config.vc_unsynched_tagname, TagLib::String(
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
