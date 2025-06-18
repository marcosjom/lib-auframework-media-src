//
//  NBGestorCuerpos.cpp
//  AUFramework
//
//  Created by Nicaragua Binary on 21/02/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkMediaStdAfx.h"
#include "NBGestorCuerpos.h"

bool						NBGestorCuerpos::_gestorInicializado	= false;
bool						NBGestorCuerpos::_leerPrecacheCuerpos	= false;
bool						NBGestorCuerpos::_leerCacheCuerpos		= false;
bool						NBGestorCuerpos::_escribirCacheCuerpos	= false;
AUCadenaMutable8*			NBGestorCuerpos::_prefijoRutas			= NULL;
AUCadenaMutable8*			NBGestorCuerpos::_prefijoRutasCache		= NULL;
AUArregloMutable*			NBGestorCuerpos::_plantillasCuerpos		= NULL;

//Intermediario para la carga de cuerpos (intermediario necesario para incluir cuerpos en animaciones, y animaciones en cuerpos)
AUCargadorAnimacionesI*		NBGestorCuerpos::_cargadorAnimaciones;

void NBGestorCuerpos::inicializar(bool leerPrecacheCuerpos, bool leerCacheCuerpos, bool escribirCacheCuerpos, AUCargadorAnimacionesI* cargadorAnimaciones){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorCuerpos::inicializar")
	_gestorInicializado		= false;
	_cargadorAnimaciones	= cargadorAnimaciones; if(_cargadorAnimaciones != NULL) _cargadorAnimaciones->retener(NB_RETENEDOR_NULL);
	_leerPrecacheCuerpos	= leerPrecacheCuerpos;
	_leerCacheCuerpos		= leerCacheCuerpos;
	_escribirCacheCuerpos	= escribirCacheCuerpos;
	_prefijoRutas			= new(ENMemoriaTipo_General) AUCadenaMutable8();	NB_DEFINE_NOMBRE_PUNTERO(_prefijoRutas, "NBGestorCuerpos::_prefijoRutas");
	_prefijoRutasCache		= new(ENMemoriaTipo_General) AUCadenaMutable8();	NB_DEFINE_NOMBRE_PUNTERO(_prefijoRutasCache, "NBGestorCuerpos::_prefijoRutasCache");
	_plantillasCuerpos		= new(ENMemoriaTipo_General) AUArregloMutable();	NB_DEFINE_NOMBRE_PUNTERO(_plantillasCuerpos, "NBGestorCuerpos::_plantillasCuerpos");
	NBASSERT(_cargadorAnimaciones != NULL)
	_gestorInicializado		= true;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

void NBGestorCuerpos::finalizar(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorCuerpos::finalizar")
	if(_prefijoRutas != NULL){
		_prefijoRutas->liberar(NB_RETENEDOR_NULL);
		_prefijoRutas = NULL;
	}
	if(_prefijoRutasCache != NULL){
		_prefijoRutasCache->liberar(NB_RETENEDOR_NULL);
		_prefijoRutasCache = NULL;
	}
	if(_plantillasCuerpos != NULL){
		#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
		/*SI32 i;
		for(i = 0; i <= _plantillasCuerpos->conteo; i++){
			const SI32 conteoReferencias = _plantillasCuerpos->elemento[i]->conteoReferencias();
			NBASSERT(conteoReferencias==1) //Si falla aun hay plantillas retenidas, no se puede asegurar la buena gestion de la memoria y punteros
		}*/
		#endif
		_plantillasCuerpos->vaciar();
		_plantillasCuerpos->liberar(NB_RETENEDOR_NULL);
		_plantillasCuerpos = NULL;
	}
	if(_cargadorAnimaciones != NULL){
		_cargadorAnimaciones->liberar(NB_RETENEDOR_NULL);
		_cargadorAnimaciones = NULL;
	}
	_gestorInicializado	= false;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

bool NBGestorCuerpos::gestorInicializado(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorCuerpos::gestorInicializado")
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return _gestorInicializado;
}

//Prefijos de rutas
const char*	NBGestorCuerpos::pefijoRutas(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorCuerpos::pefijoRutas")
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return _prefijoRutas->str();
}

void NBGestorCuerpos::establecerPrefijoRutas(const char* prefijoStr, const char* prefijoStrCache){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorCuerpos::establecerPrefijoRutas")
	_prefijoRutas->establecer(prefijoStr);
	_prefijoRutasCache->establecer(prefijoStrCache);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

//

UI16 NBGestorCuerpos::liberarPlantillasSinReferencias(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorCuerpos::liberarPlantillasSinReferencias")
	UI16 conteoLiberadas = 0;
	SI32 iCpo;
	for(iCpo=_plantillasCuerpos->conteo-1; iCpo>=0; iCpo--){
		if(_plantillasCuerpos->elemento[iCpo]->conteoReferencias() == 1){ //==1 Solo lo retiene el arreglo
			//PRINTF_INFO("PLANTILLA CUERPO ELIMINADA ret(%d) autolib(%d) '%s'\n", _plantillasCuerpos->elemento[iCpo]->conteoReferencias(), _plantillasCuerpos->elemento[iCpo]->conteoAutoliberacionesPendientes(), ((AUCuerpoDef*)_plantillasCuerpos->elemento[iCpo])->rutaRecurso.str());
			_plantillasCuerpos->quitarElementoEnIndice(iCpo);
			conteoLiberadas++;
		} else {
			//PRINTF_INFO("PLANTILLA CUERPO +PERSISTE+ ret(%d) autolib(%d) '%s'\n", _plantillasCuerpos->elemento[iCpo]->conteoReferencias(), _plantillasCuerpos->elemento[iCpo]->conteoAutoliberacionesPendientes(), ((AUCuerpoDef*)_plantillasCuerpos->elemento[iCpo])->rutaRecurso.str());
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return conteoLiberadas;
}

//

AUCuerpoDef* NBGestorCuerpos::definicionCuerpoDesdeArchivo(const char* rutaCuerpo, bool buscarEnBibliotecaLocal){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorCuerpos::definicionCuerpoDesdeArchivo")
	AUCuerpoDef* defCuerpo	= NULL;
	if(buscarEnBibliotecaLocal){
		//Buscar plantilla en las cargadas anteriormente
		SI32 iCpo;
		for(iCpo=0; iCpo<_plantillasCuerpos->conteo && defCuerpo == NULL; iCpo++){
			if(((AUCuerpoDef*)_plantillasCuerpos->elemento[iCpo])->rutaRecurso.esIgual(rutaCuerpo)){
				defCuerpo = (AUCuerpoDef*)_plantillasCuerpos->elemento[iCpo];
				//PRINTF_INFO("PLANTILLA DE CUERPO encontrada en las previamente cargadas\n");
			}
		}
	}
	//Cargar desde un archivo (si es necesario)
	if(defCuerpo == NULL){
		const STNBSha1HashHex xHash = NBSha1_getHashHexBytes((const BYTE*)rutaCuerpo, AUCadena8::tamano(rutaCuerpo));
		AUCadenaMutable8* rutaVirtualCuerpo		= new(ENMemoriaTipo_Temporal) AUCadenaMutable8(_prefijoRutas->str()); rutaVirtualCuerpo->agregar(rutaCuerpo);
		AUCadenaMutable8* rutaVirtualCpoBin		= new(ENMemoriaTipo_Temporal) AUCadenaMutable8(_prefijoRutas->str()); rutaVirtualCpoBin->agregar(xHash.v); rutaVirtualCpoBin->agregar(".cpoBin");
		AUCadenaMutable8* rutaVirtualCpoCache	= new(ENMemoriaTipo_Temporal) AUCadenaMutable8(_prefijoRutasCache->str()); rutaVirtualCpoCache->agregar(xHash.v); rutaVirtualCpoCache->agregar(".cpoBin");
		AUCadenaMutable8* rutaCuerpoXML			= new(ENMemoriaTipo_Temporal) AUCadenaMutable8(NBGestorArchivos::rutaHaciaRecursoEnPaquete(rutaVirtualCuerpo->str()));
		AUCadenaMutable8* rutaCuerpoBIN			= new(ENMemoriaTipo_Temporal) AUCadenaMutable8(NBGestorArchivos::rutaHaciaRecursoEnPaquete(rutaVirtualCpoBin->str())); 
		AUCadenaMutable8* rutaCuerpoCache		= new(ENMemoriaTipo_Temporal) AUCadenaMutable8(NBGestorArchivos::rutaHaciaRecursoEnCacheLogica(rutaVirtualCpoCache->str()));
		//Intentar cargar binario precacheado (hermano del original)
		if(defCuerpo == NULL && _leerPrecacheCuerpos){
			//PRINTF_INFO("Cargando cuerpo: '%s'\n", rutaCuerpoBIN->str());
			defCuerpo = NBGestorCuerpos::privCargarPlantillaCuerpoDesdeBinario(rutaCuerpoBIN->str(), rutaCuerpo);
			if(defCuerpo != NULL){
				PRINTF_INFO("OPTIMIZACION, cuerpo cargado desde binario precacheado: '%s'\n", rutaCuerpoBIN->str());
			}
		}
		//Intentar cargar el archivo cache bianrio
		if(defCuerpo == NULL && _leerCacheCuerpos){
			defCuerpo = NBGestorCuerpos::privCargarPlantillaCuerpoDesdeBinario(rutaCuerpoCache->str(), rutaCuerpo);
			if(defCuerpo != NULL){
				PRINTF_INFO("OPTIMIZACION, cuerpo cargado desde binario cache: '%s'\n", rutaCuerpoCache->str());
			}
		}
		//Cargar desde el XML
		if(defCuerpo == NULL){
			AUDatosXMLMutable* datosXml = new(ENMemoriaTipo_Temporal) AUDatosXMLMutable(); NB_DEFINE_NOMBRE_PUNTERO(datosXml, "AUEscenaCuerpo::datosXml")
			if(datosXml->cargaDatosXMLDesdeArchivo(rutaCuerpoXML->str())){
				const sNodoXML* nodoRaiz = datosXml->nodoHijo("cuerpoFisica");
				if(nodoRaiz == NULL){
					PRINTF_ERROR("el nodo raiz del XML de cuerpo no existe\n");
				} else {
					AUCuerpoDef* nuevaPlantilla = new(ENMemoriaTipo_General) AUCuerpoDef(); NB_DEFINE_NOMBRE_PUNTERO(nuevaPlantilla, "AUEscenaCuerpo::nuevaPlantilla")
					if(!NBGestorCuerpos::privCargarPlantillaCuerpoDesdeXml(nuevaPlantilla, datosXml, nodoRaiz)){
						PRINTF_ERROR("interpretando plantilla de cuerpo\n");
					} else {
						nuevaPlantilla->rutaRecurso.establecer(rutaCuerpo);
						_plantillasCuerpos->agregarElemento(nuevaPlantilla);
						defCuerpo = (AUCuerpoDef*)_plantillasCuerpos->elemento[_plantillasCuerpos->conteo-1];
						//PRINTF_INFO("OK CUERPO, cargado desde XML: '%s'\n", rutaCuerpoXML->str());
						//Escribir cache binario
						if(_escribirCacheCuerpos){
							if(!NBGestorCuerpos::privGuardarPlantillaCuerpoEnBinario(defCuerpo, rutaCuerpoCache->str())){
								PRINTF_WARNING("no se pudo guardar la cache de cuerpo: '%s'\n", rutaCuerpoCache->str());
							}
						}
					}
					nuevaPlantilla->liberar(NB_RETENEDOR_NULL);
				}
			}
			datosXml->liberar(NB_RETENEDOR_NULL);
		}
		rutaCuerpoCache->liberar(NB_RETENEDOR_NULL);
		rutaCuerpoBIN->liberar(NB_RETENEDOR_NULL);
		rutaCuerpoXML->liberar(NB_RETENEDOR_NULL);
		rutaVirtualCpoCache->liberar(NB_RETENEDOR_NULL);
		rutaVirtualCpoBin->liberar(NB_RETENEDOR_NULL);
		rutaVirtualCuerpo->liberar(NB_RETENEDOR_NULL);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return defCuerpo;
}


AUEscenaCuerpo* NBGestorCuerpos::cuerpoDesdeArchivo(ENMemoriaTipo tipoMemoriaResidencia, const char* rutaCuerpo, bool buscarEnBibliotecaLocal){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorCuerpos::cuerpoDesdeArchivo")
	AUEscenaCuerpo* cuerpo	= NULL;
	AUCuerpoDef* defCuerpo = NBGestorCuerpos::definicionCuerpoDesdeArchivo(rutaCuerpo, buscarEnBibliotecaLocal);
	if(defCuerpo != NULL){
		cuerpo = new(tipoMemoriaResidencia) AUEscenaCuerpo(defCuerpo); NB_DEFINE_NOMBRE_PUNTERO(cuerpo, "AUEscenaCuerpo::cuerpo")
		cuerpo->autoLiberar(NB_RETENEDOR_NULL);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return cuerpo;
}

bool NBGestorCuerpos::privPuntoEstaDentroAlgunaFigura(float x, float y, AUArreglo* arregloFiguras){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorCuerpos::privPuntoEstaDentroAlgunaFigura")
	bool estaDentro = false;
	SI32 iFig;
	for(iFig=0; iFig<arregloFiguras->conteo && !estaDentro; iFig++){
		AUArregloNativoP<NBPunto>* verticesFig = (AUArregloNativoP<NBPunto>*)arregloFiguras->elemento[iFig];
		if(verticesFig->conteo>=3){
			estaDentro = true;
			SI32 iVert, iVertConteo = verticesFig->conteo; NBPunto posAnt = verticesFig->elemento[verticesFig->conteo-1];
			for(iVert=0; iVert<iVertConteo; iVert++){
				NBPunto posAct = verticesFig->elemento[iVert];
				if(!NBPUNTO_ESTA_IZQUIERDA_VECTOR(posAnt.x, posAnt.y, posAct.x, posAct.y, x, y)){
					estaDentro = false;
					break;
				}
				posAnt = posAct;
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return estaDentro;
}

bool NBGestorCuerpos::privCargarPlantillaCuerpoDesdeXml(AUCuerpoDef* nuevaPlantilla, const AUDatosXML* datosXml, const sNodoXML* nodoXml){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorCuerpos::privCargarPlantillaCuerpoDesdeXml")
	bool exito = false;
	//int versionXML = datosXml->nodoHijo("vXML", 0, nodoXml);
	//if(versionXML==1){
		nuevaPlantilla->multDanoProduce		= datosXml->nodoHijo("mDanoProd", 1.0f, nodoXml);
		nuevaPlantilla->multDanoRecibe		= datosXml->nodoHijo("mDanoRecb", 1.0f, nodoXml);
		//PRESENTACION
		AUCadenaMutable8* strTmp = new(ENMemoriaTipo_Temporal) AUCadenaMutable8();	NB_DEFINE_NOMBRE_PUNTERO(strTmp, "AUEscenaCuerpo::strTmp")
		AUCadenaMutable8* strTmp2 = new(ENMemoriaTipo_Temporal) AUCadenaMutable8(); NB_DEFINE_NOMBRE_PUNTERO(strTmp2, "AUEscenaCuerpo::strTmp2")
		AUCadenaMutable8* strTmp3 = new(ENMemoriaTipo_Temporal) AUCadenaMutable8();	NB_DEFINE_NOMBRE_PUNTERO(strTmp3, "AUEscenaCuerpo::strTmp3")
		//
		AUArregloMutable* capasCargadas					= new(ENMemoriaTipo_Temporal) AUArregloMutable();					NB_DEFINE_NOMBRE_PUNTERO(capasCargadas, "AUEditorNiveles::capasCargadas")
		AUArregloNativoMutableP<SI32>* idCapasCargadas	= new(ENMemoriaTipo_Temporal) AUArregloNativoMutableP<SI32>(1);	NB_DEFINE_NOMBRE_PUNTERO(idCapasCargadas, "AUEditorNiveles::idCapasCargadas")
		AUArregloMutable* objetosCargadosPorCapa		= new(ENMemoriaTipo_Temporal) AUArregloMutable();					NB_DEFINE_NOMBRE_PUNTERO(objetosCargadosPorCapa, "AUEditorNiveles::objetosCargadosPorCapa")
		AUArregloMutable* idsObjetosCargadosPorCapa		= new(ENMemoriaTipo_Temporal) AUArregloMutable();					NB_DEFINE_NOMBRE_PUNTERO(idsObjetosCargadosPorCapa, "AUEditorNiveles::idsObjetosCargadosPorCapa")
		AUArregloNativoMutableP<STCuerpoObjEscenaPrecargadoXML>* panelesSolPrecargados	= new(ENMemoriaTipo_Temporal) AUArregloNativoMutableP<STCuerpoObjEscenaPrecargadoXML>();	NB_DEFINE_NOMBRE_PUNTERO(panelesSolPrecargados, "AUEditorNiveles::panelesSolPrecargados")
		AUArregloNativoMutableP<STCuerpoObjEscenaPrecargadoXML>* bateriasPrecargados	= new(ENMemoriaTipo_Temporal) AUArregloNativoMutableP<STCuerpoObjEscenaPrecargadoXML>();	NB_DEFINE_NOMBRE_PUNTERO(bateriasPrecargados, "AUEditorNiveles::bateriasPrecargados")
		AUArregloNativoMutableP<STCuerpoObjEscenaPrecargadoXML>* bombillosPrecargados	= new(ENMemoriaTipo_Temporal) AUArregloNativoMutableP<STCuerpoObjEscenaPrecargadoXML>();	NB_DEFINE_NOMBRE_PUNTERO(bombillosPrecargados, "AUEditorNiveles::bombillosPrecargados")
		AUArregloNativoMutableP<STCuerpoObjEscenaPrecargadoXML>* lasersPrecargados		= new(ENMemoriaTipo_Temporal) AUArregloNativoMutableP<STCuerpoObjEscenaPrecargadoXML>();	NB_DEFINE_NOMBRE_PUNTERO(lasersPrecargados, "AUEditorNiveles::lasersPrecargados")
		AUArregloNativoMutableP<STCuerpoObjEscenaPrecargadoXML>* electornodosPrecargados = new(ENMemoriaTipo_Temporal) AUArregloNativoMutableP<STCuerpoObjEscenaPrecargadoXML>();	NB_DEFINE_NOMBRE_PUNTERO(electornodosPrecargados, "AUEditorNiveles::electornodosPrecargados")
		capasCargadas->agregarElemento(nuevaPlantilla->plantillaPresentacion);
		idCapasCargadas->agregarElemento(0);
		//
		AUArregloNativoMutableP<AUEscenaObjeto*>* objetosCargados = new(ENMemoriaTipo_Temporal) AUArregloNativoMutableP<AUEscenaObjeto*>(); NB_DEFINE_NOMBRE_PUNTERO(objetosCargados, "AUEditorCuerpos::objetosCargados")
		AUArregloNativoMutableP<SI32>* idsObjetosCargados = new(ENMemoriaTipo_Temporal) AUArregloNativoMutableP<SI32>(); NB_DEFINE_NOMBRE_PUNTERO(idsObjetosCargados, "AUEditorCuerpos::idsObjetosCargados")
		objetosCargadosPorCapa->agregarElemento(objetosCargados); objetosCargados->liberar(NB_RETENEDOR_NULL);
		idsObjetosCargadosPorCapa->agregarElemento(idsObjetosCargados); idsObjetosCargados->liberar(NB_RETENEDOR_NULL);
		//
		const sNodoXML* nodoPresent	= datosXml->nodoHijo("presentacion", nodoXml);
		if(nodoPresent != NULL){
			//PENDIENTE: CALCULAR POSICION EN BASE A LA ESCALA DE ARCHIVO Y DE PROGRAMA
			//float escalaPxPorMetro	= datosXml->nodoHijo("pxPorMetro", ESCALA_PIXELES_POR_METRO_MUNDO_FISICA, nodoPresent); 
			const sNodoXML* objsEscena	= datosXml->nodoHijo("objsEscena", nodoPresent);
			if(objsEscena != NULL){
				AUCadenaMutable8* strTipoObjEscena = new(ENMemoriaTipo_Temporal) AUCadenaMutable8(); NB_DEFINE_NOMBRE_PUNTERO(strTipoObjEscena, "AUEscenaCuerpo::strTipoObjEscena")
				const sNodoXML* objEscena; XML_NODO_HIJO(datosXml, objEscena, "obE", "objEscena", objsEscena, NULL);
				while(objEscena != NULL){
					XML_VALOR_STR_NODO_HIJO(datosXml, strTipoObjEscena, "tp", "tipo", "", objEscena, NULL)
					SI32 idObjEscena			= datosXml->nodoHijo("id", -1, objEscena);
					bool desprendible, oculto, aleatorio;
					const sNodoXML* nodoPropsObjDeCuerpo	= datosXml->nodoHijo("pObjC", objEscena);
					if(nodoPropsObjDeCuerpo != NULL){
						//Formato nuevo CSV (mas compacto)
						const char* cadenaCSV		= datosXml->cadenaValorDeNodo(nodoPropsObjDeCuerpo);
						SI32 tamCadenaCSV			= AUCadena8::tamano(cadenaCSV);
						SI32 posEnCadenaCSV			= 0;
						desprendible				= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, false);
						oculto						= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, false);
						aleatorio					= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, false);
					} else {
						//Formato viejo XML (muy inflado)
						desprendible				= datosXml->nodoHijo("desprendible", false, objEscena);
						oculto						= datosXml->nodoHijo("oculto", false, objEscena);
						aleatorio					= datosXml->nodoHijo("aleatorio", false, objEscena);
					}
					NBPunto traslado;				traslado.x = 0.0f; traslado.y = 0.0f;
					NBTamano escala;				escala.ancho = 1.0f; escala.alto = 1.0f;
					float rotacion					= 0.0f;
					NBColor8 multColor;				NBCOLOR_ESTABLECER(multColor, 255, 255, 255, 255)
					bool ilumDepend					= true;
					const sNodoXML* nodoProps;		XML_NODO_HIJO(datosXml, nodoProps, "pp", "props", objEscena, NULL)
					if(nodoProps != NULL){
						const sNodoXML* nodoEscena	= datosXml->nodoHijo("pEscn", nodoProps);
						if(nodoEscena != NULL){
							//Formato nuevo CSV (mas compacto)
							const char* cadenaCSV	= datosXml->cadenaValorDeNodo(nodoEscena);
							SI32 tamCadenaCSV		= AUCadena8::tamano(cadenaCSV);
							SI32 posEnCadenaCSV		= 0;
							traslado.x				= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0.0f);
							traslado.y				= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0.0f);
							escala.ancho			= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 1.0f);
							escala.alto				= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 1.0f);
							rotacion				= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0.0f);
							multColor.r				= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 255);
							multColor.g				= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 255);
							multColor.b				= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 255);
							multColor.a				= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 255);
							ilumDepend				= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, true);
						} else {
							//Formato viejo XML (muy inflado)
							//datosXML->nodoHijo("nombre", tmpStrNombre, "", nodoProps);
							traslado.x 		= datosXml->nodoHijo("posX", 0.0f, nodoProps);
							traslado.y 		= datosXml->nodoHijo("posY", 0.0f, nodoProps);
							escala.ancho	= datosXml->nodoHijo("escX", 1.0f, nodoProps);
							escala.alto		= datosXml->nodoHijo("escY", 1.0f, nodoProps);
							rotacion 		= datosXml->nodoHijo("rot", 0.0f, nodoProps);
							ilumDepend		= datosXml->nodoHijo("ilumDepend", true, nodoProps);
							const sNodoXML* nodoColor8 = datosXml->nodoHijo("color8", nodoProps);
							if(nodoColor8 != NULL){
								if(nodoColor8->nodosHijos == NULL){
									//Formato nuevo CSV (mas compacto)
									const char* cadenaCSV	= datosXml->cadenaValorDeNodo(nodoColor8);
									SI32 tamCadenaCSV		= AUCadena8::tamano(cadenaCSV);
									SI32 posEnCadenaCSV		= 0;
									multColor.r		= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 255);
									multColor.g		= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 255);
									multColor.b		= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 255);
									multColor.a		= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 255);
								} else {
									//Formato viejo XML (muy inflado)
									multColor.r	= datosXml->nodoHijo("r", 255, nodoColor8);
									multColor.g	= datosXml->nodoHijo("g", 255, nodoColor8);
									multColor.b	= datosXml->nodoHijo("b", 255, nodoColor8);
									multColor.a	= datosXml->nodoHijo("a", 255, nodoColor8);
								}
							} else {
								const sNodoXML* nodoColor = datosXml->nodoHijo("color", nodoProps);
								if(nodoColor != NULL){
									multColor.r	= 255.0f*datosXml->nodoHijo("r", 1.0f, nodoColor);
									multColor.g	= 255.0f*datosXml->nodoHijo("g", 1.0f, nodoColor);
									multColor.b	= 255.0f*datosXml->nodoHijo("b", 1.0f, nodoColor);
									multColor.a	= 255.0f*datosXml->nodoHijo("a", 1.0f, nodoColor);
								}
							}
						}
					}
					const sNodoXML* nodoDatos; XML_NODO_HIJO(datosXml, nodoDatos, "dt", "datos", objEscena, NULL)
					if(nodoDatos != NULL && strTipoObjEscena->tamano()>0){
						if(strTipoObjEscena->esIgual("AUEscenaLuzRadial")){
							AUEscenaLuzRadial* luz = new(nuevaPlantilla) AUEscenaLuzRadial(); NB_DEFINE_NOMBRE_PUNTERO(luz, "AUEscenaCuerpo::luz")
							if(AUEscenaLuzRadial::interpretarXmlInterno(luz, datosXml, nodoDatos)){
								nuevaPlantilla->plantillaPresentacion->agregarObjetoEscena(luz);
								luz->establecerVisible(!oculto);
								if(desprendible) nuevaPlantilla->indicesObjEscenaDesprendibles.agregarElemento(nuevaPlantilla->plantillaPresentacion->hijos()->conteo-1);
								if(aleatorio) nuevaPlantilla->indicesObjEscenaAleatorios.agregarElemento(nuevaPlantilla->plantillaPresentacion->hijos()->conteo-1);
								//
								objetosCargados->agregarElemento(luz);
								idsObjetosCargados->agregarElemento(idObjEscena);
							} else {
								PRINTF_ERROR("no se cargo adecuado AUEscenaLuzRadial desde XML\n");
								NBASSERT(false)
							}
							luz->liberar(NB_RETENEDOR_NULL);
						} else if(strTipoObjEscena->esIgual("AUEscenaLuzEspejo")){
							AUEscenaLuzEspejo* espejo = new(nuevaPlantilla) AUEscenaLuzEspejo(); NB_DEFINE_NOMBRE_PUNTERO(espejo, "AUEscenaCuerpo::espejo")
							if(AUEscenaLuzEspejo::interpretarXmlInterno(espejo, datosXml, nodoDatos)){
								nuevaPlantilla->plantillaPresentacion->agregarObjetoEscena(espejo);
								espejo->establecerVisible(!oculto);
								if(desprendible) nuevaPlantilla->indicesObjEscenaDesprendibles.agregarElemento(nuevaPlantilla->plantillaPresentacion->hijos()->conteo-1);
								if(aleatorio) nuevaPlantilla->indicesObjEscenaAleatorios.agregarElemento(nuevaPlantilla->plantillaPresentacion->hijos()->conteo-1);
								//
								objetosCargados->agregarElemento(espejo);
								idsObjetosCargados->agregarElemento(idObjEscena);
							} else {
								PRINTF_ERROR("no se cargo adecuado AUEscenaLuzRadial desde XML\n");
								NBASSERT(false)
							}
							espejo->liberar(NB_RETENEDOR_NULL);
						} else if(strTipoObjEscena->esIgual("AUEscenaSprite")){
							AUEscenaSprite* sprite = new(nuevaPlantilla) AUEscenaSprite();
							NB_DEFINE_NOMBRE_PUNTERO(sprite, "AUEscenaCuerpo::sprite")
							if(AUEscenaSprite::interpretarXmlInterno(sprite, datosXml, nodoDatos)){
								nuevaPlantilla->plantillaPresentacion->agregarObjetoEscena(sprite);
								sprite->establecerVisible(!oculto);
								if(desprendible) nuevaPlantilla->indicesObjEscenaDesprendibles.agregarElemento(nuevaPlantilla->plantillaPresentacion->hijos()->conteo-1);
								if(aleatorio) nuevaPlantilla->indicesObjEscenaAleatorios.agregarElemento(nuevaPlantilla->plantillaPresentacion->hijos()->conteo-1);
								//
								objetosCargados->agregarElemento(sprite);
								idsObjetosCargados->agregarElemento(idObjEscena);
							} else {
								PRINTF_ERROR("no se cargo adecuado AUEscenaSprite desde XML\n");
								NBASSERT(false)
							}
							sprite->liberar(NB_RETENEDOR_NULL);
						} else if(strTipoObjEscena->esIgual("AUEscenaGeneradorParticulas")){
							AUEscenaGeneradorParticulas* genParticulas = new(nuevaPlantilla) AUEscenaGeneradorParticulas(); NB_DEFINE_NOMBRE_PUNTERO(genParticulas, "AUEscenaCuerpo::genParticulas")
							if(AUEscenaGeneradorParticulas::interpretarXmlInterno(genParticulas, datosXml, nodoDatos)){
								nuevaPlantilla->plantillaPresentacion->agregarObjetoEscena(genParticulas); 
								genParticulas->establecerVisible(!oculto);
								if(desprendible) nuevaPlantilla->indicesObjEscenaDesprendibles.agregarElemento(nuevaPlantilla->plantillaPresentacion->hijos()->conteo-1);
								if(aleatorio) nuevaPlantilla->indicesObjEscenaAleatorios.agregarElemento(nuevaPlantilla->plantillaPresentacion->hijos()->conteo-1);
								//
								objetosCargados->agregarElemento(genParticulas);
								idsObjetosCargados->agregarElemento(idObjEscena);
							} else {
								PRINTF_ERROR("no se cargo adecuado AUEscenaSprite desde XML\n");
								NBASSERT(false)
							}
							genParticulas->liberar(NB_RETENEDOR_NULL);
						} else if(strTipoObjEscena->esIgual("AUEscenaCuerda")){
							AUTextura* texturaPlantilla = NBGestorTexturas::texturaDesdeArchivo("Personajes/Gancho/perGanchoDodoPatronCuerda.png");
							AUEscenaCuerda* cuerda = new(nuevaPlantilla) AUEscenaCuerda(ENCuerdaTipo_Continua, texturaPlantilla, texturaPlantilla->tamanoHD().ancho*3.0f, 0.01f, 0.0f); NB_DEFINE_NOMBRE_PUNTERO(cuerda, "AUEscenaCuerpo::cuerda")
							if(AUEscenaCuerda::interpretarXmlInterno(cuerda, datosXml, nodoDatos)){
								nuevaPlantilla->plantillaPresentacion->agregarObjetoEscena(cuerda);
								cuerda->establecerVisible(!oculto);
								if(desprendible) nuevaPlantilla->indicesObjEscenaDesprendibles.agregarElemento(nuevaPlantilla->plantillaPresentacion->hijos()->conteo-1);
								if(aleatorio) nuevaPlantilla->indicesObjEscenaAleatorios.agregarElemento(nuevaPlantilla->plantillaPresentacion->hijos()->conteo-1);
								//
								objetosCargados->agregarElemento(cuerda);
								idsObjetosCargados->agregarElemento(idObjEscena);
							} else {
								PRINTF_ERROR("no se cargo adecuado AUEscenaCuerda desde XML\n");
								NBASSERT(false)
							}
							cuerda->liberar(NB_RETENEDOR_NULL);
						} else if(strTipoObjEscena->esIgual("AUEscenaMallaGelatina")){
							AUEscenaMallaGelatina* mallaGelatina = new(nuevaPlantilla) AUEscenaMallaGelatina(); NB_DEFINE_NOMBRE_PUNTERO(mallaGelatina, "AUEscenaCuerpo::mallaGelatina")
							if(AUEscenaMallaGelatina::interpretarXmlInterno(mallaGelatina, datosXml, nodoDatos)){
								nuevaPlantilla->plantillaPresentacion->agregarObjetoEscena(mallaGelatina);
								mallaGelatina->establecerVisible(!oculto);
								if(desprendible) nuevaPlantilla->indicesObjEscenaDesprendibles.agregarElemento(nuevaPlantilla->plantillaPresentacion->hijos()->conteo-1);
								if(aleatorio) nuevaPlantilla->indicesObjEscenaAleatorios.agregarElemento(nuevaPlantilla->plantillaPresentacion->hijos()->conteo-1);
								//
								objetosCargados->agregarElemento(mallaGelatina);
								idsObjetosCargados->agregarElemento(idObjEscena);
							} else {
								PRINTF_ERROR("no se cargo adecuado AUEscenaMallaGelatina desde XML\n");
								NBASSERT(false)
							}
							mallaGelatina->liberar(NB_RETENEDOR_NULL);
						} else if(strTipoObjEscena->esIgual("AUEscenaMallaConHueso")){
							AUEscenaMallaConHueso* mallaHueso = new(nuevaPlantilla) AUEscenaMallaConHueso(); NB_DEFINE_NOMBRE_PUNTERO(mallaHueso, "AUEscenaCuerpo::mallaHueso")
							if(AUEscenaMallaConHueso::interpretarXmlInterno(mallaHueso, datosXml, nodoDatos)){
								nuevaPlantilla->plantillaPresentacion->agregarObjetoEscena(mallaHueso);
								mallaHueso->establecerVisible(!oculto);
								if(desprendible) nuevaPlantilla->indicesObjEscenaDesprendibles.agregarElemento(nuevaPlantilla->plantillaPresentacion->hijos()->conteo-1);
								if(aleatorio) nuevaPlantilla->indicesObjEscenaAleatorios.agregarElemento(nuevaPlantilla->plantillaPresentacion->hijos()->conteo-1);
								//
								objetosCargados->agregarElemento(mallaHueso);
								idsObjetosCargados->agregarElemento(idObjEscena);
							} else {
								PRINTF_ERROR("no se cargo adecuado AUEscenaMallaConHueso desde XML\n");
								NBASSERT(false)
							}
							mallaHueso->liberar(NB_RETENEDOR_NULL);
						} else if(strTipoObjEscena->esIgual("AUEscenaFigura")){
							AUEscenaFigura* figura = new(nuevaPlantilla) AUEscenaFigura(ENEscenaFiguraTipo_PoligonoCerrado); NB_DEFINE_NOMBRE_PUNTERO(figura, "AUEscenaCuerpo::figura")
							if(AUEscenaFigura::interpretarXmlInterno(figura, datosXml, nodoDatos)){
								nuevaPlantilla->plantillaPresentacion->agregarObjetoEscena(figura);
								figura->establecerVisible(!oculto);
								if(desprendible) nuevaPlantilla->indicesObjEscenaDesprendibles.agregarElemento(nuevaPlantilla->plantillaPresentacion->hijos()->conteo-1);
								if(aleatorio) nuevaPlantilla->indicesObjEscenaAleatorios.agregarElemento(nuevaPlantilla->plantillaPresentacion->hijos()->conteo-1);
								//
								objetosCargados->agregarElemento(figura);
								idsObjetosCargados->agregarElemento(idObjEscena);
							} else {
								PRINTF_ERROR("no se cargo adecuado AUFigura desde XML\n");
								NBASSERT(false)
							}
							figura->liberar(NB_RETENEDOR_NULL);
						} else if(strTipoObjEscena->esIgual("AUEscenaCuerpo")){
							datosXml->nodoHijo("URL", strTmp, "", nodoDatos);
							if(strTmp->tamano()>0){
								SI32 tipoCuerpo					= datosXml->nodoHijo("tipoCuerpo", (SI32)ENCuerpoTipo_Estatico, nodoDatos);
								float escalaGravedad			= datosXml->nodoHijo("escGrav", 1.0f, nodoDatos);
								float multDensidad				= datosXml->nodoHijo("mD", 1.0f, nodoDatos);
								float multFriccion				= datosXml->nodoHijo("mF", 1.0f, nodoDatos);
								float multRestituc				= datosXml->nodoHijo("mR", 1.0f, nodoDatos);
								UI8 mascaraProps				= (datosXml->nodoHijo("activo", 1, nodoDatos)==1?AUCUERPODEF_MASCARA_ACTIVO:0)+(datosXml->nodoHijo("dormir", 1, nodoDatos)==1?AUCUERPODEF_MASCARA_PERMITIR_DORMIR:0)+(datosXml->nodoHijo("rotFija", 0, nodoDatos)==1?AUCUERPODEF_MASCARA_ROTACION_FIJA:0)+(datosXml->nodoHijo("esBala", 0, nodoDatos)==1?AUCUERPODEF_MASCARA_ES_BALA:0);
								AUEscenaCuerpo* nuevoCuerpo		= NBGestorCuerpos::cuerpoDesdeArchivo(ENMemoriaTipo_General, strTmp->str()); //es autoliberado y el agrupador de sombras sera defindo luego
								if(nuevoCuerpo != NULL){
									nuevoCuerpo->establecerCreacionCuerpoTipo((ENCuerpoTipo)tipoCuerpo, ENAnimFisicaTipo_SeguirCuerpo);
									nuevoCuerpo->establecerCreacionCuerpoMascara(mascaraProps);
									nuevoCuerpo->establecerCreacionCuerpoFactorGravedad(escalaGravedad);
									nuevoCuerpo->establecerMultiplicadorDensidad(multDensidad);
									nuevoCuerpo->establecerMultiplicadorFriccion(multFriccion);
									nuevoCuerpo->establecerMultiplicadorRestitucion(multRestituc);
									nuevoCuerpo->establecerIluminoDependencia(ilumDepend);
									nuevoCuerpo->establecerMultiplicadorColor8(multColor);
									nuevoCuerpo->establecerTraslacion(traslado);
									nuevoCuerpo->establecerEscalacion(escala);
									nuevoCuerpo->establecerRotacion(rotacion);
									nuevaPlantilla->plantillaPresentacion->agregarObjetoEscena(nuevoCuerpo);
									nuevoCuerpo->establecerVisible(!oculto);
									if(desprendible) nuevaPlantilla->indicesObjEscenaDesprendibles.agregarElemento(nuevaPlantilla->plantillaPresentacion->hijos()->conteo-1);
									if(aleatorio) nuevaPlantilla->indicesObjEscenaAleatorios.agregarElemento(nuevaPlantilla->plantillaPresentacion->hijos()->conteo-1);
									//
									objetosCargados->agregarElemento(nuevoCuerpo);
									idsObjetosCargados->agregarElemento(idObjEscena);
								} else {
									PRINTF_ERROR("cargando CUERPO: '%s'\n", strTmp->str());
									NBASSERT(false)
								}
							}
						} else if(strTipoObjEscena->esIgual("AUEscenaAnimacion")){
							datosXml->nodoHijo("URL", strTmp, "", nodoDatos);
							if(strTmp->tamano()>0){
								AUEscenaAnimacionI* nuevaAnim = (AUEscenaAnimacionI*)_cargadorAnimaciones->cargarAnimacion(ENMemoriaTipo_General, strTmp->str()); //luego se le definira su agrupador de sombras
								if(nuevaAnim != NULL){
									NB_DEFINE_NOMBRE_PUNTERO(nuevaAnim, "AUEscenaCuerpo::nuevaAnim");
									nuevaAnim->establecerReproducirAlCargar(datosXml->nodoHijo("reproIni", true, nodoDatos));
									nuevaAnim->establecerRepetirAnimacion(datosXml->nodoHijo("repetir", true, nodoDatos));
									nuevaAnim->establecerReproducirSonidos(datosXml->nodoHijo("repSonidos", true, nodoDatos));
									nuevaAnim->establecerSuavizarAnimacion(datosXml->nodoHijo("suavizar", false, nodoDatos));
									nuevaAnim->establecerSegundosEsperarParaRepetir(datosXml->nodoHijo("segsEntreRep", 0.0f, nodoDatos));
									nuevaAnim->establecerFactorVelocidadReproduccion(datosXml->nodoHijo("velAnim", 1.0f, nodoDatos));
									nuevaAnim->establecerIluminoDependencia(ilumDepend);
									nuevaAnim->establecerMultiplicadorColor8(multColor);
									nuevaAnim->establecerTraslacion(traslado);
									nuevaAnim->establecerEscalacion(escala);
									nuevaAnim->establecerRotacion(rotacion);
									nuevaPlantilla->plantillaPresentacion->agregarObjetoEscena(nuevaAnim);
									nuevaAnim->establecerVisible(!oculto);
									if(desprendible) nuevaPlantilla->indicesObjEscenaDesprendibles.agregarElemento(nuevaPlantilla->plantillaPresentacion->hijos()->conteo-1);
									if(aleatorio) nuevaPlantilla->indicesObjEscenaAleatorios.agregarElemento(nuevaPlantilla->plantillaPresentacion->hijos()->conteo-1);
									//nuevaAnim->reanudarAnimacion();
									//
									objetosCargados->agregarElemento(nuevaAnim);
									idsObjetosCargados->agregarElemento(idObjEscena);
								} else {
									NBASSERT(false)
								}
							}
						} else if(strTipoObjEscena->esIgual("AUBateria")){
							AUBateria* bateria = new(nuevaPlantilla) AUBateria(); NB_DEFINE_NOMBRE_PUNTERO(bateria, "AUEditorNiveles::bateria")
							nuevaPlantilla->plantillaPresentacion->agregarObjetoEscena(bateria);
							bateria->liberar(NB_RETENEDOR_NULL);
							//
							objetosCargados->agregarElemento(bateria);
							idsObjetosCargados->agregarElemento(idObjEscena);
							STCuerpoObjEscenaPrecargadoXML datosPrecagado;
							datosPrecagado.objPrecargado = bateria;
							datosPrecagado.nodoDatosXML = nodoDatos;
							bateriasPrecargados->agregarElemento(datosPrecagado);
						} else if(strTipoObjEscena->esIgual("AUBombillo")){
							AUBombillo* bombillo = new(nuevaPlantilla) AUBombillo(); NB_DEFINE_NOMBRE_PUNTERO(bombillo, "AUEditorNiveles::bombillo")
							nuevaPlantilla->plantillaPresentacion->agregarObjetoEscena(bombillo);
							bombillo->liberar(NB_RETENEDOR_NULL);
							//
							objetosCargados->agregarElemento(bombillo);
							idsObjetosCargados->agregarElemento(idObjEscena);
							STCuerpoObjEscenaPrecargadoXML datosPrecagado;
							datosPrecagado.objPrecargado = bombillo;
							datosPrecagado.nodoDatosXML = nodoDatos;
							bombillosPrecargados->agregarElemento(datosPrecagado);
						} else if(strTipoObjEscena->esIgual("AULaser")){
							AULaser* laser = new(nuevaPlantilla) AULaser(); NB_DEFINE_NOMBRE_PUNTERO(laser, "AUEditorNiveles::laser")
							nuevaPlantilla->plantillaPresentacion->agregarObjetoEscena(laser);
							laser->liberar(NB_RETENEDOR_NULL);
							//
							objetosCargados->agregarElemento(laser);
							idsObjetosCargados->agregarElemento(idObjEscena);
							STCuerpoObjEscenaPrecargadoXML datosPrecagado;
							datosPrecagado.objPrecargado = laser;
							datosPrecagado.nodoDatosXML = nodoDatos;
							lasersPrecargados->agregarElemento(datosPrecagado);
						} else if(strTipoObjEscena->esIgual("AUPanelSolar")){
							AUPanelSolar* panelSolar = new(nuevaPlantilla) AUPanelSolar(); NB_DEFINE_NOMBRE_PUNTERO(panelSolar, "AUEditorNiveles::panelSolar")
							nuevaPlantilla->plantillaPresentacion->agregarObjetoEscena(panelSolar);
							panelSolar->liberar(NB_RETENEDOR_NULL);
							//
							objetosCargados->agregarElemento(panelSolar);
							idsObjetosCargados->agregarElemento(idObjEscena);
							STCuerpoObjEscenaPrecargadoXML datosPrecagado;
							datosPrecagado.objPrecargado = panelSolar;
							datosPrecagado.nodoDatosXML = nodoDatos;
							panelesSolPrecargados->agregarElemento(datosPrecagado);
						} else if(strTipoObjEscena->esIgual("AUElectronodo")){
							AUElectronodo* electronodo = new(nuevaPlantilla) AUElectronodo(); NB_DEFINE_NOMBRE_PUNTERO(electronodo, "AUEditorNiveles::electronodo")
							nuevaPlantilla->plantillaPresentacion->agregarObjetoEscena(electronodo);
							electronodo->liberar(NB_RETENEDOR_NULL);
							//
							objetosCargados->agregarElemento(electronodo);
							idsObjetosCargados->agregarElemento(idObjEscena);
							STCuerpoObjEscenaPrecargadoXML datosPrecagado;
							datosPrecagado.objPrecargado = electronodo;
							datosPrecagado.nodoDatosXML = nodoDatos;
							electornodosPrecargados->agregarElemento(datosPrecagado);
						} else if(strTipoObjEscena->esIgual("AULanzallamas")){
							AULanzallamas* lanzallamas = new(nuevaPlantilla) AULanzallamas(); NB_DEFINE_NOMBRE_PUNTERO(lanzallamas, "AUEditorNiveles::lanzallamas")
							if(AULanzallamas::interpretarXmlInterno(lanzallamas, datosXml, nodoDatos)){
								nuevaPlantilla->plantillaPresentacion->agregarObjetoEscena(lanzallamas);
								//
								objetosCargados->agregarElemento(lanzallamas);
								idsObjetosCargados->agregarElemento(idObjEscena);
							}
							lanzallamas->liberar(NB_RETENEDOR_NULL);
						} else if(strTipoObjEscena->esIgual("AUEscenaUnion")){
							XML_VALOR_STR_NODO_HIJO(datosXml, strTmp2, "tp", "tipo", "", nodoDatos, NULL)
							SI32 idCuerpoA = datosXml->nodoHijo("idCuerpoA", -1, nodoDatos);
							SI32 idCuerpoB = datosXml->nodoHijo("idCuerpoB", -1, nodoDatos);
							SI32 indiceObjCargadoA = -1;
							SI32 indiceObjCargadoB = -1;
							AUEscenaCuerpo* cuerpoA = NULL;
							AUEscenaCuerpo* cuerpoB = NULL;
							if(idCuerpoA==-1){
								cuerpoA = NULL;
							} else {
								indiceObjCargadoA = idsObjetosCargados->indiceDe(idCuerpoA);
								if(indiceObjCargadoA!=-1){
									AUEscenaObjeto* objA = objetosCargados->elemento[indiceObjCargadoA];
									if(objA->esClase(AUEscenaCuerpo::idTipoClase)) cuerpoA = (AUEscenaCuerpo*)objA;
								}
							}
							if(idCuerpoB==-1){
								cuerpoB = NULL;
							} else {
								indiceObjCargadoB = idsObjetosCargados->indiceDe(idCuerpoB);
								if(indiceObjCargadoB!=-1){
									AUEscenaObjeto* objB = objetosCargados->elemento[indiceObjCargadoB];
									if(objB->esClase(AUEscenaCuerpo::idTipoClase)) cuerpoB = (AUEscenaCuerpo*)objB;
								}
							}
							if(cuerpoA==cuerpoB){
								PRINTF_ERROR("cargando union, cuerpos no encontrados o iguales\n");
							} else {
								if(strTmp2->esIgual("Soldadura")){
									STUnionCuerposDef nuevaUnion;
									nuevaUnion.tipoUnion			= ENUnionCuerpoTipo_Soldadura;
									nuevaUnion.indiceCuerpoA		= indiceObjCargadoA;
									nuevaUnion.indiceCuerpoB		= indiceObjCargadoB;
#									ifndef CONFIG_NB_UNSUPPORT_BOX2D
									b2WeldJointDef* defUnion		= new b2WeldJointDef(); NB_DEFINE_NOMBRE_PUNTERO(defUnion, "AUEscenaCuerpo::defUnion")
									nuevaUnion.b2Definicion			= defUnion;
									nuevaUnion.estructuraAdicional	= NULL;
									const sNodoXML* nodoPosA		= datosXml->nodoHijo("posAnclaA", nodoDatos);
									if(nodoPosA != NULL){
										defUnion->localAnchorA.x	= datosXml->nodoHijo("x", 0.0f, nodoPosA);
										defUnion->localAnchorA.y	= datosXml->nodoHijo("y", 0.0f, nodoPosA);
									}
									const sNodoXML* nodoPosB		= datosXml->nodoHijo("posAnclaB", nodoDatos);
									if(nodoPosB != NULL){
										defUnion->localAnchorB.x	= datosXml->nodoHijo("x", 0.0f, nodoPosB);
										defUnion->localAnchorB.y	= datosXml->nodoHijo("y", 0.0f, nodoPosB);
									}
									defUnion->collideConnected		= datosXml->nodoHijo("colisionActiva", false, nodoDatos);
									defUnion->referenceAngle		= datosXml->nodoHijo("radianesRef", 0.0f, nodoDatos);
#									endif
									nuevaPlantilla->defUnionesCuerpos.agregarElemento(nuevaUnion);
								} else if(strTmp2->esIgual("Rotacion")){
									STUnionCuerposDef nuevaUnion;
									nuevaUnion.tipoUnion			= ENUnionCuerpoTipo_Rotacion;
									nuevaUnion.indiceCuerpoA		= indiceObjCargadoA;
									nuevaUnion.indiceCuerpoB		= indiceObjCargadoB;
#									ifndef CONFIG_NB_UNSUPPORT_BOX2D
									b2RevoluteJointDef* defUnion	= new b2RevoluteJointDef(); NB_DEFINE_NOMBRE_PUNTERO(defUnion, "AUEscenaCuerpo::defUnion")
									nuevaUnion.b2Definicion			= defUnion;
									nuevaUnion.estructuraAdicional	= NULL;
									const sNodoXML* nodoPosA		= datosXml->nodoHijo("posAnclaA", nodoDatos);
									if(nodoPosA != NULL){
										defUnion->localAnchorA.x	= datosXml->nodoHijo("x", 0.0f, nodoPosA);
										defUnion->localAnchorA.y	= datosXml->nodoHijo("y", 0.0f, nodoPosA);
									}
									const sNodoXML* nodoPosB		= datosXml->nodoHijo("posAnclaB", nodoDatos);
									if(nodoPosB != NULL){
										defUnion->localAnchorB.x	= datosXml->nodoHijo("x", 0.0f, nodoPosB);
										defUnion->localAnchorB.y	= datosXml->nodoHijo("y", 0.0f, nodoPosB);
									}
									defUnion->collideConnected		= datosXml->nodoHijo("colisionActiva", false, nodoDatos);
									defUnion->enableLimit			= datosXml->nodoHijo("limitesDef", false, nodoDatos);
									defUnion->lowerAngle			= datosXml->nodoHijo("radMin", 0.0f, nodoDatos);
									defUnion->upperAngle			= datosXml->nodoHijo("radMax", PIx2, nodoDatos);
									defUnion->enableMotor			= datosXml->nodoHijo("motorDef", false, nodoDatos);
									defUnion->motorSpeed			= datosXml->nodoHijo("motorVel", 0.0f, nodoDatos);
									defUnion->maxMotorTorque		= datosXml->nodoHijo("motorMaxTorq", 0.0f, nodoDatos);
#									endif
									nuevaPlantilla->defUnionesCuerpos.agregarElemento(nuevaUnion);
								} else if(strTmp2->esIgual("Distancia")){
									STUnionCuerposDef nuevaUnion;
									nuevaUnion.tipoUnion			= ENUnionCuerpoTipo_Distancia;
									nuevaUnion.indiceCuerpoA		= indiceObjCargadoA;
									nuevaUnion.indiceCuerpoB		= indiceObjCargadoB;
#									ifndef CONFIG_NB_UNSUPPORT_BOX2D
									b2DistanceJointDef* defUnion	= new b2DistanceJointDef(); NB_DEFINE_NOMBRE_PUNTERO(defUnion, "AUEscenaCuerpo::defUnion")
									nuevaUnion.b2Definicion			= defUnion;
									nuevaUnion.estructuraAdicional	= NULL;
									const sNodoXML* nodoPosA		= datosXml->nodoHijo("posAnclaA", nodoDatos);
									if(nodoPosA != NULL){
										defUnion->localAnchorA.x	= datosXml->nodoHijo("x", 0.0f, nodoPosA);
										defUnion->localAnchorA.y	= datosXml->nodoHijo("y", 0.0f, nodoPosA);
									}
									const sNodoXML* nodoPosB		= datosXml->nodoHijo("posAnclaB", nodoDatos);
									if(nodoPosB != NULL){
										defUnion->localAnchorB.x	= datosXml->nodoHijo("x", 0.0f, nodoPosB);
										defUnion->localAnchorB.y	= datosXml->nodoHijo("y", 0.0f, nodoPosB);
									}
									defUnion->collideConnected		= datosXml->nodoHijo("colisionActiva", false, nodoDatos);
									defUnion->length				= datosXml->nodoHijo("longitud", 0.0f, nodoDatos);
									defUnion->frequencyHz			= datosXml->nodoHijo("amortFrecuencia", 0.0f, nodoDatos);
									defUnion->dampingRatio			= datosXml->nodoHijo("amortCoeficiente", 0.0f, nodoDatos);
#									endif
									nuevaPlantilla->defUnionesCuerpos.agregarElemento(nuevaUnion);
								} else if(strTmp2->esIgual("Cuerda")){
									STUnionCuerposDef nuevaUnion;
									nuevaUnion.tipoUnion			= ENUnionCuerpoTipo_Cuerda;
									nuevaUnion.indiceCuerpoA		= indiceObjCargadoA;
									nuevaUnion.indiceCuerpoB		= indiceObjCargadoB;
#									ifndef CONFIG_NB_UNSUPPORT_BOX2D
									b2RopeJointDef* defUnion		= new b2RopeJointDef(); NB_DEFINE_NOMBRE_PUNTERO(defUnion, "AUEscenaCuerpo::defUnion")
									nuevaUnion.b2Definicion			= defUnion;
									nuevaUnion.estructuraAdicional	= NULL;
									const sNodoXML* nodoPosA		= datosXml->nodoHijo("posAnclaA", nodoDatos);
									if(nodoPosA != NULL){
										defUnion->localAnchorA.x	= datosXml->nodoHijo("x", 0.0f, nodoPosA);
										defUnion->localAnchorA.y	= datosXml->nodoHijo("y", 0.0f, nodoPosA);
									}
									const sNodoXML* nodoPosB		= datosXml->nodoHijo("posAnclaB", nodoDatos);
									if(nodoPosB != NULL){
										defUnion->localAnchorB.x	= datosXml->nodoHijo("x", 0.0f, nodoPosB);
										defUnion->localAnchorB.y	= datosXml->nodoHijo("y", 0.0f, nodoPosB);
									}
									defUnion->collideConnected		= datosXml->nodoHijo("colisionActiva", false, nodoDatos);
									defUnion->maxLength				= datosXml->nodoHijo("longitud", 0.0f, nodoDatos);
									STUnionCuerposCuerdaDef* datosCuerda	= new STUnionCuerposCuerdaDef(); NB_DEFINE_NOMBRE_PUNTERO(datosCuerda, "AUEscenaCuerpo::datosCuerda")
									datosCuerda->tipoCuerda				= (ENCuerdaTipo)datosXml->nodoHijo("tipoCuerda", (SI32)ENCuerdaTipo_Continua, nodoDatos); NBASSERT(datosCuerda->tipoCuerda>=0 && datosCuerda->tipoCuerda<ENCuerdaTipo_Conteo)
									datosCuerda->masaSegmentos			= datosXml->nodoHijo("masa", 0.01f, nodoDatos);
									datosCuerda->factorEstiramiento		= datosXml->nodoHijo("estira", 0.01f, nodoDatos);
									datosCuerda->superposicionSegmentos	= datosXml->nodoHijo("superpos", 0.0f, nodoDatos);
									datosXml->nodoHijo("texCuerda", strTmp3, "", nodoDatos);
									datosCuerda->texturaCuerda			= NBGestorTexturas::texturaDesdeArchivo(strTmp3->str()); if(datosCuerda->texturaCuerda != NULL) datosCuerda->texturaCuerda->retener(NB_RETENEDOR_NULL);
									nuevaUnion.estructuraAdicional		= datosCuerda;
#									endif
									nuevaPlantilla->defUnionesCuerpos.agregarElemento(nuevaUnion);
								} else if(strTmp2->esIgual("Friccion")){
									STUnionCuerposDef nuevaUnion;
									nuevaUnion.tipoUnion			= ENUnionCuerpoTipo_Friccion;
									nuevaUnion.indiceCuerpoA		= indiceObjCargadoA;
									nuevaUnion.indiceCuerpoB		= indiceObjCargadoB;
#									ifndef CONFIG_NB_UNSUPPORT_BOX2D
									b2FrictionJointDef* defUnion	= new b2FrictionJointDef(); NB_DEFINE_NOMBRE_PUNTERO(defUnion, "AUEscenaCuerpo::defUnion")
									nuevaUnion.b2Definicion			= defUnion;
									nuevaUnion.estructuraAdicional	= NULL;
									const sNodoXML* nodoPosA		= datosXml->nodoHijo("posAnclaA", nodoDatos);
									if(nodoPosA != NULL){
										defUnion->localAnchorA.x	= datosXml->nodoHijo("x", 0.0f, nodoPosA);
										defUnion->localAnchorA.y	= datosXml->nodoHijo("y", 0.0f, nodoPosA);
									}
									const sNodoXML* nodoPosB		= datosXml->nodoHijo("posAnclaB", nodoDatos);
									if(nodoPosB != NULL){
										defUnion->localAnchorB.x	= datosXml->nodoHijo("x", 0.0f, nodoPosB);
										defUnion->localAnchorB.y	= datosXml->nodoHijo("y", 0.0f, nodoPosB);
									}
									defUnion->collideConnected		= datosXml->nodoHijo("colisionActiva", false, nodoDatos);
									defUnion->maxForce				= datosXml->nodoHijo("fuerzaMax", 0.0f, nodoDatos);
									defUnion->maxTorque				= datosXml->nodoHijo("torqueMax", 0.0f, nodoDatos);
#									endif
									nuevaPlantilla->defUnionesCuerpos.agregarElemento(nuevaUnion);
								} else if(strTmp2->esIgual("Prismatica")){
									STUnionCuerposDef nuevaUnion;
									nuevaUnion.tipoUnion			= ENUnionCuerpoTipo_Prismatica;
									nuevaUnion.indiceCuerpoA		= indiceObjCargadoA;
									nuevaUnion.indiceCuerpoB		= indiceObjCargadoB;
#									ifndef CONFIG_NB_UNSUPPORT_BOX2D
									b2PrismaticJointDef* defUnion	= new b2PrismaticJointDef(); NB_DEFINE_NOMBRE_PUNTERO(defUnion, "AUEscenaCuerpo::defUnion")
									nuevaUnion.b2Definicion			= defUnion;
									nuevaUnion.estructuraAdicional	= NULL;
									defUnion->referenceAngle		= datosXml->nodoHijo("radRef", 0.0f, nodoDatos);
									const sNodoXML* nodoPosA		= datosXml->nodoHijo("posAnclaA", nodoDatos);
									if(nodoPosA != NULL){
										defUnion->localAnchorA.x	= datosXml->nodoHijo("x", 0.0f, nodoPosA);
										defUnion->localAnchorA.y	= datosXml->nodoHijo("y", 0.0f, nodoPosA);
									}
									const sNodoXML* nodoPosB		= datosXml->nodoHijo("posAnclaB", nodoDatos);
									if(nodoPosB != NULL){
										defUnion->localAnchorB.x	= datosXml->nodoHijo("x", 0.0f, nodoPosB);
										defUnion->localAnchorB.y	= datosXml->nodoHijo("y", 0.0f, nodoPosB);
									}
									defUnion->collideConnected		= datosXml->nodoHijo("colisionActiva", false, nodoDatos);
									defUnion->enableLimit			= datosXml->nodoHijo("limitesDef", false, nodoDatos);
									const sNodoXML* nodoVector		= datosXml->nodoHijo("vecDir", nodoDatos);
									if(nodoVector != NULL){
										defUnion->localAxisA.x		= datosXml->nodoHijo("x", 1.0f, nodoVector);
										defUnion->localAxisA.y		= datosXml->nodoHijo("y", 0.0f, nodoVector);
									}
									defUnion->lowerTranslation		= datosXml->nodoHijo("trasMin", 0.0f, nodoDatos);
									defUnion->upperTranslation		= datosXml->nodoHijo("trasMax", 0.001f, nodoDatos);
									defUnion->enableMotor			= datosXml->nodoHijo("motorDef", false, nodoDatos);
									defUnion->motorSpeed			= datosXml->nodoHijo("motorVel", 0.0f, nodoDatos);
									defUnion->maxMotorForce			= datosXml->nodoHijo("motorFuerza", 0.0f, nodoDatos);
#									endif
									nuevaPlantilla->defUnionesCuerpos.agregarElemento(nuevaUnion);
								} else {
									NBASSERT(false) //tipode union no valida
								}
							}
						} else {
							PRINTF_ERROR("cargando XML cuerpo: objeto tipo '%s' no es interpretable por esta version\n", strTipoObjEscena->str());
							NBASSERT(false) //objetoEscena no valido
						}
					}
					//
					XML_NODO_HIJO(datosXml, objEscena, "obE", "objEscena", objsEscena, objEscena);
				}
				strTipoObjEscena->liberar(NB_RETENEDOR_NULL);
			}
		}
		//Cargar paneles precargados (se deben cargar al final debido a que hacen posible referencia a objetos que estan delante de el en la lista de "Elementos")
		SI32 iPanelSol, iPanelSolConteo = panelesSolPrecargados->conteo;
		for(iPanelSol=0; iPanelSol<iPanelSolConteo; iPanelSol++){
			STCuerpoObjEscenaPrecargadoXML* datosPrecargado = &(panelesSolPrecargados->elemento[iPanelSol]);
			AUPanelSolar* panelSolar = (AUPanelSolar*)datosPrecargado->objPrecargado;
			if(!AUPanelSolar::interpretarXmlInterno(panelSolar, datosXml, datosPrecargado->nodoDatosXML, objetosCargados, idsObjetosCargados)){
				NBASSERT(false)
			}
		}
		//Cargar baterias precargados (se deben cargar al final debido a que hacen posible referencia a objetos que estan delante de el en la lista de "Elementos")
		SI32 iBateria, iBateriaConteo = bateriasPrecargados->conteo;
		for(iBateria=0; iBateria<iBateriaConteo; iBateria++){
			STCuerpoObjEscenaPrecargadoXML* datosPrecargado = &(bateriasPrecargados->elemento[iBateria]);
			AUBateria* bateria = (AUBateria*)datosPrecargado->objPrecargado;
			if(!AUBateria::interpretarXmlInterno(bateria, datosXml, datosPrecargado->nodoDatosXML, objetosCargados, idsObjetosCargados)){
				NBASSERT(false)
			}
		}
		//Cargar bombillos precargados (se deben cargar al final debido a que hacen posible referencia a objetos que estan delante de el en la lista de "Elementos")
		SI32 iBombillo, iBombilloConteo = bombillosPrecargados->conteo;
		for(iBombillo=0; iBombillo<iBombilloConteo; iBombillo++){
			STCuerpoObjEscenaPrecargadoXML* datosPrecargado = &(bombillosPrecargados->elemento[iBombillo]);
			AUBombillo* bombillo = (AUBombillo*)datosPrecargado->objPrecargado;
			if(!AUBombillo::interpretarXmlInterno(bombillo, datosXml, datosPrecargado->nodoDatosXML, objetosCargados, idsObjetosCargados)){
				NBASSERT(false)
			}
		}
		//Cargar bombillos precargados (se deben cargar al final debido a que hacen posible referencia a objetos que estan delante de el en la lista de "Elementos")
		SI32 iLaser, iLaserConteo = lasersPrecargados->conteo;
		for(iLaser=0; iLaser<iLaserConteo; iLaser++){
			STCuerpoObjEscenaPrecargadoXML* datosPrecargado = &(lasersPrecargados->elemento[iLaser]);
			AULaser* laser = (AULaser*)datosPrecargado->objPrecargado;
			if(!AULaser::interpretarXmlInterno(laser, datosXml, datosPrecargado->nodoDatosXML, objetosCargados, idsObjetosCargados)){
				NBASSERT(false)
			}
		}
		//Cargar electronodos precargados
		SI32 iElectro, iElectroConteo = electornodosPrecargados->conteo;
		for(iElectro=0; iElectro<iElectroConteo; iElectro++){
			STCuerpoObjEscenaPrecargadoXML* datosPrecagado = &(electornodosPrecargados->elemento[iElectro]);
			AUElectronodo* electronodo	= (AUElectronodo*)datosPrecagado->objPrecargado;
			if(!AUElectronodo::interpretarXmlInterno(electronodo, datosXml, datosPrecagado->nodoDatosXML, 0, idCapasCargadas, objetosCargadosPorCapa, idsObjetosCargadosPorCapa)){
				NBASSERT(false)
			}
		}
		panelesSolPrecargados->liberar(NB_RETENEDOR_NULL); panelesSolPrecargados = NULL;
		bateriasPrecargados->liberar(NB_RETENEDOR_NULL); bateriasPrecargados = NULL;
		bombillosPrecargados->liberar(NB_RETENEDOR_NULL); bombillosPrecargados = NULL;
		lasersPrecargados->liberar(NB_RETENEDOR_NULL); lasersPrecargados = NULL;
		electornodosPrecargados->liberar(NB_RETENEDOR_NULL); electornodosPrecargados = NULL;
		capasCargadas->liberar(NB_RETENEDOR_NULL);
		idCapasCargadas->liberar(NB_RETENEDOR_NULL);
		objetosCargadosPorCapa->liberar(NB_RETENEDOR_NULL);
		idsObjetosCargadosPorCapa->liberar(NB_RETENEDOR_NULL);
		//ILUMINACION
		const sNodoXML* nodoIlum	= datosXml->nodoHijo("iluminacion", nodoXml);
		if(nodoIlum != NULL){
			//PENDIENTE: CALCULAR POSICION EN BASE A LA ESCALA DE ARCHIVO Y DE PROGRAMA
			//float escalaPxPorMetro	= datosXml->nodoHijo("pxPorMetro", ESCALA_PIXELES_POR_METRO_MUNDO_FISICA, nodoIlum); 
			AUCadenaMutable8* rutaTexPatron = new(ENMemoriaTipo_Temporal) AUCadenaMutable8(); NB_DEFINE_NOMBRE_PUNTERO(rutaTexPatron, "AUEscenaCuerpo::rutaTexPatron")
			//ILUMINACION
			const sNodoXML* colorIlum	= datosXml->nodoHijo("colIlum8", nodoIlum);
			if(colorIlum != NULL){
				NBColor color;
				if(colorIlum->nodosHijos == NULL){
					//Formato nuevo CSV (mas compacto)
					const char* cadenaCSV	= datosXml->cadenaValorDeNodo(colorIlum);
					SI32 tamCadenaCSV		= AUCadena8::tamano(cadenaCSV);
					SI32 posEnCadenaCSV		= 0;
					color.r	= (float)AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 255) / 255.0f;
					color.g	= (float)AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 255) / 255.0f;
					color.b	= (float)AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 255) / 255.0f;
					color.a	= (float)AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 255) / 255.0f;
				} else {
					//Formato viejo XML (muy inflado)
					color.r = (float)datosXml->nodoHijo("r", 255, colorIlum) / 255.0f;
					color.g = (float)datosXml->nodoHijo("g", 255, colorIlum) / 255.0f;
					color.b = (float)datosXml->nodoHijo("b", 255, colorIlum) / 255.0f;
					color.a = (float)datosXml->nodoHijo("a", 255, colorIlum) / 255.0f;
				}
				nuevaPlantilla->colorIluminacion = color;
			}
			datosXml->nodoHijo("texPatronIluminacion", rutaTexPatron, "", nodoIlum);
			if(rutaTexPatron->tamano()!=0){
				AUTextura* textura = NBGestorTexturas::texturaPatronDesdeArchivoPNG(rutaTexPatron->str());
				if(textura == NULL){
					PRINTF_ERROR("no se pudo cargar la textura patron de iluminacion '%s'\n", rutaTexPatron->str());
				} else {
					nuevaPlantilla->texturaIluminacion = textura; 
					nuevaPlantilla->texturaIluminacion->retener(NB_RETENEDOR_NULL);
				}
			}
			//SOMBRA
			const sNodoXML* colorSombra	= datosXml->nodoHijo("colSomb8", nodoIlum);
			if(colorSombra != NULL){
				NBColor color;
				if(colorSombra->nodosHijos == NULL){
					//Formato nuevo CSV (mas compacto)
					const char* cadenaCSV	= datosXml->cadenaValorDeNodo(colorSombra);
					SI32 tamCadenaCSV		= AUCadena8::tamano(cadenaCSV);
					SI32 posEnCadenaCSV		= 0;
					color.r	= (float)AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 255) / 255.0f;
					color.g	= (float)AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 255) / 255.0f;
					color.b	= (float)AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 255) / 255.0f;
					color.a	= (float)AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 255) / 255.0f;
				} else {
					//Formato viejo XML (muy inflado)
					color.r = (float)datosXml->nodoHijo("r", 125, colorSombra) / 255.0f;
					color.g = (float)datosXml->nodoHijo("g", 125, colorSombra) / 255.0f;
					color.b = (float)datosXml->nodoHijo("b", 125, colorSombra) / 255.0f;
					color.a = (float)datosXml->nodoHijo("a", 255, colorSombra) / 255.0f;
				}
				nuevaPlantilla->colorSombra = color;
			}
			datosXml->nodoHijo("texPatronSombra", rutaTexPatron, "", nodoIlum);
			if(rutaTexPatron->tamano()!=0){
				AUTextura* textura = NBGestorTexturas::texturaPatronDesdeArchivoPNG(rutaTexPatron->str());
				if(textura == NULL){
					PRINTF_ERROR("no se pudo cargar la textura patron de iluminacion '%s'\n", rutaTexPatron->str());
				} else {
					nuevaPlantilla->texturaSombra = textura; textura->retener(NB_RETENEDOR_NULL);
				}
			}
			rutaTexPatron->liberar(NB_RETENEDOR_NULL);
			//
			NBCAJAAABB_INICIALIZAR(nuevaPlantilla->cajaLocalMascarasIlum);
			const sNodoXML* nodoMarcaras	= datosXml->nodoHijo("mascarasIlum", nodoIlum);
			if(nodoMarcaras != NULL){
				AUCadenaMutable8* strTextura = new(ENMemoriaTipo_Temporal) AUCadenaMutable8(); NB_DEFINE_NOMBRE_PUNTERO(strTextura, "AUEscenaCuerpo::strTextura")
				const sNodoXML* nodoMascara	= datosXml->nodoHijo("mascara", nodoMarcaras);
				while(nodoMascara != NULL){
					NBRectangulo limites; NBRECTANGULO_ESTABLECER(limites, 0.0f, 0.0f, 64.0f, 64.0f);
					NBPunto posicion;	NBPUNTO_ESTABLECER(posicion, 0.0f, 0.0f);
					NBTamano escala;	NBTAMANO_ESTABLECER(escala, 1.0f, 1.0f);
					float rotacion = 0.0f;
					const sNodoXML* nodoT	= datosXml->nodoHijo("t", nodoMascara);
					if(nodoT != NULL){
						posicion.x		= datosXml->nodoHijo("x", 0.0f, nodoT);
						posicion.y		= datosXml->nodoHijo("y", 0.0f, nodoT);
						escala.ancho	= datosXml->nodoHijo("ex", 1.0f, nodoT);
						escala.alto		= datosXml->nodoHijo("ey", 1.0f, nodoT);
						rotacion		= datosXml->nodoHijo("r", 0.0f, nodoT);
					}
					const sNodoXML* nodoL	= datosXml->nodoHijo("l", nodoMascara);
					if(nodoL != NULL){
						limites.x		= datosXml->nodoHijo("x", 0.0f, nodoL);
						limites.y		= datosXml->nodoHijo("y", 0.0f, nodoL);
						limites.ancho	= datosXml->nodoHijo("an", 64.0f, nodoL);
						limites.alto	= datosXml->nodoHijo("al", 64.0f, nodoL);
						if(limites.ancho<0.0f){ limites.x += limites.ancho; limites.ancho = -limites.ancho;}
						if(limites.alto<0.0f){ limites.y += limites.alto; limites.alto = -limites.alto;}
						NBASSERT(limites.ancho>0.0f && limites.alto>0.0f);
					}
					datosXml->nodoHijo("tex", strTextura, "", nodoMascara);
					//
					AUTextura* texturaMascara = NULL; if(strTextura->tamano()>0) texturaMascara = NBGestorTexturas::texturaDesdeArchivo(strTextura->str());
					if(texturaMascara != NULL){
						texturaMascara->retener(NB_RETENEDOR_NULL);
						STIlumMascara datosMascara;
						datosMascara.texturaMascara	= texturaMascara;
						datosMascara.limites		= limites;
						datosMascara.posicion		= posicion;
						datosMascara.escalacion		= escala;
						datosMascara.rotacion		= rotacion;
						NBMatriz matrizMascara;
						NBMATRIZ_ESTABLECER_SEGUN_TRANSFORMACIONES_GRADOS(matrizMascara, posicion.x, posicion.y, rotacion, escala.ancho, escala.alto)
						float limitesMax[2]			= {limites.x+limites.ancho, limites.y+limites.alto};
						//Rectangulo antihorario
						if((datosMascara.escalacion.ancho*datosMascara.escalacion.alto)<0.0f){
							//La matriz de mascara invierte el sentido contra-reloj
							NBMATRIZ_MULTIPLICAR_PUNTO(datosMascara.cacheVerticesCuerpo[3], matrizMascara, limites.x,		limites.y);
							NBMATRIZ_MULTIPLICAR_PUNTO(datosMascara.cacheVerticesCuerpo[2], matrizMascara, limitesMax[0],	limites.y);
							NBMATRIZ_MULTIPLICAR_PUNTO(datosMascara.cacheVerticesCuerpo[1], matrizMascara, limitesMax[0],	limitesMax[1]);
							NBMATRIZ_MULTIPLICAR_PUNTO(datosMascara.cacheVerticesCuerpo[0], matrizMascara, limites.x,		limitesMax[1]);
						} else {
							//La matriz de mascara mantiene el sentido contra-reloj
							NBMATRIZ_MULTIPLICAR_PUNTO(datosMascara.cacheVerticesCuerpo[0], matrizMascara, limites.x,		limites.y);
							NBMATRIZ_MULTIPLICAR_PUNTO(datosMascara.cacheVerticesCuerpo[1], matrizMascara, limitesMax[0],	limites.y);
							NBMATRIZ_MULTIPLICAR_PUNTO(datosMascara.cacheVerticesCuerpo[2], matrizMascara, limitesMax[0],	limitesMax[1]);
							NBMATRIZ_MULTIPLICAR_PUNTO(datosMascara.cacheVerticesCuerpo[3], matrizMascara, limites.x,		limitesMax[1]);
						}
						NBCAJAAABB_ENVOLVER_PUNTOS(nuevaPlantilla->cajaLocalMascarasIlum, datosMascara.cacheVerticesCuerpo, 4);
						NBASSERT(NBSegmentadorFiguras::figuraEsConvexaContrareloj(datosMascara.cacheVerticesCuerpo, 4))
						//
						nuevaPlantilla->mascarasIluminacion.agregarElemento(datosMascara);
					}
					//
					nodoMascara	= datosXml->nodoHijo("mascara", nodoMarcaras, nodoMascara);
				}
				strTextura->liberar(NB_RETENEDOR_NULL);
			}
			//Figuras que producen las sombras
			NBCAJAAABB_INICIALIZAR(nuevaPlantilla->cajaLocalSombras);
			const sNodoXML* objsEscena	= datosXml->nodoHijo("objsEscena", nodoIlum);
			if(objsEscena != NULL){
				AUCadenaMutable8* strTipoObjEscena = new(ENMemoriaTipo_Temporal) AUCadenaMutable8(); NB_DEFINE_NOMBRE_PUNTERO(strTipoObjEscena, "AUEscenaCuerpo::strTipoObjEscena")
				const sNodoXML* objEscena; XML_NODO_HIJO(datosXml, objEscena, "obE", "objEscena", objsEscena, NULL);
				while(objEscena != NULL){
					XML_VALOR_STR_NODO_HIJO(datosXml, strTipoObjEscena, "tp", "tipo", "", objEscena, NULL)
					const sNodoXML* nodoDatos; XML_NODO_HIJO(datosXml, nodoDatos, "dt", "datos", objEscena, NULL)
					if(nodoDatos != NULL && strTipoObjEscena->tamano()>0){
						if(strTipoObjEscena->esIgual("AUEscenaFigura")){
							AUEscenaFigura* figura = new(ENMemoriaTipo_Temporal) AUEscenaFigura(ENEscenaFiguraTipo_PoligonoCerrado); NB_DEFINE_NOMBRE_PUNTERO(figura, "AUEscenaCuerpo::figura")
							if(AUEscenaFigura::interpretarXmlInterno(figura, datosXml, nodoDatos)){
								NBMatriz matrizFigura;		NBMATRIZ_ESTABLECER_SEGUN_TRANSFORMACIONES_ESCENA_OBJETO(matrizFigura, figura->_propiedades.transformaciones);
								bool matrizInvierteOrden	= ((NBMATRIZ_ELEM00(matrizFigura)*NBMATRIZ_ELEM11(matrizFigura))<0.0f);
								AUArregloNativoMutableP<NBPunto>* puntosFiguraTransf = new(nuevaPlantilla) AUArregloNativoMutableP<NBPunto>(); NB_DEFINE_NOMBRE_PUNTERO(puntosFiguraTransf, "AUEscenaCuerpo::puntosFiguraTransf")
								AUArregloNativoP<NBPuntoColor>* puntosFigura = figura->puntosLocales();
								SI32 i, iConteo = puntosFigura->conteo;
								for(i=0; i<iConteo; i++){
									NBPunto puntoTransformado; NBMATRIZ_MULTIPLICAR_PUNTO(puntoTransformado, matrizFigura, puntosFigura->elemento[i].coordenada.x, puntosFigura->elemento[i].coordenada.y);
									puntosFiguraTransf->agregarElementoEnIndice(puntoTransformado, matrizInvierteOrden?0:puntosFiguraTransf->conteo);
									NBCAJAAABB_ENVOLVER_PUNTO(nuevaPlantilla->cajaLocalSombras, puntoTransformado.x, puntoTransformado.y);
								}
								if(puntosFiguraTransf->conteo!=0){
									#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
									if(puntosFiguraTransf->conteo>=3){ NBASSERT(NBSegmentadorFiguras::figuraEsConvexaContrareloj(puntosFiguraTransf->elemento, puntosFiguraTransf->conteo)) }
									#endif
									nuevaPlantilla->ptsFigurasSombraLocal.agregarElemento(puntosFiguraTransf);
								}
								puntosFiguraTransf->liberar(NB_RETENEDOR_NULL);
							} else {
								PRINTF_ERROR("no se cargo adecuado AUFigura desde XML\n");
							}
							figura->liberar(NB_RETENEDOR_NULL);
						}
					}
					//
					XML_NODO_HIJO(datosXml, objEscena, "obE", "objEscena", objsEscena, objEscena);
				}
				strTipoObjEscena->liberar(NB_RETENEDOR_NULL);
			}
		}
		strTmp->liberar(NB_RETENEDOR_NULL);
		strTmp2->liberar(NB_RETENEDOR_NULL);
		strTmp3->liberar(NB_RETENEDOR_NULL);
	//}
	//FIJACIONES DE FISICA
	const sNodoXML* nodoFijaciones	= datosXml->nodoHijo("fijaciones", nodoXml);
	if(nodoFijaciones != NULL){
		const sNodoXML* nodoFijacion;		XML_NODO_HIJO(datosXml, nodoFijacion, "f", "fijacion", nodoFijaciones, NULL)
		while(nodoFijacion != NULL){
			STFijacionDefinicion nuevaFijacion;
			nuevaFijacion.vertices 			= new(nuevaPlantilla) AUArregloNativoMutableP<NBPunto>(); NB_DEFINE_NOMBRE_PUNTERO(nuevaFijacion.vertices, "AUEscenaCuerpo::nuevaFijacion.vertices")
			XML_VALOR_NODO_HIJO(datosXml, nuevaFijacion.tipoFijacion, "tp", "tipo", -1, nodoFijacion, NULL) NBASSERT(nuevaFijacion.tipoFijacion>=0)
			XML_VALOR_NODO_HIJO(datosXml, nuevaFijacion.propiedadesFijacion.esSensor, "sen", "esSensor", false, nodoFijacion, NULL)
			XML_VALOR_NODO_HIJO(datosXml, nuevaFijacion.propiedadesFijacion.densidad, "den", "densidad", 1.0f, nodoFijacion, NULL)
			XML_VALOR_NODO_HIJO(datosXml, nuevaFijacion.propiedadesFijacion.friccion, "frc", "friccion", 0.50f, nodoFijacion, NULL)
			XML_VALOR_NODO_HIJO(datosXml, nuevaFijacion.propiedadesFijacion.restitucion, "rst", "restitucion", 0.25f, nodoFijacion, NULL)
			//
			NBPunto traslacion;		NBPUNTO_ESTABLECER(traslacion, 0.0f, 0.0f);
			NBTamano escalacion;	NBTAMANO_ESTABLECER(escalacion, 1.0f, 1.0f);
			float rotacion			= 0.0f;
			const sNodoXML* nodoMatriz; XML_NODO_HIJO(datosXml, nodoMatriz, "mat", "matriz", nodoFijacion, NULL)
			if(nodoMatriz != NULL){
				if(nodoMatriz->nodosHijos == NULL){
					//Formato nuevo CSV (mas compacto)
					const char* cadenaCSV		= datosXml->cadenaValorDeNodo(nodoMatriz);
					SI32 tamCadenaCSV			= AUCadena8::tamano(cadenaCSV);
					SI32 posEnCadenaCSV			= 0;
					traslacion.x				= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0.0f);
					traslacion.y				= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0.0f);
					escalacion.ancho			= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 1.0f);
					escalacion.alto				= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 1.0f);
					rotacion					= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0.0f);
				} else {
					//Formato viejo XML (muy inflado)
					const sNodoXML* nodoTras	= datosXml->nodoHijo("traslacion", nodoMatriz);
					if(nodoTras != NULL){
						traslacion.x			= datosXml->nodoHijo("x", 0.0f, nodoTras);
						traslacion.y			= datosXml->nodoHijo("y", 0.0f, nodoTras);
					}
					const sNodoXML* nodoEsca	= datosXml->nodoHijo("escalacion", nodoMatriz);
					if(nodoEsca != NULL){
						escalacion.ancho		= datosXml->nodoHijo("ancho", 1.0f, nodoEsca);
						escalacion.alto			= datosXml->nodoHijo("alto", 1.0f, nodoEsca);
					}
					rotacion					= datosXml->nodoHijo("rotacion", 0.0f, nodoMatriz);
				}
			}
			//
			NBMatriz matriz; NBMATRIZ_ESTABLECER_SEGUN_TRANSFORMACIONES_GRADOS(matriz, traslacion.x, traslacion.y, rotacion, escalacion.ancho, escalacion.alto)
			bool matrizInvierteOrden			= ((escalacion.ancho*escalacion.alto)<0.0f);
			//
			const sNodoXML* nodoVertices;		XML_NODO_HIJO(datosXml, nodoVertices, "vs", "vertices", nodoFijacion, NULL)
			const sNodoXML* nodoV				= datosXml->nodoHijo("v", nodoVertices);
			while(nodoV != NULL){
				float x, y;
				if(nodoV->nodosHijos == NULL){
					//Formato nuevo CSV (mas compacto)
					const char* cadenaCSV		= datosXml->cadenaValorDeNodo(nodoV);
					SI32 tamCadenaCSV			= AUCadena8::tamano(cadenaCSV);
					SI32 posEnCadenaCSV			= 0;
					x							= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0.0f);
					y							= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0.0f);
				} else {
					//Formato viejo XML (muy inflado)
					x = datosXml->nodoHijo("x", 0.0f, nodoV);
					y = datosXml->nodoHijo("y", 0.0f, nodoV);
				}
				NBPunto verticeLocal; NBMATRIZ_MULTIPLICAR_PUNTO(verticeLocal, matriz, x, y);
				nuevaFijacion.vertices->agregarElementoEnIndice(verticeLocal, matrizInvierteOrden?0:nuevaFijacion.vertices->conteo);
				nodoV	= datosXml->nodoHijo("v", nodoVertices, nodoV);
			}
			#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
			NBASSERT(nuevaFijacion.vertices->conteo!=0)
			//Nota: las fijaciones de fisica solo tienen que ser convexas cuando son poligonos cerrados
			//tipoFijacion, 0==linea || 1==circulo || 2==poligono || 3==secuenciaLineas
			if(nuevaFijacion.tipoFijacion==2){ NBASSERT(NBSegmentadorFiguras::figuraEsConvexaContrareloj(nuevaFijacion.vertices->elemento, nuevaFijacion.vertices->conteo)) }
			#endif
			//Validacion temporal
			if(nuevaFijacion.tipoFijacion==ENEscenaFiguraTipo_PoligonoCerrado && nuevaFijacion.vertices->conteo>8){
				PRINTF_WARNING("------------------------ ADVERTENCIA DISENO CUERPO ------------------------\n");
				PRINTF_WARNING("Fijacion poligono cerrado de cuerpo, tiene mas de 8 vertices: '%s'\n", nuevaPlantilla->rutaRecurso.str());
			}
			//Siguiente
			nuevaPlantilla->fijaciones.agregarElemento(nuevaFijacion);
			XML_NODO_HIJO(datosXml, nodoFijacion, "f", "fijacion", nodoFijaciones, nodoFijacion)
		}
	}
	//ZONAS DE GENERACION DE PARTICULAS
	const sNodoXML* nodoParticulas	= datosXml->nodoHijo("particulas", nodoXml);
	if(nodoParticulas != NULL){
		//URLs de texturas particulad
		const sNodoXML* nodoUrlsTex = datosXml->nodoHijo("urlsTexs", nodoParticulas);
		if(nodoUrlsTex != NULL){
			AUCadenaMutable8* strUrl = new(ENMemoriaTipo_Temporal) AUCadenaMutable8(); NB_DEFINE_NOMBRE_PUNTERO(strUrl, "AUEscenaCuerpo::strUrl")
			const sNodoXML* nodoTex = datosXml->nodoHijo("tex", strUrl, "", nodoUrlsTex);
			while(nodoTex != NULL){
				NBASSERT(strUrl->tamano()!=0);
				AUTextura* tex = NBGestorTexturas::texturaDesdeArchivo(strUrl->str());
				NBASSERT(tex != NULL);
				nuevaPlantilla->texturasParticulas.agregarElemento(tex);
				nodoTex = datosXml->nodoHijo("tex", strUrl, "", nodoUrlsTex, nodoTex);
			}
			strUrl->liberar(NB_RETENEDOR_NULL);
		}
		//Zonas de generacion de particulas
		const sNodoXML* nodoZonasGen	= datosXml->nodoHijo("zonasGen", nodoParticulas);
		if(nodoZonasGen != NULL){
			const sNodoXML* nodoZonaGen	= datosXml->nodoHijo("zonaGen", nodoZonasGen);
			while(nodoZonaGen != NULL){
				AUArregloNativoMutableP<NBPunto>* vertices = new(nuevaPlantilla) AUArregloNativoMutableP<NBPunto>(); NB_DEFINE_NOMBRE_PUNTERO(vertices, "AUEscenaCuerpo::vertices")
				NBMatriz matrizLocal; NBMATRIZ_ESTABLECER_IDENTIDAD(matrizLocal);
				const sNodoXML* nodoMatriz; XML_NODO_HIJO(datosXml, nodoMatriz, "mat", "matriz", nodoZonaGen, NULL)
				if(nodoMatriz != NULL){
					NBPunto traslacion;		NBPUNTO_ESTABLECER(traslacion, 0.0f, 0.0f);
					NBTamano escalacion;	NBTAMANO_ESTABLECER(escalacion, 1.0f, 1.0f);
					float rotacion			= 0.0f;
					if(nodoMatriz->nodosHijos == NULL){
						//Formato nuevo CSV (mas compacto)
						const char* cadenaCSV		= datosXml->cadenaValorDeNodo(nodoMatriz);
						SI32 tamCadenaCSV			= AUCadena8::tamano(cadenaCSV);
						SI32 posEnCadenaCSV			= 0;
						traslacion.x				= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0.0f);
						traslacion.y				= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0.0f);
						escalacion.ancho			= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 1.0f);
						escalacion.alto				= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 1.0f);
						rotacion					= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0.0f);
					} else {
						//Formato viejo XML (muy inflado)
						const sNodoXML* nodoTras	= datosXml->nodoHijo("traslacion", nodoMatriz);
						if(nodoTras != NULL){
							traslacion.x			= datosXml->nodoHijo("x", 0.0f, nodoTras);
							traslacion.y			= datosXml->nodoHijo("y", 0.0f, nodoTras);
						}
						const sNodoXML* nodoEsca	= datosXml->nodoHijo("escalacion", nodoMatriz);
						if(nodoEsca != NULL){
							escalacion.ancho		= datosXml->nodoHijo("ancho", 1.0f, nodoEsca);
							escalacion.alto			= datosXml->nodoHijo("alto", 1.0f, nodoEsca);
						}
						rotacion					= datosXml->nodoHijo("rotacion", 0.0f, nodoMatriz);
					}
					NBMATRIZ_ESTABLECER_SEGUN_TRANSFORMACIONES_GRADOS(matrizLocal, traslacion.x, traslacion.y, rotacion, escalacion.ancho, escalacion.alto)
				}
				const sNodoXML* nodoVertices;		XML_NODO_HIJO(datosXml, nodoVertices, "vs", "vertices", nodoZonaGen, NULL)
				const sNodoXML* nodoV				= datosXml->nodoHijo("v", nodoVertices);
				while(nodoV != NULL){
					float x, y;
					if(nodoV->nodosHijos == NULL){
						//Formato nuevo CSV (mas compacto)
						const char* cadenaCSV		= datosXml->cadenaValorDeNodo(nodoV);
						SI32 tamCadenaCSV			= AUCadena8::tamano(cadenaCSV);
						SI32 posEnCadenaCSV			= 0;
						x							= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0.0f);
						y							= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0.0f);
					} else {
						//Formato viejo XML (muy inflado)
						x	= datosXml->nodoHijo("x", 0.0f, nodoV);
						y	= datosXml->nodoHijo("y", 0.0f, nodoV);
					}
					NBPunto verticeLocal; NBMATRIZ_MULTIPLICAR_PUNTO(verticeLocal, matrizLocal, x, y);
					vertices->agregarElemento(verticeLocal);
					nodoV	= datosXml->nodoHijo("v", nodoVertices, nodoV);
				}
				if(vertices->conteo>=3){
					nuevaPlantilla->ptsZonasLocalGenParticulas.agregarElemento(vertices);
				}
				vertices->liberar(NB_RETENEDOR_NULL);
				nodoZonaGen	= datosXml->nodoHijo("zonaGen", nodoZonasGen, nodoZonaGen);
			}
		}
		//Sprites de particulas
		const sNodoXML* nodoSprites = datosXml->nodoHijo("sprites", nodoParticulas);
		if(nodoSprites != NULL){
			const sNodoXML* nodoSprite = datosXml->nodoHijo("sprite", nodoSprites);
			while(nodoSprite != NULL) {
				NBPunto pos(0.0f, 0.0f);
				const sNodoXML* nodoPos = datosXml->nodoHijo("pos", nodoSprite);
				if(nodoPos != NULL){
					pos.x = datosXml->nodoHijo("x", 0.0f, nodoPos);
					pos.y = datosXml->nodoHijo("y", 0.0f, nodoPos);
				}
				SI32 iTex = datosXml->nodoHijo("iTex", -1, nodoSprite);
				if(iTex!=-1){
					NBASSERT(iTex>=0);
					NBASSERT(iTex<nuevaPlantilla->texturasParticulas.conteo);
					STParticulaDef defParticula;
					defParticula.indiceTextura				= iTex;
					defParticula.textura					= (AUTextura*)nuevaPlantilla->texturasParticulas.elemento[iTex]; NBASSERT(defParticula.textura != NULL);
					defParticula.posicion					= pos;
					defParticula.estaDentroZonaGeneracion	= NBGestorCuerpos::privPuntoEstaDentroAlgunaFigura(pos.x, pos.y, &nuevaPlantilla->ptsZonasLocalGenParticulas);
					nuevaPlantilla->defParticulas.agregarElemento(defParticula);
					if(defParticula.estaDentroZonaGeneracion){
						nuevaPlantilla->indicesParticulasEnZonasGen.agregarElemento(nuevaPlantilla->defParticulas.conteo-1);
					}
				}
				nodoSprite = datosXml->nodoHijo("sprite", nodoSprites, nodoSprite);
			}
		}
	}
	//SONIDOS DE IMPACTOS
