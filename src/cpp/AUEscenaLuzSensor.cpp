
#include "AUFrameworkMediaStdAfx.h"
#include "AUEscenaLuzSensor.h"

//-------------------------------------
//--- CONSTRUCTORES Y DESTRUCTORES ----
//-------------------------------------

AUEscenaLuzSensor::AUEscenaLuzSensor() : AUEscenaObjeto() {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaLuzSensor::AUEscenaLuzSensor")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaLuzSensor")
	_ultColorAcum.r			= 0.0f;
	_ultColorAcum.g			= 0.0f;
	_ultColorAcum.b			= 0.0f;
	_ultColorAcum.a			= 0.0f;
	_ultPromedioIluminado	= 0.0f;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUEscenaLuzSensor::AUEscenaLuzSensor(AUEscenaLuzSensor* otra) : AUEscenaObjeto(otra) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaLuzSensor::AUEscenaLuzSensor")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaLuzSensor")
	NBASSERT(otra != NULL)
	_ultColorAcum.r			= 0.0f;
	_ultColorAcum.g			= 0.0f;
	_ultColorAcum.b			= 0.0f;
	_ultColorAcum.a			= 0.0f;
	_ultPromedioIluminado	= 0.0f;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUEscenaLuzSensor::~AUEscenaLuzSensor(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaLuzSensor::~AUEscenaLuzSensor")
	AU_GESTOR_PILA_LLAMADAS_POP_2
}
 
//-------------------------------
//--- FUNCIONES DE INSTANCIA ----
//-------------------------------

NBColor AUEscenaLuzSensor::ultimoColorPorLuz(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaLuzSensor::ultimoColorPorLuz")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _ultColorAcum;
}

float AUEscenaLuzSensor::ultimoPromedioRGBPorLuz(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaLuzSensor::ultimoPromedioRGBPorLuz")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _ultPromedioIluminado;
}

//

void AUEscenaLuzSensor::consumirLuces(const NBPropIluminacion &propsIluminacion){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaLuzSensor::consumirLuces")
	NBASSERT(idEscena!=-1) //Solo si esta en escena
	if(propsIluminacion.conteoFuentesIluminacion!=0){
		NBPunto posEscena;		NBMATRIZ_MULTIPLICAR_PUNTO(posEscena, _cacheObjEscena.matrizEscena, 0.0f, 0.0f);
		_ultColorAcum			= NBSegmentadorFiguras::calcularColoresAnteLucesConsiderandoSombras(propsIluminacion.fuentesIluminacion, propsIluminacion.sombrasPorFuenteIluminacion, propsIluminacion.conteoFuentesIluminacion, propsIluminacion.sombrasVerticesIntermedios->elemento, this->_agrupadorSombras, posEscena, _cacheObjEscena.cajaEscena);
		NBASSERT(_ultColorAcum.a==1.0f);
		if(_ultColorAcum.r>1.0f) _ultColorAcum.r = 1.0f;
		if(_ultColorAcum.g>1.0f) _ultColorAcum.g = 1.0f;
		if(_ultColorAcum.b>1.0f) _ultColorAcum.b = 1.0f;
		_ultPromedioIluminado	= (_ultColorAcum.r + _ultColorAcum.g + _ultColorAcum.b) / 3.0f;
	} else {
		_ultColorAcum.r			= _ultColorAcum.g = _ultColorAcum.b = _ultPromedioIluminado = 0.0f;
		_ultColorAcum.a			= 1.0f;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

NBCajaAABB AUEscenaLuzSensor::cajaAABBLocalCalculada(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaLuzSensor::cajaAABBLocalCalculada")
	NBCajaAABB cajaLocal;
	cajaLocal.xMin = -8.0f; cajaLocal.yMin = -8.0f;
	cajaLocal.xMax = 8.0f; cajaLocal.yMax = 8.0f;
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return cajaLocal;
}

void AUEscenaLuzSensor::agregadoEnEscena(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaLuzSensor::agregadoEnEscena")
	NBGestorEscena::agregarConsumidorLuces(this->idEscena, this->idCapa, this);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaLuzSensor::quitandoDeEscena(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaLuzSensor::quitandoDeEscena")
	NBGestorEscena::quitarConsumidorLuces(this->idEscena, this->idCapa, this);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

PTRfunCmdsGL AUEscenaLuzSensor::actualizarModeloGL(const NBPropRenderizado &propsRenderizado, const NBPropIluminacion &propsIluminacion, const NBPropHeredadasRender &propsHeredadas){
	return NULL;
}

//
AUOBJMETODOS_CLASESID_MULTICLASE(AUEscenaLuzSensor, AUEscenaObjeto)
AUOBJMETODOS_CLASESNOMBRES_MULTICLASE(AUEscenaLuzSensor, "AUEscenaLuzSensor", AUEscenaObjeto)
AUOBJMETODOS_CLONAR_THIS(AUEscenaLuzSensor)






