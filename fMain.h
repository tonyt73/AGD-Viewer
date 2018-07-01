//---------------------------------------------------------------------------
#ifndef fMainH
#define fMainH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <System.Actions.hpp>
#include <System.ImageList.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ImgList.hpp>
#include <Vcl.ActnCtrls.hpp>
#include <Vcl.ActnList.hpp>
#include <Vcl.ActnMan.hpp>
#include <Vcl.ActnMenus.hpp>
#include <Vcl.PlatformDefaultStyleActnCtrls.hpp>
#include <Vcl.StdActns.hpp>
#include <Vcl.ToolWin.hpp>
#include <Vcl.Menus.hpp>
#include "Image.h"
#include "Screen.h"
#include "Map.h"
#include "Window.h"
#include <Vcl.Imaging.pngimage.hpp>
#include <Vcl.Buttons.hpp>
//---------------------------------------------------------------------------
class TfrmMain : public TForm
{
__published:	// IDE-managed Components
    TActionList *actionList;
    TFileOpen *actFileOpen;
    TFileExit *actFileExit;
    TAction *actAbout;
    TImage *imgView;
    TStatusBar *barStatus;
    TScrollBox *sbxView;
    TPageControl *pgcViews;
    TTabSheet *tabImages;
    TTabSheet *tabMap;
    TToolBar *ToolBar1;
    TToolButton *ToolButton1;
    TImageList *ImageList1;
    TToolButton *ToolButton2;
    TToolButton *ToolButton3;
    TToolButton *ToolButton4;
    TToolButton *ToolButton5;
    TToolButton *ToolButton6;
    TAction *actZoomIn;
    TAction *actZoomOut;
    TAction *actZoomActualSize;
    TAction *actZoomToFit;
    TToolButton *ToolButton7;
    TAction *actPlay;
    TAction *actStop;
    TToolButton *ToolButton8;
    TToolButton *ToolButton9;
    TTimer *Timer1;
    TToolButton *ToolButton10;
    TToolButton *btnToggle;
    TAction *actToggleMonochrome;
    TScrollBox *sbxMap;
    TImage *imgMap;
    TImage *imgZXFont;
    TTimer *Timer2;
    TPanel *panReport;
    TMemo *memReport;
    TLabel *Label1;
    TPanel *Panel2;
    TSpeedButton *SpeedButton1;
    TAction *actExport;
    TToolButton *btnExport;
    TToolButton *ToolButton12;
    TTabSheet *Events;
    TMemo *memEvents;
    TTabSheet *TabSheet1;
    TMemo *memMessages;
    TPopupMenu *popExports;
    TMenuItem *ExporttoAGDXStudioProject1;
    TMenuItem *AGDFile1;
    TMenuItem *Custom1;
    TAction *actExportAGDX;
    TAction *actExportAGD;
    TAction *actExportCustom;
    void __fastcall FormResize(TObject *Sender);
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall FormDestroy(TObject *Sender);
    void __fastcall actFileOpenAccept(TObject *Sender);
    void __fastcall actZoomInExecute(TObject *Sender);
    void __fastcall actZoomOutExecute(TObject *Sender);
    void __fastcall actZoomActualSizeExecute(TObject *Sender);
    void __fastcall actZoomToFitExecute(TObject *Sender);
    void __fastcall actPlayExecute(TObject *Sender);
    void __fastcall actStopExecute(TObject *Sender);
    void __fastcall actToggleMonochromeExecute(TObject *Sender);
    void __fastcall Timer1Timer(TObject *Sender);
    void __fastcall sbxViewMouseWheel(TObject *Sender, TShiftState Shift, int WheelDelta, TPoint &MousePos, bool &Handled);
    void __fastcall imgViewMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
    void __fastcall FormActivate(TObject *Sender);
    void __fastcall Timer2Timer(TObject *Sender);
    void __fastcall FormDeactivate(TObject *Sender);
    void __fastcall SpeedButton1Click(TObject *Sender);
    void __fastcall actExportExecute(TObject *Sender);
    void __fastcall actExportAGDXExecute(TObject *Sender);
    void __fastcall actExportAGDExecute(TObject *Sender);
    void __fastcall actExportCustomExecute(TObject *Sender);
private:	// User declarations
    ImageList           m_Font;
    ImageList           m_Blocks;
    ImageList           m_Objects;
    ImageList           m_Sprites;
    ScreenList          m_Screens;
    Window              m_Window;
    std::vector<String> m_Events;
    std::vector<String> m_Messages;
   std::unique_ptr<Map> m_Map;
    float               m_Scale;
    int                 m_Animate;
    int                 m_MonochromeAttribute;
    int                 m_ImageY[4];
    TPoint              m_ImageCursorPos;
    TSize               m_ImageCursorSize;
    TPoint              m_Cursor;
    String              m_File;
    String              m_EventCode;

    virtual void __fastcall WMDropFiles(TWMDropFiles &message);
    BEGIN_MESSAGE_MAP
    MESSAGE_HANDLER(WM_DROPFILES,TWMDropFiles,WMDropFiles);
    END_MESSAGE_MAP (TForm);

    void    __fastcall  OpenFile(const String& file);
    void    __fastcall  LoadAGDFile(const String& file);
    void    __fastcall  ImportSnapshot(const String& file);
    void    __fastcall  Export();
    String  __fastcall  PreProcess(const String& data) const;
    void    __fastcall  Convert(const String& data);
    void    __fastcall  ConvertScreen(const String& data);
    void    __fastcall  ConvertMap(const String& data);
    void    __fastcall  ConvertWindow(const String& data);
    void    __fastcall  ConvertBlock(const String& data);
    void    __fastcall  ConvertObject(const String& data);
    void    __fastcall  ConvertSprite(const String& data);
    void    __fastcall  ConvertFont(const String& data);
    void    __fastcall  ConvertMessages(const String& data);
    void    __fastcall  ConvertEventCode();
    void    __fastcall  DisplayMessasges(const String& title, int& y, int scale);
    void    __fastcall  DisplayImages(const String& title, const ImageList& images, int& y, int scale, int index);
    void    __fastcall  RefreshImagesView();
    void    __fastcall  RefreshMapView();

    void    __fastcall  ExportAGDXProject();
    void    __fastcall  ExportAGDFile();
    void    __fastcall  ExportCustom();

public:		// User declarations
            __fastcall  TfrmMain(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmMain *frmMain;
//---------------------------------------------------------------------------
#endif
