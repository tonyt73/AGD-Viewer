//---------------------------------------------------------------------------
#include "agdv.pch.h"
#include <string.h>
#include "fMain.h"
#include <System.IOUtils.hpp>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmMain *frmMain;
//---------------------------------------------------------------------------
__fastcall TfrmMain::TfrmMain(TComponent* Owner)
: TForm(Owner)
, m_Scale(2.f)
, m_MonochromeAttribute(0x47)
, m_Map(nullptr)
{
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::FormResize(TObject *Sender)
{
    imgView->Picture->Bitmap->Width = imgView->Width;
    imgView->Picture->Bitmap->Height = imgView->Height;
    RefreshImagesView();
    RefreshMapView();
    imgMap->Width = imgMap->Picture->Bitmap->Width;
    imgMap->Height = imgMap->Picture->Bitmap->Height;
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::FormCreate(TObject *Sender)
{
    imgView->Picture->Bitmap->Canvas->Brush->Color = clBlack;
    imgView->Picture->Bitmap->Width = imgView->Width;
    imgView->Picture->Bitmap->Height = imgView->Height;

    imgView->Picture->Bitmap->Canvas->Brush->Style = bsClear; // Set the brush style to transparent.
    imgView->Picture->Bitmap->Canvas->Font->Color = clWhite;
    imgView->Picture->Bitmap->Canvas->Font->Name = "Courier New";
    imgView->Picture->Bitmap->Canvas->Font->Height = 24;
    imgView->Picture->Bitmap->Canvas->Font->Style <<= fsBold;

    // check command line parameters
    OpenFile(ParamStr(1));

    DragAcceptFiles(Handle, true);
    Application->OnActivate = FormActivate;
    Application->OnDeactivate = FormDeactivate;
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::FormDestroy(TObject *Sender)
{
    DragAcceptFiles(Handle, false);
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::actFileOpenAccept(TObject *Sender)
{
    if (System::Ioutils::TFile::Exists(actFileOpen->Dialog->FileName))
    {
        OpenFile(actFileOpen->Dialog->FileName);
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::OpenFile(const String& file)
{
    if (TFile::Exists(file))
    {
        m_File = file;
        Caption = "AGDv - " + file;
        m_Blocks.clear();
        m_Objects.clear();
        m_Sprites.clear();
        m_Font.clear();
        m_Screens.clear();
        m_Messages.clear();
        m_Map = nullptr;
        // Load the file
        auto lines = System::Ioutils::TFile::ReadAllLines(file);
        String data = "";
        for (auto line : lines)
        {
            // find image marker
            auto tline = String(line).Trim().LowerCase();
            // remove comments
            auto cPos = tline.Pos(';');
            if (cPos > 0)
            {
                tline = tline.SubString(1, cPos - 1);
            }
            if (tline.Length() > 0)
            {
                // process the line
                if (tline.Pos("define") > 0 || tline.Pos("map") > 0)
                {
                    if (tline.Pos("endmap") == 1)
                    {
                        ConvertMap(data);
                        data = "";
                    }
                    else if (data.Length() > 0)
                    {
                        // convert data
                        Convert(data);
                    }
                    // defining something
                    data = tline;
                }
                else
                {
                    data += " " + tline;
                }
            }
        }
        RefreshImagesView();
        RefreshMapView();
    }
}
//---------------------------------------------------------------------------
String __fastcall TfrmMain::PreProcess(const String& data) const
{
    auto sPos = data.Pos(' ');
    auto tPos = data.Pos('\t');
    sPos = sPos != 0 && tPos != 0 ? std::min(sPos, tPos) : (sPos == 0 ? tPos : sPos);
    // remove the define/map/endmap statement
    auto line = data.SubString(sPos, data.Length()).Trim();
    // replace all newlines + tabs with space
    int hPos, rPos, nPos;
    do
    {
        rPos = line.Pos("\r");
        if (rPos > 0)
        {
            line = line.SubString(1, rPos - 1) + + " " + line.SubString(rPos + 1, line.Length());
        }
        nPos = line.Pos("\n");
        if (nPos > 0)
        {
            line = line.SubString(1, nPos - 1) + + " " + line.SubString(nPos + 1, line.Length());
        }
        tPos = line.Pos("\t");
        if (tPos > 0)
        {
            line = line.SubString(1, tPos - 1) + " " + line.SubString(tPos + 1, line.Length());
        }

    }
    while (rPos != 0 || nPos != 0 || tPos != 0);
    // change ',' to ' '
    for (auto i = 1; i <= line.Length(); i++)
    {
         if (line[i] == ',')
         {
            line[i] = ' ';
         }
    }
    // all data should now be on 1 line

    // convert hex to decimal
    do
    {
        hPos = line.Pos("$");
        if (hPos > 0)
        {
            auto l1 = line.SubString(1, hPos - 1).Trim();
            auto l2 = line.SubString(hPos + 1, line.Length());
            auto l3 = String();
            sPos = l2.Pos(' ');
            if (sPos > 0)
            {
                l3 = l2.SubString(sPos, l2.Length()).Trim();
                l2 = l2.SubString(1, sPos).Trim();
            }
            auto value = StrToInt("0x" + l2);
            line = l1 + " " + IntToStr(value) + " " + l3;
        }
    }
    while (hPos != 0);
    return line.Trim();
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::Convert(const String& data)
{
    if (data.Pos("definemessages") == 0)
    {
        auto pdata = PreProcess(data);
        if (data.Pos("defineblock") == 1)
        {
            ConvertBlock(pdata);
        }
        else if (data.Pos("defineobject") == 1)
        {
            ConvertObject(pdata);
        }
        else if (data.Pos("definesprite") == 1)
        {
            ConvertSprite(pdata);
        }
        else if (data.Pos("definefont") == 1)
        {
            ConvertFont(pdata);
        }
        else if (data.Pos("definewindow") == 1)
        {
            ConvertWindow(pdata);
        }
        else if (data.Pos("definescreen") == 1)
        {
            ConvertScreen(pdata);
        }
    }
    else
    {
        ConvertMessages(data);
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::ConvertScreen(const String& data)
{
    // convert the data
    m_Screens.push_back(std::move(std::make_unique<GameScreen>(data)));
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::ConvertMap(const String& data)
{
    auto pdata = PreProcess(data);
    m_Map = std::make_unique<Map>(pdata);
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::ConvertWindow(const String& data)
{
    auto tokens = SplitString(data, " ");
    auto i = 0;
    for (auto token : tokens)
    {
        switch(i++)
        {
            case 0: m_Window.y = StrToInt(token); break;
            case 1: m_Window.x = StrToInt(token); break;
            case 2: m_Window.h = StrToInt(token); break;
            case 3: m_Window.w = StrToInt(token); break;
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::ConvertBlock(const String& data)
{
    // convert the data
    // get the type
    auto tPos = data.Pos(" ");
    auto subType = data.SubString(1, tPos - 1);
    auto idata = data.SubString(tPos + 1, data.Length());
    m_Blocks.push_back(std::move(std::make_unique<ImageBlock>(subType, idata)));
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::ConvertObject(const String& data)
{
    // convert the data
    m_Objects.push_back(std::move(std::make_unique<ImageObject>(data)));
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::ConvertSprite(const String& data)
{
    // convert the data
    m_Sprites.push_back(std::move(std::make_unique<ImageSprite>(data)));
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::ConvertFont(const String& data)
{
    // convert the data
    m_Font.push_back(std::move(std::make_unique<ImageFont>(data)));
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::ConvertMessages(const String& data)
{
    // convert the data
    auto mPos = data.Pos(" ");
    auto mdata = data.SubString(mPos + 1, data.Length());
    auto tokens = SplitString(mdata, "\"");
    for (auto token : tokens)
    {
        token = token.Trim();
        if (token != "")
        {
            m_Messages.push_back(token);
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::actZoomInExecute(TObject *Sender)
{
    m_Scale = std::min(16.f, m_Scale + 1.f);
    RefreshImagesView();
    RefreshMapView();
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::actZoomOutExecute(TObject *Sender)
{
    m_Scale = std::max(1.f, m_Scale - 1.f);
    RefreshImagesView();
    RefreshMapView();
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::actZoomActualSizeExecute(TObject *Sender)
{
    m_Scale = 1;
    RefreshImagesView();
    RefreshMapView();
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::actZoomToFitExecute(TObject *Sender)
{
    //
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::DisplayImages(const String& title, const ImageList& images, int& y, int scale, int index)
{
    auto my = 0;
    auto x = 8;
    imgView->Picture->Bitmap->Canvas->Pen->Color = clWhite;
    imgView->Picture->Bitmap->Canvas->TextOut(8, y, title);
    y += imgView->Picture->Bitmap->Canvas->TextHeight(title) + 8;
    m_ImageY[index] = y;
    for (const auto& image : images)
    {
        my = std::max(my, image->Height * scale);
        int w = image->Width * image->Frames * image->ScalarX * scale;
        if (x + w > imgView->Width - 8)
        {
            x = 8;
            y += image->Height * scale + 8;
        }
        x += image->Draw(x, y, imgView->Picture->Bitmap, scale, Timer1->Enabled ? m_Animate : -1) + 8;
    }
    y += my + 32;
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::DisplayMessasges(const String& title, int& y, int scale)
{
    if (m_Font.size() == 0)
    {
        // initialise the ZX Spectrum font
        auto bmp = std::make_unique<TBitmap>();
        bmp->Assign(imgZXFont->Picture->Graphic);
        m_Font.push_back(std::move(std::make_unique<ImageFont>(bmp.get())));
    }
    y -= 16;
    imgView->Picture->Bitmap->Canvas->Pen->Color = clWhite;
    imgView->Picture->Bitmap->Canvas->TextOut(8, y, title);
    y += imgView->Picture->Bitmap->Canvas->TextHeight(title) + 8;
    const auto font = dynamic_cast<ImageFont*>(m_Font[0].get());
    for (const auto& message : m_Messages)
    {
        auto x = 32;
        for (const auto& chr : message)
        {
            x += font->DrawChr(x, y, imgView->Picture->Bitmap, scale, chr - 32);
        }
        y += font->Height * scale;
    }
    y += 16;
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::RefreshImagesView()
{
    // clear the viewer
    BitBlt(imgView->Picture->Bitmap->Canvas->Handle, 0, 0, imgView->Picture->Bitmap->Width, imgView->Picture->Bitmap->Height, NULL, 0, 0, BLACKNESS);
    // display all the images types
    auto y = 8;
    DisplayImages("FONT", m_Font, y, m_Scale, 0);
    DisplayMessasges("MESSAGES", y, m_Scale);
    DisplayImages("BLOCKS", m_Blocks, y, m_Scale, 1);
    DisplayImages("OBJECTS", m_Objects, y, m_Scale, 2);
    DisplayImages("SPRITES", m_Sprites, y, m_Scale, 3);
    // update the statistics
    barStatus->Panels->Items[0]->Text = "Font: " + IntToStr((int)m_Font.size());
    barStatus->Panels->Items[1]->Text = "Messages: " + IntToStr((int)m_Messages.size());
    barStatus->Panels->Items[2]->Text = "Blocks: " + IntToStr((int)m_Blocks.size());
    barStatus->Panels->Items[3]->Text = "Objects: " + IntToStr((int)m_Objects.size());
    barStatus->Panels->Items[4]->Text = "Sprites: " + IntToStr((int)m_Sprites.size());
    // refresh the image
    if (y + 32 != imgView->Height)
    {
        imgView->Height = y + 32;
        imgView->Picture->Bitmap->Height = imgView->Height;
        RefreshImagesView();
    }
    if (m_ImageCursorPos.Y != 0)
    {
        // draw the image cursor
        imgView->Picture->Bitmap->Canvas->Pen->Color = clFuchsia;
        imgView->Picture->Bitmap->Canvas->Rectangle(m_ImageCursorPos.X, m_ImageCursorPos.Y, m_ImageCursorPos.X + m_ImageCursorSize.cx, m_ImageCursorPos.Y + m_ImageCursorSize.cy);
        imgView->Picture->Bitmap->Canvas->Pen->Color = clBlack;
    }
    imgView->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::RefreshMapView()
{
    if (m_Map)
    {
        m_Map->Draw(imgMap->Picture->Bitmap, (int)m_Scale, m_Window, m_Blocks, m_Objects, m_Sprites, m_Screens);
        imgMap->Width = imgMap->Picture->Bitmap->Width;
        imgMap->Height = imgMap->Picture->Bitmap->Height;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::actPlayExecute(TObject *Sender)
{
    Timer1->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::actStopExecute(TObject *Sender)
{
    Timer1->Enabled = false;
    RefreshImagesView();
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::Timer1Timer(TObject *Sender)
{
    // animate
    m_Animate++;
    RefreshImagesView();
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::sbxViewMouseWheel(TObject *Sender, TShiftState Shift, int WheelDelta, TPoint &MousePos, bool &Handled)
{
    if (Shift.Contains(ssCtrl))
    {
        Handled = true;
        float delta = WheelDelta / 200.f;
        m_Scale = std::max(1.0f, std::min(16.f, m_Scale + delta));
        RefreshImagesView();
        RefreshMapView();
    }
    else
    {
        Handled = true;
        float delta = -WheelDelta / (Shift.Contains(ssShift) ? 5.f : 20.f);
        sbxView->VertScrollBar->Position += delta;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::actToggleMonochromeExecute(TObject *Sender)
{
    if (btnToggle->Down)
    {
        for (const auto& image : m_Font)
            image->SetAttribute(0x44);
        for (const auto& image : m_Blocks)
            image->SetAttribute(0x44);
        for (const auto& image : m_Objects)
            image->SetAttribute(0x44);
        for (const auto& image : m_Sprites)
            image->SetAttribute(0x44);
    }
    else
    {
        for (const auto& image : m_Font)
            image->ResetAttribute();
        for (const auto& image : m_Blocks)
            image->ResetAttribute();
        for (const auto& image : m_Objects)
            image->ResetAttribute();
        for (const auto& image : m_Sprites)
            image->ResetAttribute();
    }
    RefreshImagesView();
    RefreshMapView();
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::imgViewMouseMove(TObject *Sender, TShiftState Shift, int X, int Y)
{
    m_Cursor = m_ImageCursorPos;
    m_ImageCursorPos.Y = 0;
    // convert the cursor to an image
    int scale = m_Scale;
    if (m_Font.size() > 0 && m_ImageY[0] <= Y && Y < m_ImageY[0] + (m_Font[0]->Height * scale))
    {
        auto w = m_Font[0]->Width;
        // could be a font
        X = ((X - 8) / scale) / w;
        if (0 <= X && X < 96)
        {
            auto chr = System::UnicodeString::UnicodeString().StringOfChar(' ' + X,1);
            if (X == 95)
            {
                chr = "©";
            }
            barStatus->Panels->Items[4]->Text = "Character: " + chr + " [0x" + IntToHex(32 + X, 2) + "]";
            // select a character
            m_ImageCursorPos.X = (8 + (X * w * scale)) - 1;
            m_ImageCursorPos.Y = (m_ImageY[0]) - 1;
            m_ImageCursorSize.cx = (8 * scale) + 2;
            m_ImageCursorSize.cy = (8 * scale) + 2;
        }
    }
    else if (m_Blocks.size() > 0 && m_ImageY[1] <= Y && Y < m_ImageY[2])
    {
        // could be a block
        auto w = m_Blocks[0]->Width;
        auto h = m_Blocks[0]->Height;
        int sw = (imgView->Width - 8) / ((scale * w) + 8);
        X = (X - 8) / ((scale * w) + 8);
        int row = (Y - m_ImageY[1]) / ((h * scale) + 8);
        int col = X;
        auto blk = row * sw + col;
        if (blk < m_Blocks.size())
        {
            // is a block
            barStatus->Panels->Items[4]->Text = "Block: " + IntToStr(blk) + " - " + m_Blocks[blk]->SubType;
            // select it
            m_ImageCursorPos.X = (8 + (X * ((w * scale) + 8))) - 2;
            m_ImageCursorPos.Y = (m_ImageY[1] + (row * (h * scale + 8))) - 2;
            m_ImageCursorSize.cx = (w * scale) + 4;
            m_ImageCursorSize.cy = (h * scale) + 4;
        }
    }
    else if (m_Objects.size() > 0 && m_ImageY[2] <= Y && Y < m_ImageY[3])
    {
        // could be an object
        auto w = m_Objects[0]->Width;
        auto h = m_Objects[0]->Height;
        int sw = (imgView->Width - 8) / ((scale * w) + 8);
        X = (X - 8) / ((scale * w) + 8);
        int row = (Y - m_ImageY[2]) / ((h * scale) + 8);
        int col = X;
        auto blk = row * sw + col;
        if (blk < m_Objects.size())
        {
            // is a block
            barStatus->Panels->Items[4]->Text = "Object: " + IntToStr(blk);
            // select it
            m_ImageCursorPos.X = (8 + (X * ((w * scale) + 8))) - 2;
            m_ImageCursorPos.Y = (m_ImageY[2] + (row * (h * scale + 8))) - 2;
            m_ImageCursorSize.cx = (w * scale) + 4;
            m_ImageCursorSize.cy = (h * scale) + 4;
        }
    }
    else if (m_Sprites.size() > 0 && m_ImageY[3] <= Y && Y < imgView->Height)
    {
        // could be a sprite
        auto w = m_Sprites[0]->Width;
        auto h = m_Sprites[0]->Height;
        if (Timer1->Enabled)
        {
            int sw = (imgView->Width - 8) / ((scale * w) + 8);
            X = (X - 8) / ((scale * w) + 8);
            int row = (Y - m_ImageY[3]) / ((h * scale) + 8);
            int col = X;
            auto blk = row * sw + col;
            // sprites are animating
            if (blk < m_Sprites.size())
            {
                // is a block
                barStatus->Panels->Items[4]->Text = "Sprite: " + IntToStr(blk);
                // select it
                m_ImageCursorPos.X = (8 + (X * ((w * scale) + 8))) - 2;
                m_ImageCursorPos.Y = (m_ImageY[3] + (row * (h * scale + 8))) - 2;
                m_ImageCursorSize.cx = (w * scale) + 4;
                m_ImageCursorSize.cy = (h * scale) + 4;
            }
        }
        else
        {
            // sprites are grouped into complete frame sets
            auto spriteIndex = 0;
            auto cx = 8;
            auto cy = m_ImageY[3];
            for (const auto& sprite : m_Sprites)
            {
                auto w = sprite->Width * scale * sprite->Frames;
                auto h = sprite->Height * scale;
                if (cx + w > imgView->Width)
                {
                    cx = 8;
                    cy += h + 8;
                }
                if (cx <= X && X < cx + w && cy <= Y && Y < cy + h)
                {
                    // it's this sprite
                    barStatus->Panels->Items[4]->Text = "Sprite: " + IntToStr(spriteIndex);
                    // select it
                    m_ImageCursorPos.X = cx - 2;
                    m_ImageCursorPos.Y = cy - 2;
                    m_ImageCursorSize.cx = w + 4;
                    m_ImageCursorSize.cy = h + 4;
                    break;
                }
                spriteIndex++;
                cx += w + 8;
            }
        }
    }
    if (m_Cursor.X != m_ImageCursorPos.X || m_Cursor.Y != m_ImageCursorPos.Y)
    {
        RefreshImagesView();
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::WMDropFiles(TWMDropFiles &message)
{
    TCHAR filename[MAX_PATH];
    HDROP hDrop = (HDROP)message.Drop;
    int numFiles = DragQueryFile(hDrop, -1, NULL, NULL);
    if (numFiles > 0)
    {
        DragQueryFile(hDrop, 0, filename, sizeof(filename));
        // process the file in 'buff'
        OpenFile(String(filename));
    }
    DragFinish(hDrop);
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::FormActivate(TObject *Sender)
{
    Timer2->Enabled = false;
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::Timer2Timer(TObject *Sender)
{
    OpenFile(m_File);
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::FormDeactivate(TObject *Sender)
{
    Timer2->Enabled = true;
}
//---------------------------------------------------------------------------

