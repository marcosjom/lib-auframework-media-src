 
#include "AUFrameworkMediaStdAfx.h"
#include "AUEscenaContenedorRaiz.h"

//-------------------------------------
//--- CONSTRUCTORES Y DESTRUCTORES ----
//-------------------------------------

AUEscenaContenedorRaiz::AUEscenaContenedorRaiz()
	: AUEscenaContenedor()
	, _topItf(NULL)
	, _viewPort(NBST_P(STNBRectI, 0, 0, 0, 0 ))
	, _sceneBox(NBST_P(STNBAABox, 0.0f, 0.0f, 0.0f, 0.0f ))
{
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaContenedorRaiz::AUEscenaContenedorRaiz")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaContenedorRaiz")
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUEscenaContenedorRaiz::AUEscenaContenedorRaiz(AUEscenaContenedorRaiz* other)
	: AUEscenaContenedor(other)
	, _topItf(NULL)
	, _viewPort(other->_viewPort)
	, _sceneBox(other->_sceneBox)
{
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaContenedorRaiz::AUEscenaContenedorRaiz")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaContenedorRaiz")
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUEscenaContenedorRaiz::~AUEscenaContenedorRaiz(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaContenedorRaiz::~AUEscenaContenedorRaiz")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	_topItf = NULL;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

//

void AUEscenaContenedorRaiz::setTopItf(AUEscenaTopItf* topItf){
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	_topItf = topItf;
}

void AUEscenaContenedorRaiz::setSceneBox(const STNBRectI viewPort, const STNBAABox sceneBox){
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	_viewPort = viewPort;
	_sceneBox = sceneBox;
}

//

BOOL AUEscenaContenedorRaiz::showQuickTipTextScene(const STNBPoint scenePos, const char* text, AUTextura* iconLeft, AUTextura* iconRight, const float secsShow, const ENEscenaTipAim aimDir){
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	AU_OBJETO_ASSERT_IS_VALID(iconLeft)
	AU_OBJETO_ASSERT_IS_VALID(iconRight)
	BOOL r = FALSE;
	if(_topItf != NULL){
		if(_sceneBox.xMin != _sceneBox.xMax && _sceneBox.yMin != _sceneBox.yMax){
			const STNBPoint viewPosF = {
				_viewPort.x + (_viewPort.width * (scenePos.x - _sceneBox.xMin) / (_sceneBox.xMax - _sceneBox.xMin))
				, _viewPort.y + _viewPort.height - (_viewPort.height * (scenePos.y - _sceneBox.yMin) / (_sceneBox.yMax - _sceneBox.yMin)) //y-inverted
			};
			r = _topItf->topShowQuickTipTextAtViewPort(NBST_P(STNBPointI, (SI32)viewPosF.x, (SI32)viewPosF.y ), text, iconLeft, iconRight, secsShow, aimDir);
		}
	}
	return r;
}

BOOL AUEscenaContenedorRaiz::addToTop(AUEscenaObjeto* obj){
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	AU_OBJETO_ASSERT_IS_VALID(obj)
	BOOL r = FALSE;
	if(_topItf != NULL){
		r = _topItf->topAddObject(obj);
	}
	return r;
}

STNBRectI AUEscenaContenedorRaiz::viewPortForSceneBox(const STNBAABox box){
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	STNBRectI r = NBST_P(STNBRectI, 0, 0, 0, 0 );
	if(_sceneBox.xMin != _sceneBox.xMax && _sceneBox.yMin != _sceneBox.yMax){
		r.x			= _viewPort.x + (_viewPort.width * (box.xMin - _sceneBox.xMin) / (_sceneBox.xMax - _sceneBox.xMin));
		r.y			= _viewPort.y + _viewPort.height - (_viewPort.height * (box.yMax - _sceneBox.yMin) / (_sceneBox.yMax - _sceneBox.yMin)); //y-inverted
		r.width		= (float)_viewPort.width * ((box.xMax - box.xMin) / (_sceneBox.xMax - _sceneBox.xMin));
		r.height	= (float)_viewPort.height * ((box.yMax - box.yMin) / (_sceneBox.yMax - _sceneBox.yMin));
	}
	return r;
}

//

AUOBJMETODOS_CLASESID_MULTICLASE(AUEscenaContenedorRaiz, AUEscenaContenedor)
AUOBJMETODOS_CLASESNOMBRES_MULTICLASE(AUEscenaContenedorRaiz, "AUEscenaContenedorRaiz", AUEscenaContenedor)
AUOBJMETODOS_CLONAR_THIS(AUEscenaContenedorRaiz)












