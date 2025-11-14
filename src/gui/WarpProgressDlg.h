#ifndef _WARP_PROGRESSDLG_H
#define _WARP_PROGRESSDLG_H

#include <wx/wx.h>
#include <wx/dialog.h>
#include "../Warp.h"

const int wxEVT_WARP_LOG = 6144;
const int wxEVT_WARP_END = 6145;

class WarpThread : public wxThread, public Log
{
protected:
    wxEvtHandler* _parent;
    Warp& _warp;
public:
    WarpThread(wxEvtHandler* parent, Warp& warp );
    virtual ~WarpThread();
    wxThread::ExitCode Entry();

    virtual void Add( const string& value );
};

class WarpProgressDialog : public wxDialog
{
public:
    WarpProgressDialog( wxWindow* parent, int id, Warp& warp );
    virtual ~WarpProgressDialog();
private:
    wxTextCtrl* _output;
    Warp& _warp;
    WarpThread* _warpThread;

    void OnInitDialog( wxInitDialogEvent& event );
    void OnClose( wxCommandEvent& event );
    void OnLog( wxCommandEvent& event );
    void OnWarpEnd( wxCommandEvent& event );

    DECLARE_EVENT_TABLE()
};

#endif //_WARP_PROGRESSDLG_H