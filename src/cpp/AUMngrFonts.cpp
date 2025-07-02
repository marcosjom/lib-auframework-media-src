//
//  AUMngrFonts.cpp
//  lib-auframework-media
//
//  Created by Marcos Ortega on 31/1/19.
//

#include "AUFrameworkMediaStdAfx.h"
#include "AUMngrFonts.h"
//
#include "nb/core/NBMemory.h"
#include "NBGestorMemoria.h"
#include "NBGestorPilaLlamadas.h"
#include "NBGestorTexturas.h"

#define AUMngrFonts_MUTEX_ACTIVATE		NBHILO_MUTEX_ACTIVAR(&_mutex) _mutexLocksCount++;
#define AUMngrFonts_MUTEX_DEACTIVATE	_mutexLocksCount--; NBHILO_MUTEX_DESACTIVAR(&_mutex)

AUMngrFonts::AUMngrFonts(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUMngrFonts::AUMngrFonts")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUMngrFonts")
/*#	ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
	{
		//If fails, '_calls' initial values are not properly set to NULL.
		SI32 i; for(i = 0; i < sizeof(_calls); i++){
			NBASSERT(((BYTE*)&_calls)[i] == 0)
		}
	}
#	endif*/
	//
	_mutexLocksCount = 0;
	NBHILO_MUTEX_INICIALIZAR(&_mutex)
	_ppi		=  NBST_P(STNBSize, 72.0f, 72.0f);
	_sizeMode	= ENMngrFontSizeMode_Medium;
	{
		NBMemory_setZero(_fontModeTypeDescs);
		//Font type desc
		{
			SI32 i; for(i = 0; i < ENMngrFontType_Count; i++){
				STMngrFontTypeDesc* tDesc = &_typesDescs[i];
				NBString_initWithStr(&tDesc->family, "Arial"); //ToDo: change to an universality-available font family
			}
		}
		//Set fonts sizes
		//https://stablekernel.com/supporting-accessibility-larger-text-in-ios/
		{
			const ENMngrFontSizeMode m = ENMngrFontSizeMode_XSmall;
			_fontModeTypeDescs[m][ENMngrFontType_Title1].pointsSz	= 25;
			_fontModeTypeDescs[m][ENMngrFontType_Title2].pointsSz	= 19;
			_fontModeTypeDescs[m][ENMngrFontType_Title3].pointsSz	= 17;
			_fontModeTypeDescs[m][ENMngrFontType_Headline].pointsSz	= 14;
			_fontModeTypeDescs[m][ENMngrFontType_Body].pointsSz		= 14;
			_fontModeTypeDescs[m][ENMngrFontType_Callout].pointsSz	= 13;
			_fontModeTypeDescs[m][ENMngrFontType_FootNote].pointsSz	= 12;
			_fontModeTypeDescs[m][ENMngrFontType_Caption1].pointsSz	= 11;
			_fontModeTypeDescs[m][ENMngrFontType_Caption2].pointsSz	= 11;
		}
		{
			const ENMngrFontSizeMode m = ENMngrFontSizeMode_Small;
			_fontModeTypeDescs[m][ENMngrFontType_Title1].pointsSz	= 26;
			_fontModeTypeDescs[m][ENMngrFontType_Title2].pointsSz	= 20;
			_fontModeTypeDescs[m][ENMngrFontType_Title3].pointsSz	= 18;
			_fontModeTypeDescs[m][ENMngrFontType_Headline].pointsSz	= 15;
			_fontModeTypeDescs[m][ENMngrFontType_Body].pointsSz		= 15;
			_fontModeTypeDescs[m][ENMngrFontType_Callout].pointsSz	= 14;
			_fontModeTypeDescs[m][ENMngrFontType_FootNote].pointsSz	= 12;
			_fontModeTypeDescs[m][ENMngrFontType_Caption1].pointsSz	= 11;
			_fontModeTypeDescs[m][ENMngrFontType_Caption2].pointsSz	= 11;
		}
		{
			const ENMngrFontSizeMode m = ENMngrFontSizeMode_Medium;
			_fontModeTypeDescs[m][ENMngrFontType_Title1].pointsSz	= 27;
			_fontModeTypeDescs[m][ENMngrFontType_Title2].pointsSz	= 21;
			_fontModeTypeDescs[m][ENMngrFontType_Title3].pointsSz	= 19;
			_fontModeTypeDescs[m][ENMngrFontType_Headline].pointsSz	= 16;
			_fontModeTypeDescs[m][ENMngrFontType_Body].pointsSz		= 16;
			_fontModeTypeDescs[m][ENMngrFontType_Callout].pointsSz	= 15;
			_fontModeTypeDescs[m][ENMngrFontType_FootNote].pointsSz	= 12;
			_fontModeTypeDescs[m][ENMngrFontType_Caption1].pointsSz	= 11;
			_fontModeTypeDescs[m][ENMngrFontType_Caption2].pointsSz	= 11;
		}
		{
			const ENMngrFontSizeMode m = ENMngrFontSizeMode_Large;
			_fontModeTypeDescs[m][ENMngrFontType_Title1].pointsSz	= 28;
			_fontModeTypeDescs[m][ENMngrFontType_Title2].pointsSz	= 22;
			_fontModeTypeDescs[m][ENMngrFontType_Title3].pointsSz	= 20;
			_fontModeTypeDescs[m][ENMngrFontType_Headline].pointsSz	= 17;
			_fontModeTypeDescs[m][ENMngrFontType_Body].pointsSz		= 17;
			_fontModeTypeDescs[m][ENMngrFontType_Callout].pointsSz	= 16;
			_fontModeTypeDescs[m][ENMngrFontType_FootNote].pointsSz	= 13;
			_fontModeTypeDescs[m][ENMngrFontType_Caption1].pointsSz	= 12;
			_fontModeTypeDescs[m][ENMngrFontType_Caption2].pointsSz	= 11;
		}
		{
			const ENMngrFontSizeMode m = ENMngrFontSizeMode_XLarge;
			_fontModeTypeDescs[m][ENMngrFontType_Title1].pointsSz	= 30;
			_fontModeTypeDescs[m][ENMngrFontType_Title2].pointsSz	= 24;
			_fontModeTypeDescs[m][ENMngrFontType_Title3].pointsSz	= 22;
			_fontModeTypeDescs[m][ENMngrFontType_Headline].pointsSz	= 19;
			_fontModeTypeDescs[m][ENMngrFontType_Body].pointsSz		= 19;
			_fontModeTypeDescs[m][ENMngrFontType_Callout].pointsSz	= 18;
			_fontModeTypeDescs[m][ENMngrFontType_FootNote].pointsSz	= 15;
			_fontModeTypeDescs[m][ENMngrFontType_Caption1].pointsSz	= 14;
			_fontModeTypeDescs[m][ENMngrFontType_Caption2].pointsSz	= 13;
		}
		{
			const ENMngrFontSizeMode m = ENMngrFontSizeMode_XXLarge;
			_fontModeTypeDescs[m][ENMngrFontType_Title1].pointsSz	= 32;
			_fontModeTypeDescs[m][ENMngrFontType_Title2].pointsSz	= 26;
			_fontModeTypeDescs[m][ENMngrFontType_Title3].pointsSz	= 24;
			_fontModeTypeDescs[m][ENMngrFontType_Headline].pointsSz	= 21;
			_fontModeTypeDescs[m][ENMngrFontType_Body].pointsSz		= 21;
			_fontModeTypeDescs[m][ENMngrFontType_Callout].pointsSz	= 20;
			_fontModeTypeDescs[m][ENMngrFontType_FootNote].pointsSz	= 17;
			_fontModeTypeDescs[m][ENMngrFontType_Caption1].pointsSz	= 16;
			_fontModeTypeDescs[m][ENMngrFontType_Caption2].pointsSz	= 15;
		}
		{
			const ENMngrFontSizeMode m = ENMngrFontSizeMode_XXXLarge;
			_fontModeTypeDescs[m][ENMngrFontType_Title1].pointsSz	= 34;
			_fontModeTypeDescs[m][ENMngrFontType_Title2].pointsSz	= 28;
			_fontModeTypeDescs[m][ENMngrFontType_Title3].pointsSz	= 26;
			_fontModeTypeDescs[m][ENMngrFontType_Headline].pointsSz	= 23;
			_fontModeTypeDescs[m][ENMngrFontType_Body].pointsSz		= 23;
			_fontModeTypeDescs[m][ENMngrFontType_Callout].pointsSz	= 22;
			_fontModeTypeDescs[m][ENMngrFontType_FootNote].pointsSz	= 19;
			_fontModeTypeDescs[m][ENMngrFontType_Caption1].pointsSz	= 18;
			_fontModeTypeDescs[m][ENMngrFontType_Caption2].pointsSz	= 17;
		}
		//Set names and verify sizes
		{
			SI32 m = 0; for(m = 0 ; m < ENMngrFontSizeMode_Count; m++){
				SI32 t = 0; for(t = 0 ; t < ENMngrFontType_Count; t++){
					NBString_init(&_fontModeTypeDescs[m][t].subfamily);
					_fontModeTypeDescs[m][t].style.bold = FALSE;
					_fontModeTypeDescs[m][t].style.italic = FALSE;
					NBASSERT(_fontModeTypeDescs[m][t].pointsSz > 0.0f);
					
				}
			}
		}
	}
	NBArray_init(&_fonts, sizeof(STMngrFontsFont), NULL);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

