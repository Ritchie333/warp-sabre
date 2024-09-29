#ifndef _WARP_GUI_H
#define _WARP_GUI_H

#include <wx/wx.h>

class WarpApp : public wxApp
{
public:
    WarpApp();
    virtual ~WarpApp();
    virtual bool OnInit();
};

#endif // WARP_GUI_H