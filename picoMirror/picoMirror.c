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

const uint32_t colour_warmWhite_5 = 0xf4fdcc;
const uint32_t colour_warmWhite_4 = 0xdce4b8;
const uint32_t colour_warmWhite_3 = 0xc4cba4;
const uint32_t colour_warmWhite_2 = 0xacb290;
const uint32_t colour_warmWhite_1 = 0x84997c;
const uint32_t colour_coolWhite = 0xffffff;


const uint longPress_Threshold = 3; //250ms x 3

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

    //bool buttonPressed = gpio_get(PIN_PWR);
    bool lightOn = false;
    uint brightness = 8;
    uint buttonPressedLength = 0;

    uint32_t currentColour = colour_warmWhite_5;

    while(1)
    {
        bool buttonPressed = gpio_get(PIN_PWR);

        if(buttonPressed)
        {
            //Pressed, work how how long of a press
            buttonPressedLength += 1;
            printf("Button Pressed : %d \n", buttonPressedLength);
        }
        else if(buttonPressedLength > 0)
        {
            if(buttonPressedLength < longPress_Threshold )
            {
                //Short Press
                if(!lightOn)
                {
                    //Turn on Lights
                    setAllPixels(NUM_PX, currentColour);
                    lightOn = true;
                }
                else
                {
                    //Turn Light Off
                    setAllPixels(NUM_PX, 0);
                    lightOn = false;
                }
            }
            else
            {
                //LongPress
                brightness += 1;
                if(brightness > 4) brightness = 0;

                switch(brightness)
                {
                    case 0:
                        currentColour = colour_warmWhite_5;
                        break;
                    case 1:
                        currentColour = colour_warmWhite_4;
                        break;
                    case 2:
                        currentColour = colour_warmWhite_3;
                        break;
                    case 3:
                        currentColour = colour_warmWhite_2;
                        break;
                    case 4:
                        currentColour = colour_warmWhite_1;
                        break;
                    default:
                        currentColour = 0;
                }
                setAllPixels(NUM_PX, currentColour);
                lightOn = true;
            }
            
            // Button no longer Pressed, Reset
            buttonPressedLength = 0;
        }

        sleep_ms(250);
    }
}