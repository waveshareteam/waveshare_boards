# This component is selected explicitly by the consuming application.
idf_component_register(
    SRCS "src/display/device.cpp" "src/display/backlight_impl.cpp"
    INCLUDE_DIRS "include"
    PRIV_INCLUDE_DIRS "src"
)

target_compile_features(${COMPONENT_LIB} PUBLIC cxx_std_17)
target_compile_definitions(${COMPONENT_LIB} PRIVATE
    BROOKESIA_HAL_CUSTOM_DISPLAY_DEVICE_PLUGIN_SYMBOL=custom_display_device_plugin_symbol
)
target_link_libraries(${COMPONENT_LIB} PRIVATE "-u custom_display_device_plugin_symbol")
