//
//  AUPanelSolar.cpp
//  DodoTownNucleo
//
//  Created by Marcos Ortega on 22/09/12.
//
//

#include "AUFrameworkMediaStdAfx.h"
#include "AUPanelSolar.h"

AUPanelSolar::AUPanelSolar() : AUNodoElectroCarga() {
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUPanelSolar::AUPanelSolar")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUPanelSolar")
	_panelActivo				= true;
	_panelEncendido				= false;
	_desactivarDespuesDeEncender = false;
	_desactivarDespuesDeApagar	= false;
	_promedioLuzRGBParaEncender	= 0.75f;
	_segsAcumularParaEncender	= 1.0f;
	_segsAcumularParaApagar		= 1.0f;
	_sensorComandosEncender		= NULL;
	_sensorComandosApagar		= NULL;
	_escuchadorPanelSolar		= NULL;
	//
	_sensorLuz					= new(this) AUEscenaLuzSensor();
	this->agregarObjetoEscena(_sensorLuz); _sensorLuz->liberar(NB_RETENEDOR_THIS);
	_presentaPanel				= new(this) AUEscenaSprite(NBGestorTexturas::texturaDesdeArchivo("Escenarios/Electricos/objPanelSolar.png"));
	this->agregarObjetoEscena(_presentaPanel); _presentaPanel->liberar(NB_RETENEDOR_THIS);
	//
	AUTextura* texCarga			= NBGestorTexturas::texturaDesdeArchivo("Escenarios/Electricos/objPanelSolarLuz.png");
	float xPos					= -16.0f;
	float xAvance				= 16.0f;
	UI16 iCarga;
	for(iCarga=0; iCarga<AUPANELSOLAR_CANTIDAD_LUCES_INDICADORAS; iCarga++){
		_indicadoresCarga[iCarga] = new(this) AUEscenaSprite(texCarga);
		_indicadoresCarga[iCarga]->establecerIluminoDependencia(false);
		_indicadoresCarga[iCarga]->establecerMultiplicadorColor8((iCarga==0?0:255), (iCarga==0?0:255), (iCarga==0?255:0), 255);
		_indicadoresCarga[iCarga]->establecerTraslacion(xPos, 0.0f);
		this->agregarObjetoEscena(_indicadoresCarga[iCarga]); _indicadoresCarga[iCarga]->liberar(NB_RETENEDOR_THIS);
		xPos += xAvance;
	}
	//
	_presentaMarco				= new(this) AUEscenaSprite(NBGestorTexturas::texturaDesdeArchivo("Escenarios/Electricos/objPanelSolarMarco.png"));
	_presentaMarco->establecerIluminoDependencia(false);
	this->agregarObjetoEscena(_presentaMarco); _presentaMarco->liberar(NB_RETENEDOR_THIS);
	//
	_marcoPanelAumentando		= false;
	_cargaActual				= 0.0f;
	_cargaDestino				= 0.0f;
	_indicaEncendidosAumentado	= false;
	_indicaMaximoCargando		= 0;
	_indicaMaximoCargado		= 0;
	_relIndicaActual			= 1.0f;
	_relIndicaDestino			= 0.0f;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

