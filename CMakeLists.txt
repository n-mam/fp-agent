project (fpd)

set(CMAKE_VERBOSE_MAKEFILE ON)

find_package(Threads REQUIRED)
find_package(OpenCV REQUIRED)
find_package(OpenSSL REQUIRED)

# SET OPENSSL_ROOT_DIR=C:/vcpkg/installed/x64-windows

ADD_EXECUTABLE(fpd fpd.cpp)

target_compile_definitions(fpd PUBLIC OPENVINO)
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
)

SET(OVL_LIBS "pt fr ag")

TARGET_LINK_LIBRARIES(
  fpd 
  fd
  pt
  fr
  ${OV_LIBS} 
  ${OpenCV_LIBS} 
  OpenSSL::SSL 
  ${CMAKE_THREAD_LIBS_INIT} 
  Ws2_32.lib
)

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