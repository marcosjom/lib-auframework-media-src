
#include "AUFrameworkMediaStdAfx.h"
#include "AUEscenaSpriteWithLoader.h"
#include "NBGestorAnimadores.h"
#include "AUEscenaSprite.h"

//-------------------------------------
//--- CONSTRUCTORES Y DESTRUCTORES ----
//-------------------------------------

AUEscenaSpriteWithLoader::AUEscenaSpriteWithLoader(AUTextura* texLoading, AUEscenaObjeto* objLoaded, const char* loadingIcon, const float loadingIconRotSpeed) : AUEscenaContenedorLimitado() {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSpriteWithLoader::AUEscenaSpriteWithLoader")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaSpriteWithLoader")
	//
	_texLoading				= texLoading; if(_texLoading != NULL) _texLoading->retener(NB_RETENEDOR_THIS);
	_texLoadingLastState	= ENTexturaEstadoCarga_Conteo;
	//
	{
		_visualWhenLoaded		= objLoaded; if(_visualWhenLoaded != NULL) _visualWhenLoaded->retener(NB_RETENEDOR_THIS);
		_visualWhenLoadedLayer	= new(this) AUEscenaContenedor();
		if(_visualWhenLoaded != NULL){
			_visualWhenLoadedLayer->agregarObjetoEscena(_visualWhenLoaded);
		}
		this->agregarObjetoEscena(_visualWhenLoadedLayer);
	}
	//
	{
		_visualWhileLoading		= NULL;
		if(loadingIcon != NULL){
			AUTextura* loadingiconTex = NBGestorTexturas::texturaDesdeArchivo(loadingIcon);
			if(loadingiconTex != NULL){
				_visualWhileLoading		= new(this) AUEscenaSprite(loadingiconTex);
			}
		}
		_visualWhileLoadingLayer= new(this) AUEscenaContenedor();
		if(_visualWhileLoading != NULL){
			_visualWhileLoadingLayer->agregarObjetoEscena(_visualWhileLoading);
		}
		this->agregarObjetoEscena(_visualWhileLoadingLayer);
		_visualWhileLoadingRotateSpeed = loadingIconRotSpeed;
	}
	//
	this->privUpdateSceneObjects();
	//
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUEscenaSpriteWithLoader::~AUEscenaSpriteWithLoader(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSpriteWithLoader::~AUEscenaSpriteWithLoader")
	if(_texLoading != NULL) _texLoading->liberar(NB_RETENEDOR_THIS); _texLoading = NULL;
	//
	if(_visualWhenLoaded != NULL) _visualWhenLoaded->liberar(NB_RETENEDOR_THIS); _visualWhenLoaded = NULL;
	if(_visualWhenLoadedLayer != NULL) _visualWhenLoadedLayer->liberar(NB_RETENEDOR_THIS); _visualWhenLoadedLayer = NULL;
	if(_visualWhileLoading != NULL) _visualWhileLoading->liberar(NB_RETENEDOR_THIS); _visualWhileLoading = NULL;
	if(_visualWhileLoadingLayer != NULL) _visualWhileLoadingLayer->liberar(NB_RETENEDOR_THIS); _visualWhileLoadingLayer = NULL;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

//

AUEscenaObjeto* AUEscenaSpriteWithLoader::visualObjWhenLoaded() const {
	return _visualWhenLoaded;
}


void AUEscenaSpriteWithLoader::origanizeVisualObjects(){
	this->privUpdateSceneObjects();
}

//

void AUEscenaSpriteWithLoader::agregadoEnEscena(){
	NBGestorAnimadores::agregarAnimador(this, this);
}

void AUEscenaSpriteWithLoader::quitandoDeEscena(){
	NBGestorAnimadores::quitarAnimador(this);
}

//

void AUEscenaSpriteWithLoader::privUpdateSceneObjects(){
	//Loading state
	if(_texLoading != NULL){
		_texLoadingLastState = _texLoading->estadoCarga();
	}
	//Center
	this->privCenterObject(_visualWhenLoaded, _visualWhileLoading);
	//Visible
	if(_visualWhenLoadedLayer != NULL){
		if(_texLoading != NULL && _texLoadingLastState != ENTexturaEstadoCarga_Cargada){
			_visualWhenLoadedLayer->establecerMultiplicadorAlpha8(0);
		}
	}
	if(_visualWhileLoadingLayer != NULL){
		_visualWhileLoadingLayer->establecerVisible(_texLoading != NULL && _texLoadingLastState != ENTexturaEstadoCarga_Cargada);
	}
	//Limits
	if(_visualWhenLoaded != NULL){
		NBCajaAABB refBoxT;
		{
			const NBCajaAABB refBox = _visualWhenLoaded->cajaAABBLocalCalculada();
			const NBMatriz refMtrx = _visualWhenLoaded->matrizLocal();
			NBMATRIZ_MULTIPLICAR_CAJAAABB(refBoxT, refMtrx, refBox);
		}
		this->establecerLimites(refBoxT);
	} else if(_visualWhileLoading != NULL){
		this->establecerLimites(_visualWhileLoading->cajaAABBLocalCalculada());
	}
}

void AUEscenaSpriteWithLoader::privCenterObject(AUEscenaObjeto* objRef, AUEscenaObjeto* objToCenter){
	if(objRef != NULL && objToCenter != NULL){
		NBCajaAABB refBoxT;
		{
			const NBCajaAABB refBox = objRef->cajaAABBLocalCalculada();
			const NBMatriz refMtrx = objRef->matrizLocal();
			NBMATRIZ_MULTIPLICAR_CAJAAABB(refBoxT, refMtrx, refBox);
		}
		objToCenter->establecerTraslacion(
			refBoxT.xMin + ((refBoxT.xMax - refBoxT.xMin) * 0.5f),
			refBoxT.yMin + ((refBoxT.yMax - refBoxT.yMin) * 0.5f)
		);
	}
}

bool AUEscenaSpriteWithLoader::privTickAnimateObjectVisibility(AUEscenaObjeto* obj, const float secs, const bool mustBeVisible, const bool hideWhenAlpha0){
	bool r = false;
	if(obj != NULL){
		r = true;
		if(mustBeVisible){
			if(!obj->visible()){
				obj->establecerMultiplicadorAlpha8(0);
				obj->establecerVisible(true);
			}
			const SI32 alpha0 = obj->multiplicadorAlpha8();
			SI32 alpha1 = alpha0 + (255 * 4 * secs);
			if(alpha0 == alpha1) alpha1++; //ensure any advance
			if(alpha1 >= 255){
				alpha1 = 255;
				r = false;
			}
			obj->establecerMultiplicadorAlpha8(alpha1);
		} else {
			if(!obj->visible()){
				r = false;
			} else {
				const SI32 alpha0 = obj->multiplicadorAlpha8();
				SI32 alpha1 = alpha0 - (255 * 4 * secs);
				if(alpha0 == alpha1) alpha1--; //ensure any advance
				if(alpha1 <= 0){
					alpha1 = 0;
					if(hideWhenAlpha0){
						obj->establecerVisible(false);
					}
					r = false;
				}
				obj->establecerMultiplicadorAlpha8(alpha1);
			}
		}
	}
	return r;
}

void AUEscenaSpriteWithLoader::tickAnimacion(const float secsTransucrridos){
	bool anyAnimation = false;
	//
	if(_texLoading != NULL){
		const ENTexturaEstadoCarga newState = _texLoading->estadoCarga();
		if(_texLoadingLastState != newState){
			_texLoadingLastState = newState;
		}
	}
	//Loading icon rotation
	if(_visualWhileLoadingRotateSpeed != 0.0f){
		if(_visualWhileLoading->visible()){
			_visualWhileLoading->establecerRotacionNormalizada(_visualWhileLoading->rotacion() + (_visualWhileLoadingRotateSpeed * secsTransucrridos));
			anyAnimation = true;
		}
	}
	//Animate visual objects
	{
		if(_visualWhenLoadedLayer != NULL){
			if(this->privTickAnimateObjectVisibility(_visualWhenLoadedLayer, secsTransucrridos, (_texLoading == NULL || _texLoadingLastState == ENTexturaEstadoCarga_Cargada), false)){
				anyAnimation = true;
			}
		}
		if(_visualWhileLoadingLayer != NULL){
			if(this->privTickAnimateObjectVisibility(_visualWhileLoadingLayer, secsTransucrridos, (_texLoading != NULL && _texLoadingLastState != ENTexturaEstadoCarga_Cargada), true)){
				anyAnimation = true;
			}
		}
	}
	//Disable animator if texture was loaded?
	if(!anyAnimation){
		//
	}
}

//

AUOBJMETODOS_CLASESID_MULTICLASE(AUEscenaSpriteWithLoader, AUEscenaObjeto)
AUOBJMETODOS_CLASESNOMBRES_MULTICLASE(AUEscenaSpriteWithLoader, "AUEscenaSpriteWithLoader", AUEscenaContenedorLimitado)
AUOBJMETODOS_CLONAR_NULL(AUEscenaSpriteWithLoader)