AUMngrFonts::~AUMngrFonts(){
	AUMngrFonts_MUTEX_ACTIVATE
	{
		_sizeMode	= ENMngrFontSizeMode_Medium;
		//Fonts
		{
			SI32 i; for(i = 0; i < _fonts.use; i++){
				STMngrFontsFont* f = NBArray_itmPtrAtIndex(&_fonts, STMngrFontsFont, i);
				if(f->fontTextures != NULL) f->fontTextures->liberar(NB_RETENEDOR_THIS); f->fontTextures = NULL;
				if(f->fontRaster != NULL) f->fontRaster->liberar(NB_RETENEDOR_THIS); f->fontRaster = NULL;
				NBString_release(&f->family);
				NBString_release(&f->subfamily);
			}
			NBArray_empty(&_fonts);
			NBArray_release(&_fonts);
		}
		//Font type desc
		{
			SI32 i; for(i = 0; i < ENMngrFontType_Count; i++){
				STMngrFontTypeDesc* tDesc = &_typesDescs[i];
				NBString_release(&tDesc->family);
			}
		}
		//Fonts
		{
			SI32 m = 0; for(m = 0 ; m < ENMngrFontSizeMode_Count; m++){
				SI32 t = 0; for(t = 0 ; t < ENMngrFontType_Count; t++){
					STMngrFontsFontModeTypeDesc* f = &_fontModeTypeDescs[m][t];
					NBString_release(&f->subfamily);
				}
			}
		}
	}
	AUMngrFonts_MUTEX_DEACTIVATE
	NBHILO_MUTEX_FINALIZAR(&_mutex);
}

