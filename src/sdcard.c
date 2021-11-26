#include "stm32f0xx.h"
#include "ff.h"
#include "diskio.h"
#include "commands.h"
#include "sdcard.h"
#include "stdio.h"
#include "wav.h"

FRESULT openSDCardFile(FATFS *FatFs, FIL *fil) {
    FRESULT res;

    res = f_mount(FatFs, "", 0); // Mount SD card

    // If mounting results in an error, return
    if (res) {
        return res;
    }

    // Open file, and return it's result code
    return f_open(fil, "test/abc.txt", FA_READ);
}

FRESULT closeSDCardFile(FATFS *FatFs, FIL *fil) {
    FRESULT res;

    res = f_close(fil); // Close file

    // If closing file results in an error, return
    if (res) {
        return res;
    }

    // Unmount SD card after using it, then return it's result code
    return f_mount(0, "", 1);
}

FRESULT printSDCardTextFile() {
    FATFS FatFs;
    FRESULT res;
    FIL fil;
    char line[100];
    //uint32_t total, free;

    res = openSDCardFile(&FatFs, &fil);

    if (res) {
        return res;
    }

    while (f_gets(line, sizeof line, &fil)) {
        printf(line);
    }

    return closeSDCardFile(&FatFs, &fil);
}
