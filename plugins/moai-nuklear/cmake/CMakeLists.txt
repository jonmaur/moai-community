cmake_minimum_required ( VERSION 2.8.5 )
project ( moai-nuklear )


  set ( CMAKE_CURRENT_SOURCE_DIR "../src" )
 

  set ( INCLUDES 
    "${MOAI_ROOT}/src/"
    "${CMAKE_BINARY_DIR}/libmoai/config"
    )

  # Use all cpp files in directory as source code
  file ( GLOB SRC "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.h" )


  add_library ( moai-nuklear STATIC ${SRC} )

  target_include_directories (moai-nuklear PUBLIC 
        "$<BUILD_INTERFACE:${INCLUDES}>"
        "$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>"
       
  )

  target_link_libraries ( moai-sim moai-core moai-util zlcore )
  set_target_properties(moai-nuklear PROPERTIES FOLDER Modules)  

  install(TARGETS moai-nuklear EXPORT libmoai ARCHIVE DESTINATION lib)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION include/moai-nuklear
          FILES_MATCHING PATTERN "*.h")
  
