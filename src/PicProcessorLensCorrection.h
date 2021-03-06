#ifndef __PICPROCESSORLENSCORRECTION_H__
#define __PICPROCESSORLENSCORRECTION_H__

#include "PicProcessor.h"

#include <locale.h>
#include <lensfun/lensfun.h>


class PicProcessorLensCorrection: public PicProcessor
{
	public:
		PicProcessorLensCorrection(wxString name, wxString command, wxTreeCtrl *tree, PicPanel *display);
		~PicProcessorLensCorrection();
		void createPanel(wxSimplebook* parent);
		void setAlternates(wxString acam, wxString alens);
		lfDatabase * getLensDatabase();
		bool processPic(bool processnext=true);

	private:
		wxString metadatacamera, metadatamount, metadatalens, altcamera, altmount, altlens;
		bool lfok;
		struct lfDatabase *ldb;
};

#endif
