#include "GentilesDlg.h"
#include <sstream>

BEGIN_EVENT_TABLE(GentilesDlg, BaseDlg)
    EVT_BUTTON(wxID_ANY, GentilesDlg::OnButton)
    EVT_COMMAND(wxID_ANY, wxEVT_GENTILES_LOG, GentilesDlg::OnLog)
    EVT_COMMAND(wxID_ANY, wxEVT_GENTILES_END, GentilesDlg::OnTilesEnd)
    EVT_COMMAND(wxID_ANY, wxEVT_GENTILES_PROGRESS, GentilesDlg::OnProgress)
    EVT_COMMAND(wxID_ANY, wxEVT_GENTILES_LENGTH, GentilesDlg::OnLength )
END_EVENT_TABLE()

const int OUTPUT_LINES = 2; // Display this many most recent lines in the output
const int MAX_ZOOM = 20;    // OSM doesn't support more than this


GentilesDlg::GentilesDlg() :
    BaseDlg( _( "gentiles" ), wxDefaultPosition, wxSize( 400, -1 ) ),
    _runner( TileJob::TargetThreads() ),
    _thread( nullptr ),
    _current( 0 ),
    _length( 0 )
{
    wxBoxSizer *topSizer = new wxBoxSizer( wxVERTICAL );

    wxSize textSize( TEXT_WIDTH, -1 );

    wxBoxSizer *inputSizer = new wxBoxSizer( wxHORIZONTAL );
    wxButton *inputBrowse = new wxButton( this, ID_InputFilesBrowse, _( "Browse" ) );
    const int inputBrowseWidth = inputBrowse->GetSize().GetWidth();
    inputSizer->Add( new wxStaticText( this, wxID_ANY, ( "Input files" ), wxDefaultPosition,
        wxSize( LABEL_WIDTH - inputBrowseWidth, -1 ) ) );
    _inputFiles = new wxTextCtrl( this, ID_InputFiles, wxEmptyString, wxDefaultPosition, textSize, wxTE_MULTILINE );
    inputSizer->Add( _inputFiles );
    inputSizer->Add( inputBrowse );
    topSizer->Add( inputSizer );

    _boundsFile = new wxFilePickerCtrl( this, ID_BoundsFile, wxEmptyString, _( "Select bounds file"), _("*.csv"), wxDefaultPosition, textSize );
    _outputFolder = new wxDirPickerCtrl( this, ID_OutputFolder, wxEmptyString, wxDirSelectorPromptStr, wxDefaultPosition, textSize );
    _minZoom = new wxTextCtrl( this, ID_MinZoom, _( "3" ), wxDefaultPosition, textSize );
    _maxZoom = new wxTextCtrl( this, ID_MaxZoom, _( "12" ), wxDefaultPosition, textSize );

    AddLine( topSizer, _boundsFile, _( "Bounds file" ) );
    AddLine( topSizer, _outputFolder, _( "Output folder" ) );
    AddLine( topSizer, _minZoom, _( "Minimum zoom" ) );
    AddLine( topSizer, _maxZoom, _( "Maximum zoom" ) );

    for( int i = 0; i < OUTPUT_LINES; i++ ) {
        wxStaticText* next = new wxStaticText( this, wxID_ANY, wxEmptyString ); 
        _output.push_back( next );
        topSizer->Add( next, wxEXPAND );
    }

    _progressBar = new wxGauge( this, ID_MinZoom, 100, wxDefaultPosition, wxSize( TEXT_WIDTH, -1 ) );
    _percentage = new wxStaticText( this, ID_Percentage, wxEmptyString );
    AddGroup( topSizer, _progressBar, _percentage, nullptr );

    _startButton = new wxButton( this, ID_Start, _( "Start" ) );
    _clearButton = new wxButton( this, ID_Clear, _( "Clear" ) );
    _closeButton = new wxButton( this, wxID_CLOSE, ("Close" ) );
    AddGroup( topSizer, _startButton, _clearButton, _closeButton, nullptr );

    SetSizerAndFit( topSizer );
}

GentilesDlg::~GentilesDlg()
{
}

bool GentilesDlg::ValidateZoom( const wxString& type, const wxTextCtrl* zoom )
{
    int iZoom = wxAtoi( zoom->GetValue() );
    if( iZoom <= 0 || iZoom > MAX_ZOOM ) {
        wxString error = _( "Invalid zoom - " ) + type;
        wxMessageBox( error );
        return false;
    }
    return true;
}

