include_directories(${CMAKE_SOURCE_DIR}/src)
macro(add_test_executable name)
  add_executable(${name} ${name}_test.cpp)
  target_link_libraries(${name} gtest_main bitcask_cpp)
  add_test(${name} ${name})
endmacro()

add_test_executable(record)
add_test_executable(bitcaskimpl)
