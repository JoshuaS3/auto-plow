idf_component_register(SRCS "main.c" "input.c"
                       INCLUDE_DIRS "."
                       REQUIRES cxx driver newlib freertos esp_hw_support heap log spi_flash soc hal esp_rom esp_common esp_system xtensa)
