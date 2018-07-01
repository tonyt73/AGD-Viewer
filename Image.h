//---------------------------------------------------------------------------
#ifndef ImageH
#define ImageH
//---------------------------------------------------------------------------
enum ImageType { itBlock, itObject, itSprite, itFont };
enum MachinePalette { mpZXSpectrum, mpZXSpectrumNext, mpAmstradCPC };
typedef std::vector<unsigned char> BitmapData;
//---------------------------------------------------------------------------
class Image
{
private:
    ImageType               m_Type;
protected:

    String                  m_SubType;  // ie. block type
    MachinePalette          m_Palette;
   std::unique_ptr<TBitmap> m_Bitmap;
    unsigned char           m_Attribute;
    BitmapData              m_BitmapData;
    int                     m_Height;
    int                     m_Width;
    int                     m_Frames;
    int                     m_ScalarX;

    void        __fastcall  DrawSpectrum(const BitmapData& data, unsigned char attr, int offset = 0);
    void        __fastcall  DrawSpectrumNext(const BitmapData& data, int offset = 0);
    void        __fastcall  DrawAmstradCPC(const BitmapData& data, int offset = 0);

public:
                __fastcall  Image(ImageType type, const String& data);
    virtual     __fastcall ~Image();
                            // draw bitmap onto image - return width of image drawn adjusted for scaling
    int         __fastcall  Draw(int x, int y, TBitmap* bitmap, int scalar, int frame = -1);
   virtual void __fastcall  SetAttribute(unsigned char attr);
   virtual void __fastcall  ResetAttribute();
   virtual void __fastcall  Assign(TBitmap* bitmap);
           void __fastcall  GetBitmapData(BitmapData& data) const;

    ImageType   __property  Type = { read = m_Type };
    String      __property  SubType = { read = m_SubType };
MachinePalette  __property  Palette = { read = m_Palette };
  int           __property  Width = { read = m_Width };
  int           __property  Height = { read = m_Height };
  int           __property  Frames = { read = m_Frames };
  int           __property  ScalarX = { read = m_ScalarX };
  unsigned char __property  Attribute = { read = m_Attribute };
};
//---------------------------------------------------------------------------
typedef std::vector<std::unique_ptr<Image>> ImageList;
//---------------------------------------------------------------------------
class ImageBlock : public Image
{
private:

public:
        __fastcall  ImageBlock(const String& subType, const String& data);
        __fastcall ~ImageBlock();
};
//---------------------------------------------------------------------------
class ImageObject : public Image
{
private:
        int         m_Room;
        TPoint      m_Position;
public:
        __fastcall  ImageObject(const String& data);
        __fastcall ~ImageObject();

    int __property  Room = { read = m_Room };
 TPoint __property  Position = { read = m_Position };
};
//---------------------------------------------------------------------------
class ImageSprite : public Image
{
public:
        __fastcall  ImageSprite(const String& data);
        __fastcall ~ImageSprite();
};
//---------------------------------------------------------------------------
class ImageFont : public Image
{
private:
    void __fastcall DrawCharacterSet(unsigned char attr);
    void __fastcall SetAttribute(unsigned char attr);
    void __fastcall ResetAttribute();

public:
        __fastcall  ImageFont(const String& data);
        __fastcall  ImageFont(TBitmap* bitmap);
        __fastcall ~ImageFont();

    int __fastcall  DrawChr(int x, int y, TBitmap* bitmap, int scalar, int character);
};
//---------------------------------------------------------------------------
#endif
