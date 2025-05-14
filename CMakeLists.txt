cmake_minimum_required(VERSION 3.12)
project(lrctag LANGUAGES C CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

#include(CheckIPOSupported)
#check_ipo_supported(RESULT LTO_SUPPORTED)

find_package(TagLib 1.12 REQUIRED)
find_package(spdlog REQUIRED)
find_package(argparse REQUIRED)
find_package(ICU COMPONENTS uc i18n REQUIRED)
find_package(CURL COMPONENTS HTTPS REQUIRED)
find_package(nlohmann_json REQUIRED)

set(lrctag_sources)

list(APPEND lrctag_sources "src/main.cpp")
list(APPEND lrctag_sources "src/lrc.cpp")
list(APPEND lrctag_sources "src/config.cpp")
list(APPEND lrctag_sources "src/utils.cpp")
list(APPEND lrctag_sources "src/unicode.cpp")
list(APPEND lrctag_sources "src/worker.cpp")
list(APPEND lrctag_sources "src/scanner.cpp")
list(APPEND lrctag_sources "src/debuglistener.cpp")
list(APPEND lrctag_sources "src/container/container.cpp")
list(APPEND lrctag_sources "src/container/containerfactory.cpp")
list(APPEND lrctag_sources "src/taghandler/taghandler.cpp")
list(APPEND lrctag_sources "src/lyricsource/filelyricsource.cpp")
list(APPEND lrctag_sources "src/lyricsource/lyricsourcefactory.cpp")
list(APPEND lrctag_sources "src/lyricdest/lyricdestfactory.cpp")
list(APPEND lrctag_sources "src/lyricdest/filelyricdest.cpp")

add_executable(lrctag ${lrctag_sources})

# TagLib >= 2.0 requires C++17, and we rely on std::filesystem (which is c++17)
set_property(TARGET lrctag PROPERTY CXX_STANDARD 17)

target_link_libraries(lrctag PRIVATE TagLib::TagLib spdlog::spdlog ICU::uc ICU::i18n CURL::libcurl nlohmann_json::nlohmann_json)

#if(LTO_SUPPORTED)
#    set_property(TARGET lrctag PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
#endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(lrctag PRIVATE "-Wall")
    target_compile_options(lrctag PRIVATE "-Wextra")
    target_compile_options(lrctag PRIVATE "-Wcast-align")
endif()

if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
    target_compile_options(lrctag PRIVATE "-Wall")
    target_compile_options(lrctag PRIVATE "-Wextra")
endif()

install(TARGETS lrctag
    DESTINATION "${CMAKE_INSTALL_PREFIX}/bin")

