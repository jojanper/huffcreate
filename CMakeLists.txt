cmake_minimum_required(VERSION 3.5)
set(APP huffcreate)
project(huffman)

if(CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
  set(CMAKE_CXX_FLAGS_RELEASE "-O3")
  set(CMAKE_C_FLAGS_RELEASE "-O3")
endif(CMAKE_COMPILER_IS_GNUCXX)

add_library(huf_library OBJECT
  bintree.c
  list.c
  huffman.c
)
add_library(huffman::library ALIAS huf_library)

set(SRC
  main.c
  $<TARGET_OBJECTS:huffman::library>
)

add_executable(${APP} ${SRC})
target_link_libraries(${APP} PRIVATE m)

message("Build type: ${CMAKE_BUILD_TYPE}")
message("C_FLAGS   : ${CMAKE_CXX_FLAGS}")
message("CXX_FLAGS : ${CMAKE_CXX_FLAGS}")
