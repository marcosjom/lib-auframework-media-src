//
//  AUEscenaMalla9.cpp
//  Gameplay_Mac
//
//  Created by Nicaragua Binary on 06/03/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkMediaStdAfx.h"
#include "AUEscenaMalla9.h"

AUEscenaMalla9::AUEscenaMalla9(const char* prefijoRutaTexturas9) : AUEscenaMallaXBase() {
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaMalla9::AUEscenaMalla9")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaMalla9")
	_mallaFondo				= NULL;
	AUCadenaMutable8* rutaPNG = new(ENMemoriaTipo_Temporal) AUCadenaMutable8();
	rutaPNG->establecer(prefijoRutaTexturas9); rutaPNG->agregar("1.png"); _texturasMarco[0]		= NBGestorTexturas::texturaDesdeArchivo(rutaPNG->str()); _texturasMarco[0]->retener(NB_RETENEDOR_THIS);
	rutaPNG->establecer(prefijoRutaTexturas9); rutaPNG->agregar("2.png"); _texturasMarco[1]		= NBGestorTexturas::texturaDesdeArchivo(rutaPNG->str()); _texturasMarco[1]->retener(NB_RETENEDOR_THIS);
	rutaPNG->establecer(prefijoRutaTexturas9); rutaPNG->agregar("3.png"); _texturasMarco[2]		= NBGestorTexturas::texturaDesdeArchivo(rutaPNG->str()); _texturasMarco[2]->retener(NB_RETENEDOR_THIS);
	rutaPNG->establecer(prefijoRutaTexturas9); rutaPNG->agregar("4.png"); _texturasMarco[3]		= NBGestorTexturas::texturaDesdeArchivo(rutaPNG->str()); _texturasMarco[3]->retener(NB_RETENEDOR_THIS);
	rutaPNG->establecer(prefijoRutaTexturas9); rutaPNG->agregar("5.png"); _texturasMarco[4]		= NBGestorTexturas::texturaDesdeArchivo(rutaPNG->str()); _texturasMarco[4]->retener(NB_RETENEDOR_THIS);
	rutaPNG->establecer(prefijoRutaTexturas9); rutaPNG->agregar("6.png"); _texturasMarco[5]		= NBGestorTexturas::texturaDesdeArchivo(rutaPNG->str()); _texturasMarco[5]->retener(NB_RETENEDOR_THIS);
	rutaPNG->establecer(prefijoRutaTexturas9); rutaPNG->agregar("7.png"); _texturasMarco[6]		= NBGestorTexturas::texturaDesdeArchivo(rutaPNG->str()); _texturasMarco[6]->retener(NB_RETENEDOR_THIS);
	rutaPNG->establecer(prefijoRutaTexturas9); rutaPNG->agregar("8.png"); _texturasMarco[7]		= NBGestorTexturas::texturaDesdeArchivo(rutaPNG->str()); _texturasMarco[7]->retener(NB_RETENEDOR_THIS);
	rutaPNG->establecer(prefijoRutaTexturas9); rutaPNG->agregar("9.png"); _texturasMarco[8]		= NBGestorTexturas::texturaDesdeArchivo(rutaPNG->str()); _texturasMarco[8]->retener(NB_RETENEDOR_THIS);
	rutaPNG->liberar(NB_RETENEDOR_THIS);
	establecerCajaMarcoMalla(cajaMinima());
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

