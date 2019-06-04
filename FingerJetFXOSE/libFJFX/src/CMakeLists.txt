project( FJFX ) 

add_definitions( "-std=c++11" )

# add include directories
include_directories("${FRFXLL_PATH}/include")
include_directories("${FJFX_PATH}/include")

add_library( "${PROJECT_NAME}_static" STATIC
  fjfx.cpp
)

add_library( ${PROJECT_NAME} SHARED
  fjfx.cpp
)
target_link_libraries( "${PROJECT_NAME}" "FRFXLL_static" ) 
