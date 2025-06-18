//
//  NBGestorFisica.cpp
//  Gameplay_Mac
//
//  Created by Nicaragua Binary on 07/06/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkMediaStdAfx.h"
#include "NBGestorFisica.h"

#ifndef CONFIG_NB_UNSUPPORT_BOX2D

bool NBGestorFisica::_gestorInicializado	= false;

UI8 NBGestorFisica::_iteracionesVelocidad	= 0;
UI8 NBGestorFisica::_iteracionesPosicion	= 0;

STMundoFisica NBGestorFisica::_mundosFisica[NBGESTORFISICA_MAX_MUNDOS];
AUArregloNativoMutableP<STUnionAnimFisica>* NBGestorFisica::_uniones	= NULL;

SI32 NBGestorFisica::_iMuestraDesvioGravedadMasVieja	= 0;
float NBGestorFisica::_radiantesDesvioGravedad			= 0.0f;
float NBGestorFisica::_sumaMuestrasDesvioGravedad		= 0.0f;
float NBGestorFisica::_muestrasDesvioGravedad[NBGESTORFISICA_ACELEROMETRO_MAXACUM_PARA_PROMEDIAR];

SI32				NBGestorFisica::_debug_conteoUnionesAgregadas	= 0;
SI32				NBGestorFisica::_debug_conteoUnionesQuitadas	 =0;

