//
//  AUEscenaSensor.cpp
//  EditorCuerpos
//
//  Created by Nicaragua Binary on 24/12/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkMediaStdAfx.h"
#include "AUEscenaSensor.h"

AUEscenaSensor::AUEscenaSensor() : AUEscenaObjeto(), _colisiones(this), _comandosSensor(this), _comandosEsperando(this) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaSensor::AUEscenaSensor")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaSensor")
	_escuchador				= NULL;
	_ultimoTickDentroEscena = false;
	_propiedadesSensor		= 0;
	_sensorActivo			= true;
	//
	_cuerpo					= NULL;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

AUEscenaSensor::AUEscenaSensor(AUEscenaSensor* otraInstancia) : AUEscenaObjeto(otraInstancia), _colisiones(this) , _comandosSensor(this, &otraInstancia->_comandosSensor), _comandosEsperando(this) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaSensor::AUEscenaSensor")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaSensor")
	_escuchador				= NULL;
	_ultimoTickDentroEscena = false;
	_propiedadesSensor		= 0;
	_sensorActivo			= otraInstancia->_sensorActivo;
	//
	_cuerpo					= NULL;
	//Retener todos los objetos destinos de los comandos
	UI32 i, iConteo = _comandosSensor.conteo;
	for(i=0; i<iConteo; i++){
		_comandosSensor.elemento[i].strComando = new(this) AUCadenaMutable8(_comandosSensor.elemento[i].strComando->str());
		if(_comandosSensor.elemento[i].objUnido != NULL) _comandosSensor.elemento[i].objUnido->retener(NB_RETENEDOR_THIS);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

AUEscenaSensor::~AUEscenaSensor(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaSensor::~AUEscenaSensor")
	UI32 i, iConteo = _comandosSensor.conteo;
	for(i=0; i<iConteo; i++){
		_comandosSensor.elemento[i].strComando->liberar(NB_RETENEDOR_THIS);
		if(_comandosSensor.elemento[i].objUnido != NULL) _comandosSensor.elemento[i].objUnido->liberar(NB_RETENEDOR_THIS);
	}
	_comandosSensor.vaciar();
	NBASSERT(_cuerpo == NULL)
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

// Fisica

/*b2Body*/void* AUEscenaSensor::cuerpo(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaSensor::cuerpo")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _cuerpo;
}

//

IEscuchadorSensor* AUEscenaSensor::escuchadorSensor(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaSensor::escuchadorSensor")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _escuchador;
}

void AUEscenaSensor::establecerEscuchadorSensor(IEscuchadorSensor* escuchador){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaSensor::establecerEscuchadorSensor")
	_escuchador = escuchador;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

bool AUEscenaSensor::sensorActivo(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaSensor::sensorActivo")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _sensorActivo;
}

void AUEscenaSensor::establecerSensorActivo(bool sensorActivo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaSensor::establecerSensorActivo")
	_sensorActivo = sensorActivo;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaSensor::acumularColisionConHeroe(AUObjeto* objColisionado){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaSensor::acumularColisionConHeroe")
	if(_sensorActivo){
		SI32 indiceEnArreglo = -1;
		UI16 iCol, conteoCols = _colisiones.conteo;
		for(iCol=0; iCol<conteoCols; iCol++){
			if(_colisiones.elemento[iCol].objetoColisionado==objColisionado){
				indiceEnArreglo = iCol;
				break;
			}
		}
		if(indiceEnArreglo==-1){
			STSensorColision datosColision;
			datosColision.objetoColisionado = objColisionado;
			datosColision.segundosAcumulados = 0.0f;
			_colisiones.agregarElemento(datosColision);
			_propiedadesSensor |= AUSENSOR_BITS_EJECUTAR_CMDS_COLISION;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaSensor::acumularEjecucionComandosManuales(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaSensor::acumularEjecucionComandosManuales")
	if(_sensorActivo){
		_propiedadesSensor |= AUSENSOR_BITS_EJECUTAR_CMDS_MANUALES;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

float AUEscenaSensor::segundosParaComandosColisionEjecutables(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaSensor::segundosParaComandosColisionEjecutables")
	float segundos = 0.0f;
	UI32 iCmd, iCmdConteo = _comandosSensor.conteo;
	for(iCmd=0; iCmd<iCmdConteo; iCmd++){
		STSensorComando* datosComando = &(_comandosSensor.elemento[iCmd]);
		if((!datosComando->ejecutarSoloUnaVez || datosComando->vecesEjecutado==0) && datosComando->eventoParaEjecutar==ENSensorEvento_ColisionHeroe){
			float segundosTotalComando = datosComando->segsEsperaParaEjecutar+datosComando->segsSuavizadoComando;
			if(segundos<segundosTotalComando) segundos = segundosTotalComando;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return segundos;
}

void AUEscenaSensor::agregarComando(const char* strComando, bool ejecutarSoloUnaVez, ENSensorEvento evento, float segundoRetardoEjecucion, ENAnimPropVelocidad tipoSuavizado, float segundosSuavizadoEjecucion, AUObjeto* objUnido){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaSensor::agregarComando")
	NBASSERT(strComando != NULL)
	STSensorComando datosComando;
	datosComando.ejecutarSoloUnaVez			= ejecutarSoloUnaVez;
	datosComando.eventoParaEjecutar			= evento;
	datosComando.tipoSuavizado				= tipoSuavizado;
	datosComando.vecesEjecutado				= 0;
	datosComando.segsEsperaParaEjecutar		= segundoRetardoEjecucion;
	datosComando.segsSuavizadoComando		= segundosSuavizadoEjecucion;
	datosComando.strComando					= new AUCadenaMutable8(strComando);
	datosComando.objUnido					= objUnido; if(objUnido != NULL) objUnido->retener(NB_RETENEDOR_THIS);
	_comandosSensor.agregarElemento(datosComando);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaSensor::actualizarComandoEnIndice(UI32 iComando, const char* strComandoNuevo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaSensor::actualizarComandoEnIndice")
	NBASSERT(iComando<_comandosSensor.conteo)
	NBASSERT(strComandoNuevo != NULL)
	_comandosSensor.elemento[iComando].strComando->establecer(strComandoNuevo);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaSensor::actualizarComandoEventoEnIndice(UI32 iComando, ENSensorEvento evento){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaSensor::actualizarComandoEnIndice")
	NBASSERT(iComando<_comandosSensor.conteo)
	NBASSERT(evento>=0 && evento<ENSensorEvento_Conteo)
	_comandosSensor.elemento[iComando].eventoParaEjecutar = evento;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaSensor::actualizarComandoParamentroEnIndice(UI32 iComando, AUObjeto* objUnido){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaSensor::actualizarComandoParamentroEnIndice")
	NBASSERT(iComando<_comandosSensor.conteo)
	if(objUnido != NULL) objUnido->retener(NB_RETENEDOR_THIS);
	if(_comandosSensor.elemento[iComando].objUnido != NULL) _comandosSensor.elemento[iComando].objUnido->liberar(NB_RETENEDOR_THIS);
	_comandosSensor.elemento[iComando].objUnido = objUnido;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaSensor::actualizarComandoRetardoEnIndice(UI32 iComando, float segundosRetardo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaSensor::actualizarComandoRetardoEnIndice")
	NBASSERT(iComando<_comandosSensor.conteo)
	NBASSERT(segundosRetardo>=0.0f && segundosRetardo<3600)
	_comandosSensor.elemento[iComando].segsEsperaParaEjecutar = segundosRetardo;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaSensor::actualizarComandoTipoSuavizadoEnIndice(UI32 iComando, ENAnimPropVelocidad tipoSuavizado){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaSensor::actualizarComandoTipoSuavizadoEnIndice")
	NBASSERT(iComando<_comandosSensor.conteo)
	NBASSERT(tipoSuavizado>=0 && tipoSuavizado<ENAnimPropVelocidad_Conteo)
	_comandosSensor.elemento[iComando].tipoSuavizado = tipoSuavizado;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaSensor::actualizarComandoSuavizadoEnIndice(UI32 iComando, float segundosSuavizado){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaSensor::actualizarComandoSUavizadoEnIndice")
	NBASSERT(iComando<_comandosSensor.conteo)
	NBASSERT(segundosSuavizado>=0.0f && segundosSuavizado<3600)
	_comandosSensor.elemento[iComando].segsSuavizadoComando = segundosSuavizado;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaSensor::quitarComandoEnIndice(UI32 iComando){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaSensor::quitarComandoEnIndice")
	NBASSERT(iComando<_comandosSensor.conteo)
	_comandosSensor.elemento[iComando].strComando->liberar(NB_RETENEDOR_THIS);
	if(_comandosSensor.elemento[iComando].objUnido != NULL) _comandosSensor.elemento[iComando].objUnido->liberar(NB_RETENEDOR_THIS);
	_comandosSensor.quitarElementoEnIndice(iComando);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

AUArregloNativoP<STSensorComando>* AUEscenaSensor::comandosSensor(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaSensor::comandosSensor")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return &_comandosSensor;
}

//

NBCajaAABB AUEscenaSensor::cajaAABBLocalCalculada(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaSensor::cajaAABBLocalCalculada")
	NBCajaAABB cajaAABBLocal;
	cajaAABBLocal.xMin = -32.0f;
	cajaAABBLocal.yMin = -32.0f;
	cajaAABBLocal.xMax = 32.0f;
	cajaAABBLocal.yMax = 32.0f;
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return cajaAABBLocal;
}

PTRfunCmdsGL AUEscenaSensor::actualizarModeloGL(const NBPropRenderizado &propsRenderizado, const NBPropIluminacion &propsIluminacion, const NBPropHeredadasRender &propsHeredadas){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaSensor::actualizarModeloGL")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return NULL;
}

void AUEscenaSensor::agregadoEnEscena(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaSensor::agregadoEnEscena")
	NBGestorAnimadores::agregarAnimador(this, this);
	//crear cuerpo en base a transformaciones de este objeto
	NBASSERT(_cuerpo == NULL)
	if(_idMundoFisica!=-1){
#		ifndef CONFIG_NB_UNSUPPORT_BOX2D
		//Transformaciones de referencia
		NBMatriz matrizEscenaCuerpo = this->matrizEscenaCalculada();
		NBPunto posEscena;		NBMATRIZ_MULTIPLICAR_PUNTO(posEscena, matrizEscenaCuerpo, 0.0f, 0.0f);
		NBPunto posEscenaDer;	NBMATRIZ_MULTIPLICAR_PUNTO(posEscenaDer, matrizEscenaCuerpo, 1.0f, 0.0f);
		float radianesRotacion;	NBPUNTO_RADIANES_VECTOR(radianesRotacion, posEscena.x, posEscena.y, posEscenaDer.x, posEscenaDer.y);
		b2BodyDef defCuerpo;
		defCuerpo.position.Set(posEscena.x/ESCALA_PIXELES_POR_METRO_MUNDO_FISICA, posEscena.y/ESCALA_PIXELES_POR_METRO_MUNDO_FISICA);
		defCuerpo.angle			= radianesRotacion;
		defCuerpo.type			= b2_staticBody;
		defCuerpo.active		= true;
		defCuerpo.allowSleep	= true;
		defCuerpo.userData		= this;
		_cuerpo					= NBGestorFisica::crearCuerpo(_idMundoFisica, &defCuerpo);
		//crear fijaciones a cuerpo
		NBPunto vectorEscalacion; NBMATRIZ_MULTIPLICAR_PUNTO(vectorEscalacion, matrizEscenaCuerpo, 1.0f, 1.0f); //Para determinar si los vertices han sido invertido en orden contrareloj
		vectorEscalacion.x -= posEscena.x; vectorEscalacion.y -= posEscena.y; NBPUNTO_ROTAR_RADIANES(vectorEscalacion, vectorEscalacion, -radianesRotacion); //De coordenada escena a local
		//
		b2FixtureDef propiedadesCuerpo;
		propiedadesCuerpo.isSensor		= true;
		propiedadesCuerpo.density		= 0.1f;
		propiedadesCuerpo.friction		= 0.0f;
		propiedadesCuerpo.restitution	= 1.0f;
		//poligono
		b2Vec2 puntosBox2D[4];
		if((vectorEscalacion.x*vectorEscalacion.y)<0.0f){	//si la escala invierte las fijaciones, entonces procesar en orden inverso
			SI32 iVert = 0;
			NBMATRIZ_MULTIPLICAR_PUNTO(puntosBox2D[iVert], matrizEscenaCuerpo, -0.5f, 0.5f); puntosBox2D[iVert].x -= posEscena.x; puntosBox2D[iVert].y -= posEscena.y; NBPUNTO_ROTAR_RADIANES(puntosBox2D[iVert], puntosBox2D[iVert], -radianesRotacion); iVert++;//De coordenada escena a local
			NBMATRIZ_MULTIPLICAR_PUNTO(puntosBox2D[iVert], matrizEscenaCuerpo, 0.5f, 0.5f); puntosBox2D[iVert].x -= posEscena.x; puntosBox2D[iVert].y -= posEscena.y; NBPUNTO_ROTAR_RADIANES(puntosBox2D[iVert], puntosBox2D[iVert], -radianesRotacion); iVert++;//De coordenada escena a local
			NBMATRIZ_MULTIPLICAR_PUNTO(puntosBox2D[iVert], matrizEscenaCuerpo, 0.5f, -0.5f); puntosBox2D[iVert].x -= posEscena.x; puntosBox2D[iVert].y -= posEscena.y; NBPUNTO_ROTAR_RADIANES(puntosBox2D[iVert], puntosBox2D[iVert], -radianesRotacion); iVert++;//De coordenada escena a local
			NBMATRIZ_MULTIPLICAR_PUNTO(puntosBox2D[iVert], matrizEscenaCuerpo, -0.5f, -0.5f); puntosBox2D[iVert].x -= posEscena.x; puntosBox2D[iVert].y -= posEscena.y; NBPUNTO_ROTAR_RADIANES(puntosBox2D[iVert], puntosBox2D[iVert], -radianesRotacion); iVert++;//De coordenada escena a local
		} else {
			SI32 iVert = 0;
			NBMATRIZ_MULTIPLICAR_PUNTO(puntosBox2D[iVert], matrizEscenaCuerpo, -0.5f, -0.5f); puntosBox2D[iVert].x -= posEscena.x; puntosBox2D[iVert].y -= posEscena.y; NBPUNTO_ROTAR_RADIANES(puntosBox2D[iVert], puntosBox2D[iVert], -radianesRotacion); iVert++;//De coordenada escena a local
			NBMATRIZ_MULTIPLICAR_PUNTO(puntosBox2D[iVert], matrizEscenaCuerpo, 0.5f, -0.5f); puntosBox2D[iVert].x -= posEscena.x; puntosBox2D[iVert].y -= posEscena.y; NBPUNTO_ROTAR_RADIANES(puntosBox2D[iVert], puntosBox2D[iVert], -radianesRotacion); iVert++;//De coordenada escena a local
			NBMATRIZ_MULTIPLICAR_PUNTO(puntosBox2D[iVert], matrizEscenaCuerpo, 0.5f, 0.5f); puntosBox2D[iVert].x -= posEscena.x; puntosBox2D[iVert].y -= posEscena.y; NBPUNTO_ROTAR_RADIANES(puntosBox2D[iVert], puntosBox2D[iVert], -radianesRotacion); iVert++;//De coordenada escena a local
			NBMATRIZ_MULTIPLICAR_PUNTO(puntosBox2D[iVert], matrizEscenaCuerpo, -0.5f, 0.5f); puntosBox2D[iVert].x -= posEscena.x; puntosBox2D[iVert].y -= posEscena.y; NBPUNTO_ROTAR_RADIANES(puntosBox2D[iVert], puntosBox2D[iVert], -radianesRotacion); iVert++;//De coordenada escena a local
		}
		b2PolygonShape defFormaPoligono; defFormaPoligono.Set(puntosBox2D, 4);
		propiedadesCuerpo.shape = &defFormaPoligono;
		((b2Body*)_cuerpo)->CreateFixture(&propiedadesCuerpo);
		//Es estatico: no requiere union
		//NBGestorFisica::agregarUnion(this, _cuerpo, tipoAnimacion);
#		endif
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaSensor::quitandoDeEscena(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaSensor::quitandoDeEscena")
	NBGestorAnimadores::quitarAnimador(this);
	if(_idMundoFisica!=-1){
		NBASSERT(_cuerpo != NULL)
		//Destruir cuerpo
		if(_cuerpo != NULL){
			//Es estatico: no requiere union
#			ifndef CONFIG_NB_UNSUPPORT_BOX2D
			//NBGestorFisica::quitarUnion(this, _cuerpo);
			NBGestorFisica::destruirCuerpo(_idMundoFisica, (b2Body*)_cuerpo);
#			endif
			_cuerpo = NULL;
		}
	}
	NBASSERT(_cuerpo == NULL)
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

//

void AUEscenaSensor::tickAnimacion(float segsTranscurridos){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaSensor::tickAnimacion")
	NBASSERT(this->idEscena!=-1)
	//Liberar colisiones viejas
	SI32 iCol ;
	for(iCol=_colisiones.conteo-1; iCol>=0; iCol--){
		_colisiones.elemento[iCol].segundosAcumulados += segsTranscurridos;
		if(_colisiones.elemento[iCol].segundosAcumulados>0.25f){
			_colisiones.quitarElementoEnIndice(iCol);
		}
	}
	if(this->_dormido){ AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO_3() }
	if(_escuchador != NULL){
		//COMANDOS PENDIENTES DE SER EJECUTADOS
		SI32 iCmd, iCmdConteo = _comandosEsperando.conteo;
		for(iCmd=0; iCmd<iCmdConteo; iCmd++){
			STSensorComandoEsperando* datosEspera = &(_comandosEsperando.elemento[iCmd]);
			datosEspera->segundosEsperar -= segsTranscurridos;
			if(datosEspera->segundosEsperar<=0.0f){
				STSensorComando* datosComando = &(_comandosSensor.elemento[datosEspera->iComando]);
				if(_escuchador->sensorEjecutarComando(this, datosComando->strComando->str(), datosComando->objUnido, datosComando->segsSuavizadoComando, (ENAnimPropVelocidad)datosComando->tipoSuavizado)){
					//
				}
				_comandosEsperando.quitarElementoEnIndice(iCmd--); iCmdConteo--;
			}
		}
		//
		if(_sensorActivo){
			//COMANDOS DE ENTRAR O SALIR DE CAMARA
			NBCajaAABB cajaProyeccion = NBGestorEscena::cajaProyeccionGrupo(this->idEscena, (ENGestorEscenaGrupo)this->idGrupo);
			NBCajaAABB cajaEscena	= this->cajaAABBEnEscena();
			bool dentroEscena		= NBCAJAAABB_INTERSECTA_CAJA(cajaProyeccion, cajaEscena);
			iCmdConteo				= _comandosSensor.conteo;
			if(dentroEscena!=_ultimoTickDentroEscena){
				for(iCmd=0; iCmd<iCmdConteo; iCmd++){
					STSensorComando* datosComando = &(_comandosSensor.elemento[iCmd]);
					if((!datosComando->ejecutarSoloUnaVez || datosComando->vecesEjecutado==0) && ((datosComando->eventoParaEjecutar==ENSensorEvento_EntraEnCamara && dentroEscena) || (datosComando->eventoParaEjecutar==ENSensorEvento_SaleDeCamara && !dentroEscena))){
						if(datosComando->segsEsperaParaEjecutar==0.0f){
							if(_escuchador->sensorEjecutarComando(this, datosComando->strComando->str(), datosComando->objUnido, datosComando->segsSuavizadoComando, (ENAnimPropVelocidad)datosComando->tipoSuavizado)){
								datosComando->vecesEjecutado++;
							}
						} else {
							STSensorComandoEsperando datosEspera;
							datosEspera.iComando = iCmd;
							datosEspera.segundosEsperar = datosComando->segsEsperaParaEjecutar;
							_comandosEsperando.agregarElemento(datosEspera);
							datosComando->vecesEjecutado++;
						}
					}
				}
			}
			_ultimoTickDentroEscena = dentroEscena;
		}
		//COMANDOS DE COLISION CON HEROE
		if(_propiedadesSensor & AUSENSOR_BITS_EJECUTAR_CMDS_COLISION){
			iCmdConteo = _comandosSensor.conteo;
			for(iCmd=0; iCmd<iCmdConteo; iCmd++){
				STSensorComando* datosComando = &(_comandosSensor.elemento[iCmd]);
				if((!datosComando->ejecutarSoloUnaVez || datosComando->vecesEjecutado==0) && datosComando->eventoParaEjecutar==ENSensorEvento_ColisionHeroe){
					if(datosComando->segsEsperaParaEjecutar==0.0f){
						if(_escuchador->sensorEjecutarComando(this, datosComando->strComando->str(), datosComando->objUnido, datosComando->segsSuavizadoComando, (ENAnimPropVelocidad)datosComando->tipoSuavizado)){
							datosComando->vecesEjecutado++;
						}
					} else {
						STSensorComandoEsperando datosEspera;
						datosEspera.iComando = iCmd;
						datosEspera.segundosEsperar = datosComando->segsEsperaParaEjecutar;
						_comandosEsperando.agregarElemento(datosEspera);
						datosComando->vecesEjecutado++;
					}
				}
			}
			_propiedadesSensor &= ~AUSENSOR_BITS_EJECUTAR_CMDS_COLISION;
		}
		//COMANDOS MANUALES
		if(_propiedadesSensor & AUSENSOR_BITS_EJECUTAR_CMDS_MANUALES){
			iCmdConteo = _comandosSensor.conteo;
			for(iCmd=0; iCmd<iCmdConteo; iCmd++){
				STSensorComando* datosComando = &(_comandosSensor.elemento[iCmd]);
				if((!datosComando->ejecutarSoloUnaVez || datosComando->vecesEjecutado==0) && datosComando->eventoParaEjecutar==ENSensorEvento_Manual){
					if(datosComando->segsEsperaParaEjecutar==0.0f){
						if(_escuchador->sensorEjecutarComando(this, datosComando->strComando->str(), datosComando->objUnido, datosComando->segsSuavizadoComando, (ENAnimPropVelocidad)datosComando->tipoSuavizado)){
							datosComando->vecesEjecutado++;
						}
					} else {
						STSensorComandoEsperando datosEspera;
						datosEspera.iComando = iCmd;
						datosEspera.segundosEsperar = datosComando->segsEsperaParaEjecutar;
						_comandosEsperando.agregarElemento(datosEspera);
						datosComando->vecesEjecutado++;
					}
				}
			}
			_propiedadesSensor &= AUSENSOR_BITS_EJECUTAR_CMDS_MANUALES;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

//

bool AUEscenaSensor::agregarXmlInternoEn(AUEscenaSensor* sensor, AUCadenaLargaMutable8* guardarEn, const char* espaciosBaseIzq, AUArreglo* arregloCapasPorID){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaSensor::agregarXmlInternoEn")
	//guardarEn->agregarConFormato("%s<vXML>1</vXML>\r\n", espaciosBaseIzq);
	guardarEn->agregarConFormato("%s<eOb>\r\n", espaciosBaseIzq);
	AUEscenaObjeto::agregarXmlInternoEn(sensor, guardarEn, espaciosBaseIzq);
	guardarEn->agregarConFormato("%s</eOb>\r\n", espaciosBaseIzq);
	guardarEn->agregarConFormato("%s<senActivo>%d</senActivo>\r\n", espaciosBaseIzq, (SI32)sensor->_sensorActivo);
	guardarEn->agregarConFormato("%s<comandos>\r\n", espaciosBaseIzq);
	UI32 iCmd, iCmdConteo = sensor->_comandosSensor.conteo;
	for(iCmd=0; iCmd<iCmdConteo; iCmd++){
		STSensorComando* comando	= &(sensor->_comandosSensor.elemento[iCmd]);
		SI32 iCapa					= -1;
		SI32 iObjDestino			= -1;
		if(comando->objUnido != NULL){
			NBASSERT(comando->objUnido->esClase(AUEscenaObjeto::idTipoClase))
			AUEscenaContenedor* contenedor = (AUEscenaContenedor*)((AUEscenaObjeto*)comando->objUnido)->contenedor();
			if(contenedor != NULL){
				iCapa = arregloCapasPorID->indiceDe(contenedor);
				if(iCapa!=-1) iObjDestino = contenedor->hijos()->indiceDe(comando->objUnido);
			}
		}
		guardarEn->agregarConFormato("%s <cmd>\r\n", espaciosBaseIzq);
		guardarEn->agregarConFormato("%s  <strCmd>%s</strCmd>\r\n", espaciosBaseIzq, comando->strComando->str());
		guardarEn->agregarConFormato("%s  <unaVez>%d</unaVez>\r\n", espaciosBaseIzq, comando->ejecutarSoloUnaVez?1:0);
		guardarEn->agregarConFormato("%s  <evnt>%d</evnt>\r\n", espaciosBaseIzq, (SI32)comando->eventoParaEjecutar);
		guardarEn->agregarConFormato("%s  <segsEsp>%f</segsEsp>\r\n", espaciosBaseIzq, comando->segsEsperaParaEjecutar);
		guardarEn->agregarConFormato("%s  <tipoSuav>%d</tipoSuav>\r\n", espaciosBaseIzq, (SI32)comando->tipoSuavizado);
		guardarEn->agregarConFormato("%s  <segsSuav>%f</segsSuav>\r\n", espaciosBaseIzq, comando->segsSuavizadoComando);
		guardarEn->agregarConFormato("%s  <iObj>%d|%d</iObj>\r\n", espaciosBaseIzq, iCapa, iObjDestino);
		guardarEn->agregarConFormato("%s </cmd>\r\n", espaciosBaseIzq);
	}
	guardarEn->agregarConFormato("%s</comandos>\r\n", espaciosBaseIzq);
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return true;
	
}

bool AUEscenaSensor::interpretarXmlInterno(AUEscenaSensor* cargarEn, const AUDatosXML* datosXml, const sNodoXML* nodoXml, SI32 idCapaPorDefecto, AUArregloNativoP<SI32>* arregloIDsCapas, AUArreglo* arreglosObjetosPorCapa, AUArreglo* arregloIDsObjetosPorCapa){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaSensor::interpretarXmlInterno")
	bool exito = false;
	//int versionXML = datosXml->nodoHijo("vXML", 0, nodoXml);
	//if(versionXML==1){
		const sNodoXML* nodoEscenaObj;			XML_NODO_HIJO(datosXml, nodoEscenaObj, "eOb", "escObj", nodoXml, NULL)
		if(nodoEscenaObj != NULL){
			exito = AUEscenaObjeto::interpretarXmlInterno(cargarEn, datosXml, nodoEscenaObj);
			cargarEn->_sensorActivo = datosXml->nodoHijo("senActivo", true, nodoXml);
			if(exito){
				const sNodoXML* nodoComandos	= datosXml->nodoHijo("comandos", nodoXml);
				if(nodoComandos != NULL){
					AUCadenaMutable8* strComando = new(ENMemoriaTipo_Temporal) AUCadenaMutable8();
					const sNodoXML* nodoCmd		= datosXml->nodoHijo("cmd", nodoComandos);
					while(nodoCmd != NULL){
						strComando->vaciar();	datosXml->nodoHijo("strCmd", strComando, "", nodoCmd);
						bool soloUnaVez			= datosXml->nodoHijo("unaVez", true, nodoCmd);
						SI32 evento				= datosXml->nodoHijo("evnt", (SI32)ENSensorEvento_ColisionHeroe, nodoCmd);
						float segsEspera		= datosXml->nodoHijo("segsEsp", 0.0f, nodoCmd);
						SI32 tipoSuavizado		= datosXml->nodoHijo("tipoSuav", 0, nodoCmd);
						float segsSuavizado		= datosXml->nodoHijo("segsSuav", 0.0f, nodoCmd);
						//
						SI32 idCapa				= -1;
						SI32 idObjeto			= -1;
						const sNodoXML* nodoIds = datosXml->nodoHijo("iObj", nodoCmd);
						if(nodoIds != NULL){
							const char* cadenaCSV	= datosXml->cadenaValorDeNodo(nodoIds);
							SI32 tamCadenaCSV		= AUCadena8::tamano(cadenaCSV);
							SI32 posEnCadenaCSV		= 0;
							idCapa					= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, -1);
							idObjeto				= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, -1);
						}
						if(idCapa!=-1 && idObjeto==-1){ idObjeto = idCapa; idCapa = idCapaPorDefecto; } //Compatibilidad con el formato cuando solo se incluia el idObjeto
						AUObjeto* objComando		= NULL;
						SI32 indiceCapa				= arregloIDsCapas->indiceDe(idCapa);
						if(indiceCapa!=-1){
							AUArregloNativoP<AUEscenaObjeto*>* arregloObjetos = (AUArregloNativoP<AUEscenaObjeto*>*)arreglosObjetosPorCapa->elemento[indiceCapa];
							AUArregloNativoP<SI32>* arregloIDsObjetos = (AUArregloNativoP<SI32>*)arregloIDsObjetosPorCapa->elemento[indiceCapa];
							SI32 indiceObjeto		= arregloIDsObjetos->indiceDe(idObjeto);
							if(indiceObjeto!=-1){
								objComando			= arregloObjetos->elemento[indiceObjeto];
							}
						}
						cargarEn->agregarComando(strComando->str(), soloUnaVez, (ENSensorEvento)evento, segsEspera, (ENAnimPropVelocidad)tipoSuavizado, segsSuavizado, objComando);
						//
						nodoCmd		= datosXml->nodoHijo("cmd", nodoComandos, nodoCmd);
					}
					strComando->liberar(NB_RETENEDOR_NULL);
				}
			}
		}
	//}
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return exito;
}

bool AUEscenaSensor::agregarBitsInternosEn(AUEscenaSensor* sensor, AUArchivo* guardarEn, AUArreglo* arregloCapasPorID){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaSensor::agregarBitsInternosEn")
	bool exito = false;
	//
	float versionDatos = 1.0f;
	guardarEn->escribir(&versionDatos, sizeof(versionDatos), 1, guardarEn);
	if(AUEscenaObjeto::agregarBitsInternosEn(sensor, guardarEn)){
		guardarEn->escribir(&sensor->_sensorActivo, sizeof(sensor->_sensorActivo), 1, guardarEn);
		SI32 iCmd, conteoComandos = sensor->_comandosSensor.conteo;
		guardarEn->escribir(&conteoComandos, sizeof(conteoComandos), 1, guardarEn);
		for(iCmd=0; iCmd<conteoComandos; iCmd++){
			STSensorComando* comando	= &(sensor->_comandosSensor.elemento[iCmd]);
			SI32 indiceCapa = -1, indiceObjeto = -1;
			if(comando->objUnido != NULL){
				AUEscenaContenedor* contenedor = (AUEscenaContenedor*)((AUEscenaObjeto*)comando->objUnido)->contenedor(); NBASSERT(comando->objUnido->esClase(AUEscenaObjeto::idTipoClase))
				if(contenedor != NULL){
					indiceCapa = arregloCapasPorID->indiceDe(contenedor);
					if(indiceCapa!=-1){
						indiceObjeto = contenedor->hijos()->indiceDe(comando->objUnido);
					}
				}
			}
			bool ejecutarUnaVez			= comando->ejecutarSoloUnaVez;
			SI32 evento					= comando->eventoParaEjecutar;
			float segsEspera			= comando->segsEsperaParaEjecutar;
			UI8 tipoSuavizado			= comando->tipoSuavizado;
			float segsSuavizado			= comando->segsSuavizadoComando;
			AUCadena8::agregarBitsInternosEn(comando->strComando, guardarEn);
			guardarEn->escribir(&ejecutarUnaVez, sizeof(ejecutarUnaVez), 1, guardarEn);
			guardarEn->escribir(&evento, sizeof(evento), 1, guardarEn);
			guardarEn->escribir(&segsEspera, sizeof(segsEspera), 1, guardarEn);
			guardarEn->escribir(&tipoSuavizado, sizeof(tipoSuavizado), 1, guardarEn);
			guardarEn->escribir(&segsSuavizado, sizeof(segsSuavizado), 1, guardarEn);
			guardarEn->escribir(&indiceCapa, sizeof(indiceCapa), 1, guardarEn);
			guardarEn->escribir(&indiceObjeto, sizeof(indiceObjeto), 1, guardarEn);
		}
		exito = true;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return exito;
}

bool AUEscenaSensor::interpretarBitsInternos(AUEscenaSensor* cargarEn, AUArchivo* cargarDe, AUArreglo* capasCargadas){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaSensor::interpretarBitsInternos")
	bool exito = false;
	float versionDatos;
	cargarDe->leer(&versionDatos, sizeof(versionDatos), 1, cargarDe);
	if(versionDatos==1.0f){
		if(AUEscenaObjeto::interpretarBitsInternos(cargarEn, cargarDe)){
			cargarDe->leer(&cargarEn->_sensorActivo, sizeof(cargarEn->_sensorActivo), 1, cargarDe);
			AUCadenaMutable8* strComando = new(ENMemoriaTipo_Temporal) AUCadenaMutable8();
			SI32 iCmd, conteoComandos; cargarDe->leer(&conteoComandos, sizeof(conteoComandos), 1, cargarDe);
			for(iCmd=0; iCmd<conteoComandos; iCmd++){
				strComando->vaciar(); AUCadenaMutable8::interpretarBitsInternos(strComando, cargarDe);
				bool ejecutarSoloUnaVez;	cargarDe->leer(&ejecutarSoloUnaVez, sizeof(ejecutarSoloUnaVez), 1, cargarDe);
				SI32 evento;				cargarDe->leer(&evento, sizeof(evento), 1, cargarDe);
				float segsEspera;			cargarDe->leer(&segsEspera, sizeof(segsEspera), 1, cargarDe);
				UI8 tipoSuavizado;			cargarDe->leer(&tipoSuavizado, sizeof(tipoSuavizado), 1, cargarDe);
				float segsSuavizado;		cargarDe->leer(&segsSuavizado, sizeof(segsSuavizado), 1, cargarDe);
				SI32 iCapaEnArreglo;		cargarDe->leer(&iCapaEnArreglo, sizeof(iCapaEnArreglo), 1, cargarDe);
				SI32 iObjEnArreglo;			cargarDe->leer(&iObjEnArreglo, sizeof(iObjEnArreglo), 1, cargarDe);
				AUEscenaObjeto* objComando	= NULL;
				if(iCapaEnArreglo>=0 && iCapaEnArreglo<capasCargadas->conteo){
					AUArreglo* hijos = ((AUEscenaContenedor*)capasCargadas->elemento[iCapaEnArreglo])->hijos();
					if(iObjEnArreglo>=0 && iObjEnArreglo<hijos->conteo){
						objComando = (AUEscenaObjeto*) hijos->elemento[iObjEnArreglo];
					}
				}
				cargarEn->agregarComando(strComando->str(), ejecutarSoloUnaVez, (ENSensorEvento)evento, segsEspera, (ENAnimPropVelocidad)tipoSuavizado, segsSuavizado, objComando);
			}
			strComando->liberar(NB_RETENEDOR_NULL);
			exito = true;
		}
	} else {
		PRINTF_ERROR("AUEscenaSensor, la version (%f) de datos binarios no es interpretable por este ejecutable\n", versionDatos);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return exito;
}

//
AUOBJMETODOS_CLASESID_MULTICLASE(AUEscenaSensor, AUEscenaObjeto)
AUOBJMETODOS_CLASESNOMBRES_MULTICLASE(AUEscenaSensor, "AUEscenaSensor", AUEscenaObjeto)
AUOBJMETODOS_CLONAR_THIS(AUEscenaSensor)






