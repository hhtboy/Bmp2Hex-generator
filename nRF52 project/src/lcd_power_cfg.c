#include "lcd_power_cfg.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "bitmap.h"
#include "string.h"

// configure every GPIO pin
void lcd_pin_init()
{
    nrf_gpio_cfg_output(A026_RESX);
    nrf_gpio_cfg_output(A026_TE);
    nrf_gpio_cfg_output(A026_CS);
    nrf_gpio_cfg_output(A026_DCX);
    nrf_gpio_cfg_output(A026_WRX);
    nrf_gpio_cfg_output(A026_RDX);

    /* D0 ~ D7 */
    nrf_gpio_cfg_output(A026_D0);
    nrf_gpio_cfg_output(A026_D1);
    nrf_gpio_cfg_output(A026_D2);
    nrf_gpio_cfg_output(A026_D3);
    nrf_gpio_cfg_output(A026_D4);
    nrf_gpio_cfg_output(A026_D5);
    nrf_gpio_cfg_output(A026_D6);
    nrf_gpio_cfg_output(A026_D7);
}

/* Power On */
void lcd_power_on(void)
{
    const COMMAND_DATA power_on_commands[61] = {
        {LOW, 0x3A}, {HIGH, 0x05}, // COLMOD
        {LOW, 0x20},  // INVOFF
        {LOW, 0xC0}, {HIGH, 0x10}, // PWCTR1
        {LOW, 0xC1}, {HIGH, 0x07}, // PWCTR2
        {LOW, 0xC2}, {HIGH, 0x33}, // PWCTR3
        {LOW, 0xB1}, {HIGH, 0x67}, // FRMCTR1
        {LOW, 0x2A}, {HIGH, 0x00}, {HIGH, 0x28}, {HIGH, 0x00}, {HIGH, 0xC7}, // CASET
        {LOW, 0x2B}, {HIGH, 0x00}, {HIGH, 0x50}, {HIGH, 0x01}, {HIGH, 0x3F}, // RASET
        {LOW, 0x2C}, {HIGH, 0x00}, // RAMWR
        // Command list 2 (Gamma & VCOM Setting)
        {LOW, 0xC5}, {HIGH, 0x31}, {HIGH, 0x39}, // VMCTR1
        {LOW, 0x26}, {HIGH, 0x10}, //GAMSET
        // GAMCTRP
        {LOW, 0xE0}, {HIGH, 0x0E}, {HIGH, 0x0F}, {HIGH, 0x12}, {HIGH, 0x06}, {HIGH, 0x0D}, 
        {HIGH, 0x12}, {HIGH, 0x3C}, {HIGH, 0x7B}, {HIGH, 0x53}, {HIGH, 0x0D}, {HIGH, 0x1}, 
        {HIGH, 0x26}, {HIGH, 0x26}, {HIGH, 0x2B}, {HIGH, 0x3F}, 
        // GAMCTRN
        {LOW, 0xE1}, {HIGH, 0x04}, {HIGH, 0x15}, {HIGH, 0x1A}, {HIGH, 0x09}, {HIGH, 0x10},
        {HIGH, 0x12}, {HIGH, 0x33}, {HIGH, 0x68}, {HIGH, 0x4D}, {HIGH, 0x0D}, {HIGH, 0x22},
        {HIGH, 0x29}, {HIGH, 0x32}, {HIGH, 0x35}, {HIGH, 0x35},

        {LOW, 0x11} // SLPOUT
    };

    nrf_gpio_pin_set(A026_RESX);    // RESX = HIGH
    nrf_gpio_pin_set(A026_TE);      // TE = HIGH
    nrf_gpio_pin_set(A026_DCX);     // DCX = HIGH
    nrf_gpio_pin_set(A026_RDX);     // RDX = HIGH
    nrf_gpio_pin_set(A026_CS);      // CS = HIGH
    nrf_gpio_pin_set(A026_WRX);     // WRX = HIGH
    
    nrf_gpio_pin_set(A026_RESX);    // RESX = HIGH
    nrf_delay_ms(1);                // Delay 1ms
    nrf_gpio_pin_clear(A026_RESX);  // RESX = LOW
    nrf_delay_ms(1);                // Delay 1ms
    nrf_gpio_pin_set(A026_RESX);    // RESX = HIGH
    nrf_delay_ms(120);              // Delay 120ms

    for(uint32_t i = 0; i < 61; i++) 
    {
        write_command_or_data(power_on_commands[i].dcx_voltage, power_on_commands[i].data);
    }

    nrf_delay_ms(120);                // Delay 120ms

    write_command_or_data(LOW, 0x29); // DISPON
}


