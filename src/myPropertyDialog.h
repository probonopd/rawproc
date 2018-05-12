#ifndef MYPROPERTYDIALOG_H_
#define MYPROPERTYDIALOG_H_

#include <wx/dialog.h>
#include <wx/propgrid/propgrid.h>
#include <wx/fileconf.h>
#include <wx/stattext.h>
#include <string>
#include <map>

class PropertyDialog: public wxDialog
{
	public:
		PropertyDialog(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize);  //uses myConfig::getConfig()
		PropertyDialog(wxWindow *parent, wxWindowID id, const wxString &title, std::map<std::string, std::string> props, const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize);
		PropertyDialog(wxWindow *parent, wxWindowID id, const wxString &title, wxFileConfig *config, const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize);
		~PropertyDialog();
		void UpdateProperty(wxPropertyGridEvent& event);
		void FilterGrid(wxCommandEvent& event);
		std::map<std::string,std::string> FilterList(wxString filter);
		std::string FilterString(wxString filter);
		
		bool PropExists(wxString name);
		void AddProp(wxCommandEvent& event);
		void DelProp(wxCommandEvent& event);
		void HideDialog(wxCommandEvent& event);
		void ClearModifiedStatus();


	private:
		wxPropertyGrid *pg;
		wxTextCtrl *fil;

};

#endif