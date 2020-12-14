cmake_minimum_required(VERSION 3.0)

project (fpd)

# set(CMAKE_VERBOSE_MAKEFILE ON)
# SET OPENCV_DIR=C:/opencv/build/install
# SET OPENSSL_ROOT_DIR=C:/vcpkg/installed/x64-windows

find_package(Threads REQUIRED)
find_package(OpenCV REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(InferenceEngine 2.0 REQUIRED)

macro(ie_add_sample_cvl)
    set(options EXCLUDE_CPPLINT)
    set(oneValueArgs NAME)
    set(multiValueArgs SOURCES HEADERS DEPENDENCIES OPENCV_DEPENDENCIES INCLUDE_DIRECTORIES)
    cmake_parse_arguments(IE_SAMPLE "${options}" "${oneValueArgs}"
                          "${multiValueArgs}" ${ARGN} )

    # Find OpenCV components if exist
    if(IE_SAMPLE_OPENCV_DEPENDENCIES)
        find_package(OpenCV COMPONENTS ${IE_SAMPLE_OPENCV_DEPENDENCIES} QUIET)
        if(NOT OpenCV_FOUND)
            message(WARNING "OPENCV is disabled or not found, " ${IE_SAMPLE_NAME} " skipped")
            return()
        else()
            add_definitions(-DUSE_OPENCV)
        endif()
    endif()

    # Create named folders for the sources within the .vcproj
    # Empty name lists them directly under the .vcproj
    source_group("src" FILES ${IE_SAMPLE_SOURCES})
    if(IE_SAMPLE_HEADERS)
        source_group("include" FILES ${IE_SAMPLE_HEADERS})
    endif()

    # Create executable file from sources
    add_executable(${IE_SAMPLE_NAME} ${IE_SAMPLE_SOURCES} ${IE_SAMPLE_HEADERS})

    if(WIN32)
        set_target_properties(${IE_SAMPLE_NAME} PROPERTIES COMPILE_PDB_NAME ${IE_SAMPLE_NAME})
    endif()

    if(IE_SAMPLE_INCLUDE_DIRECTORIES)
        target_include_directories(${IE_SAMPLE_NAME} PRIVATE ${IE_SAMPLE_INCLUDE_DIRECTORIES})
    endif()
    target_include_directories(${IE_SAMPLE_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../common")

    target_link_libraries(${IE_SAMPLE_NAME} PRIVATE ${OpenCV_LIBRARIES} ${InferenceEngine_LIBRARIES}
                                                    ${IE_SAMPLE_DEPENDENCIES} Ws2_32.lib)

    if(UNIX)
        target_link_libraries(${IE_SAMPLE_NAME} PRIVATE pthread)
    endif()

    # create global target with all samples / demo apps
    if(NOT TARGET ie_samples)
        add_custom_target(ie_samples ALL)
    endif()
    add_dependencies(ie_samples ${IE_SAMPLE_NAME})

    if(COMMAND add_cpplint_target AND NOT IE_SAMPLE_EXCLUDE_CPPLINT)
        add_cpplint_target(${IE_SAMPLE_NAME}_cpplint FOR_TARGETS ${IE_SAMPLE_NAME})
    endif()
endmacro()

ie_add_sample_cvl(
    NAME fpd
    SOURCES fpd.cpp)

SET_PROPERTY(TARGET fpd PROPERTY CXX_STANDARD 17)

if (WIN32)
  SET (CMAKE_CXX_FLAGS_RELEASE "/Zi /Od")
  SET_TARGET_PROPERTIES(
   fpd PROPERTIES 
   LINK_FLAGS 
   "/DEBUG /OPT:REF /OPT:ICF"
  )
endif (WIN32)

TARGET_INCLUDE_DIRECTORIES(
 fpd
 PRIVATE
 ${CMAKE_CURRENT_SOURCE_DIR}
 ${CMAKE_CURRENT_SOURCE_DIR}/INCLUDE
 ${CMAKE_CURRENT_SOURCE_DIR}/../cpp-npl
 ${CMAKE_CURRENT_SOURCE_DIR}/../cpp-npl/INCLUDE
 ${CMAKE_CURRENT_SOURCE_DIR}/../cpp-cvl
 ${CMAKE_CURRENT_SOURCE_DIR}/../cpp-cvl/INCLUDE
 ${CMAKE_CURRENT_SOURCE_DIR}/../cpp-osl
 ${CMAKE_CURRENT_SOURCE_DIR}/../cpp-osl/INCLUDE
)

target_link_libraries(fpd PRIVATE fr)

# NSIS installer logic for windows

if (WIN32)

SET(CMAKE_INSTALL_DEBUG_LIBRARIES TRUE)
SET(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/build/install)

install(
  TARGETS 
  fpd 
  DESTINATION 
  ${CPACK_NSIS_INSTALL_ROOT}/${CPACK_PACKAGE_INSTALL_DIRECTORY}
)

# Include this module to search for compiler-provided system runtime libraries 
# and add install rules for them. Some optional variables may be set PRIOR to 
# including the module to adjust behavior
SET(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION ${CPACK_NSIS_INSTALL_ROOT}/${CPACK_PACKAGE_INSTALL_DIRECTORY})
include(InstallRequiredSystemLibraries)

install(
  PROGRAMS
  $ENV{OPENSSL_ROOT_DIR}/bin/libssl-1_1-x64.dll
  $ENV{OPENSSL_ROOT_DIR}/bin/libcrypto-1_1-x64.dll
  $ENV{OPENCV_DIR}/x64/vc16/bin/opencv_world440.dll
  $ENV{OPENCV_DIR}/x64/vc16/bin/opencv_videoio_ffmpeg440_64.dll
  "C:/PROGRA~2/IntelSWTools/compilers_and_libraries/windows/redist/intel64_win/tbb/vc_mt/tbb.dll"
  "C:/PROGRA~2/IntelSWTools/compilers_and_libraries/windows/redist/intel64_win/compiler/libiomp5md.dll"
  DESTINATION
  ${CPACK_NSIS_INSTALL_ROOT}/${CPACK_PACKAGE_INSTALL_DIRECTORY}
)

install(
  FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/../cpp-cvl/MODELS/age_net.caffemodel
  ${CMAKE_CURRENT_SOURCE_DIR}/../cpp-cvl/MODELS/deploy_age.prototxt
  ${CMAKE_CURRENT_SOURCE_DIR}/../cpp-cvl/MODELS/deploy_gender.prototxt
  ${CMAKE_CURRENT_SOURCE_DIR}/../cpp-cvl/MODELS/gender_net.caffemodel
  ${CMAKE_CURRENT_SOURCE_DIR}/../cpp-cvl/MODELS/MobileNetSSD_deploy.caffemodel
  ${CMAKE_CURRENT_SOURCE_DIR}/../cpp-cvl/MODELS/MobileNetSSD_deploy.prototxt
  ${CMAKE_CURRENT_SOURCE_DIR}/../cpp-cvl/MODELS/ResNetSSD_deploy.caffemodel
  ${CMAKE_CURRENT_SOURCE_DIR}/../cpp-cvl/MODELS/ResNetSSD_deploy.prototxt
  DESTINATION
  MODELS
)

# Start after install checkbox
#set(CPACK_NSIS_MUI_FINISHPAGE_RUN "fpd.exe")

# Uninstall first, then install
set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)

# Kill before uninstall
set(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "ExecWait 'Taskkill /F /IM fpd.exe'")

# Kill before install
set(CPACK_NSIS_EXTRA_PREINSTALL_COMMANDS "ExecWait 'Taskkill /F /IM fpd.exe'")

SET(CPACK_NSIS_MUI_ICON "${CMAKE_CURRENT_SOURCE_DIR}/foxpad.ico")

SET(CPACK_NSIS_MUI_UNIICON "${CMAKE_CURRENT_SOURCE_DIR}/foxpad.ico")

# Icon in the add-remove control panel. Must be in an .exe file
set(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\fpd.exe")

# License file
set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_CURRENT_SOURCE_DIR}/license.txt)

set(CPACK_GENERATOR NSIS)
set(CPACK_PACKAGE_NAME "Agent")
set(CPACK_PACKAGE_VENDOR "Divya Surana")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "FP Agent")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "Foxpad")

set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "
  CreateShortCut \\\"$DESKTOP\\\\fpd.lnk\\\" \\\"$INSTDIR\\\\fpd.exe\\\"
")

set(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "
  Delete \\\"$DESKTOP\\\\fpd.lnk\\\"
")

INCLUDE(CPack)

endif (WIN32)