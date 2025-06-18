 
#include "AUFrameworkMediaStdAfx.h"
#include "AUEscenaContenedorLimitado.h"

//-------------------------------------
//--- CONSTRUCTORES Y DESTRUCTORES ----
//-------------------------------------

AUEscenaContenedorLimitado::AUEscenaContenedorLimitado() : AUEscenaContenedor() {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaContenedorLimitado::AUEscenaContenedorLimitado")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaContenedorLimitado")
	_limites.xMin = _limites.yMin = 1;
	_limites.xMax = _limites.yMax = 0;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUEscenaContenedorLimitado::AUEscenaContenedorLimitado(AUEscenaContenedorLimitado* otraInstancia) : AUEscenaContenedor(otraInstancia)
	, _limites(otraInstancia->_limites)
{
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaContenedorLimitado::AUEscenaContenedorLimitado")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaContenedorLimitado")
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUEscenaContenedorLimitado::~AUEscenaContenedorLimitado(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaContenedorLimitado::~AUEscenaContenedorLimitado")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

//

NBCajaAABB AUEscenaContenedorLimitado::limites() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaContenedorLimitado::limites")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _limites;
}

void AUEscenaContenedorLimitado::establecerLimites(const float xMin, const float xMax, const float yMin, const float yMax){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaContenedorLimitado::establecerLimites")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	if(_limites.xMin != xMin || _limites.xMax != xMax || _limites.yMin != yMin || _limites.yMax != yMax){
		_limites.xMin = xMin;
		_limites.xMax = xMax;
		_limites.yMin = yMin;
		_limites.yMax = yMax;
		if(this->idEscena >= 0){
			this->setModelChangedFlag();
		}
	}
	NBASSERT(_limites.xMin <= _limites.xMax && _limites.yMin <= _limites.yMax)
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaContenedorLimitado::establecerLimites(const NBCajaAABB limites){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaContenedorLimitado::establecerLimites")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	if(_limites.xMin != limites.xMin || _limites.xMax != limites.xMax || _limites.yMin != limites.yMin || _limites.yMax != limites.yMax){
		_limites = limites;
		if(this->idEscena >= 0){
			this->setModelChangedFlag();
		}
	}
	NBASSERT(_limites.xMin <= _limites.xMax && _limites.yMin <= _limites.yMax)
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

//

NBCajaAABB AUEscenaContenedorLimitado::cajaAABBLocalCalculada(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaContenedorLimitado::cajaAABBLocalCalculada")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	NBCajaAABB r = _limites;
	if(r.xMin > r.xMax || r.yMin > r.yMax){
		r = this->AUEscenaContenedor::cajaAABBLocalCalculada();
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return r;
}

void AUEscenaContenedorLimitado::actualizarArbolMatricesEscena(STNBSceneModelsResult* dst, const NBPropRenderizado &propsRenderizado, const NBPropHeredadasModelos &propsHeredadas){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaContenedorLimitado::actualizarArbolMatricesEscena")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	//Update children
	this->AUEscenaContenedor::actualizarArbolMatricesEscena(dst, propsRenderizado, propsHeredadas);
	//Force scene box (limited, instead of the accumulation of children boxes)
	if(_limites.xMin <= _limites.xMax && _limites.yMin <= _limites.yMax){
		NBMATRIZ_MULTIPLICAR_CAJAAABB(_cacheObjEscena.cajaEscena, _cacheObjEscena.matrizEscena, _limites);
		NBASSERT(!_dormido)
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}
	
//
AUOBJMETODOS_CLASESID_MULTICLASE(AUEscenaContenedorLimitado, AUEscenaContenedor)
AUOBJMETODOS_CLASESNOMBRES_MULTICLASE(AUEscenaContenedorLimitado, "AUEscenaContenedorLimitado", AUEscenaContenedor)
AUOBJMETODOS_CLONAR_THIS(AUEscenaContenedorLimitado)












