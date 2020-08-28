# bdf2lcdgfx
This is a font format converter that converts bdf font to lcdgfx c style free (= proportional) font.
The converted font is for [lcdgfx] (https://github.com/lexus2k/lcdgfx).
lcdgfx is very usable driver for lcds, oleds, I like it!
I also like [U8G2] (https://github.com/olikraus/u8g2) that is similar driver especialy for monochrome display.
U8G2 has a lot of fonts, but lcdgfx has a few.
So I made this converter.
For example, there are nice fonts such as [crox3hb] (https://github.com/olikraus/u8g2/wiki/fntgrpcrox#crox3hb).
It comes from win_crox has been downloaded from http://www.kovrik.com/sib/russify/x-windows/ (Xrus CP 1251 fonts).

## How to use
First, you have to build bdf2lcdgfx.cpp to executable file.
In my Win10 einviroment, it can be compiled and build with g++ on Mingw64.

Usage:
    1. Run the executable "./bdf2lcdgfx"
    2. Input bdf file name (No need to type extention ".bdf")
    3. Then 2 files are built; fontname.c and fontname.h
    4. Use the font following the guidance in [lcdgfx wiki](https://github.com/lexus2k/lcdgfx/wiki/How-to-create-new-font-for-the-library).

~~~
USERLOCATION> ./bdf2lcdgfx
!!! BDF to LCDFont format converter !!!
Input file name?
crox3hb
~~~

## And more
font_viewer.xlsx is excel sheet that can show a gryph with both bdf and lcdgfx format.
It may help you to understand both font format bitmap structures.