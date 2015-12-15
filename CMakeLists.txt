project(imageencoder.cximage)

cmake_minimum_required(VERSION 2.6)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR})

enable_language(CXX)

find_package(kodi REQUIRED)
find_package(JPEG REQUIRED)
find_package(PNG REQUIRED)
find_package(ZLIB REQUIRED)
find_package(TIFF REQUIRED)
find_package(Jasper REQUIRED)

add_definitions(-DXBMC)

add_subdirectory(lib/cximage-6.0)

include_directories(${KODI_INCLUDE_DIR}
                    ${ZLIB_INCLUDE_DIRS}
                    ${JPEG_INCLUDE_DIRS}
                    ${TIFF_INCLUDE_DIRS}
                    ${PNG_INCLUDE_DIRS}
                    ${JASPER_INCLUDE_DIR}
                    lib/cximage-6.0/CxImage)

set(CXIMAGE_SOURCES src/cximage.cpp
                    src/DllInterface.cpp)

set(DEPLIBS -Wl,--whole-archive cximage -Wl,-no-whole-archive
            ${ZLIB_LIBRARIES}
            ${JPEG_LIBRARIES}
            ${TIFF_LIBRARIES}
            ${PNG_LIBRARIES}
            ${JASPER_LIBRARIES})

build_addon(imageencoder.cximage CXIMAGE DEPLIBS)

include(CPack)
