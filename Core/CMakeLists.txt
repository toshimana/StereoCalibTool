CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT( Core )

# OpenCV
FIND_PACKAGE( OpenCV REQUIRED )
IF ( OpenCV_FOUND )
  INCLUDE_DIRECTORIES( ${OpenCV_INCLUDE_DIRS} )
ENDIF()

# OpenMP
OPTION( WITH_OPENMP "build application with openmp." ON )
IF( WITH_OPENMP ) 
    find_package(OpenMP)
    if (OPENMP_FOUND)
        set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()
ENDIF()

# Boost
FIND_PATH( BOOST_ROOT boost/signals2/signal.hpp
  /usr/local/include )
SET( Boost_USE_STATIC_LIBS ON)
FIND_PACKAGE( Boost REQUIRED COMPONENTS system filesystem thread date_time chrono )
IF ( Boost_FOUND )
  INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )
ENDIF()

# Actor
FIND_PATH( ACTOR_INCLUDE_DIRS REQUIRED actor.hpp
  /usr/local/include )
INCLUDE_DIRECTORIES( ${ACTOR_INCLUDE_DIRS} )

# cvtool
FIND_PATH( CVTOOL_INCLUDE_DIRS REQUIRED cvtool.hpp
	/usr/local/include )
INCLUDE_DIRECTORIES( ${CVTOOL_INCLUDE_DIRS} )

# ADD_DEFINITIONS( -DSTEREOCAPTUREACTOR_EXPORTS )

# TARGET
FILE( GLOB SRCS "*.cpp" )
ADD_LIBRARY( StereoCalibToolCore STATIC ${SRCS} )
TARGET_LINK_LIBRARIES( StereoCalibToolCore ${OpenCV_LIBS} )
TARGET_LINK_LIBRARIES( StereoCalibToolCore ${Boost_LIBRARIES} )