
#include "AUFrameworkMediaStdAfx.h"
#include "AUEscenaSpritePorcion.h"

//-------------------------------------
//--- CONSTRUCTORES Y DESTRUCTORES ----
//-------------------------------------

AUEscenaSpritePorcion::AUEscenaSpritePorcion() : AUEscenaSprite() {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSpritePorcion::AUEscenaSpritePorcion")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaSpritePorcion")
	_porcionTextura.xMin		= 0.0f;
	_porcionTextura.yMin		= 0.0f;
	_porcionTextura.xMax		= 1.0f;
	_porcionTextura.yMax		= 1.0f;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUEscenaSpritePorcion::AUEscenaSpritePorcion(NBTamano tamano) : AUEscenaSprite(tamano) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSpritePorcion::AUEscenaSpritePorcion")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaSpritePorcion")
	_porcionTextura.xMin		= 0.0f;
	_porcionTextura.yMin		= 0.0f;
	_porcionTextura.xMax		= 1.0f;
	_porcionTextura.yMax		= 1.0f;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUEscenaSpritePorcion::AUEscenaSpritePorcion(float ancho, float alto) : AUEscenaSprite(ancho, alto) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSpritePorcion::AUEscenaSpritePorcion")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaSpritePorcion")
	_porcionTextura.xMin		= 0.0f;
	_porcionTextura.yMin		= 0.0f;
	_porcionTextura.xMax		= 1.0f;
	_porcionTextura.yMax		= 1.0f;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUEscenaSpritePorcion::AUEscenaSpritePorcion(float x, float y, float ancho, float alto) : AUEscenaSprite(x, y, ancho, alto){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSpritePorcion::AUEscenaSpritePorcion")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaSpritePorcion")
	_porcionTextura.xMin		= 0.0f;
	_porcionTextura.yMin		= 0.0f;
	_porcionTextura.xMax		= 1.0f;
	_porcionTextura.yMax		= 1.0f;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUEscenaSpritePorcion::AUEscenaSpritePorcion(AUTextura* textura) : AUEscenaSprite(textura){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSpritePorcion::AUEscenaSpritePorcion")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaSpritePorcion")
	_porcionTextura.xMin		= 0.0f;
	_porcionTextura.yMin		= 0.0f;
	_porcionTextura.xMax		= 1.0f;
	_porcionTextura.yMax		= 1.0f;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUEscenaSpritePorcion::AUEscenaSpritePorcion(AUEscenaSpritePorcion* otra) : AUEscenaSprite(otra) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSpritePorcion::AUEscenaSpritePorcion")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaSpritePorcion")
	_porcionTextura.xMin		= otra->_porcionTextura.xMin;
	_porcionTextura.yMin		= otra->_porcionTextura.yMin;
	_porcionTextura.xMax		= otra->_porcionTextura.xMax;
	_porcionTextura.yMax		= otra->_porcionTextura.yMax;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUEscenaSpritePorcion::~AUEscenaSpritePorcion(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSpritePorcion::~AUEscenaSprite")
	//Nada
	AU_GESTOR_PILA_LLAMADAS_POP_2
}
 
//-------------------------------
//--- FUNCIONES DE INSTANCIA ----
//-------------------------------

NBCajaAABB AUEscenaSpritePorcion::porcionTextura() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSpritePorcion::porcionTextura")
	NBASSERT(_porcionTextura.xMin <= _porcionTextura.xMax)
	NBASSERT(_porcionTextura.yMin <= _porcionTextura.yMax)
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _porcionTextura;
}

