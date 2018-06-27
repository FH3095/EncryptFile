# CMakeList.txt: CMake-Projektdatei der obersten Ebene. Führen Sie eine globale Konfiguration durch,
# und schließen Sie hier Unterprojekte ein.
#
cmake_minimum_required (VERSION 3.7)

project ("EncryptFile" VERSION 1.0)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_HOME_DIRECTORY}/bin/")

# Schließen Sie Unterprojekte ein.
add_subdirectory ("EncryptFile")
