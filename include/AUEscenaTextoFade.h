//
//  AUEscenaTextoFade.h
//  AUFramework
//
//  Created by Marcos Ortega Morales on 10/11/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef AUEscenaTextoFade_H_INCLUIDO
#define AUEscenaTextoFade_H_INCLUIDO

#include "AUEscenaTexto.h"

struct NBFadeVector {
	NBPunto inicio;
	NBPunto delta;
};

struct NBFadeColor {
	NBColorF inicial;
	NBColorF final;
};

class AUEscenaTextoFade : public AUEscenaTexto {
	public:
		AUEscenaTextoFade();
		AUEscenaTextoFade(AUFuenteRender* fuenteRender);
		AUEscenaTextoFade(AUEscenaTextoFade* otraInstancia);
		virtual ~AUEscenaTextoFade();
		//
		bool			fadeUsaValorAbsoluto();
		bool			fadeUsaCoordLocal();
		NBFadeVector	fadeVector();
		NBFadeColor		fadeColor();
		//
		void			establecerFadeUsarValorAbsoluto(const bool usarValorAbsoluto);
		void			establecerFadeVector(const float xIni,const float yIni, const float deltaX, const float deltaY, const bool esCoordLocal);
		void			establecerFadeVector(const NBPunto ini, const NBPunto fin, const bool esCoordLocal);
		void			establecerFadeColores(const float rIni, const float gIni, const float bIni, const float aIni, const float rFin, const float gFin, const float bFin, const float aFin);
		void			establecerFadeColores(const NBColorF ini, const NBColorF fin);
		//
		virtual PTRfunCmdsGL		actualizarModeloGL(const NBPropRenderizado &propsRenderizado, const NBPropIluminacion &propsIluminacion, const NBPropHeredadasRender &propsHeredadas);
		//guardar y cargar en XML
		static bool					agregarXmlInternoEn(AUEscenaTextoFade* texto, AUCadenaLargaMutable8* guardarEn, const char* espaciosBaseIzq);
		static bool					interpretarXmlInterno(AUEscenaTextoFade* cargarEn, const AUDatosXML* datosXml, const sNodoXML* nodoXml);
		//guardar y cargar en binario
		static bool					agregarBitsInternosEn(AUEscenaTextoFade* texto, AUArchivo* guardarEn);
		static bool					interpretarBitsInternos(AUEscenaTextoFade* cargarEn, AUArchivo* cargarDe);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	private:
		bool			_fadeVectEsLocal;	//Coordenadas locales o de escena
		bool			_fadeUsarValorAbs;	//Valor absoluto
		NBFadeVector	_fadeVector;
		NBFadeColor		_fadeColor;
		//
		void			privAplicarColorFade(NBVerticeGL0* vertices, const UI32 cantVertices, const NBPunto posIniEsc, const NBPunto vecUnitEsc, const float vecModulo);
};

#endif
