cmake_minimum_required(VERSION 3.0)
project("PyEBI" VERSION 0.0.0
DESCRIPTION "A header only c++ library to easy the definition of python bindings for C++ functions"
LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

option(INSTALL_TESTS "Whether to install the test scripts or not" OFF)

add_library(${PROJECT_NAME} INTERFACE)

target_include_directories(
  ${PROJECT_NAME}
  INTERFACE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
$<INSTALL_INTERFACE:include>)

include(CMakePackageConfigHelpers)
write_basic_package_version_file("${PROJECT_NAME}ConfigVersion.cmake"
                                 VERSION ${PROJECT_VERSION}
                                 COMPATIBILITY SameMajorVersion)

install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}Targets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

configure_package_config_file(
 "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in"
 "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
 INSTALL_DESTINATION
 ${PROJECT_BINARY_DIR}/cmake/${PROJECT_NAME})

install(EXPORT ${PROJECT_NAME}Targets DESTINATION ${PROJECT_BINARY_DIR}/cmake/${PROJECT_NAME})

file(GLOB_RECURSE HEADER_FILES ${PROJECT_SOURCE_DIR}/include/pyebi/*.hpp)

install(FILES ${HEADER_FILES} DESTINATION ${PROJECT_BINARY_DIR}/include/pyebi)

#
# Installation of the test modules
#
if(INSTALL_TESTS)
    find_package(PythonLibs REQUIRED)
    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/test)
    include_directories(include ${PYTHON_INCLUDE_DIRS})
    file(GLOB TEST_SOURCES ${PROJECT_SOURCE_DIR}/test/src/*.cpp)
    set(CMAKE_CXX_FLAGS "-O3 -Wall -Wextra")
    set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -Wextra")
    foreach(testsourcefile ${TEST_SOURCES})
      get_filename_component(testname ${testsourcefile} NAME_WE)
      add_library(${testname} SHARED ${testsourcefile})
      set_target_properties(${testname} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/test/python/lib CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO PREFIX "")
      target_link_libraries(${testname} PUBLIC ${PYTHON_LIBRARIES})
    endforeach(testsourcefile ${TEST_SOURCES})
endif(INSTALL_TESTS)