void NBGestorFisica::inicializar(UI8 iteracionesVelocidad, UI8 iteracionesPosicion){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorFisica::inicializar")
	_gestorInicializado		= false;
	_iteracionesVelocidad	= iteracionesVelocidad;
	_iteracionesPosicion	=  iteracionesPosicion;
	//
	SI32 iMundo;
	for(iMundo=0; iMundo<NBGESTORFISICA_MAX_MUNDOS; iMundo++){
		_mundosFisica[iMundo].registroUsado			= false;
		_mundosFisica[iMundo].mundoActivo			= false;
		_mundosFisica[iMundo].mundoFisica			= NULL;
		_mundosFisica[iMundo].animadoresAdicionales	= new(ENMemoriaTipo_Nucleo) AUArregloNativoMutableP<IAnimadorFisicaAdicional*>(); NB_DEFINE_NOMBRE_PUNTERO(_mundosFisica[iMundo].animadoresAdicionales, "NBGestorFisica::animadoresAdicionales");
	}
	_uniones						= new(ENMemoriaTipo_Nucleo) AUArregloNativoMutableP<STUnionAnimFisica>(); NB_DEFINE_NOMBRE_PUNTERO(_uniones, "NBGestorFisica::_uniones")
	//
	SI32 iMuestra;
	for(iMuestra=0; iMuestra<NBGESTORFISICA_ACELEROMETRO_MAXACUM_PARA_PROMEDIAR; iMuestra++){
		_muestrasDesvioGravedad[iMuestra] = 0.0f;
	}
	_iMuestraDesvioGravedadMasVieja	= 0;
	_sumaMuestrasDesvioGravedad		= 0.0f;
	_radiantesDesvioGravedad		= 0.0f;
	//
	_gestorInicializado				= true;
	//
	_debug_conteoUnionesAgregadas	= 0;
	_debug_conteoUnionesQuitadas	= 0;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

void NBGestorFisica::finalizar(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorFisica::finalizar")
	NBASSERT(_gestorInicializado)
	if(_uniones != NULL){
		//NBASSERT(_uniones->conteo == 0) //Si falla aun hay animaciones no quitadas por sus propietarios
		SI32 iAnim;
		for(iAnim=_uniones->conteo-1; iAnim>=0; iAnim--){
			_uniones->elemento[iAnim].objetoEscena->liberar(NB_RETENEDOR_NULL);
		}
		_uniones->liberar(NB_RETENEDOR_NULL);
		_uniones = NULL;
	}
	SI32 iMundo;
	for(iMundo=0; iMundo<NBGESTORFISICA_MAX_MUNDOS; iMundo++){
		_mundosFisica[iMundo].animadoresAdicionales->liberar(NB_RETENEDOR_NULL);
		if(_mundosFisica[iMundo].registroUsado){
			NBASSERT(_mundosFisica[iMundo].mundoFisica != NULL)
			delete _mundosFisica[iMundo].mundoFisica;
			_mundosFisica[iMundo].registroUsado			= false;
			_mundosFisica[iMundo].mundoActivo			= false;
			_mundosFisica[iMundo].mundoFisica			= NULL;
			_mundosFisica[iMundo].animadoresAdicionales	= NULL;
		}
	}
	_gestorInicializado = false;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

bool NBGestorFisica::gestorInicializado(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorFisica::gestorInicializado")
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return _gestorInicializado;
}

//Mundos

SI32 NBGestorFisica::crearMundoFisica(float gravedadX, float gravedadY, IEscuchadorMundoFisica* escuchador){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorFisica::crearMundoFisica")
	SI32 iMundoLibre = -1;
	SI32 iMundo;
	for(iMundo=1; iMundo<NBGESTORFISICA_MAX_MUNDOS && iMundoLibre==-1; iMundo++){
		if(!_mundosFisica[iMundo].registroUsado){
			NBASSERT(_mundosFisica[iMundo].mundoFisica == NULL)
			NBASSERT(_mundosFisica[iMundo].animadoresAdicionales->conteo == 0)
			iMundoLibre = iMundo;
		}
	}
	if(iMundoLibre!=-1){
		b2Vec2 vGravedad; NBPUNTO_ESTABLECER(vGravedad, gravedadX, gravedadY)
		NBPUNTO_ESTABLECER(_mundosFisica[iMundoLibre].vGravedadFisica, gravedadX, gravedadY)
		NBPUNTO_ESTABLECER(_mundosFisica[iMundoLibre].vVientoFisica, 0.0f, 0.0f)
		NBPUNTO_ESTABLECER(_mundosFisica[iMundoLibre].vGravedadEscena, gravedadX * ESCALA_PIXELES_POR_METRO_MUNDO_FISICA, gravedadY * ESCALA_PIXELES_POR_METRO_MUNDO_FISICA)
		NBPUNTO_ESTABLECER(_mundosFisica[iMundoLibre].vVientoEscena, 0.0f, 0.0f)
		_mundosFisica[iMundoLibre].registroUsado	= true;
		_mundosFisica[iMundoLibre].mundoActivo		= true;
		_mundosFisica[iMundoLibre].mundoFisica		= new b2World(vGravedad); NB_DEFINE_NOMBRE_PUNTERO(_mundosFisica[iMundoLibre].mundoFisica, "NBGestorFisica::mundoFisica");
		_mundosFisica[iMundoLibre].escuchadorMundo	= escuchador;
		_mundosFisica[iMundoLibre].animadoresAdicionales->vaciar();
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return iMundoLibre;
}

void NBGestorFisica::destruirMundoFisica(SI32 iMundo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorFisica::destruirMundoFisica")
	NBASSERT(iMundo>=0 && iMundo<NBGESTORFISICA_MAX_MUNDOS);
	NBASSERT(_mundosFisica[iMundo].mundoFisica != NULL);
	delete _mundosFisica[iMundo].mundoFisica;
	_mundosFisica[iMundo].registroUsado = false;
	_mundosFisica[iMundo].mundoActivo	= false;
	_mundosFisica[iMundo].mundoFisica	= NULL;
	_mundosFisica[iMundo].animadoresAdicionales->vaciar();
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

bool NBGestorFisica::mundoFisicaActivo(SI32 iMundo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorFisica::mundoFisicaActivo")
	NBASSERT(iMundo>=0 && iMundo<NBGESTORFISICA_MAX_MUNDOS);
	NBASSERT(_mundosFisica[iMundo].mundoFisica != NULL);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return _mundosFisica[iMundo].mundoActivo;
}

void NBGestorFisica::establecerMundoActivo(SI32 iMundo, bool mundoActivo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorFisica::establecerMundoActivo")
	NBASSERT(iMundo>=0 && iMundo<NBGESTORFISICA_MAX_MUNDOS);
	NBASSERT(_mundosFisica[iMundo].mundoFisica != NULL);
	_mundosFisica[iMundo].mundoActivo = mundoActivo;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

void NBGestorFisica::establecerEscuchadorDeContactos(SI32 iMundo, b2ContactListener* escuchadorContactos){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorFisica::establecerEscuchadorDeContactos")
	NBASSERT(iMundo>=0 && iMundo<NBGESTORFISICA_MAX_MUNDOS);
	NBASSERT(_mundosFisica[iMundo].mundoFisica != NULL);
	_mundosFisica[iMundo].mundoFisica->SetContactListener(escuchadorContactos);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

//

NBPunto	NBGestorFisica::vectorGravedadFisica(SI32 iMundo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorFisica::vectorGravedadFisica")
	NBASSERT(iMundo>=0 && iMundo<NBGESTORFISICA_MAX_MUNDOS);
	NBASSERT(_mundosFisica[iMundo].mundoFisica != NULL);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return _mundosFisica[iMundo].vGravedadFisica;
}

NBPunto	NBGestorFisica::vectorVientoFisica(SI32 iMundo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorFisica::vectorVientoFisica")
	NBASSERT(iMundo>=0 && iMundo<NBGESTORFISICA_MAX_MUNDOS);
	NBASSERT(_mundosFisica[iMundo].mundoFisica != NULL);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return _mundosFisica[iMundo].vVientoFisica;
}

NBPunto	NBGestorFisica::vectorGravedadEscena(SI32 iMundo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorFisica::vectorGravedadEscena")
	NBASSERT(iMundo>=0 && iMundo<NBGESTORFISICA_MAX_MUNDOS);
	NBASSERT(_mundosFisica[iMundo].mundoFisica != NULL);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return _mundosFisica[iMundo].vGravedadEscena;
}

NBPunto	NBGestorFisica::vectorVientoEscena(SI32 iMundo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorFisica::vectorVientoEscena")
	NBASSERT(iMundo>=0 && iMundo<NBGESTORFISICA_MAX_MUNDOS);
	NBASSERT(_mundosFisica[iMundo].mundoFisica != NULL);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return _mundosFisica[iMundo].vVientoEscena;
}

void NBGestorFisica::establecerVectorGravedad(SI32 iMundo, float gravXMundo, float gravYMundo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorFisica::establecerVectorGravedad")
	NBASSERT(iMundo>=0 && iMundo<NBGESTORFISICA_MAX_MUNDOS);
	NBASSERT(_mundosFisica[iMundo].mundoFisica != NULL);
	b2Vec2 vGravedad; vGravedad.Set(gravXMundo, gravYMundo);
	_mundosFisica[iMundo].mundoFisica->SetGravity(vGravedad);
	NBPUNTO_ESTABLECER(_mundosFisica[iMundo].vGravedadFisica, gravXMundo, gravYMundo);
	NBPUNTO_ESTABLECER(_mundosFisica[iMundo].vGravedadEscena, gravXMundo*ESCALA_PIXELES_POR_METRO_MUNDO_FISICA, gravYMundo*ESCALA_PIXELES_POR_METRO_MUNDO_FISICA);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

void NBGestorFisica::establecerVectorViento(SI32 iMundo, float vientoXMundo, float vientoYMundo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorFisica::establecerVectorViento")
	NBASSERT(iMundo>=0 && iMundo<NBGESTORFISICA_MAX_MUNDOS);
	NBASSERT(_mundosFisica[iMundo].mundoFisica != NULL);
	NBPUNTO_ESTABLECER(_mundosFisica[iMundo].vVientoFisica, vientoXMundo, vientoYMundo);
	NBPUNTO_ESTABLECER(_mundosFisica[iMundo].vVientoEscena, vientoXMundo*ESCALA_PIXELES_POR_METRO_MUNDO_FISICA, vientoYMundo*ESCALA_PIXELES_POR_METRO_MUNDO_FISICA);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

//

void NBGestorFisica::acumularMuestraDesvioGravedad(float radianesDesvioGravedad){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorFisica::acumularMuestraDesvioGravedad")
	NBASSERT(_gestorInicializado)
	_sumaMuestrasDesvioGravedad += (-_muestrasDesvioGravedad[_iMuestraDesvioGravedadMasVieja] + radianesDesvioGravedad);
	_muestrasDesvioGravedad[_iMuestraDesvioGravedadMasVieja] = radianesDesvioGravedad;
	_iMuestraDesvioGravedadMasVieja++; if(_iMuestraDesvioGravedadMasVieja==NBGESTORFISICA_ACELEROMETRO_MAXACUM_PARA_PROMEDIAR) _iMuestraDesvioGravedadMasVieja = 0;
	NBASSERT(_iMuestraDesvioGravedadMasVieja>=0 && _iMuestraDesvioGravedadMasVieja<NBGESTORFISICA_ACELEROMETRO_MAXACUM_PARA_PROMEDIAR)
	_radiantesDesvioGravedad = _sumaMuestrasDesvioGravedad / (float)NBGESTORFISICA_ACELEROMETRO_MAXACUM_PARA_PROMEDIAR;
	//PRINTF_INFO("Grados desvio gravedad prom(%.1f)\n", RADIANES_A_GRADOS(_radiantesDesvioGravedad));
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

float NBGestorFisica::radianesDesvioGravedad(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorFisica::radianesDesvioGravedad")
	NBASSERT(_gestorInicializado)
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return _radiantesDesvioGravedad;
}


//Animadores adicionales

void NBGestorFisica::agregarAnimadorAdicional(SI32 iMundo, IAnimadorFisicaAdicional* animador){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorFisica::agregarAnimadorAdicional")
	NBASSERT(iMundo>=0 && iMundo<NBGESTORFISICA_MAX_MUNDOS);
	NBASSERT(_mundosFisica[iMundo].registroUsado);
	NBASSERT(_mundosFisica[iMundo].mundoFisica != NULL);
	NBASSERT(_mundosFisica[iMundo].animadoresAdicionales->indiceDe(animador)==-1)
	_mundosFisica[iMundo].animadoresAdicionales->agregarElemento(animador);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

void NBGestorFisica::quitarAnimadorAdicional(SI32 iMundo, IAnimadorFisicaAdicional* animador){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorFisica::quitarAnimadorAdicional")
	NBASSERT(iMundo>=0 && iMundo<NBGESTORFISICA_MAX_MUNDOS);
	NBASSERT(_mundosFisica[iMundo].registroUsado);
	NBASSERT(_mundosFisica[iMundo].mundoFisica != NULL);
	NBASSERT(_mundosFisica[iMundo].animadoresAdicionales != NULL)
	SI32 indiceAnimador = _mundosFisica[iMundo].animadoresAdicionales->indiceDe(animador);
	NBASSERT(indiceAnimador>=0);
	_mundosFisica[iMundo].animadoresAdicionales->quitarElementoEnIndice(indiceAnimador);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

//Consultas

void NBGestorFisica::interseccionesConLinea(SI32 iMundo, b2RayCastCallback *callback, float xIni, float yIni, float xFin, float yFin){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorFisica::interseccionesConLinea")
	NBASSERT(iMundo>=0 && iMundo<NBGESTORFISICA_MAX_MUNDOS);
	NBASSERT(_mundosFisica[iMundo].mundoFisica != NULL);
	b2Vec2 iniRayo; NBPUNTO_ESTABLECER(iniRayo, xIni, yIni)
	b2Vec2 finRayo; NBPUNTO_ESTABLECER(finRayo, xFin, yFin)
	_mundosFisica[iMundo].mundoFisica->RayCast(callback, iniRayo, finRayo);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

void NBGestorFisica::interseccionesConCajaAABB(SI32 iMundo, b2QueryCallback *callback, const NBCajaAABB &cajaBusqueda){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorFisica::interseccionesConLinea")
	NBASSERT(iMundo>=0 && iMundo<NBGESTORFISICA_MAX_MUNDOS);
	NBASSERT(_mundosFisica[iMundo].mundoFisica != NULL);
	b2AABB caja;
	caja.lowerBound.Set(cajaBusqueda.xMin, cajaBusqueda.yMin);
	caja.upperBound.Set(cajaBusqueda.xMax, cajaBusqueda.yMax);
	_mundosFisica[iMundo].mundoFisica->QueryAABB(callback, caja);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

// Cuerpos

b2Body* NBGestorFisica::crearCuerpo(SI32 iMundo, const b2BodyDef* defCuerpo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorFisica::crearCuerpo")
	NBASSERT(iMundo>=0 && iMundo<NBGESTORFISICA_MAX_MUNDOS);
	NBASSERT(_mundosFisica[iMundo].mundoFisica != NULL);
	b2Body* nuevoCuerpo = _mundosFisica[iMundo].mundoFisica->CreateBody(defCuerpo);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return nuevoCuerpo;
}

void NBGestorFisica::destruirCuerpo(SI32 iMundo, b2Body* cuerpo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorFisica::destruirCuerpo")
	NBASSERT(iMundo>=0 && iMundo<NBGESTORFISICA_MAX_MUNDOS);
	NBASSERT(_mundosFisica[iMundo].mundoFisica != NULL);
	NBASSERT(cuerpo != NULL);
	if(_mundosFisica[iMundo].escuchadorMundo != NULL){
		b2JointEdge* elemUnion = cuerpo->GetJointList();
		while(elemUnion){
			_mundosFisica[iMundo].escuchadorMundo->fisicaUnionDestruyendo(elemUnion->joint);
			elemUnion = elemUnion->next;
		}
	}
	_mundosFisica[iMundo].mundoFisica->DestroyBody(cuerpo);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

b2Joint* NBGestorFisica::crearUnionCuerpos(SI32 iMundo, const b2JointDef* defUnionCuerpos){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorFisica::crearUnionCuerpos")
	NBASSERT(iMundo>=0 && iMundo<NBGESTORFISICA_MAX_MUNDOS);
	NBASSERT(_mundosFisica[iMundo].mundoFisica != NULL);
	b2Joint* nuevaUnion = _mundosFisica[iMundo].mundoFisica->CreateJoint(defUnionCuerpos);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return nuevaUnion;
}

void NBGestorFisica::destruirUnionCuerpos(SI32 iMundo, b2Joint* unionCuerpos){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorFisica::destruirUnionCuerpos")
	NBASSERT(iMundo>=0 && iMundo<NBGESTORFISICA_MAX_MUNDOS);
	NBASSERT(_mundosFisica[iMundo].mundoFisica != NULL);
	NBASSERT(unionCuerpos != NULL);
	if(_mundosFisica[iMundo].escuchadorMundo != NULL) _mundosFisica[iMundo].escuchadorMundo->fisicaUnionDestruyendo(unionCuerpos);
	_mundosFisica[iMundo].mundoFisica->DestroyJoint(unionCuerpos);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

//Uniones 

void NBGestorFisica::agregarUnion(AUEscenaObjeto* objetoEscena, b2Body* cuerpoBox2D, ENAnimFisicaTipo tipoUnion){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorFisica::agregarUnion")
	NBASSERT(objetoEscena != NULL);
	NBASSERT(cuerpoBox2D != NULL);
	STUnionAnimFisica datosUnion;
	datosUnion.tipoUnion	= tipoUnion;
	datosUnion.objetoEscena	= objetoEscena; objetoEscena->retener(NB_RETENEDOR_NULL);
	datosUnion.cuerpo		= cuerpoBox2D;
	_uniones->agregarElemento(datosUnion);
	_debug_conteoUnionesAgregadas++;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

void NBGestorFisica::actualizarUnion(AUEscenaObjeto* objetoEscena, b2Body* cuerpoBox2D, ENAnimFisicaTipo tipoUnion){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorFisica::actualizarUnion")
	SI32 i;
	for(i=_uniones->conteo-1; i>=0; i--){
		if(_uniones->elemento[i].objetoEscena==objetoEscena && _uniones->elemento[i].cuerpo==cuerpoBox2D){
			_uniones->elemento[i].tipoUnion = tipoUnion;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

void NBGestorFisica::quitarUnion(AUEscenaObjeto* objetoEscena, b2Body* cuerpoBox2D){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorFisica::quitarUnion")
	SI32 i;
	for(i=_uniones->conteo-1; i>=0; i--){
		if(_uniones->elemento[i].objetoEscena==objetoEscena && _uniones->elemento[i].cuerpo==cuerpoBox2D){
			_uniones->elemento[i].objetoEscena->liberar(NB_RETENEDOR_NULL);
			_uniones->quitarElementoEnIndice(i);
			_debug_conteoUnionesQuitadas++;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

void NBGestorFisica::relinkearUnionesDeCuerpo(SI32 iMundo, b2Body* cuerpoConUniones, b2Body* cuerpoDestino){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorFisica::relinkearUnionesDeCuerpo")
	NBASSERT(iMundo>=0 && iMundo<NBGESTORFISICA_MAX_MUNDOS);
	NBASSERT(_mundosFisica[iMundo].mundoFisica != NULL);
	//Restablecer las uniones a las que pertenecia el cuerpo anterior
	b2JointEdge* elemUnion;
	//Contabilizar uniones a copiar
	UI16 i, conteoUniones = 0;
	elemUnion = cuerpoConUniones->GetJointList();
	while(elemUnion){ elemUnion = elemUnion->next; conteoUniones++; }
	if(conteoUniones!=0){
		//Respaldar arreglo de uniones
		b2Joint** uniones		= (b2Joint**) NBGestorMemoria::reservarMemoria(sizeof(b2Joint*)*conteoUniones, ENMemoriaTipo_Temporal);
		#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
		bool* debugEncontradas	= (bool*) NBGestorMemoria::reservarMemoria(sizeof(bool)*conteoUniones, ENMemoriaTipo_Temporal);
		#endif
		i = 0; elemUnion		= cuerpoConUniones->GetJointList();
		while(elemUnion){
			#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
			debugEncontradas[i] = false;
			#endif
			uniones[i]			= elemUnion->joint;
			i++; elemUnion		= elemUnion->next;
		}
		NBASSERT(i==conteoUniones)
		NBASSERT(cuerpoDestino->GetContactList() == NULL) //Temporal (verificar que el cuerpo comience sin uniones)
		//Copiar uniones a nuevo destino
		for(i=0; i<conteoUniones; i++){
			b2Joint* objUnion	= uniones[i];
			NBASSERT(objUnion->GetBodyA()==cuerpoConUniones || objUnion->GetBodyB()==cuerpoConUniones)
			NBASSERT(objUnion->GetBodyA()!=objUnion->GetBodyB())
			if(objUnion->GetBodyA()==cuerpoConUniones) _mundosFisica[iMundo].mundoFisica->RelinkearUnionCuerpoA(objUnion, cuerpoDestino);
			if(objUnion->GetBodyB()==cuerpoConUniones) _mundosFisica[iMundo].mundoFisica->RelinkearUnionCuerpoB(objUnion, cuerpoDestino);
			NBASSERT(objUnion->GetBodyA()==cuerpoDestino || objUnion->GetBodyB()==cuerpoDestino)
			NBASSERT(objUnion->GetBodyA()!=objUnion->GetBodyB())
			
		}
		//Verificacion
		#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
		UI16 conteoUnionesFinales = 0;
		elemUnion = cuerpoDestino->GetJointList();
		while(elemUnion){
			SI32 iBsq;
			for(iBsq=0; iBsq<conteoUniones; iBsq++){
				if(elemUnion->joint==uniones[iBsq]){
					NBASSERT(debugEncontradas[iBsq]==false) //Aparece mas de una vez
					debugEncontradas[iBsq] = true;
					break;
				}
			}
			conteoUnionesFinales++;
			elemUnion = elemUnion->next;
		}
		NBASSERT(conteoUnionesFinales==conteoUniones)
		for(i=0; i<conteoUnionesFinales; i++){
			NBASSERT(debugEncontradas[i]) //Si falla, alguna union no fue copiada
		}
		#endif
		NBGestorMemoria::liberarMemoria(uniones);
		#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
		NBGestorMemoria::liberarMemoria(debugEncontradas);
		#endif
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

/*void NBGestorFisica::relinkearUnionCuerpoA(SI32 iMundo, b2Joint* unionFisica, b2Body* cuerpoNuevo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorFisica::relinkearUnionCuerpoA")
	NBASSERT(iMundo>=0 && iMundo<NBGESTORFISICA_MAX_MUNDOS);
	NBASSERT(_mundosFisica[iMundo].mundoFisica != NULL);
	_mundosFisica[iMundo].mundoFisica->RelinkearUnionCuerpoA(unionFisica, cuerpoNuevo);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}*/

/*void NBGestorFisica::relinkearUnionCuerpoB(SI32 iMundo, b2Joint* unionFisica, b2Body* cuerpoNuevo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorFisica::relinkearUnionCuerpoB")
	NBASSERT(iMundo>=0 && iMundo<NBGESTORFISICA_MAX_MUNDOS);
	NBASSERT(_mundosFisica[iMundo].mundoFisica != NULL);
	_mundosFisica[iMundo].mundoFisica->RelinkearUnionCuerpoB(unionFisica, cuerpoNuevo);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}*/

void NBGestorFisica::tickAnimacionesAdicionalesMundos(float segsTranscurridos){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorFisica::tickAnimacionesAdicionalesMundos")
	SI32 iMundo;
	for(iMundo=0; iMundo<NBGESTORFISICA_MAX_MUNDOS; iMundo++){
		if(_mundosFisica[iMundo].registroUsado && _mundosFisica[iMundo].mundoActivo){
			UI32 iAnim, iAnimConteo = _mundosFisica[iMundo].animadoresAdicionales->conteo;
			for(iAnim=0; iAnim<iAnimConteo; iAnim++){
				_mundosFisica[iMundo].animadoresAdicionales->elemento[iAnim]->simularFisicaCuerpos(segsTranscurridos);
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

void NBGestorFisica::tickAnimacionFisicaMundos(float segsTranscurridos){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorFisica::tickAnimacionFisicaMundos")
	UI32 iMundo;
	for(iMundo=0; iMundo<NBGESTORFISICA_MAX_MUNDOS; iMundo++){
		if(_mundosFisica[iMundo].registroUsado && _mundosFisica[iMundo].mundoActivo){
			_mundosFisica[iMundo].mundoFisica->Step(segsTranscurridos, _iteracionesVelocidad, _iteracionesPosicion);
			_mundosFisica[iMundo].mundoFisica->ClearForces();
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

void NBGestorFisica::ejecutarUnionesCuerposConObjetosEscena(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorFisica::ejecutarUnionesCuerposConObjetosEscena")
	UI32 i, iConteo = _uniones->conteo;
	for(i=0; i<iConteo; i++){
		if(_uniones->elemento[i].objetoEscena->idEscena<0) continue; //Evitar procesar cuerpos con matrices incompletas
		NBASSERT(_uniones->elemento[i].objetoEscena->idEscena!=-1) //El objeto unido debe estar en escena
		//
		ENAnimFisicaTipo tipoUnion		= _uniones->elemento[i].tipoUnion;
		AUEscenaObjeto* objetoEscena	= _uniones->elemento[i].objetoEscena;
		b2Body* cuerpoBox2D				= _uniones->elemento[i].cuerpo;
		NBASSERT(objetoEscena != NULL);
		NBASSERT(cuerpoBox2D != NULL);
		if(tipoUnion==ENAnimFisicaTipo_SeguirCuerpo){
			AUEscenaObjeto* contenedor = objetoEscena->contenedor();
			if(contenedor != NULL){
				b2Vec2 posCuerpo = cuerpoBox2D->GetPosition();
				float radianesCuerpo = cuerpoBox2D->GetAngle();
				NBPunto vectApunta; NBPUNTO_OBTENER_VECTOR_ROTADO(vectApunta, 1.0f, radianesCuerpo);
				NBPunto posEscena; NBPUNTO_ESTABLECER(posEscena, posCuerpo.x * ESCALA_PIXELES_POR_METRO_MUNDO_FISICA, posCuerpo.y * ESCALA_PIXELES_POR_METRO_MUNDO_FISICA);
				NBPunto posEscenaApunta; NBPUNTO_ESTABLECER(posEscenaApunta, posEscena.x+vectApunta.x, posEscena.y+vectApunta.y);
				//
				NBMatriz matrizInversa = contenedor->matrizEscenaInversaCalculada();
				NBPunto posLocalCont;	NBMATRIZ_MULTIPLICAR_PUNTO(posLocalCont, matrizInversa, posEscena.x, posEscena.y);
				NBPunto posLocalContApuntar;	NBMATRIZ_MULTIPLICAR_PUNTO(posLocalContApuntar, matrizInversa, posEscenaApunta.x, posEscenaApunta.y);
				objetoEscena->establecerTraslacion(posLocalCont.x, posLocalCont.y);
				float radianesObjeto;	NBPUNTO_RADIANES_VECTOR(radianesObjeto, posLocalCont.x, posLocalCont.y, posLocalContApuntar.x, posLocalContApuntar.y);
				objetoEscena->establecerRotacion(RADIANES_A_GRADOS(radianesObjeto));
			}
		} else if(tipoUnion==ENAnimFisicaTipo_SeguirObjetoEscena){
			float cuadrosPorSegundo	= NBGestorAnimadores::ticksPorSegundo();
			b2BodyType tipoCpo		= cuerpoBox2D->GetType();
			b2Vec2 posCpo			= cuerpoBox2D->GetPosition();
			float radianesCpo		= cuerpoBox2D->GetAngle();
			NBMatriz matrizEscenaInv = objetoEscena->matrizEscenaInversaCalculada();
			NBMatriz matrizEscena;	NBMATRIZ_INVERSA(matrizEscena, matrizEscenaInv);
			NBPunto posEscena;		NBMATRIZ_MULTIPLICAR_PUNTO(posEscena, matrizEscena, 0.0f, 0.0f);
			NBPunto posEscenaDer;	NBMATRIZ_MULTIPLICAR_PUNTO(posEscenaDer, matrizEscena, 1.0f, 0.0f);
			b2Vec2 posDestino;
			posDestino.x			= (posEscena.x / ESCALA_PIXELES_POR_METRO_MUNDO_FISICA);
			posDestino.y			= (posEscena.y / ESCALA_PIXELES_POR_METRO_MUNDO_FISICA);
			float radDestino;		NBPUNTO_RADIANES_VECTOR(radDestino, posEscena.x, posEscena.y, posEscenaDer.x, posEscenaDer.y) //Importante siempre tener en cuenta que este valor estara siempre entre 0-360 (o su equivalente en radiantes)
			if(radianesCpo<0.0f)	radDestino -= PIx2; //conviene procesar con angulos negativos
			if(tipoCpo == b2_staticBody){
				if(posCpo.x != posDestino.x || posCpo.y != posDestino.y || radianesCpo != radDestino){
					cuerpoBox2D->SetTransform(posDestino, radDestino);
				}
			} else { //b2_kinematicBody, b2_dynamicBody
				//VALIDACION DE ROTACION DE CUERPO
				//Evita rotaciones bruscas de los cuerpos, producto del retorno del angulo al rango (0, 360).
				SI32 vueltasCuerpo		= radianesCpo / PIx2; //vueltas completas que ya ha dado el cuerpo
				radDestino		+= ((float)vueltasCuerpo * PIx2); //convertir el angulo del objeto escena a angulo relativo a la vueltas que ha dado el cuerpo
				float diffRadiantesVueltaSiguiente = ((radDestino+PIx2)-radianesCpo);
				float diffRadiantesVueltaAnterior = ((radDestino-PIx2)-radianesCpo);
				if(diffRadiantesVueltaSiguiente>=-PI && diffRadiantesVueltaSiguiente<=PI){
					radDestino	+= PIx2; //Conviene pasar a la vuelta siguiente
				} else if(diffRadiantesVueltaAnterior>=-PI && diffRadiantesVueltaAnterior<=PI){
					radDestino	-= PIx2; //Conviene pasar a la vuelta anterior
				}
				//
				b2Vec2 diffPosicion;
				diffPosicion.x		= (posDestino.x - posCpo.x) * cuadrosPorSegundo;
				diffPosicion.y		= (posDestino.y - posCpo.y) * cuadrosPorSegundo;
				float diffRadianes	= (radDestino-radianesCpo); NBASSERT(diffRadianes>=-PI && diffRadianes<=PI)
				cuerpoBox2D->SetLinearVelocity(diffPosicion);
				cuerpoBox2D->SetAngularVelocity(diffRadianes*cuadrosPorSegundo);
				//PRINTF_INFO("Animando hacia posicion Escena velMov(%f, %f) velRot(%f)\n", diffPosicion.x, diffPosicion.y, diffAngulos*cuadrosPorSegundo);
				#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
				if(NBPUNTO_DISTANCIA_VECTOR(diffPosicion.x, diffPosicion.y)>20.0f){
					AUEscenaObjeto* objCuerpo	= _uniones->elemento[i].objetoEscena;
					AUEscenaObjeto* objPadre	= objCuerpo->contenedor();
					if(objPadre == NULL){
						PRINTF_WARNING("%d: cuerpo '%s' muy alejado (%.1f metros) [MOV BRUSCO]\n", i, objCuerpo->nombreUltimaClase(), NBPUNTO_DISTANCIA_VECTOR(diffPosicion.x, diffPosicion.y));
					} else {
						//PRINTF_WARNING("%d: cuerpo '%s / %s' en '%s' muy alejado (%.1f metros) [MOV BRUSCO]\n", i, objCuerpo->nombreUltimaClase(), objPadre->dbgNombreVariableObjeto(objCuerpo), objPadre->nombreUltimaClase(), NBPUNTO_DISTANCIA_VECTOR(diffPosicion.x, diffPosicion.y));
					}
				}
				#endif
			}
		} else {
			NBASSERT(false);
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}


#endif //#ifndef CONFIG_NB_UNSUPPORT_BOX2D
