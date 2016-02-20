#ifndef __PICPROCPANEL_H__
#define __PICPROCPANEL_H__

#ifdef __BORLANDC__
	#pragma hdrstop
#endif

#ifndef WX_PRECOMP
	#include <wx/wx.h>
	#include <wx/frame.h>
#else
	#include <wx/wxprec.h>
#endif

#include "PicProcessor.h"
#define SLIDERWIDTH 70

class PicProcessor;

//class PicProcPanel: public wxScrolledWindow
class PicProcPanel: public wxPanel
{

	public:
		PicProcPanel(wxPanel *parent, PicProcessor *proc, wxString params);
		~PicProcPanel();
		wxString getParams();
		void OnSize(wxSizeEvent& event);

	protected:
		PicProcessor *q;
		wxBoxSizer *b;
		wxString p;

	//DECLARE_EVENT_TABLE()
};



#endif
