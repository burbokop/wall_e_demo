
include(${CMAKE_ROOT}/Modules/CMakeDetermineCompiler.cmake)

# Load system-specific compiler preferences for this language.
include(Platform/${CMAKE_SYSTEM_NAME}-Determine-KM2 OPTIONAL)
include(Platform/${CMAKE_SYSTEM_NAME}-KM2 OPTIONAL)
if(NOT CMAKE_KM2_COMPILER_NAMES)
  set(CMAKE_KM2_COMPILER_NAMES KM2C)
endif()

message("CMAKE_KM2_COMPILER_NAMES: ${CMAKE_KM2_COMPILER_NAMES}")

if(NOT CMAKE_KM2_COMPILER)
  message("ENTERING BRANCH(NOT CMAKE_KM2_COMPILER)")
  set(CMAKE_KM2_COMPILER_INIT NOTFOUND)

  # prefer the environment variable KM2
  if(NOT $ENV{KM2} STREQUAL "")
    get_filename_component(CMAKE_KM2_COMPILER_INIT $ENV{KM2} PROGRAM PROGRAM_ARGS CMAKE_KM2_FLAGS_ENV_INIT)
    if(CMAKE_KM2_FLAGS_ENV_INIT)
      set(CMAKE_KM2_COMPILER_ARG1 "${CMAKE_KM2_FLAGS_ENV_INIT}" CACHE STRING "Arguments to KM2 compiler")
    endif()
    if(NOT EXISTS ${CMAKE_KM2_COMPILER_INIT})
      message(FATAL_ERROR "Could not find compiler set in environment variable KM2:\n$ENV{KM2}.\n${CMAKE_KM2_COMPILER_INIT}")
    endif()
  endif()

  message("CMAKE_KM2_COMPILER_INIT: ${CMAKE_KM2_COMPILER_INIT}")
  message("CMAKE_KM2_COMPILER_ARG1: ${CMAKE_KM2_COMPILER_ARG1}")

  # next prefer the generator specified compiler
  if(CMAKE_GENERATOR_KM2)
    if(NOT CMAKE_KM2_COMPILER_INIT)
      set(CMAKE_KM2_COMPILER_INIT ${CMAKE_GENERATOR_KM2})
    endif()
  endif()


  message("CMAKE_KM2_COMPILER_INIT: ${CMAKE_KM2_COMPILER_INIT}")
  
  # finally list compilers to try
  if(NOT CMAKE_KM2_COMPILER_INIT)
    set(CMAKE_KM2_COMPILER_LIST KM2 ${_CMAKE_TOOLCHAIN_PREFIX}km2 ${_CMAKE_TOOLCHAIN_PREFIX}km2c km2c)
  endif()

  message("CMAKE_KM2_COMPILER_INIT: ${CMAKE_KM2_COMPILER_INIT}")


  _cmake_find_compiler(KM2)
else()
  _cmake_find_compiler_path(KM2)
endif()
mark_as_advanced(CMAKE_KM2_COMPILER)

message("CMAKE_KM2_COMPILER: ${CMAKE_KM2_COMPILER}")


  # Each entry in this list is a set of extra flags to try
  # adding to the compile line to see if it helps produce
  # a valid identification file.
set(CMAKE_KM2_COMPILER_ID_TEST_FLAGS_FIRST)
set(CMAKE_KM2_COMPILER_ID_TEST_FLAGS
  # Try compiling to an object file only.
  "-c"
  # IAR does not detect language automatically
  "--c++"
  "--ec++"

  # ARMClang need target options
  "--target=arm-arm-none-eabi -mcpu=cortex-m3"

  # MSVC needs at least one include directory for __has_include to function,
  # but custom toolchains may run MSVC with no INCLUDE env var and no -I flags.
  # Also avoid linking so this works with no LIB env var.
  "-c -I__does_not_exist__"
  )

if(CMAKE_KM2_COMPILER_TARGET)
  set(CMAKE_KM2_COMPILER_ID_TEST_FLAGS_FIRST "-c --target=${CMAKE_KM2_COMPILER_TARGET}")
endif()

