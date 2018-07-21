//---------------------------------------------------------------------------
#ifndef JsonFileH
#define JsonFileH
//---------------------------------------------------------------------------
#include <map>
#include <System.Json.Writers.hpp>
#include "Path.h"
#include "File.h"
//---------------------------------------------------------------------------
namespace System
{
//---------------------------------------------------------------------------
// This class and it's descendants MUST BE instantiated dynamically using 'new'
//---------------------------------------------------------------------------
class JsonFile : public TPersistent
{
private:
    TStringWriter*              m_StringWriter;
    TJsonTextWriter*            m_JsonWriter;
    String                      m_File;

protected:
    // map member variables to property names
    // the loader will set the value based on the property type
    std::map<String, void*>     m_PropertyMap;

protected:
    void            __fastcall  Open(const String& file);
    void            __fastcall  Close();
    // write (on save)
    void            __fastcall  StartObject() const;
    void            __fastcall  EndObject() const;
    void            __fastcall  Push(const String& section) const;
    void            __fastcall  Pop() const;
    void            __fastcall  ArrayStart(const String& property) const;
    void            __fastcall  ArrayEnd() const;
    void            __fastcall  Write(const String& value) const;
    void            __fastcall  Write(const int& value) const;
    void            __fastcall  Write(const String& property, const String& value) const;
    void            __fastcall  Write(const String& property, const int& value) const;
    void            __fastcall  Write(const String& property, const unsigned int& value) const;
    void            __fastcall  Write(const String& property, const float& value) const;
    void            __fastcall  Write(const String& property, const bool& value) const;
    // read (on load)
    virtual void    __fastcall  Set(const String& property, const String& value);
    virtual void    __fastcall  Set(const String& property, const int& value);
    virtual void    __fastcall  Set(const String& property, const unsigned int& value);
    virtual void    __fastcall  Set(const String& property, const float& value);
    virtual void    __fastcall  Set(const String& property, const bool& value);

            String  __fastcall  ProcessPath(const String& path) const;
    virtual void    __fastcall  Load(const String& file);

    virtual void    __fastcall  OnStartObject(const String& object);
    virtual void    __fastcall  OnEndObject(const String& object);

public:
                    __fastcall  JsonFile();
    virtual         __fastcall ~JsonFile();
};
//---------------------------------------------------------------------------
} // namespace File
//---------------------------------------------------------------------------
#endif