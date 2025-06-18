//
//  AUEscenaGrupoParticulasFuego.cpp
//  Gameplay_Mac
//
//  Created by Marcos Ortega Morales on 12/10/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkMediaStdAfx.h"
#include "AUEscenaGrupoParticulasFuego.h"

AUEscenaGrupoParticulasFuego::AUEscenaGrupoParticulasFuego() : AUEscenaObjeto(), _particulasFuego(this) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaGrupoParticulasFuego::AUEscenaGrupoParticulasFuego")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaGrupoParticulasFuego")
	_genParticulasAcumPendientes = 0.0f;
	_genParticulasPorSegundo	= 0.0f;
	_genParticulasAlcance		= 128.0f;
	_genParticulasSegsVida		= 0.25f;
	_texturasParticulas[0]		= NBGestorTexturas::texturaDesdeArchivo("Escenarios/Particulas/objHumoBlur01.png"); _texturasParticulas[0]->retener(NB_RETENEDOR_THIS);
	_texturasParticulas[1]		= NBGestorTexturas::texturaDesdeArchivo("Escenarios/Particulas/objHumoBlur02.png"); _texturasParticulas[1]->retener(NB_RETENEDOR_THIS);
	_tamTexParticulas[0]		= _texturasParticulas[0]->tamanoHD();
	_tamTexParticulas[1]		= _texturasParticulas[1]->tamanoHD();
	//
	_iLineaFuegoActual					= 0;
	_lineaFuegoActualInicializada		= false;
	_lineaFuego[0].iParticulaInicial	= -1;
	_lineaFuego[0].iParticulaFinal		= -1;
	_lineaFuego[1].iParticulaInicial	= -1;
	_lineaFuego[1].iParticulaFinal		= -1;
	//
	_partsCajaLocalSucia		= false;
	NBCAJAAABB_INICIALIZAR(_partsCajaLocal);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

