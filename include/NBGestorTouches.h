//
//  AUGameplayTouches.h
//  EditorCuerpos
//
//  Created by Marcos Ortega Morales on 16/12/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef NBGESTORTOUCHES_H_INCLUIDO
#define NBGESTORTOUCHES_H_INCLUIDO

#include "nb/core/NBArray.h"
#include "nb/2d/NBPoint.h"
#include "nb/2d/NBSize.h"


#include "NBPunto.h"
#include "AUArregloNativoMutableP.h"

#define NBGESTORTOUCHES_MAX_TOUCHES_SIMULTANEOS						10
#define NBGESTORTOUCHES_SEGUNDOS_MAXIMO_PERMITIR_TOUCH_INACTIVO		15.0f

//Scroll over position
typedef struct STGTouchScroll_ {
	STNBPoint		posPort;	//origin position
	STNBSize		size;		//size of the scroll
	BOOL			animate;	//if FALSE the the scroll is already animated
} STGTouchScroll;

//Scroll over position
typedef struct STGTouchMagnify_ {
	STNBPoint		posPort;		//origin position
	float			magnification;	//
	BOOL			isSmarthMag;	//if TRUE the app must automatically magify
} STGTouchMagnify;

//Estructura publica de un touch
typedef struct STGTouch_ {
	bool			cancelado;						//en el caso de "ENTouchEstado_Finalizado" indica si este fue finalizado por un evento del sistema (tal y como una llamada telefonica)
	//
	NBPuntoI		posSuperficieIni;				//posicion de inicio de touch (en coordenadas de SuperficieTouch)
	NBPuntoI		posSuperficieAnt;				//posicion del touch antes de la ultima actualizacion (en coordenadas de SuperficieTouch)
	STNBArray		posSuperficieMissed;			//NBPuntoI, posiciones del touch notificadas pero no procesadas
	NBPuntoI		posSuperficieAct;				//posicion actual del touch (en coordenadas de SuperficieTouch)
	//
	NBPuntoF		posEscenaIni;					//posicion de inicio de touch (en coordenadas del grupo de escena)
	NBPuntoF		posEscenaAnt;					//posicion del touch antes de la ultima actualizacion (en coordenadas del grupo de escena)
	STNBArray		posEscenaMissed;				//NBPuntoF, posiciones del touch notificadas pero no procesadas
	NBPuntoF		posEscenaAct;					//osicion actual del touch (en coordenadas del grupo de escena)
	//
	SI32			marcaConsumidor;				//marca establecida por el consumidor del touch
	void*			datosAdicionales;				//puntero a datos de interes para el consumidor del touch
	//
	bool operator==(const struct STGTouch_ &otro) const {
		return (cancelado == otro.cancelado && posSuperficieIni == otro.posSuperficieIni && posSuperficieAnt == otro.posSuperficieAnt && posSuperficieAct == otro.posSuperficieAct);
	}
	bool operator!=(const struct STGTouch_ &otro) const {
		return !(cancelado == otro.cancelado && posSuperficieIni == otro.posSuperficieIni && posSuperficieAnt == otro.posSuperficieAnt && posSuperficieAct == otro.posSuperficieAct);
	}
} STGTouch;

//Estructura privada de un touch
typedef struct STGTouchPriv_ {
	bool			registroOcupado;
	bool			marcadoComoFinalizado;			//Touch finalizado sobre la superficie
	UI32			movsAccumAtTick;				//Touch movido en la superficie (durante tick actual)
	UI64 			identificadorEnOS;				//identificador unico de este touch en el sistema operativo (nota: el SO puede reciclar estos dientificadores)
	UI16			ordenDeAparicion;				//orden en que han aparecido los touches aun activos (nota: es reiniciado a cero cuando todos los touches han desaparecido)
	UI16			ticksAcumulados;				//cantidad de ticks que ha durado el touch (cero si es nuevo touch)
	float			debugSegSinActualizar;			//tiempo en segundos transcurridos desde la ultima actualizacion del touch
	//
	AUObjeto*		consumidor;						//AUEscenaObjeto que esta consumiendo el touch
	AUObjeto*		consumidorEscObjeto;			//AUObjeto escuchador determinado al inicio del touch (se mantiene hasta que el touch finaliza)
	void*			consumidorEscInterfaz;			//Interfaz del escuchador determinado al inicio del touch (se mantiene hasta que el touch finaliza)
	//
	STGTouch		t;								//touch
	//
	bool operator==(const struct STGTouchPriv_ &otro) const {
		return (identificadorEnOS == otro.identificadorEnOS && ordenDeAparicion == otro.ordenDeAparicion);
	}
	bool operator!=(const struct STGTouchPriv_ &otro) const {
		return !(identificadorEnOS == otro.identificadorEnOS && ordenDeAparicion == otro.ordenDeAparicion);
	}
} STGTouchPriv;

