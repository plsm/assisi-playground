set(enki_INCLUDE_DIR /usr/local/include /usr/local/include/enki . )
set(enki_LIBRARIES /usr/local/lib/libenki.a)

set(test_electricField_SOURCES
./test-electricField.cpp
../interactions/AbstractElectricFieldActuator.cpp
../interactions/PointElectricFieldActuator.cpp
../Component.cpp
)


set(GLUT_Xmu_LIBRARY /usr/lib/i386-linux-gnu/libglut.so  )

find_package(GLUT REQUIRED)
include_directories(${GLUT_INCLUDE_DIRS})
link_directories(${GLUT_LIBRARY_DIRS})
#add_definitions(${GLUT_DEFINITIONS})
if(NOT GLUT_FOUND)
    message(ERROR " GLUT not found!")
endif(NOT GLUT_FOUND)

find_package(OpenGL REQUIRED)
include_directories(${OpenGL_INCLUDE_DIRS})
link_directories(${OpenGL_LIBRARY_DIRS})
#add_definitions(${OpenGL_DEFINITIONS})
if(NOT OPENGL_FOUND)
    message(ERROR " OPENGL not found!")
endif(NOT OPENGL_FOUND)


find_package(Qt4 REQUIRED)

include_directories( ${enki_INCLUDE_DIR})

#include_directories (${OPENGL_INCLUDE_DIR})    


add_executable(test-electricField ${test_electricField_SOURCES})

target_link_libraries(test-electricField
  ${enki_LIBRARIES}
  ${OPENGL_LIBRARIES}
  ${QT_LIBRARIES} 
  ${GLUT_LIBRARY}
  )


