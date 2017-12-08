#ifndef WX_PRECOMP
	#include <wx/wx.h>
#else
	#include <wx/wxprec.h>
#endif

#include "elapsedtime.h"
#include <omp.h>
#include <lcms2.h>

#ifdef WIN32
#include <sys/time.h>
#else
#include <time.h>
#endif

#include <wx/tokenzr.h>
#include <vector>

#include <gimage/gimage.h>
#include "myConfig.h"

wxString hstr="";

wxArrayString split(wxString str, wxString delim)
{
	wxArrayString a;
	wxStringTokenizer tokenizer(str, delim);
	while ( tokenizer.HasMoreTokens() ) {
		wxString token = tokenizer.GetNextToken();
		a.Add(token);
	}
	return a;
}

wxString paramString(wxString filter)
{
	wxString paramstr, name, val;

	std::map<std::string, std::string> c = myConfig::getConfig().getDefault();
	for (std::map<std::string, std::string>::iterator it=c.begin(); it!=c.end(); ++it) {
		name = wxString(it->first.c_str());
		val =  wxString(it->second.c_str());
		if (name.Find(filter) != wxNOT_FOUND) {
			val = myConfig::getConfig().getValue(std::string((const char *) name.mb_str()));
			name.Replace(filter,"");
			if (val != "") paramstr.Append(wxString::Format("%s=%s;",name, val));
		}
	}

	return paramstr;
}

void mark ()
{
	_mark();
}

wxString duration ()
{
	return wxString::Format("%fsec", _duration());
}



//File logging:
void log(wxString msg)
{
	wxString logfile = wxString(myConfig::getConfig().getValueOrDefault("log.filename","").c_str());
	if (logfile == "") return;
	FILE * f = fopen(logfile.c_str(), "a");
	if (f) {
		fputs(wxNow().c_str(), f);
		fputs(" - ",f);
		fputs(msg,f);
		fputs("\n",f);
		fclose(f);
	}
}


wxBitmap HistogramFromVec(std::vector<int> hdata, int hmax, int width, int height) 
{
	wxBitmap bmp(width, height); 
	wxMemoryDC dc;
	dc.SelectObject(bmp);
	dc.Clear();
	dc.SetUserScale((double) width / (double) hdata.size(), (double) height/ (double) hmax);
	dc.SetPen(wxPen(wxColour(128,128,128),1));
	for(int x=0; x<hdata.size(); x++) {
		dc.DrawLine(x,dc.DeviceToLogicalY(height),x,dc.DeviceToLogicalY(height)-hdata[x]);
	}

	dc.SelectObject(wxNullBitmap);
	return bmp;
}

wxBitmap ThreadedHistogramFrom(wxImage img, int width, int height) 
{
	mark();
	unsigned hdata[256], rdata[256]={0}, gdata[256]={0}, bdata[256]={0};
	int hmax = 0;
	wxBitmap bmp(width, height);  //, outbmp(width, height);
	for (int i=0; i<256; i++) hdata[i]=0;
	int iw = img.GetWidth();
	int ih = img.GetHeight();
	for (int i=0; i<256; i++) hdata[i] = 0;
	int gray;
	long pos;
	unsigned char *data = img.GetData();

	int threadcount = atoi(myConfig::getConfig().getValueOrDefault("display.wxhistogram.cores","0").c_str());
	
	if (threadcount == 0)
#if defined(_OPENMP)
		threadcount = (long) omp_get_max_threads();
#else
		threadcount = 1;
#endif

	#pragma omp parallel num_threads(threadcount)
	{
		unsigned pdata[256] = {0};
		unsigned pr[256] = {0};
		unsigned pg[256] = {0};
		unsigned pb[256] = {0};
		#pragma omp for
		for(unsigned y = 0; y < ih; y++) {
			for(unsigned x = 0; x < iw; x++) {
				long pos = (y * iw + x) * 3;
				//int gray = (data[pos]+data[pos+1]+data[pos+2]) / 3;
				//pdata[gray]++;
				pr[data[pos]]++;
				pg[data[pos+1]]++;
				pb[data[pos+2]]++;
			}
		}

		#pragma omp critical 
		{
			for (unsigned i=0; i<256; i++) {
				//hdata[i] += pdata[i];
				rdata[i] += pr[i];
				gdata[i] += pg[i];
				bdata[i] += pb[i];
			}
		}

	}


	//for (int i=0; i<256; i++) if (hdata[i]>hmax) hmax = hdata[i];
	for (int i=0; i<256; i++) if (rdata[i]>hmax) hmax = rdata[i];
	for (int i=0; i<256; i++) if (gdata[i]>hmax) hmax = gdata[i];
	for (int i=0; i<256; i++) if (bdata[i]>hmax) hmax = bdata[i];

	wxMemoryDC dc;
	dc.SelectObject(bmp);
	dc.Clear();
	dc.SetUserScale((double) width / 256.0, (double) height/ (double) hmax);
	dc.SetPen(wxPen(wxColour(128,128,128),1));
	for(int x=1; x<256; x++) {
		//dc.DrawLine(x,dc.DeviceToLogicalY(height),x,dc.DeviceToLogicalY(height)-hdata[x]);
		//dc.DrawLine(x-1,dc.DeviceToLogicalY(height)-hdata[x-1],x,dc.DeviceToLogicalY(height)-hdata[x]);
	}
	dc.SetPen(wxPen(wxColour(255,0,0),1));
	for(int x=1; x<256; x++) {
		dc.DrawLine(x-1,dc.DeviceToLogicalY(height)-rdata[x-1],x,dc.DeviceToLogicalY(height)-rdata[x]);
	}
	dc.SetPen(wxPen(wxColour(0,255,0),1));
	for(int x=1; x<256; x++) {
		dc.DrawLine(x-1,dc.DeviceToLogicalY(height)-gdata[x-1],x,dc.DeviceToLogicalY(height)-gdata[x]);
	}
	dc.SetPen(wxPen(wxColour(0,0,255),1));
	for(int x=1; x<256; x++) {
		dc.DrawLine(x-1,dc.DeviceToLogicalY(height)-bdata[x-1],x,dc.DeviceToLogicalY(height)-bdata[x]);
	}
	//dc.SetBrush(wxBrush(wxColour(128,128,128)));
	//dc.FloodFill(128, dc.DeviceToLogicalY(height)-hdata[128+50], wxColour(128,128,128), wxFLOOD_BORDER);

	dc.SelectObject(wxNullBitmap);
	wxString d = duration();
	if ((myConfig::getConfig().getValueOrDefault("display.all.log","0") == "1") || (myConfig::getConfig().getValueOrDefault("display.wxhistogram.log","0") == "1"))
		log(wxString::Format("tool=wxhistogram,imagesize=%dx%d,imagebpp=%d,threads=%d,time=%s",iw, ih,24,threadcount,d));

	return bmp;
}

