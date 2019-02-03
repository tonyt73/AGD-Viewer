//---------------------------------------------------------------------------
#ifndef MapH
#define MapH
//---------------------------------------------------------------------------
#include "Window.h"
#include "Image.h"
#include "Screen.h"
//---------------------------------------------------------------------------
//typedef std::vector<unsigned char> MapData;
//---------------------------------------------------------------------------
class Map
{
private:
    int                 m_Width;
    int                 m_Height;
    int                 m_StartScreen;
    TRect               m_Rect;

public:
//    MapData             m_MapData;
    int                 m_MapData[16][16];

            __fastcall  Map(const String& data);

    void    __fastcall  Draw(TBitmap* bitmap, int scalar, const Window& window, const ImageList& blocks, const ImageList& objects, const ImageList& sprites, const ScreenList& screens);
    TPoint  __fastcall  GetRoomPt(int room);
    int     __property  Width = { read = m_Width };
    int     __property  Height = { read = m_Height };
    int     __property  StartScreen = { read = m_StartScreen };
    TRect   __property  Rect = { read = m_Rect };
};
//---------------------------------------------------------------------------
#endif
