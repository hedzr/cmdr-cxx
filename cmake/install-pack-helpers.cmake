

# https://stackoverflow.com/questions/57092692/how-to-fix-vcpkg-cmake-is-unable-to-determine-target-architecture-windows10-v
if (DEFINED ENV{VCPKG_DEFAULT_TRIPLET} AND NOT DEFINED VCPKG_TARGET_TRIPLET)
  set(VCPKG_TARGET_TRIPLET "$ENV{VCPKG_DEFAULT_TRIPLET}" CACHE STRING "")
endif ()


macro(add_install_chapter target)

  include(GNUInstallDirs)
  include(CMakePackageConfigHelpers)

  set(CONFIG_PACKAGE_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

  get_target_property(target_type ${target} TYPE)

  #debug_print_value(CMAKE_PROJECT_VERSION)
  #debug_print_value(PROJECT_VERSION)
  message(STATUS " * CMAKE_PROJECT_VERSION = ${CMAKE_PROJECT_VERSION}")
  message(STATUS " * PROJECT_VERSION = ${PROJECT_VERSION}")

  write_basic_package_version_file(
          ${CMAKE_CURRENT_BINARY_DIR}/${target}-config-version.cmake
          VERSION ${PROJECT_VERSION}
          COMPATIBILITY SameMajorVersion
  )

  install(TARGETS ${target}
          EXPORT ${target}Targets
          LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
          ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
          RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
          DESTINATION ${CMAKE_INSTALL_BINDIR}
          )

  message(STATUS "configure install section for ${target_type} ${target}")
  if (target_type STREQUAL "EXECUTABLE")
    # Process executable target

    # dep libs
    # man 1, man 3, ...
    # shared stuffs

  else ()

    # install(TARGETS ${CLI_NAME}
    #         EXPORT ${PROJECT_NAME}Targets
    #         DESTINATION ${CMAKE_INSTALL_BINDIR}
    #         )

    install(EXPORT ${target}Targets
            FILE ${target}Targets.cmake
            NAMESPACE ${target}::
            DESTINATION ${CONFIG_PACKAGE_INSTALL_DIR})

    install(DIRECTORY include/${target}
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
            )

    install(FILES
            ${CMAKE_CURRENT_LIST_DIR}/cmake/${target}Config.cmake
            DESTINATION
            ${CMAKE_INSTALL_LIBDIR}/cmake/${target})

    # install(FILES
    #         ${CMAKE_BINARY_DIR}/generated/cmdr-version.hh
    #         DESTINATION
    #         ${CMAKE_INSTALL_INCLUDEDIR}/${target})
    # 
    # install(FILES
    #         ${CMAKE_CURRENT_SOURCE_DIR}/include/cmdr-cxx.hh
    #         DESTINATION
    #         ${CMAKE_INSTALL_INCLUDEDIR})

    #install(TARGETS ${target}
    #        # COMPONENT applications
    #        # DESTINATION "bin"
    #        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    #        )
  endif ()

endmacro()

macro(add_cpack_chapter target)

  # https://cmake.org/cmake/help/latest/module/CPack.html

  # build a CPack driven installer package
  include(InstallRequiredSystemLibraries)
  set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
  set(CPACK_PACKAGE_DIRECTORY ${CMAKE_SOURCE_DIR}/dist)
  set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
  set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
  set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
  set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
  set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
  set(CPACK_PACKAGE_DESCRIPTION "${CMAKE_PROJECT_DESCRIPTION}")
  set(CPACK_PACKAGE_HOMEPAGE "${CMAKE_PROJECT_HOMEPAGE_URL}")
  set(CPACK_SOURCE_IGNORE_FILES
      ${PROJECT_SOURCE_DIR}/build
      ${PROJECT_SOURCE_DIR}/cmake-build-debug
      ${PROJECT_SOURCE_DIR}/dist
      ${PROJECT_SOURCE_DIR}/.idea
      ${PROJECT_SOURCE_DIR}/.DS_Store
      ${PROJECT_SOURCE_DIR}/.git
      ${PROJECT_SOURCE_DIR}/.gitignore
      ${PROJECT_SOURCE_DIR}/.vscode
      ${PROJECT_SOURCE_DIR}/.PIC
      ${PROJECT_SOURCE_DIR}/ref
      ${PROJECT_SOURCE_DIR}/_assets/*)
  #set(CPACK_SOURCE_GENERATOR "TXZ")
  #set(CPACK_SOURCE_PACKAGE_FILE_NAME ${ARCHIVE_NAME})
  #  set(CPACK_PACKAGE_DESCRIPTION "A C++17 header-only command-line parser with hierarchical config data manager")
  #set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "")
  #  set(CPACK_PACKAGE_CONTACT "Hedzr Yeh <hedzrz@gmail.com>")
  #  set(CPACK_PACKAGE_VENDOR "Hedzr Studio")
  #  set(CPACK_PACKAGE_HOMEPAGE "https://github.com/hedzr/cmdr-cxx")

  get_target_property(target_type ${target} TYPE)

  if (DISTRO_ID STREQUAL "debian")
    message(STATUS ">>>> Found Debian <<<<")
    include(package-deb)
  elseif (DISTRO_ID STREQUAL "ubuntu")
    message(STATUS ">>>> Found Ubuntu <<<<")
    include(package-deb)
    if (target_type STREQUAL "EXECUTABLE")
      # Process executable target
      set(CPACK_DEBIAN_PACKAGE_NAME "${target}")
    else ()
      set(CPACK_DEBIAN_PACKAGE_NAME "lib${target}-dev")
    endif ()
  elseif (DISTRO_ID STREQUAL "fedora")
    message(STATUS ">>>> Found Fedora <<<<")
    include(package-rpm)
  elseif (DISTRO_ID STREQUAL "centos")  # DISTRO_NAME = CentOS
    message(STATUS ">>>> Found Fedora <<<<")
    include(package-rpm)
  elseif (DISTRO_ID STREQUAL "redhat")  # DISTRO_NAME = RehHat ?
    message(STATUS ">>>> Found RedHat <<<<")
    include(package-rpm)
  elseif (macOS)
    message(STATUS ">>>> Found macOS/Darwin <<<<")
    include(package-dmg)
  else ()
    message(STATUS ">>>> Found unknown distribution (DISTRO_NAME=${DISTRO_NAME}, ID=${DISTRO_ID}) <<<<")
  endif ()

  #if (NOT CPack_CMake_INCLUDED)
  include(CPack)
  #endif ()

endmacro()
