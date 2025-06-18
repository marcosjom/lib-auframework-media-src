
#include "AUFrameworkMediaStdAfx.h"


#include "AUDialogo.h"

//-------------------------------------
//--- CONSTRUCTORES Y DESTRUCTORES ----
//-------------------------------------

AUDialogo::AUDialogo() : AUObjeto(), _strCompartida(this), _arregloElementos(this), _arregloTextos(this), _arregloImagenes(this), _arregloAnimaciones(this), _bibliotecaFuentes(this) {
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUDialogo::AUDialogo")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUDialogo")
	_strCompartida.agregar('\0'); //El indice cero sera la cadena vacia
	AU_GESTOR_PILA_LLAMADAS_POP
}

AUDialogo::~AUDialogo(void){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUDialogo::~AUDialogo")
	privVaciar();
	AU_GESTOR_PILA_LLAMADAS_POP
}

void AUDialogo::privVaciar(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUDialogo::privVaciar")
	UI32 iElem, conteoElem = _arregloTextos.conteo;
	for(iElem=0; iElem<conteoElem; iElem++){
		NBASSERT(_arregloTextos.elemento[iElem].texFuente != NULL)
		if(_arregloTextos.elemento[iElem].texFuente != NULL) _arregloTextos.elemento[iElem].texFuente->liberar(NB_RETENEDOR_THIS);
		_arregloTextos.elemento[iElem].texFuente = NULL;
	}
	conteoElem = _arregloImagenes.conteo;
	for(iElem=0; iElem<conteoElem; iElem++){
		NBASSERT(_arregloImagenes.elemento[iElem].objPlantilla != NULL)
		if(_arregloImagenes.elemento[iElem].objPlantilla != NULL) _arregloImagenes.elemento[iElem].objPlantilla->liberar(NB_RETENEDOR_THIS);
		_arregloImagenes.elemento[iElem].objPlantilla = NULL;
	}
	conteoElem = _arregloAnimaciones.conteo;
	for(iElem=0; iElem<conteoElem; iElem++){
		NBASSERT(_arregloAnimaciones.elemento[iElem].objPlantilla != NULL)
		if(_arregloAnimaciones.elemento[iElem].objPlantilla != NULL) _arregloAnimaciones.elemento[iElem].objPlantilla->liberar(NB_RETENEDOR_THIS);
		_arregloAnimaciones.elemento[iElem].objPlantilla = NULL;
	}
	//
	_strCompartida.vaciar(); _strCompartida.agregar('\0'); //El indice cero sera la cadena vacia
	_bibliotecaFuentes.vaciar();
	_arregloElementos.vaciar();
	_arregloTextos.vaciar();
	_arregloImagenes.vaciar();
	_arregloAnimaciones.vaciar();
	AU_GESTOR_PILA_LLAMADAS_POP
}

//-------------------------------
//--- FUNCIONES DE INSTANCIA ----
//-------------------------------

SI32 AUDialogo::conteoElementos(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUDialogo::conteo")
	SI32 r = _arregloElementos.conteo;
	AU_GESTOR_PILA_LLAMADAS_POP
	return r;
}

const char* AUDialogo::cadenaEnIndice(UI32 iPrimerChar){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUDialogo::cadenaEnIndice")
	NBASSERT(iPrimerChar<_strCompartida.tamano());
	AU_GESTOR_PILA_LLAMADAS_POP
	return &(_strCompartida.str()[iPrimerChar]);
}

STDialogoElem AUDialogo::elemento(UI32 indice){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUDialogo::conteo")
	NBASSERT(indice<_arregloElementos.conteo)
	STDialogoElem datosElem = _arregloElementos.elemento[indice];
	AU_GESTOR_PILA_LLAMADAS_POP
	return datosElem;
}

SI32 AUDialogo::indicePrimerElementoConID(const char* strID){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUDialogo::indicePrimerElementoConID")
	NBASSERT(strID != NULL)
	SI32 iElemEncontrado = -1;
	UI32 iElem, iElemConteo = _arregloElementos.conteo;
	for(iElem=0; iElem<iElemConteo; iElem++){
		if(AUCadena8::esIgual(strID, &_strCompartida.str()[_arregloElementos.elemento[iElem].iStrID])){
			iElemEncontrado = iElem;
			break;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP
	return iElemEncontrado;
}

STDialogoTexto AUDialogo::elementoTexto(UI32 indice){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUDialogo::conteo")
	NBASSERT(indice<_arregloTextos.conteo)
	STDialogoTexto datosElem = _arregloTextos.elemento[indice];
	AU_GESTOR_PILA_LLAMADAS_POP
	return datosElem;
}

STDialogoImagen AUDialogo::elementoImagen(UI32 indice){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUDialogo::conteo")
	NBASSERT(indice<_arregloImagenes.conteo)
	STDialogoImagen datosElem = _arregloImagenes.elemento[indice];
	AU_GESTOR_PILA_LLAMADAS_POP
	return datosElem;
}

STDialogoAnimacion AUDialogo::elementoAnimacion(UI32 indice){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUDialogo::conteo")
	NBASSERT(indice<_arregloAnimaciones.conteo)
	STDialogoAnimacion datosElem = _arregloAnimaciones.elemento[indice];
	AU_GESTOR_PILA_LLAMADAS_POP
	return datosElem;
}

//
AUOBJMETODOS_CLASESID_UNICLASE(AUDialogo)
AUOBJMETODOS_CLASESNOMBRES_UNICLASE(AUDialogo, "AUDialogo")
AUOBJMETODOS_CLONAR_NULL(AUDialogo)






