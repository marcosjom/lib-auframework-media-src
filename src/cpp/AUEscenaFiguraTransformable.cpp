//
//  AUFijacion.cpp
//  EditorCuerpos
//
//  Created by Nicaragua Binary on 30/10/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkMediaStdAfx.h"
#include "AUEscenaFiguraTransformable.h"

AUEscenaFiguraTransformable::AUEscenaFiguraTransformable(ENEscenaFiguraTipo tipo) : AUEscenaFigura(tipo) {
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaFiguraTransformable")
	inicializarVariables();
	_indiceVerticeSel			= -1;
	_dibujarFiguraBase			= true;
	_dibujarEscalaH				= false;
	_dibujarEscalaV				= false;
	_dibujarAngulo				= false;
	_dibujarCentro				= false;
	_dibujarReferenciasInternas	= false;
	_dibujarVertices			= false;
	_colorVertices.r			= 0.0f;	_colorVertices.g			= 1.0f;	_colorVertices.b			= 1.0f; _colorVertices.a			= 1.0f;
	_colorVerticesReslatado.r	= 1.0f; _colorVerticesReslatado.g	= 0.0f;	_colorVerticesReslatado.b	= 0.0f; _colorVerticesReslatado.a	= 1.0f;
	_colorVerticeSeleccionado.r	= 1.0f; _colorVerticeSeleccionado.g	= 0.0f;	_colorVerticeSeleccionado.b	= 0.0f; _colorVerticeSeleccionado.a	= 1.0f;
	_colorTransformaciones.r	= 1.0f;	_colorTransformaciones.g	= 1.0f; _colorTransformaciones.b	= 1.0f;	_colorTransformaciones.a	= 1.0f;
	_colorRefInternas.r			= 0.5f; _colorRefInternas.g			= 0.5f; _colorRefInternas.b			= 0.5f; _colorRefInternas.a			= 0.5f;
}

AUEscenaFiguraTransformable::AUEscenaFiguraTransformable(AUEscenaFiguraTransformable* otro) : AUEscenaFigura(otro) {
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaFiguraTransformable")
	inicializarVariables();
	_indiceVerticeSel			= -1;
	_dibujarFiguraBase			= otro->_dibujarFiguraBase;
	_dibujarEscalaH				= otro->_dibujarEscalaH;
	_dibujarEscalaV				= otro->_dibujarEscalaV;
	_dibujarAngulo				= otro->_dibujarAngulo;
	_dibujarCentro				= otro->_dibujarCentro;
	_dibujarReferenciasInternas	= otro->_dibujarReferenciasInternas;
	_dibujarVertices			= otro->_dibujarVertices;
	_colorVertices				= otro->_colorVertices;
	_colorVerticesReslatado		= otro->_colorVerticesReslatado;
	_colorVerticeSeleccionado	= otro->_colorVerticeSeleccionado;
	_colorTransformaciones		= otro->_colorTransformaciones;
	_colorRefInternas			= otro->_colorRefInternas;
}

void AUEscenaFiguraTransformable::inicializarVariables(){
	_centroResaltado			= false;
	_anguloResaltado			= false;
	_escalaHorizontalResaltada	= false;
	_escalaVerticalResaltada	= false;
	_indiceVerticeReslatado		= -1;
}

AUEscenaFiguraTransformable::~AUEscenaFiguraTransformable(){
	//
}

void AUEscenaFiguraTransformable::seleccionarVertice(SI32 indiceSelecionar){
	NBASSERT(indiceSelecionar>=-1 && indiceSelecionar<(SI32)this->_coordenadasLocales.conteo)
	_indiceVerticeSel = indiceSelecionar;
}

int AUEscenaFiguraTransformable::indiceVerticeSelecionado(){
	return _indiceVerticeSel;
}

void AUEscenaFiguraTransformable::resaltarCentro(bool resaltado){
	//if(_dibujarTransformaciones){ //Todos deben poder trasladarse
		_centroResaltado = resaltado;
	//}
}

bool AUEscenaFiguraTransformable::centroResaltado(){
	return _centroResaltado;
}

void AUEscenaFiguraTransformable::resaltarAngulo(bool resaltado){
	if(_dibujarAngulo){
		_anguloResaltado = resaltado;
	}
}

bool AUEscenaFiguraTransformable::anguloResaltado(){
	return _anguloResaltado;
}

void AUEscenaFiguraTransformable::resaltarEscalaHorizontal(bool resaltado){
	_escalaHorizontalResaltada = resaltado;
}

bool AUEscenaFiguraTransformable::escalaHorizontalResaltada(){
	return _escalaHorizontalResaltada;
}

void AUEscenaFiguraTransformable::resaltarEscalaVertical(bool resaltado){
	_escalaVerticalResaltada = resaltado;
}

bool AUEscenaFiguraTransformable::escalaVerticalResaltada(){
	return _escalaVerticalResaltada;
}

void AUEscenaFiguraTransformable::resaltarVertice(int indice){
	_indiceVerticeReslatado	=	indice;
}

int AUEscenaFiguraTransformable::indiceVerticeResaltado(){
	return _indiceVerticeReslatado;
}

void AUEscenaFiguraTransformable::establecerColorVertices(UI8 r, UI8 g, UI8 b, UI8 a){
	_colorVertices.r = r;
	_colorVertices.g = g;
	_colorVertices.b = b;
	_colorVertices.a = a;
}

void AUEscenaFiguraTransformable::establecerColorVerticesResaltado(UI8 r, UI8 g, UI8 b, UI8 a){
	_colorVerticesReslatado.r = r;
	_colorVerticesReslatado.g = g;
	_colorVerticesReslatado.b = b;
	_colorVerticesReslatado.a = a;
}

void AUEscenaFiguraTransformable::establecerColorVerticesSeleccionado(UI8 r, UI8 g, UI8 b, UI8 a){
	_colorVerticeSeleccionado.r = r;
	_colorVerticeSeleccionado.g = g;
	_colorVerticeSeleccionado.b = b;
	_colorVerticeSeleccionado.a = a;
}

