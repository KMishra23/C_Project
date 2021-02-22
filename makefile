# Filename: makefile

all_sources = Conversion_V3.c tiff_ifd_read.c
all_objs = $(all_sources:.c=.o)

# Rules

tiff_to_bmp : $(all_objs)
	gcc -o tiff_to_bmp $(all_objs)

%.o : %.c
	gcc -c $<

clean :
	rm tiff_to_bmp *.o

