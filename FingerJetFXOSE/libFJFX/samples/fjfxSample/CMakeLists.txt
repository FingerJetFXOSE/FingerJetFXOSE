project( fjfxSample ) 

# add include directories
include_directories("${FRFXLL_PATH}/include")
include_directories("${FJFX_PATH}/include")

add_executable( "${PROJECT_NAME}_static"
  fjfxSample.c
)

if( USE_SANITIZER )
  target_link_libraries( "${PROJECT_NAME}_static" "FJFX_static" "FRFXLL_static" "asan" ) 
else()
  target_link_libraries( "${PROJECT_NAME}_static" "FJFX_static" "FRFXLL_static" ) 
endif()

add_executable( ${PROJECT_NAME}
  fjfxSample.c
)

if( USE_SANITIZER )
  target_link_libraries( "${PROJECT_NAME}" "FJFX" "asan" ) 
else()
  target_link_libraries( "${PROJECT_NAME}" "FJFX" ) 
endif()
