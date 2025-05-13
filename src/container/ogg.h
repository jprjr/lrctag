#ifndef LRCTAG_CONTAINER_OGG_INCLUDE_GUARD
#define LRCTAG_CONTAINER_OGG_INCLUDE_GUARD

#include "container.h"
#include "../taghandler/xiphcomment.h"

#include <xiphcomment.h>
#include <tfile.h>

/* all the ogg types use XiphComment for a tag, so we'll
 * just cast to that */

namespace LrcTag {
    class OggContainer: public ContainerBase {
        public:
            OggContainer(const std::filesystem::path& path)
            : ContainerBase(path), m_file(NULL) { }
    
            virtual ~OggContainer() override { }
    
            XiphTagHandler* tag() override {
                return &m_tag;
            }

            const XiphTagHandler* tag() const override {
                return &m_tag;
            }


            TagLib::AudioProperties* audioProperties() const override {
                return m_file->audioProperties();
            }

            void save() override {
                if(m_file != NULL) m_file->save();
            }
    
        protected:
            TagLib::File* m_file;
            XiphTagHandler m_tag;

            void setTag(TagLib::Ogg::XiphComment* xc) {
                m_tag = XiphTagHandler(xc);
            }

            void setFile(TagLib::File* f) {
                m_file = f;
                setTag(static_cast<TagLib::Ogg::XiphComment*>(m_file->tag()));
            }
            
    };
}

#endif
