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
            if (prevToken.LowerCase() == "width")
            {
                m_Width = StrToInt(token.Trim());
                g_ErrorReporter.Add("Info: Map - Width: " + IntToStr(m_Width));
            }
            else if (prevToken.LowerCase() == "startscreen")
            {
                m_StartScreen = StrToInt(token.Trim());
                storeScreens = true;
                g_ErrorReporter.Add("Info: Map - Start Screen: " + IntToStr(m_StartScreen));
            }
            else if (storeScreens && token.Trim() != "")
            {
                m_MapData.push_back(StrToInt(token.Trim()));
            }
            prevToken = token.Trim();
        }
        int size = m_MapData.size();
        m_Height = size / m_Width;
        g_ErrorReporter.Add("Info: Map - Screens: " + IntToStr(size) + ", " + IntToStr(m_Width) + "x" + IntToStr(m_Height));
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
    if (blocks.size() > 0)
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
        for (const auto& screen : m_MapData)
        {
            if (screen != 255)
            {
                screens[screen]->Draw(screen, x, y, bitmap, scalar, blocks, objects, sprites, window);
            }
            x += sw;
            if (++w == m_Width)
            {
                w = 0;
                x = 0;
                y += sh;
            }
        }
    }
}
//---------------------------------------------------------------------------
