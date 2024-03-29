include(Compiler/KM2C)
__compiler_clang(KM2)
__compiler_clang_KM2_standards(KM2)

if("x${CMAKE_KM2_COMPILER_FRONTEND_VARIANT}" STREQUAL "xGNU")
  if((NOT DEFINED CMAKE_DEPENDS_USE_COMPILER OR CMAKE_DEPENDS_USE_COMPILER)
      AND CMAKE_GENERATOR MATCHES "Makefiles|WMake"
      AND CMAKE_DEPFILE_FLAGS_KM2)
    # dependencies are computed by the compiler itself
    set(CMAKE_KM2_DEPFILE_FORMAT gcc)
    set(CMAKE_KM2_DEPENDS_USE_COMPILER TRUE)
  endif()

  set(CMAKE_KM2_COMPILE_OPTIONS_EXPLICIT_LANGUAGE -x c++)
  set(CMAKE_KM2_COMPILE_OPTIONS_VISIBILITY_INLINES_HIDDEN "-fvisibility-inlines-hidden")
endif()

cmake_policy(GET CMP0025 appleClangPolicy)
if(APPLE AND NOT appleClangPolicy STREQUAL NEW)
  return()
endif()

if("x${CMAKE_KM2_COMPILER_FRONTEND_VARIANT}" STREQUAL "xMSVC")
  set(CMAKE_KM2_CLANG_TIDY_DRIVER_MODE "cl")
  set(CMAKE_KM2_INCLUDE_WHAT_YOU_USE_DRIVER_MODE "cl")
  if((NOT DEFINED CMAKE_DEPENDS_USE_COMPILER OR CMAKE_DEPENDS_USE_COMPILER)
      AND CMAKE_GENERATOR MATCHES "Makefiles"
      AND CMAKE_DEPFILE_FLAGS_KM2)
    set(CMAKE_KM2_DEPENDS_USE_COMPILER TRUE)
  endif()
endif()
