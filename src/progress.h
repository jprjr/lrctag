#ifndef LRCTAG_PROGRESS_INCLUDE_GUARD
#define LRCTAG_PROGRESS_INCLUDE_GUARD

#include "config.h"

#include <atomic>
#include <spdlog/spdlog.h>

namespace LrcTag {

    class Progress {
        private:
            const Config& m_config;
            unsigned long long m_total;
            std::atomic<unsigned long long> m_processed;
            unsigned long long m_stride;

        public:
            Progress(const Config& config, unsigned long long total)
            : m_config(config), m_total(total), m_processed(0), m_stride(10) {
            }
            ~Progress() { }

            void inc() {
                unsigned long long i = m_processed.fetch_add(1, std::memory_order_relaxed) + 1;
                if(i % m_stride == 0 || i == m_total) {
                    spdlog::info("Processed {}/{} files", i, m_total);
                }
            }

    };
}

#endif
