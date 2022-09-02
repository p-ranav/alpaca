add_executable(fundamental_types fundamental_types.cpp)
target_link_libraries(fundamental_types PRIVATE alpaca::alpaca)

add_executable(arrays_vectors_strings arrays_vectors_strings.cpp)
target_link_libraries(arrays_vectors_strings PRIVATE alpaca::alpaca)

add_executable(maps_and_sets maps_and_sets.cpp)
target_link_libraries(maps_and_sets PRIVATE alpaca::alpaca)