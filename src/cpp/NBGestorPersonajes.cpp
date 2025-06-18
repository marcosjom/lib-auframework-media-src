//
//  NBGestorPersonajes.cpp
//  DodoTown
//
//  Created by Marcos Ortega on 23/08/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkMediaStdAfx.h"
#include "NBGestorPersonajes.h"

bool NBGestorPersonajes::_gestorInicializado	= false;
AUArregloNativoMutableP<STPersonajeVictima>* NBGestorPersonajes::_victimas	= NULL;
AUArregloNativoMutableP<STPersonajeVictimario>* NBGestorPersonajes::_victimarios	= NULL;


bool NBGestorPersonajes::inicializar(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorPersonajes::inicializar")
	_gestorInicializado	= false;
	_victimas			= new(ENMemoriaTipo_General) AUArregloNativoMutableP<STPersonajeVictima>(); NB_DEFINE_NOMBRE_PUNTERO(_victimas, "NBGestorPersonajes::_victimas");
	_victimarios		= new(ENMemoriaTipo_General) AUArregloNativoMutableP<STPersonajeVictimario>(); NB_DEFINE_NOMBRE_PUNTERO(_victimarios, "NBGestorPersonajes::_victimarios");
	_gestorInicializado	= true;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return true;
}

void NBGestorPersonajes::finalizar(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorPersonajes::finalizar")
	if(_victimas != NULL) _victimas->liberar(NB_RETENEDOR_NULL); _victimas = NULL;
	if(_victimarios != NULL) _victimarios->liberar(NB_RETENEDOR_NULL); _victimarios = NULL;
	_gestorInicializado	= false;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

bool NBGestorPersonajes::gestorInicializado(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorPersonajes::gestorInicializado")
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return _gestorInicializado;
}


//Victimas

AUArregloNativoP<STPersonajeVictima>* NBGestorPersonajes::victimas(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorPersonajes::victimas")
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return _victimas;
}

void NBGestorPersonajes::agregarVictima(IPersonajeVictima* objVictima, SI32 datoUsuario){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorPersonajes::agregarVictima")
	#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
	STPersonajeVictima* victimas = _victimas->elemento;
	UI16 iElem, conteoElems = _victimas->conteo;
	for(iElem=0; iElem<conteoElems; iElem++){
		NBASSERT(victimas[iElem].objVictima!=objVictima) //Si falla, se esta repitiendo elemento
	}
	#endif
	STPersonajeVictima datosVictima;
	datosVictima.objVictima		= objVictima;
	datosVictima.datoUsuario	= datoUsuario;
	_victimas->agregarElemento(datosVictima);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

void NBGestorPersonajes::quitarVictima(IPersonajeVictima* objVictima){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorPersonajes::quitarVictima")
	STPersonajeVictima* victimas = _victimas->elemento;
	UI16 iElem, conteoElems = _victimas->conteo;
	for(iElem=0; iElem<conteoElems; iElem++){
		if(victimas[iElem].objVictima==objVictima){
			_victimas->quitarElementoEnIndice(iElem);
			break;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

//Victimarios

AUArregloNativoP<STPersonajeVictimario>* NBGestorPersonajes::victimarios(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorPersonajes::victimarios")
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return _victimarios;
}

void NBGestorPersonajes::agregarVictimario(IPersonajeVictimario* objVictimario, SI32 datoUsuario){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorPersonajes::agregarVictimario")
	#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
	STPersonajeVictimario* victimarios = _victimarios->elemento;
	UI16 iElem, conteoElems = _victimarios->conteo;
	for(iElem=0; iElem<conteoElems; iElem++){
		NBASSERT(victimarios[iElem].objVictimario!=objVictimario)
	}
	#endif
	STPersonajeVictimario datosVictimario;
	datosVictimario.objVictimario	= objVictimario;
	datosVictimario.datoUsuario		= datoUsuario;
	_victimarios->agregarElemento(datosVictimario);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

void NBGestorPersonajes::quitarVictimario(IPersonajeVictimario* objVictimario){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorPersonajes::quitarVictimario")
	STPersonajeVictimario* victimarios = _victimarios->elemento;
	UI16 iElem, conteoElems = _victimarios->conteo;
	for(iElem=0; iElem<conteoElems; iElem++){
		if(victimarios[iElem].objVictimario==objVictimario){
			_victimarios->quitarElementoEnIndice(iElem);
			break;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}



