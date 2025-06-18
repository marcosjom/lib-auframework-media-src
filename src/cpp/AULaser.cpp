//
//  AULaser.cpp
//  DodoTownNucleo
//
//  Created by Marcos Ortega on 22/09/12.
//
//

#include "AUFrameworkMediaStdAfx.h"
#include "AULaser.h"

AULaser::AULaser() : AUNodoElectroCarga() {
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AULaser::AULaser")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AULaser")
	_presentacionLaser			= NULL;
	_sensorComandosColision		= NULL;
	_sensorComandosDescolision	= NULL;
	_escuchadorLaser			= NULL;
	_laserActivo				= true;
	_desactivarDespuesDeContacto = false;
	_detectarColisionConCuerdas	= false;
	_cargaActualRecibida		= 0.0f;
	_cargaParaConsiderarCargada	= 0.0f;
	_factorCortaCuerdas			= 0.0f;
	//
	_luzFuego					= NULL;
	_particulasFuego			= NULL;
	//
	_fraccionMinimaColisionLaser = 1.0f;
	_segundaFraccionMinimaColisionLaser = 1.0f;
	_cuerpoColisionMasCercana	= NULL;
	_segundoCuerpoColisionMasCercana = NULL;
	_ultimoTickConColision		= false;
	_segundosEsperarParaDispararSiguienteEvento = 0.0f;
	_segundosEsperaDespuesDeObstruccion = 0.0f; //0.0f es automatico (se calcula con base a la espera y suavizado de loscomandos)
	_segundosEsperaDespuesDeLiberacion	= 0.0f; //0.0f es automatico (se calcula con base a la espera y suavizado de loscomandos)
	_presentacionLaser			= new(this) AUEscenaLaser(64.0f, 8.0f);
	_presentacionLaser->establecerLaserEscuchador(this);
	_presentacionLaser->establecerLaserVelDesplazaTex(128.0f, 0.0f);
	this->agregarObjetoEscena(_presentacionLaser);
	this->establecerMultiplicadorColor8(255, 0, 0, 255);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

AULaser::AULaser(AULaser* otraInstancia) : AUNodoElectroCarga(otraInstancia) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AULaser::AULaser")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AULaser")
	this->copiarPropiedadesDe(otraInstancia);
	_presentacionLaser			= NULL;
	_sensorComandosColision		= NULL;
	_sensorComandosDescolision	= NULL;
	_escuchadorLaser			= NULL;
	_cargaActualRecibida		= 0.0f;
	_laserActivo				= otraInstancia->_laserActivo;
	_desactivarDespuesDeContacto = otraInstancia->_desactivarDespuesDeContacto;
	_cargaParaConsiderarCargada	= otraInstancia->_cargaParaConsiderarCargada;
	_factorCortaCuerdas			= otraInstancia->_factorCortaCuerdas;
	_detectarColisionConCuerdas	= otraInstancia->_detectarColisionConCuerdas;
	//
	_luzFuego					= NULL;
	_particulasFuego			= NULL;
	//
	_segundosEsperaDespuesDeObstruccion = otraInstancia->_segundosEsperaDespuesDeObstruccion; //0.0f es automatico (se calcula con base a la espera y suavizado de loscomandos)
	_segundosEsperaDespuesDeLiberacion	= otraInstancia->_segundosEsperaDespuesDeLiberacion; //0.0f es automatico (se calcula con base a la espera y suavizado de loscomandos)
	_fraccionMinimaColisionLaser = 1.0f;
	_segundaFraccionMinimaColisionLaser = 1.0f;
	_cuerpoColisionMasCercana	= NULL;
	_segundoCuerpoColisionMasCercana = NULL;
	_ultimoTickConColision		= false;
	_segundosEsperarParaDispararSiguienteEvento = 0.0f;
	_presentacionLaser			= new(this) AUEscenaLaser(otraInstancia->_presentacionLaser);
	_presentacionLaser->establecerLaserEscuchador(this);
	this->agregarObjetoEscena(_presentacionLaser);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

