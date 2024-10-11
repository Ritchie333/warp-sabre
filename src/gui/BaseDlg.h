#include <wx/wx.h>
#include <wx/choice.h>
#include <wx/dialog.h>

const int LABEL_WIDTH = 200;
const int TEXT_WIDTH = 500;
const int TEXT_HEIGHT = 250;
const int DESC_WIDTH = 400;

class BaseDlg : public wxDialog
{
protected:
    BaseDlg( const wxString& name );
    BaseDlg( const wxString& name, const wxPoint& position, const wxSize& size );
    virtual ~BaseDlg();

    void AddLine( wxBoxSizer *topSizer, wxControl* control, const wxString& name );

    void AddGroup( wxBoxSizer *topSizer, ... );
};

void PrintVersion();