
#include "AUFrameworkMediaStdAfx.h"
#include "AUEscenaSpriteMascara.h"

//-------------------------------------
//--- CONSTRUCTORES Y DESTRUCTORES ----
//-------------------------------------

AUEscenaSpriteMascara::AUEscenaSpriteMascara() : AUEscenaObjeto() {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSpriteMascara::AUEscenaSpriteMascara")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaSpriteMascara")
	privInlineInicializar(0.0f, 0.0f, 0.0f, 0.0f);
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUEscenaSpriteMascara::AUEscenaSpriteMascara(NBTamano tamano) : AUEscenaObjeto() {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSpriteMascara::AUEscenaSpriteMascara")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaSpriteMascara")
	privInlineInicializar(tamano.ancho / -2.0f, tamano.alto / -2.0f, tamano.ancho, tamano.alto);
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUEscenaSpriteMascara::AUEscenaSpriteMascara(float ancho, float alto) : AUEscenaObjeto() {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSpriteMascara::AUEscenaSpriteMascara")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaSpriteMascara")
	privInlineInicializar(ancho / -2.0f, alto / -2.0f, ancho, alto);
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUEscenaSpriteMascara::AUEscenaSpriteMascara(float x, float y, float ancho, float alto) : AUEscenaObjeto(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSpriteMascara::AUEscenaSpriteMascara")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaSpriteMascara")
	privInlineInicializar(x, y, ancho, alto);
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUEscenaSpriteMascara::AUEscenaSpriteMascara(AUTextura* texturaMascara, AUTextura* texturaFondo) : AUEscenaObjeto(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSpriteMascara::AUEscenaSpriteMascara")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaSpriteMascara")
	NBASSERT(texturaMascara != NULL)
	NBASSERT(texturaFondo != NULL)
	const NBTamano tamHD = texturaMascara->tamanoHD();
	if(texturaMascara->ordenFilasImagen() == ENTexturaOrdenV_HaciaArriba){
		this->privInlineInicializar(tamHD.ancho / -2.0f, tamHD.alto / 2.0f, tamHD.ancho, -tamHD.alto);
	} else {
		this->privInlineInicializar(tamHD.ancho / -2.0f, tamHD.alto / -2.0f, tamHD.ancho, tamHD.alto);
	}
	this->establecerTexturaMascara(texturaMascara);
	this->establecerTexturaFondo(texturaFondo);
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUEscenaSpriteMascara::AUEscenaSpriteMascara(AUEscenaSpriteMascara* otra) : AUEscenaObjeto(otra) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSpriteMascara::AUEscenaSpriteMascara")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaSpriteMascara")
	privInlineInicializar(otra->_limites.x, otra->_limites.y, otra->_limites.ancho, otra->_limites.alto);
	this->establecerTexturaMascara(otra->_texturaMascara);
	this->establecerTexturaFondo(otra->_texturaFondo);
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaSpriteMascara::privInlineInicializar(float x, float y, float ancho, float alto){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSpriteMascara::inicializar")
	_texturaMascara				= NULL;
	_texturaFondo				= NULL;
	_limites.x					= x;	
	_limites.y					= y;
	_limites.ancho				= ancho;
	_limites.alto				= alto;
	_desplazamientoFondo.ancho	= 0.0f;
	_desplazamientoFondo.alto	= 0.0f;
	_escalaFondo.ancho			= 1.0f;
	_escalaFondo.alto			= 1.0f;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUEscenaSpriteMascara::~AUEscenaSpriteMascara(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSpriteMascara::~AUEscenaSpriteMascara")
	if(_texturaMascara != NULL) _texturaMascara->liberar(NB_RETENEDOR_THIS); _texturaMascara = NULL;
	if(_texturaFondo != NULL) _texturaFondo->liberar(NB_RETENEDOR_THIS); _texturaFondo = NULL;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}
 
//-------------------------------
//--- FUNCIONES DE INSTANCIA ----
//-------------------------------

void AUEscenaSpriteMascara::ajustarFondoSegunMascara(){
	if(_texturaFondo != NULL && _texturaMascara != NULL){
		const NBTamano tamMasc = _texturaMascara->tamanoHD();
		const NBTamano tamFondo = _texturaFondo->tamanoHD();
		const float w = (tamMasc.ancho / tamFondo.ancho);
		const float h = (tamMasc.alto / tamFondo.alto);
		if(_desplazamientoFondo.ancho != 0.0f || _desplazamientoFondo.alto != 0.0f || _escalaFondo.ancho != w || _escalaFondo.alto != h){
			_desplazamientoFondo.ancho	= 0.0f;
			_desplazamientoFondo.alto	= 0.0f;
			_escalaFondo.ancho			= w;
			_escalaFondo.alto			= h;
			//
			if(this->idEscena >= 0){
				this->setModelChangedFlag();
			}
		}
	}
}

void AUEscenaSpriteMascara::redimensionar(AUTextura* textura){
	const NBTamano tamHD = textura->tamanoHD();
	if(textura->ordenFilasImagen() == ENTexturaOrdenV_HaciaArriba){
		this->redimensionar(tamHD.ancho / -2.0f, tamHD.alto / 2.0f, tamHD.ancho, -tamHD.alto);
	} else {
		this->redimensionar(tamHD.ancho / -2.0f, tamHD.alto / -2.0f, tamHD.ancho, tamHD.alto);
	}
}

void AUEscenaSpriteMascara::redimensionar(NBTamano tamano){
	this->redimensionar(tamano.ancho / -2.0, tamano.alto / -2.0, tamano.ancho, tamano.alto);
}

void AUEscenaSpriteMascara::redimensionar(float ancho, float alto){
	this->redimensionar(ancho / -2.0, alto / -2.0, ancho, alto);
}

void AUEscenaSpriteMascara::redimensionar(float x, float y, float ancho, float alto){
	if(_limites.x != x || _limites.y != y || _limites.ancho != ancho || _limites.alto != alto){
		_limites.x		= x;
		_limites.y		= y;
		_limites.ancho	= ancho;
		_limites.alto	= alto;
		//
		if(this->idEscena >= 0){
			this->setModelChangedFlag();
		}
	}
}

NBRectangulo AUEscenaSpriteMascara::limites() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSpriteMascara::limites")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _limites;
}

