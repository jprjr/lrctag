#include "taghandler.h"

namespace LrcTag {

void TagHandler::setTag(TagLib::Tag* tag) {
    m_tag = tag;
}

template<>
std::string TagHandler::title() const {
    return m_tag->title().to8Bit(true);
}

template<>
icu::UnicodeString TagHandler::title() const {
    return icu::UnicodeString::fromUTF8(m_tag->title().to8Bit(true));
}

template<>
std::string TagHandler::artist() const {
    return m_tag->artist().to8Bit(true);
}

template<>
icu::UnicodeString TagHandler::artist() const {
    return icu::UnicodeString::fromUTF8(m_tag->artist().to8Bit(true));
}

template<>
std::string TagHandler::album() const {
    return m_tag->album().to8Bit(true);
}

template<>
icu::UnicodeString TagHandler::album() const {
    return icu::UnicodeString::fromUTF8(album<std::string>());
}

}