__STATIC_INLINE void write_data(const uint8_t data)
{
    NRF_P0->OUTCLR = 1UL << A026_CS;  // CS clear
    NRF_P0->OUTCLR = 1UL << A026_WRX; // WRX clear

    NRF_P1->OUT = (NRF_P1->OUT & 0xFFFFFE01) | (data << 1);

    NRF_P0->OUTSET = 1UL << A026_WRX; // WRX set
    NRF_P0->OUTSET = 1UL << A026_CS;  // CS set
}


__STATIC_INLINE void write_command_or_data(VOLTAGE dcx_voltage, const uint8_t data)
{
    /* dcx_pin set */
    if (dcx_voltage == HIGH)
    {
        NRF_P0->OUTSET = 1UL << A026_DCX;   // write data
    } 
    else
    {
        NRF_P0->OUTCLR = 1UL << A026_DCX;   // write command
    }
    
    NRF_P0->OUTCLR = 1UL << A026_CS;  // CS clear
    NRF_P0->OUTCLR = 1UL << A026_WRX; // WRX clear
    
    /* data signal set */
    NRF_P1->OUT = (NRF_P1->OUT & 0xFFFFFE01) | (data << 1);

    NRF_P0->OUTSET = 1UL << A026_WRX; // WRX set
    NRF_P0->OUTSET = 1UL << A026_CS;  // CS set
}

void print_color()
{
    
    NRF_P0->OUTCLR = 1UL << A026_DCX; // DCX clear
    NRF_P0->OUTCLR = 1UL << A026_CS;  // CS clear
    NRF_P0->OUTCLR = 1UL << A026_WRX; // WRX clear

    // Memory Write
    NRF_P1->OUTSET = 0x58;        // 0x58 = 0x2C << 1
    NRF_P1->OUTCLR = 0b110100110; 

    NRF_P0->OUTSET = 1UL << A026_WRX; // WRX set
    NRF_P0->OUTSET = 1UL << A026_DCX; // DCX set
    
    uint32_t data = NRF_P1->OUT & 0xFFFFFE01;
    for(uint32_t i =0 ; i < LCD_PIXELS; i++ ) 
    {
        NRF_P0->OUTCLR = 1UL << A026_WRX; // WRX clear
        NRF_P1->OUT = data | ((lcd_layer[i] >> 8) << 1);
        NRF_P0->OUTSET = 1UL << A026_WRX; // WRX set

        NRF_P0->OUTCLR = 1UL << A026_WRX; // WRX clear
        NRF_P1->OUT = data | ((lcd_layer[i] & 0xFF) << 1);
        NRF_P0->OUTSET = 1UL << A026_WRX; // WRX set
    }

    NRF_P0->OUTSET = 1UL << A026_CS;  // CS set
}

