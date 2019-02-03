//---------------------------------------------------------------------------
#include "agdv.pch.h"
#include "Map.h"
#include "ErrorReporter.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
__fastcall Map::Map(const String& data)
{
    for (auto y = 0; y < 16; y++)
        for (auto x = 0; x < 16; x++)
            m_MapData[x][y] = 255;
    try
    {
        auto tokens = SplitString(data.Trim(), " ");
        String prevToken;
        auto storeScreens = false;
        auto x = 0;
        auto y = 0;
        auto width = 0;
        auto size = 0;
        for (auto token : tokens)
        {
            if (token == "left" || token == "right" || token == "top" || token == "bottom")
            {
                storeScreens = false;
            }
            else if (prevToken.LowerCase() == "startscreen")
            {
                m_StartScreen = StrToInt(token.Trim());
                storeScreens = true;
                g_ErrorReporter.Add("Info: Map - Start Screen: " + IntToStr(m_StartScreen));
            }
            else if (storeScreens && token.Trim() != "")
            {
                m_MapData[x][y] = StrToInt(token.Trim());
                size += m_MapData[x][y] == 255 ? 0 : 1;
                if (++x == 11)
                {
                    x = 0;
                    y++;
                }
            }
            else if (prevToken.LowerCase() == "left")
            {
                m_Rect.Left = StrToInt(token);
            }
            else if (prevToken.LowerCase() == "right")
            {
                m_Rect.Right = StrToInt(token);
            }
            else if (prevToken.LowerCase() == "top")
            {
                m_Rect.Top = StrToInt(token);
            }
            else if (prevToken.LowerCase() == "bottom")
            {
                m_Rect.Bottom = StrToInt(token);
            }
            prevToken = token.Trim();
        }
        m_Width = 16;
        m_Height = 16;
        g_ErrorReporter.Add("Info: Map - Screens: " + IntToStr(size) + ", " + IntToStr((int)m_Rect.Width()) + "x" + IntToStr((int)m_Rect.Height()));
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
        bitmap->Height = sh *  m_Height;
        // clear the map
        BitBlt(bitmap->Canvas->Handle, 0, 0, bitmap->Width, bitmap->Height, NULL, 0, 0, BLACKNESS);
        auto x = 0;
        auto y = 0;
        //for (const auto& screen : m_MapData)
        for (auto y = 0; y < 16; y++)
        {
            for (auto x = 0; x < 16; x++)
            {
                auto screen = m_MapData[x][y];
                if (screen != 255)
                {
                    screens[screen]->Draw(screen, x, y, bitmap, scalar, blocks, objects, sprites, window);
                }
            }
        }
    }
}
// ---------------------------------------------------------------------------
TPoint __fastcall Map::GetRoomPt(int room)
{
    TPoint pt(0,0);
    for (auto y = 0; y < 16; y++)
    {
        for (auto x = 0; x < 16; x++)
        {
            if (m_MapData[x][y] == room)
            {
                pt.X = x;
                pt.Y = y;
                break;
            }
        }
    }
    return pt;
}
// ---------------------------------------------------------------------------
