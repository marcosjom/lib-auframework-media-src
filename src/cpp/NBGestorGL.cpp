//
//  NBGestorGL.cpp
//  AUFramework
//
//  Created by Marcos Ortega Morales on 09/10/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkMediaStdAfx.h"
#include "NBGestorGL.h"
//
#include "nb/core/NBMemory.h"

#ifdef CONFIG_NB_GESTOR_GL_VERIFICAR_CACHE_SIEMPRE
#	define NBGL_ASSERT_CACHE_VAO					NBGestorGL::privDbgCacheAssertVAOs();
#	define NBGL_ASSERT_CACHE_CLIENT_STATE			NBGestorGL::privDbgCacheAssertClientState();
#	define NBGL_ASSERT_CACHE_ACTIVE_TEXTURE			NBGestorGL::privDbgCacheAssertActiveTexture();
#	define NBGL_ASSERT_CACHE_TEXTURE_ACTIVE_STATE	NBGestorGL::privDbgCacheAssertTextureActiveState();
#else
#	define NBGL_ASSERT_CACHE_VAO
#	define NBGL_ASSERT_CACHE_CLIENT_STATE
#	define NBGL_ASSERT_CACHE_ACTIVE_TEXTURE
#	define NBGL_ASSERT_CACHE_TEXTURE_ACTIVE_STATE
#endif

bool								NBGestorGL::_gestorInicializado = false;
GLuint								NBGestorGL::_idFramebufferAlInicializar = 0;
NBMatriz							NBGestorGL::_matrizIdentidad;
//
STGLEstado							NBGestorGL::_cacheGL;
//
STArregloVerticesGL*				NBGestorGL::_vertsGlArrs = NULL;
SI32								NBGestorGL::_vertsGlArrsTam = 0;
//
bool								NBGestorGL::_soportaVAOs = false;
bool								NBGestorGL::_soportaVAOsAnalizado = false;
bool								NBGestorGL::_soportaVAOArrayBufferBinding = false;
bool								NBGestorGL::_soportaVAOIndexBufferBinding = false;
bool								NBGestorGL::_soportaTexturas2DNoMultiploDe2 = false;
bool								NBGestorGL::_soportaDrawTexture = false;

//Blending por defecto
GLenum								NBGestorGL::_blendSoucFactorRGBAActual = 0;
GLenum								NBGestorGL::_blendDestFactorRGBAActual = 0;
GLenum								NBGestorGL::_blendSoucFactorRGBAPorDefecto = 0;
GLenum								NBGestorGL::_blendDestFactorRGBAPorDefecto = 0;

//Modo de dibujo de los triangStrips
#ifdef NB_GESTOR_ESCENAS_USA_CACHE_LOTE_INSTRUCCIONES
STGestorGLLoteDraw					NBGestorGL::_loteDrawElemsAcumTriangStrips;
#endif

SI32								NBGestorGL::_maxUnidadesTexUsar = 0;
SI32								NBGestorGL::_maxUnidadesTexturas = 0;
SI32								NBGestorGL::_maxDimensionTextura = 0;

#ifdef CONFIG_NB_GESTOR_ESCENAS_MODELOS_MEDIANTE_INDICES
#	define NBGESTORGL_INICIALIZAR_ARREGLO_VERTICESGL(PTR_STRUCT, EN_VERTICEGL) \
	{ \
		/*VBOs*/ \
		glGenBuffers(1, &(PTR_STRUCT->idBufferGlVertices));	GL_CMD_EJECUTADO("glGenBuffers(1, ret(%u))", (UI32)PTR_STRUCT->idBufferGlVertices); \
		glGenBuffers(1, &(PTR_STRUCT->idBufferGlIndices));	GL_CMD_EJECUTADO("glGenBuffers(1, ret(%u))", (UI32)PTR_STRUCT->idBufferGlIndices); \
		NBGestorGL::privInicializarCacheVAO(&PTR_STRUCT->vaoConfig); \
		if(!_soportaVAOs){ \
			PTR_STRUCT->idVertexArrayObjectGl = 0; \
		} else { \
			/*Generar VAO*/ \
			NBGestorGL::privGenVertexArraysEXT(1, &(PTR_STRUCT->idVertexArrayObjectGl)); \
			/*Configurar VAO*/ \
			NBGestorGL::privBindVertexArrayEXTParaInicializacion(EN_VERTICEGL, PTR_STRUCT->idVertexArrayObjectGl); \
			/**/ \
			glBindBuffer(GL_ARRAY_BUFFER, PTR_STRUCT->idBufferGlVertices);				GL_CMD_EJECUTADO("glBindBuffer(GL_ARRAY_BUFFER, %u)", (UI32)PTR_STRUCT->idBufferGlVertices) \
			_cacheGL.idBufferArrayLigado = PTR_STRUCT->idBufferGlVertices; \
			/**/ \
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, PTR_STRUCT->idBufferGlIndices);		GL_CMD_EJECUTADO("glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, %u)", (UI32)PTR_STRUCT->idBufferGlIndices) \
			_cacheGL.idBufferElemsLigado = PTR_STRUCT->idBufferGlIndices; \
			/**/ \
			NBGestorGL::privConfigurarClienteEstado(EN_VERTICEGL, 0, &PTR_STRUCT->vaoConfig, true/*forzarConfig*/); \
		} \
	}
#else
#	define NBGESTORGL_INICIALIZAR_ARREGLO_VERTICESGL(PTR_STRUCT, EN_VERTICEGL) \
	{ \
		/*VBOs*/ \
		glGenBuffers(1, &(PTR_STRUCT->idBufferGlVertices));	GL_CMD_EJECUTADO("glGenBuffers(1, ret(%u))", (UI32)PTR_STRUCT->idBufferGlVertices); \
		NBGestorGL::privInicializarCacheVAO(&PTR_STRUCT->vaoConfig); \
		if(!_soportaVAOs){ \
			PTR_STRUCT->idVertexArrayObjectGl = 0; \
		} else { \
			NBGestorGL::privGenVertexArraysEXT(1, &(PTR_STRUCT->idVertexArrayObjectGl)); \
			/*Configurar VAO*/ \
			NBGestorGL::privBindVertexArrayEXTParaInicializacion(EN_VERTICEGL, PTR_STRUCT->idVertexArrayObjectGl); \
			/**/ \
			glBindBuffer(GL_ARRAY_BUFFER, PTR_STRUCT->idBufferGlVertices); GL_CMD_EJECUTADO("glBindBuffer(GL_ARRAY_BUFFER, %u)", (UI32)PTR_STRUCT->idBufferGlVertices) \
			_cacheGL.idBufferArrayLigado = PTR_STRUCT->idBufferGlVertices; \
			/**/ \
			NBGestorGL::privConfigurarClienteEstado(EN_VERTICEGL, 0, &PTR_STRUCT->vaoConfig, true/*forzarConfig*/); \
		} \
	}
#endif

bool NBGestorGL::inicializar(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::inicializar")
	bool inicializado = false;
	//---------------------------------
	//--- Cargar propiedades del DriverGL
	//---------------------------------
	//GL_MAJOR_VERSION y GL_MINOR_VERSION requieren OpenGL 3 o superior
	const char* strGlVer	= (const char*)glGetString(GL_VERSION);
	AUCadena8* versionGL	= new(ENMemoriaTipo_Temporal) AUCadena8(strGlVer); GL_CMD_EJECUTADO("glGetString(GL_VERSION)"); NB_DEFINE_NOMBRE_PUNTERO(versionGL, "NBGestorGL::versionGL")
	//Se asume que el primer numero es la version mayor, 
	//si despues de la version mayor viene un punto, seguido de otro numero, entonces es la version menor.
	bool esEmbededSystems	= (versionGL->indiceDe("OpenGLES") != -1 || versionGL->indiceDe("OpenGL ES") != -1 || versionGL->indiceDe("OpenGL-ES") != -1);
	SI32 versionMayorGL		= 0;
	SI32 versionMenorGL		= 0;
	SI32 iCar, tamCadena	= versionGL->tamano();
	for(iCar=0; iCar < tamCadena && (versionMayorGL == 0 || versionMenorGL == 0); iCar++){
		char c = versionGL->str()[iCar];
		bool esDigito = AUNumericoP < int, char > ::esDigito(c);
		if(versionMayorGL == 0){
			if(esDigito) versionMayorGL = AUNumericoP < int, char > ::valorDigito(c);
		} else if(iCar > 0) {
			if(esDigito && versionGL->str()[iCar-1] == '.') versionMenorGL = AUNumericoP < int, char > ::valorDigito(c);
		}
	}
	{
		NBMemory_setZeroSt(_matrizIdentidad, NBMatriz);
		NBMemory_setZeroSt(_cacheGL, STGLEstado);
#		ifdef NB_GESTOR_ESCENAS_USA_CACHE_LOTE_INSTRUCCIONES
		NBMemory_setZeroSt(_loteDrawElemsAcumTriangStrips, STGestorGLLoteDraw);
#		endif
	}
	_soportaVAOs					= false;
	_soportaVAOsAnalizado			= false;
	_soportaVAOArrayBufferBinding	= false;	//Se establece el valor mediante prueba en la configuracion inicial
	_soportaVAOIndexBufferBinding	= false;	//Se establece el valor mediante prueba en la configuracion inicial
	_soportaTexturas2DNoMultiploDe2	= false;
	_soportaDrawTexture				= false;
	_maxDimensionTextura			= 64;
	_maxUnidadesTexUsar				= 2;
	_maxUnidadesTexturas			= 2;
	//
	if(!((esEmbededSystems && ((versionMayorGL == 1 && versionMenorGL >= 1) || (versionMayorGL > 1))) || (!esEmbededSystems && versionMayorGL >= 2))){
		PRINTF_ERROR("----------- OPENGL -------------\n");
		PRINTF_ERROR("OpenGL version:	        '%s'\n", versionGL->str());
		PRINTF_ERROR("Version de openGL no soportada\n");
		PRINTF_ERROR("Actualice su driver de video (actual %d.%d %s)\n", versionMayorGL, versionMenorGL, esEmbededSystems?"ES":"no-ES");
		PRINTF_ERROR("Version minima requerida OpenGL(2.0) o OpenGLES(1.1)\n");
		PRINTF_ERROR("----------- OPENGL -------------\n");
	} else {
		//Capacidades de OpenGL
		AUCadena8* vendorGL					= new(ENMemoriaTipo_Temporal) AUCadena8((const char*)glGetString(GL_VENDOR)); GL_CMD_EJECUTADO("glGetString(GL_VENDOR)");					NB_DEFINE_NOMBRE_PUNTERO(vendorGL, "NBGestorGL::vendorGL")
		AUCadena8* renderizadorGL			= new(ENMemoriaTipo_Temporal) AUCadena8((const char*)glGetString(GL_RENDERER)); GL_CMD_EJECUTADO("glGetString(GL_RENDERER)");				NB_DEFINE_NOMBRE_PUNTERO(renderizadorGL, "NBGestorGL::renderizadorGL")
		AUCadenaMutable8* extensionesGL		= new(ENMemoriaTipo_Temporal) AUCadenaMutable8((const char*)glGetString(GL_EXTENSIONS)); GL_CMD_EJECUTADO("glGetString(GL_EXTENSIONS)");	NB_DEFINE_NOMBRE_PUNTERO(extensionesGL, "NBGestorGL::extensionesGL")
		GLint frameBufferActual;			glGetIntegerv(GL_FRAMEBUFFER_BINDING_EXT, &frameBufferActual); GL_CMD_EJECUTADO("glGetIntegerv(GL_FRAMEBUFFER_BINDING_EXT)"); //No soportada en Windows viejo
		GLint texMaxUnidades;				glGetIntegerv(GL_MAX_TEXTURE_UNITS, &texMaxUnidades); GL_CMD_EJECUTADO("glGetIntegerv(GL_MAX_TEXTURE_UNITS)");
		GLint texMaxTamano;					glGetIntegerv(GL_MAX_TEXTURE_SIZE, &texMaxTamano); GL_CMD_EJECUTADO("glGetIntegerv(GL_MAX_TEXTURE_SIZE)");
		GLint bitsRojo;						glGetIntegerv(GL_RED_BITS, &bitsRojo); GL_CMD_EJECUTADO("glGetIntegerv(GL_RED_BITS)");
		GLint bitsVerde;					glGetIntegerv(GL_GREEN_BITS, &bitsVerde); GL_CMD_EJECUTADO("glGetIntegerv(GL_GREEN_BITS)");
		GLint bitsAzul;						glGetIntegerv(GL_BLUE_BITS, &bitsAzul); GL_CMD_EJECUTADO("glGetIntegerv(GL_BLUE_BITS)");
		GLint bitsAlpha;					glGetIntegerv(GL_ALPHA_BITS, &bitsAlpha); GL_CMD_EJECUTADO("glGetIntegerv(GL_ALPHA_BITS)");
		GLint bitsDepth;					glGetIntegerv(GL_DEPTH_BITS, &bitsDepth); GL_CMD_EJECUTADO("glGetIntegerv(GL_DEPTH_BITS)");
		GLint bitsStencil;					glGetIntegerv(GL_STENCIL_BITS, &bitsStencil); GL_CMD_EJECUTADO("glGetIntegerv(GL_STENCIL_BITS)");
		GLint bitsSubpixel;					glGetIntegerv(GL_SUBPIXEL_BITS, &bitsSubpixel); GL_CMD_EJECUTADO("glGetIntegerv(GL_SUBPIXEL_BITS)");
		//GLint maxVertices;				glGetIntegerv(GL_MAX_ELEMENTS_VERTICES, &maxVertices); GL_CMD_EJECUTADO("glGetIntegerv(GL_MAX_ELEMENTS_VERTICES)"); //ContanteGL soportados en iOS
		//GLint maxIndices;					glGetIntegerv(GL_MAX_ELEMENTS_INDICES, &maxIndices); GL_CMD_EJECUTADO("glGetIntegerv(GL_MAX_ELEMENTS_INDICES)"); //ContanteGL no soportados en iOS
		GLint maxLuces;						glGetIntegerv(GL_MAX_LIGHTS, &maxLuces); GL_CMD_EJECUTADO("glGetIntegerv(GL_MAX_LIGHTS)");
		GLint maxPilaModelo;				glGetIntegerv(GL_MAX_MODELVIEW_STACK_DEPTH, &maxPilaModelo); GL_CMD_EJECUTADO("glGetIntegerv(GL_MAX_MODELVIEW_STACK_DEPTH)");
		GLint maxPilaProyeccion;			glGetIntegerv(GL_MAX_PROJECTION_STACK_DEPTH, &maxPilaProyeccion); GL_CMD_EJECUTADO("glGetIntegerv(GL_MAX_PROJECTION_STACK_DEPTH)");
		GLint maxPilaTextura;				glGetIntegerv(GL_MAX_TEXTURE_STACK_DEPTH, &maxPilaTextura); GL_CMD_EJECUTADO("glGetIntegerv(GL_MAX_TEXTURE_STACK_DEPTH)");
		GLint maxTamPuerto[2];				glGetIntegerv(GL_MAX_VIEWPORT_DIMS, maxTamPuerto); GL_CMD_EJECUTADO("glGetIntegerv(GL_MAX_VIEWPORT_DIMS)");
		PRINTF_INFO("----------- OPENGL -------------\n");
		PRINTF_INFO("OpenGL version:        '%s'\n", versionGL->str());
		PRINTF_INFO("Version interpretacion:'%d.%d %s'\n", versionMayorGL, versionMenorGL, esEmbededSystems?"ES":"no-ES");
		PRINTF_INFO("Framebuffer inicial:   %d\n", (SI32)_idFramebufferAlInicializar);
		PRINTF_INFO("Vendor:                '%s'\n", vendorGL->str());
		PRINTF_INFO("Renderizador:          '%s'\n", renderizadorGL->str());
		PRINTF_INFO("Puerto Tamano Max:     %d x %d\n", maxTamPuerto[0], maxTamPuerto[1]);
		PRINTF_INFO("Tex Tamano Max:        %d\n", texMaxTamano);
		PRINTF_INFO("Tex Unidades Max:      %d\n", texMaxUnidades);
		PRINTF_INFO("Bits R-G-B-A:          %d-%d-%d-%d\n", bitsRojo, bitsVerde, bitsAzul, bitsAlpha);
		PRINTF_INFO("Depth-Stencil-Subpixel:%d-%d-%d\n", bitsDepth, bitsStencil, bitsSubpixel);
		//PRINTF_INFO("Max Vertices Recom:	%d\n", maxVertices);
		//PRINTF_INFO("Max Indices Recom:	%d\n", maxIndices);
		PRINTF_INFO("Max Luces:	            %d\n", maxLuces);
		PRINTF_INFO("Max Pila Modelo:       %d\n", maxPilaModelo);
		PRINTF_INFO("Max Pila Proyeccion:   %d\n", maxPilaProyeccion);
		PRINTF_INFO("Max Pila Textura:      %d\n", maxPilaTextura);
#		if defined(__ANDROID__) || defined(__QNX__) //TEMPORAL
			_soportaVAOs					= false;
#		else
			_soportaVAOs					= (extensionesGL->indiceDe("_vertex_array_object") != -1);
			//PRINTF_WARNING("--------------\nSOPORTE VAO TEMPORALMENTE DESHABILITADO!\n--------------\n");
#		endif
		//_soportaVAOArrayBufferBinding		//Se establece el valor mediante prueba en la configuracion inicial
		//_soportaVAOIndexBufferBinding		//Se establece el valor mediante prueba en la configuracion inicial
		//#endif
		_soportaTexturas2DNoMultiploDe2	= (extensionesGL->indiceDe("_texture_2D_limited_npot") != -1); //GL_APPLE_texture_2D_limited_npot
#		ifdef GL_TEXTURE_CROP_RECT_OES
		_soportaDrawTexture				= (extensionesGL->indiceDe("_draw_texture") != -1); //GL_OES_draw_texture
#		else
		_soportaDrawTexture				= FALSE; //GL_OES_draw_texture
#		endif
		_idFramebufferAlInicializar		= frameBufferActual;
		_maxDimensionTextura			= texMaxTamano;
		_maxUnidadesTexturas			= texMaxUnidades;
		_maxUnidadesTexUsar				= (texMaxUnidades > 3 ? 3 : _maxUnidadesTexturas); //Solo se usan 3 unidades de texturas (como manejar cuando sean menos de 3???)
		//
		NBGestorGL::privInicializarCaches(true);
		//
		NBASSERT(glGenBuffers != NULL) //Si falla, probablemente falta inicializar GLEW
		GL_CMD_EJECUTADO("Inicializando GestorGL") //Verifica que no se esta iniciando con un errorGL acumulado
		//Inicializar arreglos de verticesGL
		{
			_vertsGlArrsTam	= ENVerticeGlTipo_Conteo;
			_vertsGlArrs	= (STArregloVerticesGL*)NBGestorMemoria::reservarMemoria(sizeof(STArregloVerticesGL) * _vertsGlArrsTam, ENMemoriaTipo_General);
			SI32 i; for(i = 0; i < _vertsGlArrsTam; i++){ _vertsGlArrs[i].registroOcupado = false; }
		}
		//
		if(_soportaVAOs){
			PRINTF_INFO("Soporta VAOs:	        YES (%s el ARRAY_BUFFER_BINDING, %s el ELEMENT_ARRAY_BUFFER_BINDING)\n", (_soportaVAOArrayBufferBinding ? "incluye" : "excluye"), (_soportaVAOIndexBufferBinding ? "incluye" : "excluye"));
		} else {
			PRINTF_WARNING(" RENDIMIENTO GL: la implementacion no soporta VAO (las configuraciones de cliente seran establecidas en cada cambio)\n");
		}
		if(_soportaTexturas2DNoMultiploDe2){
			PRINTF_INFO("Soporta Tex2D_NPOT:    YES (textures2D-sizes-not-multiple-of-2)\n");
			
		} else {
			PRINTF_WARNING(" RENDIMIENTO GL: la implementacion no soporta Tex2D_NPOT (el renderizado a textura obligara a crear nuevos FrameBuffers y desperdiciar espacio en texturas cuadradas)\n");
		}
		if(_soportaDrawTexture){
			PRINTF_INFO("Soporta DrawTex:       YES (render textures rects without vertex nor triangles)\n");
		} else {
			PRINTF_WARNING(" RENDIMIENTO GL: la implementacion no soporta DrawTex (el renderizado requiere VBOs y Triangulos)\n");
		}
		//extensionesGL->reemplazar(" ", "\n");
		PRINTF_INFO("EXTensiones GL:\n%s\n", extensionesGL->str());
		PRINTF_INFO("--------------------------------\n");
		//
		glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST); //GL_FASTEST, GL_NICEST o GL_DONT_CARE
		//
		versionGL->liberar(NB_RETENEDOR_NULL);
		vendorGL->liberar(NB_RETENEDOR_NULL);
		renderizadorGL->liberar(NB_RETENEDOR_NULL);
		extensionesGL->liberar(NB_RETENEDOR_NULL);
		inicializado = true;
	}
	_gestorInicializado = inicializado;
	NBGestorGL::enable(GL_TEXTURE_2D);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
	return inicializado;
}

