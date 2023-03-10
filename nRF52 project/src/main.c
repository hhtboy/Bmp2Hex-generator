#include "lcd_power_cfg.h"
#include <stdint.h>
#include "nrf_delay.h"
#include "boards.h"
#include "bitmap.h"

int main(void)
{
    /* Configure board. */
    bsp_board_init(BSP_INIT_LEDS);
    
    /* Configure LCD pin */
    lcd_pin_init();

    /* Power on */
    lcd_power_on();

    while (true)
    {
        /* Led blink */
        nrf_gpio_pin_toggle(13);
        draw_layer(&image_kau_4bit, 0, 0);
        print_layer();

        nrf_delay_ms(1000);

        draw_layer(&image_kau_2bit, 0, 0);
        print_layer();

        nrf_delay_ms(1000);
    }
}