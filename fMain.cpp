//---------------------------------------------------------------------------
#include "agdv.pch.h"
#include <System.IOUtils.hpp>
#include <System.Character.hpp>
#include <System.StrUtils.hpp>
#include <string.h>
#include "fMain.h"
#include "ErrorReporter.h"
#include "Importer.h"
#include "Project.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmMain * frmMain;
//---------------------------------------------------------------------------
__fastcall TfrmMain::TfrmMain(TComponent * Owner)
: TForm(Owner)
, m_Scale(2.f)
, m_MonochromeAttribute(0x47)
, m_Map(nullptr)
, m_MapIsPanning(false)
{
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::FormCreate(TObject * Sender)
{
    g_ErrorReporter.Memo                           = memReport;
    imgView->Picture->Bitmap->Canvas->Brush->Color = clBlack;
    imgView->Picture->Bitmap->Width                = imgView->Width;
    imgView->Picture->Bitmap->Height               = imgView->Height;
    imgView->Picture->Bitmap->Canvas->Brush->Style = bsClear;
    // Set the brush style to transparent.
    imgView->Picture->Bitmap->Canvas->Font->Color  = clWhite;
    imgView->Picture->Bitmap->Canvas->Font->Name   = "Courier New";
    imgView->Picture->Bitmap->Canvas->Font->Height = 24;
    imgView->Picture->Bitmap->Canvas->Font->Style <<= fsBold;
    // check command line parameters
    OpenFile(ParamStr(1));
    DragAcceptFiles(Handle, true);
    Application->OnActivate   = FormActivate;
    Application->OnDeactivate = FormDeactivate;
    pgcViews->ActivePage      = tabImages;
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::FormDestroy(TObject * Sender)
{
    DragAcceptFiles(Handle, false);
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::FormActivate(TObject * Sender)
{
    Timer2->Enabled = false;
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::FormDeactivate(TObject * Sender)
{
    Timer2->Enabled = m_File.Trim() != "";
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::FormResize(TObject * Sender)
{
    imgView->Picture->Bitmap->Width  = imgView->Width;
    imgView->Picture->Bitmap->Height = imgView->Height;
    RefreshImagesView();
    RefreshMapView();
    imgMap->Width  = imgMap->Picture->Bitmap->Width;
    imgMap->Height = imgMap->Picture->Bitmap->Height;
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::Timer1Timer(TObject * Sender)
{
    // animate
    m_Animate++ ;
    RefreshImagesView();
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::Timer2Timer(TObject * Sender)
{
    OpenFile(m_File);
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::actFileOpenAccept(TObject * Sender)
{
    OpenFile(actFileOpen->Dialog->FileName);
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::actZoomInExecute(TObject * Sender)
{
    m_Scale = std::min(16.f, m_Scale + 1.f);
    RefreshImagesView();
    RefreshMapView();
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::actZoomOutExecute(TObject * Sender)
{
    m_Scale = std::max(1.f, m_Scale - 1.f);
    RefreshImagesView();
    RefreshMapView();
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::actZoomActualSizeExecute(TObject * Sender)
{
    m_Scale = 1;
    RefreshImagesView();
    RefreshMapView();
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::actZoomToFitExecute(TObject * Sender)
{
    //
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::actPlayExecute(TObject * Sender)
{
    Timer1->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::actStopExecute(TObject * Sender)
{
    Timer1->Enabled = false;
    RefreshImagesView();
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::actToggleMonochromeExecute(TObject * Sender)
{
    if (btnToggle->Down)
    {
        for (const auto &image : m_Font)
        {
            image->SetAttribute(0x44);
        }
        for (const auto &image : m_Blocks)
        {
            image->SetAttribute(0x44);
        }
        for (const auto &image : m_Objects)
        {
            image->SetAttribute(0x44);
        }
        for (const auto &image : m_Sprites)
        {
            image->SetAttribute(0x44);
        }
    }
    else
    {
        for (const auto &image : m_Font)
        {
            image->ResetAttribute();
        }
        for (const auto &image : m_Blocks)
        {
            image->ResetAttribute();
        }
        for (const auto &image : m_Objects)
        {
            image->ResetAttribute();
        }
        for (const auto &image : m_Sprites)
        {
            image->ResetAttribute();
        }
    }
    RefreshImagesView();
    RefreshMapView();
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::actExportExecute(TObject * Sender)
{
    if (actExportAGDX->Checked)
    {
        ExportAGDXProject();
    }
    else if (actExportAGD->Checked)
    {
        ExportAGDFile();
    }
    else if (actExportCustom->Checked)
    {
        ExportCustom();
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::actExportAGDXExecute(TObject * Sender)
{
    btnExport->Hint = actExportAGDX->Hint;
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::actExportAGDExecute(TObject * Sender)
{
    btnExport->Hint = actExportAGD->Hint;
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::actExportCustomExecute(TObject * Sender)
{
    btnExport->Hint = actExportCustom->Hint;
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::OpenFile(const String & file)
{
    g_ErrorReporter.Clear();
    auto ext = Ioutils::TPath::GetExtension(file).LowerCase();
    txtEvents->Lines->Clear();
    memMessages->Lines->Clear();
    if (ext == ".agd")
    {
        LoadAGDFile(file);
    }
    else
    {
        ImportSnapshot(file);
    }
    // report on the load
    g_ErrorReporter.Add("Info: Window - " + IntToStr(m_Window.x) + ", " + IntToStr(m_Window.y) + " - " + IntToStr(m_Window.w) + "x" + IntToStr(m_Window.h));
    g_ErrorReporter.Add("Info: Sprites - " + IntToStr((int)m_Sprites.size()));
    g_ErrorReporter.Add("Info: Blocks  - " + IntToStr((int)m_Blocks.size()));
    g_ErrorReporter.Add("Info: Objects - " + IntToStr((int)m_Objects.size()));
    RefreshImagesView();
    RefreshMapView();
    txtEvents->Lines->Add(m_EventCode);
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::LoadAGDFile(const String & file)
{
    if (TFile::Exists(file))
    {
        m_File  = file;
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
        try
        {
            String data = "";
            for (auto line : lines)
            {
                // find image marker
                auto tline = line + "\r\n";
                auto lline = tline.LowerCase();
                // remove comments
                auto cPos = tline.Pos(";");
                if (cPos > 0)
                {
                    tline = tline.SubString(1, cPos - 1);
                }
                if (tline.Length() > 0)
                {
                    // process the line
                    if (lline.Pos("define") > 0 || lline.Pos("map") > 0 || lline.Pos("event"))
                    {
                        if (lline.Pos("endmap") == 1)
                        {
                            ConvertMap(data);
                            data = "";
                        }
                        else
                        {
                            if (data.Length() > 0)
                            {
                                // convert data
                                Convert(data);
                            }
                        }
                        // defining something
                        data = tline;
                    }
                    else
                    {
                        data += tline;
                    }
                }
            }
        }
        catch (...)
        {
            g_ErrorReporter.Add("Error: Exception caught while processing file. [" + file + "]");
        }
    }
    else
    {
        g_ErrorReporter.Add("Error: File not found. [" + file + "]");
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::ImportSnapshot(const String & file)
{
    // Try importing the file
    Importer importer;
    if (importer.Convert(file))
    {
        m_File  = file;
        Caption = "AGDv - " + file;
        m_Blocks.clear();
        m_Objects.clear();
        m_Sprites.clear();
        m_Font.clear();
        m_Screens.clear();
        m_Messages.clear();
        m_Events.clear();
        m_EventCode = importer.Events;
        m_Map       = nullptr;
        auto window = importer.Window;
        ConvertWindow(window);
        m_Keys = importer.Keys;
        m_JumpTable = importer.JumpTable;
        auto blocks = SplitString(importer.Blocks, "ª");
        for (auto block : blocks)
        {
            block = block.Trim();
            if (block != "")
            {
                auto pdata = PreProcess(block);
                ConvertBlock(pdata);
            }
        }
        auto objects = SplitString(importer.Objects, "ª");
        for (auto object : objects)
        {
            object = object.Trim();
            if (object != "")
            {
                auto pdata = PreProcess(object);
                ConvertObject(pdata);
            }
        }
        auto sprites = SplitString(importer.Sprites, "ª");
        for (auto sprite : sprites)
        {
            sprite = sprite.Trim();
            if (sprite != "")
            {
                auto pdata = PreProcess(sprite);
                ConvertSprite(pdata, importer.SpriteHeight);
            }
        }
        auto fonts = SplitString(importer.Font, "ª");
        for (auto font : fonts)
        {
            font = font.Trim();
            if (font != "")
            {
                ConvertFont(font);
            }
        }
        auto screens = SplitString(importer.Screens, "ª");
        for (auto screen : screens)
        {
            screen = screen.Trim();
            if (screen != "")
            {
                auto pdata = PreProcess(screen);
                ConvertScreen(pdata);
            }
        }
        auto msgs = SplitString(importer.Messages, "ª");
        for (auto msg : msgs)
        {
            msg = msg.Trim();
            if (msg != "")
            {
                ConvertMessages(msg);
            }
        }
        ConvertMap(importer.Map);
        ConvertEventCode();
    }
}
//---------------------------------------------------------------------------
String __fastcall TfrmMain::PreProcess(const String & data) const
{
    String line;
    auto sPos = data.Pos(' ');
    auto tPos = data.Pos('\t');
    try
    {
        sPos = sPos != 0 && tPos != 0 ? std::min(sPos, tPos) : (sPos == 0 ? tPos : sPos);
        // remove the define/map/endmap statement
        line = data.SubString(sPos, data.Length()).Trim();
        // replace all newlines + tabs with space
        int rPos, nPos;
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
    }
    catch (...)
    {
        g_ErrorReporter.Add("Error: Exception caught while pre-processing white space");
        throw;
    }
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
    try
    {
        int hPos = 0;
        do
        {
            hPos = line.Pos("$");
            if (hPos > 0)
            {
                auto l1 = line.SubString(1, hPos - 1).Trim();
                auto l2 = line.SubString(hPos + 1, line.Length());
                auto l3 = String();
                sPos    = l2.Pos(' ');
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
    }
    catch (...)
    {
        g_ErrorReporter.Add("Error: Exception caught while pre-processing hex into decimal");
        throw;
    }
    return line.Trim();
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::Convert(const String & data)
{
    try
    {
        auto ld = data.LowerCase();
        if (ld.Pos("event ") == 1)
        {
            txtEvents->Lines->Add(data);
            m_Events.push_back(data);
        }
        else if (ld.Pos("definemessages") == 0)
        {
            auto pdata = PreProcess(data);
            if (ld.Pos("defineblock") == 1)
            {
                ConvertBlock(pdata);
            }
            else if (ld.Pos("defineobject") == 1)
            {
                ConvertObject(pdata);
            }
            else if (ld.Pos("definesprite") == 1)
            {
                ConvertSprite(pdata, 16);
            }
            else if (ld.Pos("definefont") == 1)
            {
                ConvertFont(pdata);
            }
            else if (ld.Pos("definewindow") == 1)
            {
                ConvertWindow(pdata);
            }
            else if (ld.Pos("definescreen") == 1)
            {
                ConvertScreen(pdata);
            }
        }
        else
        {
            ConvertMessages(data);
        }
    }
    catch (...)
    {
        g_ErrorReporter.Add("Error: Exception caught while converting data to valid AGD type");
        throw;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::ConvertScreen(const String & data)
{
    try
    {
        // convert the data
        m_Screens.push_back(std::move(std::make_unique<GameScreen>(data, m_Screens.size())));
    }
    catch (...)
    {
        g_ErrorReporter.Add("Error: Exception caught while converting Screen data");
        throw;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::ConvertMap(const String & data)
{
    try
    {
        auto pdata = PreProcess(data);
        m_Map      = std::make_unique <Map> (pdata);
    }
    catch (...)
    {
        g_ErrorReporter.Add("Error: Exception caught while converting Map data");
        throw;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::ConvertEventCode()
{
    try
    {
        auto lines = SplitString(m_EventCode, "\n");
        String content;
        for (auto line : lines)
        {
            line += "\n";
            if (line.SubString(1, 5).UpperCase() == "EVENT" && content != "")
            {
                m_Events.push_back(content);
                content = "";
            }
            if (line.Trim() != "")
            {
                content += line;
            }
        }
        m_Events.push_back(content);
    }
    catch (...)
    {
        g_ErrorReporter.Add("Error: Exception caught while converting Event code");
        throw;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::ConvertWindow(const String & data)
{
    try
    {
        auto tokens = SplitString(data, " ");
        auto i      = 0;
        for (auto token : tokens)
        {
            switch (i++)
            {
            case 0:
                m_Window.y = StrToInt(token);
                break;
            case 1:
                m_Window.x = StrToInt(token);
                break;
            case 2:
                m_Window.h = StrToInt(token);
                break;
            case 3:
                m_Window.w = StrToInt(token);
                break;
            }
        }
    }
    catch (...)
    {
        g_ErrorReporter.Add("Error: Exception caught while converting Window data");
        throw;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::ConvertBlock(const String & data)
{
    try
    {
        // convert the data
        // get the type
        auto tPos    = data.Pos(" ");
        auto subType = data.SubString(1, tPos - 1);
        auto idata   = data.SubString(tPos + 1, data.Length());
        m_Blocks.push_back(std::move(std::make_unique <ImageBlock> (subType, idata)));
    }
    catch (...)
    {
        g_ErrorReporter.Add("Error: Exception caught while converting Block data");
        throw;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::ConvertObject(const String & data)
{
    try
    {
        // convert the data
        m_Objects.push_back(std::move(std::make_unique <ImageObject> (data)));
    }
    catch (...)
    {
        g_ErrorReporter.Add("Error: Exception caught while converting Object data");
        throw;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::ConvertSprite(const String & data, unsigned int spriteHeight)
{
    try
    {
        // convert the data
        m_Sprites.push_back(std::move(std::make_unique <ImageSprite> (data, spriteHeight)));
    }
    catch (...)
    {
        g_ErrorReporter.Add("Error: Exception caught while converting Sprite data");
        throw;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::ConvertFont(const String & data)
{
    try
    {
        // convert the data
        m_Font.push_back(std::move(std::make_unique <ImageFont> (data)));
    }
    catch (...)
    {
        g_ErrorReporter.Add("Error: Exception caught while converting Font data");
        throw;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::ConvertMessages(const String & data)
{
    try
    {
        auto mdata = data;
        // convert the data
        if (data.Pos("definemessages "))
        {
            auto mPos = data.Pos(" ");
            mdata  = data.SubString(mPos + 1, data.Length());
        }
        //auto tokens = SplitString(mdata, "\"");
        m_Messages.push_back(mdata);
        memMessages->Lines->Add(mdata);
//        for (auto token : tokens)
//        {
//            token = token.Trim();
//            if (token != "")
//            {
//                m_Messages.push_back(token1;
//                memMessages->Lines->Add("\"" + token + "\"");
//            }
//        }
    }
    catch (...)
    {
        g_ErrorReporter.Add("Error: Exception caught while converting Message data");
        throw;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::DisplayImages(const String & title, const ImageList & images, int & y, int scale, int index)
{
    auto my                                      = 0;
    auto x                                       = 8;
    imgView->Picture->Bitmap->Canvas->Pen->Color = clWhite;
    imgView->Picture->Bitmap->Canvas->TextOut(8, y, title);
    y += imgView->Picture->Bitmap->Canvas->TextHeight(title) + 8;
    m_ImageY[index] = y;
    for (const auto &image : images)
    {
        my    = std::max(my, image->Height * scale);
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
void __fastcall TfrmMain::DisplayMessasges(const String & title, int & y, int scale)
{
    if (m_Font.size() == 0)
    {
        // initialise the ZX Spectrum font
        auto bmp = std::make_unique <TBitmap> ();
        bmp->Assign(imgZXFont->Picture->Graphic);
        m_Font.push_back(std::move(std::make_unique <ImageFont> (bmp.get())));
    }
    y -= 16;
    imgView->Picture->Bitmap->Canvas->Pen->Color = clWhite;
    imgView->Picture->Bitmap->Canvas->TextOut(8, y, title);
    y += imgView->Picture->Bitmap->Canvas->TextHeight(title) + 8;
    const auto font     = dynamic_cast <ImageFont *> (m_Font[0].get());
    auto       newLined = false;
    for (const auto &message : m_Messages)
    {
        auto x = 32;
        for (const auto &chr : message)
        {
            if (chr - 32 >= 0)
            {
                x += font->DrawChr(x, y, imgView->Picture->Bitmap, scale, chr - 32);
                newLined = false;
            }
            else
            {
                if (!newLined)
                {
                    x = 32;
                    y += font->Height * scale;
                    newLined = true;
                }
            }
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
    barStatus->Panels->Items[0]->Text = "Blocks: " + IntToStr((int)m_Blocks.size());
    barStatus->Panels->Items[1]->Text = "Objects: " + IntToStr((int)m_Objects.size());
    barStatus->Panels->Items[2]->Text = "Sprites: " + IntToStr((int)m_Sprites.size());
    barStatus->Panels->Items[3]->Text = "Font: " + IntToStr((int)m_Font.size());
    barStatus->Panels->Items[4]->Text = "Messages: " + IntToStr((int)m_Messages.size());
    // refresh the image
    if (y + 32 != imgView->Height)
    {
        imgView->Height                  = y + 32;
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
        imgMap->Width  = imgMap->Picture->Bitmap->Width;
        imgMap->Height = imgMap->Picture->Bitmap->Height;
        imgMap->Invalidate();
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::ExportAGDXProject()
{
    int  id      = 1;
    auto project = std::make_unique <Project> ();
    auto name    = System::File::NameWithoutExtension(m_File);
    project->Create(name, "ZX Spectrum 256x192 16 Colour");
    project->WindowInfo = m_Window;
    project->Save();
    // save messages
    auto path = System::Path::Project;
    auto file = System::File::Combine(path, "Messages.txt");
    for (auto msg : m_Messages)
    {
        System::File::AppendText(file, msg + "\r\n");
    }
    // save events
    std::map <String, String> EventToFileMap;
    EventToFileMap["EVENT PLAYER"]        = "Player control";
    EventToFileMap["EVENT SPRITETYPE1"]   = "Sprite type 1";
    EventToFileMap["EVENT SPRITETYPE2"]   = "Sprite type 2";
    EventToFileMap["EVENT SPRITETYPE3"]   = "Sprite type 3";
    EventToFileMap["EVENT SPRITETYPE4"]   = "Sprite type 4";
    EventToFileMap["EVENT SPRITETYPE5"]   = "Sprite type 5";
    EventToFileMap["EVENT SPRITETYPE6"]   = "Sprite type 6";
    EventToFileMap["EVENT SPRITETYPE7"]   = "Sprite type 7";
    EventToFileMap["EVENT SPRITETYPE8"]   = "Sprite type 8";
    EventToFileMap["EVENT GAMEINIT"]      = "Game initialisation";
    EventToFileMap["EVENT INITSPRITE"]    = "Initialise sprite";
    EventToFileMap["EVENT KILLPLAYER"]    = "Kill player";
    EventToFileMap["EVENT INTROMENU"]     = "Introduction menu";
    EventToFileMap["EVENT MAINLOOP1"]     = "Main loop 1";
    EventToFileMap["EVENT MAINLOOP2"]     = "Main loop 2";
    EventToFileMap["EVENT RESTARTSCREEN"] = "Restart screen";
    EventToFileMap["EVENT FELLTOOFAR"]    = "Fell too far";
    EventToFileMap["EVENT LOSTGAME"]      = "Lost game";
    EventToFileMap["EVENT COMPLETEDGAME"] = "Completed game";
    EventToFileMap["EVENT NEWHIGHSCORE"]  = "New high score";
    EventToFileMap["EVENT COLLECTBLOCK"]  = "Collect block";
    for (auto code : m_Events)
    {
        auto event = code.SubString(1, code.Pos("\r") - 1);
        // write the file out
        if (EventToFileMap.count(event) == 1)
        {
            auto file = System::File::Combine(path, EventToFileMap[event] + ".event");
            System::File::AppendText(file, ";" + code);
//            // read the file back
//            auto lines = System::File::ReadLines(file);
//            // replace SCREEN = with new reference
//            auto newFile = String();
//            for (auto line : lines)
//            {
//                auto p = line.UpperCase().Pos("SCREEN");
//                if (p > 0)
//                {
//                    p += 6;
//                    // find the start of the room number
//                    for (auto i = p; i <= line.Length(); i++)
//                    {
//                        if (IsNumber(line[i]))
//                        {
//                            auto roomNum = StrToInt(line.SubString(i, line.Length()));
//                            // replace the room number with the new room x, y
//                            auto roomPt = m_Map->GetRoomPt(roomNum);
//                            auto roomCoord = " " + IntToStr((int)roomPt.X) + ", " + IntToStr((int)roomPt.Y);
//                            line = line.SubString(1, i - 1) + roomCoord;
//                            break;
//                        }
//                    }
//                }
//                newFile += line + "\r\n";
//			}
//			System::File::WriteText(file, newFile);
		}
	}
    // save images
    // font
    if (m_Font.size() == 1)
    {
        auto   file    = System::File::Combine(path, "Game Font.json");
        String content = "{\r\n  \"Image\": {\r\n    \"Width\": 8,\r\n    \"Height\": 8,\r\n    \"Frames\": [\r\n";
        BitmapData data;
        for (const auto &chr : m_Font)
        {
            String line = "      \"";
            chr->GetBitmapData(data);
            int i = 0;
            for (auto byte : data)
            {
                line += IntToHex(byte, 2);
                if ((++i % 8) == 0)
                {
                    content += line + "07\"" + (i == data.size() ? "" : ",") + "\r\n";
                    line = "      \"";
                }
            }
        }
        content += "    ]\r\n  }\r\n}";
        System::File::WriteText(file, content);
        project->AddFile("Game Font.json", "Image", "Character Set");
        project->Save();
    }
    auto wx = m_Window.x * 8;
    auto wy = m_Window.y * 8;
    // objects
    int objId = id + 1;
    if (m_Objects.size() != 0)
    {
        auto i = 0;
        for (auto &obj : m_Objects)
        {
            auto object    = dynamic_cast<ImageObject*>(obj.get());
            object->Id     = ++id;
            auto   file    = System::File::Combine(path, "Object " + IntToStr(++i) + ".json");
            String content = "{\r\n  \"Document\": {\r\n    \"RefId\": " + IntToStr(object->Id) + "\r\n  },\r\n  \"Image\": {\r\n    \"Width\": 16,\r\n    \"Height\": 16,\r\n";
            content += "    \"RoomIndex\": " + IntToStr(object->Room) + ",\r\n";
            content += "    \"Position\": {\r\n      \"X\": " + IntToStr((int)object->Position.X - wx) + ",\r\n      \"Y\": " + IntToStr((int)object->Position.Y - wy) + "\r\n    },\r\n";
            content += "    \"State\": " + IntToStr(object->Room < 254 ? 0 : (object->Room == 254 ? 1 : 2)) + ",\r\n";
            content += "    \"Frames\": [\r\n";
            BitmapData data;
            String line = "      \"";
            obj->GetBitmapData(data);
            for (auto byte : data)
            {
                line += IntToHex(byte, 2);
            }
            for (auto j = 0; j < 4; j++)
            {
                line += IntToHex(obj->Attribute);
            }
            content += line + "\"\r\n    ]\r\n  }\r\n}";
            System::File::WriteText(file, content);
            project->AddFile(System::File::NameWithExtension(file), "Image", "Object");
            project->Save();
        }
    }
    // sprites
    int sprId = id + 1;
    if (m_Sprites.size() != 0)
    {
        auto i = 0;
        for (auto &obj : m_Sprites)
        {
            obj->Id        = ++id;
            auto   file    = System::File::Combine(path, "Sprite " + IntToStr(++i) + ".json");
            String content = "{\r\n  \"Document\": {\r\n    \"RefId\": " + IntToStr(obj->Id) + "\r\n  },\r\n  \"Image\": {\r\n    \"Width\": " + IntToStr(obj->Width) + ",\r\n    \"Height\": " + IntToStr(obj->Height) + ",\r\n    \"Frames\": [\r\n";
            BitmapData data;
            obj->GetBitmapData(data);
            auto stride = 2 * obj->Height;
            for (auto f = 0; f < obj->Frames; f++)
            {
                String line = "      \"";
                for (auto i = 0; i < 2 * obj->Height; i++)
                {
                    line += IntToHex(data[(f * stride) + i], 2);
                }
                for (auto j = 0; j < 4; j++)
                {
                    line += IntToHex(0x47, 2);
                }
                content += line + "\"" + (f == obj->Frames - 1 ? "" : ",") + "\r\n";
            }
            content += "    ]\r\n  }\r\n}";
            System::File::WriteText(file, content);
            project->AddFile(System::File::NameWithExtension(file), "Image", "Sprite");
            project->Save();
        }
    }
    // blocks/tiles
    int blkId = id + 1;
    if (m_Blocks.size() != 0)
    {
        std::map <String, String> blockTypeMap;
        blockTypeMap["E"] = "0";
        blockTypeMap["P"] = "1";
        blockTypeMap["W"] = "2";
        blockTypeMap["L"] = "3";
        blockTypeMap["F"] = "4";
        blockTypeMap["D"] = "5";
        blockTypeMap["C"] = "6";
        auto i            = 0;
        for (auto &obj : m_Blocks)
        {
            obj->Id        = ++id;
            auto   file    = System::File::Combine(path, "Block " + IntToStr(++i) + ".json");
            String content = "{\r\n  \"Document\": {\r\n    \"RefId\": " + IntToStr(obj->Id) + "\r\n  },\r\n  \"Image\": {\r\n    \"Width\": " + IntToStr(obj->Width) + ",\r\n    \"Height\": " + IntToStr(obj->Height) + ",\r\n    \"Frames\": [\r\n";
            BitmapData data;
            String line = "      \"";
            obj->GetBitmapData(data);
            for (auto byte : data)
            {
                line += IntToHex(byte, 2);
            }
            line += IntToHex(obj->Attribute);
            content += line + "\"\r\n    ],\r\n    \"Layers\": [\r\n      {\r\n       \"Name\": \"blocktype\",\r\n       \"Data\": \"" + blockTypeMap[obj->SubType[1]] + "\"\r\n      }\r\n    ]\r\n  }\r\n}";
            System::File::WriteText(file, content);
            project->AddFile(System::File::NameWithExtension(file), "Image", "Tile");
            project->Save();
        }
    }
    // save map/screens
    if (m_Map->Width > 0 && m_Map->Height > 0)
    {
        auto file = System::File::Combine(path, "Tile Map.json");
        String content = "{\r\n  \"Map\": {"
                          "\r\n        \"StartLocation\": " + IntToStr(m_Map->StartScreen) + ","
                          "\r\n        \"Entities\": [\r\n";
        // output each rooms entity list
        auto screensDone = std::vector<int>();
        auto rx = 0;
        auto ry = 0;
        for (auto y = 0; y < 16; y++)
        {
            for (auto x = 0; x < 16; x++)
            {
                // get the blocks for the room
                auto room = m_Map->m_MapData[x][y];
                if (room != 0xff)
                {
                    screensDone.push_back(room);
                    OutputScreen(content, room, x, y);
                    ry = y;
                }
            }
        }
        if (screensDone.size() != m_Screens.size())
        {
            // we need to output the remaining screens as new screens in the map
            // we'll do it from column 0 and 1 line down from the last screen in the actual map
            rx = 0;
            ry++;
            for (auto i = 0; i < m_Screens.size(); i++)
            {
                if (std::find(screensDone.begin(), screensDone.end(), i) == screensDone.end())
                {
                    m_Map->m_MapData[rx][ry] = i;
                    // output the screen
                    OutputScreen(content, i, rx, ry);
                    if (++rx == 16)
                    {
                        rx = 0;
                        ry++;
                    }
                }
            }
        }
        // terminate the array
        content = content.SubString(1, content.Length() - 3);
        content += "\r\n    ],\r\n";
        // output the screen index mapping
        content += "    \"RoomMapping\": {";
        content += "\r\n        \"Width\": 16,";
        content += "\r\n        \"Height\": 16,";
        content += "\r\n        \"Indexes\": [";

        for (auto y = 0; y < 16; y++)
        {
            for (auto x = 0; x < 16; x++)
            {
                content += IntToStr(m_Map->m_MapData[x][y]);
                if (y != 15 || x != 15)
                {
                    content += ", ";
                }
            }
        }
        content += "]";

        content += "\r\n    }\r\n}";
        System::File::WriteText(file, content);
        project->AddFile("Tile Map.json", "Map", "Tiled");

        // TODO: Write Controls document

        // TODO: Write JumpTable document

        project->Save();
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::OutputScreen(String& content, int room, int rx, int ry)
{
    auto bx = 0;
    auto by = 0;
    for (const auto &block : m_Screens[room]->m_Blocks)
    {
        if (block)
        {
            auto x = ((rx * m_Window.w) + bx) * 8;
            auto y = ((ry * m_Window.h) + by) * 8;
            content += "        {\r\n          \"X\":" + IntToStr(x) + ",\r\n          \"Y\":" + IntToStr(y) + ",\r\n          \"RefId\":" + IntToStr(m_Blocks[block]->Id) + "\r\n        }";
            content += ",\r\n";
        }
        if (++bx == m_Window.w)
        {
            bx = 0;
            ++by;
        }
    }

    auto mapRect = TRect(m_Window.x * 8, m_Window.y * 8, (m_Window.x + m_Window.w) * 8, (m_Window.y + m_Window.h) * 8);
    for (const auto &sprite : m_Screens[room]->m_Sprites)
    {
        if (sprite.Index < m_Sprites.size())
        {
            int dx = ((sprite.Position.X / 8) - m_Window.x) * 8;
            int dy = ((sprite.Position.Y / 8) - m_Window.y) * 8;
            int x = (rx * m_Window.w * 8) + dx;
            int y = (ry * m_Window.h * 8) + dy;
            auto locked = dx < mapRect.Left  || (dx + 16) > mapRect.Right || dy < mapRect.Top || (dy + 16) > mapRect.Bottom;
            content += "      {\r\n        \"X\":" + IntToStr(x) +
                       ",\r\n        \"Y\":" + IntToStr(y) +
					   ",\r\n        \"RefId\":" + IntToStr(m_Sprites[sprite.Index]->Id) +
					   ",\r\n        \"SpriteType\":" + IntToStr(sprite.Type) +
					   ",\r\n        \"Room\": {" +
					   "\r\n            \"Locked\": " + (locked ? "true" : "false") +
					   ",\r\n            \"Index\": " + IntToStr(sprite.ScreenIndex) +
					   "\r\n        }" +
                       "\r\n      }";
            content += ",\r\n";
        }
    }
    for (const auto &object : m_Objects)
    {
        const auto& o = dynamic_cast <ImageObject *> (object.get());
        if (room == o->Room)
        {
            int x = ((rx * m_Window.w) + ((o->Position.X / 8) - m_Window.x)) * 8;
            int y = ((ry * m_Window.h) + ((o->Position.Y / 8) - m_Window.y)) * 8;
            content += "      {\r\n        \"X\":" + IntToStr(x) + ",\r\n        \"Y\":" + IntToStr(y) + ",\r\n        \"RefId\":" + IntToStr(o->Id) + "\r\n      }";
            content += ",\r\n";
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::ExportAGDFile()
{
    String content = "DEFINEWINDOW " + IntToStr(m_Window.y) + " " + IntToStr(m_Window.x) + " " + IntToStr(m_Window.h) + " " + IntToStr(m_Window.w) + "\r\n\r\n";

    content += "DEFINECONTROLS '" + String(m_Keys[4]) + "' '" + m_Keys[3] + "' '" + m_Keys[2] + "' '" + m_Keys[1] + "' '" + m_Keys[5] + "' '" + m_Keys[6] + "' '" + m_Keys[7] + "'\r\n\r\n";
    auto wx = m_Window.x * 8;
    auto wy = m_Window.y * 8;
    content += "DEFINEMESSAGES \r\n";
    for (const auto &msg : m_Messages)
    {
        content += msg + "\r\n";
    }
    content += "\r\nDEFINEFONT ";
    // font
    if (m_Font.size() == 1)
    {
        BitmapData data;
        for (const auto &chr : m_Font)
        {
            String line;
            chr->GetBitmapData(data);
            int i = 0;
            for (auto byte : data)
            {
                line += PadLeft(byte) + " ";
                if ((++i % 8) == 0)
                {
                    content += line + "\r\n           ";
                    line = "";
                }
            }
        }
    }

    content += "\r\nDEFINEJUMP " + m_JumpTable + "\r\n";
    // blocks
    if (m_Blocks.size() != 0)
    {
        for (const auto &blk : m_Blocks)
        {
            content += "\r\nDEFINEBLOCK " + blk->SubType + "            \r\n            ";
            BitmapData data;
            blk->GetBitmapData(data);
            for (auto b : data)
            {
                content += PadLeft(b) + " ";
            }
            content += "\r\n            " + PadLeft(blk->Attribute) + "\r\n";
        }
    }
    // sprites
    if (m_Sprites.size() != 0)
    {
        for (const auto &spr : m_Sprites)
        {
            content += "\r\nDEFINESPRITE " + PadLeft(spr->Frames) + "\r\n             ";
            BitmapData data;
            spr->GetBitmapData(data);
            auto stride = spr->Height * 2;
            for (auto f = 0; f < spr->Frames; f++)
            {
                auto c = 16;
                for (auto i = 0; i < stride; i++)
                {
                    content += PadLeft(data[f * stride + i]) + " ";
                    if (--c == 0)
                    {
                        content += "\r\n             ";
                        c = 16;
                    }
                }
                content += "\r\n             ";
            }
        }
    }
    // screen
    if (m_Screens.size() != 0)
    {
        for (const auto &room : m_Screens)
        {
            content += "\r\nDEFINESCREEN ";
            int w = 0;
            for (auto blk : room->m_Blocks)
            {
                content += PadLeft(blk) + " ";
                if (++w % m_Window.w == 0)
                {
                    content += "\r\n             ";
                }
            }
            for (const auto &s : room->m_Sprites)
            {
                content += "\r\nSPRITEPOSITION " + PadLeft(s.Type) + " " + PadLeft(s.Index) + " " + PadLeft((int)s.Position.Y) + " " + PadLeft((int)s.Position.X);
            }
            content += "\r\n";
        }
    }
    // object
    if (m_Objects.size() != 0)
    {
        for (const auto &obj : m_Objects)
        {
            const auto & o = dynamic_cast <ImageObject *> (obj.get());
            content += "\r\nDEFINEOBJECT " + PadLeft(o->Attribute) + " " + PadLeft(o->Room) + " " + IntToStr((int)o->Position.Y ) + " " + PadLeft((int)o->Position.X) + "\r\n             ";
            BitmapData data;
            obj->GetBitmapData(data);
            auto c = 16;
            for (auto b : data)
            {
                content += PadLeft(b) + " ";
                if (--c == 0)
                {
                    content += "\r\n             ";
                    c = 16;
                }
            }
            content += "\r\n";
        }
    }
    // map
    content += "\r\nMAP WIDTH " + IntToStr(m_Map->Rect.Width()+ 3) + "\r\n    STARTSCREEN " + IntToStr(m_Map->StartScreen) + "\r\n    ";
    auto i = 0;
    for (auto y = m_Map->Rect.Top; y <= m_Map->Rect.Bottom; y++)
    {
        for (auto x = m_Map->Rect.Left - 1; x <= m_Map->Rect.Right + 1; x++)
        {
            content += PadLeft(m_Map->m_MapData[x][y]) + " ";
        }
        content += "\r\n    ";
    }
    content += "\r\nENDMAP\r\n\r\n";
    // events
    for (auto l : txtEvents->Lines)
    {
        content += l + "\r\n";
    }
    auto name = System::File::NameWithoutExtension(m_File);
    System::Path::ProjectName = name;
    auto path                 = System::Path::Project;
    auto file                 = System::File::Combine(path, "game.agd");
    System::File::WriteText(file, content);
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::ExportCustom()
{
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::SpeedButton1Click(TObject * Sender)
{
    panReport->Visible = false;
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::imgViewMouseMove(TObject * Sender, TShiftState Shift, int X, int Y)
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
            auto chr = System::UnicodeString::StringOfChar(' ' + X, 1);
            if (X == 95)
            {
                chr = "©";
            }
            barStatus->Panels->Items[5]->Text = "Character: " + chr + " [0x" + IntToHex(32 + X, 2) + "]";
            // select a character
            m_ImageCursorPos.X   = (8 + (X * w * scale)) - 1;
            m_ImageCursorPos.Y   = (m_ImageY[0]) - 1;
            m_ImageCursorSize.cx = (8 * scale) + 2;
            m_ImageCursorSize.cy = (8 * scale) + 2;
        }
    }
    else if (m_Blocks.size() > 0 && m_ImageY[1] <= Y && Y < m_ImageY[2])
    {
        // could be a block
        auto w   = m_Blocks[0]->Width;
        auto h   = m_Blocks[0]->Height;
        int  sw  = (imgView->Width - 8) / ((scale * w) + 8);
        X        = (X - 8) / ((scale * w) + 8);
        int  row = (Y - m_ImageY[1]) / ((h * scale) + 8);
        int  col = X;
        auto blk = row * sw + col;
        if (blk < m_Blocks.size())
        {
            // is a block
            barStatus->Panels->Items[5]->Text = "Block: " + IntToStr(blk) + " - " + m_Blocks[blk]->SubType;
            // select it
            m_ImageCursorPos.X   = (8 + (X * ((w * scale) + 8))) - 2;
            m_ImageCursorPos.Y   = (m_ImageY[1] + (row * (h * scale + 8))) - 2;
            m_ImageCursorSize.cx = (w * scale) + 4;
            m_ImageCursorSize.cy = (h * scale) + 4;
        }
    }
    else if (m_Objects.size() > 0 && m_ImageY[2] <= Y && Y < m_ImageY[3])
    {
        // could be an object
        auto w   = m_Objects[0]->Width;
        auto h   = m_Objects[0]->Height;
        int  sw  = (imgView->Width - 8) / ((scale * w) + 8);
        X        = (X - 8) / ((scale * w) + 8);
        int  row = (Y - m_ImageY[2]) / ((h * scale) + 8);
        int  col = X;
        auto blk = row * sw + col;
        if (blk < m_Objects.size())
        {
            // is a block
            barStatus->Panels->Items[5]->Text = "Object: " + IntToStr(blk);
            // select it
            m_ImageCursorPos.X   = (8 + (X * ((w * scale) + 8))) - 2;
            m_ImageCursorPos.Y   = (m_ImageY[2] + (row * (h * scale + 8))) - 2;
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
            // sprites are animating
            int sw   = (imgView->Width - 8) / ((scale * w) + 8);
            X        = (X - 8) / ((scale * w) + 8);
            int  row = (Y - m_ImageY[3]) / ((h * scale) + 8);
            int  col = X;
            auto spr = row * sw + col;
            if (spr < m_Sprites.size())
            {
                // is a sprite
                barStatus->Panels->Items[5]->Text = "Sprite: " + IntToStr(spr);
                // select it
                m_ImageCursorPos.X   = (8 + (X * ((w * scale) + 8))) - 2;
                m_ImageCursorPos.Y   = (m_ImageY[3] + (row * (h * scale + 8))) - 2;
                m_ImageCursorSize.cx = (w * scale) + 4;
                m_ImageCursorSize.cy = (h * scale) + 4;
            }
        }
        else
        {
            // sprites are grouped into complete frame sets
            auto spriteIndex = 0;
            auto cx          = 8;
            auto cy          = m_ImageY[3];
            for (const auto &sprite : m_Sprites)
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
                    barStatus->Panels->Items[5]->Text = "Sprite: " + IntToStr(spriteIndex);
                    // select it
                    m_ImageCursorPos.X   = cx - 2;
                    m_ImageCursorPos.Y   = cy - 2;
                    m_ImageCursorSize.cx = w + 4;
                    m_ImageCursorSize.cy = h + 4;
                    break;
                }
                spriteIndex++ ;
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
void __fastcall TfrmMain::sbxViewMouseWheel(TObject *Sender, TShiftState Shift, int WheelDelta, TPoint & MousePos, bool &Handled)
{
    if (Shift.Contains(ssCtrl))
    {
        Handled     = true;
        float delta = WheelDelta / 200.f;
        m_Scale     = std::max(1.0f, std::min(16.f, m_Scale + delta));
        RefreshImagesView();
        RefreshMapView();
    }
    else
    {
        Handled     = true;
        float delta = -WheelDelta / (Shift.Contains(ssShift) ? 5.f : 20.f);
        sbxView->VertScrollBar->Position += delta;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::WMDropFiles(TWMDropFiles & message)
{
    TCHAR filename[MAX_PATH];
    HDROP hDrop    = (HDROP)message.Drop;
    int   numFiles = DragQueryFile(hDrop, -1, NULL, NULL);
    if (numFiles > 0)
    {
        DragQueryFile(hDrop, 0, filename, sizeof(filename));
        // process the file in 'buff'
        OpenFile(String(filename));
    }
    DragFinish(hDrop);
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::cmbEventChange(TObject *Sender)
{
    // find the combo box item
    auto combo = dynamic_cast<TComboBox*>(Sender);
    if (combo != nullptr)
    {
        auto event = "EVENT " + ReplaceText(combo->Items->Strings[combo->ItemIndex], " ", "");

        auto foundAt = txtEvents->FindText(event, 0, txtEvents->Text.Length(), TSearchTypes() << stWholeWord << stMatchCase);
        if (foundAt != -1)
        {
            txtEvents->SetFocus();
            txtEvents->SelStart = foundAt;
            txtEvents->SelLength = 0;
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::imgMapMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
    //
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::imgMapMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
    //
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::imgMapMouseMove(TObject *Sender, TShiftState Shift, int X, int Y)
{
    //
}
//---------------------------------------------------------------------------
String __fastcall TfrmMain::PadLeft(unsigned int number, int size)
{
    return RightStr(StringOfChar(' ', size) + IntToStr((int)number), size);
}
//---------------------------------------------------------------------------

