//---------------------------------------------------------------------------
#include "agdv.pch.h"
#include "Screen.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
__fastcall GameScreen::GameScreen(const String& data)
{
    // convert the SPRITEPOSITION commands
    auto sp = data.Pos("spriteposition");
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
            if (prevToken == "spriteposition")
            {
                i = 1;
                spritePos.Type = StrToInt(token);
            }
            else if (i == 1)
            {
                spritePos.Index = StrToInt(token);
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
//---------------------------------------------------------------------------
void __fastcall GameScreen::Draw(int room, int x, int y, TBitmap* bitmap, int scalar, const ImageList& blocks, const ImageList& objects, const ImageList& sprites, const Window& window)
{
    auto sx = x;
    auto sy = y;
    auto bx = 0;
    auto by = 0;
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

    for (const auto& sprite : m_Sprites)
    {
        sprites[sprite.Index]->Draw(sx + (sprite.Position.X * scalar), sy + (sprite.Position.Y * scalar), bitmap, scalar, 0);
    }

    for (const auto& object : objects)
    {
        auto image = dynamic_cast<ImageObject*>(object.get());
        if (image != nullptr && image->Room == room && image->Room != -1)
        {
            image->Draw(sx + (image->Position.X * scalar), sy + (image->Position.Y * scalar), bitmap, scalar, 0);
        }
    }
}
//---------------------------------------------------------------------------