void AUEscenaFiguraTransformable::dibujarFiguraBase(bool dibujar){
	_dibujarFiguraBase = dibujar;
}

void AUEscenaFiguraTransformable::dibujarEscalaH(bool dibujar){
	_dibujarEscalaH = dibujar;
}

void AUEscenaFiguraTransformable::dibujarEscalaV(bool dibujar){
	_dibujarEscalaV  = dibujar;
}

void AUEscenaFiguraTransformable::dibujarAngulo(bool dibujar){
	_dibujarAngulo  = dibujar;
}

void AUEscenaFiguraTransformable::dibujarCentro(bool dibujar){
	_dibujarCentro  = dibujar;
}

void AUEscenaFiguraTransformable::dibujarReferenciasInternas(bool dibujar){
	_dibujarReferenciasInternas = dibujar;
}

void AUEscenaFiguraTransformable::dibujarVertices(bool dibujar){
	_dibujarVertices = dibujar;
}

bool AUEscenaFiguraTransformable::dibujandoFiguraBase(){
	return _dibujarFiguraBase;
}

bool AUEscenaFiguraTransformable::dibujandoEscalaH(){
	return _dibujarEscalaH;
}

bool AUEscenaFiguraTransformable::dibujandoEscalaV(){
	return _dibujarEscalaV;
}

bool AUEscenaFiguraTransformable::dibujandoAngulo(){
	return _dibujarAngulo;
}

bool AUEscenaFiguraTransformable::dibujandoCentro(){
	return _dibujarCentro;
}

bool AUEscenaFiguraTransformable::dibujandoReferenciasInternas(){
	return _dibujarReferenciasInternas;
}

bool AUEscenaFiguraTransformable::dibujandoVertices(){
	return _dibujarVertices;
}

void AUEscenaFiguraTransformable::sobreIndicadorEscala(float xEscena, float yEscena, bool* guardarEnEscalaHorizontalEn, bool* guardarEnEscalaVerticalEn){
	*guardarEnEscalaHorizontalEn 	= false;
	*guardarEnEscalaVerticalEn		= false;
	if(this->idEscena!=-1 && !this->_dormido && (_dibujarEscalaH || _dibujarEscalaV)){
		//
		NBTamano escalaProyeccion 			= NBGestorEscena::escalaCajaProyeccionOrtogonal(this->idEscena, (ENGestorEscenaGrupo)this->idGrupo);
		NBMatriz ultimaMatriz				= this->matrizEscena();
		NBCajaAABB ultimaCajaEscena			= this->cajaAABBEnEscena();
		NBPunto centroEnEscena;				  NBMATRIZ_MULTIPLICAR_PUNTO(centroEnEscena, ultimaMatriz, 0.0f, 0.0f);
		float distanciaCentroHaciaIzquierda	= centroEnEscena.x - ultimaCajaEscena.xMin;
		float distanciaCentroHaciaDerecha	= ultimaCajaEscena.xMax - centroEnEscena.x;
		float distanciaCentroHaciaArriba	= ultimaCajaEscena.yMax - centroEnEscena.y;
		float distanciaCentroHaciaAbajo		= centroEnEscena.y - ultimaCajaEscena.yMin;
		float distanciaEnX					= (distanciaCentroHaciaIzquierda>distanciaCentroHaciaDerecha?distanciaCentroHaciaIzquierda:distanciaCentroHaciaDerecha);
		float distanciaEnY					= (distanciaCentroHaciaArriba>distanciaCentroHaciaAbajo?distanciaCentroHaciaArriba:distanciaCentroHaciaAbajo);
		float tamanoFlechas					= AU_FIJACION_TAMANO_CAJA_PUNTERO * 2.0f * escalaProyeccion.ancho;
		if(
		   (xEscena>centroEnEscena.x-distanciaEnX-tamanoFlechas && xEscena<centroEnEscena.x-distanciaEnX && yEscena>centroEnEscena.y+distanciaEnY && yEscena<centroEnEscena.y+distanciaEnY+tamanoFlechas) //indicador izq/arr
		   || (xEscena>centroEnEscena.x-distanciaEnX-tamanoFlechas && xEscena<centroEnEscena.x-distanciaEnX && yEscena>centroEnEscena.y-distanciaEnY-tamanoFlechas && yEscena<centroEnEscena.y-distanciaEnY) //indicador izq/aba
		   || (xEscena>centroEnEscena.x+distanciaEnX && xEscena<centroEnEscena.x+distanciaEnX+tamanoFlechas && yEscena>centroEnEscena.y+distanciaEnY && yEscena<centroEnEscena.y+distanciaEnY+tamanoFlechas) //indicador der/arr
		   || (xEscena>centroEnEscena.x+distanciaEnX && xEscena<centroEnEscena.x+distanciaEnX+tamanoFlechas && yEscena>centroEnEscena.y-distanciaEnY-tamanoFlechas && yEscena<centroEnEscena.y-distanciaEnY) //indicador der/aba
		   ){
			*guardarEnEscalaHorizontalEn 	= (true && _dibujarEscalaH);
			*guardarEnEscalaVerticalEn		= (true && _dibujarEscalaV);
		} else {
			if(
			   (xEscena>centroEnEscena.x-distanciaEnX-tamanoFlechas && xEscena<centroEnEscena.x-distanciaEnX && yEscena>centroEnEscena.y-(tamanoFlechas/2.0f) && yEscena<centroEnEscena.y+(tamanoFlechas/2.0f)) //indicador izq
			   || (xEscena>centroEnEscena.x+distanciaEnX && xEscena<centroEnEscena.x+distanciaEnX+tamanoFlechas && yEscena>centroEnEscena.y-(tamanoFlechas/2.0f) && yEscena<centroEnEscena.y+(tamanoFlechas/2.0f)) //indicador der
			   ){
				*guardarEnEscalaHorizontalEn 	= (true && _dibujarEscalaH);
			}
			if(
			   (xEscena>centroEnEscena.x-(tamanoFlechas/2.0f) && xEscena<centroEnEscena.x+(tamanoFlechas/2.0f) && yEscena>centroEnEscena.y+distanciaEnY && yEscena<centroEnEscena.y+distanciaEnY+tamanoFlechas) //indicador arr
			   || (xEscena>centroEnEscena.x-(tamanoFlechas/2.0f) && xEscena<centroEnEscena.x+(tamanoFlechas/2.0f) && yEscena>centroEnEscena.y-distanciaEnY-tamanoFlechas && yEscena<centroEnEscena.y-distanciaEnY) //indicador aba
			   ){
				*guardarEnEscalaVerticalEn		= (true && _dibujarEscalaV);
			}
		}
	}
}

