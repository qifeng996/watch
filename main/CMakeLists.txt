idf_component_register(SRCS
        "watch.cpp"
        "HardWare.cpp"
        "welcome_ui.c"
        "Gui.cpp"
        "Wifi.cpp"
        "MsgController.cpp"
        #        "gui/charge.cpp"
        "gui/wifi_info.cpp"
        "gui/bat_info.cpp"
        "gui/alert.cpp"
        "audio.cpp"
        INCLUDE_DIRS "." "include" "gui"
        EMBED_TXTFILES audio/gs-16b-1c-44100hz.mp3)

lvgl_port_create_c_image("images/esp_logo.png" "images/" "ARGB8888" "NONE")
lvgl_port_create_c_image("images/esp_text.png" "images/" "ARGB8888" "NONE")
lvgl_port_create_c_image("images/code.png" "images/" "ARGB8888" "NONE")
lvgl_port_create_c_image("images/wifi.png" "images/" "ARGB8888" "NONE")
lvgl_port_create_c_image("images/diswifi.png" "images/" "ARGB8888" "NONE")
lvgl_port_create_c_image("images/danger.png" "images/" "ARGB8888" "NONE")

lvgl_port_add_images(${COMPONENT_LIB} "images/")

set_source_files_properties(
        PROPERTIES COMPILE_OPTIONS
        "-DLV_LVGL_H_INCLUDE_SIMPLE;-Wno-format;"
)