AUEscenaGrupoParticulasFuego::~AUEscenaGrupoParticulasFuego(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaGrupoParticulasFuego::~AUEscenaGrupoParticulasFuego")
	if(_texturasParticulas[0] != NULL) _texturasParticulas[0]->liberar(NB_RETENEDOR_THIS);
	if(_texturasParticulas[1] != NULL) _texturasParticulas[1]->liberar(NB_RETENEDOR_THIS);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

//

float AUEscenaGrupoParticulasFuego::alcanceParticulas(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaGrupoParticulasFuego::alcanceParticulas")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _genParticulasAlcance;
}

float AUEscenaGrupoParticulasFuego::particulasPorSegundo(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaGrupoParticulasFuego::particulasPorSegundo")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _genParticulasPorSegundo;
}

UI16 AUEscenaGrupoParticulasFuego::conteoParticulasFuegoVivas(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaGrupoParticulasFuego::conteoParticulasFuegoVivas")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _particulasFuego.conteo;
}

void AUEscenaGrupoParticulasFuego::establecerAlcanceParticulas(float alcanceParticulas){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaGrupoParticulasFuego::establecerAlcanceParticulas")
	_genParticulasAlcance = alcanceParticulas;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaGrupoParticulasFuego::establecerParticulasPorSegundo(float particulasPorSegundo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaGrupoParticulasFuego::establecerParticulasPorSegundo")
	_genParticulasPorSegundo = particulasPorSegundo;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaGrupoParticulasFuego::establecerParticulasSegsVida(float segsVida){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaGrupoParticulasFuego::establecerParticulasSegsVida")
	_genParticulasSegsVida = segsVida;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

//

NBCajaAABB AUEscenaGrupoParticulasFuego::cajaAABBLocalCalculada(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaTexto::cajaAABBLocalCalculada")
	if(_partsCajaLocalSucia){
		NBMatriz matrizInversa; NBMATRIZ_INVERSA(matrizInversa, _cacheObjEscena.matrizEscena)
		NBCajaAABB cajaLocalCentros; NBCAJAAABB_INICIALIZAR(cajaLocalCentros)
		UI16 iPart;
		for(iPart=0; iPart<_particulasFuego.conteo; iPart++){
			NBPunto posLocal;
			NBMATRIZ_MULTIPLICAR_PUNTO(posLocal, matrizInversa, _particulasFuego.elemento[iPart].posicionEscena.x, _particulasFuego.elemento[iPart].posicionEscena.y) ;
			NBCAJAAABB_ENVOLVER_PUNTO(cajaLocalCentros, posLocal.x, posLocal.y)
		}
		//La cajaCentros no considerada los bordes de las sprites.
		//Se debe inflar un poco la caja para que las particulas no desasparezcan repentinamente.
		_partsCajaLocal.xMin = cajaLocalCentros.xMin - 32.0f;
		_partsCajaLocal.yMin = cajaLocalCentros.yMin - 32.0f;
		_partsCajaLocal.xMax = cajaLocalCentros.xMax + 32.0f;
		_partsCajaLocal.yMax = cajaLocalCentros.yMax + 32.0f;
		_partsCajaLocalSucia = false;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _partsCajaLocal;
}

void AUEscenaGrupoParticulasFuego::agregadoEnEscena(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaGrupoParticulasFuego::agregadoEnEscena")
	NBGestorAnimadores::agregarAnimador(this, this);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaGrupoParticulasFuego::quitandoDeEscena(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaGrupoParticulasFuego::quitandoDeEscena")
	NBGestorAnimadores::quitarAnimador(this);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

//

void AUEscenaGrupoParticulasFuego::activarSiguienteLineaParticulas(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaGrupoParticulasFuego::activarSiguienteLineaParticulas")
	if(_lineaFuegoActualInicializada){
		_lineaFuegoActualInicializada = false;
		_iLineaFuegoActual = (_iLineaFuegoActual==0?1:0);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaGrupoParticulasFuego::lineasParticulas(STParticulaFuegoLinea* guardarLinea1En, STParticulaFuegoLinea* guardarLinea2En){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaGrupoParticulasFuego::lineasParticulas")
	if(guardarLinea1En){
		if((_lineaFuego[0].iParticulaInicial!=-1 && _lineaFuego[0].iParticulaInicial<_lineaFuego[0].iParticulaFinal)){
			guardarLinea1En->lineaEstablecida = true;
			NBASSERT(_lineaFuego[0].iParticulaInicial>=0 && _lineaFuego[0].iParticulaInicial<(SI32)_particulasFuego.conteo)
			NBASSERT(_lineaFuego[0].iParticulaFinal>=0 && _lineaFuego[0].iParticulaFinal<(SI32)_particulasFuego.conteo)
			guardarLinea1En->posEscenaIni = _particulasFuego.elemento[_lineaFuego[0].iParticulaInicial].posicionEscena;
			guardarLinea1En->posEscenaFin = _particulasFuego.elemento[_lineaFuego[0].iParticulaFinal].posicionEscena;
		} else {
			guardarLinea1En->lineaEstablecida	= false;
			guardarLinea1En->posEscenaIni.x		= 0.0f;
			guardarLinea1En->posEscenaIni.y		= 0.0f;
			guardarLinea1En->posEscenaFin.x		= 0.0f;
			guardarLinea1En->posEscenaFin.y		= 0.0f;
		}
		//PRINTF_INFO("Linea1 %d (%d -> %d) ", guardarLinea1En->lineaEstablecida?1:0, _lineaFuego[0].iParticulaInicial, _lineaFuego[0].iParticulaFinal);
	}
	if(guardarLinea2En){
		if((_lineaFuego[1].iParticulaInicial!=-1 && _lineaFuego[1].iParticulaInicial<_lineaFuego[1].iParticulaFinal)){
			guardarLinea2En->lineaEstablecida = true;
			NBASSERT(_lineaFuego[1].iParticulaInicial>=0 && _lineaFuego[1].iParticulaInicial<(SI32)_particulasFuego.conteo)
			NBASSERT(_lineaFuego[1].iParticulaFinal>=0 && _lineaFuego[1].iParticulaFinal<(SI32)_particulasFuego.conteo)
			guardarLinea2En->posEscenaIni = _particulasFuego.elemento[_lineaFuego[1].iParticulaInicial].posicionEscena;
			guardarLinea2En->posEscenaFin = _particulasFuego.elemento[_lineaFuego[1].iParticulaFinal].posicionEscena;
		} else {
			guardarLinea2En->lineaEstablecida = false;
			guardarLinea2En->posEscenaIni.x		= 0.0f;
			guardarLinea2En->posEscenaIni.y		= 0.0f;
			guardarLinea2En->posEscenaFin.x		= 0.0f;
			guardarLinea2En->posEscenaFin.y		= 0.0f;
		}
		//PRINTF_INFO("Linea2 %d (%d -> %d)", guardarLinea2En->lineaEstablecida?1:0, _lineaFuego[1].iParticulaInicial, _lineaFuego[1].iParticulaFinal);
	}
	//PRINTF_INFO("\n");
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaGrupoParticulasFuego::tickAnimacion(float segsTranscurridos){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaGrupoParticulasFuego::tickAnimacion")
	if(!this->_dormido){
		//Determinar viento y gravedad
		NBPunto vGravedad;
		NBPunto vViento;
		NBPUNTO_ESTABLECER(vGravedad, 0.0f, -9.8f*ESCALA_PIXELES_POR_METRO_MUNDO_FISICA)
		NBPUNTO_ESTABLECER(vViento, 0.0f, 0.0f)
		if(_idMundoFisica!=-1){
#			ifndef CONFIG_NB_UNSUPPORT_BOX2D
			vGravedad		= NBGestorFisica::vectorGravedadEscena(_idMundoFisica);
			vViento			= NBGestorFisica::vectorVientoEscena(_idMundoFisica);
#			endif
		}
		//ANIMAR PARTICULAS EXISTENTES
		SI32 iPart, conteoParts = _particulasFuego.conteo;
		for(iPart=conteoParts-1; iPart>=0; iPart--){
			STParticulaFuegoProps* datosPart = &_particulasFuego.elemento[iPart];
			datosPart->segundosHaVivido		+= segsTranscurridos;
			float relativoVivido			= datosPart->segundosHaVivido / datosPart->segundosVive;
			float factorCrecimiento			= datosPart->alcanceParticula / 256.0f;
			datosPart->posicionEscena.x		+= (datosPart->vectVelocidad.x * segsTranscurridos);
			datosPart->posicionEscena.y		+= (datosPart->vectVelocidad.y * segsTranscurridos);
			datosPart->radianes				+= (datosPart->velAngular * segsTranscurridos);
			if(datosPart->segundosHaVivido<datosPart->segundosVive){
				datosPart->escala			= 0.15f + (0.55f * relativoVivido * factorCrecimiento) + (0.30f * relativoVivido * relativoVivido * factorCrecimiento);
				datosPart->vectVelocidad.x	+= (vGravedad.x * segsTranscurridos * relativoVivido * 0.15f);
				datosPart->vectVelocidad.y	+= (vGravedad.y * segsTranscurridos * relativoVivido * 0.15f);
				datosPart->color.r			= 1.0f - (relativoVivido / 4.0f);
				datosPart->color.g			= 1.0f - relativoVivido;
			} else {
				datosPart->vectVelocidad.x	+= (-vGravedad.x * segsTranscurridos * 2.0f) + (vViento.x * segsTranscurridos);
				datosPart->vectVelocidad.y	+= (-vGravedad.y * segsTranscurridos * 2.0f) + (vViento.y * segsTranscurridos);
				datosPart->escala			+= segsTranscurridos * 2.0f;
				datosPart->color.r			+= segsTranscurridos * 4.0f;
				datosPart->color.g			+= segsTranscurridos * 4.0f;
				datosPart->color.b			+= segsTranscurridos * 4.0f;
				datosPart->color.a			-= segsTranscurridos * 4.0f;
				if(datosPart->color.r>1.0f) datosPart->color.r = 1.0f;
				if(datosPart->color.g>1.0f) datosPart->color.g = 1.0f;
				if(datosPart->color.b>1.0f) datosPart->color.b = 1.0f;
				if(datosPart->color.a<=0.0f){
					_particulasFuego.quitarElementoEnIndice(iPart);
					if(_lineaFuego[0].iParticulaInicial>iPart) _lineaFuego[0].iParticulaInicial--;
					if(_lineaFuego[1].iParticulaInicial>iPart) _lineaFuego[1].iParticulaInicial--;
					if(_lineaFuego[0].iParticulaFinal>iPart) _lineaFuego[0].iParticulaFinal--;
					if(_lineaFuego[1].iParticulaFinal>iPart) _lineaFuego[1].iParticulaFinal--;
				}
			}
		}
		//GENERAR NUEVAS PARTICULAS
		_genParticulasAcumPendientes			+= (segsTranscurridos * _genParticulasPorSegundo);
		NBPunto centroEscena;					NBMATRIZ_MULTIPLICAR_PUNTO(centroEscena, _cacheObjEscena.matrizEscena, 0.0f, 0.0f)
		NBPunto vecVelInicial;
		vecVelInicial.x							= (NBMATRIZ_ELEM00(_cacheObjEscena.matrizEscena) * _genParticulasAlcance * 4.0f);
		vecVelInicial.y							= (NBMATRIZ_ELEM10(_cacheObjEscena.matrizEscena) * _genParticulasAlcance * 4.0f);
		float particulasAcumOrig				= _genParticulasAcumPendientes;
		while(_genParticulasAcumPendientes>=1.0f){
			_genParticulasAcumPendientes--;
			STParticulaFuegoProps datosParticulas;
			datosParticulas.iTextura			= rand()%2;
			datosParticulas.radianes			= 0.0f;
			datosParticulas.escala				= 0.15f;
			float avanceParticulaRel			= _genParticulasAcumPendientes / particulasAcumOrig;
			NBCOLOR_ESTABLECER(datosParticulas.color, 1.0f, 1.0f, 0.0f, 1.0f);
			//
			datosParticulas.esquinaMinSprite.x	= _tamTexParticulas[datosParticulas.iTextura].ancho / -2.0f;
			datosParticulas.esquinaMinSprite.y	= _tamTexParticulas[datosParticulas.iTextura].alto / -2.0f;
			datosParticulas.esquinaMaxSprite.x	= _tamTexParticulas[datosParticulas.iTextura].ancho / 2.0f;
			datosParticulas.esquinaMaxSprite.y	= _tamTexParticulas[datosParticulas.iTextura].alto / 2.0f;
			//
			float relRotacionAleatoriaX			= ((rand() % 1000) - 500.0f) / 1000.0f;
			float relRotacionAleatoriaY			= ((rand() % 1000) - 500.0f) / 1000.0f;
			NBPUNTO_ESTABLECER(datosParticulas.vectVelocidad, (0.95f*vecVelInicial.x)+(0.05*vecVelInicial.x*relRotacionAleatoriaX), (0.95f*vecVelInicial.y)+(0.05*vecVelInicial.y*relRotacionAleatoriaY));
			//datosParticulas.posicionEscena		= centroEscena;
			NBPUNTO_ESTABLECER(datosParticulas.posicionEscena, centroEscena.x+(datosParticulas.vectVelocidad.x*segsTranscurridos*avanceParticulaRel), centroEscena.y+(datosParticulas.vectVelocidad.y*segsTranscurridos*avanceParticulaRel))
			datosParticulas.velAngular			= relRotacionAleatoriaX * PIx2 * 4.0f;
			datosParticulas.alcanceParticula	= _genParticulasAlcance;
			datosParticulas.segundosVive		= _genParticulasSegsVida;
			datosParticulas.segundosHaVivido	= 0.0f;
			//
			_particulasFuego.agregarElemento(datosParticulas);
			_lineaFuego[_iLineaFuegoActual].iParticulaFinal			= _particulasFuego.conteo - 1;
			if(!_lineaFuegoActualInicializada){
				NBASSERT(_particulasFuego.conteo!=0)
				_lineaFuego[_iLineaFuegoActual].iParticulaInicial	= _particulasFuego.conteo - 1;
				_lineaFuegoActualInicializada = true;
			}
		}
	}
	_partsCajaLocalSucia = true;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

//

PTRfunCmdsGL AUEscenaGrupoParticulasFuego::actualizarModeloGL(const NBPropRenderizado &propsRenderizado, const NBPropIluminacion &propsIluminacion, const NBPropHeredadasRender &propsHeredadas){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaGrupoParticulasFuego::actualizarModeloGL")
	NBASSERT(idEscena!=-1) //Solo si esta en escena
	UI16 iPart, conteoParts = _particulasFuego.conteo;
	if(conteoParts!=0){
		NBColor colorMultiplicado;			NBCOLOR_MULTIPLICAR_CON_UI8(colorMultiplicado, propsHeredadas.colorPadre, _propiedades.color8)
		STBloqueGL bloqueVerticesGL;
		bloqueVerticesGL.cantidadElementos	= (conteoParts * 4);
		bloqueVerticesGL.primerElemento		= (*propsRenderizado.funcReservarVertices)(ENVerticeGlTipo_MonoTextura, bloqueVerticesGL.cantidadElementos);
		NBVerticeGL* verticesGL				= &((*propsRenderizado.verticesGL1)[bloqueVerticesGL.primerElemento]);
		UI16 conteoParticulas[2]			= {0, 0};
		if(conteoParts>0){
			UI16 iTex;
			for(iTex=0; iTex<2; iTex++){
				for(iPart=0; iPart<conteoParts; iPart++){
					if(iTex!=_particulasFuego.elemento[iPart].iTextura) continue;
					STParticulaFuegoProps* datosPart = &_particulasFuego.elemento[iPart];
					NBMatriz matrizParticula;	NBMATRIZ_ESTABLECER_SEGUN_TRANSFORMACIONES_RADIANES(matrizParticula, datosPart->posicionEscena.x, datosPart->posicionEscena.y, datosPart->radianes, datosPart->escala, datosPart->escala)
					NBMATRIZ_MULTIPLICAR_PUNTO(verticesGL[0], matrizParticula, datosPart->esquinaMinSprite.x, datosPart->esquinaMinSprite.y)
					NBMATRIZ_MULTIPLICAR_PUNTO(verticesGL[1], matrizParticula, datosPart->esquinaMaxSprite.x, datosPart->esquinaMinSprite.y)
					NBMATRIZ_MULTIPLICAR_PUNTO(verticesGL[2], matrizParticula, datosPart->esquinaMinSprite.x, datosPart->esquinaMaxSprite.y)
					NBMATRIZ_MULTIPLICAR_PUNTO(verticesGL[3], matrizParticula, datosPart->esquinaMaxSprite.x, datosPart->esquinaMaxSprite.y)
					NBCOLOR_ESTABLECER(verticesGL[0], 255*datosPart->color.r*colorMultiplicado.r, 255*datosPart->color.g*colorMultiplicado.g, 255*datosPart->color.b*colorMultiplicado.b, 255*datosPart->color.a*colorMultiplicado.a)
					NBCOLOR_ESTABLECER(verticesGL[1], 255*datosPart->color.r*colorMultiplicado.r, 255*datosPart->color.g*colorMultiplicado.g, 255*datosPart->color.b*colorMultiplicado.b, 255*datosPart->color.a*colorMultiplicado.a)
					NBCOLOR_ESTABLECER(verticesGL[2], 255*datosPart->color.r*colorMultiplicado.r, 255*datosPart->color.g*colorMultiplicado.g, 255*datosPart->color.b*colorMultiplicado.b, 255*datosPart->color.a*colorMultiplicado.a)
					NBCOLOR_ESTABLECER(verticesGL[3], 255*datosPart->color.r*colorMultiplicado.r, 255*datosPart->color.g*colorMultiplicado.g, 255*datosPart->color.b*colorMultiplicado.b, 255*datosPart->color.a*colorMultiplicado.a)
					_texturasParticulas[datosPart->iTextura]->cargarCoordenadasTextura(verticesGL);
					verticesGL		+= 4;
					conteoParticulas[iTex]++;
				}
			}
		}
		NBASSERT(conteoParts==(conteoParticulas[0]+conteoParticulas[1]))
		#ifdef AUGRUPOPARTICULASFUEGO_DIBUJAR_INFO_DEBUG
		//Lineas de particulas
		STBloqueGL bloqueVerticesGLDebug;
		bloqueVerticesGLDebug.cantidadElementos	= 4;
		bloqueVerticesGLDebug.primerElemento	= (*propsRenderizado.funcReservarVertices)(ENVerticeGlTipo_MonoTextura, 4);
		NBVerticeGL* verticesGLDbg				= &((*propsRenderizado.verticesGL1)[bloqueVerticesGLDebug.primerElemento]);
		STParticulaFuegoLinea lineasFuego[2];	this->lineasParticulas(&lineasFuego[0], &lineasFuego[1]);
		NBPUNTO_ESTABLECER(verticesGLDbg[0], lineasFuego[0].posEscenaIni.x, lineasFuego[0].posEscenaIni.y)
		NBPUNTO_ESTABLECER(verticesGLDbg[1], lineasFuego[0].posEscenaFin.x, lineasFuego[0].posEscenaFin.y)
		NBPUNTO_ESTABLECER(verticesGLDbg[2], lineasFuego[1].posEscenaIni.x, lineasFuego[1].posEscenaIni.y)
		NBPUNTO_ESTABLECER(verticesGLDbg[3], lineasFuego[1].posEscenaFin.x, lineasFuego[1].posEscenaFin.y)
		NBCOLOR_ESTABLECER(verticesGLDbg[0], 255, 255, 255, 255)
		NBCOLOR_ESTABLECER(verticesGLDbg[1], 255, 255, 255, 255)
		NBCOLOR_ESTABLECER(verticesGLDbg[2], 255, 255, 255, 255)
		NBCOLOR_ESTABLECER(verticesGLDbg[3], 255, 255, 255, 255)
		NBPUNTO_ESTABLECER(verticesGLDbg[0].tex, 0.0f, 0.0f)
		NBPUNTO_ESTABLECER(verticesGLDbg[1].tex, 0.0f, 0.0f)
		NBPUNTO_ESTABLECER(verticesGLDbg[2].tex, 0.0f, 0.0f)
		NBPUNTO_ESTABLECER(verticesGLDbg[3].tex, 0.0f, 0.0f)
		#endif
		//Datos render
		const SI32 indiceDatosRender		= propsRenderizado.bytesDatosModelos->conteo; propsRenderizado.bytesDatosModelos->reservarRegistrosAlFinal(sizeof(STGrupoParticulasFuegoRender));
		STGrupoParticulasFuegoRender* datosRender= (STGrupoParticulasFuegoRender*)&(propsRenderizado.bytesDatosModelos->elemento[indiceDatosRender]);
		if(_texturasParticulas[0] != NULL){
			datosRender->idTexturaGL[0]		= _texturasParticulas[0]->idTexturaGL; propsRenderizado.piscinaRetecionRender->agregarElemento(_texturasParticulas[0]);
		} else {
			//required on some systems where texture-0 will not be drawn
			datosRender->idTexturaGL[0]		= NBGestorTexturas::texturaBlanca->idTexturaGL;
		}
		if(_texturasParticulas[1] != NULL){
			datosRender->idTexturaGL[1]		= _texturasParticulas[1]->idTexturaGL; propsRenderizado.piscinaRetecionRender->agregarElemento(_texturasParticulas[1]);
		} else {
			//required on some systems where texture-0 will not be drawn
			datosRender->idTexturaGL[1]		= NBGestorTexturas::texturaBlanca->idTexturaGL;
		}
		NBASSERT(conteoParts> 0 )
		datosRender->conteoParticulasTex[0]	= conteoParticulas[0];
		datosRender->conteoParticulasTex[1]	= conteoParticulas[1];
		datosRender->bloqueIndicesGL		= NB_GESTOR_GL_DAME_ELEMS_PARA_TRIANGSTRIPS_4(ENVerticeGlTipo_MonoTextura, bloqueVerticesGL, conteoParts);
		#ifdef AUGRUPOPARTICULASFUEGO_DIBUJAR_INFO_DEBUG
		datosRender->bloqueVerticesGLDebug	= bloqueVerticesGLDebug;
		#endif
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return conteoParts!=0 ? &AUEscenaGrupoParticulasFuego::enviarComandosGL : NULL;
}

void AUEscenaGrupoParticulasFuego::enviarComandosGL(UI8* punteroDatosModelo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaGrupoParticulasFuego::enviarComandosGL")
	STGrupoParticulasFuegoRender* datosRender	= (STGrupoParticulasFuegoRender*)punteroDatosModelo;
	NBGestorGL::activarVerticesGL(ENVerticeGlTipo_MonoTextura); NBGESTORGL_DBG_NOMBRAR_ACTIVADOR_VERTICES_GL("AUEscenaGrupoParticulasFuego")
	UI16 iElemGL = datosRender->bloqueIndicesGL.primerElemento;
	UI16 iTex; UI16 conteoParts;
	for(iTex=0; iTex<2; iTex++){
		conteoParts = datosRender->conteoParticulasTex[iTex];
		if(conteoParts!=0){
			NBASSERT(datosRender->idTexturaGL[iTex] > 0)
			NBGestorGL::bindTexture(0, datosRender->idTexturaGL[iTex]);
			NB_GESTOR_GL_RENDER_TRIANGSTRIPS_4(iElemGL, conteoParts * NB_GESTOR_GL_ELEMS_POR_TRIANGSTRIP); NBGESTORGL_DBG_NOMBRAR_CONVOCADOR_ACUMULAR_TRIANG_STRIPS_INDEP("AUEscenaGrupoParticulasFuego")
			iElemGL += conteoParts * NB_GESTOR_GL_ELEMS_POR_TRIANGSTRIP;
		}
	}
	#ifdef AUGRUPOPARTICULASFUEGO_DIBUJAR_INFO_DEBUG
	NBGestorGL::bindTexture(0, NBGestorTexturas::texturaBlanca->idTexturaGL, NBGestorTexturas::texturaBlanca->modoPintado);
	NBGestorGL::drawArrays(GL_LINES, datosRender->bloqueVerticesGLDebug.primerElemento, 4); NBGESTORGL_DBG_NOMBRAR_CONVOCADOR_DRAW_ARRAY("AUEscenaFigura(1)")
	#endif
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

//
AUOBJMETODOS_CLASESID_MULTICLASE(AUEscenaGrupoParticulasFuego, AUEscenaObjeto)
AUOBJMETODOS_CLASESNOMBRES_MULTICLASE(AUEscenaGrupoParticulasFuego, "AUEscenaGrupoParticulasFuego", AUEscenaObjeto)
AUOBJMETODOS_CLONAR_NULL(AUEscenaGrupoParticulasFuego)






