//
//  NBGestorFisica.h
//  Gameplay_Mac
//
//  Created by Nicaragua Binary on 07/06/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef NBGestorFisica_h_INCLUIDO
#define NBGestorFisica_h_INCLUIDO

#include "NBGestorFisicaDefs.h"

#ifndef CONFIG_NB_UNSUPPORT_BOX2D

#include "Box2D.h"
#include "AUEscenaObjeto.h"
#include "AUArregloMutable.h"
#include "AUArregloNativoMutable.h"
#include "IAnimadorFisicaAdicional.h"
#include "NBGestorAnimadores.h"
#include "AUEscenaCuerdaDef.h"

#define NBGESTORFISICA_MAX_MUNDOS							16
#define NBGESTORFISICA_ACELEROMETRO_MAXACUM_PARA_PROMEDIAR	30


class IEscuchadorMundoFisica {
	public:
		virtual ~IEscuchadorMundoFisica(){
			//
		}
		virtual void	fisicaUnionDestruyendo(const b2Joint* b2UnionFisica) = 0;
};

	
struct STMundoFisica {
	bool					registroUsado;
	bool					mundoActivo;
	b2World*				mundoFisica;
	IEscuchadorMundoFisica*	escuchadorMundo;
	NBPunto					vGravedadFisica;	//vectorGravedad (en escala de la fisica, metros)
	NBPunto					vVientoFisica;		//vectorViento (en escala de la fisica, metros)
	NBPunto					vGravedadEscena;	//vectorGravedad (en escala de escena, pixeles o puntos)
	NBPunto					vVientoEscena;		//vectorViento (en escala de escena, pixeles o puntos)
	AUArregloNativoMutableP<IAnimadorFisicaAdicional*>*	animadoresAdicionales;
	//
	bool operator==(const STMundoFisica &otro) const {
		return (mundoFisica==otro.mundoFisica);
	}
	bool operator!=(const STMundoFisica &otro) const {
		return !(mundoFisica==otro.mundoFisica);
	}
};

struct STUnionAnimFisica {
	ENAnimFisicaTipo	tipoUnion;
	AUEscenaObjeto*		objetoEscena;
	b2Body*				cuerpo;
	//
	bool operator==(const STUnionAnimFisica &otro) const {
		return (tipoUnion==otro.tipoUnion && objetoEscena==otro.objetoEscena && cuerpo==otro.cuerpo);
	}
	bool operator!=(const STUnionAnimFisica &otro) const {
		return !(tipoUnion==otro.tipoUnion && objetoEscena==otro.objetoEscena && cuerpo==otro.cuerpo);
	}
};

class NBGestorFisica {
	public:
		//constructores y destructores
		static void		inicializar(UI8 iteracionesVelocidad, UI8 iteracionesPosicion);
		static void		finalizar();
		static bool		gestorInicializado();
		//
		static SI32		crearMundoFisica(float gravedadX, float gravedadY, IEscuchadorMundoFisica* escuchador);
		static bool		mundoFisicaActivo(SI32 iMundo);
		static void		destruirMundoFisica(SI32 iMundo);
		static void		establecerMundoActivo(SI32 iMundo, bool mundoActivo);
		static void		establecerEscuchadorDeContactos(SI32 iMundo, b2ContactListener* escuchadorContactos);
		//
		static NBPunto	vectorGravedadFisica(SI32 iMundo);
		static NBPunto	vectorVientoFisica(SI32 iMundo);
		static NBPunto	vectorGravedadEscena(SI32 iMundo);
		static NBPunto	vectorVientoEscena(SI32 iMundo);
		static void		establecerVectorGravedad(SI32 iMundo, float gravXMundo, float gravYMundo);
		static void		establecerVectorViento(SI32 iMundo, float vientoXMundo, float vientoYMundo);
		//
		static void		acumularMuestraDesvioGravedad(float radianesDesvioGravedad);
		static float	radianesDesvioGravedad();
		//
		static void		agregarAnimadorAdicional(SI32 iMundo, IAnimadorFisicaAdicional* animador);
		static void		quitarAnimadorAdicional(SI32 iMundo, IAnimadorFisicaAdicional* animador);
		//
		static void		interseccionesConLinea(SI32 iMundo, b2RayCastCallback *callback, float xIni, float yIni, float xFin, float yFin);
		static void		interseccionesConCajaAABB(SI32 iMundo, b2QueryCallback *callback, const NBCajaAABB &cajaBusqueda);
		//
		static b2Body*	crearCuerpo(SI32 iMundo, const b2BodyDef* defCuerpo);
		static void		destruirCuerpo(SI32 iMundo, b2Body* cuerpo);
		static b2Joint*	crearUnionCuerpos(SI32 iMundo, const b2JointDef* defUnionCuerpos);
		static void		destruirUnionCuerpos(SI32 iMundo, b2Joint* unionCuerpos);
		//uniones entre cuerpos y objetos escena
		static void		agregarUnion(AUEscenaObjeto* objetoEscena, b2Body* cuerpoBox2D, ENAnimFisicaTipo tipoUnion=ENAnimFisicaTipo_SeguirCuerpo);
		static void		actualizarUnion(AUEscenaObjeto* objetoEscena, b2Body* cuerpoBox2D, ENAnimFisicaTipo tipoUnion);
		static void		quitarUnion(AUEscenaObjeto* objetoEscena, b2Body* cuerpoBox2D);
		static void		relinkearUnionesDeCuerpo(SI32 iMundo, b2Body* cuerpoConUniones, b2Body* cuerpoDestino);
		//static void	relinkearUnionCuerpoA(SI32 iMundo, b2Joint* unionFisica, b2Body* cuerpoNuevo);
		//static void	relinkearUnionCuerpoB(SI32 iMundo, b2Joint* unionFisica, b2Body* cuerpoNuevo);
		//
		static void		tickAnimacionesAdicionalesMundos(float segsTranscurridos); //liquidos y otros posibles
		static void		tickAnimacionFisicaMundos(float segsTranscurridos);
		static void		ejecutarUnionesCuerposConObjetosEscena();
	private:
		static bool		_gestorInicializado;
		//Fisica
		static UI8		_iteracionesVelocidad;
		static UI8		_iteracionesPosicion;
		static STMundoFisica _mundosFisica[NBGESTORFISICA_MAX_MUNDOS];
		static AUArregloNativoMutableP<STUnionAnimFisica>* _uniones;
		//Acelerometro
		static SI32		_iMuestraDesvioGravedadMasVieja;
		static float	_sumaMuestrasDesvioGravedad;
		static float	_muestrasDesvioGravedad[NBGESTORFISICA_ACELEROMETRO_MAXACUM_PARA_PROMEDIAR];
		static float	_radiantesDesvioGravedad;
		//DEBUG
		static SI32		_debug_conteoUnionesAgregadas;
		static SI32		_debug_conteoUnionesQuitadas;
};


#endif
#endif
