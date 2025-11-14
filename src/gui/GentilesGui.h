#ifndef GENTILES_GUI_H
#define GENTILES_GUI_H

#include <wx/wx.h>

class GentilesApp : public wxApp
{
public:
    GentilesApp();
    virtual ~GentilesApp();
    virtual bool OnInit();
};

#endif // GENTILES_GUI_H