AUEscenaMalla9::~AUEscenaMalla9(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaMalla9::~AUEscenaMalla9")
	if(_mallaFondo != NULL) _mallaFondo->liberar(NB_RETENEDOR_THIS); _mallaFondo = NULL;
	//
	if(_texturasMarco[0] != NULL) _texturasMarco[0]->liberar(NB_RETENEDOR_THIS);
	if(_texturasMarco[1] != NULL) _texturasMarco[1]->liberar(NB_RETENEDOR_THIS);
	if(_texturasMarco[2] != NULL) _texturasMarco[2]->liberar(NB_RETENEDOR_THIS);
	if(_texturasMarco[3] != NULL) _texturasMarco[3]->liberar(NB_RETENEDOR_THIS);
	if(_texturasMarco[4] != NULL) _texturasMarco[4]->liberar(NB_RETENEDOR_THIS);
	if(_texturasMarco[5] != NULL) _texturasMarco[5]->liberar(NB_RETENEDOR_THIS);
	if(_texturasMarco[6] != NULL) _texturasMarco[6]->liberar(NB_RETENEDOR_THIS);
	if(_texturasMarco[7] != NULL) _texturasMarco[7]->liberar(NB_RETENEDOR_THIS);
	if(_texturasMarco[8] != NULL) _texturasMarco[8]->liberar(NB_RETENEDOR_THIS);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

NBCajaAABB AUEscenaMalla9::cajaMinima(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaMalla9::cajaMinima")
	NBTamano tamanoMinimo;
	tamanoMinimo.ancho = _texturasMarco[0]->tamanoHD().ancho /*+ _texturasMarco[1]->tamanoHD().ancho*/ + _texturasMarco[2]->tamanoHD().ancho;
	tamanoMinimo.alto = _texturasMarco[0]->tamanoHD().alto /*+ _texturasMarco[3]->tamanoHD().alto*/ + _texturasMarco[6]->tamanoHD().alto;
	NBCajaAABB cajaMinima;
	NBCAJAAABB_INICIALIZAR(cajaMinima);
	NBCAJAAABB_ENVOLVER_PUNTO(cajaMinima, tamanoMinimo.ancho/-2.0f, tamanoMinimo.alto/-2.0f);
	NBCAJAAABB_ENVOLVER_PUNTO(cajaMinima, tamanoMinimo.ancho/2.0f, tamanoMinimo.alto/2.0f);
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return cajaMinima;
}

void AUEscenaMalla9::actualizarPresentacionMalla(const NBCajaAABB &cajaMarco){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaMalla9::actualizarPresentacionMalla")
	float xMin	= cajaMarco.xMin;
	float yMin	= cajaMarco.yMin;
	float xMax	= cajaMarco.xMax;
	float yMax	= cajaMarco.yMax;
	//NBTamano tamCajaMarco; NBTAMANO_ESTABLECER(tamCajaMarco, xMax-xMin, yMax-yMin);
	NBPuntoColor puntosLocales[16];
	NBPuntoColor punto; NBCOLOR_ESTABLECER(punto.color, 255, 255, 255, 255)
	NBPUNTO_ESTABLECER(punto.coordenada, xMin, yMax); puntosLocales[0] = punto;
	NBPUNTO_ESTABLECER(punto.coordenada, xMin+_texturasMarco[0]->tamanoHD().ancho, yMax); puntosLocales[1] = punto;
	NBPUNTO_ESTABLECER(punto.coordenada, xMax-_texturasMarco[2]->tamanoHD().ancho, yMax); puntosLocales[2] = punto;
	NBPUNTO_ESTABLECER(punto.coordenada, xMax, yMax); puntosLocales[3] = punto;
	//
	NBPUNTO_ESTABLECER(punto.coordenada, xMin, yMax-_texturasMarco[0]->tamanoHD().alto); puntosLocales[4] = punto;
	NBPUNTO_ESTABLECER(punto.coordenada, xMin+_texturasMarco[0]->tamanoHD().ancho, yMax-_texturasMarco[0]->tamanoHD().alto); puntosLocales[5] = punto;
	NBPUNTO_ESTABLECER(punto.coordenada, xMax-_texturasMarco[2]->tamanoHD().ancho, yMax-_texturasMarco[0]->tamanoHD().alto); puntosLocales[6] = punto;
	NBPUNTO_ESTABLECER(punto.coordenada, xMax, yMax-_texturasMarco[0]->tamanoHD().alto); puntosLocales[7] = punto;
	//
	NBPUNTO_ESTABLECER(punto.coordenada, xMin, yMin+_texturasMarco[6]->tamanoHD().alto); puntosLocales[8] = punto;
	NBPUNTO_ESTABLECER(punto.coordenada, xMin+_texturasMarco[0]->tamanoHD().ancho, yMin+_texturasMarco[6]->tamanoHD().alto); puntosLocales[9] = punto;
	NBPUNTO_ESTABLECER(punto.coordenada, xMax-_texturasMarco[2]->tamanoHD().ancho, yMin+_texturasMarco[6]->tamanoHD().alto); puntosLocales[10] = punto;
	NBPUNTO_ESTABLECER(punto.coordenada, xMax, yMin+_texturasMarco[6]->tamanoHD().alto); puntosLocales[11] = punto;
	//
	NBPUNTO_ESTABLECER(punto.coordenada, xMin, yMin); puntosLocales[12] = punto;
	NBPUNTO_ESTABLECER(punto.coordenada, xMin+_texturasMarco[0]->tamanoHD().ancho, yMin); puntosLocales[13] = punto;
	NBPUNTO_ESTABLECER(punto.coordenada, xMax-_texturasMarco[2]->tamanoHD().ancho, yMin); puntosLocales[14] = punto;
	NBPUNTO_ESTABLECER(punto.coordenada, xMax, yMin); puntosLocales[15] = punto;
	if(_mallaFondo != NULL){
		_mallaFondo->establecerVerticesLocales(puntosLocales, 16);
	} else {
		NBRectangulo areaTextua; NBRECTANGULO_ESTABLECER(areaTextua, 0.0f, 0.0f, 1.0f, 1.0f)
		_mallaFondo = new(this) AUEscenaMalla(3, 3, puntosLocales, 16, _texturasMarco, 9, &areaTextua, 1);
		this->agregarObjetoEscena(_mallaFondo);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

//
AUOBJMETODOS_CLASESID_MULTICLASE(AUEscenaMalla9, AUEscenaMallaXBase)
AUOBJMETODOS_CLASESNOMBRES_MULTICLASE(AUEscenaMalla9, "AUEscenaMalla9", AUEscenaMallaXBase)
AUOBJMETODOS_CLONAR_NULL(AUEscenaMalla9)






