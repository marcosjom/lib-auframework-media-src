//
//  AUFuenteRaster.cpp
//  AUFramework
//
//  Created by Marcos Ortega Morales on 10/11/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkMediaStdAfx.h"
#include "AUFuenteRaster.h"
#include "nb/core/NBEncoding.h"

typedef struct STFuenteRasterVisualDataChar_ {
	STNBPoint	localPos;		//local position of the char
	UI32		points;			//total points
	UI32		lines;			//total lines
} STFuenteRasterVisualDataChar;
	
typedef struct STFuenteRasterVisualData_ {
	STNBFontLines*	font;
	float			sceneFontSz;
	float			screenFontSz;
	STNBMatrix		screenMatrix;
	STNBArray		chars;			//STFuenteRasterVisualDataChar
	STNBArray		vPoints;		//NBVerticeGL0
	STNBArray		vLines;			//NBVerticeGL0
	STNBArray		charsMetrics;	//STNBTextMetricsChar
} STFuenteRasterVisualData;

typedef struct STFuenteRasterRender_ {
	SI32		iVao;
	UI32		iPrimerPunto;
	UI32		conteoPuntos;
	UI32		iPrimerLinea;
	UI32		conteoLineas;
	NBColor8	color;
} STFuenteRasterRender;

AUFuenteRaster::AUFuenteRaster(STNBFontLines* fuenteDef, const float sceneFontSz, const float escalaEspaciado) : AUFuenteRender() {
	_fuenteDef			= fuenteDef; NBFontLines_retain(_fuenteDef);
	_isDisabled			= FALSE;
	{
		NBFontLines_getStyle(_fuenteDef, &_family, &_subFamily, &_styleMask);
		_sceneFontSz	= sceneFontSz;
		_sceneMetrics	= NBFontLines_getMetricsForSz(_fuenteDef, _sceneFontSz);
	}
	_escalaEspaciado	= escalaEspaciado;
	//
	/*_vertsPuntos = new(ENMemoriaTipo_Temporal) AUArregloNativoMutableP<NBVerticeGL0>();
	_vertsLineas = new(ENMemoriaTipo_Temporal) AUArregloNativoMutableP<NBVerticeGL0>();
	buffPuntos = new(ENMemoriaTipo_Temporal) AUArregloNativoMutableP<STFuenteLineasVertice>();
	buffLineas = new(ENMemoriaTipo_Temporal) AUArregloNativoMutableP<STFuenteLineasVertice>();*/
}

AUFuenteRaster::~AUFuenteRaster(){
	if(_fuenteDef != NULL) NBFontLines_release(_fuenteDef); _fuenteDef = NULL;
	//
	/*_vertsPuntos->liberar(NB_RETENEDOR_THIS);
	_vertsLineas->liberar(NB_RETENEDOR_THIS);
	buffPuntos->liberar(NB_RETENEDOR_THIS);
	buffLineas->liberar(NB_RETENEDOR_THIS);*/
}

//

BOOL AUFuenteRaster::isDisabled(){
	return _isDisabled;
}

void AUFuenteRaster::setDisabled(const BOOL isDisabled){
	_isDisabled = isDisabled;
}

//

STNBFontLines* AUFuenteRaster::fuenteDef(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUFuenteRaster::fuenteDef")
	AU_GESTOR_PILA_LLAMADAS_POP
	return _fuenteDef;
}

const char* AUFuenteRaster::familia(){
	return _family;
}

const char* AUFuenteRaster::subfamilia(){
	return _subFamily;
}

bool AUFuenteRaster::esNegrilla(){
	return ((_styleMask & ENNBFontStyleBit_Bold) != 0);
}

bool AUFuenteRaster::esCursiva(){
	return ((_styleMask & ENNBFontStyleBit_Italic) != 0);
}

float AUFuenteRaster::tamanoEscena(){
	return _sceneFontSz;
}

float AUFuenteRaster::ascendenteEscena(){
	return _sceneMetrics.ascender * _escalaEspaciado;
}

float AUFuenteRaster::descendenteEscena(){
	return _sceneMetrics.descender * _escalaEspaciado;
}

float AUFuenteRaster::altoGuiaLineaEscena(){
	return _sceneMetrics.height * _escalaEspaciado;
}