#	ifndef CONFIG_NB_UNSUPPORT_AUDIO_IO
	const sNodoXML* nodoSonidos	= datosXml->nodoHijo("sonidos", nodoXml);
	if(nodoSonidos != NULL){
		const sNodoXML* nodoImpactos	= datosXml->nodoHijo("impactos", nodoSonidos);
		if(nodoImpactos != NULL){
			AUCadenaMutable8* strRutaSnd = new(ENMemoriaTipo_Temporal) AUCadenaMutable8(); NB_DEFINE_NOMBRE_PUNTERO(strRutaSnd, "AUEscenaCuerpo::strRutaSnd")
			const sNodoXML* nodoSnd	= datosXml->nodoHijo("sonido", nodoImpactos);
			while(nodoSnd != NULL){
				datosXml->nodoHijo("ruta", strRutaSnd, "", nodoSnd);
				bool esDestructivo	= (datosXml->nodoHijo("esDestructivo", 0, nodoSnd)==1);
				float velImpactoMin = datosXml->nodoHijo("velImpactoMin", 0.0f, nodoSnd);
				float velImpactoMax = datosXml->nodoHijo("velImpactoMax", 100.0f, nodoSnd);
				if(strRutaSnd->tamano()>0){
					const UI16 bufferAL = NBGestorSonidos::bufferDesdeArchivoWav(strRutaSnd->str());
					if(bufferAL!=0){
						STEscenaSonidoImpactoDef datosSonido;
						datosSonido.bufferAL			= bufferAL; NBGestorSonidos::bufferRetener(bufferAL);
						datosSonido.segsDuracion		= NBGestorSonidos::bufferSegundos(bufferAL); NBASSERT(datosSonido.segsDuracion!=0.0f);
						datosSonido.esSonidoDestruccion	= esDestructivo;
						datosSonido.velImpactoMin		= velImpactoMin;
						datosSonido.velImpactoMax		= velImpactoMax;
						nuevaPlantilla->defSonidosDeImpacto.agregarElemento(datosSonido);
					}
				}
				//Siguiente
				nodoSnd	= datosXml->nodoHijo("sonido", nodoImpactos, nodoSnd);
			}
			strRutaSnd->liberar(NB_RETENEDOR_NULL);
		}
	}
