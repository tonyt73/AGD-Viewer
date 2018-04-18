//---------------------------------------------------------------------------
#ifndef MapH
#define MapH
//---------------------------------------------------------------------------
#include "Window.h"
#include "Image.h"
#include "Screen.h"
//---------------------------------------------------------------------------
typedef std::vector<unsigned char> MapData;
//---------------------------------------------------------------------------
class Map
{
private:
    int                 m_Width;
    int                 m_StartScreen;
    MapData             m_MapData;

public:
            __fastcall  Map(const String& data);

    void    __fastcall  Draw(TBitmap* bitmap, int scalar, const Window& window, const ImageList& blocks, const ImageList& objects, const ImageList& sprites, const ScreenList& screens);

    int     __property  Width = { read = m_Width };
    int     __property  StartScreen = { read = m_StartScreen };
};
//---------------------------------------------------------------------------
#endif
