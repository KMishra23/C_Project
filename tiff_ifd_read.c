#include <sys/types.h>
//#include <sys/stat.h>
//#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
//#include <stdlib.h>
#include <string.h>
#include "tiff_header.h"

/* Print routine to print all image attributes extracted for debugging purposes */
int print_image_attrs(struct image_attrs img_attrs)
{
    printf("Endianness: %u\n", img_attrs.endian_flag);
    printf("IFD count: %u\n", img_attrs.ifd_cnt);
    printf("Image width: %u\n", img_attrs.image_width);
    printf("Image length: %u\n", img_attrs.image_length);
    printf("Bits per sample: %u\n", img_attrs.bits_per_sample_valoff);
    printf("Compression: %u\n", img_attrs.compression);
    printf("Photometric interpretation: %u\n", img_attrs.photometric_interpretation);
    printf("Strip counts: %u\n", img_attrs.strip_counts);
    printf("Strip offsets: %u\n", img_attrs.strip_offsets);
    printf("Samples per pixel: %u\n", img_attrs.samples_per_pixel);
    printf("Row per strip: %u\n", img_attrs.rows_per_strip);
    printf("Strip byte counts: %u\n", img_attrs.strip_byte_counts);
    printf("X resolution numerator: %u\n", img_attrs.x_resolution_numr);
    printf("X resolution denominator: %u\n", img_attrs.x_resolution_denr);
    printf("Y resolution numerator: %u\n", img_attrs.y_resolution_numr);
    printf("Y resolution denominator: %u\n", img_attrs.y_resolution_denr);
    printf("Planar configuration: %u\n", img_attrs.planar_configuration);
    printf("Resolution unit: %u\n", img_attrs.resolution_unit);
    return 0;
}

