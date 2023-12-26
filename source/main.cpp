#include <stdio.h>

#include "pico/stdlib.h"
#include "pico/time.h"
#include "pico/cyw43_arch.h"

#include "sd_card.h"
#include "ff.h"

static inline uint64_t millis();
static inline uint64_t micros();

int main( void )
{
    uint64_t bootTime = millis();
    stdio_init_all();
    if (cyw43_arch_init()) {
        printf("Wi-Fi init failed");
        return -1;
    }
    while( millis() - bootTime < 1000ULL )
    {
        printf( "Booting\n" );
        sleep_ms( 100 );
    }
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);

    FRESULT fr;
    FATFS fs;
    FIL fil;
    int ret;
    char buf[100];
    char filename[] = "tomstest.txt";

    // Initialize the SD casd
    if( !sd_init_driver() )
    {
        printf( "Couldn't initialise the SD card :(\n" );
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        while( true );
    }
    else
        printf( "SD card initialised ok!\n" );

    // Mount the SD card
    fr = f_mount( &fs, "0:", 1 ); // "0:" must be the same in the hw_config.c file
    if( fr != FR_OK )
    {
        printf( "ERROR: Couldn't mount filesystem (%d)\r\n", fr );
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        while( true );
    }
    else
        printf( "SD card mounted ok!\n" );

    // Open file for writing
    fr = f_open( &fil, filename, FA_WRITE | FA_CREATE_ALWAYS );
    if( fr != FR_OK )
    {
        printf( "Error with f_open\n" );
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        while( true );
    }
    else
        printf( "File opened ok!\n" );

    // Write stuff to the file
    ret = f_printf( &fil, "This is my test string.\r\n");
    if( ret < 0 )
    {
        printf( "ERROR: Could not write to the file" );
        f_close( &fil );
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        while( true );
    }
    else
        printf( "Write operation successful!\n" );

    ret = f_printf( &fil, "Spud is a noisy bird.\r\n");
    if( ret < 0 )
    {
        printf( "ERROR: Could not write to the file" );
        f_close( &fil );
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        while( true );
    }
    else
        printf( "Write operation successful!\n" );

    // Close the file (very important)
    fr = f_close( &fil );
    if( fr != FR_OK )
    {
        printf( "Error, couldn't close the file" );
        f_close( &fil );
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        while( true );
    }
    else
        printf( "File closed ok!\n" );

    // Open the file again to read
    fr = f_open( &fil, filename, FA_READ );
    if( fr != FR_OK )
    {
        printf( "Error, couldn't open the file for reading" );
        f_close( &fil );
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        while( true );
    }
    else
        printf( "File reopened ok!\n" );
    
    // Read the contents of the file
    printf( "Reading from file %s:\r\n", filename );
    printf( "---\r\n" );
    while( f_gets( buf, sizeof(buf), &fil ) )
    {
        printf( buf );
    }
    printf( "---\r\n" );

    fr = f_close( &fil );
    if( fr != FR_OK )
    {
        printf( "Error, couldn't close the file" );
        f_close( &fil );
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        while( true );
    }
    else
        printf( "File closed ok!\n" );

    f_unmount( "0:" );
    printf( "SD card unmounted!\n" );

    // If you see flashing lights, no errors!
    while (true) {
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        sleep_ms(250);
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        sleep_ms(250);
    }
}

static inline uint64_t millis()
{
    return micros() / 1000ULL;
}

static inline uint64_t micros()
{
    return to_us_since_boot( get_absolute_time() );
}
