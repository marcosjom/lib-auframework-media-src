//
//  NBGestorAnimaciones.cpp
//  DodoTown
//
//  Created by Marcos Ortega on 23/08/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkMediaStdAfx.h"
#include "NBGestorAnimaciones.h"

bool NBGestorAnimaciones::_gestorInicializado				= false;
bool NBGestorAnimaciones::_leerPrecacheAnimaciones			= false;
bool NBGestorAnimaciones::_leerCacheAnimaciones				= false;
bool NBGestorAnimaciones::_escribirCacheAnimaciones			= false;
AUCadenaMutable8* NBGestorAnimaciones::_prefijoRutas		= NULL;
AUCadenaMutable8* NBGestorAnimaciones::_prefijoRutasCache	= NULL;

//Intermediario para la carga de cuerpos (intermediario necesario para incluir cuerpos en animaciones, y animaciones en cuerpos)
AUCargadorCuerposI*	NBGestorAnimaciones::_cargadorCuerpos	= NULL;

//Animaciones
SI32 NBGestorAnimaciones::_secuencialIDsAnimaciones			= 0;
AUArregloMutable* NBGestorAnimaciones::_plantillasAnimaciones = NULL;

bool NBGestorAnimaciones::inicializar(bool leerPrecacheAnimaciones, bool leerCacheAnimaciones, bool escribirCacheAnimaciones, AUCargadorCuerposI* cargadorCuerpos){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorAnimaciones::inicializar")
	_gestorInicializado				= false;
	_cargadorCuerpos				= cargadorCuerpos; if(_cargadorCuerpos != NULL) _cargadorCuerpos->retener(NB_RETENEDOR_NULL);
	_leerPrecacheAnimaciones		= leerPrecacheAnimaciones;
	_leerCacheAnimaciones			= leerCacheAnimaciones;
	_escribirCacheAnimaciones		= escribirCacheAnimaciones;
	_prefijoRutas					= new(ENMemoriaTipo_General) AUCadenaMutable8();	NB_DEFINE_NOMBRE_PUNTERO(_prefijoRutas, "NBGestorAnimaciones::_prefijoRutas");
	_prefijoRutasCache				= new(ENMemoriaTipo_General) AUCadenaMutable8();	NB_DEFINE_NOMBRE_PUNTERO(_prefijoRutasCache, "NBGestorAnimaciones::_prefijoRutasCache");
	//Animaciones
	_secuencialIDsAnimaciones 		= -1;
	_plantillasAnimaciones			= new(ENMemoriaTipo_General) AUArregloMutable();	NB_DEFINE_NOMBRE_PUNTERO(_plantillasAnimaciones, "NBGestorAnimaciones::_plantillasAnimaciones");
	_gestorInicializado				= true;
	NBASSERT(_cargadorCuerpos != NULL)
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return true;
}

