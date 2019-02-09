//---------------------------------------------------------------------------
#include "agdv.pch.h"
#include "Snapshot.h"
#include "ErrorReporter.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
__fastcall Snapshot::Snapshot(CpuEndianness endianness, unsigned int snapshotSize)
: m_Endianness(endianness)
, m_SnapshotSize(snapshotSize)
{
}
//---------------------------------------------------------------------------
void __fastcall Snapshot::ReadBytes(const String& file, std::vector<unsigned char>& contents) const
{
    auto allBytes = System::Ioutils::TFile::ReadAllBytes(file);
    for (auto byte : allBytes)
    {
        contents.push_back(byte);
    }
}
//---------------------------------------------------------------------------
bool __fastcall Snapshot::Load(const String& file)
{
    g_ErrorReporter.Clear();
    if (TFile::Exists(file))
    {
        ReadBytes(file, m_Memory);
        if (m_Memory.size() > 0)
        {
            if (ValidateSize(m_Memory))
            {
                auto address = Word(Location.PointersA);
                if (address == 36924)
                {
                    m_Version = 46;
                    Location.Pointers = Location.PointersA;
                    Location.Keys = Location.KeysA;
                    Location.StartScreen = Location.StartScreenA;
                    Location.JumpTable = Location.JumpTableA;
                }
                else
                {
                    m_Version = 47;
                    Location.Pointers = Location.PointersB;
                    Location.Keys = Location.KeysB;
                    Location.StartScreen = Location.StartScreenB;
                    Location.JumpTable = Location.JumpTableB;
                }
                return true;
            }
            g_ErrorReporter.Add("Error: File is not a valid " + m_Snapshoter + " snapshot file. [" + file + "]");
            return false;
        }
        g_ErrorReporter.Add("Error: File: is empty. [" + file + "]");
        return false;
    }
    g_ErrorReporter.Add("Error: File was not found. [" + file + "]");
    return false;
}
//---------------------------------------------------------------------------
UInt8 __fastcall Snapshot::Byte(unsigned int address) const
{
    if (address < m_Memory.size())
    {
        return m_Memory[address];
    }
    return 0;
}
//---------------------------------------------------------------------------
UInt16 __fastcall Snapshot::Word(unsigned int address) const
{
    auto b1 = m_Memory[address+0] * (m_Endianness == LittleEndian ? 1 : 256);
    auto b2 = m_Memory[address+1] * (m_Endianness == LittleEndian ? 256 : 1);
    auto little = m_Memory[address+0] + (256 * m_Memory[address+1]);
    auto big = m_Memory[address+1] + (256 * m_Memory[address+0]);
    return m_Endianness == LittleEndian ? little : big;
}
//---------------------------------------------------------------------------
bool __fastcall Snapshot::ValidateSize(const std::vector<unsigned char>& memory)
{
    return memory.size() == m_SnapshotSize;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
const int Z80v1 = 65536 + 30;
//---------------------------------------------------------------------------
__fastcall ZXSpectrumZ80::ZXSpectrumZ80()
: Snapshot(LittleEndian, 0)
{
    m_Snapshoter = "ZX Spectrum 48K";
    if (m_Version == 1)
    {
        m_Locations.StuffToSkip = 16384 - 27;
        m_Locations.Font = 31232 - m_Locations.StuffToSkip;
        m_Locations.Map = 31959 - m_Locations.StuffToSkip;
        m_Locations.Window = 32008 - m_Locations.StuffToSkip;
        m_Locations.NumberOfScreens = 32018 - m_Locations.StuffToSkip;
        m_Locations.PointersA = 32057 - m_Locations.StuffToSkip;
        m_Locations.PointersB = 32059 - m_Locations.StuffToSkip;
        m_Locations.Pointers = m_Locations.PointersB;
        m_Locations.KeysA = 35331 - m_Locations.StuffToSkip;
        m_Locations.KeysB = 35339 - m_Locations.StuffToSkip;
        m_Locations.Keys = m_Locations.KeysB;
        m_Locations.JumpTableA = 36217 - m_Locations.StuffToSkip;
        m_Locations.JumpTableB = 36238 - m_Locations.StuffToSkip;
        m_Locations.JumpTable = m_Locations.JumpTableB;
        m_Locations.StartScreenA = 33580 - m_Locations.StuffToSkip;
        m_Locations.StartScreenB = 33615 - m_Locations.StuffToSkip;
        m_Locations.SpriteSize = 35931 - m_Locations.StuffToSkip;
    }
}
//---------------------------------------------------------------------------
bool __fastcall ZXSpectrumZ80::ValidateSize(const std::vector<unsigned char>& memory)
{
    if (memory.size() == Z80v1)
    {
        m_Header = 30;
        m_Version = 1;
        return true;
    }
    else if (memory[6] == 0 && memory[7] == 0)
    {
        m_Header = 87;
        // possible version 2, 3
        if (memory[30] == 23)
        {
            m_Version = 2;
            return true;
        }
        else if (memory[30] == 54 || memory[30] == 55)
        {
            m_Version = 3;
            return true;
        }
    }
    return false;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
const unsigned int ZX_48K_SNAPSHOT_SIZE = 27 + 49152;
//---------------------------------------------------------------------------
__fastcall ZXSpectrum48KSnapshot::ZXSpectrum48KSnapshot()
: Snapshot(LittleEndian, ZX_48K_SNAPSHOT_SIZE)
{
    m_Snapshoter = "ZX Spectrum 48K";
    m_Locations.StuffToSkip = 16384 - 27;
    m_Locations.Font = 31232 - m_Locations.StuffToSkip;
    m_Locations.Map = 31959 - m_Locations.StuffToSkip;
    m_Locations.Window = 32008 - m_Locations.StuffToSkip;
    m_Locations.NumberOfScreens = 32018 - m_Locations.StuffToSkip;
    m_Locations.Pointers = 32059 - m_Locations.StuffToSkip;
    m_Locations.PointersA = 32057 - m_Locations.StuffToSkip;
    m_Locations.PointersB = 32059 - m_Locations.StuffToSkip;
    m_Locations.KeysA = 35331 - m_Locations.StuffToSkip;
    m_Locations.KeysB = 35339 - m_Locations.StuffToSkip;
    m_Locations.Keys = m_Locations.KeysB;
    m_Locations.JumpTableA = 36217 - m_Locations.StuffToSkip;
    m_Locations.JumpTableB = 36238 - m_Locations.StuffToSkip;
    m_Locations.JumpTable = m_Locations.JumpTableB;
    m_Locations.StartScreenA = 33580 - m_Locations.StuffToSkip;
    m_Locations.StartScreenB = 33615 - m_Locations.StuffToSkip;
    m_Locations.SpriteSize = 35931 - m_Locations.StuffToSkip;
}
//---------------------------------------------------------------------------
const unsigned int ZX_128K_SNAPSHOT_SIZE = 131103;//24 + 81920 + 3;
//---------------------------------------------------------------------------
__fastcall ZXSpectrum128KSnapshot::ZXSpectrum128KSnapshot()
: ZXSpectrum48KSnapshot()
{
    m_SnapshotSize = ZX_128K_SNAPSHOT_SIZE;
    m_Snapshoter = "ZX Spectrum 128K";
}
//---------------------------------------------------------------------------
const unsigned int CPC_SNAPSHOT_SIZE = 0;
//---------------------------------------------------------------------------
__fastcall AmstradCPCSnapshot::AmstradCPCSnapshot()
: Snapshot(LittleEndian, CPC_SNAPSHOT_SIZE)
{
    m_Snapshoter = "Amstrad CPC";
    // TODO: fill out
    m_Locations.Font = 0;
    m_Locations.Map = 0;
    m_Locations.Window = 0;
    m_Locations.NumberOfScreens = 0;
    m_Locations.Pointers = 0;
    m_Locations.PointersA = 0;
    m_Locations.PointersB = 0;
    m_Locations.Keys= 0;
	m_Locations.KeysA = 0;
    m_Locations.KeysB = 0;
    m_Locations.JumpTable = 0;
    m_Locations.JumpTableA = 0;
    m_Locations.JumpTableB = 0;
    m_Locations.StartScreenA = 0;
    m_Locations.StartScreenB = 0;
    m_Locations.StartScreen = 0;
    m_Locations.StuffToSkip = 0;
}
//---------------------------------------------------------------------------
