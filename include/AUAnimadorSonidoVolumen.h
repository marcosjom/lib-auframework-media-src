//
//  AUAnimadorSonidoVolumen.h
//  AUFramework
//
//  Created by Nicaragua Binary on 27/02/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#if !defined(AUAnimadorSonidoVolumen_H_INCLUIDO) && !defined(CONFIG_NB_UNSUPPORT_AUDIO_IO)
#define AUAnimadorSonidoVolumen_H_INCLUIDO

#include "AUObjeto.h"
#include "NBGestorAnimadores.h"
#include "NBAnimador.h"
#include "AUArregloNativoMutableP.h"
#include "AUEscenaSnd.h"
#include "AUArchivo.h"

struct STDatosSonidoVolumen {
	float	segundo;
	float	volumen;
	//
	bool operator==(const STDatosSonidoVolumen &otro) const {
		return (segundo==otro.segundo && volumen==otro.volumen);
	}
	bool operator!=(const STDatosSonidoVolumen &otro) const {
		return !(segundo==otro.segundo && volumen==otro.volumen);
	}
};

class AUAnimadorSonidoVolumen : public AUObjeto, public NBAnimador {
	public:
		AUAnimadorSonidoVolumen();
		AUAnimadorSonidoVolumen(AUAnimadorSonidoVolumen* otraInstancia);
		virtual ~AUAnimadorSonidoVolumen();
		//
		AUEscenaSnd*				sonidoAnimado();
		float						segundoInicio();			//inicio del sonido
		float						segundoFin();				//fin del sonido
		SI32						repeticiones();				//veces repetir el sonido
		AUArregloNativoP<STDatosSonidoVolumen>* puntosDeVolumen();
		void						establecerSonidoAnimar(AUEscenaSnd* sonido);
		void						establecerSegundoInicio(float segInicio);
		void						establecerSegundoFin(float segFin);
		void						establecerRepeticiones(SI32 repeticiones);
		void						agregarPuntoDeVolumen(float segundo, float volumen);
		//
		virtual void				tickAnimacion(float segsTranscurridos);
		//guardar y cargar en/de binario
		static bool					agregarBitsInternosEn(AUAnimadorSonidoVolumen* animadorSonido, AUArchivo* guardarEn);
		static bool					interpretarBitsInternos(AUAnimadorSonidoVolumen* cargarEn, AUArchivo* cargarDe);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	protected:
		//Datos cache de animacion
		float						_segsAcumulados;
		AUEscenaSnd*				_sonidoAnimar;
		//Parametros animacion
		float						_segundoInicio;			//inicio del sonido
		float						_segundoFin;			//fin del sonido
		SI32						_repeticiones;			//veces repetir el sonido
		AUArregloNativoMutableP<STDatosSonidoVolumen>* _puntosDeVolumen;
		//
		float						_ultimoOffsetSegundos;
		SI32						_conteoRepeticiones;
		SI16						_indicePuntoVolumenActual;
};

#endif
