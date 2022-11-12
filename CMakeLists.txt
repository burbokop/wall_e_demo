cmake_minimum_required(VERSION 3.18.4)

project(km2ide LANGUAGES CXX VERSION 0.0.1)

#tell linker to search for .so in same folder as executable
set(CMAKE_EXE_LINKER_FLAGS "-Wl,-rpath='${ORIGIN}'")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(WIN32)
    set(LINK_ALL_STATIC ON)
endif()

if(MSVC)
    message("CURRENT COMPILER MSVC")
    add_compile_options("/MT")
#  target_compile_options(<target> PUBLIC "/ZI")
#  target_link_options(<target> PUBLIC "/INCREMENTAL")
endif()
#/MD, /MT, /LD


set(LLVM_LINK_COMPONENTS support)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

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

message("CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")


set(KM2_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/km2.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/ast_token.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/utility/math_patterns.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/utility/function.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/utility/markup_string.cpp

    ${CMAKE_CURRENT_LIST_DIR}/src/km2/lsp/service.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/lsp/tables.service.cpp

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
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/tree/imp_node.cpp

    ${CMAKE_CURRENT_LIST_DIR}/src/km2/backend/backend.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/backend/unit/unit.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/backend/unit/capabilities/abstract_capability.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/backend/unit/capabilities/type_capability.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/backend/unit/capabilities/constants_capability.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/backend/unit/capabilities/namespace_capability.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/backend/entities/function.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/backend/entities/type.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/backend/entities/value.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/backend/entities/block.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/backend/entities/argument.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/backend/models/symbol.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/backend/models/overload.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/backend/models/context.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/backend/models/function_ref.cpp    
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/backend/entities/printable.cpp
    )

set(KM2_HEADERS
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/km2.h
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/ast_token.h
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/utility/math_patterns.h
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/utility/function.h
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/utility/markup_string.h

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
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/tree/imp_node.h

    ${CMAKE_CURRENT_LIST_DIR}/src/km2/backend/backend.h
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/backend/as.h
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/backend/unit/unit.h
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/backend/unit/capabilities/abstract_capability.h
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/backend/unit/capabilities/type_capability.h
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/backend/unit/capabilities/constants_capability.h
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/backend/unit/capabilities/namespace_capability.h
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/backend/entities/function.h
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/backend/entities/type.h
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/backend/entities/value.h
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/backend/entities/block.h
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/backend/entities/argument.h
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/backend/models/symbol.h
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/backend/models/overload.h
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/backend/models/context.h
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/backend/models/function_ref.h
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/backend/entities/printable.h
)

set(KM2_LLVM_BACKEND_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/llvm_backend/llvm_backend.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/llvm_backend/unit/unit.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/llvm_backend/unit/capabilities/abstract_capability.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/llvm_backend/unit/capabilities/type_capability.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/llvm_backend/unit/capabilities/constants_capability.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/llvm_backend/unit/capabilities/namespace_capability.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/llvm_backend/entities/function.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/llvm_backend/entities/type.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/llvm_backend/entities/value.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/llvm_backend/entities/block.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/llvm_backend/entities/argument.cpp
)

set(KM2_LLVM_BACKEND_HEADERS
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/llvm_backend/llvm_backend.h
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/llvm_backend/unit/unit.h
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/llvm_backend/unit/private.unit.h
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/llvm_backend/unit/capabilities/abstract_capability.h
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/llvm_backend/unit/capabilities/type_capability.h
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/llvm_backend/unit/capabilities/constants_capability.h
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/llvm_backend/unit/capabilities/namespace_capability.h
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/llvm_backend/entities/function.h
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/llvm_backend/entities/type.h
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/llvm_backend/entities/value.h
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/llvm_backend/entities/block.h
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/llvm_backend/entities/argument.h
)

set(KM2_INTERPRETER_BACKEND_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/interpreter_backend/interpreter_backend.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/interpreter_backend/unit/unit.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/interpreter_backend/unit/capabilities/abstract_capability.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/interpreter_backend/unit/capabilities/type_capability.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/interpreter_backend/unit/capabilities/constants_capability.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/interpreter_backend/unit/capabilities/namespace_capability.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/interpreter_backend/entities/function.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/interpreter_backend/entities/type.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/interpreter_backend/entities/value.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/interpreter_backend/entities/block.cpp    
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/interpreter_backend/entities/argument.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/interpreter_backend/entities/constant.cpp

    ${CMAKE_CURRENT_LIST_DIR}/src/km2/interpreter_backend/instructions/instruction.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/interpreter_backend/instructions/math_instructions.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/interpreter_backend/instructions/call_instruction.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/interpreter_backend/instructions/execution_ctx.cpp    
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/interpreter_backend/instructions/dyn_value.cpp
)

