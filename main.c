#include "header.h"

int main(int argc,char *argv[]) {
    struct BMP_Header bmphead;
    read_header(argv[1], &bmphead);
    struct RGB pixels[height][width];
    read_pixel_data(argv[1], pixels, &bmphead);
    writebmp(argv[2],&bmphead, pixels);
}