//

STNBSize AUMngrFonts::getPPI() const {
	return _ppi;
}

void AUMngrFonts::setPPI(const STNBSize ppi){
	NBASSERT(ppi.width > 0.0f && ppi.height > 0.0f)
	if(ppi.width > 0.0f && ppi.height > 0.0f){
		AUMngrFonts_MUTEX_ACTIVATE
		if(_ppi.width != ppi.width || _ppi.height != ppi.height){
			PRINTF_INFO("AUMngrFonts setPPI(%.2f, %.2f).\n", ppi.width, ppi.height);
			_ppi = ppi;
			//Update existing fonts
			/*{
				SI32 i; for(i = 0; i < _fonts.use; i++){
					STMngrFontsFont* f = NBArray_itmPtrAtIndex(&_fonts, STMngrFontsFont, i);
					if(f->ppiOrg.height > 0 && (f->ppiCur.height != ppi.height || f->ppiCur.width != ppi.width)){
						const float relNewSz = (ppi.height / f->ppiOrg.height);
						const UI16 pointsSz2 = (f->pointsSzOrg * relNewSz * 2.0f); //2x (all points are in 0.5 jumps)
						PRINTF_INFO("Updating font record from %d to %d size in points.\n", f->pointsSz2, pointsSz2);
						f->ppiCur		= ppi;
						f->pointsSz2	= pointsSz2;
					}
				}
			}*/
			//Release exiting fonts
			//ToDo: remove
			{
				{
					SI32 i; for(i = 0; i < _fonts.use; i++){
						STMngrFontsFont* f = NBArray_itmPtrAtIndex(&_fonts, STMngrFontsFont, i);
						if(f->fontTextures != NULL) f->fontTextures->liberar(NB_RETENEDOR_THIS); f->fontTextures = NULL;
						if(f->fontRaster != NULL) f->fontRaster->liberar(NB_RETENEDOR_THIS); f->fontRaster = NULL;
						NBString_release(&f->family);
						NBString_release(&f->subfamily);
					}
					NBArray_empty(&_fonts);
				}
				//NBGestorTexturas::fuentesResetCurrent();
			}
		}
		AUMngrFonts_MUTEX_DEACTIVATE
	}
}