AUPanelSolar::AUPanelSolar(AUPanelSolar* otraInstancia) : AUNodoElectroCarga(otraInstancia) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUPanelSolar::AUPanelSolar")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUPanelSolar")
	this->copiarPropiedadesDe(otraInstancia);
	_panelActivo				= otraInstancia->_panelActivo;
	_panelEncendido				= otraInstancia->_panelEncendido;
	_desactivarDespuesDeEncender = otraInstancia->_desactivarDespuesDeEncender;
	_desactivarDespuesDeApagar	= otraInstancia->_desactivarDespuesDeApagar;
	_promedioLuzRGBParaEncender	= otraInstancia->_promedioLuzRGBParaEncender;
	_segsAcumularParaEncender	= otraInstancia->_segsAcumularParaEncender;
	_segsAcumularParaApagar		= otraInstancia->_segsAcumularParaApagar;
	_sensorComandosEncender		= NULL;
	_sensorComandosApagar		= NULL;
	_escuchadorPanelSolar		= NULL;
	//
	_sensorLuz					= new(this) AUEscenaLuzSensor();
	this->agregarObjetoEscena(_sensorLuz); _sensorLuz->liberar(NB_RETENEDOR_THIS);
	_presentaPanel				= new(this) AUEscenaSprite(otraInstancia->_presentaPanel);
	this->agregarObjetoEscena(_presentaPanel); _presentaPanel->liberar(NB_RETENEDOR_THIS);
	//
	AUTextura* texCarga			= NBGestorTexturas::texturaDesdeArchivo("Escenarios/Electricos/objPanelSolarLuz.png");
	float xPos					= -16.0f;
	float xAvance				= 16.0f;
	UI16 iCarga;
	for(iCarga=0; iCarga<AUPANELSOLAR_CANTIDAD_LUCES_INDICADORAS; iCarga++){
		_indicadoresCarga[iCarga] = new(this) AUEscenaSprite(texCarga);
		_indicadoresCarga[iCarga]->establecerIluminoDependencia(false);
		_indicadoresCarga[iCarga]->establecerMultiplicadorColor8((iCarga==0?0:255), (iCarga==0?0:255), (iCarga==0?255:0), 255);
		_indicadoresCarga[iCarga]->establecerTraslacion(xPos, 0.0f);
		this->agregarObjetoEscena(_indicadoresCarga[iCarga]); _indicadoresCarga[iCarga]->liberar(NB_RETENEDOR_THIS);
		xPos += xAvance;
	}
	//
	_presentaMarco				= new(this) AUEscenaSprite(otraInstancia->_presentaMarco);
	_presentaMarco->establecerIluminoDependencia(false);
	this->agregarObjetoEscena(_presentaMarco); _presentaMarco->liberar(NB_RETENEDOR_THIS);
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

