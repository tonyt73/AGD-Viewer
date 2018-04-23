//---------------------------------------------------------------------------
#include "agdv.pch.h"
#include "ErrorReporter.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
ErrorReporter s_ErrorReporter;
ErrorReporter& g_ErrorReporter = s_ErrorReporter;
//---------------------------------------------------------------------------
__fastcall ErrorReporter::ErrorReporter()
{
}
//---------------------------------------------------------------------------
void __fastcall ErrorReporter::Clear()
{
    m_Memo->Lines->Clear();
    m_Memo->Parent->Visible = false;
}
//---------------------------------------------------------------------------
void __fastcall ErrorReporter::Add(const String& message)
{
    if (message != m_LastMessage)
    {
        m_Memo->Lines->Add(message);
        m_Memo->Parent->Visible = true;
        m_LastMessage = message;
    }
}
//---------------------------------------------------------------------------
