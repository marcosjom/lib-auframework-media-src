//
//  AUEscenaGrupoParticulasLinea.cpp
//  Gameplay_Mac
//
//  Created by Marcos Ortega Morales on 12/10/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkMediaStdAfx.h"
#include "AUEscenaGrupoParticulasLinea.h"

AUEscenaGrupoParticulasLinea::AUEscenaGrupoParticulasLinea() : AUEscenaObjeto(), _datosParticulas(this), _datosAnimHumo(this) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaGrupoParticulasLinea::AUEscenaGrupoParticulasLinea")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaGrupoParticulasLinea")
	_texturaParticula		= NBGestorTexturas::texturaDesdeArchivo("Escenarios/Particulas/objBurbuja.png"); _texturaParticula->retener(NB_RETENEDOR_THIS);
	_texturaTamano			= _texturaParticula->tamanoHD();
	//
	_segsDesaparecen		= 0.5f;
	_escalaInicial			= 0.75f;
	_velocidadBurbuja		= 64.0f;
	_escalaParaDisipar		= 3.0f;
	_particulasPorSegundoPorMetro = 6.0f;
	_longitudHaciaDerecha	= 256.0f;
	//
	_segsAcumSinGenerar		= 0.0f;
	//
	_partsCajaLocalSucia	= true;
	NBCAJAAABB_INICIALIZAR(_partsCajaLocal);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

AUEscenaGrupoParticulasLinea::AUEscenaGrupoParticulasLinea(AUEscenaGrupoParticulasLinea* otraInstancia) : AUEscenaObjeto(otraInstancia), _datosParticulas(this), _datosAnimHumo(this) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaGrupoParticulasLinea::AUEscenaGrupoParticulasLinea")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaGrupoParticulasLinea")
	_texturaParticula				= otraInstancia->_texturaParticula; _texturaParticula->retener(NB_RETENEDOR_THIS);
	_texturaTamano					= _texturaParticula->tamanoHD();
	//
	_segsDesaparecen				= otraInstancia->_segsDesaparecen;
	_escalaInicial					= otraInstancia->_escalaInicial;
	_velocidadBurbuja				= otraInstancia->_velocidadBurbuja;
	_escalaParaDisipar				= otraInstancia->_escalaParaDisipar;
	_particulasPorSegundoPorMetro	= otraInstancia->_particulasPorSegundoPorMetro;
	_longitudHaciaDerecha			= otraInstancia->_longitudHaciaDerecha;
	//
	_segsAcumSinGenerar				= 0.0f;
	//
	_partsCajaLocalSucia	= true;
	NBCAJAAABB_INICIALIZAR(_partsCajaLocal);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

