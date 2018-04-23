//---------------------------------------------------------------------------
#ifndef ErrorReporterH
#define ErrorReporterH
//---------------------------------------------------------------------------
class ErrorReporter
{
private:
    TMemo*              m_Memo;
    String              m_LastMessage;
public:
            __fastcall  ErrorReporter();

    void    __fastcall  Clear();
    void    __fastcall  Add(const String& message);

    __property  TMemo*  Memo = { read = m_Memo, write = m_Memo };
};
//---------------------------------------------------------------------------
extern PACKAGE ErrorReporter& g_ErrorReporter;
//---------------------------------------------------------------------------
#endif
