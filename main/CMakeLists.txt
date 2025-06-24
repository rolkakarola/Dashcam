set(srcs "main.c"
         "connect_wifi.c"
         "web_server.c"
        #  "sd_card.c"
         "camera_init.c")

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ".")
