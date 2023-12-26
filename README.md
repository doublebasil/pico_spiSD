This is based on a YouTube video by digikey, although the video is a little
out of date due to some changes to the repo they were using.

https://www.youtube.com/watch?v=u-vmsIr-s7w&t=541s

How I got this to work:

Copy the consicely named no-OS-FatFS-SD-SPI-Rpi-Pico from here:

```git clone https://github.com/carlk3/no-OS-FatFS-SD-SPI-Rpi-Pico.git```
```https://github.com/carlk3/no-OS-FatFS-SD-SPI-RPi-Pico```

This is the latest hash when I cloned: 196016f

And the date is 26/12/2023

# Note

There are some warnings when compiling the no-OS-FatFS-SD-SPI-Rpi-Pico repo, so
you should not use -Werror when compiling.

I believe the no-OS-FatFS-SD-SPI-Rpi-Pico repo is based on 
http://elm-chan.org/fsw/ff/00index_e.html

So you can look there for the f_open type commands.

I am using something similar to this board:

https://www.amazon.co.uk/Module-Conversion-Arduino-Adapter-Reader/dp/B08P5JJDZK/ref=sr_1_3?crid=DUD4KNKPFTNH&keywords=sd%2Bcard%2Breader%2Barduino%2B3v&qid=1703597266&sprefix=%2Caps%2C207&sr=8-3&th=1

I power it using 5V, and obviously control it using pico's 3v3 gpios. It didn't work when VCC
was connected to 3v3

# How to get it working

Copy simple_example/hw_config.c into FatFs_SPI/sd_driver.
This file is needed for the SD card config.

Within hw_config.c, modify the spi_t struct:
- .hw_inst = spi1, // I used spi1 instead of spi0
- .miso_gpio = 8,  // GPIO number (not Pico pin number)
- .mosi_gpio = 11,
- .sck_gpio = 10,
- .baud_rate = 1000 * 1000 // You can probably increase this later, but this is a good starting point

Next, still within hw_config.c, set .ss_gpio = 9, and set .use_card_detect = false

Note the value of .pcName (usually "0:") as this will be needed for mounting.

Next, you need to add the hw_config.c file to the CMakeLists.txt.
Go to FatFs_SPI/CMakeLists.txt, and uncomment the following line:

```${CMAKE_CURRENT_LIST_DIR}/sd_driver/hw_config.c```

Now in the root of your project, in source/CMakeLists.txt, you need to add the 
FatFs_SPI library using

```add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/no-OS-FatFS-SD-SPI-Rpi-Pico/FatFs_SPI build)```

And link it to your project

```
target_link_libraries(${PROJECT_NAME}
    pico_stdlib
    pico_cyw43_arch_none
    FatFs_SPI
    # You'll need to link other libraries for other pico functions
    )
```

And remember to disable -Werror
