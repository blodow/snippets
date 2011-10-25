set (CMAKE_BUILD_TYPE Debug)
add_executable (sync_call_across_threads sync_call_across_threads.cpp)
target_link_libraries (sync_call_across_threads pthread boost_thread)

