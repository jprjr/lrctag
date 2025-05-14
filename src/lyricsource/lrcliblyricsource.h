#ifndef LRCTAG_LRCLIB_LYRICSOURCE_INCLUDE_GUARD
#define LRCTAG_LRCLIB_LYRICSOURCE_INCLUDE_GUARD

#include "lyricsource.h"

#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <cstring>

#include "../debuglistener.h"
#include "../lrc.h"

namespace LrcTag {
    class LrcLibLyricSource: public LyricSource {
        private:
            const Config& m_config;
            const Container* m_container;
            bool m_http_called;

            icu::UnicodeString m_unsynched_lyrics;
            std::vector<SynchedLyric> m_synched_lyrics;


            static std::size_t writeCbC(void* buffer, size_t size, size_t nmeb, void* userp) {
                std::string* data = (std::string*) userp;

                data->append((char*)buffer, size * nmeb);
                return size * nmeb;
            }

            void getLyrics() {
                CURLcode res = CURLE_OK;
                CURLHcode h = CURLHE_OK;
                CURL* handle = NULL;
                struct curl_header *type = NULL;
                char* title_escaped = NULL;
                char* artist_escaped = NULL;
                char* album_escaped = NULL;
                auto logger = LRCTAGDebugListener::getLogger();
                std::string url = m_config.lrclib_endpoint;

                if(!m_http_called) {
                    handle = curl_easy_init();
                    if(handle == NULL) {
                        logger->critical("error creating CURL handle");
                        goto cleanup;
                    }

                    std::string title = m_container->tag()->title<std::string>();
                    std::string artist = m_container->tag()->artist<std::string>();
                    std::string album = m_container->tag()->album<std::string>();
                    std::string duration = std::to_string( ((double)m_container->audioProperties()->lengthInMilliseconds()) / 1000.0);
                    std::string response;

                    if(title.length() == 0) {
                        logger->warn("{}: missing title tag", m_container->path().string());
                        goto cleanup;
                    }

                    if(artist.length() == 0) {
                        logger->warn("{}: missing artist tag", m_container->path().string());
                        goto cleanup;
                    }

                    if(album.length() == 0) {
                        logger->warn("{}: missing album tag", m_container->path().string());
                        goto cleanup;
                    }

                    title_escaped = curl_easy_escape(handle,
                      title.c_str(), title.length());
                    artist_escaped = curl_easy_escape(handle,
                      artist.c_str(), artist.length());
                    album_escaped = curl_easy_escape(handle,
                      album.c_str(), album.length());

                    url += 
                      "?track_name="
                      + std::string(title_escaped)
                      + "&artist_name="
                      + std::string(artist_escaped)
                      + "&album_name="
                      + std::string(album_escaped)
                      + "&duration=" + duration;

                    logger->debug("calling url: {}", url);

                    curl_easy_setopt(handle, CURLOPT_URL, url.c_str());
                    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, writeCbC);
                    curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void*) &response);

                    res = curl_easy_perform(handle);

                    m_http_called = true;

                    if(res != CURLE_OK) {
                        logger->warn("curl_easy_perform error: {}", curl_easy_strerror(res));
                        goto cleanup;
                    }

                    h = curl_easy_header(handle, "Content-Type", 0, CURLH_HEADER, -1, &type);
                    if(h != CURLHE_OK) {
                        logger->warn("curl_easy_header error");
                        goto cleanup;
                    }

                    logger->debug("response: {}", response);
                    if(strcmp(type->value,"application/json") != 0) {
                        logger->warn("api returned non-json: {}", type->value);
                        goto cleanup;
                    }

                    auto res = nlohmann::json::parse(response);
                    if(res.contains("plainLyrics") &&
                      res.at("plainLyrics").is_string()) {
                        m_unsynched_lyrics = icu::UnicodeString::fromUTF8(res.at("plainLyrics").get<std::string>());
                    }
                    if(res.contains("syncedLyrics") &&
                      res.at("syncedLyrics").is_string()) {
                        m_synched_lyrics = LRC::parse(
                          icu::UnicodeString::fromUTF8(res.at("syncedLyrics").get<std::string>())
                        , m_container->audioProperties()->lengthInMilliseconds());
                    }
                }

                cleanup:
                if(handle != NULL) curl_easy_cleanup(handle);
                if(title_escaped != NULL) curl_free(title_escaped);
                if(artist_escaped != NULL) curl_free(artist_escaped);
                if(album_escaped != NULL) curl_free(album_escaped);
            }

        public:
            LrcLibLyricSource(const Config& conf, const Container* container)
            : m_config(conf), m_container(container), m_http_called(false) {
            }

            ~LrcLibLyricSource() override { }

            bool hasSynchronizedLyrics() override {
                getLyrics();
                return m_synched_lyrics.size() > 0;
            }

            bool hasUnsynchronizedLyrics() override {
                getLyrics();
                return m_unsynched_lyrics.length() > 0;
            }

            icu::UnicodeString getUnsynchronizedLyrics() override {
                getLyrics();
                return m_unsynched_lyrics;
            }

            std::vector<SynchedLyric> getSynchronizedLyrics() override {
                getLyrics();
                return m_synched_lyrics;
            }

    };
}

#endif
