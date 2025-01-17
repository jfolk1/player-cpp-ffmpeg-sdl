FIND_PACKAGE(PkgConfig)

PKG_CHECK_MODULES(PC_X264 x264)
FIND_PATH(X264_INCLUDE_DIR x264.h
	HINTS ${PC_X264_INCLUDEDIR} ${PC_X264_INCLUDE_DIRS}
	NO_DEFAULT_PATH
	)

IF(${X264_INCLUDE_DIR} STREQUAL X264_INCLUDE_DIR-NOTFOUND)
	FIND_PATH(X264_INCLUDE_DIR x264.h) #try to use default dirs...
ENDIF()

IF(${X264_INCLUDE_DIR} STREQUAL X264_INCLUDE_DIR-NOTFOUND)
	FILE(GLOB_RECURSE X264_INCLUDE_DIR "/x264*/x264.h") #last resort! , besides the directory name may be different... it's very bad anyway
	IF(NOT ${X264_INCLUDE_DIR} STREQUAL X264_INCLUDE_DIR-NOTFOUND)
		GET_FILENAME_COMPONENT(X264_INCLUDE_DIR ${X264_INCLUDE_DIR} PATH)
	ENDIF()
ENDIF()

IF(${X264_INCLUDE_DIR} STREQUAL X264_INCLUDE_DIR-NOTFOUND)
	PKG_CHECK_MODULES(PC_X264 libx264)
	FIND_PATH(X264_INCLUDE_DIR x264.h
		HINTS ${PC_X264_INCLUDEDIR} ${PC_X264_INCLUDE_DIRS}
		NO_DEFAULT_PATH)
ENDIF()

IF(${X264_INCLUDE_DIR} STREQUAL X264_INCLUDE_DIR-NOTFOUND)
	MESSAGE(STATUS "Can't find x264!")
ELSE()
	MESSAGE(STATUS "Found x264: ${X264_INCLUDE_DIR}")

	#GET_FILENAME_COMPONENT(X264_PARENT ${X264_INCLUDE_DIR} PATH)
	SET(X264_PARENT ${X264_INCLUDE_DIR})
	MESSAGE(STATUS "Using x264 dir parent as hint: ${X264_PARENT}")

	IF(NOT WIN32)
		FIND_LIBRARY(X264_LIBRARIES x264
			HINTS ${PC_X264_LIBDIR} ${PC_X264_LIBRARY_DIR} ${X264_PARENT}
			NO_DEFAULT_PATH)
		IF(${X264_LIBRARIES} STREQUAL X264_LIBRARIES-NOTFOUND)
			FIND_LIBRARY(X264_LIBRARIES libx264
				HINTS ${PC_X264_LIBDIR} ${PC_X264_LIBRARY_DIR} ${X264_PARENT}
				NO_DEFAULT_PATH)
		ENDIF()
	ELSE()
		FIND_FILE(X264_LIBRARIES NAMES libx264.lib HINTS ${X264_PARENT})
#		SET(X264_LIBRARIES "${X264_LIBRARIES}/libx264.lib")
	ENDIF()

	IF(NOT ${X264_INCLUDE_DIR} STREQUAL X264_INCLUDE_DIR-NOTFOUND
		AND NOT ${X264_LIBRARIES} STREQUAL X264_LIBRARIES-NOTFOUND)

		SET(X264_FOUND 1)
		SET(X264_INCLUDE_DIRS ${X264_INCLUDE_DIR})
		SET(X264_LIBS ${X264_LIBRARIES})

		MESSAGE(STATUS "x264 include: ${X264_INCLUDE_DIRS}")
		MESSAGE(STATUS "x264 lib: ${X264_LIBS}")
	ELSE()
		MESSAGE(STATUS "Can't find x264")
	ENDIF()
ENDIF()

