project( FJFX_MINEX ) 

add_definitions( "-std=c++11" )

set( FJFX_MINEX_PATH ${ROOT_PATH}/libMINEX )

# add include directories
include_directories("${FRFXLL_PATH}/include")

add_library( ${PROJECT_NAME} SHARED
  minex.cpp
)

if( USE_SANITIZER )
  target_link_libraries( ${PROJECT_NAME} "FRFXLL_static" "asan" ) 
else()
  target_link_libraries( ${PROJECT_NAME} "FRFXLL_static" ) 
endif()
