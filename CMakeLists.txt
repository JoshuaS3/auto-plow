cmake_minimum_required(VERSION 3.16)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

set(PROJECT_NAME "espy")
set(EXTRA_COMPONENT_DIRS "src") # use src/ instead of main/

project(firmware)

idf_build_set_property(COMPILE_OPTIONS "-fdiagnostics-color=always" APPEND)
if(DEFINED DEV_MODE)
    idf_build_set_property(COMPILE_OPTIONS "-D DEBUG_MODE" APPEND)
endif()
