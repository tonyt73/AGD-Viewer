//---------------------------------------------------------------------------
#include "agdv.pch.h"
#include "Project.h"
#include "Path.h"
#include "File.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
__fastcall Project::Project()
{
}
//---------------------------------------------------------------------------
void __fastcall Project::Create(const String& name, const String& machine)
{
    m_Name = name;
    m_Machine = machine;
    System::Path::ProjectName = name;

    AddFile("Messages.txt", "Text", "Message");
    AddFile("Sound effects.sfx", "Text", "SoundFx");
    AddFile("Player control.event", "Text", "Event");
    AddFile("Sprite type 1.event", "Text", "Event");
    AddFile("Sprite type 2.event", "Text", "Event");
    AddFile("Sprite type 3.event", "Text", "Event");
    AddFile("Sprite type 4.event", "Text", "Event");
    AddFile("Sprite type 5.event", "Text", "Event");
    AddFile("Sprite type 6.event", "Text", "Event");
    AddFile("Sprite type 7.event", "Text", "Event");
    AddFile("Sprite type 8.event", "Text", "Event");
    AddFile("Game initialisation.event", "Text", "Event");
    AddFile("Initialise sprite.event", "Text", "Event");
    AddFile("Kill player.event", "Text", "Event");
    AddFile("Introduction menu.event", "Text", "Event");
    AddFile("Main loop 1.event", "Text", "Event");
    AddFile("Main loop 2.event", "Text", "Event");
    AddFile("Restart screen.event", "Text", "Event");
    AddFile("Fell too far.event", "Text", "Event");
    AddFile("Lost game.event", "Text", "Event");
    AddFile("Completed game.event", "Text", "Event");
    AddFile("New high score.event", "Text", "Event");
}
//---------------------------------------------------------------------------
void __fastcall Project::Save()
{
    auto path = System::Path::Project;
    auto file = System::File::Combine(path, m_Name + ".agdx");
    Open(file);
    Push("Project");
        Write("Version", String("1.0"));
        Write("Author", String(""));
        Write("Description", String(""));
        Write("Machine", m_Machine);
        Push("Window");
            Write("X", m_Window.x);
            Write("Y", m_Window.y);
            Write("Width", m_Window.w);
            Write("Height", m_Window.h);
        Pop(); // Window
    Pop();  // Project
    ArrayStart("Files");
    for (const auto& fi : m_Files)
    {
        StartObject();
            Write("Name", fi.Name);
            Write("Type", fi.Type);
            Write("SubType", fi.SubType);
        EndObject();
    }
    ArrayEnd(); // Files
    Close();
}
//---------------------------------------------------------------------------
void __fastcall Project::AddFile(const String& name, const String& type, const String& subType, bool create)
{
    m_Files.push_back(FileEntry {name, type, subType});
    if (create && type == "Text")
    {
        auto path = System::Path::Project;
        auto file = System::File::Combine(path, name);
        System::File::WriteText(file, "; Created by AGDv import\r\n");
    }
}
//---------------------------------------------------------------------------

