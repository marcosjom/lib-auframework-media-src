
#include "AUFrameworkMediaStdAfx.h"
#include "AUEscenaSprite.h"

//-------------------------------------
//--- CONSTRUCTORES Y DESTRUCTORES ----
//-------------------------------------

AUEscenaSprite::AUEscenaSprite() : AUEscenaObjeto() {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSprite::AUEscenaSprite")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaSprite")
	privInlineInicializar(0.0f, 0.0f, 0.0f, 0.0f, false);
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUEscenaSprite::AUEscenaSprite(NBTamano tamano) : AUEscenaObjeto() {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSprite::AUEscenaSprite")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaSprite")
	privInlineInicializar(tamano.ancho/-2.0f, tamano.alto/-2.0f, tamano.ancho, tamano.alto, false);
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUEscenaSprite::AUEscenaSprite(float ancho, float alto) : AUEscenaObjeto() {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSprite::AUEscenaSprite")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaSprite")
	privInlineInicializar(ancho/-2.0f, alto/-2.0f, ancho, alto, false);
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUEscenaSprite::AUEscenaSprite(float x, float y, float ancho, float alto) : AUEscenaObjeto(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSprite::AUEscenaSprite")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaSprite")
	privInlineInicializar(x, y, ancho, alto, false);
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUEscenaSprite::AUEscenaSprite(AUTextura* textura) : AUEscenaObjeto(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSprite::AUEscenaSprite")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaSprite")
	NBASSERT(textura != NULL)
	NBTamano tamHD = textura->tamanoHD();
	//Probando, ajustar para que las coordenadas locales siempre sean enteras.
	//Intentando ver si las coordenadas no enteras produce que las texturas se pinten "borrosas".
	//NBPunto posSupIzq;
	//posSupIzq.x = ((SI32)tamHD.ancho % 2)==0? tamHD.ancho/-2.0f : (tamHD.ancho+1)/-2.0f;
	//posSupIzq.y = ((SI32)tamHD.alto % 2)==0? tamHD.alto/2.0f : (tamHD.alto+1)/2.0f;
	//inicializar(posSupIzq.x, posSupIzq.y, tamHD.ancho, -tamHD.alto);
	if(textura->ordenFilasImagen()==ENTexturaOrdenV_HaciaArriba){
		privInlineInicializar(tamHD.ancho/-2.0f, tamHD.alto/2.0f, tamHD.ancho, -tamHD.alto, false);
	} else {
		privInlineInicializar(tamHD.ancho/-2.0f, tamHD.alto/-2.0f, tamHD.ancho, tamHD.alto, false);
	}
	establecerTextura(textura);
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUEscenaSprite::AUEscenaSprite(AUTextura* textura, const ENSpriteAlineaH alinH, const ENSpriteAlineaV alinV) : AUEscenaObjeto(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSprite::AUEscenaSprite")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaSprite")
	NBASSERT(textura != NULL)
	NBTamano tamHD = textura->tamanoHD();
	if(textura->ordenFilasImagen()==ENTexturaOrdenV_HaciaArriba){
		privInlineInicializar(
							  alinH==ENSpriteAlineaH_Izq?-tamHD.ancho:alinH==ENSpriteAlineaH_Der?0.0f:tamHD.ancho/-2.0f
							  , alinV==ENSpriteAlineaV_Arr?tamHD.alto:alinV==ENSpriteAlineaV_Aba?0.0f:tamHD.alto/2.0f
							  , tamHD.ancho, -tamHD.alto, false);
	} else {
		privInlineInicializar(
							  alinH==ENSpriteAlineaH_Izq?-tamHD.ancho:alinH==ENSpriteAlineaH_Der?0.0f:tamHD.ancho/-2.0f
							  , alinV==ENSpriteAlineaV_Arr?0.0f:alinV==ENSpriteAlineaV_Aba?-tamHD.alto:tamHD.alto/-2.0f
							  , tamHD.ancho, tamHD.alto, false);
	}
	establecerTextura(textura);
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUEscenaSprite::AUEscenaSprite(AUEscenaSprite* otra) : AUEscenaObjeto(otra) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSprite::AUEscenaSprite")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaSprite")
	NBASSERT(otra != NULL)
	privInlineInicializar(otra->_limites.x, otra->_limites.y, otra->_limites.ancho, otra->_limites.alto, otra->_esMascaraAlpha);
	establecerTextura(otra->_textura);
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaSprite::privInlineInicializar(float x, float y, float ancho, float alto, bool esMascaraAlpha){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSprite::inicializar")
	_textura				= NULL;
	_limites.x				= x;	
	_limites.y				= y;
	_limites.ancho			= ancho;
	_limites.alto			= alto;
	_esMascaraAlpha			= esMascaraAlpha;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUEscenaSprite::~AUEscenaSprite(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSprite::~AUEscenaSprite")
	if(_textura != NULL) _textura->liberar(NB_RETENEDOR_THIS); _textura = NULL;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}
 
//-------------------------------
//--- FUNCIONES DE INSTANCIA ----
//-------------------------------

void AUEscenaSprite::redimensionar(AUTextura* textura){
	NBASSERT(textura != NULL)
	NBTamano tamHD = textura->tamanoHD();
	if(textura->ordenFilasImagen() == ENTexturaOrdenV_HaciaArriba){
		this->redimensionar(tamHD.ancho / -2.0f, tamHD.alto / 2.0f, tamHD.ancho, -tamHD.alto);
	} else {
		this->redimensionar(tamHD.ancho / -2.0f, tamHD.alto / -2.0f, tamHD.ancho, tamHD.alto);
	}
}

void AUEscenaSprite::redimensionar(AUTextura* textura, const ENSpriteAlineaH alinH, const ENSpriteAlineaV alinV){
	NBASSERT(textura != NULL)
	NBTamano tamHD = textura->tamanoHD();
	if(textura->ordenFilasImagen()==ENTexturaOrdenV_HaciaArriba){
		this->redimensionar(alinH == ENSpriteAlineaH_Izq ? -tamHD.ancho:alinH == ENSpriteAlineaH_Der ? 0.0f : tamHD.ancho / -2.0f, alinV == ENSpriteAlineaV_Arr ? tamHD.alto : alinV == ENSpriteAlineaV_Aba ? 0.0f : tamHD.alto / 2.0f, tamHD.ancho, -tamHD.alto);
	} else {
		this->redimensionar(alinH == ENSpriteAlineaH_Izq ? -tamHD.ancho:alinH == ENSpriteAlineaH_Der ? 0.0f : tamHD.ancho / -2.0f, alinV == ENSpriteAlineaV_Arr ? 0.0f : alinV == ENSpriteAlineaV_Aba ? -tamHD.alto : tamHD.alto / -2.0f, tamHD.ancho, tamHD.alto);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaSprite::redimensionar(const NBTamano tamano){
	this->redimensionar(tamano.ancho / -2.0, tamano.alto / -2.0, tamano.ancho, tamano.alto);
}

void AUEscenaSprite::redimensionar(const float ancho, const float alto){
	this->redimensionar(ancho / -2.0, alto / -2.0, ancho, alto);
}

void AUEscenaSprite::redimensionar(const float x, const float y, const float ancho, const float alto){
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

AUTextura* AUEscenaSprite::textura() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSprite::textura")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _textura;
}

NBRectangulo AUEscenaSprite::limites() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSprite::limites")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _limites;
}

bool AUEscenaSprite::esMascaraAlpha() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSprite::esMascaraAlpha")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _esMascaraAlpha;
}

void AUEscenaSprite::establecerTextura(AUTextura* nuevaTextura){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSprite::establecerTextura")
	if(_textura != nuevaTextura){
		if(nuevaTextura != NULL) nuevaTextura->retener(NB_RETENEDOR_THIS);
		if(_textura != NULL) _textura->liberar(NB_RETENEDOR_THIS);
		_textura = nuevaTextura;
#		ifdef CONFIG_NB_SPRITES_ESCENA_ADVERTIR_ESTIRAMIENTO_LEVE_TEXTURAS
		if(_textura != NULL){
			NBTamano tamTex = _textura->tamanoHD();
			NBTamano relacionTex; NBTAMANO_ESTABLECER(relacionTex, tamTex.ancho / (float)_limites.ancho, tamTex.alto / (float)_limites.alto);
			if((relacionTex.ancho != 1.0f && relacionTex.ancho > 0.99f && relacionTex.ancho < 1.01f) || (relacionTex.ancho != -1.0f && relacionTex.ancho > -1.01f && relacionTex.ancho < -0.99f) || (relacionTex.alto != 1.0f && relacionTex.alto > 0.99f && relacionTex.alto < 1.01f) || (relacionTex.alto != -1.0f && relacionTex.alto > -1.01f && relacionTex.alto < -0.99f)){
				PRINTF_WARNING("se estira (%f, %f) la textura '%s'\n", relacionTex.ancho, relacionTex.alto, NBGestorTexturas::nombreRecursoTextura(_textura));
			}
		}
#		endif
		//
		if(this->idEscena >= 0){
			this->setModelChangedFlag();
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaSprite::establecerEsMascaraAlpha(bool esMascaraAlpha){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSprite::establecerEsMascaraAlpha")
	if(_esMascaraAlpha != esMascaraAlpha){
		_esMascaraAlpha = esMascaraAlpha;
		//
		if(this->idEscena >= 0){
			this->setModelChangedFlag();
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

NBCajaAABB AUEscenaSprite::cajaAABBLocalCalculada(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSprite::cajaAABBLocalCalculada")
	NBCajaAABB cajaLocal;
	NBCAJAAABB_INICIALIZAR(cajaLocal);
	NBCAJAAABB_ENVOLVER_PUNTO(cajaLocal, _limites.x, _limites.y);
	NBCAJAAABB_ENVOLVER_PUNTO(cajaLocal, _limites.x + _limites.ancho, _limites.y + _limites.alto);
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return cajaLocal;
}

STNBRectI16 AUEscenaSprite::privGetTextureRect(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaSprite::privGetTextureRect")
	STNBRectI16 r;
	r.x = r.y = r.width = r.height = 0;
	if(_textura != NULL){
		const NBRectanguloP<UI16> rect = _textura->areaEnAtlas();
		r.x			= rect.x;
		r.y			= rect.y;
		r.width		= rect.ancho;
		r.height	= rect.alto;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return r;
}

void AUEscenaSprite::privEstablecerCoordenadasTextura(NBVerticeGL* verticesGL){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaSprite::privEstablecerCoordenadasTextura")
	if(_textura != NULL){
		_textura->cargarCoordenadasTextura(verticesGL);
	} else {
		NBPUNTO_ESTABLECER(verticesGL[0].tex, 0.0f, 0.0f);
		NBPUNTO_ESTABLECER(verticesGL[1].tex, 0.0f, 0.0f);
		NBPUNTO_ESTABLECER(verticesGL[2].tex, 0.0f, 0.0f);
		NBPUNTO_ESTABLECER(verticesGL[3].tex, 0.0f, 0.0f);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaSprite::agregadoEnEscena(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaSprite::agregadoEnEscena")
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaSprite::quitandoDeEscena(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaSprite::quitandoDeEscena")
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

PTRfunCmdsGL AUEscenaSprite::actualizarModeloGL(const NBPropRenderizado &propsRenderizado, const NBPropIluminacion &propsIluminacion, const NBPropHeredadasRender &propsHeredadas){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSprite::actualizarModeloGL")
	NBASSERT(idEscena!=-1) //Solo si esta en escena
	//
	BOOL useDrawTexExt		= FALSE;
	const float limitesMaxX	= _limites.x + _limites.ancho;
	const float limitesMaxY	= _limites.y + _limites.alto;
	NBColor colorMultiplicado; NBCOLOR_MULTIPLICAR_CON_UI8(colorMultiplicado, propsHeredadas.colorPadre, _propiedades.color8);
	//Analyze if drawTexture extension can be used
	if(!NBMATRIZ_IS_ROTATED(_cacheObjEscena.matrizEscena)){
		if(NBGestorGL::soportaDrawTexture()){
			useDrawTexExt = TRUE;
		}
	}
	//
	if(useDrawTexExt){
		//Final structure
		const SI32 posicionDatos		= propsRenderizado.bytesDatosModelos->conteo; propsRenderizado.bytesDatosModelos->reservarRegistrosAlFinal(sizeof(STSpriteRenderTex));
		STSpriteRenderTex* propsRender	= (STSpriteRenderTex*)&(propsRenderizado.bytesDatosModelos->elemento[posicionDatos]);
		propsRender->type				= ENSpriteRenderType_Tex;
		propsRender->esMascaraAlpha		= _esMascaraAlpha;
		if(_textura != NULL){
			propsRender->idTexturaGL	= _textura->idTexturaGL;
			propsRenderizado.piscinaRetecionRender->agregarElemento(_textura);
		} else {
			//required on some systems where texture-0 will not be drawn
			propsRender->idTexturaGL	= NBGestorTexturas::texturaBlanca->idTexturaGL;
		}
		//Apply color
		{
			propsRender->color.r	= (255.0f * colorMultiplicado.r);
			propsRender->color.g	= (255.0f * colorMultiplicado.g);
			propsRender->color.b	= (255.0f * colorMultiplicado.b);
			propsRender->color.a	= (255.0f * colorMultiplicado.a);
		}
		//Apply texCoords
		{
			propsRender->texRect	= this->privGetTextureRect();
		}
		//Apply coords
		{
			NBPunto pMinScn, pMaxScn;
			NBTamano unidadesEscenaPorPixel;
			unidadesEscenaPorPixel.ancho		= (propsRenderizado.cajaProyeccion.xMax - propsRenderizado.cajaProyeccion.xMin) / propsRenderizado.puertoDeVision.ancho;
			unidadesEscenaPorPixel.alto			= (propsRenderizado.cajaProyeccion.yMax - propsRenderizado.cajaProyeccion.yMin) / propsRenderizado.puertoDeVision.alto;
			//
			NBMATRIZ_MULTIPLICAR_PUNTO(pMinScn, _cacheObjEscena.matrizEscena, _limites.x, _limites.y);
			NBMATRIZ_MULTIPLICAR_PUNTO(pMaxScn, _cacheObjEscena.matrizEscena, limitesMaxX, limitesMaxY);
			//
			if(pMinScn.x <= pMaxScn.x){
				propsRender->viewRect.x			= (pMinScn.x - propsRenderizado.cajaProyeccion.xMin) / unidadesEscenaPorPixel.ancho;
				propsRender->viewRect.width		= (pMaxScn.x - pMinScn.x) / unidadesEscenaPorPixel.ancho;
			} else {
				propsRender->viewRect.x			= (pMaxScn.x - propsRenderizado.cajaProyeccion.xMin) / unidadesEscenaPorPixel.ancho;
				propsRender->viewRect.width		= (pMinScn.x - pMaxScn.x) / unidadesEscenaPorPixel.ancho;
				//Invert texture rect
				propsRender->texRect.x			+= propsRender->texRect.width;
				propsRender->texRect.width		= -propsRender->texRect.width;
			}
			if(pMinScn.y <= pMaxScn.y){
				propsRender->viewRect.y			= (pMinScn.y - propsRenderizado.cajaProyeccion.yMin) / unidadesEscenaPorPixel.alto; //y-inverted
				propsRender->viewRect.height	= (pMaxScn.y - pMinScn.y) / unidadesEscenaPorPixel.alto;
			} else {
				propsRender->viewRect.y			= (pMaxScn.y - propsRenderizado.cajaProyeccion.yMin) / unidadesEscenaPorPixel.alto; //y-inverted
				propsRender->viewRect.height	= (pMinScn.y - pMaxScn.y) / unidadesEscenaPorPixel.alto;
				//Invert texture rect
				propsRender->texRect.y			+= propsRender->texRect.height;
				propsRender->texRect.height		= -propsRender->texRect.height;
			}
			//Pixel precision
			/*if(_textura != NULL){
				if((_textura->modoPintado() & NB_TEXTURA_BIT_PINTADO_PRECISO) != 0){
					//Horizontal
					{
						const float xExtraBelow		= propsRender->texRect.x - (float)((SI32)propsRender->texRect.x);
						propsRender->texRect.x		-= xExtraBelow;
						{
							const float xMax 		= (propsRender->texRect.x + propsRender->texRect.width);
							const float xExtraUp	= (xMax - (float)((SI32)xMax));
							propsRender->texRect.width = (float)((SI32)propsRender->texRect.width + (xExtraUp < 0.5f ? 0 : 1));
						}
						
					}
					//Vertical
					{
						const float yExtraBelow		= propsRender->texRect.y - (float)((SI32)propsRender->texRect.y);
						propsRender->texRect.y		-= yExtraBelow;
						{
							const float yMax 		= (propsRender->texRect.y + propsRender->texRect.height);
							const float yExtraUp	= (yMax - (float)((SI32)yMax));
							propsRender->texRect.height = (float)((SI32)propsRender->texRect.height + (yExtraUp < 0.5f ? 0 : 1));
						}
					}
					NBASSERT(propsRender->texRect.x == (SI32)propsRender->texRect.x)
					NBASSERT(propsRender->texRect.y == (SI32)propsRender->texRect.y)
					NBASSERT((propsRender->texRect.x + propsRender->texRect.width) == (SI32)(propsRender->texRect.x + propsRender->texRect.width))
					NBASSERT((propsRender->texRect.y + propsRender->texRect.height) == (SI32)(propsRender->texRect.y + propsRender->texRect.height))
				}
			}*/
			NBASSERT(propsRender->viewRect.width >= 0.0f)	//Must be positive
			NBASSERT(propsRender->viewRect.height >= 0.0f)	//Must be positive
		}
	} else {
		NBVerticeGL verts[4];
		//Update verts
		{
			NBMATRIZ_MULTIPLICAR_PUNTO(verts[0], _cacheObjEscena.matrizEscena, _limites.x, _limites.y);
			NBMATRIZ_MULTIPLICAR_PUNTO(verts[1], _cacheObjEscena.matrizEscena, limitesMaxX, _limites.y);
			NBMATRIZ_MULTIPLICAR_PUNTO(verts[2], _cacheObjEscena.matrizEscena, _limites.x, limitesMaxY);
			NBMATRIZ_MULTIPLICAR_PUNTO(verts[3], _cacheObjEscena.matrizEscena, limitesMaxX, limitesMaxY);
		}
		//Update text coords
		this->privEstablecerCoordenadasTextura(verts);
		//Update colors
		{
			NBCOLOR_ESTABLECER(verts[0], 255.0f * colorMultiplicado.r, 255.0f * colorMultiplicado.g, 255.0f * colorMultiplicado.b, 255.0f * colorMultiplicado.a);
			NBCOLOR_ESTABLECER(verts[1], 255.0f * colorMultiplicado.r, 255.0f * colorMultiplicado.g, 255.0f * colorMultiplicado.b, 255.0f * colorMultiplicado.a);
			NBCOLOR_ESTABLECER(verts[2], 255.0f * colorMultiplicado.r, 255.0f * colorMultiplicado.g, 255.0f * colorMultiplicado.b, 255.0f * colorMultiplicado.a);
			NBCOLOR_ESTABLECER(verts[3], 255.0f * colorMultiplicado.r, 255.0f * colorMultiplicado.g, 255.0f * colorMultiplicado.b, 255.0f * colorMultiplicado.a);
			/*PRINTF_INFO("Sprite Colores(%d, %d, %d, %d)-(%d, %d, %d, %d)-(%d, %d, %d, %d)-(%d, %d, %d, %d).\n"
			 , verticesGL[0].r, verticesGL[0].g, verticesGL[0].b, verticesGL[0].a
			 , verticesGL[1].r, verticesGL[1].g, verticesGL[1].b, verticesGL[1].a
			 , verticesGL[2].r, verticesGL[2].g, verticesGL[2].b, verticesGL[2].a
			 , verticesGL[3].r, verticesGL[3].g, verticesGL[3].b, verticesGL[3].a)*/
		}
		//Pixel precision
		if(_textura != NULL){
			if((_textura->modoPintado() & NB_TEXTURA_BIT_PINTADO_PRECISO) != 0){
				if((NBMATRIZ_ELEM00(_cacheObjEscena.matrizEscena) == -1 || NBMATRIZ_ELEM00(_cacheObjEscena.matrizEscena) == 0 || NBMATRIZ_ELEM00(_cacheObjEscena.matrizEscena) == 1)
				   && (NBMATRIZ_ELEM01(_cacheObjEscena.matrizEscena) == -1 || NBMATRIZ_ELEM01(_cacheObjEscena.matrizEscena) == 0 || NBMATRIZ_ELEM01(_cacheObjEscena.matrizEscena) == 1)
				   && (NBMATRIZ_ELEM10(_cacheObjEscena.matrizEscena) == -1 || NBMATRIZ_ELEM10(_cacheObjEscena.matrizEscena) == 0 || NBMATRIZ_ELEM10(_cacheObjEscena.matrizEscena) == 1)
				   && (NBMATRIZ_ELEM11(_cacheObjEscena.matrizEscena) == -1 || NBMATRIZ_ELEM11(_cacheObjEscena.matrizEscena) == 0 || NBMATRIZ_ELEM11(_cacheObjEscena.matrizEscena) == 1)){
					NBTamano unidadesEscenaPorPixel;
					unidadesEscenaPorPixel.ancho	= (propsRenderizado.cajaProyeccion.xMax - propsRenderizado.cajaProyeccion.xMin) / propsRenderizado.puertoDeVision.ancho;
					unidadesEscenaPorPixel.alto		= (propsRenderizado.cajaProyeccion.yMax - propsRenderizado.cajaProyeccion.yMin) / propsRenderizado.puertoDeVision.alto;
					NBTamano sobranteDePixel;
					sobranteDePixel.ancho			= (verts[0].x - propsRenderizado.cajaProyeccion.xMin) / unidadesEscenaPorPixel.ancho;
					sobranteDePixel.alto			= (verts[0].y - propsRenderizado.cajaProyeccion.yMin) / unidadesEscenaPorPixel.alto;
					sobranteDePixel.ancho			-= (float)((SI32)sobranteDePixel.ancho);
					sobranteDePixel.alto			-= (float)((SI32)sobranteDePixel.alto);
					NBTamano correccionEnEscena;
					correccionEnEscena.ancho		= (sobranteDePixel.ancho <= 0.5f ? -sobranteDePixel.ancho : 1.0f - sobranteDePixel.ancho) * unidadesEscenaPorPixel.ancho;
					correccionEnEscena.alto			= (sobranteDePixel.alto <= 0.5f ? -sobranteDePixel.alto : 1.0f - sobranteDePixel.alto) * unidadesEscenaPorPixel.alto;
					if(correccionEnEscena.ancho != 0.0f || correccionEnEscena.alto != 0.0f){
						verts[0].x += correccionEnEscena.ancho; verts[0].y += correccionEnEscena.alto;
						verts[1].x += correccionEnEscena.ancho; verts[1].y += correccionEnEscena.alto;
						verts[2].x += correccionEnEscena.ancho; verts[2].y += correccionEnEscena.alto;
						verts[3].x += correccionEnEscena.ancho; verts[3].y += correccionEnEscena.alto;
					}
				}
			}
		}
		//Final structure
		{
			STBloqueGL bloqueVertices;
			bloqueVertices.cantidadElementos = 4;
			bloqueVertices.primerElemento	= (*propsRenderizado.funcReservarVertices)(ENVerticeGlTipo_MonoTextura, 4);
			{
				NBVerticeGL* verticesGL			= &((*propsRenderizado.verticesGL1)[bloqueVertices.primerElemento]);
				//Set vertexs
				verticesGL[0]					= verts[0];
				verticesGL[1]					= verts[1];
				verticesGL[2]					= verts[2];
				verticesGL[3]					= verts[3];
				{
					const SI32 posicionDatos		= propsRenderizado.bytesDatosModelos->conteo; propsRenderizado.bytesDatosModelos->reservarRegistrosAlFinal(sizeof(STSpriteRenderGeo));
					STSpriteRenderGeo* propsRender	= (STSpriteRenderGeo*)&(propsRenderizado.bytesDatosModelos->elemento[posicionDatos]);
					propsRender->type				= ENSpriteRenderType_Geom;
					propsRender->esMascaraAlpha		= _esMascaraAlpha;
					if(_textura != NULL){
						propsRender->idTexturaGL	= _textura->idTexturaGL;
						propsRenderizado.piscinaRetecionRender->agregarElemento(_textura);
					} else {
						//required on some systems where texture-0 will not be drawn
						propsRender->idTexturaGL	= NBGestorTexturas::texturaBlanca->idTexturaGL;
					}
					propsRender->bloqueIndicesGL	= NB_GESTOR_GL_DAME_ELEMS_PARA_TRIANGSTRIP_4(ENVerticeGlTipo_MonoTextura, bloqueVertices);
				}
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return &AUEscenaSprite::enviarComandosGL;
}

void AUEscenaSprite::enviarComandosGL(UI8* punteroDatosModelo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSprite::enviarComandosGL")
	const ENSpriteRenderType* type = (ENSpriteRenderType*)punteroDatosModelo;
	if(*type == ENSpriteRenderType_Tex){
		const STSpriteRenderTex* propsRender = (const STSpriteRenderTex*)punteroDatosModelo;
		const NBColor colorBefore = NBGestorGL::colorActivo();
		{
			NBGestorGL::flushCacheLocal();
			NBGestorGL::color4f((float)propsRender->color.r / 255.0f, (float)propsRender->color.g / 255.0f, (float)propsRender->color.b / 255.0f, (float)propsRender->color.a / 255.0f);
			//PRINTF_INFO("Sprite(%f, %f, %f, %f).\n", (float)propsRender->color.r / 255.0f, (float)propsRender->color.g / 255.0f, (float)propsRender->color.b / 255.0f, (float)propsRender->color.a / 255.0f);
			{
				NBGestorGL::activarVerticesGL(ENVerticeGlTipo_MonoTextura); NBGESTORGL_DBG_NOMBRAR_ACTIVADOR_VERTICES_GL("AUEscenaSprite")
				NBGestorGL::bindTexture(0, propsRender->idTexturaGL);
				NBGestorGL::setTextureCropRect(&propsRender->texRect);
				if(propsRender->esMascaraAlpha){
					NBGestorGL::blendFunc(GL_ZERO, GL_SRC_ALPHA);
					NBGestorGL::colorMask(false, false, false, true);
					NBGestorGL::drawTex(propsRender->viewRect.x, propsRender->viewRect.y, 0.0f, propsRender->viewRect.width, propsRender->viewRect.height);
					NBGestorGL::colorMask(true, true, true, true);
					NBGestorGL::blendFuncCargarValoresPorDefecto();
				} else {
					NBGestorGL::drawTex(propsRender->viewRect.x, propsRender->viewRect.y, 0.0f, propsRender->viewRect.width, propsRender->viewRect.height);
				}
			}
			NBGestorGL::color4f(colorBefore.r, colorBefore.g, colorBefore.b, colorBefore.a);
		}
	} else {
		NBASSERT(*type == ENSpriteRenderType_Geom)
		const STSpriteRenderGeo* propsRender = (const STSpriteRenderGeo*)punteroDatosModelo;
		NBGestorGL::activarVerticesGL(ENVerticeGlTipo_MonoTextura); NBGESTORGL_DBG_NOMBRAR_ACTIVADOR_VERTICES_GL("AUEscenaSprite")
		//Configurar textura
		NBGestorGL::bindTexture(0, propsRender->idTexturaGL);
		//Acumular triangStrip independiente mediante indices.
		if(propsRender->esMascaraAlpha){
			NBGestorGL::blendFunc(GL_ZERO, GL_SRC_ALPHA);
			NBGestorGL::colorMask(false, false, false, true);
			//PRINTF_INFO("   Sprite\n");
			NB_GESTOR_GL_RENDER_TRIANGSTRIP_4(propsRender->bloqueIndicesGL.primerElemento, propsRender->bloqueIndicesGL.cantidadElementos); NBGESTORGL_DBG_NOMBRAR_CONVOCADOR_ACUMULAR_TRIANG_STRIPS_INDEP("AUEscenaSprite")
			NBGestorGL::colorMask(true, true, true, true);
			NBGestorGL::blendFuncCargarValoresPorDefecto();
		} else {
			//PRINTF_INFO("   Sprite\n");
			NB_GESTOR_GL_RENDER_TRIANGSTRIP_4(propsRender->bloqueIndicesGL.primerElemento, propsRender->bloqueIndicesGL.cantidadElementos); NBGESTORGL_DBG_NOMBRAR_CONVOCADOR_ACUMULAR_TRIANG_STRIPS_INDEP("AUEscenaSprite")
		}
		//PRINTF_INFO("Sprite color antes(%f, %f, %f, %f) despues(%f, %f, %f, %f).\n", dbgColorAntes[0], dbgColorAntes[1], dbgColorAntes[2], dbgColorAntes[3], dbgColorDesp[0], dbgColorDesp[1], dbgColorDesp[2], dbgColorDesp[3]);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

//


bool AUEscenaSprite::agregarXmlInternoEn(AUEscenaSprite* sprite, AUCadenaLargaMutable8* guardarEn, const char* espaciosBaseIzq){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSprite::agregarXmlInternoEn")
	//guardarEn->agregarConFormato("%s<vXML>1</vXML>\r\n", espaciosBaseIzq);
	guardarEn->agregarConFormato("%s<eOb>\r\n", espaciosBaseIzq);
	AUEscenaObjeto::agregarXmlInternoEn(sprite, guardarEn, espaciosBaseIzq);
	guardarEn->agregarConFormato("%s</eOb>\r\n", espaciosBaseIzq);
	guardarEn->agregarConFormato("%s<lims>%f|%f|%f|%f</lims>\r\n", espaciosBaseIzq, sprite->_limites.x, sprite->_limites.y, sprite->_limites.ancho, sprite->_limites.alto);
	guardarEn->agregarConFormato("%s<m>%d</m>\r\n", espaciosBaseIzq, sprite->_modoTextura);
	if(sprite->_textura != NULL){
		const char* nombreTextura = NBGestorTexturas::nombreRecursoTextura(sprite->_textura);
		if(nombreTextura != NULL){
			guardarEn->agregarConFormato("%s<tex>%s</tex>\r\n", espaciosBaseIzq, nombreTextura);
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return true;

}

bool AUEscenaSprite::interpretarXmlInterno(AUEscenaSprite* cargarEn, const AUDatosXML* datosXml, const sNodoXML* nodoXml){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSprite::interpretarXmlInterno")
	bool exito = false;
	//int versionXML = datosXml->nodoHijo("vXML", 0, nodoXml);
	//if(versionXML==1){
	const sNodoXML* nodoEscenaObj;			XML_NODO_HIJO(datosXml, nodoEscenaObj, "eOb", "escObj", nodoXml, NULL)
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
			AUCadenaMutable8* nomTextura = new(ENMemoriaTipo_Temporal) AUCadenaMutable8();
			datosXml->nodoHijo("tex", nomTextura, "", nodoXml);
			cargarEn->redimensionar(x, y, ancho, alto);
			if(nomTextura->tamano()!=0){
				cargarEn->establecerTextura(NBGestorTexturas::texturaDesdeArchivo(nomTextura->str()));
			}
			nomTextura->liberar(NB_RETENEDOR_NULL);
		}
	//}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

bool AUEscenaSprite::agregarBitsInternosEn(AUEscenaSprite* sprite, AUArchivo* guardarEn){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSprite::agregarBitsInternosEn")
	bool exito = false;
	//
	float versionDatos = 1.0f;
	guardarEn->escribir(&versionDatos, sizeof(versionDatos), 1, guardarEn);
	if(AUEscenaObjeto::agregarBitsInternosEn(sprite, guardarEn)){
		guardarEn->escribir(&sprite->_limites, sizeof(sprite->_limites), 1, guardarEn);
		guardarEn->escribir(&sprite->_modoTextura, sizeof(sprite->_modoTextura), 1, guardarEn);
		//
		const char* nombreTextura = NULL;
		SI32 tamanoNombreTextura = 0;
		if(sprite->_textura != NULL) nombreTextura = NBGestorTexturas::nombreRecursoTextura(sprite->_textura);
		if(nombreTextura == NULL){
			guardarEn->escribir(&tamanoNombreTextura, sizeof(tamanoNombreTextura), 1, guardarEn);
		} else {
			tamanoNombreTextura = AUCadenaMutable8::tamano(nombreTextura);
			guardarEn->escribir(&tamanoNombreTextura, sizeof(tamanoNombreTextura), 1, guardarEn);
			guardarEn->escribir(nombreTextura, sizeof(char), tamanoNombreTextura, guardarEn);
		}
		exito = true;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

bool AUEscenaSprite::interpretarBitsInternos(AUEscenaSprite* cargarEn, AUArchivo* cargarDe){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSprite::interpretarBitsInternos")
	bool exito = false;
	float versionDatos;
	cargarDe->leer(&versionDatos, sizeof(versionDatos), 1, cargarDe);
	if(versionDatos==1.0f){
		if(AUEscenaObjeto::interpretarBitsInternos(cargarEn, cargarDe)){
			cargarDe->leer(&cargarEn->_limites, sizeof(cargarEn->_limites), 1, cargarDe);
			cargarDe->leer(&cargarEn->_modoTextura, sizeof(cargarEn->_modoTextura), 1, cargarDe);
			cargarEn->redimensionar(cargarEn->_limites.x, cargarEn->_limites.y, cargarEn->_limites.ancho, cargarEn->_limites.alto);
			SI32 tamanoNombreTextura;
			cargarDe->leer(&tamanoNombreTextura, sizeof(tamanoNombreTextura), 1, cargarDe);
			if(tamanoNombreTextura>0){
				char* nombreTextura = (char*)NBGestorMemoria::reservarMemoria(tamanoNombreTextura+1, ENMemoriaTipo_Temporal); NB_DEFINE_NOMBRE_PUNTERO(nombreTextura, "AUEscenaSprite::nombreTextura") //+1 es el "\0"
				cargarDe->leer(nombreTextura, sizeof(char), tamanoNombreTextura, cargarDe);
				nombreTextura[tamanoNombreTextura] = '\0';
				cargarEn->establecerTextura(NBGestorTexturas::texturaDesdeArchivo(nombreTextura));
				NBGestorMemoria::liberarMemoria(nombreTextura);
			}
			exito = true;
		}
	} else {
		PRINTF_ERROR("AUEscenaSprite, la version (%f) de datos binarios no es interpretable por este ejecutable\n", versionDatos);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}


//

AUOBJMETODOS_CLASESID_MULTICLASE(AUEscenaSprite, AUEscenaObjeto)
AUOBJMETODOS_CLASESNOMBRES_MULTICLASE(AUEscenaSprite, "AUEscenaSprite", AUEscenaObjeto)
AUOBJMETODOS_CLONAR_THIS(AUEscenaSprite)





