add_executable(fundamental_types fundamental_types.cpp)
target_link_libraries(fundamental_types PRIVATE alpaca::alpaca)

add_executable(arrays_vectors_strings arrays_vectors_strings.cpp)
target_link_libraries(arrays_vectors_strings PRIVATE alpaca::alpaca)

add_executable(maps_and_sets maps_and_sets.cpp)
target_link_libraries(maps_and_sets PRIVATE alpaca::alpaca)

add_executable(nested_structures nested_structures.cpp)
target_link_libraries(nested_structures PRIVATE alpaca::alpaca)

add_executable(optional_values optional_values.cpp)
target_link_libraries(optional_values PRIVATE alpaca::alpaca)

add_executable(variant variant.cpp)
target_link_libraries(variant PRIVATE alpaca::alpaca)

add_executable(unique_ptr unique_ptr.cpp)
target_link_libraries(unique_ptr PRIVATE alpaca::alpaca)

add_executable(demo demo.cpp)
target_link_libraries(demo PRIVATE alpaca::alpaca)

add_executable(write_file write_file.cpp)
target_link_libraries(write_file PRIVATE alpaca::alpaca)

add_executable(chrono_duration chrono_duration.cpp)
target_link_libraries(chrono_duration PRIVATE alpaca::alpaca)

add_executable(time_t time_t.cpp)
target_link_libraries(time_t PRIVATE alpaca::alpaca)
