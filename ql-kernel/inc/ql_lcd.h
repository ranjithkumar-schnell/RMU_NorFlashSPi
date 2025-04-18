/*=================================================================

						EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

WHEN			  WHO		  WHAT, WHERE, WHY
------------	 -------	 -------------------------------------------------------------------------------

=================================================================*/


#ifndef QL_LCD_H
#define QL_LCD_H


#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
 * Macro Definition
 ===========================================================================*/
#define QL_LCD_ERRCODE_BASE (QL_COMPONENT_BSP_LCD<<16)

/*===========================================================================
 * Enum
 ===========================================================================*/
/**
 * LCD errcode
 */
typedef enum
{
    QL_LCD_SUCCESS = QL_SUCCESS,

    QL_LCD_EXECUTE_ERR                 = 1|QL_LCD_ERRCODE_BASE,
    QL_LCD_INVALID_PARAM_ERR,
    QL_LCD_INIT_ERR,
    QL_LCD_BUFFER_CREATE_ERR,
    QL_LCD_BLOCK_SET_ERR,
    QL_LCD_OPERATION_ERR
}ql_errcode_lcd;

/**
 * LCD bus mode
 */
typedef enum
{
    QL_LCD_SPI_LINE_4        = 0,
    QL_LCD_SPI_LINE_3        = 1,
    QL_LCD_SPI_LINE_3_2_LANE = 3,

    QL_LCD_SPI_LINE_MAX
}ql_lcd_busmode_s;

/*===========================================================================
 * Struct
 ===========================================================================*/

/*===========================================================================
 * Functions declaration
 ===========================================================================*/
/**
 * @brief Extend init the LCD moudle
 * @param color : background color
 * @param backlight_sw : whether to open backlight in LCD initialize program
                         if this argument is false,
                         customers need set ql_lcd_set_brightness.
 * @return
 *      QL_LCD_INIT_ERR
 *      QL_LCD_BUFFER_CREATE_ERR
 *      QL_LCD_BLOCK_SET_ERR
 *      QL_LCD_SUCCESS
 */
ql_errcode_lcd ql_lcd_init_ext(uint16_t color, bool backlight_sw);

/**
 * @brief Init the LCD moudle
 * @return
 *      QL_LCD_INIT_ERR
 *      QL_LCD_BUFFER_CREATE_ERR
 *      QL_LCD_BLOCK_SET_ERR
 *      QL_LCD_SUCCESS
 */
ql_errcode_lcd ql_lcd_init(void);

/**
 * @brief Init the LCD(st7567a) moudle
 * @return
 *      QL_LCD_INIT_ERR
 *      QL_LCD_BUFFER_CREATE_ERR
 *      QL_LCD_BLOCK_SET_ERR
 *      QL_LCD_SUCCESS
 */
ql_errcode_lcd ql_lcd_init_st7567a(void);

/**
 * @brief Free LCD memory space
 * @return
 *      QL_LCD_SUCCESS
 */
ql_errcode_lcd ql_lcd_free(void);

/**
 * @brief Clear screen background to a kind of color
 * @param color : background color
 * @return
 *      QL_LCD_BLOCK_SET_ERR
 *      QL_LCD_SUCCESS
 */
ql_errcode_lcd ql_lcd_clear_screen(uint16_t color);

/*** this API is apply to LCD's drive what except ST7567A ***/
/*** if drive is ST7567A, directly use write cmd&data API ***/
/*** location param is page and column                    ***/
/**
 * @brief LCD part area set
 * @param buffer  : write-in data buffer
 * @param start_x : horizontal coordinate x start location
 * @param start_y : vertical coordinate y start location
 * @param end_x   : horizontal coordinate x end location
 * @param end_x   : vertical coordinate y end location
 * @return
 *      QL_LCD_INVALID_PARAM_ERR
 *      QL_LCD_BLOCK_SET_ERR
 *      QL_LCD_SUCCESS
 */
ql_errcode_lcd ql_lcd_write(uint16_t *buffer, uint16_t start_x, uint16_t start_y, uint16_t end_x, uint16_t end_y);

/**
 * @brief LCD display turn on
 * @return
 *      QL_LCD_OPERATION_ERR
 *      QL_LCD_SUCCESS
 */
ql_errcode_lcd ql_lcd_display_on(void);

/**
 * @brief LCD display turn off
 * @return
 *      QL_LCD_OPERATION_ERR
 *      QL_LCD_SUCCESS
 */
ql_errcode_lcd ql_lcd_display_off(void);

/**
 * @brief LCD SPI CMD send interface
 * @param cmd : LCD SPI CMD param
 * @return
 *      QL_LCD_INVALID_PARAM_ERR
 *      QL_LCD_SUCCESS
 */
ql_errcode_lcd ql_spi_lcd_write_cmd(uint8_t cmd);

/**
 * @brief LCD SPI DATA send interface
 * @param data : LCD SPI DATA param
 * @return
 *      QL_LCD_INVALID_PARAM_ERR
 *      QL_LCD_SUCCESS
 */
ql_errcode_lcd ql_spi_lcd_write_data(uint8_t data);

/**
 * @brief LCD backlight brightness adjust function
 * @param level : LCD backlight brightness level
 * @return
 *      QL_LCD_INVALID_PARAM_ERR
 *      QL_LCD_SUCCESS
 */
ql_errcode_lcd ql_lcd_set_brightness(uint32_t level);

/**
 * @brief LCD backlight brightness one channel of each level set function
 * @param level : LCD backlight brightness level
 *                level 0: means close backlight, so can't set
 *                set backlight brightness, from level 1 to level 5
 * @param level_brightness : each level steps set, range is 0~63, one step is 0.84mA
 *                           one channel current: mix is 1.68mA, max is 54.6mA
 *                           there is 4 channels
 *                           total current is (1.68 + 0.84 * level_brightness) * 4 (mA)
 * @return
 *      QL_LCD_INVALID_PARAM_ERR
 *      QL_LCD_SUCCESS
 */
ql_errcode_lcd ql_lcd_set_level_brightness(uint32_t level, uint8_t level_brightness);

/**
 * @brief LCD enter and exit sleep function
 *        if the lcd has been initialized, the function must be called before enter sleep
 * @param is_sleep : true --lcd enter sleep 
 *                   false--lcd exit sleep 
 * @return
 *      QL_LCD_OPERATION_ERR
 *      QL_LCD_SUCCESS
 */
ql_errcode_lcd ql_lcd_enter_sleep(bool is_sleep);

/**
 * @brief LCD display area set
 * @param start_x_offset : start pixel's x_offset (distance from (0,0) in x)
 * @param start_y_offset : start pixel's y_offset (distance from (0,0) in y)
 * @param width_offset   : width's x_offset (the diff between the supported width in driver and the width user want)
 * @param height_offset  : height's y_offset (the diff between the supported height in driver and the height user want)
 * @return
 *      QL_LCD_SUCCESS
 */
ql_errcode_lcd ql_lcd_set_display_offset(uint16_t start_x_offset, uint16_t start_y_offset, uint16_t width_offset, uint16_t height_offset);


ql_errcode_lcd ql_lcd_set_busmode(ql_lcd_busmode_s bus_mode);

#ifdef __cplusplus
} /*"C" */
#endif

#endif /* QL_LCD_H */


