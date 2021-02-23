
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

4.1 IMT2020025 Aniruddha Shailesh Chaudhari
    (a) IFD entries reading and extraction code
    (b) assistance in makefile creation
4.2
4.3
4.4
4.5
4.6


