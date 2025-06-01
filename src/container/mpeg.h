#ifndef LRCTAG_CONTAINER_MPEG_INCLUDE_GUARD
#define LRCTAG_CONTAINER_MPEG_INCLUDE_GUARD

#include "container.h"
#include "../taghandler/id3v2.h"

#include <taglib.h>
#include <mpegfile.h>
#include <tpropertymap.h>

namespace LrcTag {
    class MPEGContainer: public ContainerBase {
        private:
            TagLib::MPEG::File m_file;
            ID3v2TagHandler m_tag;
        public:
            MPEGContainer(const Config& config, const std::filesystem::path& path, TagLib::FileStream* fs)
            : ContainerBase(config, path),
#if TAGLIB_MAJOR_VERSION >= 2
              m_file(fs),
#else
              m_file(fs, NULL),
#endif
              m_tag(config, m_file.hasID3v2Tag() ? m_file.ID3v2Tag(false) : NULL) {
                if(!m_file.hasID3v2Tag()) {
                    TagLib::PropertyMap p = m_file.properties();
                    if(!p.isEmpty()) {
                        m_file.setProperties(p);
                    }
                    m_tag.setTag(m_file.ID3v2Tag(true));
                }
            }

            ~MPEGContainer() override { }

            ID3v2TagHandler* tag() override {
                return &m_tag;
            }

            const ID3v2TagHandler* tag() const override {
                return &m_tag;
            }

            TagLib::AudioProperties* audioProperties() const override {
                return m_file.audioProperties();
            }

            void save() override {
                /* strip requested tags */
                int tagsToStrip = TagLib::MPEG::File::NoTags;

                if(ContainerBase::m_config.mp3_strip_id3v1 && m_file.hasID3v1Tag()) {
                    tagsToStrip |= TagLib::MPEG::File::ID3v1;
                }

                if(ContainerBase::m_config.mp3_strip_ape && m_file.hasAPETag()) {
                    tagsToStrip |= TagLib::MPEG::File::APE;
                }

                if(tagsToStrip != TagLib::MPEG::File::NoTags) {
                    m_file.strip(tagsToStrip);
                }

                m_file.save(
                  TagLib::MPEG::File::ID3v2,
                  TagLib::File::StripNone,
                  ContainerBase::m_config.id3v2_version,
                  TagLib::File::DoNotDuplicate);
            }
    };
}

#endif
