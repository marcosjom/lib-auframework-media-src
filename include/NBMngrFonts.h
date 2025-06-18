//
//  NBMngrFonts.h
//  lib-auframework-media
//
//  Created by Marcos Ortega on 31/1/19.
//

#ifndef NBMngrFonts_h
#define NBMngrFonts_h

#include "AUMngrFonts.h"

class NBMngrFonts {
	public:
		static void		init();
		static void		finish();
		static bool		isInited();
		//
		static STNBSize				getPPI();
		static ENMngrFontSizeMode	getSizeMode();
		static const char*			getFontFamilyForType(const ENMngrFontType type);
		//
		static void					setPPI(const STNBSize ppi);
		static void					setCurSizeMode(const ENMngrFontSizeMode szMode);
		static void					setFontFamilyForType(const ENMngrFontType type, const char* fontFamiliy);
		//
		//static void				updateFonts();
		//
		static AUFuenteTextura*		fontTextures(const ENMngrFontType type);
		static AUFuenteTextura*		fontTextures(const char* family, const char* subfamily, const float szPts);
		static AUFuenteTextura*		fontTextures(const char* family, const BOOL bold, const BOOL italic, const float szPts);
		//
		static AUFuenteTextura*		fontTexturesInPixels(const ENMngrFontType type, const float szPxs);
		static AUFuenteTextura*		fontTexturesInPixels(const char* family, const char* subfamily, const float szPxs);
		static AUFuenteTextura*		fontTexturesInPixels(const char* family, const BOOL bold, const BOOL italic, const float szPxs);
		//
		static AUFuenteRaster*		fontRaster(const ENMngrFontType type);
		static AUFuenteRaster*		fontRaster(const char* family, const char* subfamily, const float szPts);
		static AUFuenteRaster*		fontRaster(const char* family, const BOOL bold, const BOOL italic, const float szPts);
		//
		static void		lock();
		static void		unlock();
	private:
		static AUMngrFonts* _instance;
};

#endif /* NBMngrFonts_h */
