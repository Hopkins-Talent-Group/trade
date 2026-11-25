cmake_minimum_required(VERSION 3.20)

project(TradingAnalysisEngine VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(MANIFEST_BUILD_TESTS "Build the GoogleTest test suite" ON)
option(MANIFEST_ENABLE_ASAN_UBSAN "Enable AddressSanitizer and UndefinedBehaviorSanitizer in Debug builds" ON)
option(MANIFEST_ENABLE_TSAN "Enable ThreadSanitizer (separate from ASan/UBSan)" OFF)

if(MANIFEST_ENABLE_TSAN AND MANIFEST_ENABLE_ASAN_UBSAN)
    message(FATAL_ERROR "ThreadSanitizer must use a separate build; disable MANIFEST_ENABLE_ASAN_UBSAN.")
endif()

if(MANIFEST_ENABLE_TSAN)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        add_compile_options("$<$<CONFIG:Debug>:-fsanitize=thread>" "$<$<CONFIG:Debug>:-fno-omit-frame-pointer>")
        add_link_options("$<$<CONFIG:Debug>:-fsanitize=thread>")
    else()
        message(WARNING "ThreadSanitizer is not configured for this compiler.")
    endif()
elseif(MANIFEST_ENABLE_ASAN_UBSAN)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        add_compile_options("$<$<CONFIG:Debug>:-fsanitize=address,undefined>" "$<$<CONFIG:Debug>:-fno-omit-frame-pointer>")
        add_link_options("$<$<CONFIG:Debug>:-fsanitize=address,undefined>")
    else()
        message(STATUS "ASan/UBSan flags are not configured for this compiler.")
    endif()
endif()

find_package(OpenSSL QUIET)

include(cmake/Dependencies.cmake)

add_library(manifest_foundation
    src/Scanner.cpp
    src/Hasher.cpp
)

target_include_directories(manifest_foundation
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(OPENSSL_FOUND)
    target_link_libraries(manifest_foundation
        PUBLIC
            OpenSSL::Crypto
    )
endif()

if(NOT OPENSSL_FOUND)
    target_compile_definitions(manifest_foundation PRIVATE TRADING_NO_OPENSSL)
    message(STATUS "OpenSSL not found; manifest_foundation will not have cryptographic features.")
endif()

target_compile_features(manifest_foundation PUBLIC cxx_std_20)

if(UNIX AND NOT APPLE)
    target_compile_definitions(manifest_foundation PRIVATE _GNU_SOURCE)
elseif(APPLE)
    target_compile_definitions(manifest_foundation PRIVATE _DARWIN_C_SOURCE)
endif()

if(MSVC)
    target_compile_options(manifest_foundation PRIVATE /W4 /permissive-)
else()
    target_compile_options(manifest_foundation PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()

if(MANIFEST_BUILD_TESTS)
    include(CTest)
    enable_testing()

    add_executable(manifest_foundation_tests
        tests/ScannerTests.cpp
        tests/HasherTests.cpp
    )

    target_link_libraries(manifest_foundation_tests
        PRIVATE
            manifest_foundation
            GTest::gtest_main
    )

    if(MSVC)
        target_compile_options(manifest_foundation_tests PRIVATE /W4 /permissive-)
    else()
        target_compile_options(manifest_foundation_tests PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
    endif()

    include(GoogleTest)
    gtest_discover_tests(manifest_foundation_tests)

    add_executable(trading_framework_tests
        tests/TradingFrameworkTests.cpp
    )

    target_link_libraries(trading_framework_tests
        PRIVATE
            trading_framework
            GTest::gtest_main
    )

    if(MSVC)
        target_compile_options(trading_framework_tests PRIVATE /W4 /permissive-)
    else()
        target_compile_options(trading_framework_tests PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
    endif()

    gtest_discover_tests(trading_framework_tests)
endif()

add_library(trading_framework
    src/trading/Math.cpp
    src/trading/RegimeFilter.cpp
    src/trading/EntryValidator.cpp
    src/trading/DecisionMemo.cpp
    src/trading/OrderType.cpp
    src/trading/Evidence.cpp
    src/trading/config/AssetSpec.cpp
    src/trading/config/SessionFilter.cpp
    src/trading/config/Config.cpp
    src/trading/ai_filter/Regime.cpp
    src/trading/risk/Manager.cpp
    src/trading/strategy/Engine.cpp
    src/trading/execution/IBKR.cpp
)

target_include_directories(trading_framework
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/manifest
)

target_link_libraries(trading_framework
    PRIVATE
        manifest_foundation
)

# C6 — Orchestration executable: --asset MNQ|MGC + health check
add_executable(mnq-robot src/main.cpp)

target_link_libraries(mnq-robot
    PRIVATE
        trading_framework
)

target_include_directories(mnq-robot
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/manifest
)

target_compile_features(mnq-robot PUBLIC cxx_std_20)

target_include_directories(trading_framework
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/manifest
)

target_link_libraries(trading_framework
    PRIVATE
        manifest_foundation
)
