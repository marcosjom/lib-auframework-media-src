
#include "AUFrameworkMediaStdAfx.h"
#include "NBGestorTexturas.h"
#include "AUArchivoEnBufferExterno.h"
#include "AUMapaBitsProps.h"
#include "nb/crypto/NBSha1.h"
#include "NBGestorFuentes.h"

#define NBGESTORTEXTURAS_CARGA_MAX_BYTES_POR_BUFFER			(512 * 1024) //(256 * 1024)
#define NBGESTORTEXTURAS_PIX_SEPARACION_ENTRE_TEXTURAS		4
//Componentes
STTexturaResProps NBGestorTexturas::_propsByResolution[ENTexturaResolucion_Conteo];
//
UI8 NBGestorTexturas::_indiceAmbitoActivo = 0;
STGestorTexAmbito NBGestorTexturas::_ambitosAtlases[GESTOR_TEXTURAS_MAX_AMBITOS_TEXTURAS];
AUArregloNativoMutableP<STGestorTexFuente>* NBGestorTexturas::_fuentesTexturas = NULL;
AUArregloNativoMutableP<STGestorTexFuenteR>* NBGestorTexturas::_fuentesRaster = NULL;
AUArregloNativoOrdenadoMutableP<STGestorTexFuenteVAO>* NBGestorTexturas::_fuentesRasterVAOs = NULL;
//
bool NBGestorTexturas::_gestorInicializado = false;
bool NBGestorTexturas::_bloqueado = false;
UI32 NBGestorTexturas::_texsChangesCount = 0;
//
AUArregloNativoMutableP<ENGestorTexturaModo>* NBGestorTexturas::_pilaModosCargaTexturas = NULL;
NBHILO_MUTEX_CLASE NBGestorTexturas::_pilaModosCargaTexturasMutex;
AUTextura* NBGestorTexturas::texturaBlanca = NULL; //required on some systems where texture-0 will not be drawn

//CARGA DE TEXTURAS EN SEGUNDO PLANO
ENTexturaModoHilo NBGestorTexturas::_texLoadThreadMode	= ENTexturaModoHilo_MonoHilo;
AUHilo* NBGestorTexturas::_texLoadThread		= NULL;
bool NBGestorTexturas::_texLoadThreadWorking	= false;
bool NBGestorTexturas::_texLoadThreadStopFlag	= false;
NBHILO_MUTEX_CLASE NBGestorTexturas::_texLoadingRefsMutex;
AUArregloNativoMutableP<STGestorTexTexturaRef*>* NBGestorTexturas::_texLoadingRefs = NULL;
//Unorganized textures (preloaded textures)
NBHILO_MUTEX_CLASE NBGestorTexturas::_texUnorganizedRefsMutex;
AUArregloNativoMutableP<STGestorTexTexturaRef>* NBGestorTexturas::_texUnorganizedRefs = NULL;

bool NBGestorTexturas::inicializar(const ENTexturaModoHilo modoHilos, UI16 tamanosTexturasPrincipales, UI16 tamanosTexturasSecundarias){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTexturas::inicializar")
	PRINTF_INFO("Inicializando NBGestorTexturas.\n");
	_gestorInicializado			= false;
	//Props per resolution
	{
		SI32 i;
		for(i = 0 ; i < ENTexturaResolucion_Conteo; i++){
			STTexturaResProps* props = &_propsByResolution[i];
			props->setStatus		= ENTexturaPropsSetStatus_Unset;
			props->pathPrefix		= new(ENMemoriaTipo_General) AUCadenaMutable8();	NB_DEFINE_NOMBRE_PUNTERO(props->pathPrefix, "NBGestorTexturas::pathPrefix");
			props->scaleBase2		= 8;
			props->scaleToHD		= 1.0f;
		}
	}
	_fuentesTexturas			= new(ENMemoriaTipo_General) AUArregloNativoMutableP<STGestorTexFuente>();			NB_DEFINE_NOMBRE_PUNTERO(_fuentesTexturas, "NBGestorTexturas::_fuentesTexturas");
	_fuentesRaster				= new(ENMemoriaTipo_General) AUArregloNativoMutableP<STGestorTexFuenteR>();			NB_DEFINE_NOMBRE_PUNTERO(_fuentesRaster, "NBGestorTexturas::_fuentesRaster");
	_fuentesRasterVAOs			= new(ENMemoriaTipo_General) AUArregloNativoOrdenadoMutableP<STGestorTexFuenteVAO>(); NB_DEFINE_NOMBRE_PUNTERO(_fuentesRasterVAOs, "NBGestorTexturas::_fuentesRasterVAOs");
	//Inicializar ambitos de atlases
	_indiceAmbitoActivo			= 0; //El primer ambito es por defecto
	SI32 iAmb;
	for(iAmb=0; iAmb<GESTOR_TEXTURAS_MAX_AMBITOS_TEXTURAS; iAmb++){
		STGestorTexAmbito* ambTex = &_ambitosAtlases[iAmb];
		if(iAmb == _indiceAmbitoActivo){
			ambTex->ambitoReservado				= true;
			ambTex->tamanosTexturasPrincipales	= tamanosTexturasPrincipales;
			ambTex->tamanosTexturasSecundarias	= tamanosTexturasSecundarias;
		} else {
			ambTex->ambitoReservado				= false;
			ambTex->tamanosTexturasPrincipales	= 1024;
			ambTex->tamanosTexturasSecundarias	= 512;
		}
		ambTex->atlasesTexturas					= new(ENMemoriaTipo_General) AUArregloNativoMutableP<STGestorTexAtlas*>();		NB_DEFINE_NOMBRE_PUNTERO(ambTex->atlasesTexturas, "NBGestorTexturas::_atlasesTexturas");
		//NBHILO_MUTEX_INICIALIZAR(&ambTex->atlasesTexturasMutex)
	}
	//Variables
	_bloqueado				= false;
	_texsChangesCount		= 0;
	_pilaModosCargaTexturas = new(ENMemoriaTipo_General) AUArregloNativoMutableP<ENGestorTexturaModo>();
	_pilaModosCargaTexturas->agregarElemento(ENGestorTexturaModo_cargaInmediata);
	NBHILO_MUTEX_INICIALIZAR(&_pilaModosCargaTexturasMutex)
	//---------------------------------------
	//---- Textura por defecto (blanca, para evitar bind al idTextura '0')
	//---- Required on some systems where texture-0 will not be drawn.
	//---------------------------------------
	{
		AUMapaBitsLienzo* mapaBitsBlancoRGBA = new(ENMemoriaTipo_Temporal) AUMapaBitsLienzo(64, 64); NB_DEFINE_NOMBRE_PUNTERO(mapaBitsBlancoRGBA, "NBGestorTexturas::mapaBitsBlancoRGBA");
		mapaBitsBlancoRGBA->formatearLienzo(255, 255, 255, 255);
		AUMapaBits* mapaBitsBlancoGris = new(ENMemoriaTipo_Temporal) AUMapaBits(mapaBitsBlancoRGBA, COLOR_GRIS8); NB_DEFINE_NOMBRE_PUNTERO(mapaBitsBlancoGris, "NBGestorTexturas::mapaBitsBlancoGris");
		STNBSha1Hash firmaSHA1Vacia; SHA1_HASH_INIT(firmaSHA1Vacia);
		texturaBlanca = NBGestorTexturas::privTexturaDesdeMapaBits(_indiceAmbitoActivo, mapaBitsBlancoGris, "__nbWhite__", ENTexturaTipoAlmacenamientoGL_AtlasUnico, ENTExturaTipoUsoGL_Lectura, ENTexturaModoPintado_Patron_Preciso, ENTexturaMipMap_Habilitado, ENTexturaOrdenV_HaciaAbajo, NULL, 1, 1.0f)->textura; NBASSERT(texturaBlanca != NULL) NB_DEFINE_NOMBRE_PUNTERO(texturaBlanca, "NBGestorTexturas::texturaBlanca")
		texturaBlanca->retener(NB_RETENEDOR_NULL); //Necesario retener, para evitar que se elimine su atlas
		mapaBitsBlancoGris->liberar(NB_RETENEDOR_NULL); 
		mapaBitsBlancoRGBA->liberar(NB_RETENEDOR_NULL);
	}
	//
	NBHILO_MUTEX_INICIALIZAR(&_texLoadingRefsMutex)
	_texLoadingRefs	= new(ENMemoriaTipo_General) AUArregloNativoMutableP<STGestorTexTexturaRef*>();
	//Unorganized textures (preloaded textures)
	NBHILO_MUTEX_INICIALIZAR(&_texUnorganizedRefsMutex);
	_texUnorganizedRefs = new(ENMemoriaTipo_General) AUArregloNativoMutableP<STGestorTexTexturaRef>();
	//-------------------------------
	//-- Hilo de carga de texturas en segundo plano
	//-------------------------------
	_texLoadThreadMode			= modoHilos;
	_texLoadThread				= NULL;
	_texLoadThreadStopFlag	= false;
	_texLoadThreadWorking		= false;
	NBGestorTexturas::trabajadorSegundoPlanoIniciar();
	//
	_gestorInicializado			= true;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return true;
}

void NBGestorTexturas::finalizar(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTexturas::finalizar")
	//-------------------------------
	//-- Hilo de carga de texturas en segundo plano
	//-------------------------------
	NBGestorTexturas::trabajadorSegundoPlanoDetenerYEsperar();
	if(texturaBlanca != NULL){
		texturaBlanca->liberar(NB_RETENEDOR_NULL);
		texturaBlanca = NULL;
	}
	{
		SI32 i;
		for(i = 0 ; i < ENTexturaResolucion_Conteo; i++){
			STTexturaResProps* props = &_propsByResolution[i];
			props->setStatus	= ENTexturaPropsSetStatus_Unset;
			props->scaleToHD	= 1.0f;
			props->scaleBase2	= 8;
			if(props->pathPrefix != NULL) props->pathPrefix->liberar(NB_RETENEDOR_NULL); props->pathPrefix = NULL;
		}
	}
	if(_fuentesRaster != NULL){
		_fuentesRaster->liberar(NB_RETENEDOR_NULL);
		_fuentesRaster = NULL;
	}
	//PENDIENTE: eleiminar las texturas en el OpenGL
	//
	//
	//Loading data textures (loading in background)
	if(_texLoadingRefs != NULL){
		NBHILO_MUTEX_ACTIVAR(&_texLoadingRefsMutex);
		SI32 i; const SI32 count = _texLoadingRefs->conteo;
		for(i = 0; i < count; i++){
			STGestorTexTexturaRef* texRef = _texLoadingRefs->elem(i);
			NBASSERT(texRef->atlas != NULL)
			NBASSERT(texRef->texture != NULL)
			NBGestorTexturas::privSTGestorTexAtlasRelease(texRef->atlas);
			NBGestorTexturas::privSTGestorTexTexturaRelease(texRef->texture);
			texRef->atlas = NULL;
			texRef->texture = NULL;
			NBGestorMemoria::liberarMemoria(texRef);
		}
		_texLoadingRefs->vaciar();
		_texLoadingRefs->liberar(NB_RETENEDOR_NULL);
		_texLoadingRefs = NULL;
		NBHILO_MUTEX_DESACTIVAR(&_texLoadingRefsMutex);
		NBHILO_MUTEX_FINALIZAR(&_texLoadingRefsMutex);
	}
	//Unorganized textures (preloaded textures)
	if(_texUnorganizedRefs != NULL){
		NBHILO_MUTEX_ACTIVAR(&_texUnorganizedRefsMutex);
		SI32 i; const SI32 count = _texUnorganizedRefs->conteo;
		for(i = 0; i < count; i++){
			STGestorTexTexturaRef* texRef = _texUnorganizedRefs->elemPtr(i);
			NBASSERT(texRef->atlas != NULL)
			NBASSERT(texRef->texture != NULL)
			NBGestorTexturas::privSTGestorTexAtlasRelease(texRef->atlas);
			NBGestorTexturas::privSTGestorTexTexturaRelease(texRef->texture);
			texRef->atlas = NULL;
			texRef->texture = NULL;
		}
		_texUnorganizedRefs->vaciar();
		_texUnorganizedRefs->liberar(NB_RETENEDOR_NULL);
		_texUnorganizedRefs = NULL;
		NBHILO_MUTEX_DESACTIVAR(&_texUnorganizedRefsMutex);
		NBHILO_MUTEX_FINALIZAR(&_texUnorganizedRefsMutex);
	}
	//Finalizar ambitos de atlases
	SI32 iAmb;
	for(iAmb = 0; iAmb < GESTOR_TEXTURAS_MAX_AMBITOS_TEXTURAS; iAmb++){
		STGestorTexAmbito* ambTex = &_ambitosAtlases[iAmb];
		//NBHILO_MUTEX_ACTIVAR(&ambTex->atlasesTexturasMutex)
		if(ambTex->atlasesTexturas != NULL){
			NBGestorGL::bindTexture(0, 0);
			UI32 iAtlas, iAtlasConteo = ambTex->atlasesTexturas->conteo;
			for(iAtlas=0; iAtlas<iAtlasConteo; iAtlas++){
				STGestorTexAtlas* atlasTex = ambTex->atlasesTexturas->elem(iAtlas);
				NBGestorTexturas::privSTGestorTexAtlasRelease(atlasTex);
			}
			ambTex->atlasesTexturas->liberar(NB_RETENEDOR_NULL);
			ambTex->atlasesTexturas = NULL;
		}
		//NBHILO_MUTEX_DESACTIVAR(&ambTex->atlasesTexturasMutex)
		//NBHILO_MUTEX_FINALIZAR(&ambTex->atlasesTexturasMutex)
	}
	if(_fuentesTexturas != NULL){
		//NBGestorGL::bindTexture(0, 0);
		SI32 iFnt;
		for(iFnt = _fuentesTexturas->conteo - 1; iFnt >= 0; iFnt--){
			STGestorTexFuente* datosFuente = _fuentesTexturas->elemPtr(iFnt);
			SI32 iTex; const SI32 conteoTex = datosFuente->texturas->conteo;
			for(iTex = 0; iTex < conteoTex; iTex++){
				STGestorTexFuenteTex* datTex = datosFuente->texturas->elemPtr(iTex);
				GLuint idTextura = datTex->propsTexturaGL.idTexturaGL;
				NBGestorGL::deleteTextures(1, &idTextura);
			}
			datosFuente->texturaFuente->liberar(NB_RETENEDOR_NULL);
			datosFuente->texturas->liberar(NB_RETENEDOR_NULL);
		}
		_fuentesTexturas->liberar(NB_RETENEDOR_NULL);
		_fuentesTexturas = NULL;
	}
	if(_fuentesRaster != NULL){
		SI32 iFnt;
		for(iFnt = _fuentesRaster->conteo - 1; iFnt >= 0; iFnt--){
			STGestorTexFuenteR* datosFuente = _fuentesRaster->elemPtr(iFnt);
			datosFuente->fuenteRaster->liberar(NB_RETENEDOR_NULL);
		}
		_fuentesRaster->liberar(NB_RETENEDOR_NULL);
		_fuentesRaster = NULL;
	}
	if(_fuentesRasterVAOs != NULL){
		SI32 iFnt;
		for(iFnt = _fuentesRasterVAOs->conteo - 1; iFnt >= 0; iFnt--){
			STGestorTexFuenteVAO* datosFuente = _fuentesRasterVAOs->elemPtr(iFnt);
			if(datosFuente->iVao >= 0){
				NBGestorGL::bufferVerticesLiberar(datosFuente->iVao);
			}
			if(datosFuente->fuenteLineas != NULL){
				NBFontLines_release(datosFuente->fuenteLineas);
				datosFuente->fuenteLineas = NULL;
			}
		}
		_fuentesRasterVAOs->liberar(NB_RETENEDOR_NULL);
		_fuentesRasterVAOs = NULL;
	}
	if(_pilaModosCargaTexturas != NULL){
		_pilaModosCargaTexturas->liberar(NB_RETENEDOR_NULL);
		_pilaModosCargaTexturas = NULL;
	}
	NBHILO_MUTEX_FINALIZAR(&_pilaModosCargaTexturasMutex)
	_gestorInicializado = false;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

//

static void NBGestorTexturas_getExtraChunck(void* param, const BYTE* blockId4, const BYTE* blockData, const SI32 blockDataSize){
	AUPngChuncksMutable* dstChuncks = (AUPngChuncksMutable*)param;
	if(dstChuncks != NULL){
		dstChuncks->setChunck((const char*)blockId4, blockData, blockDataSize);
	}
}

bool NBGestorTexturas::trabajadorSegundoPlanoIniciar(){
	bool r = false;
	if(_texLoadThread == NULL && _texLoadThreadMode == ENTexturaModoHilo_MultiHilo){
		_texLoadThread				= new(ENMemoriaTipo_General) AUHilo("NBGestorTexturas::_texLoadThread");
		_texLoadThreadStopFlag	= false;
		_texLoadThreadWorking		= true;
		if(!_texLoadThread->crearHiloYEjecuta(&NBGestorTexturas::funcHiloCargaDatosTexturas, NULL)){
			NBASSERT(false);
			_texLoadThreadWorking	= false;
			r = true;
		}
	}
	return r;
}

void NBGestorTexturas::trabajadorSegundoPlanoDetenerYEsperar(){
	while(_texLoadThreadWorking){
		_texLoadThreadStopFlag = true;
		NBHILO_SLEEP_MS(100);
	}
	if(_texLoadThread != NULL){
		_texLoadThread->liberar(NB_RETENEDOR_NULL);
		_texLoadThread = NULL;
	}
}

void NBGestorTexturas::funcHiloCargaDatosTexturas(void* param){
	_texLoadThreadWorking = true;
	PRINTF_INFO("TEX, entrando a hilo de carga de texturas.\n");
	while(!_texLoadThreadStopFlag){
		bool somethingLoaded = true;
		while(somethingLoaded && !_texLoadThreadStopFlag){
			somethingLoaded = false;
			STGestorTexAtlas* atlasTexToload = NULL;
			STGestorTexTextura* texToLoad = NULL;
			//Find the texture object to load
			{
				NBHILO_MUTEX_ACTIVAR(&_texLoadingRefsMutex);
				SI32 i; const SI32 count = _texLoadingRefs->conteo;
				for(i = 0; i < count; i++){
					STGestorTexTexturaRef* texRef	= _texLoadingRefs->elem(i);
					STGestorTexAtlas* atlasTex		= texRef->atlas;
					STGestorTexTextura* tex			= texRef->texture;
					NBASSERT(tex->loadingState != NULL)
					if(tex->loadingState != NULL){
						NBASSERT(tex->loadingState->linesLoadingBuffersInited)
						if(tex->loadingState->linesLoadingBuffersInited && !tex->loadingState->linesLoadingEnded){
							NBGestorTexturas::privSTGestorTexAtlasRetain(atlasTex);
							NBGestorTexturas::privSTGestorTexTexturaRetain(tex);
							atlasTexToload = atlasTex;
							texToLoad = tex;
							break;
						}
					}
				}
				NBHILO_MUTEX_DESACTIVAR(&_texLoadingRefsMutex);
			}
			/*{
				SI32 iAmb;
				for(iAmb = 0; iAmb < GESTOR_TEXTURAS_MAX_AMBITOS_TEXTURAS && texToLoad == NULL; iAmb++){
					STGestorTexAmbito* ambTex = &_ambitosAtlases[iAmb];
					NBHILO_MUTEX_ACTIVAR(&ambTex->atlasesTexturasMutex)
					{
						UI32 iAtlas, iAtlasConteo = ambTex->atlasesTexturas->conteo;
						for(iAtlas=0; iAtlas < iAtlasConteo && texToLoad == NULL; iAtlas++){
							STGestorTexAtlas* atlasTex = ambTex->atlasesTexturas->elem(iAtlas);
							if(atlasTex->mapaAtlas != NULL && atlasTex->texturasEnMapa != NULL){
								NBHILO_MUTEX_ACTIVAR(&atlasTex->texturasEnMapaMutex)
								SI32 iTex; const SI32 iTexCount = atlasTex->texturasEnMapa->conteo;
								for(iTex = 0 ; iTex < iTexCount; iTex++){
									STGestorTexTextura* tex = atlasTex->texturasEnMapa->elem(iTex);
									if(tex->loadingState != NULL){
										if(tex->loadingState->linesLoadingBuffersInited && !tex->loadingState->linesLoadingEnded){
											NBGestorTexturas::privSTGestorTexAtlasRetain(atlasTex);
											NBGestorTexturas::privSTGestorTexTexturaRetain(tex);
											atlasTexToload = atlasTex;
											texToLoad = tex;
											break;
										}
									}
								}
								NBHILO_MUTEX_DESACTIVAR(&atlasTex->texturasEnMapaMutex)
							}
						}
						ambTex->atlasesTexturas->liberar(NB_RETENEDOR_NULL);
						ambTex->atlasesTexturas = NULL;
					}
					NBHILO_MUTEX_DESACTIVAR(&ambTex->atlasesTexturasMutex)
				}
			}*/
			//Process texture
			if(texToLoad != NULL){
				NBASSERT(atlasTexToload != NULL)
				NBASSERT(texToLoad->loadingState->linesLoadingBuffersInited)
				NBASSERT(texToLoad->loadingState->linesBuffers != NULL)
				//Load buffer of lines
				//STGestorTexAtlas* atlasTex	= atlasTexToload;
				//STGestorTexTextura* texture		= texToLoad;
				STGestoTexLoadingBase* stateBase	= texToLoad->loadingState;
				if(stateBase->type == ENGestorTexturasLoadingType_Png){
					STGestoTexLoadingPNG* state = (STGestoTexLoadingPNG*)stateBase;
					const STNBBitmapProps props = NBPngLoadState_getProps(&state->load);
					NBASSERT(props.size.width > 0 && props.size.height > 0 && props.bytesPerLine > 0)
					{
						const UI32 pixsSz = (props.size.height * props.bytesPerLine);
						BYTE* pixs = (BYTE*)NBMemory_alloc(pixsSz);
						{
							NBPng_loadStreamLock(&state->load);
							if(!NBPng_loadWithState(&state->load, pixs, pixsSz, &state->extraChunks, NULL, NULL)){
								//
							} else {
								//Agregar buffer a cola y terminar ciclo
								STGestoTexLoadingBuffer buff;
								buff.data		= pixs; pixs = NULL;
								buff.size		= pixsSz;
								buff.use		= pixsSz;
								buff.linesLoaded= props.size.height;
								NBHILO_MUTEX_ACTIVAR(&stateBase->linesBuffersMutex)
								stateBase->linesBuffers->agregarElemento(buff);
								NBHILO_MUTEX_DESACTIVAR(&stateBase->linesBuffersMutex)
							}
							NBPng_loadStreamUnlock(&state->load);
						}
						//Release bufer (if not consumed)
						if(pixs != NULL){
							NBMemory_free(pixs);
							pixs = NULL;
						}
						state->linesLoadingEnded = true;;
					}
					//
					//
					//
					//ToDo: remove
					/*
					//Definir la cantidad de lineas por buffer
					const UI32 bmpDatosTamano				= (state->estado.propsMapaBits.bytesPorLinea * state->estado.propsMapaBits.alto);
					const UI32 tamMaxBuffer					= NBGESTORTEXTURAS_CARGA_MAX_BYTES_POR_BUFFER;
					const float cantMaxBuffersReq			= (float)bmpDatosTamano / (float)tamMaxBuffer;
					const UI32 lineasPorBuffer				= (cantMaxBuffersReq < 1.0f ? state->estado.propsMapaBits.alto : (tamMaxBuffer / state->estado.propsMapaBits.bytesPorLinea) < 1 ? 1 : (tamMaxBuffer / state->estado.propsMapaBits.bytesPorLinea));
					const UI32 bufferDestinoTamano			= (state->estado.propsMapaBits.bytesPorLinea * lineasPorBuffer);	NBASSERT(bufferDestinoTamano != 0)
					const UI32 bufferEntrelazadoTamano		= (1 + (state->estado.ancho * state->estado.bytesPorPixelPNG)) * lineasPorBuffer; NBASSERT(bufferEntrelazadoTamano != 0) //+1 para el entrelazado
					//
					if(state->estado.bufferEntrelazadoTamano != bufferEntrelazadoTamano){
						if(state->estado.bufferEntrelazado != NULL) NBGestorMemoria::liberarMemoria(state->estado.bufferEntrelazado);
						state->estado.bufferEntrelazadoTamano = bufferEntrelazadoTamano;
						state->estado.bufferEntrelazado		= (BYTE*)NBGestorMemoria::reservarMemoria(state->estado.bufferEntrelazadoTamano, ENMemoriaTipo_Temporal);
					}
					//const NBRectanguloUI16 areaEnAtlas = texture->textura->areaEnAtlas(); NBASSERT(areaEnAtlas.ancho > 0 && areaEnAtlas.alto > 0)
					//NBGestorGL::bindTexture(0, (GLuint)atlasTex->propsTexturaGL.idTexturaGL);
					while(!state->estado.IEND_leido){
						//Create new buffer if necesary
						if(state->estado.bufferDestinoTamano != bufferDestinoTamano){
							if(state->estado.bufferDestino != NULL) NBGestorMemoria::liberarMemoria(state->estado.bufferDestino);
							state->estado.bufferDestinoTamano	= bufferDestinoTamano;
							state->estado.bufferDestino			= (BYTE*)NBGestorMemoria::reservarMemoria(state->estado.bufferDestinoTamano, ENMemoriaTipo_Temporal);
						}
						if(!AUMapaBitsMutable::cargarDesdeArchivoPNG(&state->estado, (STNBSha1Hash*)NULL, NBGestorTexturas_getExtraChunck, state->extraChunks)){
							NBASSERT(false)
							stateBase->linesLoadingEnded = true;
							break;
						} else {
							if(state->estado.bufferDestinoPos != 0){
								NBASSERT((state->estado.bufferDestinoPos % state->estado.propsMapaBits.bytesPorLinea) == 0) //Se cargaron lineas completas
								NBASSERT((state->estado.bufferDestinoPos / state->estado.propsMapaBits.bytesPorLinea) == (state->estado.bufferEntrelazadoPos / (1 + (state->estado.ancho * state->estado.bytesPorPixelPNG)))) //Mismas cant de lineas en ambos bufferes
								const UI32 lineasCargadas = (state->estado.bufferDestinoPos / state->estado.propsMapaBits.bytesPorLinea); NBASSERT(lineasCargadas != 0)
								//NBGestorGL::texSubImage2D(GL_TEXTURE_2D, 0, areaEnAtlas.x, areaEnAtlas.y + lineasTotalCargadas, areaEnAtlas.ancho, lineasCargadas, state->formatoGlLocal, state->tpoDatoGlLocal, state->estado.bufferDestino, &atlasTex->propsTexturaGL); //GL_UNSIGNED_BYTE
								//_texsChangesCount++;
								if(state->calcularFirmaSHA1){
									NBSha1_feed(&state->sha1, state->estado.bufferDestino, state->estado.bufferDestinoPos);
								}
								//Agregar buffer a cola y terminar ciclo
								{
									STGestoTexLoadingBuffer buff;
									buff.data		= state->estado.bufferDestino;
									buff.size		= state->estado.bufferDestinoTamano;
									buff.use		= state->estado.bufferDestinoPos;
									buff.linesLoaded= lineasCargadas;
									NBHILO_MUTEX_ACTIVAR(&stateBase->linesBuffersMutex)
									stateBase->linesBuffers->agregarElemento(buff);
									NBHILO_MUTEX_DESACTIVAR(&stateBase->linesBuffersMutex)
									//Force the creation of a new buffer
									state->estado.bufferDestino			= NULL;
									state->estado.bufferDestinoTamano	= 0;
								}
								//PRINTF_INFO("Buffer PNG cargado con %d lineas (van %d de %d).\n", lineasCargadas, (state->linesAlreadyLoaded + lineasCargadas), state->estado.propsMapaBits.alto);
								state->linesAlreadyLoaded			+= lineasCargadas;
								state->estado.bufferEntrelazadoPos	= 0; //Reiniciar el buffer de entrelazado
								state->estado.bufferDestinoPos		= 0;
								break;
							}
						}
					}
					//End reading
					if(state->estado.IEND_leido){
						stateBase->linesLoadingEnded = true;
						if(state->calcularFirmaSHA1){
							if(NBSha1_feedEnd(&state->sha1)){
								state->firmaEnCarga = state->sha1.hash;
							}
						}
					}
					//Release buffers
					if(stateBase->linesLoadingEnded){
						if(state->estado.bufferDestino != NULL) NBGestorMemoria::liberarMemoria(state->estado.bufferDestino); state->estado.bufferDestino = NULL;
						if(state->estado.bufferEntrelazado != NULL) NBGestorMemoria::liberarMemoria(state->estado.bufferEntrelazado); state->estado.bufferEntrelazado = NULL;
						state->estado.bufferDestinoTamano = 0;
						state->estado.bufferEntrelazadoTamano = 0;
					}*/
				} else if(stateBase->type == ENGestorTexturasLoadingType_Jpg){
					stateBase->archivo->lock();
					{
						STGestoTexLoadingJPEG* state = (STGestoTexLoadingJPEG*)stateBase;
						//Definir la cantidad de lineas por buffer
						MapaDeBitsDesciptor props;
						if(!AUMapaBitsMutable::jpegReadStateGetProps(state->estado, &props)){
							stateBase->linesLoadingEnded = true;
							NBASSERT(false)
						} else {
							const UI32 bmpDatosTamano			= (props.bytesPorLinea * props.alto);
							const UI32 tamMaxBuffer				= NBGESTORTEXTURAS_CARGA_MAX_BYTES_POR_BUFFER;
							const float cantMaxBuffersReq		= (float)bmpDatosTamano / (float)tamMaxBuffer;
							const UI32 lineasPorBuffer			= (cantMaxBuffersReq < 1.0f ? props.alto : (tamMaxBuffer / props.bytesPorLinea) < 1 ? 1 : (tamMaxBuffer / props.bytesPorLinea));
							const UI32 bufferDestinoTamano		= (props.bytesPorLinea * lineasPorBuffer);	NBASSERT(bufferDestinoTamano != 0)
							//Create new buffer if necesary
							if(state->buffSz != bufferDestinoTamano){
								if(state->buff != NULL) NBGestorMemoria::liberarMemoria(state->buff);
								state->buffSz	= bufferDestinoTamano;
								state->buff		= (BYTE*)NBGestorMemoria::reservarMemoria(state->buffSz, ENMemoriaTipo_Temporal);
							}
							//Fill buffer
							UI32 linesRead = 0;
							ENJpegReadResult readResult;
							while(ENJpegReadResult_partial == (readResult = AUMapaBitsMutable::jpegReadStateRead(state->estado, state->buff, state->buffSz, &linesRead))){
								//read more
							}
							//Process result
							if(readResult != ENJpegReadResult_end){
								stateBase->linesLoadingEnded = true;
								NBASSERT(false)
							} else {
								NBASSERT(linesRead != 0)
								//Add buffer to queue
								{
									STGestoTexLoadingBuffer buff;
									buff.data		= state->buff;
									buff.size		= state->buffSz;
									buff.use		= (linesRead * props.bytesPorLinea);
									buff.linesLoaded= linesRead;
									NBHILO_MUTEX_ACTIVAR(&stateBase->linesBuffersMutex)
									stateBase->linesBuffers->agregarElemento(buff);
									NBHILO_MUTEX_DESACTIVAR(&stateBase->linesBuffersMutex)
									//Force the creation of a new buffer
									state->buff		= NULL;
									state->buffSz	= 0;
								}
								//
								state->linesAlreadyLoaded += linesRead;
								NBASSERT(state->linesAlreadyLoaded <= props.alto)
							}
							//End reading
							if(state->linesAlreadyLoaded >= props.alto){
								stateBase->linesLoadingEnded = true;
							}
						}
						//Release buffers
						if(stateBase->linesLoadingEnded){
							if(state->buff != NULL){
								NBGestorMemoria::liberarMemoria(state->buff);
								state->buff = NULL;
								state->buffSz = 0;
							}
						}
					}
					stateBase->archivo->unlock();
				} else  {
					NBASSERT(false)
				}
				//Release
				NBGestorTexturas::privSTGestorTexAtlasRelease(atlasTexToload);
				NBGestorTexturas::privSTGestorTexTexturaRelease(texToLoad);
				somethingLoaded = true; //mark, to continue without sleeping
#				ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
				//Simulate slow loading
				//NBHILO_SLEEP_MS(25);
#				endif
			}
		}
		//Sleep
		NBHILO_SLEEP_MS(100);
	}
	PRINTF_INFO("TEX, saliendo a hilo de carga de texturas.\n");
	_texLoadThreadWorking = false;
}

//

bool NBGestorTexturas::gestorInicializado(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTexturas::gestorInicializado")
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return _gestorInicializado;
}

bool NBGestorTexturas::reiniciarContextoGrafico(const bool elimObjetosAnteriores){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTexturas::reiniciarContextoGrafico")
	bool exito = true;
	NBASSERT(_gestorInicializado)
	//Recargar texturas (que vienen desde archivo)
	NBGestorTexturas::privRecargarTexturas(elimObjetosAnteriores);
	//Regenerar texturas que son lienzos (no cargadas desde archivo)
	NBGestorTexturas::privRegenerarAtlasesTexturasEscritura(elimObjetosAnteriores);
	//Regenerar fuentes
	NBGestorTexturas::privRegenerarAtlasesFuentesTexturas(elimObjetosAnteriores);
	//Reiniciar versiones de VAO (obliga a actualizar los datos)
	if(_fuentesRasterVAOs != NULL){
		UI32 i; UI32 conteo = _fuentesRasterVAOs->conteo;
		for(i = 0; i < conteo; i++){
			STGestorTexFuenteVAO* dat = _fuentesRasterVAOs->elemPtr(i);
			dat->versionEnVao = 0;
		}
	}
	//Actualizar MipMaps
	NBGestorTexturas::generarMipMapsDeTexturas();
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return exito;
}

//

void NBGestorTexturas::privSTGestorTexTexturaRetain(STGestorTexTextura* obj){
	NBASSERT(obj->retainCount > 0)
	obj->retainCount++;
}

void NBGestorTexturas::privSTGestorTexTexturaRelease(STGestorTexTextura* obj){
	NBASSERT(obj->retainCount > 0)
	obj->retainCount--;
	//
	if(obj->retainCount == 0){
		//Release
		obj->textura->liberar(NB_RETENEDOR_NULL);
		obj->nombreTextura->liberar(NB_RETENEDOR_NULL);
		//Release subtextures
		if(obj->subTexturasMapa != NULL) obj->subTexturasMapa->liberar(NB_RETENEDOR_NULL);
		if(obj->subTexturas != NULL) obj->subTexturas->liberar(NB_RETENEDOR_NULL);
		//Release loading state
		if(obj->loadingState != NULL){
			NBGestorTexturas::privLoadingStateRelease(obj->loadingState);
			NBGestorMemoria::liberarMemoria(obj->loadingState);
			obj->loadingState = NULL;
		}
		NBGestorMemoria::liberarMemoria(obj);
	}
}

//

void NBGestorTexturas::privSTGestorTexAtlasRetain(STGestorTexAtlas* obj){
	NBASSERT(obj->retainCount > 0)
	obj->retainCount++;
}

void NBGestorTexturas::privSTGestorTexAtlasRelease(STGestorTexAtlas* obj){
	NBASSERT(obj->retainCount > 0)
	obj->retainCount--;
	//
	if(obj->retainCount == 0){
		if(obj->mapaAtlas != NULL){
			obj->mapaAtlas->liberar(NB_RETENEDOR_NULL);
			obj->mapaAtlas = NULL;
		}
		if(obj->texturasEnMapa != NULL){
			//NBHILO_MUTEX_ACTIVAR(&obj->texturasEnMapaMutex)
			SI32 i; const SI32 count = obj->texturasEnMapa->conteo;
			for(i = 0; i < count; i++){
				NBGestorTexturas::privSTGestorTexTexturaRelease(obj->texturasEnMapa->elem(i));
			}
			obj->texturasEnMapa->vaciar();
			obj->texturasEnMapa->liberar(NB_RETENEDOR_NULL);
			obj->texturasEnMapa = NULL;
			//NBHILO_MUTEX_DESACTIVAR(&obj->texturasEnMapaMutex)
			//NBHILO_MUTEX_FINALIZAR(&obj->texturasEnMapaMutex);
		}
		if(obj->propsTexturaGL.idTexturaGL != 0){
			GLuint idTextura = obj->propsTexturaGL.idTexturaGL;
			//NBGestorGL::bindTexture(0, 0); //NBGestorTexturas::texturaBlanca->idTexturaGL
			NBGestorGL::deleteTextures(1, &idTextura);
			obj->propsTexturaGL.idTexturaGL = 0;
		}
		//
		NBGestorMemoria::liberarMemoria(obj);
	}
}

//

void NBGestorTexturas::bloquear(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTexturas::bloquear")
	NBASSERT(_gestorInicializado)
	_bloqueado = true;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

void NBGestorTexturas::desbloquear(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTexturas::desbloquear")
	NBASSERT(_gestorInicializado)
	_bloqueado = false;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

bool NBGestorTexturas::bloqueado(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTexturas::bloqueado")
	NBASSERT(_gestorInicializado)
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return _bloqueado;
}

const char* NBGestorTexturas::pefijoRutas(const ENTexturaResolucion res){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTexturas::pefijoRutas")
	NBASSERT(_gestorInicializado)
	const char* r = _propsByResolution[res].pathPrefix->str();
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return r;
}

void NBGestorTexturas::establecerPrefijoRutas(const char* prefijoStr, const UI16 escalaTexturasBase2, const float escalaTexturasHaciaHD, const ENTexturaResolucion res){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTexturas::establecerPrefijoRutas")
	NBASSERT(_gestorInicializado)
	NBASSERT(escalaTexturasBase2 == 1 || escalaTexturasBase2 == 2 || escalaTexturasBase2 == 4 || escalaTexturasBase2 == 8 || escalaTexturasBase2 == 16)
	STTexturaResProps* resProps = &_propsByResolution[res];
	resProps->setStatus		= ENTexturaPropsSetStatus_Explicit;
	resProps->pathPrefix->establecer(prefijoStr);
	resProps->scaleBase2	= escalaTexturasBase2;
	resProps->scaleToHD		= escalaTexturasHaciaHD;
	//Compatibility code, set other undefined props
	{
		SI32 i; for(i = 0 ; i < ENTexturaResolucion_Conteo; i++){
			if(i != res){
				STTexturaResProps* resProps = &_propsByResolution[i];
				if(resProps->setStatus == ENTexturaPropsSetStatus_Unset || (resProps->setStatus == ENTexturaPropsSetStatus_Implicit && res == ENTexturaResolucion_Escena)){
					resProps->setStatus		= ENTexturaPropsSetStatus_Implicit;
					resProps->pathPrefix->establecer(prefijoStr);
					resProps->scaleBase2	= escalaTexturasBase2;
					resProps->scaleToHD		= escalaTexturasHaciaHD;
				}
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

SI32 NBGestorTexturas::escalaTexturasBase2(const ENTexturaResolucion res){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTexturas::escalaTexturasBase2")
	NBASSERT(_gestorInicializado)
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return _propsByResolution[res].scaleBase2;
}

float NBGestorTexturas::escalaTexturasHaciaHD(const ENTexturaResolucion res){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTexturas::escalaTexturasHaciaHD")
	NBASSERT(_gestorInicializado)
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return _propsByResolution[res].scaleToHD;
}

ENGestorTexturaModo NBGestorTexturas::modoCargaTexturas(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTexturas::modoCargaTexturas")
	NBASSERT(_gestorInicializado)
	//
	NBHILO_MUTEX_ACTIVAR(&_pilaModosCargaTexturasMutex)
	NBASSERT(_pilaModosCargaTexturas->conteo > 0)
	ENGestorTexturaModo r = _pilaModosCargaTexturas->elem(_pilaModosCargaTexturas->conteo - 1);
	NBHILO_MUTEX_DESACTIVAR(&_pilaModosCargaTexturasMutex)
	//
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return r;
}

void NBGestorTexturas::modoCargaTexturasPush(const ENGestorTexturaModo nuevoModo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTexturas::modoCargaTexturasPush")
	NBASSERT(_gestorInicializado)
	NBASSERT(nuevoModo >= 0 && nuevoModo < ENGestorTexturaModo_conteo)
	NBASSERT(nuevoModo != ENGestorTexturaModo_cargaSegundoPlano || _texLoadThreadMode == ENTexturaModoHilo_MultiHilo)
	//
	NBHILO_MUTEX_ACTIVAR(&_pilaModosCargaTexturasMutex)
	NBASSERT(_pilaModosCargaTexturas->conteo > 0)
	_pilaModosCargaTexturas->agregarElemento(nuevoModo);
	NBHILO_MUTEX_DESACTIVAR(&_pilaModosCargaTexturasMutex)
	//
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

void NBGestorTexturas::modoCargaTexturasPop(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTexturas::modoCargaTexturasPop")
	NBASSERT(_gestorInicializado)
	//
	NBHILO_MUTEX_ACTIVAR(&_pilaModosCargaTexturasMutex)
	NBASSERT(_pilaModosCargaTexturas->conteo > 1)
	if(_pilaModosCargaTexturas->conteo > 1){
		_pilaModosCargaTexturas->quitarElementoEnIndice(_pilaModosCargaTexturas->conteo - 1);
	}
	NBHILO_MUTEX_DESACTIVAR(&_pilaModosCargaTexturasMutex)
	//
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

SI32 NBGestorTexturas::texPendienteOrganizarConteo(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTexturas::texPendienteOrganizarConteo")
	NBASSERT(_gestorInicializado)
	SI32 r = 0;
	NBHILO_MUTEX_ACTIVAR(&_texUnorganizedRefsMutex);
	r = _texUnorganizedRefs->conteo;
	NBHILO_MUTEX_DESACTIVAR(&_texUnorganizedRefsMutex);
#	ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
	//Verification
	{
		SI32 iAmb, conteoPrecacheadas = 0;
		for(iAmb=0; iAmb < GESTOR_TEXTURAS_MAX_AMBITOS_TEXTURAS; iAmb++){
			const STGestorTexAmbito* ambTex = &_ambitosAtlases[iAmb];
			SI32 iAtlas; const SI32 atlasCount = ambTex->atlasesTexturas->conteo;
			for(iAtlas = 0; iAtlas < atlasCount; iAtlas++){
				const STGestorTexAtlas* atlas = ambTex->atlasesTexturas->elem(iAtlas);
				if(atlas->mapaAtlas == NULL){ //Preloaded textures
					conteoPrecacheadas += atlas->texturasEnMapa->conteo;
				}
			}
		}
		NBASSERT(r == conteoPrecacheadas);
	}
#	endif
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return r;
}

void NBGestorTexturas::texPendienteOrganizarProcesar(const SI32 maxProcesar){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTexturas::texPendienteOrganizarProcesar")
	NBASSERT(_gestorInicializado)
	const ENGestorTexturaModo modoCargaTexturas = NBGestorTexturas::modoCargaTexturas();
	if(modoCargaTexturas == ENGestorTexturaModo_cargaPendiente){
		PRINTF_WARNING("ENGestorTexturaModo_cargaPendiente al intentar cargar pendientes\n");
	} else {
		SI32 conteoCargadas = 0;
		while(conteoCargadas < maxProcesar && NBGestorTexturas::texPendienteOrganizarConteo() > 0){
			bool unaProcesada = false;
			UI8 iAmb;
			for(iAmb = 0; iAmb < GESTOR_TEXTURAS_MAX_AMBITOS_TEXTURAS && !unaProcesada; iAmb++){
				STGestorTexAmbito* ambTex = &_ambitosAtlases[iAmb];
				SI32 iAtlas; const SI32 atlasCount = ambTex->atlasesTexturas->conteo;
				for(iAtlas = 0; iAtlas < atlasCount; iAtlas++){
					STGestorTexAtlas* atlas = ambTex->atlasesTexturas->elem(iAtlas);
					if(atlas->mapaAtlas == NULL){ //Preloaded textures
						SI32 indiceUsar			= 0;
						UI32 areaMasGrande		= 0;
						//Encontrar la mas grande (ha demostrado que procesar de grande a pequena organiza mejor el espacio)
						UI32 iPend, iPendConteo = atlas->texturasEnMapa->conteo;
						for(iPend=0; iPend<iPendConteo; iPend++){
							NBTamano tamTextura = atlas->texturasEnMapa->elem(iPend)->textura->tamano();
							UI32 area = tamTextura.ancho * tamTextura.alto;
							if(iPend == 0 || area > areaMasGrande){
								indiceUsar		= iPend;
								areaMasGrande	= area;
							}
						}
						//
						STGestorTexTextura* preTextura = atlas->texturasEnMapa->elem(indiceUsar);
						{
							switch (preTextura->origenFormato) {
								case ENTexturaOrigenFormato_Png:
									NBGestorTexturas::privTexturaDesdeArchivoPNG(iAmb, preTextura->origenTextura, preTextura->escalaBase2, preTextura->escalaParaHD, preTextura->nombreTextura->str(), false, preTextura->tipoAlmacenamientoGL, preTextura->tipoUsoGL, preTextura->tipoPintado, preTextura->mipMapHabiltado, preTextura);
									break;
								case ENTexturaOrigenFormato_Jpg:
									NBGestorTexturas::privTexturaDesdeArchivoJPEG(iAmb, preTextura->origenTextura, preTextura->escalaBase2, preTextura->escalaParaHD, preTextura->nombreTextura->str(), false, preTextura->tipoAlmacenamientoGL, preTextura->tipoUsoGL, preTextura->tipoPintado, preTextura->mipMapHabiltado, preTextura);
									break;
								default:
									NBASSERT(false)
									break;
							}
						}
						//Remove from list
						{
							STGestorTexTexturaRef texRef;
							texRef.iAmb		= iAmb;
							texRef.atlas	= atlas;
							texRef.texture	= preTextura;
							NBHILO_MUTEX_ACTIVAR(&_texUnorganizedRefsMutex)
							const SI32 iEncontrado = _texUnorganizedRefs->indiceDe(texRef); NBASSERT(iEncontrado != -1)
							if(iEncontrado != -1){
								NBGestorTexturas::privSTGestorTexAtlasRelease(texRef.atlas);
								NBGestorTexturas::privSTGestorTexTexturaRelease(texRef.texture);
								_texUnorganizedRefs->quitarElementoEnIndice(iEncontrado);
								NBASSERT(_texUnorganizedRefs->indiceDe(texRef) == -1)
							}
							NBHILO_MUTEX_DESACTIVAR(&_texUnorganizedRefsMutex)
						}
						//Remove from atlas
						//NBHILO_MUTEX_ACTIVAR(&atlas->texturasEnMapaMutex)
						NBGestorTexturas::privSTGestorTexTexturaRelease(preTextura);
						atlas->texturasEnMapa->quitarElementoEnIndice(indiceUsar);
						//NBHILO_MUTEX_DESACTIVAR(&atlas->texturasEnMapaMutex)
						unaProcesada = true;
						conteoCargadas++;
					}
				}
			}
		}
		PRINTF_INFO("%d textures loaded from preloaded state.\n", conteoCargadas);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

SI32 NBGestorTexturas::texCargandoSegundoPlanoConteo(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTexturas::texCargandoSegundoPlanoConteo")
	NBASSERT(_gestorInicializado)
	SI32 r = 0;
	NBHILO_MUTEX_ACTIVAR(&_texLoadingRefsMutex);
	r = _texLoadingRefs->conteo;
#	ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
	//Verification
	{
		SI32 iAmb, conteoEnCarga = 0;
		for(iAmb=0; iAmb < GESTOR_TEXTURAS_MAX_AMBITOS_TEXTURAS; iAmb++){
			const STGestorTexAmbito* ambTex = &_ambitosAtlases[iAmb];
			SI32 iAtlas; const SI32 atlasCount = ambTex->atlasesTexturas->conteo;
			for(iAtlas = 0; iAtlas < atlasCount; iAtlas++){
				const STGestorTexAtlas* atlas = ambTex->atlasesTexturas->elem(iAtlas);
				if(atlas->mapaAtlas != NULL){ //Preloaded textures
					SI32 iTex; const SI32 iTexCount = atlas->texturasEnMapa->conteo;
					for(iTex = 0; iTex < iTexCount; iTex++){
						STGestorTexTextura* tex = atlas->texturasEnMapa->elem(iTex);
						if(tex->loadingState != NULL){
							if(tex->loadingState->linesLoadingBuffersInited){
								conteoEnCarga++;
							}
						}
					}
				}
			}
		}
		NBASSERT(r == conteoEnCarga);
	}
#	endif
	NBHILO_MUTEX_DESACTIVAR(&_texLoadingRefsMutex);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return r;
}

SI32 NBGestorTexturas::texCargandoSegundoPlanoProcesarBufferes(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTexturas::texCargandoSegundoPlanoProcesarBufferes")
	SI32 r = 0;
	NBHILO_MUTEX_ACTIVAR(&_texLoadingRefsMutex);
	//if(_texLoadingRefs->conteo != 0){ PRINTF_INFO("Analizando %d texturas en carga en segundo plano.\n", _texLoadingRefs->conteo); }
	SI32 i;
	for(i = (_texLoadingRefs->conteo - 1); i >= 0 ; i--){
		STGestorTexTexturaRef* texRef = _texLoadingRefs->elem(i);
		STGestorTexTextura* texture = texRef->texture;
		NBASSERT(texture->loadingState != NULL)
		if(texture->loadingState != NULL){
			STGestoTexLoadingBase* loadState = texture->loadingState;
			NBASSERT(loadState->linesLoadingBuffersInited)
			if(loadState->linesLoadingBuffersInited){
				//Area at atlas
				const NBRectanguloUI16 areaEnAtlas = texture->textura->areaEnAtlas(); NBASSERT(areaEnAtlas.ancho > 0 && areaEnAtlas.alto > 0)
				//Bind atlas
				NBASSERT(texRef->atlas->propsTexturaGL.idTexturaGL != 0)
				NBGestorGL::bindTexture(0, (GLuint)texRef->atlas->propsTexturaGL.idTexturaGL);
				//Process buffers
				NBHILO_MUTEX_ACTIVAR(&loadState->linesBuffersMutex);
				//if(loadState->linesBuffers->conteo != 0){ PRINTF_INFO("Procesando %d bufferes de carga en segundo plano de texturas.\n", loadState->linesBuffers->conteo); }
				while(loadState->linesBuffers->conteo != 0){
					STGestoTexLoadingBuffer* buffer = loadState->linesBuffers->elemPtr(0);
					NBASSERT(buffer->data != NULL && buffer->use > 0 && buffer->linesLoaded > 0)
					if(buffer->data != NULL && buffer->use > 0 && buffer->linesLoaded > 0){
						NBGestorGL::texSubImage2D(GL_TEXTURE_2D, 0, areaEnAtlas.x, areaEnAtlas.y + loadState->linesAlreadyPasted, areaEnAtlas.ancho, buffer->linesLoaded, loadState->formatoGlLocal, loadState->tpoDatoGlLocal, buffer->data, &texRef->atlas->propsTexturaGL); //GL_UNSIGNED_BYTE
						_texsChangesCount++;
						loadState->linesAlreadyPasted += buffer->linesLoaded;
					}
					//Release buffer
					if(buffer->data != NULL) NBGestorMemoria::liberarMemoria(buffer->data); buffer->data = NULL;
					buffer->size = 0; buffer->use = 0; buffer->linesLoaded = 0;
					loadState->linesBuffers->quitarElementoEnIndice(0);
				}
				NBHILO_MUTEX_DESACTIVAR(&loadState->linesBuffersMutex);
				//Remove texture if loading finished
				if(loadState->linesLoadingEnded){
					texture->textura->privEstablecerEstadoCarga(ENTexturaEstadoCarga_Cargada);
					//Release loading state
					NBASSERT(texture->loadingState != NULL)
					if(texture->loadingState != NULL){
						NBGestorTexturas::privLoadingStateRelease(texture->loadingState);
						NBGestorMemoria::liberarMemoria(texture->loadingState);
						texture->loadingState = NULL;
					}
					//
					NBGestorTexturas::privSTGestorTexAtlasRelease(texRef->atlas);
					NBGestorTexturas::privSTGestorTexTexturaRelease(texRef->texture);
					NBGestorMemoria::liberarMemoria(texRef);
					_texLoadingRefs->quitarElementoEnIndice(i);
				}
			}
		}
	}
	NBHILO_MUTEX_DESACTIVAR(&_texLoadingRefsMutex);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return r;
}

//Font and other textures sync
void NBGestorTexturas::texSyncAny(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTexturas::texSyncAny")
	//Sync necesary textureFonts
	{
		SI32 iFnt; const SI32 countFnt = _fuentesTexturas->conteo;
		for(iFnt = 0; iFnt < countFnt; iFnt++){
			STGestorTexFuente* f = _fuentesTexturas->elemPtr(iFnt);
			const UI32 curVer = NBFontBitmaps_getVersion(f->fontBitmaps);
			if(f->version != curVer){
				//PRINTF_INFO("Syncing fontTexture #%d, from v%u to v%u ('%s'/'%s'/%s%s::%.1f).\n", (iFnt + 1), f->version, curVer, f->texturaFuente->familia(), f->texturaFuente->subfamilia(), f->texturaFuente->esNegrilla() ? "n" : "", f->texturaFuente->esCursiva() ? "c" : "", f->texturaFuente->tamanoEscena());
				UI32 i = 0, updatedCount = 0;
				do {
					STNBFontBitmap bmp = NBFontBitmaps_getBitmapAtIndex(f->fontBitmaps, i);
					if(bmp.bitmap == NULL){
						break;
					} else {
						const STNBBitmapProps bmpProps = NBBitmap_getProps(bmp.bitmap);
						const BYTE* bmpData = NBBitmap_getData(bmp.bitmap);
						NBASSERT(bmpProps.size.width > 0 && bmpProps.size.height > 0);
						MapaBitsColor colorDestino = COLOR_NO_DEFINIDO;
						switch (bmpProps.color) {
							case ENNBBitmapColor_ALPHA8: colorDestino = COLOR_ALPHA8; break;
							case ENNBBitmapColor_GRIS8: colorDestino = COLOR_GRIS8; break;
							case ENNBBitmapColor_GRISALPHA8: colorDestino = COLOR_GRISALPHA8; break;
							case ENNBBitmapColor_RGB8: colorDestino = COLOR_RGB8; break;
							case ENNBBitmapColor_RGBA8: colorDestino = COLOR_RGBA8; break;
							case ENNBBitmapColor_ARGB8: colorDestino = COLOR_ARGB8; break;
							case ENNBBitmapColor_BGRA8: colorDestino = COLOR_BGRA8; break;
#							ifdef NB_CONFIG_INCLUDE_ASSERTS
							default:
								NBASSERT(FALSE) 
								break;
#							endif
						}
						GLenum formatoGlLocal		= NBGestorTexturas::formatoGlLocal(colorDestino); NBASSERT(formatoGlLocal != 0)
						GLenum formatoGlDriver		= NBGestorTexturas::formatoGlDriver(colorDestino); NBASSERT(formatoGlDriver != 0)
						GLenum tpoDatoGlDriver		= NBGestorTexturas::tipoDatoGlDriver(colorDestino); NBASSERT(tpoDatoGlDriver != 0)
						if(formatoGlLocal == 0 || formatoGlDriver == 0 || tpoDatoGlDriver == 0){
							PRINTF_ERROR("no se pudo establecer el formatoGLLocal(%d) formatoGLDriver(%d) o tipoDatoGLDriver(%d) del nuevo atlas\n", formatoGlLocal, formatoGlDriver, tpoDatoGlDriver);
						} else {
							if(i < f->texturas->conteo){
								STGestorTexFuenteTex* fTex = f->texturas->elemPtr(i);
								NBASSERT(fTex->iAtlasEnFuente == i)
								if(fTex->version != bmp.version){
									//PRINTF_INFO("Syncing fontTexture #%d, updated bitmap #%d from v%u to v%u ('%s'/'%s'/%s%s::%.1f).\n", (iFnt + 1), (i + 1), fTex->version, bmp.version, f->texturaFuente->familia(), f->texturaFuente->subfamilia(), f->texturaFuente->esNegrilla() ? "n" : "", f->texturaFuente->esCursiva() ? "c" : "", f->texturaFuente->tamanoEscena());
									GLuint idTextura = fTex->propsTexturaGL.idTexturaGL;
#									ifdef CONFIG_NB_GESTOR_GL_IMPRIMIR_COMANDOS_GL
									//PRINTF_WARNING("Actualizando textura de fuente: '%s%s%s'::%f tam(%d, %d) color(%d).\n", nombreFuente, (negrilla ? "_n" : ""), (cursiva ? "_c" : ""), tamanoFuente, (SI32)bmpProps.size.width, (SI32)bmpProps.size.height, (SI32)colorDestino);
#									endif
									NBGestorGL::bindTexture(0, idTextura);
									NBGestorGL::configurarTextura(ENTexturaModoPintado_Imagen_Precisa, false, &fTex->propsTexturaGL);
									NBGestorGL::texImage2D(GL_TEXTURE_2D, 0, formatoGlLocal, bmpProps.size.width, bmpProps.size.height, 0, formatoGlDriver, tpoDatoGlDriver, bmpData, &fTex->propsTexturaGL);
									_texsChangesCount++;
									fTex->version = bmp.version;
									updatedCount++;
								}
							} else {
								GLuint idTextura;
								STGestorTexFuenteTex nuevaFuenteTex;
#								ifdef CONFIG_NB_GESTOR_GL_IMPRIMIR_COMANDOS_GL
								//PRINTF_WARNING("Generando textura de fuente: '%s%s%s'::%f tam(%d, %d) color(%d).\n", nombreFuente, (negrilla ? "_n" : ""), (cursiva ? "_c" : ""), tamanoFuente, (SI32)bmpProps.size.width, (SI32)bmpProps.size.height, (SI32)colorDestino);
#								endif
								NBGestorGL::genTextures(1, &idTextura, &nuevaFuenteTex.propsTexturaGL);
								NBGestorGL::bindTexture(0, idTextura);
								NBGestorGL::configurarTextura(ENTexturaModoPintado_Imagen_Precisa, false, &nuevaFuenteTex.propsTexturaGL);
								NBGestorGL::texImage2D(GL_TEXTURE_2D, 0, formatoGlLocal, bmpProps.size.width, bmpProps.size.height, 0, formatoGlDriver, tpoDatoGlDriver, bmpData, &nuevaFuenteTex.propsTexturaGL);
								_texsChangesCount++;
								NBASSERT(NBGestorGL::isTexture(idTextura))
								nuevaFuenteTex.version			= bmp.version;
								nuevaFuenteTex.iAtlasEnFuente	= i;
								nuevaFuenteTex.colorFormato		= colorDestino;
								//Nota: las texturas de texto no estan definidas en base a una resolucion (X4, X2, X1, etc..)
								//es decir, no se escalan hacia HD (ya son HD)
								f->texturas->agregarElemento(nuevaFuenteTex);
								f->texturaFuente->ligarAtlasConTexturaGl(nuevaFuenteTex.iAtlasEnFuente, idTextura);
								updatedCount++;
								//PRINTF_INFO("Syncing fontTexture #%d, creating bitmap #%dv%u ('%s'/'%s'/%s%s::%.1f).\n", (iFnt + 1), (i + 1), bmp.version, f->texturaFuente->familia(), f->texturaFuente->subfamilia(), f->texturaFuente->esNegrilla() ? "n" : "", f->texturaFuente->esCursiva() ? "c" : "", f->texturaFuente->tamanoEscena());
							}
						}
					}
					i++;
				} while(TRUE);
				NBASSERT(updatedCount > 0)
				f->version = curVer;
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

UI32 NBGestorTexturas::texsChangesCount(const BOOL resetCount){
	const UI32 r = _texsChangesCount;
	if(resetCount) _texsChangesCount = 0;
	return r;
}

//
//Volver a cargar texturas (probablemente cambio de resolucion)
//
void NBGestorTexturas::recargarTexturas(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTexturas::recargarTexturas")
	NBASSERT(_gestorInicializado)
	//Recargar texturas
	NBGestorTexturas::privRecargarTexturas(true);
	//Actualizar MipMaps
	NBGestorTexturas::generarMipMapsDeTexturas();
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

void NBGestorTexturas::privRecargarTexturas(const bool elimTexturasGLAnteriores){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTexturas::privRecargarTexturas")
	NBASSERT(_gestorInicializado)
	//Almacen temporal
	AUArregloNativoMutableP<STGestorTexTextura*>* texturasRecargar = new(ENMemoriaTipo_Temporal) AUArregloNativoMutableP<STGestorTexTextura*>();
	//Para cada ambito de textura
	UI32 iAmb;
	for(iAmb = 0; iAmb < GESTOR_TEXTURAS_MAX_AMBITOS_TEXTURAS; iAmb++){
		STGestorTexAmbito* ambTex = &_ambitosAtlases[iAmb];
		if(ambTex->atlasesTexturas != NULL){
			texturasRecargar->vaciar();
			//
			//Recopilar los datos de todas las texturas cargadas en memoria de este Ambito.
			//Liberar todos los atlas.
			//
			SI32 iAtlas;
			for(iAtlas = ambTex->atlasesTexturas->conteo - 1; iAtlas >= 0; iAtlas--){
				STGestorTexAtlas* atlasTex = ambTex->atlasesTexturas->elem(iAtlas);
				if(atlasTex->tipoUsoGL == ENTExturaTipoUsoGL_Lectura){
					//AUArregloNativoMutableP<STGestorTexTextura*>* texturas = atlasTex->texturasEnMapa;
					UI32 iTextura, iTexturaConteo = atlasTex->texturasEnMapa->conteo;
					//Determinar si todos los recursos podran volver a ser cargados
					bool contieneRecursosSinNombre = false;
					for(iTextura=0; iTextura<iTexturaConteo; iTextura++){
						if(atlasTex->texturasEnMapa->elem(iTextura)->nombreTextura->tamano() == 0){
							contieneRecursosSinNombre = true;
							break;
						}
					}
					//Agregar las texturas a la lista (recargar) y eliminar los atlas actuales
					if(!contieneRecursosSinNombre){
						for(iTextura=0; iTextura < iTexturaConteo; iTextura++){
							STGestorTexTextura* textura = atlasTex->texturasEnMapa->elem(iTextura);
							NBGestorTexturas::privSTGestorTexTexturaRetain(textura);
							texturasRecargar->agregarElemento(textura);
						}
						//NBHILO_MUTEX_ACTIVAR(&ambTex->atlasesTexturasMutex)
						if(!elimTexturasGLAnteriores){ atlasTex->propsTexturaGL.idTexturaGL = 0; } //Set zero before releasing
						NBGestorTexturas::privSTGestorTexAtlasRelease(atlasTex);
						ambTex->atlasesTexturas->quitarElementoEnIndice(iAtlas);
						//NBHILO_MUTEX_DESACTIVAR(&ambTex->atlasesTexturasMutex)
					}
				}
			}
			//
			//Recargar todas las texturas previamente cargadas
			//
			UI32 iTex;
			for(iTex = 0; iTex < texturasRecargar->conteo; iTex++){
				STGestorTexTextura* textura = texturasRecargar->elem(iTex);
				NBASSERT(textura->textura != NULL)
				NBASSERT(textura->nombreTextura != NULL)
				switch (textura->origenFormato) {
					case ENTexturaOrigenFormato_Png:
						NBGestorTexturas::privTexturaDesdeArchivoPNG(iAmb, textura->origenTextura, textura->escalaBase2, textura->escalaParaHD, textura->nombreTextura->str(), false, textura->tipoAlmacenamientoGL, textura->tipoUsoGL, textura->tipoPintado, textura->mipMapHabiltado, textura);
						break;
					case ENTexturaOrigenFormato_Jpg:
						NBGestorTexturas::privTexturaDesdeArchivoJPEG(iAmb, textura->origenTextura, textura->escalaBase2, textura->escalaParaHD, textura->nombreTextura->str(), false, textura->tipoAlmacenamientoGL, textura->tipoUsoGL, textura->tipoPintado, textura->mipMapHabiltado, textura);
						break;
					default:
						NBASSERT(false)
						break;
				}
				NBGestorTexturas::privSTGestorTexTexturaRelease(textura);
			}
		}
	}
	texturasRecargar->liberar(NB_RETENEDOR_NULL); texturasRecargar = NULL;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

void NBGestorTexturas::privRegenerarAtlasesTexturasEscritura(const bool elimTexturasGLAnteriores){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTexturas::privRegenerarAtlasesTexturasEscritura")
	NBASSERT(_gestorInicializado)
	UI32 iAmb;
	for(iAmb = 0; iAmb < GESTOR_TEXTURAS_MAX_AMBITOS_TEXTURAS; iAmb++){
		STGestorTexAmbito* ambTex = &_ambitosAtlases[iAmb];
		if(ambTex->atlasesTexturas != NULL){
			//
			//Recopilar los datos de todas las texturas cargadas en memoria de este Ambito.
			//Liberar todos los atlas.
			//
			SI32 iAtlas;
			for(iAtlas = ambTex->atlasesTexturas->conteo - 1; iAtlas >= 0; iAtlas--){
				STGestorTexAtlas* atlasTex = ambTex->atlasesTexturas->elem(iAtlas);
				if(atlasTex->tipoUsoGL == ENTExturaTipoUsoGL_EscrituraLectura){
					const SI32 anchoAtlas					= atlasTex->propsTexturaGL.anchoGL;
					const SI32 altoAtlas					= atlasTex->propsTexturaGL.altoGL;
					const MapaBitsColor colorDestino		= atlasTex->colorFormato;
					const ENTexturaTipoAlmacenamientoGL tipoAlmacenamiento = atlasTex->tipoAlmacenamientoGL;
					const ENTexturaTipoUsoGL tipoUso		= atlasTex->tipoUsoGL;
					const ENTexturaModoPintado modoPintado	= atlasTex->modoPintado;
					///const ENTexturaMipMap habilitarMipMap= (ENTexturaMipMap)atlasTex->propsTexturaGL.mipMapHabilitado;
					AUArregloNativoMutableP<STGestorTexTextura*>* texturas = atlasTex->texturasEnMapa;
					//Eliminar el atlasGL actual
					if(elimTexturasGLAnteriores){
						GLuint idTextura = atlasTex->propsTexturaGL.idTexturaGL;
						NBGestorGL::deleteTextures(1, &idTextura);
					}
					atlasTex->propsTexturaGL.idTexturaGL = 0;
					//Volver a crear el atlasGL
					{
						const GLenum formatoGlLocal		= NBGestorTexturas::formatoGlLocal(colorDestino); NBASSERT(formatoGlLocal != 0)
						const GLenum formatoGlDriver	= NBGestorTexturas::formatoGlDriver(colorDestino); NBASSERT(formatoGlDriver != 0)
						const GLenum tpoDatoGlDriver	= NBGestorTexturas::tipoDatoGlDriver(colorDestino); NBASSERT(tpoDatoGlDriver != 0)
						if(formatoGlLocal == 0 || formatoGlDriver == 0 || tpoDatoGlDriver == 0){
							PRINTF_ERROR("no se pudo establecer el formatoGLLocal(%d) formatoGLDriver(%d) o tipoDatoGLDriver(%d) del nuevo atlas\n", formatoGlLocal, formatoGlDriver, tpoDatoGlDriver);
						} else {
							GLuint idTextura;
#							ifdef CONFIG_NB_GESTOR_GL_IMPRIMIR_COMANDOS_GL
							PRINTF_WARNING("REgenerando textura de atlas ambito(%d) tam(%d x %d) color(%d).\n", (SI32)iAmb, (SI32)anchoAtlas, (SI32)altoAtlas, (SI32)colorDestino);
#							endif
							NBGestorGL::genTextures(1, &idTextura, &atlasTex->propsTexturaGL);
							NBGestorGL::bindTexture(0, idTextura);
							NBGestorGL::configurarTextura(modoPintado, false, &atlasTex->propsTexturaGL);
							NBASSERT((tipoAlmacenamiento == ENTexturaTipoAlmacenamientoGL_AtlasCompartido && (modoPintado == ENTexturaModoPintado_Imagen_Suavizada || modoPintado == ENTexturaModoPintado_Imagen_Precisa)) ||
									 (tipoAlmacenamiento == ENTexturaTipoAlmacenamientoGL_AtlasUnico && (modoPintado == ENTexturaModoPintado_Patron_Suavizado || modoPintado == ENTexturaModoPintado_Patron_Preciso)))
							/*Formatos: GL_ALPHA, GL_RGB, GL_RGBA, GL_LUMINANCE (gris), GL_LUMINANCE_ALPHA (gris+alpha)*/
							/*Tipos: GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT_5_6_5, GL_UNSIGNED_SHORT_4_4_4_4, GL_UNSIGNED_SHORT_5_5_5_1*/
							NBGestorGL::texImage2D(GL_TEXTURE_2D, 0, formatoGlLocal, anchoAtlas, altoAtlas, 0, formatoGlDriver, tpoDatoGlDriver, NULL, &atlasTex->propsTexturaGL);
							_texsChangesCount++;
							NBASSERT(NBGestorGL::isTexture(idTextura))
							//Registrar nuevo Atlas para el Atlas-Textura-GL
							atlasTex->colorFormato			= colorDestino;
							atlasTex->tipoAlmacenamientoGL	= tipoAlmacenamiento;
							atlasTex->tipoUsoGL				= tipoUso;
							atlasTex->modoPintado			= modoPintado;
							NBASSERT(atlasTex->mapaAtlas != NULL)
							NBASSERT(atlasTex->texturasEnMapa != NULL)
							//Verificar valores
#							ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
							if(atlasTex->mapaAtlas != NULL){
								const NBTamanoP<UI16> tamAtlas = atlasTex->mapaAtlas->tamano();
								if(tamAtlas.ancho != atlasTex->propsTexturaGL.anchoGL || tamAtlas.alto != atlasTex->propsTexturaGL.altoGL){
									PRINTF_ERROR("Atlas tamanos difieren: atlasOrg(%d, %d) atlasGl(%d, %d).\n", (SI32)tamAtlas.ancho, (SI32)tamAtlas.alto, (SI32)atlasTex->propsTexturaGL.anchoGL, (SI32)atlasTex->propsTexturaGL.altoGL);
								}
								NBASSERT(tamAtlas.ancho == atlasTex->propsTexturaGL.anchoGL)
								NBASSERT(tamAtlas.alto == atlasTex->propsTexturaGL.altoGL)
							}
#							endif
							//
							NBASSERT(atlasTex->propsTexturaGL.idTexturaGL == idTextura)
						}
					}
					PRINTF_INFO("Senal-6.\n");
					//Actualizar referencia en texturas
					{
						SI32 i; const SI32 conteo = texturas->conteo;
						for(i = 0; i < conteo; i++){
							PRINTF_INFO("Senal-i%d de %d.\n", i, conteo);
							STGestorTexTextura* tex = texturas->elem(i);
							//Actualizar
							NBASSERT(tex->tipoAlmacenamientoGL == atlasTex->tipoAlmacenamientoGL)
							NBASSERT(tex->tipoUsoGL == atlasTex->tipoUsoGL)
							NBASSERT(tex->tipoPintado == atlasTex->modoPintado)
							tex->textura->relinkearTextura(atlasTex->propsTexturaGL.idTexturaGL);
							//Actualizar referencias en subtexturas
							if(tex->subTexturas != NULL){
								SI32 i; const SI32 conteo = tex->subTexturas->conteo;
								for(i = 0; i < conteo; i++){
									AUTextura* subTex = (AUTextura*)tex->subTexturas->elem(i);
									subTex->relinkearTextura(atlasTex->propsTexturaGL.idTexturaGL);
								}
							}
						}
					}
					PRINTF_INFO("Senal-7.\n");
				}
			}
		}
	}
	PRINTF_INFO("Senal-Fin.\n");
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

void NBGestorTexturas::privRegenerarAtlasesFuentesTexturas(const bool elimTexturasGLAnteriores){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTexturas::privRegenerarAtlasesFuentesTexturas")
	NBASSERT(_gestorInicializado)
	if(_fuentesTexturas != NULL){
		UI32 iAtlas, iAtlasConteo = _fuentesTexturas->conteo;
		for(iAtlas = 0; iAtlas < iAtlasConteo; iAtlas++){
			STGestorTexFuente* atlasFuente = _fuentesTexturas->elemPtr(iAtlas);
			AUFuenteTextura* font = atlasFuente->texturaFuente;
			STNBFontBitmaps* fontDef = font->fuenteDef();
			AUArregloNativoP<STGestorTexFuenteTex>* texturas = atlasFuente->texturas;
			{
				//Now sync bitmaps-to-GL
				UI32 i = 0;
				do {
					STNBFontBitmap bmp = NBFontBitmaps_getBitmapAtIndex(fontDef, i);
					if(bmp.bitmap == NULL){
						break;
					} else {
						const STNBBitmapProps bmpProps = NBBitmap_getProps(bmp.bitmap);
						const BYTE* bmpData = NBBitmap_getData(bmp.bitmap);
						NBASSERT(bmpProps.size.width > 0 && bmpProps.size.height > 0);
						if(i < texturas->conteo){
							//Reload texture
							STGestorTexFuenteTex* datTex = texturas->elemPtr(i);
							//Eliminar el atlasGL actual
							if(elimTexturasGLAnteriores){
								GLuint idTextura = datTex->propsTexturaGL.idTexturaGL;
								NBGestorGL::deleteTextures(1, &idTextura);
							}
							datTex->propsTexturaGL.idTexturaGL	= 0;
							//Volver a generar atlasGL
							//AUMapaBits* mapaBitsHaciaGL		= datAtlas->mapaDatos;
							const MapaBitsColor colorDestino	= (MapaBitsColor)datTex->colorFormato;
							const GLenum formatoGlLocal			= NBGestorTexturas::formatoGlLocal(colorDestino); NBASSERT(formatoGlLocal != 0)
							const GLenum formatoGlDriver		= NBGestorTexturas::formatoGlDriver(colorDestino); NBASSERT(formatoGlDriver != 0)
							const GLenum tpoDatoGlDriver		= NBGestorTexturas::tipoDatoGlDriver(colorDestino); NBASSERT(tpoDatoGlDriver != 0)
							if(formatoGlLocal == 0 || formatoGlDriver == 0 || tpoDatoGlDriver == 0){
								PRINTF_ERROR("no se pudo establecer el formatoGLLocal(%d) formatoGLDriver(%d) o tipoDatoGLDriver(%d) del nuevo atlas\n", formatoGlLocal, formatoGlDriver, tpoDatoGlDriver);
							} else {
								GLuint idTextura;
								//STFuenteMapaBitsAtlas* fuenteAtlas = atlasFuente->texturaFuente->fuenteDef()->mapasAtlas()->elemPtr(datTex->iAtlasEnFuente);
								NBTamanoI tamanoPNG; NBTAMANO_ESTABLECER(tamanoPNG, datTex->propsTexturaGL.anchoGL, datTex->propsTexturaGL.altoGL)
								NBASSERT(datTex->propsTexturaGL.anchoGL == datTex->propsTexturaGL.altoGL)
#								ifdef CONFIG_NB_GESTOR_GL_IMPRIMIR_COMANDOS_GL
								{
									AUFuenteTextura* fuenteTextura	= atlasFuente->texturaFuente;
									PRINTF_WARNING("RE-generando textura de fuente: '%s%s%s'::%f tam(%d, %d) color(%d).\n", fuenteTextura->familia(), (fuenteTextura->esNegrilla() ? "_n" : ""), (fuenteTextura->esCursiva() ? "_c" : ""), fuenteTextura->tamanoEscena(), (SI32)tamanoPNG.ancho, (SI32)tamanoPNG.alto, (SI32)colorDestino);
								}
#								endif
								NBGestorGL::genTextures(1, &idTextura, &datTex->propsTexturaGL);
								NBGestorGL::bindTexture(0, idTextura);
								NBGestorGL::configurarTextura(ENTexturaModoPintado_Imagen_Precisa, false, &datTex->propsTexturaGL);
								NBGestorGL::texImage2D(GL_TEXTURE_2D, 0, formatoGlLocal, tamanoPNG.ancho, tamanoPNG.alto, 0, formatoGlDriver, tpoDatoGlDriver, bmpData, &datTex->propsTexturaGL);
								_texsChangesCount++;
								NBASSERT(NBGestorGL::isTexture(idTextura))
								atlasFuente->texturaFuente->ligarAtlasConTexturaGl(datTex->iAtlasEnFuente, idTextura);
							}
						} else {
							//First time loading texture
							MapaBitsColor colorDestino = COLOR_NO_DEFINIDO;
							switch (bmpProps.color) {
								case ENNBBitmapColor_ALPHA8: colorDestino = COLOR_ALPHA8; break;
								case ENNBBitmapColor_GRIS8: colorDestino = COLOR_GRIS8; break;
								case ENNBBitmapColor_GRISALPHA8: colorDestino = COLOR_GRISALPHA8; break;
								case ENNBBitmapColor_RGB8: colorDestino = COLOR_RGB8; break;
								case ENNBBitmapColor_RGBA8: colorDestino = COLOR_RGBA8; break;
								case ENNBBitmapColor_ARGB8: colorDestino = COLOR_ARGB8; break;
								case ENNBBitmapColor_BGRA8: colorDestino = COLOR_BGRA8; break;
#								ifdef NB_CONFIG_INCLUDE_ASSERTS
								default:
									NBASSERT(FALSE) 
									break;
#								endif
							}
							GLenum formatoGlLocal		= NBGestorTexturas::formatoGlLocal(colorDestino); NBASSERT(formatoGlLocal != 0)
							GLenum formatoGlDriver		= NBGestorTexturas::formatoGlDriver(colorDestino); NBASSERT(formatoGlDriver != 0)
							GLenum tpoDatoGlDriver		= NBGestorTexturas::tipoDatoGlDriver(colorDestino); NBASSERT(tpoDatoGlDriver != 0)
							if(formatoGlLocal == 0 || formatoGlDriver == 0 || tpoDatoGlDriver == 0){
								PRINTF_ERROR("no se pudo establecer el formatoGLLocal(%d) formatoGLDriver(%d) o tipoDatoGLDriver(%d) del nuevo atlas\n", formatoGlLocal, formatoGlDriver, tpoDatoGlDriver);
							} else {
								GLuint idTextura;
								STGestorTexFuenteTex nuevaFuenteTex;
#								ifdef CONFIG_NB_GESTOR_GL_IMPRIMIR_COMANDOS_GL
								//PRINTF_WARNING("Generando textura de fuente: '%s%s%s'::%f tam(%d, %d) color(%d).\n", nombreFuente, (negrilla ? "_n" : ""), (cursiva ? "_c" : ""), tamanoFuente, (SI32)bmpProps.size.width, (SI32)bmpProps.size.height, (SI32)colorDestino);
#								endif
								NBGestorGL::genTextures(1, &idTextura, &nuevaFuenteTex.propsTexturaGL);
								NBGestorGL::bindTexture(0, idTextura);
								NBGestorGL::configurarTextura(ENTexturaModoPintado_Imagen_Precisa, false, &nuevaFuenteTex.propsTexturaGL);
								NBGestorGL::texImage2D(GL_TEXTURE_2D, 0, formatoGlLocal, bmpProps.size.width, bmpProps.size.height, 0, formatoGlDriver, tpoDatoGlDriver, bmpData, &nuevaFuenteTex.propsTexturaGL);
								_texsChangesCount++;
								NBASSERT(NBGestorGL::isTexture(idTextura))
								nuevaFuenteTex.iAtlasEnFuente	= i;
								nuevaFuenteTex.colorFormato		= colorDestino;
								//Nota: las texturas de texto no estan definidas en base a una resolucion (X4, X2, X1, etc..)
								//es decir, no se escalan hacia HD (ya son HD)
								atlasFuente->texturas->agregarElemento(nuevaFuenteTex);
								atlasFuente->texturaFuente->ligarAtlasConTexturaGl(nuevaFuenteTex.iAtlasEnFuente, idTextura);
							}
						}
					}
					i++;
				} while(TRUE);
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

const char*	NBGestorTexturas::nombreRecursoTextura(AUTextura* textura){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTexturas::nombreRecursoTextura")
	NBASSERT(_gestorInicializado)
	const char* nombre = NULL;
	UI8 iAmb;
	for(iAmb = 0; iAmb < GESTOR_TEXTURAS_MAX_AMBITOS_TEXTURAS && nombre == NULL; iAmb++){
		STGestorTexAmbito* ambTex = &_ambitosAtlases[iAmb];
		//Buscar en las cargadas
		UI32 iAtlas, iAtlasConteo = ambTex->atlasesTexturas->conteo;
		for(iAtlas=0; iAtlas<iAtlasConteo && nombre == NULL; iAtlas++){
			STGestorTexAtlas* atlasTex = ambTex->atlasesTexturas->elem(iAtlas);
			AUArregloNativoMutableP<STGestorTexTextura*>* texturas = atlasTex->texturasEnMapa;
			UI32 iTextura, iTexturaConteo = texturas->conteo;
			for(iTextura=0; iTextura<iTexturaConteo && nombre == NULL; iTextura++){
				STGestorTexTextura* datosTextura = texturas->elem(iTextura);
				if(datosTextura->textura == textura){
					nombre = datosTextura->nombreTextura->str();
				}
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return nombre;
}

STGestorTexTextura* NBGestorTexturas::privTexturaPorNombre(UI8 iAmbitoTexturas, const char* filepath, ENTexturaTipoAlmacenamientoGL tipoAlmacenamiento){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTexturas::privTexturaPorNombre")
	STGestorTexTextura* r = NULL;
	NBASSERT(_gestorInicializado)
	NBASSERT(iAmbitoTexturas < GESTOR_TEXTURAS_MAX_AMBITOS_TEXTURAS)
	NBASSERT(_ambitosAtlases[iAmbitoTexturas].ambitoReservado)
	if(filepath[0] != '\0'){ //Si no es cadena vacia
		STGestorTexAmbito* ambTex = &_ambitosAtlases[iAmbitoTexturas];
		//Buscar en las cargadas
		UI32 iAtlas, iAtlasConteo = ambTex->atlasesTexturas->conteo;
		for(iAtlas=0; iAtlas<iAtlasConteo && r == NULL; iAtlas++){
			STGestorTexAtlas* atlasTex = ambTex->atlasesTexturas->elem(iAtlas);
			if(atlasTex->tipoAlmacenamientoGL == tipoAlmacenamiento){
				AUArregloNativoMutableP<STGestorTexTextura*>* texturas = atlasTex->texturasEnMapa;
				UI32 iTextura, iTexturaConteo = texturas->conteo;
				for(iTextura=0; iTextura<iTexturaConteo; iTextura++){
					STGestorTexTextura* tex = texturas->elem(iTextura);
					if(tex->tipoAlmacenamientoGL == tipoAlmacenamiento && tex->nombreTextura->esIgual(filepath)){
						r = tex;
						break;
					}
				}
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return r;
}

//ToDo: remove
/*STGestorTexTextura* NBGestorTexturas::privTexturaPorFirmaSHA1(UI8 iAmbitoTexturas, const STNBSha1Hash &firmaSHA1, ENTexturaTipoAlmacenamientoGL tipoAlmacenamiento){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTexturas::privTexturaPorFirmaSHA1")
	STGestorTexTextura* r = NULL;
	NBASSERT(_gestorInicializado)
	NBASSERT(iAmbitoTexturas < GESTOR_TEXTURAS_MAX_AMBITOS_TEXTURAS)
	NBASSERT(_ambitosAtlases[iAmbitoTexturas].ambitoReservado)
	if(!SHA1_HASH_IS_EMPTY(firmaSHA1)){ //Si no es firma vacia
		STGestorTexAmbito* ambTex = &_ambitosAtlases[iAmbitoTexturas];
		//Buscar en las cargadas
		UI32 iAtlas, iAtlasConteo = ambTex->atlasesTexturas->conteo;
		for(iAtlas=0; iAtlas<iAtlasConteo && r == NULL; iAtlas++){
			STGestorTexAtlas* atlasTex = ambTex->atlasesTexturas->elem(iAtlas);
			if(atlasTex->tipoAlmacenamientoGL == tipoAlmacenamiento){
				AUArregloNativoMutableP<STGestorTexTextura*>* texturas = atlasTex->texturasEnMapa;
				UI32 iTextura, iTexturaConteo = texturas->conteo;
				for(iTextura=0; iTextura<iTexturaConteo; iTextura++){
					STGestorTexTextura* tex = texturas->elem(iTextura);
					if(tex->tipoAlmacenamientoGL == tipoAlmacenamiento && SHA1_HASH_ARE_EQUALS(tex->firmaTextura, firmaSHA1)){
						r = tex;
						break;
					}
				}
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return r;
}*/

bool NBGestorTexturas::privReservaAreaEnAtlas(UI8 iAmbitoTexturas, const char* nombreTextura, const SI32 nomTamano, UI32 ancho, UI32 alto, MapaBitsColor colorDestino, ENTexturaTipoUsoGL tipoUsoTextura, ENTexturaModoPintado modoPintado, SI32* guardarIndiceAtlasEn, NBRectanguloUI16* guardarAreaEn){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTexturas::privReservaAreaEnAtlas")
	NBASSERT(_gestorInicializado)
	NBASSERT(iAmbitoTexturas < GESTOR_TEXTURAS_MAX_AMBITOS_TEXTURAS)
	NBASSERT(_ambitosAtlases[iAmbitoTexturas].ambitoReservado)
	bool encontrado = false;
	STGestorTexAmbito* ambTex = &_ambitosAtlases[iAmbitoTexturas];
	UI32 iAtlas, iAtlasConteo = ambTex->atlasesTexturas->conteo;
	for(iAtlas=0; iAtlas<iAtlasConteo; iAtlas++){
		STGestorTexAtlas* atlasTex = ambTex->atlasesTexturas->elem(iAtlas);
		if(atlasTex->mapaAtlas != NULL){
			if(atlasTex->colorFormato == colorDestino && atlasTex->tipoAlmacenamientoGL == ENTexturaTipoAlmacenamientoGL_AtlasCompartido && atlasTex->tipoUsoGL == tipoUsoTextura && atlasTex->modoPintado == modoPintado){
				if(atlasTex->mapaAtlas->hayEspacioDisponible(ancho, alto, NBGESTORTEXTURAS_PIX_SEPARACION_ENTRE_TEXTURAS, NBGESTORTEXTURAS_PIX_SEPARACION_ENTRE_TEXTURAS)){
					*guardarIndiceAtlasEn	= iAtlas;
					*guardarAreaEn			= atlasTex->mapaAtlas->reservaEspacioConNombre(ancho, alto, NBGESTORTEXTURAS_PIX_SEPARACION_ENTRE_TEXTURAS, NBGESTORTEXTURAS_PIX_SEPARACION_ENTRE_TEXTURAS, nombreTextura, nomTamano);
					encontrado 				= true;
					break;
				}
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return encontrado;
}

bool NBGestorTexturas::privReservaAreaEnAtlasDePrecarga(UI8 iAmbitoTexturas, SI32* guardarIndiceAtlasEn){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTexturas::privReservaAreaEnAtlasDePrecarga")
	NBASSERT(_gestorInicializado)
	NBASSERT(iAmbitoTexturas < GESTOR_TEXTURAS_MAX_AMBITOS_TEXTURAS)
	NBASSERT(_ambitosAtlases[iAmbitoTexturas].ambitoReservado)
	bool encontrado = false;
	STGestorTexAmbito* ambTex = &_ambitosAtlases[iAmbitoTexturas];
	UI32 iAtlas, iAtlasConteo = ambTex->atlasesTexturas->conteo;
	for(iAtlas=0; iAtlas<iAtlasConteo; iAtlas++){
		STGestorTexAtlas* atlasTex = ambTex->atlasesTexturas->elem(iAtlas);
		if(atlasTex->mapaAtlas == NULL){
			*guardarIndiceAtlasEn	= iAtlas;
			encontrado 				= true;
			break;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return encontrado;
}


UI8 NBGestorTexturas::indiceAmbitoTexturasActual(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTexturas::indiceAmbitoTexturasActual")
	NBASSERT(_gestorInicializado)
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return _indiceAmbitoActivo;
}

bool NBGestorTexturas::activarAmbitoTexturas(UI8 iAmbito){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTexturas::activarAmbitoTexturas")
	bool exito = false;
	NBASSERT(_gestorInicializado)
	NBASSERT(iAmbito < GESTOR_TEXTURAS_MAX_AMBITOS_TEXTURAS)
	NBASSERT(_ambitosAtlases[iAmbito].ambitoReservado);
	if(_ambitosAtlases[iAmbito].ambitoReservado){
		_indiceAmbitoActivo = iAmbito;
		exito = true;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return exito;
}

void NBGestorTexturas::establecerTamanosTexturasDeAmbito(UI8 iAmbito, UI16 tamanosTexturasPrincipales, UI16 tamanosTexturasSecundarias){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTexturas::establecerTamanosTexturasDeAmbito")
	NBASSERT(_gestorInicializado)
	NBASSERT(iAmbito < GESTOR_TEXTURAS_MAX_AMBITOS_TEXTURAS)
	NBASSERT(_ambitosAtlases[iAmbito].ambitoReservado);
	if(_ambitosAtlases[iAmbito].ambitoReservado){
		_ambitosAtlases[iAmbito].tamanosTexturasPrincipales	= tamanosTexturasPrincipales;
		_ambitosAtlases[iAmbito].tamanosTexturasSecundarias	= tamanosTexturasSecundarias;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

UI8 NBGestorTexturas::reservarAmbitoTexturas(UI16 tamTexturasPrincipales, UI16 tamTexturasSecundarias){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTexturas::reservarAmbitoTexturas")
	NBASSERT(_gestorInicializado)
	UI8 iAmbitoRetorno = 0;
	if(NBGestorTexturas::dimensionTexturaEsMultiploDe2Valido(tamTexturasPrincipales) && NBGestorTexturas::dimensionTexturaEsMultiploDe2Valido(tamTexturasSecundarias)){
		UI8 iAmb;
		for(iAmb = 0; iAmb < GESTOR_TEXTURAS_MAX_AMBITOS_TEXTURAS && iAmbitoRetorno == 0; iAmb++){
			STGestorTexAmbito* ambTex = &_ambitosAtlases[iAmb];
			if(!ambTex->ambitoReservado){
				ambTex->ambitoReservado				= true;
				ambTex->tamanosTexturasPrincipales	= tamTexturasPrincipales;
				ambTex->tamanosTexturasSecundarias	= tamTexturasSecundarias;
				iAmbitoRetorno = iAmb;
			}
		}
	}
	NBASSERT(iAmbitoRetorno != 0); //Se acabaron los ambitos o los tamanos para texturas no son validos
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return iAmbitoRetorno;
}


void NBGestorTexturas::liberarAmbitoTexturas(UI8 iAmbito){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTexturas::liberarAmbitoTexturas")
	NBASSERT(_gestorInicializado)
	NBASSERT(!_bloqueado)
	NBASSERT(iAmbito < GESTOR_TEXTURAS_MAX_AMBITOS_TEXTURAS)
	NBASSERT(_ambitosAtlases[iAmbito].ambitoReservado);
	NBASSERT(iAmbito != 0); //No se puede liberar el ambito por defecto
	//NBASSERT(ambTex->atlasesTexturas->conteo == 0) //El ambito a liberar aun no esta vacio
	if(iAmbito != 0){
		STGestorTexAmbito* ambTex = &_ambitosAtlases[iAmbito];
		ambTex->ambitoReservado = false;
		if(iAmbito == _indiceAmbitoActivo) {
			_indiceAmbitoActivo = 0;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

void NBGestorTexturas::rutaArchivoTextura(AUCadenaMutable8* dst, UI8* dstEscalaBase2, float* dstEscalaParaHD, const char* nombre, const ENGestorTexturaOrigen origenTextura){
	//Get subtexture name separation ('file.png#face')
	const SI32 nomTam = AUCadena8::tamano(nombre);
	SI32 nomPosSepara = AUCadena8::indiceDe(nombre, '#', 0);
	if(nomPosSepara == -1){
		nomPosSepara = nomTam;
	}
	//
	{
		STNBString str;
		NBString_init(&str);
		{
			const char* path = NBGestorTexturas::privRutaArchivoTextura(&str, dstEscalaBase2, dstEscalaParaHD, nombre, nomPosSepara, origenTextura);
			if(dst != NULL) dst->establecer(path);
		}
		NBString_release(&str);
	}
}

AUTextura* NBGestorTexturas::texturaDesdeArchivo(const char* nombre, const ENGestorTexturaOrigen origenTextura, const UI8 pEscalaBase2, const float pEscalaParaHD){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTexturas::texturaDesdeArchivo")
	NBASSERT(_gestorInicializado)
	NBASSERT(!_bloqueado)
	AUTextura* r = NULL;
	//Get subtexture name separation ('file.png#face')
	const SI32 nomTam = AUCadena8::tamano(nombre);
	SI32 nomPosSepara = AUCadena8::indiceDe(nombre, '#', 0);
	if(nomPosSepara == -1){
		nomPosSepara = nomTam;
	}
	//
	if(nomPosSepara > 4){
		UI8 escalaBase2		= pEscalaBase2;
		float escalaParaHD	= pEscalaParaHD;
		STNBString buffPath;
		NBString_init(&buffPath);
		{
			const char* filepath = NBGestorTexturas::privRutaArchivoTextura(&buffPath, &escalaBase2, &escalaParaHD, nombre, nomPosSepara, origenTextura);
			STGestorTexTextura* tex = NULL;
			const char* nomUlt4 = &nombre[nomPosSepara - 4];
			if(nomUlt4[0] == '.' && (nomUlt4[1] == 'p' || nomUlt4[1] == 'P') && (nomUlt4[2] == 'n' || nomUlt4[2] == 'N') && (nomUlt4[3] == 'g' || nomUlt4[3] == 'G')){
				tex = NBGestorTexturas::privTexturaDesdeArchivoPNG(_indiceAmbitoActivo, origenTextura, escalaBase2, escalaParaHD, filepath, true, ENTexturaTipoAlmacenamientoGL_AtlasCompartido, ENTExturaTipoUsoGL_Lectura, ENTexturaModoPintado_Imagen_Suavizada, ENTexturaMipMap_Habilitado, NULL);
			} else if(nomUlt4[0] == '.' && (nomUlt4[1] == 'j' || nomUlt4[1] == 'J') && (nomUlt4[2] == 'p' || nomUlt4[2] == 'P') && (nomUlt4[3] == 'g' || nomUlt4[3] == 'G')){
				tex = NBGestorTexturas::privTexturaDesdeArchivoJPEG(_indiceAmbitoActivo, origenTextura, escalaBase2, escalaParaHD, filepath, true, ENTexturaTipoAlmacenamientoGL_AtlasCompartido, ENTExturaTipoUsoGL_Lectura, ENTexturaModoPintado_Imagen_Suavizada, ENTexturaMipMap_Habilitado, NULL);
			} else if(nomUlt4[-1] == '.' && (nomUlt4[0] == 'j' || nomUlt4[0] == 'J') && (nomUlt4[1] == 'p' || nomUlt4[1] == 'P') && (nomUlt4[2] == 'e' || nomUlt4[2] == 'E') && (nomUlt4[3] == 'g' || nomUlt4[3] == 'G')){
				tex = NBGestorTexturas::privTexturaDesdeArchivoJPEG(_indiceAmbitoActivo, origenTextura, escalaBase2, escalaParaHD, filepath, true, ENTexturaTipoAlmacenamientoGL_AtlasCompartido, ENTExturaTipoUsoGL_Lectura, ENTexturaModoPintado_Imagen_Suavizada, ENTexturaMipMap_Habilitado, NULL);
			}
			if(tex != NULL){
				if(nomPosSepara >= nomTam){
					r = tex->textura;
				} else if(tex->subTexturasMapa != NULL && tex->subTexturas != NULL){
					//Search subtexture
					const SI32 iSubtex = tex->subTexturasMapa->indiceAreaPorNombre(&nombre[nomPosSepara + 1]);
					if(iSubtex >= 0 && iSubtex < tex->subTexturas->conteo){
						r = (AUTextura*)tex->subTexturas->elem(iSubtex);
					}
					//PRINTF_INFO("Subtexture '%s' at index %d.\n", &nombre[nomPosSepara + 1], iSubtex);
				}
			}
		}
		NBString_release(&buffPath);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return r;
}

AUTextura* NBGestorTexturas::texturaDesdeArchivoPNG(const char* nombrePNG, const ENGestorTexturaOrigen origenTextura, const UI8 pEscalaBase2, const float pEscalaParaHD){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTexturas::texturaDesdeArchivoPNG")
	NBASSERT(_gestorInicializado)
	NBASSERT(!_bloqueado)
	AUTextura* r = NULL;
	//Get subtexture name separation ('file.png#face')
	const SI32 nomTam = AUCadena8::tamano(nombrePNG);
	SI32 nomPosSepara = AUCadena8::indiceDe(nombrePNG, '#', 0);
	if(nomPosSepara == -1){
		nomPosSepara = nomTam;
	}
	//
	{
		UI8 escalaBase2		= pEscalaBase2;
		float escalaParaHD	= pEscalaParaHD;
		STNBString buffPath;
		NBString_init(&buffPath);
		{
			const char* filepath = NBGestorTexturas::privRutaArchivoTextura(&buffPath, &escalaBase2, &escalaParaHD, nombrePNG, nomPosSepara, origenTextura);
			STGestorTexTextura* tex = NBGestorTexturas::privTexturaDesdeArchivoPNG(_indiceAmbitoActivo, origenTextura, escalaBase2, escalaParaHD, filepath, true, ENTexturaTipoAlmacenamientoGL_AtlasCompartido, ENTExturaTipoUsoGL_Lectura, ENTexturaModoPintado_Imagen_Suavizada, ENTexturaMipMap_Habilitado, NULL);
			if(tex != NULL){
				if(nomPosSepara >= nomTam){
					r = tex->textura;
				} else if(tex->subTexturasMapa != NULL && tex->subTexturas != NULL){
					//Search subtexture
					const SI32 iSubtex = tex->subTexturasMapa->indiceAreaPorNombre(&nombrePNG[nomPosSepara + 1]);
					if(iSubtex >= 0 && iSubtex < tex->subTexturas->conteo){
						r = (AUTextura*)tex->subTexturas->elem(iSubtex);
					}
					//PRINTF_INFO("Subtexture '%s' at index %d.\n", &nombrePNG[nomPosSepara + 1], iSubtex);
				}
			}
		}
		NBString_release(&buffPath);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return r;
}

AUTextura* NBGestorTexturas::texturaDesdeArchivoJPEG(const char* nombreJPEG, const ENGestorTexturaOrigen origenTextura, const UI8 pEscalaBase2, const float pEscalaParaHD){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTexturas::texturaDesdeArchivoJPEG")
	NBASSERT(_gestorInicializado)
	NBASSERT(!_bloqueado)
	AUTextura* r = NULL;
	//Get subtexture name separation ('file.png#face')
	const SI32 nomTam = AUCadena8::tamano(nombreJPEG);
	SI32 nomPosSepara = AUCadena8::indiceDe(nombreJPEG, '#', 0);
	if(nomPosSepara == -1){
		nomPosSepara = nomTam;
	}
	//
	{
		UI8 escalaBase2		= pEscalaBase2;
		float escalaParaHD	= pEscalaParaHD;
		STNBString buffPath;
		NBString_init(&buffPath);
		{
			const char* filepath = NBGestorTexturas::privRutaArchivoTextura(&buffPath, &escalaBase2, &escalaParaHD, nombreJPEG, nomPosSepara, origenTextura);
			STGestorTexTextura* tex = NBGestorTexturas::privTexturaDesdeArchivoJPEG(_indiceAmbitoActivo, origenTextura, escalaBase2, escalaParaHD, filepath, true, ENTexturaTipoAlmacenamientoGL_AtlasCompartido, ENTExturaTipoUsoGL_Lectura, ENTexturaModoPintado_Imagen_Suavizada, ENTexturaMipMap_Habilitado, NULL);
			if(tex != NULL){
				if(nomPosSepara >= nomTam){
					r = tex->textura;
				} else if(tex->subTexturasMapa != NULL && tex->subTexturas != NULL){
					//Search subtexture
					const SI32 iSubtex = tex->subTexturasMapa->indiceAreaPorNombre(&nombreJPEG[nomPosSepara + 1]);
					if(iSubtex >= 0 && iSubtex < tex->subTexturas->conteo){
						r = (AUTextura*)tex->subTexturas->elem(iSubtex);
					}
					//PRINTF_INFO("Subtexture '%s' at index %d.\n", &nombreJPEG[nomPosSepara + 1], iSubtex);
				}
			}
		}
		NBString_release(&buffPath);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return r;
}

AUTextura* NBGestorTexturas::texturaDesdeMapaBits(AUMapaBits* mapaBitsACargar, const char* guardarConNombre, const ENTexturaModoPintado modoPintado, const ENTexturaMipMap habilitarMipMap, const ENTexturaOrdenV ordenVTextura, const UI8 escalaBase2, const float escalaParaHD){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTexturas::texturaDesdeMapaBits")
	NBASSERT(_gestorInicializado)
	NBASSERT(!_bloqueado)
	AUTextura* r = NULL;
	STGestorTexTextura* tex = NBGestorTexturas::privTexturaDesdeMapaBits(_indiceAmbitoActivo, mapaBitsACargar, guardarConNombre, ENTexturaTipoAlmacenamientoGL_AtlasCompartido, ENTExturaTipoUsoGL_Lectura, modoPintado, habilitarMipMap, ordenVTextura, NULL, escalaBase2, escalaParaHD);
	if(tex != NULL){
		r = tex->textura;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return r;
}


AUTextura* NBGestorTexturas::texturaDesdeDatos(const MapaDeBitsDesciptor* props, const void* data, const ENTexturaTipoAlmacenamientoGL tipoAlmacenamiento, const ENTexturaTipoUsoGL tipoUso, const ENTexturaModoPintado modoPintado, const ENTexturaMipMap habilitarMipMap, const ENTexturaOrdenV ordenVTextura, const UI8 escalaBase2, const float escalaParaHD){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTexturas::texturaDesdeDatos")
	NBASSERT(!_bloqueado)
	AUTextura* r = NULL;
	STGestorTexTextura* tex = NBGestorTexturas::privTexturaDesdeDatos(_indiceAmbitoActivo, props->ancho, props->alto, data, (MapaBitsColor)props->color, ENGestorTexturaOrigen_datos, escalaBase2, escalaParaHD, "", tipoAlmacenamiento, tipoUso, modoPintado, habilitarMipMap, ordenVTextura, NULL);
	if(tex != NULL){
		r = tex->textura;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return r;
}

AUTextura* NBGestorTexturas::texturaDesdeAreaVacia(SI32 ancho, SI32 alto, MapaBitsColor color, const ENTexturaTipoAlmacenamientoGL tipoAlmacenamiento, const ENTexturaTipoUsoGL tipoUso, const ENTexturaModoPintado modoPintado, const ENTexturaMipMap habilitarMipMap, const ENTexturaOrdenV ordenVTextura, const UI8 escalaBase2, const float escalaParaHD){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTexturas::texturaDesdeAreaVacia")
	NBASSERT(!_bloqueado)
	AUTextura* r = NULL;
	STGestorTexTextura* tex = NBGestorTexturas::privTexturaDesdeDatos(_indiceAmbitoActivo, ancho, alto, NULL, color, ENGestorTexturaOrigen_datos, escalaBase2, escalaParaHD, "", tipoAlmacenamiento, tipoUso, modoPintado, habilitarMipMap, ordenVTextura, NULL);
	if(tex != NULL){
		r = tex->textura;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return r;
}

//

AUTextura* NBGestorTexturas::texturaPatronDesdeArchivoPNG(const char* nombrePNG, const ENGestorTexturaOrigen origenTextura, const UI8 pEscalaBase2, const float pEscalaParaHD){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTexturas::texturaPatronDesdeArchivoPNG")
	NBASSERT(!_bloqueado)
	AUTextura* r = NULL;
	//Get subtexture name separation ('file.png#face')
	const SI32 nomTam = AUCadena8::tamano(nombrePNG);
	SI32 nomPosSepara = AUCadena8::indiceDe(nombrePNG, '#', 0);
	if(nomPosSepara == -1){
		nomPosSepara = nomTam;
	}
	//
	{
		UI8 escalaBase2		= pEscalaBase2;
		float escalaParaHD	= pEscalaParaHD;
		STNBString buffPath;
		NBString_init(&buffPath);
		{
			const char* filePath = NBGestorTexturas::privRutaArchivoTextura(&buffPath, &escalaBase2, &escalaParaHD, nombrePNG, nomPosSepara, origenTextura);
			STGestorTexTextura* tex = NBGestorTexturas::privTexturaDesdeArchivoPNG(_indiceAmbitoActivo, origenTextura, escalaBase2, escalaParaHD, filePath, true, ENTexturaTipoAlmacenamientoGL_AtlasUnico, ENTExturaTipoUsoGL_Lectura, ENTexturaModoPintado_Patron_Suavizado, ENTexturaMipMap_Habilitado, NULL);
			if(tex != NULL){
				r = tex->textura;
			}
		}
		NBString_release(&buffPath);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return r;
}

AUTextura* NBGestorTexturas::texturaPatronDesdeArchivoJPEG(const char* nombreJPEG, const ENGestorTexturaOrigen origenTextura, const UI8 pEscalaBase2, const float pEscalaParaHD){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTexturas::texturaPatronDesdeArchivoJPEG")
	NBASSERT(!_bloqueado)
	AUTextura* r = NULL;
	//Get subtexture name separation ('file.png#face')
	const SI32 nomTam = AUCadena8::tamano(nombreJPEG);
	SI32 nomPosSepara = AUCadena8::indiceDe(nombreJPEG, '#', 0);
	if(nomPosSepara == -1){
		nomPosSepara = nomTam;
	}
	//
	{
		UI8 escalaBase2		= pEscalaBase2;
		float escalaParaHD	= pEscalaParaHD;
		STNBString buffPath;
		NBString_init(&buffPath);
		{
			const char* filePath = NBGestorTexturas::privRutaArchivoTextura(&buffPath, &escalaBase2, &escalaParaHD, nombreJPEG, nomPosSepara, origenTextura);
			STGestorTexTextura* tex = NBGestorTexturas::privTexturaDesdeArchivoJPEG(_indiceAmbitoActivo, origenTextura, escalaBase2, escalaParaHD, filePath, true, ENTexturaTipoAlmacenamientoGL_AtlasUnico, ENTExturaTipoUsoGL_Lectura, ENTexturaModoPintado_Patron_Suavizado, ENTexturaMipMap_Habilitado, NULL);
			if(tex != NULL){
				r = tex->textura;
			}
		}
		NBString_release(&buffPath);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return r;
}

AUTextura* NBGestorTexturas::texturaPatronDesdeMapaBits(AUMapaBits* mapaBitsACargar, const char* guardarConNombre, ENTexturaOrdenV ordenVTextura, const UI8 escalaBase2, const float escalaParaHD){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTexturas::texturaPatronDesdeMapaBits")
	NBASSERT(!_bloqueado)
	AUTextura* r = NULL;
	STGestorTexTextura* tex = NBGestorTexturas::privTexturaDesdeMapaBits(_indiceAmbitoActivo, mapaBitsACargar, guardarConNombre, ENTexturaTipoAlmacenamientoGL_AtlasUnico, ENTExturaTipoUsoGL_Lectura, ENTexturaModoPintado_Patron_Suavizado, ENTexturaMipMap_Habilitado, ordenVTextura, NULL, escalaBase2, escalaParaHD);
	if(tex != NULL){
		r = tex->textura;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return r;
}

bool NBGestorTexturas::texturaEstablecerContenido(const AUTextura* textura, const AUMapaBits* mapaBits){
	bool r = false;
	if(textura != NULL && mapaBits != NULL){
		const MapaDeBitsDesciptor props = mapaBits->propiedades();
		r = NBGestorTexturas::texturaEstablecerContenidoData(textura, &props, mapaBits->datos());
	}
	return r;
}

bool NBGestorTexturas::texturaEstablecerContenidoData(const AUTextura* textura, const MapaDeBitsDesciptor* props, const void* data){
	bool r = false;
	if(textura != NULL && props != NULL && data != NULL){
		const NBRectanguloP<UI16> areaEnAtlas = textura->areaEnAtlas();
		NBASSERT(areaEnAtlas.ancho == props->ancho)
		NBASSERT(areaEnAtlas.alto == props->alto)
		if(props->ancho > 0 && props->alto > 0 && areaEnAtlas.ancho == props->ancho && areaEnAtlas.alto == props->alto){
			bool encontrado = false;
			UI8 iAmb; UI32 idTetxuraGL = textura->idTexturaGL;
			for(iAmb = 0; iAmb < GESTOR_TEXTURAS_MAX_AMBITOS_TEXTURAS && !encontrado; iAmb++){
				STGestorTexAmbito* ambTex = &_ambitosAtlases[iAmb];
				if(ambTex->ambitoReservado){
					UI16 iAtlas; const UI16 conteoAtlas = ambTex->atlasesTexturas->conteo;
					for(iAtlas = 0; iAtlas < conteoAtlas && !encontrado; iAtlas++){
						STGestorTexAtlas* atlas = ambTex->atlasesTexturas->elem(iAtlas);
						if(atlas->propsTexturaGL.idTexturaGL == idTetxuraGL){
							UI16 iArea; const UI16 conteoAreas = atlas->texturasEnMapa->conteo;
							for(iArea = 0; iArea < conteoAreas; iArea++){
								const STGestorTexTextura* datosTex = atlas->texturasEnMapa->elem(iArea);
								if(datosTex->textura == textura){
									NBASSERT(atlas->colorFormato == props->color)
									if(atlas->colorFormato == props->color){
										//
										GLenum formatoGlLocal = NBGestorTexturas::formatoGlDriver((MapaBitsColor)props->color); NBASSERT(formatoGlLocal != 0)
										GLenum tpoDatoGlLocal = NBGestorTexturas::tipoDatoGlLocal((MapaBitsColor)props->color); NBASSERT(tpoDatoGlLocal != 0)
										if(formatoGlLocal == 0 || tpoDatoGlLocal == 0){
											PRINTF_ERROR("NBGestorTexturas, el formatoGl(%d) o tpoDatoGlDriver(%d) no pudo definirse\n", formatoGlLocal, tpoDatoGlLocal);
											NBASSERT(false)
										} else {
											NBGestorGL::bindTexture(0, (GLuint)atlas->propsTexturaGL.idTexturaGL);
											//texSubImage2D is prefered over texImage2D because the first only updates pixel data, the second update texture parameters also.
											NBGestorGL::texSubImage2D(GL_TEXTURE_2D, 0, areaEnAtlas.x, areaEnAtlas.y, props->ancho, props->alto, formatoGlLocal, tpoDatoGlLocal, data, &atlas->propsTexturaGL); //GL_UNSIGNED_BYTE
											_texsChangesCount++;
											r = true;
										}
									} else {
										PRINTF_ERROR("El color de textura y mapa de bits no concuerda.\n");
									}
									//
									encontrado = true;
									break;
								}
							}
						}
					}
				}
			}
		}
	}
	return r;
}

void NBGestorTexturas::liberarEspacioTexturaSiSoloTieneUnaReferenciaExterna(AUTextura* textura){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTexturas::liberarEspacioTexturaSiSoloTieneUnaReferenciaExterna")
	NBASSERT(!_bloqueado)
	if(textura->conteoReferencias() > 2){ //1 referencia del gestor y otra referencia externa
		PRINTF_WARNING("NBGestorTexturas: el area-textura que no se puede liberar porque esta retenida por otros objetos\n");
	} else {
		UI32 iAmb;
		for(iAmb = 0; iAmb < GESTOR_TEXTURAS_MAX_AMBITOS_TEXTURAS; iAmb++){
			STGestorTexAmbito* ambTex = &_ambitosAtlases[iAmb];
			UI32 iAtlas, iAtlasConteo = ambTex->atlasesTexturas->conteo;
			for(iAtlas = 0; iAtlas < iAtlasConteo && textura != NULL; iAtlas++){
				STGestorTexAtlas* atlasTex = ambTex->atlasesTexturas->elem(iAtlas);
				AUArregloNativoMutableP<STGestorTexTextura*>* texturas = atlasTex->texturasEnMapa;
				UI32 iTextura, iTexturaConteo = texturas->conteo;
				for(iTextura=0; iTextura<iTexturaConteo && textura != NULL; iTextura++){
					STGestorTexTextura* datosTextura = texturas->elem(iTextura);
					if(datosTextura->textura == textura){
						//Search for used subtextures
						bool algunaSubtexturaEnUso = false;
						if(datosTextura->subTexturas != NULL){
							SI32 i; const SI32 conteo = datosTextura->subTexturas->conteo;
							for(i = 0; i < conteo; i++){
								if(datosTextura->subTexturas->elem(i)->conteoReferencias() != 1){
									algunaSubtexturaEnUso = true;
									break;
								}
							}
						}
						if(!algunaSubtexturaEnUso){
							//PRINTF_INFO("LIBERANDO AREA EN TEXTURA, iAtlas(%d) iTextura(%d) [solicitante]\n", iAtlas, iTextura);
							//NBHILO_MUTEX_ACTIVAR(&atlasTex->texturasEnMapaMutex)
							if(atlasTex->mapaAtlas != NULL){
								atlasTex->mapaAtlas->liberarEspacioEnIndice(iTextura);
							}
							NBGestorTexturas::privSTGestorTexTexturaRelease(datosTextura);
							atlasTex->texturasEnMapa->quitarElementoEnIndice(iTextura);
							textura = NULL;
							//NBHILO_MUTEX_DESACTIVAR(&atlasTex->texturasEnMapaMutex)
						}
						break;
					}
				}
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

UI16 NBGestorTexturas::liberarTexturasSinReferenciasEnAmbito(UI8 iAmb){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTexturas::liberarTexturasSinReferenciasEnAmbito")
	NBASSERT(!_bloqueado)
	UI16 conteoLiberados = 0;
	//NBASSERT(ambTex->ambitoReservado)
	//Free preloaded textures
	STGestorTexAmbito* ambTex = &_ambitosAtlases[iAmb];
	//Free loaded textures
	SI32 iAtlas;
	for(iAtlas=ambTex->atlasesTexturas->conteo-1; iAtlas>=0; iAtlas--){
		STGestorTexAtlas* atlasTex = ambTex->atlasesTexturas->elem(iAtlas);
		SI32 iTextura;
		for(iTextura = (atlasTex->texturasEnMapa->conteo - 1); iTextura>=0; iTextura--){
			STGestorTexTextura* datosTextura = atlasTex->texturasEnMapa->elem(iTextura);
			//PRINTF_INFO("Textura %d-%d referencias(%d): '%s'.\n", iAtlas, iTextura, datosTextura->textura->conteoReferencias(), datosTextura->nombreTextura->str());
			//NBGestorMemoria::debug_imprimeRetencionesPuntero(datosTextura->textura);
			if(datosTextura->textura->conteoReferencias() == 1){ //solo el gestor tiene referencia a ella
				//Search for used subtextures
				bool algunaSubtexturaEnUso = false;
				if(datosTextura->subTexturas != NULL){
					SI32 i; const SI32 conteo = datosTextura->subTexturas->conteo;
					for(i = 0; i < conteo; i++){
						if(datosTextura->subTexturas->elem(i)->conteoReferencias() != 1){
							algunaSubtexturaEnUso = true;
							break;
						}
					}
				}
				//Free texture
				if(!algunaSubtexturaEnUso){
					//Remove from unorganized-list
					{
						STGestorTexTexturaRef texRef;
						texRef.iAmb		= iAmb;
						texRef.atlas	= atlasTex;
						texRef.texture	= datosTextura;
						NBHILO_MUTEX_ACTIVAR(&_texUnorganizedRefsMutex)
						const SI32 iEncontrado = _texUnorganizedRefs->indiceDe(texRef);
						if(iEncontrado != -1){
							NBGestorTexturas::privSTGestorTexAtlasRelease(texRef.atlas);
							NBGestorTexturas::privSTGestorTexTexturaRelease(texRef.texture);
							_texUnorganizedRefs->quitarElementoEnIndice(iEncontrado);
							NBASSERT(_texUnorganizedRefs->indiceDe(texRef) == -1)
						}
						NBHILO_MUTEX_DESACTIVAR(&_texUnorganizedRefsMutex)
					}
					//Remove from loading-list
					{
						STGestorTexTexturaRef texRef;
						texRef.iAmb		= iAmb;
						texRef.atlas	= atlasTex;
						texRef.texture	= datosTextura;
						NBHILO_MUTEX_ACTIVAR(&_texLoadingRefsMutex);
						{
							SI32 iEncontrado = -1;
							SI32 i; const SI32 count = _texLoadingRefs->conteo;
							for(i = 0; i < count; i++){
								STGestorTexTexturaRef* itm = _texLoadingRefs->elem(i);
								if(texRef == *itm){
									iEncontrado = i;
								}
							}
							if(iEncontrado != -1){
								NBGestorTexturas::privSTGestorTexAtlasRelease(texRef.atlas);
								NBGestorTexturas::privSTGestorTexTexturaRelease(texRef.texture);
								NBGestorMemoria::liberarMemoria(_texLoadingRefs->elem(iEncontrado));
								_texLoadingRefs->quitarElementoEnIndice(iEncontrado);
							}
						}
						NBHILO_MUTEX_DESACTIVAR(&_texLoadingRefsMutex);
					}
					//Remove from Atlas
					//NBHILO_MUTEX_ACTIVAR(&atlasTex->texturasEnMapaMutex)
					//PRINTF_INFO("TEXTURA %d ELIMINANDO ret(%d) autolib(%d) '%s'.\n", iTextura, datosTextura->textura->conteoReferencias(), datosTextura->textura->conteoAutoliberacionesPendientes(), datosTextura->nombreTextura->str());
					if(atlasTex->mapaAtlas != NULL){
						atlasTex->mapaAtlas->liberarEspacioEnIndice(iTextura);
					}
					NBGestorTexturas::privSTGestorTexTexturaRelease(datosTextura);
					atlasTex->texturasEnMapa->quitarElementoEnIndice(iTextura);
					conteoLiberados++;
					//NBHILO_MUTEX_DESACTIVAR(&atlasTex->texturasEnMapaMutex)
				}
			} else {
				//PRINTF_INFO("TEXTURA %d +PERSISTE+ ret(%d) autolib(%d) '%s'.\n", iTextura, datosTextura->textura->conteoReferencias(), datosTextura->textura->conteoAutoliberacionesPendientes(), datosTextura->nombreTextura->str());
			}
		}
		//Liberar el atlas completo (si ha quedado vacio)
		//PENDIENTE: optimizar para que quede un atlas vacio al final, para no tener que crearlo nuevamente
		if(atlasTex->texturasEnMapa->conteo == 0){
			//PRINTF_INFO("LIBERANDO ATLAS DE TEXTURA, iAtlas(%d)\n", iAtlas);
			//NBHILO_MUTEX_ACTIVAR(&ambTex->atlasesTexturasMutex)
			NBGestorTexturas::privSTGestorTexAtlasRelease(atlasTex);
			ambTex->atlasesTexturas->quitarElementoEnIndice(iAtlas);
			conteoLiberados++;
			//NBHILO_MUTEX_DESACTIVAR(&ambTex->atlasesTexturasMutex)
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return conteoLiberados;
}

UI16 NBGestorTexturas::liberarTexturasSinReferencias(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTexturas::liberarTexturasSinReferencias")
	NBASSERT(!_bloqueado)
	UI16 conteoLiberados = 0;
	//LIBERAR TEXTURAS DE AMBITOS
	UI8 iAmb;
	for(iAmb=0; iAmb<GESTOR_TEXTURAS_MAX_AMBITOS_TEXTURAS; iAmb++){
		conteoLiberados += NBGestorTexturas::liberarTexturasSinReferenciasEnAmbito(iAmb);
	}
	//LIBERAR FUENTES
	SI32 iFnt;
	for(iFnt = _fuentesTexturas->conteo - 1; iFnt >= 0; iFnt--){
		STGestorTexFuente* datosFuente = _fuentesTexturas->elemPtr(iFnt);
		NBASSERT(datosFuente->texturaFuente != NULL);
		if(datosFuente->texturaFuente->conteoReferencias() == 1){ // == 1 el gestor es el unico usuario de la fuente
			SI32 iTex; const SI32 conteoTex = datosFuente->texturas->conteo;
			for(iTex = 0; iTex < conteoTex; iTex++){
				STGestorTexFuenteTex* datTex = datosFuente->texturas->elemPtr(iTex);
				GLuint idTextura = datTex->propsTexturaGL.idTexturaGL;
				NBGestorGL::deleteTextures(1, &idTextura);
			}
			datosFuente->texturaFuente->liberar(NB_RETENEDOR_NULL);
			datosFuente->texturas->liberar(NB_RETENEDOR_NULL);
			_fuentesTexturas->quitarElementoEnIndice(iFnt);
			conteoLiberados++;
		}
	}
	for(iFnt = _fuentesRaster->conteo - 1; iFnt >= 0; iFnt--){
		STGestorTexFuenteR* datosFuente = _fuentesRaster->elemPtr(iFnt);
		if(datosFuente->fuenteRaster->conteoReferencias() == 1){ // == 1 el gestor es el unico usuario de la fuente
			datosFuente->fuenteRaster->liberar(NB_RETENEDOR_NULL);
			_fuentesRaster->quitarElementoEnIndice(iFnt);
			conteoLiberados++;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return conteoLiberados;
}

GLenum NBGestorTexturas::formatoGlLocal(MapaBitsColor colorDestino){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTexturas::formatoGlLocal")
	GLenum r = 0;
	switch (colorDestino) {
		case COLOR_GRIS8:		r = GL_LUMINANCE; break;
		case COLOR_ALPHA8:		r = GL_ALPHA; break;
		case COLOR_GRISALPHA8:	r = GL_LUMINANCE_ALPHA; break;
		case COLOR_RGB8:		r = GL_RGB; break;
		case COLOR_RGBA8:		r = GL_RGBA; break;
#		ifdef __ANDROID__
		case COLOR_BGRA8:		r = GL_BGRA_EXT; break; //defined in <GLES/glext.h> (also GL_BGRA8_EXT available)
#		else
		case COLOR_BGRA8:		r = GL_RGBA; break; //GL_BGRA;
#		endif
		default:				r = 0; break;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return r;
}

GLenum NBGestorTexturas::formatoGlDriver(MapaBitsColor colorDestino){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTexturas::formatoGlDriver")
	//Apple, formatos optimos
	//https://developer.apple.com/library/mac/#documentation/graphicsimaging/Conceptual/OpenGL-MacProgGuide/opengl_texturedata/opengl_texturedata.html#//apple_ref/doc/uid/TP40001987-CH407-SW22
	//The best format and data type combinations to use for texture data are:
	//GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV
	//GL_BGRA, GL_UNSIGNED_SHORT_1_5_5_5_REV)
	//GL_YCBCR_422_APPLE, GL_UNSIGNED_SHORT_8_8_REV_APPLE
	//The combination GL_RGBA and GL_UNSIGNED_BYTE needs to be swizzled by many cards when the data is loaded, so it's not recommended.
	GLenum r = 0;
	switch (colorDestino) {
		case COLOR_GRIS8:		r = GL_LUMINANCE; break;
		case COLOR_ALPHA8:		r = GL_ALPHA; break;
		case COLOR_GRISALPHA8:	r = GL_LUMINANCE_ALPHA; break;
		case COLOR_RGB8:		r = GL_RGB; break;
		case COLOR_RGBA8:		r = GL_RGBA; break;
#		ifdef __ANDROID__
		case COLOR_BGRA8:		r = GL_BGRA_EXT; break; //defined in <GLES/glext.h> (also GL_BGRA8_EXT available)
#		else
		case COLOR_BGRA8:		r = GL_BGRA; break;
#		endif
		default:				r = 0; break;
	} NBASSERT(r > 0)
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return r;
}

GLenum NBGestorTexturas::tipoDatoGlLocal(MapaBitsColor colorDestino){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTexturas::tipoDatoGlLocal")
	GLenum r = 0;
	switch (colorDestino) {
		case COLOR_GRIS8:
		case COLOR_ALPHA8:
		case COLOR_GRISALPHA8:
		case COLOR_RGB8:
		case COLOR_RGBA8:
			r = GL_UNSIGNED_BYTE;
			break;
		case COLOR_BGRA8:
			r = GL_UNSIGNED_BYTE; //ToDo: change to GL_UNSIGNED_INT_8_8_8_8_REV
			break;
		default:
			r = GL_UNSIGNED_BYTE;
			break;
	} NBASSERT(r > 0)
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return r;
}

GLenum NBGestorTexturas::tipoDatoGlDriver(MapaBitsColor colorDestino){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTexturas::tipoDatoGlDriver")
	//Apple, formatos optimos
	//https://developer.apple.com/library/mac/#documentation/graphicsimaging/Conceptual/OpenGL-MacProgGuide/opengl_texturedata/opengl_texturedata.html#//apple_ref/doc/uid/TP40001987-CH407-SW22
	//The best format and data type combinations to use for texture data are:
	//GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV
	//GL_BGRA, GL_UNSIGNED_SHORT_1_5_5_5_REV)
	//GL_YCBCR_422_APPLE, GL_UNSIGNED_SHORT_8_8_REV_APPLE
	//The combination GL_RGBA and GL_UNSIGNED_BYTE needs to be swizzled by many cards when the data is loaded, so it's not recommended.
	GLenum r = 0;
	switch (colorDestino) {
		case COLOR_ALPHA8:
		case COLOR_GRIS8:
		case COLOR_GRISALPHA8:
		case COLOR_RGB8:
			r = GL_UNSIGNED_BYTE;
			break;
		case COLOR_RGBA8:
			r = GL_UNSIGNED_BYTE;
			break;
		case COLOR_BGRA8:
			r = GL_UNSIGNED_BYTE; //ToDo: change to GL_UNSIGNED_INT_8_8_8_8_REV
			break;
		default:
			r = GL_UNSIGNED_BYTE;
			break;
	} NBASSERT(r > 0)
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return r;
}

UI8 NBGestorTexturas::bytesPorPixel(MapaBitsColor colorDestino){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTexturas::formatoGl")
	UI8 r = 0;
	switch (colorDestino) {
		case COLOR_ALPHA8:
		case COLOR_GRIS8:
			r = 1;
			break;
		case COLOR_GRISALPHA8:
			r = 2;
			break;
		case COLOR_RGB8:
			r = 3;
			break;
		case COLOR_RGBA8:
		case COLOR_BGRA8:
			r = 4;
			break;
		default:
			r = 0;
			break;
	} NBASSERT(r > 0)
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return r;
}

STGestorTexFuenteVAO* NBGestorTexturas::privFuenteRasterVao(STNBFontLines* fuenteDef, const float tamano, const bool crearSiNoExiste){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTexturas::privFuenteRasterVao")
	NBASSERT(!_bloqueado)
	NBASSERT(_fuentesRaster != NULL)
	STGestorTexFuenteVAO* rFuente = NULL;
	//BUSCAR - buscar la fuente previamente cargada
	{
		STGestorTexFuenteVAO datBsq;
		datBsq.fuenteLineas = fuenteDef;
		datBsq.tamano		= tamano;
		const SI32 indice	= _fuentesRasterVAOs->indiceDe(datBsq);
		if(indice != -1){
			rFuente = _fuentesRasterVAOs->elemPtr(indice);
		}
	}
	//CARGAR la fuentetextura
	if(rFuente == NULL && crearSiNoExiste){
		STGestorTexFuenteVAO datNvo;
		datNvo.fuenteLineas = fuenteDef; NBFontLines_retain(fuenteDef);
		datNvo.tamano		= tamano;
		datNvo.versionEnVao	= NBFontLines_getLinesSize(fuenteDef, tamano)->version;
		datNvo.iVao			= NBGestorGL::bufferVerticesReservar(ENVerticeGlTipo_SinTextura, &NBGestorTexturas::bufferObtenerDatos, NULL); NBASSERT(datNvo.iVao >= 0)
		if(datNvo.iVao >= 0){
			const SI32 indiceNvo = _fuentesRasterVAOs->agregarElemento(datNvo);
			rFuente = _fuentesRasterVAOs->elemPtr(indiceNvo);
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return rFuente;
}

AUFuenteRaster* NBGestorTexturas::fuenteRaster(const char* nombreFuente, const float tamanoFuenteEnEscena, const char* subfamily){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTexturas::fuenteRaster")
	AUFuenteRaster* r = NBGestorTexturas::privFuenteRaster(nombreFuente, tamanoFuenteEnEscena, subfamily, false, false);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return r;
}

AUFuenteRaster* NBGestorTexturas::fuenteRaster(const char* nombreFuente, const float tamanoFuenteEnEscena, const bool negrilla, const bool cursiva){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTexturas::fuenteRaster")
	AUFuenteRaster* r = NBGestorTexturas::privFuenteRaster(nombreFuente, tamanoFuenteEnEscena, NULL, negrilla, cursiva);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return r;
}

AUFuenteRaster* NBGestorTexturas::privFuenteRaster(const char* nombreFuente, const float tamanoFuenteEnEscena, const char* optSubfamily, const bool negrilla, const bool cursiva){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTexturas::privFuenteRaster")
	NBASSERT(!_bloqueado)
	NBASSERT(_fuentesRaster != NULL)
	const float tamanoFuente = tamanoFuenteEnEscena / _propsByResolution[ENTexturaResolucion_Escena].scaleToHD; //PRINTF_INFO("Consultando fuenteRASTER tamanoEscena(%f) tamanoPx(%f) escalaEscenaParaHD(%f).\n", tamanoFuenteEnEscena, tamanoFuente, _propsByResolution[ENTexturaResolucion_Escena].scaleToHD);
	AUFuenteRaster* rFuente = NULL;
	//BUSCAR - buscar la fuente previamente cargada
	UI32 iAtlas, iAtlasConteo = _fuentesRaster->conteo;
	for(iAtlas = 0; iAtlas < iAtlasConteo; iAtlas++){
		AUFuenteRaster* fuenteTex = (AUFuenteRaster*)_fuentesRaster->elem(iAtlas).fuenteRaster;
		if(!fuenteTex->isDisabled()){
			if(fuenteTex->tamanoEscena() == tamanoFuente){
				if(AUCadena8::esIgual(fuenteTex->familia(), nombreFuente)){
					if(optSubfamily != NULL){
						if(AUCadena8::esIgual(fuenteTex->subfamilia(), optSubfamily)){
							rFuente = fuenteTex;
							//PRINTF_INFO("La fuente '%s' %u (%s, %s) ya estaba cargada\n", nombreFuente, tamanoFuente, negrilla ? "negrilla" : "normal", cursiva ? "cursiva" : "normal");
							break;
						}
					} else if(fuenteTex->esNegrilla() == negrilla && fuenteTex->esCursiva() == cursiva){
						rFuente = fuenteTex;
						//PRINTF_INFO("La fuente '%s' %u (%s, %s) ya estaba cargada\n", nombreFuente, tamanoFuente, negrilla ? "negrilla" : "normal", cursiva ? "cursiva" : "normal");
						break;
					}
				}
			}
		}
	}
	//CARGAR la fuentetextura
	if(rFuente == NULL){
		AUFuenteLineas* fuenteLineas = NULL;
		if(NBString_strIsEmpty(optSubfamily)){
			fuenteLineas = NBGestorFuentes::fuenteLineas(nombreFuente, negrilla, cursiva);
		} else {
			fuenteLineas = NBGestorFuentes::fuenteLineasSubFamily(nombreFuente, optSubfamily);
		}
		//
		if(fuenteLineas == NULL){
			PRINTF_ERROR("no se pudo obtener fuenteLineas para '%s%s%s'\n", nombreFuente, negrilla ? "_n" : "", cursiva ? "_c" : "");
		} else {
			STNBFontLines* fontDef = fuenteLineas->fuenteDef();
			STGestorTexFuenteVAO* fuenteVAO = NBGestorTexturas::privFuenteRasterVao(fontDef, tamanoFuente, true);
			if(fuenteVAO == NULL){
				PRINTF_ERROR("no se pudo crear VAO fuenteLineas para '%s%s%s'\n", nombreFuente, negrilla ? "_n" : "", cursiva ? "_c" : "");
			} else {
				STGestorTexFuenteR nuevaFuente;
				nuevaFuente.fontLines		= fontDef;
				nuevaFuente.fuenteRaster	= new(ENMemoriaTipo_Nucleo) AUFuenteRaster(fontDef, tamanoFuente, _propsByResolution[ENTexturaResolucion_Escena].scaleToHD); NB_DEFINE_NOMBRE_PUNTERO(nuevaFuente.fuenteRaster, "NBGestorTexturas::nuevaFuente.fuenteRaster")
				_fuentesRaster->agregarElemento(nuevaFuente);
				//
				rFuente = nuevaFuente.fuenteRaster;
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return rFuente;
}

/*BOOL NBGestorTexturas::updateFontRasterToSize(AUFuenteRaster* font, const float newSize){
	BOOL r = FALSE;
	const float tamanoFuente = newSize / _propsByResolution[ENTexturaResolucion_Escena].scaleToHD; //PRINTF_INFO("Consultando fuenteRASTER tamanoEscena(%f) tamanoPx(%f) escalaEscenaParaHD(%f).\n", tamanoFuenteEnEscena, tamanoFuente, _propsByResolution[ENTexturaResolucion_Escena].scaleToHD);
	{
		UI32 iAtlas, iAtlasConteo = _fuentesRaster->conteo;
		for(iAtlas = 0; iAtlas < iAtlasConteo; iAtlas++){
			AUFuenteRaster* fuenteTex 	= (AUFuenteRaster*)_fuentesRaster->elem(iAtlas).fuenteRaster;
			if(fuenteTex == font){
				fuenteTex->updateFontSize(tamanoFuente);
				r = TRUE;
				break;
			}
		}
	}
	return r;
}*/

//ToDO: remove
/*void NBGestorTexturas::fuentesResetCurrent(){
	//Texture fonts
	{
		UI32 iAtlas, iAtlasConteo = _fuentesTexturas->conteo;
		for(iAtlas = 0; iAtlas < iAtlasConteo; iAtlas++){
			STGestorTexFuente* record = _fuentesTexturas->elemPtr(iAtlas);
			AUFuenteTextura* fuenteTex = (AUFuenteTextura*)record->texturaFuente;
			fuenteTex->setDisabled(TRUE);
		}
	}
	//Txture rasters
	{
		UI32 iAtlas, iAtlasConteo = _fuentesRaster->conteo;
		for(iAtlas = 0; iAtlas < iAtlasConteo; iAtlas++){
			AUFuenteRaster* fuenteTex = (AUFuenteRaster*)_fuentesRaster->elem(iAtlas).fuenteRaster;
			fuenteTex->setDisabled(TRUE);
		}
	}
}*/

//

void NBGestorTexturas::bufferObtenerDatos(void* param, const SI32 iVao, STVerticesDatos* guardarDatosEn){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTexturas::bufferObtenerDatos")
	UI32 i; const UI32 conteo = _fuentesRasterVAOs->conteo;
	for(i = 0; i < conteo; i++){
		STGestorTexFuenteVAO* dat = _fuentesRasterVAOs->elemPtr(i);
		if(dat->iVao == iVao){
			const STNBFontLinesSize* datosEnTam = NBFontLines_getLinesSize(dat->fuenteLineas, dat->tamano);
			if(datosEnTam != NULL){
				//Actualizar datos de vertices si la version difiere
				if(dat->versionEnVao != datosEnTam->version){
					const char* family = NULL; const char* subfamily = NULL; UI8 styleMask = 0;
					NBFontLines_getStyle(dat->fuenteLineas, &family, &subfamily, &styleMask);
					PRINTF_INFO("Fuente, actualizando datos VAO para '%s_%d' (verVao = %d, verFuente = %d).\n", family, styleMask, dat->versionEnVao, datosEnTam->version);
					if(datosEnTam->vertexs.use > 0){ //STNBFontLineVertex
						const STNBFontLineVertex* vertsArr = NBArray_dataPtr(&datosEnTam->vertexs, STNBFontLineVertex);
						//Convertir los vertices a VerticesGL
						NBVerticeGL0 vNvo;
						AUArregloNativoMutableP<NBVerticeGL0>* arrTmp = new(ENMemoriaTipo_Temporal) AUArregloNativoMutableP<NBVerticeGL0>();
						UI32 i; const UI32 conteo = datosEnTam->vertexs.use;
						for(i = 0; i < conteo; i++){
							const STNBFontLineVertex* datVer = &vertsArr[i];
							vNvo.x = datVer->pos;
							vNvo.y = datVer->line;
							vNvo.r = vNvo.g = vNvo.b = 255;
							vNvo.a = datVer->intensity;
							arrTmp->agregarElemento(vNvo);
						}
						guardarDatosEn->verticesDatos = (BYTE*)arrTmp->arreglo();
						guardarDatosEn->verticesCantBytes = arrTmp->conteo * sizeof(NBVerticeGL0);
						arrTmp->autoLiberar(NB_RETENEDOR_NULL);
					}
					dat->versionEnVao = datosEnTam->version;
				}
			}
			guardarDatosEn->indicesDatos = NULL;
			guardarDatosEn->indicesCantBytes = 0;
			break;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

/*STNBFontLines* NBGestorTexturas::fuenteLineas(const char* nomFuente, const bool negrilla, const bool cursiva){
	return NBGestorFuentes::fuenteLineas(const char* nomFuente, const bool negrilla, const bool cursiva);
}*/

/*STNBFontBitmaps* NBGestorTexturas::fuenteBitmaps(const char* nomFuente, const float fontSz, const bool negrilla, const bool cursiva){
	return NBGestorFuentes::fuenteBitmaps(const char* nomFuente, const float fontSz, const bool negrilla, const bool cursiva);
}*/

AUFuenteTextura* NBGestorTexturas::fuenteTextura(const char* nombreFuente, const float tamanoFuenteEnEscena, const char* subfamily, const float escalaEscenaParaHD){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTexturas::fuenteTextura")
	AUFuenteTextura* r = NBGestorTexturas::privFuenteTextura(nombreFuente, tamanoFuenteEnEscena, subfamily, false, false, escalaEscenaParaHD);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return r;
}

AUFuenteTextura* NBGestorTexturas::fuenteTextura(const char* nombreFuente, const float tamanoFuenteEnEscena, const bool negrilla, const bool cursiva, const float escalaEscenaParaHD){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTexturas::fuenteTextura")
	AUFuenteTextura* r = NBGestorTexturas::privFuenteTextura(nombreFuente, tamanoFuenteEnEscena, NULL, negrilla, cursiva, escalaEscenaParaHD);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return r;
}

AUFuenteTextura* NBGestorTexturas::privFuenteTextura(const char* nombreFuente, const float tamanoFuenteEnEscena, const char* optSubfamily, const bool negrilla, const bool cursiva, const float escalaEscenaParaHD){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTexturas::privFuenteTextura")
	NBASSERT(!_bloqueado)
	NBASSERT(_fuentesTexturas != NULL)
	const float tamanoFuente = (float)((SI32)(tamanoFuenteEnEscena / escalaEscenaParaHD)); //PRINTF_INFO("Consultando fuenteTEXTURA tamanoEscena(%f) tamanoPx(%f) escalaEscenaParaHD(%f).\n", tamanoFuenteEnEscena, tamanoFuente, escalaEscenaParaHD);
	const float escalaSubTexturas = 1.0f * escalaEscenaParaHD;
	AUFuenteTextura* rFuente = NULL;
	//BUSCAR - buscar la fuente previamente cargada
	UI32 iAtlas, iAtlasConteo = _fuentesTexturas->conteo;
	for(iAtlas = 0; iAtlas < iAtlasConteo; iAtlas++){
		AUFuenteTextura* fuenteTex 	= (AUFuenteTextura*)_fuentesTexturas->elem(iAtlas).texturaFuente;
		if(!fuenteTex->isDisabled()){
			const float pSize	= fuenteTex->tamanoEscena();
			const float pScale	= fuenteTex->escalaSubTexturas();
			if(pSize == tamanoFuente && pScale == escalaSubTexturas){
				const char* pFamily = fuenteTex->familia();
				if(AUCadena8::esIgual(pFamily, nombreFuente)){
					const bool pIsBold	= fuenteTex->esNegrilla();
					const bool pIsItlc	= fuenteTex->esCursiva();
					const char* pSFamly	= fuenteTex->subfamilia();
					if(pSFamly != NULL && optSubfamily != NULL){
						if(AUCadena8::esIgual(pSFamly, optSubfamily)){
							rFuente = fuenteTex;
							//PRINTF_INFO("La fuente '%s' %f (%s, %s) ya estaba cargada\n", nombreFuente, tamanoFuente, negrilla ? "negrilla" : "normal", cursiva ? "cursiva" : "normal");
							break;
						}
					} else if(pIsBold == negrilla && pIsItlc == cursiva){
						rFuente = fuenteTex;
						//PRINTF_INFO("La fuente '%s' %f (%s, %s) ya estaba cargada\n", nombreFuente, tamanoFuente, negrilla ? "negrilla" : "normal", cursiva ? "cursiva" : "normal");
						break;
					}
				}
			}
		}
	}
	//CARGAR la fuentetextura
	if(rFuente == NULL){
		AUFuenteMapaBits* fuenteMapaBits = NULL;
		if(NBString_strIsEmpty(optSubfamily)){
			fuenteMapaBits = NBGestorFuentes::fuenteBitmaps(nombreFuente, tamanoFuente, negrilla, cursiva);
		} else {
			fuenteMapaBits = NBGestorFuentes::fuenteBitmapsSubFamily(nombreFuente, tamanoFuente, optSubfamily);
		}
		//
		if(fuenteMapaBits == NULL){
			PRINTF_ERROR("no se pudo crear textura para fuente '%s%s%s'\n", nombreFuente, negrilla ? "_negrilla" : "_normal", cursiva ? "_cursiva" : "_normal");
			NBASSERT(FALSE)
		} else {
			STNBFontBitmaps* fontDef = fuenteMapaBits->fuenteDef();
			NBASSERT(NBFontBitmaps_getSize(fontDef) == tamanoFuente)
			//PRINTF_INFO("Creating new fontTexture (%s/%s%s%s/%.1fx%.1f).\n", nombreFuente, optSubfamily != NULL ? optSubfamily : "", negrilla ? "_negrilla" : "_normal", cursiva ? "_cursiva" : "_normal", tamanoFuente, escalaEscenaParaHD);
			STGestorTexFuente nuevaFuente;
			nuevaFuente.version			= NBFontBitmaps_getVersion(fontDef);
			nuevaFuente.fontBitmaps		= fontDef;
			nuevaFuente.texturaFuente	= new(ENMemoriaTipo_Nucleo) AUFuenteTextura(fontDef, ENTexturaModoPintado_Imagen_Precisa, escalaSubTexturas); NB_DEFINE_NOMBRE_PUNTERO(nuevaFuente.texturaFuente, "NBGestorTexturas::nuevaFuente.texturaFuente")
			nuevaFuente.texturas		= new(ENMemoriaTipo_Nucleo) AUArregloNativoMutableP<STGestorTexFuenteTex>(); NB_DEFINE_NOMBRE_PUNTERO(nuevaFuente.texturaFuente, "NBGestorTexturas::nuevaFuente.texturas")
			//Create atlases
			NBGestorTexturas::privCreateFontTextureAtlases(&nuevaFuente, fontDef);
			//Add
			_fuentesTexturas->agregarElemento(nuevaFuente);
			rFuente = nuevaFuente.texturaFuente;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return rFuente;
}

UI32 NBGestorTexturas::privCreateFontTextureAtlases(STGestorTexFuente* record, STNBFontBitmaps* fontDef){
	UI32 i = 0;
	do {
		STNBFontBitmap bmp = NBFontBitmaps_getBitmapAtIndex(fontDef, i);
		if(bmp.bitmap == NULL){
			break;
		} else {
			const STNBBitmapProps bmpProps = NBBitmap_getProps(bmp.bitmap);
			const BYTE* bmpData = NBBitmap_getData(bmp.bitmap);
			NBASSERT(bmpProps.size.width > 0 && bmpProps.size.height > 0);
			MapaBitsColor colorDestino = COLOR_NO_DEFINIDO;
			switch (bmpProps.color) {
				case ENNBBitmapColor_ALPHA8:		colorDestino = COLOR_ALPHA8; break;
				case ENNBBitmapColor_GRIS8:			colorDestino = COLOR_GRIS8; break;
				case ENNBBitmapColor_GRISALPHA8:	colorDestino = COLOR_GRISALPHA8; break;
				case ENNBBitmapColor_RGB8:			colorDestino = COLOR_RGB8; break;
				case ENNBBitmapColor_RGBA8:			colorDestino = COLOR_RGBA8; break;
				case ENNBBitmapColor_ARGB8:			colorDestino = COLOR_ARGB8; break;
				case ENNBBitmapColor_BGRA8:			colorDestino = COLOR_BGRA8; break;
#				ifdef NB_CONFIG_INCLUDE_ASSERTS
				default:
					NBASSERT(FALSE) 
					break;
#				endif
			}
			GLenum formatoGlLocal		= NBGestorTexturas::formatoGlLocal(colorDestino); NBASSERT(formatoGlLocal != 0)
			GLenum formatoGlDriver		= NBGestorTexturas::formatoGlDriver(colorDestino); NBASSERT(formatoGlDriver != 0)
			GLenum tpoDatoGlDriver		= NBGestorTexturas::tipoDatoGlDriver(colorDestino); NBASSERT(tpoDatoGlDriver != 0)
			if(formatoGlLocal == 0 || formatoGlDriver == 0 || tpoDatoGlDriver == 0){
				PRINTF_ERROR("no se pudo establecer el formatoGLLocal(%d) formatoGLDriver(%d) o tipoDatoGLDriver(%d) del nuevo atlas\n", formatoGlLocal, formatoGlDriver, tpoDatoGlDriver);
			} else {
				GLuint idTextura;
				STGestorTexFuenteTex nuevaFuenteTex;
#				ifdef CONFIG_NB_GESTOR_GL_IMPRIMIR_COMANDOS_GL
				//PRINTF_WARNING("Generando textura de fuente: '%s%s%s'::%f tam(%d, %d) color(%d).\n", nombreFuente, (negrilla ? "_negrilla" : "_normal"), (cursiva ? "_cursiva" : "_normal"), tamanoFuente, (SI32)bmpProps.size.width, (SI32)bmpProps.size.height, (SI32)colorDestino);
#				endif
				NBGestorGL::genTextures(1, &idTextura, &nuevaFuenteTex.propsTexturaGL);
				NBGestorGL::bindTexture(0, idTextura);
				NBGestorGL::configurarTextura(ENTexturaModoPintado_Imagen_Precisa, false, &nuevaFuenteTex.propsTexturaGL);
				NBGestorGL::texImage2D(GL_TEXTURE_2D, 0, formatoGlLocal, bmpProps.size.width, bmpProps.size.height, 0, formatoGlDriver, tpoDatoGlDriver, bmpData, &nuevaFuenteTex.propsTexturaGL);
				_texsChangesCount++;
				NBASSERT(NBGestorGL::isTexture(idTextura))
				nuevaFuenteTex.version			= bmp.version;
				nuevaFuenteTex.iAtlasEnFuente	= i;
				nuevaFuenteTex.colorFormato		= colorDestino;
				//Nota: las texturas de texto no estan definidas en base a una resolucion (X4, X2, X1, etc..)
				//es decir, no se escalan hacia HD (ya son HD)
				record->texturas->agregarElemento(nuevaFuenteTex);
				record->texturaFuente->ligarAtlasConTexturaGl(nuevaFuenteTex.iAtlasEnFuente, idTextura);
			}
		}
		i++;
	} while(TRUE);
	return i;
}
	
//

void NBGestorTexturas::privLoadingStateInitBase(STGestoTexLoadingBase* state, AUArchivo* archivo, const ENGestorTexturaOrigen origenTextura, const UI8 escalaBase2, const float escalaParaHD, const char* enListarConNombre, const SI32 nomTamano, const ENTexturaTipoAlmacenamientoGL tipoAlmacenamiento, const ENTexturaTipoUsoGL tipoUso, const ENTexturaModoPintado modoPintado, const ENTexturaMipMap habilitarMipMap, const ENTexturaOrdenV ordenVTextura){
	state->type					= ENGestorTexturasLoadingType_Count; //Undefined
	state->archivo				= archivo; if(archivo != NULL) archivo->retener(NB_RETENEDOR_NULL);
	state->origenTextura		= origenTextura;
	state->escalaBase2			= escalaBase2;
	state->escalaParaHD			= escalaParaHD;
	state->enListarConNombre	= new(ENMemoriaTipo_General) AUCadena8(enListarConNombre);
	state->nomTamano			= nomTamano;
	state->tipoAlmacenamiento	= tipoAlmacenamiento;
	state->tipoUso				= tipoUso;
	state->modoPintado			= modoPintado;
	state->habilitarMipMap		= habilitarMipMap;
	state->ordenVTextura		= ordenVTextura;
	//
	state->formatoGlLocal		= 0;
	state->tpoDatoGlLocal		= 0;
	//Loading lines buffers
	state->linesLoadingBuffersInited = false;
	state->linesLoadingEnded	= false;
	state->linesAlreadyPasted	= 0;
	state->linesAlreadyLoaded	= 0;
	state->linesBuffers			= NULL;
}

void NBGestorTexturas::privLoadingStateInitPNG(STGestoTexLoadingPNG* state, AUArchivo* archivo, const ENGestorTexturaOrigen origenTextura, const UI8 escalaBase2, const float escalaParaHD, const char* enListarConNombre, const SI32 nomTamano, const ENTexturaTipoAlmacenamientoGL tipoAlmacenamiento, const ENTexturaTipoUsoGL tipoUso, const ENTexturaModoPintado modoPintado, const ENTexturaMipMap habilitarMipMap, const ENTexturaOrdenV ordenVTextura){
	NBGestorTexturas::privLoadingStateInitBase(state, archivo, origenTextura, escalaBase2, escalaParaHD, enListarConNombre, nomTamano, tipoAlmacenamiento, tipoUso, modoPintado, habilitarMipMap, ordenVTextura);
	//
	state->type			= ENGestorTexturasLoadingType_Png;
	//
	{
		NBPngLoadState_init(&state->load);
		{
			IFileItf itf = AUArchivo::getFileItf();
			NBPng_loadStreamSetItf(&state->load, &itf, archivo);
		}
		NBArray_init(&state->extraChunks, sizeof(STNBPngChunk), NULL);
	}
}

void NBGestorTexturas::privLoadingStateInitJPEG(STGestoTexLoadingJPEG* state, AUArchivo* archivo, const ENGestorTexturaOrigen origenTextura, const UI8 escalaBase2, const float escalaParaHD, const char* enListarConNombre, const SI32 nomTamano, const ENTexturaTipoAlmacenamientoGL tipoAlmacenamiento, const ENTexturaTipoUsoGL tipoUso, const ENTexturaModoPintado modoPintado, const ENTexturaMipMap habilitarMipMap, const ENTexturaOrdenV ordenVTextura){
	NBGestorTexturas::privLoadingStateInitBase(state, archivo, origenTextura, escalaBase2, escalaParaHD, enListarConNombre, nomTamano, tipoAlmacenamiento, tipoUso, modoPintado, habilitarMipMap, ordenVTextura);
	//
	state->type		= ENGestorTexturasLoadingType_Jpg;
	state->estado	= AUMapaBitsMutable::jpegReadStateCreate(archivo); //NBASSERT(state->estado != NULL)
	state->buff		= NULL;
	state->buffSz	= 0;
}

void NBGestorTexturas::privLoadingStateRelease(STGestoTexLoadingBase* pState){
	//Custom type
	if(pState->type == ENGestorTexturasLoadingType_Png){
		STGestoTexLoadingPNG* state = (STGestoTexLoadingPNG*)pState;
		NBPngLoadState_release(&state->load);
		{
			SI32 i; for(i = 0; i < state->extraChunks.use; i++){
				STNBPngChunk* c = NBArray_itmPtrAtIndex(&state->extraChunks, STNBPngChunk, i);
				NBPngChunk_release(c);
			}
			NBArray_empty(&state->extraChunks);
			NBArray_release(&state->extraChunks);
		}
	} else if(pState->type == ENGestorTexturasLoadingType_Jpg){
		STGestoTexLoadingJPEG* stateJpg = (STGestoTexLoadingJPEG*)pState;
		AUMapaBitsMutable::jpegReadStateDestroy(stateJpg->estado);
		stateJpg->estado = NULL;
		if(stateJpg->buff != NULL){
			NBGestorMemoria::liberarMemoria(stateJpg->buff);
			stateJpg->buff = NULL;
			stateJpg->buffSz = 0;
		}
	} else {
		NBASSERT(false)
	}
	//Base type
	{
		pState->type = ENGestorTexturasLoadingType_Count; //Undefined
		if(pState->archivo != NULL){
			pState->archivo->liberar(NB_RETENEDOR_NULL);
			pState->archivo = NULL;
		}
		if(pState->enListarConNombre != NULL){
			pState->enListarConNombre->liberar(NB_RETENEDOR_NULL);
			pState->enListarConNombre = NULL;
		}
		//Loading buffers
		if(pState->linesLoadingBuffersInited){
			NBHILO_MUTEX_ACTIVAR(&pState->linesBuffersMutex);
			if(pState->linesBuffers != NULL){
				SI32 i; const SI32 count = pState->linesBuffers->conteo;
				for(i = 0; i < count; i++){
					STGestoTexLoadingBuffer* buffer = pState->linesBuffers->elemPtr(i);
					NBASSERT(buffer->data != NULL)
					if(buffer->data != NULL){
						NBGestorMemoria::liberarMemoria(buffer->data);
						buffer->data = NULL;
					}
				}
				pState->linesBuffers->vaciar();
				pState->linesBuffers->liberar(NB_RETENEDOR_NULL);
				pState->linesBuffers = NULL;
			}
			pState->linesLoadingBuffersInited = false;
			NBHILO_MUTEX_DESACTIVAR(&pState->linesBuffersMutex);
			NBHILO_MUTEX_FINALIZAR(&pState->linesBuffersMutex);
		}
	}
}

//-------------------------
//-- Textura desde archivos
//-------------------------

const char* NBGestorTexturas::privRutaArchivoTextura(STNBString* buff, UI8* dstEscalaBase2, float* dstEscalaParaHD, const char* nombre, const SI32 nomTamano, const ENGestorTexturaOrigen origenTextura){
	const char* r		= nombre;
	UI8 escalaBase2		= 1;
	float escalaParaHD	= 1;
	if(origenTextura == ENGestorTexturaOrigen_paquete){
		NBString_set(buff, _propsByResolution[ENTexturaResolucion_Escena].pathPrefix->str());
		NBString_concatBytes(buff, nombre, nomTamano);
		{
			const char* path = NBGestorArchivos::rutaHaciaRecursoEnPaquete(buff->str);
			NBString_set(buff, path);
			escalaBase2		= _propsByResolution[ENTexturaResolucion_Escena].scaleBase2;
			escalaParaHD	= _propsByResolution[ENTexturaResolucion_Escena].scaleToHD;
			r				= buff->str;
		}
	} else if(origenTextura == ENGestorTexturaOrigen_paquetePantallaPPP){
		NBString_set(buff, _propsByResolution[ENTexturaResolucion_Escena].pathPrefix->str());
		NBString_concatBytes(buff, nombre, nomTamano);
		{
			const char* path = NBGestorArchivos::rutaHaciaRecursoEnPaquete(buff->str);
			NBString_set(buff, path);
			escalaBase2		= _propsByResolution[ENTexturaResolucion_Pantalla].scaleBase2;
			escalaParaHD	= _propsByResolution[ENTexturaResolucion_Pantalla].scaleToHD;
			r				= buff->str;
		}
	} else if(origenTextura == ENGestorTexturaOrigen_rutaFisica){
		NBString_setBytes(buff, nombre, nomTamano);
	} else { //ENGestorTexturaOrigen_datos
		//
	}
	//
	if(dstEscalaBase2 != NULL) *dstEscalaBase2 = escalaBase2;
	if(dstEscalaParaHD != NULL) *dstEscalaParaHD = escalaParaHD;
	//
	return r;
}

STGestorTexTextura* NBGestorTexturas::privTexturaDesdeArchivoPNG(UI8 iAmbitoTexturas, const ENGestorTexturaOrigen origenTextura, const UI8 escalaBase2, const float escalaParaHD, const char* filepath, bool verificarEnListas, ENTexturaTipoAlmacenamientoGL tipoAlmacenamiento, ENTexturaTipoUsoGL tipoUso, ENTexturaModoPintado modoPintado, ENTexturaMipMap habilitarMipMap, STGestorTexTextura* inOptPreloadedTex){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTexturas::privTexturaDesdeArchivoPNG")
	NBASSERT(!_bloqueado)
	NBASSERT(iAmbitoTexturas < GESTOR_TEXTURAS_MAX_AMBITOS_TEXTURAS)
	NBASSERT(_ambitosAtlases[iAmbitoTexturas].ambitoReservado);
	STGestorTexTextura* r = NULL;
	const ENGestorTexturaModo modoCargaTexturas = NBGestorTexturas::modoCargaTexturas();
	//Intentar buscar textura pre-cargada
	if(verificarEnListas){
		r = NBGestorTexturas::privTexturaPorNombre(iAmbitoTexturas, filepath, tipoAlmacenamiento);
	}
	//Intentar cargar desde PNG
	if(r == NULL){
		AUArchivo* archivo = NBGestorArchivos::flujoDeArchivo(ENMemoriaTipo_Temporal, filepath, ENArchivoModo_SoloLectura);
		if(archivo == NULL){
			//PRINTF_ERROR("TEX, no se pudo cargar el PNG (1): '%s'\n", rutaPNG->str()); //NBASSERT(false)
		} else {
			bool releaseLoadingState = true;
			STGestoTexLoadingPNG* state = (STGestoTexLoadingPNG*)NBGestorMemoria::reservarMemoria(sizeof(STGestoTexLoadingPNG), ENMemoriaTipo_General);
			NBGestorTexturas::privLoadingStateInitPNG(state, archivo, origenTextura, escalaBase2, escalaParaHD, filepath, NBString_strLenBytes(filepath), tipoAlmacenamiento, tipoUso, modoPintado, habilitarMipMap, ENTexturaOrdenV_HaciaArriba);
			//Primero cargar sin bufferes de datos (carga solo los encabezados)
			{
				NBPng_loadStreamLock(&state->load);
				if(!NBPng_loadWithState(&state->load, NULL, 0, &state->extraChunks, NULL, NULL)){
					PRINTF_ERROR("no se pudo cargar el encabezado del PNG: '%s'\n", filepath);
				} else {
					//ToDo: remove
					/*if(verificarEnListas) {
					 //Verificar si esta cargada una textura con la misma firma
					 r = privTexturaPorFirmaSHA1(iAmbitoTexturas, state->firmaEnCarga, tipoAlmacenamiento);
					 }*/
					if(r == NULL){
						const STNBBitmapProps propsN = NBPngLoadState_getProps(&state->load);
						const MapaDeBitsDesciptor props = AUMapaBits::nativeToprops(&propsN);
						//STGestorTexAmbito* ambTex = &_ambitosAtlases[iAmbitoTexturas];
						if(modoCargaTexturas == ENGestorTexturaModo_cargaPendiente){
							//PRINTF_INFO("TEX cargando PNG (precarga): '%s'\n", rutaPNG->str());
							r = NBGestorTexturas::privTexturaRegistrar(iAmbitoTexturas, false /*do not store into atlasGL*/, false /*do not init loading lines state*/, state, ENTexturaOrigenFormato_Png, props, &state->extraChunks, NULL, inOptPreloadedTex);
							NBASSERT(r->textura->estadoCarga() == ENTexturaEstadoCarga_Precargada)
						} else if(modoCargaTexturas == ENGestorTexturaModo_cargaSegundoPlano && _texLoadThreadMode == ENTexturaModoHilo_MultiHilo){
							//PRINTF_INFO("TEX cargando PNG (carga en segundo plano): '%s'\n", rutaPNG->str());
							r = NBGestorTexturas::privTexturaRegistrar(iAmbitoTexturas, true /*do not store into atlasGL*/, true /*init loading lines state*/, state, ENTexturaOrigenFormato_Png, props, &state->extraChunks, NULL, inOptPreloadedTex);
							r->textura->privEstablecerEstadoCarga(ENTexturaEstadoCarga_Cargando);
							releaseLoadingState = false;
						} else /*if(modoCargaTexturas == ENGestorTexturaModo_cargaInmediata)*/{
							//PRINTF_INFO("TEX cargando PNG (carga inmediata): '%s'\n", rutaPNG->str());
							STGestorTexAtlas* atlasTex = NULL;
							r = NBGestorTexturas::privTexturaRegistrar(iAmbitoTexturas, true /*store into atlasGL*/, false /*do not init loading lines state*/, state, ENTexturaOrigenFormato_Png, props, &state->extraChunks, &atlasTex, inOptPreloadedTex);
							NBGestorTexturas::privTexturaCargarDatosStreamLocked(atlasTex, r, state);
							NBASSERT(r->textura->estadoCarga() == ENTexturaEstadoCarga_Cargada)
							/*} else {
							 PRINTF_ERROR("GestorTexturas modo-carga desconocido\n");
							 NBASSERT(false)*/
						}
					}
				}
				NBPng_loadStreamUnlock(&state->load);
			}
			if(releaseLoadingState){
				NBGestorTexturas::privLoadingStateRelease(state);
				NBGestorMemoria::liberarMemoria(state);
				state = NULL;
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return r;
}

STGestorTexTextura*	NBGestorTexturas::privTexturaDesdeArchivoJPEG(UI8 iAmbitoTexturas, const ENGestorTexturaOrigen origenTextura, const UI8 escalaBase2, const float escalaParaHD, const char* filepath, bool verificarEnListas, ENTexturaTipoAlmacenamientoGL tipoAlmacenamiento, ENTexturaTipoUsoGL tipoUso, ENTexturaModoPintado modoPintado, ENTexturaMipMap habilitarMipMap, STGestorTexTextura* inOptPreloadedTex){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTexturas::privTexturaDesdeArchivoJPEG")
	NBASSERT(!_bloqueado)
	NBASSERT(iAmbitoTexturas < GESTOR_TEXTURAS_MAX_AMBITOS_TEXTURAS)
	NBASSERT(_ambitosAtlases[iAmbitoTexturas].ambitoReservado)
	STGestorTexTextura* r = NULL;
	const ENGestorTexturaModo modoCargaTexturas = NBGestorTexturas::modoCargaTexturas();
	//Intentar buscar textura pre-cargada
	if(verificarEnListas){
		r = privTexturaPorNombre(iAmbitoTexturas, filepath, tipoAlmacenamiento);
	}
	//Intentar cargar desde JPEG
	if(r == NULL){
		AUArchivo* archivo = NBGestorArchivos::flujoDeArchivo(ENMemoriaTipo_Temporal, filepath, ENArchivoModo_SoloLectura);
		if(archivo == NULL){
			PRINTF_ERROR("TEX, no se pudo cargar el JPEG (1): '%s'\n", filepath); //NBASSERT(false)
		} else {
			//NBASSERT(archivo->posicionActual() >= 0) //2025-07-04: removed, subfile allways starts at zero
			bool releaseLoadingState = true;
			STGestoTexLoadingJPEG* state = (STGestoTexLoadingJPEG*)NBGestorMemoria::reservarMemoria(sizeof(STGestoTexLoadingJPEG), ENMemoriaTipo_General);
            {
                NBGestorTexturas::privLoadingStateInitJPEG(state, archivo, origenTextura, escalaBase2, escalaParaHD, filepath, NBString_strLenBytes(filepath), tipoAlmacenamiento, tipoUso, modoPintado, habilitarMipMap, ENTexturaOrdenV_HaciaArriba);
            }
			if(state->estado == NULL){
				PRINTF_ERROR("could not create the jpegRead state for: '%s'\n", filepath); //NBASSERT(false)
			} else {
				//Read headers (no buffers)
                ENJpegReadResult readResult = ENJpegReadResult_error;
                while(ENJpegReadResult_partial == (readResult = AUMapaBitsMutable::jpegReadStateRead(state->estado, NULL, 0, NULL))){
                    //read more
                }
				//Process result
				if(readResult != ENJpegReadResult_end){
					PRINTF_ERROR("no se pudo cargar el encabezado del JPEG: '%s'\n", filepath); //NBASSERT(false)
				} else {
					MapaDeBitsDesciptor props;
					if(!AUMapaBitsMutable::jpegReadStateGetProps(state->estado, &props)){
						PRINTF_ERROR("no se pudo obtene propiedades del JPEG: '%s'\n", filepath); //NBASSERT(false)
					} else {
						//STGestorTexAmbito* ambTex = &_ambitosAtlases[iAmbitoTexturas];
						if(modoCargaTexturas == ENGestorTexturaModo_cargaPendiente){
							PRINTF_INFO("TEX cargando JPEG (precarga): '%s'\n", filepath);
							r = NBGestorTexturas::privTexturaRegistrar(iAmbitoTexturas, false /*do not store into atlasGL*/, false /*do not init loading lines state*/, state, ENTexturaOrigenFormato_Jpg, props, NULL, NULL, inOptPreloadedTex);
							NBASSERT(r->textura->estadoCarga() == ENTexturaEstadoCarga_Precargada)
						} else if(modoCargaTexturas == ENGestorTexturaModo_cargaSegundoPlano && _texLoadThreadMode == ENTexturaModoHilo_MultiHilo){
							PRINTF_INFO("TEX cargando JPEG (carga en segundo plano): '%s'\n", filepath);
							r = NBGestorTexturas::privTexturaRegistrar(iAmbitoTexturas, true /*store into atlasGL*/, true /*init loading lines state*/, state, ENTexturaOrigenFormato_Jpg, props, NULL, NULL, inOptPreloadedTex);
							r->textura->privEstablecerEstadoCarga(ENTexturaEstadoCarga_Cargando);
							releaseLoadingState = false;
						} else /*if(modoCargaTexturas == ENGestorTexturaModo_cargaInmediata)*/{
							PRINTF_INFO("TEX cargando JPEG (carga inmediata): '%s'\n", filepath);
							STGestorTexAtlas* atlasTex = NULL;
							r = NBGestorTexturas::privTexturaRegistrar(iAmbitoTexturas, true /*store into atlasGL*/, false /*do not init loading lines state*/, state, ENTexturaOrigenFormato_Jpg, props, NULL, &atlasTex, inOptPreloadedTex);
							NBASSERT(r != NULL)
							NBGestorTexturas::privTexturaCargarDatosStreamLocked(atlasTex, r, state);
							NBASSERT(r->textura->estadoCarga() == ENTexturaEstadoCarga_Cargada)
							/*} else {
							 PRINTF_ERROR("GestorTexturas modo-carga desconocido\n");
							 NBASSERT(false)*/
						}
					}
				}
			}
			if(releaseLoadingState){
				NBGestorTexturas::privLoadingStateRelease(state);
				NBGestorMemoria::liberarMemoria(state);
				state = NULL;
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return r;
}

STNBPngChunk* NBGestorTexturas_chunkById(const char* uid, STNBArray* extraChunks){
	STNBPngChunk* r = NULL;
	if(extraChunks != NULL){
		SI32 i; for(i = 0; i < extraChunks->use; i++){
			STNBPngChunk* c = NBArray_itmPtrAtIndex(extraChunks, STNBPngChunk, i);
			if(NBString_strIsEqual(uid, c->name)){
				r = c;
				break;
			}
		}
	}
	return r;
}

STGestorTexTextura* NBGestorTexturas::privTexturaRegistrar(UI8 iAmbitoTexturas, const bool reservarEnAtlasGL, const bool inicializarEstadoCargaLineas, STGestoTexLoadingBase* state, const ENTexturaOrigenFormato formato, const MapaDeBitsDesciptor propsMapaBits, STNBArray* extraChunks, STGestorTexAtlas** outAtlasTex, STGestorTexTextura* inOptPreloadedTex){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTexturas::privTexturaRegistrar")
	NBASSERT(!_bloqueado)
	NBASSERT(iAmbitoTexturas < GESTOR_TEXTURAS_MAX_AMBITOS_TEXTURAS)
	NBASSERT(_ambitosAtlases[iAmbitoTexturas].ambitoReservado)
	NBASSERT(_ambitosAtlases[iAmbitoTexturas].atlasesTexturas != NULL)
	STGestorTexTextura* r = NULL;
	if(outAtlasTex != NULL) *outAtlasTex = NULL;
	//Verify color, scaleBase2 and pixelPrecision
	ENTexturaModoPintado modoPintado = state->modoPintado;
	UI8 escalaBase2 = state->escalaBase2;
	MapaBitsColor color = (MapaBitsColor)propsMapaBits.color;
	if(extraChunks != NULL){
		STNBPngChunk* chunckBP = NBGestorTexturas_chunkById("nbBP", extraChunks); //NicaraguaBinaryBitmapProps
		if(chunckBP != NULL){
			AUMapaBitsProps* bitmapProps = new(ENMemoriaTipo_Temporal) AUMapaBitsProps();
			AUArchivoEnBufferExterno* flujo = new(ENMemoriaTipo_Temporal) AUArchivoEnBufferExterno((const char*)chunckBP->data, chunckBP->dataSz);
			flujo->lock();
			if(!AUMapaBitsProps::cargarXmlEn(bitmapProps, flujo)){
				PRINTF_ERROR("Could not load bitmapProps from png data chunck: (%d bytes).\n", chunckBP->dataSz);
			} else {
				//Verify scaleBase2
				escalaBase2 = bitmapProps->escalaBase2();
				//Prefered pixelPrecision
				if(bitmapProps->precisionPixelPrefererida()){
					if(modoPintado == ENTexturaModoPintado_Imagen_Suavizada){
						modoPintado = ENTexturaModoPintado_Imagen_Precisa;
					} else if(modoPintado == ENTexturaModoPintado_Patron_Suavizado){
						modoPintado = ENTexturaModoPintado_Patron_Preciso;
					}
				}
				//Prefered color mode
				if(color == COLOR_GRIS8 && bitmapProps->mascaraAlphaPreferido()){
					color = COLOR_ALPHA8;
				}
			}
			flujo->unlock();
			flujo->liberar(NB_RETENEDOR_NULL);
			bitmapProps->liberar(NB_RETENEDOR_NULL);
		}
	}
	//
	const GLenum formatoGlLocal = NBGestorTexturas::formatoGlDriver(color); NBASSERT(formatoGlLocal != 0)
	const GLenum tpoDatoGlLocal = NBGestorTexturas::tipoDatoGlLocal(color); NBASSERT(tpoDatoGlLocal != 0)
	if(formatoGlLocal == 0 || tpoDatoGlLocal == 0){
		PRINTF_ERROR("NBGestorTexturas, el formatoGl(%d) o tpoDatoGlDriver(%d) no pudo definirse\n", formatoGlLocal, tpoDatoGlLocal);
		NBASSERT(false)
	} else {
		state->formatoGlLocal		= formatoGlLocal;
		state->tpoDatoGlLocal		= tpoDatoGlLocal;
		SI32 maxDimensionTextura	= NBGestorGL::maximaDimensionTextura();
		NBASSERT(propsMapaBits.ancho > 0 && propsMapaBits.alto > 0 && propsMapaBits.ancho <= maxDimensionTextura && propsMapaBits.alto <= maxDimensionTextura)
		if(!(propsMapaBits.ancho > 0 && propsMapaBits.alto > 0 && propsMapaBits.ancho <= maxDimensionTextura && propsMapaBits.alto <= maxDimensionTextura)){
			PRINTF_ERROR("NBGestorTexturas, las dimensiones del mapa de bits no son validas para un atlas (%d, %d)\n", propsMapaBits.ancho, propsMapaBits.alto);
			NBASSERT(false)
		} else {
			STGestorTexAmbito* ambTex = &_ambitosAtlases[iAmbitoTexturas];
			SI32 indiceAtlas = -1; NBRectanguloUI16 areaEnAtlas;
			if(reservarEnAtlasGL){
				//Intentar reservar area en atlas existentes
				if(state->tipoAlmacenamiento == ENTexturaTipoAlmacenamientoGL_AtlasCompartido){
					if(NBGestorTexturas::privReservaAreaEnAtlas(iAmbitoTexturas, state->enListarConNombre->str(), state->nomTamano, propsMapaBits.ancho, propsMapaBits.alto, color, state->tipoUso, state->modoPintado, &indiceAtlas, &areaEnAtlas)){
						NBASSERT(indiceAtlas >= 0 && indiceAtlas < ambTex->atlasesTexturas->conteo)
						if(outAtlasTex != NULL) *outAtlasTex = ambTex->atlasesTexturas->elem(indiceAtlas);
					}
				}
			} else {
				//Buscar el atlas para texturas precargadas
				if(NBGestorTexturas::privReservaAreaEnAtlasDePrecarga(iAmbitoTexturas, &indiceAtlas)){
					NBASSERT(indiceAtlas >= 0 && indiceAtlas < ambTex->atlasesTexturas->conteo)
					if(outAtlasTex != NULL) *outAtlasTex = ambTex->atlasesTexturas->elem(indiceAtlas);
					areaEnAtlas.x		= 0;
					areaEnAtlas.y		= 0;
					areaEnAtlas.ancho	= propsMapaBits.ancho;
					areaEnAtlas.alto	= propsMapaBits.alto;
				}
			}
			//Intentar crear nuevo atlas
			if(indiceAtlas == -1){
				if(reservarEnAtlasGL){
					NBTamanoI tamanoNuevoAtlas;
					if(state->tipoAlmacenamiento == ENTexturaTipoAlmacenamientoGL_AtlasCompartido){
						SI32 maxDimensionSolicitada	= propsMapaBits.ancho > propsMapaBits.alto ? propsMapaBits.ancho : propsMapaBits.alto;
						SI32 maxDimensionRecomenada = (color == COLOR_RGBA8 || color == COLOR_ARGB8 || color == COLOR_BGRA8 || color == COLOR_GRIS8) ? ambTex->tamanosTexturasPrincipales : ambTex->tamanosTexturasSecundarias;
						SI32 maxDimensionAtlas		= maxDimensionRecomenada;
						if(maxDimensionSolicitada>maxDimensionAtlas){ maxDimensionAtlas		= 64.0f; while(maxDimensionAtlas<maxDimensionSolicitada) maxDimensionAtlas *= 2; }
						tamanoNuevoAtlas.ancho		= maxDimensionAtlas;
						tamanoNuevoAtlas.alto		= maxDimensionAtlas;
					} else {
						tamanoNuevoAtlas.ancho		= propsMapaBits.ancho;
						tamanoNuevoAtlas.alto		= propsMapaBits.alto;
					}
					//crear nuevo atlas
					if(!NBGestorGL::soportaTexturas2DNoMultiploDe2() && !NBGestorTexturas::tamanoTexturaEsCuadradaMultiploDe2Valido(tamanoNuevoAtlas.ancho, tamanoNuevoAtlas.alto)){
						PRINTF_ERROR("NBGestorTexturas, no se permiten atlas no-base-dos (%d x %d) y tamano menor o igual al limite de implementacion GL para '%s'.\n", tamanoNuevoAtlas.ancho, tamanoNuevoAtlas.alto, state->enListarConNombre->str());
						NBASSERT(false)
					} else {
						indiceAtlas = NBGestorTexturas::privCrearAtlasTexturaGL(iAmbitoTexturas, tamanoNuevoAtlas.ancho, tamanoNuevoAtlas.alto, color, state->tipoAlmacenamiento, state->tipoUso, modoPintado, state->habilitarMipMap);
						if(indiceAtlas == -1){
							PRINTF_ERROR("NBGestorTexturas, no se pudo crear el AtlasGL para '%s'.\n", state->enListarConNombre->str());
							NBASSERT(false)
						} else {
							//PRINTF_INFO("Atlas creado: %d x %d (para satisfacer recurso a registrar '%s')\n", tamanoNuevoAtlas.ancho, tamanoNuevoAtlas.alto, state->enListarConNombre->str());
							//reservar en este nuevo atlas
							STGestorTexAtlas* atlasTex = ambTex->atlasesTexturas->elem(indiceAtlas);
							NBASSERT(atlasTex->mapaAtlas != NULL)
							areaEnAtlas = atlasTex->mapaAtlas->reservaEspacioConNombre(propsMapaBits.ancho, propsMapaBits.alto, NBGESTORTEXTURAS_PIX_SEPARACION_ENTRE_TEXTURAS, NBGESTORTEXTURAS_PIX_SEPARACION_ENTRE_TEXTURAS, state->enListarConNombre->str(), state->nomTamano);
							if(outAtlasTex != NULL) *outAtlasTex = atlasTex;
						}
					}
				} else {
					STGestorTexAtlas* descAtlas	= (STGestorTexAtlas*)NBGestorMemoria::reservarMemoria(sizeof(STGestorTexAtlas), ENMemoriaTipo_General);
					descAtlas->retainCount				= 1;
					descAtlas->propsTexturaGL.idTexturaGL= 0;
					descAtlas->propsTexturaGL.anchoGL	= 0;
					descAtlas->propsTexturaGL.altoGL	= 0;
					descAtlas->propsTexturaGL.mipMapHabilitado = false;
					descAtlas->propsTexturaGL.mipMapSucio = false;
					descAtlas->colorFormato				= color;
					descAtlas->tipoAlmacenamientoGL		= ENTexturaTipoAlmacenamientoGL_AtlasCompartido;
					descAtlas->tipoUsoGL				= ENTExturaTipoUsoGL_Lectura;
					descAtlas->modoPintado				= ENTexturaModoPintado_Imagen_Precisa;
					descAtlas->mapaAtlas				= NULL;
					descAtlas->texturasEnMapa			= new(ENMemoriaTipo_General) AUArregloNativoMutableP<STGestorTexTextura*>(); NB_DEFINE_NOMBRE_PUNTERO(descAtlas->texturasEnMapa, "NBGestorTexturas::descAtlas->texturasEnMapa")
					//NBHILO_MUTEX_INICIALIZAR(&descAtlas->texturasEnMapaMutex);
					//
					//NBHILO_MUTEX_ACTIVAR(&ambTex->atlasesTexturasMutex)
					ambTex->atlasesTexturas->agregarElemento(descAtlas);
					indiceAtlas = (ambTex->atlasesTexturas->conteo - 1);
					//NBHILO_MUTEX_DESACTIVAR(&ambTex->atlasesTexturasMutex)
					//
					areaEnAtlas.x		= 0;
					areaEnAtlas.y		= 0;
					areaEnAtlas.ancho	= propsMapaBits.ancho;
					areaEnAtlas.alto	= propsMapaBits.alto;
				}
			}
			//Nueva area reservada?
			if(indiceAtlas != -1){
				STGestorTexAtlas* atlasTex = ambTex->atlasesTexturas->elem(indiceAtlas);
				NBTamanoP<UI16> tamanoAtlas; tamanoAtlas.ancho = tamanoAtlas.alto = 1;
				if(atlasTex->mapaAtlas != NULL){
					tamanoAtlas = atlasTex->mapaAtlas->tamano();
				}
				//Try to load bitmap's local meshMap
				AUMallaMapa* textureMesh		= NULL;
				if(extraChunks != NULL){
					STNBPngChunk* chunckMH = NBGestorTexturas_chunkById("nbMH", extraChunks); //NicaraguaBinaryMesH
					if(chunckMH != NULL){
						AUMallaMapa* localMesh		= new(ENMemoriaTipo_General) AUMallaMapa(1);
						AUArchivoEnBufferExterno* flujo	= new(ENMemoriaTipo_Temporal) AUArchivoEnBufferExterno((const char*)chunckMH->data, chunckMH->dataSz);
						flujo->lock();
						if(AUMallaMapa::cargarXmlEn(localMesh, flujo)){
							textureMesh = localMesh; localMesh->retener(NB_RETENEDOR_NULL);
						}
						flujo->unlock();
						flujo->liberar(NB_RETENEDOR_NULL);
						localMesh->liberar(NB_RETENEDOR_NULL);
					}
				}
				//Texture-instance
				STGestorTexTextura* datosTextura = (STGestorTexTextura*)NBGestorMemoria::reservarMemoria(sizeof(STGestorTexTextura), ENMemoriaTipo_General);
				datosTextura->retainCount			= 1;
				datosTextura->textura				= NULL;
				datosTextura->subTexturasMapa		= NULL;
				datosTextura->subTexturas			= NULL;
				if(inOptPreloadedTex != NULL){
					if(inOptPreloadedTex->textura != NULL){
						datosTextura->textura		= inOptPreloadedTex->textura; inOptPreloadedTex->textura->retener(NB_RETENEDOR_NULL);
						datosTextura->textura->privInicializar(atlasTex->propsTexturaGL.idTexturaGL, state->ordenVTextura, ENTexturaEstadoCarga_Precargada, tamanoAtlas.ancho, tamanoAtlas.alto, areaEnAtlas.x, areaEnAtlas.y, areaEnAtlas.ancho, areaEnAtlas.alto, escalaBase2, state->escalaParaHD, modoPintado, state->enListarConNombre->str(), textureMesh);
						//PRINTF_INFO("Preload, TexturaFromPNGState reinicializada tamAtlas(%d, %d) area(%d, %d, %d, %d) escalaBase2(%d) escalaHD(%.2f).\n", tamanoAtlas.ancho, tamanoAtlas.alto, areaEnAtlas.x, areaEnAtlas.y, areaEnAtlas.ancho, areaEnAtlas.alto, escalaBase2, escalaParaHD);
					}
					if(inOptPreloadedTex->subTexturasMapa != NULL){
						datosTextura->subTexturasMapa = inOptPreloadedTex->subTexturasMapa; inOptPreloadedTex->subTexturasMapa->retener(NB_RETENEDOR_NULL);
						//PRINTF_INFO("Preload, TexturaFromPNGState mapa de subtexturas reutilizado.\n");
					}
					if(inOptPreloadedTex->subTexturas != NULL){
						datosTextura->subTexturas	 = inOptPreloadedTex->subTexturas; inOptPreloadedTex->subTexturas->retener(NB_RETENEDOR_NULL);
						SI32 i; const SI32 conteo = datosTextura->subTexturas->conteo;
						for(i = 0; i < conteo; i++){
							AUTextura* subtextura = (AUTextura*)datosTextura->subTexturas->elem(i);
							subtextura->privInicializar(atlasTex->propsTexturaGL.idTexturaGL, state->ordenVTextura, ENTexturaEstadoCarga_Precargada, tamanoAtlas.ancho, tamanoAtlas.alto, areaEnAtlas.x, areaEnAtlas.y, areaEnAtlas.ancho, areaEnAtlas.alto, escalaBase2, state->escalaParaHD, modoPintado, state->enListarConNombre->str(), NULL);
						}
						//PRINTF_INFO("Preload, TexturaFromPNGState %d subtexturas reinicializadas.\n", conteo);
						/*if(inOptPreloadedTex->subTexturasMapa == NULL){
							for(i = 0; i < conteo; i++){
						 AUTextura* subTex = (AUTextura*)inOptPreloadedTex->subTexturas->elem(i);
						 subTex->privInicializar(0, ENTexturaOrdenV_HaciaArriba, ENTexturaEstadoCarga_Precargada, tamanoAtlas.ancho, tamanoAtlas.alto, 0, 0, 0, 0, state->escalaBase2, state->escalaParaHD, state->modoPintado, NULL);
							}
						 } else {
							const AUArregloNativoP<STAtlasArea>* localAreas = inOptPreloadedTex->subTexturasMapa->areasOcupadas();
							NBASSERT(localAreas->conteo == inOptPreloadedTex->subTexturas->conteo)
							if(localAreas->conteo == inOptPreloadedTex->subTexturas->conteo){
						 SI32 i; const SI32 count = localAreas->conteo;
						 for(i = 0; i < count; i++){
						 const STAtlasArea* locArea = localAreas->elemPtr(i);
						 AUTextura* subTex = (AUTextura*)inOptPreloadedTex->subTexturas->elem(i);
						 subTex->privInicializar(atlasTex->propsTexturaGL.idTexturaGL, state->ordenVTextura, ENTexturaEstadoCarga_Precargada, tamanoAtlas.ancho, tamanoAtlas.alto, areaEnAtlas.x + locArea->area.x, areaEnAtlas.y + locArea->area.y, locArea->area.ancho, locArea->area.alto, state->escalaBase2, state->escalaParaHD, state->modoPintado, NULL);
						 }
							}
						 }*/
					}
				}
				if(datosTextura->textura == NULL){
					datosTextura->textura			= new(ENMemoriaTipo_Nucleo) AUTextura(atlasTex->propsTexturaGL.idTexturaGL, state->ordenVTextura, ENTexturaEstadoCarga_Precargada, tamanoAtlas.ancho, tamanoAtlas.alto, areaEnAtlas.x, areaEnAtlas.y, areaEnAtlas.ancho, areaEnAtlas.alto, escalaBase2, state->escalaParaHD, modoPintado, state->enListarConNombre->str(), textureMesh); NB_DEFINE_NOMBRE_PUNTERO(datosTextura->textura, state->enListarConNombre->str()/*"NBGestorTexturas::datosTextura->textura"*/)
					//PRINTF_INFO("TexturaFromPNGState generada tamAtlas(%d, %d) area(%d, %d, %d, %d) escalaBase2(%d) escalaHD(%.2f).\n", tamanoAtlas.ancho, tamanoAtlas.alto, areaEnAtlas.x, areaEnAtlas.y, areaEnAtlas.ancho, areaEnAtlas.alto, escalaBase2, escalaParaHD);
				}
				//
				datosTextura->tipoAlmacenamientoGL	= state->tipoAlmacenamiento;
				datosTextura->tipoUsoGL				= state->tipoUso;
				datosTextura->tipoPintado			= modoPintado;
				datosTextura->mipMapHabiltado		= state->habilitarMipMap;
				datosTextura->codigoTextura			= 0;
				datosTextura->origenTextura			= state->origenTextura;
				datosTextura->origenFormato			= formato;
				datosTextura->escalaBase2			= escalaBase2;
				datosTextura->escalaParaHD			= state->escalaParaHD;
				datosTextura->nombreTextura			= new(ENMemoriaTipo_General) AUCadena8(state->enListarConNombre->str(), state->nomTamano); NB_DEFINE_NOMBRE_PUNTERO(datosTextura->nombreTextura, "NBGestorTexturas::datosTextura->nombreTextura")
				datosTextura->loadingState			= NULL;
				if(inicializarEstadoCargaLineas){
					NBASSERT(!state->linesLoadingBuffersInited)
					state->linesLoadingBuffersInited = true;
					state->linesLoadingEnded		= false;
					state->linesAlreadyPasted		= 0;
					state->linesAlreadyLoaded		= 0;
					NBHILO_MUTEX_INICIALIZAR(&state->linesBuffersMutex)
					state->linesBuffers				= new(ENMemoriaTipo_General) AUArregloNativoMutableP<STGestoTexLoadingBuffer>();
					datosTextura->loadingState		= state;
				}
				//Try to load bitmap's local atlas
				if(extraChunks != NULL){
					STNBPngChunk* chunckAT = NBGestorTexturas_chunkById("nbAT", extraChunks); //NicaraguaBinaryMesH
					NBASSERT(state->enListarConNombre->str()[state->nomTamano] == '\0' || chunckAT != NULL)
					if(chunckAT != NULL){
						AUAtlasMapaMutable* atlasNvo = new(ENMemoriaTipo_General) AUAtlasMapaMutable(0, 0, 0);
						AUArchivoEnBufferExterno* flujo = new(ENMemoriaTipo_Temporal) AUArchivoEnBufferExterno((const char*)chunckAT->data, chunckAT->dataSz);
						flujo->lock();
						if(AUAtlasMapa::cargarXmlEn(atlasNvo, flujo)){
							const float mapMultiplier			= (float)escalaBase2 / (float)atlasNvo->escalaBase2();
							AUAtlasMapaMutable* atlasOriginal	= datosTextura->subTexturasMapa;
							AUArregloMutable* arrSubtexsOriginal= datosTextura->subTexturas;
							AUArregloMutable* arrSubtexsNvo		= new(ENMemoriaTipo_General) AUArregloMutable();
							//
							const AUArregloNativoP<STAtlasArea>* localAreas = atlasNvo->areasOcupadas();
							SI32 i; const SI32 count = localAreas->conteo;
							for(i = 0; i < count; i++){
								const STAtlasArea* locArea = localAreas->elemPtr(i);
								AUTextura* subTex = NULL;
								//Intentar reutilizar el objeto textura original
								if(atlasOriginal != NULL){
									NBASSERT(atlasOriginal->areasOcupadas()->conteo == atlasNvo->areasOcupadas()->conteo)
									NBASSERT(arrSubtexsOriginal != NULL)
									SI32 iSubtexOrig = i; //Same index by default.
									if(locArea->nombre != NULL){ //Verify index when name is provided.
										iSubtexOrig = atlasOriginal->indiceAreaConNombre(locArea->nombre->str());
										NBASSERT(iSubtexOrig == i) //Tmp: just testing behavior
									}
									if(iSubtexOrig >= 0){
										NBASSERT(iSubtexOrig >= 0 && iSubtexOrig < arrSubtexsOriginal->conteo)
										subTex = (AUTextura*)arrSubtexsOriginal->elem(iSubtexOrig);
										subTex->privInicializar(atlasTex->propsTexturaGL.idTexturaGL, state->ordenVTextura, ENTexturaEstadoCarga_Precargada, tamanoAtlas.ancho, tamanoAtlas.alto, areaEnAtlas.x + (locArea->area.x * mapMultiplier), areaEnAtlas.y + (locArea->area.y * mapMultiplier), locArea->area.ancho * mapMultiplier, locArea->area.alto * mapMultiplier, escalaBase2, state->escalaParaHD, modoPintado, state->enListarConNombre->str(), locArea->mallaMapa);
										arrSubtexsNvo->agregarElemento(subTex);
									} else {
										NBASSERT(false); //Temporal, no siempre debe cumplirse. Borrar cuando sea necesario.
									}
								}
								//Genera nueva textura
								if(subTex == NULL){
									subTex = new(ENMemoriaTipo_Nucleo) AUTextura(atlasTex->propsTexturaGL.idTexturaGL, state->ordenVTextura, ENTexturaEstadoCarga_Precargada, tamanoAtlas.ancho, tamanoAtlas.alto, areaEnAtlas.x + (locArea->area.x * mapMultiplier), areaEnAtlas.y + (locArea->area.y * mapMultiplier), locArea->area.ancho * mapMultiplier, locArea->area.alto * mapMultiplier, escalaBase2, state->escalaParaHD, modoPintado, state->enListarConNombre->str(), locArea->mallaMapa); NB_DEFINE_NOMBRE_PUNTERO(subTex, "NBGestorTexturas::datosTextura->textura.subTex")
									arrSubtexsNvo->agregarElemento(subTex);
									subTex->liberar(NB_RETENEDOR_NULL);
								}
							}
							//
							NBASSERT(atlasNvo->areasOcupadas()->conteo == arrSubtexsNvo->conteo)
							datosTextura->subTexturasMapa	= atlasNvo; atlasNvo->retener(NB_RETENEDOR_NULL);
							datosTextura->subTexturas		= arrSubtexsNvo;
							if(atlasOriginal != NULL) atlasOriginal->liberar(NB_RETENEDOR_NULL);
							if(arrSubtexsOriginal != NULL)arrSubtexsOriginal->liberar(NB_RETENEDOR_NULL);
						}
						flujo->unlock();
						flujo->liberar(NB_RETENEDOR_NULL);
						atlasNvo->liberar(NB_RETENEDOR_NULL);
					}
				}
				//NBHILO_MUTEX_ACTIVAR(&atlasTex->texturasEnMapaMutex);
				atlasTex->texturasEnMapa->agregarElemento(datosTextura);
				r = datosTextura;
				//NBHILO_MUTEX_DESACTIVAR(&atlasTex->texturasEnMapaMutex);
				if(!reservarEnAtlasGL){
					STGestorTexTexturaRef texRef;
					texRef.iAmb		= iAmbitoTexturas;
					texRef.atlas	= atlasTex; NBGestorTexturas::privSTGestorTexAtlasRetain(texRef.atlas);
					texRef.texture	= datosTextura; NBGestorTexturas::privSTGestorTexTexturaRetain(texRef.texture);
					NBHILO_MUTEX_ACTIVAR(&_texUnorganizedRefsMutex);
					_texUnorganizedRefs->agregarElemento(texRef);
					NBHILO_MUTEX_DESACTIVAR(&_texUnorganizedRefsMutex);
				}
				if(inicializarEstadoCargaLineas){
					NBASSERT(datosTextura->loadingState != NULL)
					STGestorTexTexturaRef* texRef = (STGestorTexTexturaRef*)NBGestorMemoria::reservarMemoria(sizeof(STGestorTexTexturaRef), ENMemoriaTipo_General);
					texRef->iAmb	= iAmbitoTexturas;
					texRef->atlas	= atlasTex; NBGestorTexturas::privSTGestorTexAtlasRetain(texRef->atlas);
					texRef->texture	= datosTextura; NBGestorTexturas::privSTGestorTexTexturaRetain(texRef->texture);
					NBHILO_MUTEX_ACTIVAR(&_texLoadingRefsMutex);
					_texLoadingRefs->agregarElemento(texRef);
					NBHILO_MUTEX_DESACTIVAR(&_texLoadingRefsMutex);
				}
				//
				if(textureMesh != NULL) textureMesh->liberar(NB_RETENEDOR_NULL);
			}
			//NBGestorTexturas::volcarAltasHaciaArchivos();
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return r;
}

bool NBGestorTexturas::privTexturaCargarDatosStreamLocked(STGestorTexAtlas* atlasTex, STGestorTexTextura* texture, STGestoTexLoadingBase* state){
	bool r = false;
	NBASSERT(state != NULL)
	STGestoTexLoadingBase* stateBase = state;
	if(stateBase->type == ENGestorTexturasLoadingType_Png){
		STGestoTexLoadingPNG* state = (STGestoTexLoadingPNG*)stateBase;
		const STNBBitmapProps props = NBPngLoadState_getProps(&state->load);
		NBASSERT(props.size.width > 0 && props.size.height > 0 && props.bytesPerLine > 0)
		{
			const UI32 pixsSz = (props.size.height * props.bytesPerLine);
			BYTE* pixs = (BYTE*)NBMemory_alloc(pixsSz);
			{
				//Note: already locked by caller
				if(!NBPng_loadWithState(&state->load, pixs, pixsSz, &state->extraChunks, NULL, NULL)){
					//
				} else {
					const NBRectanguloUI16 areaEnAtlas = texture->textura->areaEnAtlas(); NBASSERT(areaEnAtlas.ancho > 0 && areaEnAtlas.alto > 0)
					NBGestorGL::bindTexture(0, (GLuint)atlasTex->propsTexturaGL.idTexturaGL);
					NBGestorGL::texSubImage2D(GL_TEXTURE_2D, 0, areaEnAtlas.x, areaEnAtlas.y, areaEnAtlas.ancho, props.size.height, state->formatoGlLocal, state->tpoDatoGlLocal, pixs, &atlasTex->propsTexturaGL); //GL_UNSIGNED_BYTE
					_texsChangesCount++;
				}
			}
			//Release bufer (if not consumed)
			if(pixs != NULL){
				NBMemory_free(pixs);
				pixs = NULL;
			}
			state->linesLoadingEnded = true;;
			texture->textura->privEstablecerEstadoCarga(ENTexturaEstadoCarga_Cargada);
		}
		//
		//
		//
		//ToDo: remove
		/* //Definir la cantidad de lineas por buffer
		const UI32 bmpDatosTamano				= (state->estado.propsMapaBits.bytesPorLinea * state->estado.propsMapaBits.alto);
		const UI32 tamMaxBuffer					= NBGESTORTEXTURAS_CARGA_MAX_BYTES_POR_BUFFER;
		const float cantMaxBuffersReq			= (float)bmpDatosTamano / (float)tamMaxBuffer;
		const UI32 lineasPorBuffer				= (cantMaxBuffersReq < 1.0f ? state->estado.propsMapaBits.alto : (tamMaxBuffer / state->estado.propsMapaBits.bytesPorLinea) < 1 ? 1 : (tamMaxBuffer / state->estado.propsMapaBits.bytesPorLinea));
		const UI32 bufferDestinoTamano			= (state->estado.propsMapaBits.bytesPorLinea * lineasPorBuffer);	NBASSERT(bufferDestinoTamano != 0)
		const UI32 bufferEntrelazadoTamano		= (1 + (state->estado.ancho * state->estado.bytesPorPixelPNG)) * lineasPorBuffer; NBASSERT(bufferEntrelazadoTamano != 0) //+1 para el entrelazado
		if(state->estado.bufferDestinoTamano != bufferDestinoTamano){
			if(state->estado.bufferDestino != NULL) NBGestorMemoria::liberarMemoria(state->estado.bufferDestino);
			state->estado.bufferDestinoTamano	= bufferDestinoTamano;
			state->estado.bufferDestino			= (BYTE*)NBGestorMemoria::reservarMemoria(state->estado.bufferDestinoTamano, ENMemoriaTipo_Temporal);
		}
		if(state->estado.bufferEntrelazadoTamano != bufferEntrelazadoTamano){
			if(state->estado.bufferEntrelazado != NULL) NBGestorMemoria::liberarMemoria(state->estado.bufferEntrelazado);
			state->estado.bufferEntrelazadoTamano = bufferEntrelazadoTamano;
			state->estado.bufferEntrelazado		= (BYTE*)NBGestorMemoria::reservarMemoria(state->estado.bufferEntrelazadoTamano, ENMemoriaTipo_Temporal);
		}
		//
		const NBRectanguloUI16 areaEnAtlas = texture->textura->areaEnAtlas(); NBASSERT(areaEnAtlas.ancho > 0 && areaEnAtlas.alto > 0)
		NBGestorGL::bindTexture(0, (GLuint)atlasTex->propsTexturaGL.idTexturaGL);
		UI32 lineasTotalCargadas = 0;
		//PRINTF_WARNING("DBG-Temporal.\n");
		//AUMapaBits* dbgMapaBits	= new(ENMemoriaTipo_Temporal) AUMapaBits(state->estado.propsMapaBits.ancho, state->estado.propsMapaBits.alto, color, 0); NBASSERT(dbgMapaBits->propiedades().bytesPorLinea == state->estado.propsMapaBits.bytesPorLinea)
		//BYTE* dbgMapaBitsDatos	= dbgMapaBits->datos();
		r = true;
		while(!state->estado.IEND_leido){
			if(!AUMapaBitsMutable::cargarDesdeArchivoPNG(&state->estado, (STNBSha1Hash*)NULL, NBGestorTexturas_getExtraChunck, state->extraChunks)){
				NBASSERT(false)
				r = false;
				break;
			} else {
				if(state->estado.bufferDestinoPos != 0){
					NBASSERT((state->estado.bufferDestinoPos % state->estado.propsMapaBits.bytesPorLinea) == 0) //Se cargaron lineas completas
					NBASSERT((state->estado.bufferDestinoPos / state->estado.propsMapaBits.bytesPorLinea) == (state->estado.bufferEntrelazadoPos / (1 + (state->estado.ancho * state->estado.bytesPorPixelPNG)))) //Mismas cant de lineas en ambos bufferes
					const UI32 lineasCargadas = (state->estado.bufferDestinoPos / state->estado.propsMapaBits.bytesPorLinea); NBASSERT(lineasCargadas != 0)
					NBGestorGL::texSubImage2D(GL_TEXTURE_2D, 0, areaEnAtlas.x, areaEnAtlas.y + lineasTotalCargadas, areaEnAtlas.ancho, lineasCargadas, state->formatoGlLocal, state->tpoDatoGlLocal, state->estado.bufferDestino, &atlasTex->propsTexturaGL); //GL_UNSIGNED_BYTE
					_texsChangesCount++;
					if(state->calcularFirmaSHA1){
						NBSha1_feed(&state->sha1, state->estado.bufferDestino, state->estado.bufferDestinoPos);
					}
					//memcpy(&dbgMapaBitsDatos[lineasTotalCargadas * state->estado.propsMapaBits.bytesPorLinea], state->estado.bufferDestino, state->estado.bufferDestinoPos);
					state->estado.bufferEntrelazadoPos = 0; //Reiniciar el buffer de entrelazado
					state->estado.bufferDestinoPos = 0;
					lineasTotalCargadas += lineasCargadas;
				}
			}
		}
		if(state->calcularFirmaSHA1){
			if(NBSha1_feedEnd(&state->sha1)){
				state->firmaEnCarga = state->sha1.hash;
			}
		}
		texture->textura->privEstablecerEstadoCarga(ENTexturaEstadoCarga_Cargada);
		//
		if(state->estado.bufferDestino != NULL) NBGestorMemoria::liberarMemoria(state->estado.bufferDestino); state->estado.bufferDestino = NULL;
		if(state->estado.bufferEntrelazado != NULL) NBGestorMemoria::liberarMemoria(state->estado.bufferEntrelazado); state->estado.bufferEntrelazado = NULL;*/
	} else if(stateBase->type == ENGestorTexturasLoadingType_Jpg){
		STGestoTexLoadingJPEG* state = (STGestoTexLoadingJPEG*)stateBase;
		MapaDeBitsDesciptor props;
		if(!AUMapaBitsMutable::jpegReadStateGetProps(state->estado, &props)){
			PRINTF_ERROR("no se pudo obtene propiedades del JPEG.\n"); //NBASSERT(false)
		} else {
			//Definir la cantidad de lineas por buffer
			const UI32 bmpDatosTamano			= (props.bytesPorLinea * props.alto);
			const UI32 tamMaxBuffer				= NBGESTORTEXTURAS_CARGA_MAX_BYTES_POR_BUFFER;
			const float cantMaxBuffersReq		= (float)bmpDatosTamano / (float)tamMaxBuffer;
			const UI32 lineasPorBuffer			= (cantMaxBuffersReq < 1.0f ? props.alto : (tamMaxBuffer / props.bytesPorLinea) < 1 ? 1 : (tamMaxBuffer / props.bytesPorLinea));
			const UI32 bufferDestinoTamano		= (props.bytesPorLinea * lineasPorBuffer);	NBASSERT(bufferDestinoTamano != 0)
			if(state->buffSz != bufferDestinoTamano){
				if(state->buff != NULL) NBGestorMemoria::liberarMemoria(state->buff);
				state->buffSz	= bufferDestinoTamano;
				state->buff		= (BYTE*)NBGestorMemoria::reservarMemoria(state->buffSz, ENMemoriaTipo_Temporal);
			}
			//
			const NBRectanguloUI16 areaEnAtlas = texture->textura->areaEnAtlas(); NBASSERT(areaEnAtlas.ancho > 0 && areaEnAtlas.alto > 0)
			NBGestorGL::bindTexture(0, (GLuint)atlasTex->propsTexturaGL.idTexturaGL);
			r = true;
			//Read all lines
			NBASSERT(state->linesAlreadyLoaded == 0)
			while(state->linesAlreadyLoaded < props.alto){
				//Fill buffer
				UI32 linesRead = 0;
				ENJpegReadResult readResult;
				while(ENJpegReadResult_partial == (readResult = AUMapaBitsMutable::jpegReadStateRead(state->estado, state->buff, state->buffSz, &linesRead))){
					//read more
				}
				//Process result
				if(readResult != ENJpegReadResult_end){
					NBASSERT(false);
					r = false;
					break;
				} else {
					NBASSERT(linesRead != 0)
					if(linesRead != 0){
						NBGestorGL::texSubImage2D(GL_TEXTURE_2D, 0, areaEnAtlas.x, areaEnAtlas.y + state->linesAlreadyLoaded, areaEnAtlas.ancho, linesRead, state->formatoGlLocal, state->tpoDatoGlLocal, state->buff, &atlasTex->propsTexturaGL); //GL_UNSIGNED_BYTE
						_texsChangesCount++;
						state->linesAlreadyLoaded += linesRead;
						NBASSERT(state->linesAlreadyLoaded <= props.alto)
					}
				}
			}
			NBASSERT(state->linesAlreadyLoaded == props.alto)
			texture->textura->privEstablecerEstadoCarga(ENTexturaEstadoCarga_Cargada);
			//Release buffer
			if(state->buff != NULL){
				NBGestorMemoria::liberarMemoria(state->buff);
				state->buff = NULL;
				state->buffSz = 0;
			}
		}
	} else  {
		NBASSERT(false)
	}
	return r;
}

//-------------------------
//-- Textura desde datos
//-------------------------

STGestorTexTextura* NBGestorTexturas::privTexturaDesdeMapaBits(UI8 iAmbitoTexturas, AUMapaBits* mapaBitsACargar, const char* enListarConNombre, ENTexturaTipoAlmacenamientoGL tipoAlmacenamiento, ENTexturaTipoUsoGL tipoUso, ENTexturaModoPintado modoPintado, ENTexturaMipMap habilitarMipMap, ENTexturaOrdenV ordenVTextura, STGestorTexTextura* inOptPreloadedTex, const UI8 escalaBase2, const float escalaParaHD){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTexturas::privTexturaDesdeMapaBits")
	//NBASSERT(false) //PENDIENTE: minimizar su uso gracias a la optimizacion de carga de streams de PNGs
	NBASSERT(!_bloqueado)
	NBASSERT(iAmbitoTexturas < GESTOR_TEXTURAS_MAX_AMBITOS_TEXTURAS)
	NBASSERT(_ambitosAtlases[iAmbitoTexturas].ambitoReservado)
	STGestorTexTextura* r = NULL;
	MapaDeBitsDesciptor propsMapaBitsCargar = mapaBitsACargar->propiedades();
	AUMapaBits* mapaBitsValidado = NULL;
	if(propsMapaBitsCargar.color == COLOR_GRISALPHA8){
		mapaBitsValidado = new(ENMemoriaTipo_Temporal) AUMapaBits(mapaBitsACargar, COLOR_RGBA8); NB_DEFINE_NOMBRE_PUNTERO(mapaBitsValidado, "NBGestorTexturas::mapaBitsValidado")
		PRINTF_WARNING("MAPA BITS: se ha obligado a convertir de 'GRISALPHA8' a 'RGBA8': '%s'\n", enListarConNombre);
	} else if(propsMapaBitsCargar.color == COLOR_RGB8){
		mapaBitsValidado = new(ENMemoriaTipo_Temporal) AUMapaBits(mapaBitsACargar, COLOR_RGBA8); NB_DEFINE_NOMBRE_PUNTERO(mapaBitsValidado, "NBGestorTexturas::mapaBitsValidado")
		PRINTF_WARNING("MAPA BITS: se ha obligado a convertir de 'RGB8' a 'RGBA8': '%s'\n", enListarConNombre);
	} else {
		mapaBitsValidado = mapaBitsACargar;
		mapaBitsValidado->retener(NB_RETENEDOR_NULL);
	}
	NBASSERT(mapaBitsValidado != NULL);
	MapaDeBitsDesciptor propsMapaBitsValidado = mapaBitsValidado->propiedades();
	r = NBGestorTexturas::privTexturaDesdeDatos(iAmbitoTexturas, propsMapaBitsValidado.ancho, propsMapaBitsValidado.alto, mapaBitsValidado->datos(), (MapaBitsColor)propsMapaBitsValidado.color, ENGestorTexturaOrigen_datos, escalaBase2, escalaParaHD, enListarConNombre, tipoAlmacenamiento, tipoUso, modoPintado, habilitarMipMap, ordenVTextura, inOptPreloadedTex);
	mapaBitsValidado->liberar(NB_RETENEDOR_NULL);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return r;
}

STGestorTexTextura* NBGestorTexturas::privTexturaDesdeDatos(UI8 iAmbitoTexturas, SI32 anchoMapaBits, SI32 altoMapaBits, const void* datosMapaBits, MapaBitsColor colorMapaBits, const ENGestorTexturaOrigen origenTextura, const UI8 escalaBase2, const float escalaParaHD, const char* enListarConNombre, ENTexturaTipoAlmacenamientoGL tipoAlmacenamiento, ENTexturaTipoUsoGL tipoUso, ENTexturaModoPintado modoPintado, ENTexturaMipMap habilitarMipMap, ENTexturaOrdenV ordenVTextura, STGestorTexTextura* inOptPreloadedTex){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTexturas::privTexturaDesdeDatos")
	//NBASSERT(false) //PENDIENTE: minimizar su uso gracias a la optimizacion de carga de streams de PNGs
	NBASSERT(!_bloqueado)
	NBASSERT(iAmbitoTexturas < GESTOR_TEXTURAS_MAX_AMBITOS_TEXTURAS)
	NBASSERT(_ambitosAtlases[iAmbitoTexturas].ambitoReservado)
	NBASSERT(_ambitosAtlases[iAmbitoTexturas].atlasesTexturas != NULL)
	STGestorTexTextura* r = NULL;
	GLenum formatoGlLocal = NBGestorTexturas::formatoGlDriver(colorMapaBits); NBASSERT(formatoGlLocal != 0)
	GLenum tpoDatoGlLocal = NBGestorTexturas::tipoDatoGlLocal(colorMapaBits); NBASSERT(tpoDatoGlLocal != 0)
	if(formatoGlLocal == 0 || tpoDatoGlLocal == 0){
		PRINTF_ERROR("NBGestorTexturas, el formatoGl(%d) o tpoDatoGlDriver(%d) no pudo definirse\n", formatoGlLocal, tpoDatoGlLocal);
		NBASSERT(false)
	} else {
		SI32 maxDimensionTextura = NBGestorGL::maximaDimensionTextura();
		NBASSERT(anchoMapaBits > 0 && altoMapaBits > 0 && anchoMapaBits <= maxDimensionTextura && altoMapaBits <= maxDimensionTextura)
		if(!(anchoMapaBits > 0 && altoMapaBits > 0 && anchoMapaBits<=maxDimensionTextura && altoMapaBits<=maxDimensionTextura)){
			PRINTF_ERROR("NBGestorTexturas, las dimensiones del mapa de bits no son validas para un atlas (%d, %d)\n", anchoMapaBits, altoMapaBits);
			NBASSERT(false)
		} else {
			STGestorTexAmbito* ambTex = &_ambitosAtlases[iAmbitoTexturas];
			SI32 indiceAtlas = -1;
			NBRectanguloUI16 areaEnAtlas;
			//if(tipoAlmacenamiento == ENTexturaTipoAlmacenamientoGL_AtlasUnico){
			//	PRINTF_INFO("CREANDO TEXTURA EXCLUSIVA TAMANO(%d, %d).\n", anchoMapaBits, altoMapaBits);
			//}
			//Intentar reservar area en atlas existentes
			if(tipoAlmacenamiento == ENTexturaTipoAlmacenamientoGL_AtlasCompartido){
				if(NBGestorTexturas::privReservaAreaEnAtlas(iAmbitoTexturas, enListarConNombre, AUCadena8::tamano(enListarConNombre), anchoMapaBits, altoMapaBits, colorMapaBits, tipoUso, modoPintado, &indiceAtlas, &areaEnAtlas)){
					if(datosMapaBits != NULL){
						NBASSERT(indiceAtlas >= 0 && indiceAtlas < ambTex->atlasesTexturas->conteo)
						STGestorTexAtlas* atlasTex = ambTex->atlasesTexturas->elem(indiceAtlas);
						NBGestorGL::bindTexture(0, (GLuint)atlasTex->propsTexturaGL.idTexturaGL);
						NBGestorGL::texSubImage2D(GL_TEXTURE_2D, 0, areaEnAtlas.x, areaEnAtlas.y, areaEnAtlas.ancho, areaEnAtlas.alto, formatoGlLocal, tpoDatoGlLocal, datosMapaBits, &atlasTex->propsTexturaGL); //GL_UNSIGNED_BYTE
						_texsChangesCount++;
						//PRINTF_INFO("Existia espacio en atlas para la nueva imagen '%s'.\n", enListarConNombre);
					}
				}
			}
			//Intentar crear nuevo atlas
			if(indiceAtlas == -1){
				const bool soportaTexturasNM2 = NBGestorGL::soportaTexturas2DNoMultiploDe2();
				NBTamanoI tamanoNuevoAtlas;
				if(tipoAlmacenamiento == ENTexturaTipoAlmacenamientoGL_AtlasCompartido || !soportaTexturasNM2){
					SI32 maxDimensionSolicitada	= anchoMapaBits > altoMapaBits ? anchoMapaBits : altoMapaBits;
					SI32 maxDimensionRecomenada = (colorMapaBits == COLOR_RGBA8 || colorMapaBits == COLOR_ARGB8 || colorMapaBits == COLOR_BGRA8 || colorMapaBits == COLOR_GRIS8) ? ambTex->tamanosTexturasPrincipales : ambTex->tamanosTexturasSecundarias;
					SI32 maxDimensionAtlas		= maxDimensionRecomenada;
					if(maxDimensionSolicitada > maxDimensionAtlas){ maxDimensionAtlas = 64.0f; while(maxDimensionAtlas < maxDimensionSolicitada) maxDimensionAtlas *= 2; }
					tamanoNuevoAtlas.ancho		= maxDimensionAtlas;
					tamanoNuevoAtlas.alto		= maxDimensionAtlas;
				} else {
					//Direct size (note: in some devices is not working, showing empty texture after rendering)
					//tamanoNuevoAtlas.ancho		= anchoMapaBits;
					//tamanoNuevoAtlas.alto		= altoMapaBits;
					//Min power-of-2 size
					SI32 minP2W					= 64;
					SI32 mixP2H					= 64;
					while(minP2W < anchoMapaBits) minP2W *= 2;
					while(mixP2H < altoMapaBits) mixP2H *= 2;
					tamanoNuevoAtlas.ancho		= minP2W;
					tamanoNuevoAtlas.alto		= mixP2H;
				}
				//crear nuevo atlas
				if(!soportaTexturasNM2 && !NBGestorTexturas::tamanoTexturaEsCuadradaMultiploDe2Valido(tamanoNuevoAtlas.ancho, tamanoNuevoAtlas.alto)){
					PRINTF_ERROR("NBGestorTexturas, no se permiten atlas no-base-dos (%d x %d) y tamano menor o igual al limite de implementacion GL para '%s'.\n", tamanoNuevoAtlas.ancho, tamanoNuevoAtlas.alto, enListarConNombre);
					NBASSERT(false)
				} else {
					//PRINTF_INFO("Creando atlas: %d x %d (para satisfacer recurso desde datos '%s')\n", tamanoNuevoAtlas.ancho, tamanoNuevoAtlas.alto, enListarConNombre);
					indiceAtlas = NBGestorTexturas::privCrearAtlasTexturaGL(iAmbitoTexturas, tamanoNuevoAtlas.ancho, tamanoNuevoAtlas.alto, colorMapaBits, tipoAlmacenamiento, tipoUso, modoPintado, habilitarMipMap);
					if(indiceAtlas == -1){
						PRINTF_ERROR("NBGestorTexturas, no se pudo crear el AtlasGL para '%s'.\n", enListarConNombre);
						NBASSERT(false)
					} else {
						//PRINTF_INFO("Atlas creado: %d x %d (para satisfacer recurso desde datos '%s')\n", tamanoNuevoAtlas.ancho, tamanoNuevoAtlas.alto, enListarConNombre);
						//reservar en este nuevo atlas
						STGestorTexAtlas* atlasTex = ambTex->atlasesTexturas->elem(indiceAtlas);
						NBASSERT(atlasTex->mapaAtlas != NULL)
						areaEnAtlas = atlasTex->mapaAtlas->reservaEspacioConNombre(anchoMapaBits, altoMapaBits, NBGESTORTEXTURAS_PIX_SEPARACION_ENTRE_TEXTURAS, NBGESTORTEXTURAS_PIX_SEPARACION_ENTRE_TEXTURAS, enListarConNombre);
						if(datosMapaBits != NULL){
							NBGestorGL::texSubImage2D(GL_TEXTURE_2D, 0, areaEnAtlas.x, areaEnAtlas.y, areaEnAtlas.ancho, areaEnAtlas.alto, formatoGlLocal, tpoDatoGlLocal, datosMapaBits, &atlasTex->propsTexturaGL); //GL_UNSIGNED_BYTE
							_texsChangesCount++;
						}
					}
				}
			}
			//Nueva area reservada?
			if(indiceAtlas != -1){
				STGestorTexAtlas* atlasTex		= ambTex->atlasesTexturas->elem(indiceAtlas); NBASSERT(atlasTex->mapaAtlas != NULL)
				NBTamanoP<UI16> tamanoAtlas			= atlasTex->mapaAtlas->tamano();
				STGestorTexTextura* datosTextura	= (STGestorTexTextura*)NBGestorMemoria::reservarMemoria(sizeof(STGestorTexTextura), ENMemoriaTipo_General);
				//Texture-instance
				datosTextura->retainCount			= 1;
				datosTextura->textura				= NULL;
				datosTextura->subTexturasMapa		= NULL;
				datosTextura->subTexturas			= NULL;
				if(inOptPreloadedTex != NULL){
					if(inOptPreloadedTex->textura != NULL){
						datosTextura->textura		= inOptPreloadedTex->textura; inOptPreloadedTex->textura->retener(NB_RETENEDOR_NULL);
						datosTextura->textura->privInicializar(atlasTex->propsTexturaGL.idTexturaGL, ordenVTextura, ENTexturaEstadoCarga_Cargada, tamanoAtlas.ancho, tamanoAtlas.alto, areaEnAtlas.x, areaEnAtlas.y, areaEnAtlas.ancho, areaEnAtlas.alto, escalaBase2, escalaParaHD, modoPintado, enListarConNombre, NULL);
						//PRINTF_INFO("TexturaFromDatos reinicializada tamAtlas(%d, %d) area(%d, %d, %d, %d) escalaBase2(%d) escalaHD(%.2f).\n", tamanoAtlas.ancho, tamanoAtlas.alto, areaEnAtlas.x, areaEnAtlas.y, areaEnAtlas.ancho, areaEnAtlas.alto, escalaBase2, escalaParaHD);
					}
					if(inOptPreloadedTex->subTexturasMapa){
						datosTextura->subTexturasMapa= inOptPreloadedTex->subTexturasMapa; inOptPreloadedTex->subTexturasMapa->retener(NB_RETENEDOR_NULL);
						PRINTF_INFO("TexturaFromDatos mapa de subtexturas reutilizado.\n");
					}
					if(inOptPreloadedTex->subTexturas != NULL){
						datosTextura->subTexturas	= inOptPreloadedTex->subTexturas; inOptPreloadedTex->subTexturas->retener(NB_RETENEDOR_NULL);
						SI32 i; const SI32 conteo = datosTextura->subTexturas->conteo;
						if(inOptPreloadedTex->subTexturasMapa == NULL){
							for(i = 0; i < conteo; i++){
								AUTextura* subTex = (AUTextura*)inOptPreloadedTex->subTexturas->elem(i);
								subTex->privInicializar(0, ENTexturaOrdenV_HaciaArriba, ENTexturaEstadoCarga_Cargada, tamanoAtlas.ancho, tamanoAtlas.alto, 0, 0, 0, 0, escalaBase2, escalaParaHD, modoPintado, enListarConNombre, NULL);
							}
						} else {
							const AUArregloNativoP<STAtlasArea>* localAreas = inOptPreloadedTex->subTexturasMapa->areasOcupadas();
							NBASSERT(localAreas->conteo == inOptPreloadedTex->subTexturas->conteo)
							if(localAreas->conteo == inOptPreloadedTex->subTexturas->conteo){
								SI32 i; const SI32 count = localAreas->conteo;
								for(i = 0; i < count; i++){
									const STAtlasArea* locArea = localAreas->elemPtr(i);
									AUTextura* subTex = (AUTextura*)inOptPreloadedTex->subTexturas->elem(i);
									subTex->privInicializar(atlasTex->propsTexturaGL.idTexturaGL, ordenVTextura, ENTexturaEstadoCarga_Cargada, tamanoAtlas.ancho, tamanoAtlas.alto, areaEnAtlas.x + locArea->area.x, areaEnAtlas.y + locArea->area.y, locArea->area.ancho, locArea->area.alto, escalaBase2, escalaParaHD, modoPintado, enListarConNombre, NULL);
								}
							}
						}
						
						PRINTF_INFO("TexturaFromDatos %d subtexturas reinicializadas.\n", conteo);
					}
				}
				if(datosTextura->textura == NULL){
					datosTextura->textura			= new(ENMemoriaTipo_Nucleo) AUTextura(atlasTex->propsTexturaGL.idTexturaGL, ordenVTextura, ENTexturaEstadoCarga_Cargada, tamanoAtlas.ancho, tamanoAtlas.alto, areaEnAtlas.x, areaEnAtlas.y, areaEnAtlas.ancho, areaEnAtlas.alto, escalaBase2, escalaParaHD, modoPintado, enListarConNombre, NULL); NB_DEFINE_NOMBRE_PUNTERO(datosTextura->textura, enListarConNombre/*"NBGestorTexturas::datosTextura->textura"*/)
					//PRINTF_INFO("TexturaFromDatos inicializada tamAtlas(%d, %d) area(%d, %d, %d, %d) escalaBase2(%d) escalaHD(%.2f).\n", tamanoAtlas.ancho, tamanoAtlas.alto, areaEnAtlas.x, areaEnAtlas.y, areaEnAtlas.ancho, areaEnAtlas.alto, escalaBase2, escalaParaHD);
				}
				//
				datosTextura->tipoAlmacenamientoGL	= tipoAlmacenamiento;
				datosTextura->tipoUsoGL				= tipoUso;
				datosTextura->tipoPintado			= modoPintado;
				datosTextura->mipMapHabiltado		= habilitarMipMap;
				datosTextura->codigoTextura			= 0;
				datosTextura->origenTextura			= origenTextura;
				datosTextura->origenFormato			= ENTexturaOrigenFormato_Conteo;
				datosTextura->escalaBase2			= escalaBase2;
				datosTextura->escalaParaHD			= escalaParaHD;
				datosTextura->nombreTextura			= new(ENMemoriaTipo_General) AUCadena8(enListarConNombre); NB_DEFINE_NOMBRE_PUNTERO(datosTextura->nombreTextura, "NBGestorTexturas::datosTextura->nombreTextura")
				datosTextura->loadingState			= NULL;
				//NBHILO_MUTEX_ACTIVAR(&atlasTex->texturasEnMapaMutex);
				atlasTex->texturasEnMapa->agregarElemento(datosTextura);
				r = datosTextura;
				//NBHILO_MUTEX_DESACTIVAR(&atlasTex->texturasEnMapaMutex);
			}
			//NBGestorTexturas::volcarAltasHaciaArchivos();
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return r;
}

SI32 NBGestorTexturas::privCrearAtlasTexturaGL(UI8 iAmbitoTexturas, SI32 anchoAtlas, SI32 altoAtlas, MapaBitsColor colorDestino, ENTexturaTipoAlmacenamientoGL tipoAlmacenamiento, ENTexturaTipoUsoGL tipoUso, ENTexturaModoPintado modoPintado, ENTexturaMipMap habilitarMipMap){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTexturas::privCrearAtlasTexturaGL")
	NBASSERT(iAmbitoTexturas < GESTOR_TEXTURAS_MAX_AMBITOS_TEXTURAS)
	NBASSERT(_ambitosAtlases[iAmbitoTexturas].ambitoReservado)
	SI32 indiceAtlasGL				= -1;
	const GLenum formatoGlLocal		= NBGestorTexturas::formatoGlLocal(colorDestino); NBASSERT(formatoGlLocal != 0)
	const GLenum formatoGlDriver	= NBGestorTexturas::formatoGlDriver(colorDestino); NBASSERT(formatoGlDriver != 0)
	const GLenum tpoDatoGlDriver	= NBGestorTexturas::tipoDatoGlDriver(colorDestino); NBASSERT(tpoDatoGlDriver != 0)
	if(formatoGlLocal == 0 || formatoGlDriver == 0 || tpoDatoGlDriver == 0){
		PRINTF_ERROR("no se pudo establecer el formatoGLLocal(%d) formatoGLDriver(%d) o tipoDatoGLDriver(%d) del nuevo atlas\n", formatoGlLocal, formatoGlDriver, tpoDatoGlDriver);
	} else {
		STGestorTexAmbito* ambTex = &_ambitosAtlases[iAmbitoTexturas];
		GLuint idTextura;
		STGestorTexAtlas* descAtlas = (STGestorTexAtlas*)NBGestorMemoria::reservarMemoria(sizeof(STGestorTexAtlas), ENMemoriaTipo_General);
		descAtlas->retainCount		= 1;
		#ifdef CONFIG_NB_GESTOR_GL_IMPRIMIR_COMANDOS_GL
		PRINTF_WARNING("Generando textura de atlas ambito(%d) tam(%d x %d) color(%d).\n", (SI32)iAmbitoTexturas, (SI32)anchoAtlas, (SI32)altoAtlas, (SI32)colorDestino);
		#endif
		NBGestorGL::genTextures(1, &idTextura, &descAtlas->propsTexturaGL);
		NBGestorGL::bindTexture(0, idTextura);
		NBGestorGL::configurarTextura(modoPintado, false, &descAtlas->propsTexturaGL);
		//ToDo: remove; not valid anymore; video stream textures
		//NBASSERT((tipoAlmacenamiento == ENTexturaTipoAlmacenamientoGL_AtlasCompartido && (modoPintado == ENTexturaModoPintado_Imagen_Suavizada || modoPintado == ENTexturaModoPintado_Imagen_Precisa)) ||
		//		 (tipoAlmacenamiento == ENTexturaTipoAlmacenamientoGL_AtlasUnico && (modoPintado == ENTexturaModoPintado_Patron_Suavizado || modoPintado == ENTexturaModoPintado_Patron_Preciso)))
		/*Formatos: GL_ALPHA, GL_RGB, GL_RGBA, GL_LUMINANCE (gris), GL_LUMINANCE_ALPHA (gris+alpha)*/
		/*Tipos: GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT_5_6_5, GL_UNSIGNED_SHORT_4_4_4_4, GL_UNSIGNED_SHORT_5_5_5_1*/
		NBGestorGL::texImage2D(GL_TEXTURE_2D, 0, formatoGlLocal, anchoAtlas, altoAtlas, 0, formatoGlDriver, tpoDatoGlDriver, NULL, &descAtlas->propsTexturaGL);
		_texsChangesCount++;
		NBASSERT(NBGestorGL::isTexture(idTextura))
		//Registrar nuevo Atlas para el Atlas-Textura-GL
		descAtlas->colorFormato			= colorDestino;
		descAtlas->tipoAlmacenamientoGL	= tipoAlmacenamiento;
		descAtlas->tipoUsoGL			= tipoUso;
		descAtlas->modoPintado			= modoPintado;
		descAtlas->mapaAtlas			= new(ENMemoriaTipo_General) AUAtlasMapaMutable(1, anchoAtlas, altoAtlas); NB_DEFINE_NOMBRE_PUNTERO(descAtlas->mapaAtlas, "NBGestorTexturas::nuevoAtlas")
		descAtlas->texturasEnMapa		= new(ENMemoriaTipo_General) AUArregloNativoMutableP<STGestorTexTextura*>(); NB_DEFINE_NOMBRE_PUNTERO(descAtlas->texturasEnMapa, "NBGestorTexturas::descAtlas->texturasEnMapa")
		//NBHILO_MUTEX_INICIALIZAR(&descAtlas->texturasEnMapaMutex);
		//
		//NBHILO_MUTEX_ACTIVAR(&ambTex->atlasesTexturasMutex)
		ambTex->atlasesTexturas->agregarElemento(descAtlas);
		indiceAtlasGL = ambTex->atlasesTexturas->conteo-1;
		//NBHILO_MUTEX_DESACTIVAR(&ambTex->atlasesTexturasMutex)
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return indiceAtlasGL;
}

bool NBGestorTexturas::dimensionTexturaEsMultiploDe2Valido(int tamanoTexeles){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTexturas::dimensionTexturaEsMultiploDe2Valido")
	//El minimo textura que debe soportar GLES es 64, el maximo depende
	bool esValido = ((tamanoTexeles == 16 || tamanoTexeles == 32 || tamanoTexeles == 64 || tamanoTexeles == 128 || tamanoTexeles == 256 || tamanoTexeles == 512 || tamanoTexeles == 1024 || tamanoTexeles == 2048 || tamanoTexeles == 4096 || tamanoTexeles == 8192) && tamanoTexeles <= NBGestorGL::maximaDimensionTextura());
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return esValido;
}

bool NBGestorTexturas::tamanoTexturaEsCuadradaMultiploDe2Valido(int anchoTexeles, int altoTexeles){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTexturas::tamanoTexturaEsCuadradaMultiploDe2Valido")
	bool esValido = (NBGestorTexturas::dimensionTexturaEsMultiploDe2Valido(anchoTexeles) && anchoTexeles == altoTexeles);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return esValido;
}

void NBGestorTexturas::estadoTexturas(UI32* guardarBytesReservadosEn, UI32* guardarBytesUsadosEn, UI32* guardarAtlasEn){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTexturas::estadoTexturas")
	UI32 bytesAtlas = 0, bytesTexturas =0, cantAtlas = 0;
	UI32 iAmb;
	for(iAmb = 0; iAmb < GESTOR_TEXTURAS_MAX_AMBITOS_TEXTURAS; iAmb++){
		STGestorTexAmbito* ambTex = &_ambitosAtlases[iAmb];
		UI32 iAtlas, iAtlasConteo = ambTex->atlasesTexturas->conteo;
		for(iAtlas=0; iAtlas<iAtlasConteo; iAtlas++){
			STGestorTexAtlas* atlasTex = ambTex->atlasesTexturas->elem(iAtlas);
			if(atlasTex->mapaAtlas != NULL){
				NBTamanoP<UI16> tamAtlas = atlasTex->mapaAtlas->tamano();
				bytesAtlas += (UI32)(tamAtlas.ancho * tamAtlas.alto * 4);
				//
				AUArregloNativoMutableP<STGestorTexTextura*>* texturas = atlasTex->texturasEnMapa;
				UI32 iTextura;
				for(iTextura=0; iTextura<texturas->conteo; iTextura++){
					NBTamano tamanoArea = atlasTex->texturasEnMapa->elem(iTextura)->textura->tamano();
					bytesTexturas += (tamanoArea.ancho * tamanoArea.alto * 4);
				}
			}
		}
		iAtlasConteo = _fuentesTexturas->conteo;
		for(iAtlas = 0; iAtlas < iAtlasConteo; iAtlas++){
			STGestorTexFuente* datFuenteTex = _fuentesTexturas->elemPtr(iAtlas);
			AUArregloNativoP<STGestorTexFuenteTex>* texturas = datFuenteTex->texturas;
			SI32 iTex; const SI32 conteoTex = texturas->conteo;
			for(iTex = 0; iTex < conteoTex; iTex++){
				STGestorTexFuenteTex* tex = texturas->elemPtr(iTex);
				bytesAtlas += (tex->propsTexturaGL.anchoGL * tex->propsTexturaGL.altoGL * 4);
				bytesTexturas += (tex->propsTexturaGL.anchoGL * tex->propsTexturaGL.altoGL * 4);
			}
		}
		cantAtlas += ambTex->atlasesTexturas->conteo;
	}
	if(guardarBytesReservadosEn != NULL) *guardarBytesReservadosEn = bytesAtlas;
	if(guardarBytesUsadosEn != NULL) *guardarBytesUsadosEn = bytesTexturas;
	if(guardarAtlasEn != NULL) *guardarAtlasEn = cantAtlas + _fuentesTexturas->conteo;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

#ifdef CONFIG_NB_RECOPILAR_ESTADISTICAS_DE_GESTION_MEMORIA
UI32 NBGestorTexturas::debugBytesTotalTexturas(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTexturas::debugBytesTotalTexturas")
	UI32 bytesTotal = 0;
	UI32 iAmb;
	for(iAmb=0; iAmb<GESTOR_TEXTURAS_MAX_AMBITOS_TEXTURAS; iAmb++){
		UI32 iAtlas, iAtlasConteo = ambTex->atlasesTexturas->conteo;
		//Atlas de texturas generales
		for(iAtlas=0; iAtlas<iAtlasConteo; iAtlas++){
			if(atlasTex->mapaAtlas != NULL){
				NBTamanoP<UI16> tamanoAtlas	= atlasTex->mapaAtlas->tamano();
				SI32 bytesPorPixel		= NBGestorTexturas::bytesPorPixel(atlasTex->colorFormato);
				bytesTotal				+= (tamanoAtlas.ancho * tamanoAtlas.alto * bytesPorPixel);
				//PRINTF_INFO("%d-%d) ATLAS texturas tam(%dx%d) bytesPorPixel(%d)\n", iAmb, iAtlas, tamanoAtlas.ancho, tamanoAtlas.alto, bytesPorPixel);
			}
		}
	}
	UI32 iAtlas, iAtlasConteo = _fuentesTexturas->conteo;
	//Atlas de texturas fuentes
	for(iAtlas=0; iAtlas<iAtlasConteo; iAtlas++){
		if(_fuentesTexturas->elem(iAtlas).mapaAtlas != NULL){
			NBTamanoP<UI16> tamanoAtlas	= _fuentesTexturas->elem(iAtlas).mapaAtlas->tamano();
			SI32 bytesPorPixel		= NBGestorTexturas::bytesPorPixel(_fuentesTexturas->elem(iAtlas).colorFormato);
			bytesTotal				+= (tamanoAtlas.ancho * tamanoAtlas.alto * bytesPorPixel);
			//PRINTF_INFO("%d) ATLAS fuente tam(%dx%d) bytesPorPixel(%d)\n", iAtlas, tamanoAtlas.ancho, tamanoAtlas.alto, bytesPorPixel);
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return bytesTotal;
}
#endif

#ifdef CONFIG_NB_RECOPILAR_ESTADISTICAS_DE_GESTION_MEMORIA
UI32 NBGestorTexturas::debugBytesTotalTexturasDelColor(MapaBitsColor colorBuscado, SI32* guardarConteoEn){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTexturas::debugBytesTotalTexturas")
	UI32 bytesTotal = 0; SI32 conteoTotal = 0;
	UI32 iAmb;
	for(iAmb=0; iAmb<GESTOR_TEXTURAS_MAX_AMBITOS_TEXTURAS; iAmb++){
		UI32 iAtlas, iAtlasConteo = ambTex->atlasesTexturas->conteo;
		//Atlas de texturas generales
		for(iAtlas=0; iAtlas<iAtlasConteo; iAtlas++){
			if(atlasTex->colorFormato == colorBuscado){
				if(atlasTex->mapaAtlas != NULL){
					NBTamanoP<UI16> tamanoAtlas	= atlasTex->mapaAtlas->tamano();
					SI32 bytesPorPixel			= NBGestorTexturas::bytesPorPixel(atlasTex->colorFormato);
					bytesTotal					+= (tamanoAtlas.ancho * tamanoAtlas.alto * bytesPorPixel);
					conteoTotal++;
				}
			}
		}
	}
	UI32 iAtlas, iAtlasConteo = _fuentesTexturas->conteo;
	//Atlas de texturas fuentes
	for(iAtlas=0; iAtlas<iAtlasConteo; iAtlas++){
		if(_fuentesTexturas->elem(iAtlas).colorFormato == colorBuscado){
			if(_fuentesTexturas->elem(iAtlas).mapaAtlas != NULL){
				NBTamanoP<UI16> tamanoAtlas	= _fuentesTexturas->elem(iAtlas).mapaAtlas->tamano();
				SI32 bytesPorPixel			= NBGestorTexturas::bytesPorPixel(_fuentesTexturas->elem(iAtlas).colorFormato);
				bytesTotal					+= (tamanoAtlas.ancho * tamanoAtlas.alto * bytesPorPixel);
				conteoTotal++;
			}
		}
	}
	*guardarConteoEn = conteoTotal;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return bytesTotal;
}
#endif

void NBGestorTexturas::generarMipMapsDeTexturas(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTexturas::generarMipMapsDeTexturas")
	UI32 iAmb;
	for(iAmb = 0; iAmb < GESTOR_TEXTURAS_MAX_AMBITOS_TEXTURAS; iAmb++){
		STGestorTexAmbito* ambTex = &_ambitosAtlases[iAmb];
		UI32 i, iConteo = ambTex->atlasesTexturas->conteo;
		for(i=0; i<iConteo; i++){
			STGestorTexAtlas* atlasTex = ambTex->atlasesTexturas->elem(i);
			if(atlasTex->propsTexturaGL.mipMapHabilitado && atlasTex->propsTexturaGL.mipMapSucio){
				NBGestorGL::bindTexture(0, atlasTex->propsTexturaGL.idTexturaGL);
				NBGestorGL::generateMipmapEXT(GL_TEXTURE_2D, &atlasTex->propsTexturaGL);
			}
		}
	}
	//Fuentes (todas)
	SI32 iFnt; const SI32 conteoFnt = _fuentesTexturas->conteo;
	for(iFnt = 0; iFnt < conteoFnt; iFnt++){
		STGestorTexFuente* datFuenteTex = _fuentesTexturas->elemPtr(iFnt);
		AUArregloNativoP<STGestorTexFuenteTex>* texturas = datFuenteTex->texturas;
		SI32 iTex; const SI32 conteoTex = texturas->conteo;
		for(iTex = 0; iTex < conteoTex; iTex++){
			STGestorTexFuenteTex* tex = texturas->elemPtr(iTex);
			if(tex->propsTexturaGL.mipMapHabilitado && tex->propsTexturaGL.mipMapSucio){
				NBGestorGL::bindTexture(0, tex->propsTexturaGL.idTexturaGL);
				NBGestorGL::generateMipmapEXT(GL_TEXTURE_2D, &tex->propsTexturaGL);
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

void NBGestorTexturas::generarMipMapsDeTexturas(UI16 iAmbitoTexturas){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTexturas::generarMipMapsDeTexturas")
	NBASSERT(iAmbitoTexturas < GESTOR_TEXTURAS_MAX_AMBITOS_TEXTURAS)
	NBASSERT(_ambitosAtlases[iAmbitoTexturas].ambitoReservado)
	if(iAmbitoTexturas < GESTOR_TEXTURAS_MAX_AMBITOS_TEXTURAS){
		STGestorTexAmbito* ambTex = &_ambitosAtlases[iAmbitoTexturas];
		if(ambTex->ambitoReservado){
			UI32 i, iConteo = ambTex->atlasesTexturas->conteo;
			for(i=0; i<iConteo; i++){
				STGestorTexAtlas* atlasTex = ambTex->atlasesTexturas->elem(i);
				if(atlasTex->propsTexturaGL.mipMapHabilitado && atlasTex->propsTexturaGL.mipMapSucio){
					NBGestorGL::bindTexture(0, atlasTex->propsTexturaGL.idTexturaGL);
					NBGestorGL::generateMipmapEXT(GL_TEXTURE_2D, &atlasTex->propsTexturaGL);
				}
			}
		}
	}
	//Fuentes (todas)
	SI32 iFnt; const SI32 conteoFnt = _fuentesTexturas->conteo;
	for(iFnt = 0; iFnt < conteoFnt; iFnt++){
		STGestorTexFuente* datFuenteTex = _fuentesTexturas->elemPtr(iFnt);
		AUArregloNativoP<STGestorTexFuenteTex>* texturas = datFuenteTex->texturas;
		SI32 iTex; const SI32 conteoTex = texturas->conteo;
		for(iTex = 0; iTex < conteoTex; iTex++){
			STGestorTexFuenteTex* tex = texturas->elemPtr(iTex);
			if(tex->propsTexturaGL.mipMapHabilitado && tex->propsTexturaGL.mipMapSucio){
				NBGestorGL::bindTexture(0, tex->propsTexturaGL.idTexturaGL);
				NBGestorGL::generateMipmapEXT(GL_TEXTURE_2D, &tex->propsTexturaGL);
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

void NBGestorTexturas::volcarAltasHaciaArchivos(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorTexturas::volcarAltasHaciaArchivos")
	GLenum errorGL = NBGestorGL::getError();
	if(errorGL != GL_NO_ERROR) PRINTF_ERROR("GL (antes): %d\n", errorGL);
	//
	AUCadenaMutable8* rutaDestino		= new(ENMemoriaTipo_Temporal) AUCadenaMutable8(); NB_DEFINE_NOMBRE_PUNTERO(rutaDestino, "NBGestorTexturas::rutaDestino")
	UI32 iAmb;
	for(iAmb = 0; iAmb < GESTOR_TEXTURAS_MAX_AMBITOS_TEXTURAS; iAmb++){
		STGestorTexAmbito* ambTex = &_ambitosAtlases[iAmb];
		//Atlas regulares
		UI32 i, iConteo = ambTex->atlasesTexturas->conteo;
		for(i=0; i<iConteo && errorGL == GL_NO_ERROR; i++){
			STGestorTexAtlas* atlasTex = ambTex->atlasesTexturas->elem(i);
			if(atlasTex->mapaAtlas != NULL){
				AUAtlasMapaMutable* descAtlas	= atlasTex->mapaAtlas;
				GLuint idTextura				= (GLuint)atlasTex->propsTexturaGL.idTexturaGL;
				NBTamanoP<UI16> tamanoAtlas		= descAtlas->tamano();
				MapaBitsColor colorTextura		= atlasTex->colorFormato;
				GLenum formatoGlLocal			= NBGestorTexturas::formatoGlLocal(colorTextura);
				AUMapaBitsMutable* mapaBits 	= new(ENMemoriaTipo_Temporal) AUMapaBitsMutable(tamanoAtlas.ancho, tamanoAtlas.alto, colorTextura, 0); NB_DEFINE_NOMBRE_PUNTERO(mapaBits, "NBGestorTexturas::mapaBits")
#				ifdef NB_LIB_GRAFICA_ES_EMBEDDED
				rutaDestino->vaciar();
				rutaDestino->agregar("altas_mapa_general_");
				rutaDestino->agregarNumerico(iAmb);
				rutaDestino->agregar("_");
				rutaDestino->agregarNumerico(i);
				rutaDestino->agregar("_");
				rutaDestino->agregar(AUMapaBits::strColor(colorTextura));
				rutaDestino->agregar(".png");
				GLuint textureFrameBuffer;
				NBGestorGL::genFramebuffersEXT(1, &textureFrameBuffer);
				NBGestorGL::bindFramebufferEXT(GL_FRAMEBUFFER_EXT, textureFrameBuffer);
				NBGestorGL::framebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, idTextura, 0);
				glReadPixels(0, 0, tamanoAtlas.ancho, tamanoAtlas.alto, formatoGlLocal, GL_UNSIGNED_BYTE, mapaBits->datos());
				if(!mapaBits->guardarComoPNG(rutaDestino->str(), true)){
					PRINTF_INFO("No se pudo volcar el mapa de bits (de atlas %d)\n", i);
				} else {
					PRINTF_INFO("Mapa de bits (de atlas %d) volcado a PNG\n", i);
				}
				NBGestorGL::bindFramebufferInicialEXT(GL_FRAMEBUFFER_EXT);
				NBGestorGL::deleteFramebuffersEXT(1, &textureFrameBuffer);
#				else
				rutaDestino->vaciar();
				rutaDestino->agregar("altas_mapa_general_");
				rutaDestino->agregarNumerico(iAmb);
				rutaDestino->agregar("_");
				rutaDestino->agregarNumerico(i);
				rutaDestino->agregar("_");
				rutaDestino->agregar(AUMapaBits::strColor(colorTextura));
				rutaDestino->agregar(".png");
				NBGestorGL::bindTexture(0, idTextura);
				glGetTexImage(GL_TEXTURE_2D, 0, formatoGlLocal, GL_UNSIGNED_BYTE, mapaBits->datos());
				if(!mapaBits->guardarComoPNG(rutaDestino->str(), false)){
					PRINTF_INFO("No se pudo volcar el mapa de bits (de atlas_general %d)\n", i);
				} else {
					PRINTF_INFO("Mapa de bits (de atlas_general %d) volcado a PNG\n", i);
				}
#				endif
				errorGL = NBGestorGL::getError();
				if(errorGL != GL_NO_ERROR){
					PRINTF_ERROR("GL (despues): %d\n", errorGL);
				}
				mapaBits->liberar(NB_RETENEDOR_NULL);
			}
		}
	}
	//Atlas de fuentes
	SI32 iFnt; const SI32 conteoFnt = _fuentesTexturas->conteo;
	for(iFnt = 0; iFnt < conteoFnt && errorGL == GL_NO_ERROR; iFnt++){
		STGestorTexFuente* datFuenteTex = _fuentesTexturas->elemPtr(iFnt);
		AUArregloNativoP<STGestorTexFuenteTex>* texturas = datFuenteTex->texturas;
		SI32 iTex; const SI32 conteoTex = texturas->conteo;
		for(iTex = 0; iTex < conteoTex && errorGL == GL_NO_ERROR; iTex++){
			STGestorTexFuenteTex* tex = texturas->elemPtr(iTex);
			GLuint idTextura				= (GLuint)tex->propsTexturaGL.idTexturaGL;
			NBTamanoP<UI16> tamanoAtlas;	NBTAMANO_ESTABLECER(tamanoAtlas, tex->propsTexturaGL.anchoGL, tex->propsTexturaGL.altoGL)
			MapaBitsColor colorTextura		= tex->colorFormato;
			GLenum formatoGlLocal			= NBGestorTexturas::formatoGlLocal(colorTextura);
			AUMapaBitsMutable* mapaBits 	= new(ENMemoriaTipo_Temporal) AUMapaBitsMutable(tamanoAtlas.ancho, tamanoAtlas.alto, colorTextura, 0); NB_DEFINE_NOMBRE_PUNTERO(mapaBits, "NBGestorTexturas::mapaBits")
#			ifdef NB_LIB_GRAFICA_ES_EMBEDDED
			rutaDestino->vaciar();
			rutaDestino->agregar("altas_mapa_fuente");
			rutaDestino->agregarNumerico(iFnt);
			rutaDestino->agregar("_");
			rutaDestino->agregarNumerico(iTex);
			rutaDestino->agregar("_");
			rutaDestino->agregar(AUMapaBits::strColor(colorTextura));
			rutaDestino->agregar(".png");
			GLuint textureFrameBuffer;
			NBGestorGL::genFramebuffersEXT(1, &textureFrameBuffer);
			NBGestorGL::bindFramebufferEXT(GL_FRAMEBUFFER_EXT, textureFrameBuffer);
			NBGestorGL::framebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, idTextura, 0);
			glReadPixels(0, 0, tamanoAtlas.ancho, tamanoAtlas.alto, formatoGlLocal, GL_UNSIGNED_BYTE, mapaBits->datos());
			if(!mapaBits->guardarComoPNG(rutaDestino->str(), true)){
				PRINTF_INFO("No se pudo volcar el mapa de bits (de atlas_fuente %d_%d)\n", iFnt, iTex);
			} else {
				PRINTF_INFO("Mapa de bits (de atlas_fuente %d_%d) volcado a PNG\n", iFnt, iTex);
			}
			NBGestorGL::bindFramebufferInicialEXT(GL_FRAMEBUFFER_EXT);
			NBGestorGL::deleteFramebuffersEXT(1, &textureFrameBuffer);
#			else
			rutaDestino->vaciar();
			rutaDestino->agregar("altas_mapa_fuente");
			rutaDestino->agregarNumerico(iFnt);
			rutaDestino->agregar("_");
			rutaDestino->agregarNumerico(iTex);
			rutaDestino->agregar("_");
			rutaDestino->agregar(AUMapaBits::strColor(colorTextura));
			rutaDestino->agregar(".png");
			NBGestorGL::bindTexture(0, idTextura);
			glGetTexImage(GL_TEXTURE_2D, 0, formatoGlLocal, GL_UNSIGNED_BYTE, mapaBits->datos());
			if(!mapaBits->guardarComoPNG(rutaDestino->str(), false)){
				PRINTF_INFO("No se pudo volcar el mapa de bits (de atlas_fuente %d_%d)\n", iFnt, iTex);
			} else {
				PRINTF_INFO("Mapa de bits (de atlas_fuente %d_%d) volcado a PNG\n", iFnt, iTex);
			}
#			endif
			errorGL = NBGestorGL::getError();
			if(errorGL != GL_NO_ERROR){
				PRINTF_ERROR("GL (despues): %d\n", errorGL);
			}
			mapaBits->liberar(NB_RETENEDOR_NULL);
		}
	}
	rutaDestino->liberar(NB_RETENEDOR_NULL);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}
