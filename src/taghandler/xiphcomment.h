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
            if(TagHandler::m_config.synchronized_lyrics && TagHandler::m_config.vc_synched_tagname.size() > 0 && m_xc != NULL) {
                return m_xc->contains(TagHandler::m_config.vc_synched_tagname.upper());
            }
            return false;
        }

        bool hasUnsynchronizedLyrics() const override {
            if(TagHandler::m_config.unsynchronized_lyrics && TagHandler::m_config.vc_unsynched_tagname.size() > 0 && m_xc != NULL) {
                return m_xc->contains(TagHandler::m_config.vc_unsynched_tagname.upper());
            }
            return false;
        }

        std::vector<SynchedLyric> getSynchronizedLyrics(unsigned int len) const override {
            if(TagHandler::m_config.unsynchronized_lyrics && TagHandler::m_config.vc_unsynched_tagname.size() > 0 && m_xc != NULL) {
                return LRC::parse(icu::UnicodeString::fromUTF8(m_xc->fieldListMap()[TagHandler::m_config.vc_synched_tagname.upper()][0].to8Bit(true)), len);
            }
            return std::vector<SynchedLyric>();
        }

        icu::UnicodeString getUnsynchronizedLyrics() const override {
            if(TagHandler::m_config.synchronized_lyrics && TagHandler::m_config.vc_synched_tagname.size() > 0 && m_xc != NULL) {
                return icu::UnicodeString::fromUTF8(m_xc->fieldListMap()[TagHandler::m_config.vc_synched_tagname.upper()][0].to8Bit(true));
            }
            return icu::UnicodeString();
        }

        bool setSynchronizedLyrics(const std::vector<SynchedLyric>& lyrics) override {
            if(TagHandler::m_config.synchronized_lyrics && TagHandler::m_config.vc_synched_tagname.size() > 0 && m_xc != NULL) {
                m_xc->addField(TagHandler::m_config.vc_synched_tagname, TagLib::String(
                  String::toUTF8(
                    LRC::format(lyrics)
                  ), TagLib::String::UTF8
                ), true);
            }
            return true;
        }

        bool setUnsynchronizedLyrics(const icu::UnicodeString& lyrics) override {
            if(TagHandler::m_config.unsynchronized_lyrics && TagHandler::m_config.vc_unsynched_tagname.size() > 0 && m_xc != NULL) {
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
