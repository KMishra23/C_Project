#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#pragma pack (1)

struct TIFF_Header header;
struct IFD ifd;
short int Content;
short int bits_per_sample;
short int samples_per_pixel;
int height;
int width;

struct TIFF_Header {
    short int tag;
    short int version;
    int offsetToIFD;
};

struct IFD {
    short int Tag;
    short int Data_Type;
    int length_info;
    int info;   
};

struct BMP_Header {
        int size;
        int width;
        int height;
        short int bytes_per_pixel;
        int image_size;
        int Xres;
        int Yres;
};

struct RGB {
    unsigned char blue;
    unsigned char green;
    unsigned char red;
};