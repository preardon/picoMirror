add_executable(picoMirror)

pico_generate_pio_header(picoMirror ${CMAKE_CURRENT_LIST_DIR}/ws2812.pio OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/generated)

target_sources(picoMirror PRIVATE picoMirror.c)

target_link_libraries(picoMirror PRIVATE pico_stdlib hardware_pio)
pico_add_extra_outputs(picoMirror)