/* TIFF image IFD entries read routine to extract image attributes */
int tiff_read_image_attrs(FILE *fp, unsigned int endian_ness, unsigned int ifd_start, struct image_attrs *pimg_attrs)
{
    /* Get fd corresponding to file pointer fp to be used in low level file IO routines */
    int tfd = fileno(fp);
    if (tfd == -1) {
        printf("Invalid input file name\n");
        return -1;
    }
    /* Initialize the image attributes structure with 0s */
    memset(pimg_attrs, 0, sizeof(struct image_attrs));
    /* Keep endian_ness together with image attributes for further use if needed */
    if (endian_ness == 0) {
        pimg_attrs->endian_flag = LITTLE_ENDIAN;
    } else if (endian_ness == 1) {
        pimg_attrs->endian_flag = BIG_ENDIAN;
    } else {
        printf("Invalid file format: Endianness field error\n");
        return -1;
    }
    /* Seek to the start of the ifd entries information in the image file */
    int off = lseek(tfd, ifd_start, 0);
    /* Extract IFD entries count as image attribute */
    int sz = read(tfd, &(pimg_attrs->ifd_cnt), sizeof(pimg_attrs->ifd_cnt));
    pimg_attrs->ifd_cnt = SWAP_UINT16(pimg_attrs->ifd_cnt, pimg_attrs->endian_flag);
    //printf("%8.8X %8.8X\n", pimg_attrs->endian_flag, pimg_attrs->ifd_cnt);
    struct ifd_entry ifd;
    int idx;
    unsigned int offset;
    /* Iterate over IFD entries and extract corresponding image attributes */
    for (idx = 0; idx < pimg_attrs->ifd_cnt; idx++) {
        sz = read(tfd, &ifd, sizeof(ifd));
        ifd.field_tag = SWAP_UINT16(ifd.field_tag, pimg_attrs->endian_flag);
        ifd.field_type = SWAP_UINT16(ifd.field_type, pimg_attrs->endian_flag);
        ifd.val_len = SWAP_UINT32(ifd.val_len, pimg_attrs->endian_flag);
        //printf("%8.8X %8.8X %8.8X %8.8X\n", ifd.field_tag, ifd.field_type, ifd.val_len, ifd.val_off);
        switch (ifd.field_tag) {
            case ImageWidth:
		/* type == 3 indicates short */
                if (ifd.field_type == 3)
                    pimg_attrs->image_width = SWAP_UINT16((unsigned short)ifd.val_off, pimg_attrs->endian_flag);
		/* type == 4 indicates short */
                else if (ifd.field_type == 4)
                    pimg_attrs->image_width = SWAP_UINT32((unsigned int)ifd.val_off, pimg_attrs->endian_flag);
                break;
            case ImageLength:
                if (ifd.field_type == 3)
                    pimg_attrs->image_length = SWAP_UINT16((unsigned short)ifd.val_off, pimg_attrs->endian_flag);
                else if (ifd.field_type == 4)
                    pimg_attrs->image_length = SWAP_UINT32((unsigned int)ifd.val_off, pimg_attrs->endian_flag);
                break;
            case BitsPerSample:
                pimg_attrs->bits_per_sample_valoff = SWAP_UINT32((unsigned int)ifd.val_off, pimg_attrs->endian_flag);
                break;
            case Compression:
                pimg_attrs->compression = SWAP_UINT16((unsigned short)ifd.val_off, pimg_attrs->endian_flag);
                break;
            case PhotometricInterpretation:
                pimg_attrs->photometric_interpretation = SWAP_UINT16((unsigned short)ifd.val_off, pimg_attrs->endian_flag);
                break;
            case StripOffsets:
                /* Number of strip offsets gives number of strips */
                pimg_attrs->strip_counts = ifd.val_len;
                if (ifd.field_type == 3)
                    pimg_attrs->strip_offsets = SWAP_UINT16((unsigned short)ifd.val_off, pimg_attrs->endian_flag);
                else if (ifd.field_type == 4)
                    pimg_attrs->strip_offsets = SWAP_UINT32((unsigned int)ifd.val_off, pimg_attrs->endian_flag);
                break;
            case SamplesPerPixel:
                pimg_attrs->samples_per_pixel = SWAP_UINT16((unsigned short)ifd.val_off, pimg_attrs->endian_flag);
                break;
            case RowsPerStrip:
                if (ifd.field_type == 3)
                    pimg_attrs->rows_per_strip = SWAP_UINT16((unsigned short)ifd.val_off, pimg_attrs->endian_flag);
                else if (ifd.field_type == 4)
                    pimg_attrs->rows_per_strip = SWAP_UINT32((unsigned int)ifd.val_off, pimg_attrs->endian_flag);
                break;
            case StripByteCounts:
                if (ifd.field_type == 3)
                    pimg_attrs->strip_byte_counts = SWAP_UINT16((unsigned short)ifd.val_off, pimg_attrs->endian_flag);
                else if (ifd.field_type == 4)
                    pimg_attrs->strip_byte_counts = SWAP_UINT32((unsigned int)ifd.val_off, pimg_attrs->endian_flag);
                break;
            case XResolution:
		/* type == 5 indicates Rational i.e. two LONGs (4 bytes each) */
		/* Extract offset for X resolution numerator (first) and denominator (second) */
                offset = SWAP_UINT32((unsigned int)ifd.val_off, pimg_attrs->endian_flag);
		/* Store current offset for continuing IFD reads */
                off = lseek(tfd, 0, SEEK_CUR);
		/* Seek offset for X resolution */
                offset = lseek(tfd, offset, 0);
                sz = read(tfd, &(pimg_attrs->x_resolution_numr), sizeof(pimg_attrs->x_resolution_numr));
                sz = read(tfd, &(pimg_attrs->x_resolution_denr), sizeof(pimg_attrs->x_resolution_denr));
                pimg_attrs->x_resolution_numr = SWAP_UINT32(pimg_attrs->x_resolution_numr, pimg_attrs->endian_flag);
                pimg_attrs->x_resolution_denr = SWAP_UINT32(pimg_attrs->x_resolution_denr, pimg_attrs->endian_flag);
		/* Resume offset back for continuing IFD reads */
                off = lseek(tfd, off, 0);
                break;
            case YResolution:
                offset = SWAP_UINT32((unsigned int)ifd.val_off, pimg_attrs->endian_flag);
                off = lseek(tfd, 0, SEEK_CUR);
                offset = lseek(tfd, offset, 0);
                sz = read(tfd, &(pimg_attrs->y_resolution_numr), sizeof(pimg_attrs->y_resolution_numr));
                sz = read(tfd, &(pimg_attrs->y_resolution_denr), sizeof(pimg_attrs->y_resolution_denr));
                pimg_attrs->y_resolution_numr = SWAP_UINT32(pimg_attrs->y_resolution_numr, pimg_attrs->endian_flag);
                pimg_attrs->y_resolution_denr = SWAP_UINT32(pimg_attrs->y_resolution_denr, pimg_attrs->endian_flag);
                off = lseek(tfd, off, 0);
                break;
            case PlanarConfiguration:
                pimg_attrs->planar_configuration = SWAP_UINT16((unsigned short)ifd.val_off, pimg_attrs->endian_flag);
                break;
            case ResolutionUnit:
                pimg_attrs->resolution_unit = SWAP_UINT16((unsigned short)ifd.val_off, pimg_attrs->endian_flag);
                break;
        }
        //printf("%8.8X %8.8X %8.8X %8.8X\n", ifd.field_tag, ifd.field_type, ifd.val_len, ifd.val_off);
    }
    //printf("%8.8X\n", idx);
    unsigned int next_ifd_off;
    /* Subfiles are not supported */
    sz = read(tfd, &next_ifd_off, sizeof(next_ifd_off));
    if (next_ifd_off != 0) {
        printf("File format not supported: Tiff file with subfiles is not supported\n");
        return -1;
    }
    return 0;
}

