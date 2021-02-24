#include "header.h"


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