float AUFuenteRaster::altoParaSiguienteLineaEscena(){
	return (_sceneMetrics.height - (_sceneMetrics.ascender - _sceneMetrics.descender)) * _escalaEspaciado;
}

//

void AUFuenteRaster::updateFontSize(const float sceneFontSz){
	_sceneFontSz	= sceneFontSz;
	_sceneMetrics	= NBFontLines_getMetricsForSz(_fuenteDef, _sceneFontSz);
}

//

STNBFontShapeMetrics AUFuenteRaster::propiedadesParaCaracter(const float tamFuentePx, const UI32 unicode32){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUFuenteRaster::propiedadesParaCaracter")
	NBASSERT(_fuenteDef != NULL)
	STNBFontShapeMetrics r = NBFontLines_getShapeMetricsForSz(_fuenteDef, NBFontLines_getShapeId(_fuenteDef, unicode32), tamFuentePx);
	r.hAdvance *= _escalaEspaciado;
	r.vAdvance *= _escalaEspaciado;
	AU_GESTOR_PILA_LLAMADAS_POP
	return r;
}

float AUFuenteRaster::propiedadesKerning(const float tamFuentePx, const UI32 unicode32, const UI32 unicode32Anterior){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUFuenteRaster::propiedadesKerning")
	const float kern = 0; //(float)_fuenteDef->propiedadesKerning(tamFuentePx, unicode32, unicode32Anterior) * _escalaEspaciado;
	AU_GESTOR_PILA_LLAMADAS_POP
	return kern;
}

STNBFontShapeMetrics AUFuenteRaster::propiedadesParaCaracter(void* objBiblioteca, const float tamFuentePx, const UI32 unicode32){
	NBASSERT(((AUFuenteRaster*)objBiblioteca)->esClase("AUFuenteRaster") && ((AUFuenteRaster*)objBiblioteca)->esClase(AUFuenteRaster::idTipoClase));
	return ((AUFuenteRaster*)objBiblioteca)->propiedadesParaCaracter(tamFuentePx, unicode32);
}

float AUFuenteRaster::propiedadesKerning(void* objBiblioteca, const float tamFuentePx, const UI32 unicode32, const UI32 unicode32Anterior){
	NBASSERT(objBiblioteca != NULL)
	NBASSERT(((AUFuenteRaster*)objBiblioteca)->esClase("AUFuenteRaster") && ((AUFuenteRaster*)objBiblioteca)->esClase(AUFuenteRaster::idTipoClase));
	return ((AUFuenteRaster*)objBiblioteca)->propiedadesKerning(tamFuentePx, unicode32, unicode32Anterior);
}

//Set fontProperties to metricsBuilder

STNBFontMetricsIRef AUFuenteRaster::getFontMetricsIRef(const float defFontSz){
	STNBFontMetricsIRef r;
	NBMemory_setZeroSt(r, STNBFontMetricsIRef);
	r.itf.retain					= retain_;
	r.itf.release					= release_;
	r.itf.getNativeSz				= NULL;
	r.itf.getShapeId				= getShapeId_;
	r.itf.getFontMetrics			= getFontMetrics_;
	r.itf.getFontMetricsForSz		= getFontMetricsForSz_;
	r.itf.getFontShapeMetrics		= getFontShapeMetrics_;
	r.itf.getFontShapeMetricsForSz	= getFontShapeMetricsForSz_;
	r.itfParam						= this;
	r.fontSz						= (defFontSz <= 0 ? _sceneFontSz : defFontSz);
	return r;
}

void AUFuenteRaster::configureMetricsBuilder(STNBTextMetricsBuilder* mBuilder){
	STNBFontMetricsIRef itfRef = this->getFontMetricsIRef(0);
	NBTextMetricsBuilder_setFont(mBuilder, itfRef);
	NBTextMetricsBuilder_setFontSz(mBuilder, _sceneFontSz);
}

//Metrics to VisualModel