//Estructura privada de un touch-hover
typedef struct STGHoverPriv_ {
	bool			registroOcupado;
	bool			marcadoComoMovido;		//Touch movido en la superficie
	UI64 			identificadorEnOS;		//identificador unico de este touch en el sistema operativo (nota: el SO puede reciclar estos dientificadores)
	//
	NBPuntoI		posSuperficieAct;		//posicion actual del touch (en coordenadas de SuperficieTouch)
	AUObjeto*		consumidor;				//AUEscenaObjeto que esta consumiendo el touch
	AUObjeto*		consumidorEscObjeto;	//AUObjeto escuchador determinado al inicio del touch (se mantiene hasta que el touch finaliza)
	void*			consumidorEscInterfaz;	//Interfaz del escuchador determinado al inicio del touch (se mantiene hasta que el touch finaliza)
	//
	bool operator==(const struct STGHoverPriv_ &otro) const {
		return (identificadorEnOS == otro.identificadorEnOS);
	}
	bool operator!=(const struct STGHoverPriv_ &otro) const {
		return !(identificadorEnOS == otro.identificadorEnOS);
	}
} STGHoverPriv;

class NBGestorTouches {
	public:
		static void					inicializar();
		static void					finalizar();
		//
		static bool					gestorInicializado();
		//Mutex
		static void					mutexActivar();
		static void					mutexDesactivar();
		//Consumidores
		//static SI32				consumidorNulificar(AUObjeto* consumidor); //Todo: eliminar
		//Scroll
		static void					scrollAdd(const STNBPoint posPort, const STNBSize scrollSz, const BOOL animate);
		static STGTouchScroll*		scrollsPend(UI32* dstCount);
		static void					scrollsPendClear();
		//Magnify
		static void					magnifyAdd(const STNBPoint posPort, const float magnification, const BOOL isSmathMag);
		static STGTouchMagnify*		magnifyPend(UI32* dstCount);
		static void					magnifyPendClear();
		//Touches (métodos del consumidor de eventos)
		static STGTouchPriv*		touchesArreglo();
		static UI16					touchesArregloTamano();
		static UI16					touchesConteoActivos();
		static AUObjeto*			touchConsumidor(const STGTouch* touch, AUObjeto** outConsumidorEscuchadorObj, void** outConsumidorEscuchadorInterfaz);
		static void					touchEstablecerConsumidor(const STGTouch* touch, AUObjeto* consumidor, AUObjeto* consumidorEscuchadorObj, void* consumidorEscuchadorInterfaz);
		static void					touchConsumirEstados();
		//Touches (métodos del productor de eventos)
		static void					touchIniciar(const UI64 identificadorEnOS, const SI32 posX, const SI32 posY);
		static void					touchMover(const UI64 identificadorEnOS, const SI32 posX, const SI32 posY);
		static void					touchFinalizar(const UI64 identificadorEnOS, const SI32 posX, const SI32 posY, const bool cancelado);
		//Hover (métodos del consumidor de eventos)
		static STGHoverPriv*		hoverArreglo();
		static UI16					hoverArregloTamano();
		//Hover (métodos del productor de eventos)
		static void					hoverMover(const UI64 identificadorEnOS, const SI32 posX, const SI32 posY);
		//Métodos de depuración
		static void					debugTick(const float segundosTranscurridos);
	protected:
		static bool					_gestorInicializado;
		//Scroll
		static STNBArray			_scrollsPend;	//STGTouchScroll
		static NBHILO_MUTEX_CLASE	_scrollsMutex;
		//Magnify
		static STNBArray			_magnifyPend;	//STGTouchMagnify
		static NBHILO_MUTEX_CLASE	_magnifyMutex;
		//Touches
		static UI16					_touchesSecuencial;
		static UI16					_touchesTamano;
		static STGTouchPriv*		_touches;
		static NBHILO_MUTEX_CLASE	_touchesMutex;
		//Touches-hover
		static UI16					_hoversTamano;
		static STGHoverPriv*		_hovers;
		static NBHILO_MUTEX_CLASE	_hoversMutex;
		//
		static STGTouchPriv*		privTouchFreeRecord();
		static STGTouchPriv*		privTouchById(const UI64 idAtOS, const bool markedAsFinalized); //El identificadorOS puede repetirse (solo uno activo y el resto marcados como finalizados)
		static STGTouchPriv*		privTouchByPublicPointer(const STGTouch* publicPointer);
		static void					privTouchMovedLocked(STGTouchPriv* record, const SI32 posX, const SI32 posY);
		//
		static STGHoverPriv*		privHoverFreeRecord();
		static STGHoverPriv*		privHoverById(const UI64 idAtOS);
};

#endif
