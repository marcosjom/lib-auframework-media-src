//
//  AUEscenaTexto.h
//  AUFramework
//
//  Created by Marcos Ortega Morales on 10/11/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef AUESCENATEXTO_H_INCLUIDO
#define AUESCENATEXTO_H_INCLUIDO

#include "AUEscenaContenedor.h"
#include "AUArregloMutable.h"
#include "AUArregloNativoMutable.h"
#include "AUCadenaMutable.h"
#include "nb/fonts/NBText.h"
#include "nb/fonts/NBFontLines.h"
#include "AUFuenteRender.h"
#include "AUEscenaSprite.h"
#include "NBRango.h"
#include "NBCajaAABB.h"

typedef struct STNBTextSceneBox_ {
	STNBAABox		box;		//Box
	STNBRangeI		rng;		//Indexes of lines (for level 0) or groups (for levels > 0)
} STNBTextSceneBox;

class AUEscenaTexto : public AUEscenaObjeto {
	public:
		AUEscenaTexto();
		AUEscenaTexto(AUFuenteRender* fuenteRender);
		AUEscenaTexto(AUEscenaTexto* otraInstancia);
		virtual ~AUEscenaTexto();
		//
		virtual void				syncDataWithOther(const AUEscenaTexto* other);
		//
		ENNBTextLineAlignH			alineacionH() const;
		ENNBTextAlignV				alineacionV() const;
		AUFuenteRender*				fuenteRender() const;
		float						factorSeparacionCaracteres() const;
		float						factorSeparacionLineas() const;
		const char*					texto() const;
		UI32						textoLen() const;
		const STNBText*				getVisualTextDef() const;
		UI32						getCharDefsCount() const;
		UI32						getLinesCount() const;
		UI32						textoDataSeqId() const;
		STNBRect					columnLimit() const;
		void						setColumnLimit(const STNBRect defCol);
		BOOL						allowMultiline() const;
		const STNBTextMetrics*		textMetrics() const;
		void						establecerAlineacionH(ENNBTextLineAlignH alineacionH);
		void						establecerAlineacionV(ENNBTextAlignV alineacionV);
		void						establecerAlineaciones(ENNBTextLineAlignH alineacionH, ENNBTextAlignV alineacionV);
		void						establecerFactorSeparacionCaracteres(float factorSeparacionCaracteres);
		void						establecerFactorSeparacionLineas(float factorSeparacionLineas);
		void						establecerFuenteRender(AUFuenteRender* fuenteRender);
		void						vaciarTexto();
		void						organizarTexto(const float ancho = 0.0f, const float alto = 0.0f);
		void						organizarTextoEnCol(const STNBRect column, const BOOL allowMultiline = TRUE);
		//
		STNBRect					rectForCursor(const UI32 iChar);
		//Mono-format
		void						establecerTexto(const char* texto, const float ancho = 0.0f, const float alto = 0.0f);
		void						establecerTextoBytes(const char* texto, const UI32 textoLen, const float ancho = 0.0f, const float alto = 0.0f);
		void						establecerTextoEnCol(const char* texto, const STNBRect column = NBST_P(STNBRect, 0, 0, 0, 0), const BOOL allowMultiline = TRUE);
		void						establecerTextoBytesEnCol(const char* texto, const UI32 textoLen, const STNBRect column = NBST_P(STNBRect, 0, 0, 0, 0), const BOOL allowMultiline = TRUE);
		void						establecerTextoRepitiendo(const char* strRepetir, const SI32 veces, const float ancho = 0.0f, const float alto = 0.0f);
		void						establecerTextoRepitiendoEnCol(const char* strRepetir, const SI32 veces, const STNBRect column = NBST_P(STNBRect, 0, 0, 0, 0), const BOOL allowMultiline = TRUE);
		void						establecerTexto(const STNBText* texto, const float ancho = 0.0f, const float alto = 0.0f);
		void						establecerTextoEnCol(const STNBText* texto, const STNBRect column = NBST_P(STNBRect, 0, 0, 0, 0), const BOOL allowMultiline = TRUE);
		NBCajaAABB					cajaParaTexto(const char* texto, const float ancho = 0.0f, const float alto = 0.0f);
		NBCajaAABB					cajaParaTextoEnCol(const char* texto, const STNBRect column = NBST_P(STNBRect, 0, 0, 0, 0), const BOOL allowMultiline = TRUE);
		static NBCajaAABB			cajaParaTexto(const char* texto, AUFuenteRender* objFuente, const ENNBTextLineAlignH alineacionH, const ENNBTextAlignV alineacionV, const float ancho = 0.0f, const float alto = 0.0f);
		static NBCajaAABB			cajaParaTextoEnCol(const char* texto, AUFuenteRender* objFuente, const ENNBTextLineAlignH alineacionH, const ENNBTextAlignV alineacionV, const STNBRect column, const BOOL allowMultiline);
		//Multi-format
		void						appendText(AUFuenteRender* font, const char* text);
		void						appendTextFinish();
		//Filtes
		void						pushVisualFilter(AUTextoFiltro* filter);
		void						emptyVisualFilters();
		//
		NBCajaAABB					boxForCurrentText(const BOOL withCursor);
		virtual	NBCajaAABB			cajaAABBLocalCalculada();
		virtual PTRfunCmdsGL		actualizarModeloGL(const NBPropRenderizado &propsRenderizado, const NBPropIluminacion &propsIluminacion, const NBPropHeredadasRender &propsHeredadas);
		//ToDo: remove
		//PTRfunCmdsGL				actualizarModeloGL(const NBPropRenderizado &propsRenderizado, const NBPropIluminacion &propsIluminacion, const NBPropHeredadasRender &propsHeredadas, STBloqueGL* dstVertexBlock, UI8* dstVertexType);
		static void					enviarComandosGL(UI8* punteroDatosModelo);
		//guardar y cargar en XML
		static bool					agregarXmlInternoEn(AUEscenaTexto* texto, AUCadenaLargaMutable8* guardarEn, const char* espaciosBaseIzq);
		static bool					interpretarXmlInterno(AUEscenaTexto* cargarEn, const AUDatosXML* datosXml, const sNodoXML* nodoXml);
		//guardar y cargar en binario
		static bool					agregarBitsInternosEn(AUEscenaTexto* texto, AUArchivo* guardarEn);
		static bool					interpretarBitsInternos(AUEscenaTexto* cargarEn, AUArchivo* cargarDe);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	protected:
		AUFuenteRender*				_defFont;
		STNBRect					_defCol;
		BOOL						_defAllowMultiline;
		STNBText					_textData;
		UI32						_textDataSeqId;		//last text edited
		AUArregloMutable*			_textVisualFilters;	//AUTextoFiltro, to apply to text (NULL if none)
		AUArregloMutable			_textVisuals;		//AUFuenteVisualModel (only populated/updated at rendering)
		UI32						_textVisualsSeqId;	//last visual synced (only populated/updated at rendering)
		//Optimizations (chars boxes multi-levels)
		STNBArray					_textsBoxes;		//STNBFontRenderBox
		STNBArray					_textsBoxesLvls;	//STNBRangeI
		//
		virtual void				agregadoEnEscena();
		virtual void				quitandoDeEscena();
		//
		void						privOrganizarTexto(const STNBRect column, const BOOL allowMultiline);
		void						privSyncVisualModels();
		STNBRect					privDefaultColumn(const float width, const float height);
		static STNBRect				privDefaultColumnForAligns(const float width, const float height, const ENNBTextLineAlignH alignH, const ENNBTextAlignV alignV);
		//
		void						privDbgAccumRenderModelsGL(const STNBRangeI rngAtLvl, const SI32 iLvl, UI32* dstCharsCount);
		void						privAddRenderModelsGL(const NBPropRenderizado &propsRenderizado, const NBPropIluminacion &propsIluminacion, const NBPropHeredadasRender &propsHeredadas, const NBColor colorMultiplicado, const STNBRangeI rngAtLvl, const SI32 iLvl, UI32* dstGrpsAdded, UI32* dstCharsIgnored);
};

#endif