#	endif
	exito = true;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return exito;
}

AUCuerpoDef* NBGestorCuerpos::privCargarPlantillaCuerpoDesdeBinario(const char* rutaArchivo, const char* nombreRecurso){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorCuerpos::privCargarPlantillaCuerpoDesdeBinario")
	bool exitoCargando = false;
	AUCuerpoDef* registroRetorno = NULL;
	AUArchivo* cargarDe = NBGestorArchivos::flujoDeArchivo(ENMemoriaTipo_Temporal, rutaArchivo, ENArchivoModo_SoloLectura);
	if(cargarDe != NULL){
		exitoCargando = true;
		cargarDe->lock();
		SI32 cantElem, iElem;
		AUCadenaMutable8* strTmp = new(ENMemoriaTipo_Temporal) AUCadenaMutable8();	NB_DEFINE_NOMBRE_PUNTERO(strTmp, "AUEscenaCuerpo::strTmp")
		AUCuerpoDef* nuevaPlantilla = new(ENMemoriaTipo_General) AUCuerpoDef();		NB_DEFINE_NOMBRE_PUNTERO(nuevaPlantilla, "AUEscenaCuerpo::nuevaPlantilla")
		nuevaPlantilla->rutaRecurso.establecer(nombreRecurso);
		//
		NBGESTORCUERPOS_LEER_VERIFICACION_BIN(cargarDe, exitoCargando)
		if(exitoCargando){
			//Variables
			cargarDe->leer(&(nuevaPlantilla->multDanoProduce), sizeof(nuevaPlantilla->multDanoProduce), 1, cargarDe);
			cargarDe->leer(&(nuevaPlantilla->multDanoRecibe), sizeof(nuevaPlantilla->multDanoRecibe), 1, cargarDe);
			//Datos simples
			nuevaPlantilla->indicesObjEscenaDesprendibles.interpretarBitsInternos(cargarDe);
			nuevaPlantilla->indicesObjEscenaAleatorios.interpretarBitsInternos(cargarDe);
			cargarDe->leer(&(nuevaPlantilla->cajaLocalMascarasIlum), sizeof(nuevaPlantilla->cajaLocalMascarasIlum), 1, cargarDe);
			cargarDe->leer(&(nuevaPlantilla->cajaLocalSombras), sizeof(nuevaPlantilla->cajaLocalSombras), 1, cargarDe);
			cargarDe->leer(&(nuevaPlantilla->colorIluminacion), sizeof(nuevaPlantilla->colorIluminacion), 1, cargarDe);
			cargarDe->leer(&(nuevaPlantilla->colorSombra), sizeof(nuevaPlantilla->colorSombra), 1, cargarDe);
			AUCadenaMutable8::interpretarBitsInternos(strTmp, cargarDe); 
			if(strTmp->tamano()!=0) { 
				nuevaPlantilla->texturaIluminacion = NBGestorTexturas::texturaDesdeArchivo(strTmp->str()); 
				if(nuevaPlantilla->texturaIluminacion != NULL) 
					nuevaPlantilla->texturaIluminacion->retener(NB_RETENEDOR_NULL); 
				else 
					PRINTF_WARNING("cpoBin, textura no se cargo: '%s'\n", strTmp->str());
			}
			AUCadenaMutable8::interpretarBitsInternos(strTmp, cargarDe); 
			if(strTmp->tamano()!=0) { 
				nuevaPlantilla->texturaSombra = NBGestorTexturas::texturaDesdeArchivo(strTmp->str()); 
				if(nuevaPlantilla->texturaSombra != NULL) 
					nuevaPlantilla->texturaSombra->retener(NB_RETENEDOR_NULL); 
				else  
					PRINTF_WARNING("cpoBin, textura no se cargo: '%s'\n", strTmp->str());
			}
			cargarDe->leer(&cantElem, sizeof(cantElem), 1, cargarDe); NBASSERT(cantElem>=0 && cantElem<99999) 
			for(iElem=0; iElem<cantElem; iElem++){
				AUCadenaMutable8::interpretarBitsInternos(strTmp, cargarDe); 
				if(strTmp->tamano()!=0){
					AUTextura* tex =  NBGestorTexturas::texturaDesdeArchivo(strTmp->str());
					if(tex != NULL) nuevaPlantilla->texturasParticulas.agregarElemento(tex);
					else PRINTF_WARNING("cpoBin, textura no se cargo: '%s'\n", strTmp->str());
				}
			}
			nuevaPlantilla->indicesParticulasEnZonasGen.interpretarBitsInternos(cargarDe);
			cargarDe->leer(&cantElem, sizeof(cantElem), 1, cargarDe); NBASSERT(cantElem>=0 && cantElem<99999)
			for(iElem=0; iElem<cantElem; iElem++){
				AUArregloNativoMutableP<NBPunto>* arreglo = new(nuevaPlantilla) AUArregloNativoMutableP<NBPunto>(); NB_DEFINE_NOMBRE_PUNTERO(arreglo, "AUEscenaCuerpo::arreglo")
				arreglo->interpretarBitsInternos(cargarDe);
				nuevaPlantilla->ptsFigurasSombraLocal.agregarElemento(arreglo);
				arreglo->liberar(NB_RETENEDOR_NULL);
			}
			cargarDe->leer(&cantElem, sizeof(cantElem), 1, cargarDe); NBASSERT(cantElem>=0 && cantElem<99999)
			for(iElem=0; iElem<cantElem; iElem++){
				AUArregloNativoMutableP<NBPunto>* arreglo = new(nuevaPlantilla) AUArregloNativoMutableP<NBPunto>(); NB_DEFINE_NOMBRE_PUNTERO(arreglo, "AUEscenaCuerpo::arreglo")
				arreglo->interpretarBitsInternos(cargarDe);
				nuevaPlantilla->ptsZonasLocalGenParticulas.agregarElemento(arreglo);
				arreglo->liberar(NB_RETENEDOR_NULL);
			}
		}
		NBGESTORCUERPOS_LEER_VERIFICACION_BIN(cargarDe, exitoCargando)
		if(exitoCargando){
			//Datos compuestos simples
			cargarDe->leer(&cantElem, sizeof(cantElem), 1, cargarDe); NBASSERT(cantElem>=0 && cantElem<99999)
			for(iElem=0; iElem<cantElem; iElem++){
				STParticulaDef estructura;
				cargarDe->leer(&(estructura.indiceTextura), sizeof(estructura.indiceTextura), 1, cargarDe);
				cargarDe->leer(&(estructura.posicion), sizeof(estructura.posicion), 1, cargarDe);
				cargarDe->leer(&(estructura.estaDentroZonaGeneracion), sizeof(estructura.estaDentroZonaGeneracion), 1, cargarDe);
				estructura.textura = NULL;
				strTmp->vaciar(); AUCadenaMutable8::interpretarBitsInternos(strTmp, cargarDe);
				if(strTmp->tamano()!=0){
					estructura.textura = NBGestorTexturas::texturaDesdeArchivo(strTmp->str());
					if(estructura.textura != NULL) 
						estructura.textura->retener(NB_RETENEDOR_NULL);
					else
						PRINTF_WARNING("cpoBin, textura no se cargo: '%s'\n", strTmp->str());	
				}
				nuevaPlantilla->defParticulas.agregarElemento(estructura);
			}
			cargarDe->leer(&cantElem, sizeof(cantElem), 1, cargarDe); NBASSERT(cantElem>=0 && cantElem<99999)
			for(iElem=0; iElem<cantElem; iElem++){
				STIlumMascara estructura;
				cargarDe->leer(&(estructura.limites), sizeof(estructura.limites), 1, cargarDe);
				cargarDe->leer(&(estructura.posicion), sizeof(estructura.posicion), 1, cargarDe);
				cargarDe->leer(&(estructura.escalacion), sizeof(estructura.escalacion), 1, cargarDe);
				cargarDe->leer(&(estructura.rotacion), sizeof(estructura.rotacion), 1, cargarDe);
				cargarDe->leer(&(estructura.cacheVerticesCuerpo[0]), sizeof(estructura.cacheVerticesCuerpo[0]), 1, cargarDe);
				cargarDe->leer(&(estructura.cacheVerticesCuerpo[1]), sizeof(estructura.cacheVerticesCuerpo[1]), 1, cargarDe);
				cargarDe->leer(&(estructura.cacheVerticesCuerpo[2]), sizeof(estructura.cacheVerticesCuerpo[2]), 1, cargarDe);
				cargarDe->leer(&(estructura.cacheVerticesCuerpo[3]), sizeof(estructura.cacheVerticesCuerpo[3]), 1, cargarDe);
				estructura.texturaMascara = NULL;
				strTmp->vaciar(); AUCadenaMutable8::interpretarBitsInternos(strTmp, cargarDe); 
				if(strTmp->tamano()!=0){
					estructura.texturaMascara = NBGestorTexturas::texturaDesdeArchivo(strTmp->str());
					if(estructura.texturaMascara != NULL) 
						estructura.texturaMascara->retener(NB_RETENEDOR_NULL);
					else
						PRINTF_WARNING("cpoBin, textura no se cargo: '%s'\n", strTmp->str());	
				}
				nuevaPlantilla->mascarasIluminacion.agregarElemento(estructura);
			}
#			ifndef CONFIG_NB_UNSUPPORT_AUDIO_IO
			cargarDe->leer(&cantElem, sizeof(cantElem), 1, cargarDe); NBASSERT(cantElem>=0 && cantElem<99999)
			for(iElem=0; iElem<cantElem; iElem++){
				STEscenaSonidoImpactoDef estructura;
				cargarDe->leer(&(estructura.segsDuracion), sizeof(estructura.segsDuracion), 1, cargarDe);
				cargarDe->leer(&(estructura.esSonidoDestruccion), sizeof(estructura.esSonidoDestruccion), 1, cargarDe);
				cargarDe->leer(&(estructura.velImpactoMin), sizeof(estructura.velImpactoMin), 1, cargarDe);
				cargarDe->leer(&(estructura.velImpactoMax), sizeof(estructura.velImpactoMax), 1, cargarDe);
				estructura.bufferAL = 0;
				strTmp->vaciar(); AUCadenaMutable8::interpretarBitsInternos(strTmp, cargarDe); 
				if(strTmp->tamano()!=0){
					estructura.bufferAL = NBGestorSonidos::bufferDesdeArchivoWav(strTmp->str());
					if(estructura.bufferAL!=0){
						NBGestorSonidos::bufferRetener(estructura.bufferAL);
					} else {
						PRINTF_WARNING("cpoBin, sonido no se cargo: '%s'\n", strTmp->str());
					}
				}
				nuevaPlantilla->defSonidosDeImpacto.agregarElemento(estructura);
			}
#			endif
			cargarDe->leer(&cantElem, sizeof(cantElem), 1, cargarDe); NBASSERT(cantElem>=0 && cantElem<99999)
			for(iElem=0; iElem<cantElem; iElem++){
				STFijacionDefinicion estructura;
				cargarDe->leer(&(estructura.tipoFijacion), sizeof(estructura.tipoFijacion), 1, cargarDe);
				cargarDe->leer(&(estructura.propiedadesFijacion), sizeof(estructura.propiedadesFijacion), 1, cargarDe);
				estructura.vertices = new(nuevaPlantilla) AUArregloNativoMutableP<NBPunto>();  NB_DEFINE_NOMBRE_PUNTERO(estructura.vertices, "AUEscenaCuerpo::estructura.vertices")
				estructura.vertices->interpretarBitsInternos(cargarDe);
				nuevaPlantilla->fijaciones.agregarElemento(estructura);
			}
		}
		NBGESTORCUERPOS_LEER_VERIFICACION_BIN(cargarDe, exitoCargando)
		if(exitoCargando){
			cargarDe->leer(&cantElem, sizeof(cantElem), 1, cargarDe); NBASSERT(cantElem>=0 && cantElem<99999)
			for(iElem=0; iElem<cantElem && exitoCargando; iElem++){
				STUnionCuerposDef estructura;
				SI32 tipoUnion; cargarDe->leer(&tipoUnion, sizeof(tipoUnion), 1, cargarDe); NBASSERT(tipoUnion>=0 && tipoUnion<ENUnionCuerpoTipo_Conteo);
				estructura.tipoUnion = (ENUnionCuerpoTipo)tipoUnion;
				cargarDe->leer(&(estructura.indiceCuerpoA), sizeof(estructura.indiceCuerpoA), 1, cargarDe);
				cargarDe->leer(&(estructura.indiceCuerpoB), sizeof(estructura.indiceCuerpoB), 1, cargarDe);
#				ifndef CONFIG_NB_UNSUPPORT_BOX2D
				estructura.b2Definicion			= NULL;
				estructura.estructuraAdicional	= NULL;
				//
				switch(estructura.tipoUnion) {
					case ENUnionCuerpoTipo_Soldadura: {
						b2WeldJointDef* defUnionW = new b2WeldJointDef(); NB_DEFINE_NOMBRE_PUNTERO(defUnionW, "AUEscenaCuerpo::defUnionW")
						cargarDe->leer(&(defUnionW->collideConnected), sizeof(defUnionW->collideConnected), 1, cargarDe);
						cargarDe->leer(&(defUnionW->localAnchorA), sizeof(defUnionW->localAnchorA), 1, cargarDe);
						cargarDe->leer(&(defUnionW->localAnchorB), sizeof(defUnionW->localAnchorB), 1, cargarDe);
						cargarDe->leer(&(defUnionW->referenceAngle), sizeof(defUnionW->referenceAngle), 1, cargarDe);
						estructura.b2Definicion = defUnionW;
						break;
					} case ENUnionCuerpoTipo_Rotacion: {
						b2RevoluteJointDef* defUnionR = new b2RevoluteJointDef(); NB_DEFINE_NOMBRE_PUNTERO(defUnionR, "AUEscenaCuerpo::defUnionR")
						cargarDe->leer(&(defUnionR->collideConnected), sizeof(defUnionR->collideConnected), 1, cargarDe);
						cargarDe->leer(&(defUnionR->localAnchorA), sizeof(defUnionR->localAnchorA), 1, cargarDe);
						cargarDe->leer(&(defUnionR->localAnchorB), sizeof(defUnionR->localAnchorB), 1, cargarDe);
						cargarDe->leer(&(defUnionR->enableLimit), sizeof(defUnionR->enableLimit), 1, cargarDe);
						cargarDe->leer(&(defUnionR->lowerAngle), sizeof(defUnionR->lowerAngle), 1, cargarDe);
						cargarDe->leer(&(defUnionR->upperAngle), sizeof(defUnionR->upperAngle), 1, cargarDe);
						cargarDe->leer(&(defUnionR->enableMotor), sizeof(defUnionR->enableMotor), 1, cargarDe);
						cargarDe->leer(&(defUnionR->motorSpeed), sizeof(defUnionR->motorSpeed), 1, cargarDe);
						cargarDe->leer(&(defUnionR->maxMotorTorque), sizeof(defUnionR->maxMotorTorque), 1, cargarDe);
						estructura.b2Definicion = defUnionR;
						break;
					} case ENUnionCuerpoTipo_Distancia: {
						b2DistanceJointDef* defUnionD = new b2DistanceJointDef(); NB_DEFINE_NOMBRE_PUNTERO(defUnionD, "AUEscenaCuerpo::defUnionD")
						cargarDe->leer(&(defUnionD->collideConnected), sizeof(defUnionD->collideConnected), 1, cargarDe);
						cargarDe->leer(&(defUnionD->localAnchorA), sizeof(defUnionD->localAnchorA), 1, cargarDe);
						cargarDe->leer(&(defUnionD->localAnchorB), sizeof(defUnionD->localAnchorB), 1, cargarDe);
						cargarDe->leer(&(defUnionD->length), sizeof(defUnionD->length), 1, cargarDe);
						cargarDe->leer(&(defUnionD->frequencyHz), sizeof(defUnionD->frequencyHz), 1, cargarDe);
						cargarDe->leer(&(defUnionD->dampingRatio), sizeof(defUnionD->dampingRatio), 1, cargarDe);
						estructura.b2Definicion = defUnionD;
						break;
					} case ENUnionCuerpoTipo_Cuerda: {
						b2RopeJointDef* defUnionC = new b2RopeJointDef(); NB_DEFINE_NOMBRE_PUNTERO(defUnionC, "AUEscenaCuerpo::defUnionC")
						cargarDe->leer(&(defUnionC->collideConnected), sizeof(defUnionC->collideConnected), 1, cargarDe);
						cargarDe->leer(&(defUnionC->localAnchorA), sizeof(defUnionC->localAnchorA), 1, cargarDe);
						cargarDe->leer(&(defUnionC->localAnchorB), sizeof(defUnionC->localAnchorB), 1, cargarDe);
						cargarDe->leer(&(defUnionC->maxLength), sizeof(defUnionC->maxLength), 1, cargarDe);
						estructura.b2Definicion = defUnionC;
						STUnionCuerposCuerdaDef* datosCuerda = new STUnionCuerposCuerdaDef(); NB_DEFINE_NOMBRE_PUNTERO(datosCuerda, "AUEscenaCuerpo::datosCuerda")
						cargarDe->leer(&(datosCuerda->tipoCuerda), sizeof(datosCuerda->tipoCuerda), 1, cargarDe);
						cargarDe->leer(&(datosCuerda->masaSegmentos), sizeof(datosCuerda->masaSegmentos), 1, cargarDe);
						cargarDe->leer(&(datosCuerda->factorEstiramiento), sizeof(datosCuerda->factorEstiramiento), 1, cargarDe);
						cargarDe->leer(&(datosCuerda->superposicionSegmentos), sizeof(datosCuerda->superposicionSegmentos), 1, cargarDe);
						SI32 tamRutaTex;
						cargarDe->leer(&tamRutaTex, sizeof(tamRutaTex), 1, cargarDe); NBASSERT(tamRutaTex>=0 && tamRutaTex<1024)
						if(tamRutaTex!=0){
							char* cadena = (char*)NBGestorMemoria::reservarMemoria(sizeof(char)*(tamRutaTex+1), ENMemoriaTipo_Temporal); NB_DEFINE_NOMBRE_PUNTERO(cadena, "NBGestorCuerpos::cadena")
							cargarDe->leer(cadena, sizeof(char), tamRutaTex, cargarDe); cadena[tamRutaTex] = '\0';
							datosCuerda->texturaCuerda = NBGestorTexturas::texturaDesdeArchivo(cadena); datosCuerda->texturaCuerda->retener(NB_RETENEDOR_NULL);
							NBGestorMemoria::liberarMemoria(cadena);
						}
						estructura.estructuraAdicional = datosCuerda;
						break;
					} case ENUnionCuerpoTipo_Friccion: {
						b2FrictionJointDef* defUnionC = new b2FrictionJointDef(); NB_DEFINE_NOMBRE_PUNTERO(defUnionC, "AUEscenaCuerpo::defUnionC")
						cargarDe->leer(&(defUnionC->collideConnected), sizeof(defUnionC->collideConnected), 1, cargarDe);
						cargarDe->leer(&(defUnionC->localAnchorA), sizeof(defUnionC->localAnchorA), 1, cargarDe);
						cargarDe->leer(&(defUnionC->localAnchorB), sizeof(defUnionC->localAnchorB), 1, cargarDe);
						cargarDe->leer(&(defUnionC->maxForce), sizeof(defUnionC->maxForce), 1, cargarDe);
						cargarDe->leer(&(defUnionC->maxTorque), sizeof(defUnionC->maxTorque), 1, cargarDe);
						estructura.b2Definicion = defUnionC;
						break;
					} case ENUnionCuerpoTipo_Prismatica: {
						b2PrismaticJointDef* defUnionP = new b2PrismaticJointDef(); NB_DEFINE_NOMBRE_PUNTERO(defUnionP, "AUEscenaCuerpo::defUnionP")
						cargarDe->leer(&(defUnionP->collideConnected), sizeof(defUnionP->collideConnected), 1, cargarDe);
						cargarDe->leer(&(defUnionP->localAnchorA), sizeof(defUnionP->localAnchorA), 1, cargarDe);
						cargarDe->leer(&(defUnionP->localAnchorB), sizeof(defUnionP->localAnchorB), 1, cargarDe);
						cargarDe->leer(&(defUnionP->referenceAngle), sizeof(defUnionP->referenceAngle), 1, cargarDe);
						cargarDe->leer(&(defUnionP->localAxisA), sizeof(defUnionP->localAxisA), 1, cargarDe);
						cargarDe->leer(&(defUnionP->enableLimit), sizeof(defUnionP->enableLimit), 1, cargarDe);
						cargarDe->leer(&(defUnionP->lowerTranslation), sizeof(defUnionP->lowerTranslation), 1, cargarDe);
						cargarDe->leer(&(defUnionP->upperTranslation), sizeof(defUnionP->upperTranslation), 1, cargarDe);
						cargarDe->leer(&(defUnionP->enableMotor), sizeof(defUnionP->enableMotor), 1, cargarDe);
						cargarDe->leer(&(defUnionP->motorSpeed), sizeof(defUnionP->motorSpeed), 1, cargarDe);
						cargarDe->leer(&(defUnionP->maxMotorForce), sizeof(defUnionP->maxMotorForce), 1, cargarDe);
						estructura.b2Definicion = defUnionP;
						break;
					} default:
						PRINTF_ERROR("cargando cuerpoBin, no se reconoce el tipo de union (%d)\n", (SI32)estructura.tipoUnion);
						NBASSERT(false);
						break;
				}
#				endif
				nuevaPlantilla->defUnionesCuerpos.agregarElemento(estructura);
			}
		}
		NBGESTORCUERPOS_LEER_VERIFICACION_BIN(cargarDe, exitoCargando)
		if(exitoCargando){
			AUArregloMutable* capasCargadas				= new(ENMemoriaTipo_Temporal) AUArregloMutable();				NB_DEFINE_NOMBRE_PUNTERO(capasCargadas, "AUGameplay::capasCargadas");
			AUArregloMutable* electronodosPrecargados	= new(ENMemoriaTipo_Temporal) AUArregloMutable();
			AUArregloMutable* bateriasPrecargados		= new(ENMemoriaTipo_Temporal) AUArregloMutable();
			AUArregloMutable* bombillosPrecargados		= new(ENMemoriaTipo_Temporal) AUArregloMutable();
			AUArregloMutable* lasersPrecargados			= new(ENMemoriaTipo_Temporal) AUArregloMutable();
			AUArregloMutable* panelesSolPrecargados		= new(ENMemoriaTipo_Temporal) AUArregloMutable();
			capasCargadas->agregarElemento(nuevaPlantilla->plantillaPresentacion);
			AUCadenaMutable8* rutaRecurso = new(ENMemoriaTipo_Temporal) AUCadenaMutable8(); NB_DEFINE_NOMBRE_PUNTERO(rutaRecurso, "AUEscenaCuerpo::rutaRecurso")
			cargarDe->leer(&cantElem, sizeof(cantElem), 1, cargarDe); NBASSERT(cantElem>=0 && cantElem<99999)
			for(iElem=0; iElem<cantElem && exitoCargando; iElem++){
				strTmp->vaciar(); AUCadenaMutable8::interpretarBitsInternos(strTmp, cargarDe);
				if(strTmp->esIgual("AUEscenaLuzRadial")){
					AUEscenaLuzRadial* luz = new(nuevaPlantilla) AUEscenaLuzRadial(); NB_DEFINE_NOMBRE_PUNTERO(luz, "AUEscenaCuerpo::luz(bin)")
					if(!AUEscenaLuzRadial::interpretarBitsInternos(luz, cargarDe)){
						exitoCargando = false;
						NBASSERT(false);
					} else {
						nuevaPlantilla->plantillaPresentacion->agregarObjetoEscena(luz);
					}
					luz->liberar(NB_RETENEDOR_NULL);
				} else if(strTmp->esIgual("AUEscenaLuzEspejo")){
					AUEscenaLuzEspejo* espejo = new(nuevaPlantilla) AUEscenaLuzEspejo(); NB_DEFINE_NOMBRE_PUNTERO(espejo, "AUEscenaCuerpo::espejo(bin)")
					if(!AUEscenaLuzEspejo::interpretarBitsInternos(espejo, cargarDe)){
						exitoCargando = false;
						NBASSERT(false);
					} else {
						nuevaPlantilla->plantillaPresentacion->agregarObjetoEscena(espejo);
					}
					espejo->liberar(NB_RETENEDOR_NULL);
				} else if(strTmp->esIgual("AUEscenaSprite")){
					AUEscenaSprite* sprite = new(nuevaPlantilla) AUEscenaSprite(); NB_DEFINE_NOMBRE_PUNTERO(sprite, "AUEscenaCuerpo::sprite(bin)")
					if(!AUEscenaSprite::interpretarBitsInternos(sprite, cargarDe)){
						exitoCargando = false;
						NBASSERT(false);
					} else {
						nuevaPlantilla->plantillaPresentacion->agregarObjetoEscena(sprite);
					}
					sprite->liberar(NB_RETENEDOR_NULL);
				} else if(strTmp->esIgual("AUEscenaGeneradorParticulas")){
					AUEscenaGeneradorParticulas* genParticulas = new(nuevaPlantilla) AUEscenaGeneradorParticulas(); NB_DEFINE_NOMBRE_PUNTERO(genParticulas, "AUEscenaCuerpo::genParticulas(bin)")
					if(!AUEscenaGeneradorParticulas::interpretarBitsInternos(genParticulas, cargarDe)){
						exitoCargando = false;
						NBASSERT(false);
					} else {
						nuevaPlantilla->plantillaPresentacion->agregarObjetoEscena(genParticulas);
					}
					genParticulas->liberar(NB_RETENEDOR_NULL);
				} else if(strTmp->esIgual("AUEscenaCuerda")){
					AUTextura* texturaPlantilla = NBGestorTexturas::texturaDesdeArchivo("Personajes/Gancho/perGanchoDodoPatronCuerda.png");
					AUEscenaCuerda* cuerda = new(nuevaPlantilla) AUEscenaCuerda(ENCuerdaTipo_Continua, texturaPlantilla, texturaPlantilla->tamanoHD().ancho*3.0f, 0.01f, 0.0f); NB_DEFINE_NOMBRE_PUNTERO(cuerda, "AUEscenaCuerpo::cuerda(bin)")
					if(!AUEscenaCuerda::interpretarBitsInternos(cuerda, cargarDe)){
						exitoCargando = false;
						NBASSERT(false);
					} else {
						nuevaPlantilla->plantillaPresentacion->agregarObjetoEscena(cuerda);
					}
					cuerda->liberar(NB_RETENEDOR_NULL);
				} else if(strTmp->esIgual("AUEscenaMallaGelatina")){
					AUEscenaMallaGelatina* mallaGelatina = new(nuevaPlantilla) AUEscenaMallaGelatina(); NB_DEFINE_NOMBRE_PUNTERO(mallaGelatina, "AUEscenaCuerpo::mallaGelatina(bin)")
					if(!AUEscenaMallaGelatina::interpretarBitsInternos(mallaGelatina, cargarDe)){
						exitoCargando = false;
						NBASSERT(false);
					} else {
						nuevaPlantilla->plantillaPresentacion->agregarObjetoEscena(mallaGelatina);
					}
					mallaGelatina->liberar(NB_RETENEDOR_NULL);
				} else if(strTmp->esIgual("AUEscenaMallaConHueso")){
					AUEscenaMallaConHueso* mallaHueso = new(nuevaPlantilla) AUEscenaMallaConHueso(); NB_DEFINE_NOMBRE_PUNTERO(mallaHueso, "AUEscenaCuerpo::mallaHueso(bin)")
					if(!AUEscenaMallaConHueso::interpretarBitsInternos(mallaHueso, cargarDe)){
						exitoCargando = false;
						NBASSERT(false);
					} else {
						nuevaPlantilla->plantillaPresentacion->agregarObjetoEscena(mallaHueso);
					}
					mallaHueso->liberar(NB_RETENEDOR_NULL);
				} else if(strTmp->esIgual("AUEscenaFigura")){
					AUEscenaFigura* figura = new(nuevaPlantilla) AUEscenaFigura(ENEscenaFiguraTipo_PoligonoCerrado); NB_DEFINE_NOMBRE_PUNTERO(figura, "AUEscenaCuerpo::figura(bin)")
					if(!AUEscenaFigura::interpretarBitsInternos(figura, cargarDe)){
						exitoCargando = false;
						NBASSERT(false);
					} else {
						nuevaPlantilla->plantillaPresentacion->agregarObjetoEscena(figura);
					}
					figura->liberar(NB_RETENEDOR_NULL);
				} else if(strTmp->esIgual("AUEscenaCuerpo")){
					rutaRecurso->vaciar(); AUCadenaMutable8::interpretarBitsInternos(rutaRecurso, cargarDe);
					STCuerpoFisicaDef propsSubcuerpo;	cargarDe->leer(&propsSubcuerpo, sizeof(propsSubcuerpo), 1, cargarDe);
					STCuerpoMultDef porpsMultip;		cargarDe->leer(&porpsMultip, sizeof(porpsMultip), 1, cargarDe);
					AUEscenaCuerpo* cuerpo = NBGestorCuerpos::cuerpoDesdeArchivo(ENMemoriaTipo_General, rutaRecurso->str()); //El agrupador de sombras sera definido luego
					if(cuerpo == NULL){
						exitoCargando = false;
						NBASSERT(false);
					} else {
						AUEscenaObjeto::interpretarBitsInternos(cuerpo, cargarDe);
						NBASSERT(propsSubcuerpo.tipoCuerpo>=0 && propsSubcuerpo.tipoCuerpo<ENCuerpoTipo_Conteo)
						NBASSERT(propsSubcuerpo.tipoAnimacion>=0 && propsSubcuerpo.tipoAnimacion<ENAnimFisicaTipo_Conteo)
						cuerpo->establecerCreacionCuerpoTipo((ENCuerpoTipo)propsSubcuerpo.tipoCuerpo, (ENAnimFisicaTipo)propsSubcuerpo.tipoAnimacion);
						cuerpo->establecerMultiplicadoresPropsCuerpo(porpsMultip);
						cuerpo->establecerCreacionCuerpoMascara(propsSubcuerpo.mascaraProps);
						cuerpo->establecerCreacionCuerpoFiltro(propsSubcuerpo.filtroCuerpos);
						cuerpo->establecerCreacionCuerpoFactorGravedad(propsSubcuerpo.factorGravedad);
						nuevaPlantilla->plantillaPresentacion->agregarObjetoEscena(cuerpo);
					}
				} else if(strTmp->esIgual("AUEscenaAnimacion")){
					rutaRecurso->vaciar(); AUCadenaMutable8::interpretarBitsInternos(rutaRecurso, cargarDe);
					AUEscenaAnimacionI* animacion = (AUEscenaAnimacionI*)_cargadorAnimaciones->cargarAnimacion(ENMemoriaTipo_General, rutaRecurso->str()); //El agrupador de sombras sera definido luego
					if(animacion == NULL){
						exitoCargando = false;
						NBASSERT(false);
					} else {
						AUEscenaObjeto::interpretarBitsInternos(animacion, cargarDe);
						bool reproAlCargar; cargarDe->leer(&reproAlCargar, sizeof(reproAlCargar), 1, cargarDe);
						bool repetir; cargarDe->leer(&repetir, sizeof(repetir), 1, cargarDe);
						bool reproducirSonidos; cargarDe->leer(&reproducirSonidos, sizeof(reproducirSonidos), 1, cargarDe);
						bool suavizar; cargarDe->leer(&suavizar, sizeof(suavizar), 1, cargarDe);
						float segsEntreRep; cargarDe->leer(&segsEntreRep, sizeof(segsEntreRep), 1, cargarDe);
						float velRepro; cargarDe->leer(&velRepro, sizeof(velRepro), 1, cargarDe);
						animacion->establecerReproducirAlCargar(reproAlCargar);
						animacion->establecerRepetirAnimacion(repetir);
						animacion->establecerReproducirSonidos(reproducirSonidos);
						animacion->establecerSuavizarAnimacion(suavizar);
						animacion->establecerSegundosEsperarParaRepetir(segsEntreRep);
						animacion->establecerFactorVelocidadReproduccion(velRepro);
						nuevaPlantilla->plantillaPresentacion->agregarObjetoEscena(animacion);
					}
				} else if(strTmp->esIgual("AUBateria")){
					AUBateria* objeto = new(nuevaPlantilla) AUBateria(); NB_DEFINE_NOMBRE_PUNTERO(objeto, "AUGameplay::bateria(bin)")
					nuevaPlantilla->plantillaPresentacion->agregarObjetoEscena(objeto);
					bateriasPrecargados->agregarElemento(objeto);
					objeto->liberar(NB_RETENEDOR_NULL);
				} else if(strTmp->esIgual("AUBombillo")){
					AUBombillo* objeto = new(nuevaPlantilla) AUBombillo(); NB_DEFINE_NOMBRE_PUNTERO(objeto, "AUGameplay::bombillo(bin)")
					nuevaPlantilla->plantillaPresentacion->agregarObjetoEscena(objeto);
					bombillosPrecargados->agregarElemento(objeto);
					objeto->liberar(NB_RETENEDOR_NULL);
				} else if(strTmp->esIgual("AULaser")){
					AULaser* objeto = new(nuevaPlantilla) AULaser(); NB_DEFINE_NOMBRE_PUNTERO(objeto, "AUGameplay::laser(bin)")
					nuevaPlantilla->plantillaPresentacion->agregarObjetoEscena(objeto);
					lasersPrecargados->agregarElemento(objeto);
					objeto->liberar(NB_RETENEDOR_NULL);
				} else if(strTmp->esIgual("AUPanelSolar")){
					AUPanelSolar* objeto = new(nuevaPlantilla) AUPanelSolar(); NB_DEFINE_NOMBRE_PUNTERO(objeto, "AUGameplay::panelSol(bin)")
					nuevaPlantilla->plantillaPresentacion->agregarObjetoEscena(objeto);
					panelesSolPrecargados->agregarElemento(objeto);
					objeto->liberar(NB_RETENEDOR_NULL);
				} else if(strTmp->esIgual("AUElectronodo")){
					AUElectronodo* objeto = new(nuevaPlantilla) AUElectronodo(); NB_DEFINE_NOMBRE_PUNTERO(objeto, "AUGameplay::sensor(bin)")
					nuevaPlantilla->plantillaPresentacion->agregarObjetoEscena(objeto);
					electronodosPrecargados->agregarElemento(objeto);
					objeto->liberar(NB_RETENEDOR_NULL);
				} else if(strTmp->esIgual("AULanzallamas")){
					AULanzallamas* objeto = new(nuevaPlantilla) AULanzallamas(); NB_DEFINE_NOMBRE_PUNTERO(objeto, "AUGameplay::flor(bin)")
					AULanzallamas::interpretarBitsInternos(objeto, cargarDe);
					nuevaPlantilla->plantillaPresentacion->agregarObjetoEscena(objeto);
					objeto->liberar(NB_RETENEDOR_NULL);
				} else {
					PRINTF_ERROR("cargando cuerpoBin, el tipo de objetoEscena '%s' no se reconoce\n", strTmp->str());
					NBASSERT(false);
				}
			}
			NBGESTORCUERPOS_LEER_VERIFICACION_BIN(cargarDe, exitoCargando)
			//Interpretar bits de paneles solares precargados (los datos deben interpretarse al final por si el sensor hace referencia a un objeto que esta delante de el en la lista)
			if(exitoCargando){
				SI32 iPanelSol;
				for(iPanelSol=0; iPanelSol<panelesSolPrecargados->conteo; iPanelSol++){
					AUPanelSolar* panelSolar = (AUPanelSolar*)panelesSolPrecargados->elemento[iPanelSol];
					if(!AUPanelSolar::interpretarBitsInternos(panelSolar, cargarDe, nuevaPlantilla->plantillaPresentacion->hijos())){
						NBASSERT(false)
					}
				}
			}
			NBGESTORCUERPOS_LEER_VERIFICACION_BIN(cargarDe, exitoCargando)
			//Interpretar bits de baterias precargados (los datos deben interpretarse al final por si el sensor hace referencia a un objeto que esta delante de el en la lista)
			if(exitoCargando){
				SI32 iBateria;
				for(iBateria=0; iBateria<bateriasPrecargados->conteo; iBateria++){
					AUBateria* objetoPrecargado = (AUBateria*)bateriasPrecargados->elemento[iBateria];
					if(!AUBateria::interpretarBitsInternos(objetoPrecargado, cargarDe, nuevaPlantilla->plantillaPresentacion->hijos())){
						NBASSERT(false)
					}
				}
			}
			NBGESTORCUERPOS_LEER_VERIFICACION_BIN(cargarDe, exitoCargando)
			//Interpretar bits de bombillos precargados (los datos deben interpretarse al final por si el sensor hace referencia a un objeto que esta delante de el en la lista)
			if(exitoCargando){
				SI32 iBombillo;
				for(iBombillo=0; iBombillo<bombillosPrecargados->conteo; iBombillo++){
					AUBombillo* objetoPrecargado = (AUBombillo*)bombillosPrecargados->elemento[iBombillo];
					if(!AUBombillo::interpretarBitsInternos(objetoPrecargado, cargarDe, nuevaPlantilla->plantillaPresentacion->hijos())){
						NBASSERT(false)
					}
				}
			}
			NBGESTORCUERPOS_LEER_VERIFICACION_BIN(cargarDe, exitoCargando)
			//Interpretar bits de lasers precargados (los datos deben interpretarse al final por si el sensor hace referencia a un objeto que esta delante de el en la lista)
			if(exitoCargando){
				SI32 iLaser;
				for(iLaser=0; iLaser<lasersPrecargados->conteo; iLaser++){
					AULaser* objetoPrecargado = (AULaser*)lasersPrecargados->elemento[iLaser];
					if(!AULaser::interpretarBitsInternos(objetoPrecargado, cargarDe, nuevaPlantilla->plantillaPresentacion->hijos())){
						NBASSERT(false)
					}
				}
			}
			NBGESTORCUERPOS_LEER_VERIFICACION_BIN(cargarDe, exitoCargando)
			//Interpretar bits de electronodos precargados (los datos deben interpretarse al final por si el sensor hace referencia a un objeto que esta delante de el en la lista)
			if(exitoCargando){
				SI32 iElectro;
				for(iElectro=0; iElectro<electronodosPrecargados->conteo; iElectro++){
					AUElectronodo* electronodo = (AUElectronodo*)electronodosPrecargados->elemento[iElectro];
					if(!AUElectronodo::interpretarBitsInternos(electronodo, cargarDe, capasCargadas)){
						NBASSERT(false)
					}
				}
			}
			NBGESTORCUERPOS_LEER_VERIFICACION_BIN(cargarDe, exitoCargando)
			//
			lasersPrecargados->liberar(NB_RETENEDOR_NULL); lasersPrecargados = NULL;
			bombillosPrecargados->liberar(NB_RETENEDOR_NULL); bombillosPrecargados = NULL;
			bateriasPrecargados->liberar(NB_RETENEDOR_NULL); bateriasPrecargados = NULL;
			panelesSolPrecargados->liberar(NB_RETENEDOR_NULL); panelesSolPrecargados = NULL;
			electronodosPrecargados->liberar(NB_RETENEDOR_NULL); electronodosPrecargados = NULL;
			capasCargadas->liberar(NB_RETENEDOR_NULL); capasCargadas = NULL;
			rutaRecurso->liberar(NB_RETENEDOR_NULL);
		}
		//FIN
		strTmp->liberar(NB_RETENEDOR_NULL);
		if(exitoCargando){
			_plantillasCuerpos->agregarElemento(nuevaPlantilla);
			registroRetorno = (AUCuerpoDef*)_plantillasCuerpos->elemento[_plantillasCuerpos->conteo-1];
		}
		nuevaPlantilla->liberar(NB_RETENEDOR_NULL);
		//
		cargarDe->unlock();
		cargarDe->cerrar();
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return registroRetorno;
}

bool NBGestorCuerpos::privGuardarPlantillaCuerpoEnBinario(AUCuerpoDef* plantilla, const char* rutaArchivo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorCuerpos::privGuardarPlantillaCuerpoEnBinario")
	bool exito = false;
	AUArchivo* guardarEn = NBGestorArchivos::flujoDeArchivo(ENMemoriaTipo_Temporal, rutaArchivo, ENArchivoModo_SoloEscritura);
	if(guardarEn == NULL){
		PRINTF_ERROR("no se pudo guardar binario de cuerpo en: '%s'\n", rutaArchivo);
	} else {
		guardarEn->lock();
		SI32 iElem, cantElem;
		AUCadenaMutable8* strTmp = new(ENMemoriaTipo_Temporal) AUCadenaMutable8(); NB_DEFINE_NOMBRE_PUNTERO(strTmp, "AUEscenaCuerpo::strTmp(bin)")
		NBGESTORCUERPOS_ESCRIBIR_VERIFICACION_BIN(guardarEn)
		//Variables
		guardarEn->escribir(&(plantilla->multDanoProduce), sizeof(plantilla->multDanoProduce), 1, guardarEn);
		guardarEn->escribir(&(plantilla->multDanoRecibe), sizeof(plantilla->multDanoRecibe), 1, guardarEn);
		//Datos simples
		plantilla->indicesObjEscenaDesprendibles.agregarBitsInternosEn(guardarEn);
		plantilla->indicesObjEscenaAleatorios.agregarBitsInternosEn(guardarEn);
		guardarEn->escribir(&(plantilla->cajaLocalMascarasIlum), sizeof(plantilla->cajaLocalMascarasIlum), 1, guardarEn);
		guardarEn->escribir(&(plantilla->cajaLocalSombras), sizeof(plantilla->cajaLocalSombras), 1, guardarEn);
		guardarEn->escribir(&(plantilla->colorIluminacion), sizeof(plantilla->colorIluminacion), 1, guardarEn);
		guardarEn->escribir(&(plantilla->colorSombra), sizeof(plantilla->colorSombra), 1, guardarEn);
		strTmp->vaciar(); if(plantilla->texturaIluminacion != NULL) strTmp->establecer(NBGestorTexturas::nombreRecursoTextura(plantilla->texturaIluminacion)); AUCadenaMutable8::agregarBitsInternosEn(strTmp, guardarEn);
		strTmp->vaciar(); if(plantilla->texturaSombra != NULL) strTmp->establecer(NBGestorTexturas::nombreRecursoTextura(plantilla->texturaSombra)); AUCadenaMutable8::agregarBitsInternosEn(strTmp, guardarEn);
		cantElem = plantilla->texturasParticulas.conteo; guardarEn->escribir(&cantElem, sizeof(cantElem), 1, guardarEn);
		for(iElem=0; iElem<cantElem; iElem++){
			AUTextura* texPart = (AUTextura*)plantilla->texturasParticulas.elemento[iElem];
			strTmp->vaciar(); if(texPart != NULL) strTmp->establecer(NBGestorTexturas::nombreRecursoTextura(texPart)); AUCadenaMutable8::agregarBitsInternosEn(strTmp, guardarEn);
		}
		plantilla->indicesParticulasEnZonasGen.agregarBitsInternosEn(guardarEn);
		cantElem = plantilla->ptsFigurasSombraLocal.conteo; guardarEn->escribir(&cantElem, sizeof(cantElem), 1, guardarEn);
		for(iElem=0; iElem<cantElem; iElem++){
			AUArregloNativoMutableP<NBPunto>* arreglo = (AUArregloNativoMutableP<NBPunto>*)plantilla->ptsFigurasSombraLocal.elemento[iElem];
			arreglo->agregarBitsInternosEn(guardarEn);
		}
		cantElem = plantilla->ptsZonasLocalGenParticulas.conteo; guardarEn->escribir(&cantElem, sizeof(cantElem), 1, guardarEn);
		for(iElem=0; iElem<cantElem; iElem++){
			AUArregloNativoMutableP<NBPunto>* arreglo = (AUArregloNativoMutableP<NBPunto>*)plantilla->ptsZonasLocalGenParticulas.elemento[iElem];
			arreglo->agregarBitsInternosEn(guardarEn);
		}
		//
		NBGESTORCUERPOS_ESCRIBIR_VERIFICACION_BIN(guardarEn)
		//Datos compuestos simples
		cantElem = plantilla->defParticulas.conteo; guardarEn->escribir(&cantElem, sizeof(cantElem), 1, guardarEn);
		for(iElem=0; iElem<cantElem; iElem++){
			STParticulaDef* estructura = &(plantilla->defParticulas.elemento[iElem]);
			guardarEn->escribir(&(estructura->indiceTextura), sizeof(estructura->indiceTextura), 1, guardarEn);
			guardarEn->escribir(&(estructura->posicion), sizeof(estructura->posicion), 1, guardarEn);
			guardarEn->escribir(&(estructura->estaDentroZonaGeneracion), sizeof(estructura->estaDentroZonaGeneracion), 1, guardarEn);
			strTmp->vaciar(); if(estructura->textura != NULL) strTmp->establecer(NBGestorTexturas::nombreRecursoTextura(estructura->textura)); 
			AUCadenaMutable8::agregarBitsInternosEn(strTmp, guardarEn);
		}
		cantElem = plantilla->mascarasIluminacion.conteo; guardarEn->escribir(&cantElem, sizeof(cantElem), 1, guardarEn);
		for(iElem=0; iElem<cantElem; iElem++){
			STIlumMascara* estructura = &(plantilla->mascarasIluminacion.elemento[iElem]);
			guardarEn->escribir(&(estructura->limites), sizeof(estructura->limites), 1, guardarEn);
			guardarEn->escribir(&(estructura->posicion), sizeof(estructura->posicion), 1, guardarEn);
			guardarEn->escribir(&(estructura->escalacion), sizeof(estructura->escalacion), 1, guardarEn);
			guardarEn->escribir(&(estructura->rotacion), sizeof(estructura->rotacion), 1, guardarEn);
			guardarEn->escribir(&(estructura->cacheVerticesCuerpo[0]), sizeof(estructura->cacheVerticesCuerpo[0]), 1, guardarEn);
			guardarEn->escribir(&(estructura->cacheVerticesCuerpo[1]), sizeof(estructura->cacheVerticesCuerpo[1]), 1, guardarEn);
			guardarEn->escribir(&(estructura->cacheVerticesCuerpo[2]), sizeof(estructura->cacheVerticesCuerpo[2]), 1, guardarEn);
			guardarEn->escribir(&(estructura->cacheVerticesCuerpo[3]), sizeof(estructura->cacheVerticesCuerpo[3]), 1, guardarEn);
			strTmp->vaciar(); if(estructura->texturaMascara != NULL) strTmp->establecer(NBGestorTexturas::nombreRecursoTextura(estructura->texturaMascara)); 
			AUCadenaMutable8::agregarBitsInternosEn(strTmp, guardarEn);
		}
#		ifndef CONFIG_NB_UNSUPPORT_AUDIO_IO
		cantElem = plantilla->defSonidosDeImpacto.conteo; guardarEn->escribir(&cantElem, sizeof(cantElem), 1, guardarEn);
		for(iElem=0; iElem<cantElem; iElem++){
			STEscenaSonidoImpactoDef* estructura = &(plantilla->defSonidosDeImpacto.elemento[iElem]);
			guardarEn->escribir(&(estructura->segsDuracion), sizeof(estructura->segsDuracion), 1, guardarEn);
			guardarEn->escribir(&(estructura->esSonidoDestruccion), sizeof(estructura->esSonidoDestruccion), 1, guardarEn);
			guardarEn->escribir(&(estructura->velImpactoMin), sizeof(estructura->velImpactoMin), 1, guardarEn);
			guardarEn->escribir(&(estructura->velImpactoMax), sizeof(estructura->velImpactoMax), 1, guardarEn);
			strTmp->vaciar(); if(estructura->bufferAL!=0) strTmp->establecer(NBGestorSonidos::nombreSonido(estructura->bufferAL));
			AUCadenaMutable8::agregarBitsInternosEn(strTmp, guardarEn);
		}
#		endif
		cantElem = plantilla->fijaciones.conteo; guardarEn->escribir(&cantElem, sizeof(cantElem), 1, guardarEn);
		for(iElem=0; iElem<cantElem; iElem++){
			STFijacionDefinicion* estructura = &(plantilla->fijaciones.elemento[iElem]);
			guardarEn->escribir(&(estructura->tipoFijacion), sizeof(estructura->tipoFijacion), 1, guardarEn);
			guardarEn->escribir(&(estructura->propiedadesFijacion), sizeof(estructura->propiedadesFijacion), 1, guardarEn);
			estructura->vertices->agregarBitsInternosEn(guardarEn);
		}
		//
		NBGESTORCUERPOS_ESCRIBIR_VERIFICACION_BIN(guardarEn)
		//Datos complejos
		cantElem = plantilla->defUnionesCuerpos.conteo; guardarEn->escribir(&cantElem, sizeof(cantElem), 1, guardarEn);
		for(iElem=0; iElem<cantElem; iElem++){
			STUnionCuerposDef* estructura = &(plantilla->defUnionesCuerpos.elemento[iElem]);
			SI32 tipoUnion = estructura->tipoUnion; guardarEn->escribir(&tipoUnion, sizeof(tipoUnion), 1, guardarEn);
			guardarEn->escribir(&(estructura->indiceCuerpoA), sizeof(estructura->indiceCuerpoA), 1, guardarEn);
			guardarEn->escribir(&(estructura->indiceCuerpoB), sizeof(estructura->indiceCuerpoB), 1, guardarEn);
			SI32 tamRutaTex = 0; const char* nombreTextura = NULL;
			if(tamRutaTex!=0) guardarEn->escribir(nombreTextura, sizeof(char), tamRutaTex, guardarEn);
#			ifndef CONFIG_NB_UNSUPPORT_BOX2D
			switch(estructura->tipoUnion) {
				case ENUnionCuerpoTipo_Soldadura: {
					b2WeldJointDef* defUnionW = (b2WeldJointDef*)estructura->b2Definicion;
					guardarEn->escribir(&(defUnionW->collideConnected), sizeof(defUnionW->collideConnected), 1, guardarEn);
					guardarEn->escribir(&(defUnionW->localAnchorA), sizeof(defUnionW->localAnchorA), 1, guardarEn);
					guardarEn->escribir(&(defUnionW->localAnchorB), sizeof(defUnionW->localAnchorB), 1, guardarEn);
					guardarEn->escribir(&(defUnionW->referenceAngle), sizeof(defUnionW->referenceAngle), 1, guardarEn);
					break;
				} case ENUnionCuerpoTipo_Rotacion: {
					b2RevoluteJointDef* defUnionR = (b2RevoluteJointDef*)estructura->b2Definicion;
					guardarEn->escribir(&(defUnionR->collideConnected), sizeof(defUnionR->collideConnected), 1, guardarEn);
					guardarEn->escribir(&(defUnionR->localAnchorA), sizeof(defUnionR->localAnchorA), 1, guardarEn);
					guardarEn->escribir(&(defUnionR->localAnchorB), sizeof(defUnionR->localAnchorB), 1, guardarEn);
					guardarEn->escribir(&(defUnionR->enableLimit), sizeof(defUnionR->enableLimit), 1, guardarEn);
					guardarEn->escribir(&(defUnionR->lowerAngle), sizeof(defUnionR->lowerAngle), 1, guardarEn);
					guardarEn->escribir(&(defUnionR->upperAngle), sizeof(defUnionR->upperAngle), 1, guardarEn);
					guardarEn->escribir(&(defUnionR->enableMotor), sizeof(defUnionR->enableMotor), 1, guardarEn);
					guardarEn->escribir(&(defUnionR->motorSpeed), sizeof(defUnionR->motorSpeed), 1, guardarEn);
					guardarEn->escribir(&(defUnionR->maxMotorTorque), sizeof(defUnionR->maxMotorTorque), 1, guardarEn);
					break;
				} case ENUnionCuerpoTipo_Distancia: {
					b2DistanceJointDef* defUnionD = (b2DistanceJointDef*)estructura->b2Definicion;
					guardarEn->escribir(&(defUnionD->collideConnected), sizeof(defUnionD->collideConnected), 1, guardarEn);
					guardarEn->escribir(&(defUnionD->localAnchorA), sizeof(defUnionD->localAnchorA), 1, guardarEn);
					guardarEn->escribir(&(defUnionD->localAnchorB), sizeof(defUnionD->localAnchorB), 1, guardarEn);
					guardarEn->escribir(&(defUnionD->length), sizeof(defUnionD->length), 1, guardarEn);
					guardarEn->escribir(&(defUnionD->frequencyHz), sizeof(defUnionD->frequencyHz), 1, guardarEn);
					guardarEn->escribir(&(defUnionD->dampingRatio), sizeof(defUnionD->dampingRatio), 1, guardarEn);
					break;
				} case ENUnionCuerpoTipo_Cuerda: {
					b2RopeJointDef* defUnionC = (b2RopeJointDef*)estructura->b2Definicion;
					guardarEn->escribir(&(defUnionC->collideConnected), sizeof(defUnionC->collideConnected), 1, guardarEn);
					guardarEn->escribir(&(defUnionC->localAnchorA), sizeof(defUnionC->localAnchorA), 1, guardarEn);
					guardarEn->escribir(&(defUnionC->localAnchorB), sizeof(defUnionC->localAnchorB), 1, guardarEn);
					guardarEn->escribir(&(defUnionC->maxLength), sizeof(defUnionC->maxLength), 1, guardarEn);
					//
					STUnionCuerposCuerdaDef* defCuerda = (STUnionCuerposCuerdaDef*)estructura->estructuraAdicional;
					guardarEn->escribir(&(defCuerda->tipoCuerda), sizeof(defCuerda->tipoCuerda), 1, guardarEn);
					guardarEn->escribir(&(defCuerda->masaSegmentos), sizeof(defCuerda->masaSegmentos), 1, guardarEn);
					guardarEn->escribir(&(defCuerda->factorEstiramiento), sizeof(defCuerda->factorEstiramiento), 1, guardarEn);
					guardarEn->escribir(&(defCuerda->superposicionSegmentos), sizeof(defCuerda->superposicionSegmentos), 1, guardarEn);
					//TexturaCuerda
					tamRutaTex = 0;
					if(defCuerda->texturaCuerda != NULL){
						nombreTextura = NBGestorTexturas::nombreRecursoTextura(defCuerda->texturaCuerda);
						if(nombreTextura != NULL) tamRutaTex = AUCadena8::tamano(nombreTextura);
					}
					guardarEn->escribir(&tamRutaTex, sizeof(tamRutaTex), 1, guardarEn);
					if(tamRutaTex!=0) guardarEn->escribir(nombreTextura, sizeof(char), tamRutaTex, guardarEn);
					break;
				} case ENUnionCuerpoTipo_Friccion: {
					b2FrictionJointDef* defUnionC = (b2FrictionJointDef*)estructura->b2Definicion;
					guardarEn->escribir(&(defUnionC->collideConnected), sizeof(defUnionC->collideConnected), 1, guardarEn);
					guardarEn->escribir(&(defUnionC->localAnchorA), sizeof(defUnionC->localAnchorA), 1, guardarEn);
					guardarEn->escribir(&(defUnionC->localAnchorB), sizeof(defUnionC->localAnchorB), 1, guardarEn);
					guardarEn->escribir(&(defUnionC->maxForce), sizeof(defUnionC->maxForce), 1, guardarEn);
					guardarEn->escribir(&(defUnionC->maxTorque), sizeof(defUnionC->maxTorque), 1, guardarEn);
					break;
				} case ENUnionCuerpoTipo_Prismatica: {
					b2PrismaticJointDef* defUnionP = (b2PrismaticJointDef*)estructura->b2Definicion;
					guardarEn->escribir(&(defUnionP->collideConnected), sizeof(defUnionP->collideConnected), 1, guardarEn);
					guardarEn->escribir(&(defUnionP->localAnchorA), sizeof(defUnionP->localAnchorA), 1, guardarEn);
					guardarEn->escribir(&(defUnionP->localAnchorB), sizeof(defUnionP->localAnchorB), 1, guardarEn);
					guardarEn->escribir(&(defUnionP->referenceAngle), sizeof(defUnionP->referenceAngle), 1, guardarEn);
					guardarEn->escribir(&(defUnionP->localAxisA), sizeof(defUnionP->localAxisA), 1, guardarEn);
					guardarEn->escribir(&(defUnionP->enableLimit), sizeof(defUnionP->enableLimit), 1, guardarEn);
					guardarEn->escribir(&(defUnionP->lowerTranslation), sizeof(defUnionP->lowerTranslation), 1, guardarEn);
					guardarEn->escribir(&(defUnionP->upperTranslation), sizeof(defUnionP->upperTranslation), 1, guardarEn);
					guardarEn->escribir(&(defUnionP->enableMotor), sizeof(defUnionP->enableMotor), 1, guardarEn);
					guardarEn->escribir(&(defUnionP->motorSpeed), sizeof(defUnionP->motorSpeed), 1, guardarEn);
					guardarEn->escribir(&(defUnionP->maxMotorForce), sizeof(defUnionP->maxMotorForce), 1, guardarEn);
					break;
				} default:
					PRINTF_ERROR("leyendo cuerpoBin, no se reconoce el tipo de union (%d)\n", (SI32)estructura->tipoUnion);
					NBASSERT(false)
					break;
			}
#			endif
		}
		//
		NBGESTORCUERPOS_ESCRIBIR_VERIFICACION_BIN(guardarEn)
		//
		AUArregloMutable* capasGuardadas			= new(ENMemoriaTipo_Temporal) AUArregloMutable(); capasGuardadas->agregarElemento(plantilla->plantillaPresentacion);
		AUArregloMutable* bateriasPreguardados		= new(ENMemoriaTipo_Temporal) AUArregloMutable();
		AUArregloMutable* bombillosPreguardados		= new(ENMemoriaTipo_Temporal) AUArregloMutable();
		AUArregloMutable* lasersPreguardados		= new(ENMemoriaTipo_Temporal) AUArregloMutable();
		AUArregloMutable* panelesSolPreguardados	= new(ENMemoriaTipo_Temporal) AUArregloMutable();
		AUArregloMutable* electronodosPreguardados	= new(ENMemoriaTipo_Temporal) AUArregloMutable();
		AUArreglo* hijos = plantilla->plantillaPresentacion->hijos();
		cantElem = hijos->conteo; guardarEn->escribir(&cantElem, sizeof(cantElem), 1, guardarEn);
		for(iElem=0; iElem<cantElem; iElem++){
			AUObjeto* hijo = hijos->elemento[iElem];
			bool resultado = false;
			if(hijo->esClase(AUEscenaLuzRadial::idTipoClase)){
				strTmp->establecer("AUEscenaLuzRadial"); AUCadenaMutable8::agregarBitsInternosEn(strTmp, guardarEn);
				resultado = AUEscenaLuzRadial::agregarBitsInternosEn((AUEscenaLuzRadial*)hijo, guardarEn); NBASSERT(resultado)
			} else if(hijo->esClase(AUEscenaLuzEspejo::idTipoClase)){
				strTmp->establecer("AUEscenaLuzEspejo"); AUCadenaMutable8::agregarBitsInternosEn(strTmp, guardarEn);
				resultado = AUEscenaLuzEspejo::agregarBitsInternosEn((AUEscenaLuzEspejo*)hijo, guardarEn); NBASSERT(resultado)
			} else if(hijo->esClase(AUEscenaSprite::idTipoClase)){
				strTmp->establecer("AUEscenaSprite"); AUCadenaMutable8::agregarBitsInternosEn(strTmp, guardarEn);
				resultado = AUEscenaSprite::agregarBitsInternosEn((AUEscenaSprite*)hijo, guardarEn); NBASSERT(resultado)
			} else if(hijo->esClase(AUEscenaGeneradorParticulas::idTipoClase)){
				strTmp->establecer("AUEscenaGeneradorParticulas"); AUCadenaMutable8::agregarBitsInternosEn(strTmp, guardarEn);
				resultado = AUEscenaGeneradorParticulas::agregarBitsInternosEn((AUEscenaGeneradorParticulas*)hijo, guardarEn); NBASSERT(resultado)	
			} else if(hijo->esClase(AUEscenaCuerda::idTipoClase)){
				strTmp->establecer("AUEscenaCuerda"); AUCadenaMutable8::agregarBitsInternosEn(strTmp, guardarEn);
				resultado = AUEscenaCuerda::agregarBitsInternosEn((AUEscenaCuerda*)hijo, guardarEn); NBASSERT(resultado)
			} else if(hijo->esClase(AUEscenaMallaGelatina::idTipoClase)){
				strTmp->establecer("AUEscenaMallaGelatina"); AUCadenaMutable8::agregarBitsInternosEn(strTmp, guardarEn);
				resultado = AUEscenaMallaGelatina::agregarBitsInternosEn((AUEscenaMallaGelatina*)hijo, guardarEn); NBASSERT(resultado)
			} else if(hijo->esClase(AUEscenaMallaConHueso::idTipoClase)){
				strTmp->establecer("AUEscenaMallaConHueso"); AUCadenaMutable8::agregarBitsInternosEn(strTmp, guardarEn);
				resultado = AUEscenaMallaConHueso::agregarBitsInternosEn((AUEscenaMallaConHueso*)hijo, guardarEn); NBASSERT(resultado)
			} else if(hijo->esClase(AUEscenaFigura::idTipoClase)){
				strTmp->establecer("AUEscenaFigura"); AUCadenaMutable8::agregarBitsInternosEn(strTmp, guardarEn);
				resultado = AUEscenaFigura::agregarBitsInternosEn((AUEscenaFigura*)hijo, guardarEn); NBASSERT(resultado)
			} else if(hijo->esClase(AUEscenaCuerpo::idTipoClase)){
				strTmp->establecer("AUEscenaCuerpo"); AUCadenaMutable8::agregarBitsInternosEn(strTmp, guardarEn);
				strTmp->establecer(((AUEscenaCuerpo*)hijo)->rutaArchivoOrigen()); AUCadenaMutable8::agregarBitsInternosEn(strTmp, guardarEn);
				STCuerpoFisicaDef propsSubcuerpo = ((AUEscenaCuerpo*)hijo)->definicionCreacionCuerpo();
				STCuerpoMultDef porpsMultip = ((AUEscenaCuerpo*)hijo)->multiplicadoresPropsCuerpo();
				guardarEn->escribir(&propsSubcuerpo, sizeof(propsSubcuerpo), 1, guardarEn);
				guardarEn->escribir(&porpsMultip, sizeof(porpsMultip), 1, guardarEn);
				resultado = AUEscenaObjeto::agregarBitsInternosEn((AUEscenaCuerpo*)hijo, guardarEn); NBASSERT(resultado)
			} else if(hijo->esClase(AUEscenaAnimacionI::idTipoClase)){
				AUEscenaAnimacionI* animacion = (AUEscenaAnimacionI*)hijo;
				strTmp->establecer("AUEscenaAnimacion"); AUCadenaMutable8::agregarBitsInternosEn(strTmp, guardarEn);
				strTmp->establecer(((AUEscenaAnimacionI*)hijo)->rutaArchivoOrigen()); AUCadenaMutable8::agregarBitsInternosEn(strTmp, guardarEn);
				resultado = AUEscenaObjeto::agregarBitsInternosEn((AUEscenaAnimacionI*)hijo, guardarEn); NBASSERT(resultado)
				bool reproAlCargar	=animacion->reproducirAlCargar();
				bool repetir		= animacion->repitiendoAnimacion();
				bool reproSounidos	= animacion->reproduciendoSonidos();
				bool suavizar		= animacion->suavizandoAnimacion();
				float segsEntreRep	= animacion->segundosEsperarParaRepetir();
				float velRepro		= animacion->factorVelocidadReproduccion();
				guardarEn->escribir(&reproAlCargar, sizeof(reproAlCargar), 1, guardarEn);
				guardarEn->escribir(&repetir, sizeof(repetir), 1, guardarEn);
				guardarEn->escribir(&reproSounidos, sizeof(reproSounidos), 1, guardarEn);
				guardarEn->escribir(&suavizar, sizeof(suavizar), 1, guardarEn);
				guardarEn->escribir(&segsEntreRep, sizeof(segsEntreRep), 1, guardarEn);
				guardarEn->escribir(&velRepro, sizeof(velRepro), 1, guardarEn);
			} else if(hijo->esClase(AUPanelSolar::idTipoClase)){
				strTmp->establecer("AUPanelSolar"); AUCadenaMutable8::agregarBitsInternosEn(strTmp, guardarEn);
				panelesSolPreguardados->agregarElemento(hijo);
			} else if(hijo->esClase(AUBateria::idTipoClase)){
				strTmp->establecer("AUBateria"); AUCadenaMutable8::agregarBitsInternosEn(strTmp, guardarEn);
				bateriasPreguardados->agregarElemento(hijo);
			} else if(hijo->esClase(AUBombillo::idTipoClase)){
				strTmp->establecer("AUBombillo"); AUCadenaMutable8::agregarBitsInternosEn(strTmp, guardarEn);
				bombillosPreguardados->agregarElemento(hijo);
			} else if(hijo->esClase(AULaser::idTipoClase)){
				strTmp->establecer("AULaser"); AUCadenaMutable8::agregarBitsInternosEn(strTmp, guardarEn);
				lasersPreguardados->agregarElemento(hijo);
			} else if(hijo->esClase(AUElectronodo::idTipoClase)){
				strTmp->establecer("AUElectronodo"); AUCadenaMutable8::agregarBitsInternosEn(strTmp, guardarEn);
				electronodosPreguardados->agregarElemento(hijo);
			} else if(hijo->esClase(AULanzallamas::idTipoClase)){
				strTmp->establecer("AULanzallamas"); AUCadenaMutable8::agregarBitsInternosEn(strTmp, guardarEn);
				resultado = AULanzallamas::agregarBitsInternosEn((AULanzallamas*)hijo, guardarEn); NBASSERT(resultado)
			} else {
				PRINTF_WARNING("guardando BIN cuerpo, objetoEscena %d de %d no volcado: '%s'\n", iElem, cantElem, hijo->nombreUltimaClase());
				NBASSERT(false) //objeto no valido
			}
		}
		NBGESTORCUERPOS_ESCRIBIR_VERIFICACION_BIN(guardarEn)
		//
		SI32 iPanelSol;
		for(iPanelSol=0; iPanelSol<panelesSolPreguardados->conteo; iPanelSol++){
			AUPanelSolar* objPreguardado = (AUPanelSolar*)panelesSolPreguardados->elemento[iPanelSol];
			if(!AUPanelSolar::agregarBitsInternosEn(objPreguardado, guardarEn, hijos)){
				PRINTF_ERROR("guardado datos binarios de PANEL_SOL\n"); //NBASSER(false) //No utilizar NBASSERT para evitar perdida de trabajo del disenador
			}
		}
		NBGESTORCUERPOS_ESCRIBIR_VERIFICACION_BIN(guardarEn)
		//
		SI32 iBateria;
		for(iBateria=0; iBateria<bateriasPreguardados->conteo; iBateria++){
			AUBateria* objPreguardado = (AUBateria*)bateriasPreguardados->elemento[iBateria];
			if(!AUBateria::agregarBitsInternosEn(objPreguardado, guardarEn, hijos)){
				PRINTF_ERROR("guardado datos binarios de BATERIA\n"); //NBASSER(false) //No utilizar NBASSERT para evitar perdida de trabajo del disenador
			}
		}
		NBGESTORCUERPOS_ESCRIBIR_VERIFICACION_BIN(guardarEn)
		//
		SI32 iBombillo;
		for(iBombillo=0; iBombillo<bombillosPreguardados->conteo; iBombillo++){
			AUBombillo* objPreguardado = (AUBombillo*)bombillosPreguardados->elemento[iBombillo];
			if(!AUBombillo::agregarBitsInternosEn(objPreguardado, guardarEn, hijos)){
				PRINTF_ERROR("guardado datos binarios de BOMBILLO\n"); //NBASSER(false) //No utilizar NBASSERT para evitar perdida de trabajo del disenador
			}
		}
		NBGESTORCUERPOS_ESCRIBIR_VERIFICACION_BIN(guardarEn)
		//
		SI32 iLaser;
		for(iLaser=0; iLaser<lasersPreguardados->conteo; iLaser++){
			AULaser* objPreguardado = (AULaser*)lasersPreguardados->elemento[iLaser];
			if(!AULaser::agregarBitsInternosEn(objPreguardado, guardarEn, hijos)){
				PRINTF_ERROR("guardado datos binarios de LASER\n"); //NBASSER(false) //No utilizar NBASSERT para evitar perdida de trabajo del disenador
			}
		}
		NBGESTORCUERPOS_ESCRIBIR_VERIFICACION_BIN(guardarEn)
		//
		SI32 iElectro;
		for(iElectro=0; iElectro<electronodosPreguardados->conteo; iElectro++){
			AUElectronodo* objPreguardado = (AUElectronodo*)electronodosPreguardados->elemento[iElectro];
			if(!AUElectronodo::agregarBitsInternosEn(objPreguardado, guardarEn, capasGuardadas)){
				PRINTF_ERROR("guardado datos binarios de ELECTRONODO\n"); //NBASSER(false) //No utilizar NBASSERT para evitar perdida de trabajo del disenador
			}
		}
		NBGESTORCUERPOS_ESCRIBIR_VERIFICACION_BIN(guardarEn)
		//
		lasersPreguardados->liberar(NB_RETENEDOR_NULL); lasersPreguardados = NULL;
		bombillosPreguardados->liberar(NB_RETENEDOR_NULL); bombillosPreguardados = NULL;
		bateriasPreguardados->liberar(NB_RETENEDOR_NULL); bateriasPreguardados = NULL;
		panelesSolPreguardados->liberar(NB_RETENEDOR_NULL); panelesSolPreguardados = NULL;
		electronodosPreguardados->liberar(NB_RETENEDOR_NULL); electronodosPreguardados = NULL;
		capasGuardadas->liberar(NB_RETENEDOR_NULL);  capasGuardadas = NULL;
		//
		strTmp->liberar(NB_RETENEDOR_NULL);
		//
		guardarEn->unlock();
		guardarEn->cerrar();
		exito = true;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return exito;
}




