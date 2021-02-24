#include "header.h"

void writebmp(const char *filename, struct BMP_Header *bmphead, struct RGB pixels[height][width]) {
    int amount_padding; 
    if(bmphead->width*3 % 4 == 0) amount_padding = 0;
    else if(bmphead->width*3 % 4 == 1) amount_padding = 3;
    else if(bmphead->width*3 % 4 == 2) amount_padding = 2;
    else amount_padding = 1; 

    int width= bmphead->width;
    int height = bmphead->height;

    int bytesPerPixel = bmphead->bytes_per_pixel;
    int i = 0;
    
    FILE *outputFile  = fopen(filename,"wb");
    const char *BM = "BM";
    fwrite(&BM[0], 1, 1, outputFile);
    fwrite(&BM[1], 1, 1, outputFile);
    
    int paddedRowSize = (width)*3 + amount_padding;
    int fileSize = paddedRowSize*height + 54;
    fwrite(&fileSize, 4, 1, outputFile);
    
    int reserved = 0x0000;
    fwrite(&reserved, 4, 1, outputFile);
    
    int dataOffset = 54;
    fwrite(&dataOffset, 4, 1, outputFile);

    //*******INFO*HEADER******//
    
    int infoHeaderSize = 40;
    fwrite(&infoHeaderSize, 4, 1, outputFile);
    
    fwrite(&width, 4, 1, outputFile);
    fwrite(&height, 4, 1, outputFile);
    
    short int planes = 1; 
    fwrite(&planes, 2, 1, outputFile);
    
    short int bitsPerPixel = bytesPerPixel * 8;
    fwrite(&bitsPerPixel, 2, 1, outputFile);
    
    int compression = 0;
    fwrite(&compression, 4, 1, outputFile);
    
    int imageSize = paddedRowSize*height;
    fwrite(&imageSize, 4, 1, outputFile);
    
    int resolutionX = 0x0000; 
    int resolutionY = 0x0000; 
    fwrite(&resolutionX, 4, 1, outputFile);
    fwrite(&resolutionY, 4, 1, outputFile);
       
    int colorsUsed = 0;
    fwrite(&colorsUsed, 4, 1, outputFile);
    
    int importantColors = 0;
    fwrite(&importantColors, 4, 1, outputFile);

    printf("Write Begin\n");
    //write to bmp
    char padding = 0x00;
    printf("Amount of padding required: %d\n",amount_padding);
    
    
    fseek(outputFile,54,SEEK_SET);
    for(int i = height-1; i >= 0; i--) {
        fwrite(pixels[i],3,width,outputFile);
        //printf("Row:%d\n",i);  
        for(int j = 0; j < amount_padding; j++) {
            fwrite(&padding,1,1,outputFile);
        }  
    }

}