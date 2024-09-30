#ifndef _WARP_DLG_H
#define _WARP_DLG_H

#include <wx/wx.h>
#include <wx/choice.h>
#include <wx/dialog.h>
#include <wx/filepicker.h>
#include <wx/sizer.h>
#include "../Warp.h"
#include "WarpProgressDlg.h"
#include "BaseDlg.h"

enum
{
    ID_InputFile,
    ID_PointsFile,
    ID_OutputName,
    ID_ProjectionType,
    ID_PolynomialOrder,
    ID_Warp,
    ID_Warp_Dlg
};

class WarpDlg : public BaseDlg
{
public:
    WarpDlg();
    virtual ~WarpDlg();
private:
    wxFilePickerCtrl* _inputFile;
    wxFilePickerCtrl* _pointsFile;
    wxFilePickerCtrl* _outputName;
    wxChoice* _projectionType;
    wxTextCtrl* _polynomialOrder;

    Warp _warp;
    WarpProgressDialog _progressDialog;

    wxChoice* PopulateProjectionType();

    void OnButton( wxCommandEvent& event );
    void OnWarp();

    DECLARE_EVENT_TABLE()
};

#endif // WARP_DLG_H