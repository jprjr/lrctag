#ifndef LRCTAG_WORKER_INCLUDE_GUARD
#define LRCTAG_WORKER_INCLUDE_GUARD

#include "lyricsource/lyricsourcefactory.h"
#include "lyricdest/lyricdestfactory.h"
#include "container/containerfactory.h"
#include "report.h"
#include "utils.h"
#include "config.h"

#include <atomic>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace LrcTag {
    class Worker {
        private:
            const Config& m_config;
            int m_id;
            std::atomic<unsigned long long>& m_index;
            const std::vector<std::filesystem::path>& m_paths;
            const std::string m_thread_name;
            ContainerFactory m_container;
            std::shared_ptr<spdlog::logger> m_logger;

            void initLogger();
            void process(const LyricSourceFactory& lsf, const LyricDestFactory& lsd, const std::filesystem::path& path);

        public:
            Worker(const Config& conf, int id, std::atomic<unsigned long long>& index, const std::vector<std::filesystem::path>& paths)
            : m_config(conf), m_id(id), m_index(index), m_paths(paths), m_thread_name(lrctag_string_format("thread %d", id+1)), m_container(conf) {
                m_logger = spdlog::stderr_color_mt(m_thread_name);
            }

            ~Worker() { }

            void work(const LyricSourceFactory& lsf, const LyricDestFactory& lsd);
            void report(std::vector<Report>& reports);

    };
}

#endif
