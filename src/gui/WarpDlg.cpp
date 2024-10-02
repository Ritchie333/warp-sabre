#include "WarpDlg.h"
#include "../Warp.h"
#include "../StringUtils.h"
#include <vector>

using namespace std;

BEGIN_EVENT_TABLE(WarpDlg, BaseDlg)
    EVT_BUTTON(wxID_ANY, WarpDlg::OnButton)
END_EVENT_TABLE()

WarpDlg::WarpDlg() :
    BaseDlg( _( "warp-sabre") ),
    _progressDialog( this, ID_Warp_Dlg, _warp )
{
    wxBoxSizer *topSizer = new wxBoxSizer( wxVERTICAL );

    wxSize textSize( TEXT_WIDTH, -1 );

    _inputFile = new wxFilePickerCtrl( this, ID_InputFile, wxEmptyString, _( "Select input file"), _("*.jpg"), wxDefaultPosition, textSize );
    _pointsFile = new wxFilePickerCtrl( this, ID_PointsFile, wxEmptyString, _( "Select points file"), _("*.csv"), wxDefaultPosition, textSize );
    _outputName = new wxFilePickerCtrl( this, ID_OutputName, wxEmptyString, wxFileSelectorPromptStr, wxFileSelectorDefaultWildcardStr, 
            wxDefaultPosition, textSize, wxFLP_SAVE | wxFLP_USE_TEXTCTRL );
    _projectionType = PopulateProjectionType();
    _polynomialOrder = new wxTextCtrl( this, ID_PolynomialOrder );
    _description = new wxTextCtrl( this, ID_Description, wxEmptyString, wxDefaultPosition, wxSize( DESC_WIDTH, -1 ) );

    AddLine( topSizer, _inputFile, _( "Input file " ) );
    AddLine( topSizer, _pointsFile, _( "Points file" ) );
    AddLine( topSizer, _outputName, _( "Output name" ) );
    AddLine( topSizer, _projectionType, _( "Projection type" ) );
    AddLine( topSizer, _polynomialOrder, _( "Polynomial order" ) );
    AddLine( topSizer, _description, _( "Description") );

    AddGroup( topSizer,
        new wxButton( this, ID_Warp, _( "Warp" ) ),
        new wxButton( this, ID_Test, _( "Test" ) ),
        new wxButton( this, wxID_CLOSE, ("Close" ) ),
        nullptr );

    SetSizerAndFit( topSizer );
}

WarpDlg::~WarpDlg()
{

}

wxChoice* WarpDlg::PopulateProjectionType()
{
    vector<wxString> choices;
    for( int i = 0; _warp.ProjectionNames[ i ]; i++ ) {
        choices.push_back( _warp.ProjectionNames[ i ] );
    }

    return new wxChoice( this, ID_ProjectionType, wxDefaultPosition, wxDefaultSize, choices.size(), &choices[0] );
}

void WarpDlg::OnWarp()
{
    if( wxEmptyString == _inputFile->GetPath() ) {
        wxMessageBox( _( "Input file not specified" ) );
        return;
    }
    if( wxEmptyString == _pointsFile->GetPath() ) {
        wxMessageBox( _( "Points file not specified" ) );
        return;
    }
    if( wxEmptyString == _outputName->GetPath() ) {
        wxMessageBox( _( "Output file not specified" ) );
        return;
    }

    _warp.inputImageFilename = _inputFile->GetPath();
    _warp.inputPointsFilename = _pointsFile->GetPath();
    _warp.outputFilename = RemoveFileExtension( _outputName->GetPath().ToStdString() );
    wxString polyString = _polynomialOrder->GetValue();
    if( polyString != wxEmptyString ) {
        _warp.polynomialOrder = wxAtoi( _polynomialOrder->GetValue() );
    }
    wxString desc = _description->GetValue();
    if( desc != wxEmptyString ) {
        _warp.kmlName = desc;
    }
    _warp.projType = ( PolyProjectArgs::ProjType ) _projectionType->GetSelection();
    _progressDialog.ShowModal();
}

void WarpDlg::OnButton( wxCommandEvent& event )
{
    const int id = event.GetId();
    switch( id ) {
        case wxID_CLOSE :
            _progressDialog.Close();
            Destroy();
            break;
        case ID_Warp :
            _warp.fitOnly = false;
            OnWarp();
            break;
        case ID_Test :
            _warp.fitOnly = true;
            OnWarp();
            break;
    }
}
