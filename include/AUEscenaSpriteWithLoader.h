#ifndef AUEscenaSpriteWithLoader_H_INCLUIDO
#define AUEscenaSpriteWithLoader_H_INCLUIDO

#include "AUEscenaObjeto.h"
#include "AUEscenaContenedorLimitado.h"
#include "NBAnimador.h"

class AUEscenaSpriteWithLoader : public AUEscenaContenedorLimitado, public NBAnimador {
	public:
		AUEscenaSpriteWithLoader(AUTextura* texLoading, AUEscenaObjeto* objLoaded, const char* loadingIcon, const float loadingIconRotSpeed);
		virtual ~AUEscenaSpriteWithLoader();
		//
		AUEscenaObjeto*	visualObjWhenLoaded() const;
		void	origanizeVisualObjects();
		//
		void	tickAnimacion(const float secsTransucrridos);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	protected:
		//
		AUTextura*			_texLoading;
		ENTexturaEstadoCarga _texLoadingLastState;
		//
		AUEscenaObjeto*		_visualWhenLoaded;
		AUEscenaContenedor*	_visualWhenLoadedLayer;
		AUEscenaObjeto*		_visualWhileLoading;
		AUEscenaContenedor*	_visualWhileLoadingLayer;
		float				_visualWhileLoadingRotateSpeed;
		//
		virtual void		agregadoEnEscena();
		virtual void		quitandoDeEscena();
		static bool			privTickAnimateObjectVisibility(AUEscenaObjeto* obj, const float secs, const bool mustBeVisible, const bool hideWhenAlpha0);
		static void			privCenterObject(AUEscenaObjeto* objRef, AUEscenaObjeto* objToCenter);
		void				privUpdateSceneObjects();
	
}; 

#endif