//

ENMngrFontSizeMode AUMngrFonts::getSizeMode() const {
	return _sizeMode;
}

void AUMngrFonts::setCurSizeMode(const ENMngrFontSizeMode szMode){
	_sizeMode = szMode;
}

//

const char* AUMngrFonts::getFontFamilyForType(const ENMngrFontType type) const {
	const char* r = NULL;
	if(type >= 0 && type < ENMngrFontType_Count){
		const STMngrFontTypeDesc* tDesc = &_typesDescs[type];
		r = tDesc->family.str;
	}
	return r;
}

void AUMngrFonts::setFontFamilyForType(const ENMngrFontType type, const char* fontFamiliy){
	if(type >= 0 && type < ENMngrFontType_Count){
		STMngrFontTypeDesc* tDesc = &_typesDescs[type];
		NBString_set(&tDesc->family, fontFamiliy);
	} else if(type == ENMngrFontType_Count){
		//Set to all
		SI32 i; for(i = 0; i < ENMngrFontType_Count; i++){
			STMngrFontTypeDesc* tDesc = &_typesDescs[i];
			NBString_set(&tDesc->family, fontFamiliy);
		}
	}
}

//

AUFuenteTextura* AUMngrFonts::fontTextures(const ENMngrFontType type){
	AUFuenteTextura* r = NULL;
	NBASSERT(_sizeMode >= 0 && _sizeMode < ENMngrFontSizeMode_Count)
	if(_sizeMode >= 0 && _sizeMode < ENMngrFontSizeMode_Count){
		if(type >= 0 && type < ENMngrFontType_Count){
			AUMngrFonts_MUTEX_ACTIVATE
			{
				const STMngrFontTypeDesc* tDesc = &_typesDescs[type];
				const STMngrFontsFontModeTypeDesc* f = &_fontModeTypeDescs[_sizeMode][type];
				r = this->privFontTextures(tDesc->family.str, (f->subfamily.length > 0 ? f->subfamily.str : NULL), f->style.bold, f->style.italic, f->pointsSz);
			}
			AUMngrFonts_MUTEX_DEACTIVATE
		}
	}
	return r;
}

AUFuenteTextura* AUMngrFonts::fontTextures(const char* family, const char* subfamily, const float szPts){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUMngrFonts::fontTextures")
	AUMngrFonts_MUTEX_ACTIVATE
	AUFuenteTextura* r = this->privFontTextures(family, subfamily, false, false, szPts);
	AUMngrFonts_MUTEX_DEACTIVATE
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return r;
}

AUFuenteTextura* AUMngrFonts::fontTextures(const char* family, const BOOL bold, const BOOL italic, const float szPts){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUMngrFonts::fontTextures")
	AUMngrFonts_MUTEX_ACTIVATE
	AUFuenteTextura* r = this->privFontTextures(family, NULL, bold, italic, szPts);
	AUMngrFonts_MUTEX_DEACTIVATE
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return r;
}

//

AUFuenteTextura* AUMngrFonts::fontTexturesInPixels(const ENMngrFontType type, const float szPxs){
	AUFuenteTextura* r = NULL;
	NBASSERT(_sizeMode >= 0 && _sizeMode < ENMngrFontSizeMode_Count)
	if(_sizeMode >= 0 && _sizeMode < ENMngrFontSizeMode_Count){
		if(type >= 0 && type < ENMngrFontType_Count){
			AUMngrFonts_MUTEX_ACTIVATE
			{
				const STMngrFontTypeDesc* tDesc = &_typesDescs[type];
				const STMngrFontsFontModeTypeDesc* f = &_fontModeTypeDescs[_sizeMode][type];
				r = this->privFontTexturesInPixels(tDesc->family.str, (f->subfamily.length > 0 ? f->subfamily.str : NULL), f->style.bold, f->style.italic, szPxs);
			}
			AUMngrFonts_MUTEX_DEACTIVATE
		}
	}
	return r;
}

AUFuenteTextura* AUMngrFonts::fontTexturesInPixels(const char* family, const char* subfamily, const float szPxs){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUMngrFonts::fontTextures")
	AUMngrFonts_MUTEX_ACTIVATE
	AUFuenteTextura* r = this->privFontTexturesInPixels(family, subfamily, false, false, szPxs);
	AUMngrFonts_MUTEX_DEACTIVATE
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return r;
}

