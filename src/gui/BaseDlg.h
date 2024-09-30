#include <wx/wx.h>
#include <wx/choice.h>
#include <wx/dialog.h>

class BaseDlg : public wxDialog
{
protected:
    BaseDlg( const wxString& name );
    virtual ~BaseDlg();

    void AddLine( wxBoxSizer *topSizer, wxControl* control, const wxString& name );

    void AddButtons( wxBoxSizer *topSizer, ... );
};