void print_layer()
{
    NRF_P0->OUTCLR = 1UL << A026_DCX; // DCX clear
    NRF_P0->OUTCLR = 1UL << A026_CS;  // CS clear
    NRF_P0->OUTCLR = 1UL << A026_WRX; // WRX clear

    NRF_P1->OUTSET = 0x58;            // 0x58 = 0x2C << 1
    NRF_P1->OUTCLR = 0b110100110;     // 0b110100110

    NRF_P0->OUTSET = 1UL << A026_WRX; // WRX set
    NRF_P0->OUTSET = 1UL << A026_DCX; // DCX set

    
    for(uint32_t i =0 ; i < LCD_PIXELS; i++ ) 
    {
        NRF_P0->OUTCLR = 1UL << A026_WRX; // WRX clear
        NRF_P1->OUT = ( NRF_P1->OUT & 0xFFFFFE01 ) | ((lcd_layer[i] >> 8) << 1);
        NRF_P0->OUTSET = 1UL << A026_WRX; // WRX set

        NRF_P0->OUTCLR = 1UL << A026_WRX; // WRX clear
        NRF_P1->OUT = ( NRF_P1->OUT & 0xFFFFFE01 ) | ((lcd_layer[i] & 0xFF) << 1);
        NRF_P0->OUTSET = 1UL << A026_WRX; // WRX set
    }

    NRF_P0->OUTSET = 1UL << A026_CS;  // CS set
}

__STATIC_INLINE uint16_t get_palette_2bit_data(const uint8_t* palette_data, const uint32_t index)
{
    const uint16_t palette_2bit_data[4] = {0x0000, 0x001F, 0xF800, 0xFFFF};
    
    switch(index % 4)
    {
        case 0:
            return palette_2bit_data[palette_data[index >> 2] >> 6];
        case 1:
            return palette_2bit_data[(palette_data[index >> 2] & 0b110000) >> 4];
        case 2:
            return palette_2bit_data[(palette_data[index >> 2] & 0b1100) >> 2];
        case 3:
            return palette_2bit_data[palette_data[index >> 2] & 0b11];
        default:
            return 0;
    }
}

__STATIC_INLINE int get_palette_4bit_index(const uint8_t* palette_data, const uint32_t index)
{
    if (index % 2 == 0)
    {
        return palette_data[index >> 1] >> 4;
    }
    else
    {
        return palette_data[index >> 1] & 0xF;
    }
}

void draw_layer(const IMAGE_DATA* image, const uint32_t x, const uint32_t y)
{
    uint32_t layer_index = (LCD_WIDTH * y) + x;
    uint32_t image_index = 0;

    uint32_t draw_height = image->height;
    uint32_t max_index = layer_index + (LCD_WIDTH * image->height) + image->width;
    uint32_t is_index_over = 0;

    /* position check */
    if (x >= LCD_WIDTH || y >= LCD_HEIGTH)
    {
        return;
    }
    else if (max_index > LCD_PIXELS)
    {
        draw_height -= ((max_index - LCD_PIXELS) / LCD_WIDTH) + 1;
        //is_index_over = 1;
    }

    /* draw layer */
    if (image->type == COLOR_4) 
    {
        for (uint32_t i = 0; i < draw_height; i++)
        {
            for (uint32_t j = 0; j < image->width; j++)
            {
                lcd_layer[layer_index++] = palette_4bit_data[get_palette_4bit_index(image->data, image_index++)];
            }
            layer_index += (LCD_WIDTH - image->width);
        }

        if (is_index_over)
        {
            for (uint32_t i = layer_index; i < LCD_PIXELS; i++)
            {
                lcd_layer[layer_index++] = palette_4bit_data[get_palette_4bit_index(image->data, image_index++)];
            }
        }
    }

    else if (image->type == COLOR_2) 
    {
        for (uint32_t i = 0; i < draw_height; i++) 
        {
            for (uint32_t j = 0; j < image->width; j++)
            {
                lcd_layer[layer_index++] = get_palette_2bit_data(image->data, image_index++);
            }
            layer_index += (LCD_WIDTH - image->width);
        }

        if (is_index_over)
        {
            for (uint32_t i = layer_index; i < LCD_PIXELS; i++)
            {
                lcd_layer[layer_index++] = get_palette_2bit_data(image->data, image_index++);
            }
        }
    }        
}
