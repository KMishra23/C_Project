#ifndef __TIFF_HEADER_H
#define __TIFF_HEADER_H

/* Swap bytes macros for 2 bytes and 4 bytes */
#define SWAP_UINT16(x, e) ((e == BIG_ENDIAN) ? (((x) >> 8) | ((x) << 8)) : x)
#define SWAP_UINT32(x, e) ((e == BIG_ENDIAN) ? (((x) >> 24) | (((x) & 0x00FF0000) >> 8) | (((x) & 0x0000FF00) << 8) | ((x) << 24)) : x)

/* TIFF IFD entry structure */
struct ifd_entry {
    unsigned short field_tag;
    unsigned short field_type;
    unsigned int   val_len;
    unsigned int   val_off;
};

/* TIFF IFD tag definitions */
#define ImageWidth 256
#define ImageLength 257
#define BitsPerSample 258
#define Compression 259
#define PhotometricInterpretation 262
#define StripOffsets 273
#define SamplesPerPixel 277
#define RowsPerStrip 278
#define StripByteCounts 279
#define XResolution 282
#define YResolution 283
#define PlanarConfiguration 284
#define ResolutionUnit 296

/* Structure for Image attributes */
struct image_attrs {
    unsigned int   endian_flag;
    unsigned short ifd_cnt;
    unsigned int   image_width;          /* 2 or 4 bytes */
    unsigned int   image_length;         /* 2 or 4 bytes */
    unsigned int   bits_per_sample_valoff;
    unsigned short compression;
    unsigned short photometric_interpretation;
    unsigned int   strip_counts;
    unsigned int   strip_offsets;        /* 2 or 4 bytes */
    unsigned short samples_per_pixel;
    unsigned int   rows_per_strip;       /* 2 or 4 bytes */
    unsigned int   strip_byte_counts;    /* 2 or 4 bytes */
    unsigned int   x_resolution_numr;    /* rational type numerator 4 bytes */
    unsigned int   x_resolution_denr;    /* rational type denominator 4 bytes */
    unsigned int   y_resolution_numr;    /* rational type numerator 4 bytes */
    unsigned int   y_resolution_denr;    /* rational type denominator 4 bytes */
    unsigned short planar_configuration;
    unsigned short resolution_unit;
};

/* TIFF IFD entries image attributes - read and print (debug) routines */
int tiff_read_image_attrs(FILE *fp, unsigned int endian_ness, unsigned int ifd_start, struct image_attrs *pimg_attrs);
int print_image_attrs(struct image_attrs img_attrs);

#endif // __TIFF_HEADER_H

