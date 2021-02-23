#include "header.h"

struct TIFF_Header header;
struct IFD ifd;
short int Content;
short int bits_per_sample;
short int samples_per_pixel;
int height;
int width;


void read_header(const char *input, struct BMP_Header *bmphead)
{
    FILE *fp = fopen(input,"rb");
    
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
    
    printf("Samples per pixel:%d\n",samples_per_pixel);
    printf("Content:%d\n",Content);
    height = bmphead->height;
    width = bmphead->width;
    printf("%d %d\n",height,width);
    fclose(fp);
}

void read_pixel_data(const char *filename, struct RGB pixels[height][width], struct BMP_Header *bmphead)
{
    FILE *fp = fopen(filename,"rb");
    int offsettopixels = 8; 
    
    if(header.offsetToIFD == 8) {
        fseek(fp,8+(Content)*12,SEEK_SET);
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
    }
    else {
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
    }
    printf("TIFF Read\n");
    fclose(fp);
}


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

int main() {
    struct BMP_Header bmphead;
    read_header("tiff2.tiff", &bmphead);
    struct RGB pixels[height][width];
    read_pixel_data("tiff2.tiff", pixels, &bmphead);
    writebmp("abc.bmp",&bmphead, pixels);
}