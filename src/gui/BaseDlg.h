#include <wx/wx.h>
#include <wx/choice.h>
#include <wx/dialog.h>

class BaseDlg : public wxDialog
{
protected:
    BaseDlg( const wxString& name );
    BaseDlg( const wxString& name, const wxPoint& position, const wxSize& size );
    virtual ~BaseDlg();

    void AddLine( wxBoxSizer *topSizer, wxControl* control, const wxString& name );

    void AddGroup( wxBoxSizer *topSizer, ... );
};
