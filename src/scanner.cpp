#include "scanner.h"

#include <spdlog/spdlog.h>

namespace LrcTag {

    const icu::UnicodeString FsScanner::DOT = icu::UnicodeString::fromUTF8(".");

    FsScanner::FsScanner(const Config& conf) : m_config(conf), m_files{}, m_exts{} {
        for(auto it = m_config.audio_extensions.cbegin();
         it != m_config.audio_extensions.cend(); ++it) {
            spdlog::debug("Treating {} as audio file", *it);

            icu::UnicodeString ext = icu::UnicodeString::fromUTF8(*it);
            if(!ext.startsWith(DOT)) {
                ext = DOT + ext;
            }
            m_exts.insert(ext);
        }
    }

    void FsScanner::addPath(const std::filesystem::path& p, const std::filesystem::file_status& fs) {
        if(m_files.count(p) > 0) {
            spdlog::debug("ignoring {} (duplicate)", p.string());
            return;
        }

        const icu::UnicodeString filename = icu::UnicodeString::fromUTF8(p.filename().u8string());
        if(filename.startsWith(DOT)) {
            spdlog::debug("ignoring {} (hidden file/folder)", p.string());
            return;
        }

        switch(fs.type()) {
            case std::filesystem::file_type::regular: {
                const icu::UnicodeString ext = icu::UnicodeString::fromUTF8(p.filename().extension().u8string()).toLower();
                if(m_exts.count(ext) == 0) {
                    spdlog::debug("ignoring {} (unknown file extension)", p.string());
                    return;
                }
                m_files.insert(p);
                return;
            }
            case std::filesystem::file_type::directory: {
                if(!m_config.recursive) {
                    spdlog::debug("ignoring directory {}", p.string());
                    return;
                }
                for(const std::filesystem::directory_entry& dir_entry:
                    std::filesystem::directory_iterator(p)) {
                    addPath(dir_entry.path(), dir_entry.status());
                }
                return;
            }
            default: break;
        }

        spdlog::warn("unhandled file type for {}", p.string());
    }


    void FsScanner::addPath(const std::filesystem::path& p) {
        addPath(p, std::filesystem::status(p));
    }

    void FsScanner::scanFiles() {
        spdlog::debug("Scanning {} paths", m_config.files.size());

        for(auto it = m_config.files.cbegin(); it != m_config.files.cend(); ++it) {
            spdlog::debug("Scanning {}", *it);

            try {
                addPath(std::filesystem::canonical(*it));
            } catch(const std::exception& e) {
                spdlog::warn("Ignoring {}: {}", *it, e.what());
            }
        }
    }

    std::vector<std::filesystem::path> FsScanner::getFiles() {
        std::vector<std::filesystem::path> flist;

        for(auto it = m_files.cbegin(); it != m_files.cend(); ++it) {
            flist.push_back(*it);
        }

        return flist;
    }

}
