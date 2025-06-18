//
//  AUEscenaIluminacion.cpp
//  Gameplay_iOS
//
//  Created by Marcos Ortega on 25/09/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkMediaStdAfx.h"
#include "AUEscenaIluminacion.h"

AUEscenaIluminacion::AUEscenaIluminacion(AUCuerpoDef* definicionCuerpo) : AUEscenaObjeto() {
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaIluminacion::AUEscenaIluminacion")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaIluminacion")
	NBASSERT(definicionCuerpo != NULL)
	_definicionCuerpo				= definicionCuerpo; _definicionCuerpo->retener(NB_RETENEDOR_THIS);
	#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
	UI16 iMasc, conteoMasc = _definicionCuerpo->mascarasIluminacion.conteo;
	for(iMasc=0; iMasc<conteoMasc; iMasc++){
		STIlumMascara mascaraIlum = _definicionCuerpo->mascarasIluminacion.elemento[iMasc];
		NBSegmentadorFiguras::figuraEsConvexaContrareloj(mascaraIlum.cacheVerticesCuerpo, 4);
	}
	#endif
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

AUEscenaIluminacion::AUEscenaIluminacion(AUEscenaIluminacion* otroCuerpo) : AUEscenaObjeto() {
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaIluminacion::AUEscenaIluminacion")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaIluminacion")
	_definicionCuerpo				= otroCuerpo->_definicionCuerpo; _definicionCuerpo->retener(NB_RETENEDOR_THIS);
	#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
	UI16 iMasc, conteoMasc = _definicionCuerpo->mascarasIluminacion.conteo;
	for(iMasc=0; iMasc<conteoMasc; iMasc++){
		STIlumMascara mascaraIlum = _definicionCuerpo->mascarasIluminacion.elemento[iMasc];
		NBSegmentadorFiguras::figuraEsConvexaContrareloj(mascaraIlum.cacheVerticesCuerpo, 4);
	}
	#endif
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

