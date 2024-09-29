#include "WarpDlg.h"
#include "../Warp.h"
#include "../StringUtils.h"
#include <vector>

using namespace std;

BEGIN_EVENT_TABLE(WarpDlg, wxDialog)
    EVT_BUTTON(wxID_ANY, WarpDlg::OnButton)
END_EVENT_TABLE()

WarpDlg::WarpDlg() :
    wxDialog( nullptr, wxID_ANY, _( "warp-sabre") ),
    _progressDialog( this, ID_Warp_Dlg, _warp )
{
    wxBoxSizer *topSizer = new wxBoxSizer( wxVERTICAL );

    _inputFile = new wxFilePickerCtrl( this, ID_InputFile );
    _pointsFile = new wxFilePickerCtrl( this, ID_PointsFile );
    _outputName = new wxFilePickerCtrl( this, ID_OutputName, wxEmptyString, wxFileSelectorPromptStr, wxFileSelectorDefaultWildcardStr, 
            wxDefaultPosition, wxDefaultSize, wxFLP_SAVE | wxFLP_USE_TEXTCTRL );
    _projectionType = PopulateProjectionType();
    _polynomialOrder = new wxTextCtrl( this, ID_PolynomialOrder );

    AddLine( topSizer, _inputFile, _( "Input file " ) );
    AddLine( topSizer, _pointsFile, _( "Points file" ) );
    AddLine( topSizer, _outputName, _( "Output name" ) );
    AddLine( topSizer, _projectionType, _( "Projection type" ) );
    AddLine( topSizer, _polynomialOrder, _( "Polynomial order" ) );

    AddButtons( topSizer );

    SetSizerAndFit( topSizer );
}

WarpDlg::~WarpDlg()
{

}

void WarpDlg::AddLine( wxBoxSizer *topSizer, wxControl* control, const wxString& name )
{
    wxBoxSizer *sideSizer = new wxBoxSizer( wxHORIZONTAL );
    sideSizer->Add( new wxStaticText( this, wxID_ANY, name ) );
    sideSizer->Add( control );
    topSizer->Add( sideSizer );
}

void WarpDlg::AddButtons( wxBoxSizer *topSizer )
{
    wxBoxSizer *sideSizer = new wxBoxSizer( wxHORIZONTAL );
    sideSizer->Add( new wxButton( this, ID_Warp, _( "Warp" )));
    sideSizer->Add( new wxButton( this, wxID_CLOSE, _( "Close" )));
    topSizer->Add( sideSizer );
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
    _warp.inputImageFilename = _inputFile->GetPath();
    _warp.inputPointsFilename = _pointsFile->GetPath();
    _warp.outputFilename = RemoveFileExtension( _outputName->GetPath().ToStdString() );
    _warp.polynomialOrder = atoi( _polynomialOrder->GetValue() );
    _warp.projType = ( PolyProjectArgs::ProjType ) _projectionType->GetSelection();
    _progressDialog.ShowModal();
}

void WarpDlg::OnButton( wxCommandEvent& event )
{
    const int id = event.GetId();
    if( wxID_CLOSE == id ) {
         _progressDialog.Close();
        Destroy();
    }
    if( ID_Warp == id )
    {
        OnWarp();
    }
}
