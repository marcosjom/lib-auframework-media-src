//
//  AUGameplayTouches.cpp
//  EditorCuerpos
//
//  Created by Marcos Ortega Morales on 16/12/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkMediaStdAfx.h"
#include "NBGestorTouches.h"
//
#include "nb/core/NBMemory.h"

bool			NBGestorTouches::_gestorInicializado	= false;
//Scroll
STNBArray NBGestorTouches::_scrollsPend;	//STGTouchScroll
NBHILO_MUTEX_CLASE NBGestorTouches::_scrollsMutex;
//Magnify
STNBArray NBGestorTouches::_magnifyPend;	//STGTouchMagnify
NBHILO_MUTEX_CLASE NBGestorTouches::_magnifyMutex;
//Touches
UI16			NBGestorTouches::_touchesSecuencial	= 0;
UI16			NBGestorTouches::_touchesTamano		= 0;
STGTouchPriv*	NBGestorTouches::_touches			= NULL;
NBHILO_MUTEX_CLASE NBGestorTouches::_touchesMutex;
//Touches-hover
UI16				NBGestorTouches::_hoversTamano	= 0;
STGHoverPriv*	NBGestorTouches::_hovers			= NULL;
NBHILO_MUTEX_CLASE	NBGestorTouches::_hoversMutex;