int AUEscenaFiguraTransformable::estableceVerticesCaja(NBVerticeGL* arregloVertsGL, float centroGLX, float centroGLY, float anchoGL, float altoGL){
	float mitadAncho	= anchoGL/2.0f;
	float mitadAlto		= altoGL/2.0f;
	arregloVertsGL[0].x	= centroGLX + mitadAncho; arregloVertsGL[0].y	= centroGLY - mitadAlto;
	arregloVertsGL[1].x	= centroGLX + mitadAncho; arregloVertsGL[1].y	= centroGLY + mitadAlto;
	arregloVertsGL[2].x	= centroGLX - mitadAncho; arregloVertsGL[2].y	= centroGLY + mitadAlto;
	arregloVertsGL[3].x	= centroGLX - mitadAncho; arregloVertsGL[3].y	= centroGLY - mitadAlto;
	return 4;
}

int AUEscenaFiguraTransformable::estableceVerticesCruz(NBVerticeGL* arregloVertsGL, float centroGLX, float centroGLY, float anchoGL, float altoGL){
	float mitadAncho	= anchoGL/2.0f;
	float mitadAlto		= altoGL/2.0f;
	arregloVertsGL[0].x	= centroGLX - mitadAncho;	arregloVertsGL[0].y	= centroGLY;
	arregloVertsGL[1].x	= centroGLX + mitadAncho;	arregloVertsGL[1].y	= centroGLY;
	arregloVertsGL[2].x	= centroGLX;				arregloVertsGL[2].y	= centroGLY + mitadAlto;
	arregloVertsGL[3].x	= centroGLX;				arregloVertsGL[3].y	= centroGLY - mitadAlto;
	return 4;
}

int AUEscenaFiguraTransformable::establecerVerticesFlecha(NBVerticeGL* arregloVertsGL, float centroGLX, float centroGLY, float anchoGL, float altoGL, float angulo){
	float mitadAncho	= anchoGL/2.0f;
	float mitadAlto		= altoGL/2.0f;
	NBPunto puntoIzq, puntoDer, puntoArr, puntoAba;
	//
	puntoIzq.x = centroGLX;					puntoIzq.y = centroGLY;
	puntoDer.x = centroGLX + anchoGL;		puntoDer.y = centroGLY;
	puntoArr.x = centroGLX + mitadAncho;	puntoArr.y = centroGLY + mitadAlto;
	puntoAba.x = centroGLX +mitadAncho;		puntoAba.y = centroGLY - mitadAlto;
	NBPUNTO_ROTAR_EN_EJE(puntoIzq, puntoIzq, angulo, centroGLX, centroGLY);
	NBPUNTO_ROTAR_EN_EJE(puntoDer, puntoDer, angulo, centroGLX, centroGLY);
	NBPUNTO_ROTAR_EN_EJE(puntoArr, puntoArr, angulo, centroGLX, centroGLY);
	NBPUNTO_ROTAR_EN_EJE(puntoAba, puntoAba, angulo, centroGLX, centroGLY);
	arregloVertsGL[0].x	= puntoIzq.x;	arregloVertsGL[0].y	= puntoIzq.y;
	arregloVertsGL[1].x	= puntoDer.x;	arregloVertsGL[1].y	= puntoDer.y;
	arregloVertsGL[2].x	= puntoArr.x;	arregloVertsGL[2].y	= puntoArr.y;
	arregloVertsGL[3].x	= puntoDer.x;	arregloVertsGL[3].y	= puntoDer.y;
	arregloVertsGL[4].x	= puntoAba.x;	arregloVertsGL[4].y	= puntoAba.y;
	arregloVertsGL[5].x	= puntoDer.x;	arregloVertsGL[5].y	= puntoDer.y;
	return 6;
}

int AUEscenaFiguraTransformable::inlineVerticesGLNecesarios(){
	int conteoVerticesNecesitar = 0;
	if(_tipo==ENEscenaFiguraTipo_Linea && _coordenadasLocales.conteo>=2){
		if(_dibujarVertices){
			conteoVerticesNecesitar		+= 4;		//cuadro indicador de vertice inicial
			conteoVerticesNecesitar		+= 4;		//cuadro indicador de vertice final
		}
	} else if(_tipo==ENEscenaFiguraTipo_Circulo && _coordenadasLocales.conteo>=2){
		if(_dibujarVertices)	conteoVerticesNecesitar	+= (_coordenadasLocales.conteo>2?8:4);		//cuadro indicador de radio/circunferencia/angulo1 (y angulo2 si es necesario)
	} else if((_tipo==ENEscenaFiguraTipo_PoligonoCerrado || _tipo==ENEscenaFiguraTipo_SecuenciaLineas) && _coordenadasLocales.conteo>=3){
		if(_dibujarVertices)	conteoVerticesNecesitar	+= (_coordenadasLocales.conteo * 4);	//cuadros indicadores de vertices
	}
	//ELEMENTOS ADICIONALES UNIVERSALES
	if(_dibujarCentro){
		conteoVerticesNecesitar			+= 4;		// caja que encierra al "+" que indica el centro de la fijacion
		conteoVerticesNecesitar			+= 4;		// "+" que indica el centro de la fijacion
	}
	if(_dibujarAngulo){
		conteoVerticesNecesitar			+= 6;		// "->" que indica el angulo de la fijacion
	}
	if(_dibujarReferenciasInternas){
		if((_tipo==ENEscenaFiguraTipo_PoligonoCerrado || _tipo==ENEscenaFiguraTipo_Linea || _tipo==ENEscenaFiguraTipo_SecuenciaLineas) && _coordenadasLocales.conteo>1){
			conteoVerticesNecesitar			+= 6;	// "->" que indica el sentido de los vertices
		}
	}
	if(_dibujarEscalaH && _dibujarEscalaV){
		conteoVerticesNecesitar			+= 6 * 8;	// "->" que indican la escalacion (2 verticales, 2 horizontales y 4 diagonales)
	} else if(_dibujarEscalaH){
		conteoVerticesNecesitar			+= 6 * 2;	// "->" que indican la escalacion horizontales
	} else if(_dibujarEscalaV){
		conteoVerticesNecesitar			+= 6 * 2;	// "->" que indican la escalacion verticales
	}
	//
	return conteoVerticesNecesitar;
}

