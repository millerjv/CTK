#
# DCMTK
#

ctkMacroShouldAddExternalproject(DCMTK_LIBRARIES add_project)
if(${add_project})

  # Sanity checks
  if(DEFINED DCMTK_DIR AND NOT EXISTS ${DCMTK_DIR})
    message(FATAL_ERROR "DCMTK_DIR variable is defined but corresponds to non-existing directory")
  endif()

  set(DCMTK_enabling_variable DCMTK_LIBRARIES)

  set(proj DCMTK)
  set(proj_DEPENDENCIES)

  list(APPEND CTK_DEPENDENCIES ${proj})

  set(${DCMTK_enabling_variable}_INCLUDE_DIRS DCMTK_INCLUDE_DIR)
  set(${DCMTK_enabling_variable}_FIND_PACKAGE_CMD DCMTK)

  if(CTK_SUPERBUILD)

    if(NOT DEFINED DCMTK_DIR)
      set(revision_tag ae3b946f6e6231)
      if(${proj}_REVISION_TAG)
        set(revision_tag ${${proj}_REVISION_TAG})
      endif()
      
      set(location_args )
      if(${proj}_URL)
        set(location_args URL ${${proj}_URL})
      elseif(${proj}_GIT_REPOSITORY)
        set(location_args GIT_REPOSITORY ${${proj}_GIT_REPOSITORY}
                          GIT_TAG ${revision_tag})
      else()
        set(location_args GIT_REPOSITORY "${git_protocol}://git.dcmtk.org/dcmtk.git"
                          GIT_TAG ${revision_tag})
      endif()

      #message(STATUS "Adding project:${proj}")
      ExternalProject_Add(${proj}
        SOURCE_DIR ${CMAKE_BINARY_DIR}/${proj}
        BINARY_DIR ${proj}-build
        PREFIX ${proj}${ep_suffix}
        ${location_args}
        CMAKE_GENERATOR ${gen}
        UPDATE_COMMAND ""
        BUILD_COMMAND ""
        CMAKE_CACHE_ARGS
          ${ep_common_cache_args}
          -DBUILD_SHARED_LIBS:BOOL=OFF
          -DDCMTK_WITH_DOXYGEN:BOOL=OFF
          -DDCMTK_WITH_ZLIB:BOOL=OFF # see github issue #25
          -DDCMTK_WITH_OPENSSL:BOOL=OFF # see github issue #25
          -DDCMTK_WITH_PNG:BOOL=OFF # see github issue #25
          -DDCMTK_WITH_TIFF:BOOL=OFF  # see github issue #25
          -DDCMTK_WITH_XML:BOOL=OFF  # see github issue #25
          -DDCMTK_FORCE_FPIC_ON_UNIX:BOOL=ON
          -DDCMTK_OVERWRITE_WIN32_COMPILER_FLAGS:BOOL=OFF
        )
      set(DCMTK_DIR ${ep_install_dir})

  # This was used during heavy development on DCMTK itself.
  # Disabling it for now. (It also leads to to build errors
  # with the XCode CMake generator on Mac).
  #
  #    ExternalProject_Add_Step(${proj} force_rebuild
  #      COMMENT "Force ${proj} re-build"
  #      DEPENDERS build    # Steps that depend on this step
  #      ALWAYS 1
  #      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/${proj}-build
  #      DEPENDS
  #        ${proj_DEPENDENCIES}
  #      )

      # Since DCMTK is statically build, there is not need to add its corresponding
      # library output directory to CTK_EXTERNAL_LIBRARY_DIRS

    else()
      ctkMacroEmptyExternalproject(${proj} "${proj_DEPENDENCIES}")
    endif()

    list(APPEND CTK_SUPERBUILD_EP_VARS DCMTK_DIR:PATH)

  endif()

endif()
