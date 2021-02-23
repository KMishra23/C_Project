#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "tiff_header.h"   /* Header for TIFF IFD structures and routines */

#pragma pack (1)//excludes unwanted padding

typedef unsigned char WORD;
typedef unsigned int DWORD;

struct _TiffHeader
{
	WORD  Identifier;  /* Byte-order Identifier */
	WORD  Version;     /* TIFF version number (always 2Ah) */
	DWORD IFDOffset;   /* Offset of the first Image File Directory*/
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
//components of a pixel
struct RGB {
    unsigned char blue;
    unsigned char green;
    unsigned char red;
};

DWORD getDwordFromBytes_littleEndian(int *b, int n)
{  
   // An Example:
   // b[0]  is D8 which is 11011000
   // b[1]  is 37 which is 00110111
   // b[2]  is 11 which is 00010001

   //We need to arrange it like : 11 37 D8 to make it little endian 
   // b[0]  = 11011000
   // b[1]<<8 = 0011011100000000
   // b[2]<<16 = 000100010000000000000000

   // 000100010000000000000000
   //    |    0011011100000000 |
   //    |    |       11011000 |
   // ========================
   // 000100010011011111011000 -> 1128408

   int sum = b[0];
   for(int i=1; i<n; i++)
      sum = sum | (b[i]<< (8 * i));
   return sum;
}

DWORD getDwordFromBytes_bigEndian(int *b, int n)
{
   //this is doing the same thing as above but does not reverse 
   //the order of the digits, hence it will start from the end 
   //of the array and left shift the previous array elements and | them.
   int sum = b[n-1];
   for(int i=n-2; i>=0; i++)
      sum = sum | (b[n-2]<< (8 * i));
   return sum;
}


void opentiff(const char *filename, struct BMP_Header *bmphead) {
    FILE *fp = fopen("Tree.tiff","rb");

    int c, i, max;
    int offsetBytes[4];
    struct _TiffHeader TIFHEAD;    
    struct IFD ifd; 
    short int Content;
    short int bits_per_sample_valoff;
    short int samples_per_pixel;

    if (fp == NULL)
    {
        fprintf(stderr, "cannot open input file\n");
    }

    // Byte Order Identifier
    fseek( fp, 0, SEEK_SET );
    int endian_identifier; //set to 0 if Little Endian and 1 if Big Endian
    for (i = 0; i < 1 && (c = getc(fp)) != EOF; i++) {
        TIFHEAD.Identifier = c; //reading one I or M is enough to read as same character is repeating II or MM
        if(c=='I')
        {
            printf("Byte Order is Little Endian\n");//I indicates little endian
            endian_identifier = 0;
        }
        else
        {
            printf("Byter Order is Big Endian\n");//M indicates big endian
            endian_identifier = 1;
        }
    }
    //Version
    if (endian_identifier==0) //if it's little endian it will be 2a00 so just read first byte
        fseek( fp, 2, SEEK_SET );
    else
        fseek(fp,3,SEEK_SET); //if it's big endian it will be 002a so read the 2a from the 3rd byte
    for (i = 0; i < 1 && (c = getc(fp)) != EOF; i++) {
        TIFHEAD.Version = c;
    }

   //Offset
   fseek( fp, 4, SEEK_SET );
   for (i = 0; i < 4 && (c = getc(fp)) != EOF; i++) {
      offsetBytes[i] = c; 
   }
   if(endian_identifier==0)
      TIFHEAD.IFDOffset = getDwordFromBytes_littleEndian(offsetBytes, 4);//to reverse the digits and make them into one address
   else
      TIFHEAD.IFDOffset = getDwordFromBytes_bigEndian(offsetBytes, 4);//create one number to access the address

   printf("Identifier: %c%c \n", TIFHEAD.Identifier,TIFHEAD.Identifier);
   printf("Version: %02x \n", TIFHEAD.Version);
   printf("Offset to IFD: %d \n", TIFHEAD.IFDOffset);

   //Read tiff_image_attrs
   struct image_attrs img_attrs;
   int ret = tiff_read_image_attrs(fp, endian_identifier, TIFHEAD.IFDOffset, &img_attrs);
   //Pick image attributes from img_attrs
   bmphead->height = img_attrs.image_length;
   bmphead->width = img_attrs.image_width;
   samples_per_pixel = img_attrs.samples_per_pixel;
   //Pick bits_per_sample val/offset
   bits_per_sample_valoff = img_attrs.bits_per_sample_valoff;
   if (img_attrs.x_resolution_denr)
       bmphead->Xres = (int)(img_attrs.x_resolution_numr / img_attrs.x_resolution_denr);
   if (img_attrs.y_resolution_denr)
       bmphead->Yres = (int)(img_attrs.y_resolution_numr / img_attrs.y_resolution_denr);
   //Pick number of IFD entries
   DWORD number_of_IFDEntries = img_attrs.ifd_cnt;
   print_image_attrs(img_attrs);

    bmphead->bytes_per_pixel = samples_per_pixel;
    bmphead->image_size = bmphead->bytes_per_pixel*bmphead->width*bmphead->height;
    
    int height = bmphead->height;
    int width = bmphead->width;
    int image_data_size = height * width * 3; //*3 for RGB
    int image_data_offset;
    WORD* image_data = (WORD*)malloc((image_data_size) * sizeof(WORD)); //array that reads byte by byte data 
    // malloc is used for dynamic memomry allocation of structures
    if(TIFHEAD.IFDOffset>8) //between the header and IFDOffset
    {
        image_data_offset = 8;
        fseek(fp, image_data_offset, SEEK_SET);       
        for(i = 0; i<(image_data_size) && (c=getc(fp)) != EOF; i++)         //Header occupies 8 bytes, so those number of bytes need not be read into the image data
            image_data[i] = c;
    }
    else //between the header and IFDOffset
    {
        //find size of IFD by using number of IFD entries and multiply it with size of each IFD 
        image_data_offset = 8 + (number_of_IFDEntries)*(12); // 8 is for header, followed by size of IFD entry (12) * number of IFD entries
        fseek(fp, image_data_offset, SEEK_SET);
        for(i=0; i<(image_data_size) && (c=getc(fp)) !=EOF; i++)
        {
            image_data[i] = c;//tiff image data getting stored in 1D array
        }
    }
   
    printf("IFD entry count: %d\n", number_of_IFDEntries);
    printf("%d %d\n",height,width);
    //read tiff
    struct RGB pixels[height][width]; // Each element of pixels 2d array has a structure RGB which include three components red green blue
    fseek(fp,image_data_offset,SEEK_SET);
    int k = 0;
    for(int i = 0; i < height; i++) 
    {
        //fread(pixels[i],3,width,fp);
        for(int j = 0; j < width; j++)
        {
            //while taking tiff data into array ,for a given pixel three components gets stored adjacently
            // so,ina group of three elements in tiff image data,first given red,then blue and then green
            pixels[i][j].red = image_data[k];
            pixels[i][j].green = image_data[k+1];
            pixels[i][j].blue = image_data[k+2];
            k = k + 3;
        }
    }
    printf("TIFF Read\n");
    fclose(fp);

    


//void writebmp(const char *filename, struct BMP_Header bmphead, struct RGB *pixel) {

    int bytesPerPixel = bmphead->bytes_per_pixel;
    FILE *outputFile  = fopen(filename,"wb");
    //BM is a header field used to identify BMP and DIB file                     2 bytes
    const char *BM = "BM";
    fwrite(&BM[0], 1, 1, outputFile);
    fwrite(&BM[1], 1, 1, outputFile);
    //size of BMP file in bytes                                                  4 bytes
    int paddedRowSize = (width + 3)*3;
    int fileSize = paddedRowSize*height + 54;
    fwrite(&fileSize, 4, 1, outputFile);
    //reserved is set to 0 if an image is created manually                       4 bytes
    int reserved = 0x0000;
    fwrite(&reserved, 4, 1, outputFile);
    //offset(starting address) of the byte where image data(array) starts        4 bytes
    int dataOffset = 54;
    fwrite(&dataOffset, 4, 1, outputFile);
    
    //*******BITMAP**INFORMATION**HEADER******//
     //starts with writing the size of this header                               4 bytes
    //default size od dib header is 40                                            
    int infoHeaderSize = 40;
    fwrite(&infoHeaderSize, 4, 1, outputFile);
    //widtha and height indicates the length and breadth of the image file   4 + 4 bytes
    fwrite(&width, 4, 1, outputFile);                                  
    fwrite(&height, 4, 1, outputFile);           
    //number of colour planes which is always 1                                  2 bytes
    short int planes = 1; 
    fwrite(&planes, 2, 1, outputFile);
    //space taken per pixel in bits which is actually colour depth of image      2 bytes                                        
    short int bitsPerPixel = bytesPerPixel * 8;
    fwrite(&bitsPerPixel, 2, 1, outputFile);
    //compression is assumed to be 0 as we take only uncompressed images         4 bytes
    int compression = 0;
    fwrite(&compression, 4, 1, outputFile);
    //imagesize is actually the size of raw bit map data                         4 bytes
    int imageSize = width*height*bytesPerPixel;
    fwrite(&imageSize, 4, 1, outputFile);
    //resolution is actually the no of pixel per meter along x and y direction 4+4 bytes 
    int resolutionX = 0x0000; 
    int resolutionY = 0x0000; 
    fwrite(&resolutionX, 4, 1, outputFile);
    fwrite(&resolutionY, 4, 1, outputFile);
    //no of colours in colour palette(default = 0)                               4 bytes
    int colorsUsed = 0;
    fwrite(&colorsUsed, 4, 1, outputFile);
    //number of important colours,0 if every co;our is important                 4 bytes
    int importantColors = 0;
    fwrite(&importantColors, 4, 1, outputFile);

    printf("Write Begin\n");
    //write to bmp
    char padding = 0x00;
    fseek(outputFile,54,SEEK_SET);
    for(int i = height-1; i >= 0; i--) {
        fwrite(pixels[i],3,width,outputFile);
        printf("Row:%d\n",i);  
        for(int j = 0; j < 2; j++) {
            fwrite(&padding,1,1,outputFile);
        }  
    }
  
}

int main() {
    struct RGB *pixels;
    struct BMP_Header bmphead;
    opentiff("Tree.bmp",&bmphead);
    //writebmp("KBMP.bmp",bmphead,pixels);
}
