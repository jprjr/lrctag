#ifndef LRCTAG_DEBUGLISTENER_INCLUDE_GUARD
#define LRCTAG_DEBUGLISTENER_INCLUDE_GUARD

#include <tdebuglistener.h>
#include <spdlog/spdlog.h>

class LRCTAGDebugListener: public TagLib::DebugListener {
    public:
        LRCTAGDebugListener()
        : TagLib::DebugListener() { }
        ~LRCTAGDebugListener() override { }

        void printMessage(const TagLib::String& msg) override;

        static void setLogger(std::shared_ptr<spdlog::logger> l);
        static std::shared_ptr<spdlog::logger> getLogger();
};


#endif
