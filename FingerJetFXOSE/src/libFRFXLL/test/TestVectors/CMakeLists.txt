project( TestVectors ) 

remove_definitions( "-std=c++11" )

file( GLOB TEST_VECTORS_SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.c" ) 
add_library( "TestVectors" STATIC
  ${TEST_VECTORS_SOURCE_FILES}
)
