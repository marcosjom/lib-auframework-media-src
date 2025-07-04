//
//  AUElectronodo.cpp
//  EditorCuerpos
//
//  Created by Marcos Ortega on 14/12/12.
//
//

#include "AUFrameworkMediaStdAfx.h"
#include "AUElectronodo.h"

AUElectronodo::AUElectronodo() : AUEscenaContenedor(), _pasosLanzamiento(this){
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUElectronodo")
	_tipoPresentacion		= ENElectronodoPresenta_Completa;
	_iPasoActual			= 255;
	_conteoPasoaActual		= 0;
	_segsEsperandoInicio	= 0.0f;
	_segsEsperaInicial		= 0.0f;
	_segsTotalPasoActual	= 0.0f;
	_segsAcumPasoActual		= 0.0f;
	_spriteBase = new(this) AUEscenaSprite(NBGestorTexturas::texturaDesdeArchivo("Escenarios/obsElectronodo.png"), ENSpriteAlineaH_Izq, ENSpriteAlineaV_Cen);
	this->agregarObjetoEscena(_spriteBase);
	_spriteBase->liberar(NB_RETENEDOR_THIS);
	//
	_rayosElectricos	= new(this) AUEscenaElectroRayo();
	this->agregarObjetoEscena(_rayosElectricos);
}

AUElectronodo::AUElectronodo(AUElectronodo* otraInstancia) : AUEscenaContenedor(), _pasosLanzamiento(this, &otraInstancia->_pasosLanzamiento){
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUElectronodo")
	_tipoPresentacion		= otraInstancia->_tipoPresentacion;
	_iPasoActual			= 255;
	_conteoPasoaActual		= 0;
	_segsEsperandoInicio	= 0.0f;
	_segsEsperaInicial		= otraInstancia->_segsEsperaInicial;
	_segsTotalPasoActual	= 0.0f;
	_segsAcumPasoActual		= 0.0f;
	this->copiarPropiedadesDe(otraInstancia);
	UI16 iPaso;
	for(iPaso=0; iPaso<_pasosLanzamiento.conteo; iPaso++){
		if(_pasosLanzamiento.elemento[iPaso].objDestino != NULL) _pasosLanzamiento.elemento[iPaso].objDestino->retener(NB_RETENEDOR_THIS);
	}
	//
	_spriteBase = new(this) AUEscenaSprite(otraInstancia->_spriteBase);
	this->agregarObjetoEscena(_spriteBase);
	_spriteBase->liberar(NB_RETENEDOR_THIS);
	//
	_rayosElectricos	= new(this) AUEscenaElectroRayo();
	this->agregarObjetoEscena(_rayosElectricos);
}

AUElectronodo::~AUElectronodo(){
	UI16 iPaso;
	for(iPaso=0; iPaso<_pasosLanzamiento.conteo; iPaso++){
		if(_pasosLanzamiento.elemento[iPaso].objDestino != NULL) _pasosLanzamiento.elemento[iPaso].objDestino->liberar(NB_RETENEDOR_THIS);
	}
	if(_rayosElectricos != NULL) _rayosElectricos->liberar(NB_RETENEDOR_THIS); _rayosElectricos = NULL;
}

//

ENElectronodoPresenta AUElectronodo::tipoPrensentacion(){
	return (ENElectronodoPresenta)_tipoPresentacion;
}

float AUElectronodo::segsEsperaInicial(){
	return _segsEsperaInicial;
}

AUArregloNativoP<STElectronodoPaso>* AUElectronodo::pasosLanzamiento(){
	return &_pasosLanzamiento;
}

//

