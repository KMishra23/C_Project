#include "header.h"

struct _TiffHeader
{
	unsigned char  Identifier;  /* Byte-order Identifier */
   unsigned char  Version;     /* TIFF version number (always 2Ah) */
	unsigned int IFDOffset;   /* Offset of the first Image File Directory*/
}TIFHEAD;

unsigned char getDwordFromBytes_littleEndian(int *b, int n)
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

unsigned char getDwordFromBytes_bigEndian(int *b, int n)
{
   //this is doing the same thing as above but does not reverse 
   //the order of the digits, hence it will start from the end 
   //of the array and left shift the previous array elements and | them.
   int sum = b[n-1];
   for(int i=n-2; i>=0; i++)
      sum = sum | (b[n-2]<< (8 * i));
   return sum;
}


void read_header(const char *input, struct BMP_Header *bmphead)
{
    FILE *fp = fopen(input,"rb");

    int c, i ,offsetBytes[4];

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

