#ifndef LRCTAG_CONTAINER_WAVPACK_INCLUDE_GUARD
#define LRCTAG_CONTAINER_WAVPACK_INCLUDE_GUARD

#include "container.h"
#include "../taghandler/ape.h"

#include <wavpackfile.h>
#include <tpropertymap.h>

namespace LrcTag {
    class WavPackContainer: public ContainerBase {
        private:
            TagLib::WavPack::File m_file;
            APETagHandler m_tag;
        public:
            WavPackContainer(const Config& config, const std::filesystem::path& path, TagLib::FileStream* fs)
            : ContainerBase(config, path), m_file(fs), m_tag(config, m_file.hasAPETag() ? m_file.APETag(false) : NULL) {
                if(!m_file.hasAPETag()) {
                    TagLib::PropertyMap p = m_file.properties();
                    if(!p.isEmpty()) {
                        /* setting properties will create an APE tag */
                        m_file.setProperties(p);
                    }
                    /* force an APE tag to be created */
                    m_tag.setTag(m_file.APETag(true));
                }
            }

            ~WavPackContainer() override { }

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
                int tagsToStrip = TagLib::WavPack::File::NoTags;

                if(ContainerBase::m_config.wavpack_strip_id3v1 && m_file.hasID3v1Tag()) {
                    tagsToStrip |= TagLib::WavPack::File::ID3v1;
                }
                if(tagsToStrip != TagLib::WavPack::File::NoTags) {
                    m_file.strip(tagsToStrip);
                }

                m_file.save();
            }
    };
}

#endif