AUPanelSolar::~AUPanelSolar(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUPanelSolar::~AUPanelSolar")
	if(_sensorComandosApagar != NULL) _sensorComandosApagar->liberar(NB_RETENEDOR_THIS); _sensorComandosApagar = NULL;
	if(_sensorComandosEncender != NULL) _sensorComandosEncender->liberar(NB_RETENEDOR_THIS); _sensorComandosEncender = NULL;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

//

bool AUPanelSolar::panelActivo(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUPanelSolar::panelActivo")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _panelActivo;
}

bool AUPanelSolar::panelEncendido(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUPanelSolar::panelEncendido")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _panelEncendido;
	
}

bool AUPanelSolar::desactivarDespuesDeEncender(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUPanelSolar::desactivarDespuesDeEncender")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _desactivarDespuesDeEncender;
}

bool AUPanelSolar::desactivarDespuesDeApagar(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUPanelSolar::desactivarDespuesDeApagar")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _desactivarDespuesDeApagar;
}

float AUPanelSolar::promedioLuzRGBParaEncender(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUPanelSolar::segsAcumularParaEncender")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _promedioLuzRGBParaEncender;
}

float AUPanelSolar::segsAcumularParaEncender(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUPanelSolar::segsAcumularParaEncender")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _segsAcumularParaEncender;
}

float AUPanelSolar::segsAcumularParaApagar(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUPanelSolar::segsAcumularParaApagar")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return  _segsAcumularParaApagar;
}

AUEscenaSensor* AUPanelSolar::sensorComandosEncender(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUPanelSolar::sensorComandosEncender")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _sensorComandosEncender;
}

AUEscenaSensor* AUPanelSolar::sensorComandosApagar(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUPanelSolar::sensorComandosApagar")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _sensorComandosApagar;
}

IEscuchadorPanelSolar* AUPanelSolar::escuchadorPanelSolar(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUPanelSolar::escuchadorPanelSolar")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _escuchadorPanelSolar;
}

//

void AUPanelSolar::establecerPanelActivo(bool panelActivo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUPanelSolar::establecerPanelActivo")
	_panelActivo = panelActivo;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUPanelSolar::establecerDesactivarDespuesDeEncender(bool desactivarDespues){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUPanelSolar::establecerDesactivarDespuesDeEncender")
	_desactivarDespuesDeEncender = desactivarDespues;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUPanelSolar::establecerDesactivarDespuesDeApagar(bool desactivarDespues){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUPanelSolar::establecerDesactivarDespuesDeApagar")
	_desactivarDespuesDeApagar = desactivarDespues;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUPanelSolar::establecerPromedioRGBParaEncender(float promedioRGB){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUPanelSolar::establecerSegsAcumularParaEncender")
	_promedioLuzRGBParaEncender = promedioRGB;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUPanelSolar::establecerSegsAcumularParaEncender(float segsAcumParaEncender){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUPanelSolar::establecerSegsAcumularParaEncender")
	_segsAcumularParaEncender = segsAcumParaEncender;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUPanelSolar::establecerSegsAcumularParaApagar(float segsAcumParaApagar){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUPanelSolar::establecerSegsAcumularParaApagar")
	_segsAcumularParaApagar = segsAcumParaApagar;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUPanelSolar::establecerSensorComandosEncender(AUEscenaSensor* objSensor){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUPanelSolar::establecerSensorComandosEncender")
	if(objSensor != NULL) objSensor->retener(NB_RETENEDOR_THIS);
	if(_sensorComandosEncender != NULL) _sensorComandosEncender->liberar(NB_RETENEDOR_THIS);
	_sensorComandosEncender = objSensor;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUPanelSolar::establecerSensorComandosApagar(AUEscenaSensor* objSensor){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUPanelSolar::establecerSensorComandosApagar")
	if(objSensor != NULL) objSensor->retener(NB_RETENEDOR_THIS);
	if(_sensorComandosApagar != NULL) _sensorComandosApagar->liberar(NB_RETENEDOR_THIS);
	_sensorComandosApagar = objSensor;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUPanelSolar::establecerSensorComandosSegunPlantilla(AUPanelSolar* instanciaPlantilla, AUArreglo* hijosPlantilla, AUArreglo* hijosLocal){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUPanelSolar::establecerSensorComandosSegunPLantilla")
	SI32 iObjOriginal	= -1;
	if(instanciaPlantilla->_sensorComandosEncender != NULL) iObjOriginal = hijosPlantilla->indiceDe(instanciaPlantilla->_sensorComandosEncender);
	if(iObjOriginal!=-1){
		NBASSERT(iObjOriginal>=0 && iObjOriginal<hijosLocal->conteo)
		AUObjeto* objDestino = hijosLocal->elemento[iObjOriginal];
		if(objDestino->esClase(AUEscenaSensor::idTipoClase)){
			this->establecerSensorComandosEncender((AUEscenaSensor*)objDestino);
		}
	}
	iObjOriginal		= -1;
	if(instanciaPlantilla->_sensorComandosApagar != NULL) iObjOriginal = hijosPlantilla->indiceDe(instanciaPlantilla->_sensorComandosApagar);
	if(iObjOriginal!=-1){
		NBASSERT(iObjOriginal>=0 && iObjOriginal<hijosLocal->conteo)
		AUObjeto* objDestino = hijosLocal->elemento[iObjOriginal];
		if(objDestino->esClase(AUEscenaSensor::idTipoClase)){
			this->establecerSensorComandosApagar((AUEscenaSensor*)objDestino);
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUPanelSolar::establecerEscuchadorPanelSolar(IEscuchadorPanelSolar* escuchador){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUPanelSolar::establecerEscuchadorPanelSolar")
	_escuchadorPanelSolar = escuchador;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

//

NBCajaAABB AUPanelSolar::cajaEscenaPresentacion(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUPanelSolar::cajaEscenaPresentacion")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _presentaPanel->cajaAABBEnEscena();
}

float AUPanelSolar::electrocargaVirtualParaDestinos(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUPanelSolar::electrocargaVirtualParaDestinos")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return (_panelEncendido?_promedioLuzRGBParaEncender:0.0f);
}

float AUPanelSolar::electrocargaRealParaDestinos(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUPanelSolar::electrocargaRealParaDestinos")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _cargaActual;
}

bool AUPanelSolar::electrocargaEsSoloConsumidor(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUPanelSolar::electrocargaEsSoloConsumidor")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return false;
}

void AUPanelSolar::tickAnimacion(float segsTranscurridos){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUPanelSolar::tickAnimacion")
	AUNodoElectroCarga::tickAnimacion(segsTranscurridos);
	if(this->_dormido){ AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO_3() }
	//Animar marco de panel
	SI16 coloR			= _presentaMarco->_propiedades.color8.r;
	if(_marcoPanelAumentando){
		coloR			+= 127 * segsTranscurridos;
		if(coloR>255){ coloR = 255; _marcoPanelAumentando = !_marcoPanelAumentando;}
	} else {
		coloR			-= 127 * segsTranscurridos;
		if(coloR<55){ coloR = 55; _marcoPanelAumentando = !_marcoPanelAumentando;}
	}
	_presentaMarco->_propiedades.color8.r	= coloR;
	_presentaMarco->_propiedades.color8.b	= coloR;
	//Animar luces y carga
	if(_panelActivo){
		bool indicadorActualModificado		= false;
		bool indicadorFinalModificado		= false;
		bool cambioEstadoPanel				= false;
		//Animar carga de panel (encender y apagar)
		if(_cargaActual!=_cargaDestino){
			if(_cargaActual<_cargaDestino){
				_cargaActual += segsTranscurridos / _segsAcumularParaEncender;
				if(_cargaActual>_cargaDestino) _cargaActual = _cargaDestino;
				if(!_panelEncendido && (_cargaActual/_promedioLuzRGBParaEncender)>=1.0f){
					_panelEncendido			= true;
					if(_escuchadorPanelSolar != NULL) _escuchadorPanelSolar->panelSolarEncendido(this, _sensorComandosEncender);
					cambioEstadoPanel		= true;
					PRINTF_INFO("Panel solar encendido\n");
				}
			} else {
				_cargaActual -= segsTranscurridos / _segsAcumularParaApagar;
				if(_cargaActual<_cargaDestino) _cargaActual = _cargaDestino;
				if(_panelEncendido && _cargaActual==0.0f){
					_panelEncendido			= false;
					cambioEstadoPanel		= true;
					if(_escuchadorPanelSolar != NULL) _escuchadorPanelSolar->panelSolarApagado(this, _sensorComandosApagar);
					PRINTF_INFO("Panel solar apagado\n");
				}
				
			}
			//Establecer colores de indicadores
			UI16 nuevoIndicaMaximo		= AUPANELSOLAR_CANTIDAD_LUCES_INDICADORAS * _cargaActual / _promedioLuzRGBParaEncender; if(nuevoIndicaMaximo>=AUPANELSOLAR_CANTIDAD_LUCES_INDICADORAS) nuevoIndicaMaximo = AUPANELSOLAR_CANTIDAD_LUCES_INDICADORAS - 1;
			if(!_panelEncendido && nuevoIndicaMaximo==AUPANELSOLAR_CANTIDAD_LUCES_INDICADORAS - 1) nuevoIndicaMaximo--;
			NBASSERT(nuevoIndicaMaximo<AUPANELSOLAR_CANTIDAD_LUCES_INDICADORAS)
			if(nuevoIndicaMaximo!=_indicaMaximoCargado){
				_indicaMaximoCargado	= nuevoIndicaMaximo;
				indicadorActualModificado = true;
			}
		}
		//Calcular iluminacion recibida al panel
		_cargaDestino					= _sensorLuz->ultimoPromedioRGBPorLuz(); //PRINTF_INFO("PanelSolar: luz recibida prom(%f)\n", promedioRGBLuz);
		UI16 nuevoIndicaMaximoCargando	= AUPANELSOLAR_CANTIDAD_LUCES_INDICADORAS * _cargaDestino / _promedioLuzRGBParaEncender; if(nuevoIndicaMaximoCargando>=AUPANELSOLAR_CANTIDAD_LUCES_INDICADORAS) nuevoIndicaMaximoCargando = AUPANELSOLAR_CANTIDAD_LUCES_INDICADORAS - 1;
		NBASSERT(nuevoIndicaMaximoCargando<AUPANELSOLAR_CANTIDAD_LUCES_INDICADORAS)
		if(nuevoIndicaMaximoCargando!=_indicaMaximoCargando){
			_indicaEncendidosAumentado	= (_indicaMaximoCargando<nuevoIndicaMaximoCargando);
			_indicaMaximoCargando		= nuevoIndicaMaximoCargando;
			indicadorFinalModificado	= true;
		}
		if(cambioEstadoPanel || indicadorActualModificado || indicadorFinalModificado){
			//Pintar colores a botones
			UI16 iInd;
			if(_cargaActual==0.0f){
				for(iInd=0; iInd<AUPANELSOLAR_CANTIDAD_LUCES_INDICADORAS; iInd++){
					_indicadoresCarga[iInd]->establecerMultiplicadorColor8(255, 0, 0, 255);
				}
			} else if(_indicaMaximoCargado==(AUPANELSOLAR_CANTIDAD_LUCES_INDICADORAS-1)){
				for(iInd=0; iInd<AUPANELSOLAR_CANTIDAD_LUCES_INDICADORAS; iInd++){
					_indicadoresCarga[iInd]->establecerMultiplicadorColor8(0, 255, 0, 255);
				}
			} else if(_indicaEncendidosAumentado){
				for(iInd=0; iInd<AUPANELSOLAR_CANTIDAD_LUCES_INDICADORAS; iInd++){
					if(iInd<=_indicaMaximoCargado){
						_indicadoresCarga[iInd]->establecerMultiplicadorColor8(0, 255, 0, 255);
					} else {
						_indicadoresCarga[iInd]->establecerMultiplicadorColor8(255, 0, 0, 255);
					}
				}
			} else {
				for(iInd=0; iInd<AUPANELSOLAR_CANTIDAD_LUCES_INDICADORAS; iInd++){
					if(iInd<=_indicaMaximoCargado){
						_indicadoresCarga[iInd]->establecerMultiplicadorColor8(255, 255, 0, 255);
					} else {
						_indicadoresCarga[iInd]->establecerMultiplicadorColor8(255, 0, 0, 255);
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
				} else if(_indicaMaximoCargado==(AUPANELSOLAR_CANTIDAD_LUCES_INDICADORAS-1)){
					_indicadoresCarga[_indicaMaximoCargado]->establecerMultiplicadorColor8(0, 255 * _relIndicaActual, 0, 255);
				}
			} else {
				_relIndicaDestino		= (_relIndicaActual==0.0f?1.0f:0.0f);
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

bool AUPanelSolar::agregarXmlInternoEn(AUPanelSolar* laser, AUCadenaLargaMutable8* guardarEn, const char* espaciosBaseIzq, AUArreglo* arregloObjetosParaID){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUPanelSolar::agregarXmlInternoEn")
	//guardarEn->agregarConFormato("%s<vXML>1</vXML>\r\n", espaciosBaseIzq);
	AUNodoElectroCarga::agregarXmlInternoEn(laser, guardarEn, espaciosBaseIzq, arregloObjetosParaID);
	guardarEn->agregarConFormato("%s<act>%d</act>\r\n", espaciosBaseIzq, laser->_panelActivo?1:0);
	guardarEn->agregarConFormato("%s<desAlEnc>%d</desAlEnc>\r\n", espaciosBaseIzq, laser->_desactivarDespuesDeEncender?1:0);
	guardarEn->agregarConFormato("%s<desAlApa>%d</desAlApa>\r\n", espaciosBaseIzq, laser->_desactivarDespuesDeApagar?1:0);
	guardarEn->agregarConFormato("%s<promRGBMin>%f</promRGBMin>\r\n", espaciosBaseIzq, laser->_promedioLuzRGBParaEncender);
	guardarEn->agregarConFormato("%s<segsEnc>%f</segsEnc>\r\n", espaciosBaseIzq, laser->_segsAcumularParaEncender);
	guardarEn->agregarConFormato("%s<segsApa>%f</segsApa>\r\n", espaciosBaseIzq, laser->_segsAcumularParaApagar);
	SI32 indiceObjEnArreglo = -1;
	if(laser->_sensorComandosEncender != NULL){
		indiceObjEnArreglo = arregloObjetosParaID->indiceDe(laser->_sensorComandosEncender);
		if(indiceObjEnArreglo==-1) PRINTF_INFO("-------------------- ADVERTENCIA: destino de panel-encender no pudo ser guardado\n");
	}
	guardarEn->agregarConFormato("%s<iSen>%d</iSen>\r\n", espaciosBaseIzq, indiceObjEnArreglo);
	indiceObjEnArreglo = -1;
	if(laser->_sensorComandosApagar != NULL){
		indiceObjEnArreglo = arregloObjetosParaID->indiceDe(laser->_sensorComandosApagar);
		if(indiceObjEnArreglo==-1) PRINTF_INFO("-------------------- ADVERTENCIA: destino de panel-apagar no pudo ser guardado\n");
	}
	guardarEn->agregarConFormato("%s<iSenD>%d</iSenD>\r\n", espaciosBaseIzq, indiceObjEnArreglo);
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return true;
}

bool AUPanelSolar::interpretarXmlInterno(AUPanelSolar* cargarEn, const AUDatosXML* datosXml, const sNodoXML* nodoXml, AUArregloNativoP<AUEscenaObjeto*>* arregloObjetos, AUArregloNativoP<SI32>* arregloIDs){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUPanelSolar::interpretarXmlInterno")
	bool exito = false;
	//int versionXML = datosXml->nodoHijo("vXML", 0, nodoXml);
	//if(versionXML==1){
	if(AUNodoElectroCarga::interpretarXmlInterno(cargarEn, datosXml, nodoXml, arregloObjetos, arregloIDs)){
		cargarEn->_panelActivo		= datosXml->nodoHijo("act", true, nodoXml);
		cargarEn->_desactivarDespuesDeEncender	= datosXml->nodoHijo("desAlEnc", false, nodoXml);
		cargarEn->_desactivarDespuesDeApagar	= datosXml->nodoHijo("desAlApa", false, nodoXml);
		cargarEn->_promedioLuzRGBParaEncender	= datosXml->nodoHijo("promRGBMin", 0.75f, nodoXml);
		cargarEn->_segsAcumularParaEncender		= datosXml->nodoHijo("segsEnc", 1.0f, nodoXml);
		cargarEn->_segsAcumularParaApagar		= datosXml->nodoHijo("segsApa", 1.0f, nodoXml);
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
		cargarEn->establecerSensorComandosEncender(objetoDestino);
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
		cargarEn->establecerSensorComandosApagar(objetoDestino);
		exito = true;
	}
	//}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

bool AUPanelSolar::agregarBitsInternosEn(AUPanelSolar* laser, AUArchivo* guardarEn, AUArreglo* arregloObjetosParaID){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUPanelSolar::agregarBitsInternosEn")
	bool exito = false;
	//
	float versionDatos = 1.0f;
	guardarEn->escribir(&versionDatos, sizeof(versionDatos), 1, guardarEn);
	if(AUNodoElectroCarga::agregarBitsInternosEn(laser, guardarEn, arregloObjetosParaID)){
		bool activo		= laser->_panelActivo;					guardarEn->escribir(&activo, sizeof(activo), 1, guardarEn);
		bool desactivar = laser->_desactivarDespuesDeEncender;	guardarEn->escribir(&desactivar, sizeof(desactivar), 1, guardarEn);
		bool desactivD	= laser->_desactivarDespuesDeApagar;	guardarEn->escribir(&desactivD, sizeof(desactivD), 1, guardarEn);
		guardarEn->escribir(&laser->_promedioLuzRGBParaEncender, sizeof(laser->_promedioLuzRGBParaEncender), 1, guardarEn);
		guardarEn->escribir(&laser->_segsAcumularParaEncender, sizeof(laser->_segsAcumularParaEncender), 1, guardarEn);
		guardarEn->escribir(&laser->_segsAcumularParaApagar, sizeof(laser->_segsAcumularParaApagar), 1, guardarEn);
		SI32 indiceObjEnCarreglo = -1;
		if(laser->_sensorComandosEncender != NULL){
			indiceObjEnCarreglo = arregloObjetosParaID->indiceDe(laser->_sensorComandosEncender);
			if(indiceObjEnCarreglo==-1) PRINTF_INFO("-------------------- ADVERTENCIA: panel-encender no pudo ser guardado\n");
		}
		guardarEn->escribir(&indiceObjEnCarreglo, sizeof(indiceObjEnCarreglo), 1, guardarEn);
		indiceObjEnCarreglo = -1;
		if(laser->_sensorComandosApagar != NULL){
			indiceObjEnCarreglo = arregloObjetosParaID->indiceDe(laser->_sensorComandosApagar);
			if(indiceObjEnCarreglo==-1) PRINTF_INFO("-------------------- ADVERTENCIA: panel-apagar no pudo ser guardado\n");
		}
		guardarEn->escribir(&indiceObjEnCarreglo, sizeof(indiceObjEnCarreglo), 1, guardarEn);
		exito = true;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

bool AUPanelSolar::interpretarBitsInternos(AUPanelSolar* cargarEn, AUArchivo* cargarDe, AUArreglo* arregloObjetosParaID){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUPanelSolar::interpretarBitsInternos")
	bool exito = false;
	float versionDatos;
	cargarDe->leer(&versionDatos, sizeof(versionDatos), 1, cargarDe);
	if(versionDatos==1.0f){
		if(AUNodoElectroCarga::interpretarBitsInternos(cargarEn, cargarDe, arregloObjetosParaID)){
			bool activo;		cargarDe->leer(&activo, sizeof(activo), 1, cargarDe);			cargarEn->_panelActivo = activo;
			bool desactivar;	cargarDe->leer(&desactivar, sizeof(desactivar), 1, cargarDe);	cargarEn->_desactivarDespuesDeEncender = desactivar;
			bool desactivarD;	cargarDe->leer(&desactivarD, sizeof(desactivarD), 1, cargarDe);	cargarEn->_desactivarDespuesDeApagar = desactivarD;
			cargarDe->leer(&cargarEn->_promedioLuzRGBParaEncender, sizeof(cargarEn->_promedioLuzRGBParaEncender), 1, cargarDe);
			cargarDe->leer(&cargarEn->_segsAcumularParaEncender, sizeof(cargarEn->_segsAcumularParaEncender), 1, cargarDe);
			cargarDe->leer(&cargarEn->_segsAcumularParaApagar, sizeof(cargarEn->_segsAcumularParaApagar), 1, cargarDe);
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
			cargarEn->establecerSensorComandosEncender(objDestino);
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
			cargarEn->establecerSensorComandosApagar(objDestino);
			exito = true;
		}
	} else {
		PRINTF_ERROR("AUPanelSolar, la version (%f) de datos binarios no es interpretable por este ejecutable\n", versionDatos);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

//
AUOBJMETODOS_CLASESID_MULTICLASE(AUPanelSolar, AUNodoElectroCarga)
AUOBJMETODOS_CLASESNOMBRES_MULTICLASE(AUPanelSolar, "AUPanelSolar", AUNodoElectroCarga)
AUOBJMETODOS_CLONAR_THIS(AUPanelSolar)




