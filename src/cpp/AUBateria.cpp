//
//  AUBateria.cpp
//  DodoTownNucleo
//
//  Created by Marcos Ortega on 22/09/12.
//
//

#include "AUFrameworkMediaStdAfx.h"
#include "AUBateria.h"

AUBateria::AUBateria() : AUNodoElectroCarga() {
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUBateria::AUBateria")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUBateria")
	_bateriaActiva				= true;
	_bateriaCargada				= false;
	_desactivarDespuesDeCargar	= false;
	_desactivarDespuesDeDescargar	= false;
	_cargaParaConsiderarCargada	= 0.75f;
	_segsAcumularParaCargar		= 1.0f;
	_segsAcumularParaDescargar	= 1.0f;
	_sensorComandosAlCargar		= NULL;
	_sensorComandosAlDescargar	= NULL;
	_escuchadorBateria			= NULL;
	//
	_presentaBateria			= new(this) AUEscenaSprite(NBGestorTexturas::texturaDesdeArchivo("Escenarios/Electricos/objBateria.png"));
	this->agregarObjetoEscena(_presentaBateria); _presentaBateria->liberar(NB_RETENEDOR_THIS);
	//
	AUTextura* texCarga			= NBGestorTexturas::texturaDesdeArchivo("Escenarios/Electricos/objBateriaLuz.png");
	float xPos					= -27.0f * 0.5f;
	float xAvance				= 18.0f * 0.5f;
	UI16 iCarga;
	for(iCarga=0; iCarga<AUBateria_CANTIDAD_LUCES_INDICADORAS; iCarga++){
		_indicadoresCarga[iCarga] = new(this) AUEscenaSprite(texCarga);
		_indicadoresCarga[iCarga]->establecerIluminoDependencia(false);
		_indicadoresCarga[iCarga]->establecerMultiplicadorColor8((iCarga==0?0:255), (iCarga==0?0:255), (iCarga==0?255:0), 255);
		_indicadoresCarga[iCarga]->establecerTraslacion(xPos, 0.0f);
		this->agregarObjetoEscena(_indicadoresCarga[iCarga]); _indicadoresCarga[iCarga]->liberar(NB_RETENEDOR_THIS);
		xPos += xAvance;
	}
	//
	_presentaMarcoLuces			= new(this) AUEscenaSprite(NBGestorTexturas::texturaDesdeArchivo("Escenarios/Electricos/objBateriaMarcoLuces.png"));
	this->agregarObjetoEscena(_presentaMarcoLuces); _presentaMarcoLuces->liberar(NB_RETENEDOR_THIS);
	//
	_cargaActual				= 0.0f;
	_cargaDestino				= 0.0f;
	_indicaEncendidosAumentado	= false;
	_indicaMaximoCargando		= 0;
	_indicaMaximoCargado		= 0;
	_relIndicaActual			= 1.0f;
	_relIndicaDestino			= 0.0f;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

AUBateria::AUBateria(AUBateria* otraInstancia) : AUNodoElectroCarga(otraInstancia) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUBateria::AUBateria")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUBateria")
	_bateriaActiva				= otraInstancia->_bateriaActiva;
	_bateriaCargada				= otraInstancia->_bateriaCargada;
	_desactivarDespuesDeCargar	= otraInstancia->_desactivarDespuesDeCargar;
	_desactivarDespuesDeDescargar	= otraInstancia->_desactivarDespuesDeDescargar;
	_cargaParaConsiderarCargada	= otraInstancia->_cargaParaConsiderarCargada;
	_segsAcumularParaCargar		= otraInstancia->_segsAcumularParaCargar;
	_segsAcumularParaDescargar	= otraInstancia->_segsAcumularParaDescargar;
	_sensorComandosAlCargar		= NULL;
	_sensorComandosAlDescargar	= NULL;
	_escuchadorBateria			= NULL;
	//
	_presentaBateria			= new(this) AUEscenaSprite(otraInstancia->_presentaBateria);
	this->agregarObjetoEscena(_presentaBateria); _presentaBateria->liberar(NB_RETENEDOR_THIS);
	//
	AUTextura* texCarga			= NBGestorTexturas::texturaDesdeArchivo("Escenarios/Electricos/objBateriaLuz.png");
	float xPos					= -27.0f * 0.5f;
	float xAvance				= 18.0f * 0.5f;
	UI16 iCarga;
	for(iCarga=0; iCarga<AUBateria_CANTIDAD_LUCES_INDICADORAS; iCarga++){
		_indicadoresCarga[iCarga] = new(this) AUEscenaSprite(texCarga);
		_indicadoresCarga[iCarga]->establecerIluminoDependencia(false);
		_indicadoresCarga[iCarga]->establecerMultiplicadorColor8((iCarga==0?0:255), (iCarga==0?0:255), (iCarga==0?255:0), 255);
		_indicadoresCarga[iCarga]->establecerTraslacion(xPos, 0.0f);
		this->agregarObjetoEscena(_indicadoresCarga[iCarga]); _indicadoresCarga[iCarga]->liberar(NB_RETENEDOR_THIS);
		xPos += xAvance;
	}
	//
	_presentaMarcoLuces			= new(this) AUEscenaSprite(otraInstancia->_presentaMarcoLuces);
	this->agregarObjetoEscena(_presentaMarcoLuces); _presentaMarcoLuces->liberar(NB_RETENEDOR_THIS);
	//
	_cargaActual				= 0.0f;
	_cargaDestino				= 0.0f;
	_indicaEncendidosAumentado	= false;
	_indicaMaximoCargando		= 0;
	_indicaMaximoCargado		= 0;
	_relIndicaActual			= 1.0f;
	_relIndicaDestino			= 0.0f;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

