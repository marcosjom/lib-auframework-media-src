//
//  AURenderBuffer.cpp
//  AUFramework
//
//  Created by Nicaragua Binary on 15/04/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkMediaStdAfx.h"
#include "AURenderBuffer.h"

AURenderBuffer::AURenderBuffer(UI32 idRenderBufferGl, FLOAT anchoDestinoGl, FLOAT altoDestinoGl, FLOAT escalaParaHD){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AURenderBuffer::AURenderBuffer")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AURenderBuffer");
	_idRenderBufferGl		= idRenderBufferGl;
	_tamanoDestinoGl.ancho	= anchoDestinoGl;
	_tamanoDestinoGl.alto	= altoDestinoGl;
	_escalaParaHD			= escalaParaHD;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AURenderBuffer::~AURenderBuffer(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AURenderBuffer::~AURenderBuffer")
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

//

void AURenderBuffer::relinkearRenderBuffer(const UI32 idRenderBufferGl){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AURenderBuffer::relinkearRenderBuffer")
	_idRenderBufferGl = idRenderBufferGl;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

//

UI32 AURenderBuffer::idRenderBufferGl(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AURenderBuffer::idRenderBufferGl")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _idRenderBufferGl;
}

NBTamano AURenderBuffer::tamanoDestinoGl(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AURenderBuffer::tamanoDestinoGl")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _tamanoDestinoGl;
}

FLOAT AURenderBuffer::escalaParaHD(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AURenderBuffer::escalaParaHD")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _escalaParaHD;
}

//
AUOBJMETODOS_CLASESID_UNICLASE(AURenderBuffer)
AUOBJMETODOS_CLASESNOMBRES_UNICLASE(AURenderBuffer, "AURenderBuffer")
AUOBJMETODOS_CLONAR_NULL(AURenderBuffer)




