project(smalloc_s)

set( smalloc_source smalloc.cpp headers.cpp )

add_library(smalloc STATIC ${smalloc_source} )
