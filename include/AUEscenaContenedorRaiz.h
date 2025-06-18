#ifndef AUEscenaContenedorRaiz_H_INCLUIDO
#define AUEscenaContenedorRaiz_H_INCLUIDO

#include "AUEscenaContenedor.h"
#include "AUEscenaTopItf.h"
#include "nb/2d/NBRect.h"
#include "nb/2d/NBAABox.h"

class AUEscenaContenedorRaiz : public AUEscenaContenedor {
	public:
		AUEscenaContenedorRaiz();
		AUEscenaContenedorRaiz(AUEscenaContenedorRaiz* other);
		virtual ~AUEscenaContenedorRaiz();
		//
		void		setTopItf(AUEscenaTopItf* topItf);
		void		setSceneBox(const STNBRectI viewPort, const STNBAABox sceneBox);
		//
		virtual BOOL		showQuickTipTextScene(const STNBPoint scenePos, const char* text, AUTextura* iconLeft, AUTextura* iconRight, const float secsShow, const ENEscenaTipAim aimDir);
		virtual BOOL		addToTop(AUEscenaObjeto* obj);
		virtual STNBRectI	viewPortForSceneBox(const STNBAABox box);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	private:
		AUEscenaTopItf*		_topItf;
		STNBRectI			_viewPort;
		STNBAABox			_sceneBox;
};
 
#endif
