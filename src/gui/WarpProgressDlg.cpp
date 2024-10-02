#include "WarpProgressDlg.h"
#include "BaseDlg.h"
#include "../ImgMagick.h"

BEGIN_EVENT_TABLE(WarpProgressDialog, wxDialog)
    EVT_INIT_DIALOG( WarpProgressDialog::OnInitDialog)
    EVT_BUTTON(wxID_CLOSE, WarpProgressDialog::OnClose)
    EVT_COMMAND(wxID_ANY, wxEVT_WARP_LOG, WarpProgressDialog::OnLog)
    EVT_COMMAND(wxID_ANY, wxEVT_WARP_END, WarpProgressDialog::OnWarpEnd)
END_EVENT_TABLE()

WarpProgressDialog::WarpProgressDialog( wxWindow* parent, int id, Warp& warp ) :
    wxDialog( parent, id, _( "Output - in progress" ), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER ),
    _warp( warp ),
    _warpThread( nullptr )
{
    wxBoxSizer *topSizer = new wxBoxSizer( wxVERTICAL );
    _output = new wxTextCtrl( this, wxID_ANY, wxEmptyString,
        wxDefaultPosition, wxSize( TEXT_WIDTH, TEXT_HEIGHT ), wxTE_MULTILINE | wxHSCROLL );
    topSizer->Add( _output, 0, wxEXPAND | wxALL );

    wxButton *close = new wxButton( this, wxID_CLOSE, _( "Close" ) );
    topSizer->Add( close );

    SetSizerAndFit( topSizer );
}

WarpProgressDialog::~WarpProgressDialog()
{
    if( _warpThread ) {
        _warpThread->Delete();
    }
}

void WarpProgressDialog::OnInitDialog( wxInitDialogEvent& event )
{
    _output->SetValue( wxEmptyString );
    _warpThread = new WarpThread( this, _warp );
    _warpThread->Run();
}

void WarpProgressDialog::OnClose( wxCommandEvent& /* event */ )
{
    if( _warpThread ) {
        _warpThread->Delete();
    }
    _warpThread = nullptr;
    EndModal( wxID_CLOSE );
}

void WarpProgressDialog::OnLog( wxCommandEvent& event )
{
    _output->AppendText( event.GetString() );
}

void WarpProgressDialog::OnWarpEnd( wxCommandEvent& /* event */ )
{
    SetTitle( _( "Output - completed" ) );
}

WarpThread::WarpThread(wxEvtHandler* parent, Warp& warp ) :
    _parent( parent ),
    _warp( warp )
{

}

WarpThread::~WarpThread()
{

}

wxThread::ExitCode WarpThread::Entry()
{
    _warp.logger = this;
    ImgMagick::Init();
    _warp.Run();
    ImgMagick::Term();
    wxCommandEvent evt( wxEVT_WARP_END, wxID_ANY );
    _parent->AddPendingEvent( evt );
    return EXIT_SUCCESS;
}


void WarpThread::Add( const string& value )
{
    // Log something to the main window
    wxCommandEvent evt( wxEVT_WARP_LOG, wxID_ANY );
    evt.SetString( value + _( "\n" ) );
    _parent->AddPendingEvent( evt );
}
