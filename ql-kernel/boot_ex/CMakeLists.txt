# Copyright (C) 2020 QUECTEL Technologies Limited and/or its affiliates("QUECTEL").
# All rights reserved.
#

# Copyright (C) 2020 QUECTEL Technologies Limited and/or its affiliates("QUECTEL").
# All rights reserved.
#
set(target boot_ex_ql_pin)
add_app_libraries($<TARGET_FILE:${target}>)
add_library(${target} STATIC)

set_target_properties(${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${out_quec_lib_dir})
target_compile_definitions(${target} PRIVATE OSI_LOG_TAG=LOG_TAG_QUEC)
target_include_directories(${target} PUBLIC inc)
target_include_directories(${target} PUBLIC petitfatfs)
target_link_libraries(${target} PRIVATE boot_encrypt calclib driver)
target_include_targets(${target} PRIVATE hal fs sffs bdev driver fsmount nvm cpio_parser bootloader fupdate)
target_sources(${target} PRIVATE quec_boot_pin_map.c)

set(target boot_ex_ql)
include(core_ex_ql.cmake)

if(CONFIG_QUEC_PROJECT_FEATURE_BOOT_SDMMC)
set(target boot_ex_sdmmc)
include(core_ex_sdmmc.cmake)
endif()

if(CONFIG_QUEC_PROJECT_FEATURE_BOOT_FAT)
set(target boot_ex_fat)
include(core_ex_fat.cmake)
endif()

if(CONFIG_QUEC_PROJECT_FEATURE_SPI6_EXT_NOR)
set(target boot_ex_spi6nor)
include(core_ex_spi6nor.cmake)
endif()

if(CONFIG_QUEC_PROJECT_FEATURE_BOOT_SPI_FLASH)
set(target boot_ex_spiflash)
include(core_ex_spiflash.cmake)
endif()

if(CONFIG_QUEC_PROJECT_FEATURE_BOOT_LCD)
set(target boot_ex_lcd)
include(core_ex_lcd.cmake)
endif()

if (CONFIG_QUEC_PROJECT_FEATURE_BOOT_NAND_FLASH)
set(target boot_ex_nandflash)
include(core_ex_nandflash.cmake)
endif()

if(CONFIG_QUEC_PROJECT_FEATURE_BOOT_LED)
set(target boot_ex_led)
include(core_ex_led.cmake)
endif()

if (CONFIG_QUEC_PROJECT_FEATURE_BOOT_ADC)
set(target boot_ex_adc)
include(core_ex_adc.cmake)
endif()

if (CONFIG_QUEC_PROJECT_FEATURE_BOOT_SCATTER)
set(target boot_ex_scatter)
include(core_ex_scatter.cmake)
endif()

relative_glob(srcs include/*.h src/*.c *.c inc/*.h)
beautify_c_code(${target} ${srcs})