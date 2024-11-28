
add_executable(wstl_test test.cpp)

target_link_libraries(wstl_test wstl)

enable_testing()
add_test(NAME wstl_test COMMAND wstl_test)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)