AUFuenteTextura* AUMngrFonts::fontTexturesInPixels(const char* family, const BOOL bold, const BOOL italic, const float szPxs){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUMngrFonts::fontTextures")
	AUMngrFonts_MUTEX_ACTIVATE
	AUFuenteTextura* r = this->privFontTexturesInPixels(family, NULL, bold, italic, szPxs);
	AUMngrFonts_MUTEX_DEACTIVATE
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return r;
}

//

AUFuenteRaster* AUMngrFonts::fontRaster(const ENMngrFontType type){
	AUFuenteRaster* r = NULL;
	NBASSERT(_sizeMode >= 0 && _sizeMode < ENMngrFontSizeMode_Count)
	if(_sizeMode >= 0 && _sizeMode < ENMngrFontSizeMode_Count){
		if(type >= 0 && type < ENMngrFontType_Count){
			AUMngrFonts_MUTEX_ACTIVATE
			{
				const STMngrFontTypeDesc* tDesc = &_typesDescs[type];
				const STMngrFontsFontModeTypeDesc* f = &_fontModeTypeDescs[_sizeMode][type];
				r = this->privFontRaster(tDesc->family.str, (f->subfamily.length > 0 ? f->subfamily.str : NULL), f->style.bold, f->style.italic, f->pointsSz);
			}
			AUMngrFonts_MUTEX_DEACTIVATE
		}
	}
	return r;
}

AUFuenteRaster* AUMngrFonts::fontRaster(const char* family, const char* subfamily, const float szPts){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUMngrFonts::fontRaster")
	AUMngrFonts_MUTEX_ACTIVATE
	AUFuenteRaster* r = this->privFontRaster(family, subfamily, false, false, szPts);
	AUMngrFonts_MUTEX_DEACTIVATE
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return r;
}

AUFuenteRaster* AUMngrFonts::fontRaster(const char* family, const BOOL bold, const BOOL italic, const float szPts){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUMngrFonts::fontRaster")
	AUMngrFonts_MUTEX_ACTIVATE
	AUFuenteRaster* r = this->privFontRaster(family, NULL, bold, italic, szPts);
	AUMngrFonts_MUTEX_DEACTIVATE
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return r;
}

//