AUBateria::~AUBateria(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUBateria::~AUBateria")
	if(_sensorComandosAlDescargar != NULL) _sensorComandosAlDescargar->liberar(NB_RETENEDOR_THIS); _sensorComandosAlDescargar = NULL;
	if(_sensorComandosAlCargar != NULL) _sensorComandosAlCargar->liberar(NB_RETENEDOR_THIS); _sensorComandosAlCargar = NULL;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

//

bool AUBateria::bateriaActiva(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUBateria::bateriaActiva")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _bateriaActiva;
}

bool AUBateria::bateriaCargada(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUBateria::bateriaCargada")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _bateriaCargada;
	
}

bool AUBateria::desactivarDespuesDeCargar(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUBateria::desactivarDespuesDeCargar")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _desactivarDespuesDeCargar;
}

bool AUBateria::desactivarDespuesDeDescargar(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUBateria::desactivarDespuesDeDescargar")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _desactivarDespuesDeDescargar;
}

float AUBateria::cargaParaConsiderarCargada(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUBateria::cargaParaConsiderarCargada")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _cargaParaConsiderarCargada;
}

float AUBateria::segsAcumularParaCargar(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUBateria::segsAcumularParaCargar")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _segsAcumularParaCargar;
}

float AUBateria::segsAcumularParaDescargar(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUBateria::_segsAcumularParaDescargar")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return  _segsAcumularParaDescargar;
}

AUEscenaSensor* AUBateria::sensorComandosAlCargar(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUBateria::sensorComandosAlCargar")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _sensorComandosAlCargar;
}

AUEscenaSensor* AUBateria::sensorComandosAlDescargar(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUBateria::sensorComandosAlDescargar")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _sensorComandosAlDescargar;
}

IEscuchadorBateria* AUBateria::escuchadorBateria(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUBateria::escuchadorBateria")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _escuchadorBateria;
}

//

void AUBateria::establecerBateriaActiva(bool bateriaActiva){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUBateria::establecerBateriaActiva")
	_bateriaActiva = bateriaActiva;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUBateria::establecerDesactivarDespuesDeCargar(bool desactivarDespues){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUBateria::establecerDesactivarDespuesDeCargar")
	_desactivarDespuesDeCargar = desactivarDespues;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUBateria::establecerDesactivarDespuesDeDescargar(bool desactivarDespues){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUBateria::establecerDesactivarDespuesDeDescargar")
	_desactivarDespuesDeDescargar = desactivarDespues;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUBateria::establecerCargaParaConsiderarCargada(float cargaRel){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUBateria::establecerCargaParaConsiderarCargada")
	_cargaParaConsiderarCargada = cargaRel;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUBateria::establecerSegsAcumularParaCargar(float segsAcumParaCargar){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUBateria::establecerSegsAcumularParaCargar")
	_segsAcumularParaCargar = segsAcumParaCargar;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUBateria::establecerSegsAcumularParaDescargar(float segsAcumParaCargar){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUBateria::establecerSegsAcumularParaDescargar")
	_segsAcumularParaDescargar = segsAcumParaCargar;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUBateria::establecerSensorComandosAlCargar(AUEscenaSensor* objSensor){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUBateria::establecerSensorComandosAlCargar")
	if(objSensor != NULL) objSensor->retener(NB_RETENEDOR_THIS);
	if(_sensorComandosAlCargar != NULL) _sensorComandosAlCargar->liberar(NB_RETENEDOR_THIS);
	_sensorComandosAlCargar = objSensor;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUBateria::establecerSensorComandosAlDescargar(AUEscenaSensor* objSensor){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUBateria::establecerSensorComandosAlDescargar")
	if(objSensor != NULL) objSensor->retener(NB_RETENEDOR_THIS);
	if(_sensorComandosAlDescargar != NULL) _sensorComandosAlDescargar->liberar(NB_RETENEDOR_THIS);
	_sensorComandosAlDescargar = objSensor;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUBateria::establecerSensorComandosSegunPlantilla(AUBateria* instanciaPlantilla, AUArreglo* hijosPlantilla, AUArreglo* hijosLocal){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUBateria::establecerSensorComandosSegunPLantilla")
	SI32 iObjOriginal	= -1;
	if(instanciaPlantilla->_sensorComandosAlCargar != NULL) iObjOriginal = hijosPlantilla->indiceDe(instanciaPlantilla->_sensorComandosAlCargar);
	if(iObjOriginal!=-1){
		NBASSERT(iObjOriginal>=0 && iObjOriginal<hijosLocal->conteo)
		AUObjeto* objDestino = hijosLocal->elemento[iObjOriginal];
		if(objDestino->esClase(AUEscenaSensor::idTipoClase)){
			this->establecerSensorComandosAlCargar((AUEscenaSensor*)objDestino);
		}
	}
	iObjOriginal		= -1;
	if(instanciaPlantilla->_sensorComandosAlDescargar != NULL) iObjOriginal = hijosPlantilla->indiceDe(instanciaPlantilla->_sensorComandosAlDescargar);
	if(iObjOriginal!=-1){
		NBASSERT(iObjOriginal>=0 && iObjOriginal<hijosLocal->conteo)
		AUObjeto* objDestino = hijosLocal->elemento[iObjOriginal];
		if(objDestino->esClase(AUEscenaSensor::idTipoClase)){
			this->establecerSensorComandosAlDescargar((AUEscenaSensor*)objDestino);
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUBateria::establecerEscuchadorBateria(IEscuchadorBateria* escuchador){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUBateria::establecerEscuchadorBateria")
	_escuchadorBateria = escuchador;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

//

NBCajaAABB AUBateria::cajaEscenaPresentacion(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUPanelSolar::cajaEscenaPresentacion")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _presentaBateria->cajaAABBEnEscena();
}

float AUBateria::electrocargaVirtualParaDestinos(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUBateria::electrocargaVirtualParaDestinos")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return (_bateriaCargada?_cargaParaConsiderarCargada==0.0f?1.0f:_cargaParaConsiderarCargada:0.0f);
}

float AUBateria::electrocargaRealParaDestinos(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUBateria::electrocargaRealParaDestinos")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return (_cargaParaConsiderarCargada==0.0f?1.0f:_cargaActual);
}

bool AUBateria::electrocargaEsSoloConsumidor(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUBateria::electrocargaEsSoloConsumidor")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return false;
}

void AUBateria::tickAnimacion(float segsTranscurridos){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUBateria::tickAnimacion")
	AUNodoElectroCarga::tickAnimacion(segsTranscurridos);
	if(!this->_dormido && _bateriaActiva){
		bool indicadorActualModificado		= false;
		bool indicadorFinalModificado		= false;
		bool cambioEstadoPanel				= false;
		//Animar carga de panel (encender y apagar)
		if(_cargaActual!=_cargaDestino){
			if(_cargaActual<_cargaDestino){
				_cargaActual += segsTranscurridos / _segsAcumularParaCargar;
				if(_cargaActual>_cargaDestino) _cargaActual = _cargaDestino;
				if(!_bateriaCargada && (_cargaActual/(_cargaParaConsiderarCargada==0.0f?1.0f:_cargaParaConsiderarCargada))>=1.0f){
					_bateriaCargada			= true;
					if(_escuchadorBateria != NULL) _escuchadorBateria->bateriaCargada(this, _sensorComandosAlCargar);
					cambioEstadoPanel		= true;
					PRINTF_INFO("Bateria cargada\n");
				}
			} else {
				_cargaActual -= segsTranscurridos / _segsAcumularParaDescargar;
				if(_cargaActual<_cargaDestino) _cargaActual = _cargaDestino;
				if(_bateriaCargada && _cargaActual==0.0f){
					_bateriaCargada			= false;
					cambioEstadoPanel		= true;
					if(_escuchadorBateria != NULL) _escuchadorBateria->bateriaDescargada(this, _sensorComandosAlDescargar);
					PRINTF_INFO("Bateria descargada\n");
				}
				
			}
			//Establecer colores de indicadores
			UI16 nuevoIndicaMaximo		= AUBateria_CANTIDAD_LUCES_INDICADORAS * _cargaActual / (_cargaParaConsiderarCargada==0.0f?1.0f:_cargaParaConsiderarCargada); if(nuevoIndicaMaximo>=AUBateria_CANTIDAD_LUCES_INDICADORAS) nuevoIndicaMaximo = AUBateria_CANTIDAD_LUCES_INDICADORAS - 1;
			if(!_bateriaCargada && nuevoIndicaMaximo==AUBateria_CANTIDAD_LUCES_INDICADORAS - 1) nuevoIndicaMaximo--;
			NBASSERT(nuevoIndicaMaximo<AUBateria_CANTIDAD_LUCES_INDICADORAS)
			if(nuevoIndicaMaximo!=_indicaMaximoCargado){
				_indicaMaximoCargado	= nuevoIndicaMaximo;
				indicadorActualModificado = true;
			}
		}
		//Calcular iluminacion recibida al panel
		if(_cargaParaConsiderarCargada==0.0f){
			_cargaDestino				= 1.0f;
		} else {
			_cargaDestino				= this->electrocargaVirtualTotalRecibida();
		}
		UI16 nuevoIndicaMaximoCargando	= AUBateria_CANTIDAD_LUCES_INDICADORAS * _cargaDestino / (_cargaParaConsiderarCargada==0.0f?1.0f:_cargaParaConsiderarCargada); if(nuevoIndicaMaximoCargando>=AUBateria_CANTIDAD_LUCES_INDICADORAS) nuevoIndicaMaximoCargando = AUBateria_CANTIDAD_LUCES_INDICADORAS - 1;
		NBASSERT(nuevoIndicaMaximoCargando<AUBateria_CANTIDAD_LUCES_INDICADORAS)
		if(nuevoIndicaMaximoCargando!=_indicaMaximoCargando){
			_indicaEncendidosAumentado	= (_indicaMaximoCargando<nuevoIndicaMaximoCargando);
			_indicaMaximoCargando		= nuevoIndicaMaximoCargando;
			indicadorFinalModificado	= true;
		}
		if(cambioEstadoPanel || indicadorActualModificado || indicadorFinalModificado){
			//Pintar colores a botones
			UI16 iInd;
			if(_cargaActual==0.0f){
				for(iInd=0; iInd<AUBateria_CANTIDAD_LUCES_INDICADORAS; iInd++){
					_indicadoresCarga[iInd]->establecerMultiplicadorColor8(255, 0, 0, 255); //rojo
				}
			} else if(_indicaMaximoCargado==(AUBateria_CANTIDAD_LUCES_INDICADORAS-1)){
				for(iInd=0; iInd<AUBateria_CANTIDAD_LUCES_INDICADORAS; iInd++){
					_indicadoresCarga[iInd]->establecerMultiplicadorColor8(255, 255, 0, 255); //verde
				}
			} else if(_indicaEncendidosAumentado){
				for(iInd=0; iInd<AUBateria_CANTIDAD_LUCES_INDICADORAS; iInd++){
					if(iInd<=_indicaMaximoCargado){
						_indicadoresCarga[iInd]->establecerMultiplicadorColor8(255, 255, 0, 255); //verde
					} else {
						_indicadoresCarga[iInd]->establecerMultiplicadorColor8(255, 0, 0, 255); //rojo
					}
				}
			} else {
				for(iInd=0; iInd<AUBateria_CANTIDAD_LUCES_INDICADORAS; iInd++){
					if(iInd<=_indicaMaximoCargado){
						_indicadoresCarga[iInd]->establecerMultiplicadorColor8(255, 255, 0, 255); //amarillo
					} else {
						_indicadoresCarga[iInd]->establecerMultiplicadorColor8(255, 0, 0, 255); //rojo
					}
				}
			}
			_relIndicaActual		= 1.0f;
			_relIndicaDestino		= 0.0f;
		} else {
			//Animar parpadeo de indicador-maximo actual
			if(_relIndicaActual!=_relIndicaDestino){
				if(_relIndicaActual<_relIndicaDestino){
					_relIndicaActual += segsTranscurridos * 2.0f;
					if(_relIndicaActual>_relIndicaDestino) _relIndicaActual = _relIndicaDestino;
				} else {
					_relIndicaActual -= segsTranscurridos * 2.0f;
					if(_relIndicaActual<_relIndicaDestino) _relIndicaActual = _relIndicaDestino;
				}
				if(_cargaActual==0.0f){
					_indicadoresCarga[_indicaMaximoCargado]->establecerMultiplicadorColor8(255 * _relIndicaActual, 0, 0, 255);
				} else if(_indicaMaximoCargado==(AUBateria_CANTIDAD_LUCES_INDICADORAS-1)){
					_indicadoresCarga[_indicaMaximoCargado]->establecerMultiplicadorColor8(255 * _relIndicaActual, 255 * _relIndicaActual, 0, 255);
				}
			} else {
				_relIndicaDestino		= (_relIndicaActual==0.0f?1.0f:0.0f);
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

bool AUBateria::agregarXmlInternoEn(AUBateria* laser, AUCadenaLargaMutable8* guardarEn, const char* espaciosBaseIzq, AUArreglo* arregloObjetosParaID){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUBateria::agregarXmlInternoEn")
	//guardarEn->agregarConFormato("%s<vXML>1</vXML>\r\n", espaciosBaseIzq);
	AUNodoElectroCarga::agregarXmlInternoEn(laser, guardarEn, espaciosBaseIzq, arregloObjetosParaID);
	guardarEn->agregarConFormato("%s<act>%d</act>\r\n", espaciosBaseIzq, laser->_bateriaActiva?1:0);
	guardarEn->agregarConFormato("%s<desAlEnc>%d</desAlEnc>\r\n", espaciosBaseIzq, laser->_desactivarDespuesDeCargar?1:0);
	guardarEn->agregarConFormato("%s<desAlApa>%d</desAlApa>\r\n", espaciosBaseIzq, laser->_desactivarDespuesDeDescargar?1:0);
	guardarEn->agregarConFormato("%s<promRGBMin>%f</promRGBMin>\r\n", espaciosBaseIzq, laser->_cargaParaConsiderarCargada);
	guardarEn->agregarConFormato("%s<segsEnc>%f</segsEnc>\r\n", espaciosBaseIzq, laser->_segsAcumularParaCargar);
	guardarEn->agregarConFormato("%s<segsApa>%f</segsApa>\r\n", espaciosBaseIzq, laser->_segsAcumularParaDescargar);
	SI32 indiceObjEnArreglo = -1;
	if(laser->_sensorComandosAlCargar != NULL){
		indiceObjEnArreglo = arregloObjetosParaID->indiceDe(laser->_sensorComandosAlCargar);
		if(indiceObjEnArreglo==-1) PRINTF_INFO("-------------------- ADVERTENCIA: destino de panel-encender no pudo ser guardado\n");
	}
	guardarEn->agregarConFormato("%s<iSen>%d</iSen>\r\n", espaciosBaseIzq, indiceObjEnArreglo);
	indiceObjEnArreglo = -1;
	if(laser->_sensorComandosAlDescargar != NULL){
		indiceObjEnArreglo = arregloObjetosParaID->indiceDe(laser->_sensorComandosAlDescargar);
		if(indiceObjEnArreglo==-1) PRINTF_INFO("-------------------- ADVERTENCIA: destino de panel-apagar no pudo ser guardado\n");
	}
	guardarEn->agregarConFormato("%s<iSenD>%d</iSenD>\r\n", espaciosBaseIzq, indiceObjEnArreglo);
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return true;
}

bool AUBateria::interpretarXmlInterno(AUBateria* cargarEn, const AUDatosXML* datosXml, const sNodoXML* nodoXml, AUArregloNativoP<AUEscenaObjeto*>* arregloObjetos, AUArregloNativoP<SI32>* arregloIDs){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUBateria::interpretarXmlInterno")
	bool exito = false;
	//int versionXML = datosXml->nodoHijo("vXML", 0, nodoXml);
	//if(versionXML==1){
	if(AUNodoElectroCarga::interpretarXmlInterno(cargarEn, datosXml, nodoXml, arregloObjetos, arregloIDs)){
		cargarEn->_bateriaActiva		= datosXml->nodoHijo("act", true, nodoXml);
		cargarEn->_desactivarDespuesDeCargar	= datosXml->nodoHijo("desAlEnc", false, nodoXml);
		cargarEn->_desactivarDespuesDeDescargar	= datosXml->nodoHijo("desAlApa", false, nodoXml);
		cargarEn->_cargaParaConsiderarCargada	= datosXml->nodoHijo("promRGBMin", 0.75f, nodoXml);
		cargarEn->_segsAcumularParaCargar		= datosXml->nodoHijo("segsEnc", 1.0f, nodoXml);
		cargarEn->_segsAcumularParaDescargar		= datosXml->nodoHijo("segsApa", 1.0f, nodoXml);
		//
		AUEscenaSensor* objetoDestino = NULL;
		SI32 iObj = datosXml->nodoHijo("iSen", -1, nodoXml);
		if(iObj!=-1){
			SI32 indiceObjeto = arregloIDs->indiceDe(iObj);
			if(indiceObjeto!=-1){
				AUEscenaObjeto* objEscena = arregloObjetos->elemento[indiceObjeto];
				if(objEscena->esClase(AUEscenaSensor::idTipoClase)){
					objetoDestino = (AUEscenaSensor*)objEscena;
				} else {
					PRINTF_INFO("-------------------- ADVERTENCIA: panel-encender no pudo ser cargado (no es clase 'AUEscenaSensor')\n");
				}
			}
		}
		cargarEn->establecerSensorComandosAlCargar(objetoDestino);
		objetoDestino = NULL;
		iObj = datosXml->nodoHijo("iSenD", -1, nodoXml);
		if(iObj!=-1){
			SI32 indiceObjeto = arregloIDs->indiceDe(iObj);
			if(indiceObjeto!=-1){
				AUEscenaObjeto* objEscena = arregloObjetos->elemento[indiceObjeto];
				if(objEscena->esClase(AUEscenaSensor::idTipoClase)){
					objetoDestino = (AUEscenaSensor*)objEscena;
				} else {
					PRINTF_INFO("-------------------- ADVERTENCIA: panel-apagar no pudo ser cargado (no es clase 'AUEscenaSensor')\n");
				}
			}
		}
		cargarEn->establecerSensorComandosAlDescargar(objetoDestino);
		exito = true;
	}
	//}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

bool AUBateria::agregarBitsInternosEn(AUBateria* laser, AUArchivo* guardarEn, AUArreglo* arregloObjetosParaID){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUBateria::agregarBitsInternosEn")
	bool exito = false;
	//
	float versionDatos = 1.0f;
	guardarEn->escribir(&versionDatos, sizeof(versionDatos), 1, guardarEn);
	if(AUNodoElectroCarga::agregarBitsInternosEn(laser, guardarEn, arregloObjetosParaID)){
		bool activo		= laser->_bateriaActiva;					guardarEn->escribir(&activo, sizeof(activo), 1, guardarEn);
		bool desactivar = laser->_desactivarDespuesDeCargar;	guardarEn->escribir(&desactivar, sizeof(desactivar), 1, guardarEn);
		bool desactivD	= laser->_desactivarDespuesDeDescargar;	guardarEn->escribir(&desactivD, sizeof(desactivD), 1, guardarEn);
		guardarEn->escribir(&laser->_cargaParaConsiderarCargada, sizeof(laser->_cargaParaConsiderarCargada), 1, guardarEn);
		guardarEn->escribir(&laser->_segsAcumularParaCargar, sizeof(laser->_segsAcumularParaCargar), 1, guardarEn);
		guardarEn->escribir(&laser->_segsAcumularParaDescargar, sizeof(laser->_segsAcumularParaDescargar), 1, guardarEn);
		SI32 indiceObjEnCarreglo = -1;
		if(laser->_sensorComandosAlCargar != NULL){
			indiceObjEnCarreglo = arregloObjetosParaID->indiceDe(laser->_sensorComandosAlCargar);
			if(indiceObjEnCarreglo==-1) PRINTF_INFO("-------------------- ADVERTENCIA: panel-encender no pudo ser guardado\n");
		}
		guardarEn->escribir(&indiceObjEnCarreglo, sizeof(indiceObjEnCarreglo), 1, guardarEn);
		indiceObjEnCarreglo = -1;
		if(laser->_sensorComandosAlDescargar != NULL){
			indiceObjEnCarreglo = arregloObjetosParaID->indiceDe(laser->_sensorComandosAlDescargar);
			if(indiceObjEnCarreglo==-1) PRINTF_INFO("-------------------- ADVERTENCIA: panel-apagar no pudo ser guardado\n");
		}
		guardarEn->escribir(&indiceObjEnCarreglo, sizeof(indiceObjEnCarreglo), 1, guardarEn);
		exito = true;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

bool AUBateria::interpretarBitsInternos(AUBateria* cargarEn, AUArchivo* cargarDe, AUArreglo* arregloObjetosParaID){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUBateria::interpretarBitsInternos")
	bool exito = false;
	float versionDatos;
	cargarDe->leer(&versionDatos, sizeof(versionDatos), 1, cargarDe);
	if(versionDatos==1.0f){
		if(AUNodoElectroCarga::interpretarBitsInternos(cargarEn, cargarDe, arregloObjetosParaID)){
			bool activo;		cargarDe->leer(&activo, sizeof(activo), 1, cargarDe);			cargarEn->_bateriaActiva = activo;
			bool desactivar;	cargarDe->leer(&desactivar, sizeof(desactivar), 1, cargarDe);	cargarEn->_desactivarDespuesDeCargar = desactivar;
			bool desactivarD;	cargarDe->leer(&desactivarD, sizeof(desactivarD), 1, cargarDe);	cargarEn->_desactivarDespuesDeDescargar = desactivarD;
			cargarDe->leer(&cargarEn->_cargaParaConsiderarCargada, sizeof(cargarEn->_cargaParaConsiderarCargada), 1, cargarDe);
			cargarDe->leer(&cargarEn->_segsAcumularParaCargar, sizeof(cargarEn->_segsAcumularParaCargar), 1, cargarDe);
			cargarDe->leer(&cargarEn->_segsAcumularParaDescargar, sizeof(cargarEn->_segsAcumularParaDescargar), 1, cargarDe);
			//
			SI32 iObj;			cargarDe->leer(&iObj, sizeof(iObj), 1, cargarDe);
			AUEscenaSensor*		objDestino = NULL;
			if(iObj!=-1){
				AUObjeto* objEscena = (AUObjeto*)arregloObjetosParaID->elemento[iObj];
				if(objEscena->esClase(AUEscenaSensor::idTipoClase)){
					objDestino = (AUEscenaSensor*)objEscena;
				} else {
					PRINTF_INFO("-------------------- ADVERTENCIA: panel-encender no pudo ser cargado (no es clase 'AUEscenaSensor')\n");
				}
			}
			cargarEn->establecerSensorComandosAlCargar(objDestino);
			cargarDe->leer(&iObj, sizeof(iObj), 1, cargarDe);
			objDestino = NULL;
			if(iObj!=-1){
				AUObjeto* objEscena = (AUObjeto*)arregloObjetosParaID->elemento[iObj];
				if(objEscena->esClase(AUEscenaSensor::idTipoClase)){
					objDestino = (AUEscenaSensor*)objEscena;
				} else {
					PRINTF_INFO("-------------------- ADVERTENCIA: panel-apagar no pudo ser cargado (no es clase 'AUEscenaSensor')\n");
				}
			}
			cargarEn->establecerSensorComandosAlDescargar(objDestino);
			exito = true;
		}
	} else {
		PRINTF_ERROR("AUBateria, la version (%f) de datos binarios no es interpretable por este ejecutable\n", versionDatos);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

//
AUOBJMETODOS_CLASESID_MULTICLASE(AUBateria, AUNodoElectroCarga)
AUOBJMETODOS_CLASESNOMBRES_MULTICLASE(AUBateria, "AUBateria", AUNodoElectroCarga)
AUOBJMETODOS_CLONAR_THIS(AUBateria)






