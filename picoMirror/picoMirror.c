#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "ws2812.pio.h"

static inline void put_pixel(uint32_t pixel_grb) {
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}

static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b) {
    return
            ((uint32_t) (r) << 8) |
            ((uint32_t) (g) << 16) |
            (uint32_t) (b);
}

const int PIN_TX = 0;
const int PIN_PWR = 2;
const int NUM_PX = 42;

const uint32_t colour_warmWhite = 0xfdf4dc;
const uint32_t colour_coolWhite = 0xffffff;

void setAllPixels(uint len, uint32_t colour) {
    for(int i = 0; i < len; i++){
        put_pixel(colour);
    }
}

int main() {
    //set_sys_clock_48();
    stdio_init_all();

    gpio_init(PIN_PWR);
    gpio_set_dir(PIN_PWR, GPIO_IN);

    puts("WS2812 Smoke Test");

    // todo get free sm
    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);

    ws2812_program_init(pio, sm, offset, PIN_TX, 800000, false);

    int t = 0;
   
    bool warm = false;
    bool buttonPressed = gpio_get(PIN_PWR);

    while(1)
    {
        bool buttonState = gpio_get(PIN_PWR);

        if(buttonState != buttonPressed)
        {
            if(buttonState)
                {
                if(warm){
                    setAllPixels(NUM_PX, colour_warmWhite);
                    warm = false;
                }
                else{
                    setAllPixels(NUM_PX, colour_coolWhite);
                    warm = true;
                }
            }
            else
            {
                setAllPixels(NUM_PX, 0);
            }
            buttonPressed = buttonState;
        }

        sleep_ms(250);
    }
}