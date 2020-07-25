project (fpd)

find_package(Threads)

if (UNIX)
 find_package(OpenSSL REQUIRED)
endif (UNIX)

link_directories(
  C:/opencv/build/lib/Release
)

ADD_EXECUTABLE(fpd fpd.cpp)

SET_PROPERTY(TARGET fpd PROPERTY CXX_STANDARD 17)

SET (CMAKE_CXX_FLAGS "/Zi /Od")

SET_TARGET_PROPERTIES(fpd PROPERTIES 
  LINK_FLAGS "/DEBUG /OPT:REF /OPT:ICF"
)

TARGET_INCLUDE_DIRECTORIES(
 fpd
 PRIVATE
 ${CMAKE_SOURCE_DIR}
 ${CMAKE_SOURCE_DIR}/INCLUDE
 ${CMAKE_SOURCE_DIR}/../cpp-npl
 ${CMAKE_SOURCE_DIR}/../cpp-npl/INCLUDE
 ${CMAKE_SOURCE_DIR}/../cpp-cvl
 ${CMAKE_SOURCE_DIR}/../cpp-cvl/INCLUDE
 C:/opencv/build/install/include
)

if (WIN32)
 TARGET_LINK_LIBRARIES(fpd Ws2_32.lib opencv_world440)
else()
 TARGET_LINK_LIBRARIES(fpd ${CMAKE_THREAD_LIBS_INIT} OpenSSL::SSL)
endif (WIN32)