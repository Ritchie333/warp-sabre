#include "BaseDlg.h"

BaseDlg::BaseDlg( const wxString& name ) :
    wxDialog( nullptr, wxID_ANY, name )
{

}

BaseDlg::BaseDlg( const wxString& name, const wxPoint& position, const wxSize& size ) :
    wxDialog( nullptr, wxID_ANY, name, position, size )
{

}

BaseDlg::~BaseDlg()
{

}

void BaseDlg::AddLine( wxBoxSizer *topSizer, wxControl* control, const wxString& name )
{
    wxBoxSizer *sideSizer = new wxBoxSizer( wxHORIZONTAL );
    sideSizer->Add( new wxStaticText( this, wxID_ANY, name, wxDefaultPosition, wxSize( LABEL_WIDTH, -1 ) ) );
    sideSizer->Add( control );
    topSizer->Add( sideSizer );
}

void BaseDlg::AddGroup( wxBoxSizer *topSizer, ... )
{
    va_list ap;

    wxBoxSizer *sideSizer = new wxBoxSizer( wxHORIZONTAL );
    va_start( ap, topSizer );
    wxControl* next = nullptr;
    do {
        next = va_arg( ap, wxControl* );
        if( next ) {
            sideSizer->Add( next );
        }
    } while( next );
    va_end( ap );
    topSizer->Add( sideSizer );
}
