#include "result.h"


#include <spdlog/spdlog.h>
#include <unicode/unistr.h>

static const icu::UnicodeString DOT = icu::UnicodeString::fromUTF8(".");

static void add_path_regular(std::vector<LrcTag::Result>& flist, const std::set<icu::UnicodeString>& audio_extensions, const std::filesystem::path& p) {
    const icu::UnicodeString extension = icu::UnicodeString::fromUTF8(p.filename().extension().u8string()).toLower();
    if(audio_extensions.count(extension) == 0) {
        spdlog::debug("ignoring {} (unknown file extension)", p.string());
        return;
    }

    spdlog::debug("adding {}", p.string());
    flist.emplace_back(p);
    return;
}

static void add_path_type(std::set<std::filesystem::path>& seen_files, std::vector<LrcTag::Result>& flist, const std::set<icu::UnicodeString>& audio_extensions, const std::filesystem::path& p, const std::filesystem::file_status& fs, bool recursive) {
    if(seen_files.count(p) > 0) {
        spdlog::debug("ignoring {} (duplicate)", p.string());
        return;
    }
    seen_files.insert(p);

    const icu::UnicodeString filename = icu::UnicodeString::fromUTF8(p.filename().u8string());

    if(filename.startsWith(DOT)) {
        spdlog::debug("ignoring {} (hidden file/folder)", p.string());
        return;
    }

    switch(fs.type()) {
        case std::filesystem::file_type::regular: {
            add_path_regular(flist, audio_extensions, p);
            break;
        }
        case std::filesystem::file_type::directory: {
            if(!recursive) {
                spdlog::debug("ignoring directory {}", p.string());
                break;
            }

            for(const std::filesystem::directory_entry& dir_entry :
                std::filesystem::directory_iterator(p)) {
                add_path_type(seen_files, flist, audio_extensions, dir_entry.path(), dir_entry.status(), recursive);
            }
            break;
        }
        default: break;
    }

}

static void add_path(std::set<std::filesystem::path>& seen_files, std::vector<LrcTag::Result>& flist, const std::set<icu::UnicodeString>& audio_extensions, const std::filesystem::path& p, bool recursive) {
    if(seen_files.count(p) > 0) {
        spdlog::debug("ignoring {} (duplicate)", p.string());
        return;
    }
    add_path_type(seen_files, flist, audio_extensions, p, std::filesystem::status(p), recursive);
}

namespace LrcTag {
    std::vector<Result> Result::loadFiles(const Config& c) {
        std::vector<Result> flist;
        std::set<std::filesystem::path> seen_files;
        std::set<icu::UnicodeString> audio_extensions;

        for(auto it = c.audio_extensions.cbegin(); it != c.audio_extensions.cend(); ++it) {
            spdlog::debug("Treating {} as audio file", *it);

            icu::UnicodeString ext = icu::UnicodeString::fromUTF8(*it);
            if(!ext.startsWith(DOT)) {
                ext = DOT + ext;
            }
            audio_extensions.insert(ext);
        }

        spdlog::debug("Scanning {} paths", c.files.size());
        for(auto it = c.files.cbegin(); it != c.files.cend(); ++it) {
            spdlog::debug("Scanning {}", *it);

            try {
                add_path(seen_files, flist, audio_extensions, std::filesystem::canonical(*it), c.recursive);
            } catch(const std::exception& e) {
                spdlog::warn("Ignoring file {}: {}", *it, e.what());
            }
        }

        return flist;
    }
}
