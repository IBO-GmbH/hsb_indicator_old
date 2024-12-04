idf_component_register(SRCS "button_adc.c" "button_gpio.c" "iot_button.c"
                        INCLUDE_DIRS include
                        REQUIRES driver esp_adc esp_timer)

target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-deprecated")
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-deprecated-declarations")
