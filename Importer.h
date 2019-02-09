//---------------------------------------------------------------------------
#ifndef ImporterH
#define ImporterH
//---------------------------------------------------------------------------
#include <memory>
#include <vector>
#include "Snapshot.h"
//---------------------------------------------------------------------------
class Importer
{
private:
    struct Values
    {
        TPoint          WindowPosition;
        TSize           WindowSize;
        unsigned int    Rooms;
    };

    struct Addresses
    {
        unsigned int    SpriteFrames;
        unsigned int    nSpriteFrames;
        unsigned int    SpriteData;
        unsigned int    nSpriteData;
        unsigned int    BlockImageData;
        unsigned int    nBlockImageData;
        unsigned int    BlockAttrsData;
        unsigned int    nBlockAttrsData;
        unsigned int    BlockPropsData;
        unsigned int    nBlockPropsData;
        unsigned int    ScreenLayouts;
        unsigned int    nScreenLayouts;
        unsigned int    EnemyPositions;
        unsigned int    nEnemyPositions;
        unsigned int    AYSoundFx;
        unsigned int    nAYSoundFx;
        unsigned int    Messages;
        unsigned int    nMessages;
        unsigned int    Keys;
        unsigned int    JumpTable;
        unsigned int    Objects;
        unsigned int    nObjects;
        unsigned int    EventCode;
        unsigned int    nEventCode;
        unsigned int    Font;
        unsigned int    StartScreen;
        unsigned int    SpriteSize;
    };

    Addresses                   m_AddressOf;
	Values                      m_ValueOf;
    unsigned int                m_Version;

	String                      m_Window;
	String                      m_Blocks;
	String                      m_Objects;
	String                      m_Sprites;
	String                      m_Font;
	String                      m_Screens;
	String                      m_Map;
	String                      m_Messages;
	String                      m_Keys;
	String                      m_Events;
	String                      m_JumpTable;
	unsigned int                m_SpriteHeight;

	int                         nTabs;
	int                         nPrintMode;

	std::unique_ptr<Snapshot>   m_Snapshot;

	void        __fastcall  WriteText(const String& text);
	void        __fastcall  CodeLine(char cInstruction, char cNext);
	void        __fastcall  NewLine();
	void        __fastcall  ConvertByte(unsigned char& cByte) const;
	void        __fastcall  ImportWindow();
	void        __fastcall  ImportMessages();
	void        __fastcall  ImportKeys();
	void        __fastcall  ImportJumpTable();
	void        __fastcall  ImportEvents();
	void        __fastcall  ImportFont();
	void        __fastcall  ImportBlocks();
	void        __fastcall  ImportSprites();
	void        __fastcall  ImportObjects();
	void        __fastcall  ImportScreens();
	void        __fastcall  ImportMap();

public:
				__fastcall  Importer();

	bool        __fastcall  Convert(const String& file);

	String      __property  Window = { read = m_Window };
	String      __property  Blocks = { read = m_Blocks };
	String      __property  Objects = { read = m_Objects };
	String      __property  Sprites = { read = m_Sprites };
	String      __property  Font = { read = m_Font };
	String      __property  Screens = { read = m_Screens };
	String      __property  Map = { read = m_Map };
	String      __property  Messages = { read = m_Messages };
	String      __property  Keys = { read = m_Keys };
	String      __property  Events = { read = m_Events };
	String      __property  JumpTable = { read = m_JumpTable };
   unsigned int __property  SpriteHeight = { read = m_SpriteHeight };
   unsigned int __property  Version = { read = m_Version, write = m_Version };
};
//---------------------------------------------------------------------------
#endif