AUEscenaIluminacion::~AUEscenaIluminacion(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaIluminacion::~AUEscenaIluminacion")
	if(_definicionCuerpo != NULL) { _definicionCuerpo->liberar(NB_RETENEDOR_THIS); _definicionCuerpo = NULL; }
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaIluminacion::agregadoEnEscena(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaIluminacion::agregadoEnEscena")
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaIluminacion::quitandoDeEscena(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaIluminacion::quitandoDeEscena")
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

NBCajaAABB AUEscenaIluminacion::cajaAABBLocalCalculada(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaIluminacion::cajaAABBLocalCalculada")
	NBASSERT(_definicionCuerpo != NULL)
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _definicionCuerpo->cajaLocalMascarasIlum;
}

PTRfunCmdsGL AUEscenaIluminacion::actualizarModeloGL(const NBPropRenderizado &propsRenderizado, const NBPropIluminacion &propsIluminacion, const NBPropHeredadasRender &propsHeredadas){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaIluminacion::actualizarModeloGL")
	NBASSERT(this->idEscena!=-1)
	//Convertir a figuras GL
	/*STIluminacionRender datosRender;
	datosRender.conteoFigurasGL = 0;
	#if defined(CONFIG_NB_ILUMINACION_DIBUJAR_CAJA_MASCARAS) || defined(CONFIG_NB_ILUMINACION_DIBUJAR_PORCIONES_ILUMINADAS_MASCARAS) || defined(CONFIG_NB_ILUMINACION_DIBUJAR_SOMBRAS_PROYECTADAS)
	datosRender.conteoFigurasGLDbg = 0;
	#endif
	//Para evitar redundancia de texturas en la piscina de retencion
	AUTextura* texturasRegistradas[8];  SI32 conteoTexturasRegistradas = 0;
	texturasRegistradas[0] = NULL; texturasRegistradas[1] = NULL; texturasRegistradas[2] = NULL; texturasRegistradas[3] = NULL;
	texturasRegistradas[4] = NULL; texturasRegistradas[5] = NULL; texturasRegistradas[6] = NULL; texturasRegistradas[7] = NULL;
	//
	if(_definicionCuerpo->cajaLocalMascarasIlum.xMin!=_definicionCuerpo->cajaLocalMascarasIlum.xMax && _definicionCuerpo->cajaLocalMascarasIlum.yMin!=_definicionCuerpo->cajaLocalMascarasIlum.yMax){
		NBMatriz matrizEscenaInversa; NBMATRIZ_INVERSA(matrizEscenaInversa, _cacheObjEscena.matrizEscena);
		NBCajaAABB cajaEscenaTodasMascaras; NBCAJAAABB_INICIALIZAR(cajaEscenaTodasMascaras)
		//Recorrer mascaras de iluminacion
		SI32 iMasc;
		for(iMasc=0; iMasc<_definicionCuerpo->mascarasIluminacion.conteo; iMasc++){
			STIlumMascara* datosMascaraIlum = &(_definicionCuerpo->mascarasIluminacion.elemento[iMasc]);
			NBASSERT(datosMascaraIlum->texturaMascara != NULL)
			STBloqueGL bloqueMasc;
			bloqueMasc.cantidadElementos	= 4;
			bloqueMasc.primerElemento		= (*propsRenderizado.funcReservarVertices)(ENVerticeGlTipo_TriTextura, bloqueMasc.cantidadElementos);
			NBVerticeGL3* verticesGL		= &(*propsRenderizado.verticesGL3)[bloqueMasc.primerElemento];
			//Determinar coordenadas en escena de la mascara y color de inicializacion de iluminacion
			NBMATRIZ_MULTIPLICAR_PUNTO(verticesGL[0], _cacheObjEscena.matrizEscena, datosMascaraIlum->cacheVerticesCuerpo[0].x, datosMascaraIlum->cacheVerticesCuerpo[0].y);
			NBMATRIZ_MULTIPLICAR_PUNTO(verticesGL[1], _cacheObjEscena.matrizEscena, datosMascaraIlum->cacheVerticesCuerpo[1].x, datosMascaraIlum->cacheVerticesCuerpo[1].y);
			NBMATRIZ_MULTIPLICAR_PUNTO(verticesGL[2], _cacheObjEscena.matrizEscena, datosMascaraIlum->cacheVerticesCuerpo[2].x, datosMascaraIlum->cacheVerticesCuerpo[2].y);
			NBMATRIZ_MULTIPLICAR_PUNTO(verticesGL[3], _cacheObjEscena.matrizEscena, datosMascaraIlum->cacheVerticesCuerpo[3].x, datosMascaraIlum->cacheVerticesCuerpo[3].y);
			NBCOLOR_ESTABLECER(verticesGL[0], 255, 255, 255, 255)
			NBCOLOR_ESTABLECER(verticesGL[1], 255, 255, 255, 255)
			NBCOLOR_ESTABLECER(verticesGL[2], 255, 255, 255, 255)
			NBCOLOR_ESTABLECER(verticesGL[3], 255, 255, 255, 255)
			datosMascaraIlum->texturaMascara->cargarCoordenadasTextura2(verticesGL, 0);
			NBCAJAAABB_ENVOLVER_PUNTOS(cajaEscenaTodasMascaras, verticesGL, 4);
		}
	}*/
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return NULL;
}

bool AUEscenaIluminacion::privConfigurarGestorGlParaMascaras(){
	//AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaIluminacion::privConfigurarGestorGlParaMascaras")
	NBGestorGL::activarVerticesGL(ENVerticeGlTipo_TriTextura); NBGESTORGL_DBG_NOMBRAR_ACTIVADOR_VERTICES_GL("AUEscenaIluminacion")
	//////COMENTRIADO TEMPORAL///// NBGestorGL::blendFunc(GL_DST_COLOR, GL_ONE);
	//NBGestorGL::blendFunc(GL_SRC_COLOR, GL_ONE);					//Las partes negras de la mascara no se pintan, las blancas aclaran. "PERFECTCO para solo iluminar!"
	//NBGestorGL::blendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	//Esta es la normal
	//NBGestorGL::blendFunc(GL_ZERO, GL_ZERO);						//pinta todo negro
	//NBGestorGL::blendFunc(GL_ZERO, GL_ONE);						//source: ignora lo que se pinta encima
	//NBGestorGL::blendFunc(GL_ZERO, GL_SRC_COLOR);					//source: pinta de negro lo transparente, pero si es solido ignora lo blanco y oscurece lo negro (efecto sombra, OK pero solo oscurece, no aclara)
	//NBGestorGL::blendFunc(GL_ZERO, GL_ONE_MINUS_SRC_COLOR);		//Source: no pinta lo transparente, pero si es solido ignora lo negro y oscurece lo blanco (efecto de sombra invertido)
	//NBGestorGL::blendFunc(GL_ONE, GL_ZERO);						//Pinta de negro lo negro, y de color-GL lo blanco
	//NBGestorGL::blendFunc(GL_ONE, GL_ONE);
	//NBGestorGL::blendFunc(GL_ONE, GL_SRC_COLOR);
	//NBGestorGL::blendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
	//NBGestorGL::blendFunc(GL_DST_COLOR, GL_ZERO);
	//NBGestorGL::blendFunc(GL_DST_COLOR, GL_ONE);					//Source: mantiene el colore original en lo negro, y aclarece lo blanco (efecto iluminacion pero sin oscurecer)
	//NBGestorGL::blendFunc(GL_DST_COLOR, GL_SRC_COLOR);			//Soruce: oscurece, aclara brinda tono de color-GL (el negro se oscurece totalmente) (efecto sombra, OK pero deben mantenerse tonos grises)
	//NBGestorGL::blendFunc(GL_DST_COLOR, GL_ONE_MINUS_SRC_COLOR);	//Source: ignora lo que se pinta encima
	//NBGestorGL::blendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO);		//Source: Invierte el tono de lo claro que se pinta encima (no soporta transparencia)
	//NBGestorGL::blendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE);		//Source: aclara pero no oscurece
	//NBGestorGL::blendFunc(GL_ONE_MINUS_DST_COLOR, GL_SRC_COLOR);	//Source: pinta solido lo de encima
	//NBGestorGL::blendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_SRC_COLOR);
	//AU_GESTOR_PILA_LLAMADAS_POP_3
	return true;
}

void AUEscenaIluminacion::enviarComandosGL(UI8* punteroDatosModelo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaIluminacion::enviarComandosGL")
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

//
AUOBJMETODOS_CLASESID_MULTICLASE(AUEscenaIluminacion, AUEscenaObjeto)
AUOBJMETODOS_CLASESNOMBRES_MULTICLASE(AUEscenaIluminacion, "AUEscenaIluminacion", AUEscenaObjeto)
AUOBJMETODOS_CLONAR_THIS(AUEscenaIluminacion)








