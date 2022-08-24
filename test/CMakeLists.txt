cmake_minimum_required(VERSION 3.6)
project(alpaca)

if(MSVC)
  # Force to always compile with W4
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif()
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  # Update if necessary
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic")
endif()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# Disable deprecation for windows
if (WIN32)
	add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif()

# ALPACA executable
file(GLOB ALPACA_TEST_SOURCES
    main.cpp
	test_serialize_uint.cpp
	test_serialize_int.cpp
	test_serialize_string.cpp
	test_serialize_float.cpp
	test_serialize_bool.cpp
	test_serialize_array.cpp
	test_serialize_vector.cpp
	test_serialize_tuple.cpp
	test_serialize_pair.cpp
	test_serialize_enum_class.cpp
	test_serialize_nested_struct.cpp
	test_serialize_map.cpp
	test_serialize_set.cpp
	test_serialize_optional.cpp
	test_deserialize_uint.cpp
	test_deserialize_int.cpp
	test_deserialize_string.cpp
	test_deserialize_float.cpp
	test_deserialize_bool.cpp
	test_deserialize_array.cpp
	test_deserialize_vector.cpp
	test_deserialize_tuple.cpp
	test_deserialize_pair.cpp
	test_deserialize_map.cpp
	test_deserialize_set.cpp
	test_deserialize_enum_class.cpp
	test_deserialize_optional.cpp
)
set_source_files_properties(main.cpp
    PROPERTIES
    COMPILE_DEFINITIONS DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN)
ADD_EXECUTABLE(ALPACA ${ALPACA_TEST_SOURCES})
INCLUDE_DIRECTORIES("../include" ".")
set_target_properties(ALPACA PROPERTIES OUTPUT_NAME tests)
set_property(TARGET ALPACA PROPERTY CXX_STANDARD 17)

# Set ${PROJECT_NAME} as the startup project
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ALPACA)