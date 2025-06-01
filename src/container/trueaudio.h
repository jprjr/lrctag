#ifndef LRCTAG_CONTAINER_TRUEAUDIO_INCLUDE_GUARD
#define LRCTAG_CONTAINER_TRUEAUDIO_INCLUDE_GUARD

#include "container.h"
#include "../taghandler/id3v2.h"

#include <trueaudiofile.h>
#include <tpropertymap.h>
#include <apetag.h>
#include <apefooter.h>
#include <id3v1tag.h>

#include "../debuglistener.h"

namespace LrcTag {
    class TrueAudioContainer: public ContainerBase {
        private:
            TagLib::TrueAudio::File m_file;
            APETagHandler m_ape_tag;
            ID3v2TagHandler m_id3v2_tag;
            TagLib::APE::Tag* m_ape;
            TagLib::ID3v2::Tag* m_id3v2;
            TagLib::ID3v1::Tag* m_id3v1;

            TagLib::offset_t findID3v1() {
                TagLib::offset_t id3v1_off = -1;

                if(!m_file.isValid()) return id3v1_off;
                if(m_file.length() < 131) return id3v1_off;

                m_file.seek(m_file.length() - 131, TagLib::File::Beginning);
                id3v1_off = m_file.tell() + 3;
                const TagLib::ByteVector data = m_file.readBlock(8);
                if(data.containsAt(TagLib::ID3v1::Tag::fileIdentifier(), 3) &&
                   data != TagLib::APE::Tag::fileIdentifier()) {
                    return id3v1_off;
                }
                return -1;
            }

            TagLib::offset_t findAPE(TagLib::offset_t id3v1_off) {
                TagLib::offset_t ape_off = -1;

                if(!m_file.isValid()) return ape_off;
                if(m_file.length() < TagLib::APE::Footer::size()) return ape_off;

                if(id3v1_off >= 0) { /* we found an ID3v1 tag, go 32 bytes byte that for APE */
                    m_file.seek(id3v1_off - TagLib::APE::Footer::size(), TagLib::File::Beginning);
                } else {
                    m_file.seek(m_file.length() - TagLib::APE::Footer::size(), TagLib::File::Beginning);
                }

                ape_off = m_file.tell();
                if(m_file.readBlock(8) == TagLib::APE::Tag::fileIdentifier()) {
                    return ape_off;
                }

                return -1;
            }
        public:
            TrueAudioContainer(const Config& config, const std::filesystem::path& path, TagLib::FileStream* fs)
            : ContainerBase(config, path), m_file(fs),
              m_ape_tag(config, NULL),
              m_id3v2_tag(config, NULL),
              m_ape(NULL), m_id3v2(NULL), m_id3v1(NULL) {
                /* try to find an existing APE tag */
                TagLib::offset_t ape_off = findAPE(findID3v1());
                if(ape_off >= 0) {
                    m_ape = new TagLib::APE::Tag(static_cast<TagLib::File*>(&m_file), ape_off);
                    if(m_ape->isEmpty()) {
                        delete m_ape;
                        m_ape = NULL;
                    }
                }

                /* try to find an existing ID3v2 tag */
                if(m_file.hasID3v2Tag()) {
                    m_id3v2 = m_file.ID3v2Tag(false);
                }

                /* try to find an existing ID3v1 tag */
                if(m_file.hasID3v1Tag()) {
                    m_id3v1 = m_file.ID3v1Tag(false);
                }

                if(ContainerBase::m_config.trueaudio_ape) {
                    /* if m_ape is null, either there was no APE tag or it had no data,
                     * so we'll create it and try to copy data from an ID3v2 or ID3v1 tag */
                    if(m_ape == NULL) {
                        m_ape = new TagLib::APE::Tag();
                        /* if there is an existing ID3v2Tag, get properties and transfer to APE */
                        if(m_id3v2 != NULL) {
                            m_ape->setProperties(m_id3v2->properties());
                        } else if(m_id3v1 != NULL) {
                            m_ape->setProperties(m_id3v1->properties());
                        }
                    }
                    m_ape_tag.setTag(m_ape);
                } else {
                    if(m_id3v2 == NULL) {
                        /* make an ID3v2 tag */
                        m_id3v2 = m_file.ID3v2Tag(true);
                        if(m_ape != NULL) {
                            m_id3v2->setProperties(m_ape->properties());
                        } else if(m_id3v1 != NULL) {
                            m_id3v2->setProperties(m_id3v2->properties());
                        }
                    }
                    m_id3v2_tag.setTag(m_id3v2);
                }
            }

            ~TrueAudioContainer() override {
                if(m_ape != NULL) delete m_ape;
            }

            TagHandler* tag() override {
                if(ContainerBase::m_config.trueaudio_ape) {
                    return static_cast<TagHandler*>(&m_ape_tag);
                }
                return static_cast<TagHandler*>(&m_id3v2_tag);
            }

            const TagHandler* tag() const override {
                if(ContainerBase::m_config.trueaudio_ape) {
                    return static_cast<const TagHandler*>(&m_ape_tag);
                }
                return static_cast<const TagHandler*>(&m_id3v2_tag);
            }

            TagLib::AudioProperties* audioProperties() const override {
                return m_file.audioProperties();
            }

            void save() override {
                auto logger = LRCTAGDebugListener::getLogger();
                TagLib::offset_t id3v1_off = -1;
                TagLib::offset_t   ape_off = -1;

                int tagsToStrip = TagLib::TrueAudio::File::NoTags;
                if(m_file.hasID3v1Tag() && ContainerBase::m_config.trueaudio_strip_id3v1) {
                    tagsToStrip |= TagLib::TrueAudio::File::ID3v1;
                }
                if(m_file.hasID3v2Tag() && ContainerBase::m_config.trueaudio_ape) {
                    tagsToStrip |= TagLib::TrueAudio::File::ID3v2;
                }

                if(tagsToStrip != TagLib::TrueAudio::File::NoTags) {
                    m_file.strip(tagsToStrip);
                    m_file.save();
                }

                /* check if we have old APE data at the end to remove */
                id3v1_off = findID3v1();
                ape_off = findAPE(id3v1_off);
                if(ape_off >= 0) {
                    /* we've already read APETAGEX */
                    m_file.seek(ape_off, TagLib::File::Beginning);
                    TagLib::APE::Footer footer(m_file.readBlock(TagLib::APE::Footer::size()));
                    /* "rewind" by the completeTagSize - footerSize since our
                     * ape_off value already accounts for the footer size */
                    ape_off -= (footer.completeTagSize() - TagLib::APE::Footer::size());
                    logger->trace("removing old ape tag at offset {}, size {}",
                      ape_off, footer.completeTagSize());
                    m_file.removeBlock(ape_off, footer.completeTagSize());
                    /* reset our ID3v1 offset */
                    id3v1_off = findID3v1();
                }

                if(ContainerBase::m_config.trueaudio_ape) {
                    if(id3v1_off < 0) {
                        logger->trace("adding new ape tag at end of file, size {}", m_ape->render().size());
                        m_file.seek(0, TagLib::File::End);
                        m_file.writeBlock(m_ape->render());
                    } else {
                        m_file.insert(m_ape->render(), id3v1_off, 0);
                    }
                }
            }
    };
}

#endif
