#ifndef LRCTAG_FILE_LYRICSOURCE_INCLUDE_GUARD
#define LRCTAG_FILE_LYRICSOURCE_INCLUDE_GUARD

#include "lyricsource.h"
#include "../config.h"
#include <filesystem>

namespace LrcTag {
    class FileLyricSource: public LyricSource {
        private:
            unsigned int m_ms;
            std::filesystem::path m_path_lrc;
            std::filesystem::path m_path_txt;
            
            icu::UnicodeString m_unsynched_lyrics;
            std::vector<SynchedLyric> m_synched_lyrics;

            bool m_unsynched_lyrics_attempted;
            bool m_synched_lyrics_attempted;

            void loadUnsynchedLyrics();
            void loadSynchedLyrics();

            static icu::UnicodeString loadFile(const std::filesystem::path& p);

        public:
            FileLyricSource(const std::filesystem::path& p, unsigned int song_ms);
            ~FileLyricSource() override { }

            bool hasSynchronizedLyrics() override {
                loadSynchedLyrics();
                return m_synched_lyrics.size() > 0;
            }

            bool hasUnsynchronizedLyrics() override {
                loadUnsynchedLyrics();
                return m_unsynched_lyrics.length() > 0;
            }

            icu::UnicodeString getUnsynchronizedLyrics() override {
                loadUnsynchedLyrics();
                return m_unsynched_lyrics;
            }

            std::vector<SynchedLyric> getSynchronizedLyrics() override {
                loadSynchedLyrics();
                return m_synched_lyrics;
            }

    };
}

#endif
