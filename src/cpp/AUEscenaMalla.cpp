//
//  AUEscenaMalla.cpp
//  Gameplay_Mac
//
//  Created by Nicaragua Binary on 24/01/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkMediaStdAfx.h"
#include "AUEscenaMalla.h"

AUEscenaMalla::AUEscenaMalla() : AUEscenaObjeto(), _verticesLocales(this), _texturas(this), _areasTexturas(this) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMalla::AUEscenaMalla")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaMalla")
	_columnas					= 0;
	_filas						= 0;
	_esMascaraAlpha				= false;
	//
	_mascaraCacheSuciasMalla	= 0xFF;
	NBCAJAAABB_INICIALIZAR(_cacheCajaLocalCalculada);
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUEscenaMalla::AUEscenaMalla(UI8 columnas, UI8 filas, AUArregloNativoP<NBPuntoColor>* verticesLocales, AUArregloNativoP<AUTextura*>* texturas, AUArregloNativoP<NBRectangulo>* areasTex) : AUEscenaObjeto(), _verticesLocales(this, verticesLocales), _texturas(this, texturas), _areasTexturas(this, areasTex) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMalla::AUEscenaMalla")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaMalla")
	_columnas					= columnas;
	_filas						= filas;
	_esMascaraAlpha				= false;
	//Retener todas las texturas
	UI32 iTex, iTexConteo = _texturas.conteo;
	for(iTex=0; iTex<iTexConteo; iTex++){
		if(_texturas.elemento[iTex] != NULL) _texturas.elemento[iTex]->retener(NB_RETENEDOR_THIS);
	}
	//NBASSERT(_texturas.conteo<=1)
	_mascaraCacheSuciasMalla	= 0xFF;
	NBCAJAAABB_INICIALIZAR(_cacheCajaLocalCalculada);
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUEscenaMalla::AUEscenaMalla(UI8 columnas, UI8 filas, NBPuntoColor* verticesLocales, UI16 conteoVertices, AUTextura** texturas, UI16 conteoTexturas, NBRectangulo* areasTex, UI16 conteoAreas) : AUEscenaObjeto(), _verticesLocales(this, verticesLocales, conteoVertices), _texturas(this, texturas, conteoTexturas), _areasTexturas(this, areasTex, conteoAreas) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMalla::AUEscenaMalla")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaMalla")
	_columnas					= columnas;
	_filas						= filas;
	_esMascaraAlpha				= false;
	//Retener todas las texturas
	UI32 iTex, iTexConteo = _texturas.conteo;
	for(iTex=0; iTex<iTexConteo; iTex++){
		if(_texturas.elemento[iTex] != NULL) _texturas.elemento[iTex]->retener(NB_RETENEDOR_THIS);
	}
	//NBASSERT(_texturas.conteo<=1)
	_mascaraCacheSuciasMalla	= 0xFF;
	NBCAJAAABB_INICIALIZAR(_cacheCajaLocalCalculada);
	AU_GESTOR_PILA_LLAMADAS_POP_2
}


AUEscenaMalla::AUEscenaMalla(AUEscenaMalla* otraInstancia) : AUEscenaObjeto(otraInstancia), _verticesLocales(this, &otraInstancia->_verticesLocales), _texturas(this, &otraInstancia->_texturas), _areasTexturas(this, &otraInstancia->_areasTexturas) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMalla::AUEscenaMalla")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaMalla")
	_columnas					= otraInstancia->_columnas;
	_filas						= otraInstancia->_filas;
	_esMascaraAlpha				= otraInstancia->_esMascaraAlpha;
	//Retener todas las texturas
	UI32 iTex, iTexConteo = _texturas.conteo;
	for(iTex=0; iTex<iTexConteo; iTex++){
		if(_texturas.elemento[iTex] != NULL) _texturas.elemento[iTex]->retener(NB_RETENEDOR_THIS);
	}
	//
	_mascaraCacheSuciasMalla	= 0xFF;
	NBCAJAAABB_INICIALIZAR(_cacheCajaLocalCalculada);
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUEscenaMalla::~AUEscenaMalla(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMalla::~AUEscenaMalla")
	//Liberar todas las texturas retenidas
	UI32 iTex, iTexConteo = _texturas.conteo;
	for(iTex=0; iTex<iTexConteo; iTex++){
		if(_texturas.elemento[iTex] != NULL) _texturas.elemento[iTex]->liberar(NB_RETENEDOR_THIS);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

UI8 AUEscenaMalla::columnas() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMalla::columnas")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _columnas;
}

UI8 AUEscenaMalla::filas() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMalla::filas")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _filas;
}

AUArregloNativoP<NBPuntoColor>* AUEscenaMalla::verticesLocales() /*const*/ {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMalla::verticesLocales")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return &_verticesLocales;
}

AUArregloNativoP<AUTextura*>* AUEscenaMalla::texturas() /*const*/ {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMalla::texturas")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return &_texturas;
}

AUArregloNativoP<NBRectangulo>* AUEscenaMalla::areasTexturas() /*const*/ {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMalla::areasTexturas")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return &_areasTexturas;
}

bool AUEscenaMalla::esMascaraAlpha() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMalla::esMascaraAlpha")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _esMascaraAlpha;
}

