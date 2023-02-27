cmake_minimum_required(VERSION 3.16)

project(future)

set(BUILD_TESTS ON CACHE BOOL "Build tests")

if(BUILD_TESTS)
    include(CTest)
    enable_testing()

    function(add_future_project_test NAME)
        add_executable(${NAME} ${NAME}.cpp)
        target_include_directories(${NAME} PRIVATE .)
        add_test(NAME ${NAME} COMMAND ${NAME})
    endfunction()

    add_future_project_test(test_async_future)
    add_future_project_test(test_execute)
    add_future_project_test(test_future)
    add_future_project_test(test_promise)
    add_future_project_test(test_then_execute)
endif()