void AUEscenaFiguraTransformable::inlineActualizarModelo(const NBPropRenderizado &propsRenderizado, const STBloqueGL &bloqueVerticesGL){
	NBASSERT(idEscena!=-1) //Solo si esta en escena
	NBTamano escalaProyeccion	= NBGestorEscena::escalaCajaProyeccionOrtogonal(this->idEscena, (ENGestorEscenaGrupo)this->idGrupo);
	NBVerticeGL* verticesGL		= &((*propsRenderizado.verticesGL1)[bloqueVerticesGL.primerElemento]);
	int posEnVertices			= 0;
	if(_tipo==ENEscenaFiguraTipo_Linea){
		if(_coordenadasLocales.conteo<2){
			PRINTF_INFO("La linea no cuenta con dos vertices minimo\n");
		} else {
			if(_dibujarVertices){
				NBPunto pos1; NBMATRIZ_MULTIPLICAR_PUNTO(pos1, _cacheObjEscena.matrizEscena, _coordenadasLocales.elemento[0].coordenada.x, _coordenadasLocales.elemento[0].coordenada.y);
				NBPunto pos2; NBMATRIZ_MULTIPLICAR_PUNTO(pos2, _cacheObjEscena.matrizEscena, _coordenadasLocales.elemento[1].coordenada.x, _coordenadasLocales.elemento[1].coordenada.y);
				posEnVertices += estableceVerticesCaja(&(verticesGL[posEnVertices]), pos1.x, pos1.y, AU_FIJACION_TAMANO_CAJA_PUNTERO*escalaProyeccion.ancho, AU_FIJACION_TAMANO_CAJA_PUNTERO*escalaProyeccion.alto);
				posEnVertices += estableceVerticesCaja(&(verticesGL[posEnVertices]), pos2.x, pos2.y, AU_FIJACION_TAMANO_CAJA_PUNTERO*escalaProyeccion.ancho, AU_FIJACION_TAMANO_CAJA_PUNTERO*escalaProyeccion.alto);
			}
		}
	} else if(_tipo==ENEscenaFiguraTipo_Circulo){
		if(_coordenadasLocales.conteo<2){
			PRINTF_INFO("El circulo no cuenta con dos vertices\n");
		} else {
			if(_dibujarVertices){
				if(_coordenadasLocales.conteo==2){
					NBPunto circun			= _coordenadasLocales.elemento[1].coordenada;
					NBPunto punto1GL;		  NBMATRIZ_MULTIPLICAR_PUNTO(punto1GL, _cacheObjEscena.matrizEscena, circun.x, circun.y);
					posEnVertices			+= estableceVerticesCaja(&(verticesGL[posEnVertices]), punto1GL.x, punto1GL.y, AU_FIJACION_TAMANO_CAJA_PUNTERO*escalaProyeccion.ancho, AU_FIJACION_TAMANO_CAJA_PUNTERO*escalaProyeccion.alto);
				} else {
					NBPunto centro			= _coordenadasLocales.elemento[0].coordenada;
					NBPunto vAngulo1		= _coordenadasLocales.elemento[1].coordenada;
					NBPunto vAngulo2		= _coordenadasLocales.elemento[2].coordenada;
					float radio				= NBPUNTO_DISTANCIA(centro.x, centro.y, vAngulo1.x, vAngulo1.y);
					float angulo1;			  NBPUNTO_ANGULO_VECTOR(angulo1, centro.x, centro.y, vAngulo1.x, vAngulo1.y);
					float angulo2;			  NBPUNTO_ANGULO_VECTOR(angulo2, centro.x, centro.y, vAngulo2.x, vAngulo2.y);
					NBPunto circunNormal;	  NBPUNTO_ESTABLECER(circunNormal, radio, 0.0f);
					NBPunto v1Ajustado;		  NBPUNTO_ROTAR(v1Ajustado, circunNormal, angulo1);
					NBPunto v2Ajustado;		  NBPUNTO_ROTAR(v2Ajustado, circunNormal, angulo2);
					NBPunto circunLocal1;	  NBPUNTO_ESTABLECER(circunLocal1, centro.x + v1Ajustado.x, centro.y + v1Ajustado.y)
					NBPunto circunLocal2;	  NBPUNTO_ESTABLECER(circunLocal2, centro.x + v2Ajustado.x, centro.y + v2Ajustado.y)
					NBPunto punto1GL;		  NBMATRIZ_MULTIPLICAR_PUNTO(punto1GL, _cacheObjEscena.matrizEscena, circunLocal1.x, circunLocal1.y);
					NBPunto punto2GL;		  NBMATRIZ_MULTIPLICAR_PUNTO(punto2GL, _cacheObjEscena.matrizEscena, circunLocal2.x, circunLocal2.y);
					posEnVertices += estableceVerticesCaja(&(verticesGL[posEnVertices]), punto1GL.x, punto1GL.y, AU_FIJACION_TAMANO_CAJA_PUNTERO*escalaProyeccion.ancho, AU_FIJACION_TAMANO_CAJA_PUNTERO*escalaProyeccion.alto);
					posEnVertices += estableceVerticesCaja(&(verticesGL[posEnVertices]), punto2GL.x, punto2GL.y, AU_FIJACION_TAMANO_CAJA_PUNTERO*escalaProyeccion.ancho, AU_FIJACION_TAMANO_CAJA_PUNTERO*escalaProyeccion.alto);
				}
			}
		}
	} else if(_tipo==ENEscenaFiguraTipo_PoligonoCerrado || _tipo==ENEscenaFiguraTipo_SecuenciaLineas){
		if(_coordenadasLocales.conteo<3){
			PRINTF_ERROR("el poligono no cuenta con tres o mas vertices\n");
		} else {
			if(_dibujarVertices){
				UI32 i, iConteo = _coordenadasLocales.conteo;
				for(i=0; i<iConteo; i++){
					NBPunto puntoGL; NBMATRIZ_MULTIPLICAR_PUNTO(puntoGL, _cacheObjEscena.matrizEscena, _coordenadasLocales.elemento[i].coordenada.x, _coordenadasLocales.elemento[i].coordenada.y);
					posEnVertices += estableceVerticesCaja(&(verticesGL[posEnVertices]), puntoGL.x, puntoGL.y, AU_FIJACION_TAMANO_CAJA_PUNTERO*escalaProyeccion.ancho, AU_FIJACION_TAMANO_CAJA_PUNTERO*escalaProyeccion.alto);
				}
			}
		}
	}
	//ELEMENTOS ADICIONALES UNIVERSALES DEL MODELO
	NBPunto centro; NBMATRIZ_MULTIPLICAR_PUNTO(centro, _cacheObjEscena.matrizEscena, 0.0f, 0.0f);
	if(_dibujarCentro){
		//caja que encierra al "+" del centro
		posEnVertices	+= estableceVerticesCaja(&(verticesGL[posEnVertices]), centro.x, centro.y, AU_FIJACION_TAMANO_CAJA_PUNTERO * escalaProyeccion.ancho, AU_FIJACION_TAMANO_CAJA_PUNTERO * escalaProyeccion.alto);
		//"+" del centro
		posEnVertices	+= estableceVerticesCruz(&(verticesGL[posEnVertices]), centro.x, centro.y, AU_FIJACION_TAMANO_CAJA_PUNTERO*escalaProyeccion.ancho, AU_FIJACION_TAMANO_CAJA_PUNTERO*escalaProyeccion.alto);
	}
	if(_dibujarAngulo){
		//Flecha de rotacion
		posEnVertices += establecerVerticesFlecha(&(verticesGL[posEnVertices]), centro.x, centro.y, AU_FIJACION_TAMANO_CAJA_PUNTERO*1.5f*escalaProyeccion.ancho, AU_FIJACION_TAMANO_CAJA_PUNTERO*escalaProyeccion.alto, this->rotacion());
	}
	if(_dibujarReferenciasInternas){
		//Flecha se sentido de primer segmento
		if((_tipo==ENEscenaFiguraTipo_PoligonoCerrado || _tipo==ENEscenaFiguraTipo_Linea || _tipo==ENEscenaFiguraTipo_SecuenciaLineas) && _coordenadasLocales.conteo>1){
			NBPunto iniPrimerSegmento; NBMATRIZ_MULTIPLICAR_PUNTO(iniPrimerSegmento, _cacheObjEscena.matrizEscena, _coordenadasLocales.elemento[0].coordenada.x, _coordenadasLocales.elemento[0].coordenada.y);
			NBPunto finPrimerSegmento; NBMATRIZ_MULTIPLICAR_PUNTO(finPrimerSegmento, _cacheObjEscena.matrizEscena, _coordenadasLocales.elemento[1].coordenada.x, _coordenadasLocales.elemento[1].coordenada.y);
			float anguloPrimerSegmento;	NBPUNTO_ANGULO_VECTOR(anguloPrimerSegmento, iniPrimerSegmento.x, iniPrimerSegmento.y, finPrimerSegmento.x, finPrimerSegmento.y);
			posEnVertices += establecerVerticesFlecha(&(verticesGL[posEnVertices]), iniPrimerSegmento.x, iniPrimerSegmento.y, AU_FIJACION_TAMANO_CAJA_PUNTERO*1.5f*escalaProyeccion.ancho, AU_FIJACION_TAMANO_CAJA_PUNTERO*escalaProyeccion.alto, anguloPrimerSegmento);
		}
	}
	//Flechas que encierran la caja (escalacion)
	if(_dibujarEscalaH || _dibujarEscalaV){
		NBCajaAABB cajaEscena				= this->cajaAABBEnEscena();
		NBPunto centroEnEscena;				NBMATRIZ_MULTIPLICAR_PUNTO(centroEnEscena, _cacheObjEscena.matrizEscena, 0.0f, 0.0f);
		float distanciaCentroHaciaIzquierda	= centroEnEscena.x - cajaEscena.xMin;
		float distanciaCentroHaciaDerecha	= cajaEscena.xMax - centroEnEscena.x;
		float distanciaCentroHaciaArriba	= cajaEscena.yMax - centroEnEscena.y;
		float distanciaCentroHaciaAbajo		= centroEnEscena.y - cajaEscena.yMin;
		float distanciaEnX					= (distanciaCentroHaciaIzquierda>distanciaCentroHaciaDerecha?distanciaCentroHaciaIzquierda:distanciaCentroHaciaDerecha);
		float distanciaEnY					= (distanciaCentroHaciaArriba>distanciaCentroHaciaAbajo?distanciaCentroHaciaArriba:distanciaCentroHaciaAbajo);
		//flechas de escalacion diagonales
		if(_dibujarEscalaH && _dibujarEscalaV){
			posEnVertices += establecerVerticesFlecha(&(verticesGL[posEnVertices]), centroEnEscena.x+distanciaEnX, centroEnEscena.y+distanciaEnY, AU_FIJACION_TAMANO_CAJA_PUNTERO*2.0f*escalaProyeccion.ancho, AU_FIJACION_TAMANO_CAJA_PUNTERO*escalaProyeccion.alto, 45.0f);
			posEnVertices += establecerVerticesFlecha(&(verticesGL[posEnVertices]), centroEnEscena.x-distanciaEnX, centroEnEscena.y+distanciaEnY, AU_FIJACION_TAMANO_CAJA_PUNTERO*2.0f*escalaProyeccion.ancho, AU_FIJACION_TAMANO_CAJA_PUNTERO*escalaProyeccion.alto, 135.0f);
			posEnVertices += establecerVerticesFlecha(&(verticesGL[posEnVertices]), centroEnEscena.x-distanciaEnX, centroEnEscena.y-distanciaEnY, AU_FIJACION_TAMANO_CAJA_PUNTERO*2.0f*escalaProyeccion.ancho, AU_FIJACION_TAMANO_CAJA_PUNTERO*escalaProyeccion.alto, 225.0f);
			posEnVertices += establecerVerticesFlecha(&(verticesGL[posEnVertices]), centroEnEscena.x+distanciaEnX, centroEnEscena.y-distanciaEnY, AU_FIJACION_TAMANO_CAJA_PUNTERO*2.0f*escalaProyeccion.ancho, AU_FIJACION_TAMANO_CAJA_PUNTERO*escalaProyeccion.alto, 315.0f);
		}
		//flechas de escalacion verticales u horizontales
		if(_dibujarEscalaH){
			posEnVertices += establecerVerticesFlecha(&(verticesGL[posEnVertices]), centroEnEscena.x+distanciaEnX, centroEnEscena.y, AU_FIJACION_TAMANO_CAJA_PUNTERO*2.0f*escalaProyeccion.ancho, AU_FIJACION_TAMANO_CAJA_PUNTERO*escalaProyeccion.alto, 0.0f);
			posEnVertices += establecerVerticesFlecha(&(verticesGL[posEnVertices]), centroEnEscena.x-distanciaEnX, centroEnEscena.y, AU_FIJACION_TAMANO_CAJA_PUNTERO*2.0f*escalaProyeccion.ancho, AU_FIJACION_TAMANO_CAJA_PUNTERO*escalaProyeccion.alto, 180.0f);
		}
		if(_dibujarEscalaV){
			posEnVertices += establecerVerticesFlecha(&(verticesGL[posEnVertices]), centroEnEscena.x, centroEnEscena.y+distanciaEnY, AU_FIJACION_TAMANO_CAJA_PUNTERO*2.0f*escalaProyeccion.ancho, AU_FIJACION_TAMANO_CAJA_PUNTERO*escalaProyeccion.alto, 90.0f);
			posEnVertices += establecerVerticesFlecha(&(verticesGL[posEnVertices]), centroEnEscena.x, centroEnEscena.y-distanciaEnY, AU_FIJACION_TAMANO_CAJA_PUNTERO*2.0f*escalaProyeccion.ancho, AU_FIJACION_TAMANO_CAJA_PUNTERO*escalaProyeccion.alto, 270.0f);
		}
	}
}

