
#include "AUFrameworkMediaStdAfx.h"
#include "AUEscenaObjeto.h"
#include "NBMargenes.h"

#define AUOBJETOESCENA_SET_FLAGS_AND_PARENTS_UNTIL_ALREADY_ENABLED(OBJ_PTR, BITS_OBJ, BITS_PARENTS) \
	{ \
		AUEscenaObjeto* p; \
		(OBJ_PTR)->_sceneMask |= (BITS_OBJ); \
		p = (OBJ_PTR)->_contenedor; \
		while(p != NULL){ \
			/*Stop if parent already has this bits enabled*/ \
			if((p->_sceneMask & (BITS_PARENTS)) == (BITS_PARENTS)) break; \
			p->_sceneMask |= (BITS_PARENTS); \
			p = p->_contenedor; \
		} \
	}

//-------------------------------------

AUEscenaObjeto::AUEscenaObjeto() : AUObjeto(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaObjeto::AUEscenaObjeto")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaObjeto")
	privInlineInicializarVariables();
	privInlineInicializarPropiedades(0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f);
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUEscenaObjeto::AUEscenaObjeto(AUEscenaObjeto* otro) : AUObjeto() {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaObjeto::AUEscenaObjeto")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaObjeto")
	privInlineInicializarVariables();
	_propiedades				= otro->_propiedades;
	_iluminoDependiente			= false; //otro->_iluminoDependiente;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaObjeto::privInlineInicializarVariables(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaObjeto::privInlineInicializarVariables")
	_contenedor						= NULL;
	_agrupadorSombras				= this;	//Inicialmente, el agrupador es si mismo.
	_idMundoFisica					= -1;
	idEscena						= -1; NBASSERT(idEscena==-1) //en ARM char es unsigned, en android se resuelve compilando con "LOCAL_CFLAGS := -fsigned-char"
	idGrupo							= 0;
	idCapa							= -1;
	//Touches
	{
		_touches.count				= 0;
		NBMemory_setZero(_touches.itf);
	}
	_contadorCafeina				= 0;
	_escuchadorTouchObjeto			= NULL;
	_escuchadorTouchInterfaz		= NULL;
	_touchFilterMask				= AUOBJETOESCENA_TOUCH_BIT_LISTEN_TOUCH;
	_dormido						= false;
	_iluminoDependiente				= false; //true;
	_sceneMask						= 0;
#	ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
	_debugBloqueadoActualizandoModelos = false;
	_debugAgregadoEscena			= false;
	_debugAgregadoFisica			= false;
#	endif
	//
#	ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
	_cacheObjEscena.debugCacheActualizada = false;
