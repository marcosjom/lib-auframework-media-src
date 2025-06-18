//
//  AUEscenaAnimacion.cpp
//  AUFramework
//
//  Created by Marcos Ortega Morales on 11/12/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkMediaStdAfx.h"
#include "AUEscenaAnimacion.h"


AUEscenaAnimacion::AUEscenaAnimacion(AUAnimacionBibliotecaDef* plantillaBiblioteca, AUAnimacionDef* plantillaAnimacion) : AUEscenaAnimacionI(plantillaBiblioteca, plantillaAnimacion) {
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaAnimacion")
	//Gestion de sombras
	this->fijarAgrupadorDeSombras();
	this->establecerPreagruparSombrasParaHijos(true);
}

AUEscenaAnimacion::AUEscenaAnimacion(AUEscenaAnimacion* otraInstancia) : AUEscenaAnimacionI(otraInstancia) {
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaAnimacion")
	//Gestion de sombras
	this->fijarAgrupadorDeSombras();
	this->establecerPreagruparSombrasParaHijos(true);
	//Inicializar hijos heredados
	privInicializarHijosHeredados(this->hijos(), otraInstancia->hijos(), otraInstancia->_plantillaBiblioteca->tipoEncapsulamientoHijos);
}

AUEscenaAnimacion::~AUEscenaAnimacion(){
	//
}

//

