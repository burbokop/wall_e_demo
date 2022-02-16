cmake_minimum_required(VERSION 3.5)

project(km2ide LANGUAGES CXX VERSION 0.0.1)

#tell linker to search for .so in same folder as executable
set(CMAKE_EXE_LINKER_FLAGS "-Wl,-rpath='${ORIGIN}'")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(LLVM_LINK_COMPONENTS support)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()


set(COMPONENT_LIST mcparser bitreader support mc option)

set(CLANG_LIBRARY_LIST
    clangFrontend
    clangSerialization
    clangDriver
    clangParse
    clangRewriteFrontend
    clangStaticAnalyzerFrontend
    clangSema
    clangAnalysis
    clangEdit
    clangAST
    clangLex
    clangBasic
    clangTooling
    clangRewrite
    clangASTMatchers
    clangToolingCore
    )


find_package(Qt6 COMPONENTS Core Quick Qml Concurrent)
if (NOT Qt6_FOUND)
    find_package(Qt5 COMPONENTS Core Quick Qml Concurrent REQUIRED)
endif()


set(KM2_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/cpp/cpp_parser.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/cpp/cpp_consumer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/cpp/cpp_parse_action.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/cpp/clang_namespace_node.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/cpp/clang_block_node.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/cpp/clang_function_node.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/cpp/clang_decl_arg_node.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/cpp/clang_type_node.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/cpp/buffered_reverse_range.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/cpp/tools.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/cpp/abstract_clang_node.cpp

    ${CMAKE_CURRENT_LIST_DIR}/src/km2/km2.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/utility/math_patterns.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/utility/function.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/translation_unit/translation_unit.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/translation_unit/capabilities/abstract_capability.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/translation_unit/capabilities/type_capability.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/translation_unit/capabilities/constants_capability.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/translation_unit/capabilities/namespace_capability.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/translation_unit/models/symbol.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/translation_unit/models/function.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/translation_unit/models/overload.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/translation_unit/models/context.cpp

    ${CMAKE_CURRENT_LIST_DIR}/src/km2/lsp/service.cpp

    ${CMAKE_CURRENT_LIST_DIR}/src/km2/tree/abstract/abstract_type_node.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/tree/abstract/abstract_value_node.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/tree/abstract/abstract_node.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/tree/namespace_node.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/tree/function_node.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/tree/call_node.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/tree/asm_node.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/tree/block_node.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/tree/stmt_node.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/tree/internal_block_node.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/tree/arg_node.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/tree/proto_node.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/tree/type_node.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/tree/decl_arg_node.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/tree/const_node.cpp
    )

set(KM2_HEADERS
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/cpp/cpp_parser.h
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/cpp/cpp_consumer.h
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/cpp/cpp_parse_action.h
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/cpp/clang_namespace_node.h
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/cpp/clang_block_node.h
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/cpp/clang_function_node.h
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/cpp/clang_decl_arg_node.h
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/cpp/clang_type_node.h
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/cpp/buffered_reverse_range.h
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/cpp/tools.h
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/cpp/abstract_clang_node.h

    ${CMAKE_CURRENT_LIST_DIR}/src/km2/km2.h
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/utility/math_patterns.h
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/utility/function.h
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/translation_unit/translation_unit.h
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/translation_unit/capabilities/abstract_capability.h
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/translation_unit/capabilities/type_capability.h
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/translation_unit/capabilities/constants_capability.h
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/translation_unit/capabilities/namespace_capability.h
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/translation_unit/models/symbol.h
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/translation_unit/models/function.h
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/translation_unit/models/overload.h
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/translation_unit/models/context.h

    ${CMAKE_CURRENT_LIST_DIR}/src/km2/lsp/service.h

    ${CMAKE_CURRENT_LIST_DIR}/src/km2/tree/abstract/abstract_node.h
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/tree/abstract/abstract_type_node.h
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/tree/abstract/abstract_value_node.h
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/tree/namespace_node.h
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/tree/function_node.h
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/tree/call_node.h
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/tree/asm_node.h
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/tree/block_node.h
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/tree/stmt_node.h
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/tree/internal_block_node.h
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/tree/arg_node.h
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/tree/proto_node.h
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/tree/type_node.h
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/tree/decl_arg_node.h
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/tree/const_node.h
    )


set(SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/src/main.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/to_pdf.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/gui/kgramtreeview.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/gui/mainwindow.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/gui/appcore.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/gui/klibcore/kdelegate.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/gui/klibcore/kdelegatechooser.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/gui/klibcore/klibinfo.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/gui/asmexecutor.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/gui/highlighter.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/gui/jitexecutor.cpp

)
set(HEADERS
    ${CMAKE_CURRENT_LIST_DIR}/src/to_pdf.h
    ${CMAKE_CURRENT_LIST_DIR}/src/gui/kgramtreeview.h
    ${CMAKE_CURRENT_LIST_DIR}/src/gui/mainwindow.h
    ${CMAKE_CURRENT_LIST_DIR}/src/gui/appcore.h
    ${CMAKE_CURRENT_LIST_DIR}/src/gui/klibcore/kdelegate.h
    ${CMAKE_CURRENT_LIST_DIR}/src/gui/klibcore/kdelegatechooser.h
    ${CMAKE_CURRENT_LIST_DIR}/src/gui/klibcore/kmacro.h
    ${CMAKE_CURRENT_LIST_DIR}/src/gui/klibcore/klibinfo.h
    ${CMAKE_CURRENT_LIST_DIR}/src/gui/asmexecutor.h
    ${CMAKE_CURRENT_LIST_DIR}/src/gui/highlighter.h
    ${CMAKE_CURRENT_LIST_DIR}/src/gui/jitexecutor.h
    )



