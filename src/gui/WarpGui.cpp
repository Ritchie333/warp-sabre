#include "WarpGui.h"
#include "WarpDlg.h"

IMPLEMENT_APP(WarpApp)

WarpApp::WarpApp()
{
}

WarpApp::~WarpApp()
{
}

bool WarpApp::OnInit()
{
    WarpDlg *dlg = new WarpDlg();
    dlg->Show();
    SetTopWindow( dlg );
    return true;
}