int AUEscenaFiguraTransformable::inlineEstablecerColoresVertices(NBVerticeGL* verticesGL, int indicePrimero, int cantidad, const NBColor &colorDestino){
	int i, indicePosteriorUltimo = indicePrimero + cantidad;
	for(i=indicePrimero; i<indicePosteriorUltimo; i++){
		verticesGL[i].r = 255*colorDestino.r;
		verticesGL[i].g = 255*colorDestino.g;
		verticesGL[i].b = 255*colorDestino.b;
		verticesGL[i].a = 255*colorDestino.a;
	}
	return cantidad;
}

void AUEscenaFiguraTransformable::inlineAplicarColoresEspecificosModelo(NBVerticeGL* verticesGL){
	int posVertice = 0;
	if(_tipo==ENEscenaFiguraTipo_Linea && _coordenadasLocales.conteo>=2){
		if(_dibujarVertices){
			posVertice += inlineEstablecerColoresVertices(verticesGL, posVertice, 4, _indiceVerticeSel==0?_colorVerticeSeleccionado:_indiceVerticeReslatado==0?_colorVerticesReslatado:_colorVertices);
			posVertice += inlineEstablecerColoresVertices(verticesGL, posVertice, 4, _indiceVerticeSel==1?_colorVerticeSeleccionado:_indiceVerticeReslatado==1?_colorVerticesReslatado:_colorVertices);
		}
	} else if(_tipo==ENEscenaFiguraTipo_Circulo && _coordenadasLocales.conteo>=2){
		if(_dibujarVertices){
			posVertice += inlineEstablecerColoresVertices(verticesGL, posVertice, 4, _indiceVerticeSel==1?_colorVerticeSeleccionado:_indiceVerticeReslatado==1?_colorVerticesReslatado:_colorVertices);
			if(_coordenadasLocales.conteo>2) posVertice += inlineEstablecerColoresVertices(verticesGL, posVertice, 4, _indiceVerticeSel==2?_colorVerticeSeleccionado:_indiceVerticeReslatado==2?_colorVerticesReslatado:_colorVertices);
		}
	} else if((_tipo==ENEscenaFiguraTipo_PoligonoCerrado || _tipo==ENEscenaFiguraTipo_SecuenciaLineas) && _coordenadasLocales.conteo>=3){
		if(_dibujarVertices){
			UI32 iVert, iVertConteo = _coordenadasLocales.conteo;
			for(iVert=0; iVert<iVertConteo; iVert++){
				posVertice += inlineEstablecerColoresVertices(verticesGL, posVertice, 4, _indiceVerticeSel==(SI32)iVert?_colorVerticeSeleccionado:_indiceVerticeReslatado==(SI32)iVert?_colorVerticesReslatado:_colorVertices);
			}
		}
	}
	//ELEMENTOS ADICIONALES UNIVERSALES
	NBColor colorCajaCentro; NBCOLOR_ESTABLECER(colorCajaCentro, 0.0f, 1.0f, 0.0f, 1.0f)
	if(_dibujarCentro){
		posVertice += inlineEstablecerColoresVertices(verticesGL, posVertice, 4, colorCajaCentro);	//caja del centro
		posVertice += inlineEstablecerColoresVertices(verticesGL, posVertice, 4, _centroResaltado?_colorVerticesReslatado:_colorTransformaciones); //"+" del centro
	}
	if(_dibujarAngulo){
		posVertice += inlineEstablecerColoresVertices(verticesGL, posVertice, 6, _anguloResaltado?_colorVerticesReslatado:_colorTransformaciones); //"->" del angulo
	}
	if(_dibujarReferenciasInternas){
		if((_tipo==ENEscenaFiguraTipo_PoligonoCerrado || _tipo==ENEscenaFiguraTipo_Linea || _tipo==ENEscenaFiguraTipo_SecuenciaLineas) && _coordenadasLocales.conteo>1){
			posVertice += inlineEstablecerColoresVertices(verticesGL, posVertice, 6, colorCajaCentro); // "->" que indica el sentido de los vertices
		}
	}
	if(_dibujarEscalaH || _dibujarEscalaV){
		if(_dibujarEscalaH && _dibujarEscalaV) posVertice += inlineEstablecerColoresVertices(verticesGL, posVertice, 6*4, (_escalaHorizontalResaltada && _escalaVerticalResaltada?_colorVerticesReslatado:_colorTransformaciones));
		if(_dibujarEscalaH) posVertice += inlineEstablecerColoresVertices(verticesGL, posVertice, 6*2, (_escalaHorizontalResaltada && !_escalaVerticalResaltada?_colorVerticesReslatado:_colorTransformaciones));
		if(_dibujarEscalaV) posVertice += inlineEstablecerColoresVertices(verticesGL, posVertice, 6*2, (!_escalaHorizontalResaltada && _escalaVerticalResaltada?_colorVerticesReslatado:_colorTransformaciones));
	}
}

