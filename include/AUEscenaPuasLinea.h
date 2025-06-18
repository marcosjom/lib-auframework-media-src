//
//  AUEscenaPuasLinea.h
//  Gameplay_Mac
//
//  Created by Marcos Ortega Morales on 12/10/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef AUEscenaPuasLineaLINEA_H_INCLUIDO
#define AUEscenaPuasLineaLINEA_H_INCLUIDO

#include "AUEscenaObjeto.h"
#include "NBGestorTexturas.h"
#include "NBGestorPersonajes.h"
#include "AUEscenaPuasLineaDef.h"

class AUEscenaPuasLinea: public AUEscenaObjeto, public IPersonajeVictimario {
	public:
		AUEscenaPuasLinea();
		AUEscenaPuasLinea(AUEscenaPuasLinea* otraInstancia);
		virtual ~AUEscenaPuasLinea();
		//
		float				puasLongitudHaciaDerecha() const;
		float				puasSeparacion() const;
		ENPuasLineaTamano	puasTamano() const;
		//
		void				establecerPuasLongitudHaciaDerecha(const float longitud);
		void				establecerPuasSeparacion(const float separacion);
		void				establecerPuasTamano(const ENPuasLineaTamano tamano);
		//
		virtual bool		personajeDanoHaciaVictima(const NBCajaAABB &cajaEscenaVictima, STPersonajeDano* guardarDanoEn);
		//
		virtual	NBCajaAABB	cajaAABBLocalCalculada();
		virtual PTRfunCmdsGL actualizarModeloGL(const NBPropRenderizado &propsRenderizado, const NBPropIluminacion &propsIluminacion, const NBPropHeredadasRender &propsHeredadas);
		static void			enviarComandosGL(UI8* punteroDatosModelo);
		//guardar y cargar en XML
		static bool			agregarXmlInternoEn(AUEscenaPuasLinea* lineaParts, AUCadenaLargaMutable8* guardarEn, const char* espaciosBaseIzq);
		static bool			interpretarXmlInterno(AUEscenaPuasLinea* cargarEn, const AUDatosXML* datosXml, const sNodoXML* nodoXml);
		//guardar y cargar en binario
		static bool			agregarBitsInternosEn(AUEscenaPuasLinea* lineaParts, AUArchivo* guardarEn);
		static bool			interpretarBitsInternos(AUEscenaPuasLinea* cargarEn, AUArchivo* cargarDe);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	protected:
		virtual void		agregadoEnEscena();
		virtual void		quitandoDeEscena();
	private:
		AUTextura*			_puaTextura;
		NBTamano			_puaTexturaTamano;
		//Propiedades
		UI8					_puasTipoTamano; //ENPuasLineaTamano
		float				_puasLongitudHaciaDerecha;
		float				_puasSeparacion;
		//
		void				privActualizarTexturaPuas();
};

#endif
