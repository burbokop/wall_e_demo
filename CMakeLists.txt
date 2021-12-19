cmake_minimum_required(VERSION 3.5)

project(km2ide LANGUAGES CXX)

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


find_package(Qt6 COMPONENTS Core Quick Qml)
if (NOT Qt6_FOUND)
    find_package(Qt5 COMPONENTS Core Quick Qml REQUIRED)
endif()

set(SOURCES
    src/main.cpp
    src/to_pdf.cpp
    src/gui/kgramtreeview.cpp
    src/gui/mainwindow.cpp
    src/gui/appcore.cpp
    src/gui/klibcore/kdelegate.cpp
    src/gui/klibcore/kdelegatechooser.cpp
    src/gui/klibcore/klibinfo.cpp
    src/gui/asmexecutor.cpp
    src/gui/highlighter.cpp
    src/gui/jitexecutor.cpp
    src/cpp/cpp_parser.cpp
    src/cpp/cpp_consumer.cpp
    src/cpp/cpp_parse_action.cpp
    src/cpp/cpp_visitor.cpp
    src/cpp/clang_namespace_node.cpp
    src/cpp/clang_block_node.cpp
    src/cpp/clang_function_node.cpp
    src/cpp/clang_decl_arg_node.cpp
    src/cpp/clang_type_node.cpp
    src/cpp/buffered_reverse_range.cpp
    src/cpp/tools.cpp
    src/cpp/abstract_clang_node.cpp
    src/km2/km2.cpp
    src/km2/utility/math_patterns.cpp
    src/km2/utility/function.cpp
    src/km2/module.cpp
    src/km2/error.cpp

    src/km2/tree/abstract/abstract_type_node.cpp
    src/km2/tree/abstract/abstract_value_node.cpp
    src/km2/tree/abstract/abstract_node.cpp
    src/km2/tree/namespace_node.cpp
    src/km2/tree/function_node.cpp
    src/km2/tree/call_node.cpp
    src/km2/tree/asm_node.cpp
    src/km2/tree/block_node.cpp
    src/km2/tree/stmt_node.cpp
    src/km2/tree/internal_block_node.cpp
    src/km2/tree/arg_node.cpp
    src/km2/tree/proto_node.cpp
    src/km2/tree/type_node.cpp
    src/km2/tree/decl_arg_node.cpp
    src/km2/tree/const_node.cpp
)
set(HEADERS
    src/to_pdf.h
    src/gui/kgramtreeview.h
    src/gui/mainwindow.h
    src/gui/appcore.h
    src/gui/klibcore/kdelegate.h
    src/gui/klibcore/kdelegatechooser.h
    src/gui/klibcore/kmacro.h
    src/gui/klibcore/klibinfo.h
    src/gui/asmexecutor.h
    src/gui/highlighter.h
    src/gui/jitexecutor.h
    src/cpp/cpp_parser.h
    src/cpp/cpp_consumer.h
    src/cpp/cpp_parse_action.h
    src/cpp/cpp_visitor.h
    src/cpp/clang_namespace_node.h
    src/cpp/clang_block_node.h
    src/cpp/clang_function_node.h
    src/cpp/clang_decl_arg_node.h
    src/cpp/clang_type_node.h
    src/cpp/buffered_reverse_range.h
    src/cpp/tools.h
    src/cpp/abstract_clang_node.h
    src/km2/km2.h
    src/km2/utility/math_patterns.h
    src/km2/utility/function.h
    src/km2/module.h
    src/km2/error.h

    src/km2/tree/abstract/abstract_node.h
    src/km2/tree/abstract/abstract_type_node.h
    src/km2/tree/abstract/abstract_value_node.h
    src/km2/tree/namespace_node.h
    src/km2/tree/function_node.h
    src/km2/tree/call_node.h
    src/km2/tree/asm_node.h
    src/km2/tree/block_node.h
    src/km2/tree/stmt_node.h
    src/km2/tree/internal_block_node.h
    src/km2/tree/arg_node.h
    src/km2/tree/proto_node.h
    src/km2/tree/type_node.h
    src/km2/tree/decl_arg_node.h    
    src/km2/tree/const_node.h
    )




add_executable(km2ide
    ${SOURCES}
    ${HEADERS}
)


add_library(km2ide_resources SHARED resources.qrc)

target_include_directories(km2ide PRIVATE ${CMAKE_CURRENT_LIST_DIR})


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
target_link_libraries(km2ide
    PRIVATE
    km2ide_resources
    wall_e
    sproc
    pthread
    LLVM-${LLVM_VERSION_MAJOR}
    ${CLANG_LIBRARY_LIST}
    Qt::Core
    Qt::Qml
    Qt::Quick
    #clangTooling
    #clangBasic
    #clangASTMatchers
)


#target_link_libraries(km2ide
#  PRIVATE
#  clangTooling
#  clangBasic
#  clangASTMatchers
#  )
