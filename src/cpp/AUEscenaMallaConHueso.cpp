//
//  AUEscenaMallaConHuesoAnimada.cpp
//  Gameplay_Mac
//
//  Created by Nicaragua Binary on 24/01/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkMediaStdAfx.h"
#include "AUEscenaMallaConHueso.h"
//
#include "NBLinea.h"

AUEscenaMallaConHueso::AUEscenaMallaConHueso() : AUEscenaMalla(), _verticesConHuesos(this), _datosHuesos(this) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMallaConHueso::AUEscenaMallaConHueso")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaMallaConHueso")
	#ifdef AUESCENAMALLACONHUESO_AGREGAR_HIJOS_DEBUG
	_debugVerticesReposo		= new(this) AUArregloMutable();
	_debugVerticesActuales		= new(this) AUArregloMutable();
	_debugVerticesInterPre		= new(this) AUArregloMutable();
	_debugVerticesInterPost		= new(this) AUArregloMutable();
	_debugUnionesVerticesArtics = new(this) AUArregloMutable();
	_debugLineasBisectoras		= new(this) AUArregloMutable();
	#endif
	//
	_cacheDefinicionHuesosSucia	= true;
	_cacheHuesosMovidos			= true;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUEscenaMallaConHueso::AUEscenaMallaConHueso(UI8 columnas, UI8 filas, AUArregloNativoP<NBPuntoColor>* verticesLocales, AUArregloNativoP<AUTextura*>* texturas, AUArregloNativoP<NBRectangulo>* areasTex) : AUEscenaMalla(columnas, filas, verticesLocales, texturas, areasTex), _verticesConHuesos(this, verticesLocales->conteo), _datosHuesos(this) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMallaConHueso::AUEscenaMallaConHueso")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaMallaConHueso")
	#ifdef AUESCENAMALLACONHUESO_AGREGAR_HIJOS_DEBUG
	_debugVerticesReposo		= new(this) AUArregloMutable();
	_debugVerticesActuales		= new(this) AUArregloMutable();
	_debugVerticesInterPre		= new(this) AUArregloMutable();
	_debugVerticesInterPost		= new(this) AUArregloMutable();
	_debugUnionesVerticesArtics = new(this) AUArregloMutable();
	_debugLineasBisectoras		= new(this) AUArregloMutable();
	#endif
	//Copiar las coordenadas originales de los vertices
	UI32 iVertice;
	for(iVertice=0; iVertice<this->_verticesLocales.conteo; iVertice++){
		STMallaConHuesoVertice datoVertice;
		datoVertice.verticeNormal			= this->_verticesLocales.elemento[iVertice].coordenada;
		datoVertice.indiceHueso				= 0;
		datoVertice.factorAvancePolilinea	= 0.0f;
		_verticesConHuesos.agregarElemento(datoVertice);
		#ifdef AUESCENAMALLACONHUESO_AGREGAR_HIJOS_DEBUG
		privInlineAgregarFigurasDebugParaVertice();
		#endif
	}
	//
	_cacheDefinicionHuesosSucia	= true;
	_cacheHuesosMovidos			= true;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUEscenaMallaConHueso::AUEscenaMallaConHueso(AUEscenaMallaConHueso* otraInstancia) : AUEscenaMalla(otraInstancia), _verticesConHuesos(this, &otraInstancia->_verticesConHuesos), _datosHuesos(this, &otraInstancia->_datosHuesos) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMallaConHueso::AUEscenaMallaConHueso")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaMallaConHueso")
	_cacheDefinicionHuesosSucia	= true;
	_cacheHuesosMovidos			= true;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUEscenaMallaConHueso::~AUEscenaMallaConHueso(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMallaConHueso::~AUEscenaMallaConHueso")
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

//

#ifdef AUESCENAMALLACONHUESO_AGREGAR_HIJOS_DEBUG
void AUEscenaMallaConHueso::privInlineAgregarFigurasDebugParaVertice(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMallaConHueso::privInlineAgregarFigurasDebugParaVertice")
	AUEscenaFigura* debugFig = new(this) AUEscenaFigura(ENEscenaFiguraTipo_Circulo);
	debugFig->agregarCoordenadaLocal(0.0f, 0.0f);
	debugFig->agregarCoordenadaLocal(4.0f, 0.0f);
	debugFig->establecerMultiplicadorColor8(0, 0, 0, 255);
	_debugVerticesActuales->agregarElemento(debugFig);
	this->agregarObjetoEscena(debugFig); debugFig->liberar(NB_RETENEDOR_THIS);
	//
	debugFig = new(this) AUEscenaFigura(ENEscenaFiguraTipo_Circulo);
	debugFig->establecerMultiplicadorColor8(80, 255, 80, 255);
	debugFig->agregarCoordenadaLocal(0.0f, 0.0f);
	debugFig->agregarCoordenadaLocal(4.0f, 0.0f);
	_debugVerticesReposo->agregarElemento(debugFig);
	this->agregarObjetoEscena(debugFig); debugFig->liberar(NB_RETENEDOR_THIS);
	//
	debugFig = new(this) AUEscenaFigura(ENEscenaFiguraTipo_Linea);
	debugFig->agregarCoordenadaLocal(0.0f, 0.0f);
	debugFig->agregarCoordenadaLocal(4.0f, 0.0f);
	debugFig->establecerMultiplicadorColor8(0, 255, 0, 255);
	_debugVerticesInterPre->agregarElemento(debugFig);
	this->agregarObjetoEscena(debugFig); debugFig->liberar(NB_RETENEDOR_THIS);
	//
	debugFig = new(this) AUEscenaFigura(ENEscenaFiguraTipo_Linea);
	debugFig->agregarCoordenadaLocal(0.0f, 0.0f);
	debugFig->agregarCoordenadaLocal(4.0f, 0.0f);
	debugFig->establecerMultiplicadorColor8(0, 0, 255, 255);
	_debugVerticesInterPost->agregarElemento(debugFig);
	this->agregarObjetoEscena(debugFig); debugFig->liberar(NB_RETENEDOR_THIS);
	//
	debugFig = new(this) AUEscenaFigura(ENEscenaFiguraTipo_Linea);
	debugFig->agregarCoordenadaLocal(0.0f, 0.0f);
	debugFig->agregarCoordenadaLocal(4.0f, 0.0f);
	debugFig->establecerMultiplicadorColor8(112, 0, 0, 255);
	_debugUnionesVerticesArtics->agregarElemento(debugFig);
	this->agregarObjetoEscena(debugFig); debugFig->liberar(NB_RETENEDOR_THIS);
	AU_GESTOR_PILA_LLAMADAS_POP_2
}
#endif

#ifdef AUESCENAMALLACONHUESO_AGREGAR_HIJOS_DEBUG
void AUEscenaMallaConHueso::privInlineAgregarFigurasDebugParaArticulacion(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMallaConHueso::privInlineAgregarFigurasDebugParaArticulacion")
	AUEscenaFigura* debugFig = new(this) AUEscenaFigura(ENEscenaFiguraTipo_Linea);
	debugFig->agregarCoordenadaLocal(0.0f, 0.0f);
	debugFig->agregarCoordenadaLocal(4.0f, 0.0f);
	debugFig->establecerMultiplicadorColor8(0, 112, 112, 255);
	_debugLineasBisectoras->agregarElemento(debugFig);
	this->agregarObjetoEscena(debugFig); debugFig->liberar(NB_RETENEDOR_THIS);
	AU_GESTOR_PILA_LLAMADAS_POP_2
}
#endif

//

void AUEscenaMallaConHueso::vaciarArticulaciones(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMallaConHueso::vaciarArticulaciones")
	_datosHuesos.vaciar();
	_cacheDefinicionHuesosSucia = true;
	AUESCENAMALLA_ENSUCIAR_MODELO
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaMallaConHueso::agregarArticulacion(float posX, float posY){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMallaConHueso::agregarArticulacion")
	STMallaConHuesoArticulacion articulacion;
	articulacion.posicionReposo.x	= posX;
	articulacion.posicionReposo.y	= posY;
	articulacion.posicionActual		= articulacion.posicionReposo;
	_datosHuesos.agregarElemento(articulacion);
	#ifdef AUESCENAMALLACONHUESO_AGREGAR_HIJOS_DEBUG
	privInlineAgregarFigurasDebugParaArticulacion();
	#endif
	_cacheDefinicionHuesosSucia = true;
	AUESCENAMALLA_ENSUCIAR_MODELO
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaMallaConHueso::agregarArticulacionEnIndice(UI32 indice, float posX, float posY){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMallaConHueso::agregarArticulacion")
	STMallaConHuesoArticulacion articulacion;
	articulacion.posicionReposo.x	= posX;
	articulacion.posicionReposo.y	= posY;
	articulacion.posicionActual		= articulacion.posicionReposo;
	_datosHuesos.agregarElementoEnIndice(articulacion, indice);
	#ifdef AUESCENAMALLACONHUESO_AGREGAR_HIJOS_DEBUG
	privInlineAgregarFigurasDebugParaArticulacion();
	#endif
	_cacheDefinicionHuesosSucia = true;
	AUESCENAMALLA_ENSUCIAR_MODELO
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaMallaConHueso::agregarArticulacionEnPuntoMedioDeHueso(UI32 indiceArticulacionIniHueso){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMallaConHueso::agregarArticulacion")
	NBASSERT(indiceArticulacionIniHueso<_datosHuesos.conteo-1);
	STMallaConHuesoArticulacion articulaAnterior = _datosHuesos.elemento[indiceArticulacionIniHueso];
	STMallaConHuesoArticulacion articulaPosterior = _datosHuesos.elemento[indiceArticulacionIniHueso+1];
	STMallaConHuesoArticulacion articulacion;
	articulacion.posicionReposo.x	= articulaAnterior.posicionReposo.x+((articulaPosterior.posicionReposo.x-articulaAnterior.posicionReposo.x)/2.0f);
	articulacion.posicionReposo.y	= articulaAnterior.posicionReposo.y+((articulaPosterior.posicionReposo.y-articulaAnterior.posicionReposo.y)/2.0f);
	articulacion.posicionActual.x	= articulaAnterior.posicionActual.x+((articulaPosterior.posicionActual.x-articulaAnterior.posicionActual.x)/2.0f);
	articulacion.posicionActual.y	= articulaAnterior.posicionActual.y+((articulaPosterior.posicionActual.y-articulaAnterior.posicionActual.y)/2.0f);
	_datosHuesos.agregarElementoEnIndice(articulacion, indiceArticulacionIniHueso+1);
	#ifdef AUESCENAMALLACONHUESO_AGREGAR_HIJOS_DEBUG
	privInlineAgregarFigurasDebugParaArticulacion();
	#endif
	_cacheDefinicionHuesosSucia = true;
	AUESCENAMALLA_ENSUCIAR_MODELO
	AU_GESTOR_PILA_LLAMADAS_POP_2
}


void AUEscenaMallaConHueso::agregarHuesosEnLinea(float xIniLinea, float yIniLinea, float xFinLinea, float yFinLinea, UI8 cantidadHuesos){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMallaConHueso::agregarHuesosEnLinea")
	NBASSERT(cantidadHuesos>=2)
	UI8 iArticulacion; NBPunto vectorLinea; NBPUNTO_ESTABLECER(vectorLinea, xFinLinea-xIniLinea, yFinLinea-yIniLinea);
	for(iArticulacion=0; iArticulacion<=cantidadHuesos; iArticulacion++){ //
		this->agregarArticulacion(xIniLinea+(vectorLinea.x*((float)iArticulacion/(float)cantidadHuesos)), yIniLinea+(vectorLinea.y*((float)iArticulacion/(float)cantidadHuesos)));
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaMallaConHueso::quitarArticulacionEnIndice(UI32 indice){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMallaConHueso::agregarArticulacion")
	NBASSERT(indice<_datosHuesos.conteo)
	NBASSERT(_datosHuesos.conteo>3) //Si falla, no se debe dejar los huesos con dos articulaciones
	_datosHuesos.quitarElementoEnIndice(indice);
	#ifdef AUESCENAMALLACONHUESO_AGREGAR_HIJOS_DEBUG
	_debugLineasBisectoras->quitarElementoEnIndice(0);
	#endif
	_cacheDefinicionHuesosSucia = true;
	AUESCENAMALLA_ENSUCIAR_MODELO
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

NBPunto AUEscenaMallaConHueso::verticeNormal(UI32 indice){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMallaConHueso::verticeNormal")
	NBASSERT(indice<_verticesConHuesos.conteo)
	NBPunto r = _verticesConHuesos.elemento[indice].verticeNormal;
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return r;
}

void AUEscenaMallaConHueso::moverVerticeNormal(UI32 indice, float posX, float posY){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMallaConHueso::moverVerticeNormal")
	NBASSERT(indice<_verticesConHuesos.conteo)
	_verticesConHuesos.elemento[indice].verticeNormal.x = posX;
	_verticesConHuesos.elemento[indice].verticeNormal.y = posY;
	_cacheDefinicionHuesosSucia = true;
	AUESCENAMALLA_ENSUCIAR_MODELO
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaMallaConHueso::moverArticulacionReposo(UI32 indice, float posX, float posY){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMallaConHueso::moverArticulacionReposo")
	NBASSERT(indice<_datosHuesos.conteo)
	_datosHuesos.elemento[indice].posicionReposo.x = posX;
	_datosHuesos.elemento[indice].posicionReposo.y = posY;
	_cacheDefinicionHuesosSucia = true;
	AUESCENAMALLA_ENSUCIAR_MODELO
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaMallaConHueso::moverArticulacion(UI32 indice, float posX, float posY){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMallaConHueso::moverArticulacion")
	NBASSERT(indice<_datosHuesos.conteo)
	_datosHuesos.elemento[indice].posicionActual.x = posX;
	_datosHuesos.elemento[indice].posicionActual.y = posY;
	_cacheHuesosMovidos	= true;
	AUESCENAMALLA_ENSUCIAR_MODELO
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaMallaConHueso::restablecerPosicionArticulaciones(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMallaConHueso::restablecerPosicionArticulaciones")
	UI8 iArt;
	for(iArt=0; iArt<_datosHuesos.conteo; iArt++){
		STMallaConHuesoArticulacion* datosArticulacion =  &(_datosHuesos.elemento[iArt]);
		datosArticulacion->posicionActual = datosArticulacion->posicionReposo;
	}
	_cacheHuesosMovidos = true;
	AUESCENAMALLA_ENSUCIAR_MODELO
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUArregloNativoP<STMallaConHuesoVertice>* AUEscenaMallaConHueso::datosVertices(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMallaConHueso::datosVertices")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return &_verticesConHuesos;
}

AUArregloNativoP<STMallaConHuesoArticulacion>* AUEscenaMallaConHueso::datosHuesos(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMallaConHueso::datosHuesos")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return &_datosHuesos;
}

void AUEscenaMallaConHueso::privInlineCalcularDatosVerticesAnteHuesos(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMallaConHueso::privInlineCalcularDatosVerticesAnteHuesos")
	if(_datosHuesos.conteo<=1){
		PRINTF_WARNING("una malla con huesos requiere por lo menos dos articulaciones");
	} else {
		NBASSERT(_datosHuesos.conteo>=3)
		//Reiniciar las relaciones de vertices
		SI32 iVertice;
		for(iVertice=_verticesConHuesos.conteo-1; iVertice>=0; iVertice--){
			_verticesConHuesos.elemento[iVertice].indiceHueso = 0;
			#ifdef AUESCENAMALLACONHUESO_AGREGAR_HIJOS_DEBUG
			((AUEscenaFigura*)_debugVerticesReposo->elemento[iVertice])->establecerTraslacion(_verticesConHuesos.elemento[iVertice].verticeNormal);
			#endif
		}
		//Actualizar los angulos bisectores de las articulaciones y calcular la relaciones de cada hueso con los vertices
		NBPunto posArticulaINI	= _datosHuesos.elemento[0].posicionReposo;
		NBPunto posArticulaACT	= _datosHuesos.elemento[1].posicionReposo;
		float anguloHuesoInicio; NBPUNTO_ANGULO_VECTOR(anguloHuesoInicio, posArticulaINI.x, posArticulaINI.y, posArticulaACT.x, posArticulaACT.y);
		float anguloIzquierdaAnt;		//Angulo formado a la izquierda de la articulacion (entre dos huesos)
		NBPunto vecNormalPerpendIni;	//Vector normalizado que apunta hacia la izquierda de la articulacion (mitad de angulo del hueso anterior y la otra mitad del hueso posterior)
		NBPunto vecDirPerpendIni;		//Vector que apunta hacia la izquierda de la articulacion (mitad de angulo del hueso anterior y la otra mitad del hueso posterior)
		//NBASSERT(NBPUNTO_ESTA_IZQUIERDA_VECTOR(posArticulaINI.x, posArticulaINI.y, posArticulaACT.x, posArticulaACT.y, vecDirPerpendIni.x, vecDirPerpendIni.y));
		UI32 iArtic, iArticConteo = _datosHuesos.conteo;
		for(iArtic=2; iArtic<=iArticConteo; iArtic++){
			NBPunto posArticulaFIN, vecNormalPerpendFin, vecDirPerpendFin; float anguloHuesoFinal, anguloIzquierda;
			if(iArtic<iArticConteo){
				posArticulaFIN		= _datosHuesos.elemento[iArtic].posicionReposo;
				NBPUNTO_ANGULO_VECTOR(anguloHuesoFinal, posArticulaACT.x, posArticulaACT.y, posArticulaFIN.x, posArticulaFIN.y);
				anguloIzquierda = 180.0f-(anguloHuesoFinal-anguloHuesoInicio);  //Angulo formado a la izquierda de la articulacion (entre dos huesos)
				if(anguloIzquierda>180.0) anguloIzquierda -= 360.0f;
				if(anguloIzquierda<-180.0) anguloIzquierda = -anguloIzquierda;
				else if(anguloIzquierda<0.0f) anguloIzquierda += 360.0f;
				if(iArtic==2){
					anguloIzquierdaAnt = anguloIzquierda;
					float anguloRotar = (anguloIzquierdaAnt/2.0f);
					NBPUNTO_ESTABLECER(vecNormalPerpendIni, posArticulaACT.x-posArticulaINI.x, posArticulaACT.y-posArticulaINI.y);  NBPUNTO_ROTAR(vecNormalPerpendIni, vecNormalPerpendIni, anguloRotar); NBPUNTO_NORMALIZAR_VECTOR(vecNormalPerpendIni);
					NBPUNTO_ESTABLECER(vecDirPerpendIni, posArticulaINI.x+vecNormalPerpendIni.x, posArticulaINI.y+vecNormalPerpendIni.y);
					#ifdef AUESCENAMALLACONHUESO_AGREGAR_HIJOS_DEBUG
					((AUEscenaFigura*)_debugLineasBisectoras->elemento[0])->moverVerticeHacia(0, posArticulaINI.x+(vecNormalPerpendIni.x*256.0f), posArticulaINI.y+(vecNormalPerpendIni.y*256.0f));
					((AUEscenaFigura*)_debugLineasBisectoras->elemento[0])->moverVerticeHacia(1, posArticulaINI.x-(vecNormalPerpendIni.x*256.0f), posArticulaINI.y-(vecNormalPerpendIni.y*256.0f));
					#endif
				}
				float anguloRotar = (anguloIzquierda/2.0f);
				NBPUNTO_ESTABLECER(vecNormalPerpendFin, posArticulaFIN.x-posArticulaACT.x, posArticulaFIN.y-posArticulaACT.y); NBPUNTO_ROTAR(vecNormalPerpendFin, vecNormalPerpendFin, anguloRotar); NBPUNTO_NORMALIZAR_VECTOR(vecNormalPerpendFin);
				NBPUNTO_ESTABLECER(vecDirPerpendFin, posArticulaACT.x+vecNormalPerpendFin.x, posArticulaACT.y+vecNormalPerpendFin.y);
			} else {
				posArticulaFIN		= posArticulaACT;	 //Necesarios establecer aunque no se use
				anguloHuesoFinal	= anguloHuesoInicio; //Necesarios establecer aunque no se use
				anguloIzquierda		= anguloIzquierdaAnt;
				float anguloRotar	= (180.0f-anguloIzquierda)+(anguloIzquierda/2.0f);
				NBPUNTO_ESTABLECER(vecNormalPerpendFin, posArticulaACT.x-posArticulaINI.x, posArticulaACT.y-posArticulaINI.y); NBPUNTO_ROTAR(vecNormalPerpendFin, vecNormalPerpendFin, anguloRotar); NBPUNTO_NORMALIZAR_VECTOR(vecNormalPerpendFin);
				NBPUNTO_ESTABLECER(vecDirPerpendFin, posArticulaACT.x+vecNormalPerpendFin.x, posArticulaACT.y+vecNormalPerpendFin.y);
			}
			//
			#ifdef AUESCENAMALLACONHUESO_AGREGAR_HIJOS_DEBUG
			((AUEscenaFigura*)_debugLineasBisectoras->elemento[iArtic-1])->moverVerticeHacia(0, posArticulaACT.x+(vecNormalPerpendIni.x*256.0f), posArticulaACT.y+(vecNormalPerpendIni.y*256.0f));
			((AUEscenaFigura*)_debugLineasBisectoras->elemento[iArtic-1])->moverVerticeHacia(1, posArticulaACT.x-(vecNormalPerpendIni.x*256.0f), posArticulaACT.y-(vecNormalPerpendIni.y*256.0f));
			#endif
			//NBASSERT(NBPUNTO_ESTA_IZQUIERDA_VECTOR(posArticulaACT.x, posArticulaACT.y, posArticulaFIN.x, posArticulaFIN.y, vecDirPerpendFin.x, vecDirPerpendFin.y));
			SI32 iVertice; //PRINTF_INFO("HUESO EN REPOSO %d\n", iArtic-1);
			for(iVertice=_verticesConHuesos.conteo-1; iVertice>=0; iVertice--){
				if(_verticesConHuesos.elemento[iVertice].indiceHueso==0){
					NBPunto verticeNormal = _verticesConHuesos.elemento[iVertice].verticeNormal;
					if(NBPUNTO_ESTA_IZQUIERDA_O_SOBRE_VECTOR(vecDirPerpendIni.x, vecDirPerpendIni.y, posArticulaINI.x, posArticulaINI.y, verticeNormal.x, verticeNormal.y)  && NBPUNTO_ESTA_IZQUIERDA_O_SOBRE_VECTOR(posArticulaACT.x, posArticulaACT.y, vecDirPerpendFin.x, vecDirPerpendFin.y, verticeNormal.x, verticeNormal.y)){
						float distanciaPerpendicular = -1.0f * NBPUNTO_DISTANCIA_CON_SIGNO_HACIA_LINEA(verticeNormal.x, verticeNormal.y, posArticulaINI.x, posArticulaINI.y, posArticulaACT.x, posArticulaACT.y);
						NBPunto vectorParaleloHueso; NBPUNTO_ESTABLECER(vectorParaleloHueso, 1.0f, 0.0f); NBPUNTO_ROTAR(vectorParaleloHueso, vectorParaleloHueso, anguloHuesoInicio); NBPUNTO_MOVER(vectorParaleloHueso, verticeNormal.x, verticeNormal.y)
						float denom, relativoHueso;
						NBLINEA_INSTERSECCION_RELATIVO_L1(relativoHueso, denom, verticeNormal.x, verticeNormal.y, vectorParaleloHueso.x, vectorParaleloHueso.y, posArticulaINI.x, posArticulaINI.y, vecDirPerpendIni.x, vecDirPerpendIni.y) NBASSERT(denom!=0.0f)
						NBPunto interseccionIzq;		NBPUNTO_ESTABLECER(interseccionIzq, verticeNormal.x + ((vectorParaleloHueso.x - verticeNormal.x) * relativoHueso), verticeNormal.y + ((vectorParaleloHueso.y - verticeNormal.y) * relativoHueso))
						NBLINEA_INSTERSECCION_RELATIVO_L1(relativoHueso, denom, verticeNormal.x, verticeNormal.y, vectorParaleloHueso.x, vectorParaleloHueso.y, posArticulaACT.x, posArticulaACT.y, vecDirPerpendFin.x, vecDirPerpendFin.y) NBASSERT(denom!=0.0f)
						NBPunto interseccionDer;		NBPUNTO_ESTABLECER(interseccionDer, verticeNormal.x + ((vectorParaleloHueso.x - verticeNormal.x) * relativoHueso), verticeNormal.y + ((vectorParaleloHueso.y - verticeNormal.y) * relativoHueso))
						//NBPunto interseccionIzq; NBLINEA_INSTERSECCION_POSICION_CONTRA_RAYO_INFINITO(interseccionIzq, verticeNormal.x, verticeNormal.y, vectorParaleloHueso.x, vectorParaleloHueso.y, posArticulaINI.x, posArticulaINI.y, vecDirPerpendIni.x, vecDirPerpendIni.y);
						//NBPunto interseccionDer; NBLINEA_INSTERSECCION_POSICION_CONTRA_RAYO_INFINITO(interseccionDer, verticeNormal.x, verticeNormal.y, vectorParaleloHueso.x, vectorParaleloHueso.y, posArticulaACT.x, posArticulaACT.y, vecDirPerpendFin.x, vecDirPerpendFin.y);
						float factorAvancePolilinea = NBPUNTO_DISTANCIA(interseccionIzq.x, interseccionIzq.y, verticeNormal.x, verticeNormal.y) / NBPUNTO_DISTANCIA(interseccionIzq.x, interseccionIzq.y, interseccionDer.x, interseccionDer.y);
						#ifdef AUESCENAMALLACONHUESO_AGREGAR_HIJOS_DEBUG
						((AUEscenaFigura*)_debugVerticesInterPre->elemento[iVertice])->moverVerticeHacia(0, verticeNormal.x, verticeNormal.y);
						((AUEscenaFigura*)_debugVerticesInterPre->elemento[iVertice])->moverVerticeHacia(1, interseccionIzq.x, interseccionIzq.y);
						((AUEscenaFigura*)_debugVerticesInterPost->elemento[iVertice])->moverVerticeHacia(0, verticeNormal.x, verticeNormal.y);
						((AUEscenaFigura*)_debugVerticesInterPost->elemento[iVertice])->moverVerticeHacia(1, interseccionDer.x, interseccionDer.y);
						#endif
						//PRINTF_INFO("VERTICE %d avance(%f)\n", iVertice, factorAvancePolilinea);
						_verticesConHuesos.elemento[iVertice].indiceHueso					= iArtic-1;
						if(distanciaPerpendicular>=0.0f){ //Está a la izquierda del hueso
							_verticesConHuesos.elemento[iVertice].distanciaBisectorIni			= NBPUNTO_DISTANCIA(posArticulaINI.x, posArticulaINI.y, interseccionIzq.x, interseccionIzq.y);
							_verticesConHuesos.elemento[iVertice].distanciaBisectorFin			= NBPUNTO_DISTANCIA(posArticulaACT.x, posArticulaACT.y, interseccionDer.x, interseccionDer.y);
							_verticesConHuesos.elemento[iVertice].anguloAperturaBisectorIni	= (anguloIzquierdaAnt/2.0f);
							_verticesConHuesos.elemento[iVertice].anguloAperturaBisectorFin	= (anguloIzquierda/2.0f);
						} else { //Está a la derecha del hueso
							_verticesConHuesos.elemento[iVertice].distanciaBisectorIni			= -NBPUNTO_DISTANCIA(posArticulaINI.x, posArticulaINI.y, interseccionIzq.x, interseccionIzq.y);
							_verticesConHuesos.elemento[iVertice].distanciaBisectorFin			= -NBPUNTO_DISTANCIA(posArticulaACT.x, posArticulaACT.y, interseccionDer.x, interseccionDer.y);
							_verticesConHuesos.elemento[iVertice].anguloAperturaBisectorIni	= ((360.0f-anguloIzquierdaAnt)/2.0f);
							_verticesConHuesos.elemento[iVertice].anguloAperturaBisectorFin	= ((360.0f-anguloIzquierda)/2.0f);
						}
						_verticesConHuesos.elemento[iVertice].factorAvancePolilinea			= factorAvancePolilinea;
					}
				}
			}
			//Siguiente
			posArticulaINI		= posArticulaACT;
			posArticulaACT		= posArticulaFIN;
			anguloHuesoInicio	= anguloHuesoFinal;
			anguloIzquierdaAnt	= anguloIzquierda;
			vecNormalPerpendIni	= vecNormalPerpendFin;
			vecDirPerpendIni	= vecDirPerpendFin;
		}
		_cacheDefinicionHuesosSucia = false;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaMallaConHueso::privInlineActualizarMallaSegunHuesos(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMallaConHueso::privInlineActualizarMallaSegunHuesos")
	if(_datosHuesos.conteo<=1){
		PRINTF_WARNING("una malla con huesos requiere por lo menos dos articulaciones\n");
	} else {
		if(_cacheDefinicionHuesosSucia){
			privInlineCalcularDatosVerticesAnteHuesos();
		}
		NBASSERT(_datosHuesos.conteo>=3)
		//Actualizar las posiciones de los vertices
		NBPunto posArticulaINI	= _datosHuesos.elemento[0].posicionActual;
		NBPunto posArticulaACT	= _datosHuesos.elemento[1].posicionActual;
		float anguloHuesoInicio; NBPUNTO_ANGULO_VECTOR(anguloHuesoInicio, posArticulaINI.x, posArticulaINI.y, posArticulaACT.x, posArticulaACT.y);
		float anguloIzquierdaAnt; //Angulo formado a la izquierda de la articulacion (entre dos huesos)
		NBPunto vecNormalPoliIni; //Vector normalizado que apunta hacia la izquierda de la articulacion (mitad de angulo del hueso anterior y la otra mitad del hueso posterior)
		UI32 iArtic, iArticConteo = _datosHuesos.conteo;
		for(iArtic=2; iArtic<=iArticConteo; iArtic++){
			NBPunto posArticulaFIN, vecNormalPoliFin; float anguloHuesoFinal, anguloIzquierda;
			if(iArtic<iArticConteo){
				posArticulaFIN		= _datosHuesos.elemento[iArtic].posicionActual;
				NBPUNTO_ANGULO_VECTOR(anguloHuesoFinal, posArticulaACT.x, posArticulaACT.y, posArticulaFIN.x, posArticulaFIN.y);
				anguloIzquierda = 180.0f-(anguloHuesoFinal-anguloHuesoInicio);  //Angulo formado a la izquierda de la articulacion (entre dos huesos)
				if(anguloIzquierda>180.0) anguloIzquierda -= 360.0f;
				if(anguloIzquierda<-180.0) anguloIzquierda = -anguloIzquierda;
				else if(anguloIzquierda<0.0f) anguloIzquierda += 360.0f;
				if(iArtic==2){
					anguloIzquierdaAnt = anguloIzquierda;
					float anguloRotar = (anguloIzquierdaAnt/2.0f);
					NBPUNTO_ESTABLECER(vecNormalPoliIni, posArticulaACT.x-posArticulaINI.x, posArticulaACT.y-posArticulaINI.y);  NBPUNTO_ROTAR(vecNormalPoliIni, vecNormalPoliIni, anguloRotar); NBPUNTO_NORMALIZAR_VECTOR(vecNormalPoliIni);
				}
				float anguloRotar = anguloIzquierda/2.0f;
				NBPUNTO_ESTABLECER(vecNormalPoliFin, posArticulaFIN.x-posArticulaACT.x, posArticulaFIN.y-posArticulaACT.y);  NBPUNTO_ROTAR(vecNormalPoliFin, vecNormalPoliFin, anguloRotar); NBPUNTO_NORMALIZAR_VECTOR(vecNormalPoliFin);
			} else {
				posArticulaFIN		= posArticulaACT;
				anguloHuesoFinal	= anguloHuesoInicio;
				anguloIzquierda		= anguloIzquierdaAnt;
				float anguloRotar	= (180.0f-anguloIzquierda)+(anguloIzquierda/2.0f);
				NBPUNTO_ESTABLECER(vecNormalPoliFin, posArticulaACT.x-posArticulaINI.x, posArticulaACT.y-posArticulaINI.y);  NBPUNTO_ROTAR(vecNormalPoliFin, vecNormalPoliFin, anguloRotar); NBPUNTO_NORMALIZAR_VECTOR(vecNormalPoliFin);
				//PRINTF_INFO("Angulo ante al final: %f posAnt(%f, %f) posAct(%f, %f) vectorNormalizado(%f, %f)\n", anguloIzquierda, posArticulaINI.x, posArticulaINI.y, posArticulaACT.x, posArticulaACT.y, vecNormalPoliFin.x, vecNormalPoliFin.y);
			}
			//Mover los vertices que le corresponden
			SI32 iVertice, indiceArticAsociada = (iArtic-1); //PRINTF_INFO("HUESO ACTUAL: %d\n", indiceArticAsociada);
			for(iVertice=_verticesConHuesos.conteo-1; iVertice>=0; iVertice--){
				if(_verticesConHuesos.elemento[iVertice].indiceHueso==indiceArticAsociada){
					float avancePolilinea			= _verticesConHuesos.elemento[iVertice].factorAvancePolilinea;
					float distanciaPoliIni			= _verticesConHuesos.elemento[iVertice].distanciaBisectorIni;
					float distanciaPoliFin			= _verticesConHuesos.elemento[iVertice].distanciaBisectorFin;
					float factorAvancePoliIni, factorAvancePoliFin;
					if(distanciaPoliIni>=0.0f){
						factorAvancePoliIni			= (_verticesConHuesos.elemento[iVertice].anguloAperturaBisectorIni*(anguloIzquierdaAnt/2.0f))/8100.0f; //8100.0f = 90.0f*90.0f
						factorAvancePoliFin			= (_verticesConHuesos.elemento[iVertice].anguloAperturaBisectorFin*(anguloIzquierda/2.0f))/8100.0f;
					} else {
						factorAvancePoliIni			= (_verticesConHuesos.elemento[iVertice].anguloAperturaBisectorIni*((360.0f-anguloIzquierdaAnt)/2.0f))/8100.0f;
						factorAvancePoliFin			= (_verticesConHuesos.elemento[iVertice].anguloAperturaBisectorFin*((360.0f-anguloIzquierda)/2.0f))/8100.0f;
					}
					NBPunto posPolilineaIni;		NBPUNTO_ESTABLECER(posPolilineaIni, posArticulaINI.x+(vecNormalPoliIni.x*distanciaPoliIni*(factorAvancePoliIni*factorAvancePoliIni)), posArticulaINI.y+(vecNormalPoliIni.y*distanciaPoliIni*(factorAvancePoliIni*factorAvancePoliIni)));
					NBPunto posPolilineaFin;		NBPUNTO_ESTABLECER(posPolilineaFin, posArticulaACT.x+(vecNormalPoliFin.x*distanciaPoliFin*(factorAvancePoliFin*factorAvancePoliFin)), posArticulaACT.y+(vecNormalPoliFin.y*distanciaPoliFin*(factorAvancePoliFin*factorAvancePoliFin)));
					//Resultado
					this->_verticesLocales.elemento[iVertice].coordenada.x = posPolilineaIni.x+((posPolilineaFin.x-posPolilineaIni.x)*avancePolilinea);
					this->_verticesLocales.elemento[iVertice].coordenada.y = posPolilineaIni.y+((posPolilineaFin.y-posPolilineaIni.y)*avancePolilinea);
					//PRINTF_INFO("Vertice %d pos (%f, %f)\n", iVertice, this->_verticesLocales.elemento[iVertice].x, this->_verticesLocales.elemento[iVertice].y);
					#ifdef AUESCENAMALLACONHUESO_AGREGAR_HIJOS_DEBUG
					((AUEscenaFigura*)_debugVerticesActuales->elemento[iVertice])->establecerTraslacion(this->_verticesLocales.elemento[iVertice]);
					((AUEscenaFigura*)_debugUnionesVerticesArtics->elemento[iVertice])->moverVerticeHacia(0, posArticulaACT.x, posArticulaACT.y);
					((AUEscenaFigura*)_debugUnionesVerticesArtics->elemento[iVertice])->moverVerticeHacia(1, this->_verticesLocales.elemento[iVertice].x, this->_verticesLocales.elemento[iVertice].y);
					#endif
				}
			}
			//Siguiente
			posArticulaINI		= posArticulaACT;
			posArticulaACT		= posArticulaFIN;
			anguloHuesoInicio	= anguloHuesoFinal;
			anguloIzquierdaAnt	= anguloIzquierda;
			vecNormalPoliIni	= vecNormalPoliFin;
		}
		#ifdef AUESCENAMALLACONHUESO_AGREGAR_HIJOS_DEBUG
		SI32 iVertice;
		for(iVertice=0; iVertice<_verticesConHuesos.conteo; iVertice++){
			((AUEscenaFigura*)_debugUnionesVerticesArtics->elemento[iVertice])->establecerVisible(_verticesConHuesos.elemento[iVertice].indiceHueso!=0);
			((AUEscenaFigura*)_debugVerticesReposo->elemento[iVertice])->establecerVisible(_verticesConHuesos.elemento[iVertice].indiceHueso!=0);
			((AUEscenaFigura*)_debugVerticesActuales->elemento[iVertice])->establecerVisible(_verticesConHuesos.elemento[iVertice].indiceHueso!=0);
			((AUEscenaFigura*)_debugVerticesInterPre->elemento[iVertice])->establecerVisible(_verticesConHuesos.elemento[iVertice].indiceHueso!=0);
			((AUEscenaFigura*)_debugVerticesInterPost->elemento[iVertice])->establecerVisible(_verticesConHuesos.elemento[iVertice].indiceHueso!=0);
		}
		#endif
		_cacheHuesosMovidos = false;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

PTRfunCmdsGL AUEscenaMallaConHueso::actualizarModeloGL(const NBPropRenderizado &propsRenderizado, const NBPropIluminacion &propsIluminacion, const NBPropHeredadasRender &propsHeredadas){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMallaConHueso::actualizarModeloGL")
	if(_cacheDefinicionHuesosSucia || _cacheHuesosMovidos){
		privInlineActualizarMallaSegunHuesos();
		AUESCENAMALLA_ENSUCIAR_MODELO
	}
	PTRfunCmdsGL r = AUEscenaMalla::actualizarModeloGL(propsRenderizado, propsIluminacion, propsHeredadas);
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return r;
}

//

AUEscenaMallaConHueso* AUEscenaMallaConHueso::crearMallaConHuesoSprite(ENMemoriaTipo tipoMemoriaResidencia, AUTextura* texturaSprite, UI8 columnas, UI8 filas, ENEscenaMallaAlineacionH alineacionH, ENEscenaMallaAlineacionV alineacionV){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMallaConHueso::crearMallaConHuesoSprite")
	AUArregloNativoMutableP<NBPuntoColor>* vertices	= new(ENMemoriaTipo_Temporal) AUArregloNativoMutableP<NBPuntoColor>();
	AUArregloNativoMutableP<AUTextura*>* texturas	= new(ENMemoriaTipo_Temporal) AUArregloNativoMutableP<AUTextura*>();
	AUArregloNativoMutableP<NBRectangulo>* areasTex	= new(ENMemoriaTipo_Temporal) AUArregloNativoMutableP<NBRectangulo>();
	NB_DEFINE_NOMBRE_PUNTERO(vertices, "AUEscenaMalla::vertices");
	NB_DEFINE_NOMBRE_PUNTERO(texturas, "AUEscenaMalla::texturas");
	NB_DEFINE_NOMBRE_PUNTERO(areasTex, "AUEscenaMalla::areasTex");
	//
	AUEscenaMalla::llenarParametrosMallaSprite(texturaSprite, columnas, filas, vertices, texturas, areasTex, alineacionH, alineacionV);
	AUEscenaMallaConHueso* nuevaMalla = new(tipoMemoriaResidencia) AUEscenaMallaConHueso(columnas, filas, vertices, texturas, areasTex);
	nuevaMalla->autoLiberar(NB_RETENEDOR_NULL);
	//
	vertices->liberar(NB_RETENEDOR_NULL);
	texturas->liberar(NB_RETENEDOR_NULL);
	areasTex->liberar(NB_RETENEDOR_NULL);
	//
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return nuevaMalla;
}

AUEscenaMallaConHueso* AUEscenaMallaConHueso::crearMallaConHuesoPatron(ENMemoriaTipo tipoMemoriaResidencia, AUTextura* texturaRepetir, UI8 columnas, UI8 filas, ENEscenaMallaAlineacionH alineacionH, ENEscenaMallaAlineacionV alineacionV){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMallaConHueso::crearMallaConHuesoPatron")
	AUArregloNativoMutableP<NBPuntoColor>* vertices	= new(ENMemoriaTipo_Temporal) AUArregloNativoMutableP<NBPuntoColor>();
	AUArregloNativoMutableP<AUTextura*>* texturas	= new(ENMemoriaTipo_Temporal) AUArregloNativoMutableP<AUTextura*>();
	AUArregloNativoMutableP<NBRectangulo>* areasTex	= new(ENMemoriaTipo_Temporal) AUArregloNativoMutableP<NBRectangulo>();
	NB_DEFINE_NOMBRE_PUNTERO(vertices, "AUEscenaMalla::vertices");
	NB_DEFINE_NOMBRE_PUNTERO(texturas, "AUEscenaMalla::texturas");
	NB_DEFINE_NOMBRE_PUNTERO(areasTex, "AUEscenaMalla::areasTex");
	//
	AUEscenaMalla::llenarParametrosMallaPatron(texturaRepetir, columnas, filas, vertices, texturas, areasTex, alineacionH, alineacionV);
	AUEscenaMallaConHueso* nuevaMalla = new(tipoMemoriaResidencia) AUEscenaMallaConHueso(columnas, filas, vertices, texturas, areasTex);
	nuevaMalla->autoLiberar(NB_RETENEDOR_NULL);
	//
	vertices->liberar(NB_RETENEDOR_NULL);
	texturas->liberar(NB_RETENEDOR_NULL);
	areasTex->liberar(NB_RETENEDOR_NULL);
	//
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return nuevaMalla;
}

AUEscenaMallaConHueso* AUEscenaMallaConHueso::crearMallaConHuesoDesdeArchivoXml(ENMemoriaTipo tipoMemoriaResidencia, const char* rutaArchivoMalla, ENEscenaMallaAlineacionH alineacionH, ENEscenaMallaAlineacionV alineacionV){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMallaConHueso::crearMallaConHuesoDesdeArchivoXml")
	AUArregloNativoMutableP<NBPuntoColor>* vertices	= new(ENMemoriaTipo_Temporal) AUArregloNativoMutableP<NBPuntoColor>();
	AUArregloNativoMutableP<AUTextura*>* texturas	= new(ENMemoriaTipo_Temporal) AUArregloNativoMutableP<AUTextura*>();
	AUArregloNativoMutableP<NBRectangulo>* areasTex	= new(ENMemoriaTipo_Temporal) AUArregloNativoMutableP<NBRectangulo>();
	NB_DEFINE_NOMBRE_PUNTERO(vertices, "AUEscenaMalla::vertices");
	NB_DEFINE_NOMBRE_PUNTERO(texturas, "AUEscenaMalla::texturas");
	NB_DEFINE_NOMBRE_PUNTERO(areasTex, "AUEscenaMalla::areasTex");
	//
	UI8 columnas, filas;
	AUEscenaMalla::llenarParametrosMallaDesdeArchivoXML(rutaArchivoMalla, &columnas, &filas, vertices, texturas, areasTex, alineacionH, alineacionV);
	AUEscenaMallaConHueso* nuevaMalla = new(tipoMemoriaResidencia) AUEscenaMallaConHueso(columnas, filas, vertices, texturas, areasTex);
	nuevaMalla->autoLiberar(NB_RETENEDOR_NULL);
	//
	vertices->liberar(NB_RETENEDOR_NULL);
	texturas->liberar(NB_RETENEDOR_NULL);
	areasTex->liberar(NB_RETENEDOR_NULL);
	//
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return nuevaMalla;
}

//

bool AUEscenaMallaConHueso::agregarXmlInternoEn(AUEscenaMallaConHueso* malla, AUCadenaLargaMutable8* guardarEn, const char* espaciosBaseIzq){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMallaConHueso::agregarXmlInternoEn")
	//guardarEn->agregarConFormato("%s<vXML>1</vXML>\r\n", espaciosBaseIzq);
	guardarEn->agregarConFormato("%s<eOb>\r\n", espaciosBaseIzq);
	AUEscenaMalla::agregarXmlInternoEn(malla, guardarEn, espaciosBaseIzq);
	guardarEn->agregarConFormato("%s</eOb>\r\n", espaciosBaseIzq);
	//Datos de huesos
	UI32 i, iConteo;
	guardarEn->agregarConFormato("%s<vsRep>\r\n", espaciosBaseIzq);
	iConteo = malla->_verticesConHuesos.conteo;
	for(i=0; i<iConteo; i++){
		STMallaConHuesoVertice vertice = malla->_verticesConHuesos.elemento[i];
		guardarEn->agregarConFormato("%s  <v>%f|%f</v>\r\n", espaciosBaseIzq, vertice.verticeNormal.x, vertice.verticeNormal.y);
	}
	guardarEn->agregarConFormato("%s</vsRep>\r\n", espaciosBaseIzq);
	//
	guardarEn->agregarConFormato("%s<arts>\r\n", espaciosBaseIzq);
	iConteo = malla->_datosHuesos.conteo;
	for(i=0; i<iConteo; i++){
		STMallaConHuesoArticulacion articulacion = malla->_datosHuesos.elemento[i];
		guardarEn->agregarConFormato("%s  <a>%f|%f|%f|%f</a>\r\n", espaciosBaseIzq, articulacion.posicionReposo.x, articulacion.posicionReposo.y, articulacion.posicionActual.x, articulacion.posicionActual.y);
	}
	guardarEn->agregarConFormato("%s</arts>\r\n", espaciosBaseIzq);
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return true;
}

bool AUEscenaMallaConHueso::interpretarXmlInterno(AUEscenaMallaConHueso* cargarEn, const AUDatosXML* datosXml, const sNodoXML* nodoXml){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMallaConHueso::interpretarXmlInterno")
	bool exito = false;
	//int versionXML = datosXml->nodoHijo("vXML", 0, nodoXml);
	//if(versionXML==1){
		const sNodoXML* nodoEscenaObj;	XML_NODO_HIJO(datosXml, nodoEscenaObj, "eOb", "escObj", nodoXml, NULL)
		if(nodoEscenaObj != NULL){
			if(AUEscenaMalla::interpretarXmlInterno(cargarEn, datosXml, nodoEscenaObj)){
				//Vaciar arreglos
				cargarEn->_verticesConHuesos.vaciar();
				cargarEn->_datosHuesos.vaciar();
				//Cargar datos XML
				const sNodoXML* nodoVertices; XML_NODO_HIJO(datosXml, nodoVertices, "vsRep", "verticesReposo", nodoXml, NULL);
				if(nodoVertices != NULL){
					const sNodoXML* nodoV = datosXml->nodoHijo("v", nodoVertices);
					while(nodoV != NULL){
						STMallaConHuesoVertice nuevoVertice;
						if(nodoV->nodosHijos == NULL){
							//Formato nuevo CSV (mas compacto)
							const char* cadenaCSV		= datosXml->cadenaValorDeNodo(nodoV);
							SI32 tamCadenaCSV		= AUCadena8::tamano(cadenaCSV);
							SI32 posEnCadenaCSV		= 0;
							nuevoVertice.verticeNormal.x			= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0.0f);
							nuevoVertice.verticeNormal.y			= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0.0f);
						} else {
							//Formato viejo XML (muy inflado)
							nuevoVertice.verticeNormal.x			= datosXml->nodoHijo("x", 0.0f, nodoV);
							nuevoVertice.verticeNormal.y			= datosXml->nodoHijo("y", 0.0f, nodoV);
						}
						cargarEn->_verticesConHuesos.agregarElemento(nuevoVertice);
						//siguiente
						nodoV = datosXml->nodoHijo("v", nodoVertices, nodoV);
					}
				}
				const sNodoXML* nodoArtics; XML_NODO_HIJO(datosXml, nodoArtics, "arts", "articulaciones", nodoXml, NULL);
				if(nodoArtics != NULL){
					const sNodoXML* nodoArt; XML_NODO_HIJO(datosXml, nodoArt, "a", "art", nodoArtics, NULL)
					while(nodoArt != NULL){
						STMallaConHuesoArticulacion nuevaArtic;
						if(nodoArt->nodosHijos == NULL){
							//Formato nuevo CSV (mas compacto)
							const char* cadenaCSV			= datosXml->cadenaValorDeNodo(nodoArt);
							SI32 tamCadenaCSV				= AUCadena8::tamano(cadenaCSV);
							SI32 posEnCadenaCSV				= 0;
							nuevaArtic.posicionReposo.x		= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0.0f);
							nuevaArtic.posicionReposo.y		= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0.0f);
							nuevaArtic.posicionActual.x		= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0.0f);
							nuevaArtic.posicionActual.y		= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0.0f);
						} else {
							//Formato viejo XML (muy inflado)
							nuevaArtic.posicionReposo.x = datosXml->nodoHijo("xRep", 0.0f, nodoArt);
							nuevaArtic.posicionReposo.y = datosXml->nodoHijo("yRep", 0.0f, nodoArt);
							nuevaArtic.posicionActual.x = datosXml->nodoHijo("xAct", 0.0f, nodoArt);
							nuevaArtic.posicionActual.y = datosXml->nodoHijo("yAct", 0.0f, nodoArt);
						}
						cargarEn->_datosHuesos.agregarElemento(nuevaArtic);
						//siguiente
						XML_NODO_HIJO(datosXml, nodoArt, "a", "art", nodoArtics, nodoArt)
					}
				}
				cargarEn->_cacheDefinicionHuesosSucia = true;
				cargarEn->_cacheHuesosMovidos = true;
				exito = true;
				AUESCENAMALLA_ENSUCIAR_MODELO_DE(cargarEn);
			}
		}
	//}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

bool AUEscenaMallaConHueso::agregarBitsInternosEn(AUEscenaMallaConHueso* malla, AUArchivo* guardarEn){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMallaConHueso::agregarBitsInternosEn")
	bool exito = false;
	//
	float versionDatos = 1.0f;
	guardarEn->escribir(&versionDatos, sizeof(versionDatos), 1, guardarEn);
	if(AUEscenaMalla::agregarBitsInternosEn(malla, guardarEn)){
		//Arreglos simples
		malla->_verticesConHuesos.agregarBitsInternosEn(guardarEn);
		malla->_datosHuesos.agregarBitsInternosEn(guardarEn);
		//
		exito = true;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

bool AUEscenaMallaConHueso::interpretarBitsInternos(AUEscenaMallaConHueso* cargarEn, AUArchivo* cargarDe){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMallaConHueso::interpretarBitsInternos")
	bool exito = false;
	float versionDatos;
	cargarDe->leer(&versionDatos, sizeof(versionDatos), 1, cargarDe);
	if(versionDatos==1.0f){
		if(AUEscenaMalla::interpretarBitsInternos(cargarEn, cargarDe)){
			//Arreglos simples
			cargarEn->_verticesConHuesos.interpretarBitsInternos(cargarDe);
			cargarEn->_datosHuesos.interpretarBitsInternos(cargarDe);
			//
			cargarEn->_cacheDefinicionHuesosSucia = true;
			cargarEn->_cacheHuesosMovidos = true;
			exito = true;
		}
	} else {
		PRINTF_ERROR("AUEscenaMallaConHueso, la version (%f) de datos binarios no es interpretable por este ejecutable\n", versionDatos);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}


//
AUOBJMETODOS_CLASESID_MULTICLASE(AUEscenaMallaConHueso, AUEscenaMalla)
AUOBJMETODOS_CLASESNOMBRES_MULTICLASE(AUEscenaMallaConHueso, "AUEscenaMallaConHueso", AUEscenaMalla)
AUOBJMETODOS_CLONAR_THIS(AUEscenaMallaConHueso)







