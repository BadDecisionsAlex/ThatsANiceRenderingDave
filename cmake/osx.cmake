IF (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	list(APPEND stdgl_libraries iconv)
ENDIF()