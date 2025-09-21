cmake_minimum_required(VERSION 3.22)
project(sample-ui)

find_package(Threads REQUIRED)
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)

# lvgl
file(GLOB_RECURSE LVGL_SOURCES ${PROJECT_SOURCE_DIR}/src/lvgl/src/*.c)
set(LVGL_INCLUDES ${PROJECT_SOURCE_DIR}/src/lvgl/)

# lv_drivers
file(GLOB LV_DRIVERS_SOURCES
  ${PROJECT_SOURCE_DIR}/src/lv_drivers/display/fbdev.c
  ${PROJECT_SOURCE_DIR}/src/lv_drivers/indev/evdev.c
)
set(LV_DRIVERS_INCLUDES ${PROJECT_SOURCE_DIR}/src/lv_drivers/)

# common
file(GLOB COMMON_SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)
set(COMMON_INCLUDES ${PROJECT_SOURCE_DIR}/src/)

# ui
file(GLOB UI_SOURCES ${PROJECT_SOURCE_DIR}/src/ui/*.cpp)
set(UI_INCLUDES ${PROJECT_SOURCE_DIR}/src/ui/)

add_executable(${PROJECT_NAME}
  ${LVGL_SOURCES}
  ${LV_DRIVERS_SOURCES}
  ${COMMON_SOURCES}
  ${UI_SOURCES}
)

target_include_directories(${PROJECT_NAME} PUBLIC
  ${COMMON_INCLUDES}
  ${LVGL_INCLUDES}
  ${LV_DRIVERS_INCLUDES}
  ${UI_INCLUDES}
)

target_compile_options(${PROJECT_NAME} PUBLIC
)

target_compile_features(${PROJECT_NAME} PUBLIC
  cxx_std_11
)

target_compile_definitions(${PROJECT_NAME} PUBLIC
  LOG_PRINT
  LOG_TAG="UI"
  LOG_PRINT_TIMESTAMP
  LV_LVGL_H_INCLUDE_SIMPLE
  LV_CONF_INCLUDE_SIMPLE
)

# Note:
# paho-mqtt3a - asynchronous (MQTTAsync)
# paho-mqtt3as - asynchronous with SSL (MQTTAsync)
# paho-mqtt3c - "classic" / synchronous (MQTTClient)
# paho-mqtt3cs - "classic" / synchronous with SSL (MQTTClient)
# paho-mqttpp3 - C++ (requires paho-mqtt3as)

target_link_libraries(${PROJECT_NAME}
  ${CMAKE_THREAD_LIBS_INIT}
)

set_target_properties(${PROJECT_NAME} PROPERTIES
  LINKER_LANGUAGE CXX
  CXX_STANDARD 11
)

# for yocto
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
