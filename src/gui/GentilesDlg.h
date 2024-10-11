#ifndef _GENTILES_DLG_H
#define _GENTILES_DLG_H

#include <wx/wx.h>
#include <wx/stattext.h>
#include <wx/filepicker.h>
#include <wx/gauge.h>

#include "BaseDlg.h"
#include "../TileJob.h"

#include <vector>
using namespace std;

const int wxEVT_GENTILES_LOG = 6147;
const int wxEVT_GENTILES_END = 6148;
const int wxEVT_GENTILES_PROGRESS = 6149;
const int wxEVT_GENTILES_LENGTH = 6150;
const int wxEVT_GENTILES_ERR = 6151;

enum
{
    ID_InputFiles,
    ID_InputFilesBrowse,
    ID_BoundsFile,
    ID_OutputFolder,
    ID_MinZoom,
    ID_MaxZoom,
    ID_Output,
    ID_Progress,
    ID_Percentage,
    ID_Start,
    ID_Clear,
    ID_About
};

class GentilesThread : public wxThread, public Log
{
private:
    TileRunner& _runner;
protected:
    wxEvtHandler* _parent;
public:
    GentilesThread(wxEvtHandler* parent, TileRunner& runner );
    int source;
    virtual ~GentilesThread();
    wxThread::ExitCode Entry();

    virtual void Add( const string& value );
    virtual void Progress( const int position );
    virtual void Error( const string& value );
};

class GentilesDlg : public BaseDlg
{
public:
    GentilesDlg();
    virtual ~GentilesDlg();

private:
    wxTextCtrl* _inputFiles;
    wxFilePickerCtrl* _boundsFile;
    wxDirPickerCtrl* _outputFolder;
    wxTextCtrl* _minZoom;
    wxTextCtrl* _maxZoom;
    vector<wxStaticText*> _output;
    wxStaticText* _percentage;
    wxGauge* _progressBar;
    wxButton* _startButton;
    wxButton* _clearButton;
    wxButton* _closeButton;

    TileRunner _runner;
    GentilesThread* _thread;

    int _current;
    int _length;

    void OnButton( wxCommandEvent& event );
    void OnLog( wxCommandEvent& event );
    void OnTilesEnd( wxCommandEvent& event );
    void OnProgress( wxCommandEvent& event );
    void OnLength( wxCommandEvent& event );
    void OnError( wxCommandEvent& event );

    bool ValidateZoom( const wxString& type, const wxTextCtrl* zoom );

    DECLARE_EVENT_TABLE()

};

#endif // _GENTILES_DLG_H