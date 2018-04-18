//---------------------------------------------------------------------------
#include "agdv.pch.h"
#include "Image.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
int g_File = 1;
//---------------------------------------------------------------------------
__fastcall Image::Image(ImageType type, const String& data)
: m_ScalarX(1)
, m_Frames(1)
, m_Palette(mpZXSpectrum)
{
    m_Bitmap = std::make_unique<TBitmap>();
    m_Bitmap->PixelFormat = pf32bit;
    // create an empty type of image
    switch (type)
    {
        case itBlock:
            m_Bitmap->Width = 8;
            m_Bitmap->Height = 8;
            break;
        case itObject:
            m_Bitmap->Width = 16;
            m_Bitmap->Height = 16;
            break;
        case itSprite:
            m_Bitmap->Width = 16;
            m_Bitmap->Height = 16;
            break;
        case itFont:
            m_Bitmap->Width = 8;
            m_Bitmap->Height = 8;
            break;
    }
    m_Width = m_Bitmap->Width;
    m_Height = m_Bitmap->Height;
    // convert the data string to a bitmap data list
    auto tokens = SplitString(data.Trim(), " ");
    for (auto token : tokens)
    {
        if (token.Trim() != "")
        {
            m_BitmapData.push_back(StrToInt(token.Trim()));
        }
    }
}
//---------------------------------------------------------------------------
__fastcall Image::~Image()
{
}
//---------------------------------------------------------------------------
int __fastcall Image::Draw(int x, int y, TBitmap* bitmap, int scalar, int frame)
{
    if (frame == -1 || m_Frames == 1)
    {
        int w = m_Bitmap->Width * scalar * ScalarX;
        ::StretchBlt(bitmap->Canvas->Handle, x, y, w, m_Bitmap->Height * scalar, m_Bitmap->Canvas->Handle, 0, 0, m_Bitmap->Width, m_Bitmap->Height, SRCCOPY);
        return w;
    }
    //
    int w = m_Width * scalar * ScalarX;
    auto f = frame % m_Frames;
    ::StretchBlt(bitmap->Canvas->Handle, x, y, w, m_Bitmap->Height * scalar, m_Bitmap->Canvas->Handle, m_Width * f, 0, m_Width, m_Bitmap->Height, SRCCOPY);
    return w;
}
//---------------------------------------------------------------------------
const TColor g_ColorsSpectrum[16] =
{
            clBlack   ,
    (TColor)0x00BF0000,
    (TColor)0x000000BF,
    (TColor)0x00BF00BF,
    (TColor)0x0000BF00,
    (TColor)0x00BFBF00,
    (TColor)0x0000BFBF,
    (TColor)0x00BFBFBF,
            clBlack   ,
            clBlue    ,
            clRed     ,
            clFuchsia ,
            clLime    ,
            clAqua    ,
            clYellow  ,
            clWhite
};
//---------------------------------------------------------------------------
void __fastcall Image::DrawSpectrum(const BitmapData& data, unsigned char attr, int offset)
{
    m_Palette = mpZXSpectrum;
    auto ink = (attr & 0x07) + ((attr & 0x40) ? 8 : 0);
    auto paper = ((attr & 0x38) >> 3) + ((attr & 0x40) ? 8 : 0);
    auto cw = m_Width / 8;
    for (auto y = 0; y < m_Bitmap->Height; y++)
    {
        for (auto x = 0; x < cw; x++)
        {
            auto pixels = data[y * cw + x];
            m_Bitmap->Canvas->Pixels[offset+(x*8)+0][y] = pixels & 0x80 ? g_ColorsSpectrum[ink] : g_ColorsSpectrum[paper];
            m_Bitmap->Canvas->Pixels[offset+(x*8)+1][y] = pixels & 0x40 ? g_ColorsSpectrum[ink] : g_ColorsSpectrum[paper];
            m_Bitmap->Canvas->Pixels[offset+(x*8)+2][y] = pixels & 0x20 ? g_ColorsSpectrum[ink] : g_ColorsSpectrum[paper];
            m_Bitmap->Canvas->Pixels[offset+(x*8)+3][y] = pixels & 0x10 ? g_ColorsSpectrum[ink] : g_ColorsSpectrum[paper];
            m_Bitmap->Canvas->Pixels[offset+(x*8)+4][y] = pixels & 0x08 ? g_ColorsSpectrum[ink] : g_ColorsSpectrum[paper];
            m_Bitmap->Canvas->Pixels[offset+(x*8)+5][y] = pixels & 0x04 ? g_ColorsSpectrum[ink] : g_ColorsSpectrum[paper];
            m_Bitmap->Canvas->Pixels[offset+(x*8)+6][y] = pixels & 0x02 ? g_ColorsSpectrum[ink] : g_ColorsSpectrum[paper];
            m_Bitmap->Canvas->Pixels[offset+(x*8)+7][y] = pixels & 0x01 ? g_ColorsSpectrum[ink] : g_ColorsSpectrum[paper];
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall Image::DrawSpectrumNext(const BitmapData& data, int offset)
{
    m_Palette = mpZXSpectrumNext;
    auto cw = m_Width / 8;
    for (auto y = 0; y < m_Bitmap->Height; y++)
    {
        for (auto x = 0; x < cw; x++)
        {
            auto pixels = data[y*2*cw+x+0];
            auto attr = data[y*2*cw+x+1];
            auto ink = (attr & 0x07) + ((attr & 0x40) ? 8 : 0);
            auto paper = ((attr & 0x38) >> 3) + ((attr & 0x40) ? 8 : 0);
            m_Bitmap->Canvas->Pixels[offset+(x*8)+0][y] = pixels & 0x80 ? g_ColorsSpectrum[ink] : g_ColorsSpectrum[paper];
            m_Bitmap->Canvas->Pixels[offset+(x*8)+1][y] = pixels & 0x40 ? g_ColorsSpectrum[ink] : g_ColorsSpectrum[paper];
            m_Bitmap->Canvas->Pixels[offset+(x*8)+2][y] = pixels & 0x20 ? g_ColorsSpectrum[ink] : g_ColorsSpectrum[paper];
            m_Bitmap->Canvas->Pixels[offset+(x*8)+3][y] = pixels & 0x10 ? g_ColorsSpectrum[ink] : g_ColorsSpectrum[paper];
            m_Bitmap->Canvas->Pixels[offset+(x*8)+4][y] = pixels & 0x08 ? g_ColorsSpectrum[ink] : g_ColorsSpectrum[paper];
            m_Bitmap->Canvas->Pixels[offset+(x*8)+5][y] = pixels & 0x04 ? g_ColorsSpectrum[ink] : g_ColorsSpectrum[paper];
            m_Bitmap->Canvas->Pixels[offset+(x*8)+6][y] = pixels & 0x02 ? g_ColorsSpectrum[ink] : g_ColorsSpectrum[paper];
            m_Bitmap->Canvas->Pixels[offset+(x*8)+7][y] = pixels & 0x01 ? g_ColorsSpectrum[ink] : g_ColorsSpectrum[paper];
        }
    }
}
//---------------------------------------------------------------------------
const TColor g_ColorsAmstrad[27] =
{
            clBlack   ,
            clNavy    ,
            clBlue    ,
            clMaroon  ,
            clPurple  ,
    (TColor)0x00FF0080,
            clRed     ,
    (TColor)0x008000FF,
            clFuchsia ,
            clGreen   ,
            clTeal    ,
    (TColor)0x00FF8000,
            clOlive   ,
            clGray    ,
    (TColor)0x00FF8080,
    (TColor)0x000080FF,
    (TColor)0x008080FF,
    (TColor)0x00FF80FF,
            clLime    ,
    (TColor)0x0080FF00,
            clAqua    ,
    (TColor)0x0000FF80,
    (TColor)0x0080FF80,
    (TColor)0x00FFFF80,
            clYellow  ,
    (TColor)0x0080FFFF,
            clWhite
};
//---------------------------------------------------------------------------
void __fastcall Image::DrawAmstradCPC(const BitmapData& data, int offset)
{
    m_Palette = mpAmstradCPC;
    m_ScalarX = 2;
    auto cw = m_Width / 2;
    for (auto y = 0; y < m_Bitmap->Height; y++)
    {
        for (auto x = 0; x < cw; x++)
        {
            auto pixels = data[y*cw+x];
            auto p0  = (pixels & 0x80) >> 7;
                 p0 |= (pixels & 0x20) >> 3;
                 p0 |= (pixels & 0x08) >> 2;
                 p0 |= (pixels & 0x02) << 2;
            auto p1  = (pixels & 0x40) >> 6;
                 p1 |= (pixels & 0x10) >> 2;
                 p1 |= (pixels & 0x04) >> 1;
                 p1 |= (pixels & 0x01) << 3;
            m_Bitmap->Canvas->Pixels[offset+(x*2)+0][y] = g_ColorsAmstrad[p0];
            m_Bitmap->Canvas->Pixels[offset+(x*2)+1][y] = g_ColorsAmstrad[p1];
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall Image::SetAttribute(unsigned char attr)
{
    if (m_Palette == mpZXSpectrum)
    {
        auto idx = 0;
        auto offset = 0;
        BitmapData bitmapData;
        auto size = m_BitmapData.size();
        auto frame_size = size / m_Frames;
        for (auto f = 0; f < m_Frames; f++)
        {
            bitmapData.clear();
            for (auto i = 0; i < frame_size; i++)
            {
                bitmapData.push_back(m_BitmapData[idx++]);
            }
            DrawSpectrum(bitmapData, attr, offset);
            offset += m_Width;
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall Image::ResetAttribute()
{
    if (m_Palette == mpZXSpectrum)
    {
        auto idx = 0;
        auto offset = 0;
        BitmapData bitmapData;
        auto size = m_BitmapData.size();
        auto frame_size = size / m_Frames;
        for (auto f = 0; f < m_Frames; f++)
        {
            bitmapData.clear();
            for (auto i = 0; i < frame_size; i++)
            {
                bitmapData.push_back(m_BitmapData[idx++]);
            }
            DrawSpectrum(bitmapData, m_Attribute, offset);
            offset += m_Width;
        }
    }
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
__fastcall ImageBlock::ImageBlock(const String& subType, const String& data)
: Image(itBlock, data)
{
    m_SubType = subType.UpperCase();
    switch (m_BitmapData.size())
    {
        case 9: // ZX Spectrum
        {
            m_Attribute = m_BitmapData[8];
            m_BitmapData.pop_back();
            DrawSpectrum(m_BitmapData, m_Attribute);
            break;
        }
        case 16: // ZX Spectrum Next/Timex
            DrawSpectrumNext(m_BitmapData);
            break;
        case 24: // Amstrad CPC
            m_Width = 6;
            m_Bitmap->Width = m_Width;
            DrawAmstradCPC(m_BitmapData);
            break;
    }
}
//---------------------------------------------------------------------------
__fastcall ImageBlock::~ImageBlock()
{
}
//---------------------------------------------------------------------------
__fastcall ImageObject::ImageObject(const String& data)
: Image(itObject, data)
, m_Room(-1)
, m_Position(0,0)
{
    auto size = m_BitmapData.size();
    m_Attribute = 0x47;
    switch (size)
    {
        case 36: // ZX Spectrum
            m_Attribute = m_BitmapData.front(); // White on Black
            m_BitmapData.erase(m_BitmapData.begin());
        case 35: // ZX Spectrum Next/Timex/Monochrome
            m_Room = m_BitmapData.front();
            m_BitmapData.erase(m_BitmapData.begin());
            m_Position.Y = m_BitmapData.front();
            m_BitmapData.erase(m_BitmapData.begin());
            m_Position.X = m_BitmapData.front();
            m_BitmapData.erase(m_BitmapData.begin());
            DrawSpectrum(m_BitmapData, m_Attribute);
            break;
        case 16: // ZX Spectrum Next/Timex
            DrawSpectrumNext(m_BitmapData);
            break;
        case 67: // Amstrad CPC
            m_Width = 8;
            m_Bitmap->Width = m_Width;
            DrawAmstradCPC(m_BitmapData);
            break;
    }
}
//---------------------------------------------------------------------------
__fastcall ImageObject::~ImageObject()
{

}
//---------------------------------------------------------------------------
__fastcall ImageSprite::ImageSprite(const String& data)
: Image(itSprite, data)
{
    m_Attribute = 0x47;
    m_Frames = m_BitmapData.front();
    m_BitmapData.erase(m_BitmapData.begin());
    m_Bitmap->Width = m_Bitmap->Width * m_Frames;
    auto size = m_BitmapData.size();
    auto frame_size = size / m_Frames;
    if (frame_size == 48)
    {
        m_Bitmap->Height = 24;
    }

    auto idx = 0;
    auto offset = 0;
    BitmapData bitmapData;
    for (auto f = 0; f < m_Frames; f++)
    {
        bitmapData.clear();
        for (auto i = 0; i < frame_size; i++)
        {
            bitmapData.push_back(m_BitmapData[idx++]);
        }
        switch (frame_size)
        {
            case 32:    // ZX Spectrum
            case 48:
                DrawSpectrum(bitmapData, m_Attribute, offset);
                break;
            case 80:
                m_Width = 10;
                m_Bitmap->Width = m_Width * m_Frames;
                DrawAmstradCPC(bitmapData, offset);
                break;

        }
        offset += m_Width;
    }
}
//---------------------------------------------------------------------------
__fastcall ImageSprite::~ImageSprite()
{
}
//---------------------------------------------------------------------------
__fastcall ImageFont::ImageFont(const String& data)
: Image(itFont, data)
{
    m_Frames = 1;
    m_Bitmap->Width = m_Bitmap->Width * 96;
    auto size = m_BitmapData.size();
    auto frame_size = size / 96;

    auto idx = 0;
    auto offset = 0;
    BitmapData bitmapData;
    for (auto f = 0; f < 96; f++)
    {
        bitmapData.clear();
        for (auto i = 0; i < frame_size; i++)
        {
            bitmapData.push_back(m_BitmapData[idx++]);
        }
        switch (frame_size)
        {
            case 8:    // ZX Spectrum
                DrawSpectrum(bitmapData, 0x47, offset);
                break;
            case 24:
                m_Width = 6;
                m_Bitmap->Width = (m_Width + 1) * m_Frames;
                DrawAmstradCPC(bitmapData, offset);
                break;

        }
        offset += m_Width;
    }
}
//---------------------------------------------------------------------------
__fastcall ImageFont::~ImageFont()
{
}
//---------------------------------------------------------------------------

