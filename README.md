
ESS111 : Programming 1 (C Programming) Class project
====================================================
Group project 17. Source format: TIFF. Destination format: 24-bit BMP
=====================================================================

1. build
========

make clean (ignore first time errors of files not found)
make

The make uses makefile that has tiff_to_bmp and clean targets and rules and dependancies for building those.
The tiff_to_bmp is the target for building the final tiff to bmp conversion executable.
The clean target is the target for cleaning up previously made all object files and tiff_to_bmp executable file.

2. test
=======

To test the tiff_to_bmp conversion, a Tree.tiff sample image file is used. On executing tiff_to_bmp, it converts Tree.tiff to equivalent Tree.bmp

test: Tree.tiff (image is used)
$ ./tiff_to_bmp 
...


3. Team members
===============

GROUP 17
IMT2020137 Kaushik Mishra
IMT2020511 Sirigiri Sai Keerthan
IMT2020531 Saketh Gajawada
IMT2020535 Shreeya Manasvi Venneti
IMT2020025 Aniruddha Shailesh Chaudhari
IMT2020026 Vyom Sharma


4. Contributions
================

4.1 IMT2020535 Shreeya Manasvi Venneti
    (a) Read and understood the TIFF file format. Explored different ways of reading the file.  
    (b) Defined the general structures and struct for header on the basis of the TIFF file format. Wrote the code for reading the header of the TIFF file.  
    (c) Wrote the code for reading the image/pixel data from the TIFF file into appropriate data structures. 
    (d) Reorganised the logic of the code to support conversion and wrote additional code, so that it will aid in conversion of TIFF file to BMP file. 
6.2 IMT2020025 Aniruddha Shailesh Chaudhari :
    (a) IFD entries reading (tiff_ifd_read.c and tiff_header.h) and extraction code.
    (b) assistance in makefile creation.
4.3 IMT2020531 Saketh Gajawada :
    (a) read about mainly about tiff and pixel data conversion. 
    (b) assistance in IFD(tags) extraction and defining structures for those tags.
    (c) taking tiff image data and converting it into bmp image data.
4.4 IMT2020137 Kaushik Mishra:
    (a) Read and understood about both tiff and bmp files and mainly how pixel data is stored.
    (b) Conversion of image data obtained from tiff file to appropriate form that can be used to write to bmp file.
    (c) Figured out what IFD Tags read from the tiff file are required to write the bmp header file. 
4.5
4.6 IMT2020511 Sai Keerthan :
    (a) forming a output bmp file using binary mode.
    (b) makefile formation for this program.