#[[
# Build a small source file to identify the compiler.
if(NOT CMAKE_KM2_COMPILER_ID_RUN)
  set(CMAKE_KM2_COMPILER_ID_RUN 1)

  # Try to identify the compiler.
  set(CMAKE_KM2_COMPILER_ID)
  set(CMAKE_KM2_PLATFORM_ID)
  file(READ ${CMAKE_ROOT}/Modules/CMakePlatformId.h.in
    CMAKE_KM2_COMPILER_ID_PLATFORM_CONTENT)

  # The IAR compiler produces weird output.
  # See https://gitlab.kitware.com/cmake/cmake/-/issues/10176#note_153591
  list(APPEND CMAKE_KM2_COMPILER_ID_VENDORS IAR)
  set(CMAKE_KM2_COMPILER_ID_VENDOR_FLAGS_IAR )
  set(CMAKE_KM2_COMPILER_ID_VENDOR_REGEX_IAR "IAR .+ Compiler")

  # Match the link line from xcodebuild output of the form
  #  Ld ...
  #      ...
  #      /path/to/cc ...CompilerIdKM2/...
  # to extract the compiler front-end for the language.
  set(CMAKE_KM2_COMPILER_ID_TOOL_MATCH_REGEX "\nLd[^\n]*(\n[ \t]+[^\n]*)*\n[ \t]+([^ \t\r\n]+)[^\r\n]*-o[^\r\n]*CompilerIdKM2/(\\./)?(CompilerIdKM2.(framework|xctest|build/[^ \t\r\n]+)/)?CompilerIdKM2[ \t\n\\\"]")
  set(CMAKE_KM2_COMPILER_ID_TOOL_MATCH_INDEX 2)

  include(${CMAKE_ROOT}/Modules/CMakeDetermineCompilerId.cmake)
  CMAKE_DETERMINE_COMPILER_ID(KM2 KM2FLAGS CMakeKM2CompilerId.cpp)

  _cmake_find_compiler_sysroot(KM2)

  # Set old compiler and platform id variables.
  if(CMAKE_KM2_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_COMPILER_IS_GNUKM2 1)
  endif()
else()
  if(NOT DEFINED CMAKE_KM2_COMPILER_FRONTEND_VARIANT)
    # Some toolchain files set our internal CMAKE_KM2_COMPILER_ID_RUN
    # variable but are not aware of CMAKE_KM2_COMPILER_FRONTEND_VARIANT.
    # They pre-date our support for the GNU-like variant targeting the
    # MSVC ABI so we do not consider that here.
    if(CMAKE_KM2_COMPILER_ID STREQUAL "Clang"
      OR "x${CMAKE_KM2_COMPILER_ID}" STREQUAL "xIntelLLVM")
      if("x${CMAKE_KM2_SIMULATE_ID}" STREQUAL "xMSVC")
        set(CMAKE_KM2_COMPILER_FRONTEND_VARIANT "MSVC")
      else()
        set(CMAKE_KM2_COMPILER_FRONTEND_VARIANT "GNU")
      endif()
    else()
      set(CMAKE_KM2_COMPILER_FRONTEND_VARIANT "")
    endif()
  endif()
endif()
]]
if (NOT _CMAKE_TOOLCHAIN_LOCATION)
  get_filename_component(_CMAKE_TOOLCHAIN_LOCATION "${CMAKE_KM2_COMPILER}" PATH)
endif ()

# if we have a g++ cross compiler, they have usually some prefix, like
# e.g. powerpc-linux-g++, arm-elf-g++ or i586-mingw32msvc-g++ , optionally
# with a 3-component version number at the end (e.g. arm-eabi-gcc-4.5.2).
# The other tools of the toolchain usually have the same prefix
# NAME_WE cannot be used since then this test will fail for names like
# "arm-unknown-nto-qnx6.3.0-gcc.exe", where BASENAME would be
# "arm-unknown-nto-qnx6" instead of the correct "arm-unknown-nto-qnx6.3.0-"