void AUElectronodo::establecerTipoPresentacion(ENElectronodoPresenta tipoElectronodo){
	NBASSERT(tipoElectronodo>=0 && tipoElectronodo<ENElectronodoPresenta_Conteo)
	if(_tipoPresentacion!=tipoElectronodo){
		_tipoPresentacion = tipoElectronodo;
		if(_tipoPresentacion==ENElectronodoPresenta_Completa){
			AUTextura* tex = NBGestorTexturas::texturaDesdeArchivo("Escenarios/obsElectronodo.png");
			_spriteBase->redimensionar(tex, ENSpriteAlineaH_Izq, ENSpriteAlineaV_Cen);
			_spriteBase->establecerTextura(tex);
			_spriteBase->establecerTraslacion(0.0f, 0.0f);
		} else if(_tipoPresentacion==ENElectronodoPresenta_Simple){
			AUTextura* tex = NBGestorTexturas::texturaDesdeArchivo("Escenarios/obsElectronodoSimple.png");
			_spriteBase->redimensionar(tex, ENSpriteAlineaH_Cen, ENSpriteAlineaV_Cen);
			_spriteBase->establecerTextura(tex);
			_spriteBase->establecerTraslacion(0.0f, 0.0f);
		}
	}
}

void AUElectronodo::establecerSegsEsperaInicial(float segsEsperaInicial){
	_segsEsperaInicial = segsEsperaInicial;
}

void AUElectronodo::agregarPasoLanzamiento(float segsDuracion, AUEscenaObjeto* objDestino){
	STElectronodoPaso datosPaso;
	datosPaso.segsDuracion	= segsDuracion;
	datosPaso.objDestino	= objDestino; if(objDestino != NULL) objDestino->retener(NB_RETENEDOR_THIS);
	_pasosLanzamiento.agregarElemento(datosPaso);
}

void AUElectronodo::actualizarDestinoPasoLanzamiento(UI16 indice, AUEscenaObjeto* objDestino){
	NBASSERT(indice<_pasosLanzamiento.conteo)
	if(objDestino != NULL) objDestino->retener(NB_RETENEDOR_THIS);
	if(_pasosLanzamiento.elemento[indice].objDestino != NULL) _pasosLanzamiento.elemento[indice].objDestino->liberar(NB_RETENEDOR_THIS);
	_pasosLanzamiento.elemento[indice].objDestino = objDestino;
}

void AUElectronodo::actualizarDestinosPasosLanzamientosSegunPlantilla(AUElectronodo* instanciaPlantilla, AUArreglo* hijosPlantilla, AUArreglo* hijosLocal){
	NBASSERT(instanciaPlantilla->_pasosLanzamiento.conteo==this->_pasosLanzamiento.conteo)
	//Actualizar todas las referencias a objetos destinos de comandos
	UI16 conteoHijosLocal = hijosLocal->conteo;
	UI16 iCmd, conteoCmds = this->_pasosLanzamiento.conteo;
	for(iCmd=0; iCmd<conteoCmds; iCmd++){
		AUObjeto* objUnidoOriginal = instanciaPlantilla->_pasosLanzamiento.elemento[iCmd].objDestino;
		if(objUnidoOriginal != NULL){
			SI32 indiceObjetoOriginal = hijosPlantilla->indiceDe(objUnidoOriginal);
			if(indiceObjetoOriginal>=0 && indiceObjetoOriginal<conteoHijosLocal)
				this->actualizarDestinoPasoLanzamiento(iCmd, (AUEscenaObjeto*)hijosLocal->elemento[indiceObjetoOriginal]);
		} else {
			this->actualizarDestinoPasoLanzamiento(iCmd, NULL);
		}
	}
}

void AUElectronodo::quitarPasoLanzamientoEnindice(UI16 indice){
	NBASSERT(indice<_pasosLanzamiento.conteo)
	if(_pasosLanzamiento.elemento[indice].objDestino != NULL) _pasosLanzamiento.elemento[indice].objDestino->liberar(NB_RETENEDOR_THIS);
	_pasosLanzamiento.quitarElementoEnIndice(indice);
}

void AUElectronodo::moverPasoLanzamiento(UI16 indiceActual, UI16 indiceDestino){
	NBASSERT(indiceActual<_pasosLanzamiento.conteo && indiceDestino<_pasosLanzamiento.conteo)
	STElectronodoPaso datosPaso = _pasosLanzamiento.elemento[indiceActual];
	_pasosLanzamiento.quitarElementoEnIndice(indiceActual);
	_pasosLanzamiento.agregarElementoEnIndice(datosPaso, indiceDestino);
}

