#include "BaseDlg.h"

BaseDlg::BaseDlg( const wxString& name ) :
    wxDialog( nullptr, wxID_ANY, name )
{

}

BaseDlg::~BaseDlg()
{

}

void BaseDlg::AddLine( wxBoxSizer *topSizer, wxControl* control, const wxString& name )
{
    wxBoxSizer *sideSizer = new wxBoxSizer( wxHORIZONTAL );
    sideSizer->Add( new wxStaticText( this, wxID_ANY, name ) );
    sideSizer->Add( control );
    topSizer->Add( sideSizer );
}

void BaseDlg::AddButtons( wxBoxSizer *topSizer, ... )
{
    va_list ap;

    wxBoxSizer *sideSizer = new wxBoxSizer( wxHORIZONTAL );
    va_start( ap, topSizer );
    wxButton* button = nullptr;
    do {
        button = va_arg( ap, wxButton* );
        if( button ) {
            sideSizer->Add( button );
        }
    } while( button );
    va_end( ap );
    topSizer->Add( sideSizer );
}