#[[
if (NOT _CMAKE_TOOLCHAIN_PREFIX)

  if("${CMAKE_KM2_COMPILER_ID}" MATCHES "GNU|Clang|QCC|LCC")
    get_filename_component(COMPILER_BASENAME "${CMAKE_KM2_COMPILER}" NAME)
    if (COMPILER_BASENAME MATCHES "^(.+-)?(clang\\+\\+|[gc]\\+\\+|clang-cl)(-[0-9]+(\\.[0-9]+)*)?(-[^.]+)?(\\.exe)?$")
      set(_CMAKE_TOOLCHAIN_PREFIX ${CMAKE_MATCH_1})
      set(_CMAKE_TOOLCHAIN_SUFFIX ${CMAKE_MATCH_3})
      set(_CMAKE_COMPILER_SUFFIX ${CMAKE_MATCH_5})
    elseif("${CMAKE_KM2_COMPILER_ID}" MATCHES "Clang")
      if(CMAKE_KM2_COMPILER_TARGET)
        set(_CMAKE_TOOLCHAIN_PREFIX ${CMAKE_KM2_COMPILER_TARGET}-)
      endif()
    elseif(COMPILER_BASENAME MATCHES "QCC(\\.exe)?$")
      if(CMAKE_KM2_COMPILER_TARGET MATCHES "gcc_nto([a-z0-9]+_[0-9]+|[^_le]+)(le)")
        set(_CMAKE_TOOLCHAIN_PREFIX nto${CMAKE_MATCH_1}-)
      endif()
    endif ()

    # if "llvm-" is part of the prefix, remove it, since llvm doesn't have its own binutils
    # but uses the regular ar, objcopy, etc. (instead of llvm-objcopy etc.)
    if ("${_CMAKE_TOOLCHAIN_PREFIX}" MATCHES "(.+-)?llvm-$")
      set(_CMAKE_TOOLCHAIN_PREFIX ${CMAKE_MATCH_1})
    endif ()
  elseif("${CMAKE_KM2_COMPILER_ID}" MATCHES "TI")
    # TI compilers are named e.g. cl6x, cl470 or armcl.exe
    get_filename_component(COMPILER_BASENAME "${CMAKE_KM2_COMPILER}" NAME)
    if (COMPILER_BASENAME MATCHES "^(.+)?cl([^.]+)?(\\.exe)?$")
      set(_CMAKE_TOOLCHAIN_PREFIX "${CMAKE_MATCH_1}")
      set(_CMAKE_TOOLCHAIN_SUFFIX "${CMAKE_MATCH_2}")
    endif ()

  endif()

endif ()
]]
set(_CMAKE_PROCESSING_LANGUAGE "KM2")
include(CMakeFindBinUtils)
include(Compiler/${CMAKE_KM2_COMPILER_ID}-FindBinUtils OPTIONAL)
unset(_CMAKE_PROCESSING_LANGUAGE)

if(CMAKE_KM2_COMPILER_SYSROOT)
  string(CONCAT _SET_CMAKE_KM2_COMPILER_SYSROOT
    "set(CMAKE_KM2_COMPILER_SYSROOT \"${CMAKE_KM2_COMPILER_SYSROOT}\")\n"
    "set(CMAKE_COMPILER_SYSROOT \"${CMAKE_KM2_COMPILER_SYSROOT}\")")
else()
  set(_SET_CMAKE_KM2_COMPILER_SYSROOT "")
endif()

if(CMAKE_KM2_COMPILER_ARCHITECTURE_ID)
  set(_SET_CMAKE_KM2_COMPILER_ARCHITECTURE_ID
    "set(CMAKE_KM2_COMPILER_ARCHITECTURE_ID ${CMAKE_KM2_COMPILER_ARCHITECTURE_ID})")
else()
  set(_SET_CMAKE_KM2_COMPILER_ARCHITECTURE_ID "")
endif()

if(MSVC_KM2_ARCHITECTURE_ID)
  set(SET_MSVC_KM2_ARCHITECTURE_ID
    "set(MSVC_KM2_ARCHITECTURE_ID ${MSVC_KM2_ARCHITECTURE_ID})")
endif()

if(CMAKE_KM2_XCODE_ARCHS)
  set(SET_CMAKE_XCODE_ARCHS
    "set(CMAKE_XCODE_ARCHS \"${CMAKE_KM2_XCODE_ARCHS}\")")
endif()

message("CMAKE_PLATFORM_INFO_DIR: ${CMAKE_PLATFORM_INFO_DIR}")
message("CMAKE_CURRENT_LIST_DIR: ${CMAKE_CURRENT_LIST_DIR}")

# configure all variables set in this file
configure_file(${CMAKE_CURRENT_LIST_DIR}/CMakeKM2Compiler.cmake.in
  ${CMAKE_PLATFORM_INFO_DIR}/CMakeKM2Compiler.cmake
  @ONLY
  )

set(CMAKE_KM2_COMPILER_ENV_VAR "KM2")
