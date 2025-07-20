#include "WarpDlg.h"
#include "../Warp.h"
#include "../StringUtils.h"
#include "../ReadDelimitedFile.h"
#include <vector>
#include <ranges>

const int MAX_POLY = 5;

using namespace std;

BEGIN_EVENT_TABLE(WarpDlg, BaseDlg)
    EVT_BUTTON(wxID_ANY, WarpDlg::OnButton)
    EVT_CHECKBOX(ID_Vis, WarpDlg::OnVisChecked)
    EVT_FILEPICKER_CHANGED(wxID_ANY, WarpDlg::OnFilePickerChanged)
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
    _polynomialOrder = PopulatePolynomialOrder();
    _name = new wxTextCtrl( this, ID_FolderName, wxEmptyString, wxDefaultPosition, wxSize( DESC_WIDTH, -1 ) );
    _description = new wxTextCtrl( this, ID_Description, wxEmptyString, wxDefaultPosition, wxSize( DESC_WIDTH, -1 ) );
    _vis = new wxCheckBox( this, ID_Vis, wxEmptyString );

    AddLine( topSizer, _inputFile, _( "Input file " ) );
    AddLine( topSizer, _pointsFile, _( "Points file" ) );
    AddLine( topSizer, _outputName, _( "Output name" ) );
    AddLine( topSizer, _projectionType, _( "Projection type" ) );
    AddLine( topSizer, _polynomialOrder, _( "Polynomial order" ) );
    AddLine( topSizer, _vis, _( "Visualise errors") );
    AddLine( topSizer, _name, _( "Map name" ) );
    AddLine( topSizer, _description, _( "Description") );

    AddGroup( topSizer,
        new wxButton( this, ID_About, _( "About" ) ),
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

wxChoice* WarpDlg::PopulatePolynomialOrder()
{
    vector<wxString> orders;
    for( int i = 1; i <= MAX_POLY; i++ ) {
        orders.push_back( wxString::Format(wxT("%i"),i) );
    }
    return new wxChoice( this, ID_PolynomialOrder, wxDefaultPosition, wxDefaultSize, orders.size(), &orders[0] );
}

void WarpDlg::OnWarp()
{
    if( wxEmptyString == _inputFile->GetPath() && !_warp.fitOnly ) {
        wxMessageBox( _( "Input file not specified" ) );
        return;
    }
    if( wxEmptyString == _pointsFile->GetPath() ) {
        wxMessageBox( _( "Points file not specified" ) );
        return;
    }
    if( wxEmptyString == _outputName->GetPath() && !_warp.fitOnly ) {
        wxMessageBox( _( "Output file not specified" ) );
        return;
    }

    _warp.inputImageFilename = _inputFile->GetPath();
    _warp.inputPointsFilename = _pointsFile->GetPath();
    _warp.outputFilename = RemoveFileExtension( _outputName->GetPath().ToStdString() );
    _warp.polynomialOrder = _polynomialOrder->GetSelection() + 1;
    wxString name = _name->GetValue();
    if( name != wxEmptyString ) {
        _warp.kmlName = name;
    }
    wxString desc = _description->GetValue();
    if( desc != wxEmptyString ) {
        _warp.kmlDesc = desc;
    }
    _warp.projType = ( PolyProjectArgs::ProjType ) _projectionType->GetSelection();
    _progressDialog.ShowModal();
}

void WarpDlg::OnButton( wxCommandEvent& event )
{
    const int id = event.GetId();
    switch( id ) {
        case ID_About :
            PrintVersion(); 
            break;
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

void WarpDlg::OnPointsFileChanged( const string& path )
{
    if( fileExists( path.c_str() ) ) {
        DelimitedFile file;
        if( file.Open( path.c_str() ) > 0 ) {
            // Guess the projection type from the input file
            // Note that if a type is not defined here, it assumes mercator or "mixed mode" projection
            const char* projTypes[] = {
                "os", "", "cas", "bns", "bni", "bnf", "osi", "wo", "woi", "osy", nullptr
            };

            PolyProjectArgs::ProjType projType = PolyProjectArgs::ProjType::Mercator;
            bool projSet = false;
            for( unsigned int i = 0; i < file.NumLines(); i++ ) {
                PolyProjectArgs::ProjType nextProjType = PolyProjectArgs::ProjType::Mercator;
                DelimitedFileLine& line = file.GetLine( i );
                DelimitedFileValue& val = line.GetVal( 0 );
                string proj = val.GetVals();
                if( !proj.empty() ) {
                    for( int j = 0; projTypes[ j ]; j++ ) {
                        if ( proj == projTypes[ j ] ) {
                            nextProjType = ( PolyProjectArgs::ProjType) j;
                            break;
                        }
                    }
                    
                    // Projection has been found, but if it clashes with another one,
                    // then it's a "mixed mode" and must revert to mercator
                    if( !projSet ) {
                        projType = nextProjType;
                        projSet = true;
                    } else {
                        if( nextProjType != projType ) {
                            projType = PolyProjectArgs::ProjType::Mercator;
                        }
                    }
                }
                if( projType == PolyProjectArgs::ProjType::Mercator ) {
                    break;
                }
            }
            _projectionType->SetSelection( projType );
            // Guess the polynomial order from the points
            int polynomialOrder = CalcOrderFitForNumConstraints( 2 * file.NumLines() );
            if( polynomialOrder < 1 ) {
                polynomialOrder = 1;
            }
            if( polynomialOrder > MAX_POLY ) {
                polynomialOrder = MAX_POLY;
            }
            _polynomialOrder->SetSelection( polynomialOrder - 1 );
        } else {
            wxMessageBox( _("Unable to open points file ") + path );
        }
    }
}

void WarpDlg::OnFilePickerChanged( wxFileDirPickerEvent& event )
{
    // If a valid points file has been selected, auto-guess things
    const int id = event.GetId();
    string path = event.GetPath().ToStdString();
    switch( id ) {
        case ID_InputFile : {
            string pointsFile = RemoveFileExtension( path ) + ".csv";
            if( fileExists( pointsFile.c_str() ) ) {
                _pointsFile->SetPath( pointsFile );
                OnPointsFileChanged( pointsFile );
            }
            break;
        }
        case ID_PointsFile: {
            OnPointsFileChanged( path );
            break;
        }
        case ID_OutputName: {
            _outputName->SetPath( RemoveFileExtension( path ) );
            break;
        }
    }
}

void WarpDlg::OnVisChecked(wxCommandEvent& event)
{
    _warp.visualiseErrors = _vis->GetValue();
}