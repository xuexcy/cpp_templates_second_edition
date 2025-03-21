if(DEFINED ENV{build_output_bin_dir})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $ENV{build_output_bin_dir})
else()
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output/bin)
endif()
message("CMAKE_RUNTIME_OUTPUT_DIRECTORY: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

add_executable(chapter_01_main chapter_01/main.cc)
add_executable(chapter_01_max3ref chapter_01/max3ref.cc)

add_executable(chapter_02_main chapter_02/main.cc)
# add_subdirectory(utils)
# include_directories(${CMAKE_CURRENT_SOURCE_DIR})
#
# set(main_name address_of)
# add_executable(${main_name} ${main_name}.cc)
# target_link_libraries(${main_name} main_decorator)
#
# set(main_name attach_by_initialization)
# add_executable(${main_name} ${main_name}.cc)
# target_link_libraries(${main_name} main_decorator)
# add_subdirectory(${main_name}_deps)
# target_link_libraries(${main_name} server)
#
# set(main_name boost_mutant)
# add_executable(${main_name} ${main_name}.cc)
# target_link_libraries(${main_name} main_decorator)
# target_link_libraries(${main_name} boost_bimap)
#
# set(main_name checked_delete)
# file(GLOB main_deps ${main_name}_deps/*.h ${main_name}_deps/*.cpp)
# add_executable(${main_name} ${main_name}.cc ${main_deps})
# target_link_libraries(${main_name} main_decorator)
#
# set(main_name inline_guard_macro)
# add_executable(${main_name} ${main_name}.cc)
# target_link_libraries(${main_name} main_decorator)
# # add_definitions(-DENABLE_INLINE)
# # add_definitions(-DENABLE_PRINT_HI)
# set(deps ${main_name}_deps)
# add_subdirectory(${deps})
# target_link_libraries(${main_name} ${deps}_test)
#
# set(main_name pointer_to_implementation)
# add_executable(${main_name} ${main_name}.cc)
# target_link_libraries(${main_name} main_decorator)
# add_subdirectory(${main_name}_deps)
# target_link_libraries(${main_name} book book_with_pimpl book_template_with_pimpl)
#
