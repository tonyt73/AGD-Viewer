//---------------------------------------------------------------------------
#include "agdv.pch.h"
#include "Screen.h"
#include "ErrorReporter.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
__fastcall GameScreen::GameScreen(const String& data)
{
    try
    {
        // convert the SPRITEPOSITION commands
        auto ld = data.LowerCase();
        auto sp = ld.Pos("spriteposition");
        auto sprites = sp != 0 ? data.SubString(sp, data.Length()) : "";
        auto blocks = sp != 0 ? data.SubString(1, sp - 1) : data;
        auto spTokens = SplitString(sprites.Trim(), " ");
        String  prevToken = "";
        SpritePosition spritePos;
        int i = 0;
        for (auto token : spTokens)
        {
            token = token.Trim();
            if (token != "")
            {
                if (prevToken.LowerCase() == "spriteposition")
                {
                    i = 1;
                    spritePos.Type = StrToInt(token);
                }
                else if (i == 1)
                {
                    spritePos.Index = StrToInt(token);
                    if (spritePos.Index == 90)
                    {
                        spritePos.Index = 0;
                    }
                    i = 2;
                }
                else if (i == 2)
                {
                    spritePos.Position.Y = StrToInt(token);
                    i = 3;
                }
                else if (i == 3)
                {
                    spritePos.Position.X = StrToInt(token);
                    i = 0;
                    m_Sprites.push_back(spritePos);
                }
                prevToken = token;
            }
        }

        // Convert the block data
        auto tokens = SplitString(blocks.Trim(), " ");
        for (auto token : tokens)
        {
            if (token.Trim() != "")
            {
                m_Blocks.push_back(StrToInt(token.Trim()));
            }
        }
    }
    catch(...)
    {
        g_ErrorReporter.Add("Error: Exception caught while converting Scrren data. [" + data + "]");
        throw;
    }
}
//---------------------------------------------------------------------------
void __fastcall GameScreen::Draw(int room, int x, int y, TBitmap* bitmap, int scalar, const ImageList& blocks, const ImageList& objects, const ImageList& sprites, const Window& window)
{
    auto sx = x;
    auto sy = y;
    auto bx = 0;
    auto by = 0;
    auto wx = window.x * 8;
    auto wy = window.y * 8;
    auto bw = blocks[0]->Width * scalar;
    auto bh = blocks[0]->Height * scalar;
    for (const auto& block : m_Blocks)
    {
        blocks[block]->Draw(x, y, bitmap, scalar, 0);
        x += bw;
        if (++bx >= window.w)
        {
            x = sx;
            y += bh;
            bx = 0;
            if (++by == window.h)
            {
                break;
            }
        }
    }

    for (auto& sprite : m_Sprites)
    {
        if (sprite.Index >= sprites.size())
        {
            sprite.Index = 0;
        }
        if (sprite.Index < sprites.size())
        {
			sprites[sprite.Index]->Draw(sx + ((sprite.Position.X - wx) * scalar), sy + ((sprite.Position.Y - wy) * scalar), bitmap, scalar, 0);
        }
        else
        {
            g_ErrorReporter.Add("Error: Invalid Sprite index referenced in screen data. Room: " + IntToStr(room) + ", SpriteIndex: " + IntToStr(sprite.Index));
        }
    }

    for (const auto& object : objects)
    {
        auto image = dynamic_cast<ImageObject*>(object.get());
        if (image != nullptr && image->Room == room && image->Room != -1)
        {
			image->Draw(sx + ((image->Position.X - wx) * scalar), sy + ((image->Position.Y - wy) * scalar), bitmap, scalar, 0);
        }
    }
}
//---------------------------------------------------------------------------
