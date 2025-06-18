//
//  AUEscenaLuzSombra.h
//  Gameplay_iOS
//
//  Created by Marcos Ortega on 25/09/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef AUEscenaLuzSombra_H_INCLUIDO
#define AUEscenaLuzSombra_H_INCLUIDO

//#include "AUCuerpoDef.h"
//#include "AUEscenaFigura.h"
#include "NBGestorEscena.h"
#include "NBCajaPolar.h"

class AUEscenaLuzSombra: public AUEscenaObjeto, public IEscenaProductorSombras {
	public:
		AUEscenaLuzSombra(AUArregloNativoP<NBPunto>* puntosFigura, AUTextura* texturaSombra);
		AUEscenaLuzSombra(NBPunto* puntosFigura, UI16 conteoPuntos, AUTextura* texturaSombra);
		AUEscenaLuzSombra(AUEscenaLuzSombra* otraInstancia);
		virtual ~AUEscenaLuzSombra();
		//
		AUTextura*							texturaSombra();
		void								establecerTexturaSombra(AUTextura* textura);
		//
		virtual void						producirSombras2(const NBPropIluminacion &propsIluminacion);
		//
		virtual	NBCajaAABB					cajaAABBLocalCalculada();
		virtual PTRfunCmdsGL				actualizarModeloGL(const NBPropRenderizado &propsRenderizado, const NBPropIluminacion &propsIluminacion, const NBPropHeredadasRender &propsHeredadas);
		//guardar y cargar en XML
		static bool							agregarXmlInternoEn(AUEscenaLuzSombra* sombra, AUCadenaLargaMutable8* guardarEn, const char* espaciosBaseIzq);
		static bool							interpretarXmlInterno(AUEscenaLuzSombra* cargarEn, const AUDatosXML* datosXml, const sNodoXML* nodoXml);
		//guardar y cargar en binario
		static bool							agregarBitsInternosEn(AUEscenaLuzSombra* sombra, AUArchivo* guardarEn);
		static bool							interpretarBitsInternos(AUEscenaLuzSombra* cargarEn, AUArchivo* cargarDe);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	protected:
		bool								_registradoEnMotorSombras;
		AUArregloNativoMutableP<NBPunto>	_puntosFiguraSombra;		//Preferi una instancia por figura, debido a que en la mayoria de los casos, la sombra simplificada consta de una unica figura
		AUTextura*							_texturaSombra;
		NBCajaAABB							_cacheCajaLocalFiguraSombra;
		//
		virtual void						agregadoEnEscena();
		virtual void						quitandoDeEscena();
		void								privActualizarCajaLocalFiguraSombra();
};

#endif
