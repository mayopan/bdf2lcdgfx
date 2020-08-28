#include <string>
#include <vector>
#include <map>
#define MAX_FONT_PIX_WIDTH 32
#define MAX_FONT_PIX_HEIGHT 32
using namespace std;

template <typename ... Args>
string format(const string& fmt, Args ... args )
{
    size_t len = snprintf( nullptr, 0, fmt.c_str(), args ... );
    vector<char> buf(len + 1);
    snprintf(&buf[0], len + 1, fmt.c_str(), args ... );
    return string(&buf[0], &buf[0] + len);
}

//  Parsing map for bdf font header


class FontHeader
{
    private:

    public:
    int8_t  width, height, offset_x, offset_y;
    int16_t default_char, char_num;
    string  copyright;
    bool    filled = false;
};

void Fontboundingbox(FontHeader& head, vector<string> v)
{
    head.width=stoi(v[1]);
    head.height=stoi(v[2]);
    head.offset_x=stoi(v[3]);
    head.offset_y=stoi(v[4]);
};

void DefaultChar(FontHeader& head, vector<string> v)
{
    head.default_char=stoi(v[1]);
};

void Copyright(FontHeader& head, vector<string> v)
{
    string text;
    for(int i=1; i<v.size(); i++)
    {
        head.copyright+=v[i];
        head.copyright+=" ";
    }
};

void Chars(FontHeader& head, vector<string> v)
{
    head.char_num=stoi(v[1]);
    head.filled=true;
};

map<string , void(*)(FontHeader&, vector<string>)> HeaderMap= {
    { "FONTBOUNDINGBOX", Fontboundingbox },
    { "DEFAULT_CHAR", DefaultChar },
    { "COPYRIGHT", Copyright },
    { "CHARS", Chars }
};//switch action acording to header text

class FontChar
{
    private:

    public:
    int8_t offset_x, offset_y;
    int16_t encoding;
    string  name;
    uint8_t bdfgryph[MAX_FONT_PIX_WIDTH/8][MAX_FONT_PIX_HEIGHT],width,height;
    uint8_t glcdgryph[MAX_FONT_PIX_HEIGHT/8][MAX_FONT_PIX_WIDTH];
    bool    filled = false;
    bool    bitmap = false;

    void reset();

    uint8_t pad_w()
    {
        uint8_t pad_w;
        pad_w = ((width+7)/8)*8;
        return pad_w;
    }//Padding pixel size to multiple 8 (one uint8 can express monochrome 8 pixels)

    uint8_t pad_h()
    {
        uint8_t pad_h;
        pad_h = ((height+7)/8)*8;
        return pad_h;
    }//Padding pixel size to multiple 8 (one uint8 can express monochrome 8 pixels)
};//Holding all char data 

inline void FontChar::reset()
{
    width=0;
    height=0;
    offset_x=0;
    offset_y=0;
    encoding=0;
    name="";
    bitmap=false;
    filled=false;
    for (int i=0; i<MAX_FONT_PIX_WIDTH/8; i++)
    {
        for (int j=0; j<MAX_FONT_PIX_HEIGHT; j++)
        {
            bdfgryph[i][j]=0;
            glcdgryph[i][j]=0;
        }
    }
}

void Char_name(FontChar& data, vector<string> v)
{
    data.name=v[1];
};

void Char_code(FontChar& data, vector<string> v)
{
    data.encoding=stoi(v[1]); 
};

void Char_bbx(FontChar& data, vector<string> v)
{
    data.width=stoi(v[1]);
    data.height=stoi(v[2]);
    data.offset_x=stoi(v[3]);
    data.offset_y=stoi(v[4]);
};

void Bit_map(FontChar& data, vector<string> v)
{
    data.bitmap=true;
};

void Char_end(FontChar& data, vector<string> v)
{
    data.filled=true;
};

map<string , void(*)(FontChar&, vector<string>)> CharMap= {
    { "STARTCHAR", Char_name },
    { "ENCODING", Char_code },
    { "BBX", Char_bbx },
    { "BITMAP", Bit_map},
    { "ENDCHAR", Char_end }
};//switch action acording to header text

const string header_copyright = 
"/*\n\
    MIT License\n\
\n\
    Copyright (c) 2018-2019, Alexey Dynda\n\
    Portions Copyright (c) 2020, Mayopan\n\
\n\
    Permission is hereby granted, free of charge, to any person obtaining a copy\n\
    of this software and associated documentation files (the ""Software""), to deal\n\
    in the Software without restriction, including without limitation the rights\n\
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell\n\
    copies of the Software, and to permit persons to whom the Software is\n\
    furnished to do so, subject to the following conditions:\n\
\n\
    The above copyright notice and this permission notice shall be included in all\n\
    copies or substantial portions of the Software.\n\
\n\
    THE SOFTWARE IS PROVIDED ""AS IS"", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR\n\
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,\n\
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE\n\
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER\n\
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,\n\
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE\n\
    SOFTWARE.\n\
*/\n";

const string header_cpp1 ="\
\n\
#pragma once\n\
\n\
#include \"canvas/canvas_types.h\"\n\
\n\
#ifdef __cplusplus\n\
extern \"C\" {\n\
#endif\n\
\n\
/**\n\
 * @defgroup LCD_FONTS FONTS: Supported LCD fonts\n\
 * @{\n\
 */\n\
\n";

const string header_cpp2 ="\n\n\
/**\n\
 * @}\n\
 */\n\
\n\
#ifdef __cplusplus\n\
}\n\
#endif\n";
