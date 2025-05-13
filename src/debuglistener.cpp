#include "debuglistener.h"

#include <thread>

static thread_local std::shared_ptr<spdlog::logger> logger = nullptr;

void LRCTAGDebugListener::printMessage(const TagLib::String& msg) {
    logger->warn(msg.to8Bit(true));
    return;
}

void LRCTAGDebugListener::setLogger(std::shared_ptr<spdlog::logger> l) {
    logger = l;
}

std::shared_ptr<spdlog::logger> LRCTAGDebugListener::getLogger() {
    return logger;
}
