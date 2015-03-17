CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

PROJECT( Test )

# For CMake
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# OpenCV
FIND_PACKAGE( OpenCV REQUIRED )
IF ( OpenCV_FOUND )
  INCLUDE_DIRECTORIES( ${OpenCV_INCLUDE_DIRS} )
ENDIF()

# Boost
FIND_PATH( BOOST_ROOT boost/signals2/signal.hpp
  /usr/local/include )
FIND_PACKAGE( Boost REQUIRED )
IF ( Boost_FOUND )
  INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )
ENDIF()

# GMock
FIND_PATH( GMOCK_ROOT gmock/gmock.hpp
			/usr/local/include )
set( GMOCK_SRCS ${GMOCK_ROOT}/gmock-gtest-all.cc ${GMOCK_ROOT}/gmock_main.cc )
INCLUDE_DIRECTORIES( ${GMOCK_ROOT} )

# Actor
FIND_PATH( ACTOR_INCLUDE_DIRS REQUIRED actor.hpp
  /usr/local/include )
INCLUDE_DIRECTORIES( ${ACTOR_INCLUDE_DIRS} )

# Test
INCLUDE_DIRECTORIES( ../Core )
FILE( GLOB TESTS "*.cpp" )

ADD_EXECUTABLE( StereoCalibToolTest ${TESTS} ${GMOCK_SRCS} )
TARGET_LINK_LIBRARIES( StereoCalibToolTest StereoCalibToolCore )
