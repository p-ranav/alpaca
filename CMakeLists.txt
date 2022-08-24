cmake_minimum_required(VERSION 3.12.4)

project(structbyte
        VERSION 1.0.0
        DESCRIPTION "Serialization library written in C++17 - Pack C++ structs into a compact byte-array without any macros or boilerplate code."
        HOMEPAGE_URL "https://github.com/p-ranav/structbyte"
        LANGUAGES CXX
)

option(STRUCTBYTE_BUILD_TESTS OFF)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
string(REPLACE "/${CMAKE_LIBRARY_ARCHITECTURE}" "" CMAKE_INSTALL_LIBDIR_ARCHIND "${CMAKE_INSTALL_LIBDIR}")


add_library(structbyte INTERFACE)
add_library(structbyte::structbyte ALIAS structbyte)

target_compile_features(structbyte INTERFACE cxx_std_17)
target_include_directories(structbyte INTERFACE
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>)


if(STRUCTBYTE_BUILD_TESTS)
  add_subdirectory(test)
endif()

install(TARGETS structbyte EXPORT structbyteConfig)
install(EXPORT structbyteConfig
        NAMESPACE structbyte::
        DESTINATION ${CMAKE_INSTALL_LIBDIR_ARCHIND}/cmake/${PROJECT_NAME})

install(FILES ${CMAKE_CURRENT_LIST_DIR}/include/structbyte/structbyte.h
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/structbyte)

FILE(GLOB STRUCTBYTE_HEADERS 
    "${PROJECT_SOURCE_DIR}/include/structbyte/detail/*.h"
    )

install(FILES ${STRUCTBYTE_HEADERS}
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/structbyte/detail)

set(CONFIG_FILE_NAME_WITHOUT_EXT "${PROJECT_NAME}Config")
set(CMAKE_CONFIG_FILE_BASENAME "${CMAKE_CURRENT_BINARY_DIR}/${CONFIG_FILE_NAME_WITHOUT_EXT}")
set(CMAKE_CONFIG_VERSION_FILE_NAME "${CMAKE_CONFIG_FILE_BASENAME}-version.cmake")
set(CMAKE_CONFIG_FILE_NAME "${CMAKE_CONFIG_FILE_BASENAME}.cmake")

if(${CMAKE_VERSION} VERSION_GREATER "3.14")
	set(OPTIONAL_ARCH_INDEPENDENT "ARCH_INDEPENDENT")
endif()

write_basic_package_version_file("${CMAKE_CONFIG_VERSION_FILE_NAME}"
    COMPATIBILITY ExactVersion
    ${OPTIONAL_ARCH_INDEPENDENT}
)

export(EXPORT structbyteConfig
       NAMESPACE structbyte::)

install(FILES "${CMAKE_CONFIG_VERSION_FILE_NAME}"
       DESTINATION "${CMAKE_INSTALL_LIBDIR_ARCHIND}/cmake/${PROJECT_NAME}")

set(PackagingTemplatesDir "${CMAKE_CURRENT_SOURCE_DIR}/packaging")

set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_VENDOR "structbyte (C++) developers")
set(CPACK_PACKAGE_DESCRIPTION "${PROJECT_DESCRIPTION}")
set(CPACK_DEBIAN_PACKAGE_NAME "${CPACK_PACKAGE_NAME}")
set(CPACK_RPM_PACKAGE_NAME "${CPACK_PACKAGE_NAME}")
set(CPACK_PACKAGE_HOMEPAGE_URL "${PROJECT_HOMEPAGE_URL}")
set(CPACK_PACKAGE_MAINTAINER "Pranav Srinivas Kumar")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "${CPACK_PACKAGE_MAINTAINER}")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")

set(CPACK_DEBIAN_PACKAGE_NAME "lib${PROJECT_NAME}-dev")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6-dev")
set(CPACK_DEBIAN_PACKAGE_SUGGESTS "cmake, pkg-config, pkg-conf")

set(CPACK_RPM_PACKAGE_NAME "lib${PROJECT_NAME}-devel")
set(CPACK_RPM_PACKAGE_SUGGESTS "${CPACK_DEBIAN_PACKAGE_SUGGESTS}")

set(CPACK_DEB_COMPONENT_INSTALL ON)
set(CPACK_RPM_COMPONENT_INSTALL ON)
set(CPACK_NSIS_COMPONENT_INSTALL ON)
set(CPACK_DEBIAN_COMPRESSION_TYPE "xz")

set(PKG_CONFIG_FILE_NAME "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc")
configure_file("${PackagingTemplatesDir}/pkgconfig.pc.in" "${PKG_CONFIG_FILE_NAME}" @ONLY)
install(FILES "${PKG_CONFIG_FILE_NAME}"
        DESTINATION "${CMAKE_INSTALL_LIBDIR_ARCHIND}/pkgconfig"
)

include(CPack)