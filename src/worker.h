#ifndef LRCTAG_WORKER_INCLUDE_GUARD
#define LRCTAG_WORKER_INCLUDE_GUARD

#include "result.h"
#include "lyricsource/lyricsourcefactory.h"
#include "lyricdest/lyricdestfactory.h"

#include <atomic>
#include <spdlog/spdlog.h>

namespace LrcTag {
    class Worker {
        private:
            int m_id;

            void process(const std::shared_ptr<spdlog::logger> logger, const LyricSourceFactory& lsf, const LyricDestFactory& lsd, Result& result);

        public:
            constexpr Worker(int id)
            : m_id(id) { }

            ~Worker() { }

            void work(const LyricSourceFactory& lsf, const LyricDestFactory& lsd, std::atomic<unsigned long long>& index, std::vector<Result>& results);

    };
}

#endif
