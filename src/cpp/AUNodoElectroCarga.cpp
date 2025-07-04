//
//  AUNodoElectroCarga.cpp
//  DodoTownNucleo
//
//  Created by Marcos Ortega on 18/12/12.
//
//

#include "AUFrameworkMediaStdAfx.h"
#include "AUNodoElectroCarga.h"

AUNodoElectroCarga::AUNodoElectroCarga() : AUEscenaContenedor(), _electroNodosSalientesss(this), _electroNodosEntrantes(this) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUNodoElectroCarga::AUNodoElectroCarga")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUNodoElectroCarga")
	NBASSERT(_electroNodosSalientesss.conteo==0)
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

AUNodoElectroCarga::AUNodoElectroCarga(AUNodoElectroCarga* otraInstancia) : AUEscenaContenedor(), _electroNodosSalientesss(this, otraInstancia->_electroNodosSalientesss.conteo), _electroNodosEntrantes(this, otraInstancia->_electroNodosEntrantes.conteo) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUNodoElectroCarga::AUNodoElectroCarga")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUNodoElectroCarga")
	this->copiarPropiedadesDe(otraInstancia);
	//Los electroNodos destinos no se heredan.
	//Deben reestablecerse manualmente.
	NBASSERT(_electroNodosSalientesss.conteo==0)
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

