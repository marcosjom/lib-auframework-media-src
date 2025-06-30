
#include "AUFrameworkMediaStdAfx.h"
#include "AUEscenaSpriteElastico.h"

//-------------------------------------
//--- CONSTRUCTORES Y DESTRUCTORES ----
//-------------------------------------

AUEscenaSpriteElastico::AUEscenaSpriteElastico() : AUEscenaSprite()
	, _mallaMapa(NULL)
{
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSpriteElastico::AUEscenaSpriteElastico")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaSpriteElastico")
	NBTAMANO_ESTABLECER(_mallaFactorEstira, 0.5f, 0.5f)
	//Map cache
	{
		NBMemory_setZero(_mapCache);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUEscenaSpriteElastico::AUEscenaSpriteElastico(NBTamano tamano) : AUEscenaSprite(tamano)
	, _mallaMapa(NULL)
{
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSpriteElastico::AUEscenaSpriteElastico")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaSpriteElastico")
	NBTAMANO_ESTABLECER(_mallaFactorEstira, 0.5f, 0.5f)
	//Map cache
	{
		NBMemory_setZero(_mapCache);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUEscenaSpriteElastico::AUEscenaSpriteElastico(float ancho, float alto) : AUEscenaSprite(ancho, alto)
	, _mallaMapa(NULL)
{
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSpriteElastico::AUEscenaSpriteElastico")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaSpriteElastico")
	NBTAMANO_ESTABLECER(_mallaFactorEstira, 0.5f, 0.5f)
	//Map cache
	{
		NBMemory_setZero(_mapCache);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUEscenaSpriteElastico::AUEscenaSpriteElastico(float x, float y, float ancho, float alto) : AUEscenaSprite(x, y, ancho, alto)
	, _mallaMapa(NULL)
{
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSpriteElastico::AUEscenaSpriteElastico")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaSpriteElastico")
	NBTAMANO_ESTABLECER(_mallaFactorEstira, 0.5f, 0.5f)
	//Map cache
	{
		NBMemory_setZero(_mapCache);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUEscenaSpriteElastico::AUEscenaSpriteElastico(AUTextura* textura) : AUEscenaSprite(textura)
	, _mallaMapa(NULL)
{
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSpriteElastico::AUEscenaSpriteElastico")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaSpriteElastico")
	NBTAMANO_ESTABLECER(_mallaFactorEstira, 0.5f, 0.5f)
	//Map cache
	{
		NBMemory_setZero(_mapCache);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUEscenaSpriteElastico::AUEscenaSpriteElastico(AUTextura* textura, const ENSpriteAlineaH alinH, const ENSpriteAlineaV alinV) : AUEscenaSprite(textura, alinH, alinV)
	, _mallaMapa(NULL)
{
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSpriteElastico::AUEscenaSpriteElastico")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaSpriteElastico")
	NBTAMANO_ESTABLECER(_mallaFactorEstira, 0.5f, 0.5f)
	//Map cache
	{
		NBMemory_setZero(_mapCache);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUEscenaSpriteElastico::AUEscenaSpriteElastico(AUEscenaSpriteElastico* otra) : AUEscenaSprite(otra)
	, _mallaMapa(NULL)
{
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSpriteElastico::AUEscenaSpriteElastico")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaSpriteElastico")
	NBTAMANO_ESTABLECER(_mallaFactorEstira, 0.5f, 0.5f)
	//Map cache
	{
		NBMemory_setZero(_mapCache);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUEscenaSpriteElastico::~AUEscenaSpriteElastico(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSpriteElastico::~AUEscenaSpriteElastico")
	if(_mallaMapa != NULL) _mallaMapa->liberar(NB_RETENEDOR_THIS); _mallaMapa = NULL;
	//Map cache
	{
		NBMemory_setZero(_mapCache);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

//

AUMallaMapa* AUEscenaSpriteElastico::mallaMapa() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSpriteElastico::mallaMapa")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _mallaMapa;
}

void AUEscenaSpriteElastico::establecerMallaMapa(AUMallaMapa* mallaMapa){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSpriteElastico::establecerMallaMapa")
	if(_mallaMapa != mallaMapa){
		if(mallaMapa != NULL) mallaMapa->retener(NB_RETENEDOR_THIS);
		if(_mallaMapa != NULL) _mallaMapa->liberar(NB_RETENEDOR_THIS);
		_mallaMapa = mallaMapa;
		//
		if(this->idEscena >= 0){
			this->setModelChangedFlag();
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

NBTamano AUEscenaSpriteElastico::mallaFactorEstira() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSpriteElastico::mallaFactorEstira")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _mallaFactorEstira;

}

void AUEscenaSpriteElastico::establecerMallaFactorEstira(const NBTamano factor){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSpriteElastico::establecerMallaFactorEstira")
	STNBSize sz;
	sz.width	= (factor.ancho < 0.0f ? 0.0f : (factor.ancho > 1.0f ? 1.0f : factor.ancho));
	sz.height	= (factor.alto < 0.0f ? 0.0f : (factor.alto > 1.0f ? 1.0f : factor.alto));
	if(_mallaFactorEstira.ancho != sz.width || _mallaFactorEstira.alto != sz.height){
		_mallaFactorEstira.ancho	= sz.width;
		_mallaFactorEstira.alto		= sz.height;
		//
		if(this->idEscena >= 0){
			this->setModelChangedFlag();
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaSpriteElastico::establecerMallaFactorEstira(const float factorH, const float factorV){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSpriteElastico::establecerMallaFactorEstira")
	STNBSize sz;
	sz.width	= (factorH < 0.0f ? 0.0f : (factorH > 1.0f ? 1.0f : factorH));
	sz.height	= (factorV < 0.0f ? 0.0f : (factorV > 1.0f ? 1.0f : factorV));
	if(_mallaFactorEstira.ancho != sz.width || _mallaFactorEstira.alto != sz.height){
		_mallaFactorEstira.ancho	= sz.width;
		_mallaFactorEstira.alto		= sz.height;
		//
		if(this->idEscena >= 0){
			this->setModelChangedFlag();
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

//-------------------------------
//--- FUNCIONES DE INSTANCIA ----
//-------------------------------

NBVerticeGL* AUEscenaSpriteElastico::privBuildModelAddRow(const float yLocal, const float texAtlasY, NBVerticeGL* verticesGL, const NBVerticeGL* verticesGLSigUlt, const AUArregloNativoP<STMallaCorte>* cutsV, const float mapMultiplier, const bool forceElasticH, const float totalElasticWidths, const SI32 totalElasticCutsH, const float factorElasticH, const float scaleFixedCols, const float scaleElasticCols, const float xLocalMin, const float texWidth, const float texScaleH, const double texAtlasXMin, const double texAtlasXMax, const NBColor8* colorMultiplicado, const NBMatriz &matrizEscena){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSpriteElastico::privBuildModelAddRow")
	//Column first vertex
	NBMATRIZ_MULTIPLICAR_PUNTO(verticesGL[0], matrizEscena, xLocalMin, yLocal);
	NBPUNTO_ESTABLECER(verticesGL[0].tex, texAtlasXMin, texAtlasY)
	NBCOLOR_ESTABLECER(verticesGL[0], colorMultiplicado->r, colorMultiplicado->g, colorMultiplicado->b, colorMultiplicado->a);
	verticesGL++; NBASSERT(verticesGL <= verticesGLSigUlt)
	//Column vertexs sequence
	ENMallaCorteTipo lastType = ENMallaCorteTipo_Rigido;
	float xTex = 0.0f, xTexAnt, xSprite = xLocalMin, xSpriteAnt;
	double texAtlasX = texAtlasXMin, texAtlasXAnt;
	SI32 iElasticCut = 0;
	SI32 i; const SI32 count = cutsV->conteo;
	for(i = 0; i < count && xTex < texWidth; i++){
		STMallaCorte* range = cutsV->elemPtr(i);
		float sizeInTex = (float)range->tamano * mapMultiplier;
		const ENMallaCorteTipo rangeType = (forceElasticH ? ENMallaCorteTipo_Estirar : range->tipo);
		//Fix the last column to remainig size
		if((i + 1) == count){ sizeInTex = texWidth - xTex; }
		//
		NBASSERT(sizeInTex >= 0.0f)
		if(sizeInTex > 0.0f){
			xTexAnt	= xTex;
			xTex	+= sizeInTex;
			float sizeInSprite = (sizeInTex * texScaleH * (rangeType == ENMallaCorteTipo_Rigido ? scaleFixedCols : scaleElasticCols));
			if(sizeInSprite != 0.0f){ //sizeInSprite can be negative or positive
				xSpriteAnt	= xSprite;
				xSprite		+= sizeInSprite;
				texAtlasXAnt= texAtlasX;
				texAtlasX	= texAtlasXMin + ((texAtlasXMax - texAtlasXMin) * (double)(xTex / texWidth)); NBASSERT(texAtlasX >= texAtlasXMin && texAtlasX <= texAtlasXMax)
				//End last repeat-seq (could be a partial-texture segment)
				if(lastType == ENMallaCorteTipo_Repetir){
					NBMATRIZ_MULTIPLICAR_PUNTO(verticesGL[0], matrizEscena, xSpriteAnt, yLocal);
					NBPUNTO_ESTABLECER(verticesGL[0].tex, texAtlasXAnt, texAtlasY)
					NBCOLOR_ESTABLECER(verticesGL[0], colorMultiplicado->r, colorMultiplicado->g, colorMultiplicado->b, colorMultiplicado->a);
					verticesGL++; NBASSERT(verticesGL <= verticesGLSigUlt)
				}
				//Add additional vertexs (only for repeat)
				if(rangeType == ENMallaCorteTipo_Repetir){
					const float repeatsF	= (sizeInSprite / sizeInTex);
					const SI32 repeatsI		= (SI32)repeatsF - (repeatsF == (float)((SI32)repeatsF) ? 1 : 0);
					SI32 i; float xSpriteR	= xSpriteAnt; //PRINTF_INFO("Repeats %.2f times, starts on x(%.2f) tex(%.2f).\n", repeatsF, xSpriteR, texAtlasXAnt);
					for(i = (repeatsI - 1); i >= 0 ; i--){
						xSpriteR += (sizeInTex * texScaleH); //PRINTF_INFO("Full repeat to x(%.2f) tex(%.2f, %.2f).\n", xSpriteR, texAtlasX, texAtlasXAnt);
						//End of one full-repeat
						NBMATRIZ_MULTIPLICAR_PUNTO(verticesGL[0], matrizEscena, xSpriteR, yLocal);
						NBPUNTO_ESTABLECER(verticesGL[0].tex, texAtlasX, texAtlasY)
						NBCOLOR_ESTABLECER(verticesGL[0], colorMultiplicado->r, colorMultiplicado->g, colorMultiplicado->b, colorMultiplicado->a);
						verticesGL++; NBASSERT(verticesGL <= verticesGLSigUlt)
						//Begin of the next repeat
						NBMATRIZ_MULTIPLICAR_PUNTO(verticesGL[0], matrizEscena, xSpriteR, yLocal);
						NBPUNTO_ESTABLECER(verticesGL[0].tex, texAtlasXAnt, texAtlasY)
						NBCOLOR_ESTABLECER(verticesGL[0], colorMultiplicado->r, colorMultiplicado->g, colorMultiplicado->b, colorMultiplicado->a);
						verticesGL++; NBASSERT(verticesGL <= verticesGLSigUlt)
					}
					if(repeatsF != (float)repeatsI){
						const float relSizeRepeat	= repeatsF - (float)repeatsI; NBASSERT(relSizeRepeat >= 0.0f && relSizeRepeat <= 1.0f)
						const float texAtlasXPartial= texAtlasXAnt + ((texAtlasX - texAtlasXAnt) * relSizeRepeat);
						//End of the repeat sequence (partial size)
						NBMATRIZ_MULTIPLICAR_PUNTO(verticesGL[0], matrizEscena, xSprite, yLocal); //PRINTF_INFO("Partial(%.2f) end of repeat to x(%.2f) tex(%.2f).\n", relSizeRepeat, xSprite, texAtlasXPartial);
						NBPUNTO_ESTABLECER(verticesGL[0].tex, texAtlasXPartial, texAtlasY)
						NBCOLOR_ESTABLECER(verticesGL[0], colorMultiplicado->r, colorMultiplicado->g, colorMultiplicado->b, colorMultiplicado->a);
						verticesGL++; NBASSERT(verticesGL <= verticesGLSigUlt)
					} else {
						//End of the repeat sequence (full size)
						NBMATRIZ_MULTIPLICAR_PUNTO(verticesGL[0], matrizEscena, xSprite, yLocal); //PRINTF_INFO("Full end to x(%.2f).\n", xSprite);
						NBPUNTO_ESTABLECER(verticesGL[0].tex, texAtlasX, texAtlasY)
						NBCOLOR_ESTABLECER(verticesGL[0], colorMultiplicado->r, colorMultiplicado->g, colorMultiplicado->b, colorMultiplicado->a);
						verticesGL++; NBASSERT(verticesGL <= verticesGLSigUlt)
					}
				} else {
					//Add vertex
					NBMATRIZ_MULTIPLICAR_PUNTO(verticesGL[0], matrizEscena, xSprite, yLocal);
					NBPUNTO_ESTABLECER(verticesGL[0].tex, texAtlasX, texAtlasY)
					NBCOLOR_ESTABLECER(verticesGL[0], colorMultiplicado->r, colorMultiplicado->g, colorMultiplicado->b, colorMultiplicado->a);
					verticesGL++; NBASSERT(verticesGL <= verticesGLSigUlt)
				}
				if(rangeType != ENMallaCorteTipo_Rigido){
					iElasticCut++;
					NBASSERT(iElasticCut <= totalElasticCutsH)
				}
				lastType = rangeType;
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return verticesGL;
}

PTRfunCmdsGL AUEscenaSpriteElastico::actualizarModeloGL(const NBPropRenderizado &propsRenderizado, const NBPropIluminacion &propsIluminacion, const NBPropHeredadasRender &propsHeredadas){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSpriteElastico::actualizarModeloGL")
	PTRfunCmdsGL r = NULL;
	if(_textura == NULL){
		//No texture (use sprite's code for optimization)
		r = AUEscenaSprite::actualizarModeloGL(propsRenderizado, propsIluminacion, propsHeredadas);
	} else {
		AUMallaMapa* mallaMapa = _mallaMapa;
		if(mallaMapa == NULL){
			mallaMapa = _textura->mallaMapa();
		}
		if(mallaMapa == NULL){
			//No mesh (use sprite's code for optimization)
			r = AUEscenaSprite::actualizarModeloGL(propsRenderizado, propsIluminacion, propsHeredadas);
		} else {
			//We have texture and mesh (visual effects can be seen)
			BOOL mapCacheIsValid = FALSE;
			//Validate or calculate mapCache
			{
				if(_mapCache.texPtr == _textura && _mapCache.meshPtr == mallaMapa && _mapCache.data.printSize.width == _limites.ancho && _mapCache.data.printSize.height == _limites.alto){
					//Optimization: reuse cache
					mapCacheIsValid = TRUE;
				} else {
					//Build new cache
					const NBTamano textSize		= _textura->tamano();
					const float texScaleHD		= _textura->escalaParaHD();
					const UI8 texScaleBase2		= _textura->escalaBase2();
					NBMemory_setZero(_mapCache.data);
					if(mallaMapa->calculateBaseMap(&_mapCache.data, NBST_P(STNBSize, _limites.ancho, _limites.alto), NBST_P(STNBSize, textSize.ancho, textSize.alto), texScaleHD, texScaleBase2)){
						_mapCache.texPtr	= _textura;
						_mapCache.meshPtr	= mallaMapa;
						mapCacheIsValid		= TRUE;
					}
				}
			}
			//Build model
			if(!mapCacheIsValid){
				//No valid map (use sprite's code for optimization)
				r = AUEscenaSprite::actualizarModeloGL(propsRenderizado, propsIluminacion, propsHeredadas);
			} else {
				const STMallaMapaBase* mapBase = &_mapCache.data;
				const AUArregloNativoP<STMallaCorte>* cutsV = mallaMapa->cortesEnSentido(ENMallaCorteSentido_Vertical);
				const AUArregloNativoP<STMallaCorte>* cutsH = mallaMapa->cortesEnSentido(ENMallaCorteSentido_Horizontal);
				//Generate model
				{
					STBloqueGL bloqueVertices;
					bloqueVertices.cantidadElementos	= (mapBase->countVertexNeededH * mapBase->countVertexNeededV);
					bloqueVertices.primerElemento		= (*propsRenderizado.funcReservarVertices)(ENVerticeGlTipo_MonoTextura, bloqueVertices.cantidadElementos); NBASSERT(bloqueVertices.cantidadElementos > 0)
					NBVerticeGL* verticesGL				= &((*propsRenderizado.verticesGL1)[bloqueVertices.primerElemento]);
					const NBVerticeGL* verticesGLSigUlt	= &verticesGL[bloqueVertices.cantidadElementos];
					//
					const SI32 posicionDatos			= propsRenderizado.bytesDatosModelos->conteo; propsRenderizado.bytesDatosModelos->reservarRegistrosAlFinal(sizeof(STSpriteElasticoRender));
					SI32 posicionIndicesSaltos			= 0;
					UI16* arrayIndicesSaltos			= NULL;
					UI16* arrayIndicesSaltosSigUlt		= NULL;
					if(mapBase->countRowsIgnorable != 0){
						posicionIndicesSaltos			= propsRenderizado.bytesDatosModelos->conteo; propsRenderizado.bytesDatosModelos->reservarRegistrosAlFinal(sizeof(UI16) * mapBase->countRowsIgnorable);
						arrayIndicesSaltos				= (UI16*)&(propsRenderizado.bytesDatosModelos->elemento[posicionIndicesSaltos]);
						arrayIndicesSaltosSigUlt		= &arrayIndicesSaltos[mapBase->countRowsIgnorable];
					}
					//
					NBColor colorMultiplicadoF;			NBCOLOR_MULTIPLICAR_CON_UI8(colorMultiplicadoF, propsHeredadas.colorPadre, _propiedades.color8);
					NBColor8 colorMultiplicado;			NBCOLOR_ESTABLECER(colorMultiplicado, 255.0f * colorMultiplicadoF.r, 255.0f * colorMultiplicadoF.g, 255.0f * colorMultiplicadoF.b, 255.0f * colorMultiplicadoF.a)
					//
					const NBCajaAABBP<double> cajaTexeles = _textura->cajaRelativaTexeles();
					NBTamanoP<double> tamCajaTexeles;	NBCAJAAABB_TAMANO(tamCajaTexeles, cajaTexeles)
					NBPuntoP<double> limitesMax;		NBPUNTO_ESTABLECER(limitesMax, (_limites.x + _limites.ancho), (_limites.y + _limites.alto));
					const ENTexturaOrdenV texOrden		= _textura->ordenFilasImagen();
					const float yInitial				= (texOrden == ENTexturaOrdenV_HaciaAbajo ? _limites.y : _limites.y + _limites.alto);
					const float	yMovSense				= (texOrden == ENTexturaOrdenV_HaciaAbajo ? 1.0f : -1.0f);
					//Generar fila inicial
					verticesGL	= AUEscenaSpriteElastico::privBuildModelAddRow(yInitial, cajaTexeles.yMax, verticesGL, verticesGLSigUlt, cutsV, mapBase->mapMultiplier, mapBase->forceElasticH, mapBase->totalElasticWidthsHD, mapBase->totalElasticCutsH, _mallaFactorEstira.ancho, mapBase->scaleFixedCols, mapBase->scaleElasticCols, _limites.x, mapBase->textSize.width, mapBase->texScaleH, cajaTexeles.xMin, cajaTexeles.xMax, &colorMultiplicado, _cacheObjEscena.matrizEscena);
					//Generar final de cada fila
					{
						ENMallaCorteTipo lastType = ENMallaCorteTipo_Rigido;
						float yTex = 0.0f, yTexAnt, ySprite = yInitial, ySpriteAnt; double texAtlasY = cajaTexeles.yMax, texAtlasYAnt;
						SI32 i; const SI32 count = cutsH->conteo;
						for( i = 0; i < count && yTex < mapBase->textSize.height; i++){
							STMallaCorte* range = cutsH->elemPtr(i);
							float sizeInTex = (float)range->tamano * mapBase->mapMultiplier;
							const ENMallaCorteTipo rangeType = (mapBase->forceElasticV ? ENMallaCorteTipo_Estirar : range->tipo);
							//Fix the last row to remainig size
							if((i + 1) == count){ sizeInTex =  mapBase->textSize.height - yTex; }
							//
							if(sizeInTex > 0.0f){
								yTexAnt	= yTex;
								yTex	+= sizeInTex;
								const float sizeInSprite = ((sizeInTex * mapBase->texScaleV) * yMovSense * (rangeType == ENMallaCorteTipo_Rigido ? mapBase->scaleFixedRows : mapBase->scaleElasticRows));// * _limites.alto /  mapBase->textSize.height;
								if(sizeInSprite != 0.0f){ //sizeInSprite can be negative or positive
									ySpriteAnt		= ySprite;
									ySprite			+= sizeInSprite;
									texAtlasYAnt	= texAtlasY;
									texAtlasY		= cajaTexeles.yMax - ((cajaTexeles.yMax - cajaTexeles.yMin) * (double)(yTex / mapBase->textSize.height)); NBASSERT(texAtlasY >= (cajaTexeles.yMin - 0.000001) && texAtlasY <= (cajaTexeles.yMax + 0.000001))
									//End last repeat-seq (could be a partial-texture segment)
									if(lastType == ENMallaCorteTipo_Repetir){
										//Copy last row-sequence and update the coordinates
										SI32 i; NBVerticeGL* lastRowVertexs = verticesGL - mapBase->countVertexNeededH;
										for(i = 0; i < mapBase->countVertexNeededH; i++){
											verticesGL[i] = lastRowVertexs[i];
											verticesGL[i].y = ySpriteAnt;
											verticesGL[i].tex.y = texAtlasYAnt;
										}
										verticesGL += mapBase->countVertexNeededH;
									}
									//
									if(rangeType == ENMallaCorteTipo_Repetir){
										const float repeatsF	= (sizeInSprite / sizeInTex);
										const SI32 repeatsI		= (SI32)repeatsF - (repeatsF == (float)((SI32)repeatsF) ? 1 : 0);
										SI32 i; float ySpriteR	= ySpriteAnt; //PRINTF_INFO("Repeats %.2f times, starts on y(%.2f) tex(%.2f).\n", repeatsF, ySpriteR, texAtlasYAnt);
										for(i = (repeatsI - 1); i >= 0 ; i--){
											ySpriteR += (sizeInTex * mapBase->texScaleV) * yMovSense;
											//PRINTF_INFO("Full repeat to y(%.2f) tex(%.2f, %.2f).\n", ySpriteR, texAtlasY, texAtlasYAnt);
											//End of one full-repeat
											verticesGL	= AUEscenaSpriteElastico::privBuildModelAddRow(ySpriteR, texAtlasY, verticesGL, verticesGLSigUlt, cutsV, mapBase->mapMultiplier, mapBase->forceElasticH, mapBase->totalElasticWidthsHD, mapBase->totalElasticCutsH, _mallaFactorEstira.ancho, mapBase->scaleFixedCols, mapBase->scaleElasticCols, _limites.x, mapBase->textSize.width, mapBase->texScaleH, cajaTexeles.xMin, cajaTexeles.xMax, &colorMultiplicado, _cacheObjEscena.matrizEscena);
											//Begin of the next repeat
											verticesGL	= AUEscenaSpriteElastico::privBuildModelAddRow(ySpriteR, texAtlasYAnt, verticesGL, verticesGLSigUlt, cutsV, mapBase->mapMultiplier, mapBase->forceElasticH, mapBase->totalElasticWidthsHD, mapBase->totalElasticCutsH, _mallaFactorEstira.ancho, mapBase->scaleFixedCols, mapBase->scaleElasticCols, _limites.x, mapBase->textSize.width, mapBase->texScaleH, cajaTexeles.xMin, cajaTexeles.xMax, &colorMultiplicado, _cacheObjEscena.matrizEscena);
										}
										if(repeatsF != (float)repeatsI){
											const float relSizeRepeat	= repeatsF - (float)repeatsI; NBASSERT(relSizeRepeat >= 0.0f && relSizeRepeat <= 1.0f)
											const float texAtlasYPartial= texAtlasYAnt + ((texAtlasY - texAtlasYAnt) * relSizeRepeat);
											//End of the repeat sequence (partial size)
											//PRINTF_INFO("Partial(%.2f) end of repeat to y(%.2f) tex(%.2f).\n", relSizeRepeat, ySprite, texAtlasYPartial);
											verticesGL	= AUEscenaSpriteElastico::privBuildModelAddRow(ySprite, texAtlasYPartial, verticesGL, verticesGLSigUlt, cutsV, mapBase->mapMultiplier, mapBase->forceElasticH, mapBase->totalElasticWidthsHD, mapBase->totalElasticCutsH, _mallaFactorEstira.ancho, mapBase->scaleFixedCols, mapBase->scaleElasticCols, _limites.x, mapBase->textSize.width, mapBase->texScaleH, cajaTexeles.xMin, cajaTexeles.xMax, &colorMultiplicado, _cacheObjEscena.matrizEscena);
										} else {
											//End of the repeat sequence (full size)
											//PRINTF_INFO("Full end to y(%.2f).\n", ySprite);
											verticesGL	= AUEscenaSpriteElastico::privBuildModelAddRow(ySprite, texAtlasY, verticesGL, verticesGLSigUlt, cutsV, mapBase->mapMultiplier, mapBase->forceElasticH, mapBase->totalElasticWidthsHD, mapBase->totalElasticCutsH, _mallaFactorEstira.ancho, mapBase->scaleFixedCols, mapBase->scaleElasticCols, _limites.x, mapBase->textSize.width, mapBase->texScaleH, cajaTexeles.xMin, cajaTexeles.xMax, &colorMultiplicado, _cacheObjEscena.matrizEscena);
										}
									} else {
										verticesGL	= AUEscenaSpriteElastico::privBuildModelAddRow(ySprite, texAtlasY, verticesGL, verticesGLSigUlt, cutsV, mapBase->mapMultiplier, mapBase->forceElasticH, mapBase->totalElasticWidthsHD, mapBase->totalElasticCutsH, _mallaFactorEstira.ancho, mapBase->scaleFixedCols, mapBase->scaleElasticCols, _limites.x, mapBase->textSize.width, mapBase->texScaleH, cajaTexeles.xMin, cajaTexeles.xMax, &colorMultiplicado, _cacheObjEscena.matrizEscena);
									}
									lastType = rangeType;
								}
							}
						}
					}
					//Pixel precision
					if(_textura != NULL){
						if((_textura->modoPintado() & NB_TEXTURA_BIT_PINTADO_PRECISO) != 0){
							if((NBMATRIZ_ELEM00(_cacheObjEscena.matrizEscena) == -1 || NBMATRIZ_ELEM00(_cacheObjEscena.matrizEscena) == 0 || NBMATRIZ_ELEM00(_cacheObjEscena.matrizEscena) == 1)
							   && (NBMATRIZ_ELEM01(_cacheObjEscena.matrizEscena) == -1 || NBMATRIZ_ELEM01(_cacheObjEscena.matrizEscena) == 0 || NBMATRIZ_ELEM01(_cacheObjEscena.matrizEscena) == 1)
							   && (NBMATRIZ_ELEM10(_cacheObjEscena.matrizEscena) == -1 || NBMATRIZ_ELEM10(_cacheObjEscena.matrizEscena) == 0 || NBMATRIZ_ELEM10(_cacheObjEscena.matrizEscena) == 1)
							   && (NBMATRIZ_ELEM11(_cacheObjEscena.matrizEscena) == -1 || NBMATRIZ_ELEM11(_cacheObjEscena.matrizEscena) == 0 || NBMATRIZ_ELEM11(_cacheObjEscena.matrizEscena) == 1)){
								NBVerticeGL* verticesGL			= &((*propsRenderizado.verticesGL1)[bloqueVertices.primerElemento]);
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
									do {
										verticesGL[0].x += correccionEnEscena.ancho;
										verticesGL[0].y += correccionEnEscena.alto;
										verticesGL++;
									} while (verticesGL < verticesGLSigUlt);
								}
							}
						}
					}
					//PRINTF_INFO("Se utilizaron %d vertices adicionales.\n", (SI32)(verticesGL - verticesGLSigUlt));
					NBASSERT(verticesGL == verticesGLSigUlt)
					//Generate indexes-array
					STBloqueGL bloqueIndices;
					bloqueIndices.cantidadElementos		= (((mapBase->countVertexNeededH * 2) + 2) * (mapBase->countVertexNeededV - 1)); //+2 (repeat the initial and last vertex to form an independent triang-strip for each row)
					bloqueIndices.primerElemento		= (*propsRenderizado.funcReservarIndices)(ENVerticeGlTipo_MonoTextura, bloqueIndices.cantidadElementos); NBASSERT(bloqueIndices.cantidadElementos > 0)
					GLushort* indicesGL					= &((*propsRenderizado.indicesGL1)[bloqueIndices.primerElemento]);
					const GLushort* indicesGLSigUlt		= &indicesGL[bloqueIndices.cantidadElementos];
					{
						SI32 v; const SI32 countV = (mapBase->countVertexNeededV - 1);
						for(v = 0; v < countV; v++){
							SI32 iVertSup = bloqueVertices.primerElemento + (mapBase->countVertexNeededH * v);
							SI32 iVertInf = bloqueVertices.primerElemento + (mapBase->countVertexNeededH * (v + 1));
							//Repeat first vertex (to build an independent triang-strip)
							indicesGL[0] = iVertSup; indicesGL++; NBASSERT(indicesGL < indicesGLSigUlt)
							//Add row sequence
							SI32 h;
							for(h = 0 ; h < mapBase->countVertexNeededH; h++){
								indicesGL[0] = iVertSup++; indicesGL++; NBASSERT(indicesGL < indicesGLSigUlt)
								indicesGL[0] = iVertInf++; indicesGL++; NBASSERT(indicesGL < indicesGLSigUlt)
							}
							//Repeat last vertex (to build an independent triang-strip)
							indicesGL[0] = (iVertInf - 1); indicesGL++; NBASSERT(indicesGL <= indicesGLSigUlt)
						}
						NBASSERT(indicesGL == indicesGLSigUlt)
					}
					//
					STSpriteElasticoRender* propsRender	= (STSpriteElasticoRender*)&(propsRenderizado.bytesDatosModelos->elemento[posicionDatos]);
					propsRender->esMascaraAlpha			= _esMascaraAlpha;
					if(_textura != NULL){
						propsRender->idTexturaGL		= _textura->idTexturaGL; propsRenderizado.piscinaRetecionRender->agregarElemento(_textura);
					} else {
						//required on some systems where texture-0 will not be drawn
						propsRender->idTexturaGL		= NBGestorTexturas::texturaBlanca->idTexturaGL;
					}
					propsRender->bloqueIndicesGL		= bloqueIndices;
					propsRender->verticesUsadosH		= mapBase->countVertexNeededH;
					propsRender->verticesUsadosV		= mapBase->countVertexNeededV;
					propsRender->conteoIndicesSaltosV	= 0;
					//
					r = AUEscenaSpriteElastico::enviarComandosGL;
					//Dbg: print vertexs
					/*{
						NBVerticeGL* vGL = &((*propsRenderizado.verticesGL1)[bloqueVertices.primerElemento]);
						SI32 i; for(i = 0; i < bloqueVertices.cantidadElementos; i++){
							PRINTF_INFO("#%d; p(%f, %f) c(%d, %d, %d, %d).\n", (i + 1), vGL->x, vGL->y, vGL->r, vGL->g, vGL->b, vGL->a);
						}
					}*/
				}
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return r;
}

void AUEscenaSpriteElastico::enviarComandosGL(UI8* punteroDatosModelo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSpriteElastico::enviarComandosGL")
	STSpriteElasticoRender* propsRender = (STSpriteElasticoRender*)punteroDatosModelo;
	NBGestorGL::activarVerticesGL(ENVerticeGlTipo_MonoTextura); NBGESTORGL_DBG_NOMBRAR_ACTIVADOR_VERTICES_GL("AUEscenaSpriteElastico")
	//Configurar textura
	NBGestorGL::bindTexture(0, propsRender->idTexturaGL);
	//Acumular triangStrip independiente mediante indices.
	if(propsRender->esMascaraAlpha){
		NBGestorGL::blendFunc(GL_ZERO, GL_SRC_ALPHA);
		NBGestorGL::colorMask(false, false, false, true);
	}
	NBGestorGL::acumularIndicesTriangStripIndependiente(propsRender->bloqueIndicesGL.primerElemento, propsRender->bloqueIndicesGL.cantidadElementos);
	NBGESTORGL_DBG_NOMBRAR_CONVOCADOR_ACUMULAR_TRIANG_STRIPS_INDEP("AUEscenaSpriteElastico")
	if(propsRender->esMascaraAlpha){
		NBGestorGL::colorMask(true, true, true, true);
		NBGestorGL::blendFuncCargarValoresPorDefecto();
	}
	//PRINTF_INFO("Sprite color antes(%f, %f, %f, %f) despues(%f, %f, %f, %f).\n", dbgColorAntes[0], dbgColorAntes[1], dbgColorAntes[2], dbgColorAntes[3], dbgColorDesp[0], dbgColorDesp[1], dbgColorDesp[2], dbgColorDesp[3]);
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

//


bool AUEscenaSpriteElastico::agregarXmlInternoEn(AUEscenaSpriteElastico* sprite, AUCadenaLargaMutable8* guardarEn, const char* espaciosBaseIzq){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSpriteElastico::agregarXmlInternoEn")
	AUEscenaSprite::agregarXmlInternoEn(sprite, guardarEn, espaciosBaseIzq);
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return true;

}

bool AUEscenaSpriteElastico::interpretarXmlInterno(AUEscenaSpriteElastico* cargarEn, const AUDatosXML* datosXml, const sNodoXML* nodoXml){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSpriteElastico::interpretarXmlInterno")
	bool exito = AUEscenaSprite::interpretarXmlInterno(cargarEn, datosXml, nodoXml);
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

bool AUEscenaSpriteElastico::agregarBitsInternosEn(AUEscenaSpriteElastico* sprite, AUArchivo* guardarEn){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSpriteElastico::agregarBitsInternosEn")
	bool exito = AUEscenaSprite::agregarBitsInternosEn(sprite, guardarEn);
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

bool AUEscenaSpriteElastico::interpretarBitsInternos(AUEscenaSpriteElastico* cargarEn, AUArchivo* cargarDe){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSpriteElastico::interpretarBitsInternos")
	bool exito = AUEscenaSprite::interpretarBitsInternos(cargarEn, cargarDe);
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

//

AUOBJMETODOS_CLASESID_MULTICLASE(AUEscenaSpriteElastico, AUEscenaSprite)
AUOBJMETODOS_CLASESNOMBRES_MULTICLASE(AUEscenaSpriteElastico, "AUEscenaSpriteElastico", AUEscenaSprite)
AUOBJMETODOS_CLONAR_THIS(AUEscenaSpriteElastico)





