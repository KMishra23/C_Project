#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#pragma pack (1)

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

void opentiff(const char *filename, struct BMP_Header *bmphead) {
    FILE *fp = fopen("Tree.tiff","rb");

   
    struct TIFF_Header header;
    struct IFD ifd; 
    short int Content;
    short int bits_per_sample;
    short int samples_per_pixel;

    fread(&header,sizeof(struct TIFF_Header),1,fp);
    printf("Tag:%x\nVersion:%d\nOffset:%d\n",header.tag,header.version,header.offsetToIFD);
    
    fseek(fp,header.offsetToIFD,SEEK_SET);
    fread(&Content,sizeof(short int),1,fp);
   
    int check;
    for(int i = 0; i < Content; i++) {
        fread(&ifd,sizeof(struct IFD),1,fp);

        if(ifd.Tag == 256) bmphead->width = ifd.info;
        else if(ifd.Tag == 257) bmphead->height = ifd.info;
        else if(ifd.Tag == 258) bits_per_sample = ifd.info;
        else if(ifd.Tag == 277) samples_per_pixel = ifd.info; 
        else if(ifd.Tag == 282) bmphead->Xres = ifd.info;
        else if(ifd.Tag == 283) bmphead->Yres = ifd.info;  
        //printf("Tag:%d\nData Type:%d\nLength of Information:%d\nInfo:%d\n",ifd.Tag,ifd.Data_Type,ifd.length_info,ifd.info);   
    }

    bmphead->bytes_per_pixel = samples_per_pixel;
    bmphead->image_size = bmphead->bytes_per_pixel*bmphead->width*bmphead->height;
    
    
    printf("Content:%d\n",Content);
    int height = bmphead->height;
    int width = bmphead->width;
    printf("%d %d\n",height,width);
    //read tiff

    int offsettopixels = 8;
    struct RGB pixels[height][width]; 
    fseek(fp,8,SEEK_SET);
    
    for(int i = 0; i < height; i++) {
        //fread(pixels[i],3,width,fp);
        for(int j = 0; j < width; j++){
        for(int k = 0; k < 3; k++){
            if(k==0) fread(&pixels[i][j].red,1,1,fp);
            else if(k == 2) fread(&pixels[i][j].blue,1,1,fp);
            else fread(&pixels[i][j].green,1,1,fp);
        }
        }
    }
    printf("TIFF Read\n");
    fclose(fp);

    


//void writebmp(const char *filename, struct BMP_Header bmphead, struct RGB *pixel) {

    int bytesPerPixel = bmphead->bytes_per_pixel;
    int i = 0;
    
    FILE *outputFile  = fopen(filename,"wb");
    const char *BM = "BM";
    fwrite(&BM[0], 1, 1, outputFile);
    fwrite(&BM[1], 1, 1, outputFile);
    
    int paddedRowSize = (width + 3)*3;
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
    
    int imageSize = width*height*bytesPerPixel;
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
