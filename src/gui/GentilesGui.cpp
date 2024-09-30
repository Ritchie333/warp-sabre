#include "GentilesGui.h"
#include "GentilesDlg.h"

IMPLEMENT_APP(GentilesApp)

GentilesApp::GentilesApp()
{
}

GentilesApp::~GentilesApp()
{
}

bool GentilesApp::OnInit()
{
    GentilesDlg *dlg = new GentilesDlg();
    dlg->Show();
    SetTopWindow( dlg );
    return true;
}