void AUEscenaAnimacion::privInicializarHijosHeredados(AUArreglo* hijosCapaLocal, AUArreglo* hijosCapaOriginal, const ENAnimacion_TipoEncapsulamiento tipoEncapsulamiento){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaAnimacion::privInicializarHijosHeredados")
	//Actualizar hijos
	UI16 iHijo, conteoHijosOrig		= hijosCapaOriginal->conteo;
	for(iHijo=0; iHijo<conteoHijosOrig; iHijo++){
		AUEscenaObjeto* hijoLocal	= (AUEscenaObjeto*)hijosCapaLocal->elemento[iHijo];
		AUEscenaObjeto* hijoOrig	= (AUEscenaObjeto*)hijosCapaOriginal->elemento[iHijo];
		//TEMPORAL: ignorando casos 'ENAnimacion_TipoEncapsulamiento_UnContenedorPorHijo'
		/*if(tipoEncapsulamiento==ENAnimacion_TipoEncapsulamiento_UnContenedorPorHijo){
			NBASSERT(hijoLocal->esClase(AUEscenaContenedor::idTipoClase))
			NBASSERT(hijoOrig->esClase(AUEscenaContenedor::idTipoClase))
			NBASSERT(((AUEscenaContenedor*)hijoLocal)->hijos()->conteo!=0)
			NBASSERT(((AUEscenaContenedor*)hijoOrig)->hijos()->conteo!=0)
			hijoLocal				= (AUEscenaObjeto*)((AUEscenaContenedor*)hijoLocal)->hijos()->elemento[0];
			hijoOrig				= (AUEscenaObjeto*)((AUEscenaContenedor*)hijoOrig)->hijos()->elemento[0];
		}*/
		NBASSERT(hijoLocal->esClase(hijoOrig->idUltimaClase()))
		if(hijoLocal->esClase(AUNodoElectroCarga::idTipoClase)){
			NBASSERT(hijoOrig->esClase(AUNodoElectroCarga::idTipoClase))
			((AUNodoElectroCarga*)hijoLocal)->agregarDestinosElectroCargarSegunPlantilla((AUNodoElectroCarga*)hijoOrig, hijosCapaOriginal, hijosCapaLocal);
		}
		//En las animaciones no estan implementados sensores (por tanto, se ignora actualizar AUBateria, AULaser y AUPanelSolar)
		if(hijoLocal->esClase(AUElectronodo::idTipoClase)){
			NBASSERT(hijoOrig->esClase(AUElectronodo::idTipoClase))
			((AUElectronodo*)hijoLocal)->actualizarDestinosPasosLanzamientosSegunPlantilla((AUElectronodo*)hijoOrig, hijosCapaOriginal, hijosCapaLocal);
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

//

void AUEscenaAnimacion::establecerGrupoAnimacion(ENGestorAnimadoresGrupo grupoAnimacion){
    NBAnimador::establecerGrupoAnimacion(grupoAnimacion);
    //Establecer grupo de hijos
	AUArreglo* hijos = this->hijos();
	SI32 iHijo, cantHijos = hijos->conteo;
	if(_plantillaBiblioteca->tipoEncapsulamientoHijos==ENAnimacion_TipoEncapsulamiento_UnContenedorNormal){
		for(iHijo=0; iHijo<cantHijos; iHijo++){
			AUEscenaObjeto* hijo = (AUEscenaObjeto*)hijos->elemento[iHijo];
			if(hijo->esClase(AUEscenaAnimacionI::idTipoClase)){
				((AUEscenaAnimacionI*)hijo)->establecerGrupoAnimacion(grupoAnimacion);
			} else if(hijo->esClase(AUEscenaCuerpoI::idTipoClase)){
				((AUEscenaCuerpoI*)hijo)->establecerGrupoAnimacion(grupoAnimacion);
			}
		}
	} else if(_plantillaBiblioteca->tipoEncapsulamientoHijos==ENAnimacion_TipoEncapsulamiento_UnContenedorPorHijo){
		for(iHijo=0; iHijo<cantHijos; iHijo++){
			AUEscenaObjeto* hijo = (AUEscenaObjeto*)((AUEscenaContenedor*)hijos->elemento[iHijo])->hijos()->elemento[0];
			if(hijo->esClase(AUEscenaAnimacionI::idTipoClase)){
				((AUEscenaAnimacionI*)hijo)->establecerGrupoAnimacion(grupoAnimacion);
			} else if(hijo->esClase(AUEscenaCuerpoI::idTipoClase)){
				((AUEscenaCuerpoI*)hijo)->establecerGrupoAnimacion(grupoAnimacion);
			}
		}
	} else {
		NBASSERT(false) //Tipo de encapsulamiento no reconocido
	}
}

/*b2Body*/void* AUEscenaAnimacion::cuerpo(){
	AUArreglo* hijos = this->hijos();
	void* cuerpo = NULL;
	SI32 iHijo, cantHijos = hijos->conteo;
	if(_plantillaBiblioteca->tipoEncapsulamientoHijos==ENAnimacion_TipoEncapsulamiento_UnContenedorNormal){
		for(iHijo=0; iHijo<cantHijos && cuerpo == NULL; iHijo++){
			AUEscenaObjeto* hijo = (AUEscenaObjeto*)hijos->elemento[iHijo];
			if(hijo->esClase(AUEscenaAnimacionI::idTipoClase)){
				cuerpo = ((AUEscenaAnimacionI*)hijo)->cuerpo();
			} else if(hijo->esClase(AUEscenaCuerpoI::idTipoClase)){
				cuerpo = ((AUEscenaCuerpoI*)hijo)->cuerpo();
			}
		}
	} else if(_plantillaBiblioteca->tipoEncapsulamientoHijos==ENAnimacion_TipoEncapsulamiento_UnContenedorPorHijo){
		for(iHijo=0; iHijo<cantHijos && cuerpo == NULL; iHijo++){
			AUEscenaObjeto* hijo = (AUEscenaObjeto*)((AUEscenaContenedor*)hijos->elemento[iHijo])->hijos()->elemento[0];
			if(hijo->esClase(AUEscenaAnimacionI::idTipoClase)){
				cuerpo = ((AUEscenaAnimacionI*)hijo)->cuerpo();
			} else if(hijo->esClase(AUEscenaCuerpoI::idTipoClase)){
				cuerpo = ((AUEscenaCuerpoI*)hijo)->cuerpo();
			}
		}
	} else {
		NBASSERT(false) //Tipo de encapsulamiento no reconocido
	}	
	return cuerpo;
}

//
AUOBJMETODOS_CLASESID_MULTICLASE(AUEscenaAnimacion, AUEscenaAnimacionI)
AUOBJMETODOS_CLASESNOMBRES_MULTICLASE(AUEscenaAnimacion, "AUEscenaAnimacion", AUEscenaAnimacionI)
AUOBJMETODOS_CLONAR_THIS(AUEscenaAnimacion)




