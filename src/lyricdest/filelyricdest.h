#ifndef LRCTAG_FILE_LYRICDEST_INCLUDE_GUARD
#define LRCTAG_FILE_LYRICDEST_INCLUDE_GUARD

#include "lyricdest.h"
#include "../config.h"
#include <filesystem>

namespace LrcTag {
    class FileLyricDest: public LyricDest {
        private:
            const Config& m_config;
            std::filesystem::path m_path_lrc;
            std::filesystem::path m_path_txt;

            bool shouldWriteLRC();
            bool shouldWriteTXT();

        public:
            FileLyricDest(const Config& config, const std::filesystem::path& p);
            ~FileLyricDest() override { }

            bool needsSynchronizedLyrics() override;
            bool needsUnsynchronizedLyrics() override;

            bool saveSynchronizedLyrics(const std::vector<SynchedLyric>& sl) override;
            bool saveUnsynchronizedLyrics(const icu::UnicodeString& ul) override;
    };
}

#endif
