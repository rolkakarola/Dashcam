set(srcs
    "main.c"
    "connect_wifi.c"
    "camera_init.c"
    "temp_sensor.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/owb/owb.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/owb/owb_rmt.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/ds18b20/ds18b20.c"
)

idf_component_register(SRCS ${srcs}
    INCLUDE_DIRS "."
                 "${CMAKE_CURRENT_SOURCE_DIR}/owb"
                 "${CMAKE_CURRENT_SOURCE_DIR}/ds18b20"
)