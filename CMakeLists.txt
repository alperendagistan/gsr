cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(my_adc_app)

# Source dosyaları
target_sources(app PRIVATE
    main.c
    app_drivers/adc.c
    app_tasks/app_tasks.c
    app_lib/shell.c
)

# Header dosyalarının yolu
target_include_directories(app PRIVATE
    inc
    app_drivers
    app_tasks
    app_lib
)