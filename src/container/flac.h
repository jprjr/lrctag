#ifndef LRCTAG_CONTAINER_FLAC_INCLUDE_GUARD
#define LRCTAG_CONTAINER_FLAC_INCLUDE_GUARD

#include "container.h"
#include "../taghandler/xiphcomment.h"

#include <flacfile.h>
#include <tpropertymap.h>

namespace LrcTag {
    class FLACContainer: public ContainerBase {
        public:
            FLACContainer(const Config& config, const std::filesystem::path& path, TagLib::FileStream* fs)
            : ContainerBase(config, path), m_file(fs), m_tag(config, m_file.hasXiphComment() ? m_file.xiphComment(false) : NULL) {
                if(!m_file.hasXiphComment()) {
                    /* properties will try to load tags from other tag types */
                    TagLib::PropertyMap p = m_file.properties();
                    if(!p.isEmpty()) {
                        /* setting properties will create a Xiph Comment */
                        m_file.setProperties(p);
                    }
                    /* force a xiph comment to be created */
                    m_tag.setTag(m_file.xiphComment(true));
                }
            }
    
            ~FLACContainer() override {
            }
    
            XiphTagHandler* tag() override {
                return &m_tag;
            }

            const XiphTagHandler* tag() const override {
                return &m_tag;
            }

            TagLib::AudioProperties* audioProperties() const override {
                return m_file.audioProperties();
            }

            void save() override {
                int tagsToStrip = TagLib::FLAC::File::NoTags;

                if(ContainerBase::m_config.flac_strip_id3v1 && m_file.hasID3v1Tag()) {
                    tagsToStrip |= TagLib::FLAC::File::ID3v1;
                }

                if(ContainerBase::m_config.flac_strip_id3v2 && m_file.hasID3v2Tag()) {
                    tagsToStrip |= TagLib::FLAC::File::ID3v2;
                }

                if(tagsToStrip != TagLib::FLAC::File::NoTags) {
                    m_file.strip(tagsToStrip);
                }

                m_file.save();
            }
        
        private:
            TagLib::FLAC::File m_file;
            XiphTagHandler m_tag;
    };
}

#endif