STMngrFontsFont* AUMngrFonts::privFontRecord(const char* family, const char* subfamily, const BOOL bold, const BOOL italic, const float szPts){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUMngrFonts::privFontRecord")
	STMngrFontsFont* r = NULL;
	NBASSERT(_mutexLocksCount > 0) //must be locked
	if(family != NULL){
		const UI16 pointsSz2 = (szPts * 2.0f); //2x (all points are in 0.5 jumps)
		//Search
		SI32 i; for(i = 0; i < _fonts.use; i++){
			STMngrFontsFont* f = NBArray_itmPtrAtIndex(&_fonts, STMngrFontsFont, i);
			if(f->pointsSz2 == pointsSz2){
				if(NBString_isEqual(&f->family, family)){
					if(subfamily == NULL){
						if(f->bold == bold && f->italic == italic){
							r = f;
							break;
						}
					} else if(NBString_isEqual(&f->subfamily, subfamily)){
						r = f;
						break;
					}
				}
			}
		}
		//Create new record
		if(r == NULL){
			STMngrFontsFont f;
			NBString_initWithStr(&f.family, family);
			NBString_initWithStr(&f.subfamily, (subfamily != NULL ? subfamily : ""));
			f.bold			= bold;
			f.italic		= italic;
			f.ppiOrg		= _ppi;
			f.ppiCur		= _ppi;
			f.pointsSzOrg	= szPts;
			f.pointsSz2		= pointsSz2;
			f.fontTextures	= NULL;
			f.fontRaster	= NULL;
			r = (STMngrFontsFont*)NBArray_addValue(&_fonts, f);
			NBASSERT(NBString_isEqual(&r->family, family)); //corrupt data-pointer?
		}
		//Must return a record
		NBASSERT(r != NULL)
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return r;
}

AUFuenteTextura* AUMngrFonts::privFontTextures(const char* family, const char* subfamily, const BOOL bold, const BOOL italic, const float szPts){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUMngrFonts::privFontTextures")
	AUFuenteTextura* r = NULL;
	NBASSERT(_mutexLocksCount > 0) //must be locked
	if(family != NULL){
		STMngrFontsFont* record = this->privFontRecord(family, subfamily, bold, italic, szPts); NBASSERT(record != NULL)
		if(record->fontTextures != NULL){
			r = record->fontTextures;
		} else {
			const SI32 pxSize = (((float)record->pointsSz2 / 2.0f) * _ppi.height) / 72.0f;
			//PRINTF_INFO("AUMngrFonts, loading texFont szPts(%f) szPxs(%d).\n", szPts, pxSize);
			if(NBString_strIsEmpty(subfamily)){
				record->fontTextures = NBGestorTexturas::fuenteTextura(family, pxSize, bold, italic); NBASSERT(record->fontTextures != NULL)
				record->fontTextures->retener(NB_RETENEDOR_THIS);
			} else {
				record->fontTextures = NBGestorTexturas::fuenteTextura(family, pxSize, subfamily); NBASSERT(record->fontTextures != NULL)
				record->fontTextures->retener(NB_RETENEDOR_THIS);
			}
			r = record->fontTextures;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return r;
}

AUFuenteTextura* AUMngrFonts::privFontTexturesInPixels(const char* family, const char* subfamily, const BOOL bold, const BOOL italic, const float szPxs){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUMngrFonts::privFontTexturesInPixels")
	AUFuenteTextura* r = NULL;
	NBASSERT(_mutexLocksCount > 0) //must be locked
	if(family != NULL){
		const float szPts = ((float)szPxs * 72.0f) / _ppi.height;
		STMngrFontsFont* record = this->privFontRecord(family, subfamily, bold, italic, szPts); NBASSERT(record != NULL)
		if(record->fontTextures != NULL){
			r = record->fontTextures;
		} else {
			const SI32 pxSize = (((float)record->pointsSz2 / 2.0f) * _ppi.height) / 72.0f;
			//PRINTF_INFO("AUMngrFonts, loading texFont szPts(%f) szPxs(%d).\n", szPts, pxSize);
			if(NBString_strIsEmpty(subfamily)){
				record->fontTextures = NBGestorTexturas::fuenteTextura(family, pxSize, bold, italic); NBASSERT(record->fontTextures != NULL)
				record->fontTextures->retener(NB_RETENEDOR_THIS);
			} else {
				record->fontTextures = NBGestorTexturas::fuenteTextura(family, pxSize, subfamily); NBASSERT(record->fontTextures != NULL)
				record->fontTextures->retener(NB_RETENEDOR_THIS);
			}
			r = record->fontTextures;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return r;
}

AUFuenteRaster* AUMngrFonts::privFontRaster(const char* family, const char* subfamily, const BOOL bold, const BOOL italic, const float szPts){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUMngrFonts::privFontRaster")
	AUFuenteRaster* r = NULL;
	if(family != NULL){
		const SI32 pxSize = ((szPts * _ppi.height) / 72.0f);
		STMngrFontsFont* record = this->privFontRecord(family, subfamily, bold, italic, szPts); NBASSERT(record != NULL)
		if(record->fontRaster != NULL){
			r = record->fontRaster;
		} else {
			PRINTF_INFO("AUMngrFonts, loading rasterFont szPxs(%d).\n", pxSize);
			if(subfamily == NULL){
				record->fontRaster = NBGestorTexturas::fuenteRaster(family, pxSize, subfamily); NBASSERT(record->fontRaster != NULL)
				record->fontRaster->retener(NB_RETENEDOR_THIS);
			} else {
				record->fontRaster = NBGestorTexturas::fuenteRaster(family, pxSize, bold, italic); NBASSERT(record->fontRaster != NULL)
				record->fontRaster->retener(NB_RETENEDOR_THIS);
			}
			r = record->fontRaster;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return r;
	
}

//

void AUMngrFonts::lock(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUMngrFonts::lock")
	AUMngrFonts_MUTEX_ACTIVATE
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUMngrFonts::unlock(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUMngrFonts::unlock")
	NBASSERT(_mutexLocksCount > 0)
	AUMngrFonts_MUTEX_DEACTIVATE
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

//

AUOBJMETODOS_CLASESID_UNICLASE(AUMngrFonts)
AUOBJMETODOS_CLASESNOMBRES_UNICLASE(AUMngrFonts, "AUMngrFonts")
AUOBJMETODOS_CLONAR_NULL(AUMngrFonts)
