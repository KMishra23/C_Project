#include "header.h"

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