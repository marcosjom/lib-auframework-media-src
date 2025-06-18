//
//  AUEscenaRuta.cpp
//  Gameplay_iOS
//
//  Created by Marcos Ortega Morales on 27/09/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkMediaStdAfx.h"
#include "AUEscenaRuta.h"

AUEscenaRuta::AUEscenaRuta() : AUEscenaObjeto(), _verticesRuta(this) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaRuta::AUEscenaRuta")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaRuta")
	_longitudTotalRecorrido		= 0.0f;
	//
	_rutaCajaLocalSucia			= true;
	NBCAJAAABB_INICIALIZAR(_rutaCajaLocal);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

AUEscenaRuta::AUEscenaRuta(AUEscenaRuta* otra) : AUEscenaObjeto(otra), _verticesRuta(this, &otra->_verticesRuta) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaRuta::AUEscenaRuta")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaRuta")
	_longitudTotalRecorrido		= otra->_longitudTotalRecorrido;
	NBASSERT(debugVeritificarCacheDeRuta())
	//
	_rutaCajaLocalSucia			= true;
	NBCAJAAABB_INICIALIZAR(_rutaCajaLocal);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

AUEscenaRuta::~AUEscenaRuta(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaRuta::~AUEscenaRuta")
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

AUArregloNativoP<STRutaVertice>* AUEscenaRuta::verticesRuta(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaRuta::verticesRuta")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return &_verticesRuta;
}

NBPunto AUEscenaRuta::posicionEnRutaDeAvance(const float avanceEnRuta){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaRuta::posicionEnRutaDeAvance")
	NBASSERT(_verticesRuta.conteo>1)
	NBPunto posicion;
	if(avanceEnRuta<=0.0f){
		posicion.x	= _verticesRuta.elemento[0].posicion.x + (_verticesRuta.elemento[0].vecUnitSegmento.x * avanceEnRuta); //Avanza hacia atras
		posicion.y	= _verticesRuta.elemento[0].posicion.y + (_verticesRuta.elemento[0].vecUnitSegmento.y * avanceEnRuta); //Avanza hacia atras
	} else if(avanceEnRuta>=_longitudTotalRecorrido){
		posicion.x	= _verticesRuta.elemento[_verticesRuta.conteo-1].posicion.x + (_verticesRuta.elemento[_verticesRuta.conteo-2].vecUnitSegmento.x * (avanceEnRuta-_longitudTotalRecorrido)); //Avanza hacia adelante
		posicion.y	= _verticesRuta.elemento[_verticesRuta.conteo-1].posicion.y + (_verticesRuta.elemento[_verticesRuta.conteo-2].vecUnitSegmento.y * (avanceEnRuta-_longitudTotalRecorrido)); //Avanza hacia adelante
	} else {
		posicion	= _verticesRuta.elemento[_verticesRuta.conteo-1].posicion;
		float avanceAcum = 0.0f;
		UI16 iVert, conteVert		= _verticesRuta.conteo;
		for(iVert=0; iVert<conteVert; iVert++){
			float nuevoAvanceAcum	= avanceAcum + _verticesRuta.elemento[iVert].longitudSegmento;
			if(nuevoAvanceAcum>avanceEnRuta){
				posicion.x			= _verticesRuta.elemento[iVert].posicion.x + (_verticesRuta.elemento[iVert].vecUnitSegmento.x * (avanceEnRuta-avanceAcum));
				posicion.y			= _verticesRuta.elemento[iVert].posicion.y + (_verticesRuta.elemento[iVert].vecUnitSegmento.y * (avanceEnRuta-avanceAcum));
				break;
			}
			avanceAcum				= nuevoAvanceAcum;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return posicion;
}

STRutaVertice AUEscenaRuta::verticeEnAvance(const float avanceEnRuta){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaRuta::verticeEnAvance")
	NBASSERT(_verticesRuta.conteo>1)
	STRutaVertice vertice;
	if(avanceEnRuta<=0.0f){
		vertice		= _verticesRuta.elemento[0];
	} else if(avanceEnRuta>=_longitudTotalRecorrido){
		vertice		= _verticesRuta.elemento[_verticesRuta.conteo-2];
	} else {
		vertice		= _verticesRuta.elemento[_verticesRuta.conteo-2];
		float avanceAcum = 0.0f;
		UI16 iVert, conteVert		= _verticesRuta.conteo;
		for(iVert=0; iVert<conteVert; iVert++){
			float nuevoAvanceAcum	= avanceAcum + _verticesRuta.elemento[iVert].longitudSegmento;
			if(nuevoAvanceAcum>avanceEnRuta){
				vertice		= _verticesRuta.elemento[iVert];
				break;
			}
			avanceAcum				= nuevoAvanceAcum;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return vertice;
}

void AUEscenaRuta::privActualizarCacheVerticesRuta(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaRuta::privActualizarCacheVerticesRuta")
	float longitudAcum = 0.0f;
	UI16 iVert, conteVert	= _verticesRuta.conteo;
	if(conteVert>1){
		NBPunto posAnterior		= _verticesRuta.elemento[0].posicion;
		for(iVert=1; iVert<conteVert; iVert++){
			NBPunto posActual	= _verticesRuta.elemento[iVert].posicion;
			float longitud		= NBPUNTO_DISTANCIA(posAnterior.x, posAnterior.y, posActual.x, posActual.y);
			float radianes;		NBPUNTO_RADIANES_VECTOR(radianes, posAnterior.x, posAnterior.y, posActual.x, posActual.y)
			STRutaVertice* vAnterior		= &_verticesRuta.elemento[iVert-1];
			vAnterior->longitudSegmento		= longitud;
			vAnterior->radianesSegmento		= radianes;
			vAnterior->longitudSegmentosAnteriores = longitudAcum;
			vAnterior->vecUnitSegmento.x	= (posActual.x - posAnterior.x) / longitud;
			vAnterior->vecUnitSegmento.y	= (posActual.y - posAnterior.y) / longitud;
			longitudAcum += longitud;
			posAnterior = posActual;
		}
	}
	/*} else if(_tipoRuta==ENRutaTipo_Circulo){
		UI16 iVert, conteVert	= _verticesRuta.conteo;
		if(conteVert>1){
			NBPunto posCentro		= _verticesRuta.elemento[0].posicion;
			NBPunto posRadio		= _verticesRuta.elemento[1].posicion;
			NBPunto vectorRadio;	NBPUNTO_ESTABLECER(vectorRadio, posRadio.x-posCentro.x, posRadio.y-posCentro.y)
			float radio				= NBPUNTO_DISTANCIA_VECTOR(vectorRadio.x, vectorRadio.y);
			float radianesMin;		NBPUNTO_RADIANES_VECTOR(radianesMin, posCentro.x, posCentro.y, vectorRadio.x, vectorRadio.y);
			float radianesMax;
			if(conteVert>2){
				NBPunto posMaxRad = _verticesRuta.elemento[2].posicion;
				NBPUNTO_RADIANES_VECTOR(radianesMax, posCentro.x, posCentro.y, posMaxRad.x, posMaxRad.y);
				if(radianesMin>radianesMax) radianesMin -= PIx2;
			} else {
				radianesMax = radianesMin + PIx2;
			}
			float perimetroCircunferencia	= (PIx2 * radio);
			float proprocionCircunferencia	= (radianesMax - radianesMin) / PIx2;
			longitudAcum = (perimetroCircunferencia * proprocionCircunferencia);
			for(iVert=1; iVert<conteVert; iVert++){
				NBPunto posActual	= _verticesRuta.elemento[iVert].posicion;
				float longitud		= NBPUNTO_DISTANCIA(posCentro.x, posCentro.y, posActual.x, posActual.y);
				float radianes;		NBPUNTO_RADIANES_VECTOR(radianes, posCentro.x, posCentro.y, posActual.x, posActual.y)
				_verticesRuta.elemento[iVert].longitudSegmento = longitud;
				_verticesRuta.elemento[iVert].radianesSegmento = radianes;
			}
			_verticesRuta.elemento[0].longitudSegmento = longitudAcum;
			_verticesRuta.elemento[0].radianesSegmento = 0.0f;
		}*/
	_longitudTotalRecorrido = longitudAcum;
	_rutaCajaLocalSucia = true;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
bool AUEscenaRuta::debugVeritificarCacheDeRuta(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaRuta::debugVeritificarCacheDeRuta")
	float longitudAcum = 0.0f;
	UI16 iVert, conteVert	= _verticesRuta.conteo;
	if(conteVert>1){
		NBPunto posAnterior		= _verticesRuta.elemento[0].posicion;
		for(iVert=1; iVert<conteVert; iVert++){
			NBPunto posActual	= _verticesRuta.elemento[iVert].posicion;
			float longitud		= NBPUNTO_DISTANCIA(posAnterior.x, posAnterior.y, posActual.x, posActual.y);
			float radianes;		NBPUNTO_RADIANES_VECTOR(radianes, posAnterior.x, posAnterior.y, posActual.x, posActual.y)
			float diffLongitud	= longitud - _verticesRuta.elemento[iVert-1].longitudSegmento;
			float diffRadianes	= radianes - _verticesRuta.elemento[iVert-1].radianesSegmento;
			NBASSERT(diffLongitud>-0.01f && diffLongitud<0.01f)
			NBASSERT(diffRadianes>-0.01f && diffRadianes<0.01f)
			longitudAcum += longitud;
			posAnterior = posActual;
		}
	}
	/*} else if(_tipoRuta==ENRutaTipo_Circulo){
		UI16 iVert, conteVert	= _verticesRuta.conteo;
		if(conteVert>1){
			NBPunto posCentro		= _verticesRuta.elemento[0].posicion;
			NBPunto posRadio		= _verticesRuta.elemento[1].posicion;
			NBPunto vectorRadio;	NBPUNTO_ESTABLECER(vectorRadio, posRadio.x-posCentro.x, posRadio.y-posCentro.y)
			float radio				= NBPUNTO_DISTANCIA_VECTOR(vectorRadio.x, vectorRadio.y);
			float radianesMin;		NBPUNTO_RADIANES_VECTOR(radianesMin, posCentro.x, posCentro.y, vectorRadio.x, vectorRadio.y);
			float radianesMax;
			if(conteVert>2){
				NBPunto posMaxRad = _verticesRuta.elemento[2].posicion;
				NBPUNTO_RADIANES_VECTOR(radianesMax, posCentro.x, posCentro.y, posMaxRad.x, posMaxRad.y);
				if(radianesMin>radianesMax) radianesMin -= PIx2;
			} else {
				radianesMax = radianesMin + PIx2;
			}
			float perimetroCircunferencia	= (PIx2 * radio);
			float proprocionCircunferencia	= (radianesMax - radianesMin) / PIx2;
			longitudAcum = (perimetroCircunferencia * proprocionCircunferencia);
			for(iVert=1; iVert<conteVert; iVert++){
				NBPunto posActual	= _verticesRuta.elemento[iVert].posicion;
				float longitud		= NBPUNTO_DISTANCIA(posCentro.x, posCentro.y, posActual.x, posActual.y);
				float radianes;		NBPUNTO_RADIANES_VECTOR(radianes, posCentro.x, posCentro.y, posActual.x, posActual.y)
				float diffLongitud	= longitud - _verticesRuta.elemento[iVert].longitudSegmento;
				float diffRadianes	= radianes - _verticesRuta.elemento[iVert].radianesSegmento;
				NBASSERT(diffLongitud>-0.01f && diffLongitud<0.01f)
				NBASSERT(diffRadianes>-0.01f && diffRadianes<0.01f)
			}
			float diffCircun = longitudAcum - _verticesRuta.elemento[0].longitudSegmento;
			NBASSERT(diffCircun>-0.01f && diffCircun<0.01f)
		}
	}*/
	NBASSERT(_longitudTotalRecorrido==longitudAcum)
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return true;
}
#endif

float AUEscenaRuta::longitudTotalRuta(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaRuta::longitudTotalRuta")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _longitudTotalRecorrido;
}

float AUEscenaRuta::longitudTotalRutaCalculada(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaRuta::longitudTotalRutaCalculada")
	float longitudAcum = 0.0f;
	UI16 iVert, conteVert		= _verticesRuta.conteo;
	if(conteVert>1){
		NBPunto posAnterior		= _verticesRuta.elemento[0].posicion;
		for(iVert=1; iVert<conteVert; iVert++){
			NBPunto posActual	= _verticesRuta.elemento[iVert].posicion;
			float longitud		= NBPUNTO_DISTANCIA(posAnterior.x, posAnterior.y, posActual.x, posActual.y);
			longitudAcum		+= longitud;
			posAnterior			= posActual;
		}
	}
	/*} else if(_tipoRuta==ENRutaTipo_Circulo){
		UI16 conteVert				= _verticesRuta.conteo;
		if(conteVert>1){
			NBPunto posCentro		= _verticesRuta.elemento[0].posicion;
			NBPunto posRadio		= _verticesRuta.elemento[1].posicion;
			NBPunto vectorRadio;	NBPUNTO_ESTABLECER(vectorRadio, posRadio.x-posCentro.x, posRadio.y-posCentro.y)
			float radio				= NBPUNTO_DISTANCIA_VECTOR(vectorRadio.x, vectorRadio.y);
			float radianesMin;		NBPUNTO_RADIANES_VECTOR(radianesMin, posCentro.x, posCentro.y, vectorRadio.x, vectorRadio.y);
			float radianesMax;
			if(conteVert>2){
				NBPunto posMaxRad	= _verticesRuta.elemento[2].posicion;
				NBPUNTO_RADIANES_VECTOR(radianesMax, posCentro.x, posCentro.y, posMaxRad.x, posMaxRad.y);
				if(radianesMin>radianesMax) radianesMin -= PIx2;
			} else {
				radianesMax			= radianesMin + PIx2;
			}
			float perimetroCircunferencia	= (PIx2 * radio);
			float proprocionCircunferencia	= (radianesMax - radianesMin) / PIx2;
			longitudAcum = (perimetroCircunferencia * proprocionCircunferencia);
		}
	 */
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return longitudAcum;
}

void AUEscenaRuta::agregarVerticeRuta(float xPosEnContenedorPadre, float yPosEnContenedorPadre, float factorVelocidad, float segundosEspera){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaRuta::agregarVerticeRuta")
	STRutaVertice datosVertice;
	datosVertice.posicion.x			= xPosEnContenedorPadre;
	datosVertice.posicion.y			= yPosEnContenedorPadre;
	datosVertice.factorVelocidad	= factorVelocidad;
	datosVertice.segundossEspera	= segundosEspera;
	datosVertice.radianesSegmento	= 0.0f;
	datosVertice.longitudSegmento	= 0.0f;
	datosVertice.longitudSegmentosAnteriores = 0.0f;
	datosVertice.vecUnitSegmento.x	= 1.0f;
	datosVertice.vecUnitSegmento.y	= 1.0f;
	_verticesRuta.agregarElemento(datosVertice);
	privActualizarCacheVerticesRuta();
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaRuta::agregarVerticeEnPuntoMedioSegmentoRuta(UI16 iVerticeSegmentoInicia){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaRuta::agregarVerticeEnPuntoMedioSegmentoRuta")
	NBASSERT(iVerticeSegmentoInicia<_verticesRuta.conteo)
	UI16 iVerticeFinal	= (iVerticeSegmentoInicia==_verticesRuta.conteo-1?0:iVerticeSegmentoInicia+1);
	STRutaVertice vIni	= _verticesRuta.elemento[iVerticeSegmentoInicia];
	STRutaVertice vFin	= _verticesRuta.elemento[iVerticeFinal];
	STRutaVertice datosVertice;
	NBPUNTO_ESTABLECER(datosVertice.posicion, vIni.posicion.x+((vFin.posicion.x-vIni.posicion.x)*0.5f), vIni.posicion.y+((vFin.posicion.y-vIni.posicion.y)*0.5f));
	datosVertice.factorVelocidad	= vIni.factorVelocidad+((vFin.factorVelocidad-vIni.factorVelocidad)*0.5f);
	datosVertice.segundossEspera	= vIni.segundossEspera+((vFin.segundossEspera-vIni.segundossEspera)*0.5f);
	datosVertice.radianesSegmento	= 0.0f;
	datosVertice.longitudSegmento	= 0.0f;
	datosVertice.longitudSegmentosAnteriores = 0.0f;
	datosVertice.vecUnitSegmento.x	= 1.0f;
	datosVertice.vecUnitSegmento.y	= 1.0f;
	_verticesRuta.agregarElementoEnIndice(datosVertice, iVerticeSegmentoInicia+1);
	privActualizarCacheVerticesRuta();
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaRuta::actualizarVerticeRuta(UI16 iVertice, float xPosEnContenedorPadre, float yPosEnContenedorPadre, float factorVelocidad, float segundosEspera){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaRuta::actualizarVerticeRuta")
	NBASSERT(iVertice<_verticesRuta.conteo)
	_verticesRuta.elemento[iVertice].posicion.x = xPosEnContenedorPadre;
	_verticesRuta.elemento[iVertice].posicion.y = yPosEnContenedorPadre;
	_verticesRuta.elemento[iVertice].factorVelocidad	= factorVelocidad;
	_verticesRuta.elemento[iVertice].segundossEspera	= segundosEspera;
	privActualizarCacheVerticesRuta();
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaRuta::quitarVerticeRutaEnIndice(UI16 iVertice){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaRuta::actualizarVerticeRuta")
	NBASSERT(iVertice<_verticesRuta.conteo)
	_verticesRuta.quitarElementoEnIndice(iVertice);
	privActualizarCacheVerticesRuta();
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaRuta::vaciarVerticesRuta(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaRuta::vaciarVerticesRuta")
	_verticesRuta.vaciar();
	_longitudTotalRecorrido = 0.0f;
	_rutaCajaLocalSucia = true;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

//

NBCajaAABB AUEscenaRuta::cajaAABBLocalCalculada(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaRuta::cajaAABBLocalCalculada")
	if(_rutaCajaLocalSucia){
		NBCAJAAABB_INICIALIZAR(_rutaCajaLocal)
		UI32 iVert, iVertConteo = _verticesRuta.conteo;
		for(iVert=0; iVert<iVertConteo; iVert++){
			NBPunto pos = _verticesRuta.elemento[iVert].posicion;
			NBCAJAAABB_ENVOLVER_PUNTO(_rutaCajaLocal, pos.x, pos.y);
		}
		//Bindar un poco de volumen
		if(_rutaCajaLocal.xMin==_rutaCajaLocal.xMax){ _rutaCajaLocal.xMin -= 8.0f; _rutaCajaLocal.xMax += 8.0f; }
		if(_rutaCajaLocal.yMin==_rutaCajaLocal.yMax){ _rutaCajaLocal.yMin -= 8.0f; _rutaCajaLocal.yMax += 8.0f; }
		_rutaCajaLocalSucia = false;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _rutaCajaLocal;
}

PTRfunCmdsGL AUEscenaRuta::actualizarModeloGL(const NBPropRenderizado &propsRenderizado, const NBPropIluminacion &propsIluminacion, const NBPropHeredadasRender &propsHeredadas){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaRuta::actualizarModeloGL")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return NULL;
}

//

bool AUEscenaRuta::agregarXmlInternoEn(AUEscenaRuta* ruta, AUCadenaLargaMutable8* guardarEn, const char* espaciosBaseIzq){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AULanzallamas::agregarXmlInternoEn")
	//guardarEn->agregarConFormato("%s<vXML>1</vXML>\r\n", espaciosBaseIzq);
	guardarEn->agregarConFormato("%s<eOb>\r\n", espaciosBaseIzq);
	AUEscenaObjeto::agregarXmlInternoEn(ruta, guardarEn, espaciosBaseIzq);
	guardarEn->agregarConFormato("%s</eOb>\r\n", espaciosBaseIzq);
	guardarEn->agregarConFormato("%s<vs>\r\n", espaciosBaseIzq);
	UI16 iVert, conteoVerts = ruta->_verticesRuta.conteo;
	for(iVert=0; iVert<conteoVerts; iVert++){
		STRutaVertice v = ruta->_verticesRuta.elemento[iVert];
		guardarEn->agregarConFormato("%s<v>%f|%f|%f|%f</v>\r\n", espaciosBaseIzq, v.posicion.x, v.posicion.y, v.factorVelocidad, v.segundossEspera);
	}
	guardarEn->agregarConFormato("%s</vs>\r\n", espaciosBaseIzq);
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return true;
	
}

bool AUEscenaRuta::interpretarXmlInterno(AUEscenaRuta* cargarEn, const AUDatosXML* datosXml, const sNodoXML* nodoXml){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AULanzallamas::interpretarXmlInterno")
	bool exito = false;
	//int versionXML = datosXml->nodoHijo("vXML", 0, nodoXml);
	//if(versionXML==1){
	const sNodoXML* nodoEscenaObj;			XML_NODO_HIJO(datosXml, nodoEscenaObj, "eOb", "escObj", nodoXml, NULL)
	if(nodoEscenaObj != NULL){
		exito = AUEscenaObjeto::interpretarXmlInterno(cargarEn, datosXml, nodoEscenaObj);
		if(exito){
			//
			const sNodoXML* nodoVertices		= datosXml->nodoHijo("vs", nodoXml);
			if(nodoVertices != NULL){
				const sNodoXML* nodoV			= datosXml->nodoHijo("v", nodoVertices);
				while(nodoV != NULL){
					const char* cadenaCSV		= datosXml->cadenaValorDeNodo(nodoV);
					SI32 tamCadenaCSV			= AUCadena8::tamano(cadenaCSV);
					SI32 posEnCadenaCSV			= 0;
					NBPunto posVertice;
					posVertice.x				= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0.0f);
					posVertice.y				= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0.0f);
					float factorVelocidad		= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 1.0f);
					float segsEspera			= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0.0f);
					cargarEn->agregarVerticeRuta(posVertice.x, posVertice.y, factorVelocidad, segsEspera);
					//Siguiente
					nodoV			= datosXml->nodoHijo("v", nodoVertices, nodoV);
				}
			}
		}
		cargarEn->_rutaCajaLocalSucia = true;
	}
	//}
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return exito;
}

bool AUEscenaRuta::agregarBitsInternosEn(AUEscenaRuta* ruta, AUArchivo* guardarEn){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaRuta::agregarBitsInternosEn")
	bool exito = false;
	float versionDatos = 1.0f;
	guardarEn->escribir(&versionDatos, sizeof(versionDatos), 1, guardarEn);
	if(AUEscenaObjeto::agregarBitsInternosEn(ruta, guardarEn)){
		guardarEn->escribir(&ruta->_longitudTotalRecorrido, sizeof(ruta->_longitudTotalRecorrido), 1, guardarEn);
		ruta->_verticesRuta.agregarBitsInternosEn(guardarEn);
		exito = true;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return exito;
}

bool AUEscenaRuta::interpretarBitsInternos(AUEscenaRuta* cargarEn, AUArchivo* cargarDe){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaRuta::interpretarBitsInternos")
	bool exito = false;
	float versionDatos;
	cargarDe->leer(&versionDatos, sizeof(versionDatos), 1, cargarDe);
	if(versionDatos==1.0f){
		if(AUEscenaObjeto::interpretarBitsInternos(cargarEn, cargarDe)){
			cargarDe->leer(&cargarEn->_longitudTotalRecorrido, sizeof(cargarEn->_longitudTotalRecorrido), 1, cargarDe);
			cargarEn->_verticesRuta.interpretarBitsInternos(cargarDe);
			cargarEn->_rutaCajaLocalSucia = true;
			exito = true;
		}
	} else {
		PRINTF_ERROR("AUEscenaRuta, la version (%f) de datos binarios no es interpretable por este ejecutable\n", versionDatos);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return exito;
}

//
AUOBJMETODOS_CLASESID_MULTICLASE(AUEscenaRuta, AUEscenaObjeto)
AUOBJMETODOS_CLASESNOMBRES_MULTICLASE(AUEscenaRuta, "AUEscenaRuta", AUEscenaObjeto)
AUOBJMETODOS_CLONAR_THIS(AUEscenaRuta)












