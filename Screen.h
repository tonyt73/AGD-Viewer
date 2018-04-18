//---------------------------------------------------------------------------
#ifndef ScreenH
#define ScreenH
//---------------------------------------------------------------------------
#include "Image.h"
#include "Window.h"
//---------------------------------------------------------------------------
typedef std::vector<unsigned char> ScreenData;
typedef struct {
    unsigned char   Type;
    unsigned char   Index;
    TPoint          Position;
} SpritePosition;
typedef std::vector<SpritePosition> Sprites;
//---------------------------------------------------------------------------
class GameScreen
{
private:
    ScreenData          m_Blocks;
    Sprites             m_Sprites;

public:
            __fastcall  GameScreen(const String& data);
    void    __fastcall  Draw(int room, int x, int y, TBitmap* bitmap, int scalar, const ImageList& blocks, const ImageList& objects, const ImageList& sprites, const Window& window);
};
//---------------------------------------------------------------------------
typedef std::vector<std::unique_ptr<GameScreen>> ScreenList;
//---------------------------------------------------------------------------
#endif
