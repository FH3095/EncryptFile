# CMakeList.txt: CMake-Projekt für "EncryptFile". Schließen Sie die Quelle ein, und definieren Sie
# projektspezifische Logik hier.
#
cmake_minimum_required (VERSION 3.7)
set (CMAKE_CXX_STANDARD 11)

set(INCLUDE_DIRS "include/")
file(GLOB SOURCE_FILES
	src/*.cpp
)

# Fügen Sie der ausführbaren Datei für dieses Projekt eine Quelle hinzu.
add_executable (EncryptFile ${SOURCE_FILES})
include_directories(${INCLUDE_DIRS})


set(SODIUM_PATH "lib/sodium/")
if (WIN32)
	set(SODIUM_NAME "libsodium")
else ()
	set(SODIUM_NAME "sodium")
endif ()

find_path(SODIUM_INCLUDE sodium.h PATHS "${SODIUM_PATH}/include/")
find_library(SODIUM_LIBRARY ${SODIUM_NAME} PATHS "${SODIUM_PATH}/lib/")

if (SODIUM_INCLUDE AND SODIUM_LIBRARY)
	message(STATUS "libsodium found: ${SODIUM_LIBRARY} ${SODIUM_INCLUDE}")
	add_definitions(-DSODIUM_STATIC)
    include_directories(${SODIUM_INCLUDE})
	target_link_libraries(EncryptFile ${SODIUM_LIBRARY})
else()
	message(FATAL_ERROR "Missing libsodium")
endif ()
