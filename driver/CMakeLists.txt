# Copyright (C) 2018 RDA Technologies Limited and/or its affiliates("RDA").
# All rights reserved.
#
# This software is supplied "AS IS" without any warranties.
# RDA assumes no responsibility or liability for the use of the software,
# conveys no license or title under any patent, copyright, or mask work
# right to the product. RDA reserves the right to make changes in the
# software without notification.  RDA also make no representation or
# warranty that such application will be suitable for the specified use
# without further testing or modification.

configure_file(include/drv_config.h.in ${out_inc_dir}/drv_config.h)

if(NOT ${CONFIG_KEYPAD_CSV_PATH} STREQUAL "")
    set(keypadcfg_h ${CMAKE_CURRENT_BINARY_DIR}/drv_keypad_def.h)
    set(csv_keypad ${SOURCE_TOP_DIR}/${CONFIG_KEYPAD_CSV_PATH})
    add_custom_command(OUTPUT ${keypadcfg_h}
        COMMAND python3 ${keypadgen_py} ${csv_keypad} ${keypadcfg_h}
        DEPENDS ${keypadgen_py} ${csv_keypad}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
endif()

set(target driver)
include(core.cmake)
