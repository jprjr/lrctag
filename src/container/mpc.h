#ifndef LRCTAG_CONTAINER_MPC_INCLUDE_GUARD
#define LRCTAG_CONTAINER_MPC_INCLUDE_GUARD

#include "container.h"
#include "../taghandler/ape.h"

#include <mpcfile.h>
#include <tpropertymap.h>

namespace LrcTag {
    class MPCContainer: public ContainerBase {
        private:
            TagLib::MPC::File m_file;
            APETagHandler m_tag;
        public:
            MPCContainer(const Config& config, const std::filesystem::path& path, TagLib::FileStream* fs)
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
                m_file.strip(TagLib::MPC::File::ID3v1 | TagLib::MPC::File::ID3v2);
                m_file.save();
            }
    };
}

#endif