set(LSP_SOURCES
    #src/lsp/main.cpp
    #src/lsp/ioserver.cpp
    #src/lsp/iofind.cpp
    #src/lsp/greeting.cpp
    #src/lsp/index.cpp

)
set(LSP_HEADERS
    #src/lsp/ioserver.h
    #src/lsp/iofind.h
    #src/lsp/greeting.h
    )

add_library(km2 SHARED
    ${KM2_SOURCES}
    ${KM2_HEADERS}
    )
set_property(TARGET km2 PROPERTY AUTOMOC OFF)
set_property(TARGET km2 PROPERTY AUTOUIC OFF)

add_executable(km2ide
    ${SOURCES}
    ${HEADERS}
)

#add_executable(km2lsp
#    ${LSP_SOURCES}
#    ${LSP_HEADERS}
#)


add_library(km2ide_resources SHARED resources.qrc)

target_include_directories(km2 PRIVATE ${CMAKE_CURRENT_LIST_DIR})
target_include_directories(km2ide PRIVATE ${CMAKE_CURRENT_LIST_DIR})
#target_include_directories(km2lsp PRIVATE ${CMAKE_CURRENT_LIST_DIR})


include(${CMAKE_CURRENT_LIST_DIR}/wall_e/wall_e.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/sproc/sproc.cmake)

find_package(LLVM REQUIRED)
find_package(Clang REQUIRED CONFIG)
include_directories(${LLVM_INCLUDE_DIRS})

#clang
include_directories(${LLVM_BUILD_MAIN_SRC_DIR}/tools/clang/include)
include_directories(${LLVM_BUILD_BINARY_DIR}/tools/clang/include)

add_definitions(${LLVM_DEFINITIONS})

#set(LLVM_VERSION 12)

#target_include_directories(km2ide PRIVATE "/usr/include/llvm-${LLVM_VERSION}")
#target_include_directories(km2ide PRIVATE "/usr/include/llvm-c-${LLVM_VERSION}")

#set(LLVM_LIBS
#    LLVMX86AsmParser   # MC, MCParser, Support, X86Desc, X86Info
#    LLVMX86Desc        # MC, Support, X86AsmPrinter, X86Info
#    #LLVMX86AsmPrinter  # MC, Support, X86Utils
#    LLVMX86Info        # MC, Support, Target
#    #LLVMX86Utils       # Core, Support
#    LLVMipo
#    LLVMScalarOpts
#    LLVMInstCombine
#    LLVMTransformUtils
#    #LLVMipa
#    LLVMAnalysis
#    LLVMTarget
#    LLVMOption         # Support
#    LLVMMCParser       # MC, Support
#    LLVMMC             # Object, Support
#    LLVMObject         # BitReader, Core, Support
#    LLVMBitReader      # Core, Support
#    LLVMCore           # Support
#    LLVMSupport
#    )
link_directories(${LLVM_LIBRARY_DIRS})

target_link_libraries(km2
    LLVM-${LLVM_VERSION_MAJOR}
    ${CLANG_LIBRARY_LIST}
    sproc
    )


target_link_libraries(km2ide
    PRIVATE
    km2
    km2ide_resources
    wall_e
    sproc
    pthread
    Qt::Core
    Qt::Qml
    Qt::Quick
    Qt::Concurrent
    #clangTooling
    #clangBasic
    #clangASTMatchers
)

# --- LSP DEPENDENCIES ---
#include(FindPkgConfig)
#pkg_check_modules(LIBCLSP REQUIRED libclsp)
## Header path
#target_include_directories(km2lsp PUBLIC ${LIBCLSP_INCLUDE_DIRS})
## Linking
#target_link_libraries(km2lsp PUBLIC
#    wall_e
#    km2
#    ${LIBCLSP_LIBRARIES})
## Other flags (without this rapidjson can't use std::string)
#target_compile_definitions(km2lsp PUBLIC ${LIBCLSP_CFLAGS_OTHER})


install(TARGETS km2 DESTINATION lib)

install(DIRECTORY "${CMAKE_SOURCE_DIR}/src/km2"
        DESTINATION include
        FILES_MATCHING
        PATTERN "*.h"
)


SET(CPACK_GENERATOR "DEB")
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "KK") #required
INCLUDE(CPack)


# kill lsp
#add_custom_command(
#  TARGET km2
#  COMMENT "build km2lsp"
#  POST_BUILD
#  WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/src/lsp/routes
#  COMMAND npm install && node-gyp build
#)



# kill lsp
#add_custom_command(
#  TARGET km2lsp
#  COMMENT "killall km2lsp processes"
#  POST_BUILD
#  COMMAND killall -9 km2lsp || true
#)
