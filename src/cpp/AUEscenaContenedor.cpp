 
#include "AUFrameworkMediaStdAfx.h"
#include "AUEscenaContenedor.h"

//-------------------------------------
//--- CONSTRUCTORES Y DESTRUCTORES ----
//-------------------------------------

AUEscenaContenedor::AUEscenaContenedor() : AUEscenaObjeto(), _objetosEscena(this) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaContenedor::AUEscenaContenedor")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaContenedor")
	_sceneMask			|= AUOBJETOESCENA_BIT_ESCONTENEDOR;
	NBASSERT(_agrupadorSombras==this)
	NBASSERT((_sceneMask & AUOBJETOESCENA_BIT_AGRUPADOR_SOMBRAS_FIJO) == 0)
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUEscenaContenedor::AUEscenaContenedor(AUEscenaContenedor* otraInstancia) : AUEscenaObjeto(otraInstancia), _objetosEscena(this, otraInstancia->_objetosEscena.conteo) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaContenedor::AUEscenaContenedor")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaContenedor")
	_sceneMask		|= AUOBJETOESCENA_BIT_ESCONTENEDOR;
	//Actualizar agrupador de sombras
	if((otraInstancia->_sceneMask & AUOBJETOESCENA_BIT_AGRUPADOR_SOMBRAS_FIJO) != 0) fijarAgrupadorDeSombras();
	if((otraInstancia->_sceneMask & AUOBJETOESCENA_BIT_PREAGRUPAR_SOMBRAS) != 0) _sceneMask |= AUOBJETOESCENA_BIT_PREAGRUPAR_SOMBRAS;
	//Clonar hijos de otro contenedor
	UI16 h;
	for(h = 0; h < otraInstancia->_objetosEscena.conteo; h++){
		AUEscenaObjeto* hijo = (AUEscenaObjeto*)otraInstancia->_objetosEscena.elemento[h];
		NBASSERT(hijo != NULL) AU_OBJETO_ASSERT_IS_VALID(hijo)
		AUEscenaObjeto* copiaHijo = (AUEscenaObjeto*)hijo->clonar(this->tipoMemoriaResidencia(), ENClonadoTipo_ClonarInstancias); NBASSERT(copiaHijo != NULL)
		agregarObjetoEscena(copiaHijo); copiaHijo->liberar(NB_RETENEDOR_THIS);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUEscenaContenedor::AUEscenaContenedor(AUEscenaContenedor* otraInstancia, const bool clonarHijos) : AUEscenaObjeto(otraInstancia), _objetosEscena(this) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaContenedor::AUEscenaContenedor")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaContenedor")
	_sceneMask		|= AUOBJETOESCENA_BIT_ESCONTENEDOR;
	//Actualizar agrupador de sombras
	if((otraInstancia->_sceneMask & AUOBJETOESCENA_BIT_AGRUPADOR_SOMBRAS_FIJO) != 0) fijarAgrupadorDeSombras();
	if((otraInstancia->_sceneMask & AUOBJETOESCENA_BIT_PREAGRUPAR_SOMBRAS) != 0) _sceneMask |= AUOBJETOESCENA_BIT_PREAGRUPAR_SOMBRAS;
	//Clonar hijos de otro contenedor
	if(clonarHijos){
		UI16 h;
		for(h = 0; h < otraInstancia->_objetosEscena.conteo; h++){
			AUEscenaObjeto* hijo = (AUEscenaObjeto*)otraInstancia->_objetosEscena.elemento[h];
			NBASSERT(hijo != NULL) AU_OBJETO_ASSERT_IS_VALID(hijo)
			AUEscenaObjeto* copiaHijo = (AUEscenaObjeto*)hijo->clonar(this->tipoMemoriaResidencia(), ENClonadoTipo_ClonarInstancias); NBASSERT(copiaHijo != NULL)
			agregarObjetoEscena(copiaHijo); copiaHijo->liberar(NB_RETENEDOR_THIS);
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUEscenaContenedor::~AUEscenaContenedor(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaContenedor::~AUEscenaContenedor")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	//NBASSERT(!_debugBloqueadoActualizandoModelos)
	this->vaciar();
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

//

#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
	#define AUESCENACONTENEDOR_APAGAR_BANDERA_CACHE_ACTUALIZADA(OBJ_ESCENA) (OBJ_ESCENA)->_cacheObjEscena.debugCacheActualizada = false;
#else
	#define AUESCENACONTENEDOR_APAGAR_BANDERA_CACHE_ACTUALIZADA(OBJ_ESCENA)
#endif

#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
	#define AUESCENACONTENEDOR_DEBUG_VERIFICA_NOTIFICACION_AGREGADO_ESCENA(OBJ_ESCENA)		NBASSERT(!(OBJ_ESCENA)->_debugAgregadoEscena) (OBJ_ESCENA)->_debugAgregadoEscena = true;
	#define AUESCENACONTENEDOR_DEBUG_VERIFICA_NOTIFICACION_AGREGADO_ESCENA_POST(OBJ_ESCENA)	NBASSERT((OBJ_ESCENA)->_debugAgregadoEscena)
	#define AUESCENACONTENEDOR_DEBUG_VERIFICA_NOTIFICACION_QUITANDO_ESCENA(OBJ_ESCENA)		NBASSERT((OBJ_ESCENA)->_debugAgregadoEscena) (OBJ_ESCENA)->_debugAgregadoEscena = false;
#else
	#define AUESCENACONTENEDOR_DEBUG_VERIFICA_NOTIFICACION_AGREGADO_ESCENA(OBJ_ESCENA)
	#define AUESCENACONTENEDOR_DEBUG_VERIFICA_NOTIFICACION_AGREGADO_ESCENA_POST(OBJ_ESCENA)
	#define AUESCENACONTENEDOR_DEBUG_VERIFICA_NOTIFICACION_QUITANDO_ESCENA(OBJ_ESCENA)
#endif

#define AUESCENACONTENEDOR_ACTUALIZAR_NIVEL_ESCENA_DE_OBJETO(OBJ_ESCENA, ID_ESCENA, ID_GRUPO, INDICE_CAPA)		\
	{ \
		NBASSERT(ID_ESCENA >= -1 && ID_ESCENA < 127)		/*Valor maximo para un SI8*/ \
		NBASSERT(ID_GRUPO >= 0 && ID_GRUPO < 127)			/*Valor maximo para un SI8*/ \
		NBASSERT(INDICE_CAPA >= -1 && INDICE_CAPA < 32767)	/*Valor maximo para un SI16*/ \
		NBASSERT(((OBJ_ESCENA)->idEscena == -1 && ID_ESCENA != -1) || ID_ESCENA == -1) /*Si falla entonces no se está "saliendo" ni "entrando limpio" a una escena*/ \
		/*Removing from scene*/ \
		if((OBJ_ESCENA)->idEscena != -1 && ID_ESCENA == -1){ \
			AUESCENACONTENEDOR_DEBUG_VERIFICA_NOTIFICACION_QUITANDO_ESCENA(OBJ_ESCENA) \
			(OBJ_ESCENA)->touchesReleaseAll(); \
			(OBJ_ESCENA)->quitandoDeEscena(); \
			/*if(_cantTouchesIniciadosActivos != 0){*/ \
			/*	Saliendo de escena cuando aun se tiene touches encima.*/ \
			/*	Liberar los touches*/ \
			/*}*/ \
		} \
		/*Set props*/ \
		(OBJ_ESCENA)->idEscena		= ID_ESCENA; \
		(OBJ_ESCENA)->idGrupo			= ID_GRUPO; \
		(OBJ_ESCENA)->idCapa			= INDICE_CAPA; \
		/*Added to scene*/ \
		if(ID_ESCENA >= 0){ \
			AUESCENACONTENEDOR_APAGAR_BANDERA_CACHE_ACTUALIZADA(OBJ_ESCENA) \
			NBMATRIZ_ESTABLECER_IDENTIDAD((OBJ_ESCENA)->_cacheObjEscena.matrizEscena); \
			NBCAJAAABB_INICIALIZAR((OBJ_ESCENA)->_cacheObjEscena.cajaEscena); \
			AUESCENACONTENEDOR_DEBUG_VERIFICA_NOTIFICACION_AGREGADO_ESCENA(OBJ_ESCENA) \
			(OBJ_ESCENA)->enableSceneMaskAndParentsUntilEnabled(AUOBJETOESCENA_BIT_MATRIX_LOCAL_CHANGED | AUOBJETOESCENA_BIT_MODEL_LOCAL_CHANGED, AUOBJETOESCENA_BIT_MATRIX_CHILD_CHANGED | AUOBJETOESCENA_BIT_MODEL_LOCAL_CHANGED | AUOBJETOESCENA_BIT_MODEL_CHILD_CHANGED); \
			(OBJ_ESCENA)->agregadoEnEscena(); \
		} \
	}

//-------------------------------
//--- FUNCIONES DE INSTANCIA ----
//-------------------------------

AUArreglo* AUEscenaContenedor::hijos(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaContenedor::hijos")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	//NBASSERT(!_debugBloqueadoActualizandoModelos)
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return &_objetosEscena;
}

void AUEscenaContenedor::hijoMover(const UI32 indiceActual, const UI32 indiceDestino){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaContenedor::hijoMover")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	_objetosEscena.moverElemento(indiceActual, indiceDestino);
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaContenedor::vaciar(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaContenedor::vaciar")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	//NBASSERT(!_debugBloqueadoActualizandoModelos)
	while(_objetosEscena.conteo > 0){
		this->quitarObjetoEscenaEnIndice(_objetosEscena.conteo - 1);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaContenedor::agregarObjetoEscena(AUEscenaObjeto* obj){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaContenedor::agregarObjetoEscena")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	AU_OBJETO_ASSERT_IS_VALID(obj)
	//NBASSERT(!_debugBloqueadoActualizandoModelos)
	this->agregarObjetoEscenaEnIndice(obj, _objetosEscena.conteo);
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaContenedor::agregarObjetoEscenaEnIndice(AUEscenaObjeto* obj, int indice){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaContenedor::agregarObjetoEscenaEnIndice")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	AU_OBJETO_ASSERT_IS_VALID(obj)
	//NBASSERT(!_debugBloqueadoActualizandoModelos)
	NBASSERT(obj != NULL)
	NBASSERT(obj != this) //adding to myself!
	if(obj == this){
		PRINTF_WARNING("OBJESCENA SE HA INTENTADO AGREGAR A SI MISMO\n");
	} else {
		AUEscenaContenedor* contenedorActual = (AUEscenaContenedor*)(obj->_contenedor);
		//retener por seguridad
		obj->retener(NB_RETENEDOR_THIS);
		//quitar del contenedor actual
		if(contenedorActual != NULL) contenedorActual->quitarObjetoEscena(obj);
		//agregar a este
		NBASSERT(obj->idEscena == -1)
		_objetosEscena.agregarElementoEnIndice(obj, indice);
		obj->_contenedor = this;
		{
			if((obj->_sceneMask & AUOBJETOESCENA_BIT_ESCONTENEDOR) != 0){
				if((obj->_sceneMask & AUOBJETOESCENA_BIT_MUNDO_FISICA_FIJO) == 0) ((AUEscenaContenedor*)obj)->privEstablecerIdMundoFisica(_idMundoFisica);
				((AUEscenaContenedor*)obj)->privActualizarNivelEnEscena(idEscena, idGrupo, idCapa);
				if((_sceneMask & AUOBJETOESCENA_BIT_AGRUPADOR_SOMBRAS_FIJO) != 0){
					((AUEscenaContenedor*)obj)->fijarAgrupadorDeSombras(_agrupadorSombras);
				}
			} else {
				if((obj->_sceneMask & AUOBJETOESCENA_BIT_MUNDO_FISICA_FIJO) == 0) obj->_idMundoFisica = _idMundoFisica;
				AUESCENACONTENEDOR_ACTUALIZAR_NIVEL_ESCENA_DE_OBJETO(obj, idEscena, idGrupo, idCapa)
				if((_sceneMask & AUOBJETOESCENA_BIT_AGRUPADOR_SOMBRAS_FIJO) != 0){
					obj->_agrupadorSombras			= _agrupadorSombras;
					obj->_sceneMask |= AUOBJETOESCENA_BIT_AGRUPADOR_SOMBRAS_FIJO;
				}
			}
		}
		//liberar de la retencion del principio (queda retenido en el arreglo)
		obj->liberar(NB_RETENEDOR_THIS);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

bool AUEscenaContenedor::quitarObjetoEscena(AUEscenaObjeto* obj){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaContenedor::quitarObjetoEscena")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	AU_OBJETO_ASSERT_IS_VALID(obj)
	//NBASSERT(!_debugBloqueadoActualizandoModelos)
	SI32 indice = _objetosEscena.indiceDe(obj);
	while(indice != -1){
		this->quitarObjetoEscenaEnIndice(indice);
		indice = _objetosEscena.indiceDe(obj);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return true;
}

bool AUEscenaContenedor::quitarObjetoEscenaEnIndice(int indice){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaContenedor::quitarObjetoEscenaEnIndice")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	//NBASSERT(!_debugBloqueadoActualizandoModelos)
	NBASSERT(indice >= 0 && indice < _objetosEscena.conteo)
	bool r = false;
	AUEscenaObjeto* obj = (AUEscenaObjeto*)_objetosEscena.elemento[indice]; NBASSERT(obj != NULL)
	obj->_contenedor = NULL;
	{
		if((obj->_sceneMask & AUOBJETOESCENA_BIT_ESCONTENEDOR) != 0){
			((AUEscenaContenedor*)obj)->privActualizarNivelEnEscena(-1, 0, -1);
			if((obj->_sceneMask & AUOBJETOESCENA_BIT_MUNDO_FISICA_FIJO) == 0) ((AUEscenaContenedor*)obj)->privEstablecerIdMundoFisica(-1);
			if((obj->_sceneMask & AUOBJETOESCENA_BIT_AGRUPADOR_SOMBRAS_FIJO) != 0){
				((AUEscenaContenedor*)obj)->restablecerAgrupadorDeSombras();
			}
		} else {
			AUESCENACONTENEDOR_ACTUALIZAR_NIVEL_ESCENA_DE_OBJETO(obj, -1, 0, -1)
			if((obj->_sceneMask & AUOBJETOESCENA_BIT_MUNDO_FISICA_FIJO) == 0) obj->_idMundoFisica = -1;
			if((obj->_sceneMask & AUOBJETOESCENA_BIT_AGRUPADOR_SOMBRAS_FIJO) != 0){
				obj->_agrupadorSombras	= obj;
				obj->_sceneMask			&= ~AUOBJETOESCENA_BIT_AGRUPADOR_SOMBRAS_FIJO;
			}
		}
	}
	_objetosEscena.quitarElementoEnIndice(indice);
	//
	this->enableSceneMaskAndParentsUntilEnabled(AUOBJETOESCENA_BIT_MODEL_LOCAL_CHANGED, AUOBJETOESCENA_BIT_MODEL_LOCAL_CHANGED | AUOBJETOESCENA_BIT_MODEL_CHILD_CHANGED);
	//
	r = true;
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return r;
}

//Fisica

void AUEscenaContenedor::privEstablecerIdMundoFisica(SI8 pIdMundoFisica){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaContenedor::privEstablecerIdMundoFisica")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	//NBASSERT(!_debugBloqueadoActualizandoModelos)
	//Actualizar datos de contenedor
	_idMundoFisica = pIdMundoFisica;
	//Actualizar hijos
	UI16 i;
	for(i = 0; i < _objetosEscena.conteo; i++){
		AUEscenaObjeto* hijo = ((AUEscenaObjeto*)_objetosEscena.elemento[i]);
		NBASSERT(hijo != NULL) AU_OBJETO_ASSERT_IS_VALID(hijo)
		if((hijo->_sceneMask & AUOBJETOESCENA_BIT_ESCONTENEDOR) != 0){
			((AUEscenaContenedor*)hijo)->privEstablecerIdMundoFisica(pIdMundoFisica);
		} else {
			hijo->_idMundoFisica = pIdMundoFisica;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
void AUEscenaContenedor::debugEstablecerBloqueadoActualizandoModelos(const bool bloqueado){
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	//Actualizar datos de contenedor
	AUEscenaObjeto::debugEstablecerBloqueadoActualizandoModelos(bloqueado);
	//Actualizar hijos
	UI16 i; const UI16 conteo = _objetosEscena.conteo;
	for(i = 0; i < conteo; i++){
		AUEscenaObjeto* hijo = ((AUEscenaObjeto*)_objetosEscena.elemento[i]);
		NBASSERT(hijo != NULL) AU_OBJETO_ASSERT_IS_VALID(hijo)
		hijo->debugEstablecerBloqueadoActualizandoModelos(bloqueado);
	}
}
#endif

//Sombras

bool AUEscenaContenedor::preagruparSombrasParaHijos(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaContenedor::preagruparSombrasParaHijos")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return ((_sceneMask & AUOBJETOESCENA_BIT_PREAGRUPAR_SOMBRAS) != 0);
}

void AUEscenaContenedor::establecerPreagruparSombrasParaHijos(bool preagruparSombrasParaHijos){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaContenedor::establecerPreagruparSombrasParaHijos")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	if(preagruparSombrasParaHijos){
		_sceneMask |= AUOBJETOESCENA_BIT_PREAGRUPAR_SOMBRAS;
	} else {
		_sceneMask &= ~AUOBJETOESCENA_BIT_PREAGRUPAR_SOMBRAS;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaContenedor::fijarAgrupadorDeSombras(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaContenedor::fijarAgrupadorDeSombras")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	if((_sceneMask & AUOBJETOESCENA_BIT_AGRUPADOR_SOMBRAS_FIJO) == 0 || _agrupadorSombras!=this){
		fijarAgrupadorDeSombras(this);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaContenedor::fijarAgrupadorDeSombras(AUEscenaObjeto* agrupadorSombras){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaContenedor::fijarAgrupadorDeSombras")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	AU_OBJETO_ASSERT_IS_VALID(agrupadorSombras)
	//NBASSERT(!_debugBloqueadoActualizandoModelos)
	//Evitar que actualice sus nodos hijos cuando no deberia.
	_agrupadorSombras			= agrupadorSombras;
	_sceneMask	|= AUOBJETOESCENA_BIT_AGRUPADOR_SOMBRAS_FIJO;
	UI16 i;
	for(i = 0; i < _objetosEscena.conteo; i++){
		AUEscenaObjeto* hijo = ((AUEscenaObjeto*)_objetosEscena.elemento[i]);
		NBASSERT(hijo != NULL) AU_OBJETO_ASSERT_IS_VALID(hijo)
		if((hijo->_sceneMask & AUOBJETOESCENA_BIT_ESCONTENEDOR) != 0){
			((AUEscenaContenedor*)hijo)->fijarAgrupadorDeSombras(agrupadorSombras);
		} else {
			hijo->_agrupadorSombras				= agrupadorSombras;
			hijo->_sceneMask		|= AUOBJETOESCENA_BIT_AGRUPADOR_SOMBRAS_FIJO;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}


void AUEscenaContenedor::restablecerAgrupadorDeSombras(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaContenedor::restablecerAgrupadorDeSombras")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	//NBASSERT(!_debugBloqueadoActualizandoModelos)
	_agrupadorSombras			= this;
	_sceneMask	&= ~AUOBJETOESCENA_BIT_AGRUPADOR_SOMBRAS_FIJO;
	UI16 i;
	for(i = 0; i < _objetosEscena.conteo; i++){
		AUEscenaObjeto* hijo = ((AUEscenaObjeto*)_objetosEscena.elemento[i]);
		NBASSERT(hijo != NULL) AU_OBJETO_ASSERT_IS_VALID(hijo)
		if((hijo->_sceneMask & AUOBJETOESCENA_BIT_ESCONTENEDOR) != 0){
			((AUEscenaContenedor*)hijo)->restablecerAgrupadorDeSombras();
		} else {
			hijo->_agrupadorSombras				= hijo;
			hijo->_sceneMask		&= ~AUOBJETOESCENA_BIT_AGRUPADOR_SOMBRAS_FIJO;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaContenedor::fijarIdMundoFisica(SI8 idMundoFisicaNuevo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaContenedor::fijarIdMundoFisica")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	//NBASSERT(!_debugBloqueadoActualizandoModelos)
	NBASSERT(idEscena == -1) //Pendiente, permitir que se pueda establecer una vez agregado en escena
	UI16 i;
	for(i = 0; i < _objetosEscena.conteo; i++){
		AUEscenaObjeto* hijo = ((AUEscenaObjeto*)_objetosEscena.elemento[i]);
		NBASSERT(hijo != NULL) AU_OBJETO_ASSERT_IS_VALID(hijo)
		if((hijo->_sceneMask & AUOBJETOESCENA_BIT_ESCONTENEDOR) != 0){
			((AUEscenaContenedor*)hijo)->fijarIdMundoFisica(idMundoFisicaNuevo);
		} else {
			hijo->_idMundoFisica				= idMundoFisicaNuevo;
			hijo->_sceneMask		|= AUOBJETOESCENA_BIT_MUNDO_FISICA_FIJO;
		}
	}
	_idMundoFisica				= idMundoFisicaNuevo;
	_sceneMask	|= AUOBJETOESCENA_BIT_MUNDO_FISICA_FIJO;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaContenedor::restablecerIdMundoFisica(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaContenedor::restablecerIdMundoFisica")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	//NBASSERT(!_debugBloqueadoActualizandoModelos)
	NBASSERT(idEscena == -1) //Pendiente, permitir que se pueda establecer una vez agregado en escena
	SI8 idMundoFisicaNuevo			= -1; if(_contenedor != NULL) idMundoFisicaNuevo = _contenedor->_idMundoFisica;
	//
	UI16 i;
	for(i = 0; i < _objetosEscena.conteo; i++){
		AUEscenaObjeto* hijo = ((AUEscenaObjeto*)_objetosEscena.elemento[i]);
		if((hijo->_sceneMask & AUOBJETOESCENA_BIT_ESCONTENEDOR) != 0){
			((AUEscenaContenedor*)hijo)->fijarIdMundoFisica(idMundoFisicaNuevo);
		} else {
			hijo->_idMundoFisica				= idMundoFisicaNuevo;
			hijo->_sceneMask		&= ~AUOBJETOESCENA_BIT_MUNDO_FISICA_FIJO;
		}
	}
	_idMundoFisica				= idMundoFisicaNuevo;
	_sceneMask	&= ~AUOBJETOESCENA_BIT_MUNDO_FISICA_FIJO;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaContenedor::privActualizarNivelEnEscena(SI32 idEscenaP, SI32 idGrupoP, SI32 indiceCapaP){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaContenedor::privActualizarNivelEnEscena")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	//NBASSERT(!_debugBloqueadoActualizandoModelos)
	//Actualizar datos de contenedor
	{
		AUESCENACONTENEDOR_ACTUALIZAR_NIVEL_ESCENA_DE_OBJETO(this, idEscenaP, idGrupoP, indiceCapaP)
	}
	//Update children
	{
		UI16 i;
		for(i = 0; i < _objetosEscena.conteo; i++){
			AUEscenaObjeto* hijo = ((AUEscenaObjeto*)_objetosEscena.elemento[i]);
			//Actualizar cache de hijo
			if(idEscenaP != -1){
				NBMatriz matrizLocal;	NBMATRIZ_ESTABLECER_SEGUN_TRANSFORMACIONES_ESCENA_OBJETO(matrizLocal, hijo->_propiedades.transformaciones)
				NBMATRIZ_MULTIPLICAR_CON_MATRIZ(hijo->_cacheObjEscena.matrizEscena, _cacheObjEscena.matrizEscena, matrizLocal);
				NBCajaAABB cajaLocal	= hijo->cajaAABBLocalCalculada();
				NBMATRIZ_MULTIPLICAR_CAJAAABB(hijo->_cacheObjEscena.cajaEscena, hijo->_cacheObjEscena.matrizEscena, cajaLocal);
			}
			//Actualizar nivel escena de hijo
			if((hijo->_sceneMask & AUOBJETOESCENA_BIT_ESCONTENEDOR) != 0){
				((AUEscenaContenedor*)hijo)->privActualizarNivelEnEscena(idEscenaP, idGrupoP, indiceCapaP);
			} else {
				AUESCENACONTENEDOR_ACTUALIZAR_NIVEL_ESCENA_DE_OBJETO(hijo, idEscenaP, idGrupoP, indiceCapaP)
			}
		}
	}
	//Notificar post_agregado_fisica
	if(idEscenaP != -1){
		AUESCENACONTENEDOR_DEBUG_VERIFICA_NOTIFICACION_AGREGADO_ESCENA_POST(this)
		this->agregadoEnEscenaConHijosNotificados();
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaContenedor::agregadoEnEscena(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaContenedor::actualizarNivelEnEscena")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaContenedor::quitandoDeEscena(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaContenedor::actualizarNivelEnEscena")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

//PANTALLA

void AUEscenaContenedor::puertoVisionModificado(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaContenedor::puertoVisionModificado")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	//NBASSERT(!_debugBloqueadoActualizandoModelos)
	UI16 i; const UI16 conteo = _objetosEscena.conteo;
	for(i = 0; i < conteo; i++){
		((AUEscenaObjeto*)_objetosEscena.elemento[i])->puertoVisionModificado();
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

//TOCUHES

AUEscenaObjeto* AUEscenaContenedor::consumidorTouchEnPosicion(const NBPunto &posTouchEscena, const UI8 touchFilterMask, AUObjeto** outConsumidorEscuchadorObj, void** outConsumidorEscuchadorInterfaz){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaContenedor::consumidorTouchEnPosicion")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	//NBASSERT(!_debugBloqueadoActualizandoModelos)
	AUEscenaObjeto* consumidor = NULL;
	//Search on children
	{
		SI32 i; for(i = _objetosEscena.conteo - 1; i >= 0 && consumidor == NULL; i--){
			AUEscenaObjeto* hijo = (AUEscenaObjeto*)_objetosEscena.elemento[i];
			if(!hijo->_dormido && hijo->_propiedades.visible){
				consumidor = hijo->consumidorTouchEnPosicion(posTouchEscena, touchFilterMask, outConsumidorEscuchadorObj, outConsumidorEscuchadorInterfaz);
#				ifdef NB_CONFIG_INCLUDE_ASSERTS
				if(consumidor != NULL){
					NBASSERT(consumidor->_propiedades.visible)
					NBASSERT(!consumidor->_dormido)
				}
#				endif
			}
		}
	}
	//Search on myself
	if(consumidor == NULL){
		consumidor = this->AUEscenaObjeto::consumidorTouchEnPosicion(posTouchEscena, touchFilterMask, outConsumidorEscuchadorObj, outConsumidorEscuchadorInterfaz);
#		ifdef NB_CONFIG_INCLUDE_ASSERTS
		if(consumidor != NULL){
			NBASSERT(consumidor->_propiedades.visible)
			NBASSERT(!consumidor->_dormido)
		}
#		endif
	}
	//Print
	/*if(consumidor != NULL){
		const BOOL isChild = (_objetosEscena.indiceDe(consumidor) != -1);
		const BOOL isMe = (consumidor == this);
		if(isChild || isMe){ //Filter, print only if I'm the parent (avoid multiple printings)
			PRINTF_INFO("------------------------------------\n");
			PRINTF_INFO("%s\n", isMe ? "I'm the touch consumer." : "I'm the parent of the touch consumer.\n");
			AUEscenaObjeto* prnt		= consumidor;
			while(prnt != NULL){
				const NBPunto posLcl	= prnt->coordenadaEscenaALocal(posTouchEscena.x, posTouchEscena.y);
				const NBCajaAABB boxLcl	= prnt->cajaAABBLocal();
				PRINTF_INFO("Consumer: '%s' relLcl(%d%%, %d%%) relScn(%d%%, %d%%).\n", prnt->nombreUltimaClase(), (SI32)(100.0f * (posLcl.x - boxLcl.xMin) / (boxLcl.xMax - boxLcl.xMin)), (SI32)(100.0f * (posLcl.y - boxLcl.yMin) / (boxLcl.yMax - boxLcl.yMin)), (SI32)(100.0f * (posTouchEscena.x - _cacheObjEscena.cajaEscena.xMin) / (_cacheObjEscena.cajaEscena.xMax - _cacheObjEscena.cajaEscena.xMin)), (SI32)(100.0f * (posTouchEscena.y - _cacheObjEscena.cajaEscena.yMin) / (_cacheObjEscena.cajaEscena.yMax - _cacheObjEscena.cajaEscena.yMin)));
				prnt = prnt->contenedor();
			}
		}
	}*/
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return consumidor;
}

AUEscenaObjeto* AUEscenaContenedor::consumidorTouchEnPosicionSegunArreglo(const NBPunto &posTouchEscena, const UI8 touchFilterMask, const AUArreglo* arregloFiltroObjetosEscena){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaContenedor::consumidorTouchEnPosicionSegunArreglo")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	AU_OBJETO_ASSERT_IS_VALID(arregloFiltroObjetosEscena)
	//NBASSERT(!_debugBloqueadoActualizandoModelos)
	AUEscenaObjeto* consumidor = NULL;
	//Search on children
	{
		SI32 i; for(i = _objetosEscena.conteo - 1; i >= 0 && consumidor == NULL; i--){
			AUEscenaObjeto* hijo = (AUEscenaObjeto*)_objetosEscena.elemento[i];
			if(!hijo->_dormido && hijo->_propiedades.visible){
				consumidor = hijo->consumidorTouchEnPosicionSegunArreglo(posTouchEscena, touchFilterMask, arregloFiltroObjetosEscena);
#				ifdef NB_CONFIG_INCLUDE_ASSERTS
				if(consumidor != NULL){
					NBASSERT(consumidor->_propiedades.visible)
					NBASSERT(!consumidor->_dormido)
				}
#				endif
			}
		}
	}
	//Search on myself
	if(consumidor == NULL){
		consumidor = AUEscenaObjeto::consumidorTouchEnPosicionSegunArreglo(posTouchEscena, touchFilterMask, arregloFiltroObjetosEscena);
#		ifdef NB_CONFIG_INCLUDE_ASSERTS
		if(consumidor != NULL){
			NBASSERT(consumidor->_propiedades.visible)
			NBASSERT(!consumidor->_dormido)
		}
#		endif
	}
	//Print
	/*if(consumidor != NULL){
		const BOOL isChild = (_objetosEscena.indiceDe(consumidor) != -1);
		const BOOL isMe = (consumidor == this);
		if(isChild || isMe){ //Filter, print only if I'm the parent (avoid multiple printings)
			PRINTF_INFO("------------------------------------\n");
			PRINTF_INFO("%s\n", isMe ? "I'm the touch consumer." : "I'm the parent of the touch consumer.\n");
			AUEscenaObjeto* prnt		= consumidor;
			while(prnt != NULL){
				const NBPunto posLcl	= prnt->coordenadaEscenaALocal(posTouchEscena.x, posTouchEscena.y);
				const NBCajaAABB boxLcl	= prnt->cajaAABBLocal();
				PRINTF_INFO("Consumer: '%s' relLcl(%d%%, %d%%) relScn(%d%%, %d%%).\n", prnt->nombreUltimaClase(), (SI32)(100.0f * (posLcl.x - boxLcl.xMin) / (boxLcl.xMax - boxLcl.xMin)), (SI32)(100.0f * (posLcl.y - boxLcl.yMin) / (boxLcl.yMax - boxLcl.yMin)), (SI32)(100.0f * (posTouchEscena.x - _cacheObjEscena.cajaEscena.xMin) / (_cacheObjEscena.cajaEscena.xMax - _cacheObjEscena.cajaEscena.xMin)), (SI32)(100.0f * (posTouchEscena.y - _cacheObjEscena.cajaEscena.yMin) / (_cacheObjEscena.cajaEscena.yMax - _cacheObjEscena.cajaEscena.yMin)));
				prnt = prnt->contenedor();
			}
		}
	}*/
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return consumidor;
}

//

NBCajaAABB AUEscenaContenedor::cajaAABBLocal(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaContenedor::cajaAABBLocal")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	NBCajaAABB r = cajaAABBLocalCalculada();
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return r;
}

NBCajaAABB AUEscenaContenedor::cajaAABBLocalCalculada(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaContenedor::cajaAABBLocalCalculada")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	SI32 i; NBCajaAABB cajaLocal; NBCAJAAABB_INICIALIZAR(cajaLocal);
	for(i = 0; i < _objetosEscena.conteo; i++){
		AUEscenaObjeto* obj = (AUEscenaObjeto*) _objetosEscena.elemento[i];
		if(obj->_propiedades.visible){
			NBCajaAABB cajaHijo = obj->cajaAABBLocalCalculada(); //cajaAABBLocal
			if(cajaHijo.xMin < cajaHijo.xMax && cajaHijo.yMin < cajaHijo.yMax){
				NBMatriz matrizHijo; NBMATRIZ_ESTABLECER_SEGUN_TRANSFORMACIONES_ESCENA_OBJETO(matrizHijo, obj->_propiedades.transformaciones)
				NBCajaAABB cajaHijoTransformada; NBMATRIZ_MULTIPLICAR_CAJAAABB(cajaHijoTransformada, matrizHijo, cajaHijo);
				NBCAJAAABB_ENVOLVER_CAJA(cajaLocal, cajaHijoTransformada);
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return cajaLocal;
}

void AUEscenaContenedor::actualizarArbolMatricesEscena(STNBSceneModelsResult* dst, const NBPropRenderizado &propsRenderizado, const NBPropHeredadasModelos &propsHeredadas){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaContenedor::actualizarArbolMatricesEscena")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	//NBASSERT(_debugBloqueadoActualizandoModelos)
	//Ensuciar las caches en base a las del padre
	NBPropHeredadasModelos propsHeredar;
	//Update container matrix
	{
		propsHeredar.matrizModificadaPadre = (propsHeredadas.matrizModificadaPadre || (_sceneMask & AUOBJETOESCENA_BIT_MATRIX_LOCAL_CHANGED) != 0);
		if(propsHeredar.matrizModificadaPadre){
			//Actualizar matriz
			NBMatriz matrizLocal; NBMATRIZ_ESTABLECER_SEGUN_TRANSFORMACIONES_ESCENA_OBJETO(matrizLocal, this->_propiedades.transformaciones)
			NBMATRIZ_MULTIPLICAR_CON_MATRIZ(_cacheObjEscena.matrizEscena, propsHeredadas.matrizPadre, matrizLocal);
		}
		propsHeredar.matrizPadre = _cacheObjEscena.matrizEscena;
	}
	//Actualizar modelos de hijos y mi caja de escena
	{
		const BOOL modelModified = (propsHeredar.matrizModificadaPadre || (_sceneMask & AUOBJETOESCENA_BIT_MODEL_LOCAL_CHANGED) != 0);
		const BOOL childModified = (propsHeredar.matrizModificadaPadre || (_sceneMask & (AUOBJETOESCENA_BIT_MATRIX_CHILD_CHANGED | AUOBJETOESCENA_BIT_MODEL_CHILD_CHANGED)) != 0);
		//
		{
			dst->countModels++;
			if(modelModified){
				dst->countModelsDirty++;
			}
		}
		//
		if(modelModified || childModified){
			SI32 i;
			BOOL isSncModelDirty;
			NBMatriz matrizLocal;
			NBCajaAABB cajaEscena, cajaLocal;
			NBCAJAAABB_INICIALIZAR(cajaEscena)
			for(i = 0; i < _objetosEscena.conteo; i++){
				AUEscenaObjeto* hijo = (AUEscenaObjeto*)_objetosEscena.elemento[i];
				if(!hijo->_dormido){
					if((hijo->_sceneMask & AUOBJETOESCENA_BIT_ESCONTENEDOR) != 0){
						((AUEscenaContenedor*)hijo)->actualizarArbolMatricesEscena(dst, propsRenderizado, propsHeredar);
					} else {
						isSncModelDirty = ((hijo->_sceneMask & (AUOBJETOESCENA_BIT_MODEL_LOCAL_CHANGED | AUOBJETOESCENA_BIT_MODEL_CHILD_CHANGED)) != 0);
						//Ensuciar las caches en base a las del padre
						//Update object matrix
						if(propsHeredar.matrizModificadaPadre || (hijo->_sceneMask & AUOBJETOESCENA_BIT_MATRIX_LOCAL_CHANGED) != 0){
							NBMATRIZ_ESTABLECER_SEGUN_TRANSFORMACIONES_ESCENA_OBJETO(matrizLocal, hijo->_propiedades.transformaciones)
							NBMATRIZ_MULTIPLICAR_CON_MATRIZ(hijo->_cacheObjEscena.matrizEscena, propsHeredar.matrizPadre, matrizLocal);
							isSncModelDirty = TRUE;
						}
						//Actualizar caja local y de escena
						if(isSncModelDirty){
							cajaLocal = hijo->cajaAABBLocalCalculada();
							NBMATRIZ_MULTIPLICAR_CAJAAABB(hijo->_cacheObjEscena.cajaEscena, hijo->_cacheObjEscena.matrizEscena, cajaLocal);
							dst->countModelsDirty++;
						}
						dst->countModels++;
					}
				}
				//Envolver cajas de hijos
				if(hijo->_propiedades.visible){
					if(hijo->_cacheObjEscena.cajaEscena.xMin < hijo->_cacheObjEscena.cajaEscena.xMax && hijo->_cacheObjEscena.cajaEscena.yMin < hijo->_cacheObjEscena.cajaEscena.yMax){
						NBCAJAAABB_ENVOLVER_CAJA(cajaEscena, hijo->_cacheObjEscena.cajaEscena);
					}
				}
				//Update flags
				hijo->_sceneMask &= ~(AUOBJETOESCENA_BIT_MATRIX_LOCAL_CHANGED | AUOBJETOESCENA_BIT_MATRIX_CHILD_CHANGED | AUOBJETOESCENA_BIT_MODEL_LOCAL_CHANGED | AUOBJETOESCENA_BIT_MODEL_CHILD_CHANGED);
#				ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
				hijo->_cacheObjEscena.debugCacheActualizada = true;
#				endif
			}
			_cacheObjEscena.cajaEscena = cajaEscena;
		}
	}
	//Update flags
	_sceneMask &= ~(AUOBJETOESCENA_BIT_MATRIX_LOCAL_CHANGED | AUOBJETOESCENA_BIT_MATRIX_CHILD_CHANGED | AUOBJETOESCENA_BIT_MODEL_LOCAL_CHANGED | AUOBJETOESCENA_BIT_MODEL_CHILD_CHANGED);
#	ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
	_cacheObjEscena.debugCacheActualizada = true;
#	endif
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaContenedor::actualizarArbolModelosGL(const NBPropRenderizado &propsRenderizado, const NBPropIluminacion &propsIluminacion, const NBPropHeredadasRender &propsHeredadas){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaContenedor::actualizarArbolModelosGL")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	//NBASSERT(_debugBloqueadoActualizandoModelos)
	NBASSERT(idEscena != -1) //Solo si esta en escena
	#ifdef CONFIG_NB_GESTOR_ESCENAS_VERIFICAR_FRONTERAS_BLOQUES_DATOS_RENDER
	{
		const SI32 indVerifIni			= propsRenderizado.bytesDatosModelos->conteo;
		propsRenderizado.bytesDatosModelos->reservarRegistrosAlFinal(sizeof(UI32));
		((UI32*)&propsRenderizado.bytesDatosModelos->elemento[indVerifIni])[0] = NBGESTORESCENA_VAL_VERIF_BLOQUE_DATOS_RENDER_INI;
	}
	#endif
	PTRfunCmdsGL funcionComandosGL	= NULL;
	SI32 indiceDatosModelo			= propsRenderizado.bytesDatosModelos->conteo;
	funcionComandosGL				= this->actualizarModeloGL(propsRenderizado, propsIluminacion, propsHeredadas);
	#ifdef CONFIG_NB_GESTOR_ESCENAS_VERIFICAR_FRONTERAS_BLOQUES_DATOS_RENDER
	{
		const SI32 indVerifFin			= propsRenderizado.bytesDatosModelos->conteo;
		propsRenderizado.bytesDatosModelos->reservarRegistrosAlFinal(sizeof(UI32));
		((UI32*)&propsRenderizado.bytesDatosModelos->elemento[indVerifFin])[0] = NBGESTORESCENA_VAL_VERIF_BLOQUE_DATOS_RENDER_FIN;
	}
	#endif
	if(funcionComandosGL != NULL){
		STPropRenderModelo renderDatos;
		renderDatos.indiceDatos		= indiceDatosModelo;
		renderDatos.funcEnvComadosGL= funcionComandosGL;
		propsRenderizado.renderModelos->agregarElemento(renderDatos);
	}
	//Definir propiedades a heredar a hijos
	NBPropHeredadasRender propsHeredar;
	propsHeredar.iluminoDependenciaPadre		= (propsHeredadas.iluminoDependenciaPadre && _iluminoDependiente);
	propsHeredar.colorPadre.r					= propsHeredadas.colorPadre.r*((float)_propiedades.color8.r/255.0f);
	propsHeredar.colorPadre.g					= propsHeredadas.colorPadre.g*((float)_propiedades.color8.g/255.0f);
	propsHeredar.colorPadre.b					= propsHeredadas.colorPadre.b*((float)_propiedades.color8.b/255.0f);
	propsHeredar.colorPadre.a					= propsHeredadas.colorPadre.a*((float)_propiedades.color8.a/255.0f);
	#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
	//Validar agrupador de sombras
	if(_contenedor != NULL){
		AUEscenaContenedor* contenedorPadre		= (AUEscenaContenedor*)_contenedor;
		if((contenedorPadre->_sceneMask & AUOBJETOESCENA_BIT_AGRUPADOR_SOMBRAS_FIJO) != 0){
			NBASSERT(contenedorPadre->_agrupadorSombras == _agrupadorSombras)
		}
	}
	#endif
	//Recorrer hijos
	UI16 i; for(i = 0; i < _objetosEscena.conteo; i++){
		AUEscenaObjeto* hijo = (AUEscenaObjeto*)_objetosEscena.elemento[i];
		//NBASSERT(hijo->contenedor()==this)
		//NBASSERT(_objetosEscena.conteoAparicionesDe(hijo)==1)
		NBCajaAABB cajaEscenaHijo = hijo->_cacheObjEscena.cajaEscena;
		if(hijo->_propiedades.visible && NBCAJAAABB_AREA_INTERNA_INTERSECTA_CAJA(propsRenderizado.cajaProyeccion, cajaEscenaHijo)){
			if((hijo->_sceneMask & AUOBJETOESCENA_BIT_ESCONTENEDOR) != 0){
				((AUEscenaContenedor*)hijo)->actualizarArbolModelosGL(propsRenderizado, propsIluminacion, propsHeredar);
			} else {
				NBASSERT(hijo->idEscena != -1) //Validando cuerpos que no tiene su idEscena establecido
				#ifdef CONFIG_NB_GESTOR_ESCENAS_VERIFICAR_FRONTERAS_BLOQUES_DATOS_RENDER
				{
					const SI32 indVerifIni			= propsRenderizado.bytesDatosModelos->conteo;
					propsRenderizado.bytesDatosModelos->reservarRegistrosAlFinal(sizeof(UI32));
					((UI32*)&propsRenderizado.bytesDatosModelos->elemento[indVerifIni])[0] = NBGESTORESCENA_VAL_VERIF_BLOQUE_DATOS_RENDER_INI;
				}
				#endif
				SI32 indiceDatosModelo	= propsRenderizado.bytesDatosModelos->conteo;
				PTRfuncEnviarComandosGL funcionComandosGL = hijo->actualizarModeloGL(propsRenderizado, propsIluminacion, propsHeredar);
				#ifdef CONFIG_NB_GESTOR_ESCENAS_VERIFICAR_FRONTERAS_BLOQUES_DATOS_RENDER
				{
					const SI32 indVerifFin			= propsRenderizado.bytesDatosModelos->conteo;
					propsRenderizado.bytesDatosModelos->reservarRegistrosAlFinal(sizeof(UI32));
					((UI32*)&propsRenderizado.bytesDatosModelos->elemento[indVerifFin])[0] = NBGESTORESCENA_VAL_VERIF_BLOQUE_DATOS_RENDER_FIN;
				}
				#endif
				if(funcionComandosGL != NULL){
					STPropRenderModelo renderDatos;
					renderDatos.indiceDatos		= indiceDatosModelo;
					renderDatos.funcEnvComadosGL= funcionComandosGL;
					propsRenderizado.renderModelos->agregarElemento(renderDatos);
				}
			}
			#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
			//Validar agrupador de sombras
			if((_sceneMask & AUOBJETOESCENA_BIT_AGRUPADOR_SOMBRAS_FIJO) != 0){
				NBASSERT((hijo->_sceneMask & AUOBJETOESCENA_BIT_AGRUPADOR_SOMBRAS_FIJO) != 0)
				NBASSERT(hijo->_agrupadorSombras == _agrupadorSombras)
			}
			#endif
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

PTRfunCmdsGL AUEscenaContenedor::actualizarModeloGL(const NBPropRenderizado &propsRenderizado, const NBPropIluminacion &propsIluminacion, const NBPropHeredadasRender &propsHeredadas){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaContenedor::actualizarModeloGL")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return NULL;
}

//
AUOBJMETODOS_CLASESID_MULTICLASE(AUEscenaContenedor, AUEscenaObjeto)
AUOBJMETODOS_CLASESNOMBRES_MULTICLASE(AUEscenaContenedor, "AUEscenaContenedor", AUEscenaObjeto)
AUOBJMETODOS_CLONAR_THIS(AUEscenaContenedor)