AUTextura* AUEscenaSpriteMascara::texturaMascara() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSpriteMascara::texturaMascara")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _texturaMascara;
}

AUTextura* AUEscenaSpriteMascara::texturaFondo() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSpriteMascara::texturaFondo")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _texturaFondo;
}

NBTamano AUEscenaSpriteMascara::desplazamientoFondo() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSpriteMascara::desplazamientoFondo")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _desplazamientoFondo;
}

NBTamano AUEscenaSpriteMascara::escalaFondo() const{
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSpriteMascara::escalaFondo")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _escalaFondo;
}

void AUEscenaSpriteMascara::establecerTexturaMascara(AUTextura* tex){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSpriteMascara::establecerTexturaMascara")
	if(_texturaMascara != tex){
		if(tex != NULL) tex->retener(NB_RETENEDOR_THIS);
		if(_texturaMascara != NULL) _texturaMascara->liberar(NB_RETENEDOR_THIS);
		_texturaMascara = tex;
		//
		if(this->idEscena >= 0){
			this->setModelChangedFlag();
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaSpriteMascara::establecerTexturaFondo(AUTextura* tex){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSpriteMascara::establecerTexturaFondo")
	if(_texturaFondo != tex){
		if(tex != NULL) tex->retener(NB_RETENEDOR_THIS);
		if(_texturaFondo != NULL) _texturaFondo->liberar(NB_RETENEDOR_THIS);
		_texturaFondo = tex;
		//
		if(this->idEscena >= 0){
			this->setModelChangedFlag();
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaSpriteMascara::establecerDesplazamientoFondo(const float despX, const float despY){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSpriteMascara::establecerDesplazamientoFondo")
	if(_desplazamientoFondo.ancho != despX || _desplazamientoFondo.alto != despY){
		_desplazamientoFondo.ancho	= despX;
		_desplazamientoFondo.alto	= despY;
		//
		if(this->idEscena >= 0){
			this->setModelChangedFlag();
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaSpriteMascara::establecerEscalaFondo(const float escalaAncho, const float escalaAlto){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSpriteMascara::establecerEscalaFondo")
	if(_escalaFondo.ancho != escalaAncho || _escalaFondo.alto != escalaAlto){
		_escalaFondo.ancho	= escalaAncho;
		_escalaFondo.alto	= escalaAlto;
		//
		if(this->idEscena >= 0){
			this->setModelChangedFlag();
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

NBCajaAABB AUEscenaSpriteMascara::cajaAABBLocalCalculada(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSpriteMascara::cajaAABBLocalCalculada")
	NBCajaAABB cajaLocal; 
	NBCAJAAABB_INICIALIZAR(cajaLocal);
	NBCAJAAABB_ENVOLVER_PUNTO(cajaLocal, _limites.x, _limites.y);
	NBCAJAAABB_ENVOLVER_PUNTO(cajaLocal, _limites.x + _limites.ancho, _limites.y + _limites.alto);
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return cajaLocal;
}

void AUEscenaSpriteMascara::privEstablecerCoordenadasTexturas(NBVerticeGL2* verticesGL){
	NBASSERT(_texturaMascara != NULL)
	if(_texturaFondo == NULL){
		//entrelazadas
		verticesGL[0].tex2.x	= 0.0f;	verticesGL[0].tex2.y = 0.0f;
		verticesGL[1].tex2.x	= 0.0f;	verticesGL[1].tex2.y = 0.0f;
		verticesGL[2].tex2.x	= 0.0f;	verticesGL[2].tex2.y = 0.0f;
		verticesGL[3].tex2.x	= 0.0f;	verticesGL[3].tex2.y = 0.0f;
	} else {
		//PENDIENTE: optimizar para que este calculo no se realice en cada ciclo (posiblemente calcular solo cuando la textura cambien o fondo sea movido)
		const NBTamano tamTexMascara= _texturaMascara->tamanoHD();
		const NBTamano tamTexFondo	= _texturaFondo->tamanoHD();
		NBTamano despFondo			= _desplazamientoFondo;
		NBTamano escalaFondo		= _escalaFondo;
		//Validate pan
		if(despFondo.ancho < 0.0f) despFondo.ancho = 0.0f; if(despFondo.ancho > tamTexFondo.ancho) despFondo.ancho = tamTexFondo.ancho;
		if(despFondo.alto < 0.0f) despFondo.alto = 0.0f; if(despFondo.alto > tamTexFondo.alto) despFondo.alto = tamTexFondo.alto;
		//Validate scale
		if(((tamTexFondo.ancho - despFondo.ancho) * escalaFondo.ancho) < tamTexMascara.ancho) escalaFondo.ancho = tamTexMascara.ancho / (tamTexFondo.ancho - despFondo.ancho);
		if(((tamTexFondo.alto - despFondo.alto) * escalaFondo.alto) < tamTexMascara.alto) escalaFondo.alto = tamTexMascara.alto / (tamTexFondo.alto - despFondo.alto);
		//
		_texturaMascara->cargarCoordenadasTextura2(verticesGL, 0);
		//
		const NBCajaAABBP<double> cajaTex = _texturaFondo->cajaRelativaTexeles();
		NBCajaAABBP<double> cajaRelMasc;
		//PENDIENTE: //evitar este calculo, que lo haga la textura
		cajaRelMasc.xMin		= cajaTex.xMin + ((cajaTex.xMax - cajaTex.xMin) * (double)(despFondo.ancho / tamTexFondo.ancho));
		cajaRelMasc.yMin		= cajaTex.yMin + ((cajaTex.yMax - cajaTex.yMin) * (double)(despFondo.alto / tamTexFondo.alto));
		cajaRelMasc.xMax		= cajaRelMasc.xMin + ((cajaTex.xMax - cajaTex.xMin) * (double)(((tamTexFondo.ancho - despFondo.ancho) * escalaFondo.ancho) / tamTexMascara.ancho));
		cajaRelMasc.yMax		= cajaRelMasc.yMin + ((cajaTex.yMax - cajaTex.yMin) * (double)(((tamTexFondo.alto - despFondo.alto) * escalaFondo.alto) / tamTexMascara.alto));
		//entrelazadas
		verticesGL[0].tex2.x	= cajaRelMasc.xMin;	verticesGL[0].tex2.y = cajaRelMasc.yMin;
		verticesGL[1].tex2.x	= cajaRelMasc.xMax;	verticesGL[1].tex2.y = cajaRelMasc.yMin;
		verticesGL[2].tex2.x	= cajaRelMasc.xMin;	verticesGL[2].tex2.y = cajaRelMasc.yMax;
		verticesGL[3].tex2.x	= cajaRelMasc.xMax;	verticesGL[3].tex2.y = cajaRelMasc.yMax;
	}
}

void AUEscenaSpriteMascara::agregadoEnEscena(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaSpriteMascara::agregadoEnEscena")
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaSpriteMascara::quitandoDeEscena(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaSpriteMascara::quitandoDeEscena")
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

PTRfunCmdsGL AUEscenaSpriteMascara::actualizarModeloGL(const NBPropRenderizado &propsRenderizado, const NBPropIluminacion &propsIluminacion, const NBPropHeredadasRender &propsHeredadas){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSpriteMascara::actualizarModeloGL")
	NBASSERT(idEscena!=-1) //Solo si esta en escena
	STBloqueGL			bloqueVerticesGL;
	//ACTUALIZAR VERTICES
	bloqueVerticesGL.cantidadElementos	= 4;
	bloqueVerticesGL.primerElemento		= (*propsRenderizado.funcReservarVertices)(ENVerticeGlTipo_BiTextura, 4);
	NBVerticeGL2* verticesGL			= &((*propsRenderizado.verticesGL2)[bloqueVerticesGL.primerElemento]);
	float limitesMax[2]					= {_limites.x + _limites.ancho, _limites.y + _limites.alto};
	NBMATRIZ_MULTIPLICAR_PUNTO(verticesGL[0], _cacheObjEscena.matrizEscena, _limites.x, _limites.y);
	NBMATRIZ_MULTIPLICAR_PUNTO(verticesGL[1], _cacheObjEscena.matrizEscena, limitesMax[0], _limites.y);
	NBMATRIZ_MULTIPLICAR_PUNTO(verticesGL[2], _cacheObjEscena.matrizEscena, _limites.x, limitesMax[1]);
	NBMATRIZ_MULTIPLICAR_PUNTO(verticesGL[3], _cacheObjEscena.matrizEscena, limitesMax[0], limitesMax[1]);
	//Pixel precision
	if(_texturaFondo != NULL){
		if((_texturaFondo->modoPintado() & NB_TEXTURA_BIT_PINTADO_PRECISO) != 0){
			if((NBMATRIZ_ELEM00(_cacheObjEscena.matrizEscena) == -1 || NBMATRIZ_ELEM00(_cacheObjEscena.matrizEscena) == 0 || NBMATRIZ_ELEM00(_cacheObjEscena.matrizEscena) == 1)
			   && (NBMATRIZ_ELEM01(_cacheObjEscena.matrizEscena) == -1 || NBMATRIZ_ELEM01(_cacheObjEscena.matrizEscena) == 0 || NBMATRIZ_ELEM01(_cacheObjEscena.matrizEscena) == 1)
			   && (NBMATRIZ_ELEM10(_cacheObjEscena.matrizEscena) == -1 || NBMATRIZ_ELEM10(_cacheObjEscena.matrizEscena) == 0 || NBMATRIZ_ELEM10(_cacheObjEscena.matrizEscena) == 1)
			   && (NBMATRIZ_ELEM11(_cacheObjEscena.matrizEscena) == -1 || NBMATRIZ_ELEM11(_cacheObjEscena.matrizEscena) == 0 || NBMATRIZ_ELEM11(_cacheObjEscena.matrizEscena) == 1)){
				NBTamano unidadesEscenaPorPixel;
				unidadesEscenaPorPixel.ancho	= (propsRenderizado.cajaProyeccion.xMax - propsRenderizado.cajaProyeccion.xMin) / propsRenderizado.puertoDeVision.ancho;
				unidadesEscenaPorPixel.alto		= (propsRenderizado.cajaProyeccion.yMax - propsRenderizado.cajaProyeccion.yMin) / propsRenderizado.puertoDeVision.alto;
				NBTamano sobranteDePixel;
				sobranteDePixel.ancho			= (verticesGL[0].x - propsRenderizado.cajaProyeccion.xMin) / unidadesEscenaPorPixel.ancho;
				sobranteDePixel.alto			= (verticesGL[0].y - propsRenderizado.cajaProyeccion.yMin) / unidadesEscenaPorPixel.alto;
				sobranteDePixel.ancho			-= (float)((SI32)sobranteDePixel.ancho);
				sobranteDePixel.alto			-= (float)((SI32)sobranteDePixel.alto);
				NBTamano correccionEnEscena;
				correccionEnEscena.ancho		= (sobranteDePixel.ancho <= 0.5f ? -sobranteDePixel.ancho : 1.0f - sobranteDePixel.ancho) * unidadesEscenaPorPixel.ancho;
				correccionEnEscena.alto			= (sobranteDePixel.alto <= 0.5f ? -sobranteDePixel.alto : 1.0f - sobranteDePixel.alto) * unidadesEscenaPorPixel.alto;
				if(correccionEnEscena.ancho != 0.0f || correccionEnEscena.alto != 0.0f){
					verticesGL[0].x += correccionEnEscena.ancho; verticesGL[0].y += correccionEnEscena.alto;
					verticesGL[1].x += correccionEnEscena.ancho; verticesGL[1].y += correccionEnEscena.alto;
					verticesGL[2].x += correccionEnEscena.ancho; verticesGL[2].y += correccionEnEscena.alto;
					verticesGL[3].x += correccionEnEscena.ancho; verticesGL[3].y += correccionEnEscena.alto;
				}
			}
		}
	}
	//ACTUALIZAR TEX COORDS
	this->privEstablecerCoordenadasTexturas(verticesGL);
	//ACTUALIZAR COLORES
	NBColor colorMultiplicado; NBCOLOR_MULTIPLICAR_CON_UI8(colorMultiplicado, propsHeredadas.colorPadre, _propiedades.color8);
	NBCOLOR_ESTABLECER(verticesGL[0], 255.0f*colorMultiplicado.r, 255.0f*colorMultiplicado.g, 255.0f*colorMultiplicado.b, 255.0f*colorMultiplicado.a);
	NBCOLOR_ESTABLECER(verticesGL[1], 255.0f*colorMultiplicado.r, 255.0f*colorMultiplicado.g, 255.0f*colorMultiplicado.b, 255.0f*colorMultiplicado.a);
	NBCOLOR_ESTABLECER(verticesGL[2], 255.0f*colorMultiplicado.r, 255.0f*colorMultiplicado.g, 255.0f*colorMultiplicado.b, 255.0f*colorMultiplicado.a);
	NBCOLOR_ESTABLECER(verticesGL[3], 255.0f*colorMultiplicado.r, 255.0f*colorMultiplicado.g, 255.0f*colorMultiplicado.b, 255.0f*colorMultiplicado.a);
	//DATOS RENDER
	const SI32 posicionDatos			= propsRenderizado.bytesDatosModelos->conteo; propsRenderizado.bytesDatosModelos->reservarRegistrosAlFinal(sizeof(STSpriteMascaraRender));
	STSpriteMascaraRender* propsRender	= (STSpriteMascaraRender*)&(propsRenderizado.bytesDatosModelos->elemento[posicionDatos]);
	if(_texturaFondo != NULL){
		propsRender->idTexturaGLFondo	= _texturaFondo->idTexturaGL; propsRenderizado.piscinaRetecionRender->agregarElemento(_texturaFondo);
	} else {
		//required on some systems where texture-0 will not be drawn
		propsRender->idTexturaGLFondo	= NBGestorTexturas::texturaBlanca->idTexturaGL;
	}
	if(_texturaMascara != NULL){
		propsRender->idTexturaGLMascara	= _texturaMascara->idTexturaGL; propsRenderizado.piscinaRetecionRender->agregarElemento(_texturaMascara);
	} else {
		//required on some systems where texture-0 will not be drawn
		propsRender->idTexturaGLMascara	= NBGestorTexturas::texturaBlanca->idTexturaGL;
	}
	propsRender->bloqueIndicesGL		= NB_GESTOR_GL_DAME_ELEMS_PARA_TRIANGSTRIP_4(ENVerticeGlTipo_BiTextura, bloqueVerticesGL);
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return &AUEscenaSpriteMascara::enviarComandosGL;
}

void AUEscenaSpriteMascara::enviarComandosGL(UI8* punteroDatosModelo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSpriteMascara::enviarComandosGL")
	STSpriteMascaraRender* datosRender = (STSpriteMascaraRender*)punteroDatosModelo;
	NBASSERT(datosRender->bloqueIndicesGL.cantidadElementos > 0)
	NBGestorGL::activarVerticesGL(ENVerticeGlTipo_BiTextura); NBGESTORGL_DBG_NOMBRAR_ACTIVADOR_VERTICES_GL("AUEscenaSpriteMascara")
	//Configurar texturas
	NBGestorGL::bindTexture(0, datosRender->idTexturaGLMascara);
	//NBGestorGL::texEnvi(0, GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
	NBGestorGL::bindTexture(1, datosRender->idTexturaGLFondo);
	//NBGestorGL::texEnvi(1, GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	NB_GESTOR_GL_RENDER_TRIANGSTRIP_4(datosRender->bloqueIndicesGL.primerElemento, datosRender->bloqueIndicesGL.cantidadElementos); NBGESTORGL_DBG_NOMBRAR_CONVOCADOR_ACUMULAR_TRIANG_STRIPS_INDEP("AUEscenaSpriteMascara")
	//NBGestorGL::texEnvi(0, GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

//


bool AUEscenaSpriteMascara::agregarXmlInternoEn(AUEscenaSpriteMascara* sprite, AUCadenaLargaMutable8* guardarEn, const char* espaciosBaseIzq){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSpriteMascara::agregarXmlInternoEn")
	//guardarEn->agregarConFormato("%s<vXML>1</vXML>\r\n", espaciosBaseIzq);
	guardarEn->agregarConFormato("%s<eOb>\r\n", espaciosBaseIzq);
	AUEscenaObjeto::agregarXmlInternoEn(sprite, guardarEn, espaciosBaseIzq);
	guardarEn->agregarConFormato("%s</eOb>\r\n", espaciosBaseIzq);
	guardarEn->agregarConFormato("%s<lims>%f|%f|%f|%f</lims>\r\n", espaciosBaseIzq, sprite->_limites.x, sprite->_limites.y, sprite->_limites.ancho, sprite->_limites.alto);
	if(sprite->_texturaMascara != NULL){
		const char* nombreTextura = NBGestorTexturas::nombreRecursoTextura(sprite->_texturaMascara);
		if(nombreTextura != NULL){
			guardarEn->agregarConFormato("%s<texMasc>%s</texMasc>\r\n", espaciosBaseIzq, nombreTextura);
		}
	}
	if(sprite->_texturaFondo != NULL){
		const char* nombreTextura = NBGestorTexturas::nombreRecursoTextura(sprite->_texturaFondo);
		if(nombreTextura != NULL){
			guardarEn->agregarConFormato("%s<texFondo>%s</texFondo>\r\n", espaciosBaseIzq, nombreTextura);
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return true;

}

bool AUEscenaSpriteMascara::interpretarXmlInterno(AUEscenaSpriteMascara* cargarEn, const AUDatosXML* datosXml, const sNodoXML* nodoXml){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSpriteMascara::interpretarXmlInterno")
	bool exito = false;
	//int versionXML = datosXml->nodoHijo("vXML", 0, nodoXml);
	//if(versionXML==1){
		const sNodoXML* nodoEscenaObj;		XML_NODO_HIJO(datosXml, nodoEscenaObj, "eOb", "escObj", nodoXml, NULL)
		if(nodoEscenaObj != NULL){
			exito = AUEscenaObjeto::interpretarXmlInterno(cargarEn, datosXml, nodoEscenaObj);
			float x = 0.0f, y = 0.0f, ancho = 0.0f, alto = 0.0f;
			const sNodoXML* nodoLimites	= datosXml->nodoHijo("lims", nodoXml);
			if(nodoLimites != NULL){
				if(nodoLimites->nodosHijos == NULL){
					//Formato nuevo CSV (mas compacto)
					const char* cadenaCSV	= datosXml->cadenaValorDeNodo(nodoLimites);
					SI32 tamCadenaCSV		= AUCadena8::tamano(cadenaCSV);
					SI32 posEnCadenaCSV		= 0;
					x						= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0.0f);
					y						= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0.0f);
					ancho					= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0.0f);
					alto					= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0.0f);
				} else {
					//Formato viejo XML (muy inflado)
					x			= datosXml->nodoHijo("x", 0.0f, nodoLimites);
					y			= datosXml->nodoHijo("y", 0.0f, nodoLimites);
					ancho		= datosXml->nodoHijo("ancho", 0.0f, nodoLimites);
					alto		= datosXml->nodoHijo("alto", 0.0f, nodoLimites);
				}
			}
			AUCadenaMutable8* nomTexturaMascara = new(ENMemoriaTipo_Temporal) AUCadenaMutable8();
			AUCadenaMutable8* nomTexturaFondo = new(ENMemoriaTipo_Temporal) AUCadenaMutable8();
			datosXml->nodoHijo("texMasc", nomTexturaMascara, "", nodoXml);
			datosXml->nodoHijo("texFondo", nomTexturaFondo, "", nodoXml);
			//
			cargarEn->redimensionar(x, y, ancho, alto);
			if(nomTexturaMascara->tamano()!=0){
				cargarEn->establecerTexturaMascara(NBGestorTexturas::texturaDesdeArchivo(nomTexturaMascara->str()));
			}
			if(nomTexturaFondo->tamano()!=0){
				cargarEn->establecerTexturaFondo(NBGestorTexturas::texturaDesdeArchivo(nomTexturaFondo->str()));
			}
			nomTexturaFondo->liberar(NB_RETENEDOR_NULL);
			nomTexturaMascara->liberar(NB_RETENEDOR_NULL);
		}
	//}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

bool AUEscenaSpriteMascara::agregarBitsInternosEn(AUEscenaSpriteMascara* sprite, AUArchivo* guardarEn){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSpriteMascara::agregarBitsInternosEn")
	bool exito = false;
	//
	float versionDatos = 1.0f;
	guardarEn->escribir(&versionDatos, sizeof(versionDatos), 1, guardarEn);
	if(AUEscenaObjeto::agregarBitsInternosEn(sprite, guardarEn)){
		guardarEn->escribir(&sprite->_limites, sizeof(sprite->_limites), 1, guardarEn);
		//
		const char* nombreTexturaMascara = NULL;
		const char* nombreTexturaFondo = NULL;
		SI32 tamanoNombreTextura = 0;
		if(sprite->_texturaMascara != NULL) nombreTexturaMascara = NBGestorTexturas::nombreRecursoTextura(sprite->_texturaMascara);
		if(nombreTexturaMascara == NULL){
			guardarEn->escribir(&tamanoNombreTextura, sizeof(tamanoNombreTextura), 1, guardarEn);
		} else {
			tamanoNombreTextura = AUCadenaMutable8::tamano(nombreTexturaMascara);
			guardarEn->escribir(&tamanoNombreTextura, sizeof(tamanoNombreTextura), 1, guardarEn);
			guardarEn->escribir(nombreTexturaMascara, sizeof(char), tamanoNombreTextura, guardarEn);
		}
		if(sprite->_texturaFondo != NULL) nombreTexturaFondo = NBGestorTexturas::nombreRecursoTextura(sprite->_texturaFondo);
		if(nombreTexturaFondo == NULL){
			guardarEn->escribir(&tamanoNombreTextura, sizeof(tamanoNombreTextura), 1, guardarEn);
		} else {
			tamanoNombreTextura = AUCadenaMutable8::tamano(nombreTexturaFondo);
			guardarEn->escribir(&tamanoNombreTextura, sizeof(tamanoNombreTextura), 1, guardarEn);
			guardarEn->escribir(nombreTexturaFondo, sizeof(char), tamanoNombreTextura, guardarEn);
		}
		exito = true;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

bool AUEscenaSpriteMascara::interpretarBitsInternos(AUEscenaSpriteMascara* cargarEn, AUArchivo* cargarDe){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSpriteMascara::interpretarBitsInternos")
	bool exito = false;
	float versionDatos;
	cargarDe->leer(&versionDatos, sizeof(versionDatos), 1, cargarDe);
	if(versionDatos==1.0f){
		if(AUEscenaObjeto::interpretarBitsInternos(cargarEn, cargarDe)){
			cargarDe->leer(&cargarEn->_limites, sizeof(cargarEn->_limites), 1, cargarDe);
			cargarEn->redimensionar(cargarEn->_limites.x, cargarEn->_limites.y, cargarEn->_limites.ancho, cargarEn->_limites.alto);
			//Textura mascara
			SI32 tamanoNombreTexturaMascara;
			cargarDe->leer(&tamanoNombreTexturaMascara, sizeof(tamanoNombreTexturaMascara), 1, cargarDe);
			if(tamanoNombreTexturaMascara>0){
				char* nombreTextura = (char*)NBGestorMemoria::reservarMemoria(tamanoNombreTexturaMascara+1, ENMemoriaTipo_Temporal); NB_DEFINE_NOMBRE_PUNTERO(nombreTextura, "AUEscenaSpriteMascara::nombreTextura") //+1 es el "\0"
				cargarDe->leer(nombreTextura, sizeof(char), tamanoNombreTexturaMascara, cargarDe);
				nombreTextura[tamanoNombreTexturaMascara] = '\0';
				cargarEn->establecerTexturaMascara(NBGestorTexturas::texturaDesdeArchivo(nombreTextura));
				NBGestorMemoria::liberarMemoria(nombreTextura);
			}
			//Textura fondo
			SI32 tamanoNombreTexturaFondo;
			cargarDe->leer(&tamanoNombreTexturaFondo, sizeof(tamanoNombreTexturaFondo), 1, cargarDe);
			if(tamanoNombreTexturaFondo>0){
				char* nombreTextura = (char*)NBGestorMemoria::reservarMemoria(tamanoNombreTexturaFondo+1, ENMemoriaTipo_Temporal); NB_DEFINE_NOMBRE_PUNTERO(nombreTextura, "AUEscenaSpriteMascara::nombreTextura") //+1 es el "\0"
				cargarDe->leer(nombreTextura, sizeof(char), tamanoNombreTexturaFondo, cargarDe);
				nombreTextura[tamanoNombreTexturaFondo] = '\0';
				cargarEn->establecerTexturaFondo(NBGestorTexturas::texturaDesdeArchivo(nombreTextura));
				NBGestorMemoria::liberarMemoria(nombreTextura);
			}
			//
			exito = true;
		}
	} else {
		PRINTF_ERROR("AUEscenaSpriteMascara, la version (%f) de datos binarios no es interpretable por este ejecutable\n", versionDatos);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

//
AUOBJMETODOS_CLASESID_MULTICLASE(AUEscenaSpriteMascara, AUEscenaObjeto)
AUOBJMETODOS_CLASESNOMBRES_MULTICLASE(AUEscenaSpriteMascara, "AUEscenaSpriteMascara", AUEscenaObjeto)
AUOBJMETODOS_CLONAR_THIS(AUEscenaSpriteMascara)