void AUEscenaMalla::establecerVerticesLocales(AUArregloNativoP<NBPunto>* nuevosVerticesLocales){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMalla::establecerVerticesLocales")
	UI32 iVertice;
	for(iVertice=0; iVertice<_verticesLocales.conteo && iVertice<nuevosVerticesLocales->conteo; iVertice++){
		_verticesLocales.elemento[iVertice].coordenada = nuevosVerticesLocales->elemento[iVertice];
	}
	AUESCENAMALLA_ENSUCIAR_MODELO
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaMalla::establecerVerticesLocales(AUArregloNativoP<NBPuntoColor>* nuevosVerticesLocales){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMalla::establecerVerticesLocales")
	UI32 iVertice;
	for(iVertice=0; iVertice<_verticesLocales.conteo && iVertice<nuevosVerticesLocales->conteo; iVertice++){
		_verticesLocales.elemento[iVertice] = nuevosVerticesLocales->elemento[iVertice];
	}
	AUESCENAMALLA_ENSUCIAR_MODELO
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaMalla::establecerVerticesLocales(const NBPunto* nuevosVerticesLocales, UI32 cantidadVerticesLocales){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMalla::establecerVerticesLocales")
	NBASSERT(cantidadVerticesLocales==_verticesLocales.conteo);
	UI32 iVertice;
	for(iVertice=0; iVertice<cantidadVerticesLocales; iVertice++){
		_verticesLocales.elemento[iVertice].coordenada = nuevosVerticesLocales[iVertice];
	}
	AUESCENAMALLA_ENSUCIAR_MODELO
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaMalla::establecerVerticesLocales(const NBPuntoColor* nuevosVerticesLocales, UI32 cantidadVerticesLocales){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMalla::establecerVerticesLocales")
	NBASSERT(cantidadVerticesLocales==_verticesLocales.conteo);
	UI32 iVertice, iVerticeConteo = cantidadVerticesLocales;
	for(iVertice=0; iVertice<iVerticeConteo; iVertice++){
		_verticesLocales.elemento[iVertice] = nuevosVerticesLocales[iVertice];
	}
	AUESCENAMALLA_ENSUCIAR_MODELO
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaMalla::establecerEsMascaraAlpha(bool esMascaraAlpha){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMalla::establecerEsMascaraAlpha")
	_esMascaraAlpha = esMascaraAlpha;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaMalla::establecerColorVertice(UI32 iVertice, UI8 r, UI8 g, UI8 b, UI8 a){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMalla::establecerColorVertice")
	NBASSERT(iVertice<_verticesLocales.conteo)
	_verticesLocales.elemento[iVertice].color.r = r;
	_verticesLocales.elemento[iVertice].color.g = g;
	_verticesLocales.elemento[iVertice].color.b = b;
	_verticesLocales.elemento[iVertice].color.a = a;
	AUESCENAMALLA_ENSUCIAR_MODELO
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

//

NBCajaAABB AUEscenaMalla::cajaAABBLocalCalculada(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMalla::cajaAABBLocalCalculada")
	//if(_mascaraCacheSuciasMalla & AUESCENAMALLA_BIT_SUCIO_CAJA_LOCAL){
		NBCAJAAABB_INICIALIZAR(_cacheCajaLocalCalculada);
		UI32 iVertice;
		for(iVertice=0; iVertice<_verticesLocales.conteo; iVertice++){
			NBCAJAAABB_ENVOLVER_PUNTO(_cacheCajaLocalCalculada, _verticesLocales.elemento[iVertice].coordenada.x, _verticesLocales.elemento[iVertice].coordenada.y);
		}
		_mascaraCacheSuciasMalla &= ~AUESCENAMALLA_BIT_SUCIO_CAJA_LOCAL;
	//}
	NBASSERT(_cacheCajaLocalCalculada.xMin <= _cacheCajaLocalCalculada.xMax) //Si falla, la caja esta vacia o valores no validos
	NBASSERT(_cacheCajaLocalCalculada.yMin <= _cacheCajaLocalCalculada.yMax) //Si falla, la caja esta vacia o valores no validos
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _cacheCajaLocalCalculada;
}

void AUEscenaMalla::agregadoEnEscena(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaMalla::agregadoEnEscena")
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaMalla::quitandoDeEscena(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaMalla::quitandoDeEscena")
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

PTRfunCmdsGL AUEscenaMalla::actualizarModeloGL(const NBPropRenderizado &propsRenderizado, const NBPropIluminacion &propsIluminacion, const NBPropHeredadasRender &propsHeredadas){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMalla::actualizarModeloGL")
	NBASSERT(idEscena!=-1) //Solo si esta en escena
	UI16 conteoCeldasConTexturas	= 0;
	if(_texturas.conteo == 1){
		NBASSERT(_texturas.elemento[0] != NULL)
		conteoCeldasConTexturas		= _texturas.elemento[0] != NULL ? (_columnas * _filas) : 0; //Todas comparten la primera textura
	} else {
		UI16 iTex; for(iTex = 0; iTex < _texturas.conteo; iTex++) if(_texturas.elemento[iTex] != NULL) conteoCeldasConTexturas++; //Solo las celdas con texturas
	}
	if(conteoCeldasConTexturas != 0){
		NBASSERT(_texturas.conteo!=0)
		//PENDIENTE: optimizar, si solo es una textura, pintar como un solo TRIANG_STRIP por fila
		NBColor colorMultiplicado; NBCOLOR_MULTIPLICAR_CON_UI8(colorMultiplicado, propsHeredadas.colorPadre, _propiedades.color8);
		STBloqueGL bloqueVerticesGL;
		bloqueVerticesGL.cantidadElementos	= (conteoCeldasConTexturas * 4);
		bloqueVerticesGL.primerElemento		= (*propsRenderizado.funcReservarVertices)(ENVerticeGlTipo_MonoTextura, bloqueVerticesGL.cantidadElementos);
		NBVerticeGL* verticesGL				= &((*propsRenderizado.verticesGL1)[bloqueVerticesGL.primerElemento]);
		//ACTUALIZAR VERTICES Y TEXTURA-COORDS (la malla esta definida por la secuencia de puntos que van de arriba hacia abajo, columna por columna hacia la derecha)
		NBPuntoColor* verticesMalla			= _verticesLocales.elemento;
		UI16 col, fil;
		for(fil = 0; fil < _filas; fil++){
			for(col = 0; col < _columnas; col++){
				const UI16 iTex		= (_texturas.conteo == 1 ? 0 : (fil * _columnas) + col); NBASSERT(iTex >= 0 && iTex < _texturas.conteo)
				AUTextura* texturaSegmento 	= _texturas.elemento[iTex];
				if(texturaSegmento != NULL){
					const UI16 iTexRel = (_areasTexturas.conteo == 1 ? 0 : (fil * _columnas) + col);
					const NBRectangulo areaRelativaSegmento = _areasTexturas.elemento[iTexRel]; NBASSERT(iTexRel >= 0 && iTexRel < _areasTexturas.conteo)
					//Cada cuatro puntos consecutivos representan dos triangulos en forma de strip
					//El bloque son dos triagulos:
					//  2---3
					//  |\  |
					//  | \ |
					//  0__\1
					//Vertices
					NBPuntoColor* verticesSeg[4];
					verticesSeg[0]				= &verticesMalla[((fil+1)*(this->_columnas+1)) + (col)];	NBASSERT(verticesSeg[0] < (&_verticesLocales.elemento[_verticesLocales.conteo]))
					verticesSeg[1]				= &verticesMalla[((fil+1)*(this->_columnas+1)) + (col+1)];	NBASSERT(verticesSeg[1] < (&_verticesLocales.elemento[_verticesLocales.conteo]))
					verticesSeg[2]				= &verticesMalla[(fil*(this->_columnas+1)) + (col)];		NBASSERT(verticesSeg[2] < (&_verticesLocales.elemento[_verticesLocales.conteo]))
					verticesSeg[3]				= &verticesMalla[(fil*(this->_columnas+1)) + (col+1)];		NBASSERT(verticesSeg[3] < (&_verticesLocales.elemento[_verticesLocales.conteo]))
					NBMATRIZ_MULTIPLICAR_PUNTO(verticesGL[0], _cacheObjEscena.matrizEscena, verticesSeg[0]->coordenada.x, verticesSeg[0]->coordenada.y)
					NBMATRIZ_MULTIPLICAR_PUNTO(verticesGL[1], _cacheObjEscena.matrizEscena, verticesSeg[1]->coordenada.x, verticesSeg[1]->coordenada.y)
					NBMATRIZ_MULTIPLICAR_PUNTO(verticesGL[2], _cacheObjEscena.matrizEscena, verticesSeg[2]->coordenada.x, verticesSeg[2]->coordenada.y)
					NBMATRIZ_MULTIPLICAR_PUNTO(verticesGL[3], _cacheObjEscena.matrizEscena, verticesSeg[3]->coordenada.x, verticesSeg[3]->coordenada.y)
					NBCOLOR_ESTABLECER(verticesGL[0], verticesSeg[0]->color.r*colorMultiplicado.r, verticesSeg[0]->color.g*colorMultiplicado.g, verticesSeg[0]->color.b*colorMultiplicado.b, verticesSeg[0]->color.a*colorMultiplicado.a)
					NBCOLOR_ESTABLECER(verticesGL[1], verticesSeg[1]->color.r*colorMultiplicado.r, verticesSeg[1]->color.g*colorMultiplicado.g, verticesSeg[1]->color.b*colorMultiplicado.b, verticesSeg[1]->color.a*colorMultiplicado.a)
					NBCOLOR_ESTABLECER(verticesGL[2], verticesSeg[2]->color.r*colorMultiplicado.r, verticesSeg[2]->color.g*colorMultiplicado.g, verticesSeg[2]->color.b*colorMultiplicado.b, verticesSeg[2]->color.a*colorMultiplicado.a)
					NBCOLOR_ESTABLECER(verticesGL[3], verticesSeg[3]->color.r*colorMultiplicado.r, verticesSeg[3]->color.g*colorMultiplicado.g, verticesSeg[3]->color.b*colorMultiplicado.b, verticesSeg[3]->color.a*colorMultiplicado.a)
					//Texturas
					const NBCajaAABBP<double> cajaRelTex = texturaSegmento->cajaRelativaTexeles();
					const double anchoRelTex	= (cajaRelTex.xMax - cajaRelTex.xMin);
					const double altoRelTex		= (cajaRelTex.yMax - cajaRelTex.yMin);
					verticesGL[0].tex.x	= cajaRelTex.xMin + (areaRelativaSegmento.x * anchoRelTex);
					verticesGL[0].tex.y	= cajaRelTex.yMin + (areaRelativaSegmento.y * altoRelTex) + (altoRelTex * areaRelativaSegmento.alto);
					verticesGL[1].tex.x	= cajaRelTex.xMin + (areaRelativaSegmento.x * anchoRelTex) + (anchoRelTex * areaRelativaSegmento.ancho);
					verticesGL[1].tex.y	= cajaRelTex.yMin + (areaRelativaSegmento.y * altoRelTex) + (altoRelTex * areaRelativaSegmento.alto);
					verticesGL[2].tex.x	= cajaRelTex.xMin + (areaRelativaSegmento.x * anchoRelTex);
					verticesGL[2].tex.y	= cajaRelTex.yMin + (areaRelativaSegmento.y * altoRelTex);
					verticesGL[3].tex.x	= cajaRelTex.xMin + (areaRelativaSegmento.x * anchoRelTex) + (anchoRelTex * areaRelativaSegmento.ancho);
					verticesGL[3].tex.y	= cajaRelTex.yMin + (areaRelativaSegmento.y * altoRelTex);
					verticesGL += 4;
					NBASSERT(verticesGL<=&((*propsRenderizado.verticesGL1)[bloqueVerticesGL.primerElemento+bloqueVerticesGL.cantidadElementos]))
				}
			}
		}
		NBASSERT(verticesGL==&((*propsRenderizado.verticesGL1)[bloqueVerticesGL.primerElemento+bloqueVerticesGL.cantidadElementos]))
		//
		NBASSERT(conteoCeldasConTexturas > 0)
		const UI16 conteoTexturas		= (_texturas.conteo == 1 ? 1 : conteoCeldasConTexturas);
		const SI32 posicionDatos		= propsRenderizado.bytesDatosModelos->conteo; propsRenderizado.bytesDatosModelos->reservarRegistrosAlFinal(sizeof(STMallaRender));
		const SI32 posicionTexturas		= propsRenderizado.bytesDatosModelos->conteo; propsRenderizado.bytesDatosModelos->reservarRegistrosAlFinal(sizeof(STMallaRenderTextura) * conteoTexturas);
		STMallaRender* propsRender		= (STMallaRender*)&(propsRenderizado.bytesDatosModelos->elemento[posicionDatos]);
		STMallaRenderTextura* arrTexturas = (STMallaRenderTextura*)&(propsRenderizado.bytesDatosModelos->elemento[posicionTexturas]);
		propsRender->conteoCeldas		= conteoCeldasConTexturas; NBASSERT(propsRender->conteoCeldas > 0)
		propsRender->conteoTexturas		= conteoTexturas;
		propsRender->esMascaraAlpha		= _esMascaraAlpha;
		propsRender->bloqueIndicesGL	= NB_GESTOR_GL_DAME_ELEMS_PARA_TRIANGSTRIPS_4(ENVerticeGlTipo_MonoTextura, bloqueVerticesGL, conteoCeldasConTexturas);
		/*if(this->esClase("AUEscenaMallaBezier")){ //INvestigando un mal renderizado de tallo de plantas
			PRINTF_INFO("AUEscenaMallaBezier, reservados vertices inicio(%d) conteo(%d) celdas(%d).\n", bloqueVerticesGL.primerElemento, bloqueVerticesGL.cantidadElementos, conteoCeldasConTexturas);
			PRINTF_INFO("AUEscenaMallaBezier, reservados indices inicio(%d) conteo(%d).\n", propsRender->bloqueIndicesGL.primerElemento, propsRender->bloqueIndicesGL.cantidadElementos);
			UI16 i, iV = 0; const UI16 conteo = conteoCeldasConTexturas;
			for(i = 0; i < conteo; i++){
				PRINTF_INFO("---\n");
				PRINTF_INFO("v0(%f, %f) c(%d, %d, %d, %d) tex(%f, %f).\n", verticesGL[iV].x, verticesGL[iV].y, verticesGL[iV].r, verticesGL[iV].g, verticesGL[iV].b, verticesGL[iV].a, verticesGL[iV].tex.x, verticesGL[iV].tex.y); iV++;
				PRINTF_INFO("v1(%f, %f) c(%d, %d, %d, %d) tex(%f, %f).\n", verticesGL[iV].x, verticesGL[iV].y, verticesGL[iV].r, verticesGL[iV].g, verticesGL[iV].b, verticesGL[iV].a, verticesGL[iV].tex.x, verticesGL[iV].tex.y); iV++;
				PRINTF_INFO("v2(%f, %f) c(%d, %d, %d, %d) tex(%f, %f).\n", verticesGL[iV].x, verticesGL[iV].y, verticesGL[iV].r, verticesGL[iV].g, verticesGL[iV].b, verticesGL[iV].a, verticesGL[iV].tex.x, verticesGL[iV].tex.y); iV++;
				PRINTF_INFO("v3(%f, %f) c(%d, %d, %d, %d) tex(%f, %f).\n", verticesGL[iV].x, verticesGL[iV].y, verticesGL[iV].r, verticesGL[iV].g, verticesGL[iV].b, verticesGL[iV].a, verticesGL[iV].tex.x, verticesGL[iV].tex.y); iV++;
			}
			PRINTF_INFO("----------------.\n");
		}*/
		if(_texturas.conteo == 1){
			NBASSERT(_texturas.elemento[0] != NULL)
			arrTexturas[0].idTexturaGL	= _texturas.elemento[0]->idTexturaGL; NBASSERT(&arrTexturas[0] < &arrTexturas[conteoTexturas])
			propsRenderizado.piscinaRetecionRender->agregarElemento(_texturas.elemento[0]);
		} else {
			NBASSERT(_texturas.conteo != 0)
			NBASSERT(_texturas.conteo == (_columnas * _filas))
			UI16 iTex, iTexUsada = 0;
			for(iTex=0; iTex<_texturas.conteo; iTex++){
				if(_texturas.elemento[iTex] != NULL){
					NBASSERT(&arrTexturas[iTexUsada] < &arrTexturas[conteoTexturas])
					arrTexturas[iTexUsada++].idTexturaGL = _texturas.elemento[iTex]->idTexturaGL;
					propsRenderizado.piscinaRetecionRender->agregarElemento(_texturas.elemento[iTex]); NBASSERT(iTexUsada<=conteoTexturas)
				}
			}
			NBASSERT(iTexUsada==conteoTexturas)
		}
		NBASSERT(propsRender->conteoCeldas > 0)
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return (conteoCeldasConTexturas!=0 ? &AUEscenaMalla::enviarComandosGL : NULL);
}

void AUEscenaMalla::enviarComandosGL(UI8* punteroDatosModelo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMalla::enviarComandosGL")
	STMallaRender* propsRender			= (STMallaRender*) punteroDatosModelo;
	STMallaRenderTextura* arrTexturas	= (STMallaRenderTextura*)(punteroDatosModelo + sizeof(STMallaRender));
	NBASSERT(propsRender->conteoCeldas > 0)
	NBASSERT(propsRender->conteoTexturas > 0)
	NBASSERT(propsRender->bloqueIndicesGL.cantidadElementos>0)
	NBASSERT(propsRender->conteoTexturas==1 || propsRender->conteoTexturas == propsRender->conteoCeldas)
	//
	NBGestorGL::activarVerticesGL(ENVerticeGlTipo_MonoTextura); NBGESTORGL_DBG_NOMBRAR_ACTIVADOR_VERTICES_GL("AUEscenaMalla")
	if(propsRender->esMascaraAlpha){
		NBGestorGL::blendFunc(GL_ZERO, GL_SRC_ALPHA);
		NBGestorGL::colorMask(false, false, false, true);
	}
	if(propsRender->conteoTexturas==1){
		NBASSERT(arrTexturas[0].idTexturaGL!=-1)
		NBGestorGL::bindTexture(0, arrTexturas[0].idTexturaGL);
		NBASSERT((propsRender->bloqueIndicesGL.cantidadElementos % NB_GESTOR_GL_ELEMS_POR_TRIANGSTRIP) == 0)
		NB_GESTOR_GL_RENDER_TRIANGSTRIPS_4(propsRender->bloqueIndicesGL.primerElemento, propsRender->bloqueIndicesGL.cantidadElementos); NBGESTORGL_DBG_NOMBRAR_CONVOCADOR_ACUMULAR_TRIANG_STRIPS_INDEP("AUEscenaMalla")
	} else {
		UI16 iElemGL = propsRender->bloqueIndicesGL.primerElemento;
		UI16 i; const UI16 conteo = propsRender->conteoCeldas;
		NBASSERT(propsRender->bloqueIndicesGL.cantidadElementos == (propsRender->conteoCeldas * NB_GESTOR_GL_ELEMS_POR_TRIANGSTRIP))
		for(i=0; i<conteo; i++){
			NBASSERT(arrTexturas[i].idTexturaGL!=-1)
			NBGestorGL::bindTexture(0, arrTexturas[i].idTexturaGL);
			NB_GESTOR_GL_RENDER_TRIANGSTRIP_4(iElemGL, NB_GESTOR_GL_ELEMS_POR_TRIANGSTRIP); NBGESTORGL_DBG_NOMBRAR_CONVOCADOR_ACUMULAR_TRIANG_STRIPS_INDEP("AUEscenaMalla")
			iElemGL += NB_GESTOR_GL_ELEMS_POR_TRIANGSTRIP;
		}
	}
	if(propsRender->esMascaraAlpha){
		NBGestorGL::colorMask(true, true, true, true);
		NBGestorGL::blendFuncCargarValoresPorDefecto();
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

//

AUEscenaMalla* AUEscenaMalla::crearMallaSprite(ENMemoriaTipo tipoMemoriaResidencia, AUTextura* texturaSprite, UI8 columnas, UI8 filas, ENEscenaMallaAlineacionH alineacionH, ENEscenaMallaAlineacionV alineacionV){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMalla::crearMallaSprite")
	AUArregloNativoMutableP<NBPuntoColor>* vertices	= new(ENMemoriaTipo_Temporal) AUArregloNativoMutableP<NBPuntoColor>();		
	AUArregloNativoMutableP<AUTextura*>* texturas	= new(ENMemoriaTipo_Temporal) AUArregloNativoMutableP<AUTextura*>();	
	AUArregloNativoMutableP<NBRectangulo>* areasTex	= new(ENMemoriaTipo_Temporal) AUArregloNativoMutableP<NBRectangulo>();	
	NB_DEFINE_NOMBRE_PUNTERO(vertices, "AUEscenaMalla::vertices");
	NB_DEFINE_NOMBRE_PUNTERO(texturas, "AUEscenaMalla::texturas");
	NB_DEFINE_NOMBRE_PUNTERO(areasTex, "AUEscenaMalla::areasTex");
	//
	AUEscenaMalla::llenarParametrosMallaSprite(texturaSprite, columnas, filas, vertices, texturas, areasTex, alineacionH, alineacionV);
	AUEscenaMalla* nuevaMalla = new(tipoMemoriaResidencia) AUEscenaMalla(columnas, filas, vertices, texturas, areasTex);
	nuevaMalla->autoLiberar(NB_RETENEDOR_NULL);
	//
	vertices->liberar(NB_RETENEDOR_NULL);
	texturas->liberar(NB_RETENEDOR_NULL);
	areasTex->liberar(NB_RETENEDOR_NULL);
	//
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return nuevaMalla;
}

AUEscenaMalla* AUEscenaMalla::crearMallaPatron(ENMemoriaTipo tipoMemoriaResidencia, AUTextura* texturaRepetir, UI8 columnas, UI8 filas, ENEscenaMallaAlineacionH alineacionH, ENEscenaMallaAlineacionV alineacionV){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMalla::crearMallaPatron")
	AUArregloNativoMutableP<NBPuntoColor>* vertices	= new(ENMemoriaTipo_Temporal) AUArregloNativoMutableP<NBPuntoColor>();
	AUArregloNativoMutableP<AUTextura*>* texturas	= new(ENMemoriaTipo_Temporal) AUArregloNativoMutableP<AUTextura*>();
	AUArregloNativoMutableP<NBRectangulo>* areasTex	= new(ENMemoriaTipo_Temporal) AUArregloNativoMutableP<NBRectangulo>();
	NB_DEFINE_NOMBRE_PUNTERO(vertices, "AUEscenaMalla::vertices");
	NB_DEFINE_NOMBRE_PUNTERO(texturas, "AUEscenaMalla::texturas");
	NB_DEFINE_NOMBRE_PUNTERO(areasTex, "AUEscenaMalla::areasTex");
	//
	AUEscenaMalla::llenarParametrosMallaPatron(texturaRepetir, columnas, filas, vertices, texturas, areasTex, alineacionH, alineacionV);
	AUEscenaMalla* nuevaMalla = new(tipoMemoriaResidencia) AUEscenaMalla(columnas, filas, vertices, texturas, areasTex);
	nuevaMalla->autoLiberar(NB_RETENEDOR_NULL);
	//
	vertices->liberar(NB_RETENEDOR_NULL);
	texturas->liberar(NB_RETENEDOR_NULL);
	areasTex->liberar(NB_RETENEDOR_NULL);
	//
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return nuevaMalla;
}

AUEscenaMalla* AUEscenaMalla::crearMallaDesdeArchivoXml(ENMemoriaTipo tipoMemoriaResidencia, const char* rutaArchivoMalla, ENEscenaMallaAlineacionH alineacionH, ENEscenaMallaAlineacionV alineacionV){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMalla::crearMallaDesdeArchivoXml")
	AUArregloNativoMutableP<NBPuntoColor>* vertices	= new(ENMemoriaTipo_Temporal) AUArregloNativoMutableP<NBPuntoColor>();
	AUArregloNativoMutableP<AUTextura*>* texturas	= new(ENMemoriaTipo_Temporal) AUArregloNativoMutableP<AUTextura*>();
	AUArregloNativoMutableP<NBRectangulo>* areasTex	= new(ENMemoriaTipo_Temporal) AUArregloNativoMutableP<NBRectangulo>();
	NB_DEFINE_NOMBRE_PUNTERO(vertices, "AUEscenaMalla::vertices");
	NB_DEFINE_NOMBRE_PUNTERO(texturas, "AUEscenaMalla::texturas");
	NB_DEFINE_NOMBRE_PUNTERO(areasTex, "AUEscenaMalla::areasTex");
	UI8 columnas, filas;
	//
	AUEscenaMalla::llenarParametrosMallaDesdeArchivoXML(rutaArchivoMalla, &columnas, &filas, vertices, texturas, areasTex, alineacionH, alineacionV);
	AUEscenaMalla* nuevaMalla = new(tipoMemoriaResidencia) AUEscenaMalla(columnas, filas, vertices, texturas, areasTex);
	nuevaMalla->autoLiberar(NB_RETENEDOR_NULL);
	//
	vertices->liberar(NB_RETENEDOR_NULL);
	texturas->liberar(NB_RETENEDOR_NULL);
	areasTex->liberar(NB_RETENEDOR_NULL);
	//
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return nuevaMalla;
}

void AUEscenaMalla::llenarParametrosMallaSprite(AUTextura* texturaSprite, UI8 columnas, UI8 filas, AUArregloNativoMutableP<NBPuntoColor>* guardarVerticesEn, AUArregloNativoMutableP<AUTextura*>* guardarTexturasEn, AUArregloNativoMutableP<NBRectangulo>* guardarAreasTexturasEn, ENEscenaMallaAlineacionH alineacionH, ENEscenaMallaAlineacionV alineacionV){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMalla::llenarParametrosMallaSprite")
	//Vaciar arreglos destinos
	guardarVerticesEn->vaciar();
	guardarTexturasEn->vaciar();
	guardarAreasTexturasEn->vaciar();
	//Calculos iniciales
	NBTamano tamTextura = texturaSprite->tamanoHD();
	NBTamano tamMalla;
	tamMalla.ancho  = tamTextura.ancho;
	tamMalla.alto	= tamTextura.alto;
	NBPunto baseSupIzq;
	baseSupIzq.x = (alineacionH==ENEscenaMallaAlineacionH_Izquierda?-tamMalla.ancho:alineacionH==ENEscenaMallaAlineacionH_Centro?tamMalla.ancho/-2.0f:0.0f);
	baseSupIzq.y = (alineacionV==ENEscenaMallaAlineacionV_Arriba?tamMalla.alto:alineacionV==ENEscenaMallaAlineacionV_Centro?tamMalla.alto/2.0f:0.0f);
	//Llenar arreglos destinos (TEXTURAS)
	guardarTexturasEn->agregarElemento(texturaSprite); //solo agregar una textura, la malla la interpretara como repetida
	//Llenar arreglos destinos (VERTICES)
	SI32 iCol, iFil; float ySup = baseSupIzq.y;
	for(iFil=0; iFil<=filas; iFil++){
		float xIzq = baseSupIzq.x;
		for(iCol=0; iCol<=columnas; iCol++){
			NBPuntoColor tmpPunto; 
			NBPUNTO_ESTABLECER(tmpPunto.coordenada, xIzq, ySup);
			NBCOLOR_ESTABLECER(tmpPunto.color, 255, 255, 255, 255)
			guardarVerticesEn->agregarElemento(tmpPunto);
			//PRINTF_INFO("Vertice agregado %d\n", guardarVerticesEn->conteo);
			xIzq += (tamTextura.ancho / (float)columnas);
		}
		ySup -= (tamTextura.alto / (float)filas);
	}
	//LLenar arreglos destinos (AREAS DE TEXTURAS EN ATLAS)
	for(iFil=0; iFil<filas; iFil++){
		for(iCol=0; iCol<columnas; iCol++){
			NBRectangulo areaTexturaEnSegmento;
			areaTexturaEnSegmento.x 	= 1.0f / (float)columnas * (float)iCol;
			areaTexturaEnSegmento.y 	= 1.0f / (float)filas * (float)iFil;
			areaTexturaEnSegmento.ancho = 1.0f / (float)columnas;
			areaTexturaEnSegmento.alto 	= 1.0f / (float)filas;
			guardarAreasTexturasEn->agregarElemento(areaTexturaEnSegmento);
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaMalla::llenarParametrosMallaPatron(AUTextura* texturaRepetir, UI8 columnas, UI8 filas, AUArregloNativoMutableP<NBPuntoColor>* guardarVerticesEn, AUArregloNativoMutableP<AUTextura*>* guardarTexturasEn, AUArregloNativoMutableP<NBRectangulo>* guardarAreasTexturasEn, ENEscenaMallaAlineacionH alineacionH, ENEscenaMallaAlineacionV alineacionV){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMalla::llenarParametrosMallaPatron")
	//Vaciar arreglos destinos
	guardarVerticesEn->vaciar();
	guardarTexturasEn->vaciar();
	guardarAreasTexturasEn->vaciar();
	//Calculos iniciales
	NBTamano tamTextura = texturaRepetir->tamanoHD();
	NBTamano tamMalla;
	tamMalla.ancho  = tamTextura.ancho * (float)columnas;
	tamMalla.alto	= tamTextura.alto * (float)filas;
	NBPunto baseSupIzq;
	baseSupIzq.x = (alineacionH==ENEscenaMallaAlineacionH_Izquierda?-tamMalla.ancho:alineacionH==ENEscenaMallaAlineacionH_Centro?tamMalla.ancho/-2.0f:0.0f);
	baseSupIzq.y = (alineacionV==ENEscenaMallaAlineacionV_Arriba?tamMalla.alto:alineacionV==ENEscenaMallaAlineacionV_Centro?tamMalla.alto/2.0f:0.0f);
	//Llenar arreglos destinos (TEXTURAS)
	guardarTexturasEn->agregarElemento(texturaRepetir); //solo agregar una textura, la malla la interpretara como repetida
	//Llenar arreglos destinos (VERTICES)
	SI32 iCol, iFil; float ySup = baseSupIzq.y;
	for(iFil=0; iFil<=filas; iFil++){
		float xIzq = baseSupIzq.x;
		for(iCol=0; iCol<=columnas; iCol++){
			NBPuntoColor tmpPunto; 
			NBPUNTO_ESTABLECER(tmpPunto.coordenada, xIzq, ySup);
			NBCOLOR_ESTABLECER(tmpPunto.color, 255, 255, 255, 255)
			guardarVerticesEn->agregarElemento(tmpPunto);
			xIzq += tamTextura.ancho;
		}
		ySup -= tamTextura.alto;
	}
	//LLenar arreglos destinos (AREAS DE TEXTURAS EN ATLAS)
	NBRectangulo areaTexturaEnSegmento;
	areaTexturaEnSegmento.x 	= 0.0f;
	areaTexturaEnSegmento.y 	= 0.0f;
	areaTexturaEnSegmento.ancho = 1.0f;
	areaTexturaEnSegmento.alto 	= 1.0f;
	guardarAreasTexturasEn->agregarElemento(areaTexturaEnSegmento); //solo agregar un area, la malla la interpretara como repetida
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaMalla::llenarParametrosMallaDesdeArchivoXML(const char* rutaArchivoMalla, UI8* guardarColumnasEn, UI8* guardarFilasEn, AUArregloNativoMutableP<NBPuntoColor>* guardarVerticesEn, AUArregloNativoMutableP<AUTextura*>* guardarTexturasEn, AUArregloNativoMutableP<NBRectangulo>* guardarAreasTexturasEn, ENEscenaMallaAlineacionH alineacionH, ENEscenaMallaAlineacionV alineacionV){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMalla::llenarParametrosMallaDesdeArchivoXML")
	//Vaciar arreglos destinos
	guardarVerticesEn->vaciar();
	guardarTexturasEn->vaciar();
	guardarAreasTexturasEn->vaciar();
	//Inicializar parametros
	UI32 filas		= 0;
	UI32 columnas	= 0;
	AUArregloNativoMutableP<SI32>* anchosColumnas	= new(ENMemoriaTipo_Temporal) AUArregloNativoMutableP<SI32>();	NB_DEFINE_NOMBRE_PUNTERO(anchosColumnas, "AUEscenaMalla::anchosColumnas");
	AUArregloNativoMutableP<SI32>* altosFilas		= new(ENMemoriaTipo_Temporal) AUArregloNativoMutableP<SI32>();	NB_DEFINE_NOMBRE_PUNTERO(altosFilas, "AUEscenaMalla::altosFilas");
	//Interpretar archivo CACHE-BINARIA
	AUCadenaMutable8* rutaCache = new(ENMemoriaTipo_Temporal) AUCadenaMutable8(rutaArchivoMalla);
	rutaCache->agregar("Cache");
	AUArchivo* archivoCache = NULL; //NBGestorArchivos::flujoDeArchivo(ENMemoriaTipo_Temporal, rutaCache->str(), ENArchivoModo_SoloLectura);
	if(archivoCache != NULL){
		archivoCache->lock();
		//Leer encabezado
		float version = 1.0f;		archivoCache->leer(&version, sizeof(version), 1, archivoCache);
		SI32 anchoMalla;			archivoCache->leer(&anchoMalla, sizeof(anchoMalla), 1, archivoCache);
		SI32 altoMalla;				archivoCache->leer(&altoMalla, sizeof(altoMalla), 1, archivoCache);
		SI32 anchoSegmentoPerfecto;	archivoCache->leer(&anchoSegmentoPerfecto, sizeof(anchoSegmentoPerfecto), 1, archivoCache);
		SI32 altoSegmentoPerfecto;	archivoCache->leer(&altoSegmentoPerfecto, sizeof(altoSegmentoPerfecto), 1, archivoCache);
		archivoCache->leer(&filas, sizeof(filas), 1, archivoCache);
		archivoCache->leer(&columnas, sizeof(columnas), 1, archivoCache);
		//Pre-llenar arreglos
		UI32 iFil; for(iFil=0; iFil<filas; iFil++) altosFilas->agregarElemento(0);
		UI32 iCol; for(iCol=0; iCol<columnas; iCol++) anchosColumnas->agregarElemento(0);
		UI32 iTex; for(iTex=0; iTex<(filas*columnas); iTex++) guardarTexturasEn->agregarElemento(NULL);
		//Leer segmentos
		AUCadenaMutable8* rutaTextura = new(ENMemoriaTipo_Temporal) AUCadenaMutable8();
		for(iFil=0; iFil<filas; iFil++){
			for(iCol=0; iCol<columnas; iCol++){
				SI32 anchoSegmento;	archivoCache->leer(&anchoSegmento, sizeof(anchoSegmento), 1, archivoCache);
				SI32 altoSegmento;	archivoCache->leer(&altoSegmento, sizeof(altoSegmento), 1, archivoCache);
				AUCadenaMutable8::interpretarBitsInternos(rutaTextura, archivoCache);
				//
				if(iFil==0) anchosColumnas->elemento[iCol] = anchoSegmento;
				if(iCol==0) altosFilas->elemento[iFil] = altoSegmento;
				if(rutaTextura->tamano()!=0){
					guardarTexturasEn->elemento[(iFil*columnas)+iCol] = NBGestorTexturas::texturaDesdeArchivo(rutaTextura->str());
				}
			}
		}
		rutaTextura->liberar(NB_RETENEDOR_NULL);
		PRINTF_INFO("Malla creada desde binario: '%s'\n", rutaCache->str());
		//
		archivoCache->unlock();
		archivoCache->cerrar();
	} else {
		//Interpretar XML
		AUDatosXMLMutable* datosXml = new(ENMemoriaTipo_Temporal) AUDatosXMLMutable();
		if(datosXml->cargaDatosXMLDesdeArchivo(rutaArchivoMalla)){
			AUCadenaMutable8* strTmp = new(ENMemoriaTipo_Temporal) AUCadenaMutable8();
			AUArchivo* archivoCache = NBGestorArchivos::flujoDeArchivo(ENMemoriaTipo_Temporal, rutaCache->str(), ENArchivoModo_SoloEscritura);
			if(archivoCache != NULL){
				archivoCache->lock();
			}
			const sNodoXML* nodoRaiz = datosXml->nodoHijo("malla");
			if(nodoRaiz != NULL){
				SI32 anchoMalla				= datosXml->nodoHijo("anchoMalla", 0.0f, nodoRaiz);
				SI32 altoMalla				= datosXml->nodoHijo("altoMalla", 0.0f, nodoRaiz);
				SI32 anchoSegmentoPerfecto	= datosXml->nodoHijo("anchoSegmentoPerfecto", 0.0f, nodoRaiz);
				SI32 altoSegmentoPerfecto	= datosXml->nodoHijo("altoSegmentoPerfecto", 0.0f, nodoRaiz);
				filas						= datosXml->nodoHijo("filas", 0.0f, nodoRaiz);
				columnas					= datosXml->nodoHijo("columnas", 0.0f, nodoRaiz);
				if(archivoCache != NULL){
					float version = 1.0f; archivoCache->escribir(&version, sizeof(version), 1, archivoCache);
					archivoCache->escribir(&anchoMalla, sizeof(anchoMalla), 1, archivoCache);
					archivoCache->escribir(&altoMalla, sizeof(altoMalla), 1, archivoCache);
					archivoCache->escribir(&anchoSegmentoPerfecto, sizeof(anchoSegmentoPerfecto), 1, archivoCache);
					archivoCache->escribir(&altoSegmentoPerfecto, sizeof(altoSegmentoPerfecto), 1, archivoCache);
					archivoCache->escribir(&filas, sizeof(filas), 1, archivoCache);
					archivoCache->escribir(&columnas, sizeof(columnas), 1, archivoCache);
				}
				const sNodoXML* nodoSegmentos = datosXml->nodoHijo("segmentos", nodoRaiz);
				if(nodoSegmentos != NULL){
					//Prellenar los arreglos
					UI32 iFil; for(iFil=0; iFil<filas; iFil++) altosFilas->agregarElemento(0);
					UI32 iCol; for(iCol=0; iCol<columnas; iCol++) anchosColumnas->agregarElemento(0);
					UI32 iTex; for(iTex=0; iTex<(filas*columnas); iTex++) guardarTexturasEn->agregarElemento(NULL);
					//Leer los segmentos
					UI32 indiceSegmento = 0;
					const sNodoXML* nodoSegm = datosXml->nodoHijo("segmento", nodoSegmentos);
					while(nodoSegm != NULL && indiceSegmento<(filas*columnas)){
						iFil = (indiceSegmento / columnas);
						iCol = (indiceSegmento % columnas);
						SI32 anchoSegmento	= datosXml->nodoHijo("ancho", 0.0f, nodoSegm);
						SI32 altoSegmento	= datosXml->nodoHijo("alto", 0.0f, nodoSegm);
						datosXml->nodoHijo("textura", strTmp, "", nodoSegm);
						if(archivoCache != NULL){
							archivoCache->escribir(&anchoSegmento, sizeof(anchoSegmento), 1, archivoCache);
							archivoCache->escribir(&altoSegmento, sizeof(altoSegmento), 1, archivoCache);
							AUCadena8::agregarBitsInternosEn(strTmp, archivoCache);
						}
						if(iFil==0) anchosColumnas->elemento[iCol] = anchoSegmento;
						if(iCol==0) altosFilas->elemento[iFil] = altoSegmento;
						if(strTmp->tamano()!=0){
							guardarTexturasEn->elemento[indiceSegmento] = NBGestorTexturas::texturaDesdeArchivo(strTmp->str());
						}
						//
						nodoSegm = datosXml->nodoHijo("segmento", nodoSegmentos, nodoSegm);
						indiceSegmento++;
					}
				}
			}
			if(archivoCache != NULL){
				archivoCache->unlock();
				archivoCache->cerrar();
				PRINTF_INFO("Archivo cache de malla creado: '%s'\n", rutaCache->str());
			}
			strTmp->liberar(NB_RETENEDOR_NULL);
		}
		datosXml->liberar(NB_RETENEDOR_NULL);
	}
	rutaCache->liberar(NB_RETENEDOR_NULL);
	//Calculos iniciales
	NBTamano tamMalla; UI32 iCol, iFil;
	tamMalla.ancho  = 0.0f; for(iCol=0; iCol<columnas; iCol++) tamMalla.ancho += anchosColumnas->elemento[0];
	tamMalla.alto	= 0.0f; for(iFil=0; iFil<filas; iFil++) tamMalla.alto += altosFilas->elemento[0];
	NBPunto baseSupIzq;
	baseSupIzq.x = (alineacionH==ENEscenaMallaAlineacionH_Izquierda?-tamMalla.ancho:alineacionH==ENEscenaMallaAlineacionH_Centro?tamMalla.ancho/-2.0f:0.0f);
	baseSupIzq.y = (alineacionV==ENEscenaMallaAlineacionV_Arriba?tamMalla.alto:alineacionV==ENEscenaMallaAlineacionV_Centro?tamMalla.alto/2.0f:0.0f);
	//Llenar arreglos destinos (VERTICES)
	float ySup = baseSupIzq.y;
	for(iFil=0; iFil<=filas; iFil++){
		float xIzq = baseSupIzq.x;
		for(iCol=0; iCol<=columnas; iCol++){
			NBPuntoColor tmpPunto; 
			NBPUNTO_ESTABLECER(tmpPunto.coordenada, xIzq, ySup);
			NBCOLOR_ESTABLECER(tmpPunto.color, 255, 255, 255, 255)
			guardarVerticesEn->agregarElemento(tmpPunto);
			xIzq += anchosColumnas->elemento[iCol<anchosColumnas->conteo?iCol:anchosColumnas->conteo-1];
		}
		ySup -= altosFilas->elemento[iFil<altosFilas->conteo?iFil:altosFilas->conteo-1];
	}
	//LLenar arreglos destinos (AREAS DE TEXTURAS EN ATLAS)
	NBRectangulo areaTexturaEnSegmento;
	areaTexturaEnSegmento.x 	= 0.0f;
	areaTexturaEnSegmento.y 	= 0.0f;
	areaTexturaEnSegmento.ancho = 1.0f;
	areaTexturaEnSegmento.alto 	= 1.0f;
	guardarAreasTexturasEn->agregarElemento(areaTexturaEnSegmento); //solo agregar un area, la malla la interpretara como repetida
	//Llenar dimensiones
	*guardarColumnasEn 			= columnas;
	*guardarFilasEn 			= filas;
	//
	anchosColumnas->liberar(NB_RETENEDOR_NULL);
	altosFilas->liberar(NB_RETENEDOR_NULL);
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

//

bool AUEscenaMalla::agregarXmlInternoEn(AUEscenaMalla* malla, AUCadenaLargaMutable8* guardarEn, const char* espaciosBaseIzq){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMalla::agregarXmlInternoEn")
	//guardarEn->agregarConFormato("%s<vXML>1</vXML>\r\n", espaciosBaseIzq);
	guardarEn->agregarConFormato("%s<eOb>\r\n", espaciosBaseIzq);
	AUEscenaObjeto::agregarXmlInternoEn(malla, guardarEn, espaciosBaseIzq);
	guardarEn->agregarConFormato("%s</eOb>\r\n", espaciosBaseIzq);
	//Datos de malla
	guardarEn->agregarConFormato("%s<fils>%d</fils><cols>%d</cols>\r\n", espaciosBaseIzq, malla->_filas, malla->_columnas);
	guardarEn->agregarConFormato("%s<esAlpha>%d</esAlpha>\r\n", espaciosBaseIzq, malla->_esMascaraAlpha?1:0);
	guardarEn->agregarConFormato("%s<vs>\r\n", espaciosBaseIzq);
	UI32 i;
	for(i=0; i<malla->_verticesLocales.conteo; i++){
		NBPuntoColor vertice = malla->_verticesLocales.elemento[i];
		guardarEn->agregarConFormato("%s  <v>%f|%f|%d|%d|%d|%d</v>\r\n", espaciosBaseIzq, vertice.coordenada.x, vertice.coordenada.y, (SI32)vertice.color.r, (SI32)vertice.color.g, (SI32)vertice.color.b, (SI32)vertice.color.a);
	}
	guardarEn->agregarConFormato("%s</vs>\r\n", espaciosBaseIzq);
	guardarEn->agregarConFormato("%s<texturas>\r\n", espaciosBaseIzq);
	for(i=0; i<malla->_texturas.conteo; i++){
		AUTextura* textura = malla->_texturas.elemento[i];
		guardarEn->agregarConFormato("%s  <tex>%s</tex>\r\n", espaciosBaseIzq, NBGestorTexturas::nombreRecursoTextura(textura));
	}
	guardarEn->agregarConFormato("%s</texturas>\r\n", espaciosBaseIzq);
	guardarEn->agregarConFormato("%s<areasTex>\r\n", espaciosBaseIzq);
	for(i=0; i<malla->_areasTexturas.conteo; i++){
		NBRectangulo areaTex = malla->_areasTexturas.elemento[i];
		guardarEn->agregarConFormato("%s  <a>%f|%f|%f|%f</a>\r\n", espaciosBaseIzq, areaTex.x, areaTex.y, areaTex.ancho, areaTex.alto);
	}
	guardarEn->agregarConFormato("%s</areasTex>\r\n", espaciosBaseIzq);
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return true;
	
}

bool AUEscenaMalla::interpretarXmlInterno(AUEscenaMalla* cargarEn, const AUDatosXML* datosXml, const sNodoXML* nodoXml){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaSprite::interpretarXmlInterno")
	bool exito = false;
	//int versionXML = datosXml->nodoHijo("vXML", 0, nodoXml);
	//if(versionXML==1){
		const sNodoXML* nodoEscenaObj;	XML_NODO_HIJO(datosXml, nodoEscenaObj, "eOb", "escObj", nodoXml, NULL)
		if(nodoEscenaObj != NULL){
			if(AUEscenaObjeto::interpretarXmlInterno(cargarEn, datosXml, nodoEscenaObj)){
				//Liberar todas las texturas y vaciar arreglos
				UI32 iTex; for(iTex=0; iTex<cargarEn->_texturas.conteo; iTex++){
					if(cargarEn->_texturas.elemento[iTex] != NULL) cargarEn->_texturas.elemento[iTex]->liberar(NB_RETENEDOR_NULL);
				}
				cargarEn->_verticesLocales.vaciar();
				cargarEn->_texturas.vaciar();
				cargarEn->_areasTexturas.vaciar();
				//Cargar datos XML
				cargarEn->_filas = datosXml->nodoHijo("fils", 0, nodoXml);
				cargarEn->_columnas = datosXml->nodoHijo("cols", 0, nodoXml);
				cargarEn->_esMascaraAlpha = (datosXml->nodoHijo("esAlpha", 0, nodoXml)!=0);// NBASSERT(!cargarEn->_esMascaraAlpha)
				const sNodoXML* nodoVertices; XML_NODO_HIJO(datosXml, nodoVertices, "vs", "vertices", nodoXml, NULL)
				if(nodoVertices != NULL){
					const sNodoXML* nodoV = datosXml->nodoHijo("v", nodoVertices);
					while(nodoV != NULL){
						NBPuntoColor nuevoVertice;
						if(nodoV->nodosHijos == NULL){
							//Formato nuevo CSV (mas compacto)
							const char* cadenaCSV		= datosXml->cadenaValorDeNodo(nodoV);
							SI32 tamCadenaCSV			= AUCadena8::tamano(cadenaCSV);
							SI32 posEnCadenaCSV			= 0;
							nuevoVertice.coordenada.x	= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0.0f);
							nuevoVertice.coordenada.y	= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0.0f);
							nuevoVertice.color.r		= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 255);
							nuevoVertice.color.g		= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 255);
							nuevoVertice.color.b		= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 255);
							nuevoVertice.color.a		= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 255);
						} else {
							//Formato viejo XML (muy inflado)
							nuevoVertice.coordenada.x	= datosXml->nodoHijo("x", 0.0f, nodoV);
							nuevoVertice.coordenada.y	= datosXml->nodoHijo("y", 0.0f, nodoV);
							nuevoVertice.color.r		= datosXml->nodoHijo("r", 255, nodoV);
							nuevoVertice.color.g		= datosXml->nodoHijo("g", 255, nodoV);
							nuevoVertice.color.b		= datosXml->nodoHijo("b", 255, nodoV);
							nuevoVertice.color.a		= datosXml->nodoHijo("a", 255, nodoV);
						}
						cargarEn->_verticesLocales.agregarElemento(nuevoVertice);
						//siguiente
						nodoV = datosXml->nodoHijo("v", nodoVertices, nodoV);
					}
				}
				const sNodoXML* nodoTexturas = datosXml->nodoHijo("texturas", nodoXml);
				if(nodoVertices != NULL){
					const sNodoXML* nodoTex = datosXml->nodoHijo("tex", nodoTexturas);
					while(nodoTex != NULL){
						AUTextura* textura = NBGestorTexturas::texturaDesdeArchivo(datosXml->cadenaValorDeNodo(nodoTex));
						if(textura != NULL) textura->retener(NB_RETENEDOR_NULL);
						cargarEn->_texturas.agregarElemento(textura);
						//siguiente
						nodoTex = datosXml->nodoHijo("tex", nodoTexturas, nodoTex);
					}
				}
				const sNodoXML* nodoAreas = datosXml->nodoHijo("areasTex", nodoXml);
				if(nodoAreas != NULL){
					const sNodoXML* nodoA = datosXml->nodoHijo("a", nodoAreas);
					while(nodoA != NULL){
						NBRectangulo nuevoArea;
						if(nodoA->nodosHijos == NULL){
							//Formato nuevo CSV (mas compacto)
							const char* cadenaCSV		= datosXml->cadenaValorDeNodo(nodoA);
							SI32 tamCadenaCSV			= AUCadena8::tamano(cadenaCSV);
							SI32 posEnCadenaCSV			= 0;
							nuevoArea.x					= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0.0f);
							nuevoArea.y					= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0.0f);
							nuevoArea.ancho				= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0.0f);
							nuevoArea.alto				= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0.0f);
						} else {
							//Formato viejo XML (muy inflado)
							nuevoArea.x = datosXml->nodoHijo("x", 0.0f, nodoA);
							nuevoArea.y = datosXml->nodoHijo("y", 0.0f, nodoA);
							nuevoArea.ancho = datosXml->nodoHijo("an", 0.0f, nodoA);
							nuevoArea.alto = datosXml->nodoHijo("al", 0.0f, nodoA);
						}
						cargarEn->_areasTexturas.agregarElemento(nuevoArea);
						//siguiente
						nodoA = datosXml->nodoHijo("a", nodoAreas, nodoA);
					}
				}
				exito = true;
				AUESCENAMALLA_ENSUCIAR_MODELO_DE(cargarEn);
			}
		}
	//}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

bool AUEscenaMalla::agregarBitsInternosEn(AUEscenaMalla* malla, AUArchivo* guardarEn){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMalla::agregarBitsInternosEn")
	bool exito = false;
	//
	float versionDatos = 1.0f;
	guardarEn->escribir(&versionDatos, sizeof(versionDatos), 1, guardarEn);
	if(AUEscenaObjeto::agregarBitsInternosEn(malla, guardarEn)){
		guardarEn->escribir(&malla->_columnas, sizeof(malla->_columnas), 1, guardarEn);
		guardarEn->escribir(&malla->_filas, sizeof(malla->_filas), 1, guardarEn);
		guardarEn->escribir(&malla->_esMascaraAlpha, sizeof(malla->_esMascaraAlpha), 1, guardarEn);
		//Arreglos simples
		malla->_verticesLocales.agregarBitsInternosEn(guardarEn);
		malla->_areasTexturas.agregarBitsInternosEn(guardarEn);
		//Arreglos complejos
		SI32 conteoTex = malla->_texturas.conteo; guardarEn->escribir(&conteoTex, sizeof(conteoTex), 1, guardarEn);
		SI32 iTex; const char* nombreTextura = NULL;
		for(iTex=0; iTex<conteoTex; iTex++){
			AUTextura* textura = malla->_texturas.elemento[iTex];
			SI32 tamanoNombreTextura = 0;
			if(textura != NULL) nombreTextura = NBGestorTexturas::nombreRecursoTextura(textura);
			if(nombreTextura == NULL){
				guardarEn->escribir(&tamanoNombreTextura, sizeof(tamanoNombreTextura), 1, guardarEn);
			} else {
				tamanoNombreTextura = AUCadenaMutable8::tamano(nombreTextura);
				guardarEn->escribir(&tamanoNombreTextura, sizeof(tamanoNombreTextura), 1, guardarEn);
				guardarEn->escribir(nombreTextura, sizeof(char), tamanoNombreTextura, guardarEn);
			}
		}
		exito = true;
	} else {
		NBASSERT(false)
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

bool AUEscenaMalla::interpretarBitsInternos(AUEscenaMalla* cargarEn, AUArchivo* cargarDe){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaMalla::interpretarBitsInternos")
	bool exito = false;
	float versionDatos;
	cargarDe->leer(&versionDatos, sizeof(versionDatos), 1, cargarDe);
	if(versionDatos==1.0f){
		if(AUEscenaObjeto::interpretarBitsInternos(cargarEn, cargarDe)){
			cargarDe->leer(&cargarEn->_columnas, sizeof(cargarEn->_columnas), 1, cargarDe);
			cargarDe->leer(&cargarEn->_filas, sizeof(cargarEn->_filas), 1, cargarDe);
			cargarDe->leer(&cargarEn->_esMascaraAlpha, sizeof(cargarEn->_esMascaraAlpha), 1, cargarDe);
			//Arreglos simples
			cargarEn->_verticesLocales.interpretarBitsInternos(cargarDe);
			cargarEn->_areasTexturas.interpretarBitsInternos(cargarDe);
			//Arreglos complejos
			NBASSERT(cargarEn->_texturas.conteo==0) //Las texturas deben liberarse apropiadamente
			SI32 conteoTexturas; cargarDe->leer(&conteoTexturas, sizeof(conteoTexturas), 1, cargarDe); NBASSERT(conteoTexturas>=0);
			SI32 iTex;
			for(iTex=0; iTex<conteoTexturas; iTex++){
				SI32 tamanoNombreTextura; cargarDe->leer(&tamanoNombreTextura, sizeof(tamanoNombreTextura), 1, cargarDe); NBASSERT(tamanoNombreTextura>=0);
				if(tamanoNombreTextura==0){
					cargarEn->_texturas.agregarElemento(NULL);
				} else {
					char* nombreTextura = (char*)NBGestorMemoria::reservarMemoria(tamanoNombreTextura+1, ENMemoriaTipo_Temporal); NB_DEFINE_NOMBRE_PUNTERO(nombreTextura, "AUEscenaGeneradorParticulas::nombreTextura") //+1 es el "\0"
					cargarDe->leer(nombreTextura, sizeof(char), tamanoNombreTextura, cargarDe);
					nombreTextura[tamanoNombreTextura] = '\0';
					cargarEn->_texturas.agregarElemento(NBGestorTexturas::texturaDesdeArchivo(nombreTextura));
					NBGestorMemoria::liberarMemoria(nombreTextura);
				}
			}
			exito = true;
		}
	} else {
		PRINTF_ERROR("AUEscenaMalla, la version (%f) de datos binarios no es interpretable por este ejecutable\n", versionDatos);
		NBASSERT(false);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

//
AUOBJMETODOS_CLASESID_MULTICLASE(AUEscenaMalla, AUEscenaObjeto)
AUOBJMETODOS_CLASESNOMBRES_MULTICLASE(AUEscenaMalla, "AUEscenaMalla", AUEscenaObjeto)
AUOBJMETODOS_CLONAR_THIS(AUEscenaMalla)