AUFuenteVisualModel* AUFuenteRaster::newVisualModelForChars(const STNBTextMetrics* textMetrics, const UI32 firstChar, const UI32 countChars, STNBArray* dstBoxes, STNBFontRenderBox* dstBoxesCur, const UI32 maxBoxesCharsCount, AUArreglo* visualFilters){
	STFuenteRasterVisualData* d = NBMemory_allocType(STFuenteRasterVisualData);
	{
		const STNBTextMetricsChar* char0 = NBArray_itmPtrAtIndex(&textMetrics->chars, STNBTextMetricsChar, firstChar);
		d->font			= this->_fuenteDef; NBFontLines_retain(d->font);
		d->sceneFontSz	= this->_sceneFontSz;
		d->screenFontSz	= 0.0f;
		NBMatrix_setIdentity(&d->screenMatrix);
		NBArray_init(&d->chars, sizeof(STFuenteRasterVisualDataChar), NULL);
		NBArray_init(&d->vPoints, sizeof(NBVerticeGL0), NULL);
		NBArray_init(&d->vLines, sizeof(NBVerticeGL0), NULL);
		NBArray_init(&d->charsMetrics, sizeof(STNBTextMetricsChar), NULL);
		NBArray_addItems(&d->charsMetrics, char0, sizeof(STNBTextMetricsChar), countChars);
		//Apply filters
		{
			//ToDo: apply 'filters', 'filtersCount', per char (calculating the line).
		}
	}
	return new AUFuenteVisualModel(this, d, AUFuenteRaster::produceRenderModel, AUFuenteRaster::destroyVisualModelData, visualFilters);
}

void AUFuenteRaster::destroyVisualModelData(void* data){
	if(data != NULL){
		STFuenteRasterVisualData* d = (STFuenteRasterVisualData*)data;
		if(d->font != NULL) NBFontLines_release(d->font); d->font = NULL;
		d->sceneFontSz	= 0.0f;
		d->screenFontSz	= 0.0f;
		{
			NBArray_empty(&d->chars);
			NBArray_release(&d->chars);
		}
		{
			NBArray_empty(&d->vPoints);
			NBArray_release(&d->vPoints);
		}
		{
			NBArray_empty(&d->vLines);
			NBArray_release(&d->vLines);
		}
		{
			NBArray_empty(&d->charsMetrics);
			NBArray_release(&d->charsMetrics);
		}
		NBMemory_free(d);
	}
}

//VisualModel to RenderModel

#define AU_FUENTE_RASTER_RENDER_MATRIX_MIN_DELTA 0.001f

