/*
  This is a font format converter that converts bdf font to lcdgfx c style free (= proportional) font.
  The converted font is for lcdgfx(https://github.com/lexus2k/lcdgfx).
  
  Date: August 29, 2020
  Copyright mayopan (https://github.com/mayopan)
  License: MIT. See license file for more information but you can
  basically do whatever you want with this code.
*/

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <bitset>
#include "bdf2lcdgfx.hpp"
using namespace std;

int main()
{
    const uint8_t   MAXChars = 255;
    FontHeader      header;
    uint8_t         count=0;
    uint8_t         char_num=0;
    uint16_t        bitmap;
    size_t          inc=0;
    FontChar        fontchar[MAXChars];
    string          filename, headerline, line,name;

    cout << "!!! BDF to LCDFont format converter !!!\nInput file name?" << endl;
    cin >> filename;
    if(filename.rfind(".")==string::npos)
    {
        filename+=".bdf";  
    }
    ifstream inputfile(filename);   
    if(inputfile.fail()==true)
    {
        cout << "Couldn't find input font file \"" << filename << "\" !" << endl;
        return 0;
    }

    name = filename.substr(0,filename.find(".bdf"));
    filename=name + ".c";

    ofstream outfile_c(filename);
    filename=name + ".h";
    ofstream outfile_h(filename);


/*********************************************************/
/****************  Read bdf file *************************/
/*********************************************************/
    while(getline(inputfile, line))                     //Read one line
    {
        stringstream line_s{line};
        vector<string> v;
        string s;

        /*****************************************************/
        /**************  Read header section  ****************/
        /*****************************************************/

        while(getline(line_s, s, ' '))                  //Store each part in one line to string vector
        {
            v.push_back(s);
        }

        if (HeaderMap.find(v[0]) != HeaderMap.end())    //If the read line is in the header section
        {
            HeaderMap[v[0]](header,v);                  //parse header infomation into Fontheader structure
        }
        else if(CharMap.find(v[0]) != CharMap.end())    //If the read line is in the char bitmap section
        {
            CharMap[v[0]](fontchar[char_num],v);        //parse char infomation into Fontchar structure
        }

        if(header.filled)                               //Output parserd header data into glcd format file
        {
            cout << format("Width: %d, Height: %d, Offset x: %d, Offset y: %d",header.width,header.height,header.offset_x,header.offset_y) << endl;
            outfile_c << "/* Original font " << header.copyright << " */" << endl;
            outfile_c << header_copyright;
            outfile_c << "#include \"" << name << ".h\"" << endl;
            outfile_c << "const uint8_t " << "free_" + name << "[] PROGMEM =" << endl;
            outfile_c << "{" << endl;
            outfile_c << "//  type|width|height|first char" << endl;
            outfile_c << format("    0x%02X, 0x%02X, 0x%02X, 0x%02X,",2,header.width,header.height,header.default_char) << endl;
            outfile_c << format("// GROUP first '  ' total %d chars", header.char_num) << endl;
            outfile_c << "//  unicode(LSB,MSB)|count" << endl;
            outfile_c << format("    0x%02X, 0x%02X, 0x%02X, // unicode record",0,header.default_char,header.char_num) << endl;

            outfile_h << header_copyright << header_cpp1;   //Fixed copyright and cpp setting section
            outfile_h << "/** free_" << name << format(" %dx%d font */\n",header.width, header.height);            
            outfile_h << "extern const PROGMEM uint8_t free_" << name << "[];";
            outfile_h << header_cpp2;               //Fixed cpp setting section

            header.filled=false;          
        }


        /*********************************************************/
        /*************  Read bdf bitmap data section  ************/
        /*********************************************************/
        
        if(fontchar[char_num].bitmap)
        {
            if(fontchar[char_num].filled)
            {
                cout    << "Char: " << fontchar[char_num].name << format(", Code: %d w: %d h: %d ox: %d oy: %d", fontchar[char_num].encoding, fontchar[char_num].width, fontchar[char_num].height, fontchar[char_num].offset_x, fontchar[char_num].offset_y) << endl;
                for(int j=0; j<fontchar[char_num].pad_h(); j++)
                {
                    for(int i=0; i<fontchar[char_num].pad_w(); i++)
                    {
                        if(fontchar[char_num].bdfgryph[i/8][j] & (1<<(7-i%8)) )
                        {
                            fontchar[char_num].glcdgryph[j/8][i] |= (1<<(j%8));
                        }
                    }
                }

                //  output gryph on terminal for check
                for(int j=0; j<fontchar[char_num].width; j++)
                {
                    for(int i=fontchar[char_num].pad_h()/8-1; i>=0; i--)
                    {
                        cout << static_cast<bitset<8> >(fontchar[char_num].glcdgryph[i][j]);
                    }
                    cout << endl;
                }
                cout << endl;

                char_num++;
                count = 0;
            }
            else if (count <= 0)
            {
                count++;
                // Just skip reading "BITMAP" line
            }
            else
            {
                uint8_t bit[line.size()/2];
                for(int i=0; i<line.size()/2; i++)
                {
                    stringstream bits;
                    bit[i]=stoi(line.substr(i*2,2), nullptr,16);
                    fontchar[char_num].bdfgryph[i][count-1]=bit[i];
                }
                count++;
            }

        }
        
    }

    /****************************************************************************/
    /************** Convert bitmap data from bdf format to glcd format **********/
    /****************************************************************************/

    uint16_t offset=0;
    uint8_t total_size=7;   // 1st line(4bytes): type|width|height|first char / 2nd line(3bytes): unicode(LSB,MSB)|count
    for(int char_num=0; char_num < header.char_num; char_num++)
    {
        outfile_c << "    ";
        outfile_c << format("0x%02x, 0x%02x, 0x%02x, 0x%02x, ",offset >> 8, offset & 0xFF, fontchar[char_num].width, fontchar[char_num].height);
        outfile_c << format("// char '%s' (0x%02X%02X/%d)",fontchar[char_num].name.c_str(),0,fontchar[char_num].encoding,fontchar[char_num].encoding) << endl;
        offset+= fontchar[char_num].pad_h()/8*fontchar[char_num].width;
        total_size+=4;
    }
    outfile_c << "    ";
    outfile_c << format("0x%02x, 0x%02x,",offset >> 8, offset & 0xFF) << endl;
    total_size+=2;
    for(int char_num=0; char_num < header.char_num; char_num++)
    {
        outfile_c << "    ";
        for(int i=0; i<fontchar[char_num].pad_h()/8; i++)
        {
            for(int j=0; j<fontchar[char_num].width; j++)
            {
                outfile_c << format("0x%02x, ", fontchar[char_num].glcdgryph[i][j]);
                total_size++;
            }
        }
        outfile_c << format("// char '%s' (0x%02X%02X/%d)",fontchar[char_num].name.c_str(),0,fontchar[char_num].encoding,fontchar[char_num].encoding) << endl;
    }

    outfile_c << "    0x00, 0x00, 0x00, // end of unicode tables" << endl;
    total_size+=3;
    outfile_c << format("    // FONT REQUIRES %d BYTES", total_size) << endl;
    outfile_c << "};" << endl;
    outfile_c.close();
    outfile_h.close();
    inputfile.close();

    return 0 ;
}