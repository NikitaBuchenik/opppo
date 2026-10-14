include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(test_variant3
    test_variant3.cpp
    ../variant3.cpp
)

target_include_directories(test_variant3 PRIVATE ${CMAKE_SOURCE_DIR})
target_compile_definitions(test_variant3 PRIVATE UNIT_TESTING)
target_link_libraries(test_variant3 gtest gtest_main)
set_target_properties(test_variant3 PROPERTIES LINK_FLAGS "/PROFILE")

add_test(NAME test_variant3 COMMAND test_variant3)