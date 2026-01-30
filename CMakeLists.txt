cmake_minimum_required(VERSION 3.23)

project(Herold)

# find_package(PkgConfig REQUIRED)
# pkg_check_modules(AVCODEC		REQUIRED IMPORTED_TARGET libavcodec)
# pkg_check_modules(AVFORMAT		REQUIRED IMPORTED_TARGET libavformat)
# pkg_check_modules(AVFILTER		REQUIRED IMPORTED_TARGET libavfilter)
# pkg_check_modules(AVDEVICE		REQUIRED IMPORTED_TARGET libavdevice)
# pkg_check_modules(AVUTIL		REQUIRED IMPORTED_TARGET libavutil)
# pkg_check_modules(SWRESAMPLE		REQUIRED IMPORTED_TARGET libswresample)
# pkg_check_modules(SWSCALE		REQUIRED IMPORTED_TARGET libswscale)
# add_library(FFmpeg INTERFACE IMPORTED GLOBAL)
# target_link_libraries(FFmpeg INTERFACE
# 	PkgConfig::AVCODEC
# 	PkgConfig::AVFORMAT
# 	PkgConfig::AVFILTER
# 	PkgConfig::AVDEVICE
# 	PkgConfig::AVUTIL
# 	PkgConfig::SWRESAMPLE
# 	PkgConfig::SWSCALE
# )

include(FetchContent)
FetchContent_Declare(
  potamos
  GIT_REPOSITORY  https://github.com/xardasmat/potamos.git
  GIT_TAG         main
)
FetchContent_MakeAvailable(potamos)

FetchContent_Declare(
  piper
  GIT_REPOSITORY  https://github.com/OHF-Voice/piper1-gpl.git
  GIT_TAG         v1.4.0
)

FetchContent_GetProperties(piper)
if(NOT piper_POPULATED)
  FetchContent_Populate(piper)

  # 3. Manually add ONLY the libpiper subfolder
  # This avoids building tests, docs, or other python wrappers in the root
  add_subdirectory("${piper_SOURCE_DIR}/libpiper" "${piper_BINARY_DIR}/libpiper")
endif()

add_executable(herold src/herold.cc)

target_include_directories(herold PRIVATE "${potamos_SOURCE_DIR}/src")
target_include_directories(herold PRIVATE "${piper_SOURCE_DIR}/libpiper/include")

# Now link it to your executable
target_link_libraries(herold PRIVATE FFmpeg piper)