set(KM2_INTERPRETER_BACKEND_HEADERS
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/interpreter_backend/interpreter_backend.h
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/interpreter_backend/unit/unit.h
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/interpreter_backend/unit/capabilities/abstract_capability.h
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/interpreter_backend/unit/capabilities/type_capability.h
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/interpreter_backend/unit/capabilities/constants_capability.h
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/interpreter_backend/unit/capabilities/namespace_capability.h
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/interpreter_backend/entities/function.h
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/interpreter_backend/entities/type.h
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/interpreter_backend/entities/value.h
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/interpreter_backend/entities/block.h
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/interpreter_backend/entities/argument.h
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/interpreter_backend/entities/constant.h

    ${CMAKE_CURRENT_LIST_DIR}/src/km2/interpreter_backend/instructions/instruction.h
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/interpreter_backend/instructions/math_instructions.h
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/interpreter_backend/instructions/call_instruction.h
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/interpreter_backend/instructions/execution_ctx.h
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/interpreter_backend/instructions/dyn_value.h
    ${CMAKE_CURRENT_LIST_DIR}/src/km2/interpreter_backend/instructions/size_t_ptr.h
)

if(CLANG_ENABLED)
    set(KM2_CPP_SOURCES
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
    )
endif()

if(CLANG_ENABLED)
    set(KM2_CPP_HEADERS
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
    )
endif()


set(KM2C_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/src/km2c/main.cpp
)
set(KM2C_HEADERS

)


set(SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/src/main.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/to_pdf.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/gui/either.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/gui/kgramtreeview.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/gui/mainwindow.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/gui/appcore.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/gui/klibcore/kdelegate.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/gui/klibcore/kdelegatechooser.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/gui/klibcore/klibinfo.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/gui/asmexecutor.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/gui/presentation/highlighter.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/gui/jitexecutor.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/gui/backendfactory.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/gui/compiler.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/gui/presentation/presentor.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/gui/presentation/theme.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/gui/presentation/lspservice.cpp
)
set(HEADERS
    ${CMAKE_CURRENT_LIST_DIR}/src/to_pdf.h
    ${CMAKE_CURRENT_LIST_DIR}/src/gui/either.h
    ${CMAKE_CURRENT_LIST_DIR}/src/gui/kgramtreeview.h
    ${CMAKE_CURRENT_LIST_DIR}/src/gui/mainwindow.h
    ${CMAKE_CURRENT_LIST_DIR}/src/gui/appcore.h
    ${CMAKE_CURRENT_LIST_DIR}/src/gui/klibcore/kdelegate.h
    ${CMAKE_CURRENT_LIST_DIR}/src/gui/klibcore/kdelegatechooser.h
    ${CMAKE_CURRENT_LIST_DIR}/src/gui/klibcore/kmacro.h
    ${CMAKE_CURRENT_LIST_DIR}/src/gui/klibcore/klibinfo.h
    ${CMAKE_CURRENT_LIST_DIR}/src/gui/asmexecutor.h
    ${CMAKE_CURRENT_LIST_DIR}/src/gui/presentation/highlighter.h
    ${CMAKE_CURRENT_LIST_DIR}/src/gui/jitexecutor.h
    ${CMAKE_CURRENT_LIST_DIR}/src/gui/backendfactory.h
    ${CMAKE_CURRENT_LIST_DIR}/src/gui/compiler.h
    ${CMAKE_CURRENT_LIST_DIR}/src/gui/presentation/presentor.h
    ${CMAKE_CURRENT_LIST_DIR}/src/gui/presentation/theme.h
    ${CMAKE_CURRENT_LIST_DIR}/src/gui/presentation/lspservice.h
    )

if(LINK_ALL_STATIC)
    add_library(km2 STATIC ${KM2_SOURCES} ${KM2_HEADERS})
else()
    add_library(km2 SHARED ${KM2_SOURCES} ${KM2_HEADERS})
endif()

set_property(TARGET km2 PROPERTY AUTOMOC OFF)
set_property(TARGET km2 PROPERTY AUTOUIC OFF)

add_executable(km2c
    ${KM2C_SOURCES}
    ${KM2C_HEADERS}
)

add_executable(km2ide
    ${SOURCES}
    ${HEADERS}
)


if(LINK_ALL_STATIC)
    add_library(km2ide_resources STATIC resources.qrc)
else()
    add_library(km2ide_resources SHARED resources.qrc)
endif()


find_package(Qt6 REQUIRED COMPONENTS Core Quick Qml Concurrent)

target_link_libraries(km2ide_resources Qt::Core)



