cmake_minimum_required(VERSION 2.8.9)
set(APP huffcreate)
project(huffman)

if(CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
endif(CMAKE_COMPILER_IS_GNUCXX)

set(SRC
  bintree.c
  list.c
  huffman.c
  main.c
)

add_executable(${APP} ${SRC})
target_link_libraries(${APP} PRIVATE m)