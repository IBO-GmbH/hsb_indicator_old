
idf_component_register(
    SRC_DIRS
        "icm42670"
        "touch_panel"
        "io_expander"
        "sensor/bmp3xx"
    INCLUDE_DIRS
        "icm42670/include"
        "touch_panel/include"
        "io_expander"
        "sensor/bmp3xx"
    REQUIRES
        bsp)

target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-uninitialized")
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-unused")
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-missing-field-initializers")
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-incompatible-pointer-types")
