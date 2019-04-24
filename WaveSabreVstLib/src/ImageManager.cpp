#include <WaveSabreVstLib/ImageManager.h>
#include <WaveSabreVstLib/Common.h>

#ifndef LGCM_MAC
#include "../../Data/resource.h"
#endif

using namespace std;

namespace WaveSabreVstLib
{
	CBitmap *ImageManager::Get(ImageIds imageId)
	{
		ImageManager *im = get();

		auto ret = im->bitmaps.find(imageId);
		if (ret != im->bitmaps.end()) return ret->second;

		CBitmap *b;
		switch (imageId)
		{
#ifndef LGCM_MAC
		case ImageIds::Background: b = new CBitmap(IDB_PNG1); break;
		case ImageIds::Knob1: b = new CBitmap(IDB_PNG2); break;
		case ImageIds::TinyButton: b = new CBitmap(IDB_PNG3); break;
		case ImageIds::OptionMenuUnpressed: b = new CBitmap(IDB_PNG4); break;
		case ImageIds::OptionMenuPressed: b = new CBitmap(IDB_PNG5); break;
#else
		case ImageIds::Background: b = new CBitmap("background.png"); break;
		case ImageIds::Knob1: b = new CBitmap("knob1.png"); break;
		case ImageIds::TinyButton: b = new CBitmap("tinybutton.png"); break;
		case ImageIds::OptionMenuUnpressed: b = new CBitmap("optionmenu-unpressed.png"); break;
		case ImageIds::OptionMenuPressed: b = new CBitmap("optionmenu-pressed.png"); break;
#endif
		}
		b->remember();
		im->bitmaps.emplace(imageId, b);
		return b;
	}

	ImageManager::~ImageManager()
	{
		for (auto i = bitmaps.begin(); i != bitmaps.end(); i++)
			i->second->forget();
	}

	ImageManager *ImageManager::get()
	{
		static ImageManager im;

		return &im;
	}
}