void AUEscenaSpritePorcion::establecerPorcionTextura(float relXMin, float relXMax, float relYMin, float relYMax){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSpritePorcion::establecerPorcionTextura")
	NBASSERT(relXMin <= relXMax)
	NBASSERT(relYMin <= relYMax)
	if(_porcionTextura.xMin != relXMin || _porcionTextura.yMin != relYMin || _porcionTextura.xMax != relXMax || _porcionTextura.yMax != relYMax){
		_porcionTextura.xMin = relXMin;
		_porcionTextura.yMin = relYMin;
		_porcionTextura.xMax = relXMax;
		_porcionTextura.yMax = relYMax;
		//
		if(this->idEscena >= 0){
			this->setModelChangedFlag();
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

STNBRectI16 AUEscenaSpritePorcion::privGetTextureRect(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSpritePorcion::privGetTextureRect")
	STNBRectI16 r;
	r.x = r.y = r.width = r.height = 0;
	if(_textura != NULL){
		const NBRectanguloP<UI16> rect = _textura->areaEnAtlas();
		r.x			= rect.x + (_porcionTextura.xMin * (float)rect.ancho);
		r.y			= rect.y + (_porcionTextura.yMin * (float)rect.alto);
		r.width		= ((_porcionTextura.xMax - _porcionTextura.xMin) * (float)rect.ancho);
		r.height	= ((_porcionTextura.yMax - _porcionTextura.yMin) * (float)rect.alto);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return r;
}

void AUEscenaSpritePorcion::privEstablecerCoordenadasTextura(NBVerticeGL* verticesGL){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSpritePorcion::privEstablecerCoordenadasTextura")
	if(_textura != NULL){
		_textura->cargarCoordenadasTextura(verticesGL, _porcionTextura.xMin, _porcionTextura.xMax, _porcionTextura.yMin, _porcionTextura.yMax);
	} else {
		NBPUNTO_ESTABLECER(verticesGL[0].tex, 0.0f, 0.0f);
		NBPUNTO_ESTABLECER(verticesGL[1].tex, 0.0f, 0.0f);
		NBPUNTO_ESTABLECER(verticesGL[2].tex, 0.0f, 0.0f);
		NBPUNTO_ESTABLECER(verticesGL[3].tex, 0.0f, 0.0f);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

//

bool AUEscenaSpritePorcion::agregarXmlInternoEn(AUEscenaSpritePorcion* sprite, AUCadenaLargaMutable8* guardarEn, const char* espaciosBaseIzq){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSpritePorcion::agregarXmlInternoEn")
	AUEscenaSprite::agregarXmlInternoEn(sprite, guardarEn, espaciosBaseIzq);
	guardarEn->agregarConFormato("%s<porcTex><xMin>%f</xMin><xMax>%f</xMax><yMin>%f</yMin><yMax>%f</yMax></porcTex>\r\n", espaciosBaseIzq, sprite->_porcionTextura.xMin, sprite->_porcionTextura.xMax, sprite->_porcionTextura.yMin, sprite->_porcionTextura.yMax);
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return true;
}

bool AUEscenaSpritePorcion::interpretarXmlInterno(AUEscenaSpritePorcion* cargarEn, const AUDatosXML* datosXml, const sNodoXML* nodoXml){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSpritePorcion::interpretarXmlInterno")
	bool exito = false;
	if(AUEscenaSprite::interpretarXmlInterno(cargarEn, datosXml, nodoXml)){
		float xMin = 0.0f, yMin = 0.0f, xMax = 1.0f, yMax = 1.0f;
		const sNodoXML* nodoPorcionesTextura = datosXml->nodoHijo("porcTex", nodoXml);
		if(nodoPorcionesTextura != NULL){
			xMin		= datosXml->nodoHijo("xMin", 0.0f, nodoPorcionesTextura);
			xMax		= datosXml->nodoHijo("xMax", 1.0f, nodoPorcionesTextura);
			yMin		= datosXml->nodoHijo("yMin", 0.0f, nodoPorcionesTextura);
			yMax		= datosXml->nodoHijo("yMax", 1.0f, nodoPorcionesTextura);
		}
		cargarEn->_porcionTextura.xMin	= xMin;
		cargarEn->_porcionTextura.xMax	= xMax;
		cargarEn->_porcionTextura.yMin	= yMin;
		cargarEn->_porcionTextura.yMax	= yMax;
		exito = true;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

bool AUEscenaSpritePorcion::agregarBitsInternosEn(AUEscenaSpritePorcion* sprite, AUArchivo* guardarEn){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSpritePorcion::agregarBitsInternosEn")
	bool exito = false;
	//
	if(AUEscenaSprite::agregarBitsInternosEn(sprite, guardarEn)){
		guardarEn->escribir(&sprite->_porcionTextura, sizeof(sprite->_porcionTextura), 1, guardarEn);
		exito = true;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

bool AUEscenaSpritePorcion::interpretarBitsInternos(AUEscenaSpritePorcion* cargarEn, AUArchivo* cargarDe){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSpritePorcion::interpretarBitsInternos")
	bool exito = false;
	if(AUEscenaSprite::interpretarBitsInternos(cargarEn, cargarDe)){
		cargarDe->leer(&cargarEn->_porcionTextura, sizeof(cargarEn->_porcionTextura), 1, cargarDe);
		exito = true;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

//
AUOBJMETODOS_CLASESID_MULTICLASE(AUEscenaSpritePorcion, AUEscenaSprite)
AUOBJMETODOS_CLASESNOMBRES_MULTICLASE(AUEscenaSpritePorcion, "AUEscenaSpritePorcion", AUEscenaSprite)
AUOBJMETODOS_CLONAR_THIS(AUEscenaSpritePorcion)