PTRfunCmdsGL AUEscenaFiguraTransformable::actualizarModeloGL(const NBPropRenderizado &propsRenderizado, const NBPropIluminacion &propsIluminacion, const NBPropHeredadasRender &propsHeredadas){
	SI32 cantVerticesNecesarios			= inlineVerticesGLNecesarios();
	STBloqueGL bloqueVerticesGL;
	bloqueVerticesGL.cantidadElementos	= cantVerticesNecesarios;
	bloqueVerticesGL.primerElemento		= (*propsRenderizado.funcReservarVertices)(ENVerticeGlTipo_MonoTextura, cantVerticesNecesarios);
	STBloqueGL bloqueIndicesGL;
	bloqueIndicesGL.primerElemento		= bloqueIndicesGL.cantidadElementos = 0;
	//Formatear vertices GL
	SI32 iVer; NBVerticeGL* verticesGL	= &((*propsRenderizado.verticesGL1)[bloqueVerticesGL.primerElemento]);
	for(iVer=0; iVer<bloqueVerticesGL.cantidadElementos; iVer++){
		NBPUNTO_ESTABLECER(verticesGL[iVer], 0.0f, 0.0f);
		NBCOLOR_ESTABLECER(verticesGL[iVer], 255, 255, 255, 255);
		NBPUNTO_ESTABLECER(verticesGL[iVer].tex, 0.0f, 0.0f);
	}
	inlineActualizarModelo(propsRenderizado, bloqueVerticesGL);
	inlineAplicarColoresEspecificosModelo(&((*propsRenderizado.verticesGL1)[bloqueVerticesGL.primerElemento]));
	//
	const SI32 posicionDatos				= propsRenderizado.bytesDatosModelos->conteo; propsRenderizado.bytesDatosModelos->reservarRegistrosAlFinal(sizeof(STFiguraTTRender));
	STFiguraTTRender* propsRender			= (STFiguraTTRender*)&(propsRenderizado.bytesDatosModelos->elemento[posicionDatos]);
	propsRender->tipo						= (ENEscenaFiguraTipo)_tipo;
	propsRender->dibujarFiguraBase			= _dibujarFiguraBase;
	propsRender->dibujarEscalaH				= _dibujarEscalaH;
	propsRender->dibujarEscalaV				= _dibujarEscalaV;
	propsRender->dibujarReferenciasInternas	= _dibujarReferenciasInternas;
	propsRender->dibujarAngulo				= _dibujarAngulo;
	propsRender->dibujarCentro				= _dibujarCentro;
	propsRender->dibujarVertices			= _dibujarVertices;
	propsRender->cantidadCoordenadasLocales	= _coordenadasLocales.conteo;
	propsRender->bloqueVerticesGL			= bloqueVerticesGL;
	if(_dibujarFiguraBase) AUEscenaFigura::actualizarModeloGL(propsRenderizado, propsIluminacion, propsHeredadas);
	return &AUEscenaFiguraTransformable::enviarComandosGL;
}