void NBGestorAnimaciones::finalizar(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorAnimaciones::finalizar")
	//
	if(_prefijoRutas != NULL){
		_prefijoRutas->liberar(NB_RETENEDOR_NULL);
		_prefijoRutas = NULL;
	}
	if(_prefijoRutasCache != NULL){
		_prefijoRutasCache->liberar(NB_RETENEDOR_NULL);
		_prefijoRutasCache = NULL;
	}
	//Animaciones
	if(_plantillasAnimaciones != NULL){
		SI32 i; const SI32 count = _plantillasAnimaciones->conteo;
		for(i = 0; i < count; i++){
			AUAnimacionBibliotecaDef* defBiblioteca = (AUAnimacionBibliotecaDef*)_plantillasAnimaciones->elemento[i];
			//NBASSERT(defBiblioteca->conteoReferencias() == 2); //Bad resource management? Missing "obj->release()"?
			defBiblioteca->establecerObjetoEscena(NULL); //Quita la retencion circular entre la clase plantilla y la animacion plantilla dentro de ella
		}
		_plantillasAnimaciones->vaciar();
		_plantillasAnimaciones->liberar(NB_RETENEDOR_NULL);
		_plantillasAnimaciones = NULL;
	}
	if(_cargadorCuerpos != NULL){
		_cargadorCuerpos->liberar(NB_RETENEDOR_NULL);
		_cargadorCuerpos = NULL;
	}
	_gestorInicializado = false;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

bool NBGestorAnimaciones::gestorInicializado(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorAnimaciones::gestorInicializado")
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return _gestorInicializado;
}

bool NBGestorAnimaciones::leyendoCacheAnimaciones(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorAnimaciones::leyendoCacheAnimaciones")
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return _leerCacheAnimaciones;
}

bool NBGestorAnimaciones::escribiendoCacheAnimaciones(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorAnimaciones::escribiendoCacheAnimaciones")
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return _escribirCacheAnimaciones;
}

void NBGestorAnimaciones::establecerLecturaCacheAnimaciones(bool leerCacheAnimaciones){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorAnimaciones::establecerLecturaCacheAnimaciones")
	_leerCacheAnimaciones = leerCacheAnimaciones;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

void NBGestorAnimaciones::establecerEscrituraCacheAnimaciones(bool escribirCacheAnimaciones){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorAnimaciones::establecerEscrituraCacheAnimaciones")
	_escribirCacheAnimaciones = escribirCacheAnimaciones;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

const char* NBGestorAnimaciones::pefijoRutas(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorAnimaciones::pefijoRutas")
	const char* r = _prefijoRutas->str();
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return r;
}

void NBGestorAnimaciones::establecerPrefijoRutas(const char* prefijoStr, const char* prefijoStrCache){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorAnimaciones::establecerPrefijoRutas")
	_prefijoRutas->establecer(prefijoStr);
	_prefijoRutasCache->establecer(prefijoStrCache);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

UI16 NBGestorAnimaciones::liberarAnimacionesSinReferencias(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorAnimaciones::liberarAnimacionesSinReferencias")
	UI16 conteoLiberados = 0;
	SI32 i;
	for(i = (_plantillasAnimaciones->conteo - 1); i >= 0; i--){
		AUAnimacionBibliotecaDef* defBiblioteca = (AUAnimacionBibliotecaDef*)_plantillasAnimaciones->elemento[i];
		NBASSERT(defBiblioteca->conteoReferencias() >= 2)
		if(defBiblioteca->conteoReferencias() == 2){ //2 = solo retenido por el arreglo y por su propia 'plantillaAnimacion' (el ObjetoPlantilla y su AnimacionPlantilla se retienen mutuamente)
			defBiblioteca->establecerObjetoEscena(NULL); //Quita la retencion circular entre el ObjetoPlantilla y la AnimacionPlantilla
			NBASSERT(defBiblioteca->conteoReferencias() == 1) //After releasing circular retainig
			_plantillasAnimaciones->quitarElementoEnIndice(i);
			conteoLiberados++;
		} else {
			//PRINTF_INFO("PLANTILLA ANIMACION +PERSISTE+ ret(%d) autolib(%d) '%s'\n", defBiblioteca->conteoReferencias(), defBiblioteca->conteoAutoliberacionesPendientes(), defBiblioteca->rutaRecurso.str());
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return conteoLiberados;
}

AUEscenaAnimacion* NBGestorAnimaciones::animacionDesdeArchivo(ENMemoriaTipo tipoMemoriaResidencia, const char* rutaVirtualArchivo, ENAnimacion_TipoEncapsulamiento tipoEncapsulamiento){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorAnimaciones::animacionDesdeArchivo")
	AUEscenaAnimacion* animacion = NULL;
	const UI32 scaleBase2 = NBGestorTexturas::escalaTexturasBase2();
	const float scaleToHD = NBGestorTexturas::escalaTexturasHaciaHD();
	const UI32 scaleBase2ToLoad = (float)scaleBase2 * scaleToHD;
	PRINTF_INFO("ANIM, looking for efective scale: %d texBase2(%d) scaleToHD(%f).\n", scaleBase2ToLoad, scaleBase2, scaleToHD);
	//Buscar en las animaciones actuales
	SI32 iAnim;
	for(iAnim = 0; iAnim < _plantillasAnimaciones->conteo && animacion == NULL; iAnim++){
		AUAnimacionBibliotecaDef* defBiblio = (AUAnimacionBibliotecaDef*)_plantillasAnimaciones->elemento[iAnim];
		if(defBiblio->rutaRecurso.esIgual(rutaVirtualArchivo) && ((AUEscenaAnimacion*)defBiblio->plantillaAnimacion)->tipoEncapsulamientoHijos() == tipoEncapsulamiento && defBiblio->scaleBase2 == scaleBase2ToLoad){
			animacion = (AUEscenaAnimacion*)defBiblio->plantillaAnimacion->clonar(tipoMemoriaResidencia, ENClonadoTipo_CopiarDirecciones); NB_DEFINE_NOMBRE_PUNTERO(animacion, "NBGestorAnimaciones::animacion(copia_1)");
			//PRINTF_INFO("NBGestorAnimaciones, reutilizando animacion pre-cargada para '%s'\n", rutaArchivo);
		}
	}
	if(animacion == NULL){
		//Cargar desde el archivo cache
		const STNBSha1HashHex xHash = NBSha1_getHashHexBytes((const BYTE*)rutaVirtualArchivo, AUCadena8::tamano(rutaVirtualArchivo));
		AUCadenaMutable8* rutaVirtualAnim	= new(ENMemoriaTipo_Temporal) AUCadenaMutable8(_prefijoRutas->str());  rutaVirtualAnim->agregar(rutaVirtualArchivo);
		AUCadenaMutable8* rutaVirtualAnBin	= new(ENMemoriaTipo_Temporal) AUCadenaMutable8(_prefijoRutas->str());  rutaVirtualAnBin->agregar(xHash.v); rutaVirtualAnBin->agregar(".animBin");
		AUCadenaMutable8* rutaVirtualCache	= new(ENMemoriaTipo_Temporal) AUCadenaMutable8(_prefijoRutasCache->str());  rutaVirtualCache->agregar(xHash.v); rutaVirtualCache->agregar(".animBin");
		AUCadenaMutable8* rutaAnimacion		= new(ENMemoriaTipo_Temporal) AUCadenaMutable8(NBGestorArchivos::rutaHaciaRecursoEnPaquete(rutaVirtualAnim->str())); 
		AUCadenaMutable8* rutaAnimBin		= new(ENMemoriaTipo_Temporal) AUCadenaMutable8(NBGestorArchivos::rutaHaciaRecursoEnPaquete(rutaVirtualAnBin->str()));
		AUCadenaMutable8* rutaAnimCache		= new(ENMemoriaTipo_Temporal) AUCadenaMutable8(NBGestorArchivos::rutaHaciaRecursoEnCacheLogica(rutaVirtualCache->str()));
		//Intentar cargar archivo precache
		if(animacion == NULL && _leerCacheAnimaciones){
			animacion = NBGestorAnimaciones::privAnimacionDesdeFlujoBinario(tipoMemoriaResidencia, rutaAnimBin->str(), rutaVirtualArchivo, scaleBase2ToLoad, tipoEncapsulamiento);
			if(animacion != NULL) PRINTF_INFO("OPTIOMIZACION, ANIMACION cargada desde binario (PRECACHE)\n");
		}
		//Intentar cargar archivo cache
		if(animacion == NULL && _leerCacheAnimaciones){
			animacion = NBGestorAnimaciones::privAnimacionDesdeFlujoBinario(tipoMemoriaResidencia, rutaAnimCache->str(), rutaVirtualArchivo, scaleBase2ToLoad, tipoEncapsulamiento);
			if(animacion != NULL) PRINTF_INFO("OPTIOMIZACION, ANIMACION: cargada desde binario (CACHE)\n");
		}
		//Cargar desde el archivo animacion
		if(animacion == NULL){
			AUDatosXMLMutable* datosXml = new(ENMemoriaTipo_Temporal) AUDatosXMLMutable(); NB_DEFINE_NOMBRE_PUNTERO(datosXml, "NBGestorAnimaciones::datosXml");
			if(!datosXml->cargaDatosXMLDesdeArchivo(rutaAnimacion->str())){
				NBASSERT(false)
			} else {
				const sNodoXML* nodoRaiz = datosXml->nodoHijo("animacion");
				NBASSERT(nodoRaiz != NULL)
				if(nodoRaiz != NULL){
					AUCadenaLargaMutable8* strLista = new(ENMemoriaTipo_Temporal) AUCadenaLargaMutable8();
					AUCadenaMutable8* tmpStructura = new(ENMemoriaTipo_Temporal) AUCadenaMutable8();
					AUCadenaMutable8* tmpValor1 = new(ENMemoriaTipo_Temporal) AUCadenaMutable8();
					AUCadenaMutable8* tmpValor2 = new(ENMemoriaTipo_Temporal) AUCadenaMutable8();
					AUCadenaMutable8* tmpValor3 = new(ENMemoriaTipo_Temporal) AUCadenaMutable8();
					AUCadenaMutable8* tmpValor4 = new(ENMemoriaTipo_Temporal) AUCadenaMutable8();
					AUAnimacionBibliotecaDef* nuevaBiblioteca = new(ENMemoriaTipo_General) AUAnimacionBibliotecaDef(); NB_DEFINE_NOMBRE_PUNTERO(nuevaBiblioteca, "NBGestorAnimaciones::nuevaBiblioteca(XML)");
					//leer la biblioteca de transformaciones (compatibilidad con el formato viejo)
					const UI32 fileScaleBase2	= datosXml->nodoHijo("scaleBase2", 4, nodoRaiz); //By default, animations are designed "scaleBase2 = 4";
					bool transformacionesLeidas	= false;
					const sNodoXML* nodoTransformaciones =  datosXml->nodoHijo("bibliotecaTransformaciones", nodoRaiz);
					if(nodoTransformaciones != NULL){
						transformacionesLeidas = true;
						const sNodoXML* nTransf =  datosXml->nodoHijo("t", nodoTransformaciones);
						while(nTransf != NULL){
							NBPunto traslado; NBTamano escala; float rotacion;
							traslado.x		= datosXml->nodoHijo("tx", 0.0f, nTransf);
							traslado.y		= datosXml->nodoHijo("ty", 0.0f, nTransf);
							escala.ancho	= datosXml->nodoHijo("ex", 1.0f, nTransf);
							escala.alto		= datosXml->nodoHijo("ey", 1.0f, nTransf);
							rotacion		= datosXml->nodoHijo("rot", 0.0f, nTransf);
							nuevaBiblioteca->traslaciones.agregarElemento(traslado);
							nuevaBiblioteca->escalaciones.agregarElemento(escala);
							nuevaBiblioteca->rotaciones.agregarElemento(rotacion);
							//
							nTransf =  datosXml->nodoHijo("t", nodoTransformaciones, nTransf);
						}
						//PRINTF_INFO("TAMANO BIBLIOTECA TRANSFORMACIONES: %d\n", indicesTransfLocalesAGlobales->conteo);
					} else {
						//formato nuevo
						transformacionesLeidas = true;
						const sNodoXML* nodoTraslaciones; XML_NODO_HIJO(datosXml, nodoTraslaciones, "bibTras", "bibliotecaTraslaciones", nodoRaiz, NULL)
						if(nodoTraslaciones != NULL){
							XML_VALOR_STR_NODO_HIJO(datosXml, strLista, "l", "lista", "", nodoTraslaciones, NULL)
							SI32 posSeparadorST = -1, strListaConteo = strLista->tamano();
							do {
								SI32 nuevaPosSeparadorST = strLista->indiceDe("|", posSeparadorST+1); if(nuevaPosSeparadorST==-1) nuevaPosSeparadorST = strListaConteo;
								tmpStructura->vaciar(); tmpStructura->agregar(&strLista->str()[posSeparadorST+1], nuevaPosSeparadorST-posSeparadorST-1);
								SI32 posSeparadorValores = tmpStructura->indiceDe(","); NBASSERT(posSeparadorValores>0)
								tmpValor1->vaciar(); tmpValor1->agregar(tmpStructura->str(), posSeparadorValores);
								tmpValor2->vaciar(); tmpValor2->agregar(&tmpStructura->str()[posSeparadorValores+1], tmpStructura->tamano()-posSeparadorValores-1);
								NBASSERT(AUNumericoFloat8::esDecimal(tmpValor1->str()))
								NBASSERT(AUNumericoFloat8::esDecimal(tmpValor2->str()))
								NBPunto traslacion;
								traslacion.x		= AUNumericoP<float, char>::aDecimal(tmpValor1->str());
								traslacion.y		= AUNumericoP<float, char>::aDecimal(tmpValor2->str());
								nuevaBiblioteca->traslaciones.agregarElemento(traslacion);
								//
								posSeparadorST = nuevaPosSeparadorST;
							} while(posSeparadorST<strListaConteo);
						}
						const sNodoXML* nodoEscalaciones; XML_NODO_HIJO(datosXml, nodoEscalaciones, "bibEsc", "bibliotecaEscalaciones", nodoRaiz, NULL)
						if(nodoEscalaciones != NULL){
							XML_VALOR_STR_NODO_HIJO(datosXml, strLista, "l", "lista", "", nodoEscalaciones, NULL)
							SI32 posSeparadorST = -1, strListaTamano = strLista->tamano();
							do {
								SI32 nuevaPosSeparadorST = strLista->indiceDe("|", posSeparadorST+1); if(nuevaPosSeparadorST==-1) nuevaPosSeparadorST = strListaTamano;
								tmpStructura->vaciar(); tmpStructura->agregar(&strLista->str()[posSeparadorST+1], nuevaPosSeparadorST-posSeparadorST-1);
								SI32 posSeparadorValores = tmpStructura->indiceDe(","); NBASSERT(posSeparadorValores>0)
								tmpValor1->vaciar(); tmpValor1->agregar(tmpStructura->str(), posSeparadorValores);
								tmpValor2->vaciar(); tmpValor2->agregar(&tmpStructura->str()[posSeparadorValores+1], tmpStructura->tamano()-posSeparadorValores-1);
								NBASSERT(AUNumericoFloat8::esDecimal(tmpValor1->str()))
								NBASSERT(AUNumericoFloat8::esDecimal(tmpValor2->str()))
								NBTamano escalacion;
								escalacion.ancho	= AUNumericoP<float, char>::aDecimal(tmpValor1->str());
								escalacion.alto		= AUNumericoP<float, char>::aDecimal(tmpValor2->str());
								nuevaBiblioteca->escalaciones.agregarElemento(escalacion);
								//
								posSeparadorST = nuevaPosSeparadorST;
							} while(posSeparadorST<strListaTamano);
						}
						const sNodoXML* nodoRotaciones; XML_NODO_HIJO(datosXml, nodoRotaciones, "bibRot", "bibliotecaRotaciones", nodoRaiz, NULL)
						if(nodoRotaciones != NULL){
							XML_VALOR_STR_NODO_HIJO(datosXml, strLista, "l", "lista", "", nodoRotaciones, NULL)
							SI32 posSeparadorST = -1, strListaTamano = strLista->tamano();
							do {
								SI32 nuevaPosSeparadorST = strLista->indiceDe("|", posSeparadorST+1); if(nuevaPosSeparadorST==-1) nuevaPosSeparadorST = strListaTamano;
								tmpStructura->vaciar(); tmpStructura->agregar(&strLista->str()[posSeparadorST+1], nuevaPosSeparadorST-posSeparadorST-1);
								NBASSERT(AUNumericoFloat8::esDecimal(tmpStructura->str()))
								nuevaBiblioteca->rotaciones.agregarElemento(AUNumericoP<float, char>::aDecimal(tmpStructura->str()));
								//
								posSeparadorST = nuevaPosSeparadorST;
							} while(posSeparadorST<strListaTamano);
						}
						NBASSERT(nodoTraslaciones != NULL && nodoEscalaciones != NULL && nodoRotaciones != NULL)
					}
					//leer la biblioteca de colores
					bool coloresLeidos = false;
					const sNodoXML* nodoColores8;	XML_NODO_HIJO(datosXml, nodoColores8, "bibCol8", "bibliotecaColores8", nodoRaiz, NULL)
					if(nodoColores8 != NULL){
						coloresLeidos = true;
						XML_VALOR_STR_NODO_HIJO(datosXml, strLista, "l", "lista", "", nodoColores8, NULL)
						if(strLista->tamano() != 0){
							SI32 posSeparadorST = -1, strListaTamano = strLista->tamano();
							do {
								SI32 nuevaPosSeparadorST = strLista->indiceDe("|", posSeparadorST+1); if(nuevaPosSeparadorST==-1) nuevaPosSeparadorST = strListaTamano;
								tmpStructura->vaciar(); tmpStructura->agregar(&strLista->str()[posSeparadorST+1], nuevaPosSeparadorST-posSeparadorST-1);
								SI32 posSeparador0Valores = tmpStructura->indiceDe(","); NBASSERT(posSeparador0Valores>0)
								SI32 posSeparador1Valores = tmpStructura->indiceDe(",", posSeparador0Valores+1); NBASSERT(posSeparador1Valores>posSeparador0Valores)
								SI32 posSeparador2Valores = tmpStructura->indiceDe(",", posSeparador1Valores+1); NBASSERT(posSeparador2Valores>posSeparador1Valores)
								tmpValor1->vaciar(); tmpValor1->agregar(tmpStructura->str(), posSeparador0Valores);
								tmpValor2->vaciar(); tmpValor2->agregar(&tmpStructura->str()[posSeparador0Valores+1], posSeparador1Valores-posSeparador0Valores-1);
								tmpValor3->vaciar(); tmpValor3->agregar(&tmpStructura->str()[posSeparador1Valores+1], posSeparador2Valores-posSeparador1Valores-1);
								tmpValor4->vaciar(); tmpValor4->agregar(&tmpStructura->str()[posSeparador2Valores+1], tmpStructura->tamano()-posSeparador2Valores-1);
								NBASSERT(AUNumericoFloat8::esEntero(tmpValor1->str()))
								NBASSERT(AUNumericoFloat8::esEntero(tmpValor2->str()))
								NBASSERT(AUNumericoFloat8::esEntero(tmpValor3->str()))
								NBASSERT(AUNumericoFloat8::esEntero(tmpValor4->str()))
								NBColor8 color;
								color.r		= AUNumericoP<float, char>::aEntero(tmpValor1->str());
								color.g		= AUNumericoP<float, char>::aEntero(tmpValor2->str());
								color.b		= AUNumericoP<float, char>::aEntero(tmpValor3->str());
								color.a		= AUNumericoP<float, char>::aEntero(tmpValor4->str());
								nuevaBiblioteca->colores.agregarElemento(color);
								//
								posSeparadorST = nuevaPosSeparadorST;
							} while(posSeparadorST<strListaTamano);
						} else { //COmpatibilidad con formato medio
							const sNodoXML* nColor =  datosXml->nodoHijo("c", nodoColores8);
							while(nColor != NULL){
								NBColor8 color;
								color.r					= datosXml->nodoHijo("r", 255, nColor);
								color.g					= datosXml->nodoHijo("g", 255, nColor);
								color.b					= datosXml->nodoHijo("b", 255, nColor);
								color.a					= datosXml->nodoHijo("a", 255, nColor);
								nuevaBiblioteca->colores.agregarElemento(color);
								//
								nColor =  datosXml->nodoHijo("c", nodoColores8, nColor);
							}
						}
					} else { //Conservar compatibilidad con formatos anteriores
						const sNodoXML* nodoColores =  datosXml->nodoHijo("bibliotecaColores", nodoRaiz);
						if(nodoColores != NULL){
							coloresLeidos = true;
							const sNodoXML* nColor =  datosXml->nodoHijo("c", nodoColores);
							while(nColor != NULL){
								NBColor8 color;
								color.r					= (UI8)(255.0f*datosXml->nodoHijo("r", 1.0f, nColor));
								color.g					= (UI8)(255.0f*datosXml->nodoHijo("g", 1.0f, nColor));
								color.b					= (UI8)(255.0f*datosXml->nodoHijo("b", 1.0f, nColor));
								color.a					= (UI8)(255.0f*datosXml->nodoHijo("a", 1.0f, nColor));
								nuevaBiblioteca->colores.agregarElemento(color);
								//
								nColor =  datosXml->nodoHijo("c", nodoColores, nColor);
							}
							//PRINTF_INFO("TAMANO BIBLIOTECA COLORES: %d\n", indicesColoresLocalesAGlobales->conteo);
						}
						NBASSERT(nodoColores != NULL)
					}
					//leer animacion raiz
					const sNodoXML* nodoRaizAnim =  datosXml->nodoHijo("raizAnimacion", nodoRaiz);
					NBASSERT(transformacionesLeidas && coloresLeidos)
					NBASSERT(nodoRaizAnim != NULL)
					if(transformacionesLeidas && coloresLeidos && nodoRaizAnim != NULL){
						AUEscenaAnimacion* nuevaPlantillaAnimacion = NBGestorAnimaciones::privSubanimacionDesdeNodoXml(datosXml, nodoRaizAnim, nuevaBiblioteca, &nuevaBiblioteca->traslaciones, &nuevaBiblioteca->escalaciones, &nuevaBiblioteca->rotaciones, &nuevaBiblioteca->colores, tipoEncapsulamiento);
						NBASSERT(nuevaPlantillaAnimacion != NULL)
						if(nuevaPlantillaAnimacion != NULL){
							//nuevaPlantillaAnimacion->retener(NB_RETENEDOR_NULL); //Do not retain! (will be autoreleased and "->establecerObjetoEscena()" will retain it)
							_secuencialIDsAnimaciones++;
							nuevaBiblioteca->scaleBase2		= fileScaleBase2;
							nuevaBiblioteca->identificador	= _secuencialIDsAnimaciones;
							nuevaBiblioteca->rutaRecurso.establecer(rutaVirtualArchivo);
							nuevaBiblioteca->establecerObjetoEscena(nuevaPlantillaAnimacion);
							//Adjust transformations
							if(fileScaleBase2 != scaleBase2ToLoad){
								const float scaleToApply = (float)scaleBase2ToLoad / (float)fileScaleBase2;
								PRINTF_INFO("ANIM, aplying scale (%f) to anim-xml.\n", scaleToApply);
								//Scale translations
								{
									SI32 i; const SI32 count = nuevaBiblioteca->traslaciones.conteo;
									for(i = 0; i < count; i++){
										NBPunto* v = nuevaBiblioteca->traslaciones.elemPtr(i);
										v->x *= scaleToApply;
										v->y *= scaleToApply;
									}
								}
								//Scale scales
								{
									SI32 i; const SI32 count = nuevaBiblioteca->escalaciones.conteo;
									for(i = 0; i < count; i++){
										NBTamano* v = nuevaBiblioteca->escalaciones.elemPtr(i);
										v->ancho *= scaleToApply;
										v->alto *= scaleToApply;
									}
								}
								//
								nuevaBiblioteca->scaleBase2 = scaleBase2ToLoad;
							}
							_plantillasAnimaciones->agregarElemento(nuevaBiblioteca);
							animacion = (AUEscenaAnimacion*)nuevaPlantillaAnimacion->clonar(tipoMemoriaResidencia, ENClonadoTipo_CopiarDirecciones); NB_DEFINE_NOMBRE_PUNTERO(animacion, "NBGestorAnimaciones::animacion(copia_2)");
							//ESCRIBIR CACHE BINARIA
							if(_escribirCacheAnimaciones){
								privGuardarAnimacionEnBinario(nuevaPlantillaAnimacion, rutaAnimCache->str());
							}
						}
					}
					//
					tmpStructura->liberar(NB_RETENEDOR_NULL);
					tmpValor1->liberar(NB_RETENEDOR_NULL);
					tmpValor2->liberar(NB_RETENEDOR_NULL);
					tmpValor3->liberar(NB_RETENEDOR_NULL);
					tmpValor4->liberar(NB_RETENEDOR_NULL);
					strLista->liberar(NB_RETENEDOR_NULL);
					nuevaBiblioteca->liberar(NB_RETENEDOR_NULL);
				}
			}
			datosXml->liberar(NB_RETENEDOR_NULL);
		}
		rutaVirtualAnBin->liberar(NB_RETENEDOR_NULL);
		rutaVirtualCache->liberar(NB_RETENEDOR_NULL);
		rutaVirtualAnim->liberar(NB_RETENEDOR_NULL);
		rutaAnimacion->liberar(NB_RETENEDOR_NULL);
		rutaAnimBin->liberar(NB_RETENEDOR_NULL);
		rutaAnimCache->liberar(NB_RETENEDOR_NULL);
	}
	//Autoliberar
	if(animacion == NULL){
		PRINTF_ERROR("no se puo cargar animacion: '%s'\n", rutaVirtualArchivo);
	} else {
		animacion->detenerAnimacion();
		animacion->rebobinarAnimacion();
		animacion->autoLiberar(NB_RETENEDOR_NULL);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return animacion;
}

AUEscenaAnimacion* NBGestorAnimaciones::privAnimacionDesdeFlujoBinario(ENMemoriaTipo tipoMemoriaResidencia, const char* rutaFisicaArchivo, const char* rutaRecurso, const UI32 scaleBase2ToLoad, ENAnimacion_TipoEncapsulamiento tipoEncapsulamiento){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorAnimaciones::privAnimacionDesdeFlujoBinario")
	AUEscenaAnimacion* animacion = NULL;
	AUArchivo* archivoCache = NBGestorArchivos::flujoDeArchivo(ENMemoriaTipo_Temporal, rutaFisicaArchivo, ENArchivoModo_SoloLectura);
	if(archivoCache != NULL){
		archivoCache->lock();
		bool esFlujoBinarioValido = true;
		//leer la biblioteca de propiedades
		AUAnimacionBibliotecaDef* nuevaBiblioteca	= new(ENMemoriaTipo_General) AUAnimacionBibliotecaDef(); NB_DEFINE_NOMBRE_PUNTERO(nuevaBiblioteca, "NBGestorAnimaciones::nuevaBiblioteca(cache)");
		NBGESTORANIMACIONES_LEER_VERIFICACION_BIN(archivoCache, esFlujoBinarioValido);
		if(esFlujoBinarioValido){
			archivoCache->leer(&nuevaBiblioteca->scaleBase2, sizeof(nuevaBiblioteca->scaleBase2), 1, archivoCache);
			if(nuevaBiblioteca->scaleBase2 > 0 && nuevaBiblioteca->scaleBase2 < 128){
				NBGESTORANIMACIONES_LEER_VERIFICACION_BIN(archivoCache, esFlujoBinarioValido);
				if(esFlujoBinarioValido){
					nuevaBiblioteca->traslaciones.interpretarBitsInternos(archivoCache);
					NBGESTORANIMACIONES_LEER_VERIFICACION_BIN(archivoCache, esFlujoBinarioValido);
					if(esFlujoBinarioValido){
						nuevaBiblioteca->escalaciones.interpretarBitsInternos(archivoCache);
						NBGESTORANIMACIONES_LEER_VERIFICACION_BIN(archivoCache, esFlujoBinarioValido);
						if(esFlujoBinarioValido){
							nuevaBiblioteca->rotaciones.interpretarBitsInternos(archivoCache);
							NBGESTORANIMACIONES_LEER_VERIFICACION_BIN(archivoCache, esFlujoBinarioValido);
							if(esFlujoBinarioValido){
								nuevaBiblioteca->colores.interpretarBitsInternos(archivoCache);
								NBGESTORANIMACIONES_LEER_VERIFICACION_BIN(archivoCache, esFlujoBinarioValido);
								if(esFlujoBinarioValido){
									//leer animacion raiz
									AUEscenaAnimacion* nuevaPlantillaAnimacion = NBGestorAnimaciones::privSubanimacionDesdeFlujoBinario(archivoCache, nuevaBiblioteca, &nuevaBiblioteca->traslaciones, &nuevaBiblioteca->escalaciones, &nuevaBiblioteca->rotaciones, &nuevaBiblioteca->colores, tipoEncapsulamiento);
									if(nuevaPlantillaAnimacion != NULL){
										//nuevaPlantillaAnimacion->retener(NB_RETENEDOR_NULL); //Do not retain! (will be autoreleased and "->establecerObjetoEscena()" will retain it)
										_secuencialIDsAnimaciones++;
										nuevaBiblioteca->identificador = _secuencialIDsAnimaciones;
										nuevaBiblioteca->rutaRecurso.establecer(rutaRecurso);
										nuevaBiblioteca->establecerObjetoEscena(nuevaPlantillaAnimacion);
										//Adjust transformations
										if(nuevaBiblioteca->scaleBase2 != scaleBase2ToLoad){
											const float scaleToApply = (float)scaleBase2ToLoad / (float)nuevaBiblioteca->scaleBase2;
											PRINTF_INFO("ANIM, aplying scale (%f) to anim-binary.\n", scaleToApply);
											//Scale translations
											{
												SI32 i; const SI32 count = nuevaBiblioteca->traslaciones.conteo;
												for(i = 0; i < count; i++){
													NBPunto* v = nuevaBiblioteca->traslaciones.elemPtr(i);
													v->x *= scaleToApply;
													v->y *= scaleToApply;
												}
											}
											//Scale scales
											{
												SI32 i; const SI32 count = nuevaBiblioteca->escalaciones.conteo;
												for(i = 0; i < count; i++){
													NBTamano* v = nuevaBiblioteca->escalaciones.elemPtr(i);
													v->ancho *= scaleToApply;
													v->alto *= scaleToApply;
												}
											}
											//
											nuevaBiblioteca->scaleBase2 = scaleBase2ToLoad;
										}
										_plantillasAnimaciones->agregarElemento(nuevaBiblioteca);
										animacion = (AUEscenaAnimacion*)nuevaPlantillaAnimacion->clonar(tipoMemoriaResidencia, ENClonadoTipo_CopiarDirecciones); NB_DEFINE_NOMBRE_PUNTERO(animacion, "NBGestorAnimaciones::animacion(copia_bin)");
									}
								}
							}
						}
					}
				}
			}
		}
		nuevaBiblioteca->liberar(NB_RETENEDOR_NULL);
		archivoCache->unlock();
		archivoCache->cerrar();
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return animacion;
}

AUEscenaAnimacion* NBGestorAnimaciones::privSubanimacionDesdeNodoXml(const AUDatosXML* datosXml, const sNodoXML* nodoXml, AUAnimacionBibliotecaDef* plantillaBiblio, AUArregloNativoMutableP<NBPunto>* bibTraslaciones, AUArregloNativoMutableP<NBTamano>* bibEscalaciones, AUArregloNativoMutableP<float>* bibRotaciones, AUArregloNativoMutableP<NBColor8>* bibColores, ENAnimacion_TipoEncapsulamiento tipoEncapsulamiento){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorAnimaciones::privSubanimacionDesdeNodoXml")
	AUEscenaAnimacion* nuevaAnimacion = NULL;
	AUCadenaMutable8* strTipoElem = new(ENMemoriaTipo_Temporal) AUCadenaMutable8(); NB_DEFINE_NOMBRE_PUNTERO(strTipoElem, "NBGestorAnimaciones::strTipoElem");
	//SI32 versionXML = datosXml->nodoHijo("vXML", 0, nodoXml);
	//NBASSERT(versionXML==1)
	//if(versionXML==1){
	NBCajaAABB cajaLienzoAnimacion; NBCAJAAABB_INICIALIZAR(cajaLienzoAnimacion);
	const sNodoXML* nodoCajaLienzo = datosXml->nodoHijo("cajaLienzo", nodoXml);
	if(nodoCajaLienzo != NULL){
		if(nodoCajaLienzo->nodosHijos == NULL){
			//Formato nuevo CSV (mas compacto)
			const char* cadenaCSV		= datosXml->cadenaValorDeNodo(nodoCajaLienzo);
			SI32 tamCadenaCSV			= AUCadena8::tamano(cadenaCSV);
			SI32 posEnCadenaCSV			= 0;
			cajaLienzoAnimacion.xMin	= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0.0f);
			cajaLienzoAnimacion.xMax	= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0.0f);
			cajaLienzoAnimacion.yMin	= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0.0f);
			cajaLienzoAnimacion.yMax	= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0.0f);
		} else {
			//Formato viejo XML (muy inflado)
			cajaLienzoAnimacion.xMin	= datosXml->nodoHijo("xMin", 0.0f, nodoCajaLienzo);
			cajaLienzoAnimacion.xMax	= datosXml->nodoHijo("xMax", 0.0f, nodoCajaLienzo);
			cajaLienzoAnimacion.yMin	= datosXml->nodoHijo("yMin", 0.0f, nodoCajaLienzo);
			cajaLienzoAnimacion.yMax	= datosXml->nodoHijo("yMax", 0.0f, nodoCajaLienzo);
		}
	}
	float fps		= datosXml->nodoHijo("fps", 1.0f, nodoXml); NBASSERT(fps>0.0f)
	SI32 frames		= datosXml->nodoHijo("frames", 0, nodoXml); NBASSERT(frames>0)
	plantillaBiblio->fps			= fps;
	plantillaBiblio->tipoEncapsulamientoHijos = tipoEncapsulamiento;
	AUAnimacionDef* plantillaAnim = new(ENMemoriaTipo_General) AUAnimacionDef(frames, cajaLienzoAnimacion); NB_DEFINE_NOMBRE_PUNTERO(plantillaAnim, "NBGestorAnimaciones::plantillaAnim(NodoXml)");
	SI32 iFrame; for(iFrame=0; iFrame<frames; iFrame++) plantillaAnim->framesConEventoSonido.agregarElemento(false);
	nuevaAnimacion 	= new(ENMemoriaTipo_General) AUEscenaAnimacion(plantillaBiblio, plantillaAnim); NB_DEFINE_NOMBRE_PUNTERO(nuevaAnimacion, "NBGestorAnimaciones::nuevaAnimacion(NodoXml)");
	//ELEMENTOS VISUALES
	AUArregloMutable* capasCargadas					= new(ENMemoriaTipo_Temporal) AUArregloMutable();					NB_DEFINE_NOMBRE_PUNTERO(capasCargadas, "AUEditorNiveles::capasCargadas")
	AUArregloNativoMutableP<SI32>* idCapasCargadas	= new(ENMemoriaTipo_Temporal) AUArregloNativoMutableP<SI32>(1);		NB_DEFINE_NOMBRE_PUNTERO(idCapasCargadas, "AUEditorNiveles::idCapasCargadas")
	AUArregloMutable* objetosCargadosPorCapa		= new(ENMemoriaTipo_Temporal) AUArregloMutable();					NB_DEFINE_NOMBRE_PUNTERO(objetosCargadosPorCapa, "AUEditorNiveles::objetosCargadosPorCapa")
	AUArregloMutable* idsObjetosCargadosPorCapa		= new(ENMemoriaTipo_Temporal) AUArregloMutable();					NB_DEFINE_NOMBRE_PUNTERO(idsObjetosCargadosPorCapa, "AUEditorNiveles::idsObjetosCargadosPorCapa")
	AUArregloNativoMutableP<STAnimacionObjEscenaPrecargadoXML>* panelesSolPrecargados	= new(ENMemoriaTipo_Temporal) AUArregloNativoMutableP<STAnimacionObjEscenaPrecargadoXML>();	NB_DEFINE_NOMBRE_PUNTERO(panelesSolPrecargados, "AUEditorNiveles::panelesSolPrecargados")
	AUArregloNativoMutableP<STAnimacionObjEscenaPrecargadoXML>* bateriasPrecargados		= new(ENMemoriaTipo_Temporal) AUArregloNativoMutableP<STAnimacionObjEscenaPrecargadoXML>();	NB_DEFINE_NOMBRE_PUNTERO(bateriasPrecargados, "AUEditorNiveles::bateriasPrecargados")
	AUArregloNativoMutableP<STAnimacionObjEscenaPrecargadoXML>* bombillosPrecargados	= new(ENMemoriaTipo_Temporal) AUArregloNativoMutableP<STAnimacionObjEscenaPrecargadoXML>();	NB_DEFINE_NOMBRE_PUNTERO(bombillosPrecargados, "AUEditorNiveles::bombillosPrecargados")
	AUArregloNativoMutableP<STAnimacionObjEscenaPrecargadoXML>* lasersPrecargados		= new(ENMemoriaTipo_Temporal) AUArregloNativoMutableP<STAnimacionObjEscenaPrecargadoXML>();	NB_DEFINE_NOMBRE_PUNTERO(lasersPrecargados, "AUEditorNiveles::lasersPrecargados")
	AUArregloNativoMutableP<STAnimacionObjEscenaPrecargadoXML>* electornodosPrecargados = new(ENMemoriaTipo_Temporal) AUArregloNativoMutableP<STAnimacionObjEscenaPrecargadoXML>();	NB_DEFINE_NOMBRE_PUNTERO(electornodosPrecargados, "AUEditorNiveles::electornodosPrecargados")
	capasCargadas->agregarElemento(nuevaAnimacion);
	idCapasCargadas->agregarElemento(0);
	//
	AUArregloNativoMutableP<AUEscenaObjeto*>* objetosCargados = new(ENMemoriaTipo_Temporal) AUArregloNativoMutableP<AUEscenaObjeto*>(); NB_DEFINE_NOMBRE_PUNTERO(objetosCargados, "AUEditorCuerpos::objetosCargados")
	AUArregloNativoMutableP<SI32>* idsObjetosCargados = new(ENMemoriaTipo_Temporal) AUArregloNativoMutableP<SI32>(); NB_DEFINE_NOMBRE_PUNTERO(idsObjetosCargados, "AUEditorCuerpos::idsObjetosCargados")
	objetosCargadosPorCapa->agregarElemento(objetosCargados); objetosCargados->liberar(NB_RETENEDOR_NULL);
	idsObjetosCargadosPorCapa->agregarElemento(idsObjetosCargados); idsObjetosCargados->liberar(NB_RETENEDOR_NULL);
	//
	const sNodoXML* elems	= datosXml->nodoHijo("elementos", nodoXml);
	NBASSERT(elems != NULL)
	if(elems != NULL){
		const sNodoXML* elem; XML_NODO_HIJO(datosXml, elem, "e", "elemento", elems, NULL)
		while(elem != NULL){
			SI32 indiceElem = -1;
			SI32 indicePrimerFrameAparece; XML_VALOR_NODO_HIJO(datosXml, indicePrimerFrameAparece, "iPF", "iPrimerFrame", (SI32)0, elem, NULL)
			strTipoElem->vaciar(); XML_VALOR_STR_NODO_HIJO(datosXml, strTipoElem, "tp", "tipo", "", elem, NULL)
			const sNodoXML* nDatos; XML_NODO_HIJO(datosXml, nDatos, "dt", "datos", elem, NULL)
			if(nDatos != NULL){
				if(strTipoElem->esIgual("AUEscenaSprite")){
					AUEscenaSprite* sprite = new(nuevaAnimacion) AUEscenaSprite(); NB_DEFINE_NOMBRE_PUNTERO(nuevaAnimacion, "NBGestorAnimaciones::spriteXML");
					if(!AUEscenaSprite::interpretarXmlInterno(sprite, datosXml, nDatos)){
						PRINTF_ERROR("AUEscenaAnimacion, intentando interpretar XML interno de sprite\n");
					} else {
						if(tipoEncapsulamiento==ENAnimacion_TipoEncapsulamiento_UnContenedorPorHijo){
							AUEscenaContenedor* encapsulador = new(nuevaAnimacion) AUEscenaContenedor(); NB_DEFINE_NOMBRE_PUNTERO(encapsulador, "NBGestorAnimaciones::encapsuladorXML");
							encapsulador->agregarObjetoEscena(sprite);
							nuevaAnimacion->agregarObjetoEscena(encapsulador);
							encapsulador->liberar(NB_RETENEDOR_NULL);
						} else {
							nuevaAnimacion->agregarObjetoEscena(sprite);
						}
						indiceElem = nuevaAnimacion->hijos()->conteo - 1;
						objetosCargados->agregarElemento(sprite);
						idsObjetosCargados->agregarElemento(indiceElem);
					}
					sprite->liberar(NB_RETENEDOR_NULL);
				} else if(strTipoElem->esIgual("AUEscenaGeneradorParticulas")){
					AUEscenaGeneradorParticulas* genParticulas = new(nuevaAnimacion) AUEscenaGeneradorParticulas(); NB_DEFINE_NOMBRE_PUNTERO(genParticulas, "NBGestorAnimaciones::genParticulas");
					if(!AUEscenaGeneradorParticulas::interpretarXmlInterno(genParticulas, datosXml, nDatos)){
						PRINTF_ERROR("AUEscenaAnimacion, intentando interpretar XML interno de AUEscenaGeneradorParticulas\n");
					} else {
						if(tipoEncapsulamiento==ENAnimacion_TipoEncapsulamiento_UnContenedorPorHijo){
							AUEscenaContenedor* encapsulador = new(nuevaAnimacion) AUEscenaContenedor(); NB_DEFINE_NOMBRE_PUNTERO(encapsulador, "NBGestorAnimaciones::encapsuladorXML");
							encapsulador->agregarObjetoEscena(genParticulas);
							nuevaAnimacion->agregarObjetoEscena(encapsulador);
							encapsulador->liberar(NB_RETENEDOR_NULL);
						} else {
							nuevaAnimacion->agregarObjetoEscena(genParticulas);
						}
						indiceElem = nuevaAnimacion->hijos()->conteo - 1;
						objetosCargados->agregarElemento(genParticulas);
						idsObjetosCargados->agregarElemento(indiceElem);
					}
					genParticulas->liberar(NB_RETENEDOR_NULL);
				} else if(strTipoElem->esIgual("AUEscenaCuerpo")){
					AUCadenaMutable8* rutaSubcuerpo = new(ENMemoriaTipo_Temporal) AUCadenaMutable8();  NB_DEFINE_NOMBRE_PUNTERO(rutaSubcuerpo, "NBGestorAnimaciones::rutaSubcuerpo");
					datosXml->nodoHijo("rutaArchivo", rutaSubcuerpo, "", nDatos);
					AUEscenaCuerpoI* cuerpo = (AUEscenaCuerpoI*)_cargadorCuerpos->cargarCuerpo(ENMemoriaTipo_General, rutaSubcuerpo->str()); //luego s ele definira su agrupador de sombras
					if(cuerpo != NULL){
						cuerpo->establecerCreacionCuerpoTipo(ENCuerpoTipo_Kinematico, ENAnimFisicaTipo_SeguirObjetoEscena);
						if(tipoEncapsulamiento==ENAnimacion_TipoEncapsulamiento_UnContenedorPorHijo){
							AUEscenaContenedor* encapsulador = new(nuevaAnimacion) AUEscenaContenedor(); NB_DEFINE_NOMBRE_PUNTERO(encapsulador, "NBGestorAnimaciones::encapsuladorXML");
							encapsulador->agregarObjetoEscena(cuerpo);
							nuevaAnimacion->agregarObjetoEscena(encapsulador);
							encapsulador->liberar(NB_RETENEDOR_NULL);
						} else {
							nuevaAnimacion->agregarObjetoEscena(cuerpo);
						}
						indiceElem = nuevaAnimacion->hijos()->conteo - 1;
						objetosCargados->agregarElemento(cuerpo);
						idsObjetosCargados->agregarElemento(indiceElem);
					}
					rutaSubcuerpo->liberar(NB_RETENEDOR_NULL);
				} else if(strTipoElem->esIgual("AUEscenaAnimacion")){
					AUCadenaMutable8* rutaSubanimacion = new(ENMemoriaTipo_Temporal) AUCadenaMutable8();  NB_DEFINE_NOMBRE_PUNTERO(rutaSubanimacion, "NBGestorAnimaciones::rutaSubanimacion");
					datosXml->nodoHijo("rutaArchivo", rutaSubanimacion, "", nDatos);
					AUEscenaAnimacion* subAnimacion = NULL;
					if(rutaSubanimacion->tamano()>0){
						subAnimacion = NBGestorAnimaciones::animacionDesdeArchivo(ENMemoriaTipo_General, rutaSubanimacion->str()); //luego se le asignara el agrupador de sombras
						if(subAnimacion == NULL){
							PRINTF_ERROR("NBGestorAnimaciones, intentando cargar subanimacion desde referencia\n");
						} else {
							NB_DEFINE_NOMBRE_PUNTERO(subAnimacion, "NBGestorAnimaciones::subAnimacionXMLRef");
						}
					} else {
						subAnimacion = NBGestorAnimaciones::privSubanimacionDesdeNodoXml(datosXml, nDatos, plantillaBiblio, bibTraslaciones, bibEscalaciones, bibRotaciones, bibColores, tipoEncapsulamiento);
						if(subAnimacion == NULL){
							PRINTF_ERROR("NBGestorAnimaciones, intentando interpretar XML interno de subanimacion\n");
						} else {
							NB_DEFINE_NOMBRE_PUNTERO(subAnimacion, "NBGestorAnimaciones::subAnimacionXML");
						}
					}
					if(subAnimacion != NULL){
						subAnimacion->establecerRepetirAnimacion(true);
						if(tipoEncapsulamiento==ENAnimacion_TipoEncapsulamiento_UnContenedorPorHijo){
							AUEscenaContenedor* encapsulador = new(nuevaAnimacion) AUEscenaContenedor(); NB_DEFINE_NOMBRE_PUNTERO(encapsulador, "NBGestorAnimaciones::encapsuladorXML");
							encapsulador->agregarObjetoEscena(subAnimacion);
							nuevaAnimacion->agregarObjetoEscena(encapsulador);
							encapsulador->liberar(NB_RETENEDOR_NULL);
						} else {
							nuevaAnimacion->agregarObjetoEscena(subAnimacion);
						}
						indiceElem = nuevaAnimacion->hijos()->conteo - 1;
						objetosCargados->agregarElemento(subAnimacion);
						idsObjetosCargados->agregarElemento(indiceElem);
					}
					rutaSubanimacion->liberar(NB_RETENEDOR_NULL);
				} else if(strTipoElem->esIgual("AUBateria")){
					AUBateria* bateria = new(nuevaAnimacion) AUBateria(); NB_DEFINE_NOMBRE_PUNTERO(bateria, "AUEditorNiveles::bateria")
					if(tipoEncapsulamiento==ENAnimacion_TipoEncapsulamiento_UnContenedorPorHijo){
						AUEscenaContenedor* encapsulador = new(nuevaAnimacion) AUEscenaContenedor(); NB_DEFINE_NOMBRE_PUNTERO(encapsulador, "NBGestorAnimaciones::encapsuladorXML");
						encapsulador->agregarObjetoEscena(bateria);
						nuevaAnimacion->agregarObjetoEscena(encapsulador);
						encapsulador->liberar(NB_RETENEDOR_NULL);
					} else {
						nuevaAnimacion->agregarObjetoEscena(bateria);
					}
					indiceElem = nuevaAnimacion->hijos()->conteo - 1;
					objetosCargados->agregarElemento(bateria);
					idsObjetosCargados->agregarElemento(indiceElem);
					bateria->liberar(NB_RETENEDOR_NULL);
					//
					STAnimacionObjEscenaPrecargadoXML datosPrecagado;
					datosPrecagado.objPrecargado	= bateria;
					datosPrecagado.nodoDatosXML		= nDatos;
					bateriasPrecargados->agregarElemento(datosPrecagado);
				} else if(strTipoElem->esIgual("AUBombillo")){
					AUBombillo* bombillo = new(nuevaAnimacion) AUBombillo(); NB_DEFINE_NOMBRE_PUNTERO(bombillo, "AUEditorNiveles::bombillo")
					if(tipoEncapsulamiento==ENAnimacion_TipoEncapsulamiento_UnContenedorPorHijo){
						AUEscenaContenedor* encapsulador = new(nuevaAnimacion) AUEscenaContenedor(); NB_DEFINE_NOMBRE_PUNTERO(encapsulador, "NBGestorAnimaciones::encapsuladorXML");
						encapsulador->agregarObjetoEscena(bombillo);
						nuevaAnimacion->agregarObjetoEscena(encapsulador);
						encapsulador->liberar(NB_RETENEDOR_NULL);
					} else {
						nuevaAnimacion->agregarObjetoEscena(bombillo);
					}
					indiceElem = nuevaAnimacion->hijos()->conteo - 1;
					objetosCargados->agregarElemento(bombillo);
					idsObjetosCargados->agregarElemento(indiceElem);
					bombillo->liberar(NB_RETENEDOR_NULL);
					//
					STAnimacionObjEscenaPrecargadoXML datosPrecagado;
					datosPrecagado.objPrecargado	= bombillo;
					datosPrecagado.nodoDatosXML		= nDatos;
					bombillosPrecargados->agregarElemento(datosPrecagado);
				} else if(strTipoElem->esIgual("AULaser")){
					AULaser* laser = new(nuevaAnimacion) AULaser(); NB_DEFINE_NOMBRE_PUNTERO(laser, "AUEditorNiveles::laser")
					if(tipoEncapsulamiento==ENAnimacion_TipoEncapsulamiento_UnContenedorPorHijo){
						AUEscenaContenedor* encapsulador = new(nuevaAnimacion) AUEscenaContenedor(); NB_DEFINE_NOMBRE_PUNTERO(encapsulador, "NBGestorAnimaciones::encapsuladorXML");
						encapsulador->agregarObjetoEscena(laser);
						nuevaAnimacion->agregarObjetoEscena(encapsulador);
						encapsulador->liberar(NB_RETENEDOR_NULL);
					} else {
						nuevaAnimacion->agregarObjetoEscena(laser);
					}
					indiceElem = nuevaAnimacion->hijos()->conteo - 1;
					objetosCargados->agregarElemento(laser);
					idsObjetosCargados->agregarElemento(indiceElem);
					laser->liberar(NB_RETENEDOR_NULL);
					//
					STAnimacionObjEscenaPrecargadoXML datosPrecagado;
					datosPrecagado.objPrecargado	= laser;
					datosPrecagado.nodoDatosXML		= nDatos;
					lasersPrecargados->agregarElemento(datosPrecagado);
				} else if(strTipoElem->esIgual("AUPanelSolar")){
					AUPanelSolar* panelSolar = new(nuevaAnimacion) AUPanelSolar(); NB_DEFINE_NOMBRE_PUNTERO(panelSolar, "AUEditorNiveles::panelSolar")
					if(tipoEncapsulamiento==ENAnimacion_TipoEncapsulamiento_UnContenedorPorHijo){
						AUEscenaContenedor* encapsulador = new(nuevaAnimacion) AUEscenaContenedor(); NB_DEFINE_NOMBRE_PUNTERO(encapsulador, "NBGestorAnimaciones::encapsuladorXML");
						encapsulador->agregarObjetoEscena(panelSolar);
						nuevaAnimacion->agregarObjetoEscena(encapsulador);
						encapsulador->liberar(NB_RETENEDOR_NULL);
					} else {
						nuevaAnimacion->agregarObjetoEscena(panelSolar);
					}
					indiceElem = nuevaAnimacion->hijos()->conteo - 1;
					objetosCargados->agregarElemento(panelSolar);
					idsObjetosCargados->agregarElemento(indiceElem);
					panelSolar->liberar(NB_RETENEDOR_NULL);
					//
					STAnimacionObjEscenaPrecargadoXML datosPrecagado;
					datosPrecagado.objPrecargado	= panelSolar;
					datosPrecagado.nodoDatosXML		= nDatos;
					panelesSolPrecargados->agregarElemento(datosPrecagado);
				} else if(strTipoElem->esIgual("AUElectronodo")){
					AUElectronodo* electronodo = new(nuevaAnimacion) AUElectronodo(); NB_DEFINE_NOMBRE_PUNTERO(electronodo, "AUEditorNiveles::electronodo")
					if(tipoEncapsulamiento==ENAnimacion_TipoEncapsulamiento_UnContenedorPorHijo){
						AUEscenaContenedor* encapsulador = new(nuevaAnimacion) AUEscenaContenedor(); NB_DEFINE_NOMBRE_PUNTERO(encapsulador, "NBGestorAnimaciones::encapsuladorXML");
						encapsulador->agregarObjetoEscena(electronodo);
						nuevaAnimacion->agregarObjetoEscena(encapsulador);
						encapsulador->liberar(NB_RETENEDOR_NULL);
					} else {
						nuevaAnimacion->agregarObjetoEscena(electronodo);
					}
					indiceElem = nuevaAnimacion->hijos()->conteo - 1;
					objetosCargados->agregarElemento(electronodo);
					idsObjetosCargados->agregarElemento(indiceElem);
					electronodo->liberar(NB_RETENEDOR_NULL);
					//
					STAnimacionObjEscenaPrecargadoXML datosPrecagado;
					datosPrecagado.objPrecargado	= electronodo;
					datosPrecagado.nodoDatosXML		= nDatos;
					electornodosPrecargados->agregarElemento(datosPrecagado);
				} else if(strTipoElem->esIgual("AULanzallamas")){
					AULanzallamas* lanzallamas = new(nuevaAnimacion) AULanzallamas(); NB_DEFINE_NOMBRE_PUNTERO(lanzallamas, "AUEditorNiveles::lanzallamas")
					if(AULanzallamas::interpretarXmlInterno(lanzallamas, datosXml, nDatos)){
						if(tipoEncapsulamiento==ENAnimacion_TipoEncapsulamiento_UnContenedorPorHijo){
							AUEscenaContenedor* encapsulador = new(nuevaAnimacion) AUEscenaContenedor(); NB_DEFINE_NOMBRE_PUNTERO(encapsulador, "NBGestorAnimaciones::encapsuladorXML");
							encapsulador->agregarObjetoEscena(lanzallamas);
							nuevaAnimacion->agregarObjetoEscena(encapsulador);
							encapsulador->liberar(NB_RETENEDOR_NULL);
						} else {
							nuevaAnimacion->agregarObjetoEscena(lanzallamas);
						}
						indiceElem = nuevaAnimacion->hijos()->conteo - 1;
						objetosCargados->agregarElemento(lanzallamas);
						idsObjetosCargados->agregarElemento(indiceElem);
					}
					lanzallamas->liberar(NB_RETENEDOR_NULL);
				}
				NBASSERT(indiceElem!=-1)
				//Interpretar los indices de las propiedades
				const sNodoXML* nTransPorFrame = datosXml->nodoHijo("transformacionesPorFrame", elem); //compatibilidad con formato antiguo
				const sNodoXML* nTraslPorFrame; XML_NODO_HIJO(datosXml, nTraslPorFrame, "trasPF", "traslacionesPorFrame", elem, NULL)
				const sNodoXML* nEscalPorFrame; XML_NODO_HIJO(datosXml, nEscalPorFrame, "escPF", "escalacionesPorFrame", elem, NULL)
				const sNodoXML* nRotacPorFrame; XML_NODO_HIJO(datosXml, nRotacPorFrame, "rotPF", "rotacionesPorFrame", elem, NULL)
				const sNodoXML* nColorPorFrame; XML_NODO_HIJO(datosXml, nColorPorFrame, "colPF", "coloresPorFrame", elem, NULL)
				NBASSERT((nTransPorFrame != NULL || (nTraslPorFrame != NULL && nEscalPorFrame != NULL && nRotacPorFrame != NULL)) && nColorPorFrame != NULL)
				if((nTransPorFrame != NULL || (nTraslPorFrame != NULL && nEscalPorFrame != NULL && nRotacPorFrame != NULL)) && nColorPorFrame != NULL){
					const sNodoXML* nodoIndiceTransf	= NULL; //compatibilidad con formato antiguo
					AUCadenaLargaMutable8* strListaTras = new(ENMemoriaTipo_Temporal) AUCadenaLargaMutable8();	XML_VALOR_STR_NODO_HIJO(datosXml, strListaTras, "l", "lista", "", nTraslPorFrame, NULL)
					AUCadenaLargaMutable8* strListaEscal = new(ENMemoriaTipo_Temporal) AUCadenaLargaMutable8();	XML_VALOR_STR_NODO_HIJO(datosXml, strListaEscal, "l", "lista", "", nEscalPorFrame, NULL)
					AUCadenaLargaMutable8* strListaRotac = new(ENMemoriaTipo_Temporal) AUCadenaLargaMutable8();	XML_VALOR_STR_NODO_HIJO(datosXml, strListaRotac, "l", "lista", "", nRotacPorFrame, NULL)
					AUCadenaLargaMutable8* strListaColor = new(ENMemoriaTipo_Temporal) AUCadenaLargaMutable8();	XML_VALOR_STR_NODO_HIJO(datosXml, strListaColor, "l", "lista", "", nColorPorFrame, NULL)
					AUCadenaMutable8* tmpStructura = new(ENMemoriaTipo_Temporal) AUCadenaMutable8();
					SI32 posSeparadorTraslado			= -1;
					SI32 posSeparadorEscala				= -1;
					SI32 posSeparadorRotacion			= -1;
					SI32 posSeparadorColor				= -1;
					const sNodoXML* nodoIndiceColor		= NULL;
					SI32 indiceLocalTransf				= -1; //compatibilidad con el formato viejo
					SI32 indiceLocalColor				= -1;
					SI32 indiceGlobalTrasl				= -1;
					SI32 indiceGlobalEscal				= -1;
					SI32 indiceGlobalRotac				= -1;
					SI32 indiceGlobalColor				= -1;
					SI32 indiceFrame					= 0;
					bool seguirBuscandoValoresTranf		= true; //compatibilidad con formato antiguo
					bool seguirBuscandoValoresTrasl		= true;
					bool seguirBuscandoValoresEscal		= true;
					bool seguirBuscandoValoresRotac		= true;
					bool seguirBuscandoValoresColor		= true;
					do {
						if(indiceFrame<indicePrimerFrameAparece){
							indiceLocalTransf			= -1;
							indiceLocalColor			= -1;
							indiceGlobalTrasl			= -1;
							indiceGlobalEscal			= -1;
							indiceGlobalRotac			= -1;
							indiceGlobalColor			= -1;
						} else {
							if(nTraslPorFrame != NULL && nEscalPorFrame != NULL && nRotacPorFrame != NULL){
								if(seguirBuscandoValoresTrasl){
									SI32 posNuevoSeparador			= strListaTras->indiceDe(",", posSeparadorTraslado+1); if(posNuevoSeparador==-1) posNuevoSeparador = strListaTras->tamano();
									tmpStructura->vaciar();			tmpStructura->agregar(&strListaTras->str()[posSeparadorTraslado+1], posNuevoSeparador-posSeparadorTraslado-1); NBASSERT(AUNumericoSI328::esEntero(tmpStructura->str()))
									posSeparadorTraslado			= posNuevoSeparador;
									SI32 indiceLocalTrasl			= AUNumericoP<SI32, char>::aEntero(tmpStructura->str());
									indiceGlobalTrasl				= (indiceLocalTrasl<0?-1:indiceLocalTrasl);
									seguirBuscandoValoresTrasl		= (posSeparadorTraslado<(SI32)strListaTras->tamano());
								}
								if(seguirBuscandoValoresEscal){
									SI32 posNuevoSeparador			= strListaEscal->indiceDe(",", posSeparadorEscala+1); if(posNuevoSeparador==-1) posNuevoSeparador = strListaEscal->tamano();
									tmpStructura->vaciar();			tmpStructura->agregar(&strListaEscal->str()[posSeparadorEscala+1], posNuevoSeparador-posSeparadorEscala-1); NBASSERT(AUNumericoSI328::esEntero(tmpStructura->str()))
									posSeparadorEscala				= posNuevoSeparador;
									SI32 indiceLocalEscal			= AUNumericoP<SI32, char>::aEntero(tmpStructura->str());
									indiceGlobalEscal				= (indiceLocalEscal<0?-1:indiceLocalEscal);
									seguirBuscandoValoresEscal		= (posSeparadorEscala<(SI32)strListaEscal->tamano());
								}
								if(seguirBuscandoValoresRotac){
									SI32 posNuevoSeparador			= strListaRotac->indiceDe(",", posSeparadorRotacion+1); if(posNuevoSeparador==-1) posNuevoSeparador = strListaRotac->tamano();
									tmpStructura->vaciar();			tmpStructura->agregar(&strListaRotac->str()[posSeparadorRotacion+1], posNuevoSeparador-posSeparadorRotacion-1); NBASSERT(AUNumericoSI328::esEntero(tmpStructura->str()))
									posSeparadorRotacion			= posNuevoSeparador;
									SI32 indiceLocalRotac			= AUNumericoP<SI32, char>::aEntero(tmpStructura->str());
									indiceGlobalRotac				= (indiceLocalRotac<0?-1:indiceLocalRotac);
									seguirBuscandoValoresRotac		= (posSeparadorRotacion<(SI32)strListaRotac->tamano());
								}
							} else { //Compatibilidad con el formato viejo
								if(seguirBuscandoValoresTranf){
									indiceLocalTransf				= datosXml->nodoHijo("t", indiceLocalTransf, nTransPorFrame, nodoIndiceTransf);
									if(indiceLocalTransf<0){
										indiceGlobalTrasl			= -1;
										indiceGlobalEscal			= -1;
										indiceGlobalRotac			= -1;
									} else {
										indiceGlobalTrasl			= indiceLocalTransf;
										indiceGlobalEscal			= indiceLocalTransf;
										indiceGlobalRotac			= indiceLocalTransf;
									}
									const sNodoXML* nuevoNodoTransf	= datosXml->nodoHijo("t", nTransPorFrame, nodoIndiceTransf);
									if(nuevoNodoTransf != NULL)		nodoIndiceTransf	= nuevoNodoTransf;
									else							seguirBuscandoValoresTranf = false;
								}
							}
							if(seguirBuscandoValoresColor){
								if(strListaColor->tamano() != 0){
									SI32 posNuevoSeparador			= strListaColor->indiceDe(",", posSeparadorColor+1); if(posNuevoSeparador==-1) posNuevoSeparador = strListaColor->tamano();
									tmpStructura->vaciar();			tmpStructura->agregar(&strListaColor->str()[posSeparadorColor+1], posNuevoSeparador-posSeparadorColor-1); NBASSERT(AUNumericoSI328::esEntero(tmpStructura->str()))
									posSeparadorColor				= posNuevoSeparador;
									SI32 indiceLocalColor			= AUNumericoP<SI32, char>::aEntero(tmpStructura->str());
									indiceGlobalColor				= (indiceLocalColor<0?-1:indiceLocalColor);
									seguirBuscandoValoresColor		= (posSeparadorColor<(SI32)strListaColor->tamano());
								} else { //Compatibilidad con formato medio
									indiceLocalColor				= datosXml->nodoHijo("c", indiceLocalColor, nColorPorFrame, nodoIndiceColor);
									if(indiceLocalColor<0){
										indiceGlobalColor			= -1;
									} else {
										indiceGlobalColor			= indiceLocalColor;
									}
									const sNodoXML* nuevoNodoColor	= datosXml->nodoHijo("c", nColorPorFrame, nodoIndiceColor);
									if(nuevoNodoColor != NULL)		nodoIndiceColor		= nuevoNodoColor;
									else							seguirBuscandoValoresColor = false;
								}
							}
						}
						if(indiceElem!=-1){
							plantillaAnim->establecerIndicePropiedadesEnFrame(indiceFrame, indiceElem, indiceGlobalTrasl, indiceGlobalEscal, indiceGlobalRotac, indiceGlobalColor);
						}
						indiceFrame++;
					} while(indiceFrame<frames);
					strListaTras->liberar(NB_RETENEDOR_NULL);
					strListaEscal->liberar(NB_RETENEDOR_NULL);
					strListaRotac->liberar(NB_RETENEDOR_NULL);
					strListaColor->liberar(NB_RETENEDOR_NULL);
					tmpStructura->liberar(NB_RETENEDOR_NULL);
				}
			}
			XML_NODO_HIJO(datosXml, elem, "e", "elemento", elems, elem)
		}
	}
	//Cargar paneles precargados (se deben cargar al final debido a que hacen posible referencia a objetos que estan delante de el en la lista de "Elementos")
	SI32 iPanelSol, iPanelSolConteo = panelesSolPrecargados->conteo;
	for(iPanelSol=0; iPanelSol<iPanelSolConteo; iPanelSol++){
		STAnimacionObjEscenaPrecargadoXML* datosPrecargado = &(panelesSolPrecargados->elemento[iPanelSol]);
		AUPanelSolar* panelSolar = (AUPanelSolar*)datosPrecargado->objPrecargado;
		if(!AUPanelSolar::interpretarXmlInterno(panelSolar, datosXml, datosPrecargado->nodoDatosXML, objetosCargados, idsObjetosCargados)){
			NBASSERT(false)
		}
	}
	//Cargar baterias precargados (se deben cargar al final debido a que hacen posible referencia a objetos que estan delante de el en la lista de "Elementos")
	SI32 iBateria, iBateriaConteo = bateriasPrecargados->conteo;
	for(iBateria=0; iBateria<iBateriaConteo; iBateria++){
		STAnimacionObjEscenaPrecargadoXML* datosPrecargado = &(bateriasPrecargados->elemento[iBateria]);
		AUBateria* bateria = (AUBateria*)datosPrecargado->objPrecargado;
		if(!AUBateria::interpretarXmlInterno(bateria, datosXml, datosPrecargado->nodoDatosXML, objetosCargados, idsObjetosCargados)){
			NBASSERT(false)
		}
	}
	//Cargar bombillos precargados (se deben cargar al final debido a que hacen posible referencia a objetos que estan delante de el en la lista de "Elementos")
	SI32 iBombillo, iBombilloConteo = bombillosPrecargados->conteo;
	for(iBombillo=0; iBombillo<iBombilloConteo; iBombillo++){
		STAnimacionObjEscenaPrecargadoXML* datosPrecargado = &(bombillosPrecargados->elemento[iBombillo]);
		AUBombillo* bombillo = (AUBombillo*)datosPrecargado->objPrecargado;
		if(!AUBombillo::interpretarXmlInterno(bombillo, datosXml, datosPrecargado->nodoDatosXML, objetosCargados, idsObjetosCargados)){
			NBASSERT(false)
		}
	}
	//Cargar bombillos precargados (se deben cargar al final debido a que hacen posible referencia a objetos que estan delante de el en la lista de "Elementos")
	SI32 iLaser, iLaserConteo = lasersPrecargados->conteo;
	for(iLaser=0; iLaser<iLaserConteo; iLaser++){
		STAnimacionObjEscenaPrecargadoXML* datosPrecargado = &(lasersPrecargados->elemento[iLaser]);
		AULaser* laser = (AULaser*)datosPrecargado->objPrecargado;
		if(!AULaser::interpretarXmlInterno(laser, datosXml, datosPrecargado->nodoDatosXML, objetosCargados, idsObjetosCargados)){
			NBASSERT(false)
		}
	}
	//Cargar electronodos precargados
	SI32 iElectro, iElectroConteo = electornodosPrecargados->conteo;
	for(iElectro=0; iElectro<iElectroConteo; iElectro++){
		STAnimacionObjEscenaPrecargadoXML* datosPrecagado = &(electornodosPrecargados->elemento[iElectro]);
		AUElectronodo* electronodo	= (AUElectronodo*)datosPrecagado->objPrecargado;
		if(!AUElectronodo::interpretarXmlInterno(electronodo, datosXml, datosPrecagado->nodoDatosXML, 0, idCapasCargadas, objetosCargadosPorCapa, idsObjetosCargadosPorCapa)){
			NBASSERT(false)
		}
	}
	panelesSolPrecargados->liberar(NB_RETENEDOR_NULL); panelesSolPrecargados = NULL;
	bateriasPrecargados->liberar(NB_RETENEDOR_NULL); bateriasPrecargados = NULL;
	lasersPrecargados->liberar(NB_RETENEDOR_NULL); lasersPrecargados = NULL;
	bombillosPrecargados->liberar(NB_RETENEDOR_NULL); bombillosPrecargados = NULL;
	electornodosPrecargados->liberar(NB_RETENEDOR_NULL); electornodosPrecargados = NULL;
	capasCargadas->liberar(NB_RETENEDOR_NULL);
	idCapasCargadas->liberar(NB_RETENEDOR_NULL);
	objetosCargadosPorCapa->liberar(NB_RETENEDOR_NULL);
	idsObjetosCargadosPorCapa->liberar(NB_RETENEDOR_NULL);
	//EVENTOS DE SONIDO
#	ifndef CONFIG_NB_UNSUPPORT_AUDIO_IO
	const sNodoXML* eventosSonido	= datosXml->nodoHijo("eventosSonido", nodoXml);
	if(eventosSonido != NULL){
		const sNodoXML* evSonido	= datosXml->nodoHijo("eventoSonido", eventosSonido);
		while(evSonido != NULL){
			AUCadenaMutable8* rutaSonido = new(ENMemoriaTipo_Temporal) AUCadenaMutable8(); NB_DEFINE_NOMBRE_PUNTERO(rutaSonido, "NBGestorAnimaciones::rutaSonido");
			SI32 iFrame				= datosXml->nodoHijo("iFrame", -1, evSonido);
			SI32 detener			= datosXml->nodoHijo("detener", -1, evSonido);
			SI32 noMultiple			= datosXml->nodoHijo("noMultiple", -1, evSonido);
			float segsInicio		= datosXml->nodoHijo("segsInicio", -1.0f, evSonido);
			float segsFin			= datosXml->nodoHijo("segsFin", -1.0f, evSonido);
			SI32 repetir			= datosXml->nodoHijo("repetir", 0, evSonido);
			SI32 repeticiones		= datosXml->nodoHijo("repeticiones", -1, evSonido);
			rutaSonido->vaciar();	  datosXml->nodoHijo("rutaSonido", rutaSonido, "", evSonido);
			if(iFrame<0 || rutaSonido->tamano() == 0){
				PRINTF_INFO("No se pudo registrar el evento de sonido de animacion, parametros no validos iFrame(%d) ruta('%s')\n", iFrame, rutaSonido->str());
			} else {
				//Determinar animador de sonido
				bool esApropiadoParaStream = true;

				AUAnimadorSonidoVolumen* animadorSonido = NULL;
				if(segsInicio>0.0f || segsFin>0.0f || (repetir != 0 && repeticiones>0)){
					if(animadorSonido == NULL) {
						animadorSonido = new(ENMemoriaTipo_General) AUAnimadorSonidoVolumen();  NB_DEFINE_NOMBRE_PUNTERO(animadorSonido, "NBGestorAnimaciones::animadorSonido");
					}
					animadorSonido->establecerSegundoInicio(segsInicio);
					animadorSonido->establecerSegundoFin(segsFin);
					animadorSonido->establecerRepeticiones(repeticiones);
					esApropiadoParaStream = (segsInicio<=0.0f && segsFin<0.0f); //Los stream no son apropiados para reproducir porciones
				}
				const sNodoXML* cambiosVolumen	= datosXml->nodoHijo("cambiosVolumen", evSonido);
				if(cambiosVolumen != NULL){
					const sNodoXML* cambioVol	= datosXml->nodoHijo("cambioVolumen", cambiosVolumen);
					while(cambioVol != NULL){
						if(animadorSonido == NULL){
							animadorSonido = new(ENMemoriaTipo_General) AUAnimadorSonidoVolumen(); NB_DEFINE_NOMBRE_PUNTERO(animadorSonido, "NBGestorAnimaciones::animadorSonido(XML)");
						}
						float segundo = datosXml->nodoHijo("seg", 0.0f, cambioVol);
						float volumen = datosXml->nodoHijo("vol", 1.0f, cambioVol);
						animadorSonido->agregarPuntoDeVolumen(segundo, volumen);
						//Siguiente canbio de volumen
						cambioVol =  datosXml->nodoHijo("cambioVolumen", cambiosVolumen, cambioVol);
					}
				}
				AUSonido* sonidoSinDatos = NBGestorSonidos::sonidoSinDatosDesdeArchivoWav(rutaSonido->str());
				if(sonidoSinDatos != NULL){
					UI16 bufferAL					= 0;
					AUSonidoStream* streamSonido	= NULL;
					STSonidoDescriptor propsSonido	= sonidoSinDatos->propiedades();
					float duracionSonido			= (float)propsSonido.bytesDatosPCM / (float)propsSonido.alineacionBloques / (float)propsSonido.muestrasPorSegundo;
					//Primero determinar stream
					if(duracionSonido>=2.0f){
						if(!esApropiadoParaStream){
							PRINTF_WARNING("el sonido de animacion es largo (%f segs) pero no es apropiado para stream por los parametros de su Animador\n", duracionSonido);
						} else {
							//PRINTF_INFO("Sonido de animacion sera cargado como stream (%f segs)\n", duracionSonido);
							streamSonido			= NBGestorSonidos::streamDesdeArchivo(rutaSonido->str());
						}
					}
					//Intentar cargar como buffer
					if(streamSonido == NULL){
						//PRINTF_INFO("Sonido de animacion sera cargado como buffer (%f segs)\n", duracionSonido);
						bufferAL					= NBGestorSonidos::bufferDesdeArchivoWav(rutaSonido->str());
					}
					//
					if(streamSonido == NULL && bufferAL == 0){
						PRINTF_ERROR("no se pudo cargar el bufferAL o streamSonido para el WAV: '%s'\n", rutaSonido->str());
					} else {
						STAnimacionEventoSonido eventoSonido;
						eventoSonido.iFrame			= iFrame;
						eventoSonido.bufferSonido	= bufferAL; if(bufferAL != 0) NBGestorSonidos::bufferRetener(bufferAL);
						eventoSonido.streamSonido	= streamSonido; if(streamSonido != NULL) streamSonido->retener(NB_RETENEDOR_NULL);
						eventoSonido.detener		= (detener != 0);
						eventoSonido.noMultiple		= (noMultiple != 0);
						eventoSonido.repetir		= (repetir != 0);
						eventoSonido.repeticiones	= repeticiones;
						eventoSonido.rutaSonido		= new(ENMemoriaTipo_General) AUCadena8(rutaSonido->str()); NB_DEFINE_NOMBRE_PUNTERO(eventoSonido.rutaSonido, "NBGestorAnimaciones::eventoSonido.rutaSonido(XML)");
						eventoSonido.animadorSonido	= animadorSonido;  if(animadorSonido != NULL) animadorSonido->retener(NB_RETENEDOR_NULL);
						//PRINTF_INFO("Animacion tiene animador de sonido: '%s'\n", eventoSonido.animadorSonido != NULL?"SI":"NO");
						plantillaAnim->eventosDeSonido.agregarElemento(eventoSonido);
						plantillaAnim->framesConEventoSonido.elemento[iFrame] = true;
					}
				}
				if(animadorSonido != NULL) animadorSonido->liberar(NB_RETENEDOR_NULL);
			}
			rutaSonido->liberar(NB_RETENEDOR_NULL);
			//siguiente evento
			evSonido = datosXml->nodoHijo("eventoSonido", eventosSonido, evSonido);
		}
	}
#	endif
	//
	plantillaAnim->liberar(NB_RETENEDOR_NULL);
	//}
	strTipoElem->liberar(NB_RETENEDOR_NULL);
	//Autoliberar
	if(nuevaAnimacion != NULL){
		nuevaAnimacion->detenerAnimacion();
		nuevaAnimacion->rebobinarAnimacion();
		nuevaAnimacion->autoLiberar(NB_RETENEDOR_NULL);
		NB_DEFINE_NOMBRE_PUNTERO(nuevaAnimacion, "NBGestorAnimaciones::nuevaAnimacionXML");
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return nuevaAnimacion;
}

AUEscenaAnimacion* NBGestorAnimaciones::privSubanimacionDesdeFlujoBinario(AUArchivo* archivoBinario, AUAnimacionBibliotecaDef* plantillaBiblio, AUArregloNativoMutableP<NBPunto>* bibTraslaciones, AUArregloNativoMutableP<NBTamano>* bibEscalaciones, AUArregloNativoMutableP<float>* bibRotaciones, AUArregloNativoMutableP<NBColor8>* bibColores, ENAnimacion_TipoEncapsulamiento tipoEncapsulamiento){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorAnimaciones::privSubanimacionDesdeFlujoBinario")
	AUEscenaAnimacion* nuevaAnimacion = NULL; bool esFlujoBinarioValido = true;
	NBCajaAABB cajaLienzoAnimacion; NBCAJAAABB_INICIALIZAR(cajaLienzoAnimacion);
	SI32 frames;
	float fps;
	archivoBinario->leer(&cajaLienzoAnimacion, sizeof(cajaLienzoAnimacion), 1, archivoBinario);
	archivoBinario->leer(&fps, sizeof(fps), 1, archivoBinario);
	archivoBinario->leer(&frames, sizeof(frames), 1, archivoBinario);
	plantillaBiblio->fps						= fps;
	plantillaBiblio->tipoEncapsulamientoHijos	= tipoEncapsulamiento;
	AUAnimacionDef* plantillaAnim	= new(ENMemoriaTipo_General) AUAnimacionDef(frames, cajaLienzoAnimacion);	NB_DEFINE_NOMBRE_PUNTERO(plantillaAnim, "NBGestorAnimaciones::plantillaAnim(bin)");
	SI32 iFrame; for(iFrame=0; iFrame<frames; iFrame++) plantillaAnim->framesConEventoSonido.agregarElemento(false);
	nuevaAnimacion 	= new(ENMemoriaTipo_General) AUEscenaAnimacion(plantillaBiblio, plantillaAnim);				NB_DEFINE_NOMBRE_PUNTERO(nuevaAnimacion, "NBGestorAnimaciones::nuevaAnimacion(FlujoBinario)");
	//Leer hijos
	AUArregloMutable* capasCargadas				= new(ENMemoriaTipo_Temporal) AUArregloMutable();				NB_DEFINE_NOMBRE_PUNTERO(capasCargadas, "AUGameplay::capasCargadas");
	AUArregloMutable* electronodosPrecargados	= new(ENMemoriaTipo_Temporal) AUArregloMutable();
	AUArregloMutable* bateriasPrecargados		= new(ENMemoriaTipo_Temporal) AUArregloMutable();
	AUArregloMutable* bombillosPrecargados		= new(ENMemoriaTipo_Temporal) AUArregloMutable();
	AUArregloMutable* lasersPrecargados			= new(ENMemoriaTipo_Temporal) AUArregloMutable();
	AUArregloMutable* panelesSolPrecargados		= new(ENMemoriaTipo_Temporal) AUArregloMutable();
	capasCargadas->agregarElemento(nuevaAnimacion);
	AUCadenaMutable8* strTipoElemento = new(ENMemoriaTipo_Temporal) AUCadenaMutable8(); NB_DEFINE_NOMBRE_PUNTERO(strTipoElemento, "NBGestorAnimaciones::strTipoElemento(bin)");
	AUCadenaMutable8::interpretarBitsInternos(strTipoElemento, archivoBinario);
	while(strTipoElemento->tamano()>0 && esFlujoBinarioValido){
		SI32 indiceElem;
		if(strTipoElemento->esIgual("AUEscenaSprite")){
			AUEscenaSprite* sprite = new(nuevaAnimacion) AUEscenaSprite(); NB_DEFINE_NOMBRE_PUNTERO(sprite, "NBGestorAnimaciones::sprite");
			if(!AUEscenaSprite::interpretarBitsInternos(sprite, archivoBinario)){
				PRINTF_ERROR("AUEscenaAnimacion, intentando interpretar BIN interno de sprite\n");
			} else {
				if(tipoEncapsulamiento==ENAnimacion_TipoEncapsulamiento_UnContenedorPorHijo){
					AUEscenaContenedor* encapsulador = new(nuevaAnimacion) AUEscenaContenedor(); NB_DEFINE_NOMBRE_PUNTERO(encapsulador, "NBGestorAnimaciones::encapsulador(bin)");
					encapsulador->agregarObjetoEscena(sprite);
					nuevaAnimacion->agregarObjetoEscena(encapsulador);
					encapsulador->liberar(NB_RETENEDOR_NULL);
				} else {
					nuevaAnimacion->agregarObjetoEscena(sprite);
				}
				indiceElem = nuevaAnimacion->hijos()->conteo - 1;
			}
			sprite->liberar(NB_RETENEDOR_NULL);
		} else if(strTipoElemento->esIgual("AUEscenaGeneradorParticulas")){
			AUEscenaGeneradorParticulas* genParticulas = new(nuevaAnimacion) AUEscenaGeneradorParticulas(); NB_DEFINE_NOMBRE_PUNTERO(genParticulas, "NBGestorAnimaciones::genParticulas");
			if(!AUEscenaGeneradorParticulas::interpretarBitsInternos(genParticulas, archivoBinario)){
				PRINTF_ERROR("AUEscenaAnimacion, intentando interpretar BIN interno de AUEscenaGeneradorParticulas\n");
			} else {
				if(tipoEncapsulamiento==ENAnimacion_TipoEncapsulamiento_UnContenedorPorHijo){
					AUEscenaContenedor* encapsulador = new(nuevaAnimacion) AUEscenaContenedor(); NB_DEFINE_NOMBRE_PUNTERO(encapsulador, "NBGestorAnimaciones::encapsulador(bin)");
					encapsulador->agregarObjetoEscena(genParticulas);
					nuevaAnimacion->agregarObjetoEscena(encapsulador);
					encapsulador->liberar(NB_RETENEDOR_NULL);
				} else {
					nuevaAnimacion->agregarObjetoEscena(genParticulas);
				}
				indiceElem = nuevaAnimacion->hijos()->conteo - 1;
			}
			genParticulas->liberar(NB_RETENEDOR_NULL);
		} else if(strTipoElemento->esIgual("AUEscenaCuerpo")){
			AUCadenaMutable8* rutaSubcuerpo = new(ENMemoriaTipo_Temporal) AUCadenaMutable8(); NB_DEFINE_NOMBRE_PUNTERO(rutaSubcuerpo, "NBGestorAnimaciones::rutaSubcuerpo(bin)");
			AUCadenaMutable8::interpretarBitsInternos(rutaSubcuerpo, archivoBinario);	
			AUEscenaCuerpoI* cuerpo = (AUEscenaCuerpoI*)_cargadorCuerpos->cargarCuerpo(ENMemoriaTipo_General, rutaSubcuerpo->str()); //luego s ele definira su agrupador de sombras
			if(cuerpo != NULL){
				cuerpo->establecerCreacionCuerpoTipo(ENCuerpoTipo_Kinematico, ENAnimFisicaTipo_SeguirObjetoEscena);
				if(tipoEncapsulamiento==ENAnimacion_TipoEncapsulamiento_UnContenedorPorHijo){
					AUEscenaContenedor* encapsulador = new(nuevaAnimacion) AUEscenaContenedor(); NB_DEFINE_NOMBRE_PUNTERO(encapsulador, "NBGestorAnimaciones::encapsulador(bin)"); 
					encapsulador->agregarObjetoEscena(cuerpo);
					nuevaAnimacion->agregarObjetoEscena(encapsulador);
					encapsulador->liberar(NB_RETENEDOR_NULL);
				} else {
					nuevaAnimacion->agregarObjetoEscena(cuerpo);
				}
				indiceElem = nuevaAnimacion->hijos()->conteo - 1;
			}
			rutaSubcuerpo->liberar(NB_RETENEDOR_NULL);
		} else if(strTipoElemento->esIgual("AUEscenaAnimacion")){
			AUCadenaMutable8* rutaSubanimacion = new(ENMemoriaTipo_Temporal) AUCadenaMutable8(); NB_DEFINE_NOMBRE_PUNTERO(rutaSubanimacion, "NBGestorAnimaciones::rutaSubanimacion(bin)");
			AUCadenaMutable8::interpretarBitsInternos(rutaSubanimacion, archivoBinario);
			AUEscenaAnimacion* subAnimacion = NULL;
			if(rutaSubanimacion->tamano()>0){
				subAnimacion = NBGestorAnimaciones::animacionDesdeArchivo(ENMemoriaTipo_General, rutaSubanimacion->str()); //luego se le asignara el agrupador de sombras
				if(subAnimacion == NULL){
					PRINTF_ERROR("NBGestorAnimaciones, intentando interpretar BIN interno de subanimacion referenciada\n");
				} else {
					NB_DEFINE_NOMBRE_PUNTERO(subAnimacion, "NBGestorAnimaciones::subAnimacionBINRef");
				}
			} else {
				subAnimacion = NBGestorAnimaciones::privSubanimacionDesdeFlujoBinario(archivoBinario, plantillaBiblio, bibTraslaciones, bibEscalaciones, bibRotaciones, bibColores, tipoEncapsulamiento);
				if(subAnimacion == NULL){
					PRINTF_ERROR("NBGestorAnimaciones, intentando interpretar BIN interno de subanimacion\n");
				} else {
					NB_DEFINE_NOMBRE_PUNTERO(subAnimacion, "NBGestorAnimaciones::subAnimacionBIN");
				}
			}
			if(subAnimacion != NULL){
				subAnimacion->establecerRepetirAnimacion(true);
				if(tipoEncapsulamiento==ENAnimacion_TipoEncapsulamiento_UnContenedorPorHijo){
					AUEscenaContenedor* encapsulador = new(nuevaAnimacion) AUEscenaContenedor(); NB_DEFINE_NOMBRE_PUNTERO(encapsulador, "NBGestorAnimaciones::encapsulador(bin)");
					encapsulador->agregarObjetoEscena(subAnimacion);
					nuevaAnimacion->agregarObjetoEscena(encapsulador);
					encapsulador->liberar(NB_RETENEDOR_NULL);
				} else {
					nuevaAnimacion->agregarObjetoEscena(subAnimacion);
				}
				indiceElem = nuevaAnimacion->hijos()->conteo - 1;
			}
			rutaSubanimacion->liberar(NB_RETENEDOR_NULL);
		} else if(strTipoElemento->esIgual("AUBateria")){
			AUBateria* objeto = new(nuevaAnimacion) AUBateria(); NB_DEFINE_NOMBRE_PUNTERO(objeto, "AUGameplay::bateria(bin)")
			if(tipoEncapsulamiento==ENAnimacion_TipoEncapsulamiento_UnContenedorPorHijo){
				AUEscenaContenedor* encapsulador = new(nuevaAnimacion) AUEscenaContenedor(); NB_DEFINE_NOMBRE_PUNTERO(encapsulador, "NBGestorAnimaciones::encapsulador(bin)");
				encapsulador->agregarObjetoEscena(objeto);
				nuevaAnimacion->agregarObjetoEscena(encapsulador);
				encapsulador->liberar(NB_RETENEDOR_NULL);
			} else {
				nuevaAnimacion->agregarObjetoEscena(objeto);
			}
			indiceElem = nuevaAnimacion->hijos()->conteo - 1;
			bateriasPrecargados->agregarElemento(objeto);
			objeto->liberar(NB_RETENEDOR_NULL);
		} else if(strTipoElemento->esIgual("AUBombillo")){
			AUBombillo* objeto = new(nuevaAnimacion) AUBombillo(); NB_DEFINE_NOMBRE_PUNTERO(objeto, "AUGameplay::bombillo(bin)")
			if(tipoEncapsulamiento==ENAnimacion_TipoEncapsulamiento_UnContenedorPorHijo){
				AUEscenaContenedor* encapsulador = new(nuevaAnimacion) AUEscenaContenedor(); NB_DEFINE_NOMBRE_PUNTERO(encapsulador, "NBGestorAnimaciones::encapsulador(bin)");
				encapsulador->agregarObjetoEscena(objeto);
				nuevaAnimacion->agregarObjetoEscena(encapsulador);
				encapsulador->liberar(NB_RETENEDOR_NULL);
			} else {
				nuevaAnimacion->agregarObjetoEscena(objeto);
			}
			indiceElem = nuevaAnimacion->hijos()->conteo - 1;
			bombillosPrecargados->agregarElemento(objeto);
			objeto->liberar(NB_RETENEDOR_NULL);
		} else if(strTipoElemento->esIgual("AULaser")){
			AULaser* objeto = new(nuevaAnimacion) AULaser(); NB_DEFINE_NOMBRE_PUNTERO(objeto, "AUGameplay::laser(bin)")
			if(tipoEncapsulamiento==ENAnimacion_TipoEncapsulamiento_UnContenedorPorHijo){
				AUEscenaContenedor* encapsulador = new(nuevaAnimacion) AUEscenaContenedor(); NB_DEFINE_NOMBRE_PUNTERO(encapsulador, "NBGestorAnimaciones::encapsulador(bin)");
				encapsulador->agregarObjetoEscena(objeto);
				nuevaAnimacion->agregarObjetoEscena(encapsulador);
				encapsulador->liberar(NB_RETENEDOR_NULL);
			} else {
				nuevaAnimacion->agregarObjetoEscena(objeto);
			}
			indiceElem = nuevaAnimacion->hijos()->conteo - 1;
			lasersPrecargados->agregarElemento(objeto);
			objeto->liberar(NB_RETENEDOR_NULL);
		} else if(strTipoElemento->esIgual("AUPanelSolar")){
			AUPanelSolar* objeto = new(nuevaAnimacion) AUPanelSolar(); NB_DEFINE_NOMBRE_PUNTERO(objeto, "AUGameplay::panelSol(bin)")
			if(tipoEncapsulamiento==ENAnimacion_TipoEncapsulamiento_UnContenedorPorHijo){
				AUEscenaContenedor* encapsulador = new(nuevaAnimacion) AUEscenaContenedor(); NB_DEFINE_NOMBRE_PUNTERO(encapsulador, "NBGestorAnimaciones::encapsulador(bin)");
				encapsulador->agregarObjetoEscena(objeto);
				nuevaAnimacion->agregarObjetoEscena(encapsulador);
				encapsulador->liberar(NB_RETENEDOR_NULL);
			} else {
				nuevaAnimacion->agregarObjetoEscena(objeto);
			}
			indiceElem = nuevaAnimacion->hijos()->conteo - 1;
			panelesSolPrecargados->agregarElemento(objeto);
			objeto->liberar(NB_RETENEDOR_NULL);
		} else if(strTipoElemento->esIgual("AUElectronodo")){
			AUElectronodo* objeto = new(nuevaAnimacion) AUElectronodo(); NB_DEFINE_NOMBRE_PUNTERO(objeto, "AUGameplay::sensor(bin)")
			if(tipoEncapsulamiento==ENAnimacion_TipoEncapsulamiento_UnContenedorPorHijo){
				AUEscenaContenedor* encapsulador = new(nuevaAnimacion) AUEscenaContenedor(); NB_DEFINE_NOMBRE_PUNTERO(encapsulador, "NBGestorAnimaciones::encapsulador(bin)");
				encapsulador->agregarObjetoEscena(objeto);
				nuevaAnimacion->agregarObjetoEscena(encapsulador);
				encapsulador->liberar(NB_RETENEDOR_NULL);
			} else {
				nuevaAnimacion->agregarObjetoEscena(objeto);
			}
			indiceElem = nuevaAnimacion->hijos()->conteo - 1;
			electronodosPrecargados->agregarElemento(objeto);
			objeto->liberar(NB_RETENEDOR_NULL);
		} else if(strTipoElemento->esIgual("AULanzallamas")){
			AULanzallamas* objeto = new(nuevaAnimacion) AULanzallamas(); NB_DEFINE_NOMBRE_PUNTERO(objeto, "AUGameplay::flor(bin)")
			AULanzallamas::interpretarBitsInternos(objeto, archivoBinario);
			if(tipoEncapsulamiento==ENAnimacion_TipoEncapsulamiento_UnContenedorPorHijo){
				AUEscenaContenedor* encapsulador = new(nuevaAnimacion) AUEscenaContenedor(); NB_DEFINE_NOMBRE_PUNTERO(encapsulador, "NBGestorAnimaciones::encapsulador(bin)");
				encapsulador->agregarObjetoEscena(objeto);
				nuevaAnimacion->agregarObjetoEscena(encapsulador);
				encapsulador->liberar(NB_RETENEDOR_NULL);
			} else {
				nuevaAnimacion->agregarObjetoEscena(objeto);
			}
			indiceElem = nuevaAnimacion->hijos()->conteo - 1;
			objeto->liberar(NB_RETENEDOR_NULL);
		} else {
			NBASSERT(false) //tipo de objeto no se reconoce
		}
		NBGESTORANIMACIONES_LEER_VERIFICACION_BIN(archivoBinario, esFlujoBinarioValido);
		if(esFlujoBinarioValido){
			//Interpretar los indices de propiedades
			int iFrame;
			for(iFrame=0; iFrame<frames; iFrame++){
				STAnimacionIndicesPropiedades propsElem;
				archivoBinario->leer(&propsElem, sizeof(propsElem), 1, archivoBinario);
				plantillaAnim->establecerIndicePropiedadesEnFrame(iFrame, indiceElem, propsElem.iTraslacion, propsElem.iEscalacion, propsElem.iRotacion, propsElem.iColor);
			}
			NBGESTORANIMACIONES_LEER_VERIFICACION_BIN(archivoBinario, esFlujoBinarioValido);
		}
		//siguiente
		AUCadenaMutable8::interpretarBitsInternos(strTipoElemento, archivoBinario);
	}
	NBGESTORANIMACIONES_LEER_VERIFICACION_BIN(archivoBinario, esFlujoBinarioValido);
	//CARGAR PRECARGADOS
	//Interpretar bits de paneles solares precargados (los datos deben interpretarse al final por si el sensor hace referencia a un objeto que esta delante de el en la lista)
	if(esFlujoBinarioValido){
		SI32 iPanelSol;
		for(iPanelSol=0; iPanelSol<panelesSolPrecargados->conteo; iPanelSol++){
			AUPanelSolar* panelSolar = (AUPanelSolar*)panelesSolPrecargados->elemento[iPanelSol];
			if(!AUPanelSolar::interpretarBitsInternos(panelSolar, archivoBinario, nuevaAnimacion->hijos())){
				NBASSERT(false)
			}
		}
	}
	NBGESTORANIMACIONES_LEER_VERIFICACION_BIN(archivoBinario, esFlujoBinarioValido);
	//Interpretar bits de baterias precargados (los datos deben interpretarse al final por si el sensor hace referencia a un objeto que esta delante de el en la lista)
	if(esFlujoBinarioValido){
		SI32 iBateria;
		for(iBateria=0; iBateria<bateriasPrecargados->conteo; iBateria++){
			AUBateria* objetoPrecargado = (AUBateria*)bateriasPrecargados->elemento[iBateria];
			if(!AUBateria::interpretarBitsInternos(objetoPrecargado, archivoBinario, nuevaAnimacion->hijos())){
				NBASSERT(false)
			}
		}
	}
	NBGESTORANIMACIONES_LEER_VERIFICACION_BIN(archivoBinario, esFlujoBinarioValido);
	//Interpretar bits de bombillos precargados (los datos deben interpretarse al final por si el sensor hace referencia a un objeto que esta delante de el en la lista)
	if(esFlujoBinarioValido){
		SI32 iBombillo;
		for(iBombillo=0; iBombillo<bombillosPrecargados->conteo; iBombillo++){
			AUBombillo* objetoPrecargado = (AUBombillo*)bombillosPrecargados->elemento[iBombillo];
			if(!AUBombillo::interpretarBitsInternos(objetoPrecargado, archivoBinario, nuevaAnimacion->hijos())){
				NBASSERT(false)
			}
		}
	}
	NBGESTORANIMACIONES_LEER_VERIFICACION_BIN(archivoBinario, esFlujoBinarioValido);
	//Interpretar bits de lasers precargados (los datos deben interpretarse al final por si el sensor hace referencia a un objeto que esta delante de el en la lista)
	if(esFlujoBinarioValido){
		SI32 iLaser;
		for(iLaser=0; iLaser<lasersPrecargados->conteo; iLaser++){
			AULaser* objetoPrecargado = (AULaser*)lasersPrecargados->elemento[iLaser];
			if(!AULaser::interpretarBitsInternos(objetoPrecargado, archivoBinario, nuevaAnimacion->hijos())){
				NBASSERT(false)
			}
		}
	}
	NBGESTORANIMACIONES_LEER_VERIFICACION_BIN(archivoBinario, esFlujoBinarioValido);
	//Interpretar bits de electronodos precargados (los datos deben interpretarse al final por si el sensor hace referencia a un objeto que esta delante de el en la lista)
	if(esFlujoBinarioValido){
		SI32 iElectro;
		for(iElectro=0; iElectro<electronodosPrecargados->conteo; iElectro++){
			AUElectronodo* electronodo = (AUElectronodo*)electronodosPrecargados->elemento[iElectro];
			if(!AUElectronodo::interpretarBitsInternos(electronodo, archivoBinario, capasCargadas)){
				NBASSERT(false)
			}
		}
	}
	NBGESTORANIMACIONES_LEER_VERIFICACION_BIN(archivoBinario, esFlujoBinarioValido); if(!esFlujoBinarioValido){NBASSERT(false);}
	lasersPrecargados->liberar(NB_RETENEDOR_NULL); lasersPrecargados = NULL;
	bombillosPrecargados->liberar(NB_RETENEDOR_NULL); bombillosPrecargados = NULL;
	bateriasPrecargados->liberar(NB_RETENEDOR_NULL); bateriasPrecargados = NULL;
	panelesSolPrecargados->liberar(NB_RETENEDOR_NULL); panelesSolPrecargados = NULL;
	electronodosPrecargados->liberar(NB_RETENEDOR_NULL); electronodosPrecargados = NULL;
	capasCargadas->liberar(NB_RETENEDOR_NULL); capasCargadas = NULL;
	strTipoElemento->liberar(NB_RETENEDOR_NULL);
	//Leer eventos de sonidos
	AUCadenaMutable8* rutaSonido = new(ENMemoriaTipo_Temporal) AUCadenaMutable8(); NB_DEFINE_NOMBRE_PUNTERO(rutaSonido, "NBGestorAnimaciones::rutaSonido(bin)");
	SI32 conteoEventosSonido; archivoBinario->leer(&conteoEventosSonido, sizeof(conteoEventosSonido), 1, archivoBinario);
	SI32 iEvento;
	for(iEvento=0; iEvento<conteoEventosSonido; iEvento++){
		STAnimacionEventoSonido eventoSonido; 
		archivoBinario->leer(&eventoSonido, sizeof(eventoSonido), 1, archivoBinario);
		rutaSonido->vaciar(); AUCadenaMutable8::interpretarBitsInternos(rutaSonido, archivoBinario);
		eventoSonido.rutaSonido		= new(ENMemoriaTipo_General) AUCadena8(rutaSonido->str()); NB_DEFINE_NOMBRE_PUNTERO(eventoSonido.rutaSonido, "NBGestorAnimaciones::eventoSonido.rutaSonido(BIN)");
#		ifndef CONFIG_NB_UNSUPPORT_AUDIO_IO
		if(eventoSonido.bufferSonido != 0){
			eventoSonido.bufferSonido	= NBGestorSonidos::bufferDesdeArchivoWav(eventoSonido.rutaSonido->str()); //PRINTF_INFO("Ruta sonido animacionBIN: '%s' y '%s'\n", rutaSonido->str(), eventoSonido.rutaSonido->str());
		}
		if(eventoSonido.streamSonido != NULL){
			eventoSonido.streamSonido	= NBGestorSonidos::streamDesdeArchivo(eventoSonido.rutaSonido->str());
		}
		if(eventoSonido.animadorSonido != NULL){
			eventoSonido.animadorSonido = new(ENMemoriaTipo_General) AUAnimadorSonidoVolumen(); NB_DEFINE_NOMBRE_PUNTERO(eventoSonido.animadorSonido, "NBGestorAnimaciones::eventoSonido.animadorSonido(BIN)");
			AUAnimadorSonidoVolumen::interpretarBitsInternos(eventoSonido.animadorSonido, archivoBinario);
		}
		//PRINTF_INFO("Animacion tiene animador de sonido: '%s'\n", eventoSonido.animadorSonido != NULL?"SI":"NO");
		if(eventoSonido.bufferSonido == 0){
			PRINTF_ERROR("Evento de SWF no pudo ser agregado para sonido '%s'\n", eventoSonido.rutaSonido->str());
		} else {
			NBGestorSonidos::bufferRetener(eventoSonido.bufferSonido);
			plantillaAnim->framesConEventoSonido.elemento[eventoSonido.iFrame] = true;
			plantillaAnim->eventosDeSonido.agregarElemento(eventoSonido);
		}
#		endif
	}
	NBGESTORANIMACIONES_LEER_VERIFICACION_BIN(archivoBinario, esFlujoBinarioValido);
	rutaSonido->liberar(NB_RETENEDOR_NULL);
	plantillaAnim->liberar(NB_RETENEDOR_NULL);
	//Autoliberar
	if(nuevaAnimacion != NULL){
		nuevaAnimacion->detenerAnimacion();
		nuevaAnimacion->rebobinarAnimacion();
		nuevaAnimacion->autoLiberar(NB_RETENEDOR_NULL);
		NB_DEFINE_NOMBRE_PUNTERO(nuevaAnimacion, "NBGestorAnimaciones::nuevaAnimacionBIN");
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return nuevaAnimacion;
}

bool NBGestorAnimaciones::privGuardarAnimacionEnBinario(AUEscenaAnimacion* animacionRaiz, const char* rutaFisicaArchivo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorAnimaciones::privGuardarAnimacionEnBinario")
	bool r = false;
	//Crear archivo binario
	AUAnimacionBibliotecaDef* nuevaBiblioteca = animacionRaiz->plantillaBiblioteca();
	AUArchivo* archivoCache = NBGestorArchivos::flujoDeArchivo(ENMemoriaTipo_Temporal, rutaFisicaArchivo, ENArchivoModo_SoloEscritura);
	if(archivoCache != NULL){
		archivoCache->lock();
		//
		NBGESTORANIMACIONES_ESCRIBIR_VERIFICACION_BIN(archivoCache);
		archivoCache->escribir(&nuevaBiblioteca->scaleBase2, sizeof(nuevaBiblioteca), 1, archivoCache);
		NBGESTORANIMACIONES_ESCRIBIR_VERIFICACION_BIN(archivoCache);
		nuevaBiblioteca->traslaciones.agregarBitsInternosEn(archivoCache);
		NBGESTORANIMACIONES_ESCRIBIR_VERIFICACION_BIN(archivoCache);
		nuevaBiblioteca->escalaciones.agregarBitsInternosEn(archivoCache);
		NBGESTORANIMACIONES_ESCRIBIR_VERIFICACION_BIN(archivoCache);
		nuevaBiblioteca->rotaciones.agregarBitsInternosEn(archivoCache);
		NBGESTORANIMACIONES_ESCRIBIR_VERIFICACION_BIN(archivoCache);
		nuevaBiblioteca->colores.agregarBitsInternosEn(archivoCache);
		NBGESTORANIMACIONES_ESCRIBIR_VERIFICACION_BIN(archivoCache);
		//escribir animacion raiz
		privAgregarBitsSubanimacion(archivoCache, animacionRaiz);
		//
		archivoCache->unlock();
		archivoCache->cerrar();
		r = true;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return r;
}

bool NBGestorAnimaciones::privAgregarBitsSubanimacion(AUArchivo* guardarBitsEn, AUEscenaAnimacion* nuevaAnimacion){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorAnimaciones::privAgregarBitsSubanimacion")
	AUCadenaMutable8* strTipoElem				= new(ENMemoriaTipo_Temporal) AUCadenaMutable8();
	AUAnimacionBibliotecaDef* plantillaBiblio	= nuevaAnimacion->plantillaBiblioteca();
	AUAnimacionDef* plantillaAnim				= nuevaAnimacion->plantillaAnimacion();
	//PROPIEDADES GENERALES
	NBCajaAABB cajaLienzo						= plantillaAnim->cajaLienzo;
	SI32 frames									= plantillaAnim->frames;
	float fps									= plantillaBiblio->fps;
	guardarBitsEn->escribir(&cajaLienzo, sizeof(cajaLienzo), 1, guardarBitsEn);
	guardarBitsEn->escribir(&fps, sizeof(fps), 1, guardarBitsEn);
	guardarBitsEn->escribir(&frames, sizeof(frames), 1, guardarBitsEn);
	//ELEMENTOS DE VISUALES
	AUArregloMutable* capasGuardadas			= new(ENMemoriaTipo_Temporal) AUArregloMutable(); capasGuardadas->agregarElemento(nuevaAnimacion);
	AUArregloMutable* bateriasPreguardados		= new(ENMemoriaTipo_Temporal) AUArregloMutable();
	AUArregloMutable* bombillosPreguardados		= new(ENMemoriaTipo_Temporal) AUArregloMutable();
	AUArregloMutable* lasersPreguardados		= new(ENMemoriaTipo_Temporal) AUArregloMutable();
	AUArregloMutable* panelesSolPreguardados	= new(ENMemoriaTipo_Temporal) AUArregloMutable();
	AUArregloMutable* electronodosPreguardados	= new(ENMemoriaTipo_Temporal) AUArregloMutable();
	AUArreglo* hijosAnim	= nuevaAnimacion->hijos();
	AUEscenaObjeto** hijos	= (AUEscenaObjeto**)hijosAnim->elemento;
	ENAnimacion_TipoEncapsulamiento tipoEncapsulamiento = nuevaAnimacion->tipoEncapsulamientoHijos();
	UI16 iElem, conteoElems = nuevaAnimacion->hijos()->conteo;
	for(iElem=0; iElem<conteoElems; iElem++){
		AUEscenaObjeto* hijo = hijos[iElem];
		if(tipoEncapsulamiento==ENAnimacion_TipoEncapsulamiento_UnContenedorPorHijo){
			NBASSERT(hijo->esClase(AUEscenaContenedor::idTipoClase))
			NBASSERT(((AUEscenaContenedor*)hijo)->hijos()->conteo != 0)
			hijo	= (AUEscenaObjeto*)((AUEscenaContenedor*)hijo)->hijos()->elemento[0];
		}
		if(hijo->esClase(AUEscenaSprite::idTipoClase)){
			strTipoElem->establecer("AUEscenaSprite");
			AUCadena8::agregarBitsInternosEn(strTipoElem, guardarBitsEn);
			AUEscenaSprite::agregarBitsInternosEn((AUEscenaSprite*)hijo, guardarBitsEn);
		} else if(hijo->esClase(AUEscenaGeneradorParticulas::idTipoClase)){
			strTipoElem->establecer("AUEscenaGeneradorParticulas");
			AUCadena8::agregarBitsInternosEn(strTipoElem, guardarBitsEn);
			AUEscenaGeneradorParticulas::agregarBitsInternosEn((AUEscenaGeneradorParticulas*)hijo, guardarBitsEn);
		} else if(hijo->esClase(AUEscenaCuerpoI::idTipoClase)){
			strTipoElem->establecer("AUEscenaCuerpo");
			AUCadena8::agregarBitsInternosEn(strTipoElem, guardarBitsEn);
			AUCadenaMutable8* rutaSubcuerpo = new(ENMemoriaTipo_Temporal) AUCadenaMutable8(((AUEscenaCuerpoI*)hijo)->rutaArchivoOrigen());  NB_DEFINE_NOMBRE_PUNTERO(rutaSubcuerpo, "NBGestorAnimaciones::rutaSubcuerpo");
			AUCadena8::agregarBitsInternosEn(rutaSubcuerpo, guardarBitsEn);
			rutaSubcuerpo->liberar(NB_RETENEDOR_NULL);
		} else if(hijo->esClase(AUEscenaAnimacion::idTipoClase)){
			strTipoElem->establecer("AUEscenaAnimacion");
			AUCadena8::agregarBitsInternosEn(strTipoElem, guardarBitsEn);
			AUEscenaAnimacion* subanim			= (AUEscenaAnimacion*)hijo;
			AUCadenaMutable8* rutaSubanimacion	= new(ENMemoriaTipo_Temporal) AUCadenaMutable8();  NB_DEFINE_NOMBRE_PUNTERO(rutaSubanimacion, "NBGestorAnimaciones::rutaSubanimacion");
			if(subanim->plantillaBiblioteca()!=nuevaAnimacion->plantillaBiblioteca()){
				rutaSubanimacion->establecer(subanim->rutaArchivoOrigen());
			}
			AUCadena8::agregarBitsInternosEn(rutaSubanimacion, guardarBitsEn);
			if(rutaSubanimacion->tamano() == 0){
				NBGestorAnimaciones::privAgregarBitsSubanimacion(guardarBitsEn, subanim);
			}
			rutaSubanimacion->liberar(NB_RETENEDOR_NULL);
		} else if(hijo->esClase(AUBateria::idTipoClase)){
			strTipoElem->establecer("AUBateria");
			AUCadena8::agregarBitsInternosEn(strTipoElem, guardarBitsEn);
			bateriasPreguardados->agregarElemento(hijo);
		} else if(hijo->esClase(AUBombillo::idTipoClase)){
			strTipoElem->establecer("AUBombillo");
			AUCadena8::agregarBitsInternosEn(strTipoElem, guardarBitsEn);
			bombillosPreguardados->agregarElemento(hijo);
		} else if(hijo->esClase(AULaser::idTipoClase)){
			strTipoElem->establecer("AULaser");
			AUCadena8::agregarBitsInternosEn(strTipoElem, guardarBitsEn);
			lasersPreguardados->agregarElemento(hijo);
		} else if(hijo->esClase(AUPanelSolar::idTipoClase)){
			strTipoElem->establecer("AUPanelSolar");
			AUCadena8::agregarBitsInternosEn(strTipoElem, guardarBitsEn);
			panelesSolPreguardados->agregarElemento(hijo);
		} else if(hijo->esClase(AUElectronodo::idTipoClase)){
			strTipoElem->establecer("AUElectronodo");
			AUCadena8::agregarBitsInternosEn(strTipoElem, guardarBitsEn);
			electronodosPreguardados->agregarElemento(hijo);
		} else if(hijo->esClase(AULanzallamas::idTipoClase)){
			strTipoElem->establecer("AULanzallamas");
			AUCadena8::agregarBitsInternosEn(strTipoElem, guardarBitsEn);
			AULanzallamas::agregarBitsInternosEn((AULanzallamas*)hijo, guardarBitsEn);
		} else {
			NBASSERT(false) //Objeto no identificado en animacion
		}
		NBGESTORANIMACIONES_ESCRIBIR_VERIFICACION_BIN(guardarBitsEn);
		//Propiedades por frame del objeto
		NBASSERT(plantillaAnim->iPropiedadesPorFrame.conteo==plantillaAnim->frames);
		UI16 iFrame, conteoFrames = plantillaAnim->frames;
		for(iFrame=0; iFrame<conteoFrames; iFrame++){
			AUArregloNativoP<STAnimacionIndicesPropiedades>* propsEnFrame = (AUArregloNativoP<STAnimacionIndicesPropiedades>*)plantillaAnim->iPropiedadesPorFrame.elemento[iFrame];
			NBASSERT(iElem<propsEnFrame->conteo)
			STAnimacionIndicesPropiedades propsElem = propsEnFrame->elemento[iElem];
			guardarBitsEn->escribir(&propsElem, sizeof(propsElem), 1, guardarBitsEn);
		}
		NBGESTORANIMACIONES_ESCRIBIR_VERIFICACION_BIN(guardarBitsEn);
	}
	//Indicar el final de la lista de objetos (tipo 'cadena vacia')
	strTipoElem->vaciar();
	AUCadena8::agregarBitsInternosEn(strTipoElem, guardarBitsEn);
	NBGESTORANIMACIONES_ESCRIBIR_VERIFICACION_BIN(guardarBitsEn);
	//OBJETOS PRECARGADOS
	SI32 iPanelSol;
	for(iPanelSol=0; iPanelSol<panelesSolPreguardados->conteo; iPanelSol++){
		AUPanelSolar* objPreguardado = (AUPanelSolar*)panelesSolPreguardados->elemento[iPanelSol];
		if(!AUPanelSolar::agregarBitsInternosEn(objPreguardado, guardarBitsEn, hijosAnim)){
			PRINTF_ERROR("guardado datos binarios de PANEL_SOL\n"); //NBASSER(false) //No utilizar NBASSERT para evitar perdida de trabajo del disenador
		}
	}
	NBGESTORANIMACIONES_ESCRIBIR_VERIFICACION_BIN(guardarBitsEn);
	//
	SI32 iBateria;
	for(iBateria=0; iBateria<bateriasPreguardados->conteo; iBateria++){
		AUBateria* objPreguardado = (AUBateria*)bateriasPreguardados->elemento[iBateria];
		if(!AUBateria::agregarBitsInternosEn(objPreguardado, guardarBitsEn, hijosAnim)){
			PRINTF_ERROR("guardado datos binarios de BATERIA\n"); //NBASSER(false) //No utilizar NBASSERT para evitar perdida de trabajo del disenador
		}
	}
	NBGESTORANIMACIONES_ESCRIBIR_VERIFICACION_BIN(guardarBitsEn);
	//
	SI32 iBombillo;
	for(iBombillo=0; iBombillo<bombillosPreguardados->conteo; iBombillo++){
		AUBombillo* objPreguardado = (AUBombillo*)bombillosPreguardados->elemento[iBombillo];
		if(!AUBombillo::agregarBitsInternosEn(objPreguardado, guardarBitsEn, hijosAnim)){
			PRINTF_ERROR("guardado datos binarios de BOMBILLO\n"); //NBASSER(false) //No utilizar NBASSERT para evitar perdida de trabajo del disenador
		}
	}
	NBGESTORANIMACIONES_ESCRIBIR_VERIFICACION_BIN(guardarBitsEn);
	//
	SI32 iLaser;
	for(iLaser=0; iLaser<lasersPreguardados->conteo; iLaser++){
		AULaser* objPreguardado = (AULaser*)lasersPreguardados->elemento[iLaser];
		if(!AULaser::agregarBitsInternosEn(objPreguardado, guardarBitsEn, hijosAnim)){
			PRINTF_ERROR("guardado datos binarios de LASER\n"); //NBASSER(false) //No utilizar NBASSERT para evitar perdida de trabajo del disenador
		}
	}
	NBGESTORANIMACIONES_ESCRIBIR_VERIFICACION_BIN(guardarBitsEn);
	//
	UI32 iElectro, iElectroConteo = electronodosPreguardados->conteo;
	for(iElectro=0; iElectro<iElectroConteo; iElectro++){
		AUElectronodo* objPreguardado = (AUElectronodo*)electronodosPreguardados->elemento[iElectro];
		if(!AUElectronodo::agregarBitsInternosEn(objPreguardado, guardarBitsEn, capasGuardadas)){
			PRINTF_ERROR("guardado datos binarios de ELECTRONODO\n"); //NBASSER(false) //No utilizar NBASSERT para evitar perdida de trabajo del disenador
		}
	}
	NBGESTORANIMACIONES_ESCRIBIR_VERIFICACION_BIN(guardarBitsEn);
	//
	lasersPreguardados->liberar(NB_RETENEDOR_NULL); lasersPreguardados = NULL;
	bombillosPreguardados->liberar(NB_RETENEDOR_NULL); bombillosPreguardados = NULL;
	bateriasPreguardados->liberar(NB_RETENEDOR_NULL); bateriasPreguardados = NULL;
	panelesSolPreguardados->liberar(NB_RETENEDOR_NULL); panelesSolPreguardados = NULL;
	electronodosPreguardados->liberar(NB_RETENEDOR_NULL); electronodosPreguardados = NULL;
	capasGuardadas->liberar(NB_RETENEDOR_NULL);  capasGuardadas = NULL;
	//
	//EVENTOS DE SONIDO
	SI32 conteoEventosSonido = plantillaAnim->eventosDeSonido.conteo;
	guardarBitsEn->escribir(&conteoEventosSonido, sizeof(conteoEventosSonido), 1, guardarBitsEn);
	UI32 iEvento, iEventoConteo = plantillaAnim->eventosDeSonido.conteo;
	for(iEvento=0; iEvento<iEventoConteo; iEvento++){
		guardarBitsEn->escribir(&(plantillaAnim->eventosDeSonido.elemento[iEvento]), sizeof(plantillaAnim->eventosDeSonido.elemento[iEvento]), 1, guardarBitsEn);
		AUCadena8::agregarBitsInternosEn(plantillaAnim->eventosDeSonido.elemento[iEvento].rutaSonido, guardarBitsEn);
#		ifndef CONFIG_NB_UNSUPPORT_AUDIO_IO
		if(plantillaAnim->eventosDeSonido.elemento[iEvento].animadorSonido != NULL){
			AUAnimadorSonidoVolumen::agregarBitsInternosEn(plantillaAnim->eventosDeSonido.elemento[iEvento].animadorSonido, guardarBitsEn);
		}
#		endif
	}
	NBGESTORANIMACIONES_ESCRIBIR_VERIFICACION_BIN(guardarBitsEn);
	strTipoElem->liberar(NB_RETENEDOR_NULL); strTipoElem = NULL;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return true;
}

