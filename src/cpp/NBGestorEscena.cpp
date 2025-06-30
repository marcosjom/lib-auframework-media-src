//
//  NBGestorEscena.cpp
//  AUFramework
//
//  Created by Nicaragua Binary on 22/10/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkMediaStdAfx.h"
#include "NBGestorEscena.h"
#include "NBGestorTeclas.h"
#include "NBGestorFuentes.h"

bool 						NBGestorEscena::_gestorInicializado = false;
SI32						NBGestorEscena::_ticksAcumulados = 0;
UI32						NBGestorEscena::_secuencialActualizacionesModelos = 0;
UI32						NBGestorEscena::_secuencialActualizacionesModelosGL = 0;
UI32						NBGestorEscena::_secuencialRenderizadasModelosGL = 0;
bool 						NBGestorEscena::_lucesSombrasActivos = false;
bool 						NBGestorEscena::_ayudasParaPantallaPequena = false;
float						NBGestorEscena::_pantallaFrecuenciaNativa = 60.0f;
float						NBGestorEscena::_pantallaFrecuenciaPintando = 60.0f;
float 						NBGestorEscena::_cuadrosPorSegundo = 60.0f;

bool						NBGestorEscena::_pantallaPermitidoRotar = false;

float						NBGestorEscena::_cambioDefinicionEscalaHaciaHD = 1.0f;
ENGestorEscenaCambioDef		NBGestorEscena::_cambioDefinicioEstado = ENGestorEscenaCambioDef_Conteo;

AUArregloNativoMutableP<STRangoSombra>* NBGestorEscena::_cacheSombrasFusionadas = NULL;

//Bufferes datos
SI32						NBGestorEscena::_indiceBufferDatosLeer = 0;
SI32						NBGestorEscena::_indiceBufferDatosEscribir = 0;
ENGestorEscenaBufferEstado	NBGestorEscena::_bufferDatosEstado[NBGESTORESCENA_CANTIDAD_BUFFERES_DATOS];
bool						NBGestorEscena::_bufferDatosBloqueado[NBGESTORESCENA_CANTIDAD_BUFFERES_DATOS];
STBufferVerticesGL			NBGestorEscena::_buffersVertices[NBGESTORESCENA_CANTIDAD_BUFFERES_DATOS][ENVerticeGlTipo_Conteo];
/*
 Para asegurar la integridad del proceso del modelo productor/consumidor
 estas variables se mantienen con valor NULL
 hasta que se invoque el bloqueParaLlenado.
 */
NBVerticeGL0*				NBGestorEscena::verticesGL0	= NULL;
NBVerticeGL*				NBGestorEscena::verticesGL1	= NULL;
NBVerticeGL2*				NBGestorEscena::verticesGL2	= NULL;
NBVerticeGL3*				NBGestorEscena::verticesGL3	= NULL;
#ifdef CONFIG_NB_GESTOR_ESCENAS_MODELOS_MEDIANTE_INDICES
GLushort*					NBGestorEscena::indicesGL0	= NULL;
GLushort*					NBGestorEscena::indicesGL1	= NULL;
GLushort*					NBGestorEscena::indicesGL2	= NULL;
GLushort*					NBGestorEscena::indicesGL3	= NULL;
#endif

//

STGestorEscenaTextura		NBGestorEscena::_texturasParaRenderizadoGL[NBGESTORESCENA_MAX_TEXTURAS_RENDER];
STGestorEscenaFrameBuffer	NBGestorEscena::_frameBuffersGL[NBGESTORESCENA_MAX_ESCENAS];
STGestorEscenaEscena		NBGestorEscena::_escenas[NBGESTORESCENA_MAX_ESCENAS];

bool NBGestorEscena::inicializar(const float pantallaFrecuencia){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::inicializar")
	_pantallaFrecuenciaNativa			= pantallaFrecuencia;
	_pantallaFrecuenciaPintando			= pantallaFrecuencia;
	_pantallaPermitidoRotar				= false;
	_cambioDefinicionEscalaHaciaHD		= 0.0f;
	_cambioDefinicioEstado				= ENGestorEscenaCambioDef_Conteo;
	_lucesSombrasActivos				= true;
	_ayudasParaPantallaPequena			= true;
	_cuadrosPorSegundo					= 60.0f;
	_ticksAcumulados					= 0;
	_secuencialActualizacionesModelos	= 2; //El '0' y '1' estan reservados
	_secuencialActualizacionesModelosGL	= 2; //El '0' y '1' estan reservados
	_secuencialRenderizadasModelosGL	= 2; //El '0' y '1' estan reservados
	_cacheSombrasFusionadas				= new(ENMemoriaTipo_General) AUArregloNativoMutableP<STRangoSombra>(16);
	//
	_indiceBufferDatosEscribir			= 0;
	_indiceBufferDatosLeer				= NBGESTORESCENA_CANTIDAD_BUFFERES_DATOS - 1;
	{
		SI32 iBuff; for(iBuff=0; iBuff<NBGESTORESCENA_CANTIDAD_BUFFERES_DATOS; iBuff++){
			_bufferDatosEstado[iBuff]		= ENGestorEscenaBufferEstado_Vacio;
			_bufferDatosBloqueado[iBuff]	= false;
		}
	}
	//Inicializar todos los registros de texturas
	{
		SI32 iTex; for(iTex=0; iTex<NBGESTORESCENA_MAX_TEXTURAS_RENDER; iTex++){
			_texturasParaRenderizadoGL[iTex].registroOcupado	= false;
		}
	}
	//Inicializar todos los registros para frambuffersGL
	{
		SI32 iFrameb; for(iFrameb=0; iFrameb<NBGESTORESCENA_MAX_ESCENAS; iFrameb++){
			_frameBuffersGL[iFrameb].registroOcupado		= false;
		}
	}
	//Inicializar todos los registros para escenas
	{
		SI32 iEscena; for(iEscena = 0; iEscena < NBGESTORESCENA_MAX_ESCENAS; iEscena++){
			STGestorEscenaEscena* scn = &_escenas[iEscena];
			scn->registroOcupado				= false;
			scn->escenaHabilitada				= false;
			scn->pendienteRenderizar			= false;
			UI16 iGrp;
			for(iGrp = 0; iGrp < ENGestorEscenaGrupo_Conteo; iGrp++){
				scn->gruposCapas[iGrp].capas	= NULL;
			}
			UI16 iBuffer;
			for(iBuffer=0; iBuffer<NBGESTORESCENA_CANTIDAD_BUFFERES_DATOS; iBuffer++){
				scn->renderCapas[iBuffer]		= NULL;
				scn->renderCapasConsumir[iBuffer] = NULL;
				scn->renderCapasProducir[iBuffer] = NULL;
			}
			scn->agrupadoresParticulas			= NULL;
			scn->escuchadoresCambioPuertoVision	= NULL;
			scn->capasEnlazadas					= NULL;
		}
	}
	//
	NBGestorEscena::privIncializarConfigurarGL();
	//Inicializar arreglos de verticesGL
	{
		SI32 iTip, iBuff; NBVerticeGL3 vP;
		for(iTip = 0; iTip < ENVerticeGlTipo_Conteo; iTip++){
			const SI32 idBuffer = NBGestorGL::bufferVerticesReservar((ENVerticeGlTipo)iTip, &NBGestorEscena::bufferObtenerDatos, NULL); NBASSERT(idBuffer == iTip)
			for(iBuff = 0; iBuff < NBGESTORESCENA_CANTIDAD_BUFFERES_DATOS; iBuff++){
				STBufferVerticesGL* ptrDat = &_buffersVertices[iBuff][iTip];
				/*Arreglo de verticesGL*/ \
				ptrDat->bytesPorRegistro		= sizeof(vP.x) + sizeof(vP.y) + sizeof(vP.r) + sizeof(vP.g) + sizeof(vP.b) + sizeof(vP.a) + ((sizeof(vP.tex.x) + sizeof(vP.tex.y)) * iTip); NBASSERT((ptrDat->bytesPorRegistro % 4) == 0) /*Debe ser multiplo de 4 bytes (32 bits)*/
				ptrDat->tamanoArregloVertices	= NB_GESTOR_GL_CRECIMIENTO_BLOQUE_VERTICES_GL;
				ptrDat->arregloVertices			= (BYTE*)NBGestorMemoria::reservarMemoria(ptrDat->tamanoArregloVertices * ptrDat->bytesPorRegistro, ENMemoriaTipo_General); NB_DEFINE_NOMBRE_PUNTERO(ptrDat->arregloVertices, "NBGestorGL::arregloVerticesGL")
				ptrDat->usoArregloVertices		= 1; /*el primer elemento esta reservado*/ NBASSERT(((ptrDat->bytesPorRegistro * ptrDat->usoArregloVertices) % 4) == 0)
#				ifdef CONFIG_NB_GESTOR_ESCENAS_MODELOS_MEDIANTE_INDICES
				/*Arreglo de indicesGL*/
				ptrDat->tamanoArregloIndices	= NB_GESTOR_GL_CRECIMIENTO_BLOQUE_INDICES_GL; NBASSERT(((sizeof(GLushort) * NB_GESTOR_GL_CRECIMIENTO_BLOQUE_INDICES_GL) % 4) == 0) /*Debe ser multiplo de 4 bytes (32 bits)*/
				ptrDat->arregloIndices			= (GLushort*)NBGestorMemoria::reservarMemoria(ptrDat->tamanoArregloIndices * sizeof(GLushort), ENMemoriaTipo_General); NB_DEFINE_NOMBRE_PUNTERO(ptrDat->arregloIndices, "NBGestorGL::arregloIndicesGL")
				ptrDat->usoArregloIndices		= 2; /*los primeros dos elementos estan reservados (multiplo de 4 bytes)*/ NBASSERT(((sizeof(GLushort) * ptrDat->usoArregloIndices) % 4) == 0)
#				endif
			}
		}
	}
	NBSegmentadorFiguras::inicializar();
	//
	_gestorInicializado			= true;
	//
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
	return true;
}

void NBGestorEscena::finalizar(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::finalizar")
	NBSegmentadorFiguras::finalizar();
	//Eliminar las escenas
	SI32 iEscena; for(iEscena = 0; iEscena < NBGESTORESCENA_MAX_ESCENAS; iEscena++){
		STGestorEscenaEscena* scn = &_escenas[iEscena];
		NBASSERT((scn->registroOcupado && scn->agrupadoresParticulas != NULL) || (!scn->registroOcupado && scn->agrupadoresParticulas == NULL))
		NBASSERT((scn->registroOcupado && scn->escuchadoresCambioPuertoVision != NULL && scn->capasEnlazadas != NULL) || (!scn->registroOcupado && scn->escuchadoresCambioPuertoVision == NULL && scn->capasEnlazadas == NULL))
		if(scn->registroOcupado){
			if(scn->texturaEscena != NULL) scn->texturaEscena->liberar(NB_RETENEDOR_NULL);
			NBGestorEscena::vaciarGrupos(iEscena);
			SI32 iGrp;
			for(iGrp = 0; iGrp < ENGestorEscenaGrupo_Conteo; iGrp++){
				STGestorEscenaGrupo* grp = &(scn->gruposCapas[iGrp]);
				if(grp->capas != NULL) grp->capas->liberar(NB_RETENEDOR_NULL); grp->capas = NULL;
			}
			SI32 iBuff;
			for(iBuff=0; iBuff<NBGESTORESCENA_CANTIDAD_BUFFERES_DATOS; iBuff++){
				AUArregloNativoMutableP<STGestorEscenaCapaRender>* renderCapas = scn->renderCapas[iBuff];
				UI16 iCapa;
				for(iCapa=0; iCapa<renderCapas->conteo; iCapa++){
					STGestorEscenaCapaRender* capaRender = &(renderCapas->elemento[iCapa]);
					capaRender->renderModelosDatos->liberar(NB_RETENEDOR_NULL);
					capaRender->renderModelos->liberar(NB_RETENEDOR_NULL);
					capaRender->renderPiscinaObjetosRetener->liberar(NB_RETENEDOR_NULL);
					//
					capaRender->renderIluminacion->liberar(NB_RETENEDOR_NULL);
					capaRender->renderEspejos->liberar(NB_RETENEDOR_NULL);
					capaRender->renderProductoresSombras->liberar(NB_RETENEDOR_NULL);
					capaRender->renderDescriptoresCuerdas->liberar(NB_RETENEDOR_NULL);
					capaRender->renderConsumidoresLuces->liberar(NB_RETENEDOR_NULL);
					capaRender->renderConsumidoresEspejos->liberar(NB_RETENEDOR_NULL);
					capaRender->renderConsumidoresCuerdas->liberar(NB_RETENEDOR_NULL);
					capaRender->renderConsumidoresPreModeloGL->liberar(NB_RETENEDOR_NULL);
					capaRender->renderSombrasPorIluminacion->liberar(NB_RETENEDOR_NULL);
					capaRender->renderSombrasRangosPorIluminacion->liberar(NB_RETENEDOR_NULL);
					capaRender->renderSombrasVerticesIntermedios->liberar(NB_RETENEDOR_NULL);
					//
					capaRender->renderMascLuz->liberar(NB_RETENEDOR_NULL); capaRender->renderMascLuz = NULL;
					capaRender->renderMascLuzBloquesGL->liberar(NB_RETENEDOR_NULL); capaRender->renderMascLuzBloquesGL = NULL;
					capaRender->renderMascLuzLucesConIntensidad->liberar(NB_RETENEDOR_NULL); capaRender->renderMascLuzLucesConIntensidad = NULL;
					STGestorEscenaTexturaLuz::inicializar(&capaRender->renderMascLucesCombinadas);
				}
				scn->renderCapas[iBuff]->liberar(NB_RETENEDOR_NULL);
				scn->renderCapasConsumir[iBuff]->liberar(NB_RETENEDOR_NULL);
				scn->renderCapasProducir[iBuff]->liberar(NB_RETENEDOR_NULL);
				scn->renderCapas[iBuff] = NULL;
				scn->renderCapasConsumir[iBuff] = NULL;
				scn->renderCapasProducir[iBuff] = NULL;
			}
			scn->agrupadoresParticulas->liberar(NB_RETENEDOR_NULL);
			scn->escuchadoresCambioPuertoVision->liberar(NB_RETENEDOR_NULL);
			scn->capasEnlazadas->liberar(NB_RETENEDOR_NULL);
		}
	}
	//Eliminar los framebuffers de renderizado de escenas
	SI32 iFrameb;
	for(iFrameb=0; iFrameb<NBGESTORESCENA_MAX_ESCENAS; iFrameb++){
		STGestorEscenaFrameBuffer* datosFB = &(_frameBuffersGL[iFrameb]);
		if(datosFB->registroOcupado){
			//Destinos actuales
			if(datosFB->texturaLigada != NULL) datosFB->texturaLigada->liberar(NB_RETENEDOR_NULL); datosFB->texturaLigada = NULL;
			if(datosFB->renderbufferLigado != NULL) datosFB->renderbufferLigado->liberar(NB_RETENEDOR_NULL); datosFB->renderbufferLigado = NULL;
			//Destinos originales
			if(datosFB->texturaOriginal != NULL){
				NBGestorTexturas::liberarEspacioTexturaSiSoloTieneUnaReferenciaExterna(datosFB->texturaOriginal);
				datosFB->texturaOriginal->liberar(NB_RETENEDOR_NULL);
				datosFB->texturaOriginal = NULL;
			}
			if(datosFB->renderbufferOriginal != NULL){
				GLuint idRender		= datosFB->renderbufferOriginal->idRenderBufferGl();
				if(idRender != 0) NBGestorGL::deleteRenderbuffersEXT(1, &idRender);
				datosFB->renderbufferOriginal->liberar(NB_RETENEDOR_NULL);
				datosFB->renderbufferOriginal = NULL;
			}
			//Eliminar buffers
			GLuint idStencil	= datosFB->idStencilBufferGl;
			GLuint idDepth		= datosFB->idDepthBufferGl;
			GLuint idFB			= datosFB->idFrameBufferGl;
			if(idStencil != 0) NBGestorGL::deleteRenderbuffersEXT(1, &idStencil);
			if(idDepth != 0 && idDepth != idStencil) NBGestorGL::deleteRenderbuffersEXT(1, &idDepth); //idDepthBufferGl y idStencilBufferGl pueden ser identicos, si la implementacion limita a depth+stencil combinados (iOS 4.0+)
			if(idFB != 0) NBGestorGL::deleteFramebuffersEXT(1, &idFB);
			datosFB->registroOcupado = false;
		}
	}
	//Eliminar todas las texturas
	NBGestorEscena::privTexturaRenderLiberarReservasTodas();
	//Eliminar arreglos de verticesGL
	{
		SI32 iTip, iBuff;
		for(iTip = 0; iTip < ENVerticeGlTipo_Conteo; iTip++){
			NBGestorGL::bufferVerticesLiberar(iTip);
			//
			for(iBuff = 0; iBuff < NBGESTORESCENA_CANTIDAD_BUFFERES_DATOS; iBuff++){
				STBufferVerticesGL* ptrDat = &_buffersVertices[iBuff][iTip];
				/*Arreglo de verticesGL*/
				if(ptrDat->arregloVertices != NULL) NBGestorMemoria::liberarMemoria(ptrDat->arregloVertices); ptrDat->arregloVertices = NULL;
				ptrDat->bytesPorRegistro		= 0;
				ptrDat->tamanoArregloVertices	= 0;
				ptrDat->usoArregloVertices		= 0;
#				ifdef CONFIG_NB_GESTOR_ESCENAS_MODELOS_MEDIANTE_INDICES
				/*Arreglo de indicesGL*/
				if(ptrDat->arregloIndices != NULL) NBGestorMemoria::liberarMemoria(ptrDat->arregloIndices); ptrDat->arregloIndices = NULL;
				ptrDat->tamanoArregloIndices	= 0;
				ptrDat->usoArregloIndices		= 0;
#				endif
			}
		}
	}
	//
	if(_cacheSombrasFusionadas != NULL) _cacheSombrasFusionadas->liberar(NB_RETENEDOR_NULL); _cacheSombrasFusionadas = NULL;
	_gestorInicializado = false;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

bool  NBGestorEscena::gestorInicializado(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::gestorInicializado")
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
	return _gestorInicializado;
}

bool NBGestorEscena::reiniciarContextoGrafico(const bool elimObjetosAnteriores){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::reiniciarContextoGrafico")
	bool exito = false;
	if(_gestorInicializado){
		//Reinicializar configuracion GL
		NBGestorEscena::privIncializarConfigurarGL();
		//Recrear FRAMEBUFFERS
		{
			SI32 i;
			for(i = 0; i < NBGESTORESCENA_MAX_ESCENAS; i++){
				STGestorEscenaFrameBuffer* datosFB = &_frameBuffersGL[i];
				if(datosFB->registroOcupado){
					if(datosFB->tipoDestinoGL != ENGestorEscenaDestinoGl_Heredado){
						NBASSERT(datosFB->tipoDestinoGL == ENGestorEscenaDestinoGl_RenderBuffer || datosFB->tipoDestinoGL == ENGestorEscenaDestinoGl_Textura)
						const GLuint idFBAnterior = datosFB->idFrameBufferGl;
						if(datosFB->renderbufferLigado != NULL){ datosFB->renderbufferLigado->liberar(NB_RETENEDOR_NULL); datosFB->renderbufferLigado = NULL; }
						if(datosFB->texturaLigada != NULL){ datosFB->texturaLigada->liberar(NB_RETENEDOR_NULL); datosFB->texturaLigada = NULL; }
						//Eliminar framebuffer
						if(elimObjetosAnteriores){
							GLuint idStencil	= datosFB->idStencilBufferGl;
							GLuint idDepth		= datosFB->idDepthBufferGl;
							GLuint idFB			= datosFB->idFrameBufferGl;
							PRINTF_INFO("Eliminando Framebuffer idGL(%d) idStencil(%d) idDepth(%d).\n", idFB, idStencil, idDepth);
							if(idStencil != 0) NBGestorGL::deleteRenderbuffersEXT(1, &idStencil);
							if(idDepth != 0 && idDepth != idStencil) NBGestorGL::deleteRenderbuffersEXT(1, &idDepth);
							if(idFB != 0) NBGestorGL::deleteFramebuffersEXT(1, &idFB);
						}
						datosFB->idStencilBufferGl = 0;
						datosFB->idDepthBufferGl = 0;
						datosFB->idFrameBufferGl = 0;
						//Recrear Framebuffer
						{
							GLuint idFB;
							NBGestorGL::genFramebuffersEXT(1, &idFB); PRINTF_INFO("REgenerando framebuffer idGL(%d) idGLAntes(%d).\n", idFB, idFBAnterior);
							NBGestorGL::bindFramebufferEXT(GL_FRAMEBUFFER_EXT, idFB);
							datosFB->idFrameBufferGl = idFB;
						}
						//Recrear renderbuffer
						if(datosFB->renderbufferOriginal != NULL){
							NBASSERT(datosFB->tipoDestinoGL == ENGestorEscenaDestinoGl_RenderBuffer)
							GLuint idRBAnterior = datosFB->renderbufferOriginal->idRenderBufferGl();
							//Eliminar render buffer
							if(elimObjetosAnteriores){
								if(idRBAnterior != 0) NBGestorGL::deleteRenderbuffersEXT(1, &idRBAnterior);
								PRINTF_INFO("Eliminando Renderbuffer idGL(%d).\n", idRBAnterior);
							}
							//Recrear renderbuffer
							{
								GLuint idRB;
								NBTamano tamGL = datosFB->renderbufferOriginal->tamanoDestinoGl();
								NBGestorGL::genRenderbuffersEXT(1, &idRB); PRINTF_INFO("REgenerando renderbuffer idGL(%d) idGLAnterior(%d).\n", idRB, idRBAnterior);
								NBGestorGL::bindRenderbufferEXT(GL_RENDERBUFFER_EXT, idRB);
								if(datosFB->funcionGeneraRenderBuffer != NULL){
									//PRINTF_INFO("Generando RENDERBUFFER desde fuente externa\n");
									const NBTamanoI tamanoRenderBuffer = datosFB->funcionGeneraRenderBuffer(tamGL.ancho, tamGL.alto, datosFB->funcionGeneraRenderBufferParams);
									tamGL.ancho	= tamanoRenderBuffer.ancho;
									tamGL.alto	= tamanoRenderBuffer.alto;
									NBASSERT(tamGL.ancho > 0 && tamGL.alto > 0);
								} else {
									NBASSERT(tamGL.ancho > 0 && tamGL.alto > 0);
									NBGestorGL::renderbufferStorageEXT(GL_RENDERBUFFER_EXT, NBGestorTexturas::formatoGlLocal(datosFB->colorFrameBufferGl), tamGL.ancho, tamGL.alto);
								}
								NBASSERT(tamGL.ancho == datosFB->anchoFrameBufferGl && tamGL.alto == datosFB->altoFrameBufferGl)
								datosFB->renderbufferOriginal->relinkearRenderBuffer(idRB);
								NBGestorGL::framebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_RENDERBUFFER_EXT, idRB);
								//datosFB->renderbufferLigado = datosFB->renderbufferOriginal; datosFB->renderbufferLigado->retener(NB_RETENEDOR_THIS);
#							ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT_GL_FRAMEBUFFERS_COMPLETOS
								if(NBGestorEscena::privFrameBufferCheckStatusEXT()){
									PRINTF_INFO("Framebuffer i%d restaurado exitosamente (to-renderbuffer).\n", i);
								} else {
									PRINTF_ERROR("Framebuffer i%d no pudo se restaurado (to-renderbuffer).\n", i);
								}
#							endif
							}
						}
						//Relinkear textura
						if(datosFB->texturaOriginal != NULL){
							NBASSERT(datosFB->tipoDestinoGL == ENGestorEscenaDestinoGl_Textura)
							//La textura ya tuvo que ser relinkeada por el NBGestorTexturas
							NBGestorGL::framebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, datosFB->texturaOriginal->idTexturaGL, 0);
							//datosFB->texturaLigada = datosFB->texturaOriginal; datosFB->texturaLigada->retener(NB_RETENEDOR_THIS);
#							ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT_GL_FRAMEBUFFERS_COMPLETOS
							if(NBGestorEscena::privFrameBufferCheckStatusEXT()){
								PRINTF_INFO("Framebuffer i%d restaurado exitosamente (to-texture).\n", i);
							} else {
								PRINTF_ERROR("Framebuffer i%d no pudo se restaurado (to-texture).\n", i);
							}
#							endif
						}
					}
				}
			}
		}
		//Relinkear escenas
		{
			SI32 i;
			for(i = 0; i < NBGESTORESCENA_MAX_ESCENAS; i++){
				STGestorEscenaEscena* scn = &_escenas[i];
				if(scn->registroOcupado){
					if(scn->texturaEscena != NULL){
						STGestorEscenaFrameBuffer* datosFB = &_frameBuffersGL[scn->iFramebufferEscena];
						PRINTF_INFO("Escena i%d, RElinkeando textura-destino idGLAntes(%d) idGLDespues(%d).\n", i, scn->texturaEscena->idTexturaGL, datosFB->texturaOriginal->idTexturaGL);
						scn->texturaEscena->relinkearTextura(datosFB->texturaOriginal->idTexturaGL);
					}
				}
			}
		}
		exito = true;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
	return exito;
}

float NBGestorEscena::pantallaFrecuenciaNativa(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::pantallaFrecuenciaNativa")
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
	return _pantallaFrecuenciaNativa;
}

float NBGestorEscena::pantallaFrecuenciaPintando(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::pantallaFrecuenciaPintando")
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
	return _pantallaFrecuenciaPintando;
}

bool NBGestorEscena::pantallaPermitidoRotar(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::pantallaPermitidoRotar")
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
	return _pantallaPermitidoRotar;
}

void NBGestorEscena::pantallaEstablecerPermitidoRotar(bool permitidoRotarPantalla){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::pantallaEstablecerPermitidoRotar")
	_pantallaPermitidoRotar = false; //permitidoRotarPantalla;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

void NBGestorEscena::cambioDefinicionListarEscalaHaciaHD(float cambiarHacia){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::cambioDefinicionListarEscalaHaciaHD")
	_cambioDefinicionEscalaHaciaHD	= cambiarHacia; NBASSERT(cambiarHacia>0.0f)
	_cambioDefinicioEstado			= ENGestorEscenaCambioDef_Inicio;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

ENGestorEscenaCambioDef NBGestorEscena::cambioDefinicionEstado(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::cambioDefinicionEstado")
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
	return _cambioDefinicioEstado;
}

void NBGestorEscena::cambioDefinicionMoverHaciaEstadoSiguiente(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::cambioDefinicionMoverHaciaEstadoSiguiente")
	NBASSERT(_cambioDefinicioEstado<ENGestorEscenaCambioDef_Conteo)
	_cambioDefinicioEstado = (ENGestorEscenaCambioDef)((SI32)_cambioDefinicioEstado + 1);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

float NBGestorEscena::cambioDefinicionEscalaHaciaHD(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::cambioDefinicionEscalaHaciaHD")
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
	return _cambioDefinicionEscalaHaciaHD;
}

//
// CONFIGURACION GL
//

void NBGestorEscena::privIncializarConfigurarGL(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::privIncializarConfigurarGL")
	NBGestorGL::disableInit(GL_ALPHA_TEST); 				//hace alpha testing (ver glAlphaFunc) //Segun este articulo "GL_ALPHA_TEST" reduce el rendimiento drasticamente en iPad: http://stackoverflow.com/questions/2785640/optimizing-iphone-opengl-es-fill-rate
	NBGestorGL::disableInit(GL_COLOR_LOGIC_OP);				//aplica la operacion logica indicada al color entrante con el del buffer ver glLogicOp)
	NBGestorGL::disableInit(GL_CULL_FACE);					//omite poligonos en base a la coordenada de la ventana (ver glCullFace)
	NBGestorGL::disableInit(GL_DEPTH_TEST);					//hace comparaciones de profucndidad y actualiza el DepthBuffer
	NBGestorGL::disableInit(GL_DITHER);						//trama los componentes de color e indices antes de escribirlos en el ColorBuffer
	NBGestorGL::disableInit(GL_FOG);						//combina el color de niebla en el color prostexturizado
	NBGestorGL::disableInit(GL_LINE_SMOOTH);				//dibuja lineas con el filtrado correcto
	NBGestorGL::disableInit(GL_MULTISAMPLE);				//realiza multisampling de fragmentos para antialiasing de una pasada y otros efectos,
	NBGestorGL::disableInit(GL_NORMALIZE);					//vectores normales son escalados a la unidad de longitud despues de la trabnsformacion
	NBGestorGL::disableInit(GL_POINT_SMOOTH);				//dibuja puntos con el filtrado apropiado
	NBGestorGL::disableInit(GL_POLYGON_OFFSET_FILL);		//un desplazamiento es agregado a los valores de profundidad de los fragmentos de poligonos antes de realizar la comparacion de profundidad
	NBGestorGL::disableInit(GL_RESCALE_NORMAL);				//los vectores normal son escalados a un factor derivado de la matriz del modelo de vista
	NBGestorGL::disableInit(GL_SAMPLE_ALPHA_TO_COVERAGE);	//convierte fragmentos de valores alpha a mascaras de cobertura de multisample
	NBGestorGL::disableInit(GL_SAMPLE_ALPHA_TO_ONE);		//establecer el fragmento alpha al maximo valor permisible despues de calcular las mascaras de cobertura de multisample
	NBGestorGL::disableInit(GL_SAMPLE_COVERAGE);			//
	//
	// NOTA: GL_LIGHTING y GL_COLOR_MATERIAL son requeridos
	// para multiplicar los colores individuales de
	// cada vertice por un color de ambiente.
	// Cuando se define un arreglo de colores de vertice (GL_COLOR_ARRAY)
	// el color definido por glColor pierde utilidad.
	//
	//NBGestorGL::colorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE); //glColorMaterial mut be called before enabling GL_COLOR_MATERIAL.
	NBGestorGL::lightModelf(GL_LIGHT_MODEL_TWO_SIDE, 0.0f);	//Determina que si debe calcularse la luz para ambos lados del poligono
	NBGestorGL::disableInit(GL_LIGHTING);					//Colorear vertices segun luz de ambiente.			//usa los parametros de las luces actuales para calcular los colores
	NBGestorGL::disableInit(GL_COLOR_MATERIAL);				//Colorear vertices segun luz de ambiente.	//los parametros del material de ambiente y difuso rastrean el color actual
	//PENDIENTE: habilitar
	NBGestorGL::enableInit(GL_BLEND);						//combina los colores entrantes (texturas) con los del buffer (ver glBlendFunc)
	NBGestorGL::enableInit(GL_TEXTURE_2D);					//Primera unidad textura
	NBGestorGL::disableInit(GL_SCISSOR_TEST);				//descarta los fragmentos que estan fuera del rectangulo de la tijera (ver glScissor) (cuando esta deshabilitado, la scissor se establece en los limites de la pantalla/buffer)
	NBGestorGL::disableInit(GL_STENCIL_TEST);				//hace comparaciones de enmascaramiento y actualiza el StencilBuffer
	NBGestorGL::matrixModeInit(GL_TEXTURE);
	NBGestorGL::matrixModeInit(GL_MODELVIEW);
	NBGestorGL::matrixModeInit(GL_PROJECTION);
	//
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

//
// FRAME BUFFERS
//

void NBGestorEscena::privFrameBufferEnlazarTextura(SI32 iFrameBuffer, AUTextura* texRender){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::privFrameBufferEnlazarTextura")
	NBASSERT(iFrameBuffer >= 0 && iFrameBuffer<NBGESTORESCENA_MAX_ESCENAS)
	NBASSERT(texRender != NULL)
	STGestorEscenaFrameBuffer* datosFB = &_frameBuffersGL[iFrameBuffer];
	NBASSERT(datosFB->registroOcupado)
	//NBASSERT(datosFB->tipoDestinoGL == ENGestorEscenaDestinoGl_Textura)
	if(datosFB->texturaLigada != texRender){
		//PRINTF_INFO("FRAMEBUFFER(%d) ENLAZANDO CON TEXTURA idGL(%d)", datosFB->idFrameBufferGl, texRender->idTexturaGL);
		NBGestorGL::bindFramebufferEXT(GL_FRAMEBUFFER_EXT, datosFB->idFrameBufferGl);
		NBGestorGL::framebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, texRender->idTexturaGL, 0);
		#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT_GL_FRAMEBUFFERS_COMPLETOS
		NBGestorEscena::privFrameBufferCheckStatusEXT();
		#endif
		//
		texRender->retener(NB_RETENEDOR_NULL);
		if(datosFB->texturaLigada != NULL) datosFB->texturaLigada->liberar(NB_RETENEDOR_NULL); datosFB->texturaLigada = NULL;
		if(datosFB->renderbufferLigado != NULL) datosFB->renderbufferLigado->liberar(NB_RETENEDOR_NULL); datosFB->renderbufferLigado = NULL;
		datosFB->texturaLigada			= texRender;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

void NBGestorEscena::privFrameBufferEnlazarRenderBuffer(SI32 iFrameBuffer, AURenderBuffer* renderBuffer){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::privFrameBufferEnlazarRenderBuffer")
	NBASSERT(iFrameBuffer >= 0 && iFrameBuffer<NBGESTORESCENA_MAX_ESCENAS)
	NBASSERT(renderBuffer != NULL)
	STGestorEscenaFrameBuffer* datosFB = &_frameBuffersGL[iFrameBuffer];
	NBASSERT(datosFB->registroOcupado)
	//NBASSERT(datosFB->tipoDestinoGL == ENGestorEscenaDestinoGl_RenderBuffer)
	if(datosFB->renderbufferLigado != renderBuffer){
		PRINTF_INFO("FRAMEBUFFER(%d) ENLAZANDO CON RENDERBUFFER idGL(%d)", datosFB->idFrameBufferGl, renderBuffer->idRenderBufferGl());
		NBGestorGL::bindFramebufferEXT(GL_FRAMEBUFFER_EXT, _frameBuffersGL[iFrameBuffer].idFrameBufferGl);
		NBGestorGL::framebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_RENDERBUFFER_EXT, renderBuffer->idRenderBufferGl());
		#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT_GL_FRAMEBUFFERS_COMPLETOS
		NBGestorEscena::privFrameBufferCheckStatusEXT();
		#endif
		//
		renderBuffer->retener(NB_RETENEDOR_NULL);
		if(datosFB->texturaLigada != NULL) datosFB->texturaLigada->liberar(NB_RETENEDOR_NULL); datosFB->texturaLigada = NULL;
		if(datosFB->renderbufferLigado != NULL) datosFB->renderbufferLigado->liberar(NB_RETENEDOR_NULL); datosFB->renderbufferLigado = NULL;
		datosFB->renderbufferLigado	= renderBuffer;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT_GL_FRAMEBUFFERS_COMPLETOS
bool NBGestorEscena::privFrameBufferCheckStatusEXT(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::privFrameBufferCheckStatusEXT")
	bool r = true;
	GLenum estadoFBO = NBGestorGL::checkFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if(estadoFBO != GL_FRAMEBUFFER_COMPLETE_EXT){
		PRINTF_ERROR("GL: el nuevo frame buffer no indica esta completo (estado %d): ", estadoFBO);
		switch (estadoFBO) {
			#ifdef GL_FRAMEBUFFER_UNDEFINED
			case GL_FRAMEBUFFER_UNDEFINED: PRINTF_INFO("GL_FRAMEBUFFER_UNDEFINED"); break;
			#endif
			#ifdef GL_FRAMEBUFFER_UNDEFINED_EXT
			case GL_FRAMEBUFFER_UNDEFINED_EXT: PRINTF_INFO("GL_FRAMEBUFFER_UNDEFINED_EXT"); break;
			#endif
			#ifdef GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT
			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: PRINTF_INFO("GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT"); break;
			#elif defined(GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT)
			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT: PRINTF_INFO("GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT"); break;
			#endif
			#ifdef GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT
			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: PRINTF_INFO("GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT"); break;
			#elif defined(GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT)
			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT: PRINTF_INFO("GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT"); break;
			#endif
			#ifdef GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER
			case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: PRINTF_INFO("GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER"); break;
			#elif defined(GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT)
			case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT: PRINTF_INFO("GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT"); break;
			#endif
			#ifdef GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER
			case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER: PRINTF_INFO("GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER"); break;
			#elif defined(GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT)
			case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT: PRINTF_INFO("GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT"); break;
			#endif
			#ifdef GL_FRAMEBUFFER_UNSUPPORTED
			case GL_FRAMEBUFFER_UNSUPPORTED: PRINTF_INFO("GL_FRAMEBUFFER_UNSUPPORTED"); break;
			#elif defined(GL_FRAMEBUFFER_UNSUPPORTED_EXT)
			case GL_FRAMEBUFFER_UNSUPPORTED_EXT: PRINTF_INFO("GL_FRAMEBUFFER_UNSUPPORTED_EXT"); break;
			#endif
			#ifdef GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE
			case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE: PRINTF_INFO("GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE"); break;
			#elif defined(GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE_EXT)
			case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE_EXT: PRINTF_INFO("GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE_EXT"); break;
			#endif
			#ifdef GL_FRAMEBUFFER_COMPLETE
			case GL_FRAMEBUFFER_COMPLETE: PRINTF_INFO("GL_FRAMEBUFFER_COMPLETE"); break;
			#elif defined(GL_FRAMEBUFFER_COMPLETE_EXT)
			case GL_FRAMEBUFFER_COMPLETE_EXT: PRINTF_INFO("GL_FRAMEBUFFER_COMPLETE_EXT"); break;
			#endif
			#ifdef GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS
			case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS: PRINTF_INFO("GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS"); break;
			#elif defined(GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS_EXT)
			case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS_EXT: PRINTF_INFO("GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS_EXT"); break;
			#endif
			default: PRINTF_INFO(" [valor-no-en-lista]"); break;
		}
		PRINTF_INFO("\n");
		r = false;
		NBASSERT(false);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
	return r;
}
#endif

//
// Texturas recicables
//

SI32 NBGestorEscena::privTexturaRenderReservarEspacioSinCrearDestino(const SI32 iEscenaPropietaria, const UI8 iBufferDatosPropietario, const MapaBitsColor colorFB, const ENTexturaModoPintado modoPintadoTextura, const UI16 anchoNecesario, const UI16 altoNecesario, NBRectanguloI* guardarAreaReservadaEn){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::privTexturaRenderReservarEspacio")
	NBASSERT(iEscenaPropietaria >= 0 && iEscenaPropietaria<NBGESTORESCENA_MAX_ESCENAS)
	NBASSERT(iBufferDatosPropietario<NBGESTORESCENA_CANTIDAD_BUFFERES_DATOS)
	NBASSERT(_escenas[iEscenaPropietaria].registroOcupado)
	NBASSERT(colorFB == COLOR_RGBA8)
	NBASSERT(anchoNecesario>0)
	NBASSERT(altoNecesario>0)
	SI32 iTexResultado = -1;
	SI32 iTex, iTexLibre = -1;
	//-----------------
	//- Buscar una que pueda reutilizarse
	//-----------------
	for(iTex=0; iTex<NBGESTORESCENA_MAX_TEXTURAS_RENDER; iTex++){
		STGestorEscenaTextura* datosTex = &(_texturasParaRenderizadoGL[iTex]);
		if(!datosTex->registroOcupado) iTexLibre	= iTex;
		if(datosTex->registroOcupado && !datosTex->texturaOcupada && datosTex->iEscenaPropietaria == iEscenaPropietaria && datosTex->colorFrameBufferGl == colorFB){
			if(datosTex->anchoFrameBufferGl >= anchoNecesario && datosTex->altoFrameBufferGl >= altoNecesario){
				datosTex->texturaOcupada			= true;
				datosTex->iBufferDatosPropietario	= iBufferDatosPropietario;
				datosTex->texturaModoPintado		= modoPintadoTextura;
				guardarAreaReservadaEn->x			= 0;
				guardarAreaReservadaEn->y			= 0;
				guardarAreaReservadaEn->ancho		= anchoNecesario;
				guardarAreaReservadaEn->alto		= altoNecesario;
				iTexResultado						= iTex;
				break;
			}
		}
	}
	//--------------------
	//- Reservar una nueva
	//--------------------
	if(iTexResultado == -1 && iTexLibre != -1){
		UI16 mayorLado						= anchoNecesario>altoNecesario?anchoNecesario:altoNecesario;
		UI16 ladoBase2						= 64; while(ladoBase2<mayorLado) ladoBase2 *= 2;
		//if(ladoBase2<512)		ladoBase2 = 512;
		//
		STGestorEscenaTextura* datosTex		= &(_texturasParaRenderizadoGL[iTexLibre]); NBASSERT(!datosTex->registroOcupado)
		datosTex->registroOcupado			= true;
		datosTex->texturaOcupada			= true;
		datosTex->texturaModoPintado		= modoPintadoTextura;
		datosTex->iEscenaPropietaria		= iEscenaPropietaria;
		datosTex->iBufferDatosPropietario	= iBufferDatosPropietario;
		datosTex->idFrameBufferGlPropio		= 0;		//Pendeiente de crear el FB propio si "requiereFBPropio" (no se crear aqui porque este hilo podria no tener acceso al OpenGL). 
		datosTex->anchoFrameBufferGl		= ladoBase2;
		datosTex->altoFrameBufferGl			= ladoBase2;
		datosTex->colorFrameBufferGl		= colorFB;
		datosTex->objTexturaAtlas			= NULL;		//Pendiente de crear la textura destino (no se crear aqui porque este hilo podria no tener acceso al OpenGL).
		guardarAreaReservadaEn->x			= 0;
		guardarAreaReservadaEn->y			= 0;
		guardarAreaReservadaEn->ancho		= anchoNecesario;
		guardarAreaReservadaEn->alto		= altoNecesario;
		iTexResultado						= iTexLibre;
	}
	#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
	if(iTexResultado == -1){
		PRINTF_ERROR("No se pudo reservar textura para iluminacion(%d, %d) iTexResultado(%d) iTexLibre(%d)\n", anchoNecesario, altoNecesario, iTexResultado, iTexLibre);
	}
	#endif
	NBASSERT(iTexResultado != -1)
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
	return iTexResultado;
}

void NBGestorEscena::privTexturaRenderAsegurarDestinosCreados(const SI32 iEscenaPropietaria){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::privTexturaRenderAsegurarDestinosCreados")
	NBASSERT(iEscenaPropietaria >= 0 && iEscenaPropietaria<NBGESTORESCENA_MAX_ESCENAS)
	NBASSERT(_indiceBufferDatosLeer < NBGESTORESCENA_CANTIDAD_BUFFERES_DATOS)
	NBASSERT(_escenas[iEscenaPropietaria].registroOcupado)
	SI32 iTex;
	for(iTex=0; iTex<NBGESTORESCENA_MAX_TEXTURAS_RENDER; iTex++){
		STGestorEscenaTextura* datosTex = &(_texturasParaRenderizadoGL[iTex]);
		if(datosTex->registroOcupado && datosTex->texturaOcupada && datosTex->iEscenaPropietaria == iEscenaPropietaria && datosTex->iBufferDatosPropietario == _indiceBufferDatosLeer){
			if(datosTex->objTexturaAtlas == NULL){
				AUTextura* objTextura = NBGestorTexturas::texturaDesdeAreaVacia(datosTex->anchoFrameBufferGl, datosTex->altoFrameBufferGl, datosTex->colorFrameBufferGl, ENTexturaTipoAlmacenamientoGL_AtlasUnico, ENTExturaTipoUsoGL_EscrituraLectura, (ENTexturaModoPintado)datosTex->texturaModoPintado, ENTexturaMipMap_Inhabilitado, ENTexturaOrdenV_HaciaAbajo, 1, 1.0f); NB_DEFINE_NOMBRE_PUNTERO(objTextura, "NBGestorEscena::texturaRecicable");
				if(objTextura != NULL){
					if(datosTex->idFrameBufferGlPropio == 0){
						GLuint idFB = 0;
						NBGestorGL::genFramebuffersEXT(1, &idFB); NBASSERT(idFB != 0)
						NBGestorGL::bindFramebufferEXT(GL_FRAMEBUFFER_EXT, idFB);
						NBGestorGL::framebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, objTextura->idTexturaGL, 0);
						#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT_GL_FRAMEBUFFERS_COMPLETOS
						NBGestorEscena::privFrameBufferCheckStatusEXT();
						#endif
						datosTex->idFrameBufferGlPropio = idFB;
					}
					datosTex->objTexturaAtlas	= objTextura; objTextura->retener(NB_RETENEDOR_NULL);
				}
				NBASSERT(objTextura != NULL)
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

void NBGestorEscena::privTexturaRenderVaciarReservasDeBuffer(const UI8 iBufferDatosPropietario){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::privTexturaRenderVaciarReservasDeBuffer")
	//Cuando se liberan las render-to-texture asociadas a un bufferDatos, es para reutilizarlas en otro.
	NBASSERT(iBufferDatosPropietario<NBGESTORESCENA_CANTIDAD_BUFFERES_DATOS)
	SI32 iTex;
	for(iTex=0; iTex<NBGESTORESCENA_MAX_TEXTURAS_RENDER; iTex++){
		STGestorEscenaTextura* datosTex = &(_texturasParaRenderizadoGL[iTex]);
		if(datosTex->registroOcupado && datosTex->texturaOcupada && datosTex->iBufferDatosPropietario == iBufferDatosPropietario){
			NBASSERT(_escenas[datosTex->iEscenaPropietaria].registroOcupado)
			datosTex->texturaOcupada = false;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

void NBGestorEscena::privTexturaRenderLiberarReservasDeEscena(const SI32 iEscenaPropietaria){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::privTexturaRenderLiberarReservasDeEscena")
	//Cuando se liberan las render-to-texture asociadas a una escena, es porque la escena esta siendo destruida.
	NBASSERT(iEscenaPropietaria >= 0 && iEscenaPropietaria<NBGESTORESCENA_MAX_ESCENAS)
	NBASSERT(_escenas[iEscenaPropietaria].registroOcupado)
	SI32 iTex;
	for(iTex=0; iTex<NBGESTORESCENA_MAX_TEXTURAS_RENDER; iTex++){
		STGestorEscenaTextura* datosTextura = &(_texturasParaRenderizadoGL[iTex]);
		if(datosTextura->registroOcupado && datosTextura->iEscenaPropietaria == iEscenaPropietaria){
			NBASSERT(datosTextura->objTexturaAtlas != NULL)
			if(datosTextura->objTexturaAtlas != NULL){
				NBGestorTexturas::liberarEspacioTexturaSiSoloTieneUnaReferenciaExterna(datosTextura->objTexturaAtlas);
				datosTextura->objTexturaAtlas->liberar(NB_RETENEDOR_NULL);
				datosTextura->objTexturaAtlas = NULL;
			}
			GLuint idFB = datosTextura->idFrameBufferGlPropio;
			if(idFB != 0) NBGestorGL::deleteFramebuffersEXT(1, &idFB); datosTextura->idFrameBufferGlPropio = 0;
			datosTextura->iEscenaPropietaria		= -1;
			datosTextura->iBufferDatosPropietario	= 0;
			datosTextura->registroOcupado			= false;
			datosTextura->texturaOcupada			= false;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

void NBGestorEscena::privTexturaRenderLiberarReservasTodas(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::privTexturaRenderLiberarReservasTodas")
	SI32 iTex;
	for(iTex=0; iTex<NBGESTORESCENA_MAX_TEXTURAS_RENDER; iTex++){
		STGestorEscenaTextura* datosTextura = &(_texturasParaRenderizadoGL[iTex]);
		if(datosTextura->registroOcupado){
			NBASSERT(datosTextura->objTexturaAtlas != NULL)
			if(datosTextura->objTexturaAtlas != NULL){
				NBGestorTexturas::liberarEspacioTexturaSiSoloTieneUnaReferenciaExterna(datosTextura->objTexturaAtlas);
				datosTextura->objTexturaAtlas->liberar(NB_RETENEDOR_NULL);
				datosTextura->objTexturaAtlas = NULL;
			}
			GLuint idFB = datosTextura->idFrameBufferGlPropio;
			if(idFB != 0) NBGestorGL::deleteFramebuffersEXT(1, &idFB); datosTextura->idFrameBufferGlPropio = 0;
			datosTextura->iEscenaPropietaria		= -1;
			datosTextura->iBufferDatosPropietario	= 0;
			datosTextura->registroOcupado			= false;
			datosTextura->texturaOcupada			= false;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

//
bool NBGestorEscena::bufferDatosEnLecturaMoverHaciaSiguienteNoBloqueado(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::bufferDatosEnLecturaMoverHaciaSiguienteNoBloqueado")
	bool exito = false;
	SI32 indiceLecturaSiguiente = _indiceBufferDatosLeer + 1; if(indiceLecturaSiguiente == NBGESTORESCENA_CANTIDAD_BUFFERES_DATOS) indiceLecturaSiguiente = 0;
	NBASSERT(indiceLecturaSiguiente < NBGESTORESCENA_CANTIDAD_BUFFERES_DATOS)
	if(_bufferDatosEstado[indiceLecturaSiguiente] != ENGestorEscenaBufferEstado_Vacio && !_bufferDatosBloqueado[indiceLecturaSiguiente] /*&& indiceLecturaSiguiente != _indiceBufferDatosEscribir*/){
		_indiceBufferDatosLeer = indiceLecturaSiguiente;
		exito = true;
	} else {
		//PRINTF_WARNING("no se pudo mover a siguiente buffer porque estado(%d == Estado_Vacio) || _bufferDatosBloqueado(%d == true).\n", _bufferDatosEstado[indiceLecturaSiguiente], (SI32)_bufferDatosBloqueado[indiceLecturaSiguiente]);
		//NBASSERT(false);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
	return exito;
}

void NBGestorEscena::bufferDatosEnLecturaMoverHaciaSiguiente(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::bufferDatosEnLecturaMoverHaciaSiguiente")
	_indiceBufferDatosLeer++;
	if(_indiceBufferDatosLeer == NBGESTORESCENA_CANTIDAD_BUFFERES_DATOS){
		_indiceBufferDatosLeer = 0; NBASSERT(_indiceBufferDatosLeer < NBGESTORESCENA_CANTIDAD_BUFFERES_DATOS)
	}
	NBASSERT(false) //PENDIENTE, temporalmente no usar
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

bool NBGestorEscena::bufferDatosEnEscrituraMoverHaciaSiguienteNoBloqueado(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::bufferDatosEnEscrituraMoverHaciaSiguientenoBloqueado")
	bool exito = false;
	SI32 indiceEscrituraSiguiente = _indiceBufferDatosEscribir+1; if(indiceEscrituraSiguiente == NBGESTORESCENA_CANTIDAD_BUFFERES_DATOS) indiceEscrituraSiguiente = 0;
	NBASSERT(indiceEscrituraSiguiente < NBGESTORESCENA_CANTIDAD_BUFFERES_DATOS)
	if(_bufferDatosEstado[indiceEscrituraSiguiente] == ENGestorEscenaBufferEstado_Vacio && !_bufferDatosBloqueado[indiceEscrituraSiguiente] /*&& indiceEscrituraSiguiente != _indiceBufferDatosLeer*/){
		_indiceBufferDatosEscribir = indiceEscrituraSiguiente;
		exito = true;
	} else {
		//PRINTF_WARNING("no se pudo mover a siguiente buffer porque estado(%d == Estado_Vacio) || _bufferDatosBloqueado(%d == true).\n", _bufferDatosEstado[indiceEscrituraSiguiente], (SI32)_bufferDatosBloqueado[indiceEscrituraSiguiente]);
		//NBASSERT(false);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
	return exito;
}

void NBGestorEscena::bufferDatosEnEscrituraMoverHaciaSiguiente(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::bufferDatosEnEscrituraMoverHaciaSiguiente")
	_indiceBufferDatosEscribir++;
	if(_indiceBufferDatosEscribir == NBGESTORESCENA_CANTIDAD_BUFFERES_DATOS){
		_indiceBufferDatosEscribir = 0; NBASSERT(_indiceBufferDatosEscribir < NBGESTORESCENA_CANTIDAD_BUFFERES_DATOS)
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

bool NBGestorEscena::bufferDatosEnEscrituraBloqueado(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::bufferDatosEnEscrituraBloqueado")
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
	return _bufferDatosBloqueado[_indiceBufferDatosEscribir];
}

bool NBGestorEscena::bufferDatosEnEscrituraBloquear(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::bufferDatosEnEscrituraBloquear")
	bool exito = false;
	NBASSERT(!_bufferDatosBloqueado[_indiceBufferDatosEscribir]); //No debe estar bloqueado
	_bufferDatosBloqueado[_indiceBufferDatosEscribir] = true;
	//Establecer punteros (para asegurar la integridad de produccion de modelos)
	verticesGL0	= (NBVerticeGL0*)_buffersVertices[_indiceBufferDatosEscribir][ENVerticeGlTipo_SinTextura].arregloVertices;
	verticesGL1	= (NBVerticeGL* )_buffersVertices[_indiceBufferDatosEscribir][ENVerticeGlTipo_MonoTextura].arregloVertices;
	verticesGL2	= (NBVerticeGL2*)_buffersVertices[_indiceBufferDatosEscribir][ENVerticeGlTipo_BiTextura].arregloVertices;
	verticesGL3	= (NBVerticeGL3*)_buffersVertices[_indiceBufferDatosEscribir][ENVerticeGlTipo_TriTextura].arregloVertices;
#	ifdef CONFIG_NB_GESTOR_ESCENAS_MODELOS_MEDIANTE_INDICES
	indicesGL0	= _buffersVertices[_indiceBufferDatosEscribir][ENVerticeGlTipo_SinTextura].arregloIndices;
	indicesGL1	= _buffersVertices[_indiceBufferDatosEscribir][ENVerticeGlTipo_MonoTextura].arregloIndices;
	indicesGL2	= _buffersVertices[_indiceBufferDatosEscribir][ENVerticeGlTipo_BiTextura].arregloIndices;
	indicesGL3	= _buffersVertices[_indiceBufferDatosEscribir][ENVerticeGlTipo_TriTextura].arregloIndices;
#	endif
	//
	exito = true;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
	return exito;
}

bool NBGestorEscena::bufferDatosEnEscrituraDesbloquear(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::bufferDatosEnEscrituraDesbloquear")
	bool exito = false;
	NBASSERT(_bufferDatosBloqueado[_indiceBufferDatosEscribir]); //Debe estar bloqueado
	//Anular punteros punteros (para asegurar la integridad de produccion de modelos)
	verticesGL0	= verticesGL1 = verticesGL2 = verticesGL3 = NULL;
#	ifdef CONFIG_NB_GESTOR_ESCENAS_MODELOS_MEDIANTE_INDICES
	indicesGL0	= indicesGL1 = indicesGL2 = indicesGL3 = NULL;
#	endif
	//
	_bufferDatosBloqueado[_indiceBufferDatosEscribir] = false;
	//
	exito = true;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
	return exito;
}

bool NBGestorEscena::bufferDatosEnLecturaBloqueado(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::bufferDatosEnLecturaBloqueado")
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
	return _bufferDatosBloqueado[_indiceBufferDatosLeer];
}

bool NBGestorEscena::bufferDatosEnLecturaBloquear(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::bufferDatosEnLecturaBloquear")
	bool exito = false;
	NBASSERT(!_bufferDatosBloqueado[_indiceBufferDatosLeer]); //No debe estar bloqueado
	_bufferDatosBloqueado[_indiceBufferDatosLeer] = true;
	exito = true;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
	return exito;
}

bool NBGestorEscena::bufferDatosEnLecturaDesbloquear(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::bufferDatosEnLecturaDesbloquear")
	bool exito = false;
	NBASSERT(_bufferDatosBloqueado[_indiceBufferDatosLeer]); //Debe estar bloqueado
	_bufferDatosBloqueado[_indiceBufferDatosLeer] = false;
	exito = true;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
	return exito;
}

bool NBGestorEscena::bufferDatosEnEscrituraLlenar(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::bufferDatosEnEscrituraLlenar")
	bool exito = false;
	NBASSERT(_bufferDatosBloqueado[_indiceBufferDatosEscribir]); //Debe estar bloqueado
	_bufferDatosEstado[_indiceBufferDatosEscribir] = ENGestorEscenaBufferEstado_Lleno;
	exito = true;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
	return exito;
}

bool NBGestorEscena::bufferDatosEnEscrituraDescartar(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::bufferDatosEnEscrituraDescartar")
	bool exito = false;
	NBASSERT(_bufferDatosBloqueado[_indiceBufferDatosEscribir]); //Debe estar bloqueado
	_bufferDatosEstado[_indiceBufferDatosEscribir] = ENGestorEscenaBufferEstado_Discarted;
	exito = true;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
	return exito;
}

//


ENGestorEscenaBufferEstado NBGestorEscena::bufferDatosEnLecturaEstado(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::bufferDatosEnLecturaEstado")
	NBASSERT(_bufferDatosBloqueado[_indiceBufferDatosLeer]); //Debe estar bloqueado
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
	return _bufferDatosEstado[_indiceBufferDatosLeer];
}

bool NBGestorEscena::bufferDatosEnEscrituraVaciar(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::bufferDatosEnEscrituraVaciar")
	bool exito = NBGestorEscena::privBufferDatosVaciar(_indiceBufferDatosEscribir);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
	return exito;
}

bool NBGestorEscena::bufferDatosEnLecturaVaciar(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::bufferDatosEnLecturaVaciar")
	bool exito = NBGestorEscena::privBufferDatosVaciar(_indiceBufferDatosLeer);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
	return exito;
}

bool NBGestorEscena::privBufferDatosVaciar(const UI16 iBuffer){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::bufferDatosEnLecturaVaciar")
	bool exito	= false;
	NBASSERT(_bufferDatosBloqueado[iBuffer]); //Debe estar bloqueado
	_bufferDatosEstado[iBuffer] = ENGestorEscenaBufferEstado_Vacio;
	//Liberar los recursos retenidos por el buffer datos
	SI32 iEscena; for(iEscena = 0; iEscena < NBGESTORESCENA_MAX_ESCENAS; iEscena++){
		STGestorEscenaEscena* scn = &_escenas[iEscena];
		NBASSERT((scn->registroOcupado && scn->agrupadoresParticulas != NULL) || (!scn->registroOcupado && scn->agrupadoresParticulas == NULL))
		NBASSERT((scn->registroOcupado && scn->escuchadoresCambioPuertoVision != NULL && scn->capasEnlazadas != NULL) || (!scn->registroOcupado && scn->escuchadoresCambioPuertoVision == NULL && scn->capasEnlazadas == NULL))
		if(scn->registroOcupado){
			NBASSERT(_bufferDatosBloqueado[iBuffer])
			NBASSERT(scn->renderCapas[iBuffer] != NULL)
			NBASSERT(scn->renderCapasConsumir[iBuffer] != NULL)
			NBASSERT(scn->renderCapasProducir[iBuffer] != NULL)
			scn->renderCapasConsumir[iBuffer]->vaciar();
			scn->renderCapasProducir[iBuffer]->vaciar();
			AUArregloNativoP<STGestorEscenaCapaRender>*	renderCapas = scn->renderCapas[iBuffer];
			SI32 iCapa, iCapaConteo = renderCapas->conteo;
			for(iCapa=0; iCapa<iCapaConteo; iCapa++){
				STGestorEscenaCapaRender* capaRender = &(renderCapas->elemento[iCapa]);
				capaRender->renderModelosDatos->vaciar();
				capaRender->renderModelos->vaciar();
				capaRender->renderPiscinaObjetosRetener->vaciar();
				capaRender->renderIluminacion->vaciar();
				capaRender->renderEspejos->vaciar();
				capaRender->renderProductoresSombras->vaciar();
				capaRender->renderDescriptoresCuerdas->vaciar();
				capaRender->renderConsumidoresLuces->vaciar();
				capaRender->renderConsumidoresEspejos->vaciar();
				capaRender->renderConsumidoresCuerdas->vaciar();
				capaRender->renderConsumidoresPreModeloGL->vaciar();
				UI16 iSubElem;
				for(iSubElem=0; iSubElem<capaRender->renderSombrasPorIluminacion->conteo; iSubElem++){ ((AUArregloNativoMutableP<NBFuenteSombra>*)capaRender->renderSombrasPorIluminacion->elemento[iSubElem])->vaciar(); }
				for(iSubElem=0; iSubElem<capaRender->renderSombrasRangosPorIluminacion->conteo; iSubElem++){ ((AUArregloNativoMutableP<STRangoSombra>*)capaRender->renderSombrasRangosPorIluminacion->elemento[iSubElem])->vaciar(); }
				//capaRender->renderSombrasPorIluminacion->vaciar(); //Mejor vaciar los subarreglos, porque vaciar el arreglo principal obligaria a volver a crear los subarreglos en el siguiente tick
				//capaRender->renderSombrasRangosPorIluminacion->vaciar(); //Mejor vaciar los subarreglos, porque vaciar el arreglo principal obligaria a volver a crear los subarreglos en el siguiente tick
				capaRender->renderSombrasVerticesIntermedios->vaciar();
				//
				capaRender->renderConteoLucesAfectanMascara = 0;
				capaRender->renderMascLuzSumaFigurasTotal	= 0;
				capaRender->renderMascLuz->vaciar();
				capaRender->renderMascLuzBloquesGL->vaciar();
				capaRender->renderMascLuzLucesConIntensidad->vaciar();
				STGestorEscenaTexturaLuz::inicializar(&capaRender->renderMascLucesCombinadas);
			}
		}
	}
	//Liberar las texturas asociadas al bufferDatos
	NBGestorEscena::privTexturaRenderVaciarReservasDeBuffer(iBuffer);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
	return exito;
}

//Vertices GL
void NBGestorEscena::resetearVerticesGL(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorEscena::resetearVerticesGL")
	NBASSERT(_gestorInicializado);
	NBASSERT(_bufferDatosEstado[_indiceBufferDatosEscribir] != ENGestorEscenaBufferEstado_Lleno)		//No debe estar lleno
	NBASSERT(_bufferDatosBloqueado[_indiceBufferDatosEscribir])		//Debe estar bloqueado
	////NBGESTORGL_LOTES_TODOS_ASSERT_ESTAN_VACIOS						//Si falla, se omitió flushear los comandos acumulados
	//
	SI32 i;
	for(i = 0; i < ENVerticeGlTipo_Conteo; i++){
		STBufferVerticesGL* datBuff = &(_buffersVertices[_indiceBufferDatosEscribir][i]);
		datBuff->usoArregloVertices	= 1; /*el primer elemento esta reservado*/
		NBASSERT(((datBuff->bytesPorRegistro * datBuff->usoArregloVertices) % 4) == 0)
#		ifdef CONFIG_NB_GESTOR_ESCENAS_MODELOS_MEDIANTE_INDICES
		datBuff->usoArregloIndices	= 2; /*los primeros dos elementos estan reservados (multiplo de 4 bytes)*/
		NBASSERT(((sizeof(GLushort) * datBuff->usoArregloIndices) % 4) == 0)
#		endif
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
}

UI32 NBGestorEscena::reservarVerticesGL(const ENVerticeGlTipo tipo, const UI32 cantidadVerticesReservar){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorEscena::reservarVerticesGL")
	NBASSERT(_gestorInicializado);
	NBASSERT(tipo < ENVerticeGlTipo_Conteo);
	NBASSERT(_bufferDatosBloqueado[_indiceBufferDatosEscribir])	//Debe estar bloqueado
	NBASSERT(_bufferDatosEstado[_indiceBufferDatosEscribir] != ENGestorEscenaBufferEstado_Lleno)	//No debe estar lleno
	STBufferVerticesGL* ptrDat = &(_buffersVertices[_indiceBufferDatosEscribir][tipo]);
	NBASSERT(((ptrDat->bytesPorRegistro * cantidadVerticesReservar) % 4) == 0) //Tamano multiplo de 4 bytes
	UI32 indice = 0;
	//Redimensionar arreglo si es necesario
	if((ptrDat->usoArregloVertices + cantidadVerticesReservar) > ptrDat->tamanoArregloVertices){
		const SI32 incremento	= (cantidadVerticesReservar > NB_GESTOR_GL_CRECIMIENTO_BLOQUE_VERTICES_GL ? cantidadVerticesReservar : NB_GESTOR_GL_CRECIMIENTO_BLOQUE_VERTICES_GL); NBASSERT((ptrDat->usoArregloVertices + cantidadVerticesReservar) < (ptrDat->tamanoArregloVertices + incremento))
		SI32 tamanoAnterior		= ptrDat->tamanoArregloVertices;
		SI32 tamanoNuevo		= tamanoAnterior + incremento;
		ptrDat->arregloVertices	= (BYTE*)NBGestorMemoria::redimensionarBloqueMemoria(ptrDat->bytesPorRegistro * tamanoNuevo, ptrDat->arregloVertices, ptrDat->bytesPorRegistro * tamanoAnterior); NB_DEFINE_NOMBRE_PUNTERO(ptrDat->arregloVertices, "NBGestorGL::arregloVerticesGL")
		ptrDat->tamanoArregloVertices	= tamanoNuevo;
		//Actualizar punteros
		if(_bufferDatosBloqueado[_indiceBufferDatosEscribir]){
			switch (tipo) {
				case ENVerticeGlTipo_SinTextura:	verticesGL0	= (NBVerticeGL0*)ptrDat->arregloVertices; break;
				case ENVerticeGlTipo_MonoTextura:	verticesGL1	= (NBVerticeGL*)ptrDat->arregloVertices; break;
				case ENVerticeGlTipo_BiTextura:		verticesGL2	= (NBVerticeGL2*)ptrDat->arregloVertices; break;
				case ENVerticeGlTipo_TriTextura:	verticesGL3	= (NBVerticeGL3*)ptrDat->arregloVertices; break;
				default: NBASSERT(false) break;
			}
		}
		PRINTF_INFO("ARREGLO DE VERTICES_GL_%d_%d REDIMENSIONADO A: %d elementos.\n", _indiceBufferDatosEscribir, tipo, ptrDat->tamanoArregloVertices);
	}
	//Valor a retornar
	indice = ptrDat->usoArregloVertices; NBASSERT(indice > 0); //El primer vertice esta reservado
	ptrDat->usoArregloVertices += cantidadVerticesReservar; NBASSERT(ptrDat->usoArregloVertices <= ptrDat->tamanoArregloVertices)
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
	return indice;
}

bool NBGestorEscena::punteroVerticeGlEsValido(const ENVerticeGlTipo tipo, const void* puntero){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorEscena::punteroVerticeGlEsValido")
	NBASSERT(tipo < ENVerticeGlTipo_Conteo);
	bool r = false;
	STBufferVerticesGL* ptrDat = &(_buffersVertices[_indiceBufferDatosEscribir][tipo]);
	const void* punteroMin = ptrDat->arregloVertices;
	if(punteroMin != NULL){
		const void* punteroSigMaxUso = (BYTE*)ptrDat->arregloVertices + (ptrDat->usoArregloVertices * ptrDat->bytesPorRegistro);
		//const void* punteroSigMaxExst = (BYTE*)ptrDat->arregloVertices + (ptrDat->tamanoArregloVertices * ptrDat->bytesPorRegistro);
		//PRINTF_INFO("Puntero min(%ld) act(%ld) maxUso(%ld) maxExist(%ld).\n", (long)punteroMin, (long)puntero, (long)punteroSigMaxUso, (long)punteroSigMaxExst);
		r = (puntero >= punteroMin && puntero < punteroSigMaxUso); NBASSERT(r);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
	return r;
}

#ifdef CONFIG_NB_GESTOR_ESCENAS_MODELOS_MEDIANTE_INDICES
UI32 NBGestorEscena::reservarIndicesGL(const ENVerticeGlTipo tipo, const UI32 cantidadIndicesReservar){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorEscena::reservarIndicesGL")
	NBASSERT(_gestorInicializado);
	NBASSERT(tipo < ENVerticeGlTipo_Conteo);
	NBASSERT(_bufferDatosBloqueado[_indiceBufferDatosEscribir])	//Debe estar bloqueado
	NBASSERT(_bufferDatosEstado[_indiceBufferDatosEscribir] != ENGestorEscenaBufferEstado_Lleno)	//No debe estar lleno bloqueado
	NBASSERT(((sizeof(GLushort) * cantidadIndicesReservar) % 4) == 0)	//Tamano multiplo de 4 bytes
	UI32 indice = 0;
	STBufferVerticesGL* ptrDat = &(_buffersVertices[_indiceBufferDatosEscribir][tipo]);
	//Redimensionar arreglo si es necesario
	if((ptrDat->usoArregloIndices + cantidadIndicesReservar) > ptrDat->tamanoArregloIndices){
		SI32 incremento				= (cantidadIndicesReservar > NB_GESTOR_GL_CRECIMIENTO_BLOQUE_INDICES_GL?cantidadIndicesReservar:NB_GESTOR_GL_CRECIMIENTO_BLOQUE_INDICES_GL);
		SI32 tamanoAnterior			= ptrDat->tamanoArregloIndices;
		SI32 tamanoNuevo			= tamanoAnterior + incremento;
		ptrDat->arregloIndices		= (GLushort*)NBGestorMemoria::redimensionarBloqueMemoria(sizeof(GLushort) * tamanoNuevo, ptrDat->arregloIndices, sizeof(GLushort) * tamanoAnterior);
		ptrDat->tamanoArregloIndices	= tamanoNuevo;
		/*Actualizar caches optimizadoras*/
		if(_bufferDatosBloqueado[_indiceBufferDatosEscribir]){
			switch (tipo) {
				case ENVerticeGlTipo_SinTextura: indicesGL0 = ptrDat->arregloIndices; break;
				case ENVerticeGlTipo_MonoTextura:indicesGL1 = ptrDat->arregloIndices; break;
				case ENVerticeGlTipo_BiTextura:  indicesGL2 = ptrDat->arregloIndices; break;
				case ENVerticeGlTipo_TriTextura: indicesGL3 = ptrDat->arregloIndices; break;
				default: NBASSERT(false) break;
			}
		}
		PRINTF_INFO("ARREGLO DE INDICES_GL_%d_%d REDIMENSIONADO A: %d\n", _indiceBufferDatosEscribir, tipo, ptrDat->tamanoArregloIndices);
	}
	//Valor a retornar
	indice = ptrDat->usoArregloIndices; NBASSERT(indice > 1); //Los primeros dos indices estan reservados
	ptrDat->usoArregloIndices += cantidadIndicesReservar;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
	return indice;
}
#endif

#ifdef CONFIG_NB_GESTOR_ESCENAS_MODELOS_MEDIANTE_INDICES
STBloqueGL NBGestorEscena::reservarIndicesParaTriangStrip4Independizado(const ENVerticeGlTipo tipo, UI16 iPrimerVertice){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorEscena::reservarIndicesParaTriangStrip4Independizado")
	NBASSERT(_gestorInicializado);
	NBASSERT(tipo < ENVerticeGlTipo_Conteo);
	STBloqueGL bloqueIndGL;
	bloqueIndGL.cantidadElementos	= 6;
	bloqueIndGL.primerElemento		= NBGestorEscena::reservarIndicesGL(tipo, 6);
	GLushort* indicesGL				= &(_buffersVertices[_indiceBufferDatosEscribir][tipo].arregloIndices[bloqueIndGL.primerElemento]);
	indicesGL[0]					= iPrimerVertice;	//Se repite el primero
	indicesGL[1]					= iPrimerVertice++;
	indicesGL[2]					= iPrimerVertice++;
	indicesGL[3]					= iPrimerVertice++;
	indicesGL[4]					= iPrimerVertice;
	indicesGL[5]					= iPrimerVertice++;	//Se repite el ultimo
	NBASSERT(bloqueIndGL.primerElemento > 1) //Los primeros dos elementos estan reservados
	NBASSERT(bloqueIndGL.cantidadElementos > 0 && (bloqueIndGL.cantidadElementos % 6) == 0)
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
	return bloqueIndGL;
}
#endif

#ifdef CONFIG_NB_GESTOR_ESCENAS_MODELOS_MEDIANTE_INDICES
STBloqueGL NBGestorEscena::reservarIndicesParaTriangStrip4IndependizadoMultiples(const ENVerticeGlTipo tipo, UI16 iPrimerVertice, UI16 cantTriangStrips4){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorEscena::reservarIndicesParaTriangStrip4IndependizadoMultiples")
	NBASSERT(_gestorInicializado);
	NBASSERT(tipo < ENVerticeGlTipo_Conteo);
	NBASSERT(cantTriangStrips4 > 0)
	STBloqueGL bloqueIndGL;
	bloqueIndGL.cantidadElementos		= cantTriangStrips4 * 6;
	bloqueIndGL.primerElemento			= NBGestorEscena::reservarIndicesGL(tipo, bloqueIndGL.cantidadElementos);
	GLushort* indicesGL					= &(_buffersVertices[_indiceBufferDatosEscribir][tipo].arregloIndices[bloqueIndGL.primerElemento]);
	GLushort* indicesGLSigUlt			= indicesGL + bloqueIndGL.cantidadElementos;
	while(indicesGL < indicesGLSigUlt){
		indicesGL[0]			= iPrimerVertice;	//Se repite el primero
		indicesGL[1]			= iPrimerVertice++;
		indicesGL[2]			= iPrimerVertice++;
		indicesGL[3]			= iPrimerVertice++;
		indicesGL[4]			= iPrimerVertice;
		indicesGL[5]			= iPrimerVertice++;	//Se repite el ultimo
		indicesGL				+= 6;
	}
	NBASSERT(indicesGL == &(_buffersVertices[_indiceBufferDatosEscribir][tipo].arregloIndices[bloqueIndGL.primerElemento + bloqueIndGL.cantidadElementos]))
	NBASSERT(bloqueIndGL.primerElemento > 1) //Los primeros dos elementos estan reservados
	NBASSERT(bloqueIndGL.cantidadElementos > 0 && (bloqueIndGL.cantidadElementos % 6) == 0)
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
	return bloqueIndGL;
}
#endif

#ifdef CONFIG_NB_GESTOR_ESCENAS_MODELOS_MEDIANTE_INDICES
STBloqueGL NBGestorEscena::reservarIndicesParaTriangStripIndependizado(const ENVerticeGlTipo tipo, UI16 iPrimerVertice, UI16 verticesPorTriangStrip){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorEscena::reservarIndicesParaTriangStripIndependizado")
	NBASSERT(_gestorInicializado);
	NBASSERT(tipo < ENVerticeGlTipo_Conteo);
	NBASSERT(verticesPorTriangStrip > 2)
	STBloqueGL bloqueIndGL;
	bloqueIndGL.cantidadElementos		= (verticesPorTriangStrip + 2);
	bloqueIndGL.primerElemento			= NBGestorEscena::reservarIndicesGL(tipo, bloqueIndGL.cantidadElementos);
	GLushort* indicesGL					= &(_buffersVertices[_indiceBufferDatosEscribir][tipo].arregloIndices[bloqueIndGL.primerElemento]);
	GLushort* indiceGLSigUlt			= indicesGL + verticesPorTriangStrip;
	*indicesGL						= iPrimerVertice; indicesGL++;		//El primero se repite
	while(indicesGL < indiceGLSigUlt){
		*indicesGL					= iPrimerVertice++; indicesGL++;
	}
	*indicesGL						= iPrimerVertice; indicesGL++;		//El ultimo se repite
	*indicesGL						= iPrimerVertice++; indicesGL++;	//El ultimo se repite
	NBASSERT(indicesGL == &(_buffersVertices[_indiceBufferDatosEscribir][tipo].arregloIndices[bloqueIndGL.primerElemento + bloqueIndGL.cantidadElementos]))
	NBASSERT(bloqueIndGL.primerElemento > 1) //Los primeros dos elementos estan reservados
	NBASSERT(bloqueIndGL.cantidadElementos > 0)
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
	return bloqueIndGL;
}
#endif

#ifdef CONFIG_NB_GESTOR_ESCENAS_MODELOS_MEDIANTE_INDICES
STBloqueGL NBGestorEscena::reservarIndicesParaTriangStripIndependizadoMultiples(const ENVerticeGlTipo tipo, UI16 iPrimerVertice, UI16 verticesPorTriangStrip, UI16 cantTriangStrips){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorEscena::reservarIndicesParaTriangStripIndependizadoMultiples")
	NBASSERT(_gestorInicializado);
	NBASSERT(tipo < ENVerticeGlTipo_Conteo);
	NBASSERT(verticesPorTriangStrip > 2)
	NBASSERT(cantTriangStrips > 0)
	STBloqueGL bloqueIndGL;
	bloqueIndGL.cantidadElementos		= (verticesPorTriangStrip + 2) * cantTriangStrips;
	bloqueIndGL.primerElemento			= NBGestorEscena::reservarIndicesGL(tipo, bloqueIndGL.cantidadElementos);
	GLushort* indicesGL					= &(_buffersVertices[_indiceBufferDatosEscribir][tipo].arregloIndices[bloqueIndGL.primerElemento]);
	UI16 iTriangStrip; GLushort* indiceGLSigUlt;
	for(iTriangStrip=0; iTriangStrip < cantTriangStrips; iTriangStrip++){
		indiceGLSigUlt					= indicesGL + verticesPorTriangStrip;
		*indicesGL						= iPrimerVertice; indicesGL++;		//El primero se repite
		while(indicesGL < indiceGLSigUlt){
			*indicesGL					= iPrimerVertice++; indicesGL++;
		}
		*indicesGL						= iPrimerVertice; indicesGL++;		//El ultimo se repite
		*indicesGL						= iPrimerVertice++; indicesGL++;	//El ultimo se repite
	}
	NBASSERT(indicesGL == &(_buffersVertices[_indiceBufferDatosEscribir][tipo].arregloIndices[bloqueIndGL.primerElemento + bloqueIndGL.cantidadElementos]))
	NBASSERT(bloqueIndGL.primerElemento > 1) //Los primeros dos elementos estan reservados
	NBASSERT(bloqueIndGL.cantidadElementos > 0 && (bloqueIndGL.cantidadElementos % 6) == 0)
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
	return bloqueIndGL;
}
#endif

#ifdef CONFIG_NB_GESTOR_ESCENAS_MODELOS_MEDIANTE_INDICES
STBloqueGL NBGestorEscena::reservarIndicesParaTriangStripIndependizadoDesdeTriangFan(const ENVerticeGlTipo tipo, UI16 iPrimerVertice, UI16 verticesPorTriangFan){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorEscena::reservarIndicesParaTriangStripIndependizadoDesdeTriangFan")
	NBASSERT(_gestorInicializado);
	NBASSERT(tipo < ENVerticeGlTipo_Conteo);
	NBASSERT(verticesPorTriangFan > 2)
	//---------------------
	//- TRIANG_FAN:   v0, v1, v2, v3, v4, v5, v6, v7, v8,.....
	//- TRIANG_STRIP: v1, v2, v0, ... v3, v0, v4, v0, v5, v0, v6, v0, v7, v0 .....
	//---------------------
	UI16 iVert, cantVerticesAdicionales	= verticesPorTriangFan - 3;
	UI16 conteoVerticesStrip			= 1 + verticesPorTriangFan + cantVerticesAdicionales + 1; //Inicio + originales + nuevos_para_strip + final
	UI16 iVertCentro					= iPrimerVertice;
	STBloqueGL bloqueIndGL;
	bloqueIndGL.cantidadElementos		= conteoVerticesStrip + (conteoVerticesStrip % 2);
	bloqueIndGL.primerElemento			= NBGestorEscena::reservarIndicesGL(tipo, bloqueIndGL.cantidadElementos);
	GLushort* indicesGL					= &(_buffersVertices[_indiceBufferDatosEscribir][tipo].arregloIndices[bloqueIndGL.primerElemento]);
	//Inicio repetido
	*indicesGL							= iPrimerVertice + 1; indicesGL++;	//V1 (repetir)
	//Triangulo inicial
	*indicesGL							= iPrimerVertice + 1; indicesGL++;	//V1
	*indicesGL							= iPrimerVertice + 2; indicesGL++;	//V2
	*indicesGL							= iVertCentro; indicesGL++;	//V0
	iPrimerVertice						+= 3;
	//Triangulos adicionales menos el ultimo
	for(iVert=0; iVert < cantVerticesAdicionales; iVert++){
		*indicesGL						= iPrimerVertice++; indicesGL++;	//Siguiente vertice
		*indicesGL						= iVertCentro; indicesGL++;			//V0
	}
	//Final repetido
	*indicesGL							= iVertCentro; indicesGL++;			//V0 (repetir)
	//Por si es impar (asegurar paridad)
	if((conteoVerticesStrip % 2) != 0){
		*indicesGL						= iVertCentro; indicesGL++;			//V0 (repetir para conservar numero par de indices)
	}
	NBASSERT(indicesGL == &(_buffersVertices[_indiceBufferDatosEscribir][tipo].arregloIndices[bloqueIndGL.primerElemento + bloqueIndGL.cantidadElementos]))
	NBASSERT(bloqueIndGL.primerElemento > 1) //Los primeros dos elementos estan reservados
	NBASSERT(bloqueIndGL.cantidadElementos > 0)
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
	return bloqueIndGL;
}
#endif

SI32 NBGestorEscena::conteoVerticesGLUsados(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorEscena::conteoVerticesGLUsados")
	NBASSERT(_gestorInicializado);
	SI32 usoVerticesGL = 0;
	SI32 iBufV;
	for(iBufV=0; iBufV < ENVerticeGlTipo_Conteo; iBufV++){
		usoVerticesGL += (_buffersVertices[_indiceBufferDatosLeer][iBufV].usoArregloVertices - 1); //-1 porque el primer elemento esta reservado
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
	return usoVerticesGL;
}

SI32 NBGestorEscena::conteoVerticesGLEnviadosHaciaBufferGL(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorEscena::conteoVerticesGLEnviadosHaciaBufferGL")
	NBASSERT(_gestorInicializado);
	SI32 conteoVerticesGL = 0;
	SI32 iBufV;
	for(iBufV=0; iBufV < ENVerticeGlTipo_Conteo; iBufV++){
		conteoVerticesGL += (_buffersVertices[_indiceBufferDatosLeer][iBufV].usoArregloVertices - 1); //-1 porque el primer elemento esta reservado
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
	return conteoVerticesGL;
}

#ifdef CONFIG_NB_GESTOR_ESCENAS_MODELOS_MEDIANTE_INDICES
SI32 NBGestorEscena::conteoIndicesGLUsados(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorEscena::conteoIndicesGLUsados")
	SI32 usoIndicesGL = 0;
	SI32 iBufV;
	for(iBufV=0; iBufV < ENVerticeGlTipo_Conteo; iBufV++){
		usoIndicesGL += (_buffersVertices[_indiceBufferDatosLeer][iBufV].usoArregloIndices - 1); //-1 porque el primer elemento esta reservado
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
	return usoIndicesGL;
}
#endif


void NBGestorEscena::bufferObtenerDatos(void* param, const SI32 iVao, STVerticesDatos* guardarDatosEn){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL("NBGestorEscena::bufferObtenerDatos")
	NBASSERT(_gestorInicializado);
	NBASSERT(guardarDatosEn != NULL)
	NBASSERT(iVao >= 0 && iVao < ENVerticeGlTipo_Conteo)
	NBASSERT(_bufferDatosBloqueado[_indiceBufferDatosLeer])	//Debe estar bloqueado
	if(guardarDatosEn != NULL && iVao >= 0 && iVao < ENVerticeGlTipo_Conteo){
		STBufferVerticesGL* ptrBuff			= &_buffersVertices[_indiceBufferDatosLeer][iVao];
		guardarDatosEn->verticesDatos		= ptrBuff->arregloVertices;
		guardarDatosEn->verticesCantBytes	= ptrBuff->usoArregloVertices * ptrBuff->bytesPorRegistro;
#		ifdef CONFIG_NB_GESTOR_ESCENAS_MODELOS_MEDIANTE_INDICES
		guardarDatosEn->indicesDatos		= (BYTE*)ptrBuff->arregloIndices;
		guardarDatosEn->indicesCantBytes	= ptrBuff->usoArregloIndices * sizeof(GLushort);
#		endif
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL
}

SI32 NBGestorEscena::debugImprimirEstadoBufferes(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::debugImprimirEstadoBufferes")
	PRINTF_INFO("Estado bufferes: ");
	SI32 conteoBufferes = 0;
	SI32 iBuff;
	for(iBuff=0; iBuff<NBGESTORESCENA_CANTIDAD_BUFFERES_DATOS; iBuff++){
		PRINTF_INFO("%s-%s ", (_bufferDatosBloqueado[iBuff] ? "BLQ" : "LIB"), (_bufferDatosEstado[iBuff] == ENGestorEscenaBufferEstado_Lleno ? "LLN" : _bufferDatosEstado[iBuff] == ENGestorEscenaBufferEstado_Discarted ? "DISC" : _bufferDatosEstado[iBuff] == ENGestorEscenaBufferEstado_Vacio ? "VAC" : "???"));
		if(_bufferDatosBloqueado[iBuff]){
			conteoBufferes++;
		}
	}
	PRINTF_INFO("\n");
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
	return conteoBufferes;
}

SI32 NBGestorEscena::bufferDatosConteoBloqueados(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::bufferDatosConteoBloqueados")
	SI32 conteoBufferes = 0;
	SI32 iBuff;
	for(iBuff=0; iBuff<NBGESTORESCENA_CANTIDAD_BUFFERES_DATOS; iBuff++){
		if(_bufferDatosBloqueado[iBuff]) conteoBufferes++;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
	return conteoBufferes;
}

SI32 NBGestorEscena::bufferDatosConteoLlenos(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::bufferDatosConteoLlenos")
	SI32 conteoBufferes = 0;
	SI32 iBuff;
	for(iBuff=0; iBuff<NBGESTORESCENA_CANTIDAD_BUFFERES_DATOS; iBuff++){
		if(_bufferDatosEstado[iBuff] != ENGestorEscenaBufferEstado_Vacio) conteoBufferes++;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
	return conteoBufferes;
}

/*SI32 NBGestorEscena::conteoBufferDatosVacios(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::conteoBufferDatosVacios")
	SI32 conteoBufferes = 0;
	SI32 iBuff;
	for(iBuff=0; iBuff<NBGESTORESCENA_CANTIDAD_BUFFERES_DATOS; iBuff++){
		if(_bufferDatosEstado[iBuff] == ENGestorEscenaBufferEstado_Vacio) conteoBufferes++;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
	return conteoBufferes;
}*/

//

float NBGestorEscena::cuadrosPorSegundo(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::cuadrosPorSegundo")
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
	return _cuadrosPorSegundo;
}

void NBGestorEscena::establecerCuadrosPorSegundo(float cuadrosPorSegundo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::establecerCuadrosPorSegundo")
	_cuadrosPorSegundo = cuadrosPorSegundo;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

SI32 NBGestorEscena::ticksAcumulados(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::ticksAcumulados")
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
	return _ticksAcumulados;
}

void NBGestorEscena::resetearTicksAcumulados(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::resetearTicksAcumulados")
	_ticksAcumulados = 0;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

NBRectangulo NBGestorEscena::puertoDeVision(SI32 iEscena){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::puertoDeVision")
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	NBASSERT(scn->puertoDeVision.ancho>0);
	NBASSERT(scn->puertoDeVision.alto>0);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
	return scn->puertoDeVision;
}

NBTamano NBGestorEscena::tamanoPulgadasPantalla(const SI32 iEscena){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::tamanoPulgadasPantalla")
	NBTamano r; r.ancho = r.alto = 0.0f;
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	NBASSERT(scn->puertoDeVision.ancho > 0);
	NBASSERT(scn->puertoDeVision.alto > 0);
	NBASSERT(scn->pixelsPorPulgadaPantalla.ancho > 0.0f);
	NBASSERT(scn->pixelsPorPulgadaPantalla.alto > 0.0f);
	if(scn->pixelsPorPulgadaPantalla.ancho > 0.0f && scn->pixelsPorPulgadaPantalla.alto > 0.0f){
		r.ancho	= scn->puertoDeVision.ancho / scn->pixelsPorPulgadaPantalla.ancho;
		r.alto	= scn->puertoDeVision.alto / scn->pixelsPorPulgadaPantalla.alto;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
	return r;
}

NBTamano NBGestorEscena::pixelsPorPulgadaPantalla(const SI32 iEscena){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::pixelsPorPulgadaPantalla")
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	NBASSERT(scn->puertoDeVision.ancho > 0);
	NBASSERT(scn->puertoDeVision.alto > 0);
	NBASSERT(scn->pixelsPorPulgadaPantalla.ancho > 0.0f);
	NBASSERT(scn->pixelsPorPulgadaPantalla.alto > 0.0f);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
	return scn->pixelsPorPulgadaPantalla;
}

NBTamano NBGestorEscena::puntosPorPulgada(const SI32 iEscena){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::puntosPorPulgada")
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	NBASSERT(scn->puertoDeVision.ancho > 0);
	NBASSERT(scn->puertoDeVision.alto > 0);
	NBASSERT(scn->puntosPorPulgada.ancho > 0.0f);
	NBASSERT(scn->puntosPorPulgada.alto > 0.0f);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
	return scn->puntosPorPulgada;
}

void NBGestorEscena::establecerCajaProyeccion(SI32 iEscena, float xMin, float xMax, float yMin, float yMax){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::establecerCajaProyeccion")
	//NBASSERT(!_bufferDatosBloqueado[_indiceBufferDatosEscribir]); //Ya no necesario validar. Se crea una copia de este dato para el hilo renderizador.
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	//NBASSERT(!scn->debugBloqueadoActualizandoModelos)
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	NBCajaAABB cajaProy;
	cajaProy.xMin		= xMin;
	cajaProy.xMax		= xMax;
	cajaProy.yMin		= yMin;
	cajaProy.yMax		= yMax;
	scn->gruposCapas[ENGestorEscenaGrupo_Escena].cajaProyeccion = cajaProy;
	NBASSERT(xMin<xMax);
	NBASSERT(yMin<yMax);
	NBASSERT(xMin == xMin); //Es Nan
	NBASSERT(xMax == xMax); //Es Nan
	NBASSERT(yMin == yMin); //Es Nan
	NBASSERT(yMax == yMax); //Es Nan
	{
		//Update roots
		STGestorEscenaGrupo* grupoCapas = &(scn->gruposCapas[ENGestorEscenaGrupo_Escena]);
		UI32 iCapa, iCapaConteo = grupoCapas->capas->conteo;
		for(iCapa = 0; iCapa < iCapaConteo; iCapa++){
			STGestorEscenaCapa* propiedadesDeCapa = &(grupoCapas->capas->elemento[iCapa]);
			NBMatriz matrizInversaCapa; NBMATRIZ_INVERSA(matrizInversaCapa, propiedadesDeCapa->matrizCapa)
			NBCajaAABB sceneBox; NBMATRIZ_MULTIPLICAR_CAJAAABB(sceneBox, matrizInversaCapa, grupoCapas->cajaProyeccion);
			propiedadesDeCapa->root->setSceneBox(NBST_P(STNBRectI, (SI32)scn->puertoDeVision.x, (SI32)scn->puertoDeVision.y, (SI32)scn->puertoDeVision.ancho, (SI32)scn->puertoDeVision.alto ), NBST_P(STNBAABox, grupoCapas->cajaProyeccion.xMin, grupoCapas->cajaProyeccion.xMax, grupoCapas->cajaProyeccion.yMin, grupoCapas->cajaProyeccion.yMax ));
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

NBTamano NBGestorEscena::escalaCajaProyeccionOrtogonal(SI32 iEscena, ENGestorEscenaGrupo idGrupo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::escalaCajaProyeccionOrtogonal")
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	NBASSERT(scn->registroOcupado)
	NBTamano escala; NBTAMANO_ESTABLECER(escala, 1.0f, 1.0f); // unidades GL por pixel
	//
	NBRectangulo puertoVision	= scn->puertoDeVision;
	NBCajaAABB	cajaProyeccion	= scn->gruposCapas[idGrupo].cajaProyeccion;
	NBTamano tamanoProyeccion;	NBCAJAAABB_TAMANO(tamanoProyeccion, cajaProyeccion);
	escala.ancho				= tamanoProyeccion.ancho / puertoVision.ancho;
	escala.alto					= tamanoProyeccion.alto / puertoVision.alto;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
	return escala;
}

float NBGestorEscena::escalaEscenaParaHD(const SI32 iEscena){
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	NBASSERT(scn->registroOcupado)
	NBASSERT(scn->escalaParaHD > 0.0f)
	return scn->escalaParaHD;
}

//-----------------------------
//-- Conversion de dimensiones (pantalla)
//-----------------------------
NBTamano NBGestorEscena::tamanoPantallaAPulgadas(const SI32 iEscena, const NBTamano tamPantalla){
	NBTamano r; r.ancho = 0.0f; r.alto = 0.0f;
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	NBASSERT(scn->registroOcupado)
	NBASSERT(scn->pixelsPorPulgadaPantalla.ancho > 0.0f && scn->pixelsPorPulgadaPantalla.alto > 0.0f)
	if(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS){
		if(scn->registroOcupado && scn->pixelsPorPulgadaPantalla.ancho > 0.0f && scn->pixelsPorPulgadaPantalla.alto > 0.0f){
			r.ancho	= tamPantalla.ancho / scn->pixelsPorPulgadaPantalla.ancho;
			r.alto	= tamPantalla.alto / scn->pixelsPorPulgadaPantalla.alto;
		}
	}
	return r;
}

NBTamano NBGestorEscena::tamanoPantallaAPulgadas(const SI32 iEscena, const float anchoPantalla, const float altoPantalla){
	NBTamano r; r.ancho = 0.0f; r.alto = 0.0f;
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	NBASSERT(scn->registroOcupado)
	NBASSERT(scn->pixelsPorPulgadaPantalla.ancho > 0.0f && scn->pixelsPorPulgadaPantalla.alto > 0.0f)
	if(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS){
		if(scn->registroOcupado && scn->pixelsPorPulgadaPantalla.ancho > 0.0f && scn->pixelsPorPulgadaPantalla.alto > 0.0f){
			r.ancho	= anchoPantalla / scn->pixelsPorPulgadaPantalla.ancho;
			r.alto	= altoPantalla / scn->pixelsPorPulgadaPantalla.alto;
		}
	}
	return r;
}

float NBGestorEscena::anchoPantallaAPulgadas(const SI32 iEscena, const float anchoPantalla){
	float r = 0.0f;
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	NBASSERT(scn->registroOcupado)
	NBASSERT(scn->pixelsPorPulgadaPantalla.ancho > 0.0f && scn->pixelsPorPulgadaPantalla.alto > 0.0f)
	if(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS){
		if(scn->registroOcupado && scn->pixelsPorPulgadaPantalla.ancho > 0.0f && scn->pixelsPorPulgadaPantalla.alto > 0.0f){
			r	= anchoPantalla / scn->pixelsPorPulgadaPantalla.ancho;
		}
	}
	return r;
}

float NBGestorEscena::altoPantallaAPulgadas(const SI32 iEscena, const float altoPantalla){
	float r = 0.0f;
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	NBASSERT(scn->registroOcupado)
	NBASSERT(scn->pixelsPorPulgadaPantalla.ancho > 0.0f && scn->pixelsPorPulgadaPantalla.alto > 0.0f)
	if(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS){
		if(scn->registroOcupado && scn->pixelsPorPulgadaPantalla.ancho > 0.0f && scn->pixelsPorPulgadaPantalla.alto > 0.0f){
			r	= altoPantalla / scn->pixelsPorPulgadaPantalla.alto;
		}
	}
	return r;
}

NBTamano NBGestorEscena::tamanoPulgadasAPantalla(const SI32 iEscena, const NBTamano tamPulgadas){
	NBTamano r; r.ancho = 0.0f; r.alto = 0.0f;
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	NBASSERT(scn->registroOcupado)
	NBASSERT(scn->pixelsPorPulgadaPantalla.ancho > 0.0f && scn->pixelsPorPulgadaPantalla.alto > 0.0f)
	if(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS){
		if(scn->registroOcupado && scn->pixelsPorPulgadaPantalla.ancho > 0.0f && scn->pixelsPorPulgadaPantalla.alto > 0.0f){
			r.ancho	= (tamPulgadas.ancho * scn->pixelsPorPulgadaPantalla.ancho);
			r.alto	= (tamPulgadas.alto * scn->pixelsPorPulgadaPantalla.alto);
		}
	}
	return r;
}

NBTamano NBGestorEscena::tamanoPulgadasAPantalla(const SI32 iEscena, const float anchoPulgadas, const float altoPulgadas){
	NBTamano r; r.ancho = 0.0f; r.alto = 0.0f;
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	NBASSERT(scn->registroOcupado)
	NBASSERT(scn->pixelsPorPulgadaPantalla.ancho > 0.0f && scn->pixelsPorPulgadaPantalla.alto > 0.0f)
	if(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS){
		if(scn->registroOcupado && scn->pixelsPorPulgadaPantalla.ancho > 0.0f && scn->pixelsPorPulgadaPantalla.alto > 0.0f){
			r.ancho	= (anchoPulgadas * scn->pixelsPorPulgadaPantalla.ancho);
			r.alto	= (altoPulgadas * scn->pixelsPorPulgadaPantalla.alto);
		}
	}
	return r;
}

float NBGestorEscena::anchoPulgadasAPantalla(const SI32 iEscena, const float anchoPulgadas){
	float r = 0.0f;
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	NBASSERT(scn->registroOcupado)
	NBASSERT(scn->pixelsPorPulgadaPantalla.ancho > 0.0f && scn->pixelsPorPulgadaPantalla.alto > 0.0f)
	if(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS){
		if(scn->registroOcupado && scn->pixelsPorPulgadaPantalla.ancho > 0.0f){
			r	= (anchoPulgadas * scn->pixelsPorPulgadaPantalla.ancho);
		}
	}
	return r;
}

float NBGestorEscena::altoPulgadasAPantalla(const SI32 iEscena, const float altoPulgadas){
	float r = 0.0f;
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	NBASSERT(scn->registroOcupado)
	NBASSERT(scn->pixelsPorPulgadaPantalla.ancho > 0.0f && scn->pixelsPorPulgadaPantalla.alto > 0.0f)
	if(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS){
		if(scn->registroOcupado && scn->pixelsPorPulgadaPantalla.alto > 0.0f){
			r	= (altoPulgadas * scn->pixelsPorPulgadaPantalla.alto);
		}
	}
	return r;
}

//-----------------------------
//-- Conversion de dimensiones (metricas escena)
//-----------------------------

float NBGestorEscena::anchoEscenaAPulgadas(const SI32 iEscena, const float anchoEscena){
	float r = 0.0f;
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	NBASSERT(scn->registroOcupado)
	NBASSERT(scn->puntosPorPulgada.ancho > 0.0f && scn->puntosPorPulgada.alto > 0.0f)
	NBASSERT(scn->escalaParaHD > 0.0f)
	if(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS){
		if(scn->registroOcupado && scn->puntosPorPulgada.ancho > 0.0f && scn->puntosPorPulgada.alto > 0.0f && scn->escalaParaHD > 0.0f){
			r	= (anchoEscena / scn->escalaParaHD) / scn->puntosPorPulgada.ancho;
		}
	}
	return r;
}

float NBGestorEscena::altoEscenaAPulgadas(const SI32 iEscena, const float altoEscena){
	float r = 0.0f;
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	NBASSERT(scn->registroOcupado)
	NBASSERT(scn->puntosPorPulgada.ancho > 0.0f && scn->puntosPorPulgada.alto > 0.0f)
	NBASSERT(scn->escalaParaHD > 0.0f)
	if(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS){
		if(scn->registroOcupado && scn->puntosPorPulgada.ancho > 0.0f && scn->puntosPorPulgada.alto > 0.0f && scn->escalaParaHD > 0.0f){
			r	= (altoEscena / scn->escalaParaHD) / scn->puntosPorPulgada.alto;
		}
	}
	return r;
}

NBTamano NBGestorEscena::tamanoEscenaAPulgadas(const SI32 iEscena, const NBTamano tamEscena){
	NBTamano r; r.ancho = 0.0f; r.alto = 0.0f;
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	NBASSERT(scn->registroOcupado)
	NBASSERT(scn->puntosPorPulgada.ancho > 0.0f && scn->puntosPorPulgada.alto > 0.0f)
	NBASSERT(scn->escalaParaHD > 0.0f)
	if(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS){
		if(scn->registroOcupado && scn->puntosPorPulgada.ancho > 0.0f && scn->puntosPorPulgada.alto > 0.0f && scn->escalaParaHD > 0.0f){
			r.ancho	= (tamEscena.ancho / scn->escalaParaHD) / scn->puntosPorPulgada.ancho;
			r.alto	= (tamEscena.alto / scn->escalaParaHD) / scn->puntosPorPulgada.alto;
		}
	}
	return r;
}

NBTamano NBGestorEscena::tamanoEscenaAPulgadas(const SI32 iEscena, const float anchoEscena, const float altoEscena){
	NBTamano r; r.ancho = 0.0f; r.alto = 0.0f;
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	NBASSERT(scn->registroOcupado)
	NBASSERT(scn->puntosPorPulgada.ancho > 0.0f && scn->puntosPorPulgada.alto > 0.0f)
	NBASSERT(scn->escalaParaHD > 0.0f)
	if(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS){
		if(scn->registroOcupado && scn->puntosPorPulgada.ancho > 0.0f && scn->puntosPorPulgada.alto > 0.0f && scn->escalaParaHD > 0.0f){
			r.ancho	= (anchoEscena / scn->escalaParaHD) / scn->puntosPorPulgada.ancho;
			r.alto	= (altoEscena / scn->escalaParaHD) / scn->puntosPorPulgada.alto;
		}
	}
	return r;
}

NBTamano NBGestorEscena::tamanoPulgadasAEscena(const SI32 iEscena, const NBTamano tamPulgadas){
	NBTamano r; r.ancho = 0.0f; r.alto = 0.0f;
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	NBASSERT(scn->registroOcupado)
	NBASSERT(scn->puntosPorPulgada.ancho > 0.0f && scn->puntosPorPulgada.alto > 0.0f)
	NBASSERT(scn->escalaParaHD > 0.0f)
	if(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS){
		if(scn->registroOcupado && scn->puntosPorPulgada.ancho > 0.0f && scn->puntosPorPulgada.alto > 0.0f && scn->escalaParaHD > 0.0f){
			r.ancho	= (tamPulgadas.ancho * scn->puntosPorPulgada.ancho) * scn->escalaParaHD;
			r.alto	= (tamPulgadas.alto * scn->puntosPorPulgada.alto) * scn->escalaParaHD;
		}
	}
	return r;
}

NBTamano NBGestorEscena::tamanoPulgadasAEscena(const SI32 iEscena, const float anchoPulgadas, const float altoPulgadas){
	NBTamano r; r.ancho = 0.0f; r.alto = 0.0f;
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	NBASSERT(scn->registroOcupado)
	NBASSERT(scn->puntosPorPulgada.ancho > 0.0f && scn->puntosPorPulgada.alto > 0.0f)
	NBASSERT(scn->escalaParaHD > 0.0f)
	if(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS){
		if(scn->registroOcupado && scn->puntosPorPulgada.ancho > 0.0f && scn->puntosPorPulgada.alto > 0.0f && scn->escalaParaHD > 0.0f){
			r.ancho	= (anchoPulgadas * scn->puntosPorPulgada.ancho) * scn->escalaParaHD;
			r.alto	= (altoPulgadas * scn->puntosPorPulgada.alto) * scn->escalaParaHD;
		}
	}
	return r;
}

float NBGestorEscena::anchoPulgadasAEscena(const SI32 iEscena, const float anchoPulgadas){
	float r = 0.0f;
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	NBASSERT(scn->registroOcupado)
	NBASSERT(scn->puntosPorPulgada.ancho > 0.0f && scn->puntosPorPulgada.alto > 0.0f)
	NBASSERT(scn->escalaParaHD > 0.0f)
	if(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS){
		if(scn->registroOcupado && scn->puntosPorPulgada.ancho > 0.0f && scn->escalaParaHD > 0.0f){
			r	= (anchoPulgadas * scn->puntosPorPulgada.ancho) * scn->escalaParaHD;
		}
	}
	return r;
}

float NBGestorEscena::altoPulgadasAEscena(const SI32 iEscena, const float altoPulgadas){
	float r = 0.0f;
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	NBASSERT(scn->registroOcupado)
	NBASSERT(scn->puntosPorPulgada.ancho > 0.0f && scn->puntosPorPulgada.alto > 0.0f)
	NBASSERT(scn->escalaParaHD > 0.0f)
	if(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS){
		if(scn->registroOcupado && scn->puntosPorPulgada.alto > 0.0f && scn->escalaParaHD > 0.0f){
			r	= (altoPulgadas * scn->puntosPorPulgada.alto) * scn->escalaParaHD;
		}
	}
	return r;
}

//-----------------------------
//-- Tamanos baseDpi72 (mismo tamano de Word)
//-----------------------------

float  NBGestorEscena::anchoEscenaAPuntos(const SI32 iEscena, const float anchoEscena){
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	const STGestorEscenaEscena* scn = &_escenas[iEscena];
	NBASSERT(scn->registroOcupado)
	NBASSERT(scn->puntosPorPulgada.alto > 0.0f)
	NBASSERT(scn->escalaParaHD > 0.0f)
	return ((anchoEscena / scn->escalaParaHD) * 72.0f) / scn->puntosPorPulgada.ancho;
}

float NBGestorEscena::altoEscenaAPuntos(const SI32 iEscena, const float altoEscena){
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	const STGestorEscenaEscena* scn = &_escenas[iEscena];
	NBASSERT(scn->registroOcupado)
	NBASSERT(scn->puntosPorPulgada.alto > 0.0f)
	NBASSERT(scn->escalaParaHD > 0.0f)
	return ((altoEscena / scn->escalaParaHD) * 72.0f) / scn->puntosPorPulgada.alto;
}

NBTamano NBGestorEscena::tamanoEscenaAPuntos(const SI32 iEscena, const NBTamano tamEscena){
	NBTamano r;
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	const STGestorEscenaEscena* scn = &_escenas[iEscena];
	NBASSERT(scn->registroOcupado)
	NBASSERT(scn->puntosPorPulgada.alto > 0.0f)
	NBASSERT(scn->escalaParaHD > 0.0f)
	r.ancho	= ((tamEscena.ancho / scn->escalaParaHD) * 72.0f) / scn->puntosPorPulgada.ancho;
	r.alto	= ((tamEscena.alto / scn->escalaParaHD) * 72.0f) / scn->puntosPorPulgada.alto;
	return r;
}

float NBGestorEscena::anchoPuntosAEscena(const SI32 iEscena, const float tamEnPuntos72){
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	const STGestorEscenaEscena* scn = &_escenas[iEscena];
	NBASSERT(scn->registroOcupado)
	NBASSERT(scn->puntosPorPulgada.alto > 0.0f)
	NBASSERT(scn->escalaParaHD > 0.0f)
	return (tamEnPuntos72 * (scn->puntosPorPulgada.ancho / 72.0f)) * scn->escalaParaHD;
}

float NBGestorEscena::altoPuntosAEscena(const SI32 iEscena, const float tamEnPuntos72){
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	const STGestorEscenaEscena* scn = &_escenas[iEscena];
	NBASSERT(scn->registroOcupado)
	NBASSERT(scn->puntosPorPulgada.alto > 0.0f)
	NBASSERT(scn->escalaParaHD > 0.0f)
	return (tamEnPuntos72 * (scn->puntosPorPulgada.alto / 72.0f)) * scn->escalaParaHD;
}

NBTamano NBGestorEscena::tamanoPuntosAEscena(const SI32 iEscena, const NBTamano tamEnPuntos72){
	NBTamano r;
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	const STGestorEscenaEscena* scn = &_escenas[iEscena];
	NBASSERT(scn->registroOcupado)
	NBASSERT(scn->puntosPorPulgada.alto > 0.0f)
	NBASSERT(scn->escalaParaHD > 0.0f)
	r.ancho	= (tamEnPuntos72.ancho * (scn->puntosPorPulgada.ancho / 72.0f)) * scn->escalaParaHD;
	r.alto	= (tamEnPuntos72.alto * (scn->puntosPorPulgada.alto / 72.0f)) * scn->escalaParaHD;
	return r;
}

NBTamano NBGestorEscena::tamanoPuntosAEscena(const SI32 iEscena, const float anchoEnPuntos72, const float altoEnPuntos72){
	NBTamano r;
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	const STGestorEscenaEscena* scn = &_escenas[iEscena];
	NBASSERT(scn->registroOcupado)
	NBASSERT(scn->puntosPorPulgada.alto > 0.0f)
	NBASSERT(scn->escalaParaHD > 0.0f)
	r.ancho	= (anchoEnPuntos72 * (scn->puntosPorPulgada.ancho / 72.0f)) * scn->escalaParaHD;
	r.alto	= (altoEnPuntos72 * (scn->puntosPorPulgada.alto / 72.0f)) * scn->escalaParaHD;
	return r;
}

//-----------------------------
//-- Fuentes tamano baseDpi72 (mismo tamano de Word)
//-----------------------------
AUFuenteLineas* NBGestorEscena::fuenteLineas(const SI32 iEscena, const char* nomFuente, const bool negrilla, const bool cursiva){
	return NBGestorFuentes::fuenteLineas(nomFuente, negrilla, cursiva);
}

AUFuenteMapaBits* NBGestorEscena::fuenteBitmapsEnPuntos(const SI32 iEscena, const char* nomFuente, const float tamEnPuntos72, const bool negrilla, const bool cursiva){
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	const STGestorEscenaEscena* scn = &_escenas[iEscena];
	NBASSERT(scn->registroOcupado)
	NBASSERT(scn->puntosPorPulgada.alto > 0.0f)
	NBASSERT(scn->escalaParaHD > 0.0f)
	const float puntosEnPuerto		= tamEnPuntos72 * (scn->puntosPorPulgada.alto / 72.0f);
	const float unidadesEnEscena	= puntosEnPuerto * scn->escalaParaHD;
	//const float escalaParaHD		= scn->escalaParaHD; //Pendiente de implementar
	return NBGestorFuentes::fuenteBitmaps(nomFuente, unidadesEnEscena, negrilla, cursiva);
}

AUFuenteTextura* NBGestorEscena::fuenteTexturaEnPuntos(const SI32 iEscena, const char* nomFuente, const float tamEnPuntos72, const bool negrilla, const bool cursiva){
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	const STGestorEscenaEscena* scn = &_escenas[iEscena];
	NBASSERT(scn->registroOcupado)
	NBASSERT(scn->puntosPorPulgada.alto > 0.0f)
	NBASSERT(scn->escalaParaHD > 0.0f)
	const float puntosEnPuerto		= tamEnPuntos72 * (scn->puntosPorPulgada.alto / 72.0f);
	const float unidadesEnEscena	= puntosEnPuerto * scn->escalaParaHD;
	const float escalaParaHD		= scn->escalaParaHD;
	return NBGestorTexturas::fuenteTextura(nomFuente, unidadesEnEscena, negrilla, cursiva, escalaParaHD);
}

AUFuenteRaster* NBGestorEscena::fuenteRasterEnPuntos(const SI32 iEscena, const char* nomFuente, const float tamEnPuntos72, const bool negrilla, const bool cursiva){
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	const STGestorEscenaEscena* scn = &_escenas[iEscena];
	NBASSERT(scn->registroOcupado)
	NBASSERT(scn->puntosPorPulgada.alto > 0.0f)
	NBASSERT(scn->escalaParaHD > 0.0f)
	const float puntosEnPuerto		= tamEnPuntos72 * (scn->puntosPorPulgada.alto / 72.0f);
	const float unidadesEnEscena	= puntosEnPuerto * scn->escalaParaHD;
	return NBGestorTexturas::fuenteRaster(nomFuente, unidadesEnEscena, negrilla, cursiva);
}

ENGestorEscenaFondoModo NBGestorEscena::fondoModo(const SI16 iEscena){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::fondoModo")
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	NBASSERT(scn->registroOcupado)
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
	return scn->fondoModo;
}

NBColor NBGestorEscena::colorFondo(SI32 iEscena){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::colorFondo")
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	NBASSERT(scn->registroOcupado)
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
	return scn->colorFondo;
}

void NBGestorEscena::establecerFondoModo(const SI16 iEscena, const ENGestorEscenaFondoModo fondoModo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::establecerFondoModo")
	//NBASSERT(!_bufferDatosBloqueado[_indiceBufferDatosEscribir]); //Ya no necesario validar. Se crea una copia de este dato para el hilo renderizador.
	NBASSERT(fondoModo >= 0 && fondoModo<ENGestorEscenaFondoModo_Conteo)
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	NBASSERT(scn->registroOcupado)
	scn->fondoModo = fondoModo;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

void NBGestorEscena::establecerColorFondo(SI32 iEscena, float r, float g, float b, float a){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::colorFondo")
	//NBASSERT(!_bufferDatosBloqueado[_indiceBufferDatosEscribir]); //Ya no necesario validar. Se crea una copia de este dato para el hilo renderizador.
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	NBASSERT(scn->registroOcupado)
	NBCOLOR_ESTABLECER(scn->colorFondo, r, g, b, a);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

void NBGestorEscena::establecerColorFondo(SI32 iEscena, NBColor color){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::colorFondo")
	//NBASSERT(!_bufferDatosBloqueado[_indiceBufferDatosEscribir]); //Ya no necesario validar. Se crea una copia de este dato para el hilo renderizador.
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	NBASSERT(scn->registroOcupado)
	scn->colorFondo = color;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

// CAPAS DE ESCENAS

STGestorEscenaCapa* NBGestorEscena::privPropiedadesCapaPorID(SI32 iEscena, int idCapa){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::privPropiedadesCapaPorID")
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS)
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	NBASSERT(scn->registroOcupado)
	STGestorEscenaCapa* r = NULL;
	SI32 iGrp; for(iGrp = 0; iGrp < ENGestorEscenaGrupo_Conteo && r == NULL; iGrp++){
		STGestorEscenaGrupo* grupoCapas = &(scn->gruposCapas[iGrp]);
		SI32 iCapa, iCapaConteo = grupoCapas->capas->conteo;
		for(iCapa=0; iCapa<iCapaConteo; iCapa++){
			if(grupoCapas->capas->elemento[iCapa].idCapa == idCapa){
				r = &(grupoCapas->capas->elemento[iCapa]);
				break;
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
	return r;
}

STGestorEscenaCapa* NBGestorEscena::privPropiedadesCapaPorObjetoEscena(SI32 iEscena, AUEscenaContenedor* objEscena){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::privPropiedadesCapaPorObjetoEscena")
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS)
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	NBASSERT(scn->registroOcupado)
	STGestorEscenaCapa* r = NULL;
	SI32 iGrp; for(iGrp = 0; iGrp < ENGestorEscenaGrupo_Conteo && r == NULL; iGrp++){
		STGestorEscenaGrupo* grupoCapas = &(scn->gruposCapas[iGrp]);
		SI32 i, iConteo = grupoCapas->capas->conteo;
		for(i=0; i<iConteo; i++){
			if(grupoCapas->capas->elemento[i].objEscena == objEscena){
				r = &(grupoCapas->capas->elemento[i]);
				break;
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
	return r;
}

//Size of areas that should not be used for content (because are used by the device's top bar or bottom area)

NBMargenes NBGestorEscena::getSceneMarginsScn(const SI32 iEscena, const ENGestorEscenaGrupo idGrupo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::getSceneMarginsScn")
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS)
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	NBASSERT(scn->registroOcupado)
	NBASSERT(idGrupo >= 0 && idGrupo < ENGestorEscenaGrupo_Conteo)
	const float w			= scn->gruposCapas[idGrupo].cajaProyeccion.xMax - scn->gruposCapas[idGrupo].cajaProyeccion.xMin;
	const float h			= scn->gruposCapas[idGrupo].cajaProyeccion.yMax - scn->gruposCapas[idGrupo].cajaProyeccion.yMin;
	const float glPerPxW	= (w / (float)scn->puertoDeVision.ancho);
	const float glPerPxH	= (h / (float)scn->puertoDeVision.alto);
	NBMargenes r;
	r.left		= scn->marginsPxs.left * glPerPxW;
	r.right		= scn->marginsPxs.right * glPerPxW;
	r.top		= scn->marginsPxs.top * glPerPxH;
	r.bottom	= scn->marginsPxs.bottom * glPerPxH;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
	return r;
}

NBMargenes NBGestorEscena::getSceneMarginsPx(const SI32 iEscena){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::getSceneMarginsPx")
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS)
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	NBASSERT(scn->registroOcupado)
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
	return scn->marginsPxs;
}

void NBGestorEscena::setSceneMarginsPx(const SI32 iEscena, const NBMargenes margins){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::getSceneMarginsPx")
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS)
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	NBASSERT(scn->registroOcupado)
	if(scn->marginsPxs.left != margins.left || scn->marginsPxs.right != margins.right || scn->marginsPxs.top != margins.top || scn->marginsPxs.bottom != margins.bottom){
		scn->marginsPxs = margins;
		scn->sceneMask |= AUOBJETOESCENA_BIT_MODEL_LOCAL_CHANGED;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

//

void NBGestorEscena::agregarObjetoCapa(SI32 iEscena, ENGestorEscenaGrupo idGrupo, AUEscenaContenedor* objetoEscena, const NBColor8 &colorLuzAmbiente, bool tmpImplementarMascaraLuces){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::agregarObjetoCapa")
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	NBASSERT(scn->registroOcupado)
	NBASSERT(!_bufferDatosBloqueado[_indiceBufferDatosEscribir]);
	//NBASSERT(!scn->debugBloqueadoActualizandoModelos)
	//NBASSERT(objetoEscena->esClase(AUEscenaContenedor::idTipoClase)) //Si falla no es un Contenedor. El motor esta optimizado para renderizar contenedores mas que objetos individuales
	NBASSERT(NBGestorEscena::privPropiedadesCapaPorObjetoEscena(iEscena, objetoEscena) == NULL) //Si falla, se esta agregando el objeto capa dos veces
	//
	STGestorEscenaGrupo* grp = &(scn->gruposCapas[idGrupo]);
	//
	STGestorEscenaCapa propiedadesCapa;
	NBMemory_setZeroSt(propiedadesCapa, STGestorEscenaCapa);
	propiedadesCapa.habilitada						= true;
	propiedadesCapa.tmpImplementarMascaraLuces		= tmpImplementarMascaraLuces;
	propiedadesCapa.idCapa							= ++scn->contadorIdUnicosCapas;
	propiedadesCapa.root							= new (ENMemoriaTipo_General) AUEscenaContenedorRaiz();
	propiedadesCapa.root->setTopItf(scn->topItf);
	propiedadesCapa.root->privActualizarNivelEnEscena(iEscena, idGrupo, propiedadesCapa.idCapa);
	propiedadesCapa.root->agregarObjetoEscena(objetoEscena);
	propiedadesCapa.objEscena						= objetoEscena; objetoEscena->retener(NB_RETENEDOR_NULL);
	propiedadesCapa.fuentesIluminacion				= new(ENMemoriaTipo_General) AUArregloNativoMutableP<NBFuenteIluminacion>();		NB_DEFINE_NOMBRE_PUNTERO(propiedadesCapa.fuentesIluminacion, "NBGestorEscena::fuentesIluminacion");
	propiedadesCapa.fuentesEspejo					= new(ENMemoriaTipo_General) AUArregloNativoMutableP<NBFuenteEspejo>();				NB_DEFINE_NOMBRE_PUNTERO(propiedadesCapa.fuentesEspejo, "NBGestorEscena::fuentesEspejo");
	propiedadesCapa.productoresSombras				= new(ENMemoriaTipo_General) AUArregloNativoMutableP<IEscenaProductorSombras*>();	NB_DEFINE_NOMBRE_PUNTERO(propiedadesCapa.productoresSombras, "NBGestorEscena::productoresSombras");
	propiedadesCapa.descriptoresCuerdas				= new(ENMemoriaTipo_General) AUArregloNativoMutableP<ICuerdaCortable*>();			NB_DEFINE_NOMBRE_PUNTERO(propiedadesCapa.descriptoresCuerdas, "NBGestorEscena::descriptoresCuerdas");
	propiedadesCapa.consumidoresLuces				= new(ENMemoriaTipo_General) AUArregloNativoMutableP<IEscenaConsumidorLuces*>();	NB_DEFINE_NOMBRE_PUNTERO(propiedadesCapa.consumidoresLuces, "NBGestorEscena::consumidoresLuces");
	propiedadesCapa.consumidoresEspejos				= new(ENMemoriaTipo_General) AUArregloNativoMutableP<IEscenaConsumidorEspejos*>();	NB_DEFINE_NOMBRE_PUNTERO(propiedadesCapa.consumidoresEspejos, "NBGestorEscena::consumidoresEspejos");
	propiedadesCapa.consumidoresCuerdas				= new(ENMemoriaTipo_General) AUArregloNativoMutableP<IEscenaConsumidorCuerdas*>();	NB_DEFINE_NOMBRE_PUNTERO(propiedadesCapa.consumidoresCuerdas, "NBGestorEscena::consumidoresCuerdas");
	propiedadesCapa.consumidoresPreModeloGL			= new(ENMemoriaTipo_General) AUArregloNativoMutableP<IEscenaConsumidorPreModeloGL*>();	NB_DEFINE_NOMBRE_PUNTERO(propiedadesCapa.consumidoresPreModeloGL, "NBGestorEscena::consumidoresPreModeloGL");
	propiedadesCapa.colorLuzAmbiente				= colorLuzAmbiente;
	NBMATRIZ_ESTABLECER_IDENTIDAD(propiedadesCapa.matrizCapa);
	{
		NBMatriz matrizInversaCapa; NBMATRIZ_INVERSA(matrizInversaCapa, propiedadesCapa.matrizCapa)
		NBCajaAABB sceneBox; NBMATRIZ_MULTIPLICAR_CAJAAABB(sceneBox, matrizInversaCapa, grp->cajaProyeccion);
		propiedadesCapa.root->setSceneBox(NBST_P(STNBRectI, (SI32)scn->puertoDeVision.x, (SI32)scn->puertoDeVision.y, (SI32)scn->puertoDeVision.ancho, (SI32)scn->puertoDeVision.alto ), NBST_P(STNBAABox, sceneBox.xMin, sceneBox.xMax, sceneBox.yMin, sceneBox.yMax ));
	}
	//PENDIENTE: optimizacion, convertir "propiedadesCapa.sombrasPorFuenteIluminacion" en un arregloCache,
	//Asi se evitara que se creen y destruyan los arreglos cache a medida que se agregan y quitan los objetos capas.
	//
	{
		scn->sceneMask |= (AUOBJETOESCENA_BIT_MATRIX_LOCAL_CHANGED | AUOBJETOESCENA_BIT_MODEL_LOCAL_CHANGED);
		grp->capas->agregarElemento(propiedadesCapa);
		//objetoEscena->privActualizarNivelEnEscena(iEscena, idGrupo, propiedadesCapa.idCapa); //ToDo: remove: not necesary "root" container will update this value
	}
	//PRINTF_INFO("Capa agregrada %lu retenciones(%d)\n", (unsigned long)objetoEscena, objetoEscena->conteoReferencias());
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

void NBGestorEscena::quitarObjetoCapa(SI32 iEscena, AUEscenaContenedor* objetoEscena){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::quitarObjetoCapa")
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	NBASSERT(scn->registroOcupado)
	NBASSERT(!_bufferDatosBloqueado[_indiceBufferDatosEscribir]);
	//NBASSERT(!scn->debugBloqueadoActualizandoModelos)
	//PRINTF_INFO("Capa quitando %lu retenciones(%d)\n", (unsigned long)objetoEscena, objetoEscena->conteoReferencias());
	SI32 iGrp; for(iGrp = 0; iGrp < ENGestorEscenaGrupo_Conteo; iGrp++){
		STGestorEscenaGrupo* grupoCapas = &(scn->gruposCapas[iGrp]);
		//Remove from layer array
		{
			SI32 i; for(i = grupoCapas->capas->conteo - 1; i >= 0; i--){
				STGestorEscenaCapa* datosCapa = &(grupoCapas->capas->elemento[i]);
				if(datosCapa->objEscena == objetoEscena){
					NBASSERT(datosCapa->root->hijos()->indiceDe(objetoEscena) == 0) //Must be the only child
					datosCapa->root->vaciar();
					datosCapa->root->privActualizarNivelEnEscena(-1, 0, -1);
					datosCapa->root->liberar(NB_RETENEDOR_NULL);
					objetoEscena->liberar(NB_RETENEDOR_NULL);
					//
					UI16 iLuz;
					for(iLuz=0; iLuz<datosCapa->fuentesIluminacion->conteo; iLuz++){
						NBFuenteIluminacion* datosLuz = &datosCapa->fuentesIluminacion->elemento[iLuz];
						if(datosLuz->texturaLuz != NULL) datosLuz->texturaLuz->liberar(NB_RETENEDOR_NULL);
					}
					datosCapa->fuentesIluminacion->liberar(NB_RETENEDOR_NULL);
					datosCapa->fuentesEspejo->liberar(NB_RETENEDOR_NULL);
					datosCapa->productoresSombras->liberar(NB_RETENEDOR_NULL);
					datosCapa->descriptoresCuerdas->liberar(NB_RETENEDOR_NULL);
					datosCapa->consumidoresLuces->liberar(NB_RETENEDOR_NULL);
					datosCapa->consumidoresEspejos->liberar(NB_RETENEDOR_NULL);
					datosCapa->consumidoresCuerdas->liberar(NB_RETENEDOR_NULL);
					datosCapa->consumidoresPreModeloGL->liberar(NB_RETENEDOR_NULL);
					//
					grupoCapas->capas->quitarElementoEnIndice(i);
					scn->sceneMask |= (AUOBJETOESCENA_BIT_MATRIX_LOCAL_CHANGED | AUOBJETOESCENA_BIT_MODEL_LOCAL_CHANGED);
				}
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

// ENLACES DE CAPAS

void NBGestorEscena::enlaceCapaAgregar(SI16 iEscena, SI16 iEscenaCopiar, SI16 idCapaCopiar){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::enlaceCapaAgregar")
	NBASSERT(iEscena != iEscenaCopiar);
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	NBASSERT(iEscenaCopiar >= 0 && iEscenaCopiar < NBGESTORESCENA_MAX_ESCENAS);
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	STGestorEscenaEscena* scnCpy = &_escenas[iEscenaCopiar];
	NBASSERT(scn->registroOcupado);
	NBASSERT(scnCpy->registroOcupado);
	NBASSERT(!_bufferDatosBloqueado[_indiceBufferDatosEscribir]);
	//NBASSERT(!scn->debugBloqueadoActualizandoModelos)
	NBASSERT(!scnCpy->debugBloqueadoActualizandoModelos)
	STGestorEscenaEnlaceCapa datosEnlace;
	datosEnlace.iEscena	= iEscenaCopiar;
	datosEnlace.idCapa	= idCapaCopiar;
	scn->capasEnlazadas->agregarElemento(datosEnlace);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

void NBGestorEscena::enlaceCapaQuitar(SI16 iEscena, SI16 iEscenaCopiada, SI16 idCapaCopiada){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::enlaceCapaAgregar")
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	NBASSERT(scn->registroOcupado);
	NBASSERT(!_bufferDatosBloqueado[_indiceBufferDatosEscribir]);
	//NBASSERT(!scn->debugBloqueadoActualizandoModelos)
	SI16 i;
	for(i=scn->capasEnlazadas->conteo - 1; i >= 0; i--){
		STGestorEscenaEnlaceCapa* datosEnlace = &scn->capasEnlazadas->elemento[i];
		if(datosEnlace->iEscena == iEscenaCopiada && datosEnlace->idCapa == idCapaCopiada){
			scn->capasEnlazadas->quitarElementoEnIndice(i);
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

void NBGestorEscena::enlaceCapaVaciar(SI16 iEscena){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::enlaceCapaAgregar")
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	NBASSERT(scn->registroOcupado);
	NBASSERT(!_bufferDatosBloqueado[_indiceBufferDatosEscribir]);
	//NBASSERT(!scn->debugBloqueadoActualizandoModelos)
	scn->capasEnlazadas->vaciar();
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

void NBGestorEscena::moverCapasHaciaEscena(SI32 iEscenaOrigen, SI32 iEscenaDestino, SI32 filtroGrupo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::moverCapasHaciaEscena")
	NBASSERT(iEscenaOrigen != iEscenaDestino);
	NBASSERT(iEscenaOrigen >= 0 && iEscenaOrigen < NBGESTORESCENA_MAX_ESCENAS);
	NBASSERT(iEscenaDestino >= 0 && iEscenaDestino < NBGESTORESCENA_MAX_ESCENAS);
	STGestorEscenaEscena* scnOrg = &_escenas[iEscenaOrigen]; NBASSERT(scnOrg->registroOcupado);
	STGestorEscenaEscena* scnDst = &_escenas[iEscenaDestino]; NBASSERT(scnDst->registroOcupado);
	NBASSERT(!_bufferDatosBloqueado[_indiceBufferDatosEscribir]);
	NBASSERT(!scnOrg->debugBloqueadoActualizandoModelos)
	NBASSERT(!scnDst->debugBloqueadoActualizandoModelos)
	SI32 iGrp; for(iGrp = 0; iGrp < ENGestorEscenaGrupo_Conteo; iGrp++){
		if((filtroGrupo<0 || filtroGrupo == iGrp) && iGrp != ENGestorEscenaGrupo_DebugFrontal){
			STGestorEscenaGrupo* grupoCapaOrigen = &(scnOrg->gruposCapas[iGrp]);
			while(grupoCapaOrigen->capas->conteo != 0){
				AUEscenaContenedor* objEscena = grupoCapaOrigen->capas->elemento[0].objEscena;
				objEscena->retener(NB_RETENEDOR_NULL);
				NBColor8 colorLuzAmbiente = grupoCapaOrigen->capas->elemento[0].colorLuzAmbiente;
				NBGestorEscena::quitarObjetoCapa(iEscenaOrigen, objEscena);
				NBGestorEscena::agregarObjetoCapa(iEscenaDestino, (ENGestorEscenaGrupo)iGrp, objEscena, colorLuzAmbiente);
				objEscena->liberar(NB_RETENEDOR_NULL);
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

void NBGestorEscena::copiarCajasDeGrupos(SI32 iEscenaOrigen, SI32 iEscenaDestino, SI32 filtroGrupo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::copiarCajasDeGrupos")
	NBASSERT(iEscenaOrigen != iEscenaDestino);
	NBASSERT(iEscenaOrigen >= 0 && iEscenaOrigen < NBGESTORESCENA_MAX_ESCENAS);
	NBASSERT(iEscenaDestino >= 0 && iEscenaDestino < NBGESTORESCENA_MAX_ESCENAS);
	const STGestorEscenaEscena* scnOrg = &_escenas[iEscenaOrigen]; NBASSERT(scnOrg->registroOcupado);
	STGestorEscenaEscena* scnDst = &_escenas[iEscenaDestino]; NBASSERT(scnDst->registroOcupado);
	NBASSERT(!_bufferDatosBloqueado[_indiceBufferDatosEscribir]);
	NBASSERT(!scnOrg->debugBloqueadoActualizandoModelos)
	NBASSERT(!scnDst->debugBloqueadoActualizandoModelos)
	SI32 iGrp; for(iGrp = 0; iGrp < ENGestorEscenaGrupo_Conteo; iGrp++){
		if((filtroGrupo<0 || filtroGrupo == iGrp) && iGrp != ENGestorEscenaGrupo_DebugFrontal){
			const STGestorEscenaGrupo* grupoCapaOrigen = &(scnOrg->gruposCapas[iGrp]);
			STGestorEscenaGrupo* grupoRenderDestino = &(scnDst->gruposCapas[iGrp]);
			if(!NBCAJAAABB_IS_EQUAL(grupoRenderDestino->cajaProyeccion, grupoCapaOrigen->cajaProyeccion)){
				grupoRenderDestino->cajaProyeccion = grupoCapaOrigen->cajaProyeccion;
				scnDst->sceneMask |= AUOBJETOESCENA_BIT_MODEL_LOCAL_CHANGED;
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

void NBGestorEscena::habilitarObjetoCapa(SI32 iEscena, AUEscenaContenedor* objetoEscena){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::habilitarObjetoCapa")
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	//NBASSERT(!_bufferDatosBloqueado[_indiceBufferDatosEscribir]);
	//NBASSERT(!scn->debugBloqueadoActualizandoModelos)
	SI32 iGrp; for(iGrp = 0; iGrp < ENGestorEscenaGrupo_Conteo; iGrp++){
		STGestorEscenaGrupo* grupoCapa = &(scn->gruposCapas[iGrp]);
		SI32 i; for(i = grupoCapa->capas->conteo - 1; i >= 0; i--){
			STGestorEscenaCapa* datosCapa = &(grupoCapa->capas->elemento[i]);
			if(datosCapa->objEscena == objetoEscena){
				if(!datosCapa->habilitada){
					datosCapa->habilitada = true;
					scn->sceneMask |= AUOBJETOESCENA_BIT_MODEL_LOCAL_CHANGED;
				}
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

void NBGestorEscena::deshabilitarObjetoCapa(SI32 iEscena, AUEscenaContenedor* objetoEscena){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::deshabilitarObjetoCapa")
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	//NBASSERT(!_bufferDatosBloqueado[_indiceBufferDatosEscribir]);
	//NBASSERT(!scn->debugBloqueadoActualizandoModelos)
	SI32 iGrp; for(iGrp = 0; iGrp < ENGestorEscenaGrupo_Conteo; iGrp++){
		STGestorEscenaGrupo* grupoCapa = &(scn->gruposCapas[iGrp]);
		SI32 i;
		for(i=grupoCapa->capas->conteo - 1; i >= 0; i--){
			STGestorEscenaCapa* datosCapa = &(grupoCapa->capas->elemento[i]);
			if(datosCapa->objEscena == objetoEscena){
				if(datosCapa->habilitada){
					datosCapa->habilitada = false;
					scn->sceneMask |= AUOBJETOESCENA_BIT_MODEL_LOCAL_CHANGED;
				}
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

const STGestorEscenaCapa* NBGestorEscena::propiedadesDeCapa(SI32 iEscena, AUEscenaContenedor* objetoEscena){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::propiedadesDeCapa")
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	//NBASSERT(!_bufferDatosBloqueado[_indiceBufferDatosEscribir]);
	STGestorEscenaCapa* propsCapa = NULL;
	SI32 iGrp; for(iGrp = 0; iGrp < ENGestorEscenaGrupo_Conteo && propsCapa == NULL; iGrp++){
		STGestorEscenaGrupo* grupoCapa = &(scn->gruposCapas[iGrp]);
		SI32 i;
		for(i=grupoCapa->capas->conteo - 1; i >= 0; i--){
			STGestorEscenaCapa* datosCapa = &(grupoCapa->capas->elemento[i]);
			if(datosCapa->objEscena == objetoEscena){
				propsCapa = datosCapa;
				AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO_ESCENAS(propsCapa)
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
	return propsCapa;
}

const STGestorEscenaCapa* NBGestorEscena::propiedadesDeCapa(SI32 iEscena, SI32 idCapa){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::propiedadesDeCapa")
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	//NBASSERT(!_bufferDatosBloqueado[_indiceBufferDatosEscribir]);
	STGestorEscenaCapa* propsCapa = NULL;
	SI32 iGrp; for(iGrp = 0; iGrp < ENGestorEscenaGrupo_Conteo && propsCapa == NULL; iGrp++){
		STGestorEscenaGrupo* grupoCapa = &(scn->gruposCapas[iGrp]);
		SI32 i;
		for(i=grupoCapa->capas->conteo - 1; i >= 0; i--){
			STGestorEscenaCapa* datosCapa = &(grupoCapa->capas->elemento[i]);
			if(datosCapa->idCapa == idCapa){
				propsCapa = datosCapa;
				AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO_ESCENAS(propsCapa)
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
	return propsCapa;
}

NBMatriz NBGestorEscena::matrizDeCapa(const SI16 iEscena, const AUEscenaContenedor* objetoEscena){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::matrizDeCapa")
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	NBMatriz matriz; NBMATRIZ_ESTABLECER_IDENTIDAD(matriz)
	SI32 iGrp; for(iGrp = 0; iGrp < ENGestorEscenaGrupo_Conteo; iGrp++){
		STGestorEscenaGrupo* grupoCapa = &(scn->gruposCapas[iGrp]);
		SI32 i;
		for(i=grupoCapa->capas->conteo - 1; i >= 0; i--){
			STGestorEscenaCapa* datosCapa = &(grupoCapa->capas->elemento[i]);
			if(datosCapa->objEscena == objetoEscena){
				matriz = datosCapa->matrizCapa;
				AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO_ESCENAS(matriz)
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
	return matriz;
}

NBMatriz NBGestorEscena::matrizDeCapa(const SI16 iEscena, const SI16 idCapa){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::matrizDeCapa")
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	NBMatriz matriz; NBMATRIZ_ESTABLECER_IDENTIDAD(matriz)
	SI32 iGrp; for(iGrp = 0; iGrp < ENGestorEscenaGrupo_Conteo; iGrp++){
		STGestorEscenaGrupo* grupoCapa = &(scn->gruposCapas[iGrp]);
		SI32 i; for(i=grupoCapa->capas->conteo - 1; i >= 0; i--){
			STGestorEscenaCapa* datosCapa = &(grupoCapa->capas->elemento[i]);
			if(datosCapa->idCapa == idCapa){
				matriz = datosCapa->matrizCapa;
				AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO_ESCENAS(matriz)
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
	return matriz;
}

NBColor8 NBGestorEscena::colorLuzAmbienteDeCapa(const SI16 iEscena, const AUEscenaContenedor* objetoEscena){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::colorLuzAmbienteDeCapa")
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	NBColor8 colorLuzAmb; NBCOLOR_ESTABLECER(colorLuzAmb, 255, 255, 255, 255);
	SI32 iGrp; for(iGrp = 0; iGrp < ENGestorEscenaGrupo_Conteo; iGrp++){
		STGestorEscenaGrupo* grupoCapa = &(scn->gruposCapas[iGrp]);
		SI32 i;
		for(i=grupoCapa->capas->conteo - 1; i >= 0; i--){
			STGestorEscenaCapa* datosCapa = &(grupoCapa->capas->elemento[i]);
			if(datosCapa->objEscena == objetoEscena){
				colorLuzAmb = datosCapa->colorLuzAmbiente;
				AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO_ESCENAS(colorLuzAmb)
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
	return colorLuzAmb;
}

NBColor8 NBGestorEscena::colorLuzAmbienteDeCapa(const SI16 iEscena, const SI16 idCapa){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::colorLuzAmbienteDeCapa")
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	NBColor8 colorLuzAmb; NBCOLOR_ESTABLECER(colorLuzAmb, 255, 255, 255, 255);
	SI32 iGrp; for(iGrp = 0; iGrp < ENGestorEscenaGrupo_Conteo; iGrp++){
		STGestorEscenaGrupo* grupoCapa = &(scn->gruposCapas[iGrp]);
		SI32 i;
		for(i=grupoCapa->capas->conteo - 1; i >= 0; i--){
			STGestorEscenaCapa* datosCapa = &(grupoCapa->capas->elemento[i]);
			if(datosCapa->idCapa == idCapa){
				colorLuzAmb = datosCapa->colorLuzAmbiente;
				AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO_ESCENAS(colorLuzAmb)
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
	return colorLuzAmb;
}

void NBGestorEscena::establecerMatrizDeCapa(SI32 iEscena, AUEscenaContenedor* objetoEscena, NBMatriz matrizCapa){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::establecerMatrizDeCapa")
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	SI32 iGrp; for(iGrp = 0; iGrp < ENGestorEscenaGrupo_Conteo; iGrp++){
		STGestorEscenaGrupo* grupoCapa = &(scn->gruposCapas[iGrp]);
		SI32 i; for(i=grupoCapa->capas->conteo - 1; i >= 0; i--){
			STGestorEscenaCapa* datosCapa = &(grupoCapa->capas->elemento[i]);
			if(datosCapa->objEscena == objetoEscena){
				if(!NBMATRIZ_IS_EQUAL(datosCapa->matrizCapa, matrizCapa)){
					datosCapa->matrizCapa = matrizCapa;
					{
						NBMatriz matrizInversaCapa; NBMATRIZ_INVERSA(matrizInversaCapa, matrizCapa)
						NBCajaAABB sceneBox; NBMATRIZ_MULTIPLICAR_CAJAAABB(sceneBox, matrizInversaCapa, grupoCapa->cajaProyeccion);
						datosCapa->root->setSceneBox(NBST_P(STNBRectI, (SI32)scn->puertoDeVision.x, (SI32)scn->puertoDeVision.y, (SI32)scn->puertoDeVision.ancho, (SI32)scn->puertoDeVision.alto ), NBST_P(STNBAABox, sceneBox.xMin, sceneBox.xMax, sceneBox.yMin, sceneBox.yMax ));
					}
					scn->sceneMask |= AUOBJETOESCENA_BIT_MODEL_LOCAL_CHANGED;
				}
				AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO_ESCENAS()
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

void NBGestorEscena::establecerMatrizDeCapa(SI32 iEscena, SI32 idCapa, NBMatriz matrizCapa){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::establecerMatrizDeCapa")
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	SI32 iGrp; for(iGrp = 0; iGrp < ENGestorEscenaGrupo_Conteo; iGrp++){
		STGestorEscenaGrupo* grupoCapa = &(scn->gruposCapas[iGrp]);
		SI32 i; for(i=grupoCapa->capas->conteo - 1; i >= 0; i--){
			STGestorEscenaCapa* datosCapa = &(grupoCapa->capas->elemento[i]);
			if(datosCapa->idCapa == idCapa){
				if(!NBMATRIZ_IS_EQUAL(datosCapa->matrizCapa, matrizCapa)){
					datosCapa->matrizCapa = matrizCapa;
					{
						NBMatriz matrizInversaCapa; NBMATRIZ_INVERSA(matrizInversaCapa, matrizCapa)
						NBCajaAABB sceneBox; NBMATRIZ_MULTIPLICAR_CAJAAABB(sceneBox, matrizInversaCapa, grupoCapa->cajaProyeccion);
						datosCapa->root->setSceneBox(NBST_P(STNBRectI, (SI32)scn->puertoDeVision.x, (SI32)scn->puertoDeVision.y, (SI32)scn->puertoDeVision.ancho, (SI32)scn->puertoDeVision.alto ), NBST_P(STNBAABox, sceneBox.xMin, sceneBox.xMax, sceneBox.yMin, sceneBox.yMax ));
					}
					scn->sceneMask |= AUOBJETOESCENA_BIT_MODEL_LOCAL_CHANGED;
				}
				AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO_ESCENAS()
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

void NBGestorEscena::establecerColorLuzAmbienteCapa(const SI16 iEscena, const AUEscenaContenedor* objetoEscena, const NBColor8 &colorLuzAmbiente){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::establecerColorLuzAmbienteCapa")
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	SI32 iGrp; for(iGrp = 0; iGrp < ENGestorEscenaGrupo_Conteo; iGrp++){
		STGestorEscenaGrupo* grupoCapa = &(scn->gruposCapas[iGrp]);
		SI32 i;
		for(i=grupoCapa->capas->conteo - 1; i >= 0; i--){
			STGestorEscenaCapa* datosCapa = &(grupoCapa->capas->elemento[i]);
			if(datosCapa->objEscena == objetoEscena){
				datosCapa->colorLuzAmbiente = colorLuzAmbiente;
				AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO_ESCENAS()
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

void NBGestorEscena::establecerColorLuzAmbienteCapa(const SI16 iEscena, const AUEscenaContenedor* objetoEscena, const UI8 colorLuzAmbienteR, const UI8 colorLuzAmbienteG, const UI8 colorLuzAmbienteB){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::establecerColorLuzAmbienteCapa")
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	SI32 iGrp; for(iGrp = 0; iGrp < ENGestorEscenaGrupo_Conteo; iGrp++){
		STGestorEscenaGrupo* grupoCapa = &(scn->gruposCapas[iGrp]);
		SI32 i;
		for(i=grupoCapa->capas->conteo - 1; i >= 0; i--){
			STGestorEscenaCapa* datosCapa = &(grupoCapa->capas->elemento[i]);
			if(datosCapa->objEscena == objetoEscena){
				if(datosCapa->colorLuzAmbiente.r != colorLuzAmbienteR || datosCapa->colorLuzAmbiente.g != colorLuzAmbienteG || datosCapa->colorLuzAmbiente.b != colorLuzAmbienteB){
					datosCapa->colorLuzAmbiente.r = colorLuzAmbienteR;
					datosCapa->colorLuzAmbiente.g = colorLuzAmbienteG;
					datosCapa->colorLuzAmbiente.b = colorLuzAmbienteB;
					scn->sceneMask |= AUOBJETOESCENA_BIT_MODEL_LOCAL_CHANGED;
				}
				AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO_ESCENAS()
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

void NBGestorEscena::establecerColorLuzAmbienteCapa(const SI16 iEscena, const SI16 idCapa, const NBColor8 &colorLuzAmbiente){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::establecerColorLuzAmbienteCapa")
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	SI32 iGrp; for(iGrp = 0; iGrp < ENGestorEscenaGrupo_Conteo; iGrp++){
		STGestorEscenaGrupo* grupoCapa = &(scn->gruposCapas[iGrp]);
		SI32 i;
		for(i=grupoCapa->capas->conteo - 1; i >= 0; i--){
			STGestorEscenaCapa* datosCapa = &(grupoCapa->capas->elemento[i]);
			if(datosCapa->idCapa == idCapa){
				if(datosCapa->colorLuzAmbiente.r != colorLuzAmbiente.r || datosCapa->colorLuzAmbiente.g != colorLuzAmbiente.g || datosCapa->colorLuzAmbiente.b != colorLuzAmbiente.b || datosCapa->colorLuzAmbiente.a != colorLuzAmbiente.a){
					datosCapa->colorLuzAmbiente = colorLuzAmbiente;
					scn->sceneMask |= AUOBJETOESCENA_BIT_MODEL_LOCAL_CHANGED;
				}
				AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO_ESCENAS()
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

void NBGestorEscena::establecerColorLuzAmbienteCapa(const SI16 iEscena, const SI16 idCapa, const UI8 colorLuzAmbienteR, const UI8 colorLuzAmbienteG, const UI8 colorLuzAmbienteB){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::establecerColorLuzAmbienteCapa")
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	SI32 iGrp; for(iGrp = 0; iGrp < ENGestorEscenaGrupo_Conteo; iGrp++){
		STGestorEscenaGrupo* grupoCapa = &(scn->gruposCapas[iGrp]);
		SI32 i;
		for(i=grupoCapa->capas->conteo - 1; i >= 0; i--){
			STGestorEscenaCapa* datosCapa = &(grupoCapa->capas->elemento[i]);
			if(datosCapa->idCapa == idCapa){
				if(datosCapa->colorLuzAmbiente.r != colorLuzAmbienteR || datosCapa->colorLuzAmbiente.g != colorLuzAmbienteG || datosCapa->colorLuzAmbiente.b != colorLuzAmbienteB){
					datosCapa->colorLuzAmbiente.r = colorLuzAmbienteR;
					datosCapa->colorLuzAmbiente.g = colorLuzAmbienteG;
					datosCapa->colorLuzAmbiente.b = colorLuzAmbienteB;
					scn->sceneMask |= AUOBJETOESCENA_BIT_MODEL_LOCAL_CHANGED;
				}
				AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO_ESCENAS()
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

void NBGestorEscena::vaciarGrupo(SI32 iEscena, ENGestorEscenaGrupo idGrupo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::vaciarGrupo")
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	NBASSERT(idGrupo >= 0 && idGrupo < ENGestorEscenaGrupo_Conteo)
	NBASSERT(!_bufferDatosBloqueado[_indiceBufferDatosEscribir]);
	//NBASSERT(!scn->debugBloqueadoActualizandoModelos)
	STGestorEscenaGrupo* grupoCapa = &(scn->gruposCapas[idGrupo]);
	SI32 i; for(i = grupoCapa->capas->conteo - 1; i >= 0; i--){
		NBGestorEscena::quitarObjetoCapa(iEscena, grupoCapa->capas->elemento[i].objEscena);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

void NBGestorEscena::vaciarGrupos(SI32 iEscena){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::vaciarGrupos")
	NBASSERT(!_bufferDatosBloqueado[_indiceBufferDatosEscribir]);
	//NBASSERT(!scn->debugBloqueadoActualizandoModelos)
	SI32 iGrupo; for(iGrupo = ENGestorEscenaGrupo_Conteo - 1; iGrupo >= 0; iGrupo--){
		NBGestorEscena::vaciarGrupo(iEscena, (ENGestorEscenaGrupo)iGrupo);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

void NBGestorEscena::normalizaCajaProyeccionEscena(SI32 iEscena){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::normalizaCajaProyeccionEscena")
	//NBASSERT(!_bufferDatosBloqueado[_indiceBufferDatosEscribir]);
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	NBASSERT(scn->registroOcupado)
	//NBASSERT(!scn->debugBloqueadoActualizandoModelos)
	const NBRectangulo puertoDeVision = scn->puertoDeVision;
	float escalaParaHD	= scn->escalaParaHD;
	if(_frameBuffersGL[scn->iFramebufferEscena].renderbufferOriginal != NULL){
		escalaParaHD	= _frameBuffersGL[scn->iFramebufferEscena].renderbufferOriginal->escalaParaHD();
	} else if(_frameBuffersGL[scn->iFramebufferEscena].texturaOriginal != NULL){
		escalaParaHD	= _frameBuffersGL[scn->iFramebufferEscena].texturaOriginal->escalaParaHD();
	}
	//NBGestorEscena::establecerCajaProyeccion(iEscena, (puertoDeVision.ancho/-2.0f)*escalaParaHD, (puertoDeVision.ancho/2.0f)*escalaParaHD, (puertoDeVision.alto/-2.0f)*escalaParaHD, (puertoDeVision.alto/2.0f)*escalaParaHD);
	{
		SI32 iGrp; for(iGrp = 0; iGrp < ENGestorEscenaGrupo_Conteo; iGrp++){
			STGestorEscenaGrupo* grp = &scn->gruposCapas[iGrp];
			NBCajaAABB cajaProyeccionGrupo;
			cajaProyeccionGrupo.xMin = puertoDeVision.ancho * -0.5f * escalaParaHD;
			cajaProyeccionGrupo.xMax = puertoDeVision.ancho * 0.5f * escalaParaHD;
			cajaProyeccionGrupo.yMin = puertoDeVision.alto * -0.5f * escalaParaHD;
			cajaProyeccionGrupo.yMax = puertoDeVision.alto * 0.5f * escalaParaHD;
			NBASSERT(cajaProyeccionGrupo.xMin == cajaProyeccionGrupo.xMin); //Es Nan
			NBASSERT(cajaProyeccionGrupo.xMax == cajaProyeccionGrupo.xMax); //Es Nan
			NBASSERT(cajaProyeccionGrupo.yMin == cajaProyeccionGrupo.yMin); //Es Nan
			NBASSERT(cajaProyeccionGrupo.yMax == cajaProyeccionGrupo.yMax); //Es Nan
			if(!NBCAJAAABB_IS_EQUAL(grp->cajaProyeccion, cajaProyeccionGrupo)){
				grp->cajaProyeccion = cajaProyeccionGrupo;
				//Update roots
				{
					UI32 iCapa, iCapaConteo = grp->capas->conteo;
					for(iCapa=0; iCapa<iCapaConteo; iCapa++){
						STGestorEscenaCapa* propiedadesCapa = &(grp->capas->elemento[iCapa]);
						NBMatriz matrizInversaCapa; NBMATRIZ_INVERSA(matrizInversaCapa, propiedadesCapa->matrizCapa)
						NBCajaAABB sceneBox; NBMATRIZ_MULTIPLICAR_CAJAAABB(sceneBox, matrizInversaCapa, scn->gruposCapas[iGrp].cajaProyeccion);
						propiedadesCapa->root->setSceneBox(NBST_P(STNBRectI, (SI32)scn->puertoDeVision.x, (SI32)scn->puertoDeVision.y, (SI32)scn->puertoDeVision.ancho, (SI32)scn->puertoDeVision.alto ), NBST_P(STNBAABox, sceneBox.xMin, sceneBox.xMax, sceneBox.yMin, sceneBox.yMax ));
					}
				}
				scn->sceneMask |= AUOBJETOESCENA_BIT_MODEL_LOCAL_CHANGED;
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

void NBGestorEscena::normalizaMatricesCapasEscena(SI32 iEscena){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::normalizaMatricesCapasEscena")
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	NBASSERT(scn->registroOcupado)
	NBASSERT(!_bufferDatosBloqueado[_indiceBufferDatosEscribir]);
	//NBASSERT(!scn->debugBloqueadoActualizandoModelos)
	SI32 iGrp; for(iGrp = 0; iGrp < ENGestorEscenaGrupo_Conteo; iGrp++){
		AUArregloNativoP<STGestorEscenaCapa>* capas =  scn->gruposCapas[iGrp].capas;
		SI32 iCapa, iCapaConteo = capas->conteo;
		for(iCapa = 0; iCapa<iCapaConteo; iCapa++){
			STGestorEscenaCapa* propiedadesCapa = &(capas->elemento[iCapa]);
			if(!NBMATRIZ_IS_IDENTITY(propiedadesCapa->matrizCapa)){
				NBMATRIZ_ESTABLECER_IDENTIDAD(propiedadesCapa->matrizCapa)
				//Update root
				NBMatriz matrizInversaCapa; NBMATRIZ_INVERSA(matrizInversaCapa, propiedadesCapa->matrizCapa)
				NBCajaAABB sceneBox; NBMATRIZ_MULTIPLICAR_CAJAAABB(sceneBox, matrizInversaCapa, scn->gruposCapas[iGrp].cajaProyeccion);
				propiedadesCapa->root->setSceneBox(NBST_P(STNBRectI, (SI32)scn->puertoDeVision.x, (SI32)scn->puertoDeVision.y, (SI32)scn->puertoDeVision.ancho, (SI32)scn->puertoDeVision.alto ), NBST_P(STNBAABox, sceneBox.xMin, sceneBox.xMax, sceneBox.yMin, sceneBox.yMax ));
				//
				scn->sceneMask |= AUOBJETOESCENA_BIT_MODEL_LOCAL_CHANGED;
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

NBCajaAABB NBGestorEscena::cajaProyeccionGrupo(SI32 iEscena, ENGestorEscenaGrupo idGrupo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::cajaProyeccionGrupo")
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	NBCajaAABB cajaProyeccionGrupo = scn->gruposCapas[idGrupo].cajaProyeccion;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
	return cajaProyeccionGrupo;
}

NBTamano NBGestorEscena::escalaCajaProyeccionGrupo(SI32 iEscena, ENGestorEscenaGrupo idGrupo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::escalaCajaProyeccionGrupo")
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	NBASSERT(scn->registroOcupado)
	NBASSERT(idGrupo >= 0 && idGrupo < ENGestorEscenaGrupo_Conteo)
	NBTamano escalaCaja;
	NBRectangulo puertoVision	= scn->puertoDeVision;
	NBCajaAABB cajaGrupo		= scn->gruposCapas[idGrupo].cajaProyeccion;
	NBTamano tamCajaGrupo;		NBCAJAAABB_TAMANO(tamCajaGrupo, cajaGrupo);
	escalaCaja.ancho			= tamCajaGrupo.ancho / puertoVision.ancho;
	escalaCaja.alto				= tamCajaGrupo.alto / puertoVision.alto;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
	return escalaCaja;
}

NBPunto NBGestorEscena::coordenadaPuertoAGrupo(SI32 iEscena, ENGestorEscenaGrupo idGrupo, float xPuertoVision, float yPuertoVision){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::coordenadaPuertoAGrupo")
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	NBASSERT(scn->registroOcupado)
	NBASSERT(idGrupo >= 0 && idGrupo < ENGestorEscenaGrupo_Conteo)
	NBRectangulo prtoVision	= scn->puertoDeVision;
	NBCajaAABB caja			= scn->gruposCapas[idGrupo].cajaProyeccion;
	NBTamano tamano;		NBCAJAAABB_TAMANO(tamano, caja);
	NBTamano unidsGLPorPx;
	unidsGLPorPx.ancho 		= (tamano.ancho / prtoVision.ancho);
	unidsGLPorPx.alto 		= (tamano.alto / prtoVision.alto);
	//transformar de coordenadas de entrada a coordenadas en escena y GUI
	NBPunto coordsTransf;
	coordsTransf.x 			= caja.xMin + (xPuertoVision * unidsGLPorPx.ancho);
	coordsTransf.y 			= caja.yMax - (yPuertoVision * unidsGLPorPx.alto);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
	return coordsTransf;
}


NBPunto NBGestorEscena::coordenadaGrupoAPuerto(SI32 iEscena, ENGestorEscenaGrupo idGrupo, float xGrupo, float yGrupo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::coordenadaGrupoAPuerto")
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	NBASSERT(scn->registroOcupado)
	NBASSERT(idGrupo >= 0 && idGrupo < ENGestorEscenaGrupo_Conteo)
	NBRectangulo prtoVision	= scn->puertoDeVision;
	NBCajaAABB caja			= scn->gruposCapas[idGrupo].cajaProyeccion;
	NBTamano tamano;		NBCAJAAABB_TAMANO(tamano, caja);
	NBTamano unidsGLPorPx;
	unidsGLPorPx.ancho 		= (tamano.ancho / prtoVision.ancho);
	unidsGLPorPx.alto 		= (tamano.alto / prtoVision.alto);
	//transformar de coordenadas de entrada a coordenadas en escena y GUI
	NBPunto coordsTransf;
	coordsTransf.x 			= ((xGrupo - caja.xMin) / unidsGLPorPx.ancho);
	coordsTransf.y 			= ((tamano.alto - (yGrupo - caja.yMin)) / unidsGLPorPx.alto);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
	return coordsTransf;
}

float NBGestorEscena::anchoGrupoAPuerto(const SI32 iEscena, const ENGestorEscenaGrupo idGrupo, const float ancho){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::anchoGrupoAPuerto")
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	NBASSERT(scn->registroOcupado)
	NBASSERT(idGrupo >= 0 && idGrupo < ENGestorEscenaGrupo_Conteo)
	const float tamano		= scn->gruposCapas[idGrupo].cajaProyeccion.xMax - scn->gruposCapas[idGrupo].cajaProyeccion.xMin;
	float unidsGLPorPx		= (tamano / scn->puertoDeVision.ancho);
	const float r			= ancho / unidsGLPorPx;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
	return r;
}

float NBGestorEscena::altoGrupoAPuerto(const SI32 iEscena, const ENGestorEscenaGrupo idGrupo, const float alto){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::altoGrupoAPuerto")
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	NBASSERT(scn->registroOcupado)
	NBASSERT(idGrupo >= 0 && idGrupo < ENGestorEscenaGrupo_Conteo)
	const float tamano		= scn->gruposCapas[idGrupo].cajaProyeccion.yMax - scn->gruposCapas[idGrupo].cajaProyeccion.yMin;
	float unidsGLPorPx		= (tamano / scn->puertoDeVision.alto);
	const float r			= alto / unidsGLPorPx;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
	return r;
}

float NBGestorEscena::anchoPuertoAGrupo(const SI32 iEscena, const ENGestorEscenaGrupo idGrupo, const float ancho){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::anchoGrupoAPuerto")
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	NBASSERT(scn->registroOcupado)
	NBASSERT(idGrupo >= 0 && idGrupo < ENGestorEscenaGrupo_Conteo)
	const float tamano		= scn->gruposCapas[idGrupo].cajaProyeccion.xMax - scn->gruposCapas[idGrupo].cajaProyeccion.xMin;
	float unidsGLPorPx		= (tamano / scn->puertoDeVision.ancho);
	const float r			= ancho * unidsGLPorPx;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
	return r;
}

float NBGestorEscena::altoPuertoAGrupo(const SI32 iEscena, const ENGestorEscenaGrupo idGrupo, const float alto){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::altoGrupoAPuerto")
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	NBASSERT(scn->registroOcupado)
	NBASSERT(idGrupo >= 0 && idGrupo < ENGestorEscenaGrupo_Conteo)
	const float tamano		= scn->gruposCapas[idGrupo].cajaProyeccion.yMax - scn->gruposCapas[idGrupo].cajaProyeccion.yMin;
	float unidsGLPorPx		= (tamano / scn->puertoDeVision.alto);
	const float r			= alto * unidsGLPorPx;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
	return r;
}

//########################################
//### AYUDAS PARA PANTALLAS PEQUENAS
//########################################

bool NBGestorEscena::ayudasParaPantallaPequena(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::ayudasParaPantallaPequena")
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
	return _ayudasParaPantallaPequena;
}

void NBGestorEscena::establecerAyudasParaPantallasPequenas(bool ayudasActivas){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::ayudasParaPantallaPequena")
	_ayudasParaPantallaPequena = ayudasActivas;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

//########################################
//### ILUMINACION
//########################################

bool NBGestorEscena::lucesSombrasActivos(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::lucesSombrasActivos")
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
	return _lucesSombrasActivos;
}

void NBGestorEscena::establecerLucesSombrasActivos(bool lucesSombrasActivos){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::establecerLucesSombrasActivos")
	_lucesSombrasActivos = lucesSombrasActivos;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

void NBGestorEscena::agregarFuenteIluminacion(SI32 iEscena, AUEscenaObjeto* objEscena, AUObjeto* objAgrupaSombras, AUTextura* texturaLuz, float alcanceLuz, ENFuenteIlumTipo tipoLuz, bool produceSombras, bool limitesAngulosActivos, float radianesMin, float radianesApertura){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::agregarFuenteIluminacion")
	NBASSERT(objEscena->idCapa >= 0)
	NBASSERT(texturaLuz != NULL)
	//NBASSERT(!scn->debugBloqueadoActualizandoModelos)
	STGestorEscenaCapa* propiedadesDeCapa = NBGestorEscena::privPropiedadesCapaPorID(iEscena, objEscena->idCapa);
	NBASSERT(propiedadesDeCapa != NULL);
	//
	SI32 indiceFuenteIluminacion = -1;
	UI32 iIlum, iIlumConteo = propiedadesDeCapa->fuentesIluminacion->conteo;
	for(iIlum = 0; iIlum < iIlumConteo; iIlum++){
		if(propiedadesDeCapa->fuentesIluminacion->elemento[iIlum].objEscena == objEscena){
			indiceFuenteIluminacion = iIlum;
			break;
		}
	}
	if(indiceFuenteIluminacion != -1){
		PRINTF_WARNING("NBGestorEscena, se ha intentado agregar una iluminacion por segunda vez.\n");
		NBASSERT(false)
	} else {
		NBFuenteIluminacion nuevaFuenteIlumnacion;
		nuevaFuenteIlumnacion.objEscena						= objEscena;
		nuevaFuenteIlumnacion.objAgrupaSombras				= objAgrupaSombras;
		nuevaFuenteIlumnacion.texturaLuz					= texturaLuz; texturaLuz->retener(NB_RETENEDOR_NULL);
		nuevaFuenteIlumnacion.alcanceLuzzz					= alcanceLuz;
		nuevaFuenteIlumnacion.radianesMin					= radianesMin;
		nuevaFuenteIlumnacion.radianesApertura				= radianesApertura; NBASSERT(!limitesAngulosActivos || (radianesApertura >= 0.0f && radianesApertura <= PI))
		nuevaFuenteIlumnacion.tipoIluminacion				= tipoLuz;
		nuevaFuenteIlumnacion.produceSombras				= produceSombras;
		nuevaFuenteIlumnacion.limitesAngulosActivos			= limitesAngulosActivos;
		propiedadesDeCapa->fuentesIluminacion->agregarElemento(nuevaFuenteIlumnacion);
		//
		{
			STGestorEscenaEscena* scn = &_escenas[iEscena];
			scn->sceneMask |= AUOBJETOESCENA_BIT_MODEL_LOCAL_CHANGED;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

void NBGestorEscena::actualizarFuenteIluminacion(SI32 iEscena, AUEscenaObjeto* objEscena, AUObjeto* objAgrupaSombras, AUTextura* texturaLuz, float alcanceLuz, ENFuenteIlumTipo tipoLuz, bool produceSombras, bool limitesAngulosActivos, float radianesMin, float radianesApertura){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::actualizarFuenteIluminacion")
	NBASSERT(objEscena->idCapa >= 0)
	NBASSERT(texturaLuz != NULL)
	//NBASSERT(!scn->debugBloqueadoActualizandoModelos)
	STGestorEscenaCapa* propiedadesDeCapa = NBGestorEscena::privPropiedadesCapaPorID(iEscena, objEscena->idCapa);
	NBASSERT(propiedadesDeCapa != NULL);
	//
	SI32 indiceFuenteIluminacion = -1;
	UI32 iIlum, iIlumConteo = propiedadesDeCapa->fuentesIluminacion->conteo;
	for(iIlum = 0; iIlum < iIlumConteo; iIlum++){
		if(propiedadesDeCapa->fuentesIluminacion->elemento[iIlum].objEscena == objEscena){
			indiceFuenteIluminacion = iIlum;
			break;
		}
	}
	if(indiceFuenteIluminacion == -1){
		PRINTF_WARNING("NBGestorEscena, se ha intentado actualizar una iluminacion que no existe.\n");
		NBASSERT(false)
	} else {
		NBFuenteIluminacion* fuenteLuz			= &(propiedadesDeCapa->fuentesIluminacion->elemento[indiceFuenteIluminacion]);
		fuenteLuz->objAgrupaSombras				= objAgrupaSombras;
		texturaLuz->retener(NB_RETENEDOR_NULL);
		if(fuenteLuz->texturaLuz != NULL) fuenteLuz->texturaLuz->liberar(NB_RETENEDOR_NULL);
		fuenteLuz->texturaLuz					= texturaLuz;
		fuenteLuz->tipoIluminacion				= tipoLuz;
		fuenteLuz->limitesAngulosActivos		= limitesAngulosActivos;
		fuenteLuz->produceSombras				= produceSombras;
		fuenteLuz->alcanceLuzzz					= alcanceLuz;
		fuenteLuz->radianesMin					= radianesMin;
		fuenteLuz->radianesApertura				= radianesApertura; NBASSERT(!limitesAngulosActivos || (radianesApertura >= 0.0f && radianesApertura <= PI))
		//
		{
			STGestorEscenaEscena* scn = &_escenas[iEscena];
			scn->sceneMask |= AUOBJETOESCENA_BIT_MODEL_LOCAL_CHANGED;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

void NBGestorEscena::quitarFuenteIluminacion(SI32 iEscena, AUEscenaObjeto* objEscena){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::quitarFuenteIluminacion")
	NBASSERT(objEscena->idCapa >= 0)
	//NBASSERT(!scn->debugBloqueadoActualizandoModelos)
	STGestorEscenaCapa* propiedadesDeCapa = NBGestorEscena::privPropiedadesCapaPorID(iEscena, objEscena->idCapa);
	NBASSERT(propiedadesDeCapa != NULL);
	//
	SI32 indiceFuenteIluminacion = -1;
	UI32 iIlum, iIlumConteo = propiedadesDeCapa->fuentesIluminacion->conteo;
	for(iIlum=0; iIlum<iIlumConteo; iIlum++){
		if(propiedadesDeCapa->fuentesIluminacion->elemento[iIlum].objEscena == objEscena){
			indiceFuenteIluminacion = iIlum;
			break;
		}
	}
	NBASSERT(indiceFuenteIluminacion != -1)
	if(indiceFuenteIluminacion != -1){
		NBFuenteIluminacion* fuenteLuz			= &(propiedadesDeCapa->fuentesIluminacion->elemento[indiceFuenteIluminacion]);
		if(fuenteLuz->texturaLuz != NULL) fuenteLuz->texturaLuz->liberar(NB_RETENEDOR_NULL);
		propiedadesDeCapa->fuentesIluminacion->quitarElementoEnIndice(indiceFuenteIluminacion);
		//
		{
			STGestorEscenaEscena* scn = &_escenas[iEscena];
			scn->sceneMask |= AUOBJETOESCENA_BIT_MODEL_LOCAL_CHANGED;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

void NBGestorEscena::agregarFuenteEspejo(SI32 iEscena, AUEscenaObjeto* objEscena, float factorReflejoLuz, NBPunto limiteLocalIni, NBPunto limiteLocalFin){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::agregarFuenteEspejo")
	NBASSERT(objEscena->idCapa >= 0)
	//NBASSERT(!scn->debugBloqueadoActualizandoModelos)
	STGestorEscenaCapa* propiedadesDeCapa = NBGestorEscena::privPropiedadesCapaPorID(iEscena, objEscena->idCapa);
	NBASSERT(propiedadesDeCapa != NULL);
	NBASSERT(factorReflejoLuz >= 0.0f && factorReflejoLuz <= 1.0f)
	//
	SI32 indiceFuenteEspejo = -1;
	UI32 iEspejo, iEspejoConteo = propiedadesDeCapa->fuentesEspejo->conteo;
	for(iEspejo = 0; iEspejo < iEspejoConteo; iEspejo++){
		if(propiedadesDeCapa->fuentesEspejo->elemento[iEspejo].objEscena == objEscena){
			indiceFuenteEspejo = iEspejo;
			break;
		}
	}
	if(indiceFuenteEspejo != -1){
		PRINTF_WARNING("NBGestorEscena, se ha intentado agregar una iluminacion por segunda vez.\n");
		NBASSERT(false)
	} else {
		NBFuenteEspejo nuevaFuenteEspejo;
		nuevaFuenteEspejo.objEscena						= objEscena;
		nuevaFuenteEspejo.factorReflejaLuz				= factorReflejoLuz;
		nuevaFuenteEspejo.limitesLocalesEspejo[0]		= limiteLocalIni;
		nuevaFuenteEspejo.limitesLocalesEspejo[1]		= limiteLocalFin;
		propiedadesDeCapa->fuentesEspejo->agregarElemento(nuevaFuenteEspejo);
		//
		{
			STGestorEscenaEscena* scn = &_escenas[iEscena];
			scn->sceneMask |= AUOBJETOESCENA_BIT_MODEL_LOCAL_CHANGED;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

STGestorEscenaTextura* NBGestorEscena::tmpTexturasAtlasRender(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::tmpTexturasAtlasRender")
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
	return _texturasParaRenderizadoGL;
}

void NBGestorEscena::actualizarFuenteEspejo(SI32 iEscena, AUEscenaObjeto* objEscena, float factorReflejoLuz, NBPunto limiteLocalIni, NBPunto limiteLocalFin){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::actualizarFuenteEspejo")
	NBASSERT(objEscena->idCapa >= 0)
	//NBASSERT(!scn->debugBloqueadoActualizandoModelos)
	STGestorEscenaCapa* propiedadesDeCapa = NBGestorEscena::privPropiedadesCapaPorID(iEscena, objEscena->idCapa);
	NBASSERT(propiedadesDeCapa != NULL);
	NBASSERT(factorReflejoLuz >= 0.0f && factorReflejoLuz <= 1.0f)
	//
	SI32 indiceFuenteEspejo = -1;
	UI32 iEspejo, iEspejoConteo = propiedadesDeCapa->fuentesEspejo->conteo;
	for(iEspejo = 0; iEspejo < iEspejoConteo; iEspejo++){
		if(propiedadesDeCapa->fuentesEspejo->elemento[iEspejo].objEscena == objEscena){
			indiceFuenteEspejo = iEspejo;
			break;
		}
	}
	if(indiceFuenteEspejo == -1){
		PRINTF_WARNING("NBGestorEscena, se ha intentado actualizar una iluminacion que no existe.\n");
		NBASSERT(false)
	} else {
		NBFuenteEspejo* fuenteEspejo			= &(propiedadesDeCapa->fuentesEspejo->elemento[indiceFuenteEspejo]);
		fuenteEspejo->factorReflejaLuz			= factorReflejoLuz;
		fuenteEspejo->limitesLocalesEspejo[0]	= limiteLocalIni;
		fuenteEspejo->limitesLocalesEspejo[1]	= limiteLocalFin;
		//
		{
			STGestorEscenaEscena* scn = &_escenas[iEscena];
			scn->sceneMask |= AUOBJETOESCENA_BIT_MODEL_LOCAL_CHANGED;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

void NBGestorEscena::quitarFuenteEspejo(SI32 iEscena, AUEscenaObjeto* objEscena){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::quitarFuenteEspejo")
	NBASSERT(objEscena->idCapa >= 0)
	//NBASSERT(!scn->debugBloqueadoActualizandoModelos)
	STGestorEscenaCapa* propiedadesDeCapa = NBGestorEscena::privPropiedadesCapaPorID(iEscena, objEscena->idCapa);
	NBASSERT(propiedadesDeCapa != NULL);
	//
	SI32 indiceFuenteEspejo = -1;
	UI32 iEspejo, iEspejoConteo = propiedadesDeCapa->fuentesEspejo->conteo;
	for(iEspejo = 0; iEspejo < iEspejoConteo; iEspejo++){
		if(propiedadesDeCapa->fuentesEspejo->elemento[iEspejo].objEscena == objEscena){
			indiceFuenteEspejo = iEspejo;
			break;
		}
	}
	NBASSERT(indiceFuenteEspejo != -1)
	if(indiceFuenteEspejo != -1){
		propiedadesDeCapa->fuentesEspejo->quitarElementoEnIndice(indiceFuenteEspejo);
		//
		{
			STGestorEscenaEscena* scn = &_escenas[iEscena];
			scn->sceneMask |= AUOBJETOESCENA_BIT_MODEL_LOCAL_CHANGED;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

// PRODUCTORES DE SOMBRAS

void NBGestorEscena::agregarProductorSombras(SI32 iEscena, SI32 iCapa, IEscenaProductorSombras* productor){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::agregarProductorSombras")
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	//NBASSERT(!scn->debugBloqueadoActualizandoModelos)
	NBASSERT(productor != NULL);
	STGestorEscenaCapa* propiedadesDeCapa = NBGestorEscena::privPropiedadesCapaPorID(iEscena, iCapa); NBASSERT(propiedadesDeCapa != NULL);
	NBASSERT(propiedadesDeCapa->productoresSombras->indiceDe(productor) == -1) //Se esta intentando agregar dos veces al mismo productor
	propiedadesDeCapa->productoresSombras->agregarElemento(productor);
	//
	{
		STGestorEscenaEscena* scn = &_escenas[iEscena];
		scn->sceneMask |= AUOBJETOESCENA_BIT_MODEL_LOCAL_CHANGED;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

void NBGestorEscena::quitarProductorSombras(SI32 iEscena, SI32 iCapa, IEscenaProductorSombras* productor){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::quitarProductorSombras")
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	//NBASSERT(!scn->debugBloqueadoActualizandoModelos)
	NBASSERT(productor != NULL);
	STGestorEscenaCapa* propiedadesDeCapa = NBGestorEscena::privPropiedadesCapaPorID(iEscena, iCapa); NBASSERT(propiedadesDeCapa != NULL);
	NBASSERT(propiedadesDeCapa->productoresSombras->indiceDe(productor) != -1) //Se esta quitando un productor inexistente
	propiedadesDeCapa->productoresSombras->quitarElemento(productor);
	//
	{
		STGestorEscenaEscena* scn = &_escenas[iEscena];
		scn->sceneMask |= AUOBJETOESCENA_BIT_MODEL_LOCAL_CHANGED;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

// DESCRIPTORES DE CUERDAS

void NBGestorEscena::agregarDescriptorCuerda(SI32 iEscena, SI32 iCapa, ICuerdaCortable* cuerda){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::agregarDescriptorCuerda")
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	//NBASSERT(!scn->debugBloqueadoActualizandoModelos)
	NBASSERT(cuerda != NULL);
	STGestorEscenaCapa* propiedadesDeCapa = NBGestorEscena::privPropiedadesCapaPorID(iEscena, iCapa); NBASSERT(propiedadesDeCapa != NULL);
	NBASSERT(propiedadesDeCapa->descriptoresCuerdas->indiceDe(cuerda) == -1) //Se esta intentando agregar dos veces al mismo productor
	propiedadesDeCapa->descriptoresCuerdas->agregarElemento(cuerda);
	//
	{
		STGestorEscenaEscena* scn = &_escenas[iEscena];
		scn->sceneMask |= AUOBJETOESCENA_BIT_MODEL_LOCAL_CHANGED;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

void NBGestorEscena::quitarDescriptorCuerda(SI32 iEscena, SI32 iCapa, ICuerdaCortable* cuerda){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::quitarDescriptorCuerda")
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	//NBASSERT(!scn->debugBloqueadoActualizandoModelos)
	NBASSERT(cuerda != NULL);
	STGestorEscenaCapa* propiedadesDeCapa = NBGestorEscena::privPropiedadesCapaPorID(iEscena, iCapa); NBASSERT(propiedadesDeCapa != NULL);
	NBASSERT(propiedadesDeCapa->descriptoresCuerdas->indiceDe(cuerda) != -1) //Se esta quitando un productor inexistente
	propiedadesDeCapa->descriptoresCuerdas->quitarElemento(cuerda);
	//
	{
		STGestorEscenaEscena* scn = &_escenas[iEscena];
		scn->sceneMask |= AUOBJETOESCENA_BIT_MODEL_LOCAL_CHANGED;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}


// CONSUMIDORES DE LUCES

void NBGestorEscena::agregarConsumidorLuces(SI32 iEscena, SI32 iCapa, IEscenaConsumidorLuces* consumidor){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::agregarConsumidorLuces")
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	//NBASSERT(!scn->debugBloqueadoActualizandoModelos)
	NBASSERT(consumidor != NULL);
	STGestorEscenaCapa* propiedadesDeCapa = NBGestorEscena::privPropiedadesCapaPorID(iEscena, iCapa); NBASSERT(propiedadesDeCapa != NULL);
	NBASSERT(propiedadesDeCapa->consumidoresLuces->indiceDe(consumidor) == -1) //Se esta intentando agregar dos veces al mismo productor
	propiedadesDeCapa->consumidoresLuces->agregarElemento(consumidor);
	//
	{
		STGestorEscenaEscena* scn = &_escenas[iEscena];
		scn->sceneMask |= AUOBJETOESCENA_BIT_MODEL_LOCAL_CHANGED;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

void NBGestorEscena::quitarConsumidorLuces(SI32 iEscena, SI32 iCapa, IEscenaConsumidorLuces* consumidor){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::quitarConsumidorLuces")
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	//NBASSERT(!scn->debugBloqueadoActualizandoModelos)
	NBASSERT(consumidor != NULL);
	STGestorEscenaCapa* propiedadesDeCapa = NBGestorEscena::privPropiedadesCapaPorID(iEscena, iCapa); NBASSERT(propiedadesDeCapa != NULL);
	NBASSERT(propiedadesDeCapa->consumidoresLuces->indiceDe(consumidor) != -1) //Se esta quitando un productor inexistente
	propiedadesDeCapa->consumidoresLuces->quitarElemento(consumidor);
	//
	{
		STGestorEscenaEscena* scn = &_escenas[iEscena];
		scn->sceneMask |= AUOBJETOESCENA_BIT_MODEL_LOCAL_CHANGED;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

// CONSUMIDORES DE ESPEJOS

void NBGestorEscena::agregarConsumidorEspejos(SI32 iEscena, SI32 iCapa, IEscenaConsumidorEspejos* consumidor){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::agregarConsumidorEspejos")
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	//NBASSERT(!scn->debugBloqueadoActualizandoModelos)
	NBASSERT(consumidor != NULL);
	STGestorEscenaCapa* propiedadesDeCapa = NBGestorEscena::privPropiedadesCapaPorID(iEscena, iCapa); NBASSERT(propiedadesDeCapa != NULL);
	NBASSERT(propiedadesDeCapa->consumidoresEspejos->indiceDe(consumidor) == -1) //Se esta intentando agregar dos veces al mismo productor
	propiedadesDeCapa->consumidoresEspejos->agregarElemento(consumidor);
	//
	{
		STGestorEscenaEscena* scn = &_escenas[iEscena];
		scn->sceneMask |= AUOBJETOESCENA_BIT_MODEL_LOCAL_CHANGED;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

void NBGestorEscena::quitarConsumidorEspejos(SI32 iEscena, SI32 iCapa, IEscenaConsumidorEspejos* consumidor){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::quitarConsumidorEspejos")
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	//NBASSERT(!scn->debugBloqueadoActualizandoModelos)
	NBASSERT(consumidor != NULL);
	STGestorEscenaCapa* propiedadesDeCapa = NBGestorEscena::privPropiedadesCapaPorID(iEscena, iCapa); NBASSERT(propiedadesDeCapa != NULL);
	NBASSERT(propiedadesDeCapa->consumidoresEspejos->indiceDe(consumidor) != -1) //Se esta quitando un productor inexistente
	propiedadesDeCapa->consumidoresEspejos->quitarElemento(consumidor);
	//
	{
		STGestorEscenaEscena* scn = &_escenas[iEscena];
		scn->sceneMask |= AUOBJETOESCENA_BIT_MODEL_LOCAL_CHANGED;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

// CONSUMIDORES DE CUERDAS

void NBGestorEscena::agregarConsumidorCuerdas(SI32 iEscena, SI32 iCapa, IEscenaConsumidorCuerdas* consumidor){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::agregarConsumidorEspejos")
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	//NBASSERT(!scn->debugBloqueadoActualizandoModelos)
	NBASSERT(consumidor != NULL);
	STGestorEscenaCapa* propiedadesDeCapa = NBGestorEscena::privPropiedadesCapaPorID(iEscena, iCapa); NBASSERT(propiedadesDeCapa != NULL);
	NBASSERT(propiedadesDeCapa->consumidoresCuerdas->indiceDe(consumidor) == -1) //Se esta intentando agregar dos veces al mismo productor
	propiedadesDeCapa->consumidoresCuerdas->agregarElemento(consumidor);
	//
	{
		STGestorEscenaEscena* scn = &_escenas[iEscena];
		scn->sceneMask |= AUOBJETOESCENA_BIT_MODEL_LOCAL_CHANGED;
	}
}

void NBGestorEscena::quitarConsumidorCuerdas(SI32 iEscena, SI32 iCapa, IEscenaConsumidorCuerdas* consumidor){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::quitarConsumidorEspejos")
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	//NBASSERT(!scn->debugBloqueadoActualizandoModelos)
	NBASSERT(consumidor != NULL);
	STGestorEscenaCapa* propiedadesDeCapa = NBGestorEscena::privPropiedadesCapaPorID(iEscena, iCapa); NBASSERT(propiedadesDeCapa != NULL);
	NBASSERT(propiedadesDeCapa->consumidoresCuerdas->indiceDe(consumidor) != -1) //Se esta quitando un productor inexistente
	propiedadesDeCapa->consumidoresCuerdas->quitarElemento(consumidor);
	//
	{
		STGestorEscenaEscena* scn = &_escenas[iEscena];
		scn->sceneMask |= AUOBJETOESCENA_BIT_MODEL_LOCAL_CHANGED;
	}
}

// CONSUMIDORES DE ANTES DE CREAR EL MODELO_GL

void NBGestorEscena::agregarConsumidorPreModeloGL(SI32 iEscena, SI32 iCapa, IEscenaConsumidorPreModeloGL* consumidor){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::agregarConsumidorPreModeloGL")
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	//NBASSERT(!scn->debugBloqueadoActualizandoModelos)
	NBASSERT(consumidor != NULL);
	STGestorEscenaCapa* propiedadesDeCapa = NBGestorEscena::privPropiedadesCapaPorID(iEscena, iCapa); NBASSERT(propiedadesDeCapa != NULL);
	NBASSERT(propiedadesDeCapa->consumidoresPreModeloGL->indiceDe(consumidor) == -1) //Se esta intentando agregar dos veces al mismo productor
	propiedadesDeCapa->consumidoresPreModeloGL->agregarElemento(consumidor);
	//
	{
		STGestorEscenaEscena* scn = &_escenas[iEscena];
		scn->sceneMask |= AUOBJETOESCENA_BIT_MODEL_LOCAL_CHANGED;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

void NBGestorEscena::quitarConsumidorPreModeloGL(SI32 iEscena, SI32 iCapa, IEscenaConsumidorPreModeloGL* consumidor){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::quitarConsumidorPreModeloGL")
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	//NBASSERT(!scn->debugBloqueadoActualizandoModelos)
	NBASSERT(consumidor != NULL);
	STGestorEscenaCapa* propiedadesDeCapa = NBGestorEscena::privPropiedadesCapaPorID(iEscena, iCapa); NBASSERT(propiedadesDeCapa != NULL);
	NBASSERT(propiedadesDeCapa->consumidoresPreModeloGL->indiceDe(consumidor) != -1) //Se esta quitando un productor inexistente
	propiedadesDeCapa->consumidoresPreModeloGL->quitarElemento(consumidor);
	//
	{
		STGestorEscenaEscena* scn = &_escenas[iEscena];
		scn->sceneMask |= AUOBJETOESCENA_BIT_MODEL_LOCAL_CHANGED;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

//########################################
//### FRAMES BUFFERS (RENDER-TO-TEXTURE)
//########################################

SI32 NBGestorEscena::crearEscena(UI16 anchoEscenaGL, UI16 altoEscenaGL, float escalaParaHD, float escalaParaMascarasIlum, const float pixelsPorPulgadaPantallaAncho, const float pixelsPorPulgadaPantallaAlto, float puntosPorPulgadaEscenaAncho, float puntosPorPulgadaEscenaAlto, MapaBitsColor color, ENGestorEscenaDestinoGl tipoDestino, PTRfuncGeneraRenderbuffer opcionalFuncionQueGeneraDatosDeRenderBuffer, void* datoUsuario){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::crearEscena")
	//Indice de registro de escena disponible
	SI32 iEscenaLibre	= -1, iEscenaRetorno = -1;
	SI32 iEscenaBsq;
	for(iEscenaBsq=0; iEscenaBsq<NBGESTORESCENA_MAX_ESCENAS && iEscenaLibre ==  - 1; iEscenaBsq++){
		if(_escenas[iEscenaBsq].registroOcupado == false){
			iEscenaLibre = iEscenaBsq;
		}
	}
	NBASSERT(iEscenaLibre != -1) //Todas las escenas permitidas estan ocupadas NBGESTORESCENA_MAX_ESCENAS
	if(iEscenaLibre != -1){
		iEscenaRetorno = NBGestorEscena::privCrearEscena(iEscenaLibre, true, anchoEscenaGL, altoEscenaGL, escalaParaHD, escalaParaMascarasIlum, pixelsPorPulgadaPantallaAncho, pixelsPorPulgadaPantallaAlto, puntosPorPulgadaEscenaAncho, puntosPorPulgadaEscenaAlto, color, tipoDestino, opcionalFuncionQueGeneraDatosDeRenderBuffer, datoUsuario);
		NBASSERT(iEscenaRetorno >= 0)
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
	return iEscenaRetorno;
}

SI32 NBGestorEscena::privCrearEscena(SI32 iEscenaLibre, bool formatearRegistro, UI16 anchoEscenaGL, UI16 altoEscenaGL, float escalaParaHD, float escalaParaMascarasIlum, const float pixelsPorPulgadaPantallaAncho, const float pixelsPorPulgadaPantallaAlto, float puntosPorPulgadaEscenaAncho, float puntosPorPulgadaEscenaAlto, MapaBitsColor color, ENGestorEscenaDestinoGl tipoDestino, PTRfuncGeneraRenderbuffer opcionalFuncionQueGeneraDatosDeRenderBuffer, void* datoUsuario){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::privCrearEscena")
	NBASSERT(_gestorInicializado);
	NBASSERT(iEscenaLibre >= 0 && iEscenaLibre < NBGESTORESCENA_MAX_ESCENAS);
	NBASSERT(!_bufferDatosBloqueado[_indiceBufferDatosEscribir] || !formatearRegistro);
	STGestorEscenaEscena* scnNew = &_escenas[iEscenaLibre];
	NBASSERT(!scnNew->registroOcupado);
	STGestorEscenaFrameBuffer* frameBuffLibre = &_frameBuffersGL[iEscenaLibre];
	NBASSERT(!frameBuffLibre->registroOcupado);
	SI32 iEscenaRetorno = -1;
	if(iEscenaRetorno == -1){
		if(formatearRegistro){
			NBASSERT(scnNew->agrupadoresParticulas == NULL)
			NBASSERT(scnNew->escuchadoresCambioPuertoVision == NULL)
			NBASSERT(scnNew->capasEnlazadas == NULL)
			//
			scnNew->topItf = NULL;
			NBMemory_setZero(scnNew->marginsPxs);
			//
			UI16 iGrp;
			for(iGrp = 0; iGrp < ENGestorEscenaGrupo_Conteo; iGrp++){
				NBASSERT(scnNew->gruposCapas[iGrp].capas == NULL)
				NBCAJAAABB_INICIALIZAR(scnNew->gruposCapas[iGrp].cajaProyeccion);
				scnNew->gruposCapas[iGrp].capas		= new(ENMemoriaTipo_General) AUArregloNativoMutableP<STGestorEscenaCapa>();
			}
			//
			UI16 iBuff;
			for(iBuff=0; iBuff<NBGESTORESCENA_CANTIDAD_BUFFERES_DATOS; iBuff++){
				NBASSERT(scnNew->renderCapas[iBuff] == NULL)
				NBASSERT(scnNew->renderCapasConsumir[iBuff] == NULL)
				NBASSERT(scnNew->renderCapasProducir[iBuff] == NULL)
				scnNew->renderCapas[iBuff]			= new(ENMemoriaTipo_General) AUArregloNativoMutableP<STGestorEscenaCapaRender>();
				scnNew->renderCapasConsumir[iBuff]	= new(ENMemoriaTipo_General) AUArregloNativoMutableP<STGestorEscenaCapaRender*>();
				scnNew->renderCapasProducir[iBuff]	= new(ENMemoriaTipo_General) AUArregloNativoMutableP<STGestorEscenaCapaRender*>();
			}
			scnNew->agrupadoresParticulas			= new(ENMemoriaTipo_General) AUArregloMutable();											NB_DEFINE_NOMBRE_PUNTERO(scnNew->agrupadoresParticulas, "NBGestorEscena::.agrupadoresParticulas");
			scnNew->escuchadoresCambioPuertoVision	= new(ENMemoriaTipo_General) AUArregloNativoMutableP<IEscuchadorCambioPuertoVision*>();		NB_DEFINE_NOMBRE_PUNTERO(scnNew->escuchadoresCambioPuertoVision, "NBGestorEscena::.escuchadoresCambioPuertoVision");
			scnNew->capasEnlazadas					= new(ENMemoriaTipo_General) AUArregloNativoMutableP<STGestorEscenaEnlaceCapa>();			NB_DEFINE_NOMBRE_PUNTERO(scnNew->capasEnlazadas, "NBGestorEscena::.capasEnlazadas");
			scnNew->fondoModo						= ENGestorEscenaFondoModo_Transparente;
		}
		NBASSERT(scnNew->agrupadoresParticulas != NULL)
		NBASSERT(scnNew->escuchadoresCambioPuertoVision != NULL)
		NBASSERT(scnNew->capasEnlazadas != NULL)
		//Crear el Framebuffer
		if(tipoDestino == ENGestorEscenaDestinoGl_RenderBuffer){
			//PRINTF_INFO("CREAR ESCENA (%f, %f), creando nuevo RENDERBUFFER\n", anchoEscena, altoEscena);
			NBASSERT(escalaParaHD > 0.0f);
			//Crear el Framebuffer y el destino de escena
			GLuint idFB = 0, idRenderBuffer = 0, idDepthBuffer = 0, idStencilBuffer = 0;
			//NBGestorGL::bindTexture(0, 0/*NBGestorTexturas::texturaBlanca->idTexturaGL*/, NBGestorTexturas::texturaBlanca->modoPintado);
			//NBGestorGL::bindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
			NBGestorGL::genFramebuffersEXT(1, &idFB);
			NBGestorGL::bindFramebufferEXT(GL_FRAMEBUFFER_EXT, idFB);
			NBGestorGL::genRenderbuffersEXT(1, &idRenderBuffer);
			NBGestorGL::bindRenderbufferEXT(GL_RENDERBUFFER_EXT, idRenderBuffer);
			if(opcionalFuncionQueGeneraDatosDeRenderBuffer != NULL){
				//PRINTF_INFO("Generando RENDERBUFFER desde fuente externa\n");
				const NBTamanoI tamanoRenderBuffer = opcionalFuncionQueGeneraDatosDeRenderBuffer(anchoEscenaGL, altoEscenaGL, datoUsuario);
				anchoEscenaGL	= tamanoRenderBuffer.ancho;
				altoEscenaGL	= tamanoRenderBuffer.alto;
				NBASSERT(anchoEscenaGL>0 && altoEscenaGL>0);
			} else {
				//PRINTF_INFO("Generando RENDERBUFFER\n");
				NBASSERT(anchoEscenaGL>0 && altoEscenaGL>0);
				NBGestorGL::renderbufferStorageEXT(GL_RENDERBUFFER_EXT, NBGestorTexturas::formatoGlLocal(color), anchoEscenaGL, altoEscenaGL);
			}
			NBGestorGL::framebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_RENDERBUFFER_EXT, idRenderBuffer);
			//Generar y ligar el stencil buffer
			//Nota: a partir de iOS4 no se permiten StencilBuffers separados del DepthBuffer.
			//El Depth y Stencil Buffers van combinados.
			//NBGestorGL::genRenderbuffersEXT(1, &idStencilBuffer); // stencil buffer
			//NBGestorGL::bindRenderbufferEXT(GL_RENDERBUFFER_EXT, idStencilBuffer);
			//NBGestorGL::renderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH24_STENCIL8_EXT, anchoAtlasGL, altoAtlasGL);
			//NBGestorGL::framebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, idStencilBuffer);
			//NBGestorGL::framebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, idStencilBuffer);
			#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT_GL_FRAMEBUFFERS_COMPLETOS
			NBGestorEscena::privFrameBufferCheckStatusEXT();
			#endif
			NBGestorGL::clear(GL_COLOR_BUFFER_BIT);
			NBGestorGL::bindFramebufferInicialEXT(GL_FRAMEBUFFER_EXT);	//En los ejemplos de "OGLES" el framebuffer se libera despues de configurado
			//Registrar FrameBuffer
			frameBuffLibre->registroOcupado			= true;
			frameBuffLibre->tipoDestinoGL			= tipoDestino;
			frameBuffLibre->idFrameBufferGl			= idFB;
			frameBuffLibre->idDepthBufferGl			= idDepthBuffer;
			frameBuffLibre->idStencilBufferGl		= idStencilBuffer;
			frameBuffLibre->colorFrameBufferGl		= color;
			frameBuffLibre->anchoFrameBufferGl		= anchoEscenaGL;
			frameBuffLibre->altoFrameBufferGl		= altoEscenaGL;
			frameBuffLibre->funcionGeneraRenderBuffer	= opcionalFuncionQueGeneraDatosDeRenderBuffer;
			frameBuffLibre->funcionGeneraRenderBufferParams = datoUsuario;
			frameBuffLibre->renderbufferOriginal	= new(ENMemoriaTipo_General) AURenderBuffer(idRenderBuffer, anchoEscenaGL, altoEscenaGL, escalaParaHD); NB_DEFINE_NOMBRE_PUNTERO(frameBuffLibre->renderbufferOriginal, "NBGestorEscena::.renderbufferOriginal")
			frameBuffLibre->texturaOriginal			= NULL;
			frameBuffLibre->renderbufferLigado		= frameBuffLibre->renderbufferOriginal; frameBuffLibre->renderbufferLigado->retener(NB_RETENEDOR_NULL);
			frameBuffLibre->texturaLigada			= NULL;
			PRINTF_INFO("FRAMEBUFFER(nuevo) ENLAZANDO CON RENDERBUFFER idGL(%d).\n", idRenderBuffer);
			NBRectanguloI areaReservada;
			areaReservada.x							= 0;
			areaReservada.y							= 0;
			areaReservada.ancho						= anchoEscenaGL;
			areaReservada.alto						= altoEscenaGL;
			//Registrar escena
			scnNew->registroOcupado				= true;
			scnNew->limpiarColorBuffer			= true;
			scnNew->sceneMask					|= AUOBJETOESCENA_BIT_MODEL_LOCAL_CHANGED;
			scnNew->iFramebufferEscena			= iEscenaLibre;
			scnNew->escalaParaMascarasIlum		= escalaParaMascarasIlum;
			scnNew->escalaParaHD				= escalaParaHD;
			scnNew->tamanoEscenaGL.ancho		= anchoEscenaGL;
			scnNew->tamanoEscenaGL.alto			= altoEscenaGL; NBASSERT((scnNew->tamanoEscenaGL.ancho * scnNew->tamanoEscenaGL.alto)>0)
			scnNew->tamanoEscenaEscn.ancho		= (float)anchoEscenaGL * escalaParaHD;
			scnNew->tamanoEscenaEscn.alto		= (float)altoEscenaGL * escalaParaHD; NBASSERT((scnNew->tamanoEscenaEscn.ancho * scnNew->tamanoEscenaEscn.alto)>0)
			scnNew->areaOcupadaDestino			= areaReservada; NBASSERT(areaReservada.ancho>0) NBASSERT(areaReservada.alto>0)
			scnNew->texturaEscena				= NULL;
			NBRECTANGULO_ESTABLECER(scnNew->puertoDeVision, areaReservada.x, areaReservada.y, areaReservada.ancho, areaReservada.alto);
			NBTAMANO_ESTABLECER(scnNew->pixelsPorPulgadaPantalla, pixelsPorPulgadaPantallaAncho, pixelsPorPulgadaPantallaAlto);
			NBTAMANO_ESTABLECER(scnNew->puntosPorPulgada, puntosPorPulgadaEscenaAncho, puntosPorPulgadaEscenaAlto);
			#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
			scnNew->debugBloqueadoActualizandoModelos = false;
			#endif
			if(formatearRegistro){
				scnNew->escenaHabilitada		= true;
				NBCOLOR_ESTABLECER(scnNew->colorFondo, 0.0f, 0.0f, 0.0f, 1.0f);
				scnNew->contadorIdUnicosCapas	= 0;
				scnNew->capaDebugHabilitada	= true;
				NBGestorEscena::normalizaCajaProyeccionEscena(iEscenaLibre);
				NBGestorEscena::normalizaMatricesCapasEscena(iEscenaLibre);
			}
			//
			iEscenaRetorno = iEscenaLibre;
		} else if(tipoDestino == ENGestorEscenaDestinoGl_Textura){
			//PRINTF_INFO("CREAR ESCENA (%f, %f), creando nuevo TEXTURA\n", anchoEscena, altoEscena);
			SI32 anchoDestinoGL	= (SI32)anchoEscenaGL;
			SI32 altoDestinoGL	= (SI32)altoEscenaGL;
			SI32 anchoAtlasGL	= anchoDestinoGL;
			SI32 altoAtlasGL	= altoDestinoGL;
			NBASSERT(anchoDestinoGL>0 && altoDestinoGL>0);
			NBASSERT(anchoAtlasGL>0 && altoAtlasGL>0);
			if(!NBGestorGL::soportaTexturas2DNoMultiploDe2()){
				SI32 mayorLado	= (anchoDestinoGL > altoDestinoGL ? anchoDestinoGL : altoDestinoGL);
				SI32 ladoBase2	= 64; while(ladoBase2<mayorLado) ladoBase2 *= 2;
				anchoAtlasGL	= ladoBase2;
				altoAtlasGL		= ladoBase2;
			}
			//Crear destino de escena
			AUTextura* texturaGL = NBGestorTexturas::texturaDesdeAreaVacia(anchoAtlasGL, altoAtlasGL, color, ENTexturaTipoAlmacenamientoGL_AtlasUnico, ENTExturaTipoUsoGL_EscrituraLectura, ENTexturaModoPintado_Patron_Preciso /*ENTexturaModoPintado_Patron_Suavizado*/, ENTexturaMipMap_Inhabilitado, ENTexturaOrdenV_HaciaAbajo, 1, 1.0f); NB_DEFINE_NOMBRE_PUNTERO(texturaGL, "NBGestorEscena::texturaGL");
			if(texturaGL == NULL){
				PRINTF_ERROR("GL, no se pudo generar la textura para el FrameBuffer\n");
			} else {
				NBTamano tamAtlas			= texturaGL->tamanoAtlas();
				#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
				//NBRectanguloP<UI16> areaEnAtlas	= texturaGL->areaEnAtlas();
				//NBASSERT(tamAtlas.ancho == areaEnAtlas.ancho && tamAtlas.alto == areaEnAtlas.alto && areaEnAtlas.x == 0 && areaEnAtlas.y == 0);
				#endif
				//Registrar FrameBuffer
				GLuint idFB = 0, idDephtBufer = 0, idStencilBuffer = 0;
				//NBGestorGL::bindTexture(0, 0/*NBGestorTexturas::texturaBlanca->idTexturaGL*/, NBGestorTexturas::texturaBlanca->modoPintado);
				//NBGestorGL::bindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
				NBGestorGL::genFramebuffersEXT(1, &idFB);
				NBGestorGL::bindFramebufferEXT(GL_FRAMEBUFFER_EXT, idFB);
				//NBGestorGL::bindTexture(0, texturaGL->idTexturaGL); //Segun el ejemplo de "OGLES" no aparece esta llamada (posiblmente porque al crear la textura ya queda linkeada)
				NBGestorGL::framebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, texturaGL->idTexturaGL, 0);
				//Generar y ligar el stencil buffer
				//Nota: a partir de iOS4 no se permiten StencilBuffers separados del DepthBuffer.
				//El Depth y Stencil Buffers van combinados.
				//NBGestorGL::genRenderbuffersEXT(1, &idStencilBuffer); // stencil buffer
				//NBGestorGL::bindRenderbufferEXT(GL_RENDERBUFFER_EXT, idStencilBuffer);
				//NBGestorGL::renderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH24_STENCIL8_EXT/*GL_STENCIL_INDEX8*/, anchoAtlasGL, altoAtlasGL);
				//NBGestorGL::framebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, idStencilBuffer);
				//NBGestorGL::framebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, idStencilBuffer);
				#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT_GL_FRAMEBUFFERS_COMPLETOS
				NBGestorEscena::privFrameBufferCheckStatusEXT();
				#endif
				NBGestorGL::clear(GL_COLOR_BUFFER_BIT);
				NBGestorGL::bindFramebufferInicialEXT(GL_FRAMEBUFFER_EXT);	//En los ejemplos de "OGLES" el framebuffer se libera despues de configurado
				frameBuffLibre->registroOcupado			= true;
				frameBuffLibre->tipoDestinoGL			= tipoDestino;
				frameBuffLibre->idFrameBufferGl			= idFB;
				frameBuffLibre->idDepthBufferGl			= idDephtBufer;
				frameBuffLibre->idStencilBufferGl		= idStencilBuffer;
				frameBuffLibre->colorFrameBufferGl		= color;
				frameBuffLibre->anchoFrameBufferGl		= tamAtlas.ancho;
				frameBuffLibre->altoFrameBufferGl		= tamAtlas.alto;
				frameBuffLibre->funcionGeneraRenderBuffer	= NULL;
				frameBuffLibre->funcionGeneraRenderBufferParams = NULL;
				frameBuffLibre->renderbufferOriginal	= NULL;
				frameBuffLibre->texturaOriginal			= texturaGL; texturaGL->retener(NB_RETENEDOR_NULL);
				frameBuffLibre->renderbufferLigado		= NULL;
				frameBuffLibre->texturaLigada			= texturaGL; texturaGL->retener(NB_RETENEDOR_NULL);
				//PRINTF_INFO("FRAMEBUFFER(nuevo) ENLAZANDO CON TEXTURA idGL(%d).\n", texturaGL->idTexturaGL);
				//Registrar
				NBRectanguloI areaReservada;
				areaReservada.x							= 0;
				areaReservada.y							= 0;
				areaReservada.ancho						= anchoDestinoGL;
				areaReservada.alto						= altoDestinoGL;
				NBTamano tamAtlasTex					= frameBuffLibre->texturaOriginal->tamanoAtlas();
				scnNew->registroOcupado				= true;
				scnNew->limpiarColorBuffer			= true;
				scnNew->sceneMask					|= AUOBJETOESCENA_BIT_MODEL_LOCAL_CHANGED;
				scnNew->iFramebufferEscena			= iEscenaLibre;
				scnNew->escalaParaMascarasIlum		= escalaParaMascarasIlum;
				scnNew->escalaParaHD				= escalaParaHD;
				scnNew->tamanoEscenaGL.ancho		= anchoEscenaGL;
				scnNew->tamanoEscenaGL.alto			= altoEscenaGL; NBASSERT((scnNew->tamanoEscenaGL.ancho * scnNew->tamanoEscenaGL.alto)>0)
				scnNew->tamanoEscenaEscn.ancho		= (float)anchoEscenaGL * escalaParaHD;
				scnNew->tamanoEscenaEscn.alto		= (float)altoEscenaGL * escalaParaHD; NBASSERT((scnNew->tamanoEscenaEscn.ancho * scnNew->tamanoEscenaEscn.alto)>0)
				scnNew->areaOcupadaDestino			= areaReservada; NBASSERT(areaReservada.ancho>0) NBASSERT(areaReservada.alto>0)
				scnNew->texturaEscena				= new(ENMemoriaTipo_General) AUTextura(texturaGL->idTexturaGL, ENTexturaOrdenV_HaciaAbajo, ENTexturaEstadoCarga_Cargada, tamAtlasTex.ancho, tamAtlasTex.alto, areaReservada.x, areaReservada.y, areaReservada.ancho, areaReservada.alto, frameBuffLibre->texturaOriginal->escalaBase2(), frameBuffLibre->texturaOriginal->escalaParaHD(), ENTexturaModoPintado_Patron_Suavizado, "", NULL); NB_DEFINE_NOMBRE_PUNTERO(scnNew->texturaEscena, "NBGestorEscena::.texturaEscena")
				NBRECTANGULO_ESTABLECER(scnNew->puertoDeVision, (float)areaReservada.x, (float)areaReservada.y, (float)areaReservada.ancho, (float)areaReservada.alto);
				NBTAMANO_ESTABLECER(scnNew->pixelsPorPulgadaPantalla, pixelsPorPulgadaPantallaAncho, pixelsPorPulgadaPantallaAlto);
				NBTAMANO_ESTABLECER(scnNew->puntosPorPulgada, puntosPorPulgadaEscenaAncho, puntosPorPulgadaEscenaAlto);
				#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
				scnNew->debugBloqueadoActualizandoModelos = false;
				#endif
				if(formatearRegistro){
					scnNew->escenaHabilitada		= true;
					NBCOLOR_ESTABLECER(scnNew->colorFondo, 0.0f, 0.0f, 0.0f, 1.0f);
					scnNew->contadorIdUnicosCapas	= 0;
					scnNew->capaDebugHabilitada	= true;
					NBGestorEscena::normalizaCajaProyeccionEscena(iEscenaLibre);
					NBGestorEscena::normalizaMatricesCapasEscena(iEscenaLibre);
				}
				//
				iEscenaRetorno = iEscenaLibre;
			}
		} else if(tipoDestino == ENGestorEscenaDestinoGl_Heredado){
			frameBuffLibre->registroOcupado			= true;
			frameBuffLibre->tipoDestinoGL			= tipoDestino;
			frameBuffLibre->idFrameBufferGl			= 0;
			frameBuffLibre->idDepthBufferGl			= 0;
			frameBuffLibre->idStencilBufferGl		= 0;
			frameBuffLibre->colorFrameBufferGl		= color;
			frameBuffLibre->anchoFrameBufferGl		= anchoEscenaGL;
			frameBuffLibre->altoFrameBufferGl		= altoEscenaGL;
			frameBuffLibre->funcionGeneraRenderBuffer	= NULL;
			frameBuffLibre->funcionGeneraRenderBufferParams = NULL;
			frameBuffLibre->renderbufferOriginal	= NULL;
			frameBuffLibre->texturaOriginal			= NULL;
			frameBuffLibre->renderbufferLigado		= NULL;
			frameBuffLibre->texturaLigada			= NULL;
			//Registrar
			NBRectanguloI areaReservada;
			areaReservada.x							= 0;
			areaReservada.y							= 0;
			areaReservada.ancho						= anchoEscenaGL;
			areaReservada.alto						= altoEscenaGL;
			scnNew->registroOcupado				= true;
			scnNew->limpiarColorBuffer			= true;
			scnNew->sceneMask					|= AUOBJETOESCENA_BIT_MODEL_LOCAL_CHANGED;
			scnNew->iFramebufferEscena			= iEscenaLibre;
			scnNew->escalaParaMascarasIlum		= escalaParaMascarasIlum;
			scnNew->escalaParaHD				= escalaParaHD;
			scnNew->tamanoEscenaGL.ancho		= anchoEscenaGL;
			scnNew->tamanoEscenaGL.alto			= altoEscenaGL; NBASSERT((scnNew->tamanoEscenaGL.ancho * scnNew->tamanoEscenaGL.alto)>0)
			scnNew->tamanoEscenaEscn.ancho		= (float)anchoEscenaGL * escalaParaHD;
			scnNew->tamanoEscenaEscn.alto		= (float)altoEscenaGL * escalaParaHD; NBASSERT((scnNew->tamanoEscenaEscn.ancho * scnNew->tamanoEscenaEscn.alto)>0)
			scnNew->areaOcupadaDestino			= areaReservada; NBASSERT(areaReservada.ancho>0) NBASSERT(areaReservada.alto>0)
			scnNew->texturaEscena				= NULL;
			NBRECTANGULO_ESTABLECER(scnNew->puertoDeVision, (float)areaReservada.x, (float)areaReservada.y, (float)areaReservada.ancho, (float)areaReservada.alto);
			NBTAMANO_ESTABLECER(scnNew->pixelsPorPulgadaPantalla, pixelsPorPulgadaPantallaAncho, pixelsPorPulgadaPantallaAlto);
			NBTAMANO_ESTABLECER(scnNew->puntosPorPulgada, puntosPorPulgadaEscenaAncho, puntosPorPulgadaEscenaAlto);
			#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
			scnNew->debugBloqueadoActualizandoModelos = false;
			#endif
			if(formatearRegistro){
				scnNew->escenaHabilitada		= true;
				NBCOLOR_ESTABLECER(scnNew->colorFondo, 0.0f, 0.0f, 0.0f, 1.0f);
				scnNew->contadorIdUnicosCapas	= 0;
				scnNew->capaDebugHabilitada	= true;
				NBGestorEscena::normalizaCajaProyeccionEscena(iEscenaLibre);
				NBGestorEscena::normalizaMatricesCapasEscena(iEscenaLibre);
			}
			//
			iEscenaRetorno = iEscenaLibre;
		} else {
			NBASSERT(false)
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
	return iEscenaRetorno;
}

void NBGestorEscena::setTopItf(SI32 iEscena, AUEscenaTopItf* topItf){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::setTopItf")
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::quitarObjetoCapa")
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	NBASSERT(scn->registroOcupado)
	NBASSERT(!_bufferDatosBloqueado[_indiceBufferDatosEscribir]);
	//NBASSERT(!scn->debugBloqueadoActualizandoModelos)
	//PRINTF_INFO("Capa quitando %lu retenciones(%d)\n", (unsigned long)objetoEscena, objetoEscena->conteoReferencias());
	//Set
	scn->topItf = topItf;
	//Update layers
	SI32 iGrp; for(iGrp = 0; iGrp < ENGestorEscenaGrupo_Conteo; iGrp++){
		STGestorEscenaGrupo* grupoCapas = &(scn->gruposCapas[iGrp]);
		SI32 i; for(i = grupoCapas->capas->conteo - 1; i >= 0; i--){
			STGestorEscenaCapa* datosCapa = &(grupoCapas->capas->elemento[i]);
			datosCapa->root->setTopItf(topItf);
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
	
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

bool NBGestorEscena::registroEscenaOcupado(SI32 iEscena){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::registroEscenaOcupado")
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	bool r = scn->registroOcupado;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
	return r;
}

void NBGestorEscena::liberarEscena(SI32 iEscena){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::liberarEscena")
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	//STGestorEscenaEscena* scn = &_escenas[iEscena];
	//NBASSERT(!scn->debugBloqueadoActualizandoModelos)
	NBGestorEscena::privLiberarEscena(iEscena, true);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

void NBGestorEscena::privLiberarEscena(SI32 iEscena, bool formatearRegistro){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::privLiberarEscena")
	NBASSERT(!_bufferDatosBloqueado[_indiceBufferDatosEscribir] || !formatearRegistro);
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	NBASSERT(scn->registroOcupado);
	//NBASSERT(!scn->debugBloqueadoActualizandoModelos)
	//
	//Eliminar las texturas render de la escena
	NBGestorEscena::privTexturaRenderLiberarReservasDeEscena(iEscena);
	//Limpiar datos de escena
	if(scn->texturaEscena != NULL) scn->texturaEscena->liberar(NB_RETENEDOR_NULL);
	//Liberar los grupos de modelos a renderizar
	if(formatearRegistro){
		scn->fondoModo				= ENGestorEscenaFondoModo_Transparente;
		scn->agrupadoresParticulas->vaciar();
		scn->escuchadoresCambioPuertoVision->vaciar();
		scn->capasEnlazadas->vaciar();
	}
	//Eliminar framBuffer de escena
	STGestorEscenaFrameBuffer* datosFB = &_frameBuffersGL[scn->iFramebufferEscena]; NBASSERT(datosFB->registroOcupado)
	//Destinos actuales
	if(datosFB->texturaLigada != NULL) datosFB->texturaLigada->liberar(NB_RETENEDOR_NULL); datosFB->texturaLigada = NULL;
	if(datosFB->renderbufferLigado != NULL) datosFB->renderbufferLigado->liberar(NB_RETENEDOR_NULL); datosFB->renderbufferLigado = NULL;
	//Destinos originales
	if(datosFB->texturaOriginal != NULL){
		NBGestorTexturas::liberarEspacioTexturaSiSoloTieneUnaReferenciaExterna(datosFB->texturaOriginal);
		datosFB->texturaOriginal->liberar(NB_RETENEDOR_NULL);
		datosFB->texturaOriginal = NULL;
	}
	if(datosFB->renderbufferOriginal != NULL){
		GLuint idRender		= datosFB->renderbufferOriginal->idRenderBufferGl();
		if(idRender != 0) NBGestorGL::deleteRenderbuffersEXT(1, &idRender);
		datosFB->renderbufferOriginal->liberar(NB_RETENEDOR_NULL);
		datosFB->renderbufferOriginal = NULL;
	}
	GLuint idStencil		= datosFB->idStencilBufferGl;
	GLuint idDepth			= datosFB->idDepthBufferGl;
	GLuint idFrameBuffer	= datosFB->idFrameBufferGl;
	if(idStencil != 0) NBGestorGL::deleteRenderbuffersEXT(1, &idStencil);
	if(idDepth != 0 && idDepth != idStencil) NBGestorGL::deleteRenderbuffersEXT(1, &idDepth); //idDepthBufferGl y idStencilBufferGl pueden ser identicos, si la implementacion limita a depth+stencil combinados (iOS 4.0+)
	if(idFrameBuffer != 0) NBGestorGL::deleteFramebuffersEXT(1, &idFrameBuffer);
	datosFB->registroOcupado = false;
	//
	NBASSERT(scn->agrupadoresParticulas != NULL)
	NBASSERT(scn->escuchadoresCambioPuertoVision != NULL)
	if(formatearRegistro){
		NBGestorEscena::vaciarGrupos(iEscena);
		//
		scn->topItf = NULL;
		//
		SI32 iGrp;
		for(iGrp = 0; iGrp < ENGestorEscenaGrupo_Conteo; iGrp++){
			STGestorEscenaGrupo* grp = &(scn->gruposCapas[iGrp]);
			if(grp->capas != NULL) grp->capas->liberar(NB_RETENEDOR_NULL); grp->capas = NULL;
		}
		//
		SI32 iBuff;
		for(iBuff=0; iBuff<NBGESTORESCENA_CANTIDAD_BUFFERES_DATOS; iBuff++){
			AUArregloNativoP<STGestorEscenaCapaRender>*	renderCapas = scn->renderCapas[iBuff]; NBASSERT(renderCapas != NULL)
			UI32 iCapa, iCapaConteo = renderCapas->conteo;
			for(iCapa=0; iCapa<iCapaConteo; iCapa++){
				STGestorEscenaCapaRender* capaRender = &(renderCapas->elemento[iCapa]);
				//
				capaRender->renderModelosDatos->liberar(NB_RETENEDOR_NULL);
				capaRender->renderModelos->liberar(NB_RETENEDOR_NULL);
				capaRender->renderPiscinaObjetosRetener->liberar(NB_RETENEDOR_NULL);
				//
				capaRender->renderIluminacion->liberar(NB_RETENEDOR_NULL);
				capaRender->renderEspejos->liberar(NB_RETENEDOR_NULL);
				capaRender->renderProductoresSombras->liberar(NB_RETENEDOR_NULL);
				capaRender->renderDescriptoresCuerdas->liberar(NB_RETENEDOR_NULL);
				capaRender->renderConsumidoresLuces->liberar(NB_RETENEDOR_NULL);
				capaRender->renderConsumidoresEspejos->liberar(NB_RETENEDOR_NULL);
				capaRender->renderConsumidoresCuerdas->liberar(NB_RETENEDOR_NULL);
				capaRender->renderConsumidoresPreModeloGL->liberar(NB_RETENEDOR_NULL);
				capaRender->renderSombrasPorIluminacion->liberar(NB_RETENEDOR_NULL);
				capaRender->renderSombrasRangosPorIluminacion->liberar(NB_RETENEDOR_NULL);
				capaRender->renderSombrasVerticesIntermedios->liberar(NB_RETENEDOR_NULL);
				//
				capaRender->renderMascLuz->liberar(NB_RETENEDOR_NULL); capaRender->renderMascLuz = NULL;
				capaRender->renderMascLuzBloquesGL->liberar(NB_RETENEDOR_NULL); capaRender->renderMascLuzBloquesGL = NULL;
				capaRender->renderMascLuzLucesConIntensidad->liberar(NB_RETENEDOR_NULL); capaRender->renderMascLuzLucesConIntensidad = NULL;
				//
				STGestorEscenaTexturaLuz::inicializar(&capaRender->renderMascLucesCombinadas);
			}
			scn->renderCapas[iBuff]->liberar(NB_RETENEDOR_NULL);
			scn->renderCapasConsumir[iBuff]->liberar(NB_RETENEDOR_NULL);
			scn->renderCapasProducir[iBuff]->liberar(NB_RETENEDOR_NULL);
			scn->renderCapas[iBuff]			= NULL;
			scn->renderCapasConsumir[iBuff]	= NULL;
			scn->renderCapasProducir[iBuff]	= NULL;
		}
		scn->agrupadoresParticulas->liberar(NB_RETENEDOR_NULL);
		scn->agrupadoresParticulas				= NULL;
		scn->escuchadoresCambioPuertoVision->liberar(NB_RETENEDOR_NULL);
		scn->escuchadoresCambioPuertoVision	= NULL;
		scn->capasEnlazadas->liberar(NB_RETENEDOR_NULL);
		scn->capasEnlazadas					= NULL;
	}
	scn->registroOcupado = false;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

void NBGestorEscena::liberarRecursosCacheRenderEscenas(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::liberarRecursosCacheRenderEscenas")
	SI32 iEscena; for(iEscena = 0; iEscena < NBGESTORESCENA_MAX_ESCENAS; iEscena++){
		STGestorEscenaEscena* scn = &_escenas[iEscena];
		if(scn->registroOcupado){
			//Liberar los grupos de modelos a renderizar
			SI32 iBuff;
			for(iBuff = 0; iBuff < NBGESTORESCENA_CANTIDAD_BUFFERES_DATOS; iBuff++){
				if(!_bufferDatosBloqueado[iBuff]){
					scn->renderCapasConsumir[iBuff]->vaciar();
					scn->renderCapasProducir[iBuff]->vaciar();
					AUArregloNativoP<STGestorEscenaCapaRender>*	renderCapas = scn->renderCapas[iBuff]; NBASSERT(renderCapas != NULL)
					UI32 iCapa, iCapaConteo = renderCapas->conteo;
					for(iCapa = 0; iCapa < iCapaConteo; iCapa++){
						STGestorEscenaCapaRender* capaRender = &(renderCapas->elemento[iCapa]);
						capaRender->renderModelosDatos->vaciar();
						capaRender->renderModelos->vaciar();
						capaRender->renderPiscinaObjetosRetener->vaciar();
						capaRender->renderIluminacion->vaciar();
						capaRender->renderEspejos->vaciar();
						capaRender->renderProductoresSombras->vaciar();
						capaRender->renderDescriptoresCuerdas->vaciar();
						capaRender->renderConsumidoresLuces->vaciar();
						capaRender->renderConsumidoresEspejos->vaciar();
						capaRender->renderConsumidoresCuerdas->vaciar();
						capaRender->renderConsumidoresPreModeloGL->vaciar();
						UI16 iSubElem;
						for(iSubElem = 0; iSubElem < capaRender->renderSombrasPorIluminacion->conteo; iSubElem++){ ((AUArregloNativoMutableP<NBFuenteSombra>*)capaRender->renderSombrasPorIluminacion->elemento[iSubElem])->vaciar(); }
						for(iSubElem = 0; iSubElem < capaRender->renderSombrasRangosPorIluminacion->conteo; iSubElem++){ ((AUArregloNativoMutableP<STRangoSombra>*)capaRender->renderSombrasRangosPorIluminacion->elemento[iSubElem])->vaciar(); }
						//capaRender->renderSombrasPorIluminacion->vaciar(); //Mejor vaciar los subarreglos, porque vaciar el arreglo principal obligaria a volver a crear los subarreglos en el siguiente tick
						//capaRender->renderSombrasRangosPorIluminacion->vaciar(); //Mejor vaciar los subarreglos, porque vaciar el arreglo principal obligaria a volver a crear los subarreglos en el siguiente tick
						capaRender->renderSombrasVerticesIntermedios->vaciar();
						//
						capaRender->renderConteoLucesAfectanMascara = 0;
						capaRender->renderMascLuzSumaFigurasTotal	= 0;
						capaRender->renderMascLuz->vaciar();
						capaRender->renderMascLuzBloquesGL->vaciar();
						capaRender->renderMascLuzLucesConIntensidad->vaciar();
						STGestorEscenaTexturaLuz::inicializar(&capaRender->renderMascLucesCombinadas);
					}
				}
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

void NBGestorEscena::habilitarEscena(SI32 iEscena){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::habilitarEscena")
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	NBASSERT(scn->registroOcupado)
	//NBASSERT(!scn->debugBloqueadoActualizandoModelos)
	if(!scn->escenaHabilitada){
		scn->escenaHabilitada = true;
		scn->sceneMask |= AUOBJETOESCENA_BIT_MODEL_LOCAL_CHANGED;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

void NBGestorEscena::deshabilitarEscena(SI32 iEscena){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::deshabilitarEscena")
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	NBASSERT(scn->registroOcupado)
	//NBASSERT(!scn->debugBloqueadoActualizandoModelos)
	if(scn->escenaHabilitada){
		scn->escenaHabilitada = false;
		scn->sceneMask |= AUOBJETOESCENA_BIT_MODEL_LOCAL_CHANGED;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

bool NBGestorEscena::escenaLimpiaColorBuffer(const SI32 iEscena){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::escenaLimpiaColorBuffer")
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	NBASSERT(scn->registroOcupado)
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
	return scn->limpiarColorBuffer;
}

void NBGestorEscena::establecerEscenaLimpiaColorBuffer(const SI32 iEscena, const bool limpiaColorBuffer){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::establecerEscenaLimpiaColorBuffer")
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	NBASSERT(scn->registroOcupado)
	if(scn->limpiarColorBuffer != limpiaColorBuffer){
		scn->limpiarColorBuffer = limpiaColorBuffer;
		scn->sceneMask |= AUOBJETOESCENA_BIT_MODEL_LOCAL_CHANGED;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

STGestorEscenaFrameBuffer NBGestorEscena::propiedadesFramebuffer(SI32 iFramebuffer){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::propiedadesFramebuffer")
	NBASSERT(iFramebuffer >= 0 && iFramebuffer<NBGESTORESCENA_MAX_ESCENAS);
	NBASSERT(_frameBuffersGL[iFramebuffer].registroOcupado)
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
	return _frameBuffersGL[iFramebuffer];
}

STGestorEscenaEscena NBGestorEscena::propiedadesEscena(SI32 iEscena){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::propiedadesEscena")
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	NBASSERT(scn->registroOcupado)
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
	return _escenas[iEscena];
}

UI32 NBGestorEscena::idGlFrameBufferParaEscena(SI32 iEscena){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::idGlFrameBufferParaEscena")
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	NBASSERT(scn->registroOcupado)
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
	return _frameBuffersGL[scn->iFramebufferEscena].idFrameBufferGl;
}

void NBGestorEscena::redimensionarEscena(SI32 iEscena, UI16 anchoEscenaGL, UI16 altoEscenaGL, float escalaParaHD, const NBTamano ppiScreen, const NBTamano dpiScene, float escalaParaMascarasIlum, bool notificar, PTRfuncGeneraRenderbuffer opcionalFuncionQueGeneraDatosDeRenderBuffer, void* datoUsuario){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::redimensionarEscena")
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	NBASSERT(scn->registroOcupado);
	//NBASSERT(!_bufferDatosBloqueado[_indiceBufferDatosEscribir]);
	//PRINTF_INFO("REDIMENSIONANDO ESCENA: %d\n", iEscena);
	if(scn->registroOcupado){
		//PRINTF_INFO("Redimensionando escena de (%d, %d)px a (%d, %d)px.\n", scn->areaOcupadaDestino.ancho, scn->areaOcupadaDestino.alto, anchoEscenaGL, altoEscenaGL);
		if(scn->areaOcupadaDestino.ancho == anchoEscenaGL && scn->areaOcupadaDestino.alto == altoEscenaGL && scn->escalaParaHD == escalaParaHD){
			PRINTF_WARNING("Escena %d, es inecesario redimensionar escena (%d, %d) escala(%f)\n", iEscena, anchoEscenaGL, altoEscenaGL, escalaParaHD);
		} else {
			//Backup de datos de escena
			const SI16 iFramebufferEscena					= scn->iFramebufferEscena;
			const NBTamano pixelsPorPulgadaPantalla			= (ppiScreen.ancho != 0 && ppiScreen.alto != 0 ? ppiScreen : scn->pixelsPorPulgadaPantalla);
			const NBTamano puntosPorPulgadaEscena			= (dpiScene.ancho != 0 && dpiScene.alto != 0 ? dpiScene : scn->puntosPorPulgada);
			//Backup de datos de frame-buffer
			const STGestorEscenaFrameBuffer* framebuffer	= &_frameBuffersGL[iFramebufferEscena];
			const MapaBitsColor colorFrameBufferGl			= framebuffer->colorFrameBufferGl;
			const ENGestorEscenaDestinoGl tipoDestinoGL		= framebuffer->tipoDestinoGL;
			NBASSERT(framebuffer->funcionGeneraRenderBuffer == opcionalFuncionQueGeneraDatosDeRenderBuffer)
			NBASSERT(framebuffer->funcionGeneraRenderBufferParams == datoUsuario)
			NBASSERT((framebuffer->texturaOriginal != NULL && framebuffer->renderbufferOriginal == NULL) || (framebuffer->texturaOriginal == NULL && framebuffer->renderbufferOriginal != NULL) || framebuffer->tipoDestinoGL == ENGestorEscenaDestinoGl_Heredado);
			//
			NBGestorEscena::privLiberarEscena(iEscena, false);
			NBGestorEscena::privCrearEscena(iEscena, false, anchoEscenaGL, altoEscenaGL, escalaParaHD, escalaParaMascarasIlum, pixelsPorPulgadaPantalla.ancho, pixelsPorPulgadaPantalla.alto, puntosPorPulgadaEscena.ancho, puntosPorPulgadaEscena.alto, colorFrameBufferGl, tipoDestinoGL, opcionalFuncionQueGeneraDatosDeRenderBuffer, datoUsuario);
			scn->sceneMask |= AUOBJETOESCENA_BIT_MODEL_LOCAL_CHANGED;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

void NBGestorEscena::notificarObjetosTamanoEscena(SI32 iEscena, const STNBViewPortSize before, const STNBViewPortSize after){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::notificarObjetosTamanoEscena")
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	NBASSERT(scn->registroOcupado)
	//NBASSERT(!_bufferDatosBloqueado[_indiceBufferDatosEscribir]);
	//Notificar a el arbol de la escena
	SI32 iGrp; for(iGrp = 0; iGrp < ENGestorEscenaGrupo_Conteo; iGrp++){
		STGestorEscenaGrupo* grupoCapas = &(scn->gruposCapas[iGrp]); NBASSERT(grupoCapas != NULL)
		SI32 i;
		for(i=grupoCapas->capas->conteo - 1; i >= 0; i--){
			grupoCapas->capas->elemento[i].objEscena->puertoVisionModificado();
		}
	}
	//Notificar a escuchadores
	{
		UI32 iEscucha, iEscuchaConteo = scn->escuchadoresCambioPuertoVision->conteo;
		for(iEscucha = 0; iEscucha < iEscuchaConteo; iEscucha++){
			scn->escuchadoresCambioPuertoVision->elemento[iEscucha]->puertoDeVisionModificado(iEscena, before, after);
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

AUTextura* NBGestorEscena::texturaEscena(SI32 iEscena){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::texturaEscena")
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	NBASSERT(scn->registroOcupado)
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
	return scn->texturaEscena;
}

AUArreglo* NBGestorEscena::agrupadoresParticulas(SI32 iEscena){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::agrupadoresParticulas")
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	NBASSERT(scn->registroOcupado)
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
	return scn->agrupadoresParticulas;
}

void NBGestorEscena::agregarAgrupadorParticulas(SI32 iEscena, AUEscenaGrupoParticulas* grupoParticulas){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::agregarAgrupadorParticulas")
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	NBASSERT(!_bufferDatosBloqueado[_indiceBufferDatosEscribir]);
	//NBASSERT(!scn->debugBloqueadoActualizandoModelos)
	NBASSERT(scn->agrupadoresParticulas->indiceDe(grupoParticulas) == -1) //Si falla, el escuchador ya esta en el arreglo
	scn->agrupadoresParticulas->agregarElemento(grupoParticulas);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

void NBGestorEscena::quitarAgrupadorParticulas(SI32 iEscena, AUEscenaGrupoParticulas* grupoParticulas){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::quitarAgrupadorParticulas")
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	NBASSERT(!_bufferDatosBloqueado[_indiceBufferDatosEscribir]);
	//NBASSERT(!scn->debugBloqueadoActualizandoModelos)
	scn->agrupadoresParticulas->quitarElemento(grupoParticulas);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

void NBGestorEscena::agregarEscuchadorCambioPuertoVision(SI32 iEscena, IEscuchadorCambioPuertoVision* escuchador){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::agregarEscuchadorCambioPuertoVision")
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	NBASSERT(!_bufferDatosBloqueado[_indiceBufferDatosEscribir]);
	//NBASSERT(!scn->debugBloqueadoActualizandoModelos)
	NBASSERT(scn->escuchadoresCambioPuertoVision->indiceDe(escuchador) == -1) //Si falla, el escuchador ya esta en el arreglo
	scn->escuchadoresCambioPuertoVision->agregarElemento(escuchador);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

void NBGestorEscena::quitarEscuchadorCambioPuertoVision(SI32 iEscena, IEscuchadorCambioPuertoVision* escuchador){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::quitarEscuchadorCambioPuertoVision")
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	NBASSERT(!_bufferDatosBloqueado[_indiceBufferDatosEscribir]);
	//NBASSERT(!scn->debugBloqueadoActualizandoModelos)
	scn->escuchadoresCambioPuertoVision->quitarElemento(escuchador);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

bool NBGestorEscena::capaDebugHabilitada(SI32 iEscena){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::capaDebugHabilitada")
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
	return scn->capaDebugHabilitada;
}

void NBGestorEscena::establecerCapaDebugHabilitada(SI32 iEscena, bool habilitada){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::establecerCapaDebugHabilitada")
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	//NBASSERT(!_bufferDatosBloqueado[_indiceBufferDatosEscribir]); //Ya no necesario validar. Se crea una copia de este dato para el hilo renderizador.
	if(scn->capaDebugHabilitada != habilitada){
		scn->capaDebugHabilitada = habilitada;
		scn->sceneMask |= AUOBJETOESCENA_BIT_MODEL_LOCAL_CHANGED;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

//########################################
//### RENDERIZADO
//########################################

UI32 NBGestorEscena::secuencialActualizacionesModelos(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::secuencialActualizacionesModelos")
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
	return _secuencialActualizacionesModelos;
}

UI32 NBGestorEscena::secuencialActualizacionesModelosGL(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::secuencialActualizacionesModelosGL")
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
	return _secuencialActualizacionesModelosGL;
}

UI32 NBGestorEscena::secuencialRenderizadasModelosGL(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::secuencialRenderizadasModelosGL")
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
	return _secuencialRenderizadasModelosGL;
}

#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
void NBGestorEscena::debugMarcarBloqueoModelosEscena(const bool bloqueados){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::debugMarcarBloqueoModelosEscena")
	SI32 iEscena; for(iEscena = (NBGESTORESCENA_MAX_ESCENAS - 1); iEscena >= 0; iEscena--){
		STGestorEscenaEscena* scn = &_escenas[iEscena];
		if(scn->registroOcupado){
			NBGestorEscena::privDebugMarcarBloqueoModelosEscena(iEscena, bloqueados);
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}
#endif

#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
void NBGestorEscena::privDebugMarcarBloqueoModelosEscena(const SI32 iEscena, const bool bloqueados){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::privDebugMarcarBloqueoModelosEscena")
	NBASSERT(_gestorInicializado);
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	NBASSERT(scn->registroOcupado);
	//Bloquear escena
	scn->debugBloqueadoActualizandoModelos = bloqueados;
	//Bloquear capas (contenedores)
	{
		UI16 iGrp; for(iGrp = 0; iGrp < ENGestorEscenaGrupo_Conteo; iGrp++){
			STGestorEscenaGrupo* grupoCapas = &(scn->gruposCapas[iGrp]);
			UI32 iCapa, iCapaConteo = grupoCapas->capas->conteo;
			for(iCapa=0; iCapa<iCapaConteo; iCapa++){
				STGestorEscenaCapa* propiedadesDeCapa = &(grupoCapas->capas->elemento[iCapa]);
				propiedadesDeCapa->objEscena->debugEstablecerBloqueadoActualizandoModelos(bloqueados);
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}
#endif

void NBGestorEscena::actualizarMatricesYModelos(STNBSceneModelsResult* dst){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::actualizarMatricesYModelos")
	NBASSERT(_gestorInicializado);
	NBASSERT(_bufferDatosEstado[_indiceBufferDatosEscribir] == ENGestorEscenaBufferEstado_Vacio); //No debe estar lleno
	NBASSERT(_bufferDatosBloqueado[_indiceBufferDatosEscribir]);	//Debe estar bloqueado
	_secuencialActualizacionesModelos++;
	//
	{
		SI32 iEscena; for(iEscena = (NBGESTORESCENA_MAX_ESCENAS - 1); iEscena >= 0; iEscena--){
			STGestorEscenaEscena* scn = &_escenas[iEscena];
			if(scn->registroOcupado){
				if(scn->escenaHabilitada){
					NBGestorEscena::privActualizarMatricesYModelosDeEscena(iEscena, dst);
				}
				if((scn->sceneMask & (AUOBJETOESCENA_BIT_MATRIX_LOCAL_CHANGED | AUOBJETOESCENA_BIT_MODEL_LOCAL_CHANGED)) != 0){
					dst->countScenesDirty++;
				}
				scn->sceneMask &= ~(AUOBJETOESCENA_BIT_MATRIX_LOCAL_CHANGED | AUOBJETOESCENA_BIT_MODEL_LOCAL_CHANGED);
				dst->countScenes++;
			}
		}
	}
	//
	{
		const UI32 texChanged = NBGestorTexturas::texsChangesCount(TRUE);
		if(dst != NULL) dst->countTexChanged = texChanged;
	}
	//
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

void NBGestorEscena::privActualizarMatricesYModelosDeEscena(const SI32 iEscena, STNBSceneModelsResult* dst){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::privActualizarMatricesYModelosDeEscena")
	NBASSERT(_gestorInicializado);
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	NBASSERT(scn->registroOcupado);
	NBASSERT(_bufferDatosEstado[_indiceBufferDatosEscribir] == ENGestorEscenaBufferEstado_Vacio);		//No debe estar lleno
	NBASSERT(_bufferDatosBloqueado[_indiceBufferDatosEscribir]);	//Debe estar bloqueado
	//
	//PRINTF_INFO("ACTUALIZANDO MODELOS escena(%d) idFBO(%d) idTextureGl(%d)\n", iEscena, scn->idGlFrameBuffer, scn->idGlTextura);
	NBPropRenderizado propsRenderizado;
	propsRenderizado.idActualizacion					= _secuencialActualizacionesModelos;
	propsRenderizado.renderModelos						= NULL;
	propsRenderizado.bytesDatosModelos					= NULL;
	propsRenderizado.piscinaRetecionRender				= NULL;
	//
	propsRenderizado.funcReservarVertices				= &NBGestorEscena::reservarVerticesGL;
	propsRenderizado.verticesGL0						= &NBGestorEscena::verticesGL0;
	propsRenderizado.verticesGL1						= &NBGestorEscena::verticesGL1;
	propsRenderizado.verticesGL2						= &NBGestorEscena::verticesGL2;
	propsRenderizado.verticesGL3						= &NBGestorEscena::verticesGL3;
#	ifdef CONFIG_NB_GESTOR_ESCENAS_MODELOS_MEDIANTE_INDICES
	propsRenderizado.funcReservarIndices				= &NBGestorEscena::reservarIndicesGL;
	propsRenderizado.funcReservarIndicesParaTriangStrip4Independizado				= &NBGestorEscena::reservarIndicesParaTriangStrip4Independizado;
	propsRenderizado.funcReservarIndicesParaTriangStrip4IndependizadoMultiples		= &NBGestorEscena::reservarIndicesParaTriangStrip4IndependizadoMultiples;
	propsRenderizado.funcReservarIndicesParaTriangStripIndependizado				= &NBGestorEscena::reservarIndicesParaTriangStripIndependizado;
	propsRenderizado.funcReservarIndicesParaTriangStripIndependizadoMultiples		= &NBGestorEscena::reservarIndicesParaTriangStripIndependizadoMultiples;
	propsRenderizado.funcReservarIndicesParaTriangStripIndependizadoDesdeTriangFan	= &NBGestorEscena::reservarIndicesParaTriangStripIndependizadoDesdeTriangFan;
	propsRenderizado.indicesGL0							= &NBGestorEscena::indicesGL0;
	propsRenderizado.indicesGL1							= &NBGestorEscena::indicesGL1;
	propsRenderizado.indicesGL2							= &NBGestorEscena::indicesGL2;
	propsRenderizado.indicesGL3							= &NBGestorEscena::indicesGL3;
#	endif
	NBPropHeredadasModelos propsParaHijos;
	propsParaHijos.matrizModificadaPadre				= false;
	NBMATRIZ_ESTABLECER_IDENTIDAD(propsParaHijos.matrizPadre);
	//
	NBASSERT(scn->renderCapasConsumir[_indiceBufferDatosEscribir]->conteo == 0)
	NBASSERT(scn->renderCapasProducir[_indiceBufferDatosEscribir]->conteo == 0)
	//
	UI16 renderRegistrosCapasUsados						= 0;
	UI16 iGrp; for(iGrp = 0; iGrp < ENGestorEscenaGrupo_Conteo; iGrp++){
		STGestorEscenaGrupo* grupoCapas = &(scn->gruposCapas[iGrp]);
		UI32 iCapa, iCapaConteo = grupoCapas->capas->conteo;
		for(iCapa = 0; iCapa < iCapaConteo; iCapa++){
			STGestorEscenaCapa* propiedadesDeCapa = &(grupoCapas->capas->elemento[iCapa]);
			//
			NBMatriz matrizInversaCapa; NBMATRIZ_INVERSA(matrizInversaCapa, propiedadesDeCapa->matrizCapa)
			propsRenderizado.puertoDeVision = scn->puertoDeVision;
			NBMATRIZ_MULTIPLICAR_CAJAAABB(propsRenderizado.cajaProyeccion, matrizInversaCapa, grupoCapas->cajaProyeccion)
			//Update root layer
			propiedadesDeCapa->root->setSceneBox(NBST_P(STNBRectI, (SI32)scn->puertoDeVision.x, (SI32)scn->puertoDeVision.y, (SI32)scn->puertoDeVision.ancho, (SI32)scn->puertoDeVision.alto ), NBST_P(STNBAABox, propsRenderizado.cajaProyeccion.xMin, propsRenderizado.cajaProyeccion.xMax, propsRenderizado.cajaProyeccion.yMin, propsRenderizado.cajaProyeccion.yMax ));
			//Update models
			if(propiedadesDeCapa->habilitada && !propiedadesDeCapa->objEscena->dormido() && propiedadesDeCapa->objEscena->visible()){
				//--------------------------
				//-- Actualizar matrices de modelos
				//--------------------------
				//PRINTF_INFO("Actualizando Matricez para caja: min(%f, %f) max(%f, %f).\n", propsRenderizado.cajaProyeccion.xMin, propsRenderizado.cajaProyeccion.yMin, propsRenderizado.cajaProyeccion.xMax, propsRenderizado.cajaProyeccion.yMax);
				propiedadesDeCapa->objEscena->actualizarArbolMatricesEscena(dst, propsRenderizado, propsParaHijos);
				//--------------------------
				//-- Preparar cacheRender de la capa
				//--------------------------
				NBGestorEscena::privInicializarCacheRender(scn->renderCapas[_indiceBufferDatosEscribir], propiedadesDeCapa, grupoCapas->cajaProyeccion, renderRegistrosCapasUsados);
				STGestorEscenaCapaRender* capaRenderProducir = &(scn->renderCapas[_indiceBufferDatosEscribir]->elemento[renderRegistrosCapasUsados]);
				scn->renderCapasConsumir[_indiceBufferDatosEscribir]->agregarElemento(capaRenderProducir);
				scn->renderCapasProducir[_indiceBufferDatosEscribir]->agregarElemento(capaRenderProducir);
				renderRegistrosCapasUsados++;
				//--------------------------
				//-- Consumir los espejos
				//--------------------------
				NBGestorEscena::privConsumirEspejosEnCapa(capaRenderProducir);
				//--------------------------
				//-- Consumir las cuerdas
				//--------------------------
				NBGestorEscena::privConsumirCuerdasEnCapa(capaRenderProducir);
				//--------------------------
				//-- Consumir el modelo previo a la creacion del modeloGL
				//--------------------------
				NBGestorEscena::privConsumirPreModeloGLEnCapa(capaRenderProducir);
				//--------------------------
				//-- Hacer efectivos los enlaces hacia esta capa
				//--------------------------
				SI16 iOtraEscena; for(iOtraEscena = 0; iOtraEscena < NBGESTORESCENA_MAX_ESCENAS; iOtraEscena++){
					if(_escenas[iOtraEscena].registroOcupado && _escenas[iOtraEscena].escenaHabilitada){
						STGestorEscenaEnlaceCapa* arrEnlaces	= _escenas[iOtraEscena].capasEnlazadas->elemento;
						UI16 i, uso				= _escenas[iOtraEscena].capasEnlazadas->conteo;
						for(i=0; i<uso; i++){
							if(arrEnlaces[i].iEscena == iEscena && arrEnlaces[i].idCapa == propiedadesDeCapa->idCapa){
								NBASSERT(iOtraEscena != iEscena) //No tendria sentido, se pintaria dos veces consecutivas la misma capa
								NBASSERT(_escenas[iOtraEscena].renderCapasConsumir[_indiceBufferDatosEscribir] != NULL)
								_escenas[iOtraEscena].renderCapasConsumir[_indiceBufferDatosEscribir]->agregarElemento(capaRenderProducir);
							}
						}
					}
				}
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

void NBGestorEscena::privInicializarCacheRender(AUArregloNativoMutableP<STGestorEscenaCapaRender>* capasRender, const STGestorEscenaCapa* propiedadesDeCapa, const NBCajaAABB &cajaCapa, const UI16 indiceRegistroUsar){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::privInicializarCacheRender")
	NBASSERT(capasRender != NULL)
	if(indiceRegistroUsar >= capasRender->conteo){
		NBASSERT(indiceRegistroUsar == capasRender->conteo)
		STGestorEscenaCapaRender datosRenderCapa;
		datosRenderCapa.renderModelos						= new(ENMemoriaTipo_General) AUArregloNativoMutableP<STPropRenderModelo>();			NB_DEFINE_NOMBRE_PUNTERO(datosRenderCapa.renderModelos, "NBGestorEscena::.renderModelos")
		datosRenderCapa.renderModelosDatos					= new(ENMemoriaTipo_General) AUArregloNativoMutableP<UI8>(256);						NB_DEFINE_NOMBRE_PUNTERO(datosRenderCapa.renderModelosDatos, "NBGestorEscena::.renderModelosDatos")
		datosRenderCapa.renderPiscinaObjetosRetener			= new(ENMemoriaTipo_General) AUArregloMutable();									NB_DEFINE_NOMBRE_PUNTERO(datosRenderCapa.renderPiscinaObjetosRetener, "NBGestorEscena::.renderPiscinaObjetosRetener")
		datosRenderCapa.renderIluminacion					= new(ENMemoriaTipo_General) AUArregloNativoMutableP<NBFuenteIluminacionRender>();	NB_DEFINE_NOMBRE_PUNTERO(datosRenderCapa.renderIluminacion, "NBGestorEscena::.renderIluminacion")
		datosRenderCapa.renderEspejos						= new(ENMemoriaTipo_General) AUArregloNativoMutableP<NBFuenteEspejoRender>();		NB_DEFINE_NOMBRE_PUNTERO(datosRenderCapa.renderEspejos, "NBGestorEscena::.renderEspejos")
		datosRenderCapa.renderProductoresSombras			= new(ENMemoriaTipo_General) AUArregloNativoMutableP<IEscenaProductorSombras*>();	NB_DEFINE_NOMBRE_PUNTERO(datosRenderCapa.renderProductoresSombras, "NBGestorEscena::.renderProductoresSombras")
		datosRenderCapa.renderDescriptoresCuerdas			= new(ENMemoriaTipo_General) AUArregloNativoMutableP<ICuerdaCortable*>();			NB_DEFINE_NOMBRE_PUNTERO(datosRenderCapa.renderDescriptoresCuerdas, "NBGestorEscena::.renderDescriptoresCuerdas")
		datosRenderCapa.renderConsumidoresLuces				= new(ENMemoriaTipo_General) AUArregloNativoMutableP<IEscenaConsumidorLuces*>();	NB_DEFINE_NOMBRE_PUNTERO(datosRenderCapa.renderConsumidoresLuces, "NBGestorEscena::.renderConsumidoresLuces")
		datosRenderCapa.renderConsumidoresEspejos			= new(ENMemoriaTipo_General) AUArregloNativoMutableP<IEscenaConsumidorEspejos*>();	NB_DEFINE_NOMBRE_PUNTERO(datosRenderCapa.renderConsumidoresEspejos, "NBGestorEscena::.renderConsumidoresEspejos")
		datosRenderCapa.renderConsumidoresCuerdas			= new(ENMemoriaTipo_General) AUArregloNativoMutableP<IEscenaConsumidorCuerdas*>();	NB_DEFINE_NOMBRE_PUNTERO(datosRenderCapa.renderConsumidoresCuerdas, "NBGestorEscena::.renderConsumidoresCuerdas")
		datosRenderCapa.renderConsumidoresPreModeloGL		= new(ENMemoriaTipo_General) AUArregloNativoMutableP<IEscenaConsumidorPreModeloGL*>();	NB_DEFINE_NOMBRE_PUNTERO(datosRenderCapa.renderConsumidoresPreModeloGL, "NBGestorEscena::.renderConsumidoresPreModeloGL")
		datosRenderCapa.renderSombrasPorIluminacion			= new(ENMemoriaTipo_General) AUArregloMutable();									NB_DEFINE_NOMBRE_PUNTERO(datosRenderCapa.renderSombrasPorIluminacion, "NBGestorEscena::.renderSombrasPorIluminacion")
		datosRenderCapa.renderSombrasRangosPorIluminacion	= new(ENMemoriaTipo_General) AUArregloMutable();									NB_DEFINE_NOMBRE_PUNTERO(datosRenderCapa.renderSombrasRangosPorIluminacion, "NBGestorEscena::.renderSombrasRangosPorIluminacion")
		datosRenderCapa.renderSombrasVerticesIntermedios	= new(ENMemoriaTipo_General) AUArregloNativoMutableP<NBPunto>();					NB_DEFINE_NOMBRE_PUNTERO(datosRenderCapa.renderSombrasVerticesIntermedios, "NBGestorEscena::.renderSombrasVerticesIntermedios") //La secuencia de vertices entre el anguloMax y el anguloMin (cuando la sombra es una linea no deposita ningun vertice ene ste arreglo)
		//
		datosRenderCapa.renderConteoLucesAfectanMascara		= 0;
		datosRenderCapa.renderMascLuzSumaFigurasTotal		= 0;
		datosRenderCapa.renderMascLuz						= new(ENMemoriaTipo_General) AUArregloNativoMutableP<STGestorEscenaLuzRender>();	NB_DEFINE_NOMBRE_PUNTERO(datosRenderCapa.renderMascLuz, "NBGestorEscena::.renderMascLuz")
		datosRenderCapa.renderMascLuzBloquesGL				= new(ENMemoriaTipo_General) AUArregloNativoMutableP<STGestorEscenaFiguraRender>();	NB_DEFINE_NOMBRE_PUNTERO(datosRenderCapa.renderMascLuzBloquesGL, "NBGestorEscena::.renderMascLuzBloquesGL")
		datosRenderCapa.renderMascLuzLucesConIntensidad		= new(ENMemoriaTipo_General) AUArregloNativoMutableP<UI16>();						NB_DEFINE_NOMBRE_PUNTERO(datosRenderCapa.renderMascLuzLucesConIntensidad, "NBGestorEscena::.renderMascLuzLucesConIntensidad")
		STGestorEscenaTexturaLuz::inicializar(&datosRenderCapa.renderMascLucesCombinadas);
		//
		capasRender->agregarElemento(datosRenderCapa);
	}
	STGestorEscenaCapaRender* renderCapa					= &(capasRender->elemento[indiceRegistroUsar]);
	renderCapa->tmpImplementarMascaraLuces					= propiedadesDeCapa->tmpImplementarMascaraLuces;
	renderCapa->objetoEscenaCapa							= propiedadesDeCapa->objEscena;
	renderCapa->renderCajaProyeccion						= cajaCapa;
	renderCapa->renderMatrizCapa							= propiedadesDeCapa->matrizCapa;
	NBCOLOR_ESTABLECER(renderCapa->renderColorLuzAmbiente, (float)propiedadesDeCapa->colorLuzAmbiente.r/255.0f, (float)propiedadesDeCapa->colorLuzAmbiente.g/255.0f, (float)propiedadesDeCapa->colorLuzAmbiente.b/255.0f, 1.0f)
	renderCapa->renderConteoLucesAfectanMascara				= 0;
	renderCapa->renderMascLuzSumaFigurasTotal				= 0;
	renderCapa->renderModelosDatos->vaciar();
	renderCapa->renderModelos->vaciar();
	renderCapa->renderPiscinaObjetosRetener->vaciar();
	renderCapa->renderIluminacion->vaciar();
	renderCapa->renderSombrasVerticesIntermedios->vaciar();
	renderCapa->renderEspejos->vaciar();
	renderCapa->renderProductoresSombras->vaciar();
	renderCapa->renderDescriptoresCuerdas->vaciar();
	renderCapa->renderConsumidoresLuces->vaciar();
	renderCapa->renderConsumidoresEspejos->vaciar();
	renderCapa->renderConsumidoresCuerdas->vaciar();
	renderCapa->renderConsumidoresPreModeloGL->vaciar();
	//Copiar productores de sombras
	renderCapa->renderProductoresSombras->agregarElementos(propiedadesDeCapa->productoresSombras->elemento, propiedadesDeCapa->productoresSombras->conteo);
	//Copiar descriptores de cuerdas
	renderCapa->renderDescriptoresCuerdas->agregarElementos(propiedadesDeCapa->descriptoresCuerdas->elemento, propiedadesDeCapa->descriptoresCuerdas->conteo);
	//Copiar consumidores de luces
	renderCapa->renderConsumidoresLuces->agregarElementos(propiedadesDeCapa->consumidoresLuces->elemento, propiedadesDeCapa->consumidoresLuces->conteo);
	//Copiar consumidores de espejos
	renderCapa->renderConsumidoresEspejos->agregarElementos(propiedadesDeCapa->consumidoresEspejos->elemento, propiedadesDeCapa->consumidoresEspejos->conteo);
	//Copiar consumidores de cuerdas
	renderCapa->renderConsumidoresCuerdas->agregarElementos(propiedadesDeCapa->consumidoresCuerdas->elemento, propiedadesDeCapa->consumidoresCuerdas->conteo);
	//Copiar consumidores pre modeloGl
	renderCapa->renderConsumidoresPreModeloGL->agregarElementos(propiedadesDeCapa->consumidoresPreModeloGL->elemento, propiedadesDeCapa->consumidoresPreModeloGL->conteo);
	//Convertir plantillas de espejos a objetos "renderEspejos"
	UI16 iEspejo, conteoEspejos = propiedadesDeCapa->fuentesEspejo->conteo;
	for(iEspejo = 0; iEspejo < conteoEspejos; iEspejo++){
		NBFuenteEspejo* plantilla		= &propiedadesDeCapa->fuentesEspejo->elemento[iEspejo];
		AUEscenaObjeto* objEscena		= (AUEscenaObjeto*)plantilla->objEscena;
		NBFuenteEspejoRender datosRender;
		datosRender.objEspejo			= objEscena;
		datosRender.cajaEscenaEspejo	= objEscena->cajaAABBEnEscena();
		NBMatriz matrizEscena			= objEscena->matrizEscena();
		datosRender.factorReflejaLuz	= plantilla->factorReflejaLuz;
		NBMATRIZ_MULTIPLICAR_PUNTO(datosRender.limitesEscenaEspejo[0], matrizEscena, plantilla->limitesLocalesEspejo[0].x, plantilla->limitesLocalesEspejo[0].y)
		NBMATRIZ_MULTIPLICAR_PUNTO(datosRender.limitesEscenaEspejo[1], matrizEscena, plantilla->limitesLocalesEspejo[1].x, plantilla->limitesLocalesEspejo[1].y)
		NBPUNTO_ESTABLECER(datosRender.vecEspejo, datosRender.limitesEscenaEspejo[1].x-datosRender.limitesEscenaEspejo[0].x, datosRender.limitesEscenaEspejo[1].y-datosRender.limitesEscenaEspejo[0].y)
		datosRender.longitudEscenaEspejo = NBPUNTO_DISTANCIA_VECTOR(datosRender.vecEspejo.x, datosRender.vecEspejo.y);
		datosRender.vecUnitEspejo.x		= datosRender.vecEspejo.x / datosRender.longitudEscenaEspejo;
		datosRender.vecUnitEspejo.y		= datosRender.vecEspejo.y / datosRender.longitudEscenaEspejo;
		NBPUNTO_RADIANES_VECTOR(datosRender.radianesEspejo, 0.0f, 0.0f, datosRender.vecUnitEspejo.x, datosRender.vecUnitEspejo.y)
		renderCapa->renderEspejos->agregarElemento(datosRender);
	}
	//Convertir plantillas de iluminacion a objetos "renderIluminacion"
	UI16 iIlumRender = 0;
	UI16 iIlum, conteoIlum = propiedadesDeCapa->fuentesIluminacion->conteo;
	for(iIlum=0; iIlum<conteoIlum; iIlum++){
		NBFuenteIluminacion* plantilla	= &propiedadesDeCapa->fuentesIluminacion->elemento[iIlum];
		if(plantilla->alcanceLuzzz <= 0.0f) continue;
		AUEscenaObjeto* objEscena		= (AUEscenaObjeto*)plantilla->objEscena;
		NBMatriz matrizEscenaLuz		= objEscena->matrizEscena();
		NBFuenteIluminacionRender datosRender;
		NBMATRIZ_MULTIPLICAR_PUNTO(datosRender.posicionEscenaLuz, matrizEscenaLuz, 0.0f, 0.0f)
		datosRender.objetoOrigenLuz		= plantilla->objEscena;
		datosRender.objetoOrigenRefleja	= NULL;
		datosRender.objetoAgrupaSombras	= plantilla->objAgrupaSombras;
		datosRender.objetoTexturaLuz	= plantilla->texturaLuz;
		if(datosRender.objetoTexturaLuz != NULL) renderCapa->renderPiscinaObjetosRetener->agregarElemento(datosRender.objetoTexturaLuz);
		datosRender.colorLuzF.r			= (float)objEscena->_propiedades.color8.r / 255.0f;
		datosRender.colorLuzF.g			= (float)objEscena->_propiedades.color8.g / 255.0f;
		datosRender.colorLuzF.b			= (float)objEscena->_propiedades.color8.b / 255.0f;
		datosRender.colorLuzF.a			= (float)objEscena->_propiedades.color8.a / 255.0f;
		datosRender.obstruida			= false;
		datosRender.tipoIluminacion		= plantilla->tipoIluminacion;
		datosRender.produceSombras		= plantilla->produceSombras;
		datosRender.limitesAngulosActivos = plantilla->limitesAngulosActivos;
		datosRender.limitesFormaActivos	= false;
		//Caja limites de la luz
		NBMATRIZ_MULTIPLICAR_PUNTO(datosRender.cajaFiguraLuzLocal[0], matrizEscenaLuz, -plantilla->alcanceLuzzz, -plantilla->alcanceLuzzz)		// 3 ------- 2 (cuando es contrareloj)
		NBMATRIZ_MULTIPLICAR_PUNTO(datosRender.cajaFiguraLuzLocal[1], matrizEscenaLuz, plantilla->alcanceLuzzz, -plantilla->alcanceLuzzz)		// |         |
		NBMATRIZ_MULTIPLICAR_PUNTO(datosRender.cajaFiguraLuzLocal[2], matrizEscenaLuz, plantilla->alcanceLuzzz, plantilla->alcanceLuzzz)		// |         |
		NBMATRIZ_MULTIPLICAR_PUNTO(datosRender.cajaFiguraLuzLocal[3], matrizEscenaLuz, -plantilla->alcanceLuzzz, plantilla->alcanceLuzzz)		// 0 ------- 1
		datosRender.cajaFiguraLuzLocal[0].x -= datosRender.posicionEscenaLuz.x; datosRender.cajaFiguraLuzLocal[0].y -= datosRender.posicionEscenaLuz.y;
		datosRender.cajaFiguraLuzLocal[1].x -= datosRender.posicionEscenaLuz.x; datosRender.cajaFiguraLuzLocal[1].y -= datosRender.posicionEscenaLuz.y;
		datosRender.cajaFiguraLuzLocal[2].x -= datosRender.posicionEscenaLuz.x; datosRender.cajaFiguraLuzLocal[2].y -= datosRender.posicionEscenaLuz.y;
		datosRender.cajaFiguraLuzLocal[3].x -= datosRender.posicionEscenaLuz.x; datosRender.cajaFiguraLuzLocal[3].y -= datosRender.posicionEscenaLuz.y;
		NBPunto vector_1_0; NBPUNTO_ESTABLECER(vector_1_0, datosRender.cajaFiguraLuzLocal[1].x-datosRender.cajaFiguraLuzLocal[0].x, datosRender.cajaFiguraLuzLocal[1].y-datosRender.cajaFiguraLuzLocal[0].y);
		NBPunto vector_2_0; NBPUNTO_ESTABLECER(vector_2_0, datosRender.cajaFiguraLuzLocal[2].x-datosRender.cajaFiguraLuzLocal[0].x, datosRender.cajaFiguraLuzLocal[2].y-datosRender.cajaFiguraLuzLocal[0].y);
		if(NBPUNTO_PRODUCTO_VECTORIAL(vector_1_0.x, vector_1_0.y, vector_2_0.x, vector_2_0.y)>0.0f){
			datosRender.cajaFiguraLuzEsContrareloj		= true;
			datosRender.posLocalVectorAvanceX			= datosRender.cajaFiguraLuzLocal[0];
			datosRender.posLocalVectorAvanceY			= datosRender.cajaFiguraLuzLocal[1];
			NBPUNTO_ESTABLECER(datosRender.vecUnitAvanceX, datosRender.cajaFiguraLuzLocal[1].x - datosRender.cajaFiguraLuzLocal[0].x, datosRender.cajaFiguraLuzLocal[1].y - datosRender.cajaFiguraLuzLocal[0].y)
			NBPUNTO_ESTABLECER(datosRender.vecUnitAvanceY, datosRender.cajaFiguraLuzLocal[2].x - datosRender.cajaFiguraLuzLocal[1].x, datosRender.cajaFiguraLuzLocal[2].y - datosRender.cajaFiguraLuzLocal[1].y)
		} else {
			datosRender.cajaFiguraLuzEsContrareloj		= false;
			datosRender.posLocalVectorAvanceX			= datosRender.cajaFiguraLuzLocal[3];
			datosRender.posLocalVectorAvanceY			= datosRender.cajaFiguraLuzLocal[2];
			NBPUNTO_ESTABLECER(datosRender.vecUnitAvanceX, datosRender.cajaFiguraLuzLocal[2].x - datosRender.cajaFiguraLuzLocal[3].x, datosRender.cajaFiguraLuzLocal[2].y - datosRender.cajaFiguraLuzLocal[3].y)
			NBPUNTO_ESTABLECER(datosRender.vecUnitAvanceY, datosRender.cajaFiguraLuzLocal[1].x - datosRender.cajaFiguraLuzLocal[2].x, datosRender.cajaFiguraLuzLocal[1].y - datosRender.cajaFiguraLuzLocal[2].y)
		}
		datosRender.maxAvanceX			= NBPUNTO_DISTANCIA_VECTOR(datosRender.vecUnitAvanceX.x, datosRender.vecUnitAvanceX.y); NBASSERT(datosRender.maxAvanceX>0.0f)
		datosRender.maxAvanceY			= NBPUNTO_DISTANCIA_VECTOR(datosRender.vecUnitAvanceY.x, datosRender.vecUnitAvanceY.y); NBASSERT(datosRender.maxAvanceY>0.0f)
		datosRender.vecUnitAvanceX.x	/= datosRender.maxAvanceX;
		datosRender.vecUnitAvanceX.y	/= datosRender.maxAvanceX;
		datosRender.vecUnitAvanceY.x	/= datosRender.maxAvanceY;
		datosRender.vecUnitAvanceY.y	/= datosRender.maxAvanceY;
		NBASSERT(NBPUNTO_PRODUCTO_VECTORIAL(datosRender.vecUnitAvanceX.x, datosRender.vecUnitAvanceX.y, datosRender.vecUnitAvanceY.x, datosRender.vecUnitAvanceY.y)>0.0f) //Si falla, el vectorY no apunta hacia la izquierda de vectorX
		//
		if(plantilla->limitesAngulosActivos){
			NBASSERT(plantilla->radianesApertura >= 0.0f && plantilla->radianesApertura <= PI)
			NBPunto posLimiteAnguloLocal, posLimiteAnguloEscena; float moduloVector, radianesMin = plantilla->radianesMin, radianesMax = plantilla->radianesMin + plantilla->radianesApertura;
			//
			NBPUNTO_OBTENER_VECTOR_ROTADO(posLimiteAnguloLocal, 1.0f, radianesMin);
			NBMATRIZ_MULTIPLICAR_PUNTO(posLimiteAnguloEscena, matrizEscenaLuz, posLimiteAnguloLocal.x, posLimiteAnguloLocal.y)
			NBPUNTO_ESTABLECER(datosRender.vecUnitAnguloMinDesdeLuz, posLimiteAnguloEscena.x-datosRender.posicionEscenaLuz.x, posLimiteAnguloEscena.y-datosRender.posicionEscenaLuz.y)
			moduloVector = NBPUNTO_DISTANCIA_VECTOR(datosRender.vecUnitAnguloMinDesdeLuz.x, datosRender.vecUnitAnguloMinDesdeLuz.y);
			datosRender.vecUnitAnguloMinDesdeLuz.x /= moduloVector;
			datosRender.vecUnitAnguloMinDesdeLuz.y /= moduloVector;
			//
			NBPUNTO_OBTENER_VECTOR_ROTADO(posLimiteAnguloLocal, 1.0f, radianesMax);
			NBMATRIZ_MULTIPLICAR_PUNTO(posLimiteAnguloEscena, matrizEscenaLuz, posLimiteAnguloLocal.x, posLimiteAnguloLocal.y)
			NBPUNTO_ESTABLECER(datosRender.vecUnitAnguloMaxDesdeLuz, posLimiteAnguloEscena.x-datosRender.posicionEscenaLuz.x, posLimiteAnguloEscena.y-datosRender.posicionEscenaLuz.y)
			moduloVector = NBPUNTO_DISTANCIA_VECTOR(datosRender.vecUnitAnguloMaxDesdeLuz.x, datosRender.vecUnitAnguloMaxDesdeLuz.y);
			datosRender.vecUnitAnguloMaxDesdeLuz.x /= moduloVector;
			datosRender.vecUnitAnguloMaxDesdeLuz.y /= moduloVector;
			//
			if(NBPUNTO_PRODUCTO_VECTORIAL(datosRender.vecUnitAnguloMinDesdeLuz.x, datosRender.vecUnitAnguloMinDesdeLuz.y, datosRender.vecUnitAnguloMaxDesdeLuz.x, datosRender.vecUnitAnguloMaxDesdeLuz.y)<0.0f){
				NBPunto vecTmp = datosRender.vecUnitAnguloMaxDesdeLuz;
				datosRender.vecUnitAnguloMaxDesdeLuz = datosRender.vecUnitAnguloMinDesdeLuz;
				datosRender.vecUnitAnguloMinDesdeLuz = vecTmp;
			}
		}
		NBGestorEscena::privInicializarCacheIluminacion(renderCapa, iIlumRender);
		NBGestorEscena::privActualizarCajaEscenaLuz(datosRender);
		renderCapa->renderIluminacion->agregarElemento(datosRender);
		iIlumRender++;
	}
	NBASSERT(renderCapa->renderIluminacion->conteo <= renderCapa->renderSombrasPorIluminacion->conteo);
	NBASSERT(renderCapa->renderIluminacion->conteo <= renderCapa->renderSombrasRangosPorIluminacion->conteo);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

void NBGestorEscena::privActualizarCajaEscenaLuz(NBFuenteIluminacionRender &datosRender){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::privActualizarCajaEscenaLuz")
	//Envolver centro
	NBCAJAAABB_INICIALIZAR_CON_PUNTO(datosRender.cajaEscenaLuz, 0.0f, 0.0f)
	if(datosRender.limitesAngulosActivos){
		//Envolver vertices que estan entre los vectoresAngulos
		UI16 iVert;
		for(iVert=0; iVert<4; iVert++){
			if(NBPUNTO_PRODUCTO_VECTORIAL(datosRender.vecUnitAnguloMinDesdeLuz.x, datosRender.vecUnitAnguloMinDesdeLuz.y, datosRender.cajaFiguraLuzLocal[iVert].x, datosRender.cajaFiguraLuzLocal[iVert].y) >= 0.0f &&
			   NBPUNTO_PRODUCTO_VECTORIAL(datosRender.vecUnitAnguloMaxDesdeLuz.x, datosRender.vecUnitAnguloMaxDesdeLuz.y, datosRender.cajaFiguraLuzLocal[iVert].x, datosRender.cajaFiguraLuzLocal[iVert].y) <= 0.0f){
				NBCAJAAABB_ENVOLVER_PUNTO_SIGUIENTE(datosRender.cajaEscenaLuz, datosRender.cajaFiguraLuzLocal[iVert].x, datosRender.cajaFiguraLuzLocal[iVert].y)
			}
		}
		//Envolver los puntos interseccion de los angulos limites
		float relA, relaB, denom; NBPunto posInterseccion;
		UI16 iVertAnt = 3;
		for(iVert=0; iVert<4; iVert++){
			NBLINEA_INSTERSECCION_RELATIVOS(relA, relaB, denom, 0.0f, 0.0f, datosRender.vecUnitAnguloMinDesdeLuz.x, datosRender.vecUnitAnguloMinDesdeLuz.y, datosRender.cajaFiguraLuzLocal[iVertAnt].x, datosRender.cajaFiguraLuzLocal[iVertAnt].y, datosRender.cajaFiguraLuzLocal[iVert].x, datosRender.cajaFiguraLuzLocal[iVert].y)
			if(denom != 0.0f && relA>0.0f && relaB >= 0.0f && relaB <= 1.0f){
				NBPUNTO_ESTABLECER(posInterseccion, datosRender.vecUnitAnguloMinDesdeLuz.x * relA, datosRender.vecUnitAnguloMinDesdeLuz.y * relA)
				NBCAJAAABB_ENVOLVER_PUNTO_SIGUIENTE(datosRender.cajaEscenaLuz, posInterseccion.x, posInterseccion.y)
			}
			NBLINEA_INSTERSECCION_RELATIVOS(relA, relaB, denom, 0.0f, 0.0f, datosRender.vecUnitAnguloMaxDesdeLuz.x, datosRender.vecUnitAnguloMaxDesdeLuz.y, datosRender.cajaFiguraLuzLocal[iVertAnt].x, datosRender.cajaFiguraLuzLocal[iVertAnt].y, datosRender.cajaFiguraLuzLocal[iVert].x, datosRender.cajaFiguraLuzLocal[iVert].y)
			if(denom != 0.0f && relA>0.0f && relaB >= 0.0f && relaB <= 1.0f){
				NBPUNTO_ESTABLECER(posInterseccion, datosRender.vecUnitAnguloMaxDesdeLuz.x * relA, datosRender.vecUnitAnguloMaxDesdeLuz.y * relA)
				NBCAJAAABB_ENVOLVER_PUNTO_SIGUIENTE(datosRender.cajaEscenaLuz, posInterseccion.x, posInterseccion.y)
			}
			iVertAnt = iVert;
		}
	} else {
		NBCAJAAABB_ENVOLVER_PUNTO_SIGUIENTE(datosRender.cajaEscenaLuz, datosRender.cajaFiguraLuzLocal[0].x, datosRender.cajaFiguraLuzLocal[0].y)
		NBCAJAAABB_ENVOLVER_PUNTO_SIGUIENTE(datosRender.cajaEscenaLuz, datosRender.cajaFiguraLuzLocal[1].x, datosRender.cajaFiguraLuzLocal[1].y)
		NBCAJAAABB_ENVOLVER_PUNTO_SIGUIENTE(datosRender.cajaEscenaLuz, datosRender.cajaFiguraLuzLocal[2].x, datosRender.cajaFiguraLuzLocal[2].y)
		NBCAJAAABB_ENVOLVER_PUNTO_SIGUIENTE(datosRender.cajaEscenaLuz, datosRender.cajaFiguraLuzLocal[3].x, datosRender.cajaFiguraLuzLocal[3].y)
	}
	datosRender.cajaEscenaLuz.xMin += datosRender.posicionEscenaLuz.x;
	datosRender.cajaEscenaLuz.xMax += datosRender.posicionEscenaLuz.x;
	datosRender.cajaEscenaLuz.yMin += datosRender.posicionEscenaLuz.y;
	datosRender.cajaEscenaLuz.yMax += datosRender.posicionEscenaLuz.y;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

void NBGestorEscena::privInicializarCacheIluminacion(STGestorEscenaCapaRender* renderCapa, UI16 iIlum){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::privInicializarCacheIluminacion")
	//Arreglo de sombras
	AUArregloNativoMutableP<NBFuenteSombra>* arregloSombras = NULL;
	if(renderCapa->renderSombrasPorIluminacion->conteo <= iIlum){
		NBASSERT(renderCapa->renderSombrasPorIluminacion->conteo == iIlum)
		arregloSombras	= new(ENMemoriaTipo_General) AUArregloNativoMutableP<NBFuenteSombra>(); NB_DEFINE_NOMBRE_PUNTERO(arregloSombras, "NBGestorEscena::arregloSombras")
		renderCapa->renderSombrasPorIluminacion->agregarElemento(arregloSombras);
		arregloSombras->liberar(NB_RETENEDOR_NULL);
	} else {
		arregloSombras	= (AUArregloNativoMutableP<NBFuenteSombra>*)renderCapa->renderSombrasPorIluminacion->elemento[iIlum];
	}
	arregloSombras->vaciar();
	//Arreglo de rangos de sombras
	AUArregloNativoMutableP<STRangoSombra>* arregloRangosSmb = NULL;
	if(renderCapa->renderSombrasRangosPorIluminacion->conteo <= iIlum){
		NBASSERT(renderCapa->renderSombrasRangosPorIluminacion->conteo == iIlum)
		arregloRangosSmb	= new(ENMemoriaTipo_General) AUArregloNativoMutableP<STRangoSombra>(); NB_DEFINE_NOMBRE_PUNTERO(arregloRangosSmb, "NBGestorEscena::arregloRangosSmb")
		renderCapa->renderSombrasRangosPorIluminacion->agregarElemento(arregloRangosSmb);
		arregloRangosSmb->liberar(NB_RETENEDOR_NULL);
	} else {
		arregloRangosSmb	= (AUArregloNativoMutableP<STRangoSombra>*)renderCapa->renderSombrasRangosPorIluminacion->elemento[iIlum];
	}
	arregloRangosSmb->vaciar();
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

void NBGestorEscena::consumirModelosYProducirBufferRender(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::consumirModelosYProducirBufferRender")
	//Producir sombras y reflejos de luces
	NBGestorEscena::privProducirSombrasYLucesReflejadas();
	//
	NBASSERT(_gestorInicializado);
	NBASSERT(_bufferDatosBloqueado[_indiceBufferDatosEscribir]);	//Debe estar bloqueado
	NBASSERT(_bufferDatosEstado[_indiceBufferDatosEscribir] == ENGestorEscenaBufferEstado_Vacio);		//No debe estar lleno
	_secuencialActualizacionesModelosGL++;
	NBGestorEscena::resetearVerticesGL();
	//Producir logica de mascaras de iluminacion que seran renderizadas-hacia-texturas
	NBGestorEscena::privProducirMascarasIluminacion();
	//ACTALIZAR MODELOS-GL
	SI32 iEscena;
	for(iEscena = (NBGESTORESCENA_MAX_ESCENAS - 1); iEscena >= 0; iEscena--){
		STGestorEscenaEscena* scn = &_escenas[iEscena];
		if(scn->registroOcupado && scn->escenaHabilitada){
			NBGestorEscena::privConsumirModelosYProducirBufferRenderDeEscena(iEscena);
		}
	}
	//
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

void NBGestorEscena::privConsumirModelosYProducirBufferRenderDeEscena(const SI32 iEscena){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::privConsumirModelosYProducirBufferRenderDeEscena")
	NBASSERT(_gestorInicializado)
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS)
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	NBASSERT(scn->registroOcupado)
	NBASSERT(_bufferDatosEstado[_indiceBufferDatosEscribir] == ENGestorEscenaBufferEstado_Vacio)		//No debe estar lleno
	NBASSERT(_bufferDatosBloqueado[_indiceBufferDatosEscribir])	//Debe estar bloqueado
	//
	//STGestorEscenaFrameBuffer* datosFB	= &_frameBuffersGL[scn->iFramebufferEscena];
	scn->renderAreaOcupadaDestino	= scn->areaOcupadaDestino;
	scn->renderPuertoDeVision		= scn->puertoDeVision;
	scn->renderColorFondo			= scn->colorFondo;
	//PRINTF_INFO("ACTUALIZANDO ILUMINACION escena(%d) idFBO(%d) idTextureGl(%d)\n", iEscena, scn->idGlFrameBuffer, scn->idGlTextura);
	NBPropHeredadasRender propsParaHijos;
	propsParaHijos.iluminoDependenciaPadre				= _lucesSombrasActivos;
	NBCOLOR_ESTABLECER(propsParaHijos.colorPadre, 1.0f, 1.0f, 1.0f, 1.0f);
	NBPropRenderizado propsRenderizado;
	propsRenderizado.idActualizacion					= _secuencialActualizacionesModelosGL;
	//
	propsRenderizado.funcReservarVertices				= &NBGestorEscena::reservarVerticesGL;
	propsRenderizado.verticesGL0						= &NBGestorEscena::verticesGL0;
	propsRenderizado.verticesGL1						= &NBGestorEscena::verticesGL1;
	propsRenderizado.verticesGL2						= &NBGestorEscena::verticesGL2;
	propsRenderizado.verticesGL3						= &NBGestorEscena::verticesGL3;
#	ifdef CONFIG_NB_GESTOR_ESCENAS_MODELOS_MEDIANTE_INDICES
	propsRenderizado.funcReservarIndices				= &NBGestorEscena::reservarIndicesGL;
	propsRenderizado.funcReservarIndicesParaTriangStrip4Independizado				= &NBGestorEscena::reservarIndicesParaTriangStrip4Independizado;
	propsRenderizado.funcReservarIndicesParaTriangStrip4IndependizadoMultiples		= &NBGestorEscena::reservarIndicesParaTriangStrip4IndependizadoMultiples;
	propsRenderizado.funcReservarIndicesParaTriangStripIndependizado				= &NBGestorEscena::reservarIndicesParaTriangStripIndependizado;
	propsRenderizado.funcReservarIndicesParaTriangStripIndependizadoMultiples		= &NBGestorEscena::reservarIndicesParaTriangStripIndependizadoMultiples;
	propsRenderizado.funcReservarIndicesParaTriangStripIndependizadoDesdeTriangFan	= &NBGestorEscena::reservarIndicesParaTriangStripIndependizadoDesdeTriangFan;
	propsRenderizado.indicesGL0							= &NBGestorEscena::indicesGL0;
	propsRenderizado.indicesGL1							= &NBGestorEscena::indicesGL1;
	propsRenderizado.indicesGL2							= &NBGestorEscena::indicesGL2;
	propsRenderizado.indicesGL3							= &NBGestorEscena::indicesGL3;
#	endif
	//
	NBASSERT(scn->renderCapasConsumir[_indiceBufferDatosEscribir] != NULL)
	NBASSERT(scn->renderCapasProducir[_indiceBufferDatosEscribir] != NULL)
	NBASSERT(scn->renderCapas[_indiceBufferDatosEscribir] != NULL)
	STGestorEscenaCapaRender** arrRenderCapa			= scn->renderCapasProducir[_indiceBufferDatosEscribir]->elemento;
	UI16 iRenderCapa/*, iRenderCapa2*/, conteoRenderCapas	= scn->renderCapasProducir[_indiceBufferDatosEscribir]->conteo;
	for(iRenderCapa=0; iRenderCapa<conteoRenderCapas; iRenderCapa++){
		STGestorEscenaCapaRender* renderCapa			= arrRenderCapa[iRenderCapa];
		propsRenderizado.puertoDeVision					= scn->puertoDeVision;
		propsRenderizado.renderModelos					= renderCapa->renderModelos;
		propsRenderizado.bytesDatosModelos				= renderCapa->renderModelosDatos;
		propsRenderizado.piscinaRetecionRender			= renderCapa->renderPiscinaObjetosRetener;
		NBPropIluminacion propsIluminacion;
		propsIluminacion.conteoFuentesIluminacion		= renderCapa->renderIluminacion->conteo;
		propsIluminacion.fuentesIluminacion				= renderCapa->renderIluminacion->elemento;
		propsIluminacion.sombrasPorFuenteIluminacion	= (AUArregloNativoMutableP<NBFuenteSombra>**)renderCapa->renderSombrasPorIluminacion->elemento; NBASSERT(renderCapa->renderIluminacion->conteo <= renderCapa->renderSombrasPorIluminacion->conteo)
		propsIluminacion.sombrasVerticesIntermedios		= renderCapa->renderSombrasVerticesIntermedios; //La secuencia de vertices entre el anguloMax y el anguloMin (cuando la sombra es una linea no deposita ningun vertice ene ste arreglo)
		//Actualizar modelosGL
		NBMatriz matrizInversaCapa; NBMATRIZ_INVERSA(matrizInversaCapa, renderCapa->renderMatrizCapa)
		NBMATRIZ_MULTIPLICAR_CAJAAABB(propsRenderizado.cajaProyeccion, matrizInversaCapa, renderCapa->renderCajaProyeccion)
		//PRINTF_INFO("Enviando comandosGL para caja: min(%f, %f) max(%f, %f).\n", propsRenderizado.cajaProyeccion.xMin, propsRenderizado.cajaProyeccion.yMin, propsRenderizado.cajaProyeccion.xMax, propsRenderizado.cajaProyeccion.yMax);
		renderCapa->objetoEscenaCapa->actualizarArbolModelosGL(propsRenderizado, propsIluminacion, propsParaHijos);
		//
		//PRINTF_INFO("Buffer de datos de capa: %d bytes\n", (SI32)renderCapa->renderModelosDatos->conteo);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

void NBGestorEscena::privProducirSombrasYLucesReflejadas(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::privProducirSombrasYLucesReflejadas")
	NBASSERT(_gestorInicializado);
	NBASSERT(_bufferDatosBloqueado[_indiceBufferDatosEscribir]);	//Debe estar bloqueado
	NBASSERT(_bufferDatosEstado[_indiceBufferDatosEscribir] == ENGestorEscenaBufferEstado_Vacio);		//No debe estar lleno
	if(_lucesSombrasActivos){
		SI32 iEscena;
		for(iEscena = (NBGESTORESCENA_MAX_ESCENAS - 1); iEscena >= 0; iEscena--){
			STGestorEscenaEscena* scn = &(_escenas[iEscena]);
			if(scn->registroOcupado && scn->escenaHabilitada){
				NBASSERT(scn->renderCapasConsumir[_indiceBufferDatosEscribir] != NULL)
				NBASSERT(scn->renderCapasProducir[_indiceBufferDatosEscribir] != NULL)
				NBASSERT(scn->renderCapas[_indiceBufferDatosEscribir] != NULL)
				STGestorEscenaCapaRender** arrRenderCapa	= scn->renderCapasProducir[_indiceBufferDatosEscribir]->elemento;
				UI16 iRenderCapa, conteoRenderCapas			= scn->renderCapasProducir[_indiceBufferDatosEscribir]->conteo;
				for(iRenderCapa=0; iRenderCapa<conteoRenderCapas; iRenderCapa++){
					STGestorEscenaCapaRender* renderCapa	= arrRenderCapa[iRenderCapa];
					//Generar sombras y luces reflejadas
					UI16 vecesReflejosProducidos			= 0;
					UI16 iPrimeraLuz						= 0;
					while(iPrimeraLuz<renderCapa->renderIluminacion->conteo && vecesReflejosProducidos<4){
						UI16 conteoLucesAntesDeProcesar		= renderCapa->renderIluminacion->conteo;
						NBGestorEscena::privProducirSombrasEnCapa(renderCapa, iPrimeraLuz);
						NBGestorEscena::privProducirLucesPorReflejoEnCapa(renderCapa, iPrimeraLuz);
						//Siguiente grupo (las nuevas generadas por reflejo)
						iPrimeraLuz	 = conteoLucesAntesDeProcesar;
						vecesReflejosProducidos++;
					}
					//Generar sombras de la ultima luz proyectada (se termino el ciclo por limite de reflejos, pero no se produjeron sombras finales)
					if(iPrimeraLuz<renderCapa->renderIluminacion->conteo){
						NBGestorEscena::privProducirSombrasEnCapa(renderCapa, iPrimeraLuz);
					}
					//Consumir luces
					NBGestorEscena::privConsumirLucesEnCapa(renderCapa);
				}
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

//
// PRODUCIR VERTICES DE MASCARAS DE ILUMINACION
//

/*bool NBGestorEscena::privIlumSombraIntersectaFigura(const NBPunto* verticesSombra, const UI16 conteoVerticesSombra, const NBPunto* vertices4Figura){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::privIlumSombraIntersectaFigura")
	NBPunto pAnteriorBase = vertices4Figura[3];
	UI16 iBase; bool intersectan = false;
	for(iBase=0; iBase<4; iBase++){
		NBPunto pActBase = vertices4Figura[iBase];
		NBPunto pAntSombra = verticesSombra[0]; //Se ignora el segmento mas lejano formado por (ultimoVert hacia primerVert)
		UI16 iSmb;
		for(iSmb=1; iSmb<conteoVerticesSombra; iSmb++){ //Se ignora el segmento mas lejano
			NBPunto pActSombra = verticesSombra[iSmb];
			NBLINEA_INSTERSECTA(intersectan, pAnteriorBase.x, pAnteriorBase.y, pActBase.x, pActBase.y, pAntSombra.x, pAntSombra.y, pActSombra.x, pActSombra.y)
			if(intersectan){ AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO_ESCENAS(true) }
			pAntSombra = pActSombra;
		}
		pAnteriorBase  = pActBase;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
	return false;
}*/

#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
bool NBGestorEscena::privDebugFiguraEsConvexaContrareloj(const NBPunto* vertices, const SI32 conteoVertices){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::privDebugFiguraEsConvexaContrareloj")
	bool exito = (conteoVertices >= 3); NBASSERT(conteoVertices >= 3)
	NBPunto verticeIniSegAnt = vertices[conteoVertices-2];
	NBPunto verticeFinSegAnt = vertices[conteoVertices-1];
	SI32 iVert; NBPunto vector1, vector2;
	for(iVert=0; iVert<conteoVertices && exito; iVert++){
		NBPunto verticeActual = vertices[iVert];
		NBPUNTO_ESTABLECER(vector1, verticeActual.x-verticeIniSegAnt.x, verticeActual.y-verticeIniSegAnt.y)
		NBPUNTO_ESTABLECER(vector2, verticeFinSegAnt.x-verticeIniSegAnt.x, verticeFinSegAnt.y-verticeIniSegAnt.y)
		float productoCruz	= NBPUNTO_PRODUCTO_VECTORIAL(vector1.x, vector1.y, vector2.x, vector2.y);
		exito = exito && (productoCruz <= 0.0f);
		if(!exito){ NBASSERT(false)}
		NBASSERT(productoCruz <= 0.0f) //Si falla el punto no esta a la izquierda o sobre el segmento (no es convexa contrareloj)
		verticeIniSegAnt	= verticeFinSegAnt;
		verticeFinSegAnt	= verticeActual;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
	return exito;
}
#endif

void NBGestorEscena::privProducirMascarasIluminacion(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::privProducirMascarasIluminacion")
	NBASSERT(_gestorInicializado);
	NBASSERT(_bufferDatosBloqueado[_indiceBufferDatosEscribir]);	//Debe estar bloqueado
	NBASSERT(_bufferDatosEstado[_indiceBufferDatosEscribir] == ENGestorEscenaBufferEstado_Vacio);		//Debe estar no-lleno
	// -----------------------------------------
	// Generar las mascaras de luces individuales
	// -----------------------------------------
	SI32 iEscena;
	for(iEscena = (NBGESTORESCENA_MAX_ESCENAS - 1); iEscena >= 0; iEscena--){
		STGestorEscenaEscena* scn = &(_escenas[iEscena]);
		if(scn->registroOcupado && scn->escenaHabilitada){
			NBASSERT(scn->renderCapas[_indiceBufferDatosEscribir] != NULL)
			NBASSERT(scn->renderCapasConsumir[_indiceBufferDatosEscribir] != NULL)
			NBASSERT(scn->renderCapasProducir[_indiceBufferDatosEscribir] != NULL)
			STGestorEscenaCapaRender** arrRenderCapa	= scn->renderCapasProducir[_indiceBufferDatosEscribir]->elemento;
			UI16 iRenderCapa, conteoRenderCapas			= scn->renderCapasProducir[_indiceBufferDatosEscribir]->conteo;
			for(iRenderCapa=0; iRenderCapa<conteoRenderCapas; iRenderCapa++){
				STGestorEscenaCapaRender* renderCapa	= arrRenderCapa[iRenderCapa];
				NBASSERT(renderCapa->renderConteoLucesAfectanMascara == 0)
				NBASSERT(renderCapa->renderMascLuzSumaFigurasTotal == 0)
				NBASSERT(renderCapa->renderMascLuzBloquesGL->conteo == 0)
				NBASSERT(renderCapa->renderMascLuz->conteo == 0)
				NBASSERT(renderCapa->renderMascLuzLucesConIntensidad->conteo == 0)
				NBASSERT(renderCapa->renderMascLuzSumaFigurasTotal == 0)
				if(renderCapa->tmpImplementarMascaraLuces){
					NBGestorEscena::privProducirMascaraIluminacionDeCapa(iEscena, renderCapa, renderCapa->renderCajaProyeccion);
				}
			}
		}
	}
	// -----------------------------------------
	// Generar las mascaras de luces combinadas
	// y la escena renderizada-en-textura
	// -----------------------------------------
	for(iEscena = (NBGESTORESCENA_MAX_ESCENAS - 1); iEscena >= 0; iEscena--){
		STGestorEscenaEscena* scn = &(_escenas[iEscena]);
		if(scn->registroOcupado && scn->escenaHabilitada){
			NBASSERT(scn->renderCapasConsumir[_indiceBufferDatosEscribir] != NULL)
			NBASSERT(scn->renderCapasProducir[_indiceBufferDatosEscribir] != NULL)
			NBASSERT(scn->renderCapas[_indiceBufferDatosEscribir] != NULL)
			STGestorEscenaCapaRender** arrRenderCapa		= scn->renderCapasProducir[_indiceBufferDatosEscribir]->elemento;
			UI16 iRenderCapa, conteoRenderCapas				= scn->renderCapasProducir[_indiceBufferDatosEscribir]->conteo;
			for(iRenderCapa=0; iRenderCapa<conteoRenderCapas; iRenderCapa++){
				STGestorEscenaCapaRender* renderCapa		= arrRenderCapa[iRenderCapa];
				NBASSERT(renderCapa->renderMascLucesCombinadas.indicesRenderMascLuz.primerElemento == 0 && renderCapa->renderMascLucesCombinadas.indicesRenderMascLuz.cantidadElementos == 0)
				if(renderCapa->renderMascLuz->conteo != 0 && renderCapa->renderConteoLucesAfectanMascara != 0){
					NBASSERT(renderCapa->renderMascLuzSumaFigurasTotal != 0) //Si falla, es inecesario reservar la mascara
					//NBColor8 colorAmbienteCapa8			= renderCapa->objetoEscenaCapa->colorLuzAmbiente8(); //Color de limpieza de acumulado de luz
					//NBColor colorAmbienteCapa;			NBCOLOR_ESTABLECER(colorAmbienteCapa, (float)colorAmbienteCapa8.r/255.0f, (float)colorAmbienteCapa8.g/255.0f, (float)colorAmbienteCapa8.b/255.0f, (float)colorAmbienteCapa8.a/255.0f)
					NBTamanoI tamMascara; NBTAMANO_ESTABLECER(tamMascara, scn->areaOcupadaDestino.ancho * scn->escalaParaMascarasIlum, scn->areaOcupadaDestino.alto * scn->escalaParaMascarasIlum);
					NBRectanguloI areaReservadaLuces;
					const SI32 iTexturaLuces				= NBGestorEscena::privTexturaRenderReservarEspacioSinCrearDestino(iEscena, _indiceBufferDatosEscribir, COLOR_RGBA8, ENTexturaModoPintado_Patron_Suavizado, tamMascara.ancho, tamMascara.alto, &areaReservadaLuces); NBASSERT(iTexturaLuces != -1)
					if(iTexturaLuces != -1){
						NBASSERT(areaReservadaLuces.ancho>0 && areaReservadaLuces.alto>0)
						renderCapa->renderMascLucesCombinadas.iTexRender	= iTexturaLuces;
						renderCapa->renderMascLucesCombinadas.areaReservada = areaReservadaLuces;
						renderCapa->renderMascLucesCombinadas.indicesRenderMascLuz.primerElemento = 0;
						renderCapa->renderMascLucesCombinadas.indicesRenderMascLuz.cantidadElementos = 0;
						STBloqueGL bloqueEscenaGL;
						bloqueEscenaGL.cantidadElementos	= 4;
						bloqueEscenaGL.primerElemento		= NBGestorEscena::reservarVerticesGL(ENVerticeGlTipo_MonoTextura, bloqueEscenaGL.cantidadElementos);
						NBVerticeGL* verticesGL				= &NBGestorEscena::verticesGL1[bloqueEscenaGL.primerElemento];
						const NBCajaAABB cajaEscena			= renderCapa->renderCajaProyeccion;
						NBPUNTO_ESTABLECER(verticesGL[0], cajaEscena.xMin, cajaEscena.yMin);
						NBPUNTO_ESTABLECER(verticesGL[1], cajaEscena.xMax, cajaEscena.yMin);
						NBPUNTO_ESTABLECER(verticesGL[2], cajaEscena.xMin, cajaEscena.yMax);
						NBPUNTO_ESTABLECER(verticesGL[3], cajaEscena.xMax, cajaEscena.yMax);
						/*NBPUNTO_ESTABLECER(verticesGL[0], cajaEscena.xMin, cajaEscena.yMin);
						NBPUNTO_ESTABLECER(verticesGL[1], cajaEscena.xMax - ((cajaEscena.xMax - cajaEscena.xMin) * 0.75f), cajaEscena.yMin);
						NBPUNTO_ESTABLECER(verticesGL[2], cajaEscena.xMin, cajaEscena.yMax - ((cajaEscena.yMax - cajaEscena.yMin) * 0.75f));
						NBPUNTO_ESTABLECER(verticesGL[3], cajaEscena.xMax - ((cajaEscena.xMax - cajaEscena.xMin) * 0.75f), cajaEscena.yMax - ((cajaEscena.yMax - cajaEscena.yMin) * 0.75f));*/
						NBCOLOR_ESTABLECER(verticesGL[0], 255, 255, 255, 255)
						NBCOLOR_ESTABLECER(verticesGL[1], 255, 255, 255, 255)
						NBCOLOR_ESTABLECER(verticesGL[2], 255, 255, 255, 255)
						NBCOLOR_ESTABLECER(verticesGL[3], 255, 255, 255, 255)
						const STGestorEscenaTextura* texParaRender = &_texturasParaRenderizadoGL[iTexturaLuces];
						AUTextura::cargarCoordenadasTextura(verticesGL, ENTexturaModoCoordenadas_Entrelazadas, ENTexturaOrdenV_HaciaArriba, texParaRender->anchoFrameBufferGl, texParaRender->altoFrameBufferGl, areaReservadaLuces.x, areaReservadaLuces.y, areaReservadaLuces.ancho, areaReservadaLuces.alto);
						renderCapa->renderMascLucesCombinadas.indicesRenderMascLuz = NB_GESTOR_GL_DAME_ELEMS_PARA_TRIANGSTRIP_4_ESCNA(ENVerticeGlTipo_MonoTextura, bloqueEscenaGL);
						//PRINTF_INFO("Vertices coord(%f, %f)-(%f, %f)-(%f, %f)-(%f, %f) tex(%f, %f)-(%f, %f)-(%f, %f)-(%f, %f).\n", verticesGL[0].x, verticesGL[0].y, verticesGL[1].x, verticesGL[1].y, verticesGL[2].x, verticesGL[2].y, verticesGL[3].x, verticesGL[3].y, verticesGL[0].tex.x, verticesGL[0].tex.y, verticesGL[1].tex.x, verticesGL[1].tex.y, verticesGL[2].tex.x, verticesGL[2].tex.y, verticesGL[3].tex.x, verticesGL[3].tex.y);
						//PRINTF_INFO("-- --.\n");
					}
				}
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

void NBGestorEscena::privProducirMascaraIluminacionDeCapa(SI32 iEscena, STGestorEscenaCapaRender* renderCapa, const NBCajaAABB &cajaEscena){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::privProducirMascaraIluminacionDeCapa")
	NBASSERT(_gestorInicializado);
	bool soloProducirLucesConBrillo	= (renderCapa->renderColorLuzAmbiente.r == 1.0f && renderCapa->renderColorLuzAmbiente.g == 1.0f && renderCapa->renderColorLuzAmbiente.b == 1.0f);
	NBASSERT(_bufferDatosBloqueado[_indiceBufferDatosEscribir]);	//Debe estar bloqueado
	NBASSERT(_bufferDatosEstado[_indiceBufferDatosEscribir] == ENGestorEscenaBufferEstado_Vacio);		//Debe estar no-lleno
	//
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS)
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	NBASSERT(renderCapa != NULL)
	NBASSERT(renderCapa->renderMascLuz->conteo == 0)
	NBASSERT(renderCapa->renderMascLuzBloquesGL->conteo == 0)
	NBASSERT(renderCapa->renderMascLuzLucesConIntensidad->conteo == 0)
	//
	/*NBPunto verticesCamara[4];
	NBPUNTO_ESTABLECER(verticesCamara[0], cajaEscena.xMin, cajaEscena.yMin)
	NBPUNTO_ESTABLECER(verticesCamara[1], cajaEscena.xMax, cajaEscena.yMin)
	NBPUNTO_ESTABLECER(verticesCamara[2], cajaEscena.xMax, cajaEscena.yMax)
	NBPUNTO_ESTABLECER(verticesCamara[3], cajaEscena.xMin, cajaEscena.yMax)*/
	NBASSERT(renderCapa->renderIluminacion->conteo <= renderCapa->renderSombrasPorIluminacion->conteo)
	NBASSERT(renderCapa->renderIluminacion->conteo <= renderCapa->renderSombrasRangosPorIluminacion->conteo)
	AUArregloNativoMutableP<NBFuenteIluminacionRender>*	lucesRender = renderCapa->renderIluminacion;
	UI16 iLuz;
	for(iLuz=0; iLuz<lucesRender->conteo; iLuz++){
		NBFuenteIluminacionRender* datosLuz			= &lucesRender->elemento[iLuz];
		if(NBCAJAAABB_INTERSECTA_CAJA(cajaEscena, datosLuz->cajaEscenaLuz)
		   && (!soloProducirLucesConBrillo || datosLuz->tipoIluminacion == ENFuenteIlumTipo_MascaraMasBrillo) //Filtra luces-mascaras que son inecesarias porque la luz de ambiente es (1, 1, 1)
		){
			if(!soloProducirLucesConBrillo) renderCapa->renderConteoLucesAfectanMascara++;
			//Datos para calculo de coordenadas de textura
			NBPunto texLuzPosiconCero;		NBPUNTO_ESTABLECER(texLuzPosiconCero, datosLuz->posicionEscenaLuz.x + datosLuz->cajaFiguraLuzLocal[3].x, datosLuz->posicionEscenaLuz.y + datosLuz->cajaFiguraLuzLocal[3].y)
			NBPunto texLuzVecUnitCrecX;		NBPUNTO_ESTABLECER(texLuzVecUnitCrecX, datosLuz->cajaFiguraLuzLocal[2].x - datosLuz->cajaFiguraLuzLocal[3].x, datosLuz->cajaFiguraLuzLocal[2].y - datosLuz->cajaFiguraLuzLocal[3].y)
			NBPunto texLuzVecUnitCrecY;		NBPUNTO_ESTABLECER(texLuzVecUnitCrecY, datosLuz->cajaFiguraLuzLocal[0].x - datosLuz->cajaFiguraLuzLocal[3].x, datosLuz->cajaFiguraLuzLocal[0].y - datosLuz->cajaFiguraLuzLocal[3].y)
			float texLuzExtensionCreceX		= NBPUNTO_DISTANCIA_VECTOR(texLuzVecUnitCrecX.x, texLuzVecUnitCrecX.y);
			float texLuzExtensionCreceY		= NBPUNTO_DISTANCIA_VECTOR(texLuzVecUnitCrecY.x, texLuzVecUnitCrecY.y);
			texLuzVecUnitCrecX.x			/= texLuzExtensionCreceX; texLuzVecUnitCrecX.y		/= texLuzExtensionCreceX;
			texLuzVecUnitCrecY.x			/= texLuzExtensionCreceY; texLuzVecUnitCrecY.y		/= texLuzExtensionCreceY;
			if(!datosLuz->cajaFiguraLuzEsContrareloj){
				texLuzExtensionCreceX		= -texLuzExtensionCreceX;
				texLuzExtensionCreceY		= -texLuzExtensionCreceY;
			}
			//
			STGestorEscenaLuzRender datosLuzRender;
			datosLuzRender.primerFiguraLuz			= 0;
			datosLuzRender.conteoFigurasLuz			= 0;
			datosLuzRender.primerFiguraDecae		= 0;
			datosLuzRender.conteoFigurasDecae		= 0;
			//
			//Segmentar figura iluminada
			AUArregloNativoP<NBFuenteSombra>* sombras = (AUArregloNativoP<NBFuenteSombra>*)renderCapa->renderSombrasPorIluminacion->elemento[iLuz];
			AUArregloNativoP<STRangoSombra>* sombrasRangosPreagrupados = (AUArregloNativoP<STRangoSombra>*)renderCapa->renderSombrasRangosPorIluminacion->elemento[iLuz];
			if(datosLuz->limitesAngulosActivos){
				if(datosLuz->vecUnitAnguloMinDesdeLuz != datosLuz->vecUnitAnguloMaxDesdeLuz){	//Si falla, los limites de angulo convierten a la luz en una linea.
					NBPunto vertsContrarelojLuz[4];
					if(datosLuz->cajaFiguraLuzEsContrareloj){
						vertsContrarelojLuz[0] = datosLuz->cajaFiguraLuzLocal[0]; vertsContrarelojLuz[1] = datosLuz->cajaFiguraLuzLocal[1]; vertsContrarelojLuz[2] = datosLuz->cajaFiguraLuzLocal[2]; vertsContrarelojLuz[3] = datosLuz->cajaFiguraLuzLocal[3];
					} else {
						vertsContrarelojLuz[0] = datosLuz->cajaFiguraLuzLocal[3]; vertsContrarelojLuz[1] = datosLuz->cajaFiguraLuzLocal[2]; vertsContrarelojLuz[2] = datosLuz->cajaFiguraLuzLocal[1]; vertsContrarelojLuz[3] = datosLuz->cajaFiguraLuzLocal[0];
					}
					NBASSERT(NBGestorEscena::privDebugFiguraEsConvexaContrareloj(vertsContrarelojLuz, 4))
					//Cargar figura
					NBSegmentadorFiguras::cargarFiguraActual(vertsContrarelojLuz);
					//Aplicar limites angulos min/max
					NBSegmentadorFiguras::limitarCajaSegunVectoresAngulos(datosLuz->vecUnitAnguloMinDesdeLuz, datosLuz->vecUnitAnguloMaxDesdeLuz);
					if(NBSegmentadorFiguras::figuraActual()->conteo>2){						//Si falla, los limites de angulo convirtieron a la luz en una linea.
						//Extraer porciones en sombra
						if(sombras->conteo != 0) NBSegmentadorFiguras::extraerPorcionesSombrasAFigura(sombras->elemento, sombras->conteo, renderCapa->renderSombrasVerticesIntermedios->elemento, renderCapa->renderSombrasVerticesIntermedios->conteo);
						if(!datosLuz->limitesFormaActivos){
							//-------------------
							//-- Caso de luz solo con limites-angulos
							//-------------------
							NBASSERT(sombrasRangosPreagrupados->conteo == 0) //Por ahora, solo se usan para luces reflejadas
							AUArregloNativoMutableP<NBPunto>* figuraActual = NBSegmentadorFiguras::figuraActual();
							//Crear suavizado de sombras
							datosLuzRender.primerFiguraDecae	= renderCapa->renderMascLuzBloquesGL->conteo;
							NBGestorEscena::privIlumAgregarFiguraLuzDecaeNormalizadaNoLimitada(renderCapa, datosLuz, vertsContrarelojLuz, figuraActual->elemento, figuraActual->conteo, texLuzPosiconCero, texLuzVecUnitCrecX, texLuzVecUnitCrecY, texLuzExtensionCreceX, texLuzExtensionCreceY);
							//NBGestorEscena::privIlumAgregarFiguraLuzDecaeSimpleNoLimitada(renderCapa, datosLuz, vertsContrarelojLuz, figuraActual->elemento, figuraActual->conteo, texLuzPosiconCero, texLuzVecUnitCrecX, texLuzVecUnitCrecY, texLuzExtensionCreceX, texLuzExtensionCreceY);
							datosLuzRender.conteoFigurasDecae	= renderCapa->renderMascLuzBloquesGL->conteo - datosLuzRender.primerFiguraDecae;
							//Crear la unica figura luz
							datosLuzRender.primerFiguraLuz		= renderCapa->renderMascLuzBloquesGL->conteo;
							NBGestorEscena::privIlumAgregarFiguraLuzDesdeCroquisUnico(renderCapa, datosLuz, figuraActual->elemento, figuraActual->conteo, texLuzPosiconCero, texLuzVecUnitCrecX, texLuzVecUnitCrecY, texLuzExtensionCreceX, texLuzExtensionCreceY);
							datosLuzRender.conteoFigurasLuz		= renderCapa->renderMascLuzBloquesGL->conteo - datosLuzRender.primerFiguraLuz;
						} else {
							//-------------------
							//-- Caso de luz con limites-angulos y limite-forma
							//-------------------
							//Aplicar sombras preagrupadas (heredadas por reflejo en espejo)
							if(sombrasRangosPreagrupados->conteo != 0) NBSegmentadorFiguras::extraerProcionesRangosSombrasAFigura(sombrasRangosPreagrupados->elemento, sombrasRangosPreagrupados->conteo);
							//Crear suavizado de sombras
							datosLuzRender.primerFiguraDecae	= renderCapa->renderMascLuzBloquesGL->conteo;
							AUArregloNativoMutableP<NBPunto>* figuraAntesDeSegmentar = NBSegmentadorFiguras::figuraActual();
							NBGestorEscena::privIlumAgregarFiguraLuzDecaeNormalizadaLimitada(renderCapa, datosLuz, vertsContrarelojLuz, figuraAntesDeSegmentar->elemento, figuraAntesDeSegmentar->conteo, datosLuz->posIniVecLimiteForma, datosLuz->vecUnitLimiteForma, texLuzPosiconCero, texLuzVecUnitCrecX, texLuzVecUnitCrecY, texLuzExtensionCreceX, texLuzExtensionCreceY);
							//NBGestorEscena::privIlumAgregarFiguraLuzDecaeSimpleLimitada(renderCapa, datosLuz, vertsContrarelojLuz, figuraAntesDeSegmentar->elemento, figuraAntesDeSegmentar->conteo, datosLuz->posIniVecLimiteForma, datosLuz->vecUnitLimiteForma, texLuzPosiconCero, texLuzVecUnitCrecX, texLuzVecUnitCrecY, texLuzExtensionCreceX, texLuzExtensionCreceY);
							datosLuzRender.conteoFigurasDecae	= renderCapa->renderMascLuzBloquesGL->conteo - datosLuzRender.primerFiguraDecae;
							//Aplicar limite de figura
							NBSegmentadorFiguras::dividirEnPorcionesDerechaLinea(datosLuz->posIniVecLimiteForma, datosLuz->vecUnitLimiteForma);
							//Crear las subfiguras convexas
							datosLuzRender.primerFiguraLuz		= renderCapa->renderMascLuzBloquesGL->conteo;
							const UI16 conteoSubFiguras			= NBSegmentadorFiguras::cacheConteoFiguras();
							AUArregloNativoMutableP<NBPunto>** subFiguras = NBSegmentadorFiguras::cacheFiguras();
							NBColor8 colorLuz8; NBCOLOR_ESTABLECER(colorLuz8, 255.0f*datosLuz->colorLuzF.r, 255.0f*datosLuz->colorLuzF.g, 255.0f*datosLuz->colorLuzF.b, 255)
							UI16 iSubFig;
							for(iSubFig=0; iSubFig<conteoSubFiguras; iSubFig++){
								AUArregloNativoMutableP<NBPunto>* unaFigura = subFiguras[iSubFig]; NBASSERT(unaFigura->conteo != 0)
								const NBPunto* verticeLuz		= unaFigura->elemento;
								const UI16  conteoVertsFig		= unaFigura->conteo;
								STBloqueGL bloqueLuzGL;
								bloqueLuzGL.cantidadElementos	= conteoVertsFig + 1; // + 1 para el vertice que cierra el ciclo
								bloqueLuzGL.primerElemento		= NBGestorEscena::reservarVerticesGL(ENVerticeGlTipo_MonoTextura, bloqueLuzGL.cantidadElementos);
								NBVerticeGL* verticesGLLuz		= &NBGestorEscena::verticesGL1[bloqueLuzGL.primerElemento];
								UI16 iVertFig, iVertGL			= 0;
								//Ultimo vertice (PENDIENTE: es necesario????)
								NBPUNTO_ESTABLECER(verticesGLLuz[iVertGL], datosLuz->posicionEscenaLuz.x + verticeLuz[conteoVertsFig-1].x, datosLuz->posicionEscenaLuz.y + verticeLuz[conteoVertsFig-1].y);
								NBCOLOR_ESTABLECER(verticesGLLuz[iVertGL], colorLuz8.r, colorLuz8.g, colorLuz8.b, 255)
								iVertGL++;
								//Secuencia de vertices (forman el triangleFan)
								for(iVertFig=0; iVertFig<conteoVertsFig; iVertFig++){
									NBPUNTO_ESTABLECER(verticesGLLuz[iVertGL], datosLuz->posicionEscenaLuz.x + verticeLuz[iVertFig].x, datosLuz->posicionEscenaLuz.y + verticeLuz[iVertFig].y);
									NBCOLOR_ESTABLECER(verticesGLLuz[iVertGL], colorLuz8.r, colorLuz8.g, colorLuz8.b, 255)
									iVertGL++;
								}
								datosLuz->objetoTexturaLuz->cargarCoordenadasTextura(verticesGLLuz, bloqueLuzGL.cantidadElementos, texLuzPosiconCero, texLuzVecUnitCrecX, texLuzVecUnitCrecY, texLuzExtensionCreceX, texLuzExtensionCreceY);
								STGestorEscenaFiguraRender datosFiguraLuz;
								datosFiguraLuz.bloqueIndicesGL	= NB_GESTOR_GL_DAME_ELEMS_PARA_TRIANGSTRIP_N_DESDE_TRIANGFAN_ESCNA(ENVerticeGlTipo_MonoTextura, bloqueLuzGL, bloqueLuzGL.cantidadElementos);
								datosFiguraLuz.esTriangFan		= true;
								datosFiguraLuz.idTextura0		= datosLuz->objetoTexturaLuz->idTexturaGL;
								datosFiguraLuz.idTextura1		= 0;
								renderCapa->renderMascLuzBloquesGL->agregarElemento(datosFiguraLuz);
							}
							datosLuzRender.conteoFigurasLuz	= renderCapa->renderMascLuzBloquesGL->conteo - datosLuzRender.primerFiguraLuz;
						}
					}
				}
			} else if(sombras->conteo != 0){
				//-------------------
				//-- Caso de luz solo con sombras (sin limitesAngulos ni limitesForma)
				//-------------------
				NBASSERT(sombrasRangosPreagrupados->conteo == 0) //Por ahora, solo se usan para luces reflejadas
				NBPunto vertsContrarelojLuz[4];
				if(datosLuz->cajaFiguraLuzEsContrareloj){
					vertsContrarelojLuz[0] = datosLuz->cajaFiguraLuzLocal[0]; vertsContrarelojLuz[1] = datosLuz->cajaFiguraLuzLocal[1]; vertsContrarelojLuz[2] = datosLuz->cajaFiguraLuzLocal[2]; vertsContrarelojLuz[3] = datosLuz->cajaFiguraLuzLocal[3];
				} else {
					vertsContrarelojLuz[0] = datosLuz->cajaFiguraLuzLocal[3]; vertsContrarelojLuz[1] = datosLuz->cajaFiguraLuzLocal[2]; vertsContrarelojLuz[2] = datosLuz->cajaFiguraLuzLocal[1]; vertsContrarelojLuz[3] = datosLuz->cajaFiguraLuzLocal[0];
				}
				NBASSERT(NBGestorEscena::privDebugFiguraEsConvexaContrareloj(vertsContrarelojLuz, 4))
				//Cargar figura
				NBSegmentadorFiguras::cargarFiguraActual(vertsContrarelojLuz);
				//Extraer porciones en sombra
				NBSegmentadorFiguras::extraerPorcionesSombrasAFigura(sombras->elemento, sombras->conteo, renderCapa->renderSombrasVerticesIntermedios->elemento, renderCapa->renderSombrasVerticesIntermedios->conteo);
				AUArregloNativoMutableP<NBPunto>* figuraActual = NBSegmentadorFiguras::figuraActual();
				//Crear suavizado de sombras
				datosLuzRender.primerFiguraDecae	= renderCapa->renderMascLuzBloquesGL->conteo;
				NBGestorEscena::privIlumAgregarFiguraLuzDecaeNormalizadaNoLimitada(renderCapa, datosLuz, vertsContrarelojLuz, figuraActual->elemento, figuraActual->conteo, texLuzPosiconCero, texLuzVecUnitCrecX, texLuzVecUnitCrecY, texLuzExtensionCreceX, texLuzExtensionCreceY);
				//NBGestorEscena::privIlumAgregarFiguraLuzDecaeSimpleNoLimitada(renderCapa, datosLuz, vertsContrarelojLuz, figuraActual->elemento, figuraActual->conteo, texLuzPosiconCero, texLuzVecUnitCrecX, texLuzVecUnitCrecY, texLuzExtensionCreceX, texLuzExtensionCreceY);
				datosLuzRender.conteoFigurasDecae	= renderCapa->renderMascLuzBloquesGL->conteo - datosLuzRender.primerFiguraDecae;
				//Crear la unica figura luz
				datosLuzRender.primerFiguraLuz		= renderCapa->renderMascLuzBloquesGL->conteo;
				NBGestorEscena::privIlumAgregarFiguraLuzDesdeCroquisUnico(renderCapa, datosLuz, figuraActual->elemento, figuraActual->conteo, texLuzPosiconCero, texLuzVecUnitCrecX, texLuzVecUnitCrecY, texLuzExtensionCreceX, texLuzExtensionCreceY);
				datosLuzRender.conteoFigurasLuz		= renderCapa->renderMascLuzBloquesGL->conteo - datosLuzRender.primerFiguraLuz;
			} else {
				//-------------------
				//-- Caso de luz intacta (sin sombra, limitesAngulos ni limitesForma)
				//-------------------
				NBASSERT(sombrasRangosPreagrupados->conteo == 0) //Por ahora, solo se usan para luces reflejadas
				datosLuzRender.primerFiguraLuz		= renderCapa->renderMascLuzBloquesGL->conteo;
				NBGestorEscena::privIlumAgregarFiguraLuzDesdeCroquisUnico(renderCapa, datosLuz, datosLuz->cajaFiguraLuzLocal, 4, texLuzPosiconCero, texLuzVecUnitCrecX, texLuzVecUnitCrecY, texLuzExtensionCreceX, texLuzExtensionCreceY);
				datosLuzRender.conteoFigurasLuz		= renderCapa->renderMascLuzBloquesGL->conteo - datosLuzRender.primerFiguraLuz;
			}
			//Ignorar si termina vacia
			if(datosLuzRender.conteoFigurasLuz != 0 || datosLuzRender.conteoFigurasDecae != 0){
				if(datosLuz->tipoIluminacion == ENFuenteIlumTipo_MascaraMasBrillo) renderCapa->renderMascLuzLucesConIntensidad->agregarElemento(renderCapa->renderMascLuz->conteo);
				renderCapa->renderMascLuz->agregarElemento(datosLuzRender);
				renderCapa->renderMascLuzSumaFigurasTotal	+= datosLuzRender.conteoFigurasLuz + datosLuzRender.conteoFigurasDecae;
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

void NBGestorEscena::privIlumAgregarFiguraLuzDesdeCroquisUnico(STGestorEscenaCapaRender* renderCapa, const NBFuenteIluminacionRender* datosLuz, const NBPunto* verticeLuz, const UI16 conteoVertsFig, const NBPunto &texLuzPosiconCero, const NBPunto &texLuzVecUnitCrecX, const NBPunto &texLuzVecUnitCrecY, const float texLuzExtensionCreceX, const float texLuzExtensionCreceY){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::privIlumAgregarFiguraLuzDesdeCroquisUnico")
	NBASSERT(conteoVertsFig != 0)
	NBColor8 colorLuz8;					NBCOLOR_ESTABLECER(colorLuz8, 255.0f*datosLuz->colorLuzF.r, 255.0f*datosLuz->colorLuzF.g, 255.0f*datosLuz->colorLuzF.b, 255)
	STBloqueGL bloqueLuzGL;
	bloqueLuzGL.cantidadElementos		= conteoVertsFig + 2; // + 1 para el vertice centro, + 1 para el vertice que cierra el ciclo (necesario porque no hay garantia que el primer vertice sea el centro de la luz)
	bloqueLuzGL.primerElemento			= NBGestorEscena::reservarVerticesGL(ENVerticeGlTipo_MonoTextura, bloqueLuzGL.cantidadElementos);
	NBVerticeGL* verticesGLLuz			= &NBGestorEscena::verticesGL1[bloqueLuzGL.primerElemento];
	UI16 iVertFig;
	//Vertice del centro
	NBPUNTO_ESTABLECER(verticesGLLuz[0], datosLuz->posicionEscenaLuz.x, datosLuz->posicionEscenaLuz.y);
	NBCOLOR_ESTABLECER(verticesGLLuz[0], colorLuz8.r, colorLuz8.g, colorLuz8.b, 255)
	verticesGLLuz++; NBASSERT(verticesGLLuz <= &NBGestorEscena::verticesGL1[bloqueLuzGL.primerElemento+bloqueLuzGL.cantidadElementos])
	//Ultimo vertice (para que la figura se cierre)
	NBPUNTO_ESTABLECER(verticesGLLuz[0], datosLuz->posicionEscenaLuz.x + verticeLuz[conteoVertsFig-1].x, datosLuz->posicionEscenaLuz.y + verticeLuz[conteoVertsFig-1].y);
	NBCOLOR_ESTABLECER(verticesGLLuz[0], colorLuz8.r, colorLuz8.g, colorLuz8.b, 255)
	verticesGLLuz++; NBASSERT(verticesGLLuz <= &NBGestorEscena::verticesGL1[bloqueLuzGL.primerElemento+bloqueLuzGL.cantidadElementos])
	//Secuencia de vertices
	for(iVertFig=0; iVertFig<conteoVertsFig; iVertFig++){
		NBPUNTO_ESTABLECER(verticesGLLuz[0], datosLuz->posicionEscenaLuz.x + verticeLuz[iVertFig].x, datosLuz->posicionEscenaLuz.y + verticeLuz[iVertFig].y);
		NBCOLOR_ESTABLECER(verticesGLLuz[0], colorLuz8.r, colorLuz8.g, colorLuz8.b, 255)
		verticesGLLuz++;
	}
	NBASSERT(verticesGLLuz == &NBGestorEscena::verticesGL1[bloqueLuzGL.primerElemento+bloqueLuzGL.cantidadElementos])
	datosLuz->objetoTexturaLuz->cargarCoordenadasTextura(&NBGestorEscena::verticesGL1[bloqueLuzGL.primerElemento], bloqueLuzGL.cantidadElementos, texLuzPosiconCero, texLuzVecUnitCrecX, texLuzVecUnitCrecY, texLuzExtensionCreceX, texLuzExtensionCreceY);
	//
	STGestorEscenaFiguraRender datosFiguraLuz;
	datosFiguraLuz.bloqueIndicesGL	= NB_GESTOR_GL_DAME_ELEMS_PARA_TRIANGSTRIP_N_DESDE_TRIANGFAN_ESCNA(ENVerticeGlTipo_MonoTextura, bloqueLuzGL, bloqueLuzGL.cantidadElementos);
	datosFiguraLuz.esTriangFan		= true;
	datosFiguraLuz.idTextura0		= datosLuz->objetoTexturaLuz->idTexturaGL;
	datosFiguraLuz.idTextura1		= 0;
	renderCapa->renderMascLuzBloquesGL->agregarElemento(datosFiguraLuz);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

void NBGestorEscena::privIlumAgregarFiguraLuzDecaeNormalizadaNoLimitada(STGestorEscenaCapaRender* renderCapa, const NBFuenteIluminacionRender* datosLuz, const NBPunto* vertsContrarelojLuz, const NBPunto* verticeFig, const UI16 conteoVertsFig, const NBPunto &texLuzPosiconCero, const NBPunto &texLuzVecUnitCrecX, const NBPunto &texLuzVecUnitCrecY, const float texLuzExtensionCreceX, const float texLuzExtensionCreceY){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::privIlumAgregarFiguraLuzDecaeNormalizadaNoLimitada")
	//Se suavizan las sombras de todos los vertices
	//
	//Se normalizan los vectores de cada dos segmentos consecutivos, se suman, y se obtiene el vector derecha del resultado.
	//Se intenta crear una figura desde cada vertice hacia su "vertice-alejado".
	NBASSERT(conteoVertsFig >= 3)
	float tamSalienteDecae				= 8.0f;
	NBPunto posEscenaLuz				= datosLuz->posicionEscenaLuz;
	NBColor8 colorLuz8;					NBCOLOR_ESTABLECER(colorLuz8, 255.0f*datosLuz->colorLuzF.r, 255.0f*datosLuz->colorLuzF.g, 255.0f*datosLuz->colorLuzF.b, 255)
	NBPunto vertice1, vertice2, vertice3, vecUnitSeg1, vecUnitSeg2, vecUnitSuma, posSalida;
	float modVec1, modVec2, modVecSuma;
	float avanceInterseccion[4], denominador[4],avanceInterseccionMin = 0.0f;
	//Se generara un solo TRIANG_STRIP
	STBloqueGL bloqueDecaeGL;
	bloqueDecaeGL.cantidadElementos		= (1 + conteoVertsFig) * 2; // + 1 para cerrar el ciclo
	bloqueDecaeGL.primerElemento		= NBGestorEscena::reservarVerticesGL(ENVerticeGlTipo_MonoTextura, bloqueDecaeGL.cantidadElementos);
	NBVerticeGL* verticesGLDecae		= &NBGestorEscena::verticesGL1[bloqueDecaeGL.primerElemento];
	{
		//Vectores unitarios iniciales
		vertice1						= verticeFig[conteoVertsFig-3];
		vertice2						= verticeFig[conteoVertsFig-2];
		vertice3						= verticeFig[conteoVertsFig-1];
		NBPUNTO_ESTABLECER(vecUnitSeg1, vertice2.x-vertice1.x, vertice2.y-vertice1.y) NBASSERT(vertice2 != vertice1)
		modVec1							= NBPUNTO_DISTANCIA_VECTOR(vecUnitSeg1.x, vecUnitSeg1.y); NBASSERT(modVec1 != 0.0f)
		vecUnitSeg1.x					/= modVec1;
		vecUnitSeg1.y					/= modVec1;
		NBPUNTO_ESTABLECER(vecUnitSeg2, vertice3.x-vertice2.x, vertice3.y-vertice2.y) NBASSERT(vertice3 != vertice2)
		modVec2							= NBPUNTO_DISTANCIA_VECTOR(vecUnitSeg2.x, vecUnitSeg2.y); NBASSERT(modVec2 != 0.0f)
		vecUnitSeg2.x					/= modVec2;
		vecUnitSeg2.y					/= modVec2;
		//Vector suma (su derecha apunta "hacia fuera")
		NBPUNTO_ESTABLECER(vecUnitSuma, vecUnitSeg1.x + vecUnitSeg2.x, vecUnitSeg1.y + vecUnitSeg2.y);
		modVecSuma						= NBPUNTO_DISTANCIA_VECTOR(vecUnitSuma.x, vecUnitSuma.y); NBASSERT(modVecSuma != 0.0f)
		vecUnitSuma.x					/= modVecSuma;
		vecUnitSuma.y					/= modVecSuma;
		//Posicion para suavizar la luz
		NBPUNTO_ESTABLECER(posSalida, vertice2.x + (vecUnitSuma.y * tamSalienteDecae), vertice2.y + (-vecUnitSuma.x * tamSalienteDecae));
		NBLINEA_INSTERSECCION_RELATIVO_L1(avanceInterseccion[0], denominador[0], 0.0f, 0.0f, posSalida.x, posSalida.y, vertsContrarelojLuz[0].x, vertsContrarelojLuz[0].y, vertsContrarelojLuz[1].x, vertsContrarelojLuz[1].y)
		NBLINEA_INSTERSECCION_RELATIVO_L1(avanceInterseccion[1], denominador[1], 0.0f, 0.0f, posSalida.x, posSalida.y, vertsContrarelojLuz[1].x, vertsContrarelojLuz[1].y, vertsContrarelojLuz[2].x, vertsContrarelojLuz[2].y)
		NBLINEA_INSTERSECCION_RELATIVO_L1(avanceInterseccion[2], denominador[2], 0.0f, 0.0f, posSalida.x, posSalida.y, vertsContrarelojLuz[2].x, vertsContrarelojLuz[2].y, vertsContrarelojLuz[3].x, vertsContrarelojLuz[3].y)
		NBLINEA_INSTERSECCION_RELATIVO_L1(avanceInterseccion[3], denominador[3], 0.0f, 0.0f, posSalida.x, posSalida.y, vertsContrarelojLuz[3].x, vertsContrarelojLuz[3].y, vertsContrarelojLuz[0].x, vertsContrarelojLuz[0].y)
		avanceInterseccionMin			= (denominador[0] == 0.0f ? 0.0f : avanceInterseccion[0]);
		if(denominador[1] != 0.0f && avanceInterseccion[1]>0.0f && (avanceInterseccion[1]<avanceInterseccionMin || avanceInterseccionMin <= 0.0f)) avanceInterseccionMin = avanceInterseccion[1];
		if(denominador[2] != 0.0f && avanceInterseccion[2]>0.0f && (avanceInterseccion[2]<avanceInterseccionMin || avanceInterseccionMin <= 0.0f)) avanceInterseccionMin = avanceInterseccion[2];
		if(denominador[3] != 0.0f && avanceInterseccion[3]>0.0f && (avanceInterseccion[3]<avanceInterseccionMin || avanceInterseccionMin <= 0.0f)) avanceInterseccionMin = avanceInterseccion[3];
		NBASSERT(avanceInterseccionMin>0.0f)
		if(avanceInterseccionMin<1.01f){
			posSalida					= vertice2;
		} else {
			posSalida.x					+= (vertice2.x  * (avanceInterseccionMin - 1.0f) * 0.05f);
			posSalida.y					+= (vertice2.y  * (avanceInterseccionMin - 1.0f) * 0.05f);
		}
		//
		NBPUNTO_ESTABLECER(verticesGLDecae[0], vertice2.x + posEscenaLuz.x, vertice2.y + posEscenaLuz.y)
		NBCOLOR_ESTABLECER(verticesGLDecae[0], colorLuz8.r, colorLuz8.g, colorLuz8.b, 255) //colorLuz8.r, colorLuz8.g, colorLuz8.b
		NBPUNTO_ESTABLECER(verticesGLDecae[1], posSalida.x + posEscenaLuz.x, posSalida.y + posEscenaLuz.y)
		NBCOLOR_ESTABLECER(verticesGLDecae[1], 0, 0, 0, 255) //0
		verticesGLDecae					+= 2; NBASSERT(verticesGLDecae <= &NBGestorEscena::verticesGL1[bloqueDecaeGL.primerElemento + bloqueDecaeGL.cantidadElementos])
		//Siguiente
		vertice1						= vertice2;
		vertice2						= vertice3;
		vecUnitSeg1						= vecUnitSeg2;
	}
	//Recorrer todos los vertices
	UI16 iVert;
	for(iVert=0; iVert<conteoVertsFig; iVert++){
		//Nuevo vector actual
		vertice3						= verticeFig[iVert];
		NBPUNTO_ESTABLECER(vecUnitSeg2, vertice3.x-vertice2.x, vertice3.y-vertice2.y) NBASSERT(vertice3 != vertice2)
		modVec2							= NBPUNTO_DISTANCIA_VECTOR(vecUnitSeg2.x, vecUnitSeg2.y); NBASSERT(modVec2 != 0.0f)
		vecUnitSeg2.x					/= modVec2;
		vecUnitSeg2.y					/= modVec2;
		//Vector suma (su derecha apunta "hacia fuera")
		NBPUNTO_ESTABLECER(vecUnitSuma, vecUnitSeg1.x + vecUnitSeg2.x, vecUnitSeg1.y + vecUnitSeg2.y);
		modVecSuma						= NBPUNTO_DISTANCIA_VECTOR(vecUnitSuma.x, vecUnitSuma.y); NBASSERT(modVecSuma != 0.0f)
		vecUnitSuma.x					/= modVecSuma;
		vecUnitSuma.y					/= modVecSuma;
		//
		NBPUNTO_ESTABLECER(posSalida, vertice2.x + (vecUnitSuma.y * tamSalienteDecae), vertice2.y + (-vecUnitSuma.x * tamSalienteDecae));
		NBLINEA_INSTERSECCION_RELATIVO_L1(avanceInterseccion[0], denominador[0], 0.0f, 0.0f, posSalida.x, posSalida.y, vertsContrarelojLuz[0].x, vertsContrarelojLuz[0].y, vertsContrarelojLuz[1].x, vertsContrarelojLuz[1].y)
		NBLINEA_INSTERSECCION_RELATIVO_L1(avanceInterseccion[1], denominador[1], 0.0f, 0.0f, posSalida.x, posSalida.y, vertsContrarelojLuz[1].x, vertsContrarelojLuz[1].y, vertsContrarelojLuz[2].x, vertsContrarelojLuz[2].y)
		NBLINEA_INSTERSECCION_RELATIVO_L1(avanceInterseccion[2], denominador[2], 0.0f, 0.0f, posSalida.x, posSalida.y, vertsContrarelojLuz[2].x, vertsContrarelojLuz[2].y, vertsContrarelojLuz[3].x, vertsContrarelojLuz[3].y)
		NBLINEA_INSTERSECCION_RELATIVO_L1(avanceInterseccion[3], denominador[3], 0.0f, 0.0f, posSalida.x, posSalida.y, vertsContrarelojLuz[3].x, vertsContrarelojLuz[3].y, vertsContrarelojLuz[0].x, vertsContrarelojLuz[0].y)
		avanceInterseccionMin		= (denominador[0] == 0.0f?0.0f:avanceInterseccion[0]);
		if(denominador[1] != 0.0f && avanceInterseccion[1]>0.0f && (avanceInterseccion[1]<avanceInterseccionMin || avanceInterseccionMin <= 0.0f)) avanceInterseccionMin = avanceInterseccion[1];
		if(denominador[2] != 0.0f && avanceInterseccion[2]>0.0f && (avanceInterseccion[2]<avanceInterseccionMin || avanceInterseccionMin <= 0.0f)) avanceInterseccionMin = avanceInterseccion[2];
		if(denominador[3] != 0.0f && avanceInterseccion[3]>0.0f && (avanceInterseccion[3]<avanceInterseccionMin || avanceInterseccionMin <= 0.0f)) avanceInterseccionMin = avanceInterseccion[3];
		NBASSERT(avanceInterseccionMin>0.0f)
		if(avanceInterseccionMin<1.01f){
			posSalida				= vertice2;
		} else {
			posSalida.x				+= (vertice2.x  * (avanceInterseccionMin - 1.0f) * 0.05f);
			posSalida.y				+= (vertice2.y  * (avanceInterseccionMin - 1.0f) * 0.05f);
		}
		NBPUNTO_ESTABLECER(verticesGLDecae[0], vertice2.x + posEscenaLuz.x, vertice2.y + posEscenaLuz.y)
		NBCOLOR_ESTABLECER(verticesGLDecae[0], colorLuz8.r, colorLuz8.g, colorLuz8.b, 255) //colorLuz8.r, colorLuz8.g, colorLuz8.b
		NBPUNTO_ESTABLECER(verticesGLDecae[1], posSalida.x + posEscenaLuz.x, posSalida.y + posEscenaLuz.y)
		NBCOLOR_ESTABLECER(verticesGLDecae[1], 0, 0, 0, 255) //0
		verticesGLDecae				+= 2; NBASSERT(verticesGLDecae <= &NBGestorEscena::verticesGL1[bloqueDecaeGL.primerElemento + bloqueDecaeGL.cantidadElementos])
		//Siguiente
		vertice1					= vertice2;
		vertice2					= vertice3;
		vecUnitSeg1					= vecUnitSeg2;
	}
	NBASSERT(verticesGLDecae == &NBGestorEscena::verticesGL1[bloqueDecaeGL.primerElemento + bloqueDecaeGL.cantidadElementos])
	//Calcular Coordenadas de Texturas
	datosLuz->objetoTexturaLuz->cargarCoordenadasTextura(&NBGestorEscena::verticesGL1[bloqueDecaeGL.primerElemento], bloqueDecaeGL.cantidadElementos, texLuzPosiconCero, texLuzVecUnitCrecX, texLuzVecUnitCrecY, texLuzExtensionCreceX, texLuzExtensionCreceY);
	//Agregar la figura (un solo TRIANG_STRIP)
	STGestorEscenaFiguraRender datosFiguraDecae;
	datosFiguraDecae.bloqueIndicesGL	= NB_GESTOR_GL_DAME_ELEMS_PARA_TRIANGSTRIP_N_ESCNA(ENVerticeGlTipo_MonoTextura, bloqueDecaeGL, bloqueDecaeGL.cantidadElementos);
	datosFiguraDecae.esTriangFan		= false;
	datosFiguraDecae.idTextura0			= datosLuz->objetoTexturaLuz->idTexturaGL;
	datosFiguraDecae.idTextura1			= 0;
	renderCapa->renderMascLuzBloquesGL->agregarElemento(datosFiguraDecae);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

void NBGestorEscena::privIlumAgregarFiguraLuzDecaeNormalizadaLimitada(STGestorEscenaCapaRender* renderCapa, const NBFuenteIluminacionRender* datosLuz, const NBPunto* vertsContrarelojLuz, const NBPunto* verticeFig, const UI16 conteoVertsFig, const NBPunto &posVectLimite, const NBPunto &vecLimite, const NBPunto &texLuzPosiconCero, const NBPunto &texLuzVecUnitCrecX, const NBPunto &texLuzVecUnitCrecY, const float texLuzExtensionCreceX, const float texLuzExtensionCreceY){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::privIlumAgregarFiguraLuzDecaeNormalizadaLimitada")
	//Se suavizan las sombras de todos los vertices
	//
	//Se normalizan los vectores de cada dos segmentos consecutivos, se suman, y se obtiene el vector derecha del resultado.
	//Se intenta crear una figura desde cada vertice hacia su "vertice-alejado".
	/*NBASSERT(conteoVertsFig >= 3)
	float tamSalienteDecae			= 8.0f;
	NBPunto posEscenaLuz			= datosLuz->posicionEscenaLuz;
	NBColor8 colorLuz8;				NBCOLOR_ESTABLECER(colorLuz8, 255.0f*datosLuz->colorLuzF.r, 255.0f*datosLuz->colorLuzF.g, 255.0f*datosLuz->colorLuzF.b, 255)
	//Recorrer todos los vertices
	float avanceInterseccion[4], denominador[4];
	bool prodCruzAntCalculado		= false;
	float prodCruzAnt				= 0.0f;
	NBPunto posVertAnt;
	//
	NBPunto vecUnitSegms[2];
	NBPunto vertsSuavizar[3];
	SI16 usoBufferVertsSuavizar		= 0;
	//
	NBPunto posDecaeBase[2];
	NBPunto posDecaeSalido[2];
	SI16 usoBufferPosDecae			= 0;
	//
	//PRINTF_INFO("\nDecae\n");
	SI16 iVert;
	for(iVert=-3; iVert<conteoVertsFig; iVert++){
		NBASSERT(usoBufferVertsSuavizar<3)
		NBASSERT(usoBufferPosDecae<2)
		SI16 usoBufferVertsAnt		= usoBufferVertsSuavizar;
		NBPunto posVertAct			= verticeFig[(iVert<0?iVert+conteoVertsFig:iVert)];
		NBPunto vectorConLimite;	NBPUNTO_ESTABLECER(vectorConLimite, posVertAct.x-posVectLimite.x, posVertAct.y-posVectLimite.y)
		float prodCruzAct			= NBPUNTO_PRODUCTO_VECTORIAL(vecLimite.x, vecLimite.y, vectorConLimite.x, vectorConLimite.y); if(!prodCruzAntCalculado){ prodCruzAnt = prodCruzAct; prodCruzAntCalculado = true; }
		if(prodCruzAct<10.0f && prodCruzAnt<10.0f){
			vertsSuavizar[usoBufferVertsSuavizar++] = posVertAct;
			//PRINTF_INFO("Usando vertice (%d)\n", (iVert<0?iVert+conteoVertsFig:iVert));
		} else if((prodCruzAct<10.0f && prodCruzAnt >= 10.0f) || (prodCruzAct >= 10.0f && prodCruzAnt<10.0f)) {
			float avanceAntes		= (prodCruzAnt<0.0f?-prodCruzAnt:prodCruzAnt);
			float avanceDespues		= (prodCruzAct<0.0f?-prodCruzAct:prodCruzAct); NBASSERT(avanceAntes != 0.0f || avanceDespues != 0.0f)
			float avanceRel			= avanceAntes / (avanceAntes + avanceDespues);
			NBPunto posInterseccion;
			if(avanceRel == 0.0f)		posInterseccion = posVertAnt;
			else if(avanceRel == 1.0f) posInterseccion = posVertAct;
			else {
				NBPUNTO_ESTABLECER(posInterseccion, posVertAnt.x + ((posVertAct.x - posVertAnt.x) * avanceRel), posVertAnt.y + ((posVertAct.y - posVertAnt.y) * avanceRel));
			}
			//PRINTF_INFO("Usando intersecion finaliza en vertice (%d) rel(%.4f)\n", (iVert<0?iVert+conteoVertsFig:iVert), avanceRel);
			vertsSuavizar[usoBufferVertsSuavizar++] = posInterseccion;
		//} else {
		//	PRINTF_INFO("Ignorando vertice (%d)\n", (iVert<0?iVert+conteoVertsFig:iVert));
		}
		if(usoBufferVertsAnt == 1 && usoBufferVertsSuavizar == 2){
			NBPUNTO_ESTABLECER(vecUnitSegms[0], vertsSuavizar[1].x-vertsSuavizar[0].x, vertsSuavizar[1].y-vertsSuavizar[0].y) NBASSERT(vecUnitSegms[0].x != 0.0f || vecUnitSegms[0].y != 0.0f) //Si falla, produciria vector punto
			float longVector	= NBPUNTO_DISTANCIA_VECTOR(vecUnitSegms[0].x, vecUnitSegms[0].y);
			vecUnitSegms[0].x	/= longVector; vecUnitSegms[0].y /= longVector;
		}
		if(usoBufferVertsSuavizar == 3){
			NBPUNTO_ESTABLECER(vecUnitSegms[1], vertsSuavizar[2].x-vertsSuavizar[1].x, vertsSuavizar[2].y-vertsSuavizar[1].y) NBASSERT(vecUnitSegms[1].x != 0.0f || vecUnitSegms[1].y != 0.0f) //Si falla, produciria vector punto
			float longVector	= NBPUNTO_DISTANCIA_VECTOR(vecUnitSegms[1].x, vecUnitSegms[1].y);
			vecUnitSegms[1].x	/= longVector; vecUnitSegms[1].y /= longVector;
			//
			NBPunto vecUnitSum;	NBPUNTO_ESTABLECER(vecUnitSum, vecUnitSegms[0].x + vecUnitSegms[1].x, vecUnitSegms[0].y + vecUnitSegms[1].y);
			if(vecUnitSum.x != 0.0f || vecUnitSum.y != 0.0f){
				NBASSERT(vecUnitSum.x != 0.0f || vecUnitSum.y != 0.0f)
				longVector			= NBPUNTO_DISTANCIA_VECTOR(vecUnitSum.x, vecUnitSum.y);
				vecUnitSum.x		/= longVector; vecUnitSum.y /= longVector;
				//
				NBPunto posDecaeSalida; NBPUNTO_ESTABLECER(posDecaeSalida, vertsSuavizar[1].x + (vecUnitSum.y * tamSalienteDecae), vertsSuavizar[1].y - (vecUnitSum.x * tamSalienteDecae))
				NBLINEA_INSTERSECCION_RELATIVO_L1(avanceInterseccion[0], denominador[0], 0.0f, 0.0f, posDecaeSalida.x, posDecaeSalida.y, vertsContrarelojLuz[0].x, vertsContrarelojLuz[0].y, vertsContrarelojLuz[1].x, vertsContrarelojLuz[1].y)
				NBLINEA_INSTERSECCION_RELATIVO_L1(avanceInterseccion[1], denominador[1], 0.0f, 0.0f, posDecaeSalida.x, posDecaeSalida.y, vertsContrarelojLuz[1].x, vertsContrarelojLuz[1].y, vertsContrarelojLuz[2].x, vertsContrarelojLuz[2].y)
				NBLINEA_INSTERSECCION_RELATIVO_L1(avanceInterseccion[2], denominador[2], 0.0f, 0.0f, posDecaeSalida.x, posDecaeSalida.y, vertsContrarelojLuz[2].x, vertsContrarelojLuz[2].y, vertsContrarelojLuz[3].x, vertsContrarelojLuz[3].y)
				NBLINEA_INSTERSECCION_RELATIVO_L1(avanceInterseccion[3], denominador[3], 0.0f, 0.0f, posDecaeSalida.x, posDecaeSalida.y, vertsContrarelojLuz[3].x, vertsContrarelojLuz[3].y, vertsContrarelojLuz[0].x, vertsContrarelojLuz[0].y)
				float avanceInterseccionMin = (denominador[0] == 0.0f?0.0f:avanceInterseccion[0]);
				if(denominador[1] != 0.0f && avanceInterseccion[1]>0.0f && (avanceInterseccion[1]<avanceInterseccionMin || avanceInterseccionMin <= 0.0f)) avanceInterseccionMin = avanceInterseccion[1];
				if(denominador[2] != 0.0f && avanceInterseccion[2]>0.0f && (avanceInterseccion[2]<avanceInterseccionMin || avanceInterseccionMin <= 0.0f)) avanceInterseccionMin = avanceInterseccion[2];
				if(denominador[3] != 0.0f && avanceInterseccion[3]>0.0f && (avanceInterseccion[3]<avanceInterseccionMin || avanceInterseccionMin <= 0.0f)) avanceInterseccionMin = avanceInterseccion[3];
				NBASSERT(avanceInterseccionMin>0.0f)
				if(avanceInterseccionMin<1.01f){
					posDecaeSalida		= vertsSuavizar[1];
				} else {
					posDecaeSalida.x	+= (vertsSuavizar[1].x  * (avanceInterseccionMin - 1.0f) * 0.15f);
					posDecaeSalida.y	+= (vertsSuavizar[1].y  * (avanceInterseccionMin - 1.0f) * 0.15f);
				}
				posDecaeBase[usoBufferPosDecae]		= vertsSuavizar[1];
				posDecaeSalido[usoBufferPosDecae]	= posDecaeSalida;
				usoBufferPosDecae++;
			}
			//Siguiente
			vecUnitSegms[0]		= vecUnitSegms[1];
			vertsSuavizar[0]	= vertsSuavizar[1];
			vertsSuavizar[1]	= vertsSuavizar[2];
			usoBufferVertsSuavizar--; NBASSERT(usoBufferVertsSuavizar == 2)
		}
		if(usoBufferPosDecae == 2){
			//Agregar figura
			STBloqueGL bloqueDecaeGL;
			bloqueDecaeGL.cantidadElementos	= 4;
			bloqueDecaeGL.primerElemento	= NBGestorEscena::reservarVerticesGL(ENVerticeGlTipo_MonoTextura, bloqueDecaeGL.cantidadElementos);
			NBVerticeGL* verticesGLDecae	= &NBGestorEscena::verticesGL1[bloqueDecaeGL.primerElemento];
			NBPUNTO_ESTABLECER(verticesGLDecae[0], posDecaeBase[0].x + posEscenaLuz.x, posDecaeBase[0].y + posEscenaLuz.y)
			NBCOLOR_ESTABLECER(verticesGLDecae[0], colorLuz8.r, colorLuz8.g, colorLuz8.b, 255) //colorLuz8.r, colorLuz8.g, colorLuz8.b
			NBPUNTO_ESTABLECER(verticesGLDecae[1], posDecaeSalido[0].x + posEscenaLuz.x, posDecaeSalido[0].y + posEscenaLuz.y)
			NBCOLOR_ESTABLECER(verticesGLDecae[1], 0, 0, 0, 255) //0
			NBPUNTO_ESTABLECER(verticesGLDecae[2], posDecaeSalido[1].x + posEscenaLuz.x, posDecaeSalido[1].y + posEscenaLuz.y)
			NBCOLOR_ESTABLECER(verticesGLDecae[2], 0, 0, 0, 255) //0
			NBPUNTO_ESTABLECER(verticesGLDecae[3], posDecaeBase[1].x + posEscenaLuz.x, posDecaeBase[1].y + posEscenaLuz.y)
			NBCOLOR_ESTABLECER(verticesGLDecae[3], colorLuz8.r, colorLuz8.g, colorLuz8.b, 255) //colorLuz8.r, colorLuz8.g, colorLuz8.b
			datosLuz->objetoTexturaLuz->cargarCoordenadasTextura(verticesGLDecae, bloqueDecaeGL.cantidadElementos, texLuzPosiconCero, texLuzVecUnitCrecX, texLuzVecUnitCrecY, texLuzExtensionCreceX, texLuzExtensionCreceY);
			STGestorEscenaFiguraRender datosFiguraDecae;
			datosFiguraDecae.bloqueVerticesGL	= bloqueDecaeGL;
			datosFiguraDecae.idTextura0			= datosLuz->objetoTexturaLuz->idTexturaGL;
			datosFiguraDecae.modoTextura0		= datosLuz->objetoTexturaLuz->modoPintado;
			datosFiguraDecae.esTriangFan		= true or false;
			datosFiguraDecae.idTextura1			= 0;
			datosFiguraDecae.modoTextura1		= ENTexturaModoPintado_Imagen_Suavizada;
			renderCapa->renderMascLuzBloquesGL->agregarElemento(datosFiguraDecae);
			//Siguiente
			posDecaeBase[0]		= posDecaeBase[1];
			posDecaeSalido[0]	= posDecaeSalido[1];
			usoBufferPosDecae--; NBASSERT(usoBufferPosDecae == 1)
		}
		prodCruzAnt					= prodCruzAct;
		posVertAnt					= posVertAct;
	}*/
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

/*void NBGestorEscena::privIlumAgregarFiguraLuzDecaeSimpleNoLimitada(STGestorEscenaCapaRender* renderCapa, const NBFuenteIluminacionRender* datosLuz, const NBPunto* vertsContrarelojLuz, const NBPunto* verticeFig, const UI16 conteoVertsFig, const NBPunto &texLuzPosiconCero, const NBPunto &texLuzVecUnitCrecX, const NBPunto &texLuzVecUnitCrecY, const float texLuzExtensionCreceX, const float texLuzExtensionCreceY){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::privIlumAgregarFiguraLuzDecaeSimpleNoLimitada")
	// Se intentan suavizar las sombras de todos los vertices.
	//
	// Esta implementacion de suavizado de sombras es simple,
	// requiere 1 calculo de distancia al cuadrado y 4 calculos de interseccion por cada vertice de la figura.
	// Pro: no hace uso de las funciones seno, coseno ni raiz-cuadrada.
	// Contra: no proyecta sombras a segmentos colineales a los rayos que se proyectan desde la luz (por ejemplo: los limites de angMin y angMax de la luz).
	NBPunto posEscenaLuz				= datosLuz->posicionEscenaLuz;
	NBColor8 colorLuz8;					NBCOLOR_ESTABLECER(colorLuz8, 255.0f*datosLuz->colorLuzF.r, 255.0f*datosLuz->colorLuzF.g, 255.0f*datosLuz->colorLuzF.b, 255)
	NBPunto posVertAnterior				= verticeFig[conteoVertsFig-1];
	float factorAvanceRestanteAnt		= 0.0f;
	float avanceInterseccion[4];
	float denominador[4];
	if(NBPUNTO_DISTANCIA_AL_CUADRADO_VECTOR(posVertAnterior.x, posVertAnterior.y)>256.0f){
		NBLINEA_INSTERSECCION_RELATIVO_L1(avanceInterseccion[0], denominador[0], 0.0f, 0.0f, posVertAnterior.x, posVertAnterior.y, vertsContrarelojLuz[0].x, vertsContrarelojLuz[0].y, vertsContrarelojLuz[1].x, vertsContrarelojLuz[1].y)
		NBLINEA_INSTERSECCION_RELATIVO_L1(avanceInterseccion[1], denominador[1], 0.0f, 0.0f, posVertAnterior.x, posVertAnterior.y, vertsContrarelojLuz[1].x, vertsContrarelojLuz[1].y, vertsContrarelojLuz[2].x, vertsContrarelojLuz[2].y)
		NBLINEA_INSTERSECCION_RELATIVO_L1(avanceInterseccion[2], denominador[2], 0.0f, 0.0f, posVertAnterior.x, posVertAnterior.y, vertsContrarelojLuz[2].x, vertsContrarelojLuz[2].y, vertsContrarelojLuz[3].x, vertsContrarelojLuz[3].y)
		NBLINEA_INSTERSECCION_RELATIVO_L1(avanceInterseccion[3], denominador[3], 0.0f, 0.0f, posVertAnterior.x, posVertAnterior.y, vertsContrarelojLuz[3].x, vertsContrarelojLuz[3].y, vertsContrarelojLuz[0].x, vertsContrarelojLuz[0].y)
		factorAvanceRestanteAnt			= (denominador[0] == 0.0f?0.0f:avanceInterseccion[0]);
		if(denominador[1] != 0.0f && avanceInterseccion[1]>0.0f && (avanceInterseccion[1]<factorAvanceRestanteAnt || factorAvanceRestanteAnt <= 0.0f)) factorAvanceRestanteAnt = avanceInterseccion[1];
		if(denominador[2] != 0.0f && avanceInterseccion[2]>0.0f && (avanceInterseccion[2]<factorAvanceRestanteAnt || factorAvanceRestanteAnt <= 0.0f)) factorAvanceRestanteAnt = avanceInterseccion[2];
		if(denominador[3] != 0.0f && avanceInterseccion[3]>0.0f && (avanceInterseccion[3]<factorAvanceRestanteAnt || factorAvanceRestanteAnt <= 0.0f)) factorAvanceRestanteAnt = avanceInterseccion[3];
		NBASSERT(factorAvanceRestanteAnt>0.999f)
	}
	UI16 iVertFig;
	for(iVertFig=0; iVertFig<conteoVertsFig; iVertFig++){
		NBPunto posVertActual			= verticeFig[iVertFig];
		float factorAvanceRestanteAct	= 0.0f;
		if(NBPUNTO_DISTANCIA_AL_CUADRADO_VECTOR(posVertActual.x, posVertActual.y)>256.0f){
			NBLINEA_INSTERSECCION_RELATIVO_L1(avanceInterseccion[0], denominador[0], 0.0f, 0.0f, posVertActual.x, posVertActual.y, vertsContrarelojLuz[0].x, vertsContrarelojLuz[0].y, vertsContrarelojLuz[1].x, vertsContrarelojLuz[1].y)
			NBLINEA_INSTERSECCION_RELATIVO_L1(avanceInterseccion[1], denominador[1], 0.0f, 0.0f, posVertActual.x, posVertActual.y, vertsContrarelojLuz[1].x, vertsContrarelojLuz[1].y, vertsContrarelojLuz[2].x, vertsContrarelojLuz[2].y)
			NBLINEA_INSTERSECCION_RELATIVO_L1(avanceInterseccion[2], denominador[2], 0.0f, 0.0f, posVertActual.x, posVertActual.y, vertsContrarelojLuz[2].x, vertsContrarelojLuz[2].y, vertsContrarelojLuz[3].x, vertsContrarelojLuz[3].y)
			NBLINEA_INSTERSECCION_RELATIVO_L1(avanceInterseccion[3], denominador[3], 0.0f, 0.0f, posVertActual.x, posVertActual.y, vertsContrarelojLuz[3].x, vertsContrarelojLuz[3].y, vertsContrarelojLuz[0].x, vertsContrarelojLuz[0].y)
			factorAvanceRestanteAct		= (denominador[0] == 0.0f?0.0f:avanceInterseccion[0]);
			if(denominador[1] != 0.0f && avanceInterseccion[1]>0.0f && (avanceInterseccion[1]<factorAvanceRestanteAct || factorAvanceRestanteAct <= 0.0f)) factorAvanceRestanteAct = avanceInterseccion[1];
			if(denominador[2] != 0.0f && avanceInterseccion[2]>0.0f && (avanceInterseccion[2]<factorAvanceRestanteAct || factorAvanceRestanteAct <= 0.0f)) factorAvanceRestanteAct = avanceInterseccion[2];
			if(denominador[3] != 0.0f && avanceInterseccion[3]>0.0f && (avanceInterseccion[3]<factorAvanceRestanteAct || factorAvanceRestanteAct <= 0.0f)) factorAvanceRestanteAct = avanceInterseccion[3];
			NBASSERT(factorAvanceRestanteAct>0.999f)
		}
		if(factorAvanceRestanteAnt>1.01f && factorAvanceRestanteAct>1.01f / *&& factorAvanceRestanteAnt<1000.0f* / / *&& factorAvanceRestanteAct<1000.0f* /){
			STBloqueGL bloqueDecaeGL;
			bloqueDecaeGL.cantidadElementos	= 4;
			bloqueDecaeGL.primerElemento	= NBGestorEscena::reservarVerticesGL(ENVerticeGlTipo_MonoTextura, bloqueDecaeGL.cantidadElementos);
			NBVerticeGL* verticesGLDecae	= &NBGestorEscena::verticesGL1[bloqueDecaeGL.primerElemento];
			NBPUNTO_ESTABLECER(verticesGLDecae[0], posEscenaLuz.x + posVertAnterior.x, posEscenaLuz.y + posVertAnterior.y)
			NBCOLOR_ESTABLECER(verticesGLDecae[0], colorLuz8.r, colorLuz8.g, colorLuz8.b, 255) //colorLuz8.r, colorLuz8.g, colorLuz8.b
			NBPUNTO_ESTABLECER(verticesGLDecae[1], posEscenaLuz.x + posVertAnterior.x + (posVertAnterior.x * (factorAvanceRestanteAnt - 1.0f) * 0.15f), posEscenaLuz.y + posVertAnterior.y + (posVertAnterior.y * (factorAvanceRestanteAnt - 1.0f) * 0.15f))
			NBCOLOR_ESTABLECER(verticesGLDecae[1], 0, 0, 0, 255) //0
			NBPUNTO_ESTABLECER(verticesGLDecae[2], posEscenaLuz.x + posVertActual.x + (posVertActual.x * (factorAvanceRestanteAct - 1.0f) * 0.15f), posEscenaLuz.y + posVertActual.y + (posVertActual.y * (factorAvanceRestanteAct - 1.0f) * 0.15f))
			NBCOLOR_ESTABLECER(verticesGLDecae[2], 0, 0, 0, 255) //0
			NBPUNTO_ESTABLECER(verticesGLDecae[3], posEscenaLuz.x + posVertActual.x, posEscenaLuz.y + posVertActual.y)
			NBCOLOR_ESTABLECER(verticesGLDecae[3], colorLuz8.r, colorLuz8.g, colorLuz8.b, 255) //colorLuz8.r, colorLuz8.g, colorLuz8.b
			datosLuz->objetoTexturaLuz->cargarCoordenadasTextura(verticesGLDecae, bloqueDecaeGL.cantidadElementos, texLuzPosiconCero, texLuzVecUnitCrecX, texLuzVecUnitCrecY, texLuzExtensionCreceX, texLuzExtensionCreceY);
			STGestorEscenaFiguraRender datosFiguraDecae;
			datosFiguraDecae.bloqueVerticesGL	= bloqueDecaeGL;
			datosFiguraDecae.idTextura0			= datosLuz->objetoTexturaLuz->idTexturaGL;
			datosFiguraDecae.modoTextura0		= datosLuz->objetoTexturaLuz->modoPintado;
			datosFiguraDecae.idTextura1			= 0;
			datosFiguraDecae.esTriangFan		= true or false;
			datosFiguraDecae.modoTextura1		= ENTexturaModoPintado_Imagen_Suavizada;
			renderCapa->renderMascLuzBloquesGL->agregarElemento(datosFiguraDecae);
		}
		//Siguiente
		posVertAnterior					= posVertActual;
		factorAvanceRestanteAnt			= factorAvanceRestanteAct;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}*/

/*void NBGestorEscena::privIlumAgregarFiguraLuzDecaeSimpleLimitada(STGestorEscenaCapaRender* renderCapa, const NBFuenteIluminacionRender* datosLuz, const NBPunto* vertsContrarelojLuz, const NBPunto* verticeFig, const UI16 conteoVertsFig, const NBPunto &posVectLimite, const NBPunto &vecLimite, const NBPunto &texLuzPosiconCero, const NBPunto &texLuzVecUnitCrecX, const NBPunto &texLuzVecUnitCrecY, const float texLuzExtensionCreceX, const float texLuzExtensionCreceY){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::privIlumAgregarFiguraLuzDecaeSimpleLimitada")
	// Solo se intentan suavizar las sombras de los vertices que esten a la izquierda del vector limite.
	// En condiciones ideales, solamente evita que se suavice el vector limite de la luz.
	//
	// Esta implementacion de suavizado de sombras es simple,
	// requiere 1 calculo de distancia al cuadrado y 4 calculos de interseccion por cada vertice de la figura.
	// Pro: no hace uso de las funciones seno, coseno ni raiz-cuadrada.
	// Contra: no proyecta sombras a segmentos colineales a los rayos que se proyectan desde la luz (por ejemplo: los limites de angMin y angMax de la luz).
	NBPunto posEscenaLuz				= datosLuz->posicionEscenaLuz;
	NBColor8 colorLuz8;					NBCOLOR_ESTABLECER(colorLuz8, 255.0f*datosLuz->colorLuzF.r, 255.0f*datosLuz->colorLuzF.g, 255.0f*datosLuz->colorLuzF.b, 255)
	NBPunto posVertAnterior				= verticeFig[conteoVertsFig-1];
	float factorAvanceRestanteAnt		= 0.0f;
	float prodCruzAnt					= 0.0f;
	float avanceInterseccion[4];
	float denominador[4];
	if(NBPUNTO_DISTANCIA_AL_CUADRADO_VECTOR(posVertAnterior.x, posVertAnterior.y)>256.0f){
		NBPunto vectorConLimite;		NBPUNTO_ESTABLECER(vectorConLimite, posVertAnterior.x-posVectLimite.x, posVertAnterior.y-posVectLimite.y)
		prodCruzAnt						= NBPUNTO_PRODUCTO_VECTORIAL(vecLimite.x, vecLimite.y, vectorConLimite.x, vectorConLimite.y);
		if(prodCruzAnt<10.0f){ // 10.0f, margen de error a (menor a cero)
			NBLINEA_INSTERSECCION_RELATIVO_L1(avanceInterseccion[0], denominador[0], 0.0f, 0.0f, posVertAnterior.x, posVertAnterior.y, vertsContrarelojLuz[0].x, vertsContrarelojLuz[0].y, vertsContrarelojLuz[1].x, vertsContrarelojLuz[1].y)
			NBLINEA_INSTERSECCION_RELATIVO_L1(avanceInterseccion[1], denominador[1], 0.0f, 0.0f, posVertAnterior.x, posVertAnterior.y, vertsContrarelojLuz[1].x, vertsContrarelojLuz[1].y, vertsContrarelojLuz[2].x, vertsContrarelojLuz[2].y)
			NBLINEA_INSTERSECCION_RELATIVO_L1(avanceInterseccion[2], denominador[2], 0.0f, 0.0f, posVertAnterior.x, posVertAnterior.y, vertsContrarelojLuz[2].x, vertsContrarelojLuz[2].y, vertsContrarelojLuz[3].x, vertsContrarelojLuz[3].y)
			NBLINEA_INSTERSECCION_RELATIVO_L1(avanceInterseccion[3], denominador[3], 0.0f, 0.0f, posVertAnterior.x, posVertAnterior.y, vertsContrarelojLuz[3].x, vertsContrarelojLuz[3].y, vertsContrarelojLuz[0].x, vertsContrarelojLuz[0].y)
			factorAvanceRestanteAnt			= (denominador[0] == 0.0f?0.0f:avanceInterseccion[0]);
			if(denominador[1] != 0.0f && avanceInterseccion[1]>0.0f && (avanceInterseccion[1]<factorAvanceRestanteAnt || factorAvanceRestanteAnt <= 0.0f)) factorAvanceRestanteAnt = avanceInterseccion[1];
			if(denominador[2] != 0.0f && avanceInterseccion[2]>0.0f && (avanceInterseccion[2]<factorAvanceRestanteAnt || factorAvanceRestanteAnt <= 0.0f)) factorAvanceRestanteAnt = avanceInterseccion[2];
			if(denominador[3] != 0.0f && avanceInterseccion[3]>0.0f && (avanceInterseccion[3]<factorAvanceRestanteAnt || factorAvanceRestanteAnt <= 0.0f)) factorAvanceRestanteAnt = avanceInterseccion[3];
			NBASSERT(factorAvanceRestanteAnt>0.999f)
		}
	}
	UI16 iVertFig;
	for(iVertFig=0; iVertFig<conteoVertsFig; iVertFig++){
		NBPunto posVertActual			= verticeFig[iVertFig];
		float factorAvanceRestanteAct	= 0.0f;
		float prodCruzActual			= 0.0f;
		if(NBPUNTO_DISTANCIA_AL_CUADRADO_VECTOR(posVertActual.x, posVertActual.y)>256.0f){
			NBPunto vectorConLimite;	NBPUNTO_ESTABLECER(vectorConLimite, posVertActual.x-posVectLimite.x, posVertActual.y-posVectLimite.y)
			prodCruzActual				= NBPUNTO_PRODUCTO_VECTORIAL(vecLimite.x, vecLimite.y, vectorConLimite.x, vectorConLimite.y);
			if(prodCruzActual<10.0f){ // 10.0f, margen de error a (menor a cero)
				NBLINEA_INSTERSECCION_RELATIVO_L1(avanceInterseccion[0], denominador[0], 0.0f, 0.0f, posVertActual.x, posVertActual.y, vertsContrarelojLuz[0].x, vertsContrarelojLuz[0].y, vertsContrarelojLuz[1].x, vertsContrarelojLuz[1].y)
				NBLINEA_INSTERSECCION_RELATIVO_L1(avanceInterseccion[1], denominador[1], 0.0f, 0.0f, posVertActual.x, posVertActual.y, vertsContrarelojLuz[1].x, vertsContrarelojLuz[1].y, vertsContrarelojLuz[2].x, vertsContrarelojLuz[2].y)
				NBLINEA_INSTERSECCION_RELATIVO_L1(avanceInterseccion[2], denominador[2], 0.0f, 0.0f, posVertActual.x, posVertActual.y, vertsContrarelojLuz[2].x, vertsContrarelojLuz[2].y, vertsContrarelojLuz[3].x, vertsContrarelojLuz[3].y)
				NBLINEA_INSTERSECCION_RELATIVO_L1(avanceInterseccion[3], denominador[3], 0.0f, 0.0f, posVertActual.x, posVertActual.y, vertsContrarelojLuz[3].x, vertsContrarelojLuz[3].y, vertsContrarelojLuz[0].x, vertsContrarelojLuz[0].y)
				factorAvanceRestanteAct		= (denominador[0] == 0.0f?0.0f:avanceInterseccion[0]);
				if(denominador[1] != 0.0f && avanceInterseccion[1]>0.0f && (avanceInterseccion[1]<factorAvanceRestanteAct || factorAvanceRestanteAct <= 0.0f)) factorAvanceRestanteAct = avanceInterseccion[1];
				if(denominador[2] != 0.0f && avanceInterseccion[2]>0.0f && (avanceInterseccion[2]<factorAvanceRestanteAct || factorAvanceRestanteAct <= 0.0f)) factorAvanceRestanteAct = avanceInterseccion[2];
				if(denominador[3] != 0.0f && avanceInterseccion[3]>0.0f && (avanceInterseccion[3]<factorAvanceRestanteAct || factorAvanceRestanteAct <= 0.0f)) factorAvanceRestanteAct = avanceInterseccion[3];
				NBASSERT(factorAvanceRestanteAct>0.999f)
			}
		}
		//PRINTF_INFO("#%d) AvanceAnt(%.2f) AvanceAct(%.2f)\n", iVertFig, factorAvanceRestanteAnt, factorAvanceRestanteAct);
		if(factorAvanceRestanteAnt>1.0f && factorAvanceRestanteAct>1.0f && prodCruzAnt<10.0f && prodCruzActual<10.0f / *&& factorAvanceRestanteAnt<1000.0f* / / *&& factorAvanceRestanteAct<1000.0f* /){
			STBloqueGL bloqueDecaeGL;
			bloqueDecaeGL.cantidadElementos	= 4;
			bloqueDecaeGL.primerElemento	= NBGestorEscena::reservarVerticesGL(ENVerticeGlTipo_MonoTextura, bloqueDecaeGL.cantidadElementos);
			NBVerticeGL* verticesGLDecae	= &NBGestorEscena::verticesGL1[bloqueDecaeGL.primerElemento];
			NBPUNTO_ESTABLECER(verticesGLDecae[0], posEscenaLuz.x + posVertAnterior.x, posEscenaLuz.y + posVertAnterior.y)
			NBCOLOR_ESTABLECER(verticesGLDecae[0], colorLuz8.r, colorLuz8.g, colorLuz8.b, 255) //colorLuz8.r, colorLuz8.g, colorLuz8.b
			NBPUNTO_ESTABLECER(verticesGLDecae[1], posEscenaLuz.x + posVertAnterior.x + (posVertAnterior.x * (factorAvanceRestanteAnt - 1.0f) * 0.15f), posEscenaLuz.y + posVertAnterior.y + (posVertAnterior.y * (factorAvanceRestanteAnt - 1.0f) * 0.15f))
			NBCOLOR_ESTABLECER(verticesGLDecae[1], 0, 0, 0, 255) //0
			NBPUNTO_ESTABLECER(verticesGLDecae[2], posEscenaLuz.x + posVertActual.x + (posVertActual.x * (factorAvanceRestanteAct - 1.0f) * 0.15f), posEscenaLuz.y + posVertActual.y + (posVertActual.y * (factorAvanceRestanteAct - 1.0f) * 0.15f))
			NBCOLOR_ESTABLECER(verticesGLDecae[2], 0, 0, 0, 255) //0
			NBPUNTO_ESTABLECER(verticesGLDecae[3], posEscenaLuz.x + posVertActual.x, posEscenaLuz.y + posVertActual.y)
			NBCOLOR_ESTABLECER(verticesGLDecae[3], colorLuz8.r, colorLuz8.g, colorLuz8.b, 255) //colorLuz8.r, colorLuz8.g, colorLuz8.b
			datosLuz->objetoTexturaLuz->cargarCoordenadasTextura(verticesGLDecae, bloqueDecaeGL.cantidadElementos, texLuzPosiconCero, texLuzVecUnitCrecX, texLuzVecUnitCrecY, texLuzExtensionCreceX, texLuzExtensionCreceY);
			STGestorEscenaFiguraRender datosFiguraDecae;
			datosFiguraDecae.bloqueVerticesGL	= bloqueDecaeGL;
			datosFiguraDecae.idTextura0			= datosLuz->objetoTexturaLuz->idTexturaGL;
			datosFiguraDecae.modoTextura0		= datosLuz->objetoTexturaLuz->modoPintado;
			datosFiguraDecae.idTextura1			= 0;
			datosFiguraDecae.esTriangFan		= true  or false;
			datosFiguraDecae.modoTextura1		= ENTexturaModoPintado_Imagen_Suavizada;
			renderCapa->renderMascLuzBloquesGL->agregarElemento(datosFiguraDecae);
		}
		//Siguiente
		posVertAnterior					= posVertActual;
		factorAvanceRestanteAnt			= factorAvanceRestanteAct;
		prodCruzAnt						= prodCruzActual;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}*/

//
// ENVIAR COMANDOS DE MASCARAS DE ILUMINACION
//

void NBGestorEscena::privEnviarComandosDibujarMascarasIluminacion(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::privEnviarComandosDibujarMascarasIluminacion")
	NBASSERT(_gestorInicializado);
	NBASSERT(_bufferDatosBloqueado[_indiceBufferDatosLeer]);	//Debe estar bloqueado
	NBASSERT(_bufferDatosEstado[_indiceBufferDatosLeer] != ENGestorEscenaBufferEstado_Vacio);		//Debe estar lleno
	// -----------------------------------------
	// Generar las mascaras de luces combinadas
	// y escena-en-textura
	// -----------------------------------------
	SI16 iEscena;
	for(iEscena = (NBGESTORESCENA_MAX_ESCENAS - 1); iEscena >= 0; iEscena--){
		STGestorEscenaEscena* scn = &(_escenas[iEscena]);
		if(scn->registroOcupado && scn->escenaHabilitada){
			NBGestorEscena::privTexturaRenderAsegurarDestinosCreados(iEscena);
			NBASSERT(scn->renderCapasConsumir[_indiceBufferDatosLeer] != NULL)
			NBASSERT(scn->renderCapasProducir[_indiceBufferDatosLeer] != NULL)
			NBASSERT(scn->renderCapas[_indiceBufferDatosLeer] != NULL)
			STGestorEscenaCapaRender** arrRenderCapa	= scn->renderCapasConsumir[_indiceBufferDatosLeer]->elemento;
			UI16 iRenderCapa, conteoRenderCapas			= scn->renderCapasConsumir[_indiceBufferDatosLeer]->conteo;
			for(iRenderCapa=0; iRenderCapa<conteoRenderCapas; iRenderCapa++){
				STGestorEscenaCapaRender* renderCapa	= arrRenderCapa[iRenderCapa];
				if(renderCapa->renderMascLuz->conteo != 0 && renderCapa->renderConteoLucesAfectanMascara != 0 && renderCapa->renderMascLuzSumaFigurasTotal != 0){
					NBASSERT(renderCapa->renderMascLucesCombinadas.iTexRender<NBGESTORESCENA_MAX_TEXTURAS_RENDER)
					NBASSERT(_texturasParaRenderizadoGL[renderCapa->renderMascLucesCombinadas.iTexRender].objTexturaAtlas != NULL) //Si falla, aun no se ha "privTexturaRenderAsegurarDestinosCreados"
					STGestorEscenaTextura* datosTexLuces		= &_texturasParaRenderizadoGL[renderCapa->renderMascLucesCombinadas.iTexRender];
					NBASSERT(datosTexLuces->iEscenaPropietaria == iEscena)
					NBASSERT(datosTexLuces->texturaOcupada)
					//Renderizar mascara de luces combinadas
					if(datosTexLuces->objTexturaAtlas != NULL){
						NBASSERT(datosTexLuces->idFrameBufferGlPropio != 0)
						NBGestorGL::bindFramebufferEXT(GL_FRAMEBUFFER_EXT, datosTexLuces->idFrameBufferGlPropio);
						NBGestorGL::clearColor(renderCapa->renderColorLuzAmbiente.r, renderCapa->renderColorLuzAmbiente.g, renderCapa->renderColorLuzAmbiente.b, 1.0f);
						//NBGestorGL::clearColor(0.25f, 0.25f, 0.25f, 1.0f);
						NBGestorGL::clear(GL_COLOR_BUFFER_BIT);
						NBGestorGL::matrixMode(GL_PROJECTION);
						NBGestorGL::loadIdentity();
						NBGestorGL::viewport(renderCapa->renderMascLucesCombinadas.areaReservada.x, renderCapa->renderMascLucesCombinadas.areaReservada.y, renderCapa->renderMascLucesCombinadas.areaReservada.ancho, renderCapa->renderMascLucesCombinadas.areaReservada.alto);
						NBGestorGL::orthof(renderCapa->renderCajaProyeccion.xMin, renderCapa->renderCajaProyeccion.xMax, renderCapa->renderCajaProyeccion.yMin, renderCapa->renderCajaProyeccion.yMax);
						NBGestorGL::matrixMode(GL_MODELVIEW);
						NBGestorGL::loadMatrixf(renderCapa->renderMatrizCapa);
						//
						NBGestorGL::activarVerticesGL(ENVerticeGlTipo_MonoTextura); NBGESTORGL_DBG_NOMBRAR_ACTIVADOR_VERTICES_GL("NBGestorEscena::privEnviarComandosDibujarMascarasIluminacion")
						NBGestorGL::blendFunc(GL_ONE, GL_ONE);
						//NBGestorGL::color4f(1.0f, 1.0f, 1.0f, 1.0f);
						NBGestorGL::lightModelAmbient(1.0f, 1.0f, 1.0f, 1.0f); //Requerido cuando HAY colores definidos en los vertices
						UI16 iMascInd, conteoMascIndv = renderCapa->renderMascLuz->conteo;
						for(iMascInd=0; iMascInd<conteoMascIndv; iMascInd++){
							STGestorEscenaLuzRender* datosMascLuzIndv = &renderCapa->renderMascLuz->elemento[iMascInd];
							UI16 iFigLuz, iFigLuzSigUlt = datosMascLuzIndv->primerFiguraLuz + datosMascLuzIndv->conteoFigurasLuz;
							for(iFigLuz=datosMascLuzIndv->primerFiguraLuz; iFigLuz<iFigLuzSigUlt; iFigLuz++){
								STGestorEscenaFiguraRender* datosFigLuz = &renderCapa->renderMascLuzBloquesGL->elemento[iFigLuz];
								NBGestorGL::bindTexture(0, datosFigLuz->idTextura0); NBASSERT(datosFigLuz->idTextura0 != 0)
								if(datosFigLuz->esTriangFan){
									NB_GESTOR_GL_RENDER_TRIANGFAN(datosFigLuz->bloqueIndicesGL.primerElemento, datosFigLuz->bloqueIndicesGL.cantidadElementos);
								} else {
									NB_GESTOR_GL_RENDER_TRIANGSTRIP_N(datosFigLuz->bloqueIndicesGL.primerElemento, datosFigLuz->bloqueIndicesGL.cantidadElementos);
								}
							}
							UI16 iFigDecae, iFigDecaeSigUlt = datosMascLuzIndv->primerFiguraDecae + datosMascLuzIndv->conteoFigurasDecae;
							for(iFigDecae=datosMascLuzIndv->primerFiguraDecae; iFigDecae<iFigDecaeSigUlt; iFigDecae++){
								STGestorEscenaFiguraRender* datosFigDecae = &renderCapa->renderMascLuzBloquesGL->elemento[iFigDecae];
								NBGestorGL::bindTexture(0, datosFigDecae->idTextura0); NBASSERT(datosFigDecae->idTextura0 != 0)
								if(datosFigDecae->esTriangFan){
									NB_GESTOR_GL_RENDER_TRIANGFAN(datosFigDecae->bloqueIndicesGL.primerElemento, datosFigDecae->bloqueIndicesGL.cantidadElementos);
								} else {
									NB_GESTOR_GL_RENDER_TRIANGSTRIP_N(datosFigDecae->bloqueIndicesGL.primerElemento, datosFigDecae->bloqueIndicesGL.cantidadElementos);
								}
							}
						}
						NBGestorGL::blendFuncCargarValoresPorDefecto();
					}
				}
				#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
				{
					//Validar el valor de la cache "renderMascLuzSumaFigurasTotal"
					UI16 dbgSumaFigurasTotal = 0;
					UI16 iMascInd, conteoMascIndv = renderCapa->renderMascLuz->conteo;
					for(iMascInd=0; iMascInd<conteoMascIndv; iMascInd++){
						STGestorEscenaLuzRender* datosMascLuzIndv = &renderCapa->renderMascLuz->elemento[iMascInd];
						dbgSumaFigurasTotal += datosMascLuzIndv->conteoFigurasLuz + datosMascLuzIndv->conteoFigurasDecae;
					}
					NBASSERT(dbgSumaFigurasTotal == renderCapa->renderMascLuzSumaFigurasTotal)
				}
				#endif
			}
		}
	}
	//NBGestorGL::flushCacheLocal(); //Esto provoca el envio de los lotes acumulados locales
	NBGestorGL::flush(); //Esto provoca el envio de los comando pendiente del cliente al servidor. (pero no espera a que terminen)
	//NBGestorGL::finish(); //PENDIENTE: comentariar //Esto provoca una espera hasta que todos los comandos GL han sido finalizados.
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

//
//
//

void NBGestorEscena::privProducirSombrasEnCapa(STGestorEscenaCapaRender* renderCapa, const UI16 iPrimeraLuz){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::privProducirSombrasEnCapa")
	NBASSERT(renderCapa->renderIluminacion->conteo <= renderCapa->renderSombrasPorIluminacion->conteo)
	NBASSERT(renderCapa->renderIluminacion->conteo <= renderCapa->renderSombrasRangosPorIluminacion->conteo)
	NBPropIluminacion propsIluminacion;
	propsIluminacion.conteoFuentesIluminacion					= renderCapa->renderIluminacion->conteo - iPrimeraLuz;
	propsIluminacion.fuentesIluminacion							= &renderCapa->renderIluminacion->elemento[iPrimeraLuz];
	propsIluminacion.sombrasPorFuenteIluminacion				= (AUArregloNativoMutableP<NBFuenteSombra>**)&renderCapa->renderSombrasPorIluminacion->elemento[iPrimeraLuz]; NBASSERT(renderCapa->renderIluminacion->conteo <= renderCapa->renderSombrasPorIluminacion->conteo)
	propsIluminacion.sombrasVerticesIntermedios					= renderCapa->renderSombrasVerticesIntermedios; //La secuencia de vertices entre el anguloMax y el anguloMin (cuando la sombra es una linea no deposita ningun vertice ene ste arreglo)
	// Producir las sombras de cada objeto
	AUArregloNativoP<IEscenaProductorSombras*>* productores		= renderCapa->renderProductoresSombras;
	UI16 iProd, conteoProd = productores->conteo;
	for(iProd=0; iProd<conteoProd; iProd++){
		productores->elemento[iProd]->producirSombras2(propsIluminacion);
	}
	// Producir las sombras de cada espejo
	UI16 iEspejo, conteoEspejos	= renderCapa->renderEspejos->conteo;
	UI16 iIlum, conteoIlum		= propsIluminacion.conteoFuentesIluminacion;
	for(iEspejo = 0; iEspejo < conteoEspejos; iEspejo++){
		NBFuenteEspejoRender* datosEspejo = &renderCapa->renderEspejos->elemento[iEspejo];
		//Definir la caja de sombra
		NBCajaAABB cajaEscenaSombra;
		NBCAJAAABB_INICIALIZAR_CON_PUNTO(cajaEscenaSombra, datosEspejo->limitesEscenaEspejo[0].x, datosEspejo->limitesEscenaEspejo[0].y)
		NBCAJAAABB_ENVOLVER_PUNTO_SIGUIENTE(cajaEscenaSombra, datosEspejo->limitesEscenaEspejo[1].x, datosEspejo->limitesEscenaEspejo[1].y)
		if(cajaEscenaSombra.xMin == cajaEscenaSombra.xMax){ cajaEscenaSombra.xMin -= 4.0f; cajaEscenaSombra.xMax += 4.0f; }
		if(cajaEscenaSombra.yMin == cajaEscenaSombra.yMax){ cajaEscenaSombra.yMin -= 4.0f; cajaEscenaSombra.yMax += 4.0f; }
		for(iIlum=0; iIlum<conteoIlum; iIlum++){
			NBFuenteIluminacionRender* datosIlum = &propsIluminacion.fuentesIluminacion[iIlum];
			//Validar que las cajas intersectan
			if(NBCAJAAABB_NO_INTERSECTA_CAJA(datosIlum->cajaEscenaLuz, cajaEscenaSombra)) continue;
			//Validar que la luz esta a la izquierda del espejo
			NBPunto vectorHaciaLuz;
			NBPUNTO_ESTABLECER(vectorHaciaLuz, datosIlum->posicionEscenaLuz.x-datosEspejo->limitesEscenaEspejo[0].x, datosIlum->posicionEscenaLuz.y-datosEspejo->limitesEscenaEspejo[0].y)
			float prodCruzHaciaLuz	= NBPUNTO_PRODUCTO_VECTORIAL(datosEspejo->vecEspejo.x, datosEspejo->vecEspejo.y, vectorHaciaLuz.x, vectorHaciaLuz.y);
			if(prodCruzHaciaLuz <= 0.0f) continue;
			//Registrar la sombra (omitir si solo forma una linea con la luz)
			NBFuenteSombra proyeccionSombra;
			NBPUNTO_ESTABLECER(proyeccionSombra.vectorAnguloMax, datosEspejo->limitesEscenaEspejo[1].x-datosIlum->posicionEscenaLuz.x, datosEspejo->limitesEscenaEspejo[1].y-datosIlum->posicionEscenaLuz.y)
			NBPUNTO_ESTABLECER(proyeccionSombra.vectorAnguloMin, datosEspejo->limitesEscenaEspejo[0].x-datosIlum->posicionEscenaLuz.x, datosEspejo->limitesEscenaEspejo[0].y-datosIlum->posicionEscenaLuz.y)
			if(NBPUNTO_PRODUCTO_VECTORIAL(proyeccionSombra.vectorAnguloMin.x, proyeccionSombra.vectorAnguloMin.y, proyeccionSombra.vectorAnguloMax.x, proyeccionSombra.vectorAnguloMax.y)>0.0f){
				AUArregloNativoMutableP<NBFuenteSombra>* arregloSombras	= propsIluminacion.sombrasPorFuenteIluminacion[iIlum];
				AUArregloNativoMutableP<NBPunto>* arregloVertices		= propsIluminacion.sombrasVerticesIntermedios;
				proyeccionSombra.objOriginaSombra		= datosEspejo->objEspejo;
				proyeccionSombra.objAgrupaSombras		= datosEspejo->objEspejo;
				proyeccionSombra.cajaEscenaSombra		= cajaEscenaSombra;
				proyeccionSombra.iPrimerVerticeAdicionalSombra	= 0;
				proyeccionSombra.conteoVerticesAdicionalSombra	= 0;
				proyeccionSombra.sombraCubiertaPorOtra	= NBSegmentadorFiguras::sombraEsCubiertaPorOtraYCubrirOtrasDosVertices(arregloSombras->elemento, arregloSombras->conteo, arregloVertices->elemento, proyeccionSombra.vectorAnguloMax, proyeccionSombra.vectorAnguloMin);
				NBASSERT(NBSegmentadorFiguras::debugValidarSombra(&arregloVertices->elemento[proyeccionSombra.iPrimerVerticeAdicionalSombra], proyeccionSombra.conteoVerticesAdicionalSombra, proyeccionSombra.vectorAnguloMin, proyeccionSombra.vectorAnguloMax));
				arregloSombras->agregarElemento(proyeccionSombra);
			}
		}
	}
	//PRINTF_INFO("%d vertices intermedios\n", propsIluminacion.sombrasVerticesIntermedios->conteo);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

void NBGestorEscena::privIlumAcumularSombraEnCache(const NBPunto &vectorAngMin, const NBPunto &vectorAngMax){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::privIlumAcumularSombraEnCache")
	//float angulosMin[1], angulosMax[1];
	//NBPUNTO_ANGULO_VECTOR(angulosMin[0], 0.0f, 0.0f, vectorAngMin.x, vectorAngMin.y);
	//NBPUNTO_ANGULO_VECTOR(angulosMax[0], 0.0f, 0.0f, vectorAngMax.x, vectorAngMax.y);
	//PRINTF_INFO("Analizando sombra angulos(%.3f, %.3f)", angulosMin[0], angulosMax[0]);
	//Quitar rangos que son totalmente cubiertos por este
	STRangoSombra* arrSmbAcum		= _cacheSombrasFusionadas->elemento;
	SI16 iSmbAcum;
	for(iSmbAcum = _cacheSombrasFusionadas->conteo - 1; iSmbAcum >= 0; iSmbAcum--){
		if(
		   NBPUNTO_PRODUCTO_VECTORIAL(vectorAngMax.x, vectorAngMax.y, arrSmbAcum[iSmbAcum].vectorAngMin.x, arrSmbAcum[iSmbAcum].vectorAngMin.y) <= 0.0f &&
		   NBPUNTO_PRODUCTO_VECTORIAL(vectorAngMin.x, vectorAngMin.y, arrSmbAcum[iSmbAcum].vectorAngMin.x, arrSmbAcum[iSmbAcum].vectorAngMin.y) >= 0.0f &&
		   NBPUNTO_PRODUCTO_VECTORIAL(vectorAngMax.x, vectorAngMax.y, arrSmbAcum[iSmbAcum].vectorAngMax.x, arrSmbAcum[iSmbAcum].vectorAngMax.y) <= 0.0f &&
		   NBPUNTO_PRODUCTO_VECTORIAL(vectorAngMin.x, vectorAngMin.y, arrSmbAcum[iSmbAcum].vectorAngMax.x, arrSmbAcum[iSmbAcum].vectorAngMax.y) >= 0.0f
		   ){
			//PRINTF_INFO("Elemento quitado porque sera totalmente cubierto por el nuevo\n");
			_cacheSombrasFusionadas->quitarElementoEnIndice(iSmbAcum);
		}
	}
	//Buscar sombras acumuladas que concuerden
	SI16 iSombraAcumAngMin	= -1, iSombraAcumAngMax = -1;
	arrSmbAcum				= _cacheSombrasFusionadas->elemento;
	SI16 conteoSmbAcum		= _cacheSombrasFusionadas->conteo;
	for(iSmbAcum=0; iSmbAcum<conteoSmbAcum; iSmbAcum++){
		if(NBPUNTO_PRODUCTO_VECTORIAL(arrSmbAcum[iSmbAcum].vectorAngMax.x, arrSmbAcum[iSmbAcum].vectorAngMax.y, vectorAngMin.x, vectorAngMin.y) <= 0.0f
		   && NBPUNTO_PRODUCTO_VECTORIAL(arrSmbAcum[iSmbAcum].vectorAngMin.x, arrSmbAcum[iSmbAcum].vectorAngMin.y, vectorAngMin.x, vectorAngMin.y) >= 0.0f){
			NBASSERT(iSombraAcumAngMin == -1) //Solo deberia existir una sombra
			iSombraAcumAngMin = iSmbAcum;
			#ifndef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
			if(iSombraAcumAngMax != -1) break;
			#endif
		}
		if(NBPUNTO_PRODUCTO_VECTORIAL(arrSmbAcum[iSmbAcum].vectorAngMax.x, arrSmbAcum[iSmbAcum].vectorAngMax.y, vectorAngMax.x, vectorAngMax.y) <= 0.0f
		   && NBPUNTO_PRODUCTO_VECTORIAL(arrSmbAcum[iSmbAcum].vectorAngMin.x, arrSmbAcum[iSmbAcum].vectorAngMin.y, vectorAngMax.x, vectorAngMax.y) >= 0.0f){
			NBASSERT(iSombraAcumAngMax == -1) //Solo deberia existir una sombra
			iSombraAcumAngMax = iSmbAcum;
			#ifndef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
			if(iSombraAcumAngMin != -1) break;
			#endif
		}
	}
	if(iSombraAcumAngMin == -1 && iSombraAcumAngMax == -1){
		//Sombra no coincide con ningun rango existente
		STRangoSombra sombraAcumular;
		sombraAcumular.vectorAngMin	= vectorAngMin;
		sombraAcumular.vectorAngMax	= vectorAngMax;
		NBASSERT(sombraAcumular.vectorAngMin.x == sombraAcumular.vectorAngMin.x && sombraAcumular.vectorAngMin.y == sombraAcumular.vectorAngMin.y)
		NBASSERT(sombraAcumular.vectorAngMax.x == sombraAcumular.vectorAngMax.x && sombraAcumular.vectorAngMax.y == sombraAcumular.vectorAngMax.y)
		_cacheSombrasFusionadas->agregarElemento(sombraAcumular);
		//PRINTF_INFO(" [agregado como nuevo]");
	} else if(iSombraAcumAngMin == iSombraAcumAngMax){
		//Un rango existente cubre a la sombra actual
		//PRINTF_INFO(" [ignorado]\n");
	} else if(iSombraAcumAngMin != -1 && iSombraAcumAngMax != -1){
		//Sombra fusiona a dos rangos existentes en uno solo
		arrSmbAcum[iSombraAcumAngMin].vectorAngMax = arrSmbAcum[iSombraAcumAngMax].vectorAngMax;
		_cacheSombrasFusionadas->quitarElementoEnIndice(iSombraAcumAngMax);
		//PRINTF_INFO(" [fusiona dos segmentos en uno solo]");
	} else if(iSombraAcumAngMin != -1){
		//Sombra alarga a un rango existente
		arrSmbAcum[iSombraAcumAngMin].vectorAngMax = vectorAngMax;
		//PRINTF_INFO(" [fusionado, incrementa el angMin]");
	} else {
		//Sombra alarga a un rango existente
		NBASSERT(iSombraAcumAngMin == -1 && iSombraAcumAngMax != -1)
		arrSmbAcum[iSombraAcumAngMax].vectorAngMin = vectorAngMin;
		//PRINTF_INFO(" [fusionado, incrementa el angMin]");
	}
	//PRINTF_INFO("\n");
	#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
	//DEBUG, validar arreglo de rangos
	//PRINTF_INFO("%d rangos\n", _cacheSombrasFusionadas->conteo);
	UI16 iRango1, iRango2; arrSmbAcum		= _cacheSombrasFusionadas->elemento;
	for(iRango1=0; iRango1<_cacheSombrasFusionadas->conteo; iRango1++){
		SI16 iSombraAcumAngMin = -1, iSombraAcumAngMax = -1;
		for(iRango2=iRango1+1; iRango2<_cacheSombrasFusionadas->conteo; iRango2++){
			//float angulosMin[2], angulosMax[2];
			//NBPUNTO_ANGULO_VECTOR(angulosMin[0], 0.0f, 0.0f, arrSmbAcum[iRango1].vectorAngMin.x, arrSmbAcum[iRango1].vectorAngMin.y);
			//NBPUNTO_ANGULO_VECTOR(angulosMin[1], 0.0f, 0.0f, arrSmbAcum[iRango2].vectorAngMin.x, arrSmbAcum[iRango2].vectorAngMin.y);
			//NBPUNTO_ANGULO_VECTOR(angulosMax[0], 0.0f, 0.0f, arrSmbAcum[iRango1].vectorAngMax.x, arrSmbAcum[iRango1].vectorAngMax.y);
			//NBPUNTO_ANGULO_VECTOR(angulosMax[1], 0.0f, 0.0f, arrSmbAcum[iRango2].vectorAngMax.x, arrSmbAcum[iRango2].vectorAngMax.y);
			//PRINTF_INFO("   ---- angulos(%.3f, %.3f) vs(%.3f, %.3f)\n", angulosMin[0], angulosMax[0], angulosMin[1], angulosMax[1]);
			if(NBPUNTO_PRODUCTO_VECTORIAL(arrSmbAcum[iRango2].vectorAngMax.x, arrSmbAcum[iRango2].vectorAngMax.y, arrSmbAcum[iRango1].vectorAngMin.x, arrSmbAcum[iRango1].vectorAngMin.y) <= 0.0f
			   && NBPUNTO_PRODUCTO_VECTORIAL(arrSmbAcum[iRango2].vectorAngMin.x, arrSmbAcum[iRango2].vectorAngMin.y, arrSmbAcum[iRango1].vectorAngMin.x, arrSmbAcum[iRango1].vectorAngMin.y) >= 0.0f){
				NBASSERT(iSombraAcumAngMin == -1) //Solo deberia existir una sombra
				iSombraAcumAngMin = iRango2;
			}
			if(NBPUNTO_PRODUCTO_VECTORIAL(arrSmbAcum[iRango2].vectorAngMax.x, arrSmbAcum[iRango2].vectorAngMax.y, arrSmbAcum[iRango1].vectorAngMax.x, arrSmbAcum[iRango1].vectorAngMax.y) <= 0.0f
			   && NBPUNTO_PRODUCTO_VECTORIAL(arrSmbAcum[iRango2].vectorAngMin.x, arrSmbAcum[iRango2].vectorAngMin.y, arrSmbAcum[iRango1].vectorAngMax.x, arrSmbAcum[iRango1].vectorAngMax.y) >= 0.0f){
				NBASSERT(iSombraAcumAngMax == -1) //Solo deberia existir una sombra
				iSombraAcumAngMax = iRango2;
			}
		}
		NBASSERT(iSombraAcumAngMin == -1 && iSombraAcumAngMax == -1)
	}
	#endif
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

void NBGestorEscena::privProducirLucesPorReflejoEnCapa(STGestorEscenaCapaRender* renderCapa, const UI16 iPrimeraLuz){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::privProducirLucesPorReflejoEnCapa")
	SI16 iIlum, conteoIlum = renderCapa->renderIluminacion->conteo;
	if(iPrimeraLuz<conteoIlum){
		SI16 iEspejo, conteoEspejos = renderCapa->renderEspejos->conteo;
		for(iIlum=conteoIlum - 1; iIlum >= iPrimeraLuz; iIlum--){ //En orden inverso, para evitar procesar las nuevas luces
			NBFuenteIluminacionRender* datosIlum		= &renderCapa->renderIluminacion->elemento[iIlum];
			//
			AUArregloNativoMutableP<NBFuenteSombra>* sombrasDeLuzOriginal2		= (AUArregloNativoMutableP<NBFuenteSombra>*)renderCapa->renderSombrasPorIluminacion->elemento[iIlum];
			NBFuenteSombra* arrSombrasDeLuzOriginal		= sombrasDeLuzOriginal2->elemento;
			UI16 conteoSombrasDeLuzOriginal				= sombrasDeLuzOriginal2->conteo;
			//Validar obstruida
			if(datosIlum->obstruida) continue;
			//Procesar cada espejo
			for(iEspejo = 0; iEspejo < conteoEspejos; iEspejo++){
				NBFuenteEspejoRender* datosEspejo = &renderCapa->renderEspejos->elemento[iEspejo];
				//Validar que no sea una luz producida por el mismo espejo
				if(datosIlum->objetoOrigenRefleja == datosEspejo->objEspejo) continue;
				//Validar interseccion de cajas
				if(NBCAJAAABB_NO_INTERSECTA_CAJA(datosEspejo->cajaEscenaEspejo, datosIlum->cajaEscenaLuz)){
					//PRINTF_INFO("Espejo no intersecta caja de luz CajaEspejo(%.2f, %.2f)-(%.2f, %.2f) CajaLuz(%.2f, %.2f)-(%.2f, %.2f)\n", datosEspejo->cajaEscenaEspejo.xMin, datosEspejo->cajaEscenaEspejo.xMax, datosEspejo->cajaEscenaEspejo.yMin, datosEspejo->cajaEscenaEspejo.yMax, datosIlum->cajaEscenaLuz.xMin, datosIlum->cajaEscenaLuz.xMax, datosIlum->cajaEscenaLuz.yMin, datosIlum->cajaEscenaLuz.yMax);
					continue;
				}
				//Limites locales a la luz
				NBPunto vectoresLimiteEspejo[2];
				NBPUNTO_ESTABLECER(vectoresLimiteEspejo[0], datosEspejo->limitesEscenaEspejo[0].x - datosIlum->posicionEscenaLuz.x, datosEspejo->limitesEscenaEspejo[0].y - datosIlum->posicionEscenaLuz.y)
				NBPUNTO_ESTABLECER(vectoresLimiteEspejo[1], datosEspejo->limitesEscenaEspejo[1].x - datosIlum->posicionEscenaLuz.x, datosEspejo->limitesEscenaEspejo[1].y - datosIlum->posicionEscenaLuz.y)
				//Validar que la luz esta a la izquierda del espejo y y en distancia perpendicular inferior al alcance
				if(NBPUNTO_PRODUCTO_VECTORIAL(datosEspejo->vecUnitEspejo.x, datosEspejo->vecUnitEspejo.y, vectoresLimiteEspejo[0].x, vectoresLimiteEspejo[0].y) >= 0.0f){
					//PRINTF_INFO("La luz no-esta a la izquierda del espejo\n");
					continue;
				}
				//Establecer vectores limites de luz
				if(datosIlum->limitesAngulosActivos){
					//Validar que el espejo esta dentro de los limites de la luz
					if(NBPUNTO_PRODUCTO_VECTORIAL(datosIlum->vecUnitAnguloMaxDesdeLuz.x, datosIlum->vecUnitAnguloMaxDesdeLuz.y, vectoresLimiteEspejo[0].x, vectoresLimiteEspejo[0].y) >= 0.0f ||
					   NBPUNTO_PRODUCTO_VECTORIAL(datosIlum->vecUnitAnguloMinDesdeLuz.x, datosIlum->vecUnitAnguloMinDesdeLuz.y, vectoresLimiteEspejo[1].x, vectoresLimiteEspejo[1].y) <= 0.0f){
						//PRINTF_INFO("La luz, sus-rangos-activos no abarcan al espejo\n");
						continue;
					}
					//Limitar la apertura de acorde a los limites del espejo
					if(NBPUNTO_PRODUCTO_VECTORIAL(datosIlum->vecUnitAnguloMinDesdeLuz.x, datosIlum->vecUnitAnguloMinDesdeLuz.y, vectoresLimiteEspejo[0].x, vectoresLimiteEspejo[0].y)<0.0f) vectoresLimiteEspejo[0] = datosIlum->vecUnitAnguloMinDesdeLuz;
					if(NBPUNTO_PRODUCTO_VECTORIAL(datosIlum->vecUnitAnguloMaxDesdeLuz.x, datosIlum->vecUnitAnguloMaxDesdeLuz.y, vectoresLimiteEspejo[1].x, vectoresLimiteEspejo[1].y)>0.0f) vectoresLimiteEspejo[1] = datosIlum->vecUnitAnguloMaxDesdeLuz;
				}
				//Acumular rangos de sombras a reflejar (y que limitan el angulo min y max del reflejo)
				_cacheSombrasFusionadas->vaciar();
				UI16 iSombra;
				for(iSombra=0; iSombra<conteoSombrasDeLuzOriginal; iSombra++){
					NBFuenteSombra* datosSombra		= &arrSombrasDeLuzOriginal[iSombra];
					//Validar que no sea la sombra del espejo, que no este cubierta por otra y que cubra al espejo en alguna porcion
					if(datosSombra->objOriginaSombra == datosEspejo->objEspejo || datosSombra->sombraCubiertaPorOtra ||
					   NBPUNTO_PRODUCTO_VECTORIAL(datosSombra->vectorAnguloMin.x, datosSombra->vectorAnguloMin.y, vectoresLimiteEspejo[1].x, vectoresLimiteEspejo[1].y) <= 0 ||
					   NBPUNTO_PRODUCTO_VECTORIAL(datosSombra->vectorAnguloMax.x, datosSombra->vectorAnguloMax.y, vectoresLimiteEspejo[0].x, vectoresLimiteEspejo[0].y) >= 0
					   ) continue;
					//La sombra esta a la izquierda del espejo. Validacion rapida (el primer y ultimo vertice estan a la izquierda)
					NBPunto vectorExtremoSombraDesdeEspejo[2];
					NBPUNTO_ESTABLECER(vectorExtremoSombraDesdeEspejo[0], datosSombra->vectorAnguloMax.x - vectoresLimiteEspejo[0].x, datosSombra->vectorAnguloMax.y - vectoresLimiteEspejo[0].y)
					NBPUNTO_ESTABLECER(vectorExtremoSombraDesdeEspejo[1], datosSombra->vectorAnguloMin.x - vectoresLimiteEspejo[0].x, datosSombra->vectorAnguloMin.y - vectoresLimiteEspejo[0].y)
					if(NBPUNTO_PRODUCTO_VECTORIAL(datosEspejo->vecUnitEspejo.x, datosEspejo->vecUnitEspejo.y, vectorExtremoSombraDesdeEspejo[0].x, vectorExtremoSombraDesdeEspejo[0].y)>0.0f &&
					   NBPUNTO_PRODUCTO_VECTORIAL(datosEspejo->vecUnitEspejo.x, datosEspejo->vecUnitEspejo.y, vectorExtremoSombraDesdeEspejo[1].x, vectorExtremoSombraDesdeEspejo[1].y)>0.0f
					   ){
						NBGestorEscena::privIlumAcumularSombraEnCache(datosSombra->vectorAnguloMin, datosSombra->vectorAnguloMax);
						continue; //Si los extremos estan a la izquierda del espejo, entonces toda la sombra esta a la izquierda
					}
					//Validar que la sombra tenga por lo menos un vertice a la izquierda del espejo (validacion lenta)
					//De ser asi, entonces validar solo la porcion a la izquierda del vector
					/*const NBPunto* arrVerticesSombra	= &renderCapa->renderSombrasVerticesIntermedios->elemento[datosSombra->iPrimerVerticeAdicionalSombra]; NBASSERT(datosSombra->iPrimerVerticeAdicionalSombra+datosSombra->conteoVerticesAdicionalSombra <= renderCapa->renderSombrasVerticesIntermedios->conteo)
					const UI16 conteoVertsSomb			= datosSombra->conteoVerticesAdicionalSombra; NBASSERT(conteoVertsSomb != 0)
					NBPunto vecInterMasIzq, vecInterMasDer;
					UI16 conteoVerticesIzqEspejo		= 0;
					UI16 iVertSomb, iVertSombAnt		= conteoVertsSomb - 1;
					NBPunto vectorVertSombraDesdeEspejo; NBPUNTO_ESTABLECER(vectorVertSombraDesdeEspejo, arrVerticesSombra[iVertSombAnt].x - vectoresLimiteEspejo[0].x, arrVerticesSombra[iVertSombAnt].y - vectoresLimiteEspejo[0].y)
					float prodCruzAnt					= NBPUNTO_PRODUCTO_VECTORIAL(datosEspejo->vecUnitEspejo.x, datosEspejo->vecUnitEspejo.y, vectorVertSombraDesdeEspejo.x, vectorVertSombraDesdeEspejo.y);
					if(prodCruzAnt>0.0f){
						NBPUNTO_ESTABLECER(vecInterMasIzq, arrVerticesSombra[iVertSombAnt].x - datosIlum->posicionEscenaLuz.x, arrVerticesSombra[iVertSombAnt].y - datosIlum->posicionEscenaLuz.y)
						vecInterMasDer					= vecInterMasIzq;
						conteoVerticesIzqEspejo++;
					}
					for(iVertSomb=0; iVertSomb<conteoVertsSomb; iVertSomb++){
						NBPUNTO_ESTABLECER(vectorVertSombraDesdeEspejo, arrVerticesSombra[iVertSomb].x - vectoresLimiteEspejo[0].x, arrVerticesSombra[iVertSomb].y - vectoresLimiteEspejo[0].y)
						float prodCruzAct				= NBPUNTO_PRODUCTO_VECTORIAL(datosEspejo->vecUnitEspejo.x, datosEspejo->vecUnitEspejo.y, vectorVertSombraDesdeEspejo.x, vectorVertSombraDesdeEspejo.y);
						if(prodCruzAct>0.0f){
							NBPunto vectorDesdeLuz;		NBPUNTO_ESTABLECER(vectorDesdeLuz, arrVerticesSombra[iVertSomb].x - datosIlum->posicionEscenaLuz.x, arrVerticesSombra[iVertSomb].y - datosIlum->posicionEscenaLuz.y)
							if(conteoVerticesIzqEspejo == 0 || NBPUNTO_PRODUCTO_VECTORIAL(vecInterMasIzq.x, vecInterMasIzq.y, vectorDesdeLuz.x, vectorDesdeLuz.y)>0.0f){
								vecInterMasIzq			= vectorDesdeLuz;
							}
							if(conteoVerticesIzqEspejo == 0 || NBPUNTO_PRODUCTO_VECTORIAL(vecInterMasDer.x, vecInterMasDer.y, vectorDesdeLuz.x, vectorDesdeLuz.y)<0.0f){
								vecInterMasDer			= vectorDesdeLuz;
							}
							conteoVerticesIzqEspejo++;
						}
						if((prodCruzAnt>0.0f && prodCruzAct <= 0.0f) || (prodCruzAnt <= 0.0f && prodCruzAct>0.0f)){
							float avanceAntes	= (prodCruzAnt<0.0f?-prodCruzAnt:prodCruzAnt);
							float avanceDespues	= (prodCruzAct<0.0f?-prodCruzAct:prodCruzAct); NBASSERT(avanceAntes != 0.0f || avanceDespues != 0.0f)
							float avanceRel		= avanceAntes / (avanceAntes + avanceDespues);
							NBPunto posInters;	NBPUNTO_ESTABLECER(posInters, arrVerticesSombra[iVertSombAnt].x + ((arrVerticesSombra[iVertSomb].x - arrVerticesSombra[iVertSombAnt].x) * avanceRel), arrVerticesSombra[iVertSombAnt].y + ((arrVerticesSombra[iVertSomb].y - arrVerticesSombra[iVertSombAnt].y) * avanceRel))
							NBPunto vectorDesdeLuz; NBPUNTO_ESTABLECER(vectorDesdeLuz, posInters.x - datosIlum->posicionEscenaLuz.x, posInters.y - datosIlum->posicionEscenaLuz.y)
							NBASSERT(conteoVerticesIzqEspejo != 0)
							if(NBPUNTO_PRODUCTO_VECTORIAL(vecInterMasIzq.x, vecInterMasIzq.y, vectorDesdeLuz.x, vectorDesdeLuz.y)>0.0f){
								vecInterMasIzq			= vectorDesdeLuz;
							}
							if(NBPUNTO_PRODUCTO_VECTORIAL(vecInterMasDer.x, vecInterMasDer.y, vectorDesdeLuz.x, vectorDesdeLuz.y)<0.0f){
								vecInterMasDer			= vectorDesdeLuz;
							}
						}
						iVertSombAnt	= iVertSomb;
						prodCruzAnt		= prodCruzAct;
					}
					if(conteoVerticesIzqEspejo != 0 && vecInterMasIzq != vecInterMasDer){
						NBASSERT(NBPUNTO_PRODUCTO_VECTORIAL(vecInterMasIzq.x, vecInterMasIzq.y, vecInterMasDer.x, vecInterMasDer.y)<0.0f)
						NBGestorEscena::privIlumAcumularSombraEnCache(vecInterMasDer, vecInterMasIzq);
					}*/
				}
				//Validar rangos que cubren a la sombra completa o que disminuyen su angulo
				//Y dejar solo los rangos de sombras internas
				SI16 iRango, iRangoConteo = _cacheSombrasFusionadas->conteo; //sombrasFusionadas->elemento;
				for(iRango=0; iRango<iRangoConteo; iRango++){
					STRangoSombra* datosRango	= &_cacheSombrasFusionadas->elemento[iRango];
					bool sombraEsExtremo		= false;
					if(NBPUNTO_PRODUCTO_VECTORIAL(datosRango->vectorAngMin.x, datosRango->vectorAngMin.y, vectoresLimiteEspejo[1].x, vectoresLimiteEspejo[1].y) >= 0.0f
					   && NBPUNTO_PRODUCTO_VECTORIAL(datosRango->vectorAngMax.x, datosRango->vectorAngMax.y, vectoresLimiteEspejo[1].x, vectoresLimiteEspejo[1].y) <= 0.0f){
						vectoresLimiteEspejo[1]	= datosRango->vectorAngMin;
						sombraEsExtremo			= true;
					}
					if(NBPUNTO_PRODUCTO_VECTORIAL(datosRango->vectorAngMin.x, datosRango->vectorAngMin.y, vectoresLimiteEspejo[0].x, vectoresLimiteEspejo[0].y) >= 0.0f
					   && NBPUNTO_PRODUCTO_VECTORIAL(datosRango->vectorAngMax.x, datosRango->vectorAngMax.y, vectoresLimiteEspejo[0].x, vectoresLimiteEspejo[0].y) <= 0.0f){
						vectoresLimiteEspejo[0]	= datosRango->vectorAngMax;
						sombraEsExtremo			= true;
					}
					if(sombraEsExtremo) _cacheSombrasFusionadas->quitarElementoEnIndice(iRango--);
				}
				if(NBPUNTO_PRODUCTO_VECTORIAL(vectoresLimiteEspejo[0].x, vectoresLimiteEspejo[0].y, vectoresLimiteEspejo[1].x, vectoresLimiteEspejo[1].y) <= 0.0f){
					//PRINTF_INFO("La luz es ocultada al espejo\n");
					continue;
				}
				float avanceEnEspejo, avanceIzq; NBPunto vectorRelejar;
				//Establecer posicion de la luz reflejada
				NBPunto posLuzReflejo;				NBPUNTO_POSICION_REFLEJADA(posLuzReflejo, datosEspejo->limitesEscenaEspejo[0], datosEspejo->vecUnitEspejo, datosIlum->posicionEscenaLuz, vectorRelejar, avanceIzq, avanceEnEspejo)
				//Definir limites de luz reflejada
				NBPunto vectUnitAngMin;				NBPUNTO_VECTOR_REFLEJADO(vectUnitAngMin, datosEspejo->vecUnitEspejo, vectoresLimiteEspejo[1], avanceIzq, avanceEnEspejo)
				NBPunto vectUnitAngMax;				NBPUNTO_VECTOR_REFLEJADO(vectUnitAngMax, datosEspejo->vecUnitEspejo, vectoresLimiteEspejo[0], avanceIzq, avanceEnEspejo)
				float longitudVectAngMin			= NBPUNTO_DISTANCIA_VECTOR(vectUnitAngMin.x, vectUnitAngMin.y);
				float longitudVectAngMax			= NBPUNTO_DISTANCIA_VECTOR(vectUnitAngMax.x, vectUnitAngMax.y);
				vectUnitAngMin.x					/= longitudVectAngMin;
				vectUnitAngMin.y					/= longitudVectAngMin;
				vectUnitAngMax.x					/= longitudVectAngMax;
				vectUnitAngMax.y					/= longitudVectAngMax;
				#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
				float debugModulo;
				debugModulo = NBPUNTO_DISTANCIA_VECTOR(vectUnitAngMin.x, vectUnitAngMin.y); NBASSERT(debugModulo >= 0.9999f && debugModulo<1.0001f)
				debugModulo = NBPUNTO_DISTANCIA_VECTOR(vectUnitAngMax.x, vectUnitAngMax.y); NBASSERT(debugModulo >= 0.9999f && debugModulo<1.0001f)
				#endif
				//
				NBFuenteIluminacionRender datosLuz;
				datosLuz.objetoOrigenLuz			= datosIlum->objetoOrigenLuz;
				datosLuz.objetoOrigenRefleja		= datosEspejo->objEspejo;
				datosLuz.objetoAgrupaSombras		= NULL;
				datosLuz.objetoTexturaLuz			= datosIlum->objetoTexturaLuz; //No hace falta retener, la luz original introdujo su textura en la piscina-retener.
				datosLuz.produceSombras				= datosIlum->produceSombras;
				datosLuz.obstruida					= datosIlum->obstruida;
				datosLuz.tipoIluminacion			= datosIlum->tipoIluminacion;
				datosLuz.colorLuzF					= datosIlum->colorLuzF;
				datosLuz.posicionEscenaLuz			= posLuzReflejo;
				//limitesAngulosActivos
				datosLuz.limitesAngulosActivos		= true;
				datosLuz.vecUnitAnguloMinDesdeLuz	= vectUnitAngMin;
				datosLuz.vecUnitAnguloMaxDesdeLuz	= vectUnitAngMax;
				if(NBPUNTO_PRODUCTO_VECTORIAL(datosLuz.vecUnitAnguloMinDesdeLuz.x, datosLuz.vecUnitAnguloMinDesdeLuz.y, datosLuz.vecUnitAnguloMaxDesdeLuz.x, datosLuz.vecUnitAnguloMaxDesdeLuz.y)<0.0f){
					NBPunto vecTmp = datosLuz.vecUnitAnguloMaxDesdeLuz;
					datosLuz.vecUnitAnguloMaxDesdeLuz = datosLuz.vecUnitAnguloMinDesdeLuz;
					datosLuz.vecUnitAnguloMinDesdeLuz = vecTmp;
				}
				//Reflejar la caja limite de la luz
				UI16 iVerCajaLuz;
				for(iVerCajaLuz=0; iVerCajaLuz<4; iVerCajaLuz++){
					NBPUNTO_VECTOR_REFLEJADO(datosLuz.cajaFiguraLuzLocal[iVerCajaLuz], datosEspejo->vecUnitEspejo, datosIlum->cajaFiguraLuzLocal[iVerCajaLuz], avanceIzq, avanceEnEspejo)
				}
				NBPunto vector_1_0; NBPUNTO_ESTABLECER(vector_1_0, datosLuz.cajaFiguraLuzLocal[1].x-datosLuz.cajaFiguraLuzLocal[0].x, datosLuz.cajaFiguraLuzLocal[1].y-datosLuz.cajaFiguraLuzLocal[0].y);
				NBPunto vector_2_0; NBPUNTO_ESTABLECER(vector_2_0, datosLuz.cajaFiguraLuzLocal[2].x-datosLuz.cajaFiguraLuzLocal[0].x, datosLuz.cajaFiguraLuzLocal[2].y-datosLuz.cajaFiguraLuzLocal[0].y);
				if(NBPUNTO_PRODUCTO_VECTORIAL(vector_1_0.x, vector_1_0.y, vector_2_0.x, vector_2_0.y)>0.0f){
					datosLuz.cajaFiguraLuzEsContrareloj		= true;
					datosLuz.posLocalVectorAvanceX			= datosLuz.cajaFiguraLuzLocal[0];
					datosLuz.posLocalVectorAvanceY			= datosLuz.cajaFiguraLuzLocal[1];
					NBPUNTO_ESTABLECER(datosLuz.vecUnitAvanceX, datosLuz.cajaFiguraLuzLocal[1].x - datosLuz.cajaFiguraLuzLocal[0].x, datosLuz.cajaFiguraLuzLocal[1].y - datosLuz.cajaFiguraLuzLocal[0].y)
					NBPUNTO_ESTABLECER(datosLuz.vecUnitAvanceY, datosLuz.cajaFiguraLuzLocal[2].x - datosLuz.cajaFiguraLuzLocal[1].x, datosLuz.cajaFiguraLuzLocal[2].y - datosLuz.cajaFiguraLuzLocal[1].y)
				} else {
					datosLuz.cajaFiguraLuzEsContrareloj		= false;
					datosLuz.posLocalVectorAvanceX			= datosLuz.cajaFiguraLuzLocal[3];
					datosLuz.posLocalVectorAvanceY			= datosLuz.cajaFiguraLuzLocal[2];
					NBPUNTO_ESTABLECER(datosLuz.vecUnitAvanceX, datosLuz.cajaFiguraLuzLocal[2].x - datosLuz.cajaFiguraLuzLocal[3].x, datosLuz.cajaFiguraLuzLocal[2].y - datosLuz.cajaFiguraLuzLocal[3].y)
					NBPUNTO_ESTABLECER(datosLuz.vecUnitAvanceY, datosLuz.cajaFiguraLuzLocal[1].x - datosLuz.cajaFiguraLuzLocal[2].x, datosLuz.cajaFiguraLuzLocal[1].y - datosLuz.cajaFiguraLuzLocal[2].y)
				}
				datosLuz.maxAvanceX			= NBPUNTO_DISTANCIA_VECTOR(datosLuz.vecUnitAvanceX.x, datosLuz.vecUnitAvanceX.y); NBASSERT(datosLuz.maxAvanceX>0.0f)
				datosLuz.maxAvanceY			= NBPUNTO_DISTANCIA_VECTOR(datosLuz.vecUnitAvanceY.x, datosLuz.vecUnitAvanceY.y); NBASSERT(datosLuz.maxAvanceY>0.0f)
				datosLuz.vecUnitAvanceX.x	/= datosLuz.maxAvanceX;
				datosLuz.vecUnitAvanceX.y	/= datosLuz.maxAvanceX;
				datosLuz.vecUnitAvanceY.x	/= datosLuz.maxAvanceY;
				datosLuz.vecUnitAvanceY.y	/= datosLuz.maxAvanceY;
				NBASSERT(NBPUNTO_PRODUCTO_VECTORIAL(datosLuz.vecUnitAvanceX.x, datosLuz.vecUnitAvanceX.y, datosLuz.vecUnitAvanceY.x, datosLuz.vecUnitAvanceY.y)>0.0f) //Si falla, el vectorY no apunta hacia la izquierda de vectorX
				//Limite de forma de luz
				datosLuz.limitesFormaActivos		= true;
				NBPunto vectoresLimitesEspejo[2];
				NBPUNTO_ESTABLECER(vectoresLimitesEspejo[0], datosEspejo->limitesEscenaEspejo[0].x - datosIlum->posicionEscenaLuz.x, datosEspejo->limitesEscenaEspejo[0].y - datosIlum->posicionEscenaLuz.y)
				NBPUNTO_ESTABLECER(vectoresLimitesEspejo[1], datosEspejo->limitesEscenaEspejo[1].x - datosIlum->posicionEscenaLuz.x, datosEspejo->limitesEscenaEspejo[1].y - datosIlum->posicionEscenaLuz.y)
				NBPUNTO_VECTOR_REFLEJADO(datosLuz.posIniVecLimiteForma, datosEspejo->vecUnitEspejo, vectoresLimitesEspejo[1], avanceIzq, avanceEnEspejo)	//En la luz relfejo, se invierte el orden (para que la luz siempre este a la izquierda del vector)
				NBPUNTO_VECTOR_REFLEJADO(datosLuz.posFinVecLimiteForma, datosEspejo->vecUnitEspejo, vectoresLimitesEspejo[0], avanceIzq, avanceEnEspejo)	//En la luz relfejo, se invierte el orden (para que la luz siempre este a la izquierda del vector)
				datosLuz.vecUnitLimiteForma.x		= -datosEspejo->vecUnitEspejo.x;
				datosLuz.vecUnitLimiteForma.y		= -datosEspejo->vecUnitEspejo.y;
				UI16 iNuevaIlum						= renderCapa->renderIluminacion->conteo;
				NBGestorEscena::privInicializarCacheIluminacion(renderCapa, iNuevaIlum);
				NBGestorEscena::privActualizarCajaEscenaLuz(datosLuz);
				renderCapa->renderIluminacion->agregarElemento(datosLuz);
				//Registrar las sombras preagrupadas
				AUArregloNativoMutableP<STRangoSombra>* sombrasPreagrupadasLuzEspejo = (AUArregloNativoMutableP<STRangoSombra>*)renderCapa->renderSombrasRangosPorIluminacion->elemento[iNuevaIlum];
				sombrasPreagrupadasLuzEspejo->vaciar();
				const STRangoSombra* arrRangos	= _cacheSombrasFusionadas->elemento;
				const UI16 cantRangos			= _cacheSombrasFusionadas->conteo;
				for(iRango=0; iRango<cantRangos; iRango++){
					STRangoSombra nuevoRango;
					NBPUNTO_VECTOR_REFLEJADO(nuevoRango.vectorAngMin, datosEspejo->vecUnitEspejo, arrRangos[iRango].vectorAngMax, avanceIzq, avanceEnEspejo)
					NBPUNTO_VECTOR_REFLEJADO(nuevoRango.vectorAngMax, datosEspejo->vecUnitEspejo, arrRangos[iRango].vectorAngMin, avanceIzq, avanceEnEspejo)
					if(nuevoRango.vectorAngMin != nuevoRango.vectorAngMax){
						NBASSERT(NBPUNTO_PRODUCTO_VECTORIAL(nuevoRango.vectorAngMin.x, nuevoRango.vectorAngMin.y, nuevoRango.vectorAngMax.x, nuevoRango.vectorAngMax.y)>0.0f)
						sombrasPreagrupadasLuzEspejo->agregarElemento(nuevoRango);
					}
				}
			}
		}
	}
	//renderCapa
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

void NBGestorEscena::privConsumirEspejosEnCapa(STGestorEscenaCapaRender* renderCapa){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::privConsumirEspejosEnCapa")
	IEscenaConsumidorEspejos** arrConsumidores = renderCapa->renderConsumidoresEspejos->elemento;
	UI16 iCsm, conteoCsm = renderCapa->renderConsumidoresEspejos->conteo;
	for(iCsm=0; iCsm<conteoCsm; iCsm++){
		arrConsumidores[iCsm]->consumirEspejos(renderCapa->renderEspejos->elemento, renderCapa->renderEspejos->conteo);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

void NBGestorEscena::privConsumirCuerdasEnCapa(STGestorEscenaCapaRender* renderCapa){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::privConsumirCuerdasEnCapa")
	ICuerdaCortable** arrCuerdas				= renderCapa->renderDescriptoresCuerdas->elemento;
	const UI16 conteoCuerdas					= renderCapa->renderDescriptoresCuerdas->conteo;
	IEscenaConsumidorCuerdas** arrConsumidores	= renderCapa->renderConsumidoresCuerdas->elemento;
	const UI16 conteoCsm						= renderCapa->renderConsumidoresCuerdas->conteo;
	UI16 iCsm;
	for(iCsm=0; iCsm<conteoCsm; iCsm++){
		arrConsumidores[iCsm]->consumirCuerdas(arrCuerdas, conteoCuerdas);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

void NBGestorEscena::privConsumirPreModeloGLEnCapa(STGestorEscenaCapaRender* renderCapa){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::privConsumirPreModeloGLEnCapa")
	IEscenaConsumidorPreModeloGL** arrConsumidores = renderCapa->renderConsumidoresPreModeloGL->elemento;
	UI16 iCsm, conteoCsm = renderCapa->renderConsumidoresPreModeloGL->conteo;
	for(iCsm=0; iCsm<conteoCsm; iCsm++){
		arrConsumidores[iCsm]->consumirAntesDeCrearModeloGL();
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

void NBGestorEscena::privConsumirLucesEnCapa(STGestorEscenaCapaRender* renderCapa){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::privConsumirLucesEnCapa")
	NBPropIluminacion propsIluminacion;
	propsIluminacion.conteoFuentesIluminacion					= renderCapa->renderIluminacion->conteo;
	propsIluminacion.fuentesIluminacion							= renderCapa->renderIluminacion->elemento;
	propsIluminacion.sombrasPorFuenteIluminacion				= (AUArregloNativoMutableP<NBFuenteSombra>**)renderCapa->renderSombrasPorIluminacion->elemento; NBASSERT(renderCapa->renderIluminacion->conteo <= renderCapa->renderSombrasPorIluminacion->conteo)
	propsIluminacion.sombrasVerticesIntermedios					= renderCapa->renderSombrasVerticesIntermedios; //La secuencia de vertices entre el anguloMax y el anguloMin (cuando la sombra es una linea no deposita ningun vertice ene ste arreglo)
	//
	IEscenaConsumidorLuces** arrConsumidores = renderCapa->renderConsumidoresLuces->elemento;
	UI16 iCsm, conteoCsm = renderCapa->renderConsumidoresLuces->conteo;
	for(iCsm=0; iCsm<conteoCsm; iCsm++){
		arrConsumidores[iCsm]->consumirLuces(propsIluminacion);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

void NBGestorEscena::consumirBufferRenderYEnviarComandosDibujo(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::consumirBufferRenderYEnviarComandosDibujo")
	NBASSERT(_gestorInicializado);
	NBASSERT(_bufferDatosBloqueado[_indiceBufferDatosLeer]);	//Debe estar bloqueado
	NBASSERT(_bufferDatosEstado[_indiceBufferDatosLeer] != ENGestorEscenaBufferEstado_Vacio);		//Debe estar lleno
	//NBGestorTexturas::bloquear();
	//PRINTF_INFO("Inicio de ciclo de dibujar modelos\n");
	_ticksAcumulados++;
	_secuencialRenderizadasModelosGL++; //NBASSERT(_secuencialRenderizadasModelosGL < 60) //TEMPORAL (para analizar lista de comandosGl ejecutados)
	//------------------------------
	// APPLE recomienda que el ciclo de renderizado conste de los siguientes pasos:
	// 1: BORRAR LOS RENDERBUFFERS con glClear, es una forma eficiente de borrado 
	//    y en algunas implementaciones evita tareas pesadas de carga de contenido previo en memoria.
	// 2: PREPARAR LOS OBJETOS OpenGL: preparar todos los recursos para renderizar (arreglos, buffers, texturas, etc...)
	// 3: EJECUTAR COMANDOS DE DIBUJO: se recomienda evitar alternar entre comandos de modificacion-objetos y dibujo. 
	//    En este paso es mas eficiente enviar comandos de dibujo sin modificar lo objetos.
	// 4: RESOLVER MULTISAMPLING
	// 5: DESCARTAR RENDER BUFFERS no necesarios: solamente disponible de iOS4 en adelante. Usando glDiscardFramebufferEXT 
	//    se le puede indicar a OpenGL que los datos no seran necesarios y asi evitar cargar pesadas e inecesarias.
	// 6: PRESENTAR RESULTADOS: volcar a pantalla
	//------------------------------
	SI32 iEscena;
	for(iEscena = NBGESTORESCENA_MAX_ESCENAS - 1; iEscena >= 0; iEscena--){
		STGestorEscenaEscena* scn = &_escenas[iEscena];
		scn->pendienteRenderizar = true;
	}
	//Usar VBOs (VRAM) ...
	NBGestorGL::prepararVerticesGLParaRenderizado();
	//Producir mascaras de iluminacion renderizadas-hacia-textura
	NBGestorEscena::privEnviarComandosDibujarMascarasIluminacion();
	//Producir escenas
	//Notas sobre los FrameBufferObject (FBO): pueden tener uno o varios adjuntos (RenderBuffer o Textures) a los cuales renderizar simultaneamente.
	//El cambio de FameBuffers es bastante costoso, el cambio de adjuntos en los FrameBuffer es mucho mas eficiente.
	for(iEscena = NBGESTORESCENA_MAX_ESCENAS - 1; iEscena >= 0; iEscena--){
		STGestorEscenaEscena* datEscena	= &_escenas[iEscena];
		if(datEscena->registroOcupado && datEscena->escenaHabilitada && datEscena->pendienteRenderizar){
			bool frameBufferLimpiado				= false;
			NBColor colorFondoLimpiado;				NBCOLOR_ESTABLECER(colorFondoLimpiado, 0.0f, 0.0f, 0.0f, 0.0f)
			// ------------------
			// -- Renderizar esta escena
			// ------------------
			NBGestorEscena::privConsumirBufferRenderYEnviarComandosDibujoDeEscenaEnOrdenDirecto(iEscena, frameBufferLimpiado, colorFondoLimpiado);
			//NBGestorEscena::privConsumirBufferRenderYEnviarComandosDibujoDeEscenaPriorizandoUnaCapaConLuz(_indiceBufferDatosLeer, iEscena, frameBufferLimpiado, colorFondoLimpiado);
			datEscena->pendienteRenderizar	= false;
			// ------------------
			// -- Si la escena no renderizo nada.
			// -- Por lo menos incializar (para que no termine presentando el mismo contenido anterior)
			// ------
			STGestorEscenaFrameBuffer* datosFB	= &_frameBuffersGL[datEscena->iFramebufferEscena];
			if(!frameBufferLimpiado){
				NBGestorGL::bindFramebufferEXT(GL_FRAMEBUFFER_EXT, datosFB->idFrameBufferGl);
				NBASSERT((datosFB->texturaOriginal == NULL && datosFB->renderbufferOriginal != NULL) || (datosFB->texturaOriginal != NULL && datosFB->renderbufferOriginal == NULL) || datosFB->tipoDestinoGL == ENGestorEscenaDestinoGl_Heredado)
				if(datosFB->texturaOriginal != NULL){
					NBGestorEscena::privFrameBufferEnlazarTextura(datEscena->iFramebufferEscena, datosFB->texturaOriginal);
				} else if(datosFB->renderbufferOriginal != NULL){
					NBGestorEscena::privFrameBufferEnlazarRenderBuffer(datEscena->iFramebufferEscena, datosFB->renderbufferOriginal);
				}
				NBGestorGL::scissor(0, 0, datosFB->anchoFrameBufferGl, datosFB->altoFrameBufferGl);
				if(datEscena->limpiarColorBuffer){
					NBGestorGL::clearColor(datEscena->renderColorFondo.r, datEscena->renderColorFondo.g, datEscena->renderColorFondo.b, (datEscena->fondoModo == ENGestorEscenaFondoModo_Opaco ? 1.0f : datEscena->renderColorFondo.a));
					NBGestorGL::clear(GL_COLOR_BUFFER_BIT);
				}
			}
			//TODO: porque este bloque? Borraria la escena previamente renderizada? Analizar y borrar si no tiene uso.
			if(frameBufferLimpiado && datEscena->fondoModo == ENGestorEscenaFondoModo_Opaco && datosFB->texturaOriginal != NULL){
				//NBASSERT(false) //PENDIENTE: establecer si es eficiente o no. //Rendimiento???
				NBGestorEscena::privFrameBufferEnlazarTextura(datEscena->iFramebufferEscena, datosFB->texturaOriginal);
				NBGestorGL::colorMask(false, false, false, true);
				NBGestorGL::clearColor(0.0f, 0.0f, 0.0f, 1.0f);
				NBGestorGL::clear(GL_COLOR_BUFFER_BIT);
				NBGestorGL::colorMask(true, true, true, true);
			}
		}
	}
	#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
	for(iEscena = NBGESTORESCENA_MAX_ESCENAS - 1; iEscena >= 0; iEscena--){
		STGestorEscenaEscena* datEscena	= &_escenas[iEscena];
		if(datEscena->registroOcupado && datEscena->escenaHabilitada){
			NBASSERT(!datEscena->pendienteRenderizar) //Si falla, alguna escena quedo sin renderizar
		}
	}
	#endif
	//NBGestorGL::flushCacheLocal(); //Esto provoca el envio de los lotes acumulados locales
	NBGestorGL::flush();  //Esto provoca el envio de los comando pendiente del cliente al servidor. (pero no espera a que terminen)
	//NBGestorGL::finish(); //PENDIENTE: comentariar //Esto provoca una espera hasta que todos los comandos GL han sido finalizados.
	//NBGestorTexturas::desbloquear();
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

void NBGestorEscena::privAsegurarFrameBufferInicializadoParaEscena(const STGestorEscenaFrameBuffer* datosFB, const STGestorEscenaEscena* scn, bool &frameBufferLimpiado, NBColor &colorFondoLimpiado, bool &escenaInicializada){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::privAsegurarFrameBufferInicializadoParaEscena")
	//STGestorEscenaFrameBuffer* frameBuff = &_frameBuffersGL[scn->iFramebufferEscena];
	if(!frameBufferLimpiado){
		NBGestorGL::bindFramebufferEXT(GL_FRAMEBUFFER_EXT, datosFB->idFrameBufferGl);
		NBASSERT((datosFB->texturaOriginal == NULL && datosFB->renderbufferOriginal != NULL) || (datosFB->texturaOriginal != NULL && datosFB->renderbufferOriginal == NULL) || datosFB->tipoDestinoGL == ENGestorEscenaDestinoGl_Heredado)
		if(datosFB->texturaOriginal != NULL){
			NBGestorEscena::privFrameBufferEnlazarTextura(scn->iFramebufferEscena, datosFB->texturaOriginal);
		} else if(datosFB->renderbufferOriginal != NULL){
			NBGestorEscena::privFrameBufferEnlazarRenderBuffer(scn->iFramebufferEscena, datosFB->renderbufferOriginal);
		}
		NBGestorGL::scissor(0, 0, datosFB->anchoFrameBufferGl, datosFB->altoFrameBufferGl);
		if(scn->limpiarColorBuffer){
			NBGestorGL::clearColor(scn->renderColorFondo.r, scn->renderColorFondo.g, scn->renderColorFondo.b, 0.0f); //alpha 0.0f es requerido para pintar los modelos en orden inverso
			NBGestorGL::clear(GL_COLOR_BUFFER_BIT); //(datEscena->fondoModo == ENGestorEscenaFondoModo_Opaco ? 1.0f : datEscena->renderColorFondo.a)
		}
		frameBufferLimpiado	= true;
		colorFondoLimpiado	= scn->renderColorFondo;
	}
	if(!escenaInicializada){
		//Preparar escena
		NBASSERT(scn->renderColorFondo == colorFondoLimpiado) //Si falla, entonces se intento limpiar el destino del buffer despues que se habia pintado contenido (el cual se perderia), lo cual no es posible sin el SCISSOR_TEST habilitado
		escenaInicializada = true;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

void NBGestorEscena::privConsumirBufferRenderYEnviarComandosDibujoDeCapa(const STGestorEscenaFrameBuffer* datosFB, const STGestorEscenaEscena* scn, const STGestorEscenaCapaRender* renderCapa, const bool enOrdenDirecto, const NBColor colorMultiplicador, bool &frameBufferLimpiado, NBColor &colorFondoLimpiado, bool &escenaInicializada){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::privConsumirBufferRenderYEnviarComandosDibujoDeCapa")
	NBGestorEscena::privAsegurarFrameBufferInicializadoParaEscena(datosFB, scn, frameBufferLimpiado, colorFondoLimpiado, escenaInicializada);
	//PROYECCION
	NBGestorGL::matrixMode(GL_PROJECTION);
	NBGestorGL::loadIdentity();
	NBGestorGL::viewport(scn->renderPuertoDeVision.x, scn->renderPuertoDeVision.y, scn->renderPuertoDeVision.ancho, scn->renderPuertoDeVision.alto);
	NBGestorGL::orthof(renderCapa->renderCajaProyeccion.xMin, renderCapa->renderCajaProyeccion.xMax, renderCapa->renderCajaProyeccion.yMin, renderCapa->renderCajaProyeccion.yMax);
	//MODELO
	NBGestorGL::matrixMode(GL_MODELVIEW);
	NBGestorGL::loadMatrixf(renderCapa->renderMatrizCapa);
	if(enOrdenDirecto){
		NBGestorGL::blendFuncEstablecerValoresPorDefecto(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //cuando se pinta de atras hacia adelante
	} else {
		NBGestorGL::blendFuncEstablecerValoresPorDefecto(GL_ONE_MINUS_DST_ALPHA, GL_DST_ALPHA); //cuando se pinta de adelante hacia atras
	}
	NBGestorGL::blendFuncCargarValoresPorDefecto();
	//PINTAR SECUENCIA DE MODELOS
	SI32 iModIni, iModSigUlt, incModelo;
	if(enOrdenDirecto){
		iModIni		= 0;
		iModSigUlt	= renderCapa->renderModelos->conteo;
		incModelo	= 1;
	} else {
		iModIni		= renderCapa->renderModelos->conteo - 1;
		iModSigUlt	= -1;
		incModelo	= -1;
	}
	//COLOR MULTIPLICADOR
	//NBGestorGL::color4f(1.0f, 1.0f, 1.0f, 1.0f); //Funciona cuando NO-HAY colores definidos en los vertices
	NBGestorGL::lightModelAmbient(colorMultiplicador); //Requerido cuando HAY colores definidos en los vertices
	#ifdef CONFIG_NB_GESTOR_ESCENAS_VERIFICAR_FRONTERAS_BLOQUES_DATOS_RENDER
	UI32* dbgPrtInicialAnterior = NULL;
	#endif
	SI32 iMod;
	for(iMod = iModIni; iMod != iModSigUlt; iMod += incModelo){
		const STPropRenderModelo* propsRender = &renderCapa->renderModelos->elemento[iMod]; NBASSERT(iMod >= 0 && iMod < renderCapa->renderModelos->conteo)
		NBASSERT(propsRender->funcEnvComadosGL != NULL)
		//
		#ifdef CONFIG_NB_GESTOR_ESCENAS_VERIFICAR_FRONTERAS_BLOQUES_DATOS_RENDER
		if(dbgPrtInicialAnterior != NULL){ NBASSERT(dbgPrtInicialAnterior[-2] == NBGESTORESCENA_VAL_VERIF_BLOQUE_DATOS_RENDER_FIN) } //Si falla, algun objeto render escribio en el espacio de datos de otro
		dbgPrtInicialAnterior = (UI32*)&renderCapa->renderModelosDatos->elemento[propsRender->indiceDatos];
		NBASSERT(dbgPrtInicialAnterior[-1] == NBGESTORESCENA_VAL_VERIF_BLOQUE_DATOS_RENDER_INI) //Si falla, algun objeto render escribio en el espacio de datos de otro
		#endif
		//
		(*propsRender->funcEnvComadosGL)(&renderCapa->renderModelosDatos->elemento[propsRender->indiceDatos]);
		//
	}
	#ifdef CONFIG_NB_GESTOR_ESCENAS_VERIFICAR_FRONTERAS_BLOQUES_DATOS_RENDER
	if(dbgPrtInicialAnterior != NULL){
		NBASSERT(((UI32*)&renderCapa->renderModelosDatos->elemento[renderCapa->renderModelosDatos->conteo])[-1] == NBGESTORESCENA_VAL_VERIF_BLOQUE_DATOS_RENDER_FIN) //Si falla, algun objeto render escribio en el espacio de datos de otro
	}
	#endif
	#ifdef NB_GESTOR_ESCENAS_USA_CACHE_LOTE_INSTRUCCIONES
	NBGestorGL::flushCacheLocal();
	#endif
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

/*
 METODO DE PINTADO SIMPLE:
 - Se pinta cada capa en orden y se aplica la iluminacion si tiene.
 NOTA: cada capa de luz afecta a la anteior.
 */
void NBGestorEscena::privConsumirBufferRenderYEnviarComandosDibujoDeEscenaEnOrdenDirecto(const SI32 iEscena, bool &frameBufferLimpiado, NBColor &colorFondoLimpiado){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::privConsumirBufferRenderYEnviarComandosDibujoDeEscenaEnOrdenDirecto")
	NBASSERT(_gestorInicializado)
	NBASSERT(_bufferDatosBloqueado[_indiceBufferDatosLeer])
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	NBASSERT(scn->registroOcupado)
	NBASSERT(scn->escenaHabilitada)
	NBASSERT(scn->pendienteRenderizar)
	STGestorEscenaFrameBuffer* datosFB	= &_frameBuffersGL[scn->iFramebufferEscena];
	bool escenaInicializada				= false;
	NBASSERT(scn->renderCapasConsumir[_indiceBufferDatosLeer] != NULL)
	NBASSERT(scn->renderCapasProducir[_indiceBufferDatosLeer] != NULL)
	NBASSERT(scn->renderCapas[_indiceBufferDatosLeer] != NULL)
	STGestorEscenaCapaRender** arrRenderCapa	= scn->renderCapasConsumir[_indiceBufferDatosLeer]->elemento;
	NBColor colorAmbiente;
	SI16 iRenderCapa, iRenderCapa2, conteoRenderCapas			= scn->renderCapasConsumir[_indiceBufferDatosLeer]->conteo;
	for(iRenderCapa=0; iRenderCapa<conteoRenderCapas; iRenderCapa++){
		STGestorEscenaCapaRender* renderCapa	= arrRenderCapa[iRenderCapa];
		if(renderCapa->renderMascLucesCombinadas.areaReservada.ancho <= 0 || renderCapa->renderMascLucesCombinadas.areaReservada.alto <= 0){
			//------------------
			//-- No tiene mascara de iluminacion
			//------------------
			const SI32 conteoModelos = renderCapa->renderModelos->conteo;
			if(conteoModelos != 0){
				//Determinar el color de ambiente (multiplicando con el de las capas superiores que tienen mascara de iluminacion)
				colorAmbiente	= renderCapa->renderColorLuzAmbiente;
				for(iRenderCapa2=iRenderCapa+1; iRenderCapa2<conteoRenderCapas; iRenderCapa2++){
					STGestorEscenaCapaRender* renderCapa2	= arrRenderCapa[iRenderCapa2];
					if(renderCapa2->tmpImplementarMascaraLuces){ //La capa afecta a sus capas anteriores
						if(renderCapa2->renderMascLucesCombinadas.areaReservada.ancho <= 0 || renderCapa2->renderMascLucesCombinadas.areaReservada.alto <= 0){ //La capa no tiene mascaraIluminacion, aplicar multiplicadorColor
							colorAmbiente.r		*= renderCapa2->renderColorLuzAmbiente.r;
							colorAmbiente.g		*= renderCapa2->renderColorLuzAmbiente.g;
							colorAmbiente.b		*= renderCapa2->renderColorLuzAmbiente.b;
							colorAmbiente.a		*= renderCapa2->renderColorLuzAmbiente.a;
						}
					}
				}
				NBGestorEscena::privAsegurarFrameBufferInicializadoParaEscena(datosFB, scn, frameBufferLimpiado, colorFondoLimpiado, escenaInicializada);
				NBGestorEscena::privConsumirBufferRenderYEnviarComandosDibujoDeCapa(datosFB, scn, renderCapa, true, colorAmbiente, frameBufferLimpiado, colorFondoLimpiado, escenaInicializada); //true, en orden directo
			}
		} else {
			//------------------
			//- Tiene mascara de iluminacion
			//------------------
			const SI32 conteoModelos = renderCapa->renderModelos->conteo;
			if(conteoModelos != 0){
				NBGestorEscena::privAsegurarFrameBufferInicializadoParaEscena(datosFB, scn, frameBufferLimpiado, colorFondoLimpiado, escenaInicializada);
				NBColor colorMultiplicador; NBCOLOR_ESTABLECER(colorMultiplicador, 1.0f, 1.0f, 1.0f, 1.0);
				NBGestorEscena::privConsumirBufferRenderYEnviarComandosDibujoDeCapa(datosFB, scn, renderCapa, true, colorMultiplicador, frameBufferLimpiado, colorFondoLimpiado, escenaInicializada); //true, en orden directo
			}
			//PINTAR MASCARA DE ILUMINACION
			NBGestorEscena::privAsegurarFrameBufferInicializadoParaEscena(datosFB, scn, frameBufferLimpiado, colorFondoLimpiado, escenaInicializada);
			NBASSERT(renderCapa->renderMascLucesCombinadas.areaReservada.ancho>0 && renderCapa->renderMascLucesCombinadas.areaReservada.alto>0)
			NBASSERT(renderCapa->renderMascLucesCombinadas.indicesRenderMascLuz.cantidadElementos != 0)
			NBASSERT(_texturasParaRenderizadoGL[renderCapa->renderMascLucesCombinadas.iTexRender].objTexturaAtlas != NULL)
			//NBGestorGL::color4f(1.0f, 1.0f, 1.0f, 1.0f);	//Funciona cuando NO-HAY colores definidos en los vertices
			NBGestorGL::lightModelAmbient(1.0f, 1.0f, 1.0f, 1.0f); //Requerido cuando HAY colores definidos en los vertices
			NBGestorGL::matrixMode(GL_PROJECTION);
			NBGestorGL::loadIdentity();
			NBGestorGL::viewport(scn->renderPuertoDeVision.x, scn->renderPuertoDeVision.y, scn->renderPuertoDeVision.ancho, scn->renderPuertoDeVision.alto);
			NBGestorGL::orthof(renderCapa->renderCajaProyeccion.xMin, renderCapa->renderCajaProyeccion.xMax, renderCapa->renderCajaProyeccion.yMin, renderCapa->renderCajaProyeccion.yMax);
			NBGestorGL::matrixMode(GL_MODELVIEW);
			NBGestorGL::loadMatrixf(renderCapa->renderMatrizCapa);
			//NBGestorGL::colorMask(true, true, true, false); //En orden directo no es necesario el colormask
			NBGestorGL::blendFunc(GL_ZERO, GL_SRC_COLOR);
			NBGestorGL::activarVerticesGL(ENVerticeGlTipo_MonoTextura); NBGESTORGL_DBG_NOMBRAR_ACTIVADOR_VERTICES_GL("NBGestorEscena::privConsumirBufferRenderYEnviarComandosDibujoDeEscenaEnOrdenDirecto(1)")
			NBGestorGL::bindTexture(0, _texturasParaRenderizadoGL[renderCapa->renderMascLucesCombinadas.iTexRender].objTexturaAtlas->idTexturaGL);
			NB_GESTOR_GL_RENDER_TRIANGSTRIP_4(renderCapa->renderMascLucesCombinadas.indicesRenderMascLuz.primerElemento, renderCapa->renderMascLucesCombinadas.indicesRenderMascLuz.cantidadElementos);
			//NBGestorGL::colorMask(true, true, true, true); //En orden directo no es necesario el colormask
			#ifdef NB_GESTOR_ESCENAS_USA_CACHE_LOTE_INSTRUCCIONES
			NBGestorGL::flushCacheLocal();
			#endif
		}
		//PINTAR LUCES QUE APLICAN BRILLO/INTENSIDAD
		if(renderCapa->renderMascLuzLucesConIntensidad->conteo != 0 && renderCapa->renderMascLuzSumaFigurasTotal != 0){
			NBGestorGL::activarVerticesGL(ENVerticeGlTipo_MonoTextura); NBGESTORGL_DBG_NOMBRAR_ACTIVADOR_VERTICES_GL("NBGestorEscena::privConsumirBufferRenderYEnviarComandosDibujoDeEscenaEnOrdenDirecto(2)")
			NBGestorGL::blendFunc(GL_ONE, GL_ONE); //GL_DST_COLOR, GL_ONE //GL_ONE_MINUS_DST_COLOR, GL_ONE);
			UI16* arrIndLucesBrillo		= renderCapa->renderMascLuzLucesConIntensidad->elemento;
			UI16 iIndLuzBrillo, conteoIndLucesBrillo	= renderCapa->renderMascLuzLucesConIntensidad->conteo;
			for(iIndLuzBrillo=0; iIndLuzBrillo<conteoIndLucesBrillo; iIndLuzBrillo++){
				NBASSERT(iIndLuzBrillo<renderCapa->renderMascLuz->conteo)
				STGestorEscenaLuzRender* datosMascLuzIndv = &renderCapa->renderMascLuz->elemento[arrIndLucesBrillo[iIndLuzBrillo]];
				UI16 iFigLuz, iFigLuzSigUlt = datosMascLuzIndv->primerFiguraLuz + datosMascLuzIndv->conteoFigurasLuz;
				for(iFigLuz=datosMascLuzIndv->primerFiguraLuz; iFigLuz<iFigLuzSigUlt; iFigLuz++){
					STGestorEscenaFiguraRender* datosFigLuz = &renderCapa->renderMascLuzBloquesGL->elemento[iFigLuz];
					NBGestorGL::bindTexture(0, datosFigLuz->idTextura0); NBASSERT(datosFigLuz->idTextura0 != 0)
					if(datosFigLuz->esTriangFan){
						NB_GESTOR_GL_RENDER_TRIANGFAN(datosFigLuz->bloqueIndicesGL.primerElemento, datosFigLuz->bloqueIndicesGL.cantidadElementos);
					} else {
						NB_GESTOR_GL_RENDER_TRIANGSTRIP_N(datosFigLuz->bloqueIndicesGL.primerElemento, datosFigLuz->bloqueIndicesGL.cantidadElementos);
					}
				}
				UI16 iFigDecae, iFigDecaeSigUlt = datosMascLuzIndv->primerFiguraDecae + datosMascLuzIndv->conteoFigurasDecae;
				for(iFigDecae=datosMascLuzIndv->primerFiguraDecae; iFigDecae<iFigDecaeSigUlt; iFigDecae++){
					STGestorEscenaFiguraRender* datosFigDecae = &renderCapa->renderMascLuzBloquesGL->elemento[iFigDecae];
					NBGestorGL::bindTexture(0, datosFigDecae->idTextura0); NBASSERT(datosFigDecae->idTextura0 != 0)
					if(datosFigDecae->esTriangFan){
						NB_GESTOR_GL_RENDER_TRIANGFAN(datosFigDecae->bloqueIndicesGL.primerElemento, datosFigDecae->bloqueIndicesGL.cantidadElementos);
					} else {
						NB_GESTOR_GL_RENDER_TRIANGSTRIP_N(datosFigDecae->bloqueIndicesGL.primerElemento, datosFigDecae->bloqueIndicesGL.cantidadElementos);
					}
				}
			}
			#ifdef NB_GESTOR_ESCENAS_USA_CACHE_LOTE_INSTRUCCIONES
			NBGestorGL::flushCacheLocal();
			#endif
		}
		NBGestorGL::blendFuncCargarValoresPorDefecto();
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

/*
 METODO DE PINTADO COMPLEJO:
 - Se pinta primero la capa con mascara,
 - luego en orden inverso las anteriores,
 - finalmente en orden directo las posteriores
 NOTA: esto permite que la iluminacion afecte solamente los elementos
 de la capa en que se encuentra la luz (dejando intactos las capas detras).
*/
void NBGestorEscena::privConsumirBufferRenderYEnviarComandosDibujoDeEscenaPriorizandoUnaCapaConLuz(const SI32 iEscena, bool &frameBufferLimpiado, NBColor &colorFondoLimpiado){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::privConsumirBufferRenderYEnviarComandosDibujoDeEscenaPriorizandoUnaCapaConLuz")
	NBASSERT(_gestorInicializado)
	NBASSERT(_bufferDatosBloqueado[_indiceBufferDatosLeer])
	NBASSERT(iEscena >= 0 && iEscena < NBGESTORESCENA_MAX_ESCENAS);
	STGestorEscenaEscena* scn = &_escenas[iEscena];
	NBASSERT(scn->registroOcupado)
	NBASSERT(scn->escenaHabilitada)
	NBASSERT(scn->pendienteRenderizar)
	STGestorEscenaFrameBuffer* datosFB	= &_frameBuffersGL[scn->iFramebufferEscena];
	bool escenaInicializada				= false;
	NBASSERT(scn->renderCapasConsumir[_indiceBufferDatosLeer] != NULL)
	NBASSERT(scn->renderCapasProducir[_indiceBufferDatosLeer] != NULL)
	NBASSERT(scn->renderCapas[_indiceBufferDatosLeer] != NULL)
	STGestorEscenaCapaRender** arrRenderCapa	= scn->renderCapasConsumir[_indiceBufferDatosLeer]->elemento;
	SI16 iRenderCapa, conteoRenderCapas			= scn->renderCapasConsumir[_indiceBufferDatosLeer]->conteo;
	//Identificar una/la capa con mascara-de-iluminacion
	SI16 iCapaConIluminacion =  -1;
	for(iRenderCapa=0; iRenderCapa<conteoRenderCapas; iRenderCapa++){
		STGestorEscenaCapaRender* renderCapa	= arrRenderCapa[iRenderCapa];
		if(renderCapa->renderMascLucesCombinadas.areaReservada.ancho>0 && renderCapa->renderMascLucesCombinadas.areaReservada.alto>0){
			iCapaConIluminacion = iRenderCapa;
		}
	}
	//Metodo de pintado que incorpora iluminacion a una capa:
	// 1 - Pintar en orden-directo el contenido de la capa iluminada (blend: GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)
	// 2 - Aplicar mascara de iluminacion
	// 3 - Pintar en orden-inverso las capas previas a la iluiminada (blend: GL_ONE_MINUS_DST_ALPHA, GL_DST_ALPHA)
	// 4 - Pintar en orden-directo las capas posteriores a la iluminada (blend: GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)
	if(iCapaConIluminacion != -1){
		STGestorEscenaCapaRender* renderCapa	= arrRenderCapa[iCapaConIluminacion];
		const SI32 conteoModelos = renderCapa->renderModelos->conteo;
		if(conteoModelos != 0){
			NBGestorEscena::privAsegurarFrameBufferInicializadoParaEscena(datosFB, scn, frameBufferLimpiado, colorFondoLimpiado, escenaInicializada);
			NBColor colorMultiplicador; NBCOLOR_ESTABLECER(colorMultiplicador, 1.0f, 1.0f, 1.0f, 1.0);
			NBGestorEscena::privConsumirBufferRenderYEnviarComandosDibujoDeCapa(datosFB, scn, renderCapa, true, colorMultiplicador, frameBufferLimpiado, colorFondoLimpiado, escenaInicializada); //true, en orden directo
		}
		//PINTAR MASCARA DE ILUMINACION
		NBGestorEscena::privAsegurarFrameBufferInicializadoParaEscena(datosFB, scn, frameBufferLimpiado, colorFondoLimpiado, escenaInicializada);
		NBASSERT(renderCapa->renderMascLucesCombinadas.areaReservada.ancho>0 && renderCapa->renderMascLucesCombinadas.areaReservada.alto>0)
		NBASSERT(renderCapa->renderMascLucesCombinadas.indicesRenderMascLuz.cantidadElementos != 0)
		NBASSERT(_texturasParaRenderizadoGL[renderCapa->renderMascLucesCombinadas.iTexRender].objTexturaAtlas != NULL)
		//NBGestorGL::color4f(1.0f, 1.0f, 1.0f, 1.0f);	//Funciona cuando NO-HAY colores definidos en los vertices
		NBGestorGL::lightModelAmbient(1.0f, 1.0f, 1.0f, 1.0f); //Requerido cuando HAY colores definidos en los vertices
		NBGestorGL::matrixMode(GL_PROJECTION);
		NBGestorGL::loadIdentity();
		NBGestorGL::viewport(scn->renderPuertoDeVision.x, scn->renderPuertoDeVision.y, scn->renderPuertoDeVision.ancho, scn->renderPuertoDeVision.alto);
		NBGestorGL::orthof(renderCapa->renderCajaProyeccion.xMin, renderCapa->renderCajaProyeccion.xMax, renderCapa->renderCajaProyeccion.yMin, renderCapa->renderCajaProyeccion.yMax);
		NBGestorGL::matrixMode(GL_MODELVIEW);
		NBGestorGL::loadMatrixf(renderCapa->renderMatrizCapa);
		NBGestorGL::colorMask(true, true, true, false);
		NBGestorGL::blendFunc(GL_ZERO, GL_SRC_COLOR);
		NBGestorGL::activarVerticesGL(ENVerticeGlTipo_MonoTextura); NBGESTORGL_DBG_NOMBRAR_ACTIVADOR_VERTICES_GL("NBGestorEscena::privConsumirBufferRenderYEnviarComandosDibujoDeEscenaPriorizandoUnaCapaConLuz")
		NBGestorGL::bindTexture(0, _texturasParaRenderizadoGL[renderCapa->renderMascLucesCombinadas.iTexRender].objTexturaAtlas->idTexturaGL);
		NB_GESTOR_GL_RENDER_TRIANGSTRIP_4(renderCapa->renderMascLucesCombinadas.indicesRenderMascLuz.primerElemento, renderCapa->renderMascLucesCombinadas.indicesRenderMascLuz.cantidadElementos);
		//PINTAR LUCES QUE APLICAN BRILLO/INTENSIDAD
		if(renderCapa->renderMascLuzLucesConIntensidad->conteo != 0 && renderCapa->renderMascLuzSumaFigurasTotal != 0){
			NBGestorGL::blendFunc(GL_DST_COLOR, GL_ONE); //GL_ONE_MINUS_DST_COLOR, GL_ONE);
			UI16* arrIndLucesBrillo		= renderCapa->renderMascLuzLucesConIntensidad->elemento;
			UI16 iIndLuzBrillo, conteoIndLucesBrillo	= renderCapa->renderMascLuzLucesConIntensidad->conteo;
			for(iIndLuzBrillo=0; iIndLuzBrillo<conteoIndLucesBrillo; iIndLuzBrillo++){
				NBASSERT(iIndLuzBrillo<renderCapa->renderMascLuz->conteo)
				STGestorEscenaLuzRender* datosMascLuzIndv = &renderCapa->renderMascLuz->elemento[arrIndLucesBrillo[iIndLuzBrillo]];
				UI16 iFigLuz, iFigLuzSigUlt = datosMascLuzIndv->primerFiguraLuz + datosMascLuzIndv->conteoFigurasLuz;
				for(iFigLuz=datosMascLuzIndv->primerFiguraLuz; iFigLuz<iFigLuzSigUlt; iFigLuz++){
					STGestorEscenaFiguraRender* datosFigLuz = &renderCapa->renderMascLuzBloquesGL->elemento[iFigLuz];
					NBGestorGL::bindTexture(0, datosFigLuz->idTextura0); NBASSERT(datosFigLuz->idTextura0 != 0)
					if(datosFigLuz->esTriangFan){
						NB_GESTOR_GL_RENDER_TRIANGFAN(datosFigLuz->bloqueIndicesGL.primerElemento, datosFigLuz->bloqueIndicesGL.cantidadElementos);
					} else {
						NB_GESTOR_GL_RENDER_TRIANGSTRIP_N(datosFigLuz->bloqueIndicesGL.primerElemento, datosFigLuz->bloqueIndicesGL.cantidadElementos);
					}
				}
				UI16 iFigDecae, iFigDecaeSigUlt = datosMascLuzIndv->primerFiguraDecae + datosMascLuzIndv->conteoFigurasDecae;
				for(iFigDecae=datosMascLuzIndv->primerFiguraDecae; iFigDecae<iFigDecaeSigUlt; iFigDecae++){
					STGestorEscenaFiguraRender* datosFigDecae = &renderCapa->renderMascLuzBloquesGL->elemento[iFigDecae];
					NBGestorGL::bindTexture(0, datosFigDecae->idTextura0); NBASSERT(datosFigDecae->idTextura0 != 0)
					if(datosFigDecae->esTriangFan){
						NB_GESTOR_GL_RENDER_TRIANGFAN(datosFigDecae->bloqueIndicesGL.primerElemento, datosFigDecae->bloqueIndicesGL.cantidadElementos);
					} else {
						NB_GESTOR_GL_RENDER_TRIANGSTRIP_N(datosFigDecae->bloqueIndicesGL.primerElemento, datosFigDecae->bloqueIndicesGL.cantidadElementos);
					}
				}
			}
		}
		//FIN
		NBGestorGL::colorMask(true, true, true, true);
		NBGestorGL::blendFuncCargarValoresPorDefecto();
		#ifdef NB_GESTOR_ESCENAS_USA_CACHE_LOTE_INSTRUCCIONES
		NBGestorGL::flushCacheLocal();
		#endif
	}
	//PRINTF_INFO("iCapaConIluminacion(%d)\n", iCapaConIluminacion);
	for(iRenderCapa=iCapaConIluminacion - 1; iRenderCapa >= 0; iRenderCapa--){
		STGestorEscenaCapaRender* renderCapa	= arrRenderCapa[iRenderCapa];
		const SI32 conteoModelos = renderCapa->renderModelos->conteo;
		if(conteoModelos != 0){
			NBGestorEscena::privAsegurarFrameBufferInicializadoParaEscena(datosFB, scn, frameBufferLimpiado, colorFondoLimpiado, escenaInicializada);
			NBGestorEscena::privConsumirBufferRenderYEnviarComandosDibujoDeCapa(datosFB, scn, renderCapa, false, renderCapa->renderColorLuzAmbiente, frameBufferLimpiado, colorFondoLimpiado, escenaInicializada); //false, en orden inverso
		}
	}
	for(iRenderCapa=iCapaConIluminacion+1; iRenderCapa<conteoRenderCapas; iRenderCapa++){
		STGestorEscenaCapaRender* renderCapa	= arrRenderCapa[iRenderCapa];
		const SI32 conteoModelos = renderCapa->renderModelos->conteo;
		if(conteoModelos != 0){
			NBGestorEscena::privAsegurarFrameBufferInicializadoParaEscena(datosFB, scn, frameBufferLimpiado, colorFondoLimpiado, escenaInicializada);
			NBGestorEscena::privConsumirBufferRenderYEnviarComandosDibujoDeCapa(datosFB, scn, renderCapa, true, renderCapa->renderColorLuzAmbiente, frameBufferLimpiado, colorFondoLimpiado, escenaInicializada); //true, en orden directo
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

//------------------------------------------
//- Touches
//------------------------------------------
void NBGestorEscena::touchesProcesar(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::touchesProcesar")
	//NBASSERT(!_bufferDatosBloqueado[_indiceBufferDatosEscribir]);
	NBASSERT(_gestorInicializado);
	NBGestorTouches::mutexActivar();
	//
	const SI32 touchesTam	= NBGestorTouches::touchesArregloTamano();
	STGTouchPriv* touches	= NBGestorTouches::touchesArreglo();
	const SI32 hoversTam	= NBGestorTouches::hoverArregloTamano();
	STGHoverPriv* hovers	= NBGestorTouches::hoverArreglo();
	// Movimientos
	SI32 iHover;
	for(iHover = 0; iHover < hoversTam; iHover++){
		STGHoverPriv* hover = &hovers[iHover];
		if(hover->registroOcupado){
			//Search on touches
			SI32 iTouch; STGTouchPriv* touchActivo = NULL;
			for(iTouch = 0; iTouch < touchesTam; iTouch++){
				STGTouchPriv* touch = &touches[iTouch];
				if(touch->registroOcupado && touch->identificadorEnOS == hover->identificadorEnOS){
					touchActivo = touch;
					break;
				}
			}
			if(touchActivo == NULL){
				NBPunto posEnEscena; AUObjeto* consumidorEscuchadorObj = NULL; void* consumidorEscuchadorInterfaz = NULL;
				AUEscenaObjeto* consumidor = NBGestorEscena::touchObjetoQueConsume(hover->posSuperficieAct.x, hover->posSuperficieAct.y, AUOBJETOESCENA_TOUCH_BIT_LISTEN_TOUCH, &posEnEscena, &consumidorEscuchadorObj, &consumidorEscuchadorInterfaz);
				NBASSERT((consumidor == NULL && consumidorEscuchadorObj == NULL && consumidorEscuchadorInterfaz == NULL) || (consumidor != NULL && consumidorEscuchadorObj != NULL && consumidorEscuchadorInterfaz != NULL))
				if(hover->consumidor != consumidor){
					//Retain new objects
					if(consumidor != NULL){ consumidor->retener(NB_RETENEDOR_NULL); }
					if(consumidorEscuchadorObj != NULL){ consumidorEscuchadorObj->retener(NB_RETENEDOR_NULL); }
					//Release old objects
					if(hover->consumidor != NULL){
						((AUEscenaObjeto*)hover->consumidor)->hoverFinalizado(hover->consumidorEscInterfaz);
						hover->consumidor->liberar(NB_RETENEDOR_NULL);
					}
					if(hover->consumidorEscObjeto != NULL){
						hover->consumidorEscObjeto->liberar(NB_RETENEDOR_NULL);
					}
					//Set objects (already retained)
					hover->consumidor			= consumidor;
					hover->consumidorEscObjeto	= consumidorEscuchadorObj;
					hover->consumidorEscInterfaz= consumidorEscuchadorInterfaz;
					//Push-event
					if(hover->consumidor != NULL){
						((AUEscenaObjeto*)hover->consumidor)->hoverIniciado(hover->consumidorEscInterfaz);
					}
				}
				if(hover->consumidor != NULL){
					((AUEscenaObjeto*)hover->consumidor)->hoverMovido(hover->consumidorEscInterfaz, posEnEscena);
				}
			}
		}
	}
	// Todo: analizar el objeto que esta sobre el hover, priorizar el touch activo con el mismo idAtOS.
	// De esta forma los mouseEntered, mouseMoved y mouseExit se disparan cuando el mouse se mueve o cuando los objetos en escena se moueven.
	//
	//Nuevos touches
	SI32 iTouch;
	for(iTouch = 0; iTouch < touchesTam; iTouch++){
		STGTouchPriv* touch = &touches[iTouch];
		if(touch->registroOcupado){
			if(touch->ticksAcumulados == 0){
				NBPunto posEnEscena; AUObjeto* consumidorEscuchadorObj = NULL; void* consumidorEscuchadorInterfaz = NULL;
				AUEscenaObjeto* consumidor = NBGestorEscena::touchObjetoQueConsume(touch->t.posSuperficieIni.x, touch->t.posSuperficieIni.y, AUOBJETOESCENA_TOUCH_BIT_LISTEN_TOUCH, &posEnEscena, &consumidorEscuchadorObj, &consumidorEscuchadorInterfaz);
				NBASSERT((consumidor == NULL && consumidorEscuchadorObj == NULL && consumidorEscuchadorInterfaz == NULL) || (consumidor != NULL && consumidorEscuchadorObj != NULL && consumidorEscuchadorInterfaz != NULL))
				//
				NBGestorTeclas::escuchadorEvaluarMantenerAnteNuevoTouch(consumidor);
				//
				if(consumidor != NULL){
					NBGestorTouches::touchEstablecerConsumidor(&touch->t, consumidor, consumidorEscuchadorObj, consumidorEscuchadorInterfaz);
					//Set itf (to receive notifications when the touch-owner leaves the screen)
					{
						//ToDo: also implement with touch-hovers
						STNBEscenaObjetoMngrTouchItf itf;
						NBMemory_setZeroSt(itf, STNBEscenaObjetoMngrTouchItf);
						itf.objWillBeRemovedFromSceneFunc = NBGestorEscena::touchesOwnerWillBeRemovedFromScene;
						consumidor->setMngrTouchesItf(&itf);
					}
					//Notify
					{
						consumidor->touchIniciado(consumidorEscuchadorInterfaz, &touch->t, posEnEscena);
					}
					//PRINTF_INFO("Touch iniciado %d consumido en (%f, %f)\n", touches->elemento[iTouch].identificadorEnOS, posicionTouch.x, posicionTouch.y);
				}
				//
				//if(consumidor == NULL)  PRINTF_INFO("Touch iniciado %d no fue consumido posPuertoTouch(%d, %d)\n", touch->identificadorEnOS, touch->posicionInicial.x, touch->posicionInicial.y);
			}
		}
	}
	//Touches movidos o finalizados
	for(iTouch = 0; iTouch < touchesTam; iTouch++){
		STGTouchPriv* touch = &touches[iTouch];
		if(touch->registroOcupado){
			AUEscenaObjeto* consumidor	= (AUEscenaObjeto*)touch->consumidor;
			void* consumidorEscInterfaz = touch->consumidorEscInterfaz;
			if(consumidor != NULL && (touch->movsAccumAtTick > 0 || touch->marcadoComoFinalizado)){
				NBASSERT(consumidor->idEscena != -1); //Si falla, eL consumidor del touch fue quitado de escena antes que terminase el touch
				if(consumidor->idEscena >= 0){
					STGestorEscenaEscena* scn		= &(_escenas[(SI32)consumidor->idEscena]);
					NBRectangulo puertoDeVision		= scn->puertoDeVision;
					NBCajaAABB cajaProyeccion		= scn->gruposCapas[(SI32)consumidor->idGrupo].cajaProyeccion;
					NBTamano tamCajaProyeccion;		NBCAJAAABB_TAMANO(tamCajaProyeccion, cajaProyeccion);
					NBTamano unidsGLPorPx;			NBTAMANO_ESTABLECER(unidsGLPorPx, tamCajaProyeccion.ancho/puertoDeVision.ancho, tamCajaProyeccion.alto/puertoDeVision.alto);
					NBPunto posicionInicial;		NBPUNTO_ESTABLECER(posicionInicial, cajaProyeccion.xMin + (touch->t.posSuperficieIni.x * unidsGLPorPx.ancho), cajaProyeccion.yMax - (touch->t.posSuperficieIni.y * unidsGLPorPx.alto));
					NBPunto posicionAnterior;		NBPUNTO_ESTABLECER(posicionAnterior, cajaProyeccion.xMin + (touch->t.posSuperficieAnt.x * unidsGLPorPx.ancho), cajaProyeccion.yMax - (touch->t.posSuperficieAnt.y * unidsGLPorPx.alto));
					NBPunto posicionActual;			NBPUNTO_ESTABLECER(posicionActual, cajaProyeccion.xMin + (touch->t.posSuperficieAct.x * unidsGLPorPx.ancho), cajaProyeccion.yMax - (touch->t.posSuperficieAct.y * unidsGLPorPx.alto));
					//Populate missed scene pos
					{
						NBArray_empty(&touch->t.posEscenaMissed);
						if(touch->t.posSuperficieMissed.use > 0){
							SI32 i; for(i = 0; i < touch->t.posSuperficieMissed.use; i++){
								const NBPuntoI pViewPort = NBArray_itmValueAtIndex(&touch->t.posSuperficieMissed, NBPuntoI, i);
								NBPuntoF pScn; NBPUNTO_ESTABLECER(pScn, cajaProyeccion.xMin + (pViewPort.x * unidsGLPorPx.ancho), cajaProyeccion.yMax - (pViewPort.y * unidsGLPorPx.alto));
								NBArray_addValue(&touch->t.posEscenaMissed, pScn);
							}
						}
					}
					if(touch->marcadoComoFinalizado){
						consumidor->touchFinalizado(consumidorEscInterfaz, &(touch->t), posicionInicial, posicionAnterior, posicionActual);
						//PRINTF_INFO("Touch %d finalizado (%f, %f)\n", touch->identificadorEnOS, posicionActual.x, posicionActual.y);
					} else if(touch->movsAccumAtTick){
						consumidor->touchMovido(consumidorEscInterfaz, &(touch->t), posicionInicial, posicionAnterior, posicionActual);
						//PRINTF_INFO("Touch %d movido (%f, %f)\n", touch->identificadorEnOS, posicionActual.x, posicionActual.y);
					}
				}
			}
		}
	}
	//Consumir los estados de los touches
	NBGestorTouches::touchConsumirEstados();
	//Scrolls
	{
		UI32 count = 0;
		const STGTouchScroll* pend = NBGestorTouches::scrollsPend(&count);
		if(pend != NULL && count > 0){
			SI32 i; for(i = 0; i < count; i++){
				const STGTouchScroll* s = &pend[i];
				{
					NBPunto posAtScn; AUObjeto* obj = NULL; void* itf = NULL;
					AUEscenaObjeto* consumer = NBGestorEscena::touchObjetoQueConsume(s->posPort.x, s->posPort.y, AUOBJETOESCENA_TOUCH_BIT_LISTEN_SCROLL, &posAtScn, &obj, &itf);
					NBASSERT((consumer == NULL && obj == NULL && itf == NULL) || (consumer != NULL && obj != NULL && itf != NULL))
					if(consumer != NULL && obj != NULL && itf != NULL){
						if(consumer->idEscena >= 0){
							const NBTamano szScn = tamanoPuntosAEscena(consumer->idEscena, s->size.width, s->size.height);
							//const float wScn = NBGestorEscena::tamanoPuntosAEscena();
							//const float hScn = NBGestorEscena::altoPuertoAGrupo(consumer->idEscena, (ENGestorEscenaGrupo)consumer->idGrupo, s->size.height);
							consumer->touchScrollApply(itf, NBST_P(STNBPoint, posAtScn.x, posAtScn.y ), NBST_P(STNBSize, szScn.ancho, szScn.alto ), s->animate);
						}
					}
				}
			}
			NBGestorTouches::scrollsPendClear();
		}
	}
	//Magnifications
	{
		UI32 count = 0;
		const STGTouchMagnify* pend = NBGestorTouches::magnifyPend(&count);
		if(pend != NULL && count > 0){
			SI32 i; for(i = 0; i < count; i++){
				const STGTouchMagnify* s = &pend[i];
				{
					NBPunto posAtScn; AUObjeto* obj = NULL; void* itf = NULL;
					AUEscenaObjeto* consumer = NBGestorEscena::touchObjetoQueConsume(s->posPort.x, s->posPort.y, AUOBJETOESCENA_TOUCH_BIT_LISTEN_MAGNIFY, &posAtScn, &obj, &itf);
					NBASSERT((consumer == NULL && obj == NULL && itf == NULL) || (consumer != NULL && obj != NULL && itf != NULL))
					if(consumer != NULL && obj != NULL && itf != NULL){
						if(consumer->idEscena >= 0){
							consumer->touchMagnifyApply(itf, NBST_P(STNBPoint, posAtScn.x, posAtScn.y ), s->magnification, s->isSmarthMag);
						}
					}
				}
			}
			NBGestorTouches::magnifyPendClear();
		}
	}
	//
	NBGestorTouches::mutexDesactivar();
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

void NBGestorEscena::touchesOwnerWillBeRemovedFromScene(AUEscenaObjeto* obj, const SI32 touchesOwnedCount, void* param){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::touchesOwnerWillBeRemovedFromScene")
	NBASSERT(obj != NULL)
	if(obj != NULL){
		PRINTF_INFO("NBGestorEscena, touch-owner(%lld) releasing %d touches before leaving the scene.\n", (SI64)obj, touchesOwnedCount)
		//ToDo: also implement with touch-hovers
		const SI32 touchesTam	= NBGestorTouches::touchesArregloTamano();
		STGTouchPriv* touches	= NBGestorTouches::touchesArreglo();
		//Touches movidos o finalizados
		SI32 iTouch, fndCount = 0;
		for(iTouch = 0; iTouch < touchesTam; iTouch++){
			STGTouchPriv* touch = &touches[iTouch];
			if(touch->registroOcupado){
				AUEscenaObjeto* consumidor		= (AUEscenaObjeto*)touch->consumidor;
				if(consumidor == obj){
					NBASSERT(consumidor->idEscena != -1); //Si falla, eL consumidor del touch fue quitado de escena antes que terminase el touch
					if(consumidor->idEscena >= 0){
						STGestorEscenaEscena* scn		= &(_escenas[(SI32)consumidor->idEscena]);
						NBRectangulo puertoDeVision			= scn->puertoDeVision;
						NBCajaAABB cajaProyeccion			= scn->gruposCapas[(SI32)consumidor->idGrupo].cajaProyeccion;
						NBTamano tamCajaProyeccion;			NBCAJAAABB_TAMANO(tamCajaProyeccion, cajaProyeccion);
						NBTamano unidsGLPorPx;				NBTAMANO_ESTABLECER(unidsGLPorPx, tamCajaProyeccion.ancho/puertoDeVision.ancho, tamCajaProyeccion.alto/puertoDeVision.alto);
						NBPunto posicionInicial;			NBPUNTO_ESTABLECER(posicionInicial, cajaProyeccion.xMin + (touch->t.posSuperficieIni.x * unidsGLPorPx.ancho), cajaProyeccion.yMax - (touch->t.posSuperficieIni.y * unidsGLPorPx.alto));
						NBPunto posicionAnterior;			NBPUNTO_ESTABLECER(posicionAnterior, cajaProyeccion.xMin + (touch->t.posSuperficieAnt.x * unidsGLPorPx.ancho), cajaProyeccion.yMax - (touch->t.posSuperficieAnt.y * unidsGLPorPx.alto));
						NBPunto posicionActual;				NBPUNTO_ESTABLECER(posicionActual, cajaProyeccion.xMin + (touch->t.posSuperficieAct.x * unidsGLPorPx.ancho), cajaProyeccion.yMax - (touch->t.posSuperficieAct.y * unidsGLPorPx.alto));
						{
							//Cancel touch
							touch->t.cancelado = TRUE;
							PRINTF_INFO("NBGestorEscena, touch-owner(%lld) releasing-touch-owned.\n", (SI64)obj);
							consumidor->touchFinalizado(touch->consumidorEscInterfaz, &(touch->t), posicionInicial, posicionAnterior, posicionActual);
							NBGestorTouches::touchEstablecerConsumidor(&touch->t, NULL, NULL, NULL);
							//PRINTF_INFO("Touch %d finalizado (%f, %f)\n", touch->identificadorEnOS, posicionActual.x, posicionActual.y);
							fndCount++;
						}
					}
				}
			}
		}
		NBASSERT(touchesOwnedCount == fndCount)
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}

AUEscenaObjeto* NBGestorEscena::touchObjetoQueConsume(const SI32 posXPuerto, const SI32 posYPuerto, const UI8 touchFilterMask, NBPunto* outPosEscena, AUObjeto** outConsumidorEscuchadorObj, void** outConsumidorEscuchadorInterfaz){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::touchObjetoQueConsume")
	AUEscenaObjeto* r = NULL;
	SI32 iEscena;
	for(iEscena = (NBGESTORESCENA_MAX_ESCENAS - 1); iEscena >= 0 && r == NULL; iEscena--){
		STGestorEscenaEscena* scn = &_escenas[iEscena];
		if(scn->registroOcupado && scn->escenaHabilitada){
			STGestorEscenaEscena* scn	= &(_escenas[iEscena]);
			NBRectangulo puertoDeVision	= scn->puertoDeVision;
			SI32 iGrupo;
			for(iGrupo = ENGestorEscenaGrupo_Conteo - 1; iGrupo >= 0 && r == NULL; iGrupo--){
				STGestorEscenaGrupo* grupoCapa	= &(scn->gruposCapas[iGrupo]);
				NBCajaAABB cajaProyeccion		= grupoCapa->cajaProyeccion;
				NBTamano tamCajaProyeccion;		NBCAJAAABB_TAMANO(tamCajaProyeccion, cajaProyeccion);
				NBTamano unidsGLPorPx;			NBTAMANO_ESTABLECER(unidsGLPorPx, tamCajaProyeccion.ancho/puertoDeVision.ancho, tamCajaProyeccion.alto/puertoDeVision.alto);
				NBPunto posicionTouch;			NBPUNTO_ESTABLECER(posicionTouch, cajaProyeccion.xMin + (posXPuerto * unidsGLPorPx.ancho), cajaProyeccion.yMax - (posYPuerto * unidsGLPorPx.alto));
				int iProp;
				for(iProp=grupoCapa->capas->conteo - 1; iProp >= 0 && r == NULL; iProp--){
					STGestorEscenaCapa* propiedadesDeCapa = &(grupoCapa->capas->elemento[iProp]);
					if(propiedadesDeCapa->habilitada && propiedadesDeCapa->objEscena->visible()){
						r = propiedadesDeCapa->objEscena->consumidorTouchEnPosicion(posicionTouch, touchFilterMask, outConsumidorEscuchadorObj, outConsumidorEscuchadorInterfaz);
						if(outPosEscena != NULL){
							*outPosEscena = posicionTouch;
						}
					}
				}
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
	return r;
}

AUEscenaObjeto* NBGestorEscena::touchObjetoQueConsumeSegunArreglo(const SI32 posXPuerto, const SI32 posYPuerto, const UI8 touchFilterMask, const AUArreglo* arregloFiltroObjetosEscena){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::touchObjetoQueConsumeSegunArreglo")
	NBASSERT(_gestorInicializado);
	NBASSERT(!_bufferDatosBloqueado[_indiceBufferDatosEscribir]);
	AUEscenaObjeto* r = NULL;
	SI32 iEscena;
	for(iEscena = (NBGESTORESCENA_MAX_ESCENAS - 1); iEscena >= 0 && r == NULL; iEscena--){
		STGestorEscenaEscena* scn = &_escenas[iEscena];
		if(scn->registroOcupado && scn->escenaHabilitada){
			STGestorEscenaEscena* scn		= &(_escenas[iEscena]);
			NBRectangulo puertoDeVision			= scn->puertoDeVision;
			SI32 iGrupo;
			for(iGrupo = ENGestorEscenaGrupo_Conteo - 1; iGrupo >= 0 && r == NULL; iGrupo--){
				NBCajaAABB cajaProyeccion				= scn->gruposCapas[iGrupo].cajaProyeccion;
				NBTamano tamCajaProyeccion;				NBCAJAAABB_TAMANO(tamCajaProyeccion, cajaProyeccion);
				NBTamano unidsGLPorPx;					NBTAMANO_ESTABLECER(unidsGLPorPx, tamCajaProyeccion.ancho/puertoDeVision.ancho, tamCajaProyeccion.alto/puertoDeVision.alto);
				NBPunto posicionTouch;					NBPUNTO_ESTABLECER(posicionTouch, cajaProyeccion.xMin + (posXPuerto * unidsGLPorPx.ancho), cajaProyeccion.yMax - (posYPuerto * unidsGLPorPx.alto));
				STGestorEscenaGrupo* grupoCapa			= &(scn->gruposCapas[iGrupo]);
				int iProp;
				for(iProp=grupoCapa->capas->conteo - 1; iProp >= 0 && r == NULL; iProp--){
					STGestorEscenaCapa* propiedadesDeCapa = &(grupoCapa->capas->elemento[iProp]);
					if(propiedadesDeCapa->habilitada && propiedadesDeCapa->objEscena->visible()){
						r = propiedadesDeCapa->objEscena->consumidorTouchEnPosicionSegunArreglo(posicionTouch, touchFilterMask, arregloFiltroObjetosEscena);
					}
				}
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
	return r;
}

#ifdef CONFIG_NB_RECOPILAR_ESTADISTICAS_DE_GESTION_MEMORIA
UI32 NBGestorEscena::debugBytesDeRenderBuffers(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::debugBytesDeRenderBuffers")
	UI32 bytesRenderBuffers = 0;
	SI32 iFrameb;
	for(iFrameb=0; iFrameb<NBGESTORESCENA_MAX_ESCENAS; iFrameb++){
		if(_frameBuffersGL[iFrameb].registroOcupado){
			if(_frameBuffersGL[iFrameb].tipoDestinoGL == ENGestorEscenaDestinoGl_RenderBuffer){
				NBTamano tamano		= _frameBuffersGL[iFrameb].renderbufferOriginal->tamanoDestinoGl();
				UI32 bytesPorPixel	= NBGestorTexturas::bytesPorPixel(_frameBuffersGL[iFrameb].colorFrameBufferGl);
				bytesRenderBuffers	+= ((UI32)tamano.ancho * (UI32)tamano.alto * bytesPorPixel);
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
	return bytesRenderBuffers;
}
#endif

/*void NBGestorEscena::debug_verificaNivelEnEscena(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS("NBGestorEscena::debug_verificaNivelEnEscena")
	_contenedorEscena->debug_verificaNivelEnEscena(1);
	_contenedorGUI->debug_verificaNivelEnEscena(1);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS
}*/