#	endif
	NBMATRIZ_ESTABLECER_IDENTIDAD(_cacheObjEscena.matrizEscena);
	NBCAJAAABB_INICIALIZAR(_cacheObjEscena.cajaEscena);
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaObjeto::privInlineInicializarPropiedades(float traslacionX, float traslacionY, float escalacionAncho, float escalacionAlto, float rotacion, float multR, float multG, float multB, float multA){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaObjeto::privInlineInicializarPropiedades")
	_visible					            = true;
	_propiedades.transformaciones.escalaX	= escalacionAncho;
	_propiedades.transformaciones.escalaY	= escalacionAlto;
	_propiedades.transformaciones.rotacion	= rotacion;
	_propiedades.transformaciones.trasladoX	= traslacionX;
	_propiedades.transformaciones.trasladoY	= traslacionY;
	_propiedades.color8.r					= 255*multR;
	_propiedades.color8.g					= 255*multG;
	_propiedades.color8.b					= 255*multB;
	_propiedades.color8.a					= 255*multA;
	_sceneMask								= 0;
	//
	NBASSERT(_propiedades.transformaciones.escalaX != 0.0f && _propiedades.transformaciones.escalaY != 0.0f) //Valor corrupto???
	NBASSERT(_propiedades.transformaciones.trasladoX > -1000000.0f && _propiedades.transformaciones.trasladoX < 1000000.0f) //Valor corrupto???
	NBASSERT(_propiedades.transformaciones.trasladoY > -1000000.0f && _propiedades.transformaciones.trasladoY < 1000000.0f) //Valor corrupto???
	NBASSERT(_propiedades.transformaciones.escalaX == _propiedades.transformaciones.escalaX && _propiedades.transformaciones.escalaY == _propiedades.transformaciones.escalaY) //NaN???
	NBASSERT(_propiedades.transformaciones.trasladoX == _propiedades.transformaciones.trasladoX && _propiedades.transformaciones.trasladoY == _propiedades.transformaciones.trasladoY) //NaN???
	NBASSERT(_propiedades.transformaciones.rotacion == _propiedades.transformaciones.rotacion) //NaN???
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUEscenaObjeto::~AUEscenaObjeto(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaObjeto::~AUEscenaObjeto")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	NBASSERT(idEscena==-1)
	NBASSERT(!_debugBloqueadoActualizandoModelos)
	NBASSERT(_touches.count == 0) //No touch should be owned
	_contenedor				= NULL;
	_agrupadorSombras		= NULL;
	_escuchadorTouchObjeto	= NULL;
	_escuchadorTouchInterfaz= NULL;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
void AUEscenaObjeto::debugEstablecerBloqueadoActualizandoModelos(const bool bloqueado){
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	_debugBloqueadoActualizandoModelos = bloqueado;
}
#endif

//---------------------------------
//--- Medicion de "cafeina"
//--- es un contador de retenciones/referencias
//--- para determinar si es posible dormir al AUEscenaObjeto
//---------------------------------

UI8 AUEscenaObjeto::conteoCafeina() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaObjeto::conteoCafeina")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _contadorCafeina;
}

void AUEscenaObjeto::cafeinizar(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaObjeto::cafeinizar")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	_contadorCafeina++;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaObjeto::decafeinizar(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaObjeto::decafeinizar")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	NBASSERT(_contadorCafeina != 0)
	_contadorCafeina--;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

//Scene state mask

void AUEscenaObjeto::enableSceneMask(const UI16 bits){ //AUOBJETOESCENA_BIT_*
	this->_sceneMask |= bits;
}

void AUEscenaObjeto::enableSceneMaskAndParentsUntilEnabled(const UI16 bits, const UI16 bitsParent){ //AUOBJETOESCENA_BIT_*
	if(this->idEscena >= 0){
		AUOBJETOESCENA_SET_FLAGS_AND_PARENTS_UNTIL_ALREADY_ENABLED(this, bits, bitsParent);
	}
}

void AUEscenaObjeto::setModelChangedFlag(){
	if(this->idEscena >= 0){
		AUOBJETOESCENA_SET_FLAGS_AND_PARENTS_UNTIL_ALREADY_ENABLED(this, AUOBJETOESCENA_BIT_MODEL_LOCAL_CHANGED, AUOBJETOESCENA_BIT_MODEL_CHILD_CHANGED)
	}
}

//-------------------------------
//--- FUNCIONES DE INSTANCIA ----
//-------------------------------

void AUEscenaObjeto::copiarPropiedadesDe(AUEscenaObjeto* otroObjeto){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaObjeto::copiarPropiedadesDe")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	AU_OBJETO_ASSERT_IS_VALID(otroObjeto)
	//Analyze and notify changes, only if already at scene
	if(idEscena >= 0){
		UI16 myBitsChange = 0, parentBitsChange = 0;
		if(*((UI32*)&_propiedades.color8) != *((UI32*)&otroObjeto->_propiedades.color8)
		   || _visible != otroObjeto->_visible
		   || _iluminoDependiente != false /*otroObjeto->_iluminoDependiente*/){
			myBitsChange		|= AUOBJETOESCENA_BIT_MODEL_LOCAL_CHANGED;
			parentBitsChange	|= AUOBJETOESCENA_BIT_MODEL_CHILD_CHANGED;
		}
		if(_propiedades.transformaciones.escalaX != otroObjeto->_propiedades.transformaciones.escalaX
		   || _propiedades.transformaciones.escalaY != otroObjeto->_propiedades.transformaciones.escalaY
		   || _propiedades.transformaciones.trasladoX != otroObjeto->_propiedades.transformaciones.trasladoX
		   || _propiedades.transformaciones.trasladoY != otroObjeto->_propiedades.transformaciones.trasladoY
		   || _propiedades.transformaciones.rotacion != otroObjeto->_propiedades.transformaciones.rotacion){
			myBitsChange		|= AUOBJETOESCENA_BIT_MATRIX_LOCAL_CHANGED;
			parentBitsChange	|= AUOBJETOESCENA_BIT_MATRIX_CHILD_CHANGED;
		}
		if((myBitsChange | parentBitsChange) != 0){
			AUOBJETOESCENA_SET_FLAGS_AND_PARENTS_UNTIL_ALREADY_ENABLED(this, myBitsChange, parentBitsChange)
		}
	}
	//Set props
	_propiedades				= otroObjeto->_propiedades;
	_iluminoDependiente			= false; //otroObjeto->_iluminoDependiente;
	//
	NBASSERT(_propiedades.transformaciones.escalaX != 0.0f && _propiedades.transformaciones.escalaY != 0.0f) //Valor corrupto???
	NBASSERT(_propiedades.transformaciones.trasladoX > -1000000.0f && _propiedades.transformaciones.trasladoX < 1000000.0f) //Valor corrupto???
	NBASSERT(_propiedades.transformaciones.trasladoY > -1000000.0f && _propiedades.transformaciones.trasladoY < 1000000.0f) //Valor corrupto???
	NBASSERT(_propiedades.transformaciones.escalaX == _propiedades.transformaciones.escalaX && _propiedades.transformaciones.escalaY == _propiedades.transformaciones.escalaY) //NaN???
	NBASSERT(_propiedades.transformaciones.trasladoX == _propiedades.transformaciones.trasladoX && _propiedades.transformaciones.trasladoY == _propiedades.transformaciones.trasladoY) //NaN???
	NBASSERT(_propiedades.transformaciones.rotacion == _propiedades.transformaciones.rotacion) //NaN???
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaObjeto::copiarTransformacionesDe(AUEscenaObjeto* otroObjeto){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaObjeto::copiarTransformacionesDe")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	AU_OBJETO_ASSERT_IS_VALID(otroObjeto)
	//Analyze and notify changes, only if already at scene
	if(idEscena >= 0){
		if(_propiedades.transformaciones.escalaX != otroObjeto->_propiedades.transformaciones.escalaX
		   || _propiedades.transformaciones.escalaY != otroObjeto->_propiedades.transformaciones.escalaY
		   || _propiedades.transformaciones.trasladoX != otroObjeto->_propiedades.transformaciones.trasladoX
		   || _propiedades.transformaciones.trasladoY != otroObjeto->_propiedades.transformaciones.trasladoY
		   || _propiedades.transformaciones.rotacion != otroObjeto->_propiedades.transformaciones.rotacion){
			AUOBJETOESCENA_SET_FLAGS_AND_PARENTS_UNTIL_ALREADY_ENABLED(this, AUOBJETOESCENA_BIT_MATRIX_LOCAL_CHANGED, AUOBJETOESCENA_BIT_MATRIX_CHILD_CHANGED)
		}
	}
	//Set props
	_propiedades.transformaciones	= otroObjeto->_propiedades.transformaciones;
	//
	NBASSERT(_propiedades.transformaciones.escalaX != 0.0f && _propiedades.transformaciones.escalaY != 0.0f) //Valor corrupto???
	NBASSERT(_propiedades.transformaciones.trasladoX > -1000000.0f && _propiedades.transformaciones.trasladoX < 1000000.0f) //Valor corrupto???
	NBASSERT(_propiedades.transformaciones.trasladoY > -1000000.0f && _propiedades.transformaciones.trasladoY < 1000000.0f) //Valor corrupto???
	NBASSERT(_propiedades.transformaciones.escalaX == _propiedades.transformaciones.escalaX && _propiedades.transformaciones.escalaY == _propiedades.transformaciones.escalaY) //NaN???
	NBASSERT(_propiedades.transformaciones.trasladoX == _propiedades.transformaciones.trasladoX && _propiedades.transformaciones.trasladoY == _propiedades.transformaciones.trasladoY) //NaN???
	NBASSERT(_propiedades.transformaciones.rotacion == _propiedades.transformaciones.rotacion) //NaN???
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

NBMatriz AUEscenaObjeto::matrizLocal(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaObjeto::matrizLocal")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	NBMatriz matrizLocal; NBMATRIZ_ESTABLECER_SEGUN_TRANSFORMACIONES_ESCENA_OBJETO(matrizLocal, _propiedades.transformaciones)
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return matrizLocal;
}

NBMatriz AUEscenaObjeto::matrizEscena(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaObjeto::matrizEscena")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	NBASSERT(idEscena!=-1)
	//NBASSERT(NBMATRIZ_ELEM00(_cacheObjEscena.matrizEscena)==NBMATRIZ_ELEM00(_cacheObjEscena.matrizEscena)); //Si falla, es Nan
	//NBASSERT(NBMATRIZ_ELEM01(_cacheObjEscena.matrizEscena)==NBMATRIZ_ELEM01(_cacheObjEscena.matrizEscena)); //Si falla, es Nan
	//NBASSERT(NBMATRIZ_ELEM02(_cacheObjEscena.matrizEscena)==NBMATRIZ_ELEM02(_cacheObjEscena.matrizEscena)); //Si falla, es Nan
	//NBASSERT(NBMATRIZ_ELEM10(_cacheObjEscena.matrizEscena)==NBMATRIZ_ELEM10(_cacheObjEscena.matrizEscena)); //Si falla, es Nan
	//NBASSERT(NBMATRIZ_ELEM11(_cacheObjEscena.matrizEscena)==NBMATRIZ_ELEM11(_cacheObjEscena.matrizEscena)); //Si falla, es Nan
	//NBASSERT(NBMATRIZ_ELEM12(_cacheObjEscena.matrizEscena)==NBMATRIZ_ELEM12(_cacheObjEscena.matrizEscena)); //Si falla, es Nan
	//NBASSERT(NBMATRIZ_ELEM20(_cacheObjEscena.matrizEscena)==NBMATRIZ_ELEM20(_cacheObjEscena.matrizEscena)); //Si falla, es Nan
	//NBASSERT(NBMATRIZ_ELEM21(_cacheObjEscena.matrizEscena)==NBMATRIZ_ELEM21(_cacheObjEscena.matrizEscena)); //Si falla, es Nan
	//NBASSERT(NBMATRIZ_ELEM22(_cacheObjEscena.matrizEscena)==NBMATRIZ_ELEM22(_cacheObjEscena.matrizEscena)); //Si falla, es Nan
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _cacheObjEscena.matrizEscena;
}

NBMatriz AUEscenaObjeto::matrizEscenaCalculada(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaObjeto::matrizEscenaCalculada")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	NBMatriz matrizInversa = matrizEscenaInversaCalculada();
	NBMatriz matrizEscena; NBMATRIZ_INVERSA(matrizEscena, matrizInversa)
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return matrizEscena;
}

NBMatriz AUEscenaObjeto::matrizEscenaInversaCalculada(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaObjeto::matrizEscenaInversaCalculada")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	NBASSERT(idEscena != -1)
	NBMatriz _matrizEscenaInversa;
	NBMATRIZ_ESTABLECER_IDENTIDAD(_matrizEscenaInversa);
	AUEscenaObjeto* padre					= this;
	NBTransformacionesF* transformaciones	= &_propiedades.transformaciones;
	NBTamano escalaInversa; float radianesInversa; NBPunto trasladoInversa;
	do {
		NBASSERT(transformaciones->escalaX != 0.0f)
		NBASSERT(transformaciones->escalaY != 0.0f)
		NBTAMANO_ESTABLECER(escalaInversa, 1.0f/transformaciones->escalaX, 1.0f/transformaciones->escalaY)
		NBPUNTO_ESTABLECER(trasladoInversa, -transformaciones->trasladoX, -transformaciones->trasladoY)
		NBMATRIZ_ESCALAR(_matrizEscenaInversa, escalaInversa.ancho, escalaInversa.alto);
		if(transformaciones->rotacion != 0.0f){
			radianesInversa = GRADOS_A_RADIANES(-transformaciones->rotacion);
			NBMATRIZ_ROTAR_RADIANES(_matrizEscenaInversa, radianesInversa)
		}
		NBMATRIZ_TRASLADAR(_matrizEscenaInversa, trasladoInversa.x, trasladoInversa.y);
		padre				= padre->_contenedor; if(padre != NULL) transformaciones = &(padre->_propiedades.transformaciones);
	} while(padre != NULL);
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _matrizEscenaInversa;
}

NBCajaAABB AUEscenaObjeto::cajaAABBEnEscena(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaObjeto::cajaAABBEnEscena")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	NBASSERT(idEscena != -1)
	//NBASSERT(_cacheObjEscena.debugCacheActualizada) //Comentariado temporalmente, descomentariar
	//NBASSERT(_cacheCajaEscena.xMin == _cacheCajaEscena.xMin); //Si falla, es Nan
	//NBASSERT(_cacheCajaEscena.xMax == _cacheCajaEscena.xMax); //Si falla, es Nan
	//NBASSERT(_cacheCajaEscena.yMin == _cacheCajaEscena.yMin); //Si falla, es Nan
	//NBASSERT(_cacheCajaEscena.yMax == _cacheCajaEscena.yMax); //Si falla, es Nan
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _cacheObjEscena.cajaEscena;
}

NBCajaAABB AUEscenaObjeto::cajaAABBLocal(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaObjeto::cajaAABBLocal")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	NBCajaAABB r = cajaAABBLocalCalculada();
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return r;
}

/*
NBPropiedadesEnEscena AUEscenaObjeto::propiedades(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaObjeto::propiedades")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _propiedades;
}*/

bool AUEscenaObjeto::dormido(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaObjeto::dormido")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _dormido;
}

bool AUEscenaObjeto::visible(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaObjeto::visible")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _visible;
}

bool AUEscenaObjeto::iluminoDependiente(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaObjeto::iluminoDependiente")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _iluminoDependiente;
}

bool AUEscenaObjeto::esContenedor(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaObjeto::esContenedor")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return ((_sceneMask & AUOBJETOESCENA_BIT_ESCONTENEDOR) != 0);
}

NBPunto AUEscenaObjeto::traslacion(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaObjeto::traslacion")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	NBPunto traslacion; traslacion.x = _propiedades.transformaciones.trasladoX; traslacion.y = _propiedades.transformaciones.trasladoY;
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return traslacion;
}

NBTamano AUEscenaObjeto::escalacion(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaObjeto::escalacion")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	NBTamano escala; escala.ancho = _propiedades.transformaciones.escalaX; escala.alto = _propiedades.transformaciones.escalaY;
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return escala;
}

float AUEscenaObjeto::rotacion(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaObjeto::rotacion")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _propiedades.transformaciones.rotacion;
}

//Scrolls

BOOL AUEscenaObjeto::getTouchScrollEnabled() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaObjeto::getTouchScrollEnabled")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return ((_touchFilterMask & AUOBJETOESCENA_TOUCH_BIT_LISTEN_SCROLL) != 0);
}

void AUEscenaObjeto::setTouchScrollEnabled(const BOOL enabled){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaObjeto::setTouchScrollEnabled")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	if(enabled){
		_touchFilterMask |= AUOBJETOESCENA_TOUCH_BIT_LISTEN_SCROLL;
	} else {
		_touchFilterMask &= ~AUOBJETOESCENA_TOUCH_BIT_LISTEN_SCROLL;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

//Magnify

BOOL AUEscenaObjeto::getTouchMagnifyEnabled() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaObjeto::getTouchMagnifyEnabled")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return ((_touchFilterMask & AUOBJETOESCENA_TOUCH_BIT_LISTEN_MAGNIFY) != 0);
}

void AUEscenaObjeto::setTouchMagnifyEnabled(const BOOL enabled){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaObjeto::setTouchMagnifyEnabled")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	if(enabled){
		_touchFilterMask |= AUOBJETOESCENA_TOUCH_BIT_LISTEN_MAGNIFY;
	} else {
		_touchFilterMask &= ~AUOBJETOESCENA_TOUCH_BIT_LISTEN_MAGNIFY;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

//

//Optimizacion: usar la variable publica "multiplicadorColor" en lugar de este metodo "multiplicadorColor()"
//Alta demanda de este valor durante ejecucion.
NBColor8 AUEscenaObjeto::multiplicadorColor8Func(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaObjeto::multiplicadorColorFunc")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _propiedades.color8;
}

UI8 AUEscenaObjeto::multiplicadorAlpha8() {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaObjeto::multiplicadorAlpha8")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _propiedades.color8.a;
}

void AUEscenaObjeto::establecerPropiedades(const bool pVisible, const NBPropiedadesEnEscena &nuevasPropiedades){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaObjeto::establecerPropiedades")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	//Analyze and notify changes, only if already at scene
	if(idEscena >= 0){
		UI16 myBitsChange = 0, parentBitsChange = 0;
		if(*((UI32*)&_propiedades.color8) != *((UI32*)&nuevasPropiedades.color8) || _visible != pVisible){
			myBitsChange		|= AUOBJETOESCENA_BIT_MODEL_LOCAL_CHANGED;
			parentBitsChange	|= AUOBJETOESCENA_BIT_MODEL_CHILD_CHANGED;
		}
		if(_propiedades.transformaciones.escalaX != nuevasPropiedades.transformaciones.escalaX
		   || _propiedades.transformaciones.escalaY != nuevasPropiedades.transformaciones.escalaY
		   || _propiedades.transformaciones.trasladoX != nuevasPropiedades.transformaciones.trasladoX
		   || _propiedades.transformaciones.trasladoY != nuevasPropiedades.transformaciones.trasladoY
		   || _propiedades.transformaciones.rotacion != nuevasPropiedades.transformaciones.rotacion){
			myBitsChange		|= AUOBJETOESCENA_BIT_MATRIX_LOCAL_CHANGED;
			parentBitsChange	|= AUOBJETOESCENA_BIT_MATRIX_CHILD_CHANGED;
		}
		if((myBitsChange | parentBitsChange) != 0){
			AUOBJETOESCENA_SET_FLAGS_AND_PARENTS_UNTIL_ALREADY_ENABLED(this, myBitsChange, parentBitsChange)
		}
	}
	//Set props
    _visible = pVisible;
	_propiedades = nuevasPropiedades;
	//
	NBASSERT(_propiedades.transformaciones.escalaX != 0.0f && _propiedades.transformaciones.escalaY != 0.0f)
	//
	NBASSERT(_propiedades.transformaciones.escalaX == _propiedades.transformaciones.escalaX && _propiedades.transformaciones.escalaY == _propiedades.transformaciones.escalaY) //NaN???
	NBASSERT(_propiedades.transformaciones.trasladoX == _propiedades.transformaciones.trasladoX && _propiedades.transformaciones.trasladoY == _propiedades.transformaciones.trasladoY) //NaN???
	NBASSERT(_propiedades.transformaciones.rotacion == _propiedades.transformaciones.rotacion) //NaN???
	//
	//NBASSERT(_propiedades.transformaciones.trasladoX > -1000000 && _propiedades.transformaciones.trasladoX < 1000000) //Valor corrupto???
	//NBASSERT(_propiedades.transformaciones.trasladoY > -1000000 && _propiedades.transformaciones.trasladoY < 1000000) //Valor corrupto???
	//NBASSERT(_propiedades.transformaciones.rotacion > -1000000 && _propiedades.transformaciones.rotacion < 1000000) //Valor corrupto???
	//NBASSERT(_propiedades.transformaciones.escalaX != 0.0f && _propiedades.transformaciones.escalaY != 0.0f) //Valor corrupto???
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaObjeto::setVisibleAndAwake(const BOOL visibleAndAwake){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaObjeto::setVisibleAndAwake")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	//Analyze and notify changes, only if already at scene
	if(idEscena >= 0){
		UI16 myBitsChange = 0, parentBitsChange = 0;
		if(_visible != visibleAndAwake){
			myBitsChange		|= AUOBJETOESCENA_BIT_MODEL_LOCAL_CHANGED;
			parentBitsChange	|= AUOBJETOESCENA_BIT_MODEL_CHILD_CHANGED;
		}
		if(_dormido != !visibleAndAwake){
			//Force a matrix update (if was sleeping, was never updated)
			myBitsChange		|= AUOBJETOESCENA_BIT_MATRIX_LOCAL_CHANGED;
			parentBitsChange	|= AUOBJETOESCENA_BIT_MATRIX_CHILD_CHANGED;
		}
		if((myBitsChange | parentBitsChange) != 0){
			AUOBJETOESCENA_SET_FLAGS_AND_PARENTS_UNTIL_ALREADY_ENABLED(this, myBitsChange, parentBitsChange)
		}
	}
	//Set props
	_visible	= visibleAndAwake;
	_dormido	= !visibleAndAwake;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaObjeto::establecerDormido(bool dormido){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaObjeto::establecerDormido")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	//Analyze and notify changes, only if already at scene
	if(idEscena >= 0){
		if(_dormido != dormido){
			//Force a matrix update (if was sleeping, was never updated)
			AUOBJETOESCENA_SET_FLAGS_AND_PARENTS_UNTIL_ALREADY_ENABLED(this, AUOBJETOESCENA_BIT_MATRIX_LOCAL_CHANGED, AUOBJETOESCENA_BIT_MATRIX_CHILD_CHANGED)
		}
	}
	//Set props
	_dormido = dormido;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaObjeto::establecerVisible(bool visible){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaObjeto::establecerVisible")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	//Analyze and notify changes, only if already at scene
	if(idEscena >= 0){
		if(_visible != visible){
			AUOBJETOESCENA_SET_FLAGS_AND_PARENTS_UNTIL_ALREADY_ENABLED(this, AUOBJETOESCENA_BIT_MODEL_LOCAL_CHANGED, AUOBJETOESCENA_BIT_MODEL_CHILD_CHANGED)
		}
	}
	//Set props
	_visible = visible;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaObjeto::establecerTraslacion(const NBPunto &nuevaTraslacion){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaObjeto::establecerTraslacion")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	NBASSERT(nuevaTraslacion.x == nuevaTraslacion.x && nuevaTraslacion.y == nuevaTraslacion.y); //Si falla es Nan
	//NBASSERT(nuevaTraslacion.x > -1000000 && nuevaTraslacion.x < 1000000) //Valor corrupto???
	//NBASSERT(nuevaTraslacion.y > -1000000 && nuevaTraslacion.y < 1000000) //Valor corrupto???
	//Analyze and notify changes, only if already at scene
	if(idEscena >= 0){
		if(_propiedades.transformaciones.trasladoX != nuevaTraslacion.x || _propiedades.transformaciones.trasladoY != nuevaTraslacion.y){
			AUOBJETOESCENA_SET_FLAGS_AND_PARENTS_UNTIL_ALREADY_ENABLED(this, AUOBJETOESCENA_BIT_MATRIX_LOCAL_CHANGED, AUOBJETOESCENA_BIT_MATRIX_CHILD_CHANGED)
		}
	}
	//Set props
	_propiedades.transformaciones.trasladoX = nuevaTraslacion.x;
	_propiedades.transformaciones.trasladoY = nuevaTraslacion.y;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaObjeto::establecerTraslacion(float nuevaTraslacionX, float nuevaTraslacionY){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaObjeto::establecerTraslacion")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	NBASSERT(nuevaTraslacionX == nuevaTraslacionX && nuevaTraslacionY == nuevaTraslacionY); //Si falla es Nan
	//NBASSERT(nuevaTraslacionX > -1000000 && nuevaTraslacionX < 1000000) //Valor corrupto???
	//NBASSERT(nuevaTraslacionY > -1000000 && nuevaTraslacionY < 1000000) //Valor corrupto???
	//Analyze and notify changes, only if already at scene
	if(idEscena >= 0){
		if(_propiedades.transformaciones.trasladoX != nuevaTraslacionX || _propiedades.transformaciones.trasladoY != nuevaTraslacionY){
			AUOBJETOESCENA_SET_FLAGS_AND_PARENTS_UNTIL_ALREADY_ENABLED(this, AUOBJETOESCENA_BIT_MATRIX_LOCAL_CHANGED, AUOBJETOESCENA_BIT_MATRIX_CHILD_CHANGED)
		}
	}
	//Set props
	_propiedades.transformaciones.trasladoX = nuevaTraslacionX;
	_propiedades.transformaciones.trasladoY = nuevaTraslacionY;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaObjeto::establecerEscalacion(const NBTamano &nuevoEscalacion){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaObjeto::establecerEscalacion")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	NBASSERT(nuevoEscalacion.ancho==nuevoEscalacion.ancho && nuevoEscalacion.alto==nuevoEscalacion.alto); //Si falla es Nan
	NBASSERT(nuevoEscalacion.ancho != 0.0f && nuevoEscalacion.alto != 0.0f) //Valor corrupto???
	//Analyze and notify changes, only if already at scene
	if(idEscena >= 0){
		if(_propiedades.transformaciones.escalaX != nuevoEscalacion.ancho || _propiedades.transformaciones.escalaY != nuevoEscalacion.alto){
			AUOBJETOESCENA_SET_FLAGS_AND_PARENTS_UNTIL_ALREADY_ENABLED(this, AUOBJETOESCENA_BIT_MATRIX_LOCAL_CHANGED, AUOBJETOESCENA_BIT_MATRIX_CHILD_CHANGED)
		}
	}
	//Set props
	_propiedades.transformaciones.escalaX	= nuevoEscalacion.ancho;
	_propiedades.transformaciones.escalaY	= nuevoEscalacion.alto;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaObjeto::establecerEscalacion(const float nuevoEscalacion){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaObjeto::establecerEscalacion")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	NBASSERT(nuevoEscalacion == nuevoEscalacion); //Si falla es Nan
	NBASSERT(nuevoEscalacion != 0.0f) //Valor corrupto???
	//Analyze and notify changes, only if already at scene
	if(idEscena >= 0){
		if(_propiedades.transformaciones.escalaX != nuevoEscalacion || _propiedades.transformaciones.escalaY != nuevoEscalacion){
			AUOBJETOESCENA_SET_FLAGS_AND_PARENTS_UNTIL_ALREADY_ENABLED(this, AUOBJETOESCENA_BIT_MATRIX_LOCAL_CHANGED, AUOBJETOESCENA_BIT_MATRIX_CHILD_CHANGED)
		}
	}
	//Set props
	_propiedades.transformaciones.escalaX	= nuevoEscalacion;
	_propiedades.transformaciones.escalaY	= nuevoEscalacion;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaObjeto::establecerEscalacion(const float nuevoEscalacionAncho, const float nuevoEscalacionAlto){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaObjeto::establecerEscalacion")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	NBASSERT(nuevoEscalacionAncho==nuevoEscalacionAncho && nuevoEscalacionAlto==nuevoEscalacionAlto); //Si falla es Nan
	NBASSERT(nuevoEscalacionAncho != 0.0f && nuevoEscalacionAlto != 0.0f) //Valor corrupto???
	//Analyze and notify changes, only if already at scene
	if(idEscena >= 0){
		if(_propiedades.transformaciones.escalaX != nuevoEscalacionAncho || _propiedades.transformaciones.escalaY != nuevoEscalacionAlto){
			AUOBJETOESCENA_SET_FLAGS_AND_PARENTS_UNTIL_ALREADY_ENABLED(this, AUOBJETOESCENA_BIT_MATRIX_LOCAL_CHANGED, AUOBJETOESCENA_BIT_MATRIX_CHILD_CHANGED)
		}
	}
	//Set props
	_propiedades.transformaciones.escalaX	= nuevoEscalacionAncho;
	_propiedades.transformaciones.escalaY	= nuevoEscalacionAlto;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaObjeto::establecerRotacion(float nuevaRotacion){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaObjeto::establecerRotacion")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	NBASSERT(nuevaRotacion == nuevaRotacion); //Si falla es Nan
	//NBASSERT(nuevaRotacion > -1000000 && nuevaRotacion < 1000000) //Valor corrupto???
	//Analyze and notify changes, only if already at scene
	if(idEscena >= 0){
		if(_propiedades.transformaciones.rotacion != nuevaRotacion){
			AUOBJETOESCENA_SET_FLAGS_AND_PARENTS_UNTIL_ALREADY_ENABLED(this, AUOBJETOESCENA_BIT_MATRIX_LOCAL_CHANGED, AUOBJETOESCENA_BIT_MATRIX_CHILD_CHANGED)
		}
	}
	//Set props
	_propiedades.transformaciones.rotacion	= nuevaRotacion;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaObjeto::establecerRotacionNormalizada(float pNewRot){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaObjeto::establecerRotacion")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	NBASSERT(pNewRot == pNewRot); //Si falla es Nan
	//NBASSERT(nuevaRotacion > -1000000 && nuevaRotacion < 1000000) //Valor corrupto???
	float newRot = pNewRot;
	while(newRot >= 360.0f) newRot -= 360.0f;
	while(newRot < 360.0f) newRot += 360.0f;
	//Analyze and notify changes, only if already at scene
	if(idEscena >= 0){
		if(_propiedades.transformaciones.rotacion != newRot){
			AUOBJETOESCENA_SET_FLAGS_AND_PARENTS_UNTIL_ALREADY_ENABLED(this, AUOBJETOESCENA_BIT_MATRIX_LOCAL_CHANGED, AUOBJETOESCENA_BIT_MATRIX_CHILD_CHANGED)
		}
	}
	//Set props
	_propiedades.transformaciones.rotacion	= newRot;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaObjeto::establecerMultiplicadorColor(const NBColor &multipplicadorColor){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaObjeto::establecerMultiplicadorColor")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	NBASSERT(multipplicadorColor.r == multipplicadorColor.r && multipplicadorColor.g == multipplicadorColor.g && multipplicadorColor.b == multipplicadorColor.b && multipplicadorColor.a == multipplicadorColor.a) //Si falla es Nan
	NBColor8 newColor8;
	newColor8.r	= 255.0f * multipplicadorColor.r;
	newColor8.g	= 255.0f * multipplicadorColor.g;
	newColor8.b	= 255.0f * multipplicadorColor.b;
	newColor8.a	= 255.0f * multipplicadorColor.a;
	//Analyze and notify changes, only if already at scene
	if(idEscena >= 0){
		if(*((UI32*)&_propiedades.color8) != *((UI32*)&newColor8)){
			AUOBJETOESCENA_SET_FLAGS_AND_PARENTS_UNTIL_ALREADY_ENABLED(this, AUOBJETOESCENA_BIT_MODEL_LOCAL_CHANGED, AUOBJETOESCENA_BIT_MODEL_CHILD_CHANGED)
		}
	}
	//Set props
	_propiedades.color8	= newColor8;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaObjeto::establecerMultiplicadorColor(float r, float g, float b, float a){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaObjeto::establecerMultiplicadorColor")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	NBASSERT(r == r && g == g && b == b && a == a); //Si falla es Nan
	NBColor8 newColor8;
	newColor8.r	= 255.0f * r;
	newColor8.g	= 255.0f * g;
	newColor8.b	= 255.0f * b;
	newColor8.a	= 255.0f * a;
	//Analyze and notify changes, only if already at scene
	if(idEscena >= 0){
		if(*((UI32*)&_propiedades.color8) != *((UI32*)&newColor8)){
			AUOBJETOESCENA_SET_FLAGS_AND_PARENTS_UNTIL_ALREADY_ENABLED(this, AUOBJETOESCENA_BIT_MODEL_LOCAL_CHANGED, AUOBJETOESCENA_BIT_MODEL_CHILD_CHANGED)
		}
	}
	//Set props
	_propiedades.color8 = newColor8;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaObjeto::establecerMultiplicadorColor8(const NBColor8 &multipplicadorColor){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaObjeto::establecerMultiplicadorColor8")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	NBASSERT(multipplicadorColor.r == multipplicadorColor.r && multipplicadorColor.g == multipplicadorColor.g && multipplicadorColor.b == multipplicadorColor.b && multipplicadorColor.a == multipplicadorColor.a) //Si falla es Nan
	//Analyze and notify changes, only if already at scene
	if(idEscena >= 0){
		if(*((UI32*)&_propiedades.color8) != *((UI32*)&multipplicadorColor)){
			AUOBJETOESCENA_SET_FLAGS_AND_PARENTS_UNTIL_ALREADY_ENABLED(this, AUOBJETOESCENA_BIT_MODEL_LOCAL_CHANGED, AUOBJETOESCENA_BIT_MODEL_CHILD_CHANGED)
		}
	}
	//Set props
	_propiedades.color8	= multipplicadorColor;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaObjeto::establecerMultiplicadorColor8(UI8 r, UI8 g, UI8 b, UI8 a){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaObjeto::establecerMultiplicadorColor8")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	NBASSERT(r == r && g == g && b == b && a == a); //Si falla es Nan
	//Analyze and notify changes, only if already at scene
	if(idEscena >= 0){
		if(_propiedades.color8.r != r || _propiedades.color8.g != g || _propiedades.color8.b != b || _propiedades.color8.a != a){
			AUOBJETOESCENA_SET_FLAGS_AND_PARENTS_UNTIL_ALREADY_ENABLED(this, AUOBJETOESCENA_BIT_MODEL_LOCAL_CHANGED, AUOBJETOESCENA_BIT_MODEL_CHILD_CHANGED)
		}
	}
	//Set props
	_propiedades.color8.r = r;
	_propiedades.color8.g = g;
	_propiedades.color8.b = b;
	_propiedades.color8.a = a;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaObjeto::establecerMultiplicadorAlpha8(UI8 a){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaObjeto::establecerMultiplicadorAlpha8")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	NBASSERT(a == a); //Si falla es Nan
	//Analyze and notify changes, only if already at scene
	if(idEscena >= 0){
		if(_propiedades.color8.a != a){
			AUOBJETOESCENA_SET_FLAGS_AND_PARENTS_UNTIL_ALREADY_ENABLED(this, AUOBJETOESCENA_BIT_MODEL_LOCAL_CHANGED, AUOBJETOESCENA_BIT_MODEL_CHILD_CHANGED)
		}
	}
	//Set props
	_propiedades.color8.a = a;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaObjeto::establecerIluminoDependencia(bool iluminoDependiente){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaObjeto::establecerIluminoDependencia")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	//Analyze and notify changes, only if already at scene
	if(idEscena >= 0){
		if(_iluminoDependiente != false /*iluminoDependiente*/){
			AUOBJETOESCENA_SET_FLAGS_AND_PARENTS_UNTIL_ALREADY_ENABLED(this, AUOBJETOESCENA_BIT_MODEL_LOCAL_CHANGED, AUOBJETOESCENA_BIT_MODEL_CHILD_CHANGED)
		}
	}
	//Set props
	_iluminoDependiente = false; //iluminoDependiente;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaObjeto::setVisibleAwakeAndAlphaByParentArea(const STNBAABox outter, const STNBAABox inner){
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	const NBCajaAABB box	= this->cajaAABBLocal();
	STNBAABox bbox;
	bbox.xMin	= box.xMin + _propiedades.transformaciones.trasladoX;
	bbox.xMax	= box.xMax + _propiedades.transformaciones.trasladoX;
	bbox.yMin	= box.yMin + _propiedades.transformaciones.trasladoY;
	bbox.yMax	= box.yMax + _propiedades.transformaciones.trasladoY;
	{
		NBMargenes margins;
		margins.left	= inner.xMin - outter.xMin; NBASSERT(margins.left >= 0.0f)
		margins.right	= outter.xMax - inner.xMax; NBASSERT(margins.right >= 0.0f)
		margins.bottom	= inner.yMin - outter.yMin; NBASSERT(margins.bottom >= 0.0f)
		margins.top		= outter.yMax - inner.yMax; NBASSERT(margins.top >= 0.0f)
		if(bbox.yMax >= outter.yMax || bbox.yMin <= outter.yMin){
			this->setVisibleAndAwake(FALSE);
		} else if(bbox.yMax > inner.yMax){
			if(margins.top == 0.0f){
				this->setVisibleAndAwake(FALSE);
			} else {
				this->setVisibleAndAwake(TRUE);
				this->establecerMultiplicadorAlpha8(255.0f * (1.0f - ((bbox.yMax - inner.yMax) / margins.top)));
			}
		} else if(bbox.yMin < inner.yMin){
			if(margins.bottom == 0.0f){
				this->setVisibleAndAwake(FALSE);
			} else {
				this->setVisibleAndAwake(TRUE);
				this->establecerMultiplicadorAlpha8(255.0f * (1.0f - ((inner.yMin - bbox.yMin) / margins.bottom)));
			}
		} else {
			this->setVisibleAndAwake(TRUE);
			this->establecerMultiplicadorAlpha8(255);
		}
	}
}

UI8 AUEscenaObjeto::mascaraPropsObjetoEscena(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaObjeto::mascaraPropsObjetoEscena")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _sceneMask;
}

AUEscenaObjeto* AUEscenaObjeto::contenedor(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaObjeto::contenedor")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _contenedor;
}

AUEscenaObjeto* AUEscenaObjeto::agrupadorSombras(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaObjeto::agrupadorSombras")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _agrupadorSombras;
}

//PANTALLA

void AUEscenaObjeto::puertoVisionModificado(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaObjeto::puertoVisionModificado")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

//Top-scene-tools (quick-tips, selectboxes, anything that must go at top of content)

BOOL AUEscenaObjeto::showQuickTipText(const STNBPoint localPos, const char* text, AUTextura* iconLeft, AUTextura* iconRight, const float secsShow, const ENEscenaTipAim aimDir){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaObjeto::showQuickTipText")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	AU_OBJETO_ASSERT_IS_VALID(iconLeft)
	AU_OBJETO_ASSERT_IS_VALID(iconRight)
	BOOL r = FALSE;
	if(_contenedor != NULL){
		STNBPoint scenePos;
		NBMATRIZ_MULTIPLICAR_PUNTO(scenePos, _cacheObjEscena.matrizEscena, localPos.x, localPos.y);
		r = _contenedor->showQuickTipTextScene(scenePos, text, iconLeft, iconRight, secsShow, aimDir);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return r;
}

BOOL AUEscenaObjeto::showQuickTipTextScene(const STNBPoint scenePos, const char* text, AUTextura* iconLeft, AUTextura* iconRight, const float secsShow, const ENEscenaTipAim aimDir){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaObjeto::showQuickTipText")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	AU_OBJETO_ASSERT_IS_VALID(iconLeft)
	AU_OBJETO_ASSERT_IS_VALID(iconRight)
	BOOL r = FALSE;
	if(_contenedor != NULL){
		r = _contenedor->showQuickTipTextScene(scenePos, text, iconLeft, iconRight, secsShow, aimDir);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return r;
}

BOOL AUEscenaObjeto::addToTop(AUEscenaObjeto* obj){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaObjeto::addToTop")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	AU_OBJETO_ASSERT_IS_VALID(obj)
	BOOL r = FALSE;
	if(_contenedor != NULL){
		r = _contenedor->addToTop(obj);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return r;
}

STNBRectI AUEscenaObjeto::viewPortForMe(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaObjeto::viewPortForMe")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	STNBRectI r = NBST_P(STNBRectI, 0, 0, 0, 0 );
	if(_contenedor != NULL){
		const NBCajaAABB localBox = this->cajaAABBLocal();
		STNBPoint scenePos[4];
		NBMATRIZ_MULTIPLICAR_PUNTO(scenePos[0], _cacheObjEscena.matrizEscena, localBox.xMin, localBox.yMin);
		NBMATRIZ_MULTIPLICAR_PUNTO(scenePos[1], _cacheObjEscena.matrizEscena, localBox.xMin, localBox.yMax);
		NBMATRIZ_MULTIPLICAR_PUNTO(scenePos[2], _cacheObjEscena.matrizEscena, localBox.xMax, localBox.yMax);
		NBMATRIZ_MULTIPLICAR_PUNTO(scenePos[3], _cacheObjEscena.matrizEscena, localBox.xMax, localBox.yMin);
		STNBAABox sceneBox;
		NBAABox_wrapFirstPoint(&sceneBox, scenePos[0]);
		NBAABox_wrapNextPoint(&sceneBox, scenePos[1]);
		NBAABox_wrapNextPoint(&sceneBox, scenePos[2]);
		NBAABox_wrapNextPoint(&sceneBox, scenePos[3]);
		r = _contenedor->viewPortForSceneBox(sceneBox);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return r;
}

STNBRectI AUEscenaObjeto::viewPortForLocalBox(const STNBAABox box){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaObjeto::viewPortForLocalBox")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	STNBRectI r = NBST_P(STNBRectI, 0, 0, 0, 0 );
	if(_contenedor != NULL){
		STNBPoint scenePos[4];
		NBMATRIZ_MULTIPLICAR_PUNTO(scenePos[0], _cacheObjEscena.matrizEscena, box.xMin, box.yMin);
		NBMATRIZ_MULTIPLICAR_PUNTO(scenePos[1], _cacheObjEscena.matrizEscena, box.xMin, box.yMax);
		NBMATRIZ_MULTIPLICAR_PUNTO(scenePos[2], _cacheObjEscena.matrizEscena, box.xMax, box.yMax);
		NBMATRIZ_MULTIPLICAR_PUNTO(scenePos[3], _cacheObjEscena.matrizEscena, box.xMax, box.yMin);
		STNBAABox sceneBox;
		NBAABox_wrapFirstPoint(&sceneBox, scenePos[0]);
		NBAABox_wrapNextPoint(&sceneBox, scenePos[1]);
		NBAABox_wrapNextPoint(&sceneBox, scenePos[2]);
		NBAABox_wrapNextPoint(&sceneBox, scenePos[3]);
		r = _contenedor->viewPortForSceneBox(sceneBox);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return r;
}

STNBRectI AUEscenaObjeto::viewPortForSceneBox(const STNBAABox box){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaObjeto::viewPortForLocalBox")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	STNBRectI r = NBST_P(STNBRectI, 0, 0, 0, 0 );
	if(_contenedor != NULL){
		r = _contenedor->viewPortForSceneBox(box);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return r;
}

//TOUCHES

IEscuchadorTouchEscenaObjeto* AUEscenaObjeto::escuchadorTouches(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaObjeto::escuchadorTouches")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _escuchadorTouchInterfaz;
}

void AUEscenaObjeto::establecerEscuchadorTouches(AUObjeto* obj, IEscuchadorTouchEscenaObjeto* interfaz){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaObjeto::establecerEscuchadorTouches")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	AU_OBJETO_ASSERT_IS_VALID(obj)
	_escuchadorTouchObjeto		= obj; //Do no retain, to avoid mutual retaining
	_escuchadorTouchInterfaz	= interfaz;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaObjeto::setMngrTouchesItf(const STNBEscenaObjetoMngrTouchItf *  itf){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaObjeto::setMngrTouchesItf")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	if(itf == NULL){
		NBMemory_setZero(_touches.itf);
	} else {
		_touches.itf = *itf;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

SI32 AUEscenaObjeto::touchesCount(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaObjeto::touchesCount")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	NBASSERT(_touches.count >= 0)
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _touches.count;
}

void AUEscenaObjeto::touchesReleaseAll(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaObjeto::touchesCount")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	NBASSERT(_touches.count >= 0)
	if(_touches.count > 0){
		NBASSERT(_touches.itf.objWillBeRemovedFromSceneFunc != NULL) //Must be set
		(*_touches.itf.objWillBeRemovedFromSceneFunc)(this, _touches.count, _touches.itf.param);
		_touches.count = 0;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaObjeto::touchIniciado(void* escuchador, STGTouch* touch, const NBPunto &posInicialEscena){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaObjeto::touchIniciado")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	NBASSERT(_touches.itf.objWillBeRemovedFromSceneFunc != NULL) //Must be set
	//Increase count before calling listener
	{
		NBASSERT(_touches.count >= 0)
		_touches.count++;
	}
	//Call listener
	{
		if(escuchador != NULL){
			((IEscuchadorTouchEscenaObjeto*)escuchador)->touchIniciado(touch, posInicialEscena, this);
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaObjeto::touchMovido(void* escuchador, STGTouch* touch, const NBPunto &posInicialEscena, const NBPunto &posAnteriorEscena, const NBPunto &posActualEscena){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaObjeto::touchMovido")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	NBASSERT(_touches.itf.objWillBeRemovedFromSceneFunc != NULL) //Must be set
	//Call listener
	if(escuchador != NULL){
		((IEscuchadorTouchEscenaObjeto*)escuchador)->touchMovido(touch, posInicialEscena, posAnteriorEscena, posActualEscena, this);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaObjeto::touchFinalizado(void* escuchador, STGTouch* touch, const NBPunto &posInicialEscena, const NBPunto &posAnteriorEscena, const NBPunto &posActualEscena){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaObjeto::touchFinalizado")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	NBASSERT(_touches.itf.objWillBeRemovedFromSceneFunc != NULL) //Must be set
	//Reduce count before calling listener;
	//if not, could result in double release attempt
	{
		_touches.count--;
		NBASSERT(_touches.count >= 0)
	}
	//Call listener
	{
		if(escuchador != NULL){
			((IEscuchadorTouchEscenaObjeto*)escuchador)->touchFinalizado(touch, posInicialEscena, posAnteriorEscena, posActualEscena, this);
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaObjeto::hoverIniciado(void* escuchador){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaObjeto::hoverIniciado")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	if(escuchador != NULL){
		((IEscuchadorTouchEscenaObjeto*)escuchador)->hoverIniciado(this);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaObjeto::hoverMovido(void* escuchador, const NBPunto &posActualEscena){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaObjeto::hoverMovido")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	if(escuchador != NULL){
		((IEscuchadorTouchEscenaObjeto*)escuchador)->hoverMovido(posActualEscena, this);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaObjeto::hoverFinalizado(void* escuchador){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaObjeto::hoverFinalizado")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	if(escuchador != NULL){
		((IEscuchadorTouchEscenaObjeto*)escuchador)->hoverFinalizado(this);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaObjeto::touchScrollApply(void* escuchador, const STNBPoint posScene, const STNBSize size, const BOOL animate){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaObjeto::scrollApply")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	if(escuchador != NULL){
		((IEscuchadorTouchEscenaObjeto*)escuchador)->touchScrollApply(this, posScene, size, animate);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaObjeto::touchMagnifyApply(void* escuchador, const STNBPoint posScene, const float magnification, const BOOL isSmarthMag){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaObjeto::magnifyApply")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	if(escuchador != NULL){
		((IEscuchadorTouchEscenaObjeto*)escuchador)->touchMagnifyApply(this, posScene, magnification, isSmarthMag);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaObjeto::liberarTouch(STGTouch* touch, const NBPunto &posInicialEscena, const NBPunto &posAnteriorEscena, const NBPunto &posActualEscena, bool cancelar, AUEscenaObjeto* nuevoDestinatario){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaObjeto::liberarTouch")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	//TODO: re-enable this code
	AUObjeto* consumidorEscuchadorObj = NULL; void* consumidorEscuchadorInterfaz = NULL;
	AUEscenaObjeto* consumidor = (AUEscenaObjeto*)NBGestorTouches::touchConsumidor(touch, &consumidorEscuchadorObj, &consumidorEscuchadorInterfaz);
	AU_OBJETO_ASSERT_IS_VALID(consumidor)
	NBASSERT((consumidor == NULL && consumidorEscuchadorObj == NULL && consumidorEscuchadorInterfaz == NULL) || (consumidor != NULL && consumidorEscuchadorObj != NULL && consumidorEscuchadorInterfaz != NULL))
	if(consumidor != NULL){
		//NBASSERT(consumidor->idEscena==nuevoDestinatario->idEscena && consumidor->idGrupo==nuevoDestinatario->idGrupo)
		touch->cancelado	   = cancelar;
		consumidor->touchFinalizado(consumidorEscuchadorInterfaz, touch, posInicialEscena, posAnteriorEscena, posActualEscena);
	}
	bool nuevoDestAplicado = false;
	if(nuevoDestinatario != NULL){
		if(nuevoDestinatario->_escuchadorTouchObjeto != NULL && nuevoDestinatario->_escuchadorTouchInterfaz != NULL){
			NBGestorTouches::touchEstablecerConsumidor(touch, nuevoDestinatario, nuevoDestinatario->_escuchadorTouchObjeto, nuevoDestinatario->_escuchadorTouchInterfaz);
			touch->cancelado			= false;
			touch->posSuperficieIni		= touch->posSuperficieAct;
			touch->posSuperficieAnt		= touch->posSuperficieAct;
			touch->posEscenaIni			= touch->posEscenaAct;
			touch->posEscenaAnt			= touch->posEscenaAct;
			//touch->marcaConsumidor	= -1; //Do not reset this value
			touch->datosAdicionales		= NULL;
			nuevoDestinatario->setMngrTouchesItf(&_touches.itf);
			nuevoDestinatario->touchIniciado(nuevoDestinatario->_escuchadorTouchInterfaz, touch, posActualEscena);
			nuevoDestAplicado = true;
		}
	}
	if(!nuevoDestAplicado){
		NBGestorTouches::touchEstablecerConsumidor(touch, NULL, NULL, NULL);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUEscenaObjeto* AUEscenaObjeto::consumidorTouchEnPosicion(const NBPunto &posTouchEscena, const UI8 touchFilterMask, AUObjeto** outConsumidorEscuchadorObj, void** outConsumidorEscuchadorInterfaz){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaObjeto::consumidorTouchEnPosicion")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	NBASSERT(idEscena != -1) //Solo si esta en escena
	//NBASSERT(_cacheObjEscena.debugCacheActualizada) //Esta validacion no se cumple periodicamente, pero no es necesaria porque los objetos recien aregados en escena no tienen cajaEscena y por ende no se estableceria coordenadas sobre ellos.
	AUEscenaObjeto* r = NULL;
	if(_escuchadorTouchObjeto != NULL && _escuchadorTouchInterfaz != NULL){
		if((touchFilterMask & _touchFilterMask) != 0){
			if(_cacheObjEscena.cajaEscena.xMin < _cacheObjEscena.cajaEscena.xMax && _cacheObjEscena.cajaEscena.yMin < _cacheObjEscena.cajaEscena.yMax){
				if(NBCAJAAABB_INTERSECTA_PUNTO(_cacheObjEscena.cajaEscena, posTouchEscena.x, posTouchEscena.y)){
					NBASSERT(this->_visible)
					NBASSERT(!this->_dormido)
					r = this;
					if(outConsumidorEscuchadorObj != NULL){
						*outConsumidorEscuchadorObj = _escuchadorTouchObjeto;
					}
					if(outConsumidorEscuchadorInterfaz != NULL){
						*outConsumidorEscuchadorInterfaz = _escuchadorTouchInterfaz;
					}
				}
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return r;
}

AUEscenaObjeto* AUEscenaObjeto::consumidorTouchEnPosicionSegunArreglo(const NBPunto &posTouchEscena, const UI8 touchFilterMask, const AUArreglo* arregloFiltroObjetosEscena){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaObjeto::consumidorTouchEnPosicionSegunArreglo")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	//NBASSERT(idEscena != -1) //Solo si esta en escena
	//NBASSERT(_cacheObjEscena.debugCacheActualizada) //Esta validacion no se cumple periodicamente, pero no es necesaria porque los objetos recien aregados en escena no tienen cajaEscena y por ende no se estableceria coordenadas sobre ellos.
	AUEscenaObjeto* r = NULL;
	if((touchFilterMask & _touchFilterMask) != 0){
		if(_cacheObjEscena.cajaEscena.xMin < _cacheObjEscena.cajaEscena.xMax && _cacheObjEscena.cajaEscena.yMin < _cacheObjEscena.cajaEscena.yMax){
			if(NBCAJAAABB_INTERSECTA_PUNTO(_cacheObjEscena.cajaEscena, posTouchEscena.x, posTouchEscena.y)){
				if((arregloFiltroObjetosEscena->indiceDe(this) != -1)){
					NBASSERT(this->_visible)
					NBASSERT(!this->_dormido)
					r = this;
				}
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return r;
}

//MODELO

NBPunto AUEscenaObjeto::coordenadaLocalAEscena(float xLocal, float yLocal){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaObjeto::coordenadaLocalAEscena")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	//NBASSERT(idEscena!=-1) //ToDo: remove
	NBPunto r; NBMATRIZ_MULTIPLICAR_PUNTO(r, _cacheObjEscena.matrizEscena, xLocal, yLocal);
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return r;
}

NBPunto AUEscenaObjeto::coordenadaEscenaALocal(float xEnEscena, float yEnEscena){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaObjeto::coordenadaEscenaALocal")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	//NBASSERT(idEscena != -1) //ToDo: remove
	NBMatriz matrizEscenaInversa; NBMATRIZ_INVERSA(matrizEscenaInversa, _cacheObjEscena.matrizEscena);
	NBPunto r; NBMATRIZ_MULTIPLICAR_PUNTO(r, matrizEscenaInversa, xEnEscena, yEnEscena)
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return r;
}

//guardar y cargar en XML

bool AUEscenaObjeto::agregarXmlInternoEn(AUEscenaObjeto* objEscena, AUCadenaLargaMutable8* guardarEn, const char* espaciosBaseIzq){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaObjeto::agregarXmlInternoEn")
	AU_OBJETO_ASSERT_IS_VALID(objEscena)
	AU_OBJETO_ASSERT_IS_VALID(guardarEn)
	//guardarEn->agregarConFormato("%s<vXML>1</vXML>\r\n", espaciosBaseIzq);
	//------------------------------------
	//Formato viejo (ineficiente en tamano)
	//------------------------------------
	/*guardarEn->agregarConFormato("%s<pp>\r\n", espaciosBaseIzq);
	guardarEn->agregarConFormato("%s\t<vis>%d</vis>\r\n", espaciosBaseIzq, objEscena->_visible?1:0);
	guardarEn->agregarConFormato("%s\t<posX>%f</posX><posY>%f</posY>\r\n", espaciosBaseIzq, objEscena->_propiedades.transformaciones.trasladoX, objEscena->_propiedades.transformaciones.trasladoY);
	guardarEn->agregarConFormato("%s\t<escX>%f</escX><escY>%f</escY>\r\n", espaciosBaseIzq, objEscena->_propiedades.transformaciones.escalaX, objEscena->_propiedades.transformaciones.escalaY);
	guardarEn->agregarConFormato("%s\t<rot>%f</rot>\r\n", espaciosBaseIzq, objEscena->_propiedades.transformaciones.rotacion);
	guardarEn->agregarConFormato("%s\t<col8><r>%d</r><g>%d</g><b>%d</b><a>%d</a></col8>\r\n", espaciosBaseIzq, (SI32)objEscena->_propiedades.color8.r, (SI32)objEscena->_propiedades.color8.g, (SI32)objEscena->_propiedades.color8.b, (SI32)objEscena->_propiedades.color8.a);
	guardarEn->agregarConFormato("%s\t<ilumDepend>%d</ilumDepend>\r\n", espaciosBaseIzq, objEscena->_iluminoDependiente?1:0);
	guardarEn->agregarConFormato("%s</pp>\r\n", espaciosBaseIzq);*/
	//------------------------------------
	//Formato nuevo (mas compacto)
	//------------------------------------
	guardarEn->agregarConFormato("%s<pp>"	, espaciosBaseIzq);
	guardarEn->agregarConFormato("%d|"			, objEscena->_visible?1:0);
	guardarEn->agregarConFormato("%f|%f|"		, objEscena->_propiedades.transformaciones.trasladoX, objEscena->_propiedades.transformaciones.trasladoY);
	guardarEn->agregarConFormato("%f|%f|"		, objEscena->_propiedades.transformaciones.escalaX, objEscena->_propiedades.transformaciones.escalaY);
	guardarEn->agregarConFormato("%f|"			, objEscena->_propiedades.transformaciones.rotacion);
	guardarEn->agregarConFormato("%d|%d|%d|%d|"	, (SI32)objEscena->_propiedades.color8.r, (SI32)objEscena->_propiedades.color8.g, (SI32)objEscena->_propiedades.color8.b, (SI32)objEscena->_propiedades.color8.a);
	guardarEn->agregarConFormato("%d"			, objEscena->_iluminoDependiente?1:0);
	guardarEn->agregarConFormato("</pp>\r\n");
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return true;
}

bool AUEscenaObjeto::interpretarXmlInterno(AUEscenaObjeto* cargarEn, const AUDatosXML* datosXml, const sNodoXML* nodoXml){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaObjeto::interpretarXmlInterno")
	AU_OBJETO_ASSERT_IS_VALID(cargarEn)
	AU_OBJETO_ASSERT_IS_VALID(datosXml)
	bool exito = false;
	//int versionXML = datosXml->nodoHijo("vXML", 0, nodoXml); NBASSERT(versionXML==1)
	//if(versionXML==1){
		const sNodoXML* nodoProps;	XML_NODO_HIJO(datosXml, nodoProps, "pp", "props", nodoXml, NULL) //NBASSERT(nodoProps != NULL)
		if(nodoProps != NULL){
			bool iluminoDependiente = false;
            BOOL visible = FALSE;
			NBPropiedadesEnEscena props;
			NBMemory_setZeroSt(props, NBPropiedadesEnEscena);
			exito = true;
			if(nodoProps->nodosHijos == NULL){
				//Formato nuevo (CSV mas compacto)
				const char* cadenaCSV				= datosXml->cadenaValorDeNodo(nodoProps);
				SI32 posEnCadenaCSV					= 0;
				SI32 tamCadenaCSV					= AUCadena8::tamano(cadenaCSV);
				visible						        = AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, true);
				props.transformaciones.trasladoX	= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0.0f); NBASSERT(props.transformaciones.trasladoX > -100000.0f && props.transformaciones.trasladoX < 100000.0f) //Valor corrupto???
				props.transformaciones.trasladoY	= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0.0f); NBASSERT(props.transformaciones.trasladoY > -100000.0f && props.transformaciones.trasladoY < 100000.0f) //Valor corrupto???
				props.transformaciones.escalaX		= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 1.0f);	NBASSERT(props.transformaciones.escalaX != 0.0f && props.transformaciones.escalaX != 0.0f) //Valor corrupto???
				props.transformaciones.escalaY		= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 1.0f);	NBASSERT(props.transformaciones.escalaY != 0.0f && props.transformaciones.escalaY != 0.0f) //Valor corrupto???
				props.transformaciones.rotacion		= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0.0f);
				props.color8.r						= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 255);
				props.color8.g						= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 255);
				props.color8.b						= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 255);
				props.color8.a						= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 255);
				iluminoDependiente					= false; //AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, true);
			} else {
				//Formato viejo (XML que ocupa mucho espacio)
				visible						        = datosXml->nodoHijo("vis", true, nodoProps);
				props.transformaciones.trasladoX	= datosXml->nodoHijo("posX", 0.0f, nodoProps); NBASSERT(props.transformaciones.trasladoX > -1000000.0f && props.transformaciones.trasladoX < 1000000.0f) //Valor corrupto???
				props.transformaciones.trasladoY	= datosXml->nodoHijo("posY", 0.0f, nodoProps); NBASSERT(props.transformaciones.trasladoY > -1000000.0f && props.transformaciones.trasladoY < 1000000.0f) //Valor corrupto???
				props.transformaciones.escalaX		= datosXml->nodoHijo("escX", 0.0f, nodoProps);	NBASSERT(props.transformaciones.escalaX != 0.0f && props.transformaciones.escalaX != 0.0f) //Valor corrupto???
				props.transformaciones.escalaY		= datosXml->nodoHijo("escY", 0.0f, nodoProps);	NBASSERT(props.transformaciones.escalaY != 0.0f && props.transformaciones.escalaY != 0.0f) //Valor corrupto???
				props.transformaciones.rotacion		= datosXml->nodoHijo("rot", 0.0f, nodoProps);
				NBCOLOR_ESTABLECER(props.color8, 255, 255, 255, 255);
				const sNodoXML* nodoColor8			= datosXml->nodoHijo("col8", nodoProps);
				if(nodoColor8 != NULL){
					props.color8.r					= datosXml->nodoHijo("r", 255, nodoColor8);
					props.color8.g					= datosXml->nodoHijo("g", 255, nodoColor8);
					props.color8.b					= datosXml->nodoHijo("b", 255, nodoColor8);
					props.color8.a					= datosXml->nodoHijo("a", 255, nodoColor8);
				} else { //Compatibilidad con archivos viejos
					const sNodoXML* nodoColor		= datosXml->nodoHijo("col", nodoProps);
					if(nodoColor != NULL){
						props.color8.r				= 255*datosXml->nodoHijo("r", 1.0f, nodoColor);
						props.color8.g				= 255*datosXml->nodoHijo("g", 1.0f, nodoColor);
						props.color8.b				= 255*datosXml->nodoHijo("b", 1.0f, nodoColor);
						props.color8.a				= 255*datosXml->nodoHijo("a", 1.0f, nodoColor);
					}
				}
				iluminoDependiente					= false; //datosXml->nodoHijo("ilumDepend", true, nodoProps);
			}
			//Set
			cargarEn->establecerPropiedades(visible, props);
			cargarEn->establecerIluminoDependencia(iluminoDependiente);
			//
			NBASSERT(cargarEn->_propiedades.transformaciones.escalaX != 0.0f && cargarEn->_propiedades.transformaciones.escalaY != 0.0f)
			NBASSERT(cargarEn->_propiedades.transformaciones.escalaX == cargarEn->_propiedades.transformaciones.escalaX && cargarEn->_propiedades.transformaciones.escalaY == cargarEn->_propiedades.transformaciones.escalaY) //NaN???
			NBASSERT(cargarEn->_propiedades.transformaciones.trasladoX == cargarEn->_propiedades.transformaciones.trasladoX && cargarEn->_propiedades.transformaciones.trasladoY == cargarEn->_propiedades.transformaciones.trasladoY) //NaN???
			NBASSERT(cargarEn->_propiedades.transformaciones.rotacion == cargarEn->_propiedades.transformaciones.rotacion) //NaN???
		}
	//}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

//guardar y cargar en binario

bool AUEscenaObjeto::agregarBitsInternosEn(AUEscenaObjeto* objEscena, AUArchivo* guardarEn){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaObjeto::agregarBitsInternosEn")
	AU_OBJETO_ASSERT_IS_VALID(objEscena)
	AU_OBJETO_ASSERT_IS_VALID(guardarEn)
	bool exito = false;
	SI32 valorVerificacion = AUOBJETOESCENA_VALOR_VERIFICACION_BIN;
	NBASSERT(objEscena->_propiedades.transformaciones.escalaX != 0.0f && objEscena->_propiedades.transformaciones.escalaY != 0.0f)
	guardarEn->escribir(&valorVerificacion, sizeof(valorVerificacion), 1, guardarEn);
    guardarEn->escribir(&(objEscena->_visible), sizeof(objEscena->_visible), 1, guardarEn);
	guardarEn->escribir(&(objEscena->_propiedades), sizeof(objEscena->_propiedades), 1, guardarEn);
	guardarEn->escribir(&(objEscena->_iluminoDependiente), sizeof(objEscena->_iluminoDependiente), 1, guardarEn);
	guardarEn->escribir(&valorVerificacion, sizeof(valorVerificacion), 1, guardarEn);
	exito = true;
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

bool AUEscenaObjeto::interpretarBitsInternos(AUEscenaObjeto* cargarEn, AUArchivo* cargarDe){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaObjeto::interpretarBitsInternos")
	AU_OBJETO_ASSERT_IS_VALID(cargarEn)
	AU_OBJETO_ASSERT_IS_VALID(cargarDe)
	bool exito = false;
	SI32 valorVerificacion;
	cargarDe->leer(&valorVerificacion, sizeof(valorVerificacion), 1, cargarDe); NBASSERT(valorVerificacion == AUOBJETOESCENA_VALOR_VERIFICACION_BIN)
	if(valorVerificacion == AUOBJETOESCENA_VALOR_VERIFICACION_BIN){
		bool iluminoDependiente = false;
        bool visible = false;
		NBPropiedadesEnEscena props;
		NBMemory_setZeroSt(props, NBPropiedadesEnEscena);
		//
        cargarDe->leer(&visible, sizeof(visible), 1, cargarDe);
		cargarDe->leer(&props, sizeof(props), 1, cargarDe);
		cargarDe->leer(&iluminoDependiente, sizeof(iluminoDependiente), 1, cargarDe);
		cargarDe->leer(&valorVerificacion, sizeof(valorVerificacion), 1, cargarDe); NBASSERT(valorVerificacion == AUOBJETOESCENA_VALOR_VERIFICACION_BIN)
		//Set
		if(valorVerificacion == AUOBJETOESCENA_VALOR_VERIFICACION_BIN){
			cargarEn->establecerPropiedades(visible, props);
			cargarEn->establecerIluminoDependencia(iluminoDependiente);
			exito = true;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

//
AUOBJMETODOS_CLASESID_UNICLASE(AUEscenaObjeto)
AUOBJMETODOS_CLASESNOMBRES_UNICLASE(AUEscenaObjeto, "AUEscenaObjeto")



