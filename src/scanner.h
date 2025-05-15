#ifndef LRCTAG_SCANNER_INCLUDE_GUARD
#define LRCTAG_SCANNER_INCLUDE_GUARD

#include "config.h"

#include <filesystem>
#include <vector>
#include <set>

#include <unicode/unistr.h>

namespace LrcTag {
    class FsScanner {
        private:
            const Config& m_config;
            std::set<std::filesystem::path> m_files;
            std::set<icu::UnicodeString> m_exts;
            static const icu::UnicodeString DOT;

            void addPath(const std::filesystem::path&, const std::filesystem::file_status&);
            void addPath(const std::filesystem::path&);
            void scanFiles();
        public:
            FsScanner(const Config& conf);
            ~FsScanner() { }

            std::vector<std::filesystem::path> getFiles();
    };
}

#endif
