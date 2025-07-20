#ifndef _WARP_DLG_H
#define _WARP_DLG_H

#include <wx/wx.h>
#include <wx/choice.h>
#include <wx/checkbox.h>
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
    ID_FolderName,
    ID_Description,
    ID_Warp,
    ID_Test,
    ID_Vis,
    ID_Warp_Dlg,
    ID_About
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
    wxChoice* _polynomialOrder;
    wxTextCtrl* _name;
    wxTextCtrl* _description;
    wxCheckBox* _vis;

    Warp _warp;
    WarpProgressDialog _progressDialog;

    wxChoice* PopulateProjectionType();
    wxChoice* PopulatePolynomialOrder();

    void OnButton( wxCommandEvent& event );
    void OnFilePickerChanged( wxFileDirPickerEvent& event );
    void OnWarp();
    void OnPointsFileChanged( const string& path );
    void OnVisChecked(wxCommandEvent& event);

    DECLARE_EVENT_TABLE()
};

#endif // WARP_DLG_H