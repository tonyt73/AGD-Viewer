//---------------------------------------------------------------------------
#include "agdv.pch.h"
#include <algorithm>
//---------------------------------------------------------------------------
#include "Importer.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
// Equates for version 4.7
enum
{
	INS_IF, INS_MSU, INS_MSD, INS_MSL, INS_MSR, INS_ENDIF, INS_CGU, INS_CGD, INS_CGL, INS_CGR, INS_LDU, INS_LDD, INS_DED, INS_CUS, INS_NUMBER, INS_TO, INS_FROM, INS_BY, INS_TYP, INS_IMAGE, INS_FRAME, INS_X, INS_Y, INS_DIR, INS_PMA, INS_PMB, INS_SCREEN, INS_LIVES, INS_A, INS_B, INS_C, INS_D, INS_E,
	INS_F, INS_G, INS_H, INS_I, INS_J, INS_K, INS_L, INS_M, INS_N, INS_O, INS_P, INS_Q, INS_R, INS_S, INS_T, INS_LINE, INS_COLUMN, INS_CLOCK, INS_RND, INS_OBJ, INS_OPT, INS_GOT, INS_KEY, INS_COLLISION, INS_EQUAL_TO, INS_GREATER_THAN, INS_NOT_EQUAL_TO, INS_LESS_THAN, INS_LET, INS_ANIMATE,
	INS_ANIMBACK, INS_PBL, INS_NEX, INS_RST, INS_SPW, INS_REM, INS_GRN, INS_ELSE, INS_DSC, INS_SCO, INS_SND, INS_BEP, INS_CLS, INS_BOR, INS_CLR, INS_DEL, INS_MESSAGE, INS_MENU, INS_INV, INS_KILL, INS_ADD, INS_SUB, INS_DIS, INS_SUP, INS_SDW, INS_SLF, INS_SRG, INS_PRE, INS_JUMP, INS_FALL, INS_OTHER,
	INS_SPAWNED, INS_ORIGINAL, INS_EGM, INS_GET, INS_PUT, INS_DET, INS_ASM, INS_EXIT, INS_REPEAT, INS_ENDREPEAT, INS_MULTIPLY, INS_DIVDE, INS_SPI, INS_TRAIL, INS_LASER, INS_EXPLODE, INS_FADE, INS_TICKER, INS_BIGMESSAGE, INS_REDRAW, INS_SIL, INS_CON, INS_DIG, INS_STR, INS_BIGSCORE, INS_STOPFALL
} ;
#define INS_ARGUMENTS			INS_CGU
//---------------------------------------------------------------------------
/* Table used to convert keycodes to ASCII. */
const unsigned char cKeyTab[ 40 ] =
{
	'B', 'H', 'Y', '6', '5', 'T', 'G', 'V',
	'N', 'J', 'U', '7', '4', 'R', 'F', 'C',
	'M', 'K', 'I', '8', '3', 'E', 'D', 'X',
	'.', 'L', 'O', '9', '2', 'W', 'S', 'Z',
	32, 13,	'P', '0', '1', 'Q', 'A', '~'
};
//---------------------------------------------------------------------------
const std::vector<String>  cKeyword =
{
	"IF",				// if.
	"SPRITEUP",			// move sprite up.
	"SPRITEDOWN",		// move sprite down.
	"SPRITELEFT",		// move sprite left.
	"SPRITERIGHT",		// move sprite right.
	"ENDIF",			// endif.
	"CANGOUP",			// sprite can go up test.
	"CANGODOWN",		// sprite can go down test.
	"CANGOLEFT",		// sprite can go left test.
	"CANGORIGHT",		// sprite can go right test.
	"LADDERABOVE",		// ladder above test.
	"LADDERBELOW",		// ladder below test.
	"DEADLY",			// check if touching deadly block.
	"CUSTOM",    		// check if touching custom block.
	".",				// number.
	"TO",           	// variable to increment.
	"FROM",          	// variable to decrement.
	"BY",	          	// multiply or divide by.

	/* Sprite variables. */

	"TYPE",				// first parameter.
	"IMAGE",			// image number.
	"FRAME",			// frame number.
	"Y",				// vertical position.
	"X",				// horizontal position.
	"DIRECTION",		// user sprite parameter.
	"SETTINGA",			// sprite parameter a.
	"SETTINGB",			// sprite parameter b.
	"SCREEN",			// screen number.
	"LIVES",			// lives.
	"A",				// variable.
	"B",				// variable.
	"C",				// variable.
	"D",				// variable.
	"E",				// variable.
	"F",				// variable.
	"G",				// variable.
	"H",				// variable.
	"I",				// variable.
	"J",				// variable.
	"K",				// variable.
	"L",				// variable.
	"M",				// variable.
	"N",				// variable.
	"O",				// variable.
	"P",				// variable.
	"Q",				// variable.
	"R",				// variable.
	"S",				// variable.
	"T",				// variable.
	"LINE",				// x coordinate.
	"COLUMN",			// y coordinate.
	"CLOCK",			// clock.
	"RND",				// last random number variable.
	"OBJ",				// last object variable.
	"OPT",				// menu option variable.
	"GOT",				// function.
	"KEY",				// function.

	/* Commands. */

	"COLLISION",		// collision with sprite.
	"=",				// equals, ignored.
	">",				// greater than.
	"<>",				// not equal to.
	"<=",				// less than or equal to.
	"LET",				// x = y.
	"ANIMATE",			// animate sprite.
	"ANIMBACK",			// animate sprite backwards.
	"PUTBLOCK",			// put block on screen.
	"NEXTLEVEL",		// next level.
	"RESTART",			// restart game.
	"SPAWN",			// spawn new sprite.
	"REMOVE",			// remove this sprite.
	"GETRANDOM",		// variable.
	"ELSE",				// else.
	"SHOWSCORE",		// show score.
	"SCORE",			// increase score.
	"SOUND",			// play sound.
	"BEEP",				// play beeper sound.
	"CLS",				// clear screen.
	"BORDER",			// set border.
	"COLOUR",			// set all attributes.
	"DELAY",			// pause for a while.
	"MESSAGE",			// display a message.
	"MENU",				// menu in a box.
	"INV",				// inventory menu.
	"KILL",				// kill the player.
	"ADD",				// add to variable.
	"SUBTRACT",			// subtract from variable.
	"DISPLAY",			// display number.
	"SCREENUP",			// up a screen.
	"SCREENDOWN",		// down a screen.
	"SCREENLEFT",		// left a screen.
	"SCREENRIGHT",		// right a screen.
	"WAITKEY",			// wait for keypress.
	"TABLEJUMP",		// jump.
	"TABLEFALL",		// fall.
	"OTHER",			// select second collision sprite.
	"SPAWNED",			// select spawned sprite.
	"ENDSPRITE",		// select original sprite.
	"ENDGAME",			// end game with victory.
	"GET",				// get object.
	"PUT",				// drop object.
	"DETECTOBJ",		// detect object.
	"ASM",			    // encode.
	"EXIT",				// exit.
	"REPEAT",			// repeat.
	"ENDREPEAT",		// endrepeat.
	"MULTIPLY",			// multiply.
	"DIVIDE",			// divide.
	"SPRITEINK",		// set sprite ink.
	"TRAIL",			// leave a trail.
	"LASER",			// shoot a laser.
	"EXPLODE",			// start a shrapnel explosion.
	"FADE",			    // fade unsupported.
	"TICKER",			// ticker message.
	"MESSAGE",			// big message.
	"REDRAW",			// redraw the play area.
	"SILENCE",			// silence AY channels.
	"WAITKEY",			// waitkey
	"DIG",				// dig.
	"STAR",				// star.
	"SHOWSCORE",			// show score double-height.
	"STOPFALL"			// stop falling.
};
const String cEvent[] =
{
	"PLAYER",			// player.
	"SPRITETYPE1",		// sprite type 1.
	"SPRITETYPE2",		// sprite type 2.
	"SPRITETYPE3",		// sprite type 3.
	"SPRITETYPE4",		// sprite type 4.
	"SPRITETYPE5",		// sprite type 5.
	"SPRITETYPE6",		// sprite type 6.
	"SPRITETYPE7",		// sprite type 7.
	"SPRITETYPE8",		// sprite type 8.
	"INITSPRITE",		// initialise sprite.
	"MAINLOOP1",		// main loop 1.
	"MAINLOOP2",		// main loop 2.
	"INTROMENU",		// main menu.
	"GAMEINIT",			// game initialisation.
	"RESTARTSCREEN",	// restart a screen.
	"FELLTOOFAR",		// sprite fell too far.
	"KILLPLAYER",		// kill player.
	"LOSTGAME",			// game over.
	"COMPLETEDGAME",	// won game.
	"NEWHIGHSCORE",		// new high score.
	"COLLECTBLOCK"		// collected block.
};
//---------------------------------------------------------------------------
__fastcall Importer::Importer()
{
}
//---------------------------------------------------------------------------
bool __fastcall Importer::Convert(const String& file)
{
    auto ext = TPath::GetExtension(file).LowerCase();
    bool loaded = false;
    if (ext == ".sna")
    {
        m_Snapshot = std::make_unique<ZXSpectrum48KSnapshot>();
        loaded = m_Snapshot->Load(file);
        if (m_Snapshot && !loaded)
        {
            m_Snapshot = std::make_unique<ZXSpectrum128KSnapshot>();
            loaded = m_Snapshot->Load(file);
        }
    }
    else if (ext == ".szx" || ext == ".zx-state")
    {
        // TODO: CPC?
        return false;
    }

    if (loaded)
	{
        Version = m_Snapshot->Version;
        // get values
        m_ValueOf.WindowPosition.Y = m_Snapshot->Byte(m_Snapshot->Location.Window+0);
        m_ValueOf.WindowPosition.X = m_Snapshot->Byte(m_Snapshot->Location.Window+1);
        m_ValueOf.WindowSize.cy = m_Snapshot->Byte(m_Snapshot->Location.Window+2);
        m_ValueOf.WindowSize.cx = m_Snapshot->Byte(m_Snapshot->Location.Window+3);
        m_ValueOf.Rooms = m_Snapshot->Byte(m_Snapshot->Location.NumberOfScreens);

        // get addresses
        m_AddressOf.SpriteFrames = m_Snapshot->Word(m_Snapshot->Location.Pointers + 0);
        m_AddressOf.nSpriteFrames = m_AddressOf.SpriteFrames - m_Snapshot->Location.StuffToSkip;
        m_AddressOf.SpriteData = m_Snapshot->Word(m_Snapshot->Location.Pointers + 2);
        m_AddressOf.nSpriteData = m_AddressOf.SpriteData - m_Snapshot->Location.StuffToSkip;
        m_AddressOf.BlockImageData = m_Snapshot->Word(m_Snapshot->Location.Pointers + 4);
        m_AddressOf.nBlockImageData = m_AddressOf.BlockImageData - m_Snapshot->Location.StuffToSkip;
        m_AddressOf.BlockAttrsData = m_Snapshot->Word(m_Snapshot->Location.Pointers + 6);
        m_AddressOf.nBlockAttrsData = m_AddressOf.BlockAttrsData - m_Snapshot->Location.StuffToSkip;
        m_AddressOf.BlockPropsData = m_Snapshot->Word(m_Snapshot->Location.Pointers + 8);
        m_AddressOf.nBlockPropsData = m_AddressOf.BlockPropsData - m_Snapshot->Location.StuffToSkip;
        m_AddressOf.ScreenLayouts = m_Snapshot->Word(m_Snapshot->Location.Pointers + 10);
        m_AddressOf.nScreenLayouts = m_AddressOf.ScreenLayouts - m_Snapshot->Location.StuffToSkip;
        m_AddressOf.EnemyPositions = m_Snapshot->Word(m_Snapshot->Location.Pointers + 12);
        m_AddressOf.nEnemyPositions = m_AddressOf.EnemyPositions - m_Snapshot->Location.StuffToSkip;
        //m_AddressOf.AYSoundFx = m_Snapshot->Word(m_Snapshot->Location.Pointers + 14);
        //m_AddressOf.nAYSoundFx = m_AddressOf.AYSoundFx - m_Snapshot->Location.StuffToSkip;
        m_AddressOf.Messages = m_Snapshot->Word(m_Snapshot->Location.Pointers + 16);
        m_AddressOf.nMessages = m_AddressOf.Messages - m_Snapshot->Location.StuffToSkip;
        m_AddressOf.Objects = m_Snapshot->Word(m_Snapshot->Location.Pointers + 18);
        m_AddressOf.nObjects = m_AddressOf.Objects - m_Snapshot->Location.StuffToSkip;
        m_AddressOf.EventCode = m_Snapshot->Word(m_Snapshot->Location.Pointers + 20);
        m_AddressOf.nEventCode = m_AddressOf.EventCode - m_Snapshot->Location.StuffToSkip;
        m_AddressOf.Keys = m_Snapshot->Location.Keys;
        m_AddressOf.JumpTable = m_Snapshot->Location.JumpTable;
        m_AddressOf.Font = 	m_Snapshot->Location.Font;
        m_AddressOf.StartScreen = m_Snapshot->Location.StartScreen;
        m_AddressOf.SpriteSize = m_Snapshot->Location.SpriteSize;

        ImportWindow();
        ImportMessages();
        ImportKeys();
        ImportJumpTable();
        ImportFont();
        ImportBlocks();
        ImportSprites();
        ImportObjects();
        ImportScreens();
        ImportMap();
        ImportEvents();
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
void __fastcall Importer::ImportWindow()
{
    m_Window = IntToStr((int)m_ValueOf.WindowPosition.Y) + " " + IntToStr((int)m_ValueOf.WindowPosition.X) + " " + IntToStr((int)m_ValueOf.WindowSize.cy) + " " + IntToStr((int)m_ValueOf.WindowSize.cx);
}
//---------------------------------------------------------------------------
void __fastcall Importer::ImportMessages()
{
    m_Messages = "definemessages ";
	auto nCount = m_AddressOf.nObjects - m_AddressOf.nMessages;
	auto addr = m_AddressOf.nMessages;

	while (nCount > 0)
	{
        String msg = "";
		while (m_Snapshot->Byte(addr) < 128)
		{
            auto byte = m_Snapshot->Byte(addr++);
            msg += (char)byte;
			if (byte == 34)						/* quotation marks? */
			{
                msg += (char)byte;
			}
			nCount--;
		}
        msg += (char)(m_Snapshot->Byte(addr++) & 0x7F);

        m_Messages += "\"" + msg + "\"ª";
		nCount--;
	}
}
//---------------------------------------------------------------------------
void __fastcall Importer::ImportKeys()
{
    m_Keys = "";
    for (auto i = 0; i < 7; i++)
    {
        char chr = cKeyTab[m_Snapshot->Byte(m_AddressOf.Keys+i)];
        m_Keys += String(chr);
    }
}
//---------------------------------------------------------------------------
void __fastcall Importer::ImportJumpTable()
{
    m_JumpTable = "";
    auto jt = m_AddressOf.JumpTable;
    auto j = 0;
    do
    {
        j = m_Snapshot->Byte(jt++);
        m_JumpTable += IntToStr(j) + " ";
    }
    while (j != 99);
}
//---------------------------------------------------------------------------
void __fastcall Importer::ImportFont()
{
    m_Font = "";
	auto addr = m_AddressOf.Font;
	for (auto i = 0; i < 96 * 8; i++)
	{
        m_Font += " ";
        m_Font += IntToStr(m_Snapshot->Byte(addr++));
	}
}
//---------------------------------------------------------------------------
void __fastcall Importer::ImportBlocks()
{
	String cBlock[14] =
	{
		"EMPTYBLOCK",		// empty space.
		"PLATFORMBLOCK",	// platform.
		"WALLBLOCK",		// wall.
		"LADDERBLOCK",		// ladder.
		"FODDERBLOCK",		// fodder.
		"DEADLYBLOCK",		// deadly.
		"CUSTOMBLOCK",		// custom.
	};

	auto imgAddr = m_AddressOf.nBlockImageData;
	auto propAddr = m_AddressOf.nBlockPropsData;
	auto attrAddr = m_AddressOf.nBlockAttrsData;
	auto defaultAttr = 71;
	auto nCount = propAddr - attrAddr;

	while (nCount-- > 0)
	{
        m_Blocks += "defineblock " + cBlock[m_Snapshot->Byte(propAddr++)];
		for ( auto i = 0; i < 8; i++ )
		{
            m_Blocks += " " + IntToStr(m_Snapshot->Byte(imgAddr++));
		}
        m_Blocks += " " + IntToStr(m_Snapshot->Byte(attrAddr++)) + "ª";
	}
}
//---------------------------------------------------------------------------
void __fastcall Importer::ImportSprites()
{
	auto nCount = m_AddressOf.nBlockImageData - m_AddressOf.nSpriteData;
	auto cFrm = m_AddressOf.nSpriteFrames + 1;	                                /* number of frames. */
	auto cImg = m_AddressOf.nSpriteData; 							            /* sprite image data. */
	auto nGroups = (m_AddressOf.nSpriteData - m_AddressOf.nSpriteFrames) / 2;   /* total sprite groups. */

    auto h = m_Snapshot->Byte(m_AddressOf.SpriteSize);
    m_SpriteHeight = h;
    if (h != 16 && h != 24)
    {
        h = 16;
    }
	while (nGroups-- > 0)
	{
		auto nFrames = m_Snapshot->Byte(cFrm);
        m_Sprites += "definesprite " + IntToStr(nFrames);

		while (nFrames-- > 0)
		{
			for (auto i = 0; i < h * 2; i++ )
			{
                m_Sprites += " " + IntToStr(m_Snapshot->Byte(cImg++));
			}
			nCount -= 8 * h;
			cImg += 6 * h;									/* skip shifted versions. */
		}
		cFrm += 2;
        m_Sprites += "ª";
	}
}
//---------------------------------------------------------------------------
void __fastcall Importer::ImportObjects()
{
	auto defaultAttr = 71;
	auto nCount = m_AddressOf.nEventCode - m_AddressOf.nObjects - 1;			/* ignore marker at end of objects. */
	auto cObj = m_AddressOf.nObjects;								/* point to first object. */
    String data;

	while ( nCount > 0 )
	{
		for (auto i = 0; i < 32; i++)
		{
            data += " " + IntToStr(m_Snapshot->Byte(cObj++));
		}

		cObj += 3;
        // attr
        m_Objects += "defineobject " + IntToStr(defaultAttr);
        // room
        m_Objects += " " + IntToStr(m_Snapshot->Byte(cObj++));
        // x
        m_Objects += " " + IntToStr(m_Snapshot->Byte(cObj++));
        // y
        m_Objects += " " + IntToStr(m_Snapshot->Byte(cObj++));
        m_Objects += data + "ª";
        data = "";
		nCount -= 38;
	}
}
//---------------------------------------------------------------------------
void __fastcall Importer::ImportScreens()
{
    auto nExpand = 0;
	auto nByteToExpand = 0;
	auto nThisRoom = 0;
	auto cScrn = m_AddressOf.nScreenLayouts + (m_ValueOf.Rooms * 2);			/* skip to room data. */
	auto cNme = m_AddressOf.nEnemyPositions;
    auto cByte = 0;

    auto nRow = 0;
    auto nCol = 0;
	while (nThisRoom < m_ValueOf.Rooms)
	{
        nRow = 0;
        nCol = 0;

		while (nRow < m_ValueOf.WindowSize.cy)
		{
			nCol = 0;

			if (nRow == 0)
			{
				m_Screens += "ªdefinescreen ";
			}

			while (nCol++ < m_ValueOf.WindowSize.cx)
			{
				if (nExpand > 0)
				{
					cByte = nByteToExpand;
					nExpand--;
				}
				else
				{
					cByte = m_Snapshot->Byte(cScrn++);
					if (cByte == 255)					/* start of compressed data block. */
					{
						cByte = nByteToExpand = m_Snapshot->Byte(cScrn++);
						nExpand = m_Snapshot->Byte(cScrn++);
						if (nExpand == 0)				/* 0 indicates 256 bytes to follow. */
						{
							nExpand = 256;
						}
						nExpand--;
					}
				}
                m_Screens += " " + IntToStr(cByte);
			}
			nRow++;
		}

		nThisRoom++;
		cByte = m_Snapshot->Byte(cNme++);
		while (cByte != 255)
		{
			m_Screens += " spriteposition  " + IntToStr(cByte);
    		cByte = m_Snapshot->Byte(cNme++);
            m_Screens += " " + IntToStr(cByte);
    		cByte = m_Snapshot->Byte(cNme++);
            m_Screens += " " + IntToStr(cByte);
    		cByte = m_Snapshot->Byte(cNme++);
            m_Screens += " " + IntToStr(cByte);
    		cByte = m_Snapshot->Byte(cNme++);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall Importer::ImportMap()
{
    const int MapWidth = 11;
    const int MapHeight = 8;
	auto nMap = m_AddressOf.nSpriteFrames - 100;
	auto nStart = m_AddressOf.StartScreen;
	auto cMap = nStart;
	auto nRoom = m_Snapshot->Byte(cMap);
	cMap = nMap + nRoom;
	m_Map = "map startscreen " + IntToStr(m_Snapshot->Byte(cMap));
	cMap = nMap;

    auto left = 11;
    auto right = 0;
    auto top = 8;
    auto bottom = 0;
    // write out the entire map
    for (auto y = 0; y < MapHeight; y++)
    {
        for (auto x = 0; x < MapWidth; x++ )
        {
            unsigned char room = m_Snapshot->Byte(cMap++);
            m_Map += " " + IntToStr(room);
            if (room != 255)
            {
                left = std::min(left, x);
                right = std::max(right, x);
                top = std::min(top, y);
                bottom = std::max(bottom, y);
            }
        }
    }
    m_Map += " left " + IntToStr(left) + " right " + IntToStr(right);
    m_Map += " top " + IntToStr(top) + " bottom " + IntToStr(bottom);
}
//---------------------------------------------------------------------------
void __fastcall Importer::ConvertByte(unsigned char& cByte) const
{
	/* There were no S and T variables vefore version 4.7 */
	if (m_Snapshot->Version < 47)
	{
		if (cByte > INS_R && cByte != 255)
		{
			cByte += 2;
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall Importer::WriteText(const String& text)
{
    m_Events += text;
}
//---------------------------------------------------------------------------
void __fastcall Importer::CodeLine(char cInstruction, char cNext)
{
	auto nIndents = 0;

	switch(cInstruction)
	{
		case INS_ELSE:
		case INS_IF:
		case INS_REPEAT:
			nTabs++;
			break;
		case INS_BIGMESSAGE:
		case INS_BIGSCORE:
			nPrintMode++;
			WriteText( "PRINTMODE 1" );
			NewLine();
			for (auto nIndents = 0; nIndents < nTabs; nIndents++ )
			{
				WriteText( "    " );
			}
			break;
	}

	WriteText(cKeyword[cInstruction]);

	switch( cInstruction )
	{
		case INS_PUT:									/* put now takes two arguments for x and y. */
			WriteText( " X Y" );
			break;
	}
}
//---------------------------------------------------------------------------
void __fastcall Importer::NewLine()
{
    m_Events += "\r\n";
}
//---------------------------------------------------------------------------
void __fastcall Importer::ImportEvents()
{
	auto nMaxEvents = 20;//Version == 46 ? 20 : 21;
	auto nEvents = 21;
	auto nThisEvent = 0;
	auto cNme = m_AddressOf.nEventCode + 1;							/* first byte is always end marker. */

	auto cNewLine = 1;
	while (nEvents-- > 0)
	{
		cNewLine = 1;
		nTabs = 0;
		NewLine();
		WriteText("EVENT ");
		WriteText(cEvent[nThisEvent]);
		if (nThisEvent++ < nMaxEvents)
		{
			auto cByte = m_Snapshot->Byte(cNme++);
			ConvertByte(cByte);
			auto nPrintMode = 0;

			while (cByte < 255)
			{
				if (cNewLine > 0 && cByte != INS_NUMBER)
				{
					cNewLine = 0;
					NewLine();
					auto nIndents = nTabs;

					while (nIndents-- > 0)
					{
						WriteText( "    " );
					}

					if (nPrintMode > 0)
					{
						nPrintMode = 0;
						WriteText( "PRINTMODE 0" );
						NewLine();
						for (auto nIndents = 0; nIndents < nTabs; nIndents++)
						{
							WriteText( "    " );
						}
					}
				}
				else
				{
					WriteText(" ");
				}

				if (cByte == INS_NUMBER)
				{
					cByte = m_Snapshot->Byte(cNme++);
					WriteText(IntToStr(cByte));
				}
				else
				{
					CodeLine(cByte, m_Snapshot->Byte(cNme));
				}
				if (cByte == INS_KEY)
				{
					cByte = m_Snapshot->Byte(cNme++);
					if (cByte == INS_NUMBER)
					{
						WriteText( " " );
						cByte = m_Snapshot->Byte(cNme++);
						switch(cByte)
						{
							case 0:
								WriteText( "RIGHT" );
								break;
							case 1:
								WriteText( "LEFT" );
								break;
							case 2:
								WriteText( "DOWN" );
								break;
							case 3:
								WriteText( "UP" );
								break;
							case 4:
								WriteText( "FIRE" );
								break;
							case 5:
								WriteText( "FIRE2" );
								break;
							case 6:
								WriteText( "FIRE3" );
								break;
							default:
								WriteText(IntToStr(cByte) + "   ; warning - unrecognised key");
								break;
						}
					}
					else
					{
						cNme--;
					}
				}

				cByte = m_Snapshot->Byte(cNme++);
				ConvertByte(cByte);

				switch( cByte )
				{
					case INS_ELSE:
					case INS_ENDIF:
					case INS_ENDREPEAT:
						nTabs--;
						if (nTabs < 0)
						{
							nTabs = 0;
							NewLine();
							WriteText( "; warning - ENDIF without IF?" );
						}
						break;
				}

				if ((cByte < INS_ARGUMENTS) || (cByte >= INS_LET && cByte != INS_BY))
				{
					cNewLine++;
				}
			}
		}
		NewLine();

		while (nTabs > 0)
		{
			auto nIndents = --nTabs;
			while (nIndents-- > 0)
			{
				WriteText( "    " );
			}
			WriteText( "ENDIF        ; warning - missing ENDIF?" );
			NewLine();
		}
	}

	NewLine();
}
//---------------------------------------------------------------------------
