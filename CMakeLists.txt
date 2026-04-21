cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(my_adc_app)

# Source dosyaları
target_sources(app PRIVATE
    src/main.c
    src/adc.c
)

# Header dosyalarının yolu
target_include_directories(app PRIVATE
    inc
)