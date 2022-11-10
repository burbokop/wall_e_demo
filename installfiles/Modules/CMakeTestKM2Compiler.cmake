

if(CMAKE_KM2_COMPILER_FORCED)
  # The compiler configuration was forced by the user.
  # Assume the user has configured all compiler information.
  set(CMAKE_KM2_COMPILER_WORKS TRUE)
  return()
endif()

include(CMakeTestCompilerCommon)

# work around enforced code signing and / or missing executable target type
set(__CMAKE_SAVED_TRY_COMPILE_TARGET_TYPE ${CMAKE_TRY_COMPILE_TARGET_TYPE})
if(_CMAKE_FEATURE_DETECTION_TARGET_TYPE)
  set(CMAKE_TRY_COMPILE_TARGET_TYPE ${_CMAKE_FEATURE_DETECTION_TARGET_TYPE})
endif()

# Remove any cached result from an older CMake version.
# We now store this in CMakeKM2Compiler.cmake.
unset(CMAKE_KM2_COMPILER_WORKS CACHE)

# Try to identify the ABI and configure it into CMakeKM2Compiler.cmake
#include(${CMAKE_ROOT}/Modules/CMakeDetermineCompilerABI.cmake)CMAKE_DETERMINE_COMPILER_ABI(KM2 ./CMakeKM2CompilerABI.km2)
#if(CMAKE_KM2_ABI_COMPILED)
#  # The compiler worked so skip dedicated test below.
set(CMAKE_KM2_COMPILER_WORKS TRUE)
#  message(STATUS "Check for working KM2 compiler: ${CMAKE_KM2_COMPILER} - skipped")
#endif()

# This file is used by EnableLanguage in cmGlobalGenerator to
# determine that the selected C++ compiler can actually compile
# and link the most basic of programs.   If not, a fatal error
# is set and cmake stops processing commands and will not generate
# any makefiles or projects.


#[[
if(NOT CMAKE_KM2_COMPILER_WORKS)
  PrintTestCompilerStatus("KM2")
  __TestCompiler_setTryCompileTargetType()
  file(WRITE ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeTmp/testKM2Compiler.KM2
    "#ifndef __cplusplus\n"
    "# error \"The CMAKE_KM2_COMPILER is set to a C compiler\"\n"
    "#endif\n"
    "int main(){return 0;}\n")
  # Clear result from normal variable.
  unset(CMAKE_KM2_COMPILER_WORKS)
  # Puts test result in cache variable.
  try_compile(CMAKE_KM2_COMPILER_WORKS ${CMAKE_BINARY_DIR}
    ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeTmp/testKM2Compiler.KM2
    OUTPUT_VARIABLE __CMAKE_KM2_COMPILER_OUTPUT)
  # Move result from cache to normal variable.
  set(CMAKE_KM2_COMPILER_WORKS ${CMAKE_KM2_COMPILER_WORKS})
  unset(CMAKE_KM2_COMPILER_WORKS CACHE)
  __TestCompiler_restoreTryCompileTargetType()
  if(NOT CMAKE_KM2_COMPILER_WORKS)
    PrintTestCompilerResult(CHECK_FAIL "broken")
    file(APPEND ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeError.log
      "Determining if the KM2 compiler works failed with "
      "the following output:\n${__CMAKE_KM2_COMPILER_OUTPUT}\n\n")
    string(REPLACE "\n" "\n  " _output "${__CMAKE_KM2_COMPILER_OUTPUT}")
    message(FATAL_ERROR "The C++ compiler\n  \"${CMAKE_KM2_COMPILER}\"\n"
      "is not able to compile a simple test program.\nIt fails "
      "with the following output:\n  ${_output}\n\n"
      "CMake will not be able to correctly generate this project.")
  endif()
  PrintTestCompilerResult(CHECK_PASS "works")
  file(APPEND ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeOutput.log
    "Determining if the KM2 compiler works passed with "
    "the following output:\n${__CMAKE_KM2_COMPILER_OUTPUT}\n\n")
endif()

# Try to identify the compiler features
include(${CMAKE_ROOT}/Modules/CMakeDetermineCompileFeatures.cmake)
CMAKE_DETERMINE_COMPILE_FEATURES(KM2)

# Re-configure to save learned information.
configure_file(
  ${CMAKE_ROOT}/Modules/CMakeKM2Compiler.cmake.in
  ${CMAKE_PLATFORM_INFO_DIR}/CMakeKM2Compiler.cmake
  @ONLY
  )
include(${CMAKE_PLATFORM_INFO_DIR}/CMakeKM2Compiler.cmake)

if(CMAKE_KM2_SIZEOF_DATA_PTR)
  foreach(f ${CMAKE_KM2_ABI_FILES})
    include(${f})
  endforeach()
  unset(CMAKE_KM2_ABI_FILES)
endif()

set(CMAKE_TRY_COMPILE_TARGET_TYPE ${__CMAKE_SAVED_TRY_COMPILE_TARGET_TYPE})
unset(__CMAKE_SAVED_TRY_COMPILE_TARGET_TYPE)
unset(__CMAKE_KM2_COMPILER_OUTPUT)
]]