PTRfunCmdsGL AUFuenteRaster::produceRenderModel(const void* data, const NBPropRenderizado &propsRenderizado, const NBMatriz matrizEscena, const NBColor colorMultiplicado, const STNBRangeI range, STBloqueGL* dstVertexBlock, UI8* dstVertexBlockType, AUArreglo* visualFilters){
	PTRfunCmdsGL r = NULL;
	if(data != NULL){
		STFuenteRasterVisualData* d = (STFuenteRasterVisualData*)data;
		//Determine the sceen fontSize
		const NBRectangulo puertoVision	= propsRenderizado.puertoDeVision;
		const NBCajaAABB cajaProyeccion	= propsRenderizado.cajaProyeccion;
		const NBTamano unidsGLPorPx((cajaProyeccion.xMax - cajaProyeccion.xMin) / puertoVision.ancho, (cajaProyeccion.yMax - cajaProyeccion.yMin) / puertoVision.alto);
		const float screenFntSz	= (d->sceneFontSz / unidsGLPorPx.alto);
		const float delta		= screenFntSz - d->screenFontSz;
		STNBMatrix matrixDelta; //only scale and rotation
		NBMATRIX_E00(matrixDelta)	= NBMATRIX_E00(d->screenMatrix) - NBMATRIZ_ELEM00(matrizEscena);
		NBMATRIX_E01(matrixDelta)	= NBMATRIX_E01(d->screenMatrix) - NBMATRIZ_ELEM01(matrizEscena);
		NBMATRIX_E10(matrixDelta)	= NBMATRIX_E10(d->screenMatrix) - NBMATRIZ_ELEM10(matrizEscena);
		NBMATRIX_E11(matrixDelta)	= NBMATRIX_E11(d->screenMatrix) - NBMATRIZ_ELEM11(matrizEscena);
		const bool matrixChangd		= (NBMATRIX_E00(matrixDelta) < -AU_FUENTE_RASTER_RENDER_MATRIX_MIN_DELTA || NBMATRIX_E00(matrixDelta) > AU_FUENTE_RASTER_RENDER_MATRIX_MIN_DELTA || NBMATRIX_E01(matrixDelta) < -AU_FUENTE_RASTER_RENDER_MATRIX_MIN_DELTA || NBMATRIX_E01(matrixDelta) > AU_FUENTE_RASTER_RENDER_MATRIX_MIN_DELTA || NBMATRIX_E10(matrixDelta) < -AU_FUENTE_RASTER_RENDER_MATRIX_MIN_DELTA || NBMATRIX_E10(matrixDelta) > AU_FUENTE_RASTER_RENDER_MATRIX_MIN_DELTA || NBMATRIX_E11(matrixDelta) < -AU_FUENTE_RASTER_RENDER_MATRIX_MIN_DELTA || NBMATRIX_E11(matrixDelta) > AU_FUENTE_RASTER_RENDER_MATRIX_MIN_DELTA);
		if(delta < -0.10f || delta > 0.10f || matrixChangd){
			//Update lines model
			NBArray_empty(&d->chars);
			NBArray_empty(&d->vPoints);
			NBArray_empty(&d->vLines);
			NBMatrix_setIdentity(&d->screenMatrix);
			NBMATRIX_E00(d->screenMatrix) = NBMATRIZ_ELEM00(matrizEscena);
			NBMATRIX_E01(d->screenMatrix) = NBMATRIZ_ELEM01(matrizEscena);
			NBMATRIX_E10(d->screenMatrix) = NBMATRIZ_ELEM10(matrizEscena);
			NBMATRIX_E11(d->screenMatrix) = NBMATRIZ_ELEM11(matrizEscena);
			{
				SI32 iStart, iEnfAfter;
				if(range.size < 0){
					iStart		= 0;
					iEnfAfter	= d->charsMetrics.use;
				} else {
					iStart		= range.start;
					iEnfAfter	= range.start + range.size;
					if(iStart < 0) iStart = 0;
					if(iStart > d->charsMetrics.use) iStart = d->charsMetrics.use;
					if(iEnfAfter > d->charsMetrics.use) iEnfAfter = d->charsMetrics.use;
				}
				{
					STNBFontLinesData data;
					NBMemory_setZeroSt(data, STNBFontLinesData);
					NBArray_init(&data.vPoints, sizeof(STNBFontLineVertex), NULL);
					NBArray_init(&data.vLines, sizeof(STNBFontLineVertex), NULL);
					UI32 i; for(i = iStart; i < iEnfAfter; i++){
						const STNBTextMetricsChar* c = NBArray_itmPtrAtIndex(&d->charsMetrics, STNBTextMetricsChar, i);
						if(!c->isControl){
							NBArray_empty(&data.vPoints);
							NBArray_empty(&data.vLines);
							if(!NBFontLines_getLinesShapeCalculated(d->font, screenFntSz, d->screenMatrix, c->shapeId, &data)){
								NBASSERT(FALSE)
							} else {
								//Append char-def
								{
									STFuenteRasterVisualDataChar cc;
									cc.localPos.x	= c->pos.x;
									cc.localPos.y	= -c->pos.y;
									cc.points		= data.vPoints.use;
									cc.lines		= (data.vLines.use / 2); NBASSERT((data.vLines.use % 2) == 0)
									NBArray_addValue(&d->chars, cc);
								}
								//Append points
								{
									UI32 i; const UI32 count = data.vPoints.use;
									for(i = 0; i < count; i++){
										const STNBFontLineVertex* v = NBArray_itmPtrAtIndex(&data.vPoints, STNBFontLineVertex, i);
										NBVerticeGL0 gv;
										gv.x = v->pos;
										gv.y = v->line; //inverted-y
										gv.r = c->color.r;
										gv.g = c->color.g;
										gv.b = c->color.b;
										gv.a = (c->color.a * v->intensity) / 255;
										NBArray_addValue(&d->vPoints, gv);
									}
								}
								//Append lines
								{
									NBVerticeGL0 gv0, gv1;
									const STNBFontLineVertex* v0;
									const STNBFontLineVertex* v1;
									UI32 i; const UI32 count = data.vLines.use;
									for(i = 0; i < count; i += 2){
										//Load vertexs pair
										v0 = NBArray_itmPtrAtIndex(&data.vLines, STNBFontLineVertex, i);
										v1 = v0 + 1;
										NBASSERT(v0->line == v1->line && v0->intensity == v1->intensity) //Must be a pair
										//Set glVertexs
										gv0.x = v0->pos;
										gv0.y = v0->line; //inverted-y
										gv0.r = c->color.r;
										gv0.g = c->color.g;
										gv0.b = c->color.b;
										gv0.a = (c->color.a * v0->intensity) / 255;
										//
										gv1.x = v1->pos + 1.0f; //+1, line definition does not includes the final
										gv1.y = v1->line; //inverted-y
										gv1.r = c->color.r;
										gv1.g = c->color.g;
										gv1.b = c->color.b;
										gv1.a = (c->color.a * v1->intensity) / 255;
										//
										NBArray_addValue(&d->vLines, gv0);
										NBArray_addValue(&d->vLines, gv1);
									}
								}
							}
						}
					}
					NBArray_release(&data.vPoints);
					NBArray_release(&data.vLines);
				}
			}
			d->screenFontSz = screenFntSz;
		}
		//Generate render models
		{
			NBASSERT((d->vLines.use % 2) == 0) //must be pairs
			if(d->vPoints.use > 0 || d->vLines.use > 0){
				const UI8 cr = 255.f * colorMultiplicado.r;
				const UI8 cg = 255.f * colorMultiplicado.g;
				const UI8 cb = 255.f * colorMultiplicado.b;
				const UI8 ca = 255.f * colorMultiplicado.a;
				STBloqueGL glBlock; glBlock.cantidadElementos = 0;
				glBlock.cantidadElementos	= d->vPoints.use + d->vLines.use;
				glBlock.primerElemento		= (*propsRenderizado.funcReservarVertices)(ENVerticeGlTipo_SinTextura, glBlock.cantidadElementos);
				NBVerticeGL0* glP			= &((*propsRenderizado.verticesGL0)[glBlock.primerElemento]);
				NBVerticeGL0* glL			= &((*propsRenderizado.verticesGL0)[glBlock.primerElemento + d->vPoints.use]);
				//Copy chars
				NBASSERT(d->chars.use > 0)
				{
					const NBVerticeGL0* vP = NBArray_dataPtr(&d->vPoints, NBVerticeGL0);
					const NBVerticeGL0* vL = NBArray_dataPtr(&d->vLines, NBVerticeGL0);
					UI32 i; for(i = 0; i < d->chars.use; i++){
						const STFuenteRasterVisualDataChar* c = NBArray_itmPtrAtIndex(&d->chars, STFuenteRasterVisualDataChar, i);
						//Calculate char's scene pos (centered to a screen pixel)
						NBPunto scenePos; NBMATRIZ_MULTIPLICAR_PUNTO(scenePos, matrizEscena, c->localPos.x, c->localPos.y);
						NBPunto pixRel;
						pixRel.x = (scenePos.x - cajaProyeccion.xMin) / unidsGLPorPx.ancho;
						pixRel.y = (scenePos.y - cajaProyeccion.yMin) / unidsGLPorPx.alto;
						pixRel.x -= (SI32)(pixRel.x); //leave only decimal part
						pixRel.y -= (SI32)(pixRel.y); //leave only decimal part
						NBASSERT(pixRel.x >= -1.0f && pixRel.x <= 1.0f)
						//Move to the cernter of a pixel
						{
							NBPunto pixAdd;
							if(pixRel.x < 0){
								pixAdd.x = -0.5f - pixRel.x;
							} else {
								pixAdd.x = pixRel.x - 0.5f;
							}
							if(pixRel.y < 0){
								pixAdd.y = -0.5f - pixRel.y;
							} else {
								pixAdd.y = pixRel.y - 0.5f;
							}
							//PRINTF_INFO("ScenePos(%.2f, %.2f) screenPixPos(%.2f, %.2f) pixAdd(%.2f, %.2f) = finalScreenPixPos(%.2f, %.2f).\n", scenePos.x, scenePos.y, pixRel.x, pixRel.y, pixAdd.x, pixAdd.y, ((scenePos.x + (pixAdd.x * unidsGLPorPx.ancho)) - cajaProyeccion.xMin) / unidsGLPorPx.ancho, ((scenePos.y - (pixAdd.y * unidsGLPorPx.alto)) - cajaProyeccion.yMin) / unidsGLPorPx.alto);
							scenePos.x += pixAdd.x * unidsGLPorPx.ancho;
							scenePos.y -= pixAdd.y * unidsGLPorPx.alto;
						}
						//Copy local-screen-vertex to scene-vertexs
						if(c->points > 0){
							const NBVerticeGL0* vAfterLast = vP + c->points;
							while(vP < vAfterLast){
								glP->r = (cr * vP->r) / 255;
								glP->g = (cg * vP->g) / 255;
								glP->b = (cb * vP->b) / 255;
								glP->a = (ca * vP->a) / 255;
								glP->x = (scenePos.x + vP->x);
								glP->y = (scenePos.y + vP->y);
								//Next
								glP++;
								vP++;
							}
						}
						//Copy local-screen-vertex to scene-vertexs
						if(c->lines > 0){
							const NBVerticeGL0* vAfterLast = vL + (c->lines * 2);
							while(vL < vAfterLast){
								glL->r = (cr * vL->r) / 255;
								glL->g = (cg * vL->g) / 255;
								glL->b = (cb * vL->b) / 255;
								glL->a = (ca * vL->a) / 255;
								glL->x = (scenePos.x + vL->x);
								glL->y = (scenePos.y + vL->y);
								//Next
								glL++;
								vL++;
							}
						}
					}
					NBASSERT(glP == &((*propsRenderizado.verticesGL0)[glBlock.primerElemento + d->vPoints.use]))
					NBASSERT(glL == &((*propsRenderizado.verticesGL0)[glBlock.primerElemento + d->vPoints.use + d->vLines.use]))
					NBASSERT(glL == &((*propsRenderizado.verticesGL0)[glBlock.primerElemento + glBlock.cantidadElementos]))
				}
				//Datos render (grupo)
				{
					const SI32 indiceDatosGrp		= propsRenderizado.bytesDatosModelos->conteo; propsRenderizado.bytesDatosModelos->reservarRegistrosAlFinal(sizeof(STFuenteRasterRender));
					STFuenteRasterRender* datosGrp	= (STFuenteRasterRender*)propsRenderizado.bytesDatosModelos->elemPtr(indiceDatosGrp);
					datosGrp->iVao					= ENVerticeGlTipo_SinTextura;
					datosGrp->iPrimerPunto			= glBlock.primerElemento;
					datosGrp->conteoPuntos			= d->vPoints.use;
					datosGrp->iPrimerLinea			= glBlock.primerElemento + d->vPoints.use;
					datosGrp->conteoLineas			= (d->vLines.use / 2);
					datosGrp->color.r				= colorMultiplicado.r * 255.0f;
					datosGrp->color.g				= colorMultiplicado.g * 255.0f;
					datosGrp->color.b				= colorMultiplicado.b * 255.0f;
					datosGrp->color.a				= colorMultiplicado.a * 255.0f;
					if(dstVertexBlock != NULL) *dstVertexBlock = glBlock;
					if(dstVertexBlockType != NULL) *dstVertexBlockType = 0;
				}
				r = AUFuenteRaster::consumeRenderModel;
			}
		}
	}
	return r;
}

