//
//  AUEscenaPuasLineaAnimSinc.h
//  Gameplay_Mac
//
//  Created by Marcos Ortega Morales on 12/10/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef AUEscenaPuasLineaAnimSincLINEA_H_INCLUIDO
#define AUEscenaPuasLineaAnimSincLINEA_H_INCLUIDO

#include "AUEscenaObjeto.h"
#include "NBGestorTexturas.h"
#include "NBGestorPersonajes.h"
#include "NBGestorAnimadores.h"
#include "AUEscenaPuasLineaDef.h"

class AUEscenaPuasLineaAnimSinc: public AUEscenaObjeto, public IPersonajeVictimario, public NBAnimador {
	public:
		AUEscenaPuasLineaAnimSinc();
		AUEscenaPuasLineaAnimSinc(AUEscenaPuasLineaAnimSinc* otraInstancia);
		virtual ~AUEscenaPuasLineaAnimSinc();
		//
		float				puasLongitudHaciaDerecha() const;
		float				puasSeparacion() const;
		ENPuasLineaTamano	puasTamano() const;
		float				puasSegsMostrar() const;
		float				puasSegsOcultar() const;
		float				puasSegsParaMostrar() const;
		float				puasSegsParaOcultar() const;
		//
		void				establecerPuasLongitudHaciaDerecha(const float longitud);
		void				establecerPuasSeparacion(const float separacion);
		void				establecerPuasTamano(const ENPuasLineaTamano tamano);
		void				establecerPuasSegsMostrar(const float segs);
		void				establecerPuasSegsOcultar(const float segs);
		void				establecerPuasSegsParaMostrar(const float segs);
		void				establecerPuasSegsParaOcultar(const float segs);
		//
		virtual bool		personajeDanoHaciaVictima(const NBCajaAABB &cajaEscenaVictima, STPersonajeDano* guardarDanoEn);
		virtual void		tickAnimacion(float segsTrancurridos);
		//
		virtual	NBCajaAABB	cajaAABBLocalCalculada();
		virtual PTRfunCmdsGL actualizarModeloGL(const NBPropRenderizado &propsRenderizado, const NBPropIluminacion &propsIluminacion, const NBPropHeredadasRender &propsHeredadas);
		static void			enviarComandosGL(UI8* punteroDatosModelo);
		//guardar y cargar en XML
		static bool			agregarXmlInternoEn(AUEscenaPuasLineaAnimSinc* lineaParts, AUCadenaLargaMutable8* guardarEn, const char* espaciosBaseIzq);
		static bool			interpretarXmlInterno(AUEscenaPuasLineaAnimSinc* cargarEn, const AUDatosXML* datosXml, const sNodoXML* nodoXml);
		//guardar y cargar en binario
		static bool			agregarBitsInternosEn(AUEscenaPuasLineaAnimSinc* lineaParts, AUArchivo* guardarEn);
		static bool			interpretarBitsInternos(AUEscenaPuasLineaAnimSinc* cargarEn, AUArchivo* cargarDe);
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
		float				_puasSegsParaOcultar;
		float				_puasSegsParaMostrar;
		float				_puasSegsOcultar;
		float				_puasSegsMostrar;
		//Animacion
		ENPuasLineaPropsAnim _puasAnimEstado;
		//
		void				privActualizarTexturaPuas();
};

#endif