void AUEscenaFiguraTransformable::enviarComandosGL(UI8* punteroDatosModelo){
	STFiguraTTRender* propsRender = (STFiguraTTRender*)punteroDatosModelo;
	if(propsRender->bloqueVerticesGL.cantidadElementos!=0){
		NBGestorGL::activarVerticesGL(ENVerticeGlTipo_MonoTextura); NBGESTORGL_DBG_NOMBRAR_ACTIVADOR_VERTICES_GL("AUEscenaFiguraTransformable")
		NBGestorGL::bindTexture(0, NBGestorTexturas::texturaBlanca->idTexturaGL); //required on some systems where texture-0 will not be drawn
		int posEnVertices = propsRender->bloqueVerticesGL.primerElemento;
		if(propsRender->tipo==ENEscenaFiguraTipo_Linea && propsRender->cantidadCoordenadasLocales>=2){
			if(propsRender->dibujarVertices){
				NBGestorGL::drawArrays(GL_LINE_LOOP, posEnVertices, 4);		posEnVertices+=4; NBGESTORGL_DBG_NOMBRAR_CONVOCADOR_DRAW_ARRAY("AUEscenaFiguraTransformable(0)")		//vertices de la linea
				NBGestorGL::drawArrays(GL_LINE_LOOP, posEnVertices, 4);		posEnVertices+=4; NBGESTORGL_DBG_NOMBRAR_CONVOCADOR_DRAW_ARRAY("AUEscenaFiguraTransformable(1)")		//vertices de la linea
			}
		} else if(propsRender->tipo==ENEscenaFiguraTipo_Circulo && propsRender->cantidadCoordenadasLocales>=2){
			if(propsRender->dibujarVertices){
				NBGestorGL::drawArrays(GL_LINE_LOOP, posEnVertices, 4);		posEnVertices+=4; NBGESTORGL_DBG_NOMBRAR_CONVOCADOR_DRAW_ARRAY("AUEscenaFiguraTransformable(2)")		//vertice de circunferencia y angulo1
				if(propsRender->cantidadCoordenadasLocales>2)
					NBGestorGL::drawArrays(GL_LINE_LOOP, posEnVertices, 4);	posEnVertices+=4; NBGESTORGL_DBG_NOMBRAR_CONVOCADOR_DRAW_ARRAY("AUEscenaFiguraTransformable(3)")		//angulo2
			}
		} else if((propsRender->tipo==ENEscenaFiguraTipo_PoligonoCerrado || propsRender->tipo==ENEscenaFiguraTipo_SecuenciaLineas) && propsRender->cantidadCoordenadasLocales>=3){
			if(propsRender->dibujarVertices){
				int i;
				for(i=0; i<propsRender->cantidadCoordenadasLocales; i++){
					NBGestorGL::drawArrays(GL_LINE_LOOP, posEnVertices, 4);	posEnVertices += 4; NBGESTORGL_DBG_NOMBRAR_CONVOCADOR_DRAW_ARRAY("AUEscenaFiguraTransformable(4)")	//vertices de poligono
				}
			}
		}
		//Flecha angulo
		if(propsRender->dibujarCentro){
			NBGestorGL::drawArrays(GL_TRIANGLE_STRIP, posEnVertices, 4); posEnVertices += 4; NBGESTORGL_DBG_NOMBRAR_CONVOCADOR_DRAW_ARRAY("AUEscenaFigura(0)")	//cuadro de centro (4)
			NBGestorGL::drawArrays(GL_LINES, posEnVertices, 4); posEnVertices += 4; NBGESTORGL_DBG_NOMBRAR_CONVOCADOR_DRAW_ARRAY("AUEscenaFigura(1)")			//"+" de centro (4), "->" de angulo (6)
		}
		if(propsRender->dibujarAngulo){
			NBGestorGL::drawArrays(GL_LINES, posEnVertices, 6);			posEnVertices += 6;	NBGESTORGL_DBG_NOMBRAR_CONVOCADOR_DRAW_ARRAY("AUEscenaFigura(1)")		//"+" de centro (4), "->" de angulo (6)
		}
		//Flecha se sentido de primer segmento
		if(propsRender->dibujarReferenciasInternas){
			if((propsRender->tipo==ENEscenaFiguraTipo_PoligonoCerrado || propsRender->tipo==ENEscenaFiguraTipo_Linea || propsRender->tipo==ENEscenaFiguraTipo_SecuenciaLineas) && propsRender->cantidadCoordenadasLocales>1){
				NBGestorGL::drawArrays(GL_LINES, posEnVertices, 6);	posEnVertices+=6; NBGESTORGL_DBG_NOMBRAR_CONVOCADOR_DRAW_ARRAY("AUEscenaFigura(1)")				//"->" sentido del primer segmento
			}
		}
		//Flechas que encierran la caja (escalacion)
		if(propsRender->dibujarEscalaH && propsRender->dibujarEscalaV){
			NBGestorGL::drawArrays(GL_LINES, posEnVertices, 6*8);		posEnVertices+=(6*8);	NBGESTORGL_DBG_NOMBRAR_CONVOCADOR_DRAW_ARRAY("AUEscenaFigura(1)")		//"->" x 8 de escalacion
		} else if(propsRender->dibujarEscalaH){
			NBGestorGL::drawArrays(GL_LINES, posEnVertices, 6*2);		posEnVertices+=(6*2);	NBGESTORGL_DBG_NOMBRAR_CONVOCADOR_DRAW_ARRAY("AUEscenaFigura(1)")		//"->" x 2 de escalacion H
		} else if(propsRender->dibujarEscalaV){
			NBGestorGL::drawArrays(GL_LINES, posEnVertices, 6*2);		posEnVertices+=(6*2);	NBGESTORGL_DBG_NOMBRAR_CONVOCADOR_DRAW_ARRAY("AUEscenaFigura(1)")		//"->" x 2 de escalacion V
		}
	}
	//
	if(propsRender->dibujarFiguraBase){
		AUEscenaFigura::enviarComandosGL(punteroDatosModelo + sizeof(STFiguraTTRender));
	}
}

//
AUOBJMETODOS_CLASESID_MULTICLASE(AUEscenaFiguraTransformable, AUEscenaFigura)
AUOBJMETODOS_CLASESNOMBRES_MULTICLASE(AUEscenaFiguraTransformable, "AUEscenaFiguraTransformable", AUEscenaFigura)
AUOBJMETODOS_CLONAR_THIS(AUEscenaFiguraTransformable)