void NBGestorGL::finalizar(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::finalizar")
	//Liberar bufferes de vertices GL
	if(_vertsGlArrs != NULL){
		SI32 i;
		for(i = 0; i < _vertsGlArrsTam; i++){
			STArregloVerticesGL* ptrDat = &_vertsGlArrs[i];
			if(ptrDat->registroOcupado){
				NBGestorGL::bufferVerticesLiberar(i);
			}
		}
		NBGestorMemoria::liberarMemoria(_vertsGlArrs);
		_vertsGlArrsTam = 0; _vertsGlArrs = NULL;
	}
#	ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT_GL_NOP
	if(_cacheGL.dbgVAONombreFuncion != NULL) _cacheGL.dbgVAONombreFuncion->liberar(NB_RETENEDOR_THIS); _cacheGL.dbgVAONombreFuncion = NULL;
	if(_cacheGL.dbgDrawArrayConvocador != NULL) _cacheGL.dbgDrawArrayConvocador->liberar(NB_RETENEDOR_THIS); _cacheGL.dbgDrawArrayConvocador = NULL;
	if(_cacheGL.dbgDrawElementsConvocador != NULL) _cacheGL.dbgDrawElementsConvocador->liberar(NB_RETENEDOR_THIS); _cacheGL.dbgDrawElementsConvocador = NULL;
	if(_cacheGL.dbgLoteTringStripsNombrePurgador != NULL) _cacheGL.dbgLoteTringStripsNombrePurgador->liberar(NB_RETENEDOR_THIS); _cacheGL.dbgLoteTringStripsNombrePurgador = NULL;
#	endif
	_gestorInicializado	= false;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
}

bool NBGestorGL::gestorInicializado(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::gestorInicializado")
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
	return _gestorInicializado;
}

void NBGestorGL::privInicializarCaches(const bool esPrimeraVez){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::privInicializarCaches")
	//
	_idFramebufferAlInicializar		= 0;
	//Blending por defecto
	//RGB(GL_SRC_ALPHA) ALPHA(GL_ONE_MINUS_SRC_ALPHA), cuando se esta pintado de atras hacia adelante
	//RGB(------------) ALPHA(----------------------), cuando se esta pintado de adelante hacia atras
	_blendSoucFactorRGBAActual		= 0;
	_blendDestFactorRGBAActual		= 0;
	_blendSoucFactorRGBAPorDefecto	= GL_SRC_ALPHA;
	_blendDestFactorRGBAPorDefecto	= GL_ONE_MINUS_SRC_ALPHA;
#	ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT_GL_NOP
	if(esPrimeraVez){
		_cacheGL.dbgVAONombreFuncion				= new(ENMemoriaTipo_General) AUCadenaMutable8();
		_cacheGL.dbgDrawArrayConvocador				= new(ENMemoriaTipo_General) AUCadenaMutable8();
		_cacheGL.dbgDrawElementsConvocador			= new(ENMemoriaTipo_General) AUCadenaMutable8();
		_cacheGL.dbgLoteTringStripsNombrePurgador	= new(ENMemoriaTipo_General) AUCadenaMutable8();
	} else {
		_cacheGL.dbgVAONombreFuncion->vaciar();
		_cacheGL.dbgDrawArrayConvocador->vaciar();
		_cacheGL.dbgDrawElementsConvocador->vaciar();
		_cacheGL.dbgLoteTringStripsNombrePurgador->vaciar();
	}
	{
		UI16 iBufV;
		for(iBufV=0; iBufV < ENVerticeGlTipo_Conteo; iBufV++){
			NBGestorGL::privDbgReiniciarCacheVerticesIndicesUsados((ENVerticeGlTipo)iBufV);
		}
	}
#	endif
	//
	NBMATRIZ_ESTABLECER_IDENTIDAD(_matrizIdentidad);
#	ifdef NB_GESTOR_ESCENAS_USA_CACHE_LOTE_INSTRUCCIONES
	_loteDrawElemsAcumTriangStrips.iPrimerElem			= 0;
	_loteDrawElemsAcumTriangStrips.cantAcumuladaElems	= 0;
#	endif
	//CacheGL
	_cacheGL.idFrameBufferGlLigado						= -1;
	_cacheGL.idRenderBufferGlLigado						= -1;
	_cacheGL.iVaoLigado									= -1;
	_cacheGL.idBufferArrayLigado						= 0;
#	ifdef CONFIG_NB_GESTOR_ESCENAS_MODELOS_MEDIANTE_INDICES
	_cacheGL.idBufferElemsLigado						= 0;
#	endif
	_cacheGL.iUnidadTexturaActivaCliente				= 0;
	_cacheGL.iUnidadTexturaActivaServidor				= 0;
	SI32 iUTex;
	for(iUTex=0; iUTex < NB_GESTOR_GL_MAX_TEXTURA_UNIDADES; iUTex++){
		_cacheGL.unidadesTex[iUTex].texture2DHabilitada		= false;
		_cacheGL.unidadesTex[iUTex].idTexturaLigada			= 0;
		_cacheGL.unidadesTex[iUTex].modoEntorno				= GL_MODULATE;
		_cacheGL.unidadesTex[iUTex].escalaCoordenadas		= 1.0f;
		NBMATRIZ_ESTABLECER_IDENTIDAD(_cacheGL.unidadesTex[iUTex].matrizTextura)
#		ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT_GL_NOP
		_cacheGL.unidadesTex[iUTex].dbgCantOperacionesConTex = 1;
#		endif
	}
	_cacheGL.ultimoFiltroMinMagTextura	= -100;
	_cacheGL.iluminacionGLHabilitada	= false;
	{
		_cacheGL.colorDeLimpieza.r		= 0.0f;
		_cacheGL.colorDeLimpieza.g		= 0.0f;
		_cacheGL.colorDeLimpieza.b		= 0.0f;
		_cacheGL.colorDeLimpieza.a		= 0.0f;
		glClearColor(_cacheGL.colorDeLimpieza.r, _cacheGL.colorDeLimpieza.g, _cacheGL.colorDeLimpieza.b, _cacheGL.colorDeLimpieza.a);
		GL_CMD_EJECUTADO("glClearColor")
	}
	{
		_cacheGL.colorActivo.r			= 1.0f;
		_cacheGL.colorActivo.g			= 1.0f;
		_cacheGL.colorActivo.b			= 1.0f;
		_cacheGL.colorActivo.a			= 1.0f;
		glColor4f(_cacheGL.colorActivo.r, _cacheGL.colorActivo.g, _cacheGL.colorActivo.b, _cacheGL.colorActivo.a);
		GL_CMD_EJECUTADO("glColor4f(%f, %f, %f, %f)", _cacheGL.colorActivo.r, _cacheGL.colorActivo.g, _cacheGL.colorActivo.b, _cacheGL.colorActivo.a)
	}
	{
		_cacheGL.colorLuzAmbienteActivo.r	= 1.0f;
		_cacheGL.colorLuzAmbienteActivo.g	= 1.0f;
		_cacheGL.colorLuzAmbienteActivo.b	= 1.0f;
		_cacheGL.colorLuzAmbienteActivo.a	= 1.0f;
		{
			GLfloat valores[4] = { _cacheGL.colorLuzAmbienteActivo.r, _cacheGL.colorLuzAmbienteActivo.g, _cacheGL.colorLuzAmbienteActivo.b, _cacheGL.colorLuzAmbienteActivo.a };
			glLightModelfv(GL_LIGHT_MODEL_AMBIENT, valores); GL_CMD_EJECUTADO("glLightModelAmbient(%f, %f, %f, %f)", _cacheGL.colorLuzAmbienteActivo.r, _cacheGL.colorLuzAmbienteActivo.g, _cacheGL.colorLuzAmbienteActivo.b, _cacheGL.colorLuzAmbienteActivo.a)
		}
	}
	_cacheGL.modoMatriz					= GL_MODELVIEW;
	NBMATRIZ_ESTABLECER_CEROS(_cacheGL.matrizModelo);
	NBMATRIZ_ESTABLECER_CEROS(_cacheGL.matrizProyeccion);
	NBRECTANGULO_ESTABLECER(_cacheGL.areaTijeras, 0, 0, 0, 0);
	NBRECTANGULO_ESTABLECER(_cacheGL.puertoDeVision, 0, 0, 0, 0);
	NBCAJAAABB_INICIALIZAR(_cacheGL.cajaProyeccion);
#	ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT_GL_NOP
	_cacheGL.dbgVAONombreFuncion->establecer("NBGestorGL::inicializar");
	_cacheGL.dbgDrawArrayConvocador->establecer("NBGestorGL::inicializar");
	_cacheGL.dbgDrawElementsConvocador->establecer("NBGestorGL::inicializar");
	_cacheGL.dbgLoteTringStripsNombrePurgador->establecer("NBGestorGL::inicializar");
	_cacheGL.dbgVAOCantOperaciones		= 1;
#	endif
	privInicializarCacheVAO(&_cacheGL.vaoEstado);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
}

void NBGestorGL::privInicializarVAOs(){
	//Determinar si los VAO conservan referencias a VBOs
	if(!_soportaVAOs){
		_soportaVAOArrayBufferBinding	= false;
		_soportaVAOIndexBufferBinding	= false;
	} else {
		_soportaVAOArrayBufferBinding	= true;
		_soportaVAOIndexBufferBinding	= true;
		GLint idArrayBufferActual = 0, idElemArrayBufferActual = 0;
		//Ligar al primer VAO (y comparar VBOs auto-ligados)
		const ENVerticeGlTipo tipoPrimero	= (ENVerticeGlTipo)0;
		{
			const STArregloVerticesGL* datArrPrimero = &_vertsGlArrs[tipoPrimero];
			NBGestorGL::privBindVertexArrayEXTParaInicializacion(tipoPrimero, datArrPrimero->idVertexArrayObjectGl); NBASSERT(datArrPrimero->idVertexArrayObjectGl != 0)
			//Verificar GL_ARRAY_BUFFER_BINDING
			{
				glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &idArrayBufferActual);
				//PRINTF_INFO("VAO-tip(%d) GL_ARRAY_BUFFER_BINDING act(%d) esperado(%d)%s.\n", tipoPrimero, idArrayBufferActual, datArrPrimero->idBufferGlVertices, (idArrayBufferActual != datArrPrimero->idBufferGlVertices ? " --DIFF--" : ""));
				_soportaVAOArrayBufferBinding = (idArrayBufferActual == datArrPrimero->idBufferGlVertices); NBASSERT(datArrPrimero->idBufferGlVertices != 0)
			}
			//Verificar GL_ELEMENT_ARRAY_BUFFER_BINDING
			{
				glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &idElemArrayBufferActual);
				//PRINTF_INFO("VAO-tip(%d) GL_ELEMENT_ARRAY_BUFFER_BINDING act(%d) esperado(%d)%s.\n", tipoPrimero, idElemArrayBufferActual, datArrPrimero->idBufferGlIndices, (idElemArrayBufferActual != datArrPrimero->idBufferGlIndices ? " --DIFF--" : ""));
				_soportaVAOIndexBufferBinding	= (idElemArrayBufferActual == datArrPrimero->idBufferGlIndices); NBASSERT(datArrPrimero->idBufferGlIndices != 0)
			}
		}
		//Ligar al ultimo VAO (y comparar VBOs auto-ligados)
		if(_soportaVAOArrayBufferBinding || _soportaVAOIndexBufferBinding){
			const ENVerticeGlTipo tipoUltimo	= (ENVerticeGlTipo)(ENVerticeGlTipo_Conteo - 1); NBASSERT(tipoPrimero < tipoUltimo) //Debe existir por lo menos uno de separacion
			const STArregloVerticesGL* datArrUltimo = &_vertsGlArrs[tipoUltimo];
			NBGestorGL::privBindVertexArrayEXTParaInicializacion(tipoUltimo, datArrUltimo->idVertexArrayObjectGl); NBASSERT(datArrUltimo->idVertexArrayObjectGl != 0)
			//Verificar GL_ARRAY_BUFFER_BINDING
			{
				glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &idArrayBufferActual);
				//PRINTF_INFO("VAO-tip(%d) GL_ARRAY_BUFFER_BINDING act(%d) esperado(%d)%s.\n", tipoUltimo, idArrayBufferActual, datArrUltimo->idBufferGlVertices, (idArrayBufferActual != datArrUltimo->idBufferGlVertices ? " --DIFF--" : ""));
				if(_soportaVAOArrayBufferBinding){
					_soportaVAOArrayBufferBinding = (idArrayBufferActual == datArrUltimo->idBufferGlVertices); NBASSERT(datArrUltimo->idBufferGlVertices != 0)
				}
			}
			//Verificar GL_ELEMENT_ARRAY_BUFFER_BINDING
			{
				glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &idElemArrayBufferActual);
				//PRINTF_INFO("VAO-tip(%d) GL_ELEMENT_ARRAY_BUFFER_BINDING act(%d) esperado(%d)%s.\n", tipoUltimo, idElemArrayBufferActual, datArrUltimo->idBufferGlIndices, (idElemArrayBufferActual != datArrUltimo->idBufferGlIndices ? " --DIFF--" : ""));
				if(_soportaVAOIndexBufferBinding){
					_soportaVAOIndexBufferBinding	= (idElemArrayBufferActual == datArrUltimo->idBufferGlIndices); NBASSERT(datArrUltimo->idBufferGlIndices != 0)
				}
			}
		}
		PRINTF_INFO("Actual GL_ARRAY_BUFFER_BINDING(%d, %s soporta binding) GL_ELEMENT_ARRAY_BUFFER_BINDING(%d, %s soporta binding).\n", idArrayBufferActual, (_soportaVAOArrayBufferBinding ? "si" : "no"), idElemArrayBufferActual, (_soportaVAOIndexBufferBinding ? "si" : "no"));
		GL_CMD_EJECUTADO("VAOBinding Verification")
		//Actualizar vaoEstado
		_cacheGL.idBufferArrayLigado = idArrayBufferActual;
#		ifdef CONFIG_NB_GESTOR_ESCENAS_MODELOS_MEDIANTE_INDICES
		_cacheGL.idBufferElemsLigado  = idElemArrayBufferActual;
#		endif
	}
}

