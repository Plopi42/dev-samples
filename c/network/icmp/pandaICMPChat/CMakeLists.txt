PROJECT(pandaICMPChat C)

IF("$ENV{DEBUG}" EQUAL 1)
  MESSAGE("== Setting build to DEBUG")
  SET(CMAKE_BUILD_TYPE DEBUG)
ELSE("$ENV{DEBUG}" EQUAL 1)
  MESSAGE("== Setting build to RELEASE")
  SET(CMAKE_BUILD_TYPE RELEASE)
ENDIF("$ENV{DEBUG}" EQUAL 1)

SET(CMAKE_C_FLAGS         "-Wall -Wextra -pedantic -Werror")
SET(CMAKE_C_FLAGS_DEBUG   "${CMAKE_C_FLAGS} -O0 -g -ggdb")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS} -O3")

ADD_EXECUTABLE(../server.bin
server/server.c
)

ADD_EXECUTABLE(../client.bin
client/client.c
)
