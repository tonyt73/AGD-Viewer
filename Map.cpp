//---------------------------------------------------------------------------
#include "agdv.pch.h"
#include "Map.h"
#include "ErrorReporter.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
__fastcall Map::Map(const String& data)
{
    try
    {
        auto tokens = SplitString(data.Trim(), " ");
        String prevToken;
        auto storeScreens = false;
        for (auto token : tokens)
        {
            if (prevToken == "width")
            {
                m_Width = StrToInt(token.Trim());
            }
            else if (prevToken == "startscreen")
            {
                m_StartScreen = StrToInt(token.Trim());
                storeScreens = true;
            }
            else if (storeScreens && token.Trim() != "")
            {
                m_MapData.push_back(StrToInt(token.Trim()));
            }
            prevToken = token.Trim();
        }
    }
    catch(...)
    {
        g_ErrorReporter.Add("Error: Exception caught while converting Map data: [" + data + "]");
        throw;
    }
}
//---------------------------------------------------------------------------
void __fastcall Map::Draw(TBitmap* bitmap, int scalar, const Window& window, const ImageList& blocks, const ImageList& objects, const ImageList& sprites, const ScreenList& screens)
{
    auto sw = window.w * blocks[0]->Width * scalar;
    auto sh = window.h * blocks[0]->Height * scalar;
    bitmap->Width = sw * m_Width;
    bitmap->Height = sh * (m_MapData.size() / m_Width);
    // clear the map
    BitBlt(bitmap->Canvas->Handle, 0, 0, bitmap->Width, bitmap->Height, NULL, 0, 0, BLACKNESS);
    auto x = 0;
    auto y = 0;
    auto w = 0;
    auto room = 0;
    for (const auto& screen : m_MapData)
    {
        if (screen != 255)
        {
            screens[screen]->Draw(room, x, y, bitmap, scalar, blocks, objects, sprites, window);
        }
        room++;
        x += sw;
        if (++w == m_Width)
        {
            x = 0;
            y += sh;
        }
    }
}
//---------------------------------------------------------------------------