AUEscenaGrupoParticulasLinea::~AUEscenaGrupoParticulasLinea(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaGrupoParticulasLinea::~AUEscenaGrupoParticulasLinea")
	if(_texturaParticula != NULL) _texturaParticula->liberar(NB_RETENEDOR_THIS); _texturaParticula = NULL;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

//

UI16 AUEscenaGrupoParticulasLinea::conteoParticulasHumo(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaGrupoParticulasLinea::conteoParticulasHumo")
	NBASSERT(_datosAnimHumo.conteo==_datosParticulas.conteo)
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _datosParticulas.conteo;
}

float AUEscenaGrupoParticulasLinea::longitudHaciaDerecha(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaGrupoParticulasLinea::longitudHaciaDerecha")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _longitudHaciaDerecha;
}

float AUEscenaGrupoParticulasLinea::particulasPorSegundoPorMetro(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaGrupoParticulasLinea::particulasPorSegundoPorMetro")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _particulasPorSegundoPorMetro;
}

float AUEscenaGrupoParticulasLinea::segsDesaparecen(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaGrupoParticulasLinea::segsDesaparecen")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _segsDesaparecen;
}

float AUEscenaGrupoParticulasLinea::velocidadBurbuja(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaGrupoParticulasLinea::velocidadBurbuja")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _velocidadBurbuja;
}

float AUEscenaGrupoParticulasLinea::escalaInicial(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaGrupoParticulasLinea::escalaInicial")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _escalaInicial;
}

float AUEscenaGrupoParticulasLinea::escalaParaDisipar(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaGrupoParticulasLinea::escalaParaDisipar")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _escalaParaDisipar;
}

void AUEscenaGrupoParticulasLinea::establecerLongitudHaciaDerecha(float longitud){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaGrupoParticulasLinea::establecerLongitudHaciaDerecha")
	NBASSERT(longitud>0.0f);
	_longitudHaciaDerecha = longitud;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaGrupoParticulasLinea::establecerParticulasPorSegundoPorMetro(float particulasPorSegundoPorMetro){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaGrupoParticulasLinea::establecerParticulasPorSegundoPorMetro")
	NBASSERT(particulasPorSegundoPorMetro>=0.0f);
	_particulasPorSegundoPorMetro = particulasPorSegundoPorMetro;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaGrupoParticulasLinea::establecerSegsDesaparecen(float segsDesaparecen){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaGrupoParticulasLinea::establecerSegsDesaparecen")
	NBASSERT(segsDesaparecen>0.0f);
	_segsDesaparecen = segsDesaparecen;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaGrupoParticulasLinea::establecerVelocidadBurbuja(float velocidadBurbuja){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaGrupoParticulasLinea::establecerVelocidadBurbuja")
	NBASSERT(velocidadBurbuja>=0.0f);
	_velocidadBurbuja = velocidadBurbuja;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaGrupoParticulasLinea::establecerEscalaInicial(float escalaInicial){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaGrupoParticulasLinea::establecerEscalaInicial")
	NBASSERT(escalaInicial>0.0f);
	_escalaInicial = escalaInicial;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaGrupoParticulasLinea::establecerEscalaParaDisipar(float escalaParaDisipar){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaGrupoParticulasLinea::establecerEscalaParaDisipar")
	NBASSERT(escalaParaDisipar>0.0f);
	_escalaParaDisipar = escalaParaDisipar;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaGrupoParticulasLinea::privAgregarParticulaHumo(float posX, float posY){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaGrupoParticulasLinea::agregarParticulaHoja")
	NBASSERT(_texturaParticula != NULL)
	STParticulaAnimHumo datosAnimacion;
	datosAnimacion.escalaInicial				= _escalaInicial;
	datosAnimacion.escalaParaDisipacion			= _escalaParaDisipar;
	datosAnimacion.factorResistenciaAire		= 0.0f;
	datosAnimacion.segundosViva					= 0.0f;
	datosAnimacion.segundosVive					= _segsDesaparecen;
	datosAnimacion.velLinealGravedad.x			= 0.0f;
	datosAnimacion.velLinealGravedad.y			= 0.0f;
	datosAnimacion.velLinealViento.x			= 0.0f;
	datosAnimacion.velLinealViento.y			= _velocidadBurbuja;
	datosAnimacion.velRadAngular				= 0.0f;
	_datosAnimHumo.agregarElemento(datosAnimacion);
	//
	NBPropiedadesEnEscena propsParticulas;
	propsParticulas.visible						= true;
	propsParticulas.transformaciones.trasladoX	= posX;
	propsParticulas.transformaciones.trasladoY	= posY;
	propsParticulas.transformaciones.escalaX	= _escalaInicial;
	propsParticulas.transformaciones.escalaY	= _escalaInicial;
	propsParticulas.transformaciones.rotacion	= 0.0f;
	propsParticulas.color8.r					= 255;
	propsParticulas.color8.g					= 255;
	propsParticulas.color8.b					= 255;
	propsParticulas.color8.a					= 255;
	_datosParticulas.agregarElemento(propsParticulas);
	//
	_partsCajaLocalSucia						= true;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

//

void AUEscenaGrupoParticulasLinea::tickAnimacion(float segsTranscurridos){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaGrupoParticulasLinea::tickAnimacion")
	NBASSERT(_datosParticulas.conteo==_datosAnimHumo.conteo)
	if(!this->_dormido){
		//GENERAR PARTICULAS
		_segsAcumSinGenerar			+= segsTranscurridos;
		float segundosPorParticula	= 1.0f / (float)_particulasPorSegundoPorMetro;
		UI16 particulasGenerar		= (UI16)(_segsAcumSinGenerar / segundosPorParticula);
		if(particulasGenerar!=0){
			_segsAcumSinGenerar			-= particulasGenerar * segundosPorParticula; NBASSERT(_segsAcumSinGenerar>=0.0f)
			float posEnLongitud			= 0.0f;
			float longitudFaltante		= _longitudHaciaDerecha;
			float lontigudRestar;
			while(longitudFaltante!=0.0f){
				lontigudRestar			= (longitudFaltante<ESCALA_PIXELES_POR_METRO_MUNDO_FISICA?longitudFaltante:ESCALA_PIXELES_POR_METRO_MUNDO_FISICA);
				UI16 iPart, conteoParts = _particulasPorSegundoPorMetro;
				for(iPart=0; iPart<conteoParts; iPart++){
					privAgregarParticulaHumo(posEnLongitud + (((rand() % 1000) / 1000.0f) * lontigudRestar), 0.0f);
				}
				posEnLongitud			+= lontigudRestar;
				longitudFaltante		-= lontigudRestar;
			}
		}
		//ANIMAR PARTICULAS
		UI16 i;
		for(i=0; i<_datosAnimHumo.conteo; i++){
			NBPropiedadesEnEscena* datosPart		= &_datosParticulas.elemento[i];
			STParticulaAnimHumo* datosAnim			= &_datosAnimHumo.elemento[i];
			datosPart->transformaciones.trasladoX	+= (datosAnim->velLinealGravedad.x+datosAnim->velLinealViento.x) * segsTranscurridos;
			datosPart->transformaciones.trasladoY	+= (datosAnim->velLinealGravedad.y+datosAnim->velLinealViento.y) * segsTranscurridos;
			datosPart->transformaciones.rotacion	+= (datosAnim->velRadAngular) * segsTranscurridos;
			datosAnim->segundosViva					+= segsTranscurridos;
			if(datosAnim->segundosViva<datosAnim->segundosVive){
				float factorVivido					= (datosAnim->segundosViva / datosAnim->segundosVive);
				datosPart->color8.a					= 255.0f * (1.0f - factorVivido); //En teoria si 'segundosDesaparece' es cero nunca se ejecuta
				datosPart->transformaciones.escalaX	= datosAnim->escalaInicial + (datosAnim->escalaInicial * datosAnim->escalaParaDisipacion * factorVivido);
				datosPart->transformaciones.escalaY = datosPart->transformaciones.escalaX;
			} else {
				_datosParticulas.quitarElementoEnIndice(i);
				_datosAnimHumo.quitarElementoEnIndice(i);
				i--;
			}
		}
	}
	_partsCajaLocalSucia = true;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

//

NBCajaAABB AUEscenaGrupoParticulasLinea::cajaAABBLocalCalculada(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaTexto::cajaAABBLocalCalculada")
	NBASSERT(_datosParticulas.conteo==_datosAnimHumo.conteo)
	if(_partsCajaLocalSucia){
		NBCAJAAABB_INICIALIZAR(_partsCajaLocal)
		UI16 iPart; NBPunto pos, posMin, posMax;
		for(iPart=0; iPart<_datosParticulas.conteo; iPart++){
			pos.x = _datosParticulas.elemento[iPart].transformaciones.trasladoX;
			pos.y = _datosParticulas.elemento[iPart].transformaciones.trasladoY;
			posMin.x = pos.x - _texturaTamano.ancho;
			posMin.y = pos.y - _texturaTamano.alto;
			posMax.x = pos.x + _texturaTamano.ancho;
			posMax.y = pos.y + _texturaTamano.alto;
			NBCAJAAABB_ENVOLVER_PUNTO(_partsCajaLocal, posMin.x, posMin.y)
			NBCAJAAABB_ENVOLVER_PUNTO(_partsCajaLocal, posMax.x, posMax.y)
		}
		//Brindar un poco de volumen
		if(_partsCajaLocal.xMin==_partsCajaLocal.xMax){ _partsCajaLocal.xMin -= 8.0f; _partsCajaLocal.xMax += 8.0f;}
		if(_partsCajaLocal.yMin==_partsCajaLocal.yMax){ _partsCajaLocal.yMin -= 8.0f; _partsCajaLocal.yMax += 8.0f;}
		_partsCajaLocalSucia = false;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _partsCajaLocal;
}

void AUEscenaGrupoParticulasLinea::agregadoEnEscena(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaGrupoParticulasLinea::agregadoEnEscena")
	NBGestorAnimadores::agregarAnimador(this, this);
	NBGestorPersonajes::agregarVictimario(this, 0);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaGrupoParticulasLinea::quitandoDeEscena(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaGrupoParticulasLinea::quitandoDeEscena")
	NBGestorPersonajes::quitarVictimario(this);
	NBGestorAnimadores::quitarAnimador(this);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

bool AUEscenaGrupoParticulasLinea::personajeDanoHaciaVictima(const NBCajaAABB &cajaEscenaVictima, STPersonajeDano* guardarDanoEn){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaGrupoParticulasLinea::personajeDanoHaciaVictima")
	bool existeDano = false;
	if(NBCAJAAABB_INTERSECTA_CAJA(_cacheObjEscena.cajaEscena, cajaEscenaVictima)){ //Caja que inclute al sprite y los rayos
		NBPunto posIniEscena; NBMATRIZ_MULTIPLICAR_PUNTO(posIniEscena, _cacheObjEscena.matrizEscena, 0.0f, 0.0f)
		NBPunto posFinEscena; NBMATRIZ_MULTIPLICAR_PUNTO(posFinEscena, _cacheObjEscena.matrizEscena, _longitudHaciaDerecha, 0.0f)
		NBPunto vecRayo; NBPUNTO_ESTABLECER(vecRayo, posFinEscena.x-posIniEscena.x, posFinEscena.y-posIniEscena.y)
		float longitudRayo	= NBPUNTO_DISTANCIA_VECTOR(vecRayo.x, vecRayo.y); NBASSERT(longitudRayo!=0.0f)
		NBPunto vecUnitRayo; NBPUNTO_ESTABLECER(vecUnitRayo, vecRayo.x/longitudRayo, vecRayo.y/longitudRayo)
		NBPunto vectorCaja[4];
		float distanciaVivenParticulas = _velocidadBurbuja * _segsDesaparecen; NBASSERT(distanciaVivenParticulas!=0.0f)
		float productoCruz[4];
		NBPUNTO_ESTABLECER(vectorCaja[0], cajaEscenaVictima.xMin-posIniEscena.x, cajaEscenaVictima.yMin-posIniEscena.y)
		NBPUNTO_ESTABLECER(vectorCaja[1], cajaEscenaVictima.xMin-posIniEscena.x, cajaEscenaVictima.yMax-posIniEscena.y)
		NBPUNTO_ESTABLECER(vectorCaja[2], cajaEscenaVictima.xMax-posIniEscena.x, cajaEscenaVictima.yMin-posIniEscena.y)
		NBPUNTO_ESTABLECER(vectorCaja[3], cajaEscenaVictima.xMax-posIniEscena.x, cajaEscenaVictima.yMax-posIniEscena.y)
		productoCruz[0]	= NBPUNTO_PRODUCTO_VECTORIAL(vecUnitRayo.x, vecUnitRayo.y, vectorCaja[0].x, vectorCaja[0].y);
		productoCruz[1]	= NBPUNTO_PRODUCTO_VECTORIAL(vecUnitRayo.x, vecUnitRayo.y, vectorCaja[1].x, vectorCaja[1].y);
		productoCruz[2]	= NBPUNTO_PRODUCTO_VECTORIAL(vecUnitRayo.x, vecUnitRayo.y, vectorCaja[2].x, vectorCaja[2].y);
		productoCruz[3]	= NBPUNTO_PRODUCTO_VECTORIAL(vecUnitRayo.x, vecUnitRayo.y, vectorCaja[3].x, vectorCaja[3].y);
		float distanciaMasCercano = distanciaVivenParticulas;
		if(productoCruz[0]>0.0f && productoCruz[0]<distanciaMasCercano) distanciaMasCercano = productoCruz[0];
		if(productoCruz[1]>0.0f && productoCruz[1]<distanciaMasCercano) distanciaMasCercano = productoCruz[1];
		if(productoCruz[2]>0.0f && productoCruz[2]<distanciaMasCercano) distanciaMasCercano = productoCruz[2];
		if(productoCruz[3]>0.0f && productoCruz[3]<distanciaMasCercano) distanciaMasCercano = productoCruz[3];
		if(!((productoCruz[0]<0.0f && productoCruz[1]<0.0f && productoCruz[2]<0.0f && productoCruz[3]<0.0f) || (productoCruz[0]>=0.0f && productoCruz[1]>=0.0f && productoCruz[2]>=0.0f && productoCruz[3]>=0.0f))) distanciaMasCercano = 0.0f; //La linea atraviesa a la caja
		if(distanciaMasCercano<distanciaVivenParticulas){ //Por lo menos un vertice esta cerca
			float productoPunto[4];
			productoPunto[0]	= NBPUNTO_PRODUCTO_PUNTO(vecUnitRayo.x, vecUnitRayo.y, vectorCaja[0].x, vectorCaja[0].y);
			productoPunto[1]	= NBPUNTO_PRODUCTO_PUNTO(vecUnitRayo.x, vecUnitRayo.y, vectorCaja[1].x, vectorCaja[1].y);
			productoPunto[2]	= NBPUNTO_PRODUCTO_PUNTO(vecUnitRayo.x, vecUnitRayo.y, vectorCaja[2].x, vectorCaja[2].y);
			productoPunto[3]	= NBPUNTO_PRODUCTO_PUNTO(vecUnitRayo.x, vecUnitRayo.y, vectorCaja[3].x, vectorCaja[3].y);
			if(!((productoPunto[0]<0.0f && productoPunto[1]<0.0f && productoPunto[2]<0.0f && productoPunto[3]<0.0f) || (productoPunto[0]>longitudRayo && productoPunto[1]>longitudRayo && productoPunto[2]>longitudRayo && productoPunto[3]>longitudRayo))){
				//La caja tiene por lo menos un vertice dentro del rayo
				existeDano = true;
				guardarDanoEn->factorDanoRecibido = 0.25f + (0.75f * (1.0f - (distanciaMasCercano / distanciaVivenParticulas)));
				float promedioPuntoCaja = (productoPunto[0] + productoPunto[1] + productoPunto[2] + productoPunto[3]) / 4.0f;
				guardarDanoEn->posEscenaOrigenDano.x = posIniEscena.x + (vecUnitRayo.x * promedioPuntoCaja);
				guardarDanoEn->posEscenaOrigenDano.y = posIniEscena.y + (vecUnitRayo.y * promedioPuntoCaja);
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return existeDano;
}

PTRfunCmdsGL AUEscenaGrupoParticulasLinea::actualizarModeloGL(const NBPropRenderizado &propsRenderizado, const NBPropIluminacion &propsIluminacion, const NBPropHeredadasRender &propsHeredadas){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaGrupoParticulasLinea::actualizarModeloGL")
	NBASSERT(idEscena!=-1) //Solo si esta en escena
	NBASSERT(_datosAnimHumo.conteo==_datosParticulas.conteo)
	SI32 conteoParticulasTotal = _datosParticulas.conteo;
	if(conteoParticulasTotal!=0){
		NBColor colorMultiplicado;  NBCOLOR_MULTIPLICAR_CON_UI8(colorMultiplicado, propsHeredadas.colorPadre, _propiedades.color8)
		STBloqueGL bloqueVerticesGL;
		//Conteo de todas las particulas a renderizar
		bloqueVerticesGL.cantidadElementos			= conteoParticulasTotal * 4;
		bloqueVerticesGL.primerElemento				= (*propsRenderizado.funcReservarVertices)(ENVerticeGlTipo_MonoTextura, bloqueVerticesGL.cantidadElementos);
		//Datos render
		const SI32 indiceDatosRender				= propsRenderizado.bytesDatosModelos->conteo; propsRenderizado.bytesDatosModelos->reservarRegistrosAlFinal(sizeof(STGrupoParticulasLineaRender));
		STGrupoParticulasLineaRender* datosRender	= (STGrupoParticulasLineaRender*)&(propsRenderizado.bytesDatosModelos->elemento[indiceDatosRender]);
		datosRender->idTexturaGL					= _texturaParticula->idTexturaGL;
		datosRender->cantModelos					= conteoParticulasTotal;
		//Retener todas las texturas de las particulas
		propsRenderizado.piscinaRetecionRender->agregarElemento(_texturaParticula);
		//Arreglos de grupos
		NBVerticeGL* verticesGL						= &((*propsRenderizado.verticesGL1)[bloqueVerticesGL.primerElemento]);
		UI16 iPart, conteoParts						= conteoParticulasTotal;
		NBPropiedadesEnEscena* datosModelo			= _datosParticulas.elemento;
		for(iPart=0; iPart<conteoParts; iPart++){
			NBColor8 colorParticula8; NBCOLOR_ESTABLECER(colorParticula8, colorMultiplicado.r*datosModelo->color8.r, colorMultiplicado.g*datosModelo->color8.g, colorMultiplicado.b*datosModelo->color8.b, colorMultiplicado.a*datosModelo->color8.a)
			NBPunto puntosLocales[4];
			puntosLocales[0].x = _texturaTamano.ancho * datosModelo->transformaciones.escalaX * -0.5f;
			puntosLocales[1].x = _texturaTamano.ancho * datosModelo->transformaciones.escalaX * 0.5f;
			puntosLocales[2].x = _texturaTamano.ancho * datosModelo->transformaciones.escalaX * -0.5f;
			puntosLocales[3].x = _texturaTamano.ancho * datosModelo->transformaciones.escalaX * 0.5f;
			puntosLocales[0].y = _texturaTamano.alto * datosModelo->transformaciones.escalaY * -0.5f;
			puntosLocales[1].y = _texturaTamano.alto * datosModelo->transformaciones.escalaY * -0.5f;
			puntosLocales[2].y = _texturaTamano.alto * datosModelo->transformaciones.escalaY * 0.5f;
			puntosLocales[3].y = _texturaTamano.alto * datosModelo->transformaciones.escalaY * 0.5f;
			//GOTA NORMAL
			/*NBPUNTO_ROTAR_RADIANES(verticesGL[0], puntosLocales[0], datosModelo->radianes);
			 NBPUNTO_ROTAR_RADIANES(verticesGL[1], puntosLocales[1], datosModelo->radianes);
			 NBPUNTO_ROTAR_RADIANES(verticesGL[2], puntosLocales[2], datosModelo->radianes);
			 NBPUNTO_ROTAR_RADIANES(verticesGL[3], puntosLocales[3], datosModelo->radianes);*/
			puntosLocales[0].x	+= datosModelo->transformaciones.trasladoX; puntosLocales[0].y	+= datosModelo->transformaciones.trasladoY;
			puntosLocales[1].x	+= datosModelo->transformaciones.trasladoX; puntosLocales[1].y	+= datosModelo->transformaciones.trasladoY;
			puntosLocales[2].x	+= datosModelo->transformaciones.trasladoX; puntosLocales[2].y	+= datosModelo->transformaciones.trasladoY;
			puntosLocales[3].x	+= datosModelo->transformaciones.trasladoX; puntosLocales[3].y	+= datosModelo->transformaciones.trasladoY;
			NBMATRIZ_MULTIPLICAR_PUNTO(verticesGL[0], _cacheObjEscena.matrizEscena, puntosLocales[0].x, puntosLocales[0].y)
			NBMATRIZ_MULTIPLICAR_PUNTO(verticesGL[1], _cacheObjEscena.matrizEscena, puntosLocales[1].x, puntosLocales[1].y)
			NBMATRIZ_MULTIPLICAR_PUNTO(verticesGL[2], _cacheObjEscena.matrizEscena, puntosLocales[2].x, puntosLocales[2].y)
			NBMATRIZ_MULTIPLICAR_PUNTO(verticesGL[3], _cacheObjEscena.matrizEscena, puntosLocales[3].x, puntosLocales[3].y)
			NBCOLOR_ESTABLECER(verticesGL[0], colorParticula8.r, colorParticula8.g, colorParticula8.b, colorParticula8.a)
			NBCOLOR_ESTABLECER(verticesGL[1], colorParticula8.r, colorParticula8.g, colorParticula8.b, colorParticula8.a)
			NBCOLOR_ESTABLECER(verticesGL[2], colorParticula8.r, colorParticula8.g, colorParticula8.b, colorParticula8.a)
			NBCOLOR_ESTABLECER(verticesGL[3], colorParticula8.r, colorParticula8.g, colorParticula8.b, colorParticula8.a)
			_texturaParticula->cargarCoordenadasTextura(verticesGL);
			//
			verticesGL += 4;
			datosModelo++;
		}
		NBASSERT(conteoParticulasTotal > 0)
		NBASSERT(verticesGL == &((*propsRenderizado.verticesGL1)[bloqueVerticesGL.primerElemento + bloqueVerticesGL.cantidadElementos]))
		datosRender->bloqueIndicesGL = NB_GESTOR_GL_DAME_ELEMS_PARA_TRIANGSTRIPS_4(ENVerticeGlTipo_MonoTextura, bloqueVerticesGL, conteoParticulasTotal);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return (conteoParticulasTotal!=0 ? &AUEscenaGrupoParticulasLinea::enviarComandosGL : NULL);
}

void AUEscenaGrupoParticulasLinea::enviarComandosGL(UI8* punteroDatosModelo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaGrupoParticulasLinea::enviarComandosGL")
	STGrupoParticulasLineaRender* datosRender		= (STGrupoParticulasLineaRender*)punteroDatosModelo;
	NBASSERT(datosRender->cantModelos>0)
	NBGestorGL::activarVerticesGL(ENVerticeGlTipo_MonoTextura); NBGESTORGL_DBG_NOMBRAR_ACTIVADOR_VERTICES_GL("AUEscenaGrupoParticulasLinea")
	NBGestorGL::bindTexture(0, datosRender->idTexturaGL);
	NB_GESTOR_GL_RENDER_TRIANGSTRIPS_4(datosRender->bloqueIndicesGL.primerElemento, datosRender->cantModelos * NB_GESTOR_GL_ELEMS_POR_TRIANGSTRIP); NBGESTORGL_DBG_NOMBRAR_CONVOCADOR_ACUMULAR_TRIANG_STRIPS_INDEP("AUEscenaGrupoParticulasLinea")
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

//

bool AUEscenaGrupoParticulasLinea::agregarXmlInternoEn(AUEscenaGrupoParticulasLinea* lineaParts, AUCadenaLargaMutable8* guardarEn, const char* espaciosBaseIzq){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaGrupoParticulasLinea::agregarXmlInternoEn")
	//guardarEn->agregarConFormato("%s<vXML>1</vXML>\r\n", espaciosBaseIzq);
	guardarEn->agregarConFormato("%s<eOb>\r\n", espaciosBaseIzq);
	AUEscenaObjeto::agregarXmlInternoEn(lineaParts, guardarEn, espaciosBaseIzq);
	guardarEn->agregarConFormato("%s</eOb>\r\n", espaciosBaseIzq);
	//
	guardarEn->agregarConFormato("%s<lng>%f</lng>\r\n", espaciosBaseIzq, lineaParts->_longitudHaciaDerecha);
	guardarEn->agregarConFormato("%s<ppsm>%f</ppsm>\r\n", espaciosBaseIzq, lineaParts->_particulasPorSegundoPorMetro);
	guardarEn->agregarConFormato("%s<sD>%f</sD>\r\n", espaciosBaseIzq, lineaParts->_segsDesaparecen);
	guardarEn->agregarConFormato("%s<vel>%f</vel>\r\n", espaciosBaseIzq, lineaParts->_velocidadBurbuja);
	guardarEn->agregarConFormato("%s<sI>%f</sI>\r\n", espaciosBaseIzq, lineaParts->_escalaInicial);
	guardarEn->agregarConFormato("%s<sF>%f</sF>\r\n", espaciosBaseIzq, lineaParts->_escalaParaDisipar);
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return true;
	
}

bool AUEscenaGrupoParticulasLinea::interpretarXmlInterno(AUEscenaGrupoParticulasLinea* cargarEn, const AUDatosXML* datosXml, const sNodoXML* nodoXml){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaGrupoParticulasLinea::interpretarXmlInterno")
	bool exito = false;
	//int versionXML = datosXml->nodoHijo("vXML", 0, nodoXml);
	//if(versionXML==1){
	const sNodoXML* nodoEscenaObj;			XML_NODO_HIJO(datosXml, nodoEscenaObj, "eOb", "escObj", nodoXml, NULL)
	if(nodoEscenaObj != NULL){
		exito = AUEscenaObjeto::interpretarXmlInterno(cargarEn, datosXml, nodoEscenaObj);
		cargarEn->_longitudHaciaDerecha		= datosXml->nodoHijo("lng", 32.0f, nodoXml);
		cargarEn->_particulasPorSegundoPorMetro	= datosXml->nodoHijo("ppsm", 8.0f, nodoXml);
		cargarEn->_segsDesaparecen			= datosXml->nodoHijo("sD", 1.0f, nodoXml);
		cargarEn->_velocidadBurbuja			= datosXml->nodoHijo("vel", 32.0f, nodoXml);
		cargarEn->_escalaInicial			= datosXml->nodoHijo("sI", 0.1f, nodoXml);
		cargarEn->_escalaParaDisipar		= datosXml->nodoHijo("sF", 3.0f, nodoXml);
	}
	//}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

bool AUEscenaGrupoParticulasLinea::agregarBitsInternosEn(AUEscenaGrupoParticulasLinea* lineaParts, AUArchivo* guardarEn){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaGrupoParticulasLinea::agregarBitsInternosEn")
	bool exito = false;
	//
	float versionDatos = 1.0f;
	guardarEn->escribir(&versionDatos, sizeof(versionDatos), 1, guardarEn);
	if(AUEscenaObjeto::agregarBitsInternosEn(lineaParts, guardarEn)){
		guardarEn->escribir(&lineaParts->_longitudHaciaDerecha, sizeof(lineaParts->_longitudHaciaDerecha), 1, guardarEn);
		guardarEn->escribir(&lineaParts->_particulasPorSegundoPorMetro, sizeof(lineaParts->_particulasPorSegundoPorMetro), 1, guardarEn);
		guardarEn->escribir(&lineaParts->_segsDesaparecen, sizeof(lineaParts->_segsDesaparecen), 1, guardarEn);
		guardarEn->escribir(&lineaParts->_velocidadBurbuja, sizeof(lineaParts->_velocidadBurbuja), 1, guardarEn);
		guardarEn->escribir(&lineaParts->_escalaInicial, sizeof(lineaParts->_escalaInicial), 1, guardarEn);
		guardarEn->escribir(&lineaParts->_escalaParaDisipar, sizeof(lineaParts->_escalaParaDisipar), 1, guardarEn);
		exito = true;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

bool AUEscenaGrupoParticulasLinea::interpretarBitsInternos(AUEscenaGrupoParticulasLinea* cargarEn, AUArchivo* cargarDe){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaGrupoParticulasLinea::interpretarBitsInternos")
	bool exito = false;
	float versionDatos;
	cargarDe->leer(&versionDatos, sizeof(versionDatos), 1, cargarDe);
	if(versionDatos==1.0f){
		if(AUEscenaObjeto::interpretarBitsInternos(cargarEn, cargarDe)){
			cargarDe->leer(&cargarEn->_longitudHaciaDerecha, sizeof(cargarEn->_longitudHaciaDerecha), 1, cargarDe);
			cargarDe->leer(&cargarEn->_particulasPorSegundoPorMetro, sizeof(cargarEn->_particulasPorSegundoPorMetro), 1, cargarDe);
			cargarDe->leer(&cargarEn->_segsDesaparecen, sizeof(cargarEn->_segsDesaparecen), 1, cargarDe);
			cargarDe->leer(&cargarEn->_velocidadBurbuja, sizeof(cargarEn->_velocidadBurbuja), 1, cargarDe);
			cargarDe->leer(&cargarEn->_escalaInicial, sizeof(cargarEn->_escalaInicial), 1, cargarDe);
			cargarDe->leer(&cargarEn->_escalaParaDisipar, sizeof(cargarEn->_escalaParaDisipar), 1, cargarDe);
			exito = true;
		}
	} else {
		PRINTF_ERROR("AUEscenaSprite, la version (%f) de datos binarios no es interpretable por este ejecutable\n", versionDatos);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

//
AUOBJMETODOS_CLASESID_MULTICLASE(AUEscenaGrupoParticulasLinea, AUEscenaObjeto)
AUOBJMETODOS_CLASESNOMBRES_MULTICLASE(AUEscenaGrupoParticulasLinea, "AUEscenaGrupoParticulasLinea", AUEscenaObjeto)
AUOBJMETODOS_CLONAR_THIS(AUEscenaGrupoParticulasLinea)