bool NBGestorGL::reiniciarContextoGrafico(const bool elimObjetosAnteriores){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::reiniciarContextoGrafico")
	bool exito = false;
	NBASSERT(_gestorInicializado)
	if(_gestorInicializado){
		//Reiniciar cache
		NBGestorGL::privInicializarCaches(false);
		//Reinicar VBOs de vertices e indices, y VAOs
		NBASSERT(glGenBuffers != NULL) //Si falla, probablemente falta inicializar GLEW
		GL_CMD_EJECUTADO("REinicializando GestorGL") //Verifica que no se esta iniciando con un errorGL acumulado
		SI32 i;
		for(i = 0; i < _vertsGlArrsTam; i++){
			STArregloVerticesGL* datVao = &_vertsGlArrs[i];
			if(datVao->registroOcupado){
				const ENVerticeGlTipo tipo = datVao->tipoVertices;
				NBGESTORGL_INICIALIZAR_ARREGLO_VERTICESGL(datVao, tipo);
			}
		}
		//Determinar si los VAO conservan referencias a VBOs
		NBGestorGL::privInicializarVAOs();
		//
		exito = true;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
	return exito;
}

//Bufferes de verticesGL

SI32 NBGestorGL::bufferVerticesReservar(const ENVerticeGlTipo tipoVerticesGl, PTRfuncObtenerDatosVao funcObtenerDatos, void* funcObtenerDatosParam){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorGL::bufferVerticesReservar")
	SI32 i, r = -1;
	//Buscar un registro disponible
	for(i = 0; i < _vertsGlArrsTam; i++){
		STArregloVerticesGL* ptrDat = &_vertsGlArrs[i];
		if(!ptrDat->registroOcupado){
			r = i;
			break;
		}
	}
	//Agrandar arreglo
	if(r == -1){
		SI32 i;
		const SI32 tamAnterior = _vertsGlArrsTam; _vertsGlArrsTam++;
		STArregloVerticesGL* arrNvo = (STArregloVerticesGL*)NBGestorMemoria::reservarMemoria(sizeof(STArregloVerticesGL) * _vertsGlArrsTam, ENMemoriaTipo_General);
		if(_vertsGlArrs != NULL){
			for(i = 0; i < tamAnterior; i++){
				arrNvo[i] = _vertsGlArrs[i];
			}
			NBGestorMemoria::liberarMemoria(_vertsGlArrs);
		}
		for(i = tamAnterior; i < _vertsGlArrsTam; i++){
			arrNvo[i].registroOcupado = false;
		}
		_vertsGlArrs = arrNvo;
		r = tamAnterior;
	}
	NBASSERT(r >= 0 && r < _vertsGlArrsTam)
	if(r != -1){
		STArregloVerticesGL* ptrDat = &_vertsGlArrs[r];
		ptrDat->registroOcupado			= true;
		ptrDat->tipoVertices			= tipoVerticesGl;
		ptrDat->funcObtenerDatos		= funcObtenerDatos;
		ptrDat->funcObtenerDatosParam	= funcObtenerDatosParam;
		NBGESTORGL_INICIALIZAR_ARREGLO_VERTICESGL(ptrDat, tipoVerticesGl);
	}
	//
	//Determinar si los VAO conservan referencias a VBOs
	if(tipoVerticesGl == (ENVerticeGlTipo_Conteo - 1) && !_soportaVAOsAnalizado){
		NBGestorGL::privInicializarVAOs();
		_soportaVAOsAnalizado = true;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
	return r;
}

void NBGestorGL::bufferVerticesLiberar(const SI32 iBuffer){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorGL::bufferVerticesLiberar")
	NBASSERT(iBuffer >= 0 && iBuffer < _vertsGlArrsTam)
	if(iBuffer >= 0 && iBuffer < _vertsGlArrsTam){
		STArregloVerticesGL* ptrDat = &_vertsGlArrs[iBuffer];
		NBASSERT(ptrDat->registroOcupado)
		if(ptrDat->registroOcupado){
#			ifdef CONFIG_NB_GESTOR_ESCENAS_MODELOS_MEDIANTE_INDICES
			if(ptrDat->idBufferGlIndices != 0){
				glDeleteBuffers(1, &(ptrDat->idBufferGlIndices)); GL_CMD_EJECUTADO("glDeleteBuffers(1, %u)", (UI32)ptrDat->idBufferGlIndices)
				ptrDat->idBufferGlIndices = 0;
			}
#			endif
			if(ptrDat->idBufferGlVertices != 0){
				glDeleteBuffers(1, &(ptrDat->idBufferGlVertices)); GL_CMD_EJECUTADO("glDeleteBuffers(1, %u)", (UI32)ptrDat->idBufferGlVertices)
				ptrDat->idBufferGlVertices = 0;
			}
			if(ptrDat->idVertexArrayObjectGl != 0){
				NBGestorGL::privDeleteVertexArraysEXT(1, &(ptrDat->idVertexArrayObjectGl));
				ptrDat->idVertexArrayObjectGl = 0;
			}
			ptrDat->registroOcupado = false;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

//

void NBGestorGL::enableInit(GLenum cap){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::enableInit")
	switch (cap){
		case GL_TEXTURE_2D: _cacheGL.unidadesTex[_cacheGL.iUnidadTexturaActivaServidor].texture2DHabilitada = true; break;
		case GL_LIGHTING: _cacheGL.iluminacionGLHabilitada = true; break;
		default: break;
	}
	NBGESTORGL_LOTES_TODOS_PURGAR("NBGestorGL::enable")
	glEnable(cap); GL_CMD_EJECUTADO("glEnable(%d = %s)", cap, STR_GL_CAP(cap))
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
}

void NBGestorGL::enable(GLenum cap){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::enable")
	bool aplicar = false;
#	ifdef CONFIG_NB_GESTOR_GL_IMPRIMIR_COMANDOS_GL
	SI32 paramExtra = -1;
#	endif
	switch (cap){
		case GL_TEXTURE_2D:
			NBGL_ASSERT_CACHE_ACTIVE_TEXTURE
			if(!_cacheGL.unidadesTex[_cacheGL.iUnidadTexturaActivaServidor].texture2DHabilitada){
#				ifdef CONFIG_NB_GESTOR_GL_IMPRIMIR_COMANDOS_GL
				paramExtra = _cacheGL.iUnidadTexturaActivaServidor;
#				endif
				aplicar = true; _cacheGL.unidadesTex[_cacheGL.iUnidadTexturaActivaServidor].texture2DHabilitada = true;
			}
			break;
		case GL_LIGHTING:
			if(!_cacheGL.iluminacionGLHabilitada){
				aplicar = true; _cacheGL.iluminacionGLHabilitada = true;
			}
			break;
		default:
			aplicar = true;
			break;
	}
	if(aplicar){
		NBGESTORGL_LOTES_TODOS_PURGAR("NBGestorGL::enable")
		glEnable(cap); GL_CMD_EJECUTADO("glEnable(%s, %d)", STR_GL_CAP(cap), paramExtra)
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
}

void NBGestorGL::disableInit(GLenum cap){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::disable")
	switch (cap){
		case GL_TEXTURE_2D: _cacheGL.unidadesTex[_cacheGL.iUnidadTexturaActivaServidor].texture2DHabilitada = false; break;
		case GL_LIGHTING: _cacheGL.iluminacionGLHabilitada = false; break;
		default: break;
	}
	NBGESTORGL_LOTES_TODOS_PURGAR("NBGestorGL::disable")
	glDisable(cap); GL_CMD_EJECUTADO("glDisable(%d = %s)", cap, STR_GL_CAP(cap))
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
}

void NBGestorGL::disable(GLenum cap){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::disable")
	bool aplicar = false;
#	ifdef CONFIG_NB_GESTOR_GL_IMPRIMIR_COMANDOS_GL
	SI32 paramExtra = -1;
#	endif
	switch (cap){
		case GL_TEXTURE_2D:
			NBGL_ASSERT_CACHE_ACTIVE_TEXTURE
			if(_cacheGL.unidadesTex[_cacheGL.iUnidadTexturaActivaServidor].texture2DHabilitada){
#				ifdef CONFIG_NB_GESTOR_GL_IMPRIMIR_COMANDOS_GL
				paramExtra = _cacheGL.iUnidadTexturaActivaServidor;
#				endif
				aplicar = true; _cacheGL.unidadesTex[_cacheGL.iUnidadTexturaActivaServidor].texture2DHabilitada = false;
			}
			break;
		case GL_LIGHTING:
			if(_cacheGL.iluminacionGLHabilitada){
				aplicar = true; _cacheGL.iluminacionGLHabilitada = false;
			}
			break;
		default:
			aplicar = true;
			break;
	}
	if(aplicar){
		NBGESTORGL_LOTES_TODOS_PURGAR("NBGestorGL::disable")
		glDisable(cap); GL_CMD_EJECUTADO("glDisable(%s, %d)", STR_GL_CAP(cap), paramExtra)
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
}

void NBGestorGL::blendFuncEstablecerValoresPorDefecto(GLenum sFactorRGBA, GLenum dFactorRGBA){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::blendFuncEstablecerValoresPorDefecto")
	_blendSoucFactorRGBAPorDefecto	= sFactorRGBA;
	_blendDestFactorRGBAPorDefecto	= dFactorRGBA;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
}

void NBGestorGL::blendFuncCargarValoresPorDefecto(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::blendFuncCargarValoresPorDefecto")
	NBGestorGL::blendFunc(_blendSoucFactorRGBAPorDefecto, _blendDestFactorRGBAPorDefecto);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
}

void NBGestorGL::blendFunc(GLenum sFactor, GLenum dFactor){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::blendFunc")
	if(_blendSoucFactorRGBAActual != sFactor || _blendDestFactorRGBAActual != dFactor){
		NBGESTORGL_LOTES_TODOS_PURGAR("NBGestorGL::blendFunc")
		glBlendFunc(sFactor, dFactor);
		GL_CMD_EJECUTADO("glBlendFunc")
		_blendSoucFactorRGBAActual		= sFactor;
		_blendDestFactorRGBAActual		= dFactor;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
}

/*void NBGestorGL::blendFuncSeparate(GLenum sFactorRGB, GLenum dFactorRGB, GLenum sFactorAlpha, GLenum dFactorAlpha){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::blendFuncSeparate")
	NBGESTORGL_LOTES_TODOS_PURGAR("NBGestorGL::blendFuncSeparate")
#	ifdef NB_LIB_GRAFICA_ES_EMBEDDED
	glBlendFuncSeparateOES(sFactorRGB, dFactorRGB, sFactorAlpha, dFactorAlpha);
#	else
	glBlendFuncSeparate(sFactorRGB, dFactorRGB, sFactorAlpha, dFactorAlpha);
#	endif
	GL_CMD_EJECUTADO("glBlendFuncSeparate")
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
}*/

//----------------
//- AMD recomiendoa evitar el uso de colorMask
//----------------
void NBGestorGL::colorMask(GLboolean r, GLboolean g, GLboolean b, GLboolean a){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::colorMask")
	NBGESTORGL_LOTES_TODOS_PURGAR("NBGestorGL::colorMask")
	glColorMask(r, g, b, a);
	GL_CMD_EJECUTADO("glColorMask(r(%s), g(%s), b(%s), a(%s))", (r ? "si" : "no"), (g ? "si" : "no"), (b ? "si" : "no"), (a ? "si" : "no"))
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
}

void NBGestorGL::clear(GLbitfield mask){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::clear")
	NBGESTORGL_LOTES_TODOS_ASSERT_ESTAN_VACIOS	//Si falla, se omitió flushear los comandos acumulados
	//PRINTF_INFO("glClear frame(%d) render(%d).\n", _cacheGL.idFrameBufferGlLigado, _cacheGL.idRenderBufferGlLigado)
	//NBASSERT(_cacheGL.idFrameBufferGlLigado >= 0)
	glClear(mask);
	GL_CMD_EJECUTADO("glClear(mask(%d))", (SI32)mask)
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
}

void NBGestorGL::matrixModeInit(GLenum mode){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::matrixModeInit")
	NBGESTORGL_LOTES_TODOS_ASSERT_ESTAN_VACIOS	//Si falla, se omitió flushear los comandos acumulados
	glMatrixMode(mode); GL_CMD_EJECUTADO("glMatrixMode(%s)", STR_GL_MATRIX_MODE(mode))
	glLoadIdentity(); GL_CMD_EJECUTADO("glLoadIdentity")
	_cacheGL.modoMatriz = mode;
	switch(_cacheGL.modoMatriz){
		case GL_MODELVIEW:	NBMATRIZ_ESTABLECER_IDENTIDAD(_cacheGL.matrizModelo); break;
		case GL_PROJECTION: NBMATRIZ_ESTABLECER_IDENTIDAD(_cacheGL.matrizProyeccion); break;
		case GL_TEXTURE:	_cacheGL.unidadesTex[_cacheGL.iUnidadTexturaActivaServidor].escalaCoordenadas = 1.0f; NBMATRIZ_ESTABLECER_IDENTIDAD(_cacheGL.unidadesTex[_cacheGL.iUnidadTexturaActivaServidor].matrizTextura); break;
		default:			NBASSERT(false); break; //Modo matriz actual no es valido
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
}

void NBGestorGL::matrixMode(GLenum mode){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::matrixMode")
	NBGESTORGL_LOTES_TODOS_ASSERT_ESTAN_VACIOS	//Si falla, se omitió flushear los comandos acumulados
	if(_cacheGL.modoMatriz != mode){
		glMatrixMode(mode);
		GL_CMD_EJECUTADO("glMatrixMode(%s)", STR_GL_MATRIX_MODE(mode))
		_cacheGL.modoMatriz = mode;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
}

void NBGestorGL::loadIdentity(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::loadIdentity")
	NBGESTORGL_LOTES_TODOS_ASSERT_ESTAN_VACIOS	//Si falla, se omitió flushear los comandos acumulados
	//if(_cacheGL.matricesActuales[_cacheGL.modoMatriz] != _matrizIdentidad){
		glLoadIdentity(); GL_CMD_EJECUTADO("glLoadIdentity")
		switch(_cacheGL.modoMatriz){
			case GL_MODELVIEW:	NBMATRIZ_ESTABLECER_IDENTIDAD(_cacheGL.matrizModelo); break;
			case GL_PROJECTION: NBMATRIZ_ESTABLECER_IDENTIDAD(_cacheGL.matrizProyeccion); break;
			case GL_TEXTURE:	_cacheGL.unidadesTex[_cacheGL.iUnidadTexturaActivaServidor].escalaCoordenadas = 1.0f; NBMATRIZ_ESTABLECER_IDENTIDAD(_cacheGL.unidadesTex[_cacheGL.iUnidadTexturaActivaServidor].matrizTextura); break;
			default:			NBASSERT(false); break; //Modo matriz actual no es valido
		}
	//}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
}

void NBGestorGL::scale3f(GLfloat x, GLfloat y, GLfloat z){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::scale3f")
	glScalef(x, y, z);
	GL_CMD_EJECUTADO("glScalef")
	switch(_cacheGL.modoMatriz){
		case GL_MODELVIEW:	NBMATRIZ_ESCALAR(_cacheGL.matrizModelo, x, y); break;
		case GL_PROJECTION: NBMATRIZ_ESCALAR(_cacheGL.matrizProyeccion, x, y); break;
		case GL_TEXTURE:	NBMATRIZ_ESCALAR(_cacheGL.unidadesTex[_cacheGL.iUnidadTexturaActivaServidor].matrizTextura, x, y); break;
		default:			NBASSERT(false); break; //Modo matriz actual no es valido
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
}

void NBGestorGL::loadMatrixf(const NBMatriz &matriz){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::loadMatrix")
	NBGESTORGL_LOTES_TODOS_ASSERT_ESTAN_VACIOS	//Si falla, se omitió flushear los comandos acumulados
	//if(_cacheGL.matricesActuales[_cacheGL.modoMatriz] != matriz){
		//Nota: OpenGL espera la matriz 4x4 en el siguiente orden:
		// m[00] m[04] m[08] m[12]
		// m[01] m[05] m[09] m[13]
		// m[02] m[06] m[10] m[14]
		// m[03] m[07] m[11] m[15]
		//La conversion de matriz 3x3
		//[-0.44]	[0.90]		[500.50]
		//[-0.90]	[-0.44]		[-405.62]
		//[0.00]	[0.00]		[1.00]
		//A matriz 4x4 es la siguiente:
		//[-0.44]	[0.90]		[0.00]		[500.50]
		//[-0.90]	[-0.44]		[0.00]		[-405.62]
		//[-0.00]	[0.00]		[1.00]		[0.00]
		//[-0.00]	[0.00]		[0.00]		[1.00]
		GLfloat matrizGL[16];
		matrizGL[0] = NBMATRIZ_ELEM00(matriz);	matrizGL[4] = NBMATRIZ_ELEM01(matriz);	matrizGL[8] = 0.0f;		matrizGL[12] = NBMATRIZ_ELEM02(matriz);
		matrizGL[1] = NBMATRIZ_ELEM10(matriz);	matrizGL[5] = NBMATRIZ_ELEM11(matriz);	matrizGL[9] = 0.0f;		matrizGL[13] = NBMATRIZ_ELEM12(matriz);
		matrizGL[2] = 0.0f;						matrizGL[6] = 0.0f;						matrizGL[10] = 1.0f;	matrizGL[14] = 0.0f;
		matrizGL[3] = 0.0f;						matrizGL[7] = 0.0f;						matrizGL[11] = 0.0f;	matrizGL[15] = 1.0f;
		glLoadMatrixf(matrizGL);
		GL_CMD_EJECUTADO("glLoadMatrixf")
		switch(_cacheGL.modoMatriz){
			case GL_MODELVIEW:	_cacheGL.matrizModelo = matriz; break;
			case GL_PROJECTION: _cacheGL.matrizProyeccion = matriz; break;
			case GL_TEXTURE:	_cacheGL.unidadesTex[_cacheGL.iUnidadTexturaActivaServidor].matrizTextura = matriz; break;
			default:			NBASSERT(false); break; //Modo matriz actual no es valido
		}
	//}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
}

#ifdef NB_GESTOR_ESCENAS_USA_CACHE_LOTE_INSTRUCCIONES
void NBGestorGL::flushCacheLocal(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::flushCacheLocal")
	NBASSERT(_gestorInicializado);
	NBGESTORGL_LOTES_TODOS_PURGAR("NBGestorGL::flush")
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
}
#endif

void NBGestorGL::flush(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::flush")
	NBASSERT(_gestorInicializado);
	NBGESTORGL_LOTES_TODOS_PURGAR("NBGestorGL::flush")
	glFlush();
	GL_CMD_EJECUTADO("glFlush")
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
}

void NBGestorGL::finish(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::finish")
	NBASSERT(_gestorInicializado);
	NBGESTORGL_LOTES_TODOS_PURGAR("NBGestorGL::finish")
	glFinish();
	GL_CMD_EJECUTADO("glFinish")
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
}

void NBGestorGL::viewport(GLint x, GLint y, GLsizei width, GLsizei height){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::viewport")
	NBGESTORGL_LOTES_TODOS_ASSERT_ESTAN_VACIOS	//Si falla, se omitió flushear los comandos acumulados
	if(_cacheGL.puertoDeVision.x != x || _cacheGL.puertoDeVision.y != y || _cacheGL.puertoDeVision.ancho != width || _cacheGL.puertoDeVision.alto != height){
		glViewport(x, y, width, height);
		GL_CMD_EJECUTADO("glViewport")
		_cacheGL.puertoDeVision.x		= x;
		_cacheGL.puertoDeVision.y		= y;
		_cacheGL.puertoDeVision.ancho	= width;
		_cacheGL.puertoDeVision.alto	= height;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
}

void NBGestorGL::orthof(GLfloat xLeft, GLfloat xRight, GLfloat yBottom, GLfloat yTop){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::orthof")
	NBGESTORGL_LOTES_TODOS_ASSERT_ESTAN_VACIOS	//Si falla, se omitió flushear los comandos acumulados
	if(xLeft != xRight && yBottom != yTop){
		//PRINTF_INFO("NBGestorGL::orthof, (%f, %f)-(%f, %f)\n", _cajaProyeccionOrtogonal.xMin, _cajaProyeccionOrtogonal.yMin, _cajaProyeccionOrtogonal.xMax, _cajaProyeccionOrtogonal.yMax);
#		ifdef NB_LIB_GRAFICA_ES_EMBEDDED
		glOrthof(xLeft, xRight, yBottom, yTop, -1.0f, 1.0f);
#		else
		glOrtho(xLeft, xRight, yBottom, yTop, -1.0f, 1.0f);
#		endif
		_cacheGL.cajaProyeccion.xMin		= xLeft;
		_cacheGL.cajaProyeccion.xMax		= xRight;
		_cacheGL.cajaProyeccion.yMin		= yBottom;
		_cacheGL.cajaProyeccion.yMax		= yTop;
		GL_CMD_EJECUTADO("glOrthof(%f, %f, %f, %f, %f, %f)", (float)xLeft, (float)xRight, (float)yBottom, (float)yTop, -1.0f, 1.0f)
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
}

void NBGestorGL::clearColor(const NBColor &colorDeLimpieza){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::clearColor")
	NBGESTORGL_LOTES_TODOS_ASSERT_ESTAN_VACIOS	//Si falla, se omitió flushear los comandos acumulados
	if(_cacheGL.colorDeLimpieza.r != colorDeLimpieza.r || _cacheGL.colorDeLimpieza.g != colorDeLimpieza.g || _cacheGL.colorDeLimpieza.b != colorDeLimpieza.b || _cacheGL.colorDeLimpieza.a != colorDeLimpieza.a){
		glClearColor(colorDeLimpieza.r, colorDeLimpieza.g, colorDeLimpieza.b, colorDeLimpieza.a);
		GL_CMD_EJECUTADO("glClearColor")
		_cacheGL.colorDeLimpieza.r = colorDeLimpieza.r;
		_cacheGL.colorDeLimpieza.g = colorDeLimpieza.g;
		_cacheGL.colorDeLimpieza.b = colorDeLimpieza.b;
		_cacheGL.colorDeLimpieza.a = colorDeLimpieza.a;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
}

void NBGestorGL::clearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::clearColor")
	NBGESTORGL_LOTES_TODOS_ASSERT_ESTAN_VACIOS	//Si falla, se omitió flushear los comandos acumulados
	if(_cacheGL.colorDeLimpieza.r != red || _cacheGL.colorDeLimpieza.g != green || _cacheGL.colorDeLimpieza.b != blue || _cacheGL.colorDeLimpieza.a != alpha){
		glClearColor(red, green, blue, alpha);
		GL_CMD_EJECUTADO("glClearColor")
		_cacheGL.colorDeLimpieza.r = red;
		_cacheGL.colorDeLimpieza.g = green;
		_cacheGL.colorDeLimpieza.b = blue;
		_cacheGL.colorDeLimpieza.a = alpha;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
}

void NBGestorGL::scissor(GLint x, GLint y, GLsizei width, GLsizei height){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::scissor")
	NBGESTORGL_LOTES_TODOS_ASSERT_ESTAN_VACIOS	//Si falla, se omitió flushear los comandos acumulados
	if(_cacheGL.areaTijeras.x != x || _cacheGL.areaTijeras.y != y || _cacheGL.areaTijeras.ancho != width || _cacheGL.areaTijeras.alto != height){
		glScissor(x, y, width, height); GL_CMD_EJECUTADO("glScissor")
		_cacheGL.areaTijeras.x		= x;
		_cacheGL.areaTijeras.y		= y;
		_cacheGL.areaTijeras.ancho	= width;
		_cacheGL.areaTijeras.alto	= height;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
}

/*void NBGestorGL::stencilFunc(GLenum func, GLint ref, GLuint mask){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::stencilFunc")
	glStencilFunc(func, ref, mask); GL_CMD_EJECUTADO("glStencilFunc")
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
}*/

/*void NBGestorGL::stencilOp(GLenum sfail, GLenum dpfail, GLenum dppass){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::stencilOp")
	glStencilOp(sfail, dpfail, dppass); GL_CMD_EJECUTADO("glStencilOp")
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
}*/

void NBGestorGL::activeTexture(SI32 indiceUnidadTextura){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::activeTexture")
	NBASSERT(indiceUnidadTextura >= 0 && indiceUnidadTextura < _maxUnidadesTexUsar);
	NBGL_ASSERT_CACHE_ACTIVE_TEXTURE
	if(_cacheGL.iUnidadTexturaActivaServidor != indiceUnidadTextura){
		NBGESTORGL_LOTES_TODOS_PURGAR("NBGestorGL::activeTexture")
		const GLenum enumTexturaGl = NBGestorGL::privUnidadTexturaEnumGl(indiceUnidadTextura);
		glActiveTexture(enumTexturaGl); GL_CMD_EJECUTADO("glActiveTexture(%s)", STR_GL_TEX_UNIT(enumTexturaGl))
		_cacheGL.iUnidadTexturaActivaServidor = indiceUnidadTextura;
		NBGL_ASSERT_CACHE_ACTIVE_TEXTURE
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
}

bool NBGestorGL::soportaTexturas2DNoMultiploDe2(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::soportaTexturas2DNoMultiploDe2")
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
	return _soportaTexturas2DNoMultiploDe2;
}

bool NBGestorGL::soportaDrawTexture(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::soportaDrawTexture")
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
	return _soportaDrawTexture;
}

void NBGestorGL::genTextures(GLsizei n, GLuint* textures, STGestorGLTextura* propsTexturas){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::genTextures")
	NBASSERT(n > 0)
	NBASSERT(textures != NULL)
	NBASSERT(propsTexturas != NULL)
	glGenTextures(n, textures); GL_CMD_EJECUTADO("glGenTextures(%d, primerRet(%u))", (SI32)n, (UI32)textures[0])
	//PRINTF_INFO("%d textura(s) generada(s)\n", n);
	GLsizei iTex;
	for(iTex = 0; iTex < n; iTex++){
		NBMemory_setZero(propsTexturas[iTex]);
		propsTexturas[iTex].idTexturaGL		= textures[iTex]; NBASSERT(textures[iTex] != 0) //cero es reservado para deshabilitar textura2D
		propsTexturas[iTex].formatoGL		= GL_NO_ERROR;
		//Configuracion de textura
		propsTexturas[iTex].configWarpST	= GL_NO_ERROR;
		propsTexturas[iTex].configMagFil	= GL_NO_ERROR;
		propsTexturas[iTex].configMinFil	= GL_NO_ERROR;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
}

bool NBGestorGL::isTexture(GLuint texture){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::isTexture")
	bool r = glIsTexture(texture); GL_CMD_EJECUTADO("glIsTexture(%u)", (UI32)texture)
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
	return r;
}

void NBGestorGL::deleteTextures(GLsizei n, GLuint* textures){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::deleteTextures")
	NBGESTORGL_LOTES_TODOS_ASSERT_ESTAN_VACIOS	//Si falla, se omitió flushear los comandos acumulados
	glDeleteTextures(n, textures);
	GL_CMD_EJECUTADO("glDeleteTextures(%d, primera(%u))", (SI32)n, (UI32)textures[0])
	//--------------
	//- Actualizar la cache, desligar de las unidades texturas
	//--------------
	SI32 iTex;
	for(iTex=0; iTex < n; iTex++){
		SI32 iTexU;
		for(iTexU=0; iTexU < _maxUnidadesTexUsar; iTexU++){
			if(_cacheGL.unidadesTex[iTexU].idTexturaLigada == textures[iTex]){
				_cacheGL.unidadesTex[iTexU].idTexturaLigada = 0;
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
}

void NBGestorGL::generateMipmapEXT(GLenum target, STGestorGLTextura* propsTextura){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::generateMipmapEXT")
	NBASSERT(propsTextura->mipMapHabilitado)
#	ifdef NB_LIB_GRAFICA_ES_EMBEDDED
	{
		glGenerateMipmapOES(target);
		GL_CMD_EJECUTADO("glGenerateMipmapOES(%d)", target)
	}
#	else
	{
		glGenerateMipmapEXT(target);
		GL_CMD_EJECUTADO("glGenerateMipmapEXT(%d)", target)
	}
#	endif
	propsTextura->mipMapSucio	= false;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
}

void NBGestorGL::privGenVertexArraysEXT(GLsizei n, GLuint* ids){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::privGenVertexArraysEXT")
#	ifdef NB_LIB_GRAFICA_ES_EMBEDDED
#	ifndef __ANDROID__ //TEMPORAL
	glGenVertexArraysOES(n, ids);	GL_CMD_EJECUTADO("glGenVertexArraysOES(%d, primerRet(%u))", (SI32)n, (UI32)ids[0])
#	endif
#	elif defined(WIN32) || defined(_WIN32)
	glGenVertexArrays(n, ids);		GL_CMD_EJECUTADO("glGenVertexArrays(%d, primerRet(%u))", (SI32)n, (UI32)ids[0])
#	else
	glGenVertexArraysAPPLE(n, ids);	GL_CMD_EJECUTADO("glGenVertexArraysAPPLE(%d, primerRet(%u))", (SI32)n, (UI32)ids[0])
#	endif
	//PRINTF_INFO("%d vertexArray(s) generado(s)\n", n);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
}

void NBGestorGL::privDeleteVertexArraysEXT(GLsizei n, GLuint* ids){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::privDeleteVertexArraysEXT")
#	ifdef NB_LIB_GRAFICA_ES_EMBEDDED
#	ifndef __ANDROID__ //TEMPORAL
	glDeleteVertexArraysOES(n, ids);
#	endif
#	elif defined(WIN32) || defined(_WIN32)
	glDeleteVertexArrays(n, ids);
#	else
	glDeleteVertexArraysAPPLE(n, ids);
#	endif
	GL_CMD_EJECUTADO("glDeleteVertexArrays(%d, primero(%u))", n, (UI32)ids[0])
	//PRINTF_INFO("%d vertexArray(s) eliminado(s)\n", n);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
}

void NBGestorGL::privBindVertexArrayEXTParaInicializacion(const SI32 indice, GLuint idVertexArrayGL){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::privBindVertexArrayEXTParaInicializacion")
	NBGESTORGL_LOTES_TODOS_ASSERT_ESTAN_VACIOS	//Si falla, se omitió flushear los comandos acumulados
	NBASSERT(indice >= 0 && indice < _vertsGlArrsTam)
	if(_soportaVAOs){
#		ifdef NB_LIB_GRAFICA_ES_EMBEDDED
#			ifndef __ANDROID__ //TEMPORAL
			glBindVertexArrayOES(idVertexArrayGL);
#			endif
#		elif defined(WIN32) || defined(_WIN32)
		glBindVertexArray(idVertexArrayGL);
#		else
		glBindVertexArrayAPPLE(idVertexArrayGL);
#		endif
		GL_CMD_EJECUTADO("glBindVertexArray(%u)init", (UI32)idVertexArrayGL)
	}
	_cacheGL.iVaoLigado = indice;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
}

void NBGestorGL::privBindVertexArrayEXT(const SI32 indice, const bool prepararParaConsumir){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::privBindVertexArrayEXT")
	NBGESTORGL_LOTES_TODOS_ASSERT_ESTAN_VACIOS	//Si falla, se omitió flushear los comandos acumulados
	NBASSERT(_cacheGL.iVaoLigado != indice) //Llamada es redundante
	NBASSERT(indice >= 0 && indice < _vertsGlArrsTam)
	//PRINTF_INFO("Ligando VAO tipo(%d)\n", (SI32)tipoVerticesGl);
	if(_soportaVAOs){
		GLuint idVertexArrayGL = 0; if(indice >= 0 && indice < _vertsGlArrsTam) idVertexArrayGL = _vertsGlArrs[indice].idVertexArrayObjectGl;
#		ifdef NB_LIB_GRAFICA_ES_EMBEDDED
#			ifndef __ANDROID__ //TEMPORAL
			glBindVertexArrayOES(idVertexArrayGL);
#			endif
#		elif defined(WIN32) || defined(_WIN32)
		glBindVertexArray(idVertexArrayGL);
#		else
		glBindVertexArrayAPPLE(idVertexArrayGL);
#		endif
		GL_CMD_EJECUTADO("glBindVertexArray(%u)", (UI32)idVertexArrayGL)
	}
	NBASSERT(_soportaVAOs || (_soportaVAOArrayBufferBinding == false && _soportaVAOIndexBufferBinding == false)) //Si no soporta VAO no deberia soportar BuffBinding
	//Asegurar configuracion
	if(indice >= 0 && indice < _vertsGlArrsTam){
		//Ligar VBO de Vertices
		STArregloVerticesGL* datArr = &_vertsGlArrs[indice];
		if(!_soportaVAOArrayBufferBinding){
			glBindBuffer(GL_ARRAY_BUFFER, datArr->idBufferGlVertices); GL_CMD_EJECUTADO("glBindBuffer(GL_ARRAY_BUFFER, %u)", (UI32)datArr->idBufferGlVertices) //PRINTF_INFO("LigandoActiva GL_ARRAY_BUFFER(%d)\n", (SI32)datArr->idBufferGlVertices);
		}
		_cacheGL.idBufferArrayLigado = datArr->idBufferGlVertices;
#		ifdef CONFIG_NB_GESTOR_ESCENAS_MODELOS_MEDIANTE_INDICES
		//Ligar VBO de Indices
		if(!_soportaVAOIndexBufferBinding){
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, datArr->idBufferGlIndices); GL_CMD_EJECUTADO("glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, %u)", (UI32)datArr->idBufferGlIndices) //PRINTF_INFO("LigandoActiva GL_ELEMENT_ARRAY_BUFFER(%d)\n", (SI32)datArr->idBufferGlIndices);
		}
		_cacheGL.idBufferElemsLigado = datArr->idBufferGlIndices;
#		endif
		//Configurar punteros
		if(!_soportaVAOs && prepararParaConsumir){
			NBGestorGL::privConfigurarClienteEstado(datArr->tipoVertices, 0, &datArr->vaoConfig, true/*forzarConfig*/);
		}
	}
	//
	_cacheGL.iVaoLigado = indice;
	NBGL_ASSERT_CACHE_VAO
	NBGL_ASSERT_CACHE_CLIENT_STATE
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
}

void NBGestorGL::privInicializarCacheVAO(STGLEstadoVAO* datosCache){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::privInicializarCacheVAO")
	datosCache->verticesHabilitado		= false;
	datosCache->coloresHabilitado		= false;
	datosCache->verticesSaltos			= 0;
	datosCache->coloresSaltos			= 0;
	datosCache->verticesPuntero			= 0;
	datosCache->coloresPuntero			= 0;
	UI16 iUTex;
	for(iUTex=0; iUTex < NB_GESTOR_GL_MAX_TEXTURA_UNIDADES; iUTex++){
		datosCache->unidadesTex[iUTex].coordsHabilitado	= false;
		datosCache->unidadesTex[iUTex].coordsSaltos		= 0;
		datosCache->unidadesTex[iUTex].coordsPuntero		= 0;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
}

#ifdef CONFIG_NB_GESTOR_GL_VERIFICAR_CACHE_SIEMPRE
void NBGestorGL::privDbgCacheAssertVAOs(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::privDbgCacheAssertVAOs")
	NBASSERT(_cacheGL.iVaoLigado < _vertsGlArrsTam);
	//TODO: reenable this validation.
	/*
	//Verificar VAO de vertices
	{
		GLint idArrayBuffer			= 0; glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &idArrayBuffer);
		NBASSERT(_vertsGlArrs[_cacheGL.vaoTipoLigado].idBufferGlVertices == idArrayBuffer)
		NBASSERT(_cacheGL.idBufferArrayLigado == idArrayBuffer)
	}
	//Verificar VAO de indices
#	ifdef CONFIG_NB_GESTOR_ESCENAS_MODELOS_MEDIANTE_INDICES
	{
		GLint idElemArrayBuffer	= 0; glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &idElemArrayBuffer);
		NBASSERT(_vertsGlArrs[_cacheGL.vaoTipoLigado].idBufferGlIndices == idElemArrayBuffer)
		NBASSERT(_cacheGL.idBufferElemsLigado == idElemArrayBuffer)
	}
#	endif
	GL_CMD_EJECUTADO("     dbg::AssertCacheVAOs")
	*/
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
}
#endif

#ifdef CONFIG_NB_GESTOR_GL_VERIFICAR_CACHE_SIEMPRE
void NBGestorGL::privDbgCacheAssertClientState(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::privDbgCacheAssertClientState")
	//TODO: reenable this validation.
	//
	/*STGLEstadoVAO* datosCache0 = &_vertsGlArrs[ENVerticeGlTipo_SinTextura].vaoConfig;
	STGLEstadoVAO* datosCache1 = &_vertsGlArrs[ENVerticeGlTipo_MonoTextura].vaoConfig;
	STGLEstadoVAO* datosCache2 = &_vertsGlArrs[ENVerticeGlTipo_BiTextura].vaoConfig;
	STGLEstadoVAO* datosCache3 = &_vertsGlArrs[ENVerticeGlTipo_TriTextura].vaoConfig;
	STGLEstadoVAO* datosvaoEstado = &_cacheGL.vaoEstado;*/
	//
	/*
	STGLEstadoVAO* datosCache = (_cacheGL.iVaoLigado < _vertsGlArrsTam ? &_vertsGlArrs[_cacheGL.vaoTipoLigado].vaoConfig : &_cacheGL.vaoEstado);
	const GLboolean enabledVert = glIsEnabled(GL_VERTEX_ARRAY);
	const GLboolean enabledColor = glIsEnabled(GL_COLOR_ARRAY);
	GLint valTexUnit; glGetIntegerv(GL_CLIENT_ACTIVE_TEXTURE, &valTexUnit);
	NBASSERT(enabledVert == datosCache->verticesHabilitado)
	NBASSERT(enabledColor == datosCache->coloresHabilitado)
	NBASSERT(valTexUnit == privUnidadTexturaEnumGl(_cacheGL.iUnidadTexturaActivaCliente))
	//Verificar las texturas
	SI32 iTexU;
	for(iTexU = _maxUnidadesTexUsar - 1; iTexU >= 0; iTexU--){
		const GLenum enumTexturaGl = privUnidadTexturaEnumGl(iTexU);
		glClientActiveTexture(enumTexturaGl); GL_CMD_EJECUTADO("     dbg::glClientActiveTexture(%d)", iTexU)
		const GLboolean enabledTexCoord = glIsEnabled(GL_TEXTURE_COORD_ARRAY);
		NBASSERT(enabledTexCoord == datosCache->unidadesTex[iTexU].coordsHabilitado)
	}
	GL_CMD_EJECUTADO("     dbg::AssertCacheClientState")
	*/
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
}
#endif

#ifdef CONFIG_NB_GESTOR_GL_VERIFICAR_CACHE_SIEMPRE
void NBGestorGL::privDbgCacheAssertActiveTexture(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::privDbgCacheAssertActiveTexture")
	GLint valAct = 0; glGetIntegerv(GL_ACTIVE_TEXTURE, &valAct);
	NBASSERT(valAct == NBGestorGL::privUnidadTexturaEnumGl(_cacheGL.iUnidadTexturaActivaServidor))
	GL_CMD_EJECUTADO("     dbg::AssertCacheActiveTexture")
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
}
#endif

#ifdef CONFIG_NB_GESTOR_GL_VERIFICAR_CACHE_SIEMPRE
void NBGestorGL::privDbgCacheAssertTextureActiveState(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::privDbgCacheAssertTextureActiveState")
	const GLboolean valAct = glIsEnabled(GL_TEXTURE_2D);
	NBASSERT(valAct == _cacheGL.unidadesTex[_cacheGL.iUnidadTexturaActivaServidor].texture2DHabilitada)
	GL_CMD_EJECUTADO("     dbg::AssertCacheTextureActiveState")
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
}
#endif

void NBGestorGL::privConfigurarClienteEstado(const ENVerticeGlTipo tipoVertices, void* punteroPrimerVertice, STGLEstadoVAO* datosCache, const bool forzarConfig){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::privConfigurarClienteEstado")
	NBGESTORGL_LOTES_TODOS_ASSERT_ESTAN_VACIOS	//Si falla, se omitió flushear los comandos acumulados
	NBGL_ASSERT_CACHE_CLIENT_STATE
	BYTE* punteroPrimerByte = (BYTE*)punteroPrimerVertice;
	void* punteroTmp;
#	ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
	NBVerticeGL0 plantilla0;	NBASSERT(sizeof(plantilla0) == (sizeof(plantilla0.x) + sizeof(plantilla0.y) + sizeof(plantilla0.r) + sizeof(plantilla0.g) + sizeof(plantilla0.b) + sizeof(plantilla0.a)))
	NBVerticeGL plantilla1;		NBASSERT(sizeof(plantilla1) == (sizeof(plantilla1.x) + sizeof(plantilla1.y) + sizeof(plantilla1.r) + sizeof(plantilla1.g) + sizeof(plantilla1.b) + sizeof(plantilla1.a) + sizeof(plantilla1.tex.x) + sizeof(plantilla1.tex.y)))
	NBVerticeGL2 plantilla2;	NBASSERT(sizeof(plantilla2) == (sizeof(plantilla2.x) + sizeof(plantilla2.y) + sizeof(plantilla2.r) + sizeof(plantilla2.g) + sizeof(plantilla2.b) + sizeof(plantilla2.a) + sizeof(plantilla2.tex.x) + sizeof(plantilla2.tex.y) + sizeof(plantilla2.tex2.x) + sizeof(plantilla2.tex2.y)))
#	endif
	NBVerticeGL3 plantilla3;	NBASSERT(sizeof(plantilla3) == (sizeof(plantilla3.x) + sizeof(plantilla3.y) + sizeof(plantilla3.r) + sizeof(plantilla3.g) + sizeof(plantilla3.b) + sizeof(plantilla3.a) + sizeof(plantilla3.tex.x) + sizeof(plantilla3.tex.y) + sizeof(plantilla3.tex2.x) + sizeof(plantilla3.tex2.y) + sizeof(plantilla3.tex3.x) + sizeof(plantilla3.tex3.y)))
	UI32 tamPlantilla = 0;
	switch (tipoVertices) {
		case ENVerticeGlTipo_SinTextura:	tamPlantilla = sizeof(NBVerticeGL0); break;
		case ENVerticeGlTipo_MonoTextura:	tamPlantilla = sizeof(NBVerticeGL); break;
		case ENVerticeGlTipo_BiTextura:		tamPlantilla = sizeof(NBVerticeGL2); break;
		case ENVerticeGlTipo_TriTextura:	tamPlantilla = sizeof(NBVerticeGL3); break;
		default: NBASSERT(false); break;
	};
	//Vertices
	if(forzarConfig || !datosCache->verticesHabilitado){
		glEnableClientState(GL_VERTEX_ARRAY);
		GL_CMD_EJECUTADO("glEnableClientState(GL_VERTEX_ARRAY)")
		datosCache->verticesHabilitado = true;
	}
	if(forzarConfig || datosCache->verticesSaltos != tamPlantilla || datosCache->verticesPuntero != punteroPrimerByte){
		glVertexPointer(2, GL_FLOAT, tamPlantilla, (GLvoid*)punteroPrimerByte);
		GL_CMD_EJECUTADO("glVertexPointer(2, GL_FLOAT, %d, %llu)", tamPlantilla, (UI64)punteroPrimerByte)
		datosCache->verticesSaltos	= tamPlantilla;
		datosCache->verticesPuntero	= punteroPrimerByte;
	}
	//Colores
	if(forzarConfig || !datosCache->coloresHabilitado){
		glEnableClientState(GL_COLOR_ARRAY);
		GL_CMD_EJECUTADO("glEnableClientState(GL_COLOR_ARRAY)")
		datosCache->coloresHabilitado = true;
	}
	punteroTmp = (punteroPrimerByte + sizeof(plantilla3.x) + sizeof(plantilla3.y));
	if(forzarConfig || datosCache->coloresSaltos != tamPlantilla || datosCache->coloresPuntero != punteroTmp){
		glColorPointer(4, GL_UNSIGNED_BYTE, tamPlantilla, (GLvoid*)punteroTmp);
		GL_CMD_EJECUTADO("glColorPointer(4, GL_UNSIGNED_BYTE, %d, %llu)", tamPlantilla, (UI64)punteroTmp)
		datosCache->coloresSaltos	= tamPlantilla;
		datosCache->coloresPuntero	= punteroTmp;
	}
	//Configurar unidades de texturas
	SI32 iTexU;
	for(iTexU = _maxUnidadesTexUsar - 1; iTexU >= 0; iTexU--){
		const bool texActivar = (iTexU < tipoVertices);
		punteroTmp = (punteroPrimerByte + sizeof(plantilla3.x) + sizeof(plantilla3.y) + sizeof(plantilla3.r) + sizeof(plantilla3.g) + sizeof(plantilla3.b) + sizeof(plantilla3.a) + ((sizeof(plantilla3.tex.x) + sizeof(plantilla3.tex.y)) * iTexU));
		if(forzarConfig || datosCache->unidadesTex[iTexU].coordsHabilitado != texActivar || (texActivar && (datosCache->unidadesTex[iTexU].coordsSaltos != tamPlantilla || datosCache->unidadesTex[iTexU].coordsPuntero != punteroTmp))){
			if(forzarConfig || _cacheGL.iUnidadTexturaActivaCliente != iTexU){
				const GLenum enumTexturaGl = privUnidadTexturaEnumGl(iTexU);
				glClientActiveTexture(enumTexturaGl); GL_CMD_EJECUTADO("glClientActiveTexture(%s)", STR_GL_TEX_UNIT(enumTexturaGl))
				_cacheGL.iUnidadTexturaActivaCliente = iTexU;
			}
			if(forzarConfig || datosCache->unidadesTex[iTexU].coordsHabilitado != texActivar){
				if(texActivar){
					glEnableClientState(GL_TEXTURE_COORD_ARRAY);
					GL_CMD_EJECUTADO("glEnableClientState(GL_TEXTURE_COORD_ARRAY)")
					datosCache->unidadesTex[iTexU].coordsHabilitado = true;
				} else {
					glDisableClientState(GL_TEXTURE_COORD_ARRAY);
					GL_CMD_EJECUTADO("glDisableClientState(GL_TEXTURE_COORD_ARRAY)")
					datosCache->unidadesTex[iTexU].coordsHabilitado = false;
				}
			}
			if(forzarConfig || (texActivar && (datosCache->unidadesTex[iTexU].coordsSaltos != tamPlantilla || datosCache->unidadesTex[iTexU].coordsPuntero != punteroTmp))){
				glTexCoordPointer(2, GL_FLOAT, tamPlantilla, (GLfloat*)punteroTmp);
				GL_CMD_EJECUTADO("glTexCoordPointer(2, %d, %d, %llu)", (SI32)GL_FLOAT, tamPlantilla, (UI64)punteroTmp)
				datosCache->unidadesTex[iTexU].coordsSaltos		= tamPlantilla;
				datosCache->unidadesTex[iTexU].coordsPuntero	= punteroTmp;
			}
		}
	}
	if(forzarConfig || _cacheGL.iUnidadTexturaActivaCliente != 0){
		const GLenum enumTexturaGl = privUnidadTexturaEnumGl(0);
		glClientActiveTexture(enumTexturaGl); GL_CMD_EJECUTADO("glClientActiveTexture(%s)", STR_GL_TEX_UNIT(enumTexturaGl))
		_cacheGL.iUnidadTexturaActivaCliente = 0;
	}
	NBGL_ASSERT_CACHE_CLIENT_STATE
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
}

SI32 NBGestorGL::privUnidadTexturaIndice(GLenum textura){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::privUnidadTexturaIndice")
	const SI32 indice = (textura - GL_TEXTURE0); NBASSERT(GL_TEXTURE31 == (GL_TEXTURE0 + 31))
	NBASSERT(indice >= 0);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
	return indice;
}

GLenum NBGestorGL::privUnidadTexturaEnumGl(SI32 iTextura){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::privUnidadTexturaEnumGl")
	const GLenum r = GL_TEXTURE0 + iTextura; NBASSERT(GL_TEXTURE31 == (GL_TEXTURE0 + 31))
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
	return r;
}

void NBGestorGL::bindTexture(SI32 indiceUnidadTexturaGL, GLuint texture){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::bindTexture")
	NBASSERT(indiceUnidadTexturaGL >= 0 && indiceUnidadTexturaGL < _maxUnidadesTexUsar);
	STGestorGLUnidadTex* datTexU = &_cacheGL.unidadesTex[indiceUnidadTexturaGL];
	if(datTexU->idTexturaLigada != texture || datTexU->texture2DHabilitada != (texture != 0)){
		NBGESTORGL_LOTES_TODOS_PURGAR("NBGestorGL::bindTexture(1)")
		//Activar unidad textura (si es necesario)
		NBGestorGL::activeTexture(indiceUnidadTexturaGL);
		//Activar o desactivar GL_TEXTURE_2D
		if(texture == 0){ //Cero esta reservado para deshabilitar
			if(datTexU->texture2DHabilitada){
				NBGestorGL::disable(GL_TEXTURE_2D);
				NBASSERT(!datTexU->texture2DHabilitada)
			}
			datTexU->idTexturaLigada = 0;
		} else {
			if(!datTexU->texture2DHabilitada){
				NBGestorGL::enable(GL_TEXTURE_2D);
				NBASSERT(datTexU->texture2DHabilitada)
			}
			//Ligar textura a unidad
			glBindTexture(GL_TEXTURE_2D, texture); GL_CMD_EJECUTADO("glBindTexture(GL_TEXTURE_2D, %u)", (UI32)texture);
			datTexU->idTexturaLigada = texture;
			//
#			ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT_GL_NOP
			NBASSERT(datTexU->dbgCantOperacionesConTex != 0) //Si falla, entonces la textura se ligo, pero no se utilizo (codigo fuente de usuario debe optimizarse)
			datTexU->dbgCantOperacionesConTex = 0;
			if(_cacheGL.iVaoLigado < _vertsGlArrsTam){
				NBGestorGL::privDbgReiniciarCacheVerticesIndicesUsados(_cacheGL.vaoTipoLigado);
			}
#			endif
		}
	}
	/*if(_cacheGL.unidadesTex[_cacheGL.iUnidadTexturaActivaServidor].escalaCoordenadas != escalaCoordenadas){
		NBGESTORGL_LOTES_TODOS_PURGAR("NBGestorGL::bindTexture(2)")
		GLenum modoMatrizActual = _cacheGL.modoMatriz;
		NBMatriz matrizTextura;
		NBMATRIZ_ESTABLECER_IDENTIDAD(matrizTextura);
		NBMATRIZ_ESCALAR(matrizTextura, 1.0f/escalaCoordenadas, 1.0f/escalaCoordenadas);
		NBGestorGL::matrixMode(GL_TEXTURE);
		NBGestorGL::loadMatrixf(matrizTextura);
		NBGestorGL::matrixMode(modoMatrizActual);
		_cacheGL.unidadesTex[_cacheGL.iUnidadTexturaActivaServidor].escalaCoordenadas = escalaCoordenadas;
	}*/
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
}

void NBGestorGL::configurarTextura(const ENTexturaModoPintado modoPintado, const bool habilitarMipMap, STGestorGLTextura* propsTextura){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::configurarTextura")
	NBASSERT(modoPintado == ENTexturaModoPintado_Patron_Suavizado || modoPintado == ENTexturaModoPintado_Patron_Preciso || modoPintado == ENTexturaModoPintado_Imagen_Suavizada || modoPintado == ENTexturaModoPintado_Imagen_Precisa)
	//--------------------------------------------------------------
	//-- Para evitar que el filtrado lineal pinte los texeles/pixeles adyacentes,
	//-- es necesario que las coordenadas relativas en los apunten al centro de los texeles/pixeles.
	//-- Inclusive evitar las coordenadas (0,0) y (1,1) porque estas apuntan a extermos y no a centros de pixeles.
	//-- El filtro GL_LINEAR es ideal para texturas rotadas/aumentadas, pero produce un "blur" aun cuando la imagen se pinta sin rotar.
	//-- EL filtro GL_NEAREST es ideal para pintar imagenes sin rotar, pero se pixelean cuando se rota o aumenta.
	//--------------------------------------------------------------
	GLenum WRAP, MAGF, MINF;
	WRAP	= (modoPintado & NB_TEXTURA_BIT_IMAGEN_PATRON) != 0 ? GL_REPEAT : GL_CLAMP_TO_EDGE;
	MAGF	= (modoPintado & NB_TEXTURA_BIT_PINTADO_PRECISO) != 0 ? GL_NEAREST : GL_LINEAR;
	if(habilitarMipMap){
		//MINF	= (modoPintado & 0x4) != 0 ? GL_NEAREST_MIPMAP_LINEAR : GL_NEAREST_MIPMAP_NEAREST;
		MINF	= (modoPintado & NB_TEXTURA_BIT_PINTADO_PRECISO) != 0 ? GL_LINEAR_MIPMAP_NEAREST :  GL_LINEAR_MIPMAP_LINEAR;
	} else {
		MINF	= (modoPintado & NB_TEXTURA_BIT_PINTADO_PRECISO) != 0 ? GL_NEAREST : GL_LINEAR;
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, WRAP);		GL_CMD_EJECUTADO("glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, %s)", STR_GL_TEX_PARAM(GL_TEXTURE_WRAP_S, WRAP))
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, WRAP);		GL_CMD_EJECUTADO("glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, %s)", STR_GL_TEX_PARAM(GL_TEXTURE_WRAP_T, WRAP))
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, MINF);	GL_CMD_EJECUTADO("glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, %s)", STR_GL_TEX_PARAM(GL_TEXTURE_MIN_FILTER, MINF))
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, MAGF);	GL_CMD_EJECUTADO("glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, %s)", STR_GL_TEX_PARAM(GL_TEXTURE_MAG_FILTER, MAGF))
#	if !defined(WIN32) && !defined(_WIN32)
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, (habilitarMipMap ? GL_TRUE : GL_FALSE));	GL_CMD_EJECUTADO("glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, %s)", (habilitarMipMap ? "GL_TRUE" : "GL_FALSE"))
#	endif
	//NBASSERT(modoPintado < 255)
	if(propsTextura != NULL){
		propsTextura->configPintado		= modoPintado;
		propsTextura->configWarpST		= WRAP;
		propsTextura->configMagFil		= MAGF;
		propsTextura->configMinFil		= MINF;
	}
	//Validar configuracion
	NBASSERT(
			 ((modoPintado == ENTexturaModoPintado_Patron_Suavizado || modoPintado == ENTexturaModoPintado_Patron_Preciso) && WRAP == GL_REPEAT) ||
			 ((modoPintado == ENTexturaModoPintado_Imagen_Suavizada || modoPintado == ENTexturaModoPintado_Imagen_Precisa) && WRAP == GL_CLAMP_TO_EDGE)
			 )
	NBASSERT(
			 ((modoPintado == ENTexturaModoPintado_Patron_Preciso || modoPintado == ENTexturaModoPintado_Imagen_Precisa) && (MAGF == GL_NEAREST && (MINF == GL_NEAREST || MINF == GL_NEAREST_MIPMAP_NEAREST || MINF == GL_LINEAR_MIPMAP_NEAREST))) ||
			 ((modoPintado == ENTexturaModoPintado_Patron_Suavizado || modoPintado == ENTexturaModoPintado_Imagen_Suavizada) && (MAGF == GL_LINEAR && (MINF == GL_LINEAR || MINF == GL_NEAREST_MIPMAP_LINEAR || MINF == GL_LINEAR_MIPMAP_LINEAR)))
			 )
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
}

void NBGestorGL::setTextureCropRect(const STNBRectI16* cropRect){
	GLint params[4];
	if(cropRect == NULL){
		params[0] = params[1] = params[2] = params[3] = 0;
	} else {
		params[0] = cropRect->x;
		params[1] = cropRect->y;
		params[2] = cropRect->width;
		params[3] = cropRect->height;
	}
#	ifdef GL_TEXTURE_CROP_RECT_OES
	glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_CROP_RECT_OES, params); GL_CMD_EJECUTADO("glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_CROP_RECT_OES)");
#	endif
}

void NBGestorGL::texImage2D(GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid* pixels, STGestorGLTextura* propsTextura){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::texImage2D")
	NBASSERT(propsTextura != NULL)
	NBASSERT(propsTextura->idTexturaGL == _cacheGL.unidadesTex[_cacheGL.iUnidadTexturaActivaServidor].idTexturaLigada)
	glTexImage2D(target, level, internalFormat, width, height, border, format, type, pixels);
	GL_CMD_EJECUTADO("glTexImage2D(target(%d), level(%d), internalFormat(%d), width(%d), height(%d), border(%d), format(%d), type(%d), pixels(%s))", (SI32)target, (SI32)level, (SI32)internalFormat, (SI32)width, (SI32)height, (SI32)border, (SI32)format, (SI32)type, (pixels == NULL ? "NULL" : "NOT-NULL"))
	propsTextura->anchoGL		= width;
	propsTextura->altoGL		= height;
	propsTextura->formatoGL		= format;
	propsTextura->mipMapSucio	= true;
#	ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT_GL_NOP
	_cacheGL.unidadesTex[_cacheGL.iUnidadTexturaActivaServidor].dbgCantOperacionesConTex++;
#	endif
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
}

void NBGestorGL::texSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels, STGestorGLTextura* propsTextura){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::texSubImage2D")
	NBASSERT(propsTextura != NULL)
	NBASSERT(propsTextura->idTexturaGL == _cacheGL.unidadesTex[_cacheGL.iUnidadTexturaActivaServidor].idTexturaLigada)
	NBASSERT(propsTextura->formatoGL == format)
	NBASSERT((xoffset + width) <= propsTextura->anchoGL)
	NBASSERT((yoffset + height) <= propsTextura->altoGL)
	glTexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, pixels);
	GL_CMD_EJECUTADO("glTexSubImage2D(target(%d), level(%d), xoffset(%d), yoffset(%d), width(%d), height(%d), format(%d), type(%d))", (SI32)target, (SI32)level, (SI32)xoffset, (SI32)yoffset, (SI32)width, (SI32)height, (SI32)format, (SI32)type);
	propsTextura->formatoGL		= format;
	propsTextura->mipMapSucio	= true;
#	ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT_GL_NOP
	_cacheGL.unidadesTex[_cacheGL.iUnidadTexturaActivaServidor].dbgCantOperacionesConTex++;
#	endif
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
}

/*
//Unused
void NBGestorGL::texEnvi(SI32 indiceUnidadTexturaGL, GLenum target, GLenum pname, GLint param){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::texEnvi")
	if(target == GL_TEXTURE_ENV && pname == GL_TEXTURE_ENV_MODE){
		if((GLint)_cacheGL.unidadesTex[_cacheGL.iUnidadTexturaActivaServidor].modoEntorno != param){
			NBGESTORGL_LOTES_TODOS_PURGAR("NBGestorGL::texEnvi(1)")
			//Activar unidad textura (si es necesario)
			NBGestorGL::activeTexture(indiceUnidadTexturaGL);
			//Aplicar parametro
			glTexEnvi(target, pname, param); GL_CMD_EJECUTADO("glTexEnvi")
			_cacheGL.unidadesTex[indiceUnidadTexturaGL].modoEntorno = param;
		}
	} else {
		NBGESTORGL_LOTES_TODOS_PURGAR("NBGestorGL::texEnvi(2)")
		//Activar unidad textura (si es necesario)
		NBGestorGL::activeTexture(indiceUnidadTexturaGL);
		glTexEnvi(target, pname, param); GL_CMD_EJECUTADO("glTexEnvi")
	}
#	ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT_GL_NOP
	_cacheGL.unidadesTex[_cacheGL.iUnidadTexturaActivaServidor].dbgCantOperacionesConTex++;
#	endif
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
}*/

STGLEstado NBGestorGL::cacheGL(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::cacheGL")
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
	return _cacheGL;
}

void NBGestorGL::lightModelf(GLenum param, GLfloat val){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::lightModelf")
	glLightModelf(param, val); GL_CMD_EJECUTADO("glLightModelf(param(%d), val(%f))", param, val)
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
}

void NBGestorGL::lightModelfv(GLenum param, GLfloat* vals){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::lightModelfv")
	NBASSERT(param != GL_LIGHT_MODEL_AMBIENT)
	if(param == GL_LIGHT_MODEL_AMBIENT){
		NBGestorGL::lightModelAmbient(vals);
	} else {
		glLightModelfv(param, vals); GL_CMD_EJECUTADO("glLightModelfv(param(%d) primerVal(%f))", param, vals[0])
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
}

void NBGestorGL::lightModelAmbient(const GLfloat* valsF){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::lightModelAmbient")
	if(_cacheGL.colorLuzAmbienteActivo.r != valsF[0] || _cacheGL.colorLuzAmbienteActivo.g != valsF[1] || _cacheGL.colorLuzAmbienteActivo.b != valsF[2] || _cacheGL.colorLuzAmbienteActivo.a != valsF[3]){
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, valsF); GL_CMD_EJECUTADO("glLightModelAmbient(%f, %f, %f, %f)", valsF[0], valsF[1], valsF[2], valsF[3])
		_cacheGL.colorLuzAmbienteActivo.r = valsF[0];
		_cacheGL.colorLuzAmbienteActivo.g = valsF[1];
		_cacheGL.colorLuzAmbienteActivo.b = valsF[2];
		_cacheGL.colorLuzAmbienteActivo.a = valsF[3];
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
}

void NBGestorGL::lightModelAmbient(const float r, const float g, const float b, const float a){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::lightModelAmbient")
	if(_cacheGL.colorLuzAmbienteActivo.r != r || _cacheGL.colorLuzAmbienteActivo.g != g || _cacheGL.colorLuzAmbienteActivo.b != b || _cacheGL.colorLuzAmbienteActivo.a != a){
		GLfloat valores[4] = {r, g, b, a};
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, valores); GL_CMD_EJECUTADO("glLightModelAmbient(%f, %f, %f, %f)",r, g, b, a)
		_cacheGL.colorLuzAmbienteActivo.r = r;
		_cacheGL.colorLuzAmbienteActivo.g = g;
		_cacheGL.colorLuzAmbienteActivo.b = b;
		_cacheGL.colorLuzAmbienteActivo.a = a;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
}

void NBGestorGL::lightModelAmbient(const NBColor &colorLuzAmbiente){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::lightModelAmbient")
	if(_cacheGL.colorLuzAmbienteActivo != colorLuzAmbiente){
		GLfloat valores[4] = {colorLuzAmbiente.r, colorLuzAmbiente.g, colorLuzAmbiente.b, colorLuzAmbiente.a};
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, valores); GL_CMD_EJECUTADO("glLightModelAmbient(%f, %f, %f, %f)", colorLuzAmbiente.r, colorLuzAmbiente.g, colorLuzAmbiente.b, colorLuzAmbiente.a)
		_cacheGL.colorLuzAmbienteActivo = colorLuzAmbiente;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
}

/*void NBGestorGL::colorMaterial(GLenum face, GLenum mode){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::colorMaterial")
	//glColorMaterial mut be called before enabling GL_COLOR_MATERIAL:
	//https://www.opengl.org/sdk/docs/man2/xhtml/glColorMaterial.xml
	glColorMaterial(face, mode); GL_CMD_EJECUTADO("glColorMaterial(%d, %d)", face, mode)
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
}*/

NBColor NBGestorGL::colorActivo(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::colorActivo")
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
	return _cacheGL.colorActivo;
}

void NBGestorGL::color4f(const NBColor &color){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::color4f")
	NBGESTORGL_LOTES_TODOS_ASSERT_ESTAN_VACIOS	//Si falla, se omitió flushear los comandos acumulados
	if(_cacheGL.colorActivo.r != color.r || _cacheGL.colorActivo.g != color.g || _cacheGL.colorActivo.b != color.b || _cacheGL.colorActivo.a != color.a){
		glColor4f(color.r, color.g, color.b, color.a);
		GL_CMD_EJECUTADO("glColor4f(%f, %f, %f, %f)", color.r, color.g, color.b, color.a)
		_cacheGL.colorActivo.r = color.r;
		_cacheGL.colorActivo.g = color.g;
		_cacheGL.colorActivo.b = color.b;
		_cacheGL.colorActivo.a = color.a;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
}

void NBGestorGL::color4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::color4f")
	NBGESTORGL_LOTES_TODOS_ASSERT_ESTAN_VACIOS	//Si falla, se omitió flushear los comandos acumulados
	if(_cacheGL.colorActivo.r != red || _cacheGL.colorActivo.g != green || _cacheGL.colorActivo.b != blue || _cacheGL.colorActivo.a != alpha){
		glColor4f(red, green, blue, alpha);
		GL_CMD_EJECUTADO("glColor4f(%f, %f, %f, %f)", red, green, blue, alpha)
		_cacheGL.colorActivo.r = red;
		_cacheGL.colorActivo.g = green;
		_cacheGL.colorActivo.b = blue;
		_cacheGL.colorActivo.a = alpha;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
}

//Frame buffers (EXTENSIONES)
void NBGestorGL::genFramebuffersEXT(GLsizei n, GLuint* frameBuffers){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::genFramebuffersEXT")
#	ifdef NB_LIB_GRAFICA_ES_EMBEDDED
	glGenFramebuffersOES(n, frameBuffers);
#	else
	glGenFramebuffersEXT(n, frameBuffers);
#	endif
	GL_CMD_EJECUTADO("glGenFramebuffersEXT(%d, primerRet(%u))", (SI32)n, (UI32)frameBuffers[0])
	//PRINTF_INFO("%d frameBuffers generados, primero(%d)\n", n, frameBuffers[0]);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
}

void NBGestorGL::deleteFramebuffersEXT(GLsizei n, GLuint* frameBuffers){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::deleteFramebuffersEXT")
#	ifdef NB_LIB_GRAFICA_ES_EMBEDDED
	glDeleteFramebuffersOES(n, frameBuffers);
#	else
	glDeleteFramebuffersEXT(n, frameBuffers);
#	endif
	GL_CMD_EJECUTADO("glDeleteFramebuffersEXT(%d, %u)", n, (UI32)frameBuffers[0])
	//PRINTF_INFO("%d frameBuffer(s) eliminado(s)\n", n);
	GLsizei i;
	for(i=0; i < n; i++){
		if((SI32)frameBuffers[i] == _cacheGL.idFrameBufferGlLigado){
			NBGestorGL::bindFramebufferInicialEXT(GL_FRAMEBUFFER_EXT);
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
}

void NBGestorGL::genRenderbuffersEXT(GLsizei n, GLuint* renderBuffers){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::genRenderbuffersEXT")
#	ifdef NB_LIB_GRAFICA_ES_EMBEDDED
	glGenRenderbuffersOES(n, renderBuffers);
#	else
	glGenRenderbuffersEXT(n, renderBuffers);
#	endif
	GL_CMD_EJECUTADO("glGenRenderbuffersEXT(%d, primerRet(%d))", n, renderBuffers[0])
	PRINTF_INFO("%d renderBuffers generados, primero(%d)\n", n, renderBuffers[0]);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
}

void NBGestorGL::deleteRenderbuffersEXT(GLsizei n, GLuint* renderBuffers){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::deleteRenderbuffersEXT")
#	ifdef NB_LIB_GRAFICA_ES_EMBEDDED
	glDeleteRenderbuffersOES(n, renderBuffers);
#	else
	glDeleteRenderbuffersEXT(n, renderBuffers);
#	endif
	GL_CMD_EJECUTADO("glDeleteFramebuffersEXT(%d, primero(%u))", n, (UI32)renderBuffers[0])
	//PRINTF_INFO("%d renderBuffer(s) eliminado(s)\n", n);
	//Vertificar si el renderbuffer actual fue eliminado
	GLsizei i; for(i=0; i < n; i++) if((SI32)renderBuffers[i] == _cacheGL.idRenderBufferGlLigado) _cacheGL.idRenderBufferGlLigado = -1;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
}

bool NBGestorGL::isRenderbufferEXT(GLuint renderbuffer){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::isRenderbufferEXT")
	bool r = false;
#	ifdef NB_LIB_GRAFICA_ES_EMBEDDED
	r = glIsRenderbufferOES(renderbuffer);
#	else
	r = glIsRenderbufferEXT(renderbuffer);
#	endif
	GL_CMD_EJECUTADO("glIsRenderbufferEXT")
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
	return r;
}

void NBGestorGL::bindRenderbufferEXT(GLenum target, GLuint renderBuffer){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::bindRenderbufferEXT")
	NBGESTORGL_LOTES_TODOS_ASSERT_ESTAN_VACIOS	//Si falla, se omitió flushear los comandos acumulados
	if(_cacheGL.idRenderBufferGlLigado != (SI32)renderBuffer){ //ESTO NO ESTA PERMITIENDO LIGAR EL RENDER-BUFFER EN EL HILO DE VOLCADO A PANTALLA
#		ifdef NB_LIB_GRAFICA_ES_EMBEDDED
		glBindRenderbufferOES(target, renderBuffer);
#		else
		glBindRenderbufferEXT(target, renderBuffer);
#		endif
		GL_CMD_EJECUTADO("glBindRenderbufferEXT(%d, %u)", (SI32)target, (UI32)renderBuffer)
		_cacheGL.idRenderBufferGlLigado = renderBuffer;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
}

void NBGestorGL::renderbufferStorageEXT(GLenum target, GLenum internalformat, GLsizei width, GLsizei height){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::renderbufferStorage")
	NBGESTORGL_LOTES_TODOS_ASSERT_ESTAN_VACIOS	//Si falla, se omitió flushear los comandos acumulados
#	ifdef NB_LIB_GRAFICA_ES_EMBEDDED
	glRenderbufferStorageOES(target, internalformat, width, height);
#	else
	glRenderbufferStorageEXT(target, internalformat, width, height);
#	endif
	GL_CMD_EJECUTADO("glRenderbufferStorageEXT")
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
}

void NBGestorGL::bindFramebufferEXT(GLenum target, GLuint frameBuffer){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::bindFramebufferEXT")
	NBGESTORGL_LOTES_TODOS_ASSERT_ESTAN_VACIOS	//Si falla, se omitió flushear los comandos acumulados
	//Target puede ser: GL_DRAW_FRAMEBUFFER, GL_READ_FRAMEBUFFER o GL_FRAMEBUFFER 
	// GL_FRAMEBUFFER equivale a GL_DRAW_FRAMEBUFFER y GL_READ_FRAMEBUFFER a la vez.
	if(_cacheGL.idFrameBufferGlLigado != (SI32)frameBuffer){
#		ifdef NB_LIB_GRAFICA_ES_EMBEDDED
		glBindFramebufferOES(target, frameBuffer);
#		else
		glBindFramebufferEXT(target, frameBuffer);
#		endif
		GL_CMD_EJECUTADO("glBindFramebufferEXT(%d, %u)", (SI32)target, (UI32)frameBuffer)
		_cacheGL.idFrameBufferGlLigado = frameBuffer;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
}

void NBGestorGL::bindFramebufferInicialEXT(GLenum target){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::bindFramebufferInicialEXT")
	//Target puede ser: GL_DRAW_FRAMEBUFFER, GL_READ_FRAMEBUFFER o GL_FRAMEBUFFER 
	// GL_FRAMEBUFFER equivale a GL_DRAW_FRAMEBUFFER y GL_READ_FRAMEBUFFER a la vez.
	NBGestorGL::bindFramebufferEXT(target, _idFramebufferAlInicializar);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
}

void NBGestorGL::framebufferTexture2DEXT(GLenum target, GLenum attachment, GLenum texTarget, GLuint texture, GLint level){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::framebufferTexture2DEXT")
	NBGESTORGL_LOTES_TODOS_ASSERT_ESTAN_VACIOS	//Si falla, se omitió flushear los comandos acumulados
#	ifdef NB_LIB_GRAFICA_ES_EMBEDDED
	glFramebufferTexture2DOES(target, attachment, texTarget, texture, level);
#	else
	glFramebufferTexture2DEXT(target, attachment, texTarget, texture, level);
#	endif
	GL_CMD_EJECUTADO("glFramebufferTexture2DEXT")
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
}

void NBGestorGL::framebufferRenderbufferEXT(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer){
    AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::framebufferRenderbufferEXT")
#    ifdef NB_LIB_GRAFICA_ES_EMBEDDED
    glFramebufferRenderbufferOES(target, attachment, renderbuffertarget, renderbuffer);
#    else
    glFramebufferRenderbufferEXT(target, attachment, renderbuffertarget, renderbuffer);
#    endif
    GL_CMD_EJECUTADO("glFramebufferRenderbufferEXT")
    AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
}

GLenum NBGestorGL::checkFramebufferStatusEXT(GLenum target){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::checkFramebufferStatusEXT")
#	ifdef NB_LIB_GRAFICA_ES_EMBEDDED
	GLenum r = glCheckFramebufferStatusOES(target);
#	else
	GLenum r = glCheckFramebufferStatusEXT(target);
#	endif
	GL_CMD_EJECUTADO("glCheckFramebufferStatusEXT")
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
	return r;
}

/*
//Unused
bool NBGestorGL::isFramebufferEXT(GLuint frameBuffer){
    AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::isFramebufferEXT")
    bool r = false;
#    ifdef NB_LIB_GRAFICA_ES_EMBEDDED
    r = glIsFramebufferOES(frameBuffer);
#    else
    r = glIsFramebufferEXT(frameBuffer);
#    endif
    GL_CMD_EJECUTADO("glIsFramebufferEXT")
    AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
    return r;
}*/

/*
//Unused
void NBGestorGL::blitFramebufferEXT(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::blitFramebufferEXT")
	NBGESTORGL_LOTES_TODOS_ASSERT_ESTAN_VACIOS	//Si falla, se omitió flushear los comandos acumulados
#	ifdef NB_LIB_GRAFICA_ES_EMBEDDED
	PRINTF_ERROR(" GL, la funcion 'blitFramebufferEXT' no esta disponible en esta compilacion\n");
	NBASSERT(false);
#	else
	glBlitFramebufferEXT(srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter);
#	endif
	GL_CMD_EJECUTADO("glBlitFramebufferEXT")
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
}*/

void NBGestorGL::prepararVerticesGLParaRenderizado(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::prepararVerticesGLParaRenderizado")
	NBASSERT(_gestorInicializado);
	GL_CMD_EJECUTADO("-----------------------------\nprepararVerticesGLParaRenderizado::verificacionInicial\n+++++++++++++++++++++++++++++")
	NBGESTORGL_LOTES_TODOS_ASSERT_ESTAN_VACIOS				//Si falla, se omitió flushear los comandos acumulados
	//Actualizar datos de VBOs
	bool arregloVerticeTieneDatos, arregloIndicesTieneDatos;
	SI32 iBufV; STVerticesDatos r;
	if(true || _soportaVAOs){
#		ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
		SI32 conteoVertexBConDatos = 0, conteoIndexBConDatos = 0;
		SI32 bytesVertexBConDatos = 0, bytesIndexBConDatos = 0;
#		endif
		for(iBufV = 0; iBufV < _vertsGlArrsTam; iBufV++){
			STArregloVerticesGL* ptrDat = &_vertsGlArrs[iBufV];
			if(ptrDat->registroOcupado && ptrDat->funcObtenerDatos != NULL){
				//Inicializar
				r.verticesDatos = NULL; r.verticesCantBytes = 0;
#				ifdef CONFIG_NB_GESTOR_ESCENAS_MODELOS_MEDIANTE_INDICES
				r.indicesDatos = NULL; r.indicesCantBytes = 0;
#				endif
				//Consultar
				(*ptrDat->funcObtenerDatos)(ptrDat->funcObtenerDatosParam, iBufV, &r);
				//Analizar
				arregloVerticeTieneDatos = (r.verticesDatos != NULL && r.verticesCantBytes > 1); // > 1 xq el primer vertice es reservado
#				ifdef CONFIG_NB_GESTOR_ESCENAS_MODELOS_MEDIANTE_INDICES
				arregloIndicesTieneDatos = (r.indicesDatos != NULL && r.indicesCantBytes > 2); // > 2 xq los primeros dos indices estan reservado (direccion de 4 bytes)
#				endif
				//
//#				ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT_GL_NOP
//				NBGestorGL::privDbgReiniciarCacheVerticesIndicesUsados((ENVerticeGlTipo)iBufV);
//#				endif
				//Establecer
				if(arregloVerticeTieneDatos || arregloIndicesTieneDatos){
					//Ligar el VAO, para que actualice sus punteros de VBOs
					if(_cacheGL.iVaoLigado != iBufV){
#						ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT_GL_NOP
						if(_cacheGL.dbgVAOCantOperaciones < 0) PRINTF_INFO("NBgestorGL_NOP, VAO fue establecido pero no utilizado: '%s'\n", _cacheGL.dbgVAONombreFuncion->str());
						NBASSERT(_cacheGL.dbgVAOCantOperaciones > 0) //Si falla, entonces se activo el ultimo VAO inecesariamente
						_cacheGL.dbgVAONombreFuncion->vaciar();
						_cacheGL.dbgVAONombreFuncion->agregarConFormato("NBGestorGL::prepararVerticesGLParaRenderizado");
						_cacheGL.dbgVAOCantOperaciones = 0;
#						endif
						NBGestorGL::privBindVertexArrayEXT((ENVerticeGlTipo)iBufV, false/*prepararParaConsumir*/);
					}
					//Ligar VBOs y actualizar datos
					if(arregloVerticeTieneDatos){
						glBufferData(GL_ARRAY_BUFFER, r.verticesCantBytes, r.verticesDatos, GL_DYNAMIC_DRAW); //GL_DYNAMIC_DRAW, GL_STREAM_DRAW (no disponible en ES1.1), GL_STATIC_DRAW
						GL_CMD_EJECUTADO("glBufferData(GL_ARRAY_BUFFER, %d bytes, ..., GL_DYNAMIC_DRAW)", r.verticesCantBytes)
#						ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT_GL_NOP
						_cacheGL.dbgVAOCantOperaciones++;
#						endif
#						ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
						conteoVertexBConDatos++;
						bytesVertexBConDatos += r.verticesCantBytes;
#						endif
					}
#					ifdef CONFIG_NB_GESTOR_ESCENAS_MODELOS_MEDIANTE_INDICES
					if(arregloIndicesTieneDatos){
						glBufferData(GL_ELEMENT_ARRAY_BUFFER, r.indicesCantBytes, r.indicesDatos, GL_DYNAMIC_DRAW); //GL_DYNAMIC_DRAW, GL_STREAM_DRAW (no disponible en ES1.1), GL_STATIC_DRAW
						GL_CMD_EJECUTADO("glBufferData(GL_ELEMENT_ARRAY_BUFFER, %d bytes, ..., GL_DYNAMIC_DRAW)", r.indicesCantBytes)
#						ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT_GL_NOP
						_cacheGL.dbgVAOCantOperaciones++;
#						endif
#						ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
						conteoIndexBConDatos++;
						bytesIndexBConDatos += r.indicesCantBytes;
#						endif
					}
#					endif
				}
			}
		}
#		ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
		//PRINTF_INFO("Buffers con datos, %d de vertices (%d KBs), %d de indices (%d KBs).\n", conteoVertexBConDatos, (bytesVertexBConDatos / 1024), conteoIndexBConDatos, (bytesIndexBConDatos / 1024));
#		endif
	} else {
		//PENDIENTE: eliminar
		for(iBufV = 0; iBufV < _vertsGlArrsTam; iBufV++){
			STArregloVerticesGL* ptrDat = &_vertsGlArrs[iBufV];
			if(ptrDat->registroOcupado && ptrDat->funcObtenerDatos != NULL){
				//Inicializar
				r.verticesDatos = NULL; r.verticesCantBytes = 0;
#				ifdef CONFIG_NB_GESTOR_ESCENAS_MODELOS_MEDIANTE_INDICES
				r.indicesDatos = NULL; r.indicesCantBytes = 0;
#				endif
				//Consultar
				(*ptrDat->funcObtenerDatos)(ptrDat->funcObtenerDatosParam, iBufV, &r);
				//Analizar
				arregloVerticeTieneDatos = (r.verticesDatos != NULL && r.verticesCantBytes > 0);
#				ifdef CONFIG_NB_GESTOR_ESCENAS_MODELOS_MEDIANTE_INDICES
				arregloIndicesTieneDatos = (r.indicesDatos != NULL && r.indicesCantBytes > 0);
#				endif
				//Establecer
				if(arregloVerticeTieneDatos || arregloIndicesTieneDatos){
					_cacheGL.iVaoLigado = iBufV;
					//Ligar VBOs y actualizar datos
					if(arregloVerticeTieneDatos){
						if(_cacheGL.idBufferArrayLigado != _vertsGlArrs[iBufV].idBufferGlVertices){
							glBindBuffer(GL_ARRAY_BUFFER, _vertsGlArrs[iBufV].idBufferGlVertices); GL_CMD_EJECUTADO("glBindBuffer(GL_ARRAY_BUFFER, %u)", (UI32)_vertsGlArrs[iBufV].idBufferGlVertices) //PRINTF_INFO("LigandoPrepara GL_ARRAY_BUFFER(%d)\n", (SI32)_vertsGlArrs[iBufV].idBufferGlVertices);
							_cacheGL.idBufferArrayLigado = _vertsGlArrs[iBufV].idBufferGlVertices;
						}
						glBufferData(GL_ARRAY_BUFFER, r.verticesCantBytes, r.verticesDatos, GL_DYNAMIC_DRAW); //GL_DYNAMIC_DRAW, GL_STREAM_DRAW (no disponible en ES1.1), GL_STATIC_DRAW
						GL_CMD_EJECUTADO("glBufferData(GL_ARRAY_BUFFER, %d bytes, ..., GL_DYNAMIC_DRAW)", r.verticesCantBytes)
					}
#					ifdef CONFIG_NB_GESTOR_ESCENAS_MODELOS_MEDIANTE_INDICES
					if(arregloIndicesTieneDatos){
						if(_cacheGL.idBufferElemsLigado != _vertsGlArrs[iBufV].idBufferGlIndices){
							glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vertsGlArrs[iBufV].idBufferGlIndices); GL_CMD_EJECUTADO("glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, %u)", (UI32)_vertsGlArrs[iBufV].idBufferGlIndices) //PRINTF_INFO("LigandoPrepara GL_ELEMENT_ARRAY_BUFFER(%d)\n", (SI32)_vertsGlArrs[iBufV].idBufferGlIndices);
							_cacheGL.idBufferElemsLigado = _vertsGlArrs[iBufV].idBufferGlIndices;
						}
						glBufferData(GL_ELEMENT_ARRAY_BUFFER, r.indicesCantBytes, r.indicesDatos, GL_DYNAMIC_DRAW); //GL_DYNAMIC_DRAW, GL_STREAM_DRAW (no disponible en ES1.1), GL_STATIC_DRAW
						GL_CMD_EJECUTADO("glBufferData(GL_ELEMENT_ARRAY_BUFFER, %d bytes, ..., GL_DYNAMIC_DRAW)", r.indicesCantBytes)
					}
#					endif
				}
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
}

void NBGestorGL::activarVerticesGL(const SI32 iVao){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::activarVerticesGL")
	NBASSERT(_gestorInicializado);
	NBASSERT(iVao >= 0 && iVao < _vertsGlArrsTam);
	NBASSERT(_vertsGlArrs[iVao].registroOcupado)
	//NBASSERT(_vertsGlArrs[iVao].datosGL[_indiceBufferDatosLeer].usoArregloVertices > 0); //Si falla, se pretende activar un buffer vacio (el primer elemento es reservado)
	//------------------
	//VALIDACION-NOP (aunque el VAO sea el mismo, se debe validar que el comando "activarVerticesGL" anterior realizo acciones)
	//------------------
#	ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT_GL_NOP
		if(_cacheGL.dbgVAOCantOperaciones < 0) PRINTF_INFO("NBgestorGL_NOP, VAO fue establecido pero no utilizado: '%s'\n", _cacheGL.dbgVAONombreFuncion->str());
		NBASSERT(_cacheGL.dbgVAOCantOperaciones > 0) //Si falla, entonces se activo el ultimo VAO inecesariamente
		_cacheGL.dbgVAONombreFuncion->vaciar();
		_cacheGL.dbgVAONombreFuncion->agregarConFormato("NBGestorGL::activarVerticesGL(%d)", (SI32)tipoVerticesGl);
		_cacheGL.dbgVAOCantOperaciones = 0;
#	endif
	//------------------
	//RETORNO TEMPRANO
	//------------------
	if(_cacheGL.iVaoLigado == iVao){
		NBGL_ASSERT_CACHE_VAO
		NBGL_ASSERT_CACHE_CLIENT_STATE
		//Retornar
		AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO_GL();
	}
	//------------------
	//CAMBIO DE VAO
	//------------------
	NBASSERT(_cacheGL.iVaoLigado != iVao)
	NBGESTORGL_LOTES_TODOS_PURGAR("NBGestorGL::activarVerticesGL")
#	ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT_GL_NOP
	if(tipoVerticesGl < ENVerticeGlTipo_Conteo){
		NBGestorGL::privDbgReiniciarCacheVerticesIndicesUsados(tipoVerticesGl);
	}
#	endif
	GL_CMD_EJECUTADO("--activarVerticesGL-start(%d)", iVao)
	//Ligar VAO (automaticamente configura los punteros y buffers)
	NBGestorGL::privBindVertexArrayEXT(iVao, true/*prepararParaConsumir*/);
	//
	//Habilitar/deshabilitar unidades de texturas
	//
	STArregloVerticesGL* ptrDat = &_vertsGlArrs[iVao];
	SI32 iTexU; //PRINTF_INFO("Activando vertices(%d).\n", tipoVerticesGl);
	for(iTexU = _maxUnidadesTexUsar - 1; iTexU >= 0; iTexU--){
		const bool habilitarTextura = (iTexU < ptrDat->tipoVertices); //PRINTF_INFO("Tex(%d) habilitada: %s.\n", iTexU, habilitarTextura ? "si" : "no");
#		ifdef CONFIG_NB_GESTOR_GL_VERIFICAR_CACHE_SIEMPRE
		NBGestorGL::activeTexture(iTexU);
		NBGL_ASSERT_CACHE_TEXTURE_ACTIVE_STATE
#		endif
		if(_cacheGL.unidadesTex[iTexU].texture2DHabilitada != habilitarTextura){
			NBGestorGL::activeTexture(iTexU);
			if(habilitarTextura){
				NBGestorGL::enable(GL_TEXTURE_2D);
			} else {
				NBGestorGL::disable(GL_TEXTURE_2D);
			}
			NBASSERT(_cacheGL.unidadesTex[iTexU].texture2DHabilitada == habilitarTextura)
		}
	}
	if(_cacheGL.iUnidadTexturaActivaServidor != 0){
		NBGestorGL::activeTexture(0);
	}
	GL_CMD_EJECUTADO("--activarVerticesGL-end(%d)", iVao)
	//
	_cacheGL.iVaoLigado = iVao;
	//
	NBGL_ASSERT_CACHE_VAO
	NBGL_ASSERT_CACHE_CLIENT_STATE
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
}

#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT_GL_NOP
void NBGestorGL::dbgNombrarActivadorVerticesGL(const char* strNombre){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::dbgNombrarActivadorVerticesGL")
	//NBASSERT(_cacheGL.dbgVAOCantOperaciones == 0) //Se debe nombrar inmediatamente despues de "activarVerticesGL"
	_cacheGL.dbgVAONombreFuncion->establecer(strNombre);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
}
#endif

//Errores
GLenum NBGestorGL::getError(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::getError")
	GLenum r = glGetError();
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
	return r;
}

//

SI32 NBGestorGL::maximaDimensionTextura(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::maximaDimensionTextura")
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
	return _maxDimensionTextura;
}

void NBGestorGL::drawTex(GLfloat x, GLfloat y, GLfloat z, GLfloat w, GLfloat h){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::drawTex")
	NBASSERT(_gestorInicializado);
	//
	NBASSERT(w >= -0.001f) //Must be positive (flipping image should be done in the GL_TEXTURE_CROP_RECT_OES)
	NBASSERT(h >= -0.001f) //Must be positive (flipping image should be done in the GL_TEXTURE_CROP_RECT_OES)
	//
	if(w > 0 && h > 0){
		NBGESTORGL_LOTES_TODOS_PURGAR("NBGestorGL::drawTex");
		//
#		ifdef GL_TEXTURE_CROP_RECT_OES
		glDrawTexfOES(x, y, z, w, h); GL_CMD_EJECUTADO("drawTex-x(%f)-y(%f)-z(%f)-w(%f)-h(%f)", x, y, z, w, h)
#		endif
		//
#		ifdef CONFIG_NB_GESTOR_GL_IMPRIMIR_COMANDOS_GL
		/*{
		 GLint i;
		 switch (_cacheGL.vaoTipoLigado) {
		 case ENVerticeGlTipo_SinTextura:
		 NBASSERT(false);
		 break;
		 case ENVerticeGlTipo_MonoTextura:
		 {
		 const NBVerticeGL* verticeAnt2 = NULL;
		 const NBVerticeGL* verticeAnt = NULL;
		 const NBVerticeGL* vertice = (NBVerticeGL*)&(_vertsGlArrs[_cacheGL.vaoTipoLigado].datosGL[_indiceBufferDatosLeer].arregloVertices[first * _vertsGlArrs[_cacheGL.vaoTipoLigado].datosGL[_indiceBufferDatosLeer].bytesPorRegistro]);
		 for(i=0; i < count; i++){
		 if(verticeAnt2 != NULL && verticeAnt != NULL){
		 NBPunto vPosBase; NBPUNTO_ESTABLECER(vPosBase, verticeAnt->x - verticeAnt2->x, verticeAnt->y - verticeAnt2->y);
		 NBPunto vPosNevo; NBPUNTO_ESTABLECER(vPosNevo, vertice->x - verticeAnt2->x, vertice->y - verticeAnt2->y);
		 NBPunto vTexBase; NBPUNTO_ESTABLECER(vTexBase, verticeAnt->tex.x - verticeAnt2->tex.x, verticeAnt->tex.y - verticeAnt2->tex.y);
		 NBPunto vTexNevo; NBPUNTO_ESTABLECER(vTexNevo, vertice->tex.x - verticeAnt2->tex.x, vertice->tex.y - verticeAnt2->tex.y);
		 PRINTF_COMANDO_GL("   v%d pos(%f, %f)[%s] col(%d, %d, %d, %d) tex(%f, %f)[%s]", (i+1), (float)vertice->x, (float)vertice->y, NBPUNTO_ESTA_IZQUIERDA_VECTOR_V(vPosBase.x, vPosBase.y, vPosNevo.x, vPosNevo.y) ? "izq" : "der", (SI32)vertice->r, (SI32)vertice->g, (SI32)vertice->b, (SI32)vertice->a, (float)vertice->tex.x, (float)vertice->tex.y, NBPUNTO_ESTA_IZQUIERDA_VECTOR_V(vTexBase.x, vTexBase.y, vTexNevo.x, vTexNevo.y) ? "izq" : "der");
		 } else {
		 PRINTF_COMANDO_GL("   v%d pos(%f, %f) col(%d, %d, %d, %d) tex(%f, %f)", (i+1), (float)vertice->x, (float)vertice->y, (SI32)vertice->r, (SI32)vertice->g, (SI32)vertice->b, (SI32)vertice->a, (float)vertice->tex.x, (float)vertice->tex.y);
		 }
		 verticeAnt2 = verticeAnt; verticeAnt = vertice; vertice++;
		 }
		 }
		 break;
		 case ENVerticeGlTipo_BiTextura:
		 {
		 const NBVerticeGL2* verticeAnt2 = NULL;
		 const NBVerticeGL2* verticeAnt = NULL;
		 const NBVerticeGL2* vertice = (NBVerticeGL2*)&(_vertsGlArrs[_cacheGL.vaoTipoLigado].datosGL[_indiceBufferDatosLeer].arregloVertices[first * _vertsGlArrs[_cacheGL.vaoTipoLigado].datosGL[_indiceBufferDatosLeer].bytesPorRegistro]);
		 for(i=0; i < count; i++){
		 PRINTF_COMANDO_GL("   v%d pos(%f, %f) col(%d, %d, %d, %d) tex1(%f, %f) tex2(%f, %f)", (i+1), (float)vertice->x, (float)vertice->y, (SI32)vertice->r, (SI32)vertice->g, (SI32)vertice->b, (SI32)vertice->a, (float)vertice->tex.x, (float)vertice->tex.y, (float)vertice->tex2.x, (float)vertice->tex2.y);
		 verticeAnt2 = verticeAnt;  verticeAnt = vertice; vertice++;
		 }
		 }
		 break;
		 case ENVerticeGlTipo_TriTextura:
		 {
		 const NBVerticeGL3* verticeAnt2 = NULL;
		 const NBVerticeGL3* verticeAnt = NULL;
		 const NBVerticeGL3* vertice = (NBVerticeGL3*)&(_vertsGlArrs[_cacheGL.vaoTipoLigado].datosGL[_indiceBufferDatosLeer].arregloVertices[first * _vertsGlArrs[_cacheGL.vaoTipoLigado].datosGL[_indiceBufferDatosLeer].bytesPorRegistro]);
		 for(i=0; i < count; i++){
		 PRINTF_COMANDO_GL("   v%d pos(%f, %f) col(%d, %d, %d, %d) tex1(%f, %f) tex2(%f, %f) tex3(%f, %f)", (i+1), (float)vertice->x, (float)vertice->y, (SI32)vertice->r, (SI32)vertice->g, (SI32)vertice->b, (SI32)vertice->a, (float)vertice->tex.x, (float)vertice->tex.y, (float)vertice->tex2.x, (float)vertice->tex2.y, (float)vertice->tex3.x, (float)vertice->tex3.y);
		 verticeAnt2 = verticeAnt;  verticeAnt = vertice; vertice++;
		 }
		 }
		 break;
		 default:
		 NBASSERT(false)
		 break;
		 }
		 }*/
#		endif
#		ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT_GL_NOP
		//NBGestorGL::privDbgAcumularOperacionRealizada();
		//Analizar si puede optimizar el comando (uniendo con su anterior)
		/*if(_cacheGL.iVaoLigado < _vertsGlArrsTam){
		 SI32 iBufV					= _cacheGL.vaoTipoLigado;
		 bool esFiguraIndependiente	= false; if(mode == GL_TRIANGLE_STRIP && count > 2) esFiguraIndependiente = (_vertsGlArrs[iBufV].datosGL[_indiceBufferDatosLeer].arregloVertices[first] == _vertsGlArrs[iBufV].datosGL[_indiceBufferDatosLeer].arregloVertices[first+1] && _vertsGlArrs[iBufV].datosGL[_indiceBufferDatosLeer].arregloVertices[first+count-2] == _vertsGlArrs[iBufV].datosGL[_indiceBufferDatosLeer].arregloVertices[first+count-1]);
		 bool esFiguraIndependizable	= NBGestorGL::dbgModoFormaGLEsIndependizable(mode);
		 if(mode == _cacheGL.dbgVerticesUltimoUsadoModo[iBufV]){
		 if((esFiguraIndependiente || esFiguraIndependizable) && (_cacheGL.dbgVerticesUltimaFormaEsIndependiente[iBufV] || _cacheGL.dbgVerticesUltimaFormaEsIndependizable[iBufV])){
		 if(_cacheGL.dbgVerticesUltimaFormaEnIndices[iBufV]){
		 PRINTF_INFO("El comando drawArrays(%s, %d, %d) puede anexarse a su anterior (USAR_INDICES_EN_ACTUAL). ANT(%s) ACT(%s) ANT[%s]\n", NBGestorGL::dbgModoFormaGLNombre(mode), (SI32)first, (SI32)count, _cacheGL.dbgVerticesUltimaFormaEsIndependiente[iBufV]?"INDEPENDIENTE":"INDEPENDIZABLE" , esFiguraIndependiente?"INDEPENDIENTE":"INDEPENDIZABLE", _cacheGL.dbgDrawElementsConvocador->str());
		 } else if(first == (_cacheGL.dbgVerticesUltimoUsadoIndice[iBufV]+1)){
		 PRINTF_INFO("El comando drawArrays(%s, %d, %d) puede anexarse a su anterior (VERTICES_EN_SECUENCIA_CONTIGUA). ANT(%s) ACT(%s) ANT[%s]\n", NBGestorGL::dbgModoFormaGLNombre(mode), (SI32)first, (SI32)count, _cacheGL.dbgVerticesUltimaFormaEsIndependiente[iBufV]?"INDEPENDIENTE":"INDEPENDIZABLE" , esFiguraIndependiente?"INDEPENDIENTE":"INDEPENDIZABLE", _cacheGL.dbgDrawArrayConvocador->str());
		 } else {
		 PRINTF_INFO("El comando drawArrays(%s, %d, %d) puede anexarse a su anterior (USAR_INDICES_EN_ANTERIOR_Y_ACTUAL). ANT(%s) ACT(%s) ANT[%s]\n", NBGestorGL::dbgModoFormaGLNombre(mode), (SI32)first, (SI32)count, _cacheGL.dbgVerticesUltimaFormaEsIndependiente[iBufV]?"INDEPENDIENTE":"INDEPENDIZABLE" , esFiguraIndependiente?"INDEPENDIENTE":"INDEPENDIZABLE", _cacheGL.dbgDrawArrayConvocador->str());
		 }
		 //NBASSERT(false)
		 }
		 }
		 _cacheGL.dbgVerticesUltimaFormaEnIndices[iBufV]			= false;
		 //
		 _cacheGL.dbgVerticesUltimaFormaEsIndependiente[iBufV]	= esFiguraIndependiente;
		 _cacheGL.dbgVerticesUltimaFormaEsIndependizable[iBufV]	= esFiguraIndependizable;
		 _cacheGL.dbgVerticesUltimoUsadoModo[iBufV]				= mode;
		 _cacheGL.dbgVerticesUltimoUsadoIndice[iBufV]			= (first + count - 1);
		 }*/
#		endif
		NBGESTORGL_DBG_NOMBRAR_CONVOCADOR_DRAW_ARRAY("[SIN_NOMBRE]")
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
}

void NBGestorGL::drawArrays(GLenum mode, GLint first, GLsizei count){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::drawArrays")
	NBASSERT(_gestorInicializado);
	NBASSERT(first > 0) //El primer elemento esta reservado
	NBASSERT(count > 0) //No se aceptan comandos vacios, optimizar codigo
#	ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
	NBASSERT(_cacheGL.iVaoLigado >= 0 && _cacheGL.iVaoLigado < _vertsGlArrsTam)
	//if(_cacheGL.iVaoLigado >= 0 && _cacheGL.iVaoLigado < _vertsGlArrsTam){
		//const UI32 iSigUlt	= (first + count);
		//const UI32 tamArr	= _vertsGlArrs[_cacheGL.iVaoLigado].datosGL[_indiceBufferDatosLeer].usoArregloVertices;
		//NBASSERT(((first * _vertsGlArrs[_cacheGL.iVaoLigado].datosGL[_indiceBufferDatosLeer].bytesPorRegistro) % 4) == 0) //La direccion debe ser multiplo de 4 bytes (direcciones de 32 bits)
		//NBASSERT(iSigUlt <= tamArr);
	//}
#	endif
	//
	NBGESTORGL_LOTES_TODOS_PURGAR("NBGestorGL::drawArrays");
	glDrawArrays(mode, first, count); GL_CMD_EJECUTADO("glDrawArrays(%s, first(%d), count(%d))", STR_GL_DRAW_MODE(mode), (SI32)first, (SI32)count)
#	ifdef CONFIG_NB_GESTOR_GL_IMPRIMIR_COMANDOS_GL
		/*{
			GLint i;
			switch (_cacheGL.vaoTipoLigado) {
				case ENVerticeGlTipo_SinTextura:
					NBASSERT(false);
					break;
				case ENVerticeGlTipo_MonoTextura:
					{
						const NBVerticeGL* verticeAnt2 = NULL;
						const NBVerticeGL* verticeAnt = NULL;
						const NBVerticeGL* vertice = (NBVerticeGL*)&(_vertsGlArrs[_cacheGL.vaoTipoLigado].datosGL[_indiceBufferDatosLeer].arregloVertices[first * _vertsGlArrs[_cacheGL.vaoTipoLigado].datosGL[_indiceBufferDatosLeer].bytesPorRegistro]);
						for(i=0; i < count; i++){
							if(verticeAnt2 != NULL && verticeAnt != NULL){
								NBPunto vPosBase; NBPUNTO_ESTABLECER(vPosBase, verticeAnt->x - verticeAnt2->x, verticeAnt->y - verticeAnt2->y);
								NBPunto vPosNevo; NBPUNTO_ESTABLECER(vPosNevo, vertice->x - verticeAnt2->x, vertice->y - verticeAnt2->y);
								NBPunto vTexBase; NBPUNTO_ESTABLECER(vTexBase, verticeAnt->tex.x - verticeAnt2->tex.x, verticeAnt->tex.y - verticeAnt2->tex.y);
								NBPunto vTexNevo; NBPUNTO_ESTABLECER(vTexNevo, vertice->tex.x - verticeAnt2->tex.x, vertice->tex.y - verticeAnt2->tex.y);
								PRINTF_COMANDO_GL("   v%d pos(%f, %f)[%s] col(%d, %d, %d, %d) tex(%f, %f)[%s]", (i+1), (float)vertice->x, (float)vertice->y, NBPUNTO_ESTA_IZQUIERDA_VECTOR_V(vPosBase.x, vPosBase.y, vPosNevo.x, vPosNevo.y) ? "izq" : "der", (SI32)vertice->r, (SI32)vertice->g, (SI32)vertice->b, (SI32)vertice->a, (float)vertice->tex.x, (float)vertice->tex.y, NBPUNTO_ESTA_IZQUIERDA_VECTOR_V(vTexBase.x, vTexBase.y, vTexNevo.x, vTexNevo.y) ? "izq" : "der");
							} else {
								PRINTF_COMANDO_GL("   v%d pos(%f, %f) col(%d, %d, %d, %d) tex(%f, %f)", (i+1), (float)vertice->x, (float)vertice->y, (SI32)vertice->r, (SI32)vertice->g, (SI32)vertice->b, (SI32)vertice->a, (float)vertice->tex.x, (float)vertice->tex.y);
							}
							verticeAnt2 = verticeAnt; verticeAnt = vertice; vertice++;
						}
					}
					break;
				case ENVerticeGlTipo_BiTextura:
					{
						const NBVerticeGL2* verticeAnt2 = NULL;
						const NBVerticeGL2* verticeAnt = NULL;
						const NBVerticeGL2* vertice = (NBVerticeGL2*)&(_vertsGlArrs[_cacheGL.vaoTipoLigado].datosGL[_indiceBufferDatosLeer].arregloVertices[first * _vertsGlArrs[_cacheGL.vaoTipoLigado].datosGL[_indiceBufferDatosLeer].bytesPorRegistro]);
						for(i=0; i < count; i++){
							PRINTF_COMANDO_GL("   v%d pos(%f, %f) col(%d, %d, %d, %d) tex1(%f, %f) tex2(%f, %f)", (i+1), (float)vertice->x, (float)vertice->y, (SI32)vertice->r, (SI32)vertice->g, (SI32)vertice->b, (SI32)vertice->a, (float)vertice->tex.x, (float)vertice->tex.y, (float)vertice->tex2.x, (float)vertice->tex2.y);
							verticeAnt2 = verticeAnt;  verticeAnt = vertice; vertice++;
						}
					}
					break;
				case ENVerticeGlTipo_TriTextura:
					{
						const NBVerticeGL3* verticeAnt2 = NULL;
						const NBVerticeGL3* verticeAnt = NULL;
						const NBVerticeGL3* vertice = (NBVerticeGL3*)&(_vertsGlArrs[_cacheGL.vaoTipoLigado].datosGL[_indiceBufferDatosLeer].arregloVertices[first * _vertsGlArrs[_cacheGL.vaoTipoLigado].datosGL[_indiceBufferDatosLeer].bytesPorRegistro]);
						for(i=0; i < count; i++){
							PRINTF_COMANDO_GL("   v%d pos(%f, %f) col(%d, %d, %d, %d) tex1(%f, %f) tex2(%f, %f) tex3(%f, %f)", (i+1), (float)vertice->x, (float)vertice->y, (SI32)vertice->r, (SI32)vertice->g, (SI32)vertice->b, (SI32)vertice->a, (float)vertice->tex.x, (float)vertice->tex.y, (float)vertice->tex2.x, (float)vertice->tex2.y, (float)vertice->tex3.x, (float)vertice->tex3.y);
							verticeAnt2 = verticeAnt;  verticeAnt = vertice; vertice++;
						}
					}
					break;
				default:
					NBASSERT(false)
					break;
			}
		}*/
#	endif
#	ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT_GL_NOP
	NBGestorGL::privDbgAcumularOperacionRealizada();
	//Analizar si puede optimizar el comando (uniendo con su anterior)
	/*if(_cacheGL.iVaoLigado < _vertsGlArrsTam){
		SI32 iBufV					= _cacheGL.vaoTipoLigado;
		bool esFiguraIndependiente	= false; if(mode == GL_TRIANGLE_STRIP && count > 2) esFiguraIndependiente = (_vertsGlArrs[iBufV].datosGL[_indiceBufferDatosLeer].arregloVertices[first] == _vertsGlArrs[iBufV].datosGL[_indiceBufferDatosLeer].arregloVertices[first+1] && _vertsGlArrs[iBufV].datosGL[_indiceBufferDatosLeer].arregloVertices[first+count-2] == _vertsGlArrs[iBufV].datosGL[_indiceBufferDatosLeer].arregloVertices[first+count-1]);
		bool esFiguraIndependizable	= NBGestorGL::dbgModoFormaGLEsIndependizable(mode);
		if(mode == _cacheGL.dbgVerticesUltimoUsadoModo[iBufV]){
			if((esFiguraIndependiente || esFiguraIndependizable) && (_cacheGL.dbgVerticesUltimaFormaEsIndependiente[iBufV] || _cacheGL.dbgVerticesUltimaFormaEsIndependizable[iBufV])){
				if(_cacheGL.dbgVerticesUltimaFormaEnIndices[iBufV]){
					PRINTF_INFO("El comando drawArrays(%s, %d, %d) puede anexarse a su anterior (USAR_INDICES_EN_ACTUAL). ANT(%s) ACT(%s) ANT[%s]\n", NBGestorGL::dbgModoFormaGLNombre(mode), (SI32)first, (SI32)count, _cacheGL.dbgVerticesUltimaFormaEsIndependiente[iBufV]?"INDEPENDIENTE":"INDEPENDIZABLE" , esFiguraIndependiente?"INDEPENDIENTE":"INDEPENDIZABLE", _cacheGL.dbgDrawElementsConvocador->str());
				} else if(first == (_cacheGL.dbgVerticesUltimoUsadoIndice[iBufV]+1)){
					PRINTF_INFO("El comando drawArrays(%s, %d, %d) puede anexarse a su anterior (VERTICES_EN_SECUENCIA_CONTIGUA). ANT(%s) ACT(%s) ANT[%s]\n", NBGestorGL::dbgModoFormaGLNombre(mode), (SI32)first, (SI32)count, _cacheGL.dbgVerticesUltimaFormaEsIndependiente[iBufV]?"INDEPENDIENTE":"INDEPENDIZABLE" , esFiguraIndependiente?"INDEPENDIENTE":"INDEPENDIZABLE", _cacheGL.dbgDrawArrayConvocador->str());
				} else {
					PRINTF_INFO("El comando drawArrays(%s, %d, %d) puede anexarse a su anterior (USAR_INDICES_EN_ANTERIOR_Y_ACTUAL). ANT(%s) ACT(%s) ANT[%s]\n", NBGestorGL::dbgModoFormaGLNombre(mode), (SI32)first, (SI32)count, _cacheGL.dbgVerticesUltimaFormaEsIndependiente[iBufV]?"INDEPENDIENTE":"INDEPENDIZABLE" , esFiguraIndependiente?"INDEPENDIENTE":"INDEPENDIZABLE", _cacheGL.dbgDrawArrayConvocador->str());
				}
				//NBASSERT(false)
			}
		}
		_cacheGL.dbgVerticesUltimaFormaEnIndices[iBufV]			= false;
		//
		_cacheGL.dbgVerticesUltimaFormaEsIndependiente[iBufV]	= esFiguraIndependiente;
		_cacheGL.dbgVerticesUltimaFormaEsIndependizable[iBufV]	= esFiguraIndependizable;
		_cacheGL.dbgVerticesUltimoUsadoModo[iBufV]				= mode;
		_cacheGL.dbgVerticesUltimoUsadoIndice[iBufV]			= (first + count - 1);
	}*/
#	endif
	NBGESTORGL_DBG_NOMBRAR_CONVOCADOR_DRAW_ARRAY("[SIN_NOMBRE]")
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
}

#ifdef CONFIG_NB_GESTOR_ESCENAS_MODELOS_MEDIANTE_INDICES
void NBGestorGL::drawElements(GLenum mode, GLint first, GLsizei count){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::drawElements")
	NBASSERT(_gestorInicializado);
	NBASSERT(first > 1) //Los dos primeros elementos estan reservados
	NBASSERT(count > 0) //No se aceptan comandos vacios, optimizar codigo
	NBASSERT(((first * sizeof(GLushort)) % 4) == 0) //La direccion debe ser multiplo de 4 bytes (direcciones de 32 bits)
#	ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
	NBASSERT(_cacheGL.iVaoLigado >= 0 && _cacheGL.iVaoLigado < _vertsGlArrsTam)
	//if(_cacheGL.iVaoLigado >= 0 && _cacheGL.iVaoLigado < _vertsGlArrsTam){
	//	const UI32 iSigUlt	= (first + count);
	//	const UI32 usoInd	= _vertsGlArrs[_cacheGL.iVaoLigado].datosGL[_indiceBufferDatosLeer].usoArregloIndices;
	//	NBASSERT(iSigUlt  <= usoInd);
	//}
#	endif
	NBGESTORGL_LOTES_TODOS_PURGAR("NBGestorGL::drawElements")
	glDrawElements(mode, count, GL_UNSIGNED_SHORT, (GLvoid*)(first * sizeof(GLushort))); GL_CMD_EJECUTADO("glDrawElements(%s, count(%d), GL_UNSIGNED_SHORT, first(%d))", STR_GL_DRAW_MODE(mode), (SI32)count, (SI32)first)
#	ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT_GL_NOP
	NBGestorGL::privDbgAcumularOperacionRealizada();
	//Analizar si puede optimizar el comando (uniendo con su anterior)
	/*if(_cacheGL.vaoTipoLigado < _vertsGlArrsTam){
		SI32 iBufV					= _cacheGL.vaoTipoLigado;
		SI32 iUltVerticeUsaCmd		= _vertsGlArrs[iBufV].datosGL[_indiceBufferDatosLeer].arregloIndices[first + count - 1];
		bool esFiguraIndependiente	= false; if(mode == GL_TRIANGLE_STRIP && count > 2) esFiguraIndependiente = (_vertsGlArrs[iBufV].datosGL[_indiceBufferDatosLeer].arregloIndices[first] == _vertsGlArrs[iBufV].datosGL[_indiceBufferDatosLeer].arregloIndices[first+1] && _vertsGlArrs[iBufV].datosGL[_indiceBufferDatosLeer].arregloIndices[first+count-2] == _vertsGlArrs[iBufV].datosGL[_indiceBufferDatosLeer].arregloIndices[first+count-1]);
		bool esFiguraIndependizable	= NBGestorGL::dbgModoFormaGLEsIndependizable(mode);
		if(GL_TRIANGLE_STRIP == _cacheGL.dbgIndicesUltimoUsadoModo[iBufV] && (esFiguraIndependiente || esFiguraIndependizable)){
			if(_cacheGL.dbgVerticesUltimaFormaEnIndices[iBufV]){
				if(first == (_cacheGL.dbgIndicesUltimoUsadoIndice[iBufV]+1) && (_cacheGL.dbgIndicesUltimaFormaEsIndependiente[iBufV] || _cacheGL.dbgIndicesUltimaFormaEsIndependizable[iBufV])){
					PRINTF_INFO("El comando PurgarLoteTriangStrips(%d, %d) puede anexarse a su anterior (INDICES_EN_SECUENCIA_CONTIGUA). ANT(%s) ACT(%s) ANT[%s]\n", (SI32)first, (SI32)count, _cacheGL.dbgVerticesUltimaFormaEsIndependiente[iBufV]?"INDEPENDIENTE":"INDEPENDIZABLE", esFiguraIndependiente?"INDEPENDIENTE":"INDEPENDIZABLE", _cacheGL.dbgDrawElementsConvocador->str());
					//NBASSERT(false)
				}
			} else {
				if(_cacheGL.dbgVerticesUltimaFormaEsIndependiente[iBufV] || _cacheGL.dbgVerticesUltimaFormaEsIndependizable[iBufV]){
					PRINTF_INFO("El comando PurgarLoteTriangStrips(%d, %d) puede anexarse a su anterior (USAR_INDICES_EN_ANTERIOR). ANT(%s) ACT(%s) ANT[%s]\n", (SI32)first, (SI32)count, _cacheGL.dbgVerticesUltimaFormaEsIndependiente[iBufV]?"INDEPENDIENTE":"INDEPENDIZABLE", esFiguraIndependiente?"INDEPENDIENTE":"INDEPENDIZABLE", _cacheGL.dbgDrawArrayConvocador->str());
					//NBASSERT(false)
				}
			}
		}
		_cacheGL.dbgVerticesUltimaFormaEnIndices[iBufV]			= true;
		//
		_cacheGL.dbgVerticesUltimaFormaEsIndependiente[iBufV]	= esFiguraIndependiente;
		_cacheGL.dbgVerticesUltimaFormaEsIndependizable[iBufV]	= esFiguraIndependizable;
		_cacheGL.dbgVerticesUltimoUsadoModo[iBufV]				= mode;
		_cacheGL.dbgVerticesUltimoUsadoIndice[iBufV]			= iUltVerticeUsaCmd;
		//
		_cacheGL.dbgIndicesUltimaFormaEsIndependiente[iBufV]	= esFiguraIndependiente;
		_cacheGL.dbgIndicesUltimaFormaEsIndependizable[iBufV]	= esFiguraIndependizable;
		_cacheGL.dbgIndicesUltimoUsadoModo[iBufV]				= mode;
		_cacheGL.dbgIndicesUltimoUsadoIndice[iBufV]				= first + count - 1;
	}*/
#	endif
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
}
#endif

#ifdef CONFIG_NB_GESTOR_ESCENAS_MODELOS_MEDIANTE_INDICES
void NBGestorGL::acumularIndicesTriangStripIndependiente(GLint first, GLsizei count){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::acumularIndicesTriangStripIndependiente")
	NBASSERT(_gestorInicializado);
	NBASSERT(first > 0)  //El primer elemento esta reservado
	NBASSERT(count > 1) //No se aceptan comandos vacios, optimizar codigo
#	ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
	/*{
		const UI32 iSigUlt	= (first + count);
		const UI32 usoInd	= _vertsGlArrs[_cacheGL.iVaoLigado].datosGL[_indiceBufferDatosLeer].usoArregloIndices;
		const UI32 ind0		= _vertsGlArrs[_cacheGL.iVaoLigado].datosGL[_indiceBufferDatosLeer].arregloIndices[first];
		const UI32 ind1		= _vertsGlArrs[_cacheGL.iVaoLigado].datosGL[_indiceBufferDatosLeer].arregloIndices[first + 1];
		const UI32 indPu	= _vertsGlArrs[_cacheGL.iVaoLigado].datosGL[_indiceBufferDatosLeer].arregloIndices[first + count - 2];
		const UI32 indUl	= _vertsGlArrs[_cacheGL.iVaoLigado].datosGL[_indiceBufferDatosLeer].arregloIndices[first + count - 1];
		NBASSERT(((sizeof(GLushort) * count) % 4) == 0) //Cantidad multiplo de 4 bytes
		NBASSERT(iSigUlt  <= usoInd) //(first + count) sobrepasa el tamano del arreglo
		NBASSERT(ind0 == ind1 && indPu == indUl) //Si falla, no es un triangStripIndependiente (no repite dos veces el primer y ultimo indice)
	}*/
#	endif
	if(_loteDrawElemsAcumTriangStrips.cantAcumuladaElems != 0 && (_loteDrawElemsAcumTriangStrips.iPrimerElem+_loteDrawElemsAcumTriangStrips.cantAcumuladaElems) != first){
		NBASSERT(((first * sizeof(GLushort)) % 4) == 0) //La direccion debe ser multiplo de 4 bytes (direcciones de 32 bits)
		//El bloque de indices no puede encadenarse con el anterior (purgar el anterior)
		NBGESTORGL_LOTE_TRIANGSTRIPS_PURGAR("NBGestorGL::acumularIndicesTriangStripIndependiente")
	}
	if(_loteDrawElemsAcumTriangStrips.cantAcumuladaElems == 0){
		//No hay bloque anterior, el nuevo bloque sera el primero
		NBASSERT(((first * sizeof(GLushort)) % 4) == 0) //La direccion debe ser multiplo de 4 bytes (direcciones de 32 bits)
		_loteDrawElemsAcumTriangStrips.iPrimerElem	= first;
	}
	//Acumular los indices al bloque actual
	_loteDrawElemsAcumTriangStrips.cantAcumuladaElems	+= count;
#	ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT_GL_NOP
		NBGestorGL::privDbgAcumularOperacionRealizada();
#	endif
	NBGESTORGL_DBG_NOMBRAR_CONVOCADOR_ACUMULAR_TRIANG_STRIPS_INDEP("[SIN_NOMBRE]")
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
}
#endif

#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT_GL_NOP
void NBGestorGL::privDbgAcumularOperacionRealizada(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::privDbgAcumularOperacionRealizada")
	UI16 iTex; for(iTex=0; iTex < _cacheGL.vaoTipoLigado; iTex++) _cacheGL.unidadesTex[iTex].dbgCantOperacionesConTex++;
	_cacheGL.dbgVAOCantOperaciones++;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
}
#endif

#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT_GL_NOP
void NBGestorGL::privDbgReiniciarCacheVerticesIndicesUsados(const ENVerticeGlTipo modo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::privDbgReiniciarCacheVerticesIndicesUsados")
	NBASSERT(modo >= 0 && modo < ENVerticeGlTipo_Conteo)
	_cacheGL.dbgVerticesUltimaFormaEnIndices[modo]			= false;
	//
	_cacheGL.dbgVerticesUltimaFormaEsIndependiente[modo]	= false;
	_cacheGL.dbgVerticesUltimaFormaEsIndependizable[modo]	= false;
	_cacheGL.dbgVerticesUltimoUsadoModo[modo]				= GL_NO_ERROR;
	_cacheGL.dbgVerticesUltimoUsadoIndice[modo]				= -2;
	//
	_cacheGL.dbgIndicesUltimaFormaEsIndependiente[modo]		= false;
	_cacheGL.dbgIndicesUltimaFormaEsIndependizable[modo]	= false;
	_cacheGL.dbgIndicesUltimoUsadoModo[modo]				= GL_NO_ERROR;
	_cacheGL.dbgIndicesUltimoUsadoIndice[modo]				= -2;
	//
	_cacheGL.dbgDrawArrayConvocador->establecer("[Vacio]");
	_cacheGL.dbgDrawElementsConvocador->establecer("[Vacio]");
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
}
#endif

#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT_GL_NOP
void NBGestorGL::dbgNombrarConvocadorDrawArrays(const char* strNombre){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::dbgNombrarConvocadorDrawArrays")
	_cacheGL.dbgDrawArrayConvocador->establecer(strNombre);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
}
#endif

#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT_GL_NOP
void NBGestorGL::dbgNombrarConvocadorAcumTriangStripsIndep(const char* strNombre){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorGL::dbgNombrarConvocadorAcumTriangStripsIndep")
	_cacheGL.dbgDrawElementsConvocador->establecer(strNombre);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
}
#endif

#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT_GL_NOP
const char* NBGestorGL::dbgModoFormaGLNombre(GLenum modo){
	switch (modo) {
		case GL_TRIANGLE_STRIP: return "GL_TRIANGLE_STRIP"; break;
		case GL_TRIANGLE_FAN:	return "GL_TRIANGLE_FAN"; break;
		case GL_LINES:			return "GL_LINES"; break;
		case GL_LINE_STRIP:		return "GL_LINE_STRIP"; break;
		case GL_LINE_LOOP:		return "GL_LINE_LOOP"; break;
		default: NBASSERT(false) break; //Modo no-valido o no-implementado
	}
	return "???";
}
#endif

#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT_GL_NOP
bool NBGestorGL::dbgModoFormaGLEsIndependizable(GLenum modo){
	switch (modo) {
		case GL_TRIANGLE_STRIP:	return true; break;
		case GL_TRIANGLE_FAN:	return true; break; //Si se convierte de GL_TRIANGLE_FAN a GL_TRIANGLE_STRIP
		case GL_LINES:			return true; break;
		case GL_LINE_STRIP:		return false; break; //Pendiente de experimentar
		case GL_LINE_LOOP:		return false; break;
		default: NBASSERT(false) break; //Modo no-valido o no-implementado
	}
	return false;
}
#endif