void AUFuenteRaster::consumeRenderModel(BYTE* punteroDatosModelo){
	//const NBColor colorAnt = NBGestorGL::colorActivo(); //TMP
	NBGestorGL::enable(GL_LIGHTING);					//TMP
	NBGestorGL::enable(GL_COLOR_MATERIAL);				//TMP
	{
		STFuenteRasterRender* datGrp = (STFuenteRasterRender*)punteroDatosModelo;
		NBGestorGL::activarVerticesGL(datGrp->iVao); NBGESTORGL_DBG_NOMBRAR_ACTIVADOR_VERTICES_GL("AUFuenteRender")
		NBGestorGL::lightModelAmbient((float)datGrp->color.r / 255.0f, (float)datGrp->color.g / 255.0f, (float)datGrp->color.b / 255.0f, (float)datGrp->color.a / 255.0f); //TMP
		//NBGestorGL::color4f((float)datGrp->color.r / 255.0f, (float)datGrp->color.g / 255.0f, (float)datGrp->color.b / 255.0f, (float)datGrp->color.a / 255.0f); //TMP
		//glDisableClientState(GL_COLOR_ARRAY); //TEMP
		//PRINTF_INFO("Color(%d, %d, %d, %d)\n", datGrp->color.r, datGrp->color.g, datGrp->color.b, datGrp->color.a);
		//Renderizar puntos y lineas
		{
			if(datGrp->conteoPuntos > 0){
				NBGestorGL::drawArrays(GL_POINTS, datGrp->iPrimerPunto, datGrp->conteoPuntos);
			}
			if(datGrp->conteoLineas > 0){
				NBGestorGL::drawArrays(GL_LINES, datGrp->iPrimerLinea, datGrp->conteoLineas * 2);
			}
		}
		//glEnableClientState(GL_COLOR_ARRAY); //TMP
	}
	//NBGestorGL::color4f(colorAnt.r, colorAnt.g, colorAnt.b, colorAnt.a); //TMP
	NBGestorGL::lightModelAmbient(1.0f, 1.0f, 1.0f, 1.0f); //TMP
	NBGestorGL::disable(GL_LIGHTING);					//TMP
	NBGestorGL::disable(GL_COLOR_MATERIAL);				//TMP
}

