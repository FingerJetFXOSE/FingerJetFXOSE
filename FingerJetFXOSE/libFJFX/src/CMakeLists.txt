project( FJFX ) 

add_definitions( "-std=c++11" )

# add include directories
include_directories("${FRFXLL_PATH}/include")
include_directories("${FJFX_PATH}/include")

add_library( "${PROJECT_NAME}_static" STATIC
  FJFX.cpp
)

add_library( ${PROJECT_NAME} SHARED
  FJFX.cpp
)

if( USE_SANITIZER )
  target_link_libraries( ${PROJECT_NAME} "FRFXLL_static" "asan" ) 
else()
  target_link_libraries( ${PROJECT_NAME} "FRFXLL_static" ) 
endif()