//

void AUElectronodo::agregadoEnEscena(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUElectronodo::agregadoEnEscena")
	NBGestorAnimadores::agregarAnimador(this, this);
	NBGestorPersonajes::agregarVictimario(this, 0);
	AUEscenaContenedor::agregadoEnEscena();
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUElectronodo::quitandoDeEscena(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUElectronodo::quitandoDeEscena")
	AUEscenaContenedor::quitandoDeEscena();
	NBGestorPersonajes::quitarVictimario(this);
	NBGestorAnimadores::quitarAnimador(this);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

bool AUElectronodo::personajeDanoHaciaVictima(const NBCajaAABB &cajaEscenaVictima, STPersonajeDano* guardarDanoEn){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUElectronodo::personajeDanoHaciaVictima")
	bool existeDano = false;
	if(NBCAJAAABB_INTERSECTA_CAJA(_cacheObjEscena.cajaEscena, cajaEscenaVictima)){ //Caja que inclute al sprite y los rayos
		NBCajaAABB cajaEscenaSprite = _spriteBase->cajaAABBEnEscena();
		if(NBCAJAAABB_INTERSECTA_CAJA(cajaEscenaSprite, cajaEscenaVictima)){ //Caja del sprite
			existeDano = true;
			guardarDanoEn->factorDanoRecibido = 1.0f;
			NBCAJAAABB_CENTRO(guardarDanoEn->posEscenaOrigenDano, _cacheObjEscena.cajaEscena)
		} else {
			NBMatriz matrizEscenaRayos = _rayosElectricos->matrizEscena();
			AUArregloNativoP<STRayo>* rayosActivos = _rayosElectricos->rayosActivos();
			STRayo* arrRayos			= rayosActivos->elemento;
			UI16 iRayo, conteoRayos		= rayosActivos->conteo;
			for(iRayo=0; iRayo<conteoRayos; iRayo++){
				NBPunto posIniEscena; NBMATRIZ_MULTIPLICAR_PUNTO(posIniEscena, matrizEscenaRayos, arrRayos[iRayo].posInicio.x, arrRayos[iRayo].posInicio.y)
				NBPunto posFinEscena; NBMATRIZ_MULTIPLICAR_PUNTO(posFinEscena, matrizEscenaRayos, arrRayos[iRayo].posFin.x, arrRayos[iRayo].posFin.y)
				NBPunto vecRayo; NBPUNTO_ESTABLECER(vecRayo, posFinEscena.x-posIniEscena.x, posFinEscena.y-posIniEscena.y)
				NBPunto vectorCaja[4];
				float productoCruz[4];
				NBPUNTO_ESTABLECER(vectorCaja[0], cajaEscenaVictima.xMin-posIniEscena.x, cajaEscenaVictima.yMin-posIniEscena.y)
				NBPUNTO_ESTABLECER(vectorCaja[1], cajaEscenaVictima.xMin-posIniEscena.x, cajaEscenaVictima.yMax-posIniEscena.y)
				NBPUNTO_ESTABLECER(vectorCaja[2], cajaEscenaVictima.xMax-posIniEscena.x, cajaEscenaVictima.yMin-posIniEscena.y)
				NBPUNTO_ESTABLECER(vectorCaja[3], cajaEscenaVictima.xMax-posIniEscena.x, cajaEscenaVictima.yMax-posIniEscena.y)
				productoCruz[0]	= NBPUNTO_PRODUCTO_VECTORIAL(vecRayo.x, vecRayo.y, vectorCaja[0].x, vectorCaja[0].y);
				productoCruz[1]	= NBPUNTO_PRODUCTO_VECTORIAL(vecRayo.x, vecRayo.y, vectorCaja[1].x, vectorCaja[1].y);
				productoCruz[2]	= NBPUNTO_PRODUCTO_VECTORIAL(vecRayo.x, vecRayo.y, vectorCaja[2].x, vectorCaja[2].y);
				productoCruz[3]	= NBPUNTO_PRODUCTO_VECTORIAL(vecRayo.x, vecRayo.y, vectorCaja[3].x, vectorCaja[3].y);
				if(!((productoCruz[0]<0.0f && productoCruz[1]<0.0f && productoCruz[2]<0.0f && productoCruz[3]<0.0f) || (productoCruz[0]>=0.0f && productoCruz[1]>=0.0f && productoCruz[2]>=0.0f && productoCruz[3]>=0.0f))){
					//La caja tiene algunos vertices a la izquierda y otros a la derecha del rayo (intersecta rayo infinito)
					float longitudRayo = NBPUNTO_DISTANCIA_VECTOR(vecRayo.x, vecRayo.y); NBASSERT(longitudRayo!=0.0f)
					NBPunto vecUnitRayo; NBPUNTO_ESTABLECER(vecUnitRayo, vecRayo.x/longitudRayo, vecRayo.y/longitudRayo)
					float productoPunto[4];
					productoPunto[0]	= NBPUNTO_PRODUCTO_PUNTO(vecUnitRayo.x, vecUnitRayo.y, vectorCaja[0].x, vectorCaja[0].y);
					productoPunto[1]	= NBPUNTO_PRODUCTO_PUNTO(vecUnitRayo.x, vecUnitRayo.y, vectorCaja[1].x, vectorCaja[1].y);
					productoPunto[2]	= NBPUNTO_PRODUCTO_PUNTO(vecUnitRayo.x, vecUnitRayo.y, vectorCaja[2].x, vectorCaja[2].y);
					productoPunto[3]	= NBPUNTO_PRODUCTO_PUNTO(vecUnitRayo.x, vecUnitRayo.y, vectorCaja[3].x, vectorCaja[3].y);
					if(!((productoPunto[0]<0.0f && productoPunto[1]<0.0f && productoPunto[2]<0.0f && productoPunto[3]<0.0f) || (productoPunto[0]>longitudRayo && productoPunto[1]>longitudRayo && productoPunto[2]>longitudRayo && productoPunto[3]>longitudRayo))){
						//La caja tiene por lo menos un vertice dentro del rayo
						existeDano = true;
						guardarDanoEn->factorDanoRecibido = 1.0f;
						float promedioPuntoCaja = (productoPunto[0] + productoPunto[1] + productoPunto[2] + productoPunto[3]) / 4.0f;
						guardarDanoEn->posEscenaOrigenDano.x = posIniEscena.x + (vecUnitRayo.x * promedioPuntoCaja);
						guardarDanoEn->posEscenaOrigenDano.y = posIniEscena.y + (vecUnitRayo.y * promedioPuntoCaja);
						break;
					}
				}
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return existeDano;
}

void AUElectronodo::tickAnimacion(float segsTranscurridos){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUElectronodo::tickAnimacion")
	if(!this->_dormido && _pasosLanzamiento.conteo!=0){
		if(_segsEsperandoInicio<_segsEsperaInicial){
			_segsEsperandoInicio += segsTranscurridos;
		} else {
			NBASSERT(this->idEscena!=-1)
			NBPunto posRayoIni; NBPUNTO_ESTABLECER(posRayoIni, 0.0f, 0.0f);
			//Animar posicion de rayos activos
			UI16 conteoPaso, iPaso = _iPasoActual;
			for(conteoPaso=0; conteoPaso<_conteoPasoaActual; conteoPaso++){
				STElectronodoPaso* pasoActivo = &_pasosLanzamiento.elemento[iPaso];
				if(pasoActivo->objDestino != NULL){
					if(pasoActivo->objDestino->idEscena!=-1){
						NBPunto posEscenaDestino	= pasoActivo->objDestino->coordenadaLocalAEscena(0.0f, 0.0f);
						NBPunto posRayoFin			= this->coordenadaEscenaALocal(posEscenaDestino.x, posEscenaDestino.y);
						_rayosElectricos->actualizarRayoPosiciones(iPaso, posRayoIni, posRayoFin);
					}
				}
				iPaso++; if(iPaso>=_pasosLanzamiento.conteo) iPaso = 0;
			}
			//Animar pasos
			_segsAcumPasoActual					+= segsTranscurridos;
			if(_segsAcumPasoActual>_segsTotalPasoActual){
				_segsAcumPasoActual				= 0.0f;
				_segsTotalPasoActual			= 0.0f;
				UI16 iPaso						= _iPasoActual + 1; if(iPaso>=_pasosLanzamiento.conteo) iPaso = 0;
				_iPasoActual					= iPaso;
				_conteoPasoaActual				= 0;
				STElectronodoPaso* pasoNuevo	= &_pasosLanzamiento.elemento[iPaso];
				_segsTotalPasoActual			= pasoNuevo->segsDuracion;
				do {
					if(pasoNuevo->objDestino != NULL){
						if(pasoNuevo->objDestino->idEscena!=-1){
							NBPunto posEscenaDestino	= pasoNuevo->objDestino->coordenadaLocalAEscena(0.0f, 0.0f);
							NBPunto posRayoFin			= this->coordenadaEscenaALocal(posEscenaDestino.x, posEscenaDestino.y);
							_rayosElectricos->agregarRayo(iPaso, posRayoIni, posRayoFin, _segsTotalPasoActual);
						}
					}
					_conteoPasoaActual++;
					iPaso++; if(iPaso>=_pasosLanzamiento.conteo) iPaso = 0;
					pasoNuevo = &_pasosLanzamiento.elemento[iPaso];
				} while(pasoNuevo->segsDuracion==0.0f && iPaso!=_iPasoActual);
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

//

bool AUElectronodo::agregarXmlInternoEn(AUElectronodo* electronodo, AUCadenaLargaMutable8* guardarEn, const char* espaciosBaseIzq, AUArreglo* arregloCapasPorID){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUElectronodo::agregarXmlInternoEn")
	//guardarEn->agregarConFormato("%s<vXML>1</vXML>\r\n", espaciosBaseIzq);
	guardarEn->agregarConFormato("%s<eOb>\r\n", espaciosBaseIzq);
	AUEscenaObjeto::agregarXmlInternoEn(electronodo, guardarEn, espaciosBaseIzq);
	guardarEn->agregarConFormato("%s</eOb>\r\n", espaciosBaseIzq);
	guardarEn->agregarConFormato("%s<segsIni>%f</segsIni>\r\n", espaciosBaseIzq, electronodo->_segsEsperaInicial);
	guardarEn->agregarConFormato("%s<tipP>%d</tipP>\r\n", espaciosBaseIzq, electronodo->_tipoPresentacion);
	guardarEn->agregarConFormato("%s<pasos>\r\n", espaciosBaseIzq);
	UI32 iCmd, iCmdConteo = electronodo->_pasosLanzamiento.conteo;
	for(iCmd=0; iCmd<iCmdConteo; iCmd++){
		STElectronodoPaso* paso	= &(electronodo->_pasosLanzamiento.elemento[iCmd]);
		SI32 iCapa					= -1;
		SI32 iObjDestino			= -1;
		if(paso->objDestino != NULL){
			NBASSERT(paso->objDestino->esClase(AUEscenaObjeto::idTipoClase))
			AUEscenaContenedor* contenedor = (AUEscenaContenedor*)((AUEscenaObjeto*)paso->objDestino)->contenedor();
			if(contenedor != NULL){
				iCapa = arregloCapasPorID->indiceDe(contenedor);
				if(iCapa!=-1) iObjDestino = contenedor->hijos()->indiceDe(paso->objDestino);
			}
		}
		guardarEn->agregarConFormato("%s <paso>\r\n", espaciosBaseIzq);
		guardarEn->agregarConFormato("%s  <iObj>%d|%d</iObj>\r\n", espaciosBaseIzq, iCapa, iObjDestino);
		guardarEn->agregarConFormato("%s  <segsDur>%f</segsDur>\r\n", espaciosBaseIzq, paso->segsDuracion);
		guardarEn->agregarConFormato("%s </paso>\r\n", espaciosBaseIzq);
	}
	guardarEn->agregarConFormato("%s</pasos>\r\n", espaciosBaseIzq);
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return true;
	
}

bool AUElectronodo::interpretarXmlInterno(AUElectronodo* cargarEn, const AUDatosXML* datosXml, const sNodoXML* nodoXml, SI32 idCapaPorDefecto, AUArregloNativoP<SI32>* arregloIDsCapas, AUArreglo* arreglosObjetosPorCapa, AUArreglo* arregloIDsObjetosPorCapa){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUElectronodo::interpretarXmlInterno")
	bool exito = false;
	//int versionXML = datosXml->nodoHijo("vXML", 0, nodoXml);
	//if(versionXML==1){
	const sNodoXML* nodoEscenaObj;			XML_NODO_HIJO(datosXml, nodoEscenaObj, "eOb", "escObj", nodoXml, NULL)
	if(nodoEscenaObj != NULL){
		exito = AUEscenaObjeto::interpretarXmlInterno(cargarEn, datosXml, nodoEscenaObj);
		if(exito){
			cargarEn->_segsEsperaInicial	= datosXml->nodoHijo("segsIni", 0.0f, nodoXml);
			SI32 tipP						= datosXml->nodoHijo("tipP", 0, nodoXml); cargarEn->establecerTipoPresentacion((ENElectronodoPresenta)tipP);
			const sNodoXML* nodoPasos		= datosXml->nodoHijo("pasos", nodoXml);
			if(nodoPasos != NULL){
				const sNodoXML* nodoPaso	= datosXml->nodoHijo("paso", nodoPasos);
				while(nodoPaso != NULL){
					float segsDuracion		= datosXml->nodoHijo("segsDur", 0.0f, nodoPaso);
					//
					SI32 idCapa				= -1;
					SI32 idObjeto			= -1;
					const sNodoXML* nodoIds = datosXml->nodoHijo("iObj", nodoPaso);
					if(nodoIds != NULL){
						const char* cadenaCSV	= datosXml->cadenaValorDeNodo(nodoIds);
						SI32 tamCadenaCSV		= AUCadena8::tamano(cadenaCSV);
						SI32 posEnCadenaCSV		= 0;
						idCapa					= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, -1);
						idObjeto				= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, -1);
					}
					if(idCapa!=-1 && idObjeto==-1){ idObjeto = idCapa; idCapa = idCapaPorDefecto; } //Compatibilidad con el formato cuando solo se incluia el idObjeto
					AUEscenaObjeto* objComando	= NULL;
					SI32 indiceCapa				= arregloIDsCapas->indiceDe(idCapa);
					if(indiceCapa!=-1){
						AUArregloNativoP<AUEscenaObjeto*>* arregloObjetos = (AUArregloNativoP<AUEscenaObjeto*>*)arreglosObjetosPorCapa->elemento[indiceCapa];
						AUArregloNativoP<SI32>* arregloIDsObjetos = (AUArregloNativoP<SI32>*)arregloIDsObjetosPorCapa->elemento[indiceCapa];
						SI32 indiceObjeto		= arregloIDsObjetos->indiceDe(idObjeto);
						if(indiceObjeto!=-1){
							objComando			= arregloObjetos->elemento[indiceObjeto];
						}
					}
					cargarEn->agregarPasoLanzamiento(segsDuracion, objComando);
					//
					nodoPaso	= datosXml->nodoHijo("paso", nodoPasos, nodoPaso);
				}
			}
		}
	}
	//}
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return exito;
}

bool AUElectronodo::agregarBitsInternosEn(AUElectronodo* electronodo, AUArchivo* guardarEn, AUArreglo* arregloCapasPorID){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUElectronodo::agregarBitsInternosEn")
	bool exito = false;
	//
	float versionDatos = 1.0f;
	guardarEn->escribir(&versionDatos, sizeof(versionDatos), 1, guardarEn);
	if(AUEscenaObjeto::agregarBitsInternosEn(electronodo, guardarEn)){
		guardarEn->escribir(&electronodo->_segsEsperaInicial, sizeof(electronodo->_segsEsperaInicial), 1, guardarEn);
		SI32 tipP = electronodo->_tipoPresentacion; guardarEn->escribir(&tipP, sizeof(tipP), 1, guardarEn);
		//
		SI32 iCmd, conteoComandos = electronodo->_pasosLanzamiento.conteo;
		guardarEn->escribir(&conteoComandos, sizeof(conteoComandos), 1, guardarEn);
		for(iCmd=0; iCmd<conteoComandos; iCmd++){
			STElectronodoPaso* comando	= &(electronodo->_pasosLanzamiento.elemento[iCmd]);
			SI32 indiceCapa = -1, indiceObjeto = -1;
			if(comando->objDestino != NULL){
				AUEscenaContenedor* contenedor = (AUEscenaContenedor*)((AUEscenaObjeto*)comando->objDestino)->contenedor(); NBASSERT(comando->objDestino->esClase(AUEscenaObjeto::idTipoClase))
				if(contenedor != NULL){
					indiceCapa = arregloCapasPorID->indiceDe(contenedor);
					if(indiceCapa!=-1){
						indiceObjeto = contenedor->hijos()->indiceDe(comando->objDestino);
					}
				}
			}
			float segsDuracion			= comando->segsDuracion;
			guardarEn->escribir(&segsDuracion, sizeof(segsDuracion), 1, guardarEn);
			guardarEn->escribir(&indiceCapa, sizeof(indiceCapa), 1, guardarEn);
			guardarEn->escribir(&indiceObjeto, sizeof(indiceObjeto), 1, guardarEn);
		}
		exito = true;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return exito;
}

bool AUElectronodo::interpretarBitsInternos(AUElectronodo* cargarEn, AUArchivo* cargarDe, AUArreglo* capasCargadas){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUElectronodo::interpretarBitsInternos")
	bool exito = false;
	float versionDatos;
	cargarDe->leer(&versionDatos, sizeof(versionDatos), 1, cargarDe);
	if(versionDatos==1.0f){
		if(AUEscenaObjeto::interpretarBitsInternos(cargarEn, cargarDe)){
			cargarDe->leer(&cargarEn->_segsEsperaInicial, sizeof(cargarEn->_segsEsperaInicial), 1, cargarDe);
			SI32 tipP; cargarDe->leer(&tipP, sizeof(tipP), 1, cargarDe); cargarEn->establecerTipoPresentacion((ENElectronodoPresenta)tipP);
			//
			SI32 iCmd, conteoComandos; cargarDe->leer(&conteoComandos, sizeof(conteoComandos), 1, cargarDe);
			for(iCmd=0; iCmd<conteoComandos; iCmd++){
				float segsDuracion;			cargarDe->leer(&segsDuracion, sizeof(segsDuracion), 1, cargarDe);
				SI32 iCapaEnArreglo;		cargarDe->leer(&iCapaEnArreglo, sizeof(iCapaEnArreglo), 1, cargarDe);
				SI32 iObjEnArreglo;			cargarDe->leer(&iObjEnArreglo, sizeof(iObjEnArreglo), 1, cargarDe);
				AUEscenaObjeto* objComando	= NULL;
				if(iCapaEnArreglo>=0 && iCapaEnArreglo<capasCargadas->conteo){
					AUArreglo* hijos = ((AUEscenaContenedor*)capasCargadas->elemento[iCapaEnArreglo])->hijos();
					if(iObjEnArreglo>=0 && iObjEnArreglo<hijos->conteo){
						objComando = (AUEscenaObjeto*) hijos->elemento[iObjEnArreglo];
					}
				}
				cargarEn->agregarPasoLanzamiento(segsDuracion, objComando);
			}
			exito = true;
		}
	} else {
		PRINTF_ERROR("AUElectronodo, la version (%f) de datos binarios no es interpretable por este ejecutable\n", versionDatos);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return exito;
}

//
AUOBJMETODOS_CLASESID_MULTICLASE(AUElectronodo, AUEscenaContenedor)
AUOBJMETODOS_CLASESNOMBRES_MULTICLASE(AUElectronodo, "AUElectronodo", AUEscenaContenedor)
AUOBJMETODOS_CLONAR_THIS(AUElectronodo)








