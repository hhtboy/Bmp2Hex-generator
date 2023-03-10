#ifndef LCD_POWER_CFG_H
#define LCD_POWER_CFG_H

#include <stdint.h>
#include "app_error.h"

/* LCD Pin Configure */
#define A026_RESX 31
#define A026_TE 30
#define A026_CS 29
#define A026_DCX 28
#define A026_WRX 4
#define A026_RDX 3
#define A026_D0 33
#define A026_D1 34
#define A026_D2 35
#define A026_D3 36
#define A026_D4 37
#define A026_D5 38
#define A026_D6 39
#define A026_D7 40

#define LCD_PIXELS 38400 // 160 x 240 = 38400
#define LCD_WIDTH 160
#define LCD_HEIGTH 240

typedef enum palette_4bit {
  BLACK, BLUE, GREEN, TURQUOIS,
  RED, PURPLE, SADDLE_BROWN, LIGHT_GREY, 
  GREY, LIGHT_BLUE, LIME, SKYBLUE,
  TOMATO, FUCHSIA, YELLOW, WHITE 
} PALETTE_4BIT;

typedef enum palette_2bit {
  BLACK_2BIT, BLUE_2BIT, RED_2BIT, WHITE_2BIT
} PALETTE_2BIT;

typedef enum color_bit_size {
  COLOR_4 = 4,
  COLOR_2 = 2
} COLOR_BIT_SIZE;
 
typedef enum voltage {LOW, HIGH} VOLTAGE;

typedef struct image_data
{
    int width;
    int height;
    COLOR_BIT_SIZE type;
    uint8_t data[LCD_PIXELS / 2];
} IMAGE_DATA; 

typedef struct command_data
{
    VOLTAGE dcx_voltage;
    uint8_t data;
} COMMAND_DATA;

/* drawing paper
first draw image on lcd_layer
second print image on lcd with this lcd_layer
*/
static uint16_t lcd_layer[LCD_PIXELS];

/* Color data array */
static const uint16_t palette_4bit_data[16] = {
    0x0000, 0x001F, 0x0400, 0x471A,
  0xF800, 0x8010, 0x8A22, 0xD69A, 
  0x8410, 0xAEDC, 0x07E0, 0x867F,
  0xFB08, 0xF81F, 0xFFE0, 0xFFFF
};

/* Configure LCD pin */
void lcd_pin_init(void);

/* Conduct command list to power on lcd */
void lcd_power_on(void);

__STATIC_INLINE void write_data(const uint8_t data);      // write data to lcd

/* After controling DCX voltage, send data to lcd */
__STATIC_INLINE void write_command_or_data(VOLTAGE dcx_voltage, const uint8_t data); 

void print_layer(); // print image on lcd according to lcd_layer

void draw_layer(const IMAGE_DATA* image, const uint32_t x, const uint32_t y);

__STATIC_INLINE uint16_t get_palette_2bit_data(const uint8_t* palette_data, const uint32_t index);

__STATIC_INLINE int get_palette_4bit_index(const uint8_t* palette_data, const uint32_t index);

#endif // LCD_POWER_CFG_H