void GentilesDlg::OnButton( wxCommandEvent& event )
{
    const int id = event.GetId();
    switch( id ) {
        case wxID_CLOSE :
            if( _thread ) {
                _runner.Abort();
                _thread->Wait();
                _thread->Delete();
            }
            Destroy();
            break;
        case ID_Start :
        case ID_Clear :
            if( !ValidateZoom( _("minimum"), _minZoom ) || !ValidateZoom( _("maximum"), _maxZoom ) ) {
                return;
            }
            _runner.inputFiles.clear();
            for( wxString& next : wxSplit( _inputFiles->GetValue(), '\n')) {
                if ( next != wxEmptyString ) {
                    _runner.inputFiles.push_back( next.ToStdString() );
                }
            }
            _runner.boundsFilename = _boundsFile->GetPath();
            _runner.outFolder = _outputFolder->GetPath();
            _runner.minZoom = atoi( _minZoom->GetValue() );
            _runner.maxZoom = atoi( _maxZoom->GetValue() );
            if( _runner.minZoom < 0 || _runner.minZoom > MAX_ZOOM ) {
                wxMessageBox( _("Invalid minimum zoom"));
            }
            _runner.maxTilesLoaded = _runner.inputFiles.size();

            _startButton->Disable();
            _clearButton->Disable();

            _thread = new GentilesThread( this, _runner );
            _thread->source = id;
            _thread->Run();
            break;
        case ID_InputFilesBrowse :
            wxFileDialog openFileDialog( this, _( "Open KML file" ), wxEmptyString, wxEmptyString,
                _( "KML files (*.kml)|*.kml" ), wxFD_DEFAULT_STYLE | wxFD_MULTIPLE );
            if( wxID_OK == openFileDialog.ShowModal() ) {
                _inputFiles->Clear();
                wxArrayString filenames;
                openFileDialog.GetPaths( filenames );
                for( wxString& next : filenames ) {
                    _inputFiles->AppendText( next );
                    _inputFiles->AppendText( "\n" );
                }
            }
            break;
    }
}

void GentilesDlg::OnLog( wxCommandEvent& event )
{
    if( _output.size() > 0 ) {
        for( int i = 1; i < _output.size(); i++ ) {
            _output[ i - 1 ]->SetLabel( _output[ i ]->GetLabel() );
        }
        _output[ _output.size() - 1 ]->SetLabel( event.GetString() );
    }
}
 
void GentilesDlg::OnTilesEnd( wxCommandEvent& /*event*/ )
{
    _thread = nullptr;
    _startButton->Enable();
    _clearButton->Enable();
}

void GentilesDlg::OnProgress( wxCommandEvent& event )
{
    _current = event.GetInt();
    if( _length > 0 ) {
        double percentage = ( (double) _current / ( double )_length ) * 100.0;        
        _percentage->SetLabel( wxString::Format( _("%.2f"), percentage ) + _("%"));

    }
    _progressBar->SetValue( _current );
}

void GentilesDlg::OnLength( wxCommandEvent& event )
{
    _length = event.GetInt();
    _progressBar->SetRange( _length );
}

GentilesThread::GentilesThread(wxEvtHandler* parent, TileRunner& runner ) :
    wxThread( wxTHREAD_JOINABLE ),
    _runner( runner ),
    _parent( parent )
{

}

GentilesThread::~GentilesThread()
{

}

wxThread::ExitCode GentilesThread::Entry()
{
    _runner.logger = this;

    _runner.Init();

    switch( this->source ) {
        case ID_Start : {
            _runner.SetupTileJobs();
            wxCommandEvent evt( wxEVT_GENTILES_LENGTH, wxID_ANY );
            evt.SetInt( _runner.CountJobs() );
            _parent->AddPendingEvent( evt );
	        _runner.RunTileJobs();
            break;
        }

        case ID_Clear : {
            _runner.Clear();
            break;
        }
    }

	
    _parent->AddPendingEvent( wxCommandEvent( wxEVT_GENTILES_END, wxID_ANY ) );
    return EXIT_SUCCESS;
}

void GentilesThread::Add( const string& value )
{
    // Log something to the main window
    wxCommandEvent evt( wxEVT_GENTILES_LOG, wxID_ANY );
    evt.SetString( value + _( "\n" ) );
    _parent->AddPendingEvent( evt );
}

void GentilesThread::Progress( const int position )
{
    wxCommandEvent evt( wxEVT_GENTILES_PROGRESS, wxID_ANY );
    evt.SetInt( position );
    _parent->AddPendingEvent( evt );
}
