//---------------------------------------------------------------------------
#ifndef ProjectH
#define ProjectH
//---------------------------------------------------------------------------
#include <vector>
#include "JsonFile.h"
#include "Window.h"
//---------------------------------------------------------------------------
class Project : public JsonFile
{
private:
    struct FileEntry
    {
        String  Name;
        String  Type;
        String  SubType;
    };
    typedef std::vector<FileEntry>  FilesList;

    String              m_Name;
    String              m_Machine;
    FilesList           m_Files;
    Window              m_Window;

public:
            __fastcall  Project();

    void    __fastcall  Create(const String& name, const String& machine);
    void    __fastcall  Save();
    void    __fastcall  AddFile(const String& name, const String& type, const String& subType, bool create = true);
    Window  __property  WindowInfo = { read = m_Window, write = m_Window };
};
//---------------------------------------------------------------------------
#endif
