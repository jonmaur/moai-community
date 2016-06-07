cmake_minimum_required ( VERSION 2.8.5 )
project ( moai-imgui )


set ( MOAI_IMGUI_INCLUDES 
  "${MOAI_ROOT}/src/"
  "${MOAI_ROOT}/3rdparty/imgui"
  "${CMAKE_BINARY_DIR}/libmoai/config"
)

include_directories ( ${MOAI_IMGUI_INCLUDES} )

set ( CMAKE_CURRENT_SOURCE_DIR "${MOAI_ROOT}/src/moai-imgui/" )
set ( IMGUI_SOURCE_DIR "${MOAI_ROOT}/3rdparty/imgui" )

# Use all cpp files in directory as source code
file ( GLOB MOAI_IMGUI_SRC "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.h" "${IMGUI_SOURCE_DIR}/*.cpp" "${IMGUI_SOURCE_DIR}/*.h" )

add_library ( moai-imgui STATIC ${MOAI_IMGUI_SRC} )

target_link_libraries ( moai-imgui moai-core moai-util )

target_include_directories (moai-imgui PUBLIC 
    "$<BUILD_INTERFACE:${MOAI_IMGUI_INCLUDES}>"
     $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>
    "$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include/imgui>"
)

set_target_properties(moai-imgui PROPERTIES FOLDER Modules)

install(TARGETS moai-imgui EXPORT libmoai ARCHIVE DESTINATION lib)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION include/moai-imgui FILES_MATCHING PATTERN "*.h")
install(DIRECTORY ${IMGUI_SOURCE_DIR} DESTINATION include/imgui FILES_MATCHING PATTERN "*.h")