void NBGestorTouches::inicializar(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTouches::inicializar")
	_gestorInicializado = false;
	//Scroll
	{
		NBArray_init(&_scrollsPend, sizeof(STGTouchScroll), NULL);
		NBHILO_MUTEX_INICIALIZAR(&_scrollsMutex);
	}
	//Magnify
	{
		NBArray_init(&_magnifyPend, sizeof(STGTouchMagnify), NULL);
		NBHILO_MUTEX_INICIALIZAR(&_magnifyMutex);
	}
	//Touches
	{
		_touchesTamano		= NBGESTORTOUCHES_MAX_TOUCHES_SIMULTANEOS;
		_touches			= (STGTouchPriv*)NBGestorMemoria::reservarMemoria(sizeof(STGTouchPriv) * _touchesTamano, ENMemoriaTipo_General); NB_DEFINE_NOMBRE_PUNTERO(_touches, "NBGestorTouches::_touches");
		_touchesSecuencial 	= 0;
		SI32 iReg;
		for(iReg = 0; iReg < _touchesTamano; iReg++){
			STGTouchPriv* rec = &_touches[iReg];
			rec->registroOcupado = false;
			NBArray_init(&rec->t.posSuperficieMissed, sizeof(NBPuntoI), NULL);
			NBArray_init(&rec->t.posEscenaMissed, sizeof(NBPuntoF), NULL);
		}
		NBHILO_MUTEX_INICIALIZAR(&_touchesMutex);
	}
	//Touches-hover
	{
		_hoversTamano		= NBGESTORTOUCHES_MAX_TOUCHES_SIMULTANEOS;
		_hovers				= (STGHoverPriv*)NBGestorMemoria::reservarMemoria(sizeof(STGHoverPriv) * _hoversTamano, ENMemoriaTipo_General); NB_DEFINE_NOMBRE_PUNTERO(_touches, "NBGestorTouches::_hovers");
		_touchesSecuencial 	= 0;
		SI32 iReg;
		for(iReg = 0; iReg < _hoversTamano; iReg++){
			_hovers[iReg].registroOcupado = false;
		}
		NBHILO_MUTEX_INICIALIZAR(&_hoversMutex);
	}
	_gestorInicializado = true;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

void NBGestorTouches::finalizar(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTouches::finalizar")
	//Scroll
	{
		NBHILO_MUTEX_ACTIVAR(&_scrollsMutex);
		{
			NBArray_empty(&_scrollsPend);
			NBArray_release(&_scrollsPend);
		}
		NBHILO_MUTEX_DESACTIVAR(&_scrollsMutex);
		NBHILO_MUTEX_FINALIZAR(&_scrollsMutex);
	}
	//Magnify
	{
		NBHILO_MUTEX_ACTIVAR(&_magnifyMutex);
		{
			NBArray_empty(&_magnifyPend);
			NBArray_release(&_magnifyPend);
		}
		NBHILO_MUTEX_DESACTIVAR(&_magnifyMutex);
		NBHILO_MUTEX_FINALIZAR(&_magnifyMutex);
	}
	//
	{
		NBHILO_MUTEX_ACTIVAR(&_touchesMutex);
		if(_touches != NULL){
			SI32 iReg;
			for(iReg = 0; iReg < _touchesTamano; iReg++){
				STGTouchPriv* rec = &_touches[iReg];
				if(rec->registroOcupado){
					if(rec->consumidor != NULL){
						rec->consumidor->liberar(NB_RETENEDOR_NULL);
						rec->consumidor = NULL;
					}
					if(rec->consumidorEscObjeto != NULL){
						rec->consumidorEscObjeto->liberar(NB_RETENEDOR_NULL);
						rec->consumidorEscObjeto = NULL;
					}
					rec->consumidorEscInterfaz = NULL;
				}
				NBArray_release(&rec->t.posSuperficieMissed);
				NBArray_release(&rec->t.posEscenaMissed);
			}
			NBGestorMemoria::liberarMemoria(_touches);
			_touches = NULL;
		}
		NBHILO_MUTEX_DESACTIVAR(&_touchesMutex);
		NBHILO_MUTEX_FINALIZAR(&_touchesMutex);
	}
	//
	{
		NBHILO_MUTEX_ACTIVAR(&_hoversMutex);
		if(_hovers != NULL){
			SI32 iReg;
			for(iReg = 0; iReg < _hoversTamano; iReg++){
				STGHoverPriv* rec = &_hovers[iReg];
				if(rec->registroOcupado){
					if(rec->consumidor != NULL){
						rec->consumidor->liberar(NB_RETENEDOR_NULL);
						rec->consumidor = NULL;
					}
					if(rec->consumidorEscObjeto != NULL){
						rec->consumidorEscObjeto->liberar(NB_RETENEDOR_NULL);
						rec->consumidorEscObjeto = NULL;
					}
					rec->consumidorEscInterfaz = NULL;
				}
			}
			NBGestorMemoria::liberarMemoria(_hovers);
			_hovers = NULL;
		}
		NBHILO_MUTEX_DESACTIVAR(&_hoversMutex);
		NBHILO_MUTEX_FINALIZAR(&_hoversMutex);
	}
	_gestorInicializado = false;
	//
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

bool NBGestorTouches::gestorInicializado(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTouches::gestorInicializado")
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return _gestorInicializado;
}

//

STGTouchPriv* NBGestorTouches::privTouchFreeRecord(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTouches::privTouchFreeRecord")
	STGTouchPriv* r = NULL;
	UI16 i;
	for(i = 0; i < _touchesTamano; i++){
		STGTouchPriv* record = &_touches[i];
		if(!record->registroOcupado){
			r = record;
			break;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return r;
}

STGTouchPriv* NBGestorTouches::privTouchById(const UI64 idAtOS, const bool markedAsFinalized){ //El identificadorOS puede repetirse (solo uno activo y el resto marcados como finalizados)
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTouches::privTouchById")
	STGTouchPriv* r = NULL;
	UI16 i;
	for(i = 0; i < _touchesTamano; i++){
		STGTouchPriv* record = &_touches[i];
		if(record->registroOcupado){
			if(record->identificadorEnOS == idAtOS && record->marcadoComoFinalizado == markedAsFinalized){
				r = record;
				break;
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return r;
}

STGTouchPriv* NBGestorTouches::privTouchByPublicPointer(const STGTouch* publicPointer){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTouches::privTouchByPublicPointer")
	STGTouchPriv* r = NULL;
	UI16 i;
	for(i = 0; i < _touchesTamano; i++){
		STGTouchPriv* record = &_touches[i];
		if(record->registroOcupado){
			if(&record->t == publicPointer){
				r = record;
				break;
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return r;
}

//

STGHoverPriv* NBGestorTouches::privHoverFreeRecord(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTouches::privHoverFreeRecord")
	STGHoverPriv* r = NULL;
	UI16 i;
	for(i = 0; i < _hoversTamano; i++){
		STGHoverPriv* record = &_hovers[i];
		if(!record->registroOcupado){
			r = record;
			break;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return r;
}

STGHoverPriv* NBGestorTouches::privHoverById(const UI64 idAtOS){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTouches::privHoverById")
	STGHoverPriv* r = NULL;
	UI16 i;
	for(i = 0; i < _hoversTamano; i++){
		STGHoverPriv* record = &_hovers[i];
		if(record->registroOcupado){
			if(record->identificadorEnOS == idAtOS){
				r = record;
				break;
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return r;
}

// Mutex

void NBGestorTouches::mutexActivar(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTouches::mutexActivar")
	NBHILO_MUTEX_ACTIVAR(&_scrollsMutex);
	NBHILO_MUTEX_ACTIVAR(&_magnifyMutex);
	NBHILO_MUTEX_ACTIVAR(&_touchesMutex);
	NBHILO_MUTEX_ACTIVAR(&_hoversMutex);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

void NBGestorTouches::mutexDesactivar(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTouches::mutexDesactivar")
	NBHILO_MUTEX_DESACTIVAR(&_hoversMutex);
	NBHILO_MUTEX_DESACTIVAR(&_touchesMutex);
	NBHILO_MUTEX_DESACTIVAR(&_magnifyMutex);
	NBHILO_MUTEX_DESACTIVAR(&_scrollsMutex);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

//Consumidores
//Todo: eliminar
/*SI32 NBGestorTouches::consumidorNulificar(AUObjeto* consumidor){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTouches::consumidorNulificar")
	//Este consumidor se está eliminando, buscarlo e inhabiltarlo donde aparezca.
	SI32 r = 0;
	//En touches
	{
		UI16 i;
		for(i=0; i<_touchesTamano; i++){
			STGTouchPriv* record = &_touches[i];
			if(record->registroOcupado){
				if(record->consumidor == consumidor){
					record->consumidor = NULL;
					r++;
				}
			}
		}
	}
	//En hovers
	{
		UI16 i;
		for(i = 0; i < _hoversTamano; i++){
			STGHoverPriv* record = &_hovers[i];
			if(record->registroOcupado){
				if(record->consumidor == consumidor){
					record->consumidor = NULL;
					r++;
				}
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return r;
}*/

//-----------------------------------------------
//- Scroll
//-----------------------------------------------

void NBGestorTouches::scrollAdd(const STNBPoint posPort, const STNBSize scrollSz, const BOOL animate){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTouches::scrollAdd")
	NBHILO_MUTEX_ACTIVAR(&_scrollsMutex);
	{
		STGTouchScroll s;
		NBMemory_setZeroSt(s, STGTouchScroll);
		s.posPort	= posPort;
		s.size		= scrollSz;
		s.animate	= animate;
		NBArray_addValue(&_scrollsPend, s);
	}
	NBHILO_MUTEX_DESACTIVAR(&_scrollsMutex);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

STGTouchScroll* NBGestorTouches::scrollsPend(UI32* dstCount){
	STGTouchScroll* r = NULL;
	if(_scrollsPend.use > 0){
		if(dstCount != NULL) *dstCount = _scrollsPend.use;
		r = (STGTouchScroll*)NBArray_data(&_scrollsPend);
	}
	return r;
}

void NBGestorTouches::scrollsPendClear(){
	NBArray_empty(&_scrollsPend);
}

//-----------------------------------------------
//- Magnify
//-----------------------------------------------

void NBGestorTouches::magnifyAdd(const STNBPoint posPort, const float magnification, const BOOL isSmarthMag){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTouches::magnifyAdd")
	NBHILO_MUTEX_ACTIVAR(&_magnifyMutex);
	{
		STGTouchMagnify s;
		NBMemory_setZeroSt(s, STGTouchMagnify);
		s.posPort			= posPort;
		s.magnification		= magnification;
		s.isSmarthMag		= isSmarthMag;
		NBArray_addValue(&_magnifyPend, s);
	}
	NBHILO_MUTEX_DESACTIVAR(&_magnifyMutex);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

STGTouchMagnify* NBGestorTouches::magnifyPend(UI32* dstCount){
	STGTouchMagnify* r = NULL;
	if(_magnifyPend.use > 0){
		if(dstCount != NULL) *dstCount = _magnifyPend.use;
		r = (STGTouchMagnify*)NBArray_data(&_magnifyPend);
	}
	return r;
}

void NBGestorTouches::magnifyPendClear(){
	NBArray_empty(&_magnifyPend);
}

//-----------------------------------------------
//- Touches (métodos del consumidor de eventos)
//-----------------------------------------------

STGTouchPriv* NBGestorTouches::touchesArreglo(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTouches::touchesArreglo")
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return _touches;
}

UI16 NBGestorTouches::touchesArregloTamano(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTouches::touchesArregloTamano")
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return _touchesTamano;
}

UI16 NBGestorTouches::touchesConteoActivos(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTouches::touchesConteoActivos")
	NBHILO_MUTEX_ACTIVAR(&_touchesMutex);
	UI16 iReg, conteoActivos = 0;
	for(iReg=0; iReg<_touchesTamano; iReg++){
		if(_touches[iReg].registroOcupado) conteoActivos++;
	}
	NBHILO_MUTEX_DESACTIVAR(&_touchesMutex);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return conteoActivos;
}

AUObjeto* NBGestorTouches::touchConsumidor(const STGTouch* touch, AUObjeto** outConsumidorEscuchadorObj, void** outConsumidorEscuchadorInterfaz){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTouches::touchConsumidor")
	AUObjeto* r = NULL;
	STGTouchPriv* record = NBGestorTouches::privTouchByPublicPointer(touch);
	if(record != NULL){
		r = record->consumidor;
		if(outConsumidorEscuchadorObj != NULL){
			*outConsumidorEscuchadorObj = record->consumidorEscObjeto;
		}
		if(outConsumidorEscuchadorInterfaz != NULL){
			*outConsumidorEscuchadorInterfaz = record->consumidorEscInterfaz;
		}
	}
	NBASSERT(r != NULL) //PRINTF_WARNING("NBGestorTouches, el touch-cuyo consumidor se consulta no esta registrado en el gestor\n");
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return r;
}

void NBGestorTouches::touchEstablecerConsumidor(const STGTouch* touch, AUObjeto* consumidorTouch, AUObjeto* consumidorEscuchadorObj, void* consumidorEscuchadorInterfaz){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTouches::touchEstablecerConsumidor")
	NBASSERT((consumidorTouch == NULL && consumidorEscuchadorObj == NULL && consumidorEscuchadorInterfaz == NULL) || (consumidorTouch != NULL && consumidorEscuchadorObj != NULL && consumidorEscuchadorInterfaz != NULL)) //Los tres deben ser nulo o los tres deben no-nulo.
	STGTouchPriv* record = NBGestorTouches::privTouchByPublicPointer(touch);
	if(record != NULL){
		if(consumidorTouch != NULL) consumidorTouch->retener(NB_RETENEDOR_NULL);
		if(consumidorEscuchadorObj != NULL) consumidorEscuchadorObj->retener(NB_RETENEDOR_NULL);
		if(record->consumidor != NULL) record->consumidor->liberar(NB_RETENEDOR_NULL);
		if(record->consumidorEscObjeto != NULL) record->consumidorEscObjeto->liberar(NB_RETENEDOR_NULL);
		record->consumidor				= consumidorTouch;
		record->consumidorEscObjeto		= consumidorEscuchadorObj;
		record->consumidorEscInterfaz	= consumidorEscuchadorInterfaz;
	}
	NBASSERT(record != NULL)
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

void NBGestorTouches::touchConsumirEstados(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTouches::touchConsumirEstados")
	SI32 iReg; SI32 conteoTouchesActivos = 0;
	for(iReg = 0; iReg < _touchesTamano; iReg++){
		STGTouchPriv* datosTouch = &_touches[iReg];
		if(datosTouch->registroOcupado){
			if(datosTouch->marcadoComoFinalizado){
				datosTouch->registroOcupado = false;
				if(datosTouch->consumidor != NULL){
					datosTouch->consumidor->liberar(NB_RETENEDOR_NULL);
					datosTouch->consumidor = NULL;
				}
				if(datosTouch->consumidorEscObjeto != NULL){
					datosTouch->consumidorEscObjeto->liberar(NB_RETENEDOR_NULL);
					datosTouch->consumidorEscObjeto = NULL;
				}
				datosTouch->consumidorEscInterfaz = NULL;
			} else {
				datosTouch->t.posSuperficieAnt	= datosTouch->t.posSuperficieAct;
				datosTouch->t.posEscenaAnt		= datosTouch->t.posEscenaAct;
				datosTouch->ticksAcumulados++;
				conteoTouchesActivos++;
			}
			if(datosTouch->movsAccumAtTick > 0){
				//PRINTF_INFO("NBGestorTouches::moved %d times at tick.\n", datosTouch->movsAccumAtTick);
			}
			datosTouch->movsAccumAtTick = 0;
			NBArray_empty(&datosTouch->t.posSuperficieMissed);
			NBArray_empty(&datosTouch->t.posEscenaMissed);
		}
	}
	//Reiniciar variables
	if(conteoTouchesActivos==0) _touchesSecuencial = 0;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

//-----------------------------------------------
//- Touches (métodos del productor de eventos)
//-----------------------------------------------

void NBGestorTouches::touchIniciar(const UI64 identificadorEnOS, const SI32 posX, const SI32 posY){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTouches::touchIniciar")
	NBHILO_MUTEX_ACTIVAR(&_touchesMutex);
	//Intentar revivir el touch-recien-finalizado que usa el mismo identificadorEnOS
	STGTouchPriv* record = NBGestorTouches::privTouchById(identificadorEnOS, true);
	if(record){
		//ToDo: remove, this NBASSERT is not valid.
		//NBASSERT(record->ticksAcumulados != 0)	//Si falla, el touch-ended no ha sido notificado a su propietario
		PRINTF_INFO("NBGestorTouches, touch-back before touch-end was consumed, reactiving after %d ticks; touch(%lld).\n", record->ticksAcumulados, (SI64)identificadorEnOS);
		record->marcadoComoFinalizado		= false;
		record->t.cancelado					= false;
		record->debugSegSinActualizar		= 0.0f;
		//Move
		NBGestorTouches::privTouchMovedLocked(record, posX, posY);
	}
	//Registrar datos de nuevo touch
	if(!record){
		record = NBGestorTouches::privTouchFreeRecord();
		NBASSERT(record != NULL) //Si falla, se supero el limite de touches
		if(record){
			record->registroOcupado			= true;
			record->marcadoComoFinalizado	= false;
			record->movsAccumAtTick			= 0;
			record->identificadorEnOS 		= identificadorEnOS;
			record->ordenDeAparicion		= _touchesSecuencial++;
			record->ticksAcumulados			= 0;
			record->consumidor				= NULL;
			record->consumidorEscObjeto		= NULL;
			record->consumidorEscInterfaz	= NULL;
			record->debugSegSinActualizar	= 0.0f;
			//
			record->t.cancelado				= false;
			record->t.posSuperficieIni.x 	= posX;
			record->t.posSuperficieIni.y 	= posY;
			record->t.posSuperficieAnt.x 	= posX;
			record->t.posSuperficieAnt.y 	= posY;
			NBArray_empty(&record->t.posSuperficieMissed);
			record->t.posSuperficieAct.x	= posX;
			record->t.posSuperficieAct.y	= posY;
			record->t.posEscenaIni.x		= 0.0f;
			record->t.posEscenaIni.y		= 0.0f;
			record->t.posEscenaAnt.x		= 0.0f;
			record->t.posEscenaAnt.y		= 0.0f;
			NBArray_empty(&record->t.posEscenaMissed);
			record->t.posEscenaAct.x		= 0.0f;
			record->t.posEscenaAct.y		= 0.0f;
			//
			record->t.marcaConsumidor		= -1;
			record->t.datosAdicionales		= NULL;
		}
	}
	NBHILO_MUTEX_DESACTIVAR(&_touchesMutex);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

void NBGestorTouches::privTouchMovedLocked(STGTouchPriv* record, const SI32 posX, const SI32 posY){
	record->debugSegSinActualizar		= 0.0f;
	if(record->ticksAcumulados == 0){
		//Considerar como si el touch fue iniciado en este punto
		record->t.posSuperficieIni.x 	= posX;
		record->t.posSuperficieIni.y 	= posY;
		record->t.posSuperficieAnt.x 	= posX;
		record->t.posSuperficieAnt.y 	= posY;
		record->t.posSuperficieAct.x	= posX;
		record->t.posSuperficieAct.y	= posY;
	} else {
		if(record->movsAccumAtTick > 0){
			//Move current position to missed list
			NBArray_addValue(&record->t.posSuperficieMissed, record->t.posSuperficieAct);
		}
		record->movsAccumAtTick++;
		record->t.posSuperficieAct.x	= posX;
		record->t.posSuperficieAct.y	= posY;
	}
}

void NBGestorTouches::touchMover(const UI64 identificadorEnOS, const SI32 posX, const SI32 posY){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTouches::touchMover")
	NBHILO_MUTEX_ACTIVAR(&_touchesMutex);
	STGTouchPriv* record = NBGestorTouches::privTouchById(identificadorEnOS, false);
	if(record){
		NBGestorTouches::privTouchMovedLocked(record, posX, posY);
	}
	NBASSERT(record != NULL)
	NBHILO_MUTEX_DESACTIVAR(&_touchesMutex);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

void NBGestorTouches::touchFinalizar(const UI64 identificadorEnOS, const SI32 posX, const SI32 posY, const bool cancelado){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTouches::touchFinalizar")
	NBHILO_MUTEX_ACTIVAR(&_touchesMutex);
	STGTouchPriv* record = NBGestorTouches::privTouchById(identificadorEnOS, false);
	if(record){
		record->marcadoComoFinalizado	= true;
		record->debugSegSinActualizar	= 0.0f;
		record->t.posSuperficieAct.x	= posX;
		record->t.posSuperficieAct.y	= posY;
		record->t.cancelado				= cancelado;
		//2016-11-11: BUG, do not discard the touch
		//Comment the code below.
		/*if(record->ticksAcumulados == 0){
			//Considerar como si el touch nunca existio (consumido ni una vez)
			record->registroOcupado		= false;
		}*/
	}
	NBASSERT(record != NULL)
	NBHILO_MUTEX_DESACTIVAR(&_touchesMutex);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

//-----------------------------------------------
//- Hover (métodos del consumidor de eventos)
//-----------------------------------------------
STGHoverPriv* NBGestorTouches::hoverArreglo(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTouches::hoverArreglo")
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return _hovers;
}

UI16 NBGestorTouches::hoverArregloTamano(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTouches::hoverArregloTamano")
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return _hoversTamano;
}

//-----------------------------------------------
//- Hover (métodos del productor de eventos)
//-----------------------------------------------

void NBGestorTouches::hoverMover(const UI64 identificadorEnOS, const SI32 posX, const SI32 posY){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTouches::hoverMover")
	STGHoverPriv* record = NBGestorTouches::privHoverById(identificadorEnOS);
	if(record){
		record->marcadoComoMovido	= true;
		record->posSuperficieAct.x	= posX;
		record->posSuperficieAct.y	= posY;
	} else {
		record = NBGestorTouches::privHoverFreeRecord();
		if(record){
			record->registroOcupado			= true;
			record->marcadoComoMovido		= true;
			record->identificadorEnOS		= identificadorEnOS;
			record->consumidor				= NULL;
			record->consumidorEscObjeto		= NULL;
			record->consumidorEscInterfaz	= NULL;
			record->posSuperficieAct.x		= posX;
			record->posSuperficieAct.y		= posY;
		}
	}
	NBASSERT(record != NULL)
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

//-----------------------------------------------
//- Metodos de depuración
//-----------------------------------------------

void NBGestorTouches::debugTick(const float segundosTranscurridos){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTouches::debugTick")
	NBHILO_MUTEX_ACTIVAR(&_touchesMutex);
	SI32 iReg;
	for(iReg=0; iReg<_touchesTamano; iReg++){
		STGTouchPriv* datosTouch = &_touches[iReg];
		if(datosTouch->registroOcupado){
			AU_OBJETO_ASSERT_IS_VALID(datosTouch->consumidor)
			AU_OBJETO_ASSERT_IS_VALID(datosTouch->consumidorEscObjeto)
			const BOOL wasBelow = (datosTouch->debugSegSinActualizar < NBGESTORTOUCHES_SEGUNDOS_MAXIMO_PERMITIR_TOUCH_INACTIVO ? TRUE : FALSE);
			datosTouch->debugSegSinActualizar += segundosTranscurridos;
			//NBASSERT(datosTouch->debugSegSinActualizar<NBGESTORTOUCHES_SEGUNDOS_MAXIMO_PERMITIR_TOUCH_INACTIVO);
			if(wasBelow && datosTouch->debugSegSinActualizar >= NBGESTORTOUCHES_SEGUNDOS_MAXIMO_PERMITIR_TOUCH_INACTIVO){
				PRINTF_WARNING("TOUCH, el touch id %llu ha sobrepasado el limite de %f segundos sin actualizacion\n", datosTouch->identificadorEnOS, NBGESTORTOUCHES_SEGUNDOS_MAXIMO_PERMITIR_TOUCH_INACTIVO);
				//NBGestorTouches::finalizarTouch(datosTouch->identificadorEnOS, datosTouch->t.posicionActual.x, datosTouch->t.posicionActual.y, false);
			}
		}
	}
	NBHILO_MUTEX_DESACTIVAR(&_touchesMutex);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}







