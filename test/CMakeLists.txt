set(enki_INCLUDE_DIR /usr/local/include)
set(enki_LIBRARIES /usr/local/lib/libenki.a)

set(test_light_SOURCES
  ./test_light.cpp
  ../interactions/LightSensor.cpp
  ../interactions/LightSource.cpp
  ../interactions/LightSourceFromAbove.cpp
  ../interactions/VibrationSensor.cpp
  ../interactions//VibrationSource.cpp
  ../interactions/QuadraticVibrationSource.cpp
  )

set(test_vibe_SOURCES
  ./test_vibe.cpp
  ../interactions/LightSensor.cpp
  ../interactions/LightSource.cpp
  ../interactions/LightSourceFromAbove.cpp
  ../interactions/VibrationSensor.cpp
  ../interactions//VibrationSource.cpp
  ../interactions/QuadraticVibrationSource.cpp
  )

include_directories( ${enki_INCLUDE_DIR})

add_executable(test_light ${test_light_SOURCES})

add_executable(test_vibe ${test_vibe_SOURCES})

target_link_libraries(test_vibe ${enki_LIBRARIES})

target_link_libraries(test_vibe ${enki_LIBRARIES})