wxBitmap HistogramFrom(wxImage img, int width, int height) 
{
	mark();
	int hdata[256];
	int hmax = 0;
	wxBitmap bmp(width, height);  //, outbmp(width, height);
	for (int i=0; i<256; i++) hdata[i]=0;
	int iw = img.GetWidth();
	int ih = img.GetHeight();
	for (int i=0; i<256; i++) hdata[i] = 0;
	int gray;
	long pos;
	unsigned char *data = img.GetData();
	for (int x=0; x<iw; x++) {
		for (int y=0; y<ih; y++) {
			pos = (y * iw + x) * 3;
			gray = (data[pos]+data[pos+1]+data[pos+2]) / 3;
			hdata[gray]++;
			if (hdata[gray] > hmax) hmax = hdata[gray];
		}
	}
	wxMemoryDC dc;
	dc.SelectObject(bmp);
	dc.Clear();
	dc.SetUserScale((double) width / 256.0, (double) height/ (double) hmax);
	dc.SetPen(wxPen(wxColour(128,128,128),1));
	for(int x=0; x<256; x++) {
		dc.DrawLine(x,dc.DeviceToLogicalY(height),x,dc.DeviceToLogicalY(height)-hdata[x]);
	}

	dc.SelectObject(wxNullBitmap);
	wxString d = duration();
	if ((myConfig::getConfig().getValueOrDefault("display.all.log","0") == "1") || (myConfig::getConfig().getValueOrDefault("display.wxhistogram.log","0") == "1"))
		log(wxString::Format("tool=wxhistogram(old),imagesize=%dx%d,imagebpp=%d,threads=%d,time=%s",iw, ih,24,1,d));

	return bmp;
}

wxImage gImage2wxImage(gImage &dib)
{
	int threadcount;
	unsigned h = dib.getHeight();
	unsigned w =  dib.getWidth();
	unsigned char *img = (unsigned char *) dib.getImageData(BPP_8);
	wxImage image(w, h);
	unsigned char *data = image.GetData();
	//ToDo: gImage2wxImage Optimize?  not so sure...
	//ToDo: gImage2wxImage logging
	//unsigned s = w*h*3;
	//#pragma omp parallel for num_threads(4)
	//for (int i = 0; i<s; i++) {
	//	data[i] = img[i];
	//}
	if (img) {
		memcpy(data,img, w*h*3);
		delete [] img;
	}
	return image;
}



unsigned hdata[256];
unsigned hmax;

void FillHistogram(unsigned *histogram)
{
	for (int i=0; i<256; i++) histogram[i] = hdata[i];
}



wxBitmap HistogramFromData(int width, int height) 
{
	wxBitmap bmp(width, height);  //, outbmp(width, height);
	wxMemoryDC dc;
	dc.SelectObject(bmp);
	dc.Clear();
	dc.SetUserScale((double) width / 256.0, (double) height/ (double) hmax);
	dc.SetPen(wxPen(wxColour(128,128,128),1));
	for(int x=0; x<256; x++) {
		dc.DrawLine(x,dc.DeviceToLogicalY(height),x,dc.DeviceToLogicalY(height)-hdata[x]);
	}

	dc.SelectObject(wxNullBitmap);
	return bmp;
}


bool ImageContainsRawprocCommand(wxString fname)
{
	std::map<std::string, std::string> p = gImage::getInfo(fname.c_str());
	if (p.find("ImageDescription") != p.end())
		if (p["ImageDescription"].find("rawproc") != std::string::npos)
			return true;
		if (p["ImageDescription"].find("gimg") != std::string::npos)
			return true;
	return false;
}