//Metrics Itf
void AUFuenteRaster::retain_(void* obj){
	NBASSERT(((AUFuenteRaster*)obj)->esClase(AUFuenteRaster::idTipoClase))
	((AUFuenteRaster*)obj)->retener(NB_RETENEDOR_NULL);
}
void AUFuenteRaster::release_(void* obj){
	NBASSERT(((AUFuenteRaster*)obj)->esClase(AUFuenteRaster::idTipoClase))
	((AUFuenteRaster*)obj)->liberar(NB_RETENEDOR_NULL);
}
UI32 AUFuenteRaster::getShapeId_(void* obj, const UI32 unicode){
	NBASSERT(((AUFuenteRaster*)obj)->esClase(AUFuenteRaster::idTipoClase))
	return NBFontLines_getShapeId(((AUFuenteRaster*)obj)->_fuenteDef, unicode);
}

STNBFontMetrics AUFuenteRaster::getFontMetrics_(void* obj){
	NBASSERT(((AUFuenteRaster*)obj)->esClase(AUFuenteRaster::idTipoClase))
	return NBFontLines_getMetrics(((AUFuenteRaster*)obj)->_fuenteDef);
}

STNBFontMetrics AUFuenteRaster::getFontMetricsForSz_(void* obj, const float fontSz){
	NBASSERT(((AUFuenteRaster*)obj)->esClase(AUFuenteRaster::idTipoClase))
	return NBFontLines_getMetricsForSz(((AUFuenteRaster*)obj)->_fuenteDef, fontSz);
}

STNBFontShapeMetrics AUFuenteRaster::getFontShapeMetrics_(void* obj, const UI32 shapeId){
	NBASSERT(((AUFuenteRaster*)obj)->esClase(AUFuenteRaster::idTipoClase))
	return NBFontLines_getShapeMetrics(((AUFuenteRaster*)obj)->_fuenteDef, shapeId);
}

STNBFontShapeMetrics AUFuenteRaster::getFontShapeMetricsForSz_(void* obj, const UI32 shapeId, const float fontSz){
	NBASSERT(((AUFuenteRaster*)obj)->esClase(AUFuenteRaster::idTipoClase))
	return NBFontLines_getShapeMetricsForSz(((AUFuenteRaster*)obj)->_fuenteDef, shapeId, fontSz);
}

//

AUOBJMETODOS_CLASESID_MULTICLASE(AUFuenteRaster, AUFuenteRender)
AUOBJMETODOS_CLASESNOMBRES_MULTICLASE(AUFuenteRaster, "AUFuenteRaster", AUFuenteRender)
AUOBJMETODOS_CLONAR_NULL(AUFuenteRaster)