if(LINK_ALL_STATIC)
    add_library(km2_interpreter_backend STATIC ${KM2_INTERPRETER_BACKEND_SOURCES} ${KM2_INTERPRETER_BACKEND_HEADERS})
else()
    add_library(km2_interpreter_backend SHARED ${KM2_INTERPRETER_BACKEND_SOURCES} ${KM2_INTERPRETER_BACKEND_HEADERS})
endif()


if(LLVM_ENABLED)

    if(LINK_ALL_STATIC)
        add_library(km2_llvm_backend STATIC ${KM2_LLVM_BACKEND_SOURCES} ${KM2_LLVM_BACKEND_HEADERS})
    else()
        add_library(km2_llvm_backend SHARED ${KM2_LLVM_BACKEND_SOURCES} ${KM2_LLVM_BACKEND_HEADERS})
    endif()


    find_package(LLVM REQUIRED)
    add_definitions(${LLVM_DEFINITIONS})
    add_compile_definitions(LLVM_ENABLED)

    message("LLVM_LIBRARY_LIST: ${LLVM_LIBRARY_LIST}")
    message("LLVM_LIBS: ${LLVM_LIBS}")
    message("LLVM_LIBRARIES: ${LLVM_LIBRARIES}")

    if (WIN32)
        llvm_map_components_to_libnames(llvm_libs mcparser bitreader support mc option)
        message("llvm_libs: ${llvm_libs}")
        message("LLVM_DYLIB_COMPONENTS: ${LLVM_DYLIB_COMPONENTS}")
        message("link_components: ${link_components}")
        message("LLVM_LINK_COMPONENTS: ${LLVM_LINK_COMPONENTS}")
        message("expanded_components: ${expanded_components}")
        set(KM2_LLVM_LIBRARIES ${llvm_libs})
    elseif (UNIX)
        set(KM2_LLVM_LIBRARIES LLVM-${LLVM_VERSION_MAJOR})
    endif()

    message("KM2_LLVM_LIBRARIES: ${KM2_LLVM_LIBRARIES}")

    target_include_directories(km2_llvm_backend PRIVATE ${LLVM_INCLUDE_DIRS})
    target_link_directories(km2_llvm_backend PRIVATE ${LLVM_LIBRARY_DIRS})

    target_compile_definitions(km2_llvm_backend PRIVATE KM2_LLVM_BACKEND_TARGET)

    target_link_libraries(km2_llvm_backend
        km2
        wall_e
        ${KM2_LLVM_LIBRARIES}
    )


endif()

if(CLANG_ENABLED)
    find_package(Clang REQUIRED CONFIG)
    include_directories(${CLANG_INCLUDE_DIRS})
    include_directories(${LLVM_BUILD_MAIN_SRC_DIR}/tools/clang/include)
    include_directories(${LLVM_BUILD_BINARY_DIR}/tools/clang/include)
    add_compile_definitions(CLANG_ENABLED)
    set(KM2_CLANG_LIBRARIES ${CLANG_LIBRARY_LIST})
endif()

include_directories(${CMAKE_CURRENT_LIST_DIR})
#target_include_directories(km2 PRIVATE ${CMAKE_CURRENT_LIST_DIR})
#target_include_directories(km2ide PRIVATE ${CMAKE_CURRENT_LIST_DIR})

include(${CMAKE_CURRENT_LIST_DIR}/wall_e/wall_e.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/sproc/sproc.cmake)

#clang


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


target_link_libraries(km2
    ${KM2_CLANG_LIBRARIES}
    wall_e
)

target_link_libraries(km2_interpreter_backend
    wall_e
    km2
)

target_link_libraries(km2c
    wall_e
    km2
    km2_interpreter_backend
)

target_link_libraries(km2ide
    PRIVATE
    sproc
    wall_e
    km2
    km2_interpreter_backend
    km2ide_resources
    Qt::Core
    Qt::Qml
    Qt::Quick
    Qt::Concurrent
)

if(NOT MSVC)
    target_link_libraries(km2ide PRIVATE pthread)
endif()

include(tests/km2_tests.cmake)

install(TARGETS km2 DESTINATION lib)
install(TARGETS km2c DESTINATION bin)

install(DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/installfiles/Modules"
        DESTINATION lib/cmake
)



install(DIRECTORY "${CMAKE_SOURCE_DIR}/src/km2"
        DESTINATION include
        FILES_MATCHING
        PATTERN "*.h"
)

if (WIN32)
    SET(CPACK_GENERATOR "NSIS")
elseif (UNIX)
    SET(CPACK_GENERATOR "DEB")
    SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "KK") #required
endif()

INCLUDE(CPack)
