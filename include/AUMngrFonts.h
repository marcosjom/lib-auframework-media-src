//
//  AUMngrFonts.h
//  lib-auframework-media
//
//  Created by Marcos Ortega on 31/1/19.
//

#ifndef AUMngrFonts_h
#define AUMngrFonts_h

#include "AUFuenteTextura.h"
#include "AUFuenteRaster.h"

typedef enum ENMngrFontType_ {
	ENMngrFontType_Title1 = 0,	//ref: ~27 in medium-size.
	ENMngrFontType_Title2,		//ref: ~21 in medium-size.
	ENMngrFontType_Title3,		//ref: ~19 in medium-size.
	ENMngrFontType_Headline,	//ref: ~16 in medium-size.
	ENMngrFontType_Body,		//ref: ~16 in medium-size.
	ENMngrFontType_Callout,		//ref: ~15 in medium-size.
	ENMngrFontType_FootNote,	//ref: ~12 in medium-size.
	ENMngrFontType_Caption1,	//ref: ~11 in medium-size.
	ENMngrFontType_Caption2,	//ref: ~11 in medium-size.
	//
	ENMngrFontType_Count
} ENMngrFontType;

typedef enum ENMngrFontSizeMode_ {
	ENMngrFontSizeMode_XSmall = 0,
	ENMngrFontSizeMode_Small,
	ENMngrFontSizeMode_Medium,
	ENMngrFontSizeMode_Large,
	ENMngrFontSizeMode_XLarge,
	ENMngrFontSizeMode_XXLarge,
	ENMngrFontSizeMode_XXXLarge,
	//
	ENMngrFontSizeMode_Count
} ENMngrFontSizeMode;

typedef struct STMngrFontTypeDesc_ {
	STNBString			family;
} STMngrFontTypeDesc;

typedef struct STMngrFontsFontModeTypeDesc_ {
	STNBString			subfamily;
	struct {
		BOOL			bold;
		BOOL			italic;
	} style;
	float				pointsSz;
} STMngrFontsFontModeTypeDesc;

typedef struct STMngrFontsFont_ {
	STNBString			family;
	STNBString			subfamily;
	BOOL				bold;
	BOOL				italic;
	STNBSize			ppiOrg;			//ppi at loading (to determine if needs update)
	STNBSize			ppiCur;			//ppi current
	float				pointsSzOrg;	//Original size in points
	UI16				pointsSz2;		//2x (all points are in 0.5 jumps, rounding sizes)
	AUFuenteTextura*	fontTextures;
	AUFuenteRaster*		fontRaster;
} STMngrFontsFont;

class AUMngrFonts : public AUObjeto {
	public:
		AUMngrFonts();
		virtual ~AUMngrFonts();
		//
		STNBSize			getPPI() const;
		ENMngrFontSizeMode	getSizeMode() const;
		const char*			getFontFamilyForType(const ENMngrFontType type) const;
		//
		void				setPPI(const STNBSize ppi);
		void				setCurSizeMode(const ENMngrFontSizeMode szMode);
		void				setFontFamilyForType(const ENMngrFontType type, const char* fontFamiliy);
		//
		//void				updateFonts();
		//
		AUFuenteTextura*	fontTextures(const ENMngrFontType type);
		AUFuenteTextura*	fontTextures(const char* family, const char* subfamily, const float szPts);
		AUFuenteTextura*	fontTextures(const char* family, const BOOL bold, const BOOL italic, const float szPts);
		AUFuenteTextura*	fontTexturesInPixels(const ENMngrFontType type, const float szPxs);
		AUFuenteTextura*	fontTexturesInPixels(const char* family, const char* subfamily, const float szPxs);
		AUFuenteTextura*	fontTexturesInPixels(const char* family, const BOOL bold, const BOOL italic, const float szPxs);
		//
		//
		AUFuenteRaster*		fontRaster(const ENMngrFontType type);
		AUFuenteRaster*		fontRaster(const char* family, const char* subfamily, const float szPts);
		AUFuenteRaster*		fontRaster(const char* family, const BOOL bold, const BOOL italic, const float szPts);
		//
		void				lock();
		void				unlock();
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	private:
		NBHILO_MUTEX_CLASE	_mutex;
		SI32				_mutexLocksCount; //Depth of calling ->lock() and unlock().
		STNBSize			_ppi;
		ENMngrFontSizeMode	_sizeMode;
		STMngrFontTypeDesc	_typesDescs[ENMngrFontType_Count];
		STMngrFontsFontModeTypeDesc _fontModeTypeDescs[ENMngrFontSizeMode_Count][ENMngrFontType_Count];
		STNBArray			_fonts; //STMngrFontsFont
		//
		STMngrFontsFont*	privFontRecord(const char* family, const char* subfamily, const BOOL bold, const BOOL italic, const float szPts);
		AUFuenteTextura*	privFontTextures(const char* family, const char* subfamily, const BOOL bold, const BOOL italic, const float szPts);
		AUFuenteTextura*	privFontTexturesInPixels(const char* family, const char* subfamily, const BOOL bold, const BOOL italic, const float szPxs);
		AUFuenteRaster*		privFontRaster(const char* family, const char* subfamily, const BOOL bold, const BOOL italic, const float szPts);
};

#endif /* AUMngrFonts_h */