AUNodoElectroCarga::~AUNodoElectroCarga(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUNodoElectroCarga::~AUNodoElectroCarga")
	vaciarDestinosElectroCargas();
	NBASSERT(_electroNodosEntrantes.conteo==0) //Si falla, los salientes no notificaron a este entrante
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

//

void AUNodoElectroCarga::privAgregarOrigenElectroCarga(AUNodoElectroCarga* objOrigen){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUNodoElectroCarga::privAgregarOrigenElectroCarga")
	NBASSERT(objOrigen != NULL)
	NBASSERT(_electroNodosEntrantes.indiceDe(objOrigen)==-1)
	_electroNodosEntrantes.agregarElemento(objOrigen);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUNodoElectroCarga::privQuitarOrigenElectroCarga(AUNodoElectroCarga* objOrigen){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUNodoElectroCarga::privAgregarOrigenElectroCarga")
	NBASSERT(objOrigen != NULL)
	NBASSERT(_electroNodosEntrantes.indiceDe(objOrigen)!=-1)
	_electroNodosEntrantes.quitarElemento(objOrigen);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

float AUNodoElectroCarga::electrocargaVirtualTotalRecibida(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUNodoElectroCarga::electrocargaVirtualTotalRecibida")
	float cargaTotal = 0.0f;
	UI16 i;
	for(i=0; i<_electroNodosEntrantes.conteo; i++){
		cargaTotal += ((AUNodoElectroCarga*)_electroNodosEntrantes.elemento[i])->electrocargaVirtualParaDestinos();
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return cargaTotal;
}

float AUNodoElectroCarga::electrocargaRealTotalRecibida(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUNodoElectroCarga::electrocargaRealTotalRecibida")
	float cargaTotal = 0.0f;
	UI16 i;
	for(i=0; i<_electroNodosEntrantes.conteo; i++){
		cargaTotal += ((AUNodoElectroCarga*)_electroNodosEntrantes.elemento[i])->electrocargaRealParaDestinos();
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return cargaTotal;
}

AUArregloNativoP<STNodoElectroSaliente>* AUNodoElectroCarga::electroNodosSalientes(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUNodoElectroCarga::electroNodosSalientes")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return &_electroNodosSalientesss;
}

void AUNodoElectroCarga::agregarDestinoElectroCarga(AUNodoElectroCarga* objDestino, float factorLongitudCable, float segsParaCorteCable){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUNodoElectroCarga::agregarDestinoElectroCarga")
	NBASSERT(objDestino != NULL)
	NBASSERT(objDestino->esClase(AUNodoElectroCarga::idTipoClase))
	STNodoElectroSaliente nodoSaliente;
	nodoSaliente.objNodoDestino			= objDestino; objDestino->retener(NB_RETENEDOR_THIS);
	nodoSaliente.objCable				= new(this) AUEscenaCuerda(ENCuerdaTipo_Continua, NBGestorTexturas::texturaDesdeArchivo(segsParaCorteCable==0.0f?AUNODOELECTROCARGA_TEXTURA_CABLE_NO_CORTABLE:AUNODOELECTROCARGA_TEXTURA_CABLE_CORTABLE), 128.0f, 0.1f, 0.0f);
	if(segsParaCorteCable!=0.0f){
		if(objDestino->electrocargaEsSoloConsumidor()){
			nodoSaliente.objCable->establecerMultiplicadorColor8(200, 200, 0, 255);
		} else {
			nodoSaliente.objCable->establecerMultiplicadorColor8(200, 0, 0, 255);
		}
	}
	nodoSaliente.objCable->habilitarAnclaCuerdaIni(0.0f, 0.0f);
	nodoSaliente.objCable->establecerSegsNecesariosParaCorte(segsParaCorteCable);
	nodoSaliente.objCable->establecerEscuchadorCuerda(this);
	nodoSaliente.objCable2				= NULL;
	nodoSaliente.factorLongitudCable	= factorLongitudCable;
	nodoSaliente.segsParaCorteCable		= segsParaCorteCable;
	nodoSaliente.cuerdaInicializada		= false;
	nodoSaliente.objNodoDestino->privAgregarOrigenElectroCarga(this);
	_electroNodosSalientesss.agregarElemento(nodoSaliente);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUNodoElectroCarga::agregarDestinosElectroCargarSegunPlantilla(AUNodoElectroCarga* instanciaPlantilla, AUArreglo* hijosPlantilla, AUArreglo* hijosLocal){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUNodoElectroCarga::agregarDestinosElectroCargarSegunPlantilla")
	STNodoElectroSaliente* salientes = instanciaPlantilla->_electroNodosSalientesss.elemento;
	UI16 iDst, conteoDest = instanciaPlantilla->_electroNodosSalientesss.conteo;
	for(iDst=0; iDst<conteoDest; iDst++){
		SI32 indiceEnPlantilla = hijosPlantilla->indiceDe(salientes[iDst].objNodoDestino);
		if(indiceEnPlantilla>=0 && indiceEnPlantilla<hijosLocal->conteo){
			AUObjeto* equivalenteLocal = hijosLocal->elemento[indiceEnPlantilla];
			if(equivalenteLocal->esClase(AUNodoElectroCarga::idTipoClase)) this->agregarDestinoElectroCarga((AUNodoElectroCarga*)equivalenteLocal, salientes[iDst].factorLongitudCable, salientes[iDst].segsParaCorteCable);
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUNodoElectroCarga::actualizarPropiedadesDestinoElectroCarga(AUNodoElectroCarga* objDestino, float factorLongitudCable, float segsParaCorteCable){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUNodoElectroCarga::actualizarPropiedadesDestinoElectroCarga")
	NBASSERT(objDestino != NULL)
	SI32 indice = -1;
	UI16 iSal;
	for(iSal=0; iSal<_electroNodosSalientesss.conteo; iSal++){
		if(_electroNodosSalientesss.elemento[iSal].objNodoDestino==objDestino) indice = iSal;
	}
	NBASSERT(indice!=-1)
	float segsParaCorteAnterior = _electroNodosSalientesss.elemento[indice].segsParaCorteCable;
	_electroNodosSalientesss.elemento[indice].factorLongitudCable = factorLongitudCable;
	_electroNodosSalientesss.elemento[indice].segsParaCorteCable = segsParaCorteCable;
	if((segsParaCorteAnterior==0.0f && segsParaCorteCable!=0.0f) || (segsParaCorteAnterior!=0.0f && segsParaCorteCable==0.0f)){
		AUTextura* texCable		= NBGestorTexturas::texturaDesdeArchivo(segsParaCorteCable==0.0f?AUNODOELECTROCARGA_TEXTURA_CABLE_NO_CORTABLE:AUNODOELECTROCARGA_TEXTURA_CABLE_CORTABLE); NBASSERT(texCable != NULL)
		_electroNodosSalientesss.elemento[indice].objCable->establecerTexturaSegmentos(texCable);
		if(segsParaCorteCable!=0.0f){
			if(objDestino->electrocargaEsSoloConsumidor()){
				_electroNodosSalientesss.elemento[indice].objCable->establecerMultiplicadorColor8(200, 200, 200, 255);
			} else {
				_electroNodosSalientesss.elemento[indice].objCable->establecerMultiplicadorColor8(200, 0, 0, 255);
			}
		} else {
			_electroNodosSalientesss.elemento[indice].objCable->establecerMultiplicadorColor8(255, 255, 255, 255);
		}
		if(_electroNodosSalientesss.elemento[indice].objCable2 != NULL){
			_electroNodosSalientesss.elemento[indice].objCable2->establecerTexturaSegmentos(texCable);
			_electroNodosSalientesss.elemento[indice].objCable2->establecerMultiplicadorColor8(_electroNodosSalientesss.elemento[indice].objCable->_propiedades.color8);
		}
	}
	if(_electroNodosSalientesss.elemento[indice].objCable != NULL && _electroNodosSalientesss.elemento[indice].objCable2 == NULL) _electroNodosSalientesss.elemento[indice].objCable->establecerSegsNecesariosParaCorte(segsParaCorteCable);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUNodoElectroCarga::actualizarPropiedadesDestinoElectroCargaEnIndice(UI16 indiceDestino, float factorLongitudCable, float segsParaCorteCable){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUNodoElectroCarga::actualizarPropiedadesDestinoElectroCargaEnIndice")
	NBASSERT(indiceDestino<_electroNodosSalientesss.conteo)
	float segsParaCorteAnterior = _electroNodosSalientesss.elemento[indiceDestino].segsParaCorteCable;
	_electroNodosSalientesss.elemento[indiceDestino].factorLongitudCable = factorLongitudCable;
	_electroNodosSalientesss.elemento[indiceDestino].segsParaCorteCable = segsParaCorteCable;
	if(_electroNodosSalientesss.elemento[indiceDestino].objCable != NULL && _electroNodosSalientesss.elemento[indiceDestino].objCable2 == NULL) _electroNodosSalientesss.elemento[indiceDestino].objCable->establecerSegsNecesariosParaCorte(segsParaCorteCable);
	if((segsParaCorteAnterior==0.0f && segsParaCorteCable!=0.0f) || (segsParaCorteAnterior!=0.0f && segsParaCorteCable==0.0f)){
		AUTextura* texCable		= NBGestorTexturas::texturaDesdeArchivo(segsParaCorteCable==0.0f?AUNODOELECTROCARGA_TEXTURA_CABLE_NO_CORTABLE:AUNODOELECTROCARGA_TEXTURA_CABLE_CORTABLE); NBASSERT(texCable != NULL)
		_electroNodosSalientesss.elemento[indiceDestino].objCable->establecerTexturaSegmentos(texCable);
		if(segsParaCorteCable!=0.0f){
			if(_electroNodosSalientesss.elemento[indiceDestino].objNodoDestino->electrocargaEsSoloConsumidor()){
				_electroNodosSalientesss.elemento[indiceDestino].objCable->establecerMultiplicadorColor8(200, 200, 0, 255);
			} else {
				_electroNodosSalientesss.elemento[indiceDestino].objCable->establecerMultiplicadorColor8(200, 0, 0, 255);
			}
		} else {
			_electroNodosSalientesss.elemento[indiceDestino].objCable->establecerMultiplicadorColor8(255, 255, 255, 255);
		}
		if(_electroNodosSalientesss.elemento[indiceDestino].objCable2 != NULL){
			_electroNodosSalientesss.elemento[indiceDestino].objCable2->establecerTexturaSegmentos(texCable);
			_electroNodosSalientesss.elemento[indiceDestino].objCable2->establecerMultiplicadorColor8(_electroNodosSalientesss.elemento[indiceDestino].objCable->_propiedades.color8);
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUNodoElectroCarga::quitarDestinoElectroCarga(AUNodoElectroCarga* objDestino){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUNodoElectroCarga::quitarDestinoElectroCarga")
	NBASSERT(objDestino != NULL)
	SI32 indice = -1;
	UI16 iSal;
	for(iSal=0; iSal<_electroNodosSalientesss.conteo; iSal++){
		if(_electroNodosSalientesss.elemento[iSal].objNodoDestino==objDestino) indice = iSal;
	}
	NBASSERT(indice!=-1)
	STNodoElectroSaliente nodoSaliente = _electroNodosSalientesss.elemento[indice];
	if(nodoSaliente.objCable2 == NULL) nodoSaliente.objNodoDestino->privQuitarOrigenElectroCarga(this); //Solo si no se ha cortado
	nodoSaliente.objNodoDestino->liberar(NB_RETENEDOR_THIS);
	if(nodoSaliente.objCable != NULL){
		if(nodoSaliente.objCable->contenedor() != NULL) ((AUEscenaContenedor*)nodoSaliente.objCable->contenedor())->quitarObjetoEscena(nodoSaliente.objCable);
		nodoSaliente.objCable->liberar(NB_RETENEDOR_THIS);
	}
	if(nodoSaliente.objCable2 != NULL){
		if(nodoSaliente.objCable2->contenedor() != NULL) ((AUEscenaContenedor*)nodoSaliente.objCable2)->quitarObjetoEscena(nodoSaliente.objCable2);
		nodoSaliente.objCable2->liberar(NB_RETENEDOR_THIS);
	}
	_electroNodosSalientesss.quitarElementoEnIndice(indice);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUNodoElectroCarga::quitarDestinoElectroCargaEnIndice(UI16 indice){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUNodoElectroCarga::quitarDestinoElectroCargaEnIndice")
	NBASSERT(indice<_electroNodosSalientesss.conteo)
	STNodoElectroSaliente nodoSaliente = _electroNodosSalientesss.elemento[indice];
	if(nodoSaliente.objCable2 == NULL) nodoSaliente.objNodoDestino->privQuitarOrigenElectroCarga(this); //Solo si no se ha cortado
	nodoSaliente.objNodoDestino->liberar(NB_RETENEDOR_THIS);
	if(nodoSaliente.objCable != NULL){
		if(nodoSaliente.objCable->contenedor() != NULL) ((AUEscenaContenedor*)nodoSaliente.objCable->contenedor())->quitarObjetoEscena(nodoSaliente.objCable);
		nodoSaliente.objCable->liberar(NB_RETENEDOR_THIS);
	}
	if(nodoSaliente.objCable2 != NULL){
		if(nodoSaliente.objCable2->contenedor() != NULL) ((AUEscenaContenedor*)nodoSaliente.objCable2)->quitarObjetoEscena(nodoSaliente.objCable2);
		nodoSaliente.objCable2->liberar(NB_RETENEDOR_THIS);
	}
	_electroNodosSalientesss.quitarElementoEnIndice(indice);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUNodoElectroCarga::vaciarDestinosElectroCargas(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUNodoElectroCarga::vaciarDestinosElectroCargas")
	UI16 iDest;
	for(iDest=0; iDest<_electroNodosSalientesss.conteo; iDest++){
		STNodoElectroSaliente* nodoSaliente	= &_electroNodosSalientesss.elemento[iDest];
		if(nodoSaliente->objCable2 == NULL) nodoSaliente->objNodoDestino->privQuitarOrigenElectroCarga(this); //Solo si no se ha cortado
		nodoSaliente->objNodoDestino->liberar(NB_RETENEDOR_THIS);
		if(nodoSaliente->objCable != NULL){
			if(nodoSaliente->objCable->contenedor() != NULL) ((AUEscenaContenedor*)nodoSaliente->objCable->contenedor())->quitarObjetoEscena(nodoSaliente->objCable);
			nodoSaliente->objCable->liberar(NB_RETENEDOR_THIS);
		}
		if(nodoSaliente->objCable2 != NULL){
			if(nodoSaliente->objCable2->contenedor() != NULL) ((AUEscenaContenedor*)nodoSaliente->objCable2)->quitarObjetoEscena(nodoSaliente->objCable2);
			nodoSaliente->objCable2->liberar(NB_RETENEDOR_THIS);
		}
	}
	_electroNodosSalientesss.vaciar();
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

//

void AUNodoElectroCarga::agregadoEnEscena(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUNodoElectroCarga::agregadoEnEscena")
	NBGestorAnimadores::agregarAnimador(this, this);
	AUEscenaContenedor::agregadoEnEscena();
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUNodoElectroCarga::quitandoDeEscena(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUNodoElectroCarga::quitandoDeEscena")
	AUEscenaContenedor::quitandoDeEscena();
	NBGestorAnimadores::quitarAnimador(this);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUNodoElectroCarga::cuerdaCortada(AUEscenaCuerda* objCuerda, UI16 iSegmentoCorte){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUNodoElectroCarga::cuerdaCortada")
	SI32 indiceUnionSaliente = -1;
	STNodoElectroSaliente* arrDests = _electroNodosSalientesss.elemento;
	UI16 iDst, conteoDest = _electroNodosSalientesss.conteo;
	for(iDst=0; iDst<conteoDest; iDst++){
		if(arrDests[iDst].objCable==objCuerda){
			indiceUnionSaliente = iDst;
			break;
		}
	}
	NBASSERT(indiceUnionSaliente!=-1)
	if(indiceUnionSaliente!=-1){
		STNodoElectroSaliente* datosDestino = &arrDests[indiceUnionSaliente];
		NBASSERT(datosDestino->objCable2 == NULL)
		if(datosDestino->objCable2 == NULL){
			NBASSERT(objCuerda->idEscena!=-1)
			datosDestino->objCable2			= objCuerda->crearCuerdaAPartirDeSegmento(iSegmentoCorte); //Es autoliberable
			datosDestino->objCable2->retener(NB_RETENEDOR_THIS);
			datosDestino->objCable2->establecerSegsNecesariosParaCorte(0.0f); //Deshabilita que se siga cortando
			//
			objCuerda->establecerSegsNecesariosParaCorte(0.0f); //Deshabilita que se siga cortando
			objCuerda->quitarSegmentosDespuesDeInclusive(iSegmentoCorte);
			//Notificar a Destino que ya no es entrada de energia
			datosDestino->objNodoDestino->privQuitarOrigenElectroCarga(this);
		}
		
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUNodoElectroCarga::tickAnimacion(float segsTranscurridos){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUNodoElectroCarga::tickAnimacion")
	if(this->_dormido){ AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO_3() }
	AUEscenaContenedor* contenedorElectro			= (AUEscenaContenedor*)this->contenedor(); NBASSERT(contenedorElectro != NULL)
	NBPunto posEste									= this->traslacion();
	SI32 indiceEste									= contenedorElectro->hijos()->indiceDe(this); NBASSERT(indiceEste!=-1)
	UI16 iDest;
	for(iDest=0; iDest<_electroNodosSalientesss.conteo; iDest++){
		STNodoElectroSaliente* nodoSaliente			= &_electroNodosSalientesss.elemento[iDest];
		NBPunto posDestino							= nodoSaliente->objNodoDestino->traslacion();
		SI32 indiceDestino							= contenedorElectro->hijos()->indiceDe(nodoSaliente->objNodoDestino);
		SI32 indiceMenor							= indiceEste<indiceDestino?indiceEste:indiceDestino;
		AUEscenaCuerda* objCable					= nodoSaliente->objCable;
		AUEscenaCuerda* objCable2					= nodoSaliente->objCable2;
		//ASEGURAR EL CONTENEDOR DE LOS CABLES
		if(indiceMenor!=-1){
			AUEscenaContenedor* contenedorCable			= (AUEscenaContenedor*)objCable->contenedor();
			if(contenedorCable!=contenedorElectro){
				if(contenedorCable != NULL) contenedorCable->quitarObjetoEscena(objCable);
				contenedorElectro->agregarObjetoEscenaEnIndice(objCable, indiceMenor);
			}
			if(objCable2 != NULL){
				AUEscenaContenedor* contenedorCable2		= (AUEscenaContenedor*)objCable2->contenedor();
				if(contenedorCable2!=contenedorElectro){
					if(contenedorCable2 != NULL) contenedorCable2->quitarObjetoEscena(objCable2);
					contenedorElectro->agregarObjetoEscenaEnIndice(objCable2, indiceMenor);
				}
			}
		}
		objCable->habilitarAnclaCuerdaIni(posEste.x, posEste.y);
		//Posicion de ancla final
		AUEscenaCuerda* objCableAnclaFinal			= (objCable2 != NULL?objCable2:objCable); NBASSERT(objCableAnclaFinal != NULL)
		if(indiceDestino==-1){
			objCableAnclaFinal->deshabilitarAnclaCuerdaFin();
		} else {
			objCableAnclaFinal->habilitarAnclaCuerdaFin(posDestino.x, posDestino.y);
		}
		//Longitud de cuerda
		if(objCable2 == NULL){
			float distancia					= NBPUNTO_DISTANCIA(posEste.x, posEste.y, posDestino.x, posDestino.y);
			if((distancia * nodoSaliente->factorLongitudCable)!=objCable->longitudHabilitadaCuerda()){
				objCable->establecerLongitudCuerdaHabilitada(distancia * nodoSaliente->factorLongitudCable, 0.0f);
			}
		}
		//Inicializacion de cuerda
		if(!nodoSaliente->cuerdaInicializada){
			NBASSERT(objCable2 == NULL)
			NBMatriz matrizContenedorElectro			= contenedorElectro->matrizEscena();
			NBPunto posEscenaEste;						NBMATRIZ_MULTIPLICAR_PUNTO(posEscenaEste, matrizContenedorElectro, posEste.x, posEste.y)
			NBPunto posEscenaDestino;					NBMATRIZ_MULTIPLICAR_PUNTO(posEscenaDestino, matrizContenedorElectro, posDestino.x, posDestino.y)
			objCable->inicializarCuerdaSegunLinea(posEscenaEste.x/ESCALA_PIXELES_POR_METRO_MUNDO_FISICA, posEscenaEste.y/ESCALA_PIXELES_POR_METRO_MUNDO_FISICA, posEscenaDestino.x/ESCALA_PIXELES_POR_METRO_MUNDO_FISICA, posEscenaDestino.y/ESCALA_PIXELES_POR_METRO_MUNDO_FISICA);
			nodoSaliente->cuerdaInicializada = true;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

//

bool AUNodoElectroCarga::agregarXmlInternoEn(AUNodoElectroCarga* nodoElectroCarga, AUCadenaLargaMutable8* guardarEn, const char* espaciosBaseIzq, AUArreglo* arregloObjetosParaID){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUNodoElectroCarga::agregarXmlInternoEn")
	//guardarEn->agregarConFormato("%s<vXML>1</vXML>\r\n", espaciosBaseIzq);
	guardarEn->agregarConFormato("%s<eOb>\r\n", espaciosBaseIzq);
	AUEscenaObjeto::agregarXmlInternoEn(nodoElectroCarga, guardarEn, espaciosBaseIzq);
	guardarEn->agregarConFormato("%s</eOb>\r\n", espaciosBaseIzq);
	//Destinos de carga electricas
	guardarEn->agregarConFormato("%s<elecDest>\r\n", espaciosBaseIzq);
	UI16 iNodoSal, conteo = nodoElectroCarga->_electroNodosSalientesss.conteo;
	for(iNodoSal=0; iNodoSal<conteo; iNodoSal++){
		SI32 indiceObjEnArreglo = -1;
		indiceObjEnArreglo = arregloObjetosParaID->indiceDe(nodoElectroCarga->_electroNodosSalientesss.elemento[iNodoSal].objNodoDestino);
		if(indiceObjEnArreglo==-1) PRINTF_INFO("-------------------- ADVERTENCIA: destino_electro_carga no pudo ser guardado\n");
		guardarEn->agregarConFormato("%s<i>%d|%f|%f</i>\r\n", espaciosBaseIzq, indiceObjEnArreglo, nodoElectroCarga->_electroNodosSalientesss.elemento[iNodoSal].factorLongitudCable, nodoElectroCarga->_electroNodosSalientesss.elemento[iNodoSal].segsParaCorteCable);
	}
	guardarEn->agregarConFormato("%s</elecDest>\r\n", espaciosBaseIzq);
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return true;
}

bool AUNodoElectroCarga::interpretarXmlInterno(AUNodoElectroCarga* cargarEn, const AUDatosXML* datosXml, const sNodoXML* nodoXml, AUArregloNativoP<AUEscenaObjeto*>* arregloObjetos, AUArregloNativoP<SI32>* arregloIDs){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUNodoElectroCarga::interpretarXmlInterno")
	bool exito = false;
	//int versionXML = datosXml->nodoHijo("vXML", 0, nodoXml);
	//if(versionXML==1){
	const sNodoXML* nodoEscenaObj;		XML_NODO_HIJO(datosXml, nodoEscenaObj, "eOb", "escObj", nodoXml, NULL)
	if(nodoEscenaObj != NULL){
		exito = AUEscenaObjeto::interpretarXmlInterno(cargarEn, datosXml, nodoEscenaObj);
		if(exito){
			//Destinos de carga electricas
			cargarEn->vaciarDestinosElectroCargas();
			const sNodoXML* nodoElecDests = datosXml->nodoHijo("elecDest", nodoXml);
			if(nodoElecDests != NULL){
				const sNodoXML* nodoI = datosXml->nodoHijo("i", nodoElecDests);
				while(nodoI != NULL){
					const char* cadenaCSV	= datosXml->cadenaValorDeNodo(nodoI);
					SI32 tamCadenaCSV		= AUCadena8::tamano(cadenaCSV);
					SI32 posEnCadenaCSV		= 0;
					SI32 iObj				= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, -1);
					float factorLongCable	= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0.8f);
					float segsParaCorteCable = AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 1.0f);
					if(iObj!=-1){
						SI32 indiceObjeto	= arregloIDs->indiceDe(iObj);
						if(indiceObjeto!=-1){
							AUEscenaObjeto* objEscena = arregloObjetos->elemento[indiceObjeto];
							if(objEscena->esClase(AUNodoElectroCarga::idTipoClase)){
								cargarEn->agregarDestinoElectroCarga((AUNodoElectroCarga*)objEscena, factorLongCable, segsParaCorteCable);
							} else {
								PRINTF_INFO("-------------------- ADVERTENCIA: destino_electro_carga no pudo ser cargado (no es clase 'AUNodoElectroCarga')\n");
							}
						} else {
							PRINTF_INFO("-------------------- ADVERTENCIA: destino_electro_carga no pudo ser cargado (no encontrado en arrelog IDs)\n");
						}
					} else {
						PRINTF_INFO("-------------------- ADVERTENCIA: destino_electro_carga no pudo ser cargado (iObj es -1)\n");
					}
					//siguiente
					nodoI = datosXml->nodoHijo("i", nodoElecDests, nodoI);
				}
			}
		}
	}
	//}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

bool AUNodoElectroCarga::agregarBitsInternosEn(AUNodoElectroCarga* nodoElectroCarga, AUArchivo* guardarEn, AUArreglo* arregloObjetosParaID){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUNodoElectroCarga::agregarBitsInternosEn")
	bool exito = false;
	//
	float versionDatos = 1.0f;
	guardarEn->escribir(&versionDatos, sizeof(versionDatos), 1, guardarEn);
	if(AUEscenaObjeto::agregarBitsInternosEn(nodoElectroCarga, guardarEn)){
		//Destinos de carga electricas
		UI32 iNodoSal, conteo = nodoElectroCarga->_electroNodosSalientesss.conteo;
		guardarEn->escribir(&conteo, sizeof(conteo), 1, guardarEn);
		for(iNodoSal=0; iNodoSal<conteo; iNodoSal++){
			SI32 indiceObjEnCarreglo	= arregloObjetosParaID->indiceDe(nodoElectroCarga->_electroNodosSalientesss.elemento[iNodoSal].objNodoDestino);
			float factorLongCable		= nodoElectroCarga->_electroNodosSalientesss.elemento[iNodoSal].factorLongitudCable;
			float segsParaCorteCable	= nodoElectroCarga->_electroNodosSalientesss.elemento[iNodoSal].segsParaCorteCable;
			if(indiceObjEnCarreglo==-1) PRINTF_INFO("-------------------- ADVERTENCIA: destino_electro_carga no pudo ser guardado\n");
			guardarEn->escribir(&indiceObjEnCarreglo, sizeof(indiceObjEnCarreglo), 1, guardarEn);
			guardarEn->escribir(&factorLongCable, sizeof(factorLongCable), 1, guardarEn);
			guardarEn->escribir(&segsParaCorteCable, sizeof(segsParaCorteCable), 1, guardarEn);
		}
		exito = true;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

bool AUNodoElectroCarga::interpretarBitsInternos(AUNodoElectroCarga* cargarEn, AUArchivo* cargarDe, AUArreglo* arregloObjetosParaID){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUNodoElectroCarga::interpretarBitsInternos")
	bool exito = false;
	float versionDatos;
	cargarDe->leer(&versionDatos, sizeof(versionDatos), 1, cargarDe);
	if(versionDatos==1.0f){
		if(AUEscenaObjeto::interpretarBitsInternos(cargarEn, cargarDe)){
			//Destinos de carga electricas
			cargarEn->vaciarDestinosElectroCargas();
			UI32 iNodoSal, conteoObjs;	cargarDe->leer(&conteoObjs, sizeof(conteoObjs), 1, cargarDe);
			for(iNodoSal=0; iNodoSal<conteoObjs; iNodoSal++){
				SI32 iObj;				cargarDe->leer(&iObj, sizeof(iObj), 1, cargarDe);
				float factorLongCable;	cargarDe->leer(&factorLongCable, sizeof(factorLongCable), 1, cargarDe);
				float segsParaCorteCable;	cargarDe->leer(&segsParaCorteCable, sizeof(segsParaCorteCable), 1, cargarDe);
				if(iObj!=-1){
					AUObjeto* objEscena = (AUObjeto*)arregloObjetosParaID->elemento[iObj];
					if(objEscena->esClase(AUNodoElectroCarga::idTipoClase)){
						cargarEn->agregarDestinoElectroCarga((AUNodoElectroCarga*)objEscena, factorLongCable, segsParaCorteCable);
					} else {
						PRINTF_INFO("-------------------- ADVERTENCIA: destino_electro_carga no pudo ser cargado (no es clase 'AUNodoElectroCarga')\n");
					}
				}
			}
			exito = true;
		}
	} else {
		PRINTF_ERROR("AUNodoElectroCarga, la version (%f) de datos binarios no es interpretable por este ejecutable\n", versionDatos);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

//
AUOBJMETODOS_CLASESID_MULTICLASE(AUNodoElectroCarga, AUEscenaContenedor)
AUOBJMETODOS_CLASESNOMBRES_MULTICLASE(AUNodoElectroCarga, "AUNodoElectroCarga", AUEscenaContenedor)
AUOBJMETODOS_CLONAR_NULL(AUNodoElectroCarga)




