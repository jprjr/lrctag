#ifndef LRCTAG_CONTAINER_APE_INCLUDE_GUARD
#define LRCTAG_CONTAINER_APE_INCLUDE_GUARD

#include "container.h"
#include "../taghandler/ape.h"

#include <apefile.h>
#include <tpropertymap.h>

namespace LrcTag {
    class APEContainer: public ContainerBase {
        private:
            TagLib::APE::File m_file;
            APETagHandler m_tag;
        public:
            APEContainer(const Config& config, const std::filesystem::path& path, TagLib::FileStream* fs)
            : ContainerBase(config, path), m_file(fs), m_tag(config, m_file.hasAPETag() ? m_file.APETag(false) : NULL) {
                if(!m_file.hasAPETag()) {
                    TagLib::PropertyMap p = m_file.properties();
                    if(!p.isEmpty()) {
                        /* setting properties will create an APE tag */
                        m_file.setProperties(p);
                    }
                    /* force an APE tag to be created, whether or not we had properties already */
                    m_tag.setTag(m_file.APETag(true));
                }
            }

            ~APEContainer() override { }

            APETagHandler* tag() override {
                return &m_tag;
            }

            const APETagHandler* tag() const override {
                return &m_tag;
            }

            TagLib::AudioProperties* audioProperties() const override {
                return m_file.audioProperties();
            }

            void save() override {
                if(m_file.hasID3v1Tag() && ContainerBase::m_config.ape_strip_id3v1) {
                    m_file.strip(TagLib::APE::File::ID3v1);
                }
                m_file.save();
            }
    };
}

#endif
