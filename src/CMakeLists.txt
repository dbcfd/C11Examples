set (TARGET ExamplesLib)
add_definitions(-DEXAMPLES_LIB_BUILD)

set(HEADERS FunctionalProgramming.h Platform.h MultiThreading.h Task.h Worker.h Manager.h)
set(SOURCES FunctionalProgramming.cpp MultiThreading.cpp Task.cpp Worker.cpp Manager.cpp)

add_library (${TARGET} SHARED ${HEADERS} ${SOURCES}) 
target_link_libraries (${TARGET} ${DEPENDENCIES})

set_target_properties (${TARGET} PROPERTIES
    SOVERSION ${PROJECT_SO_VERSION})

install (TARGETS ${TARGET} RUNTIME DESTINATION bin LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)

SetVSTargetProperties(${TARGET})

set (TARGET Examples)

set(HEADERS)
set(SOURCES main.cpp)

if(UNIX)
	set(DEPENDENCIES rt)
endif()	

SET (DEPENDENCIES ${DEPENDENCIES} ExamplesLib)

add_executable (${TARGET} ${HEADERS} ${SOURCES}) 
target_link_libraries (${TARGET} ${DEPENDENCIES})

install (TARGETS ${TARGET} RUNTIME DESTINATION bin LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)

SetVSTargetProperties(${TARGET})