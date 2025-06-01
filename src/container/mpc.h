#ifndef LRCTAG_CONTAINER_MPC_INCLUDE_GUARD
#define LRCTAG_CONTAINER_MPC_INCLUDE_GUARD

#include "container.h"
#include "../taghandler/ape.h"

#include <mpcfile.h>
#include <tpropertymap.h>
#include <id3v2tag.h>

namespace LrcTag {
    class MPCContainer: public ContainerBase {
        private:
            TagLib::MPC::File m_file;
            APETagHandler m_tag;

            bool hasID3v2Tag() {
                m_file.seek(0, TagLib::File::Beginning);
                const TagLib::ByteVector data = m_file.readBlock(3);
                return data == "ID3";
            }
        public:
            MPCContainer(const Config& config, const std::filesystem::path& path, TagLib::FileStream* fs)
            : ContainerBase(config, path), m_file(fs), m_tag(config, m_file.hasAPETag() ? m_file.APETag(false) : NULL) {
                if(!m_file.hasAPETag()) {
                    /* getting the propertymap will only return ID3v1 data, if we
                     * have ID3v2 let's try to read it */
                    TagLib::PropertyMap p = hasID3v2Tag() ? TagLib::ID3v2::Tag(static_cast<TagLib::File*>(&m_file), 0).properties() : m_file.properties();
                    if(!p.isEmpty()) {
                        /* setting properties will create an APE tag */
                        m_file.setProperties(p);
                    }
                    /* force an APE tag to be created */
                    m_tag.setTag(m_file.APETag(true));
                }
            }

            ~MPCContainer() override { }

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
                int tagsToStrip = TagLib::MPC::File::NoTags;
                if(ContainerBase::m_config.mpc_strip_id3v1 && m_file.hasID3v1Tag()) {
                    tagsToStrip |= TagLib::MPC::File::ID3v1;
                }
                if(ContainerBase::m_config.mpc_strip_id3v2 && hasID3v2Tag()) {
                    tagsToStrip |= TagLib::MPC::File::ID3v2;
                }
                if(tagsToStrip != TagLib::MPC::File::NoTags) {
                    m_file.strip(tagsToStrip);
                }
                m_file.save();
            }
    };
}

#endif
