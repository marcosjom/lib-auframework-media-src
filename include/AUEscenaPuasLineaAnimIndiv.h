//
//  AUEscenaPuasLineaAnimIndiv.h
//  Gameplay_Mac
//
//  Created by Marcos Ortega Morales on 12/10/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef AUEscenaPuasLineaAnimIndivLINEA_H_INCLUIDO
#define AUEscenaPuasLineaAnimIndivLINEA_H_INCLUIDO

#include "AUEscenaObjeto.h"
#include "NBGestorTexturas.h"
#include "NBGestorPersonajes.h"
#include "NBGestorAnimadores.h"
#include "AUEscenaPuasLineaDef.h"

class AUEscenaPuasLineaAnimIndiv: public AUEscenaObjeto, public IPersonajeVictimario, public NBAnimador {
	public:
		AUEscenaPuasLineaAnimIndiv();
		AUEscenaPuasLineaAnimIndiv(AUEscenaPuasLineaAnimIndiv* otraInstancia);
		virtual ~AUEscenaPuasLineaAnimIndiv();
		//
		float				puasLongitudHaciaDerecha() const;
		float				puasSeparacion() const;
		ENPuasLineaTamano	puasTamano() const;
		float				puasSegsParaMostrar() const;
		float				puasSegsParaOcultar() const;
		float				puasAnimHuecoAncho() const;
		float				puasAnimHuecoVel() const;
		float				puasAnimHuecoEspera() const;
		bool				puasAnimHuecoVolver() const;
		bool				puasAnimHuecoIniciarVolviendo() const;
		//
		void				establecerPuasLongitudHaciaDerecha(const float longitud);
		void				establecerPuasSeparacion(const float separacion);
		void				establecerPuasTamano(const ENPuasLineaTamano tamano);
		void				establecerPuasSegsParaMostrar(const float segs);
		void				establecerPuasSegsParaOcultar(const float segs);
		void				establecerPuasAnimHuecoAncho(const float ancho);
		void				establecerPuasAnimHuecoVel(const float vel);
		void				establecerPuasAnimHuecoEspera(const float segs);
		void				establecerPuasAnimHuecoVolver(const bool volver);
		void				establecerPuasAnimHuecoIniciarVolviendo(const bool iniciarVolviendo);
		//
		virtual bool		personajeDanoHaciaVictima(const NBCajaAABB &cajaEscenaVictima, STPersonajeDano* guardarDanoEn);
		virtual void		tickAnimacion(float segsTrancurridos);
		//
		virtual	NBCajaAABB	cajaAABBLocalCalculada();
		virtual PTRfunCmdsGL actualizarModeloGL(const NBPropRenderizado &propsRenderizado, const NBPropIluminacion &propsIluminacion, const NBPropHeredadasRender &propsHeredadas);
		static void			enviarComandosGL(UI8* punteroDatosModelo);
		//guardar y cargar en XML
		static bool			agregarXmlInternoEn(AUEscenaPuasLineaAnimIndiv* lineaParts, AUCadenaLargaMutable8* guardarEn, const char* espaciosBaseIzq);
		static bool			interpretarXmlInterno(AUEscenaPuasLineaAnimIndiv* cargarEn, const AUDatosXML* datosXml, const sNodoXML* nodoXml);
		//guardar y cargar en binario
		static bool			agregarBitsInternosEn(AUEscenaPuasLineaAnimIndiv* lineaParts, AUArchivo* guardarEn);
		static bool			interpretarBitsInternos(AUEscenaPuasLineaAnimIndiv* cargarEn, AUArchivo* cargarDe);
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
		float				_puasLongitudHaciaDerecha;
		float				_puasSeparacion;
		float				_puasSegsParaOcultar;
		float				_puasSegsParaMostrar;
		//Propiedades de anim
		UI8					_puasTipoTamano; //ENPuasLineaTamano
		UI8					_puasAnimTipo; //ENPuasLineaAnimIndiv
		bool				_puasAnimUnHuecoVolver;
		bool				_puasAnimUnHuecoIniciarVolviendo;
		bool				_puasAnimUnHuecoVolviendo;
		float				_puasAnimUnHuecoXInicioHueco;
		float				_puasAnimUnHuecoTamano;
		float				_puasAnimUnHuecoVelMov;
		float				_puasAnimUnHuecoSegsAcumEspera;
		float				_puasAnimUnHuecoSegsEsperar;
		//Animaciones
		AUArregloNativoMutableP<ENPuasLineaPropsAnim> _puasAnimEstados;
		//
		void				privActualizarTexturaPuas();
		void				privPuasActualizar();
};

#endif
