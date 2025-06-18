//
//  AUEscenaElectroRayo.h
//  Gameplay_Mac
//
//  Created by Marcos Ortega Morales on 12/10/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef AUEscenaElectroRayo_H_INCLUIDO
#define AUEscenaElectroRayo_H_INCLUIDO

#include "AUEscenaObjeto.h"
#include "NBAnimador.h"
#include "NBGestorFisica.h"
#include "NBGestorTexturas.h"
#include "NBGestorAnimadores.h"

struct STRayo {
	UI8		idRayo;				//ID asignado por quien ordeno crear el rayo
	UI8		conteoSegmentos;	//Cantidad de "quiebres" del rayo
	UI16	iPrimerSegmento;	//Indice del primer "quiebre" en el arreglo compartido
	//
	NBPunto posInicio;		//Porsicion de inicio del rayo
	NBPunto posFin;			//Porsicion de inicio del rayo
	float	alcance;		//Distancia/alcance del rayo en direccion al vectorUnitario
	NBPunto vectorUnitario;	//Vector direccion del rayo
	NBPunto vectorUnitIzq;	//Vector izquierda
	//
	float	segsAcumEnPosicion;	//Segundos sin actualizar segmentos de rayo
	float	segundosVive;
	float	segundosVividos;
	float	alpha;
	//
	bool operator==(const STRayo &otro) const {
		return (posInicio==otro.posInicio && alcance==otro.alcance && vectorUnitario==otro.vectorUnitario && vectorUnitario==otro.vectorUnitario);
	}
	bool operator!=(const STRayo &otro) const {
		return !(posInicio==otro.posInicio && alcance==otro.alcance && vectorUnitario==otro.vectorUnitario && vectorUnitario==otro.vectorUnitario);
	}
};

struct STRayoSegmento {
	float avanceRelEnRayo;
	NBPunto posFinSegmento;
	//
	bool operator==(const STRayoSegmento &otro) const {
		return (posFinSegmento==otro.posFinSegmento);
	}
	bool operator!=(const STRayoSegmento &otro) const {
		return !(posFinSegmento==otro.posFinSegmento);
	}
};

struct STRayoRender {
	UI16 cantidadRayos;
};
	
class AUEscenaElectroRayo: public AUEscenaObjeto, public NBAnimador {
	public:
		AUEscenaElectroRayo();
		virtual ~AUEscenaElectroRayo();
		//
		UI16				conteoRayosActivos();
		AUArregloNativoP<STRayo>*	rayosActivos();
		bool				existeRayoConId(const UI8 idRayo);
		void				agregarRayo(const UI8 idRayo, const NBPunto &posInicio, const NBPunto &posFin, float duracion);
		void				quitarRayo(const UI8 idRayo);
		void				actualizarRayoPosiciones(const UI8 idRayo, const NBPunto &posInicio, const NBPunto &posFin);
		//
		virtual void		tickAnimacion(float segsTranscurridos);
		//
		virtual	NBCajaAABB	cajaAABBLocalCalculada();
		virtual PTRfunCmdsGL actualizarModeloGL(const NBPropRenderizado &propsRenderizado, const NBPropIluminacion &propsIluminacion, const NBPropHeredadasRender &propsHeredadas);
		static void			enviarComandosGL(UI8* punteroDatosModelo);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	protected:
		AUArregloNativoMutableP<STRayo>	_rayos;
		AUArregloNativoMutableP<STRayoSegmento>	_rayosSegmentos;
		//
		bool				_rayosCajaLocalSucia;
		NBCajaAABB			_rayosCajaLocal;
		//
		virtual void		agregadoEnEscena();
		virtual void		quitandoDeEscena();
};

#endif
