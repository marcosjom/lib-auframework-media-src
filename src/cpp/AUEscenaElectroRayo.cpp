//
//  AUEscenaElectroRayo.cpp
//  Gameplay_Mac
//
//  Created by Marcos Ortega Morales on 12/10/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkMediaStdAfx.h"
#include "AUEscenaElectroRayo.h"

AUEscenaElectroRayo::AUEscenaElectroRayo() : AUEscenaObjeto(), _rayos(this), _rayosSegmentos(this) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaElectroRayo::AUEscenaElectroRayo")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaElectroRayo")
	//
	_rayosCajaLocalSucia	= true;
	NBCAJAAABB_INICIALIZAR(_rayosCajaLocal);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

AUEscenaElectroRayo::~AUEscenaElectroRayo(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaElectroRayo::~AUEscenaElectroRayo")
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

//

UI16 AUEscenaElectroRayo::conteoRayosActivos(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaElectroRayo::conteoRayosActivos")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _rayos.conteo;
}

AUArregloNativoP<STRayo>* AUEscenaElectroRayo::rayosActivos(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaElectroRayo::rayosActivos")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return &_rayos;
}

bool AUEscenaElectroRayo::existeRayoConId(const UI8 idRayo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaElectroRayo::existeRayoConId")
	bool existe = false;
	UI16 iRayo;
	for(iRayo=0; iRayo<_rayos.conteo; iRayo++){
		if(_rayos.elemento[iRayo].idRayo==idRayo){
			existe	= true; break;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return existe;
}

void AUEscenaElectroRayo::agregarRayo(const UI8 idRayo, const NBPunto &posInicio, const NBPunto &posFin, float duracion){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaElectroRayo::agregarRayo")
	STRayo datosRayo;
	datosRayo.idRayo			= idRayo;
	datosRayo.posInicio			= posInicio;
	datosRayo.posFin			= posFin;
	NBPUNTO_ESTABLECER(datosRayo.vectorUnitario, datosRayo.posFin.x-datosRayo.posInicio.x, datosRayo.posFin.y-datosRayo.posInicio.y);
	datosRayo.alcance			= NBPUNTO_DISTANCIA_VECTOR(datosRayo.vectorUnitario.x, datosRayo.vectorUnitario.y);
	datosRayo.vectorUnitario.x /= datosRayo.alcance;
	datosRayo.vectorUnitario.y /= datosRayo.alcance;
	NBPUNTO_PRODUCTO_CRUZ_ESCALAR_X_VECTOR(datosRayo.vectorUnitIzq, 1.0f, datosRayo.vectorUnitario.x, datosRayo.vectorUnitario.y)
	datosRayo.iPrimerSegmento	= _rayosSegmentos.conteo;
	datosRayo.conteoSegmentos	= datosRayo.alcance / 32;
	datosRayo.segsAcumEnPosicion = 99999.0f;
	datosRayo.segundosVive		= duracion;
	datosRayo.segundosVividos	= 0.0f;
	datosRayo.alpha				= 1.0f;
	_rayos.agregarElemento(datosRayo);
	//
	if(datosRayo.conteoSegmentos!=0){
		float avancePorSegmento	= datosRayo.alcance / (datosRayo.conteoSegmentos + 1);
		UI16 iSeg;
		for(iSeg=0; iSeg<datosRayo.conteoSegmentos; iSeg++){
			STRayoSegmento datosSeg;
			datosSeg.avanceRelEnRayo = (avancePorSegmento * iSeg) / datosRayo.alcance;
			NBPUNTO_ESTABLECER(datosSeg.posFinSegmento, datosRayo.posInicio.x+(datosRayo.vectorUnitario.x * avancePorSegmento * iSeg), datosRayo.posInicio.y+(datosRayo.vectorUnitario.y * avancePorSegmento * iSeg));
			_rayosSegmentos.agregarElemento(datosSeg);
		}
	}
	_rayosCajaLocalSucia = true;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaElectroRayo::actualizarRayoPosiciones(const UI8 idRayo, const NBPunto &posInicio, const NBPunto &posFin){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaElectroRayo::actualizarRayoPosiciones")
	UI16 iRayo;
	for(iRayo=0; iRayo<_rayos.conteo; iRayo++){
		if(_rayos.elemento[iRayo].idRayo==idRayo){
			STRayo* datosRayo		= &_rayos.elemento[iRayo];
			datosRayo->posInicio	= posInicio;
			datosRayo->posFin		= posFin;
			NBPUNTO_ESTABLECER(datosRayo->vectorUnitario, datosRayo->posFin.x-datosRayo->posInicio.x, datosRayo->posFin.y-datosRayo->posInicio.y);
			datosRayo->alcance			= NBPUNTO_DISTANCIA_VECTOR(datosRayo->vectorUnitario.x, datosRayo->vectorUnitario.y);
			datosRayo->vectorUnitario.x /= datosRayo->alcance;
			datosRayo->vectorUnitario.y /= datosRayo->alcance;
			NBPUNTO_PRODUCTO_CRUZ_ESCALAR_X_VECTOR(datosRayo->vectorUnitIzq, 1.0f, datosRayo->vectorUnitario.x, datosRayo->vectorUnitario.y)
			//
			_rayosCajaLocalSucia = true;
			break;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaElectroRayo::quitarRayo(const UI8 idRayo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaElectroRayo::quitarRayo")
	SI32 iRayo;
	for(iRayo=_rayos.conteo-1; iRayo>=0; iRayo--){
		STRayo* datosRayo	= &_rayos.elemento[iRayo];
		if(datosRayo->idRayo==idRayo){
			//Actualizar todos los indices de los rayos con segmentos posteriores a este
			UI16 iRayo2;
			for(iRayo2=0; iRayo2<_rayos.conteo; iRayo2++){
				if(_rayos.elemento[iRayo2].iPrimerSegmento>datosRayo->iPrimerSegmento) _rayos.elemento[iRayo2].iPrimerSegmento -= datosRayo->conteoSegmentos;
			}
			//Quitar segmentos y rayo
			_rayosSegmentos.quitarElementosEnIndice(datosRayo->iPrimerSegmento, datosRayo->conteoSegmentos);
			_rayos.quitarElementoEnIndice(iRayo);
			_rayosCajaLocalSucia = true;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}


//

NBCajaAABB AUEscenaElectroRayo::cajaAABBLocalCalculada(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaTexto::cajaAABBLocalCalculada")
	if(_rayosCajaLocalSucia){
		NBCajaAABB cajaLocalCentros; NBCAJAAABB_INICIALIZAR(cajaLocalCentros)
		UI16 iRayo;
		for(iRayo=0; iRayo<_rayos.conteo; iRayo++){
			STRayo* datosRayo	= &_rayos.elemento[iRayo];
			NBCAJAAABB_ENVOLVER_PUNTO(cajaLocalCentros, datosRayo->posInicio.x, datosRayo->posInicio.y)
			NBCAJAAABB_ENVOLVER_PUNTO(cajaLocalCentros, datosRayo->posFin.x, datosRayo->posFin.y)
		}
		//La cajaCentros no considerada los bordes de los segmentos.
		//Se debe inflar un poco la caja para que los rayos no desasparezcan repentinamente.
		_rayosCajaLocal.xMin = cajaLocalCentros.xMin - 32.0f;
		_rayosCajaLocal.yMin = cajaLocalCentros.yMin - 32.0f;
		_rayosCajaLocal.xMax = cajaLocalCentros.xMax + 32.0f;
		_rayosCajaLocal.yMax = cajaLocalCentros.yMax + 32.0f;
		_rayosCajaLocalSucia = false;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _rayosCajaLocal;
}

void AUEscenaElectroRayo::agregadoEnEscena(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaElectroRayo::agregadoEnEscena")
	NBGestorAnimadores::agregarAnimador(this, this);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaElectroRayo::quitandoDeEscena(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaElectroRayo::quitandoDeEscena")
	NBGestorAnimadores::quitarAnimador(this);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

//

void AUEscenaElectroRayo::tickAnimacion(float segsTranscurridos){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaElectroRayo::tickAnimacion")
	if(!this->_dormido){
		SI32 iRayo;
		for(iRayo=_rayos.conteo-1; iRayo>=0; iRayo--){
			STRayo* datosRayo	= &_rayos.elemento[iRayo];
			//
			datosRayo->segundosVividos += segsTranscurridos;
			if(datosRayo->segundosVividos<datosRayo->segundosVive){
				datosRayo->segsAcumEnPosicion += segsTranscurridos;
				if(datosRayo->segsAcumEnPosicion>0.033){ //(1/60) = 0.016666 ... (1/30) = 0.033333
					datosRayo->segsAcumEnPosicion = 0.0f;
					float avancePorSegmento	= datosRayo->alcance / (datosRayo->conteoSegmentos + 1); //+1 porque el ultimo segmento es el fin del rayo
					float avanceAcumTotal	= 0.0f;
					float margenAcumDeAnt	= 0.0f;
					float anchoDesvioRayo	= datosRayo->alcance * 0.25f; if(anchoDesvioRayo>16.0f) anchoDesvioRayo = 16.0f;
					STRayoSegmento* arrSegs	= &_rayosSegmentos.elemento[datosRayo->iPrimerSegmento]; NBASSERT(datosRayo->iPrimerSegmento+datosRayo->conteoSegmentos<=_rayosSegmentos.conteo)
					UI16 iSeg, conteoSeg	= datosRayo->conteoSegmentos;
					for(iSeg=0; iSeg<conteoSeg; iSeg++){
						float avanceEste	= avancePorSegmento * (0.75f + (0.25f * (rand() % 1000) / 1000.0f));
						float desvioIzq		= anchoDesvioRayo * (((rand() % 1000)-500) / 1000.0f);
						avanceAcumTotal		+= avanceEste;
						margenAcumDeAnt		+= (avancePorSegmento - avanceEste);
						arrSegs[iSeg].avanceRelEnRayo = avanceAcumTotal / datosRayo->alcance;
						NBPUNTO_ESTABLECER(arrSegs[iSeg].posFinSegmento, datosRayo->posInicio.x + (datosRayo->vectorUnitario.x * avanceAcumTotal) + (datosRayo->vectorUnitIzq.x * desvioIzq), datosRayo->posInicio.y + (datosRayo->vectorUnitario.y * avanceAcumTotal) + (datosRayo->vectorUnitIzq.y * desvioIzq))
					}
					_rayosCajaLocalSucia = true;
				}
			} else {
				//Actualizar todos los indices de los rayos con segmentos posteriores a este
				UI16 iRayo2;
				for(iRayo2=0; iRayo2<_rayos.conteo; iRayo2++){
					if(_rayos.elemento[iRayo2].iPrimerSegmento>datosRayo->iPrimerSegmento) _rayos.elemento[iRayo2].iPrimerSegmento -= datosRayo->conteoSegmentos;
				}
				//Quitar segmentos y rayo
				_rayosSegmentos.quitarElementosEnIndice(datosRayo->iPrimerSegmento, datosRayo->conteoSegmentos);
				_rayos.quitarElementoEnIndice(iRayo);
				_rayosCajaLocalSucia = true;
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

//

PTRfunCmdsGL AUEscenaElectroRayo::actualizarModeloGL(const NBPropRenderizado &propsRenderizado, const NBPropIluminacion &propsIluminacion, const NBPropHeredadasRender &propsHeredadas){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaElectroRayo::actualizarModeloGL")
	NBASSERT(idEscena!=-1) //Solo si esta en escena
	UI16 conteoRayos					= _rayos.conteo;
	UI16 conteoSegmentos				= _rayosSegmentos.conteo;
	UI16 conteoVerticesGL				= ((conteoRayos * 4) + (conteoSegmentos * 2)) * 3;
	if(conteoVerticesGL!=0){
		NBColor colorMultiplicado;  NBCOLOR_MULTIPLICAR_CON_UI8(colorMultiplicado, propsHeredadas.colorPadre, _propiedades.color8)
		STBloqueGL bloqueVerticesGL;
		bloqueVerticesGL.cantidadElementos	= conteoVerticesGL;
		bloqueVerticesGL.primerElemento		= (*propsRenderizado.funcReservarVertices)(ENVerticeGlTipo_MonoTextura, bloqueVerticesGL.cantidadElementos);
		NBVerticeGL* verticesGLIzq			= &((*propsRenderizado.verticesGL1)[bloqueVerticesGL.primerElemento]);
		NBVerticeGL* verticesGLDer			= &((*propsRenderizado.verticesGL1)[bloqueVerticesGL.primerElemento+(bloqueVerticesGL.cantidadElementos/3)]);
		NBVerticeGL* verticesGLCen			= &((*propsRenderizado.verticesGL1)[bloqueVerticesGL.primerElemento+(bloqueVerticesGL.cantidadElementos*2/3)]);
		NBColor8 colorCentroRayo;			NBCOLOR_ESTABLECER(colorCentroRayo, 200*colorMultiplicado.r, 200*colorMultiplicado.g, 255*colorMultiplicado.b, 175*colorMultiplicado.a)
		NBColor8 colorAlrededorRayo;		NBCOLOR_ESTABLECER(colorAlrededorRayo, 155*colorMultiplicado.r, 155*colorMultiplicado.g, 255*colorMultiplicado.b, 25*colorMultiplicado.a)
		//Datos Render e indices por rayo
		const SI32 indiceDatosRender		= propsRenderizado.bytesDatosModelos->conteo; propsRenderizado.bytesDatosModelos->reservarRegistrosAlFinal(sizeof(STRayoRender));
		const SI32 indiceDatosIndicesGL		= propsRenderizado.bytesDatosModelos->conteo; propsRenderizado.bytesDatosModelos->reservarRegistrosAlFinal(sizeof(STBloqueGL) * conteoRayos);
		STRayoRender* datosRender			= (STRayoRender*)&(propsRenderizado.bytesDatosModelos->elemento[indiceDatosRender]);
		STBloqueGL* bloquesIndicesGL		= (STBloqueGL*)&(propsRenderizado.bytesDatosModelos->elemento[indiceDatosIndicesGL]);
		datosRender->cantidadRayos			= conteoRayos;
		//
		UI16 iRayo; NBPunto posEscenaIniRayo, posEscenaFinRayo, vecUnitEscenaRayo, vecUnitIzqEscenaRayo; float longitudEscenaRayo;
		for(iRayo=0; iRayo<conteoRayos; iRayo++){
			STRayo* datosRayo			= &_rayos.elemento[iRayo];
			NBMATRIZ_MULTIPLICAR_PUNTO(posEscenaIniRayo, _cacheObjEscena.matrizEscena, datosRayo->posInicio.x, datosRayo->posInicio.y)
			NBMATRIZ_MULTIPLICAR_PUNTO(posEscenaFinRayo, _cacheObjEscena.matrizEscena, datosRayo->posFin.x, datosRayo->posFin.y)
			NBPUNTO_ESTABLECER(vecUnitEscenaRayo, posEscenaFinRayo.x-posEscenaIniRayo.x, posEscenaFinRayo.y-posEscenaIniRayo.y)
			longitudEscenaRayo			= NBPUNTO_DISTANCIA_VECTOR(vecUnitEscenaRayo.x, vecUnitEscenaRayo.y);
			vecUnitEscenaRayo.x			/= longitudEscenaRayo;
			vecUnitEscenaRayo.y			/= longitudEscenaRayo;
			NBPUNTO_PRODUCTO_CRUZ_ESCALAR_X_VECTOR(vecUnitIzqEscenaRayo, 1.0f, vecUnitEscenaRayo.x, vecUnitEscenaRayo.y)
			//
			NBPunto posFinSegAct, posFinSegAnt = posEscenaIniRayo;
			NBPUNTO_ESTABLECER(verticesGLCen[0], posFinSegAnt.x, posFinSegAnt.y)
			NBPUNTO_ESTABLECER(verticesGLCen[1], posFinSegAnt.x, posFinSegAnt.y)
			NBCOLOR_ESTABLECER(verticesGLCen[0], colorCentroRayo.r, colorCentroRayo.g, colorCentroRayo.b, colorCentroRayo.a)
			NBCOLOR_ESTABLECER(verticesGLCen[1], colorCentroRayo.r, colorCentroRayo.g, colorCentroRayo.b, colorCentroRayo.a)
			NBPUNTO_ESTABLECER(verticesGLCen[0].tex, 0.0f, 0.0f)
			NBPUNTO_ESTABLECER(verticesGLCen[1].tex, 0.0f, 0.0f)
			NBPUNTO_ESTABLECER(verticesGLIzq[0], posFinSegAnt.x, posFinSegAnt.y)
			NBPUNTO_ESTABLECER(verticesGLIzq[1], posFinSegAnt.x, posFinSegAnt.y)
			NBCOLOR_ESTABLECER(verticesGLIzq[0], colorAlrededorRayo.r, colorAlrededorRayo.g, colorAlrededorRayo.b, colorAlrededorRayo.a)
			NBCOLOR_ESTABLECER(verticesGLIzq[1], colorAlrededorRayo.r, colorAlrededorRayo.g, colorAlrededorRayo.b, colorAlrededorRayo.a)
			NBPUNTO_ESTABLECER(verticesGLIzq[0].tex, 0.0f, 0.0f)
			NBPUNTO_ESTABLECER(verticesGLIzq[1].tex, 0.0f, 0.0f)
			NBPUNTO_ESTABLECER(verticesGLDer[0], posFinSegAnt.x, posFinSegAnt.y)
			NBPUNTO_ESTABLECER(verticesGLDer[1], posFinSegAnt.x, posFinSegAnt.y)
			NBCOLOR_ESTABLECER(verticesGLDer[0], colorAlrededorRayo.r, colorAlrededorRayo.g, colorAlrededorRayo.b, colorAlrededorRayo.a)
			NBCOLOR_ESTABLECER(verticesGLDer[1], colorAlrededorRayo.r, colorAlrededorRayo.g, colorAlrededorRayo.b, colorAlrededorRayo.a)
			NBPUNTO_ESTABLECER(verticesGLDer[0].tex, 0.0f, 0.0f)
			NBPUNTO_ESTABLECER(verticesGLDer[1].tex, 0.0f, 0.0f)
			verticesGLCen	+= 2;
			verticesGLIzq	+= 2;
			verticesGLDer	+= 2;
			STRayoSegmento* arrSegs	= &_rayosSegmentos.elemento[datosRayo->iPrimerSegmento];
			UI16 iSeg, conteoSeg	= datosRayo->conteoSegmentos;
			for(iSeg=0; iSeg<conteoSeg; iSeg++){
				float relAnchoRayo	= (0.5f - arrSegs[iSeg].avanceRelEnRayo - 0.5f) / 0.5f; if(relAnchoRayo<0.0f) relAnchoRayo = -relAnchoRayo;
				float anchoRayoExtX	= relAnchoRayo * vecUnitIzqEscenaRayo.x * 4.0f; //vecUnitIzqEscenaRayo
				float anchoRayoExtY	= relAnchoRayo * vecUnitIzqEscenaRayo.y * 4.0f; //vecUnitIzqEscenaRayo
				float anchoRayoCenX	= relAnchoRayo * vecUnitIzqEscenaRayo.x * 1.0f; //vecUnitIzqEscenaRayo
				float anchoRayoCenY	= relAnchoRayo * vecUnitIzqEscenaRayo.y * 1.0f; //vecUnitIzqEscenaRayo
				NBMATRIZ_MULTIPLICAR_PUNTO(posFinSegAct, _cacheObjEscena.matrizEscena, arrSegs[iSeg].posFinSegmento.x, arrSegs[iSeg].posFinSegmento.y)
				NBPUNTO_ESTABLECER(verticesGLCen[0], posFinSegAnt.x-anchoRayoCenX, posFinSegAnt.y-anchoRayoCenY)
				NBPUNTO_ESTABLECER(verticesGLCen[1], posFinSegAct.x+anchoRayoCenX, posFinSegAct.y+anchoRayoCenY)
				NBCOLOR_ESTABLECER(verticesGLCen[0], colorCentroRayo.r, colorCentroRayo.g, colorCentroRayo.b, colorCentroRayo.a)
				NBCOLOR_ESTABLECER(verticesGLCen[1], colorCentroRayo.r, colorCentroRayo.g, colorCentroRayo.b, colorCentroRayo.a)
				NBPUNTO_ESTABLECER(verticesGLCen[0].tex, 0.0f, 0.0f)
				NBPUNTO_ESTABLECER(verticesGLCen[1].tex, 0.0f, 0.0f)
				NBPUNTO_ESTABLECER(verticesGLIzq[0], posFinSegAnt.x-anchoRayoExtX, posFinSegAnt.y-anchoRayoExtY)
				NBPUNTO_ESTABLECER(verticesGLIzq[1], posFinSegAct.x, posFinSegAct.y)
				NBCOLOR_ESTABLECER(verticesGLIzq[0], colorAlrededorRayo.r, colorAlrededorRayo.g, colorAlrededorRayo.b, colorAlrededorRayo.a)
				NBCOLOR_ESTABLECER(verticesGLIzq[1], colorCentroRayo.r, colorCentroRayo.g, colorCentroRayo.b, colorCentroRayo.a)
				NBPUNTO_ESTABLECER(verticesGLIzq[0].tex, 0.0f, 0.0f)
				NBPUNTO_ESTABLECER(verticesGLIzq[1].tex, 0.0f, 0.0f)
				NBPUNTO_ESTABLECER(verticesGLDer[0], posFinSegAnt.x, posFinSegAnt.y)
				NBPUNTO_ESTABLECER(verticesGLDer[1], posFinSegAct.x+anchoRayoExtX, posFinSegAct.y+anchoRayoExtY)
				NBCOLOR_ESTABLECER(verticesGLDer[0], colorCentroRayo.r, colorCentroRayo.g, colorCentroRayo.b, colorCentroRayo.a)
				NBCOLOR_ESTABLECER(verticesGLDer[1], colorAlrededorRayo.r, colorAlrededorRayo.g, colorAlrededorRayo.b, colorAlrededorRayo.a)
				NBPUNTO_ESTABLECER(verticesGLDer[0].tex, 0.0f, 0.0f)
				NBPUNTO_ESTABLECER(verticesGLDer[1].tex, 0.0f, 0.0f)
				verticesGLCen	+= 2;
				verticesGLIzq	+= 2;
				verticesGLDer	+= 2;
				posFinSegAnt	= posFinSegAct;
			}
			posFinSegAct		= posEscenaFinRayo;
			NBPUNTO_ESTABLECER(verticesGLCen[0], posFinSegAnt.x, posFinSegAnt.y)
			NBPUNTO_ESTABLECER(verticesGLCen[1], posFinSegAct.x, posFinSegAct.y)
			NBCOLOR_ESTABLECER(verticesGLCen[0], colorCentroRayo.r, colorCentroRayo.g, colorCentroRayo.b, colorCentroRayo.a)
			NBCOLOR_ESTABLECER(verticesGLCen[1], colorCentroRayo.r, colorCentroRayo.g, colorCentroRayo.b, colorCentroRayo.a)
			NBPUNTO_ESTABLECER(verticesGLCen[0].tex, 0.0f, 0.0f)
			NBPUNTO_ESTABLECER(verticesGLCen[1].tex, 0.0f, 0.0f)
			NBPUNTO_ESTABLECER(verticesGLIzq[0], posFinSegAnt.x, posFinSegAnt.y)
			NBPUNTO_ESTABLECER(verticesGLIzq[1], posFinSegAct.x, posFinSegAct.y)
			NBCOLOR_ESTABLECER(verticesGLIzq[0], colorAlrededorRayo.r, colorAlrededorRayo.g, colorAlrededorRayo.b, colorAlrededorRayo.a)
			NBCOLOR_ESTABLECER(verticesGLIzq[1], colorAlrededorRayo.r, colorAlrededorRayo.g, colorAlrededorRayo.b, colorAlrededorRayo.a)
			NBPUNTO_ESTABLECER(verticesGLIzq[0].tex, 0.0f, 0.0f)
			NBPUNTO_ESTABLECER(verticesGLIzq[1].tex, 0.0f, 0.0f)
			NBPUNTO_ESTABLECER(verticesGLDer[0], posFinSegAnt.x, posFinSegAnt.y)
			NBPUNTO_ESTABLECER(verticesGLDer[1], posFinSegAct.x, posFinSegAct.y)
			NBCOLOR_ESTABLECER(verticesGLDer[0], colorAlrededorRayo.r, colorAlrededorRayo.g, colorAlrededorRayo.b, colorAlrededorRayo.a)
			NBCOLOR_ESTABLECER(verticesGLDer[1], colorAlrededorRayo.r, colorAlrededorRayo.g, colorAlrededorRayo.b, colorAlrededorRayo.a)
			NBPUNTO_ESTABLECER(verticesGLDer[0].tex, 0.0f, 0.0f)
			NBPUNTO_ESTABLECER(verticesGLDer[1].tex, 0.0f, 0.0f)
			verticesGLCen	+= 2;
			verticesGLIzq	+= 2;
			verticesGLDer	+= 2;
			//
			bloquesIndicesGL[iRayo] = NB_GESTOR_GL_DAME_ELEMS_PARA_TRIANGSTRIPS_N(ENVerticeGlTipo_MonoTextura, bloqueVerticesGL, 4 + (conteoSeg * 2), 3);
		}
		NBASSERT(verticesGLCen == (&((*propsRenderizado.verticesGL1)[bloqueVerticesGL.primerElemento]))+bloqueVerticesGL.cantidadElementos)
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return (conteoVerticesGL!=0 ? &AUEscenaElectroRayo::enviarComandosGL : NULL);
}

void AUEscenaElectroRayo::enviarComandosGL(UI8* punteroDatosModelo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaElectroRayo::enviarComandosGL")
	STRayoRender* datosRender	= (STRayoRender*)punteroDatosModelo;
	STBloqueGL* bloquesIndicesGL = (STBloqueGL*)(punteroDatosModelo + sizeof(STRayoRender));
	NBASSERT(datosRender->cantidadRayos>0)
	NBGestorGL::activarVerticesGL(ENVerticeGlTipo_MonoTextura); NBGESTORGL_DBG_NOMBRAR_ACTIVADOR_VERTICES_GL("AUEscenaElectroRayo")
	NBGestorGL::bindTexture(0, NBGestorTexturas::texturaBlanca->idTexturaGL); //required on some systems where texture-0 will not be drawn
	UI16 iRayo, conteoRayos = datosRender->cantidadRayos;
	for(iRayo=0; iRayo<conteoRayos; iRayo++){
		NBASSERT(bloquesIndicesGL[iRayo].cantidadElementos>0)
		NB_GESTOR_GL_RENDER_TRIANGSTRIPS_N(bloquesIndicesGL[iRayo].primerElemento, bloquesIndicesGL[iRayo].cantidadElementos, 3); NBGESTORGL_DBG_NOMBRAR_CONVOCADOR_ACUMULAR_TRIANG_STRIPS_INDEP("AUEscenaElectroRayo")
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

//
AUOBJMETODOS_CLASESID_MULTICLASE(AUEscenaElectroRayo, AUEscenaObjeto)
AUOBJMETODOS_CLASESNOMBRES_MULTICLASE(AUEscenaElectroRayo, "AUEscenaElectroRayo", AUEscenaObjeto)
AUOBJMETODOS_CLONAR_NULL(AUEscenaElectroRayo)






