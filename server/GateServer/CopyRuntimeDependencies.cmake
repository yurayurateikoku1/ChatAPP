# CopyRuntimeDependencies.cmake
cmake_minimum_required(VERSION 3.16)

if(NOT DEFINED SRC_DLLS OR NOT DEFINED DEST_DIR)
    message(FATAL_ERROR "SRC_DLLS and DEST_DIR must be defined")
endif()

foreach(dll IN LISTS SRC_DLLS)
    if(EXISTS "${dll}")
        file(COPY "${dll}" DESTINATION "${DEST_DIR}")
    else()
        message(WARNING "Runtime dependency not found: ${dll}")
    endif()
endforeach()