AULaser::~AULaser(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AULaser::~AULaser")
	if(_presentacionLaser != NULL) _presentacionLaser->liberar(NB_RETENEDOR_THIS); _presentacionLaser = NULL;
	if(_sensorComandosColision != NULL) _sensorComandosColision->liberar(NB_RETENEDOR_THIS); _sensorComandosColision = NULL;
	if(_sensorComandosDescolision != NULL) _sensorComandosDescolision->liberar(NB_RETENEDOR_THIS); _sensorComandosDescolision = NULL;
	if(_luzFuego != NULL) _luzFuego->liberar(NB_RETENEDOR_THIS); _luzFuego = NULL;
	if(_particulasFuego != NULL) _particulasFuego->liberar(NB_RETENEDOR_THIS); _particulasFuego = NULL;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

//

bool AULaser::laserActivo(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AULaser::laserActivo")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _laserActivo;
}

bool AULaser::desactivarLaserAlContacto(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AULaser::desactivarLaserAlContacto")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _desactivarDespuesDeContacto;
}

bool AULaser::ignorarPrimeraColision(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AULaser::ignorarPrimeraColision")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _presentacionLaser->laserColisionIgnorarPrimera();
}

bool AULaser::detectarColisionConCuerdas(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AULaser::detectarColisionConCuerdas")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _detectarColisionConCuerdas;
}

float AULaser::cargaParaConsiderarCargada(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AULaser::cargaParaConsiderarCargada")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _cargaParaConsiderarCargada;
}

void AULaser::establecerCargaParaConsiderarCargada(float cargaRel){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AULaser::establecerCargaParaConsiderarCargada")
	_cargaParaConsiderarCargada = cargaRel;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

float AULaser::alcanceLaser(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AULaser::alcanceLaser")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _presentacionLaser->laserAlcance();
}

float AULaser::anchoLaser(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AULaser::anchoLaser")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _presentacionLaser->laserAncho();
}

float AULaser::factorCortaCuerdas(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AULaser::factorCortaCuerdas")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _factorCortaCuerdas;
}

NBPunto AULaser::velocidadDesplazamientoTextura(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AULaser::velocidadDesplazamientoTextura")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _presentacionLaser->laserVelDesplazaTex();
}

AUEscenaSensor* AULaser::sensorComandosColision(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AULaser::sensorComandosColision")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _sensorComandosColision;
}

AUEscenaSensor* AULaser::sensorComandosDescolision(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AULaser::sensorComandosDescolision")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _sensorComandosDescolision;
}

IEscuchadorLaser* AULaser::escuchadorLaser(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AULaser::sensorComandosColision")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _escuchadorLaser;
}

float AULaser::segundosEsperaDespuesDeObstruccion(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AULaser::segundosEsperaDespuesDeObstruccion")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _segundosEsperaDespuesDeObstruccion;
}

float AULaser::segundosEsperaDespuesDeLiberacion(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AULaser::segundosEsperaDespuesDeLiberacion")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _segundosEsperaDespuesDeLiberacion;
}

//

void AULaser::establecerLaserActivo(bool activo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AULaser::establecerLaserActivo")
	_laserActivo = activo;
	if(_laserActivo){
		_presentacionLaser->establecerVisible(true);
	} else {
		_presentacionLaser->establecerVisible(false);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AULaser::establecerDesactivarLaseAlContacto(bool desactivar){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AULaser::establecerDesactivarLaseAlContacto")
	_desactivarDespuesDeContacto = desactivar;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AULaser::establecerIgnorarPrimeraColision(bool ignorarPrimeraColision){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AULaser::establecerIgnorarPrimeraColision")
	_presentacionLaser->establecerLaserColisionIgnorarPrimera(ignorarPrimeraColision);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AULaser::establecerAlcanceLaser(float alcanceLaser){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AULaser::establecerAlcanceLaser")
	_presentacionLaser->establecerLaserAlcance(alcanceLaser);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AULaser::establecerAnchoLaser(float anchoLaser){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AULaser::establecerAnchoLaser")
	_presentacionLaser->establecerLaserAncho(anchoLaser);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AULaser::establecerFactorCortaCuerdas(float factorCortaCuerdas){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AULaser::establecerFactorCortaCuerdas")
	NBASSERT(factorCortaCuerdas>=0.0f)
	_factorCortaCuerdas = factorCortaCuerdas;
	_presentacionLaser->establecerLaserColisionaConCuerdas(factorCortaCuerdas!=0.0f || _detectarColisionConCuerdas);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AULaser::establecerDetectarColisionConCuerdas(bool detectarColisionConCuerdas){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AULaser::establecerDetectarColisionConCuerdas")
	_detectarColisionConCuerdas = detectarColisionConCuerdas;
	_presentacionLaser->establecerLaserColisionaConCuerdas(_factorCortaCuerdas!=0.0f || detectarColisionConCuerdas);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AULaser::establecerVelocidadDesplazamientoTextura(const NBPunto &velDespTex){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AULaser::establecerVelocidadDesplazamientoTextura")
	_presentacionLaser->establecerLaserVelDesplazaTex(velDespTex);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AULaser::establecerSensorComandosColision(AUEscenaSensor* sensorComandos){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AULaser::establecerVelocidadDesplazamientoTextura")
	if(_sensorComandosColision != NULL) _sensorComandosColision->liberar(NB_RETENEDOR_THIS);
	if(sensorComandos != NULL) sensorComandos->retener(NB_RETENEDOR_THIS);
	_sensorComandosColision = sensorComandos;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AULaser::establecerSensorComandosDescolision(AUEscenaSensor* sensorComandos){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AULaser::establecerSensorComandosDescolision")
	if(_sensorComandosDescolision != NULL) _sensorComandosDescolision->liberar(NB_RETENEDOR_THIS);
	if(sensorComandos != NULL) sensorComandos->retener(NB_RETENEDOR_THIS);
	_sensorComandosDescolision = sensorComandos;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AULaser::establecerSensorComandosSegunPlantilla(AULaser* instanciaPlantilla, AUArreglo* hijosPlantilla, AUArreglo* hijosLocal){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AULaser::establecerSensorComandosSegunPLantilla")
	SI32 iObjOriginal	= -1;
	if(instanciaPlantilla->_sensorComandosColision != NULL) iObjOriginal = hijosPlantilla->indiceDe(instanciaPlantilla->_sensorComandosColision);
	if(iObjOriginal!=-1){
		NBASSERT(iObjOriginal>=0 && iObjOriginal<hijosLocal->conteo)
		AUObjeto* objDestino = hijosLocal->elemento[iObjOriginal];
		if(objDestino->esClase(AUEscenaSensor::idTipoClase)){
			this->establecerSensorComandosColision((AUEscenaSensor*)objDestino);
		}
	}
	iObjOriginal		= -1;
	if(instanciaPlantilla->_sensorComandosDescolision != NULL) iObjOriginal = hijosPlantilla->indiceDe(instanciaPlantilla->_sensorComandosDescolision);
	if(iObjOriginal!=-1){
		NBASSERT(iObjOriginal>=0 && iObjOriginal<hijosLocal->conteo)
		AUObjeto* objDestino = hijosLocal->elemento[iObjOriginal];
		if(objDestino->esClase(AUEscenaSensor::idTipoClase)){
			this->establecerSensorComandosDescolision((AUEscenaSensor*)objDestino);
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AULaser::establecerEscuchadorLaser(IEscuchadorLaser* escuchador){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AULaser::establecerVelocidadDesplazamientoTextura")
	_escuchadorLaser = escuchador;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AULaser::establecerSegundosEsperaDespuesDeObstruccion(float segs){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AULaser::establecerSegundosEsperaDespuesDeObstruccion")
	_segundosEsperaDespuesDeObstruccion = segs;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AULaser::establecerSegundosEsperaDespuesDeLiberacion(float segs){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AULaser::establecerSegundosEsperaDespuesDeLiberacion")
	_segundosEsperaDespuesDeLiberacion = segs;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

//

NBCajaAABB AULaser::cajaEscenaPresentacion(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AULaser::cajaEscenaPresentacion")
	NBCajaAABB cajaEscena;
	NBPunto posEscena[2];
	NBMATRIZ_MULTIPLICAR_PUNTO(posEscena[0], _cacheObjEscena.matrizEscena, -16.0f, -16.0f)
	NBMATRIZ_MULTIPLICAR_PUNTO(posEscena[1], _cacheObjEscena.matrizEscena, 16.0f, 16.0f)
	NBCAJAAABB_INICIALIZAR(cajaEscena)
	NBCAJAAABB_ENVOLVER_PUNTOS(cajaEscena, posEscena, 2)
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return cajaEscena;
}

float AULaser::electrocargaVirtualParaDestinos(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AULaser::electrocargaVirtualParaDestinos")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return this->electrocargaVirtualTotalRecibida();
}

float AULaser::electrocargaRealParaDestinos(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AULaser::electrocargaRealParaDestinos")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return this->electrocargaRealTotalRecibida();
}

bool AULaser::electrocargaEsSoloConsumidor(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AULaser::electrocargaEsSoloConsumidor")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return false;
}

void AULaser::agregadoEnEscena(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AULaser::agregadoEnEscena")
	AUNodoElectroCarga::agregadoEnEscena();
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AULaser::quitandoDeEscena(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AULaser::quitandoDeEscena")
	AUNodoElectroCarga::quitandoDeEscena();
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AULaser::laserEscenaTickObstruccionPorFisica(AUEscenaLaser* objEscenaLaser, const float segsTranscurridos, /*b2Body*/ void* pObjCuerpo, const NBPunto &posEscena, const NBPunto &vecNormal){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AULaser::laserEscenaTickObstruccionPorFisica")
	NBASSERT((_luzFuego == NULL && _particulasFuego == NULL) || (_luzFuego != NULL && _particulasFuego != NULL))
//#	ifndef CONFIG_NB_UNSUPPORT_BOX2D
//	b2Body* objCuerpo = (b2Body*)pObjCuerpo;
//#	endif
	if(_luzFuego == NULL){
		_luzFuego				= new(this) AUEscenaLuzRadial();
		_luzFuego->establecerTipoLuz(ENFuenteIlumTipo_SoloMascara);
		_luzFuego->establecerProducirSombras(false);
		_luzFuego->establecerAlcanceLuz(0.0f);
		this->agregarObjetoEscena(_luzFuego);
		_particulasFuego		= new(this) AUEscenaGrupoParticulasFuego();
		_particulasFuego->establecerParticulasSegsVida(0.33f);
		_particulasFuego->establecerAlcanceParticulas(0.0f);
		_particulasFuego->establecerParticulasPorSegundo(0.0f);
		_particulasFuego->establecerRotacion(180.0f);
		this->agregarObjetoEscena(_particulasFuego);
	}
	NBMatriz matrizInversa;	NBMATRIZ_INVERSA(matrizInversa, _cacheObjEscena.matrizEscena)
	NBPunto posLocal;		NBMATRIZ_MULTIPLICAR_PUNTO(posLocal, matrizInversa, posEscena.x, posEscena.y)
	_particulasFuego->establecerTraslacion(posLocal);
	_particulasFuego->establecerAlcanceParticulas(_factorCortaCuerdas * 4.0f);
	_particulasFuego->establecerParticulasPorSegundo(_factorCortaCuerdas * 15.0f);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AULaser::laserEscenaTickObstruccionPorCuerda(AUEscenaLaser* objEscenaLaser, const float segsTranscurridos, ICuerdaCortable* objCuerda, const NBPunto &posEscena, const STCuerdaCorte &propsColision){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AULaser::laserEscenaTickObstruccionPorCuerda")
	if(_factorCortaCuerdas!=0.0f){
		NBASSERT((_luzFuego == NULL && _particulasFuego == NULL) || (_luzFuego != NULL && _particulasFuego != NULL))
		if(_luzFuego == NULL){
			_luzFuego				= new(this) AUEscenaLuzRadial();
			_luzFuego->establecerTipoLuz(ENFuenteIlumTipo_SoloMascara);
			_luzFuego->establecerProducirSombras(false);
			_luzFuego->establecerAlcanceLuz(0.0f);
			this->agregarObjetoEscena(_luzFuego);
			_particulasFuego		= new(this) AUEscenaGrupoParticulasFuego();
			_particulasFuego->establecerParticulasSegsVida(0.33f);
			_particulasFuego->establecerRotacion(180.0f);
			this->agregarObjetoEscena(_particulasFuego);
		}
		NBMatriz matrizInversa;	NBMATRIZ_INVERSA(matrizInversa, _cacheObjEscena.matrizEscena)
		NBPunto posLocal;		NBMATRIZ_MULTIPLICAR_PUNTO(posLocal, matrizInversa, posEscena.x, posEscena.y)
		_particulasFuego->establecerTraslacion(posLocal);
		_particulasFuego->establecerAlcanceParticulas(_factorCortaCuerdas * 4.0f);
		_particulasFuego->establecerParticulasPorSegundo(_factorCortaCuerdas * 15.0f);
		objCuerda->cuerdaConsumirCorteDeRayo(propsColision.cuerdaISegmentoCorte, _factorCortaCuerdas, segsTranscurridos);
	} else {
		if(_particulasFuego != NULL){
			_particulasFuego->establecerAlcanceParticulas(0.0f);
			_particulasFuego->establecerParticulasPorSegundo(0.0f);
		}
		if(_luzFuego != NULL){
			_luzFuego->establecerAlcanceLuz(0.0f);
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AULaser::tickAnimacion(float segsTranscurridos){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AULaser::tickAnimacion")
	AUNodoElectroCarga::tickAnimacion(segsTranscurridos);
	if(this->_dormido){ AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO_3() }
	if(_laserActivo){
		_cargaActualRecibida		= this->electrocargaVirtualTotalRecibida();
		float relCarga				= 1.0f; if(_cargaParaConsiderarCargada!=0.0f) relCarga = _cargaActualRecibida / _cargaParaConsiderarCargada; if(relCarga>1.0f) relCarga = 1.0f;
		if(relCarga==1.0f){
			/*
			 AUEscenaSensor*		_sensorComandosColision;
			 AUEscenaSensor*		_sensorComandosDescolision;
			 IEscuchadorLaser*		_escuchadorLaser;
			 float					_segundosEsperaDespuesDeObstruccion;
			 float					_segundosEsperaDespuesDeLiberacion;
			 //Fisica
			 float					_segundosEsperarParaDispararSiguienteEvento; //Evita que el laser ejecute comando tras comando con el roce de cada cuerpo
			 float					_fraccionMinimaColisionLaser;
			 float					_segundaFraccionMinimaColisionLaser;
			 b2Body*				_cuerpoColisionMasCercana;
			 b2Body*				_segundoCuerpoColisionMasCercana;
			 */
		}
		_presentacionLaser->establecerVisible(relCarga==1.0f);
	}
	//Animar luz de corte
	if(_particulasFuego != NULL){
		NBASSERT(_luzFuego != NULL)
		if(_particulasFuego->conteoParticulasFuegoVivas()!=0){
			NBCajaAABB cajaEscenaFuego		= _particulasFuego->cajaAABBEnEscena();
			//NBPunto centroEscenaFuego;	NBCAJAAABB_CENTRO(centroEscenaFuego, cajaEscenaFuego)
			NBTamano tamEscenaFuego;		NBCAJAAABB_TAMANO(tamEscenaFuego, cajaEscenaFuego)
			_luzFuego->establecerAlcanceLuz((tamEscenaFuego.ancho>tamEscenaFuego.alto?tamEscenaFuego.ancho:tamEscenaFuego.alto) * 1.33f);
			//NBMatriz matrizInv;			NBMATRIZ_INVERSA(matrizInv, _cacheObjEscena.matrizEscena)
			//NBPunto centroLocalFuego;		NBMATRIZ_MULTIPLICAR_PUNTO(centroLocalFuego, matrizInv, centroEscenaFuego.x, centroEscenaFuego.y)
			_luzFuego->establecerTraslacion(_particulasFuego->traslacion());
		} else {
			_luzFuego->establecerAlcanceLuz(0.0f);
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

bool AULaser::agregarXmlInternoEn(AULaser* laser, AUCadenaLargaMutable8* guardarEn, const char* espaciosBaseIzq, AUArreglo* arregloObjetosParaID){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AULaser::agregarXmlInternoEn")
	//guardarEn->agregarConFormato("%s<vXML>1</vXML>\r\n", espaciosBaseIzq);
	AUNodoElectroCarga::agregarXmlInternoEn(laser, guardarEn, espaciosBaseIzq, arregloObjetosParaID);
	guardarEn->agregarConFormato("%s<act>%d</act>\r\n", espaciosBaseIzq, laser->_laserActivo?1:0);
	guardarEn->agregarConFormato("%s<desactivar>%d</desactivar>\r\n", espaciosBaseIzq, laser->_desactivarDespuesDeContacto?1:0);
	guardarEn->agregarConFormato("%s<igP>%d</igP>\r\n", espaciosBaseIzq, laser->_presentacionLaser->laserColisionIgnorarPrimera()?1:0);
	guardarEn->agregarConFormato("%s<crgMin>%f</crgMin>\r\n", espaciosBaseIzq, laser->_cargaParaConsiderarCargada);
	guardarEn->agregarConFormato("%s<alc>%f</alc>\r\n", espaciosBaseIzq, laser->alcanceLaser());
	guardarEn->agregarConFormato("%s<anc>%f</anc>\r\n", espaciosBaseIzq, laser->anchoLaser());
	guardarEn->agregarConFormato("%s<fCrta>%f</fCrta>\r\n", espaciosBaseIzq, laser->_factorCortaCuerdas);
	guardarEn->agregarConFormato("%s<velD><x>%f</x><y>%f</y></velD>\r\n", espaciosBaseIzq, laser->velocidadDesplazamientoTextura().x, laser->velocidadDesplazamientoTextura().y);
	guardarEn->agregarConFormato("%s<segsO>%f</segsO>\r\n", espaciosBaseIzq, laser->_segundosEsperaDespuesDeObstruccion);
	guardarEn->agregarConFormato("%s<segsL>%f</segsL>\r\n", espaciosBaseIzq, laser->_segundosEsperaDespuesDeLiberacion);
	SI32 indiceObjEnArreglo = -1;
	if(laser->_sensorComandosColision != NULL){
		indiceObjEnArreglo = arregloObjetosParaID->indiceDe(laser->_sensorComandosColision);
		if(indiceObjEnArreglo==-1) PRINTF_INFO("-------------------- ADVERTENCIA: destino de laser-obstruccion no pudo ser guardado\n");
	}
	guardarEn->agregarConFormato("%s<iSen>%d</iSen>\r\n", espaciosBaseIzq, indiceObjEnArreglo);
	indiceObjEnArreglo = -1;
	if(laser->_sensorComandosDescolision != NULL){
		indiceObjEnArreglo = arregloObjetosParaID->indiceDe(laser->_sensorComandosDescolision);
		if(indiceObjEnArreglo==-1) PRINTF_INFO("-------------------- ADVERTENCIA: destino de laser-liberacion no pudo ser guardado\n");
	}
	guardarEn->agregarConFormato("%s<iSenD>%d</iSenD>\r\n", espaciosBaseIzq, indiceObjEnArreglo);
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return true;
}

bool AULaser::interpretarXmlInterno(AULaser* cargarEn, const AUDatosXML* datosXml, const sNodoXML* nodoXml, AUArregloNativoP<AUEscenaObjeto*>* arregloObjetos, AUArregloNativoP<SI32>* arregloIDs){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AULaser::interpretarXmlInterno")
	bool exito = false;
	//int versionXML = datosXml->nodoHijo("vXML", 0, nodoXml);
	//if(versionXML==1){
	if(AUNodoElectroCarga::interpretarXmlInterno(cargarEn, datosXml, nodoXml, arregloObjetos, arregloIDs)){
		cargarEn->_laserActivo							= datosXml->nodoHijo("act", true, nodoXml);
		cargarEn->_desactivarDespuesDeContacto			= datosXml->nodoHijo("desactivar", false, nodoXml);
		cargarEn->_cargaParaConsiderarCargada			= datosXml->nodoHijo("crgMin", 0.75f, nodoXml);
		cargarEn->_segundosEsperaDespuesDeObstruccion	= datosXml->nodoHijo("segsO", 0.0f, nodoXml);
		cargarEn->_segundosEsperaDespuesDeLiberacion	= datosXml->nodoHijo("segsL", 0.0f, nodoXml);
		bool ignorarPrimeraCol							= datosXml->nodoHijo("igP", false, nodoXml);
		float alcanceLaser								= datosXml->nodoHijo("alc", 64.0f, nodoXml);
		float anchoLaser								= datosXml->nodoHijo("anc", 4.0f, nodoXml);
		cargarEn->establecerFactorCortaCuerdas(datosXml->nodoHijo("fCrta", 0.0f, nodoXml));
		cargarEn->_presentacionLaser->establecerLaserColisionIgnorarPrimera(ignorarPrimeraCol);
		cargarEn->_presentacionLaser->establecerLaserAlcance(alcanceLaser);
		cargarEn->_presentacionLaser->establecerLaserAncho(anchoLaser);
		//
		NBPunto velDespTex;
		const sNodoXML* nodoVelDespTex					= datosXml->nodoHijo("velD", nodoXml);
		if(nodoVelDespTex != NULL){
			velDespTex.x				= datosXml->nodoHijo("x", 128.0f, nodoVelDespTex);
			velDespTex.y				= datosXml->nodoHijo("y", 0.0f, nodoVelDespTex);
		} else {
			velDespTex.x				= 128.0f;
			velDespTex.y				= 0.0f;
		}
		cargarEn->_presentacionLaser->establecerLaserVelDesplazaTex(velDespTex);
		AUEscenaSensor* objetoDestino = NULL;
		SI32 iObj = datosXml->nodoHijo("iSen", -1, nodoXml);
		if(iObj!=-1){
			SI32 indiceObjeto = arregloIDs->indiceDe(iObj);
			if(indiceObjeto!=-1){
				AUEscenaObjeto* objEscena = arregloObjetos->elemento[indiceObjeto];
				if(objEscena->esClase(AUEscenaSensor::idTipoClase)){
					objetoDestino = (AUEscenaSensor*)objEscena;
				} else {
					PRINTF_INFO("-------------------- ADVERTENCIA: destino-obstruccion de laser no pudo ser cargado (no es clase 'AUEscenaSensor')\n");
				}
			}
		}
		cargarEn->establecerSensorComandosColision(objetoDestino);
		objetoDestino = NULL;
		iObj = datosXml->nodoHijo("iSenD", -1, nodoXml);
		if(iObj!=-1){
			SI32 indiceObjeto = arregloIDs->indiceDe(iObj);
			if(indiceObjeto!=-1){
				AUEscenaObjeto* objEscena = arregloObjetos->elemento[indiceObjeto];
				if(objEscena->esClase(AUEscenaSensor::idTipoClase)){
					objetoDestino = (AUEscenaSensor*)objEscena;
				} else {
					PRINTF_INFO("-------------------- ADVERTENCIA: destino-liberacion de laser no pudo ser cargado (no es clase 'AUEscenaSensor')\n");
				}
			}
		}
		cargarEn->establecerSensorComandosDescolision(objetoDestino);
		exito = true;
	}
	//}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

bool AULaser::agregarBitsInternosEn(AULaser* laser, AUArchivo* guardarEn, AUArreglo* arregloObjetosParaID){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AULaser::agregarBitsInternosEn")
	bool exito = false;
	//
	float versionDatos = 1.0f;
	guardarEn->escribir(&versionDatos, sizeof(versionDatos), 1, guardarEn);
	if(AUNodoElectroCarga::agregarBitsInternosEn(laser, guardarEn, arregloObjetosParaID)){
		bool activo		= laser->_laserActivo;					guardarEn->escribir(&activo, sizeof(activo), 1, guardarEn);
		bool desactivar = laser->_desactivarDespuesDeContacto;	guardarEn->escribir(&desactivar, sizeof(desactivar), 1, guardarEn);
		float cargaReq	= laser->_cargaParaConsiderarCargada;	guardarEn->escribir(&cargaReq, sizeof(cargaReq), 1, guardarEn);
		bool igPrim		= laser->ignorarPrimeraColision();		guardarEn->escribir(&igPrim, sizeof(igPrim), 1, guardarEn);
		float alcance	= laser->alcanceLaser();				guardarEn->escribir(&alcance, sizeof(alcance), 1, guardarEn);
		float ancho		= laser->anchoLaser();					guardarEn->escribir(&ancho, sizeof(ancho), 1, guardarEn);
		NBPunto velTex	= laser->velocidadDesplazamientoTextura(); guardarEn->escribir(&velTex, sizeof(velTex), 1, guardarEn);
		float fCorta	= laser->_factorCortaCuerdas;			guardarEn->escribir(&fCorta, sizeof(fCorta), 1, guardarEn);
		guardarEn->escribir(&laser->_segundosEsperaDespuesDeObstruccion, sizeof(laser->_segundosEsperaDespuesDeObstruccion), 1, guardarEn);
		guardarEn->escribir(&laser->_segundosEsperaDespuesDeLiberacion, sizeof(laser->_segundosEsperaDespuesDeLiberacion), 1, guardarEn);
		SI32 indiceObjEnCarreglo = -1;
		if(laser->_sensorComandosColision != NULL){
			indiceObjEnCarreglo = arregloObjetosParaID->indiceDe(laser->_sensorComandosColision);
			if(indiceObjEnCarreglo==-1) PRINTF_INFO("-------------------- ADVERTENCIA: destino-colision de laser no pudo ser guardado\n");
		}
		guardarEn->escribir(&indiceObjEnCarreglo, sizeof(indiceObjEnCarreglo), 1, guardarEn);
		indiceObjEnCarreglo = -1;
		if(laser->_sensorComandosDescolision != NULL){
			indiceObjEnCarreglo = arregloObjetosParaID->indiceDe(laser->_sensorComandosDescolision);
			if(indiceObjEnCarreglo==-1) PRINTF_INFO("-------------------- ADVERTENCIA: destino-liberacion de laser no pudo ser guardado\n");
		}
		guardarEn->escribir(&indiceObjEnCarreglo, sizeof(indiceObjEnCarreglo), 1, guardarEn);
		exito = true;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

bool AULaser::interpretarBitsInternos(AULaser* cargarEn, AUArchivo* cargarDe, AUArreglo* arregloObjetosParaID){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AULaser::interpretarBitsInternos")
	bool exito = false;
	float versionDatos;
	cargarDe->leer(&versionDatos, sizeof(versionDatos), 1, cargarDe);
	if(versionDatos==1.0f){
		if(AUNodoElectroCarga::interpretarBitsInternos(cargarEn, cargarDe, arregloObjetosParaID)){
			bool activo;		cargarDe->leer(&activo, sizeof(activo), 1, cargarDe);			cargarEn->_laserActivo = activo;
			bool desactivar;	cargarDe->leer(&desactivar, sizeof(desactivar), 1, cargarDe);	cargarEn->_desactivarDespuesDeContacto = desactivar;
			float cargaReq;		cargarDe->leer(&cargaReq, sizeof(cargaReq), 1, cargarDe);		cargarEn->_cargaParaConsiderarCargada = cargaReq;
			bool igPrim;		cargarDe->leer(&igPrim, sizeof(igPrim), 1, cargarDe);
			float alcance;		cargarDe->leer(&alcance, sizeof(alcance), 1, cargarDe);
			float ancho;		cargarDe->leer(&ancho, sizeof(ancho), 1, cargarDe);
			NBPunto velTex;		cargarDe->leer(&velTex, sizeof(velTex), 1, cargarDe);
			float fCorta;		cargarDe->leer(&fCorta, sizeof(fCorta), 1, cargarDe);
			cargarDe->leer(&cargarEn->_segundosEsperaDespuesDeObstruccion, sizeof(cargarEn->_segundosEsperaDespuesDeObstruccion), 1, cargarDe);
			cargarDe->leer(&cargarEn->_segundosEsperaDespuesDeLiberacion, sizeof(cargarEn->_segundosEsperaDespuesDeLiberacion), 1, cargarDe);
			cargarEn->establecerFactorCortaCuerdas(fCorta);
			cargarEn->_presentacionLaser->establecerLaserColisionIgnorarPrimera(igPrim);
			cargarEn->_presentacionLaser->establecerLaserVelDesplazaTex(velTex);
			cargarEn->_presentacionLaser->establecerLaserAlcance(alcance);
			cargarEn->_presentacionLaser->establecerLaserAncho(ancho);
			//
			SI32 iObj;			cargarDe->leer(&iObj, sizeof(iObj), 1, cargarDe);
			AUEscenaSensor*		objDestino = NULL;
			if(iObj!=-1){
				AUObjeto* objEscena = (AUObjeto*)arregloObjetosParaID->elemento[iObj];
				if(objEscena->esClase(AUEscenaSensor::idTipoClase)){
					objDestino = (AUEscenaSensor*)objEscena;
				} else {
					PRINTF_INFO("-------------------- ADVERTENCIA: destino-obstruccion de laser no pudo ser cargado (no es clase 'AUEscenaSensor')\n");
				}
			}
			cargarEn->establecerSensorComandosColision(objDestino);
			cargarDe->leer(&iObj, sizeof(iObj), 1, cargarDe);
			objDestino = NULL;
			if(iObj!=-1){
				AUObjeto* objEscena = (AUObjeto*)arregloObjetosParaID->elemento[iObj];
				if(objEscena->esClase(AUEscenaSensor::idTipoClase)){
					objDestino = (AUEscenaSensor*)objEscena;
				} else {
					PRINTF_INFO("-------------------- ADVERTENCIA: destino-liberacion de laser no pudo ser cargado (no es clase 'AUEscenaSensor')\n");
				}
			}
			cargarEn->establecerSensorComandosDescolision(objDestino);
			exito = true;
		}
	} else {
		PRINTF_ERROR("AULaser, la version (%f) de datos binarios no es interpretable por este ejecutable\n", versionDatos);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

//
AUOBJMETODOS_CLASESID_MULTICLASE(AULaser, AUNodoElectroCarga)
AUOBJMETODOS_CLASESNOMBRES_MULTICLASE(AULaser, "AULaser", AUNodoElectroCarga)
AUOBJMETODOS_CLONAR_THIS(AULaser)





