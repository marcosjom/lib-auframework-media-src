//
//  AUEscenaTextoFade.cpp
//  AUFramework
//
//  Created by Marcos Ortega Morales on 10/11/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkMediaStdAfx.h"
#include "AUEscenaTextoFade.h"

AUEscenaTextoFade::AUEscenaTextoFade() : AUEscenaTexto() {
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaTextoFade")
	_fadeUsarValorAbs = false;
	_fadeVectEsLocal = false;
	NBPUNTO_ESTABLECER(_fadeVector.inicio, 0.0f, 0.0f)
	NBPUNTO_ESTABLECER(_fadeVector.delta, 0.0f, 0.0f)
	NBCOLOR_ESTABLECER(_fadeColor.inicial, 1.0f, 1.0f, 1.0f, 1.0f)
	NBCOLOR_ESTABLECER(_fadeColor.final, 1.0f, 1.0f, 1.0f, 1.0f)
}

AUEscenaTextoFade::AUEscenaTextoFade(AUFuenteRender* fuenteRender) : AUEscenaTexto(fuenteRender) {
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaTextoFade")
	_fadeUsarValorAbs = false;
	_fadeVectEsLocal = false;
	NBPUNTO_ESTABLECER(_fadeVector.inicio, 0.0f, 0.0f)
	NBPUNTO_ESTABLECER(_fadeVector.delta, 0.0f, 0.0f)
	NBCOLOR_ESTABLECER(_fadeColor.inicial, 1.0f, 1.0f, 1.0f, 1.0f)
	NBCOLOR_ESTABLECER(_fadeColor.final, 1.0f, 1.0f, 1.0f, 1.0f)
}

AUEscenaTextoFade::AUEscenaTextoFade(AUEscenaTextoFade* otraInstancia) : AUEscenaTexto(otraInstancia) {
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaTextoFade")
	_fadeUsarValorAbs	= otraInstancia->_fadeUsarValorAbs;
	_fadeVectEsLocal	= otraInstancia->_fadeVectEsLocal;
	_fadeVector.inicio		= otraInstancia->_fadeVector.inicio;
	_fadeVector.delta	= otraInstancia->_fadeVector.delta;
	_fadeColor.inicial	= otraInstancia->_fadeColor.inicial;
	_fadeColor.final	= otraInstancia->_fadeColor.final;
}

AUEscenaTextoFade::~AUEscenaTextoFade(){
	//
}

//

bool AUEscenaTextoFade::fadeUsaValorAbsoluto(){
	return _fadeUsarValorAbs;
}

bool AUEscenaTextoFade::fadeUsaCoordLocal(){
	return _fadeVectEsLocal;
}

NBFadeVector AUEscenaTextoFade::fadeVector(){
	return _fadeVector;
}

NBFadeColor AUEscenaTextoFade::fadeColor(){
	return _fadeColor;
}

//

void AUEscenaTextoFade::establecerFadeUsarValorAbsoluto(const bool usarValorAbsoluto){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaTextoFade::establecerFadeUsarValorAbsoluto")
	_fadeUsarValorAbs = usarValorAbsoluto;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaTextoFade::establecerFadeVector(const float xIni,const float yIni, const float deltaX, const float deltaY, const bool esCoordLocal){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaTextoFade::establecerFadeVector")
	_fadeVectEsLocal = esCoordLocal;
	_fadeVector.inicio.x = xIni; _fadeVector.inicio.y = yIni;
	_fadeVector.delta.x = deltaX; _fadeVector.delta.y = deltaY;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaTextoFade::establecerFadeVector(const NBPunto ini, const NBPunto fin, const bool esCoordLocal){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaTextoFade::establecerFadeVector")
	_fadeVectEsLocal	= esCoordLocal;
	_fadeVector.inicio	= ini;
	_fadeVector.delta	= fin;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaTextoFade::establecerFadeColores(const float rIni, const float gIni, const float bIni, const float aIni, const float rFin, const float gFin, const float bFin, const float aFin){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaTextoFade::establecerFadeColores")
	_fadeColor.inicial.r = rIni; _fadeColor.inicial.g = gIni; _fadeColor.inicial.b = bIni; _fadeColor.inicial.a = aIni;
	_fadeColor.final.r = rFin; _fadeColor.final.g = gFin; _fadeColor.final.b = bFin; _fadeColor.final.a = aFin;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaTextoFade::establecerFadeColores(const NBColorF ini, const NBColorF fin){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaTextoFade::establecerFadeColores")
	_fadeColor.inicial = ini;
	_fadeColor.final = fin;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

//

PTRfunCmdsGL AUEscenaTextoFade::actualizarModeloGL(const NBPropRenderizado &propsRenderizado, const NBPropIluminacion &propsIluminacion, const NBPropHeredadasRender &propsHeredadas){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaTextoFade::actualizarModeloGL")
	NBASSERT(idEscena!=-1) //Solo si esta en escena
	const SI32 indiceDatosRender		= propsRenderizado.bytesDatosModelos->conteo;
	PTRfunCmdsGL ptrCmds = AUEscenaTexto::actualizarModeloGL(propsRenderizado, propsIluminacion, propsHeredadas);
	/*if(ptrCmds != NULL && (this->_fadeVector.delta.x != 0.0f || this->_fadeVector.delta.y != 0.0f)){
		NBPunto posIniEsc, vecUnitEsc;
		if(!this->_fadeVectEsLocal){
			posIniEsc	= this->_fadeVector.inicio;
			vecUnitEsc	= this->_fadeVector.delta;
		} else {
			NBPunto posFinLocal; NBPUNTO_ESTABLECER(posFinLocal, this->_fadeVector.inicio.x + this->_fadeVector.delta.x, this->_fadeVector.inicio.y + this->_fadeVector.delta.y)
			NBMATRIZ_MULTIPLICAR_PUNTO(posIniEsc, _cacheObjEscena.matrizEscena, this->_fadeVector.inicio.x, this->_fadeVector.inicio.y)
			NBMATRIZ_MULTIPLICAR_PUNTO(vecUnitEsc, _cacheObjEscena.matrizEscena, posFinLocal.x, posFinLocal.y)
			vecUnitEsc.x -= posIniEsc.x;
			vecUnitEsc.y -= posIniEsc.y;
		}
		if(vecUnitEsc.x != 0.0f || vecUnitEsc.y != 0.0f){
			const float vecModulo = NBPUNTO_DISTANCIA_VECTOR(vecUnitEsc.x, vecUnitEsc.y);
			if(vecModulo != 0.0f){
				vecUnitEsc.x /= vecModulo; vecUnitEsc.y /= vecModulo;
				//
				UI8* ptrEnDatos = propsRenderizado.bytesDatosModelos->elemPtr(indiceDatosRender);
				while(1){
					const STFuenteRenderEnc enc = *((STFuenteRenderEnc*)ptrEnDatos);
					if(enc.modoRender >= ENFuenteRenderBlq_Fin){
						NBASSERT(enc.modoRender == ENFuenteRenderBlq_Fin)
						break;
					} else {
						NBASSERT(enc.fuenteRender != NULL)
						const ENFuenteRenderBlq modo = *((ENFuenteRenderBlq*)ptrEnDatos);
						if(modo == ENFuenteRenderBlq_Sprite){
							STFuenteRenderTex* datosRender	= (STFuenteRenderTex*)ptrEnDatos; ptrEnDatos += sizeof(STFuenteRenderTex);
							/ *STFuenteRenderTexGrp* arrGrps = (STFuenteRenderTexGrp*)ptrEnDatos;* / ptrEnDatos += sizeof(STFuenteRenderTexGrp) * datosRender->conteoGrpsTextura;
							NBASSERT(datosRender->bloqueIndicesGL.cantidadElementos > 0)
							NBVerticeGL0* vertices		= &(*propsRenderizado.verticesGL0)[datosRender->bloqueVerticesGL.primerElemento];
							this->privAplicarColorFade(vertices, datosRender->bloqueVerticesGL.cantidadElementos, posIniEsc, vecUnitEsc, vecModulo);
						} else if(modo == ENFuenteRenderBlq_Raster){
							STFuenteRenderRaster* datosRender = (STFuenteRenderRaster*)ptrEnDatos; ptrEnDatos += sizeof(STFuenteRenderRaster);
							STFuenteRenderRasterGrp* arrGrps = (STFuenteRenderRasterGrp*)ptrEnDatos; ptrEnDatos += sizeof(STFuenteRenderRasterGrp) * datosRender->conteoGrpsRaster;
							UI32 i; const UI32 conteo = datosRender->conteoGrpsRaster;
							for(i = 0; i < conteo; i++){
								STFuenteRenderRasterGrp* datGrp = &arrGrps[i];
								if(datGrp->iVao == ENVerticeGlTipo_SinTextura){
									if(datGrp->conteoPuntos > 0){
										NBVerticeGL0* vertices		= &(*propsRenderizado.verticesGL0)[datGrp->iPrimerPunto];
										this->privAplicarColorFade(vertices, datGrp->conteoPuntos, posIniEsc, vecUnitEsc, vecModulo);
									}
									if(datGrp->conteoLineas > 0){
										NBVerticeGL0* vertices		= &(*propsRenderizado.verticesGL0)[datGrp->iPrimerLinea];
										this->privAplicarColorFade(vertices, datGrp->conteoLineas * 2, posIniEsc, vecUnitEsc, vecModulo);
									}
								}
							}
						} else {
							NBASSERT(false)
						}
					}
				}
			}
		}
	}*/
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return ptrCmds;
}

void AUEscenaTextoFade::privAplicarColorFade(NBVerticeGL0* vertices, const UI32 cantVertices, const NBPunto posIniEsc, const NBPunto vecUnitEsc, const float vecModulo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaTextoFade::privAplicarColorFade")
	const NBVerticeGL0* vertSigUlt = (vertices + cantVertices);
	NBPunto vectorRel; float distEnVector, avanceRel;
	while(vertices < vertSigUlt){
		vectorRel.x		= vertices[0].x - posIniEsc.x;
		vectorRel.y		= vertices[0].y - posIniEsc.y;
		distEnVector	=  NBPUNTO_PRODUCTO_PUNTO(vectorRel.x, vectorRel.y, vecUnitEsc.x, vecUnitEsc.y); if(distEnVector < 0.0f && this->_fadeUsarValorAbs) distEnVector = -distEnVector;
		if(distEnVector <= 0.0f){
			vertices[0].r	*= this->_fadeColor.inicial.r;
			vertices[0].g	*= this->_fadeColor.inicial.g;
			vertices[0].b	*= this->_fadeColor.inicial.b;
			vertices[0].a	*= this->_fadeColor.inicial.a;
		} else if(distEnVector >= vecModulo){
			vertices[0].r	*= this->_fadeColor.final.r;
			vertices[0].g	*= this->_fadeColor.final.g;
			vertices[0].b	*= this->_fadeColor.final.b;
			vertices[0].a	*= this->_fadeColor.final.a;
		} else {
			avanceRel		= (distEnVector / vecModulo);
			vertices[0].r	*= this->_fadeColor.inicial.r + ((this->_fadeColor.final.r - this->_fadeColor.inicial.r) * avanceRel);
			vertices[0].g	*= this->_fadeColor.inicial.g + ((this->_fadeColor.final.g - this->_fadeColor.inicial.g) * avanceRel);
			vertices[0].b	*= this->_fadeColor.inicial.b + ((this->_fadeColor.final.b - this->_fadeColor.inicial.b) * avanceRel);
			vertices[0].a	*= this->_fadeColor.inicial.a + ((this->_fadeColor.final.a - this->_fadeColor.inicial.a) * avanceRel);
		}
		vertices++;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

//

bool AUEscenaTextoFade::agregarXmlInternoEn(AUEscenaTextoFade* texto, AUCadenaLargaMutable8* guardarEn, const char* espaciosBaseIzq){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaTextoFade::agregarXmlInternoEn")
	bool r = false;
	if(AUEscenaTexto::agregarXmlInternoEn(texto, guardarEn, espaciosBaseIzq)){
		guardarEn->agregarConFormato("%s<fade>\r\n", espaciosBaseIzq);
		//guardarEn->agregarConFormato("%s<vXML>1</vXML>\r\n", espaciosBaseIzq);
		guardarEn->agregarConFormato("%s<abs>%d</abs>\r\n", espaciosBaseIzq, (texto->_fadeUsarValorAbs ? 1 : 0));
		guardarEn->agregarConFormato("%s<loc>%d</loc>\r\n", espaciosBaseIzq, (texto->_fadeVectEsLocal ? 1 : 0));
		guardarEn->agregarConFormato("%s<vec>%f|%f|%f|%f</vec>\r\n", espaciosBaseIzq, texto->_fadeVector.inicio.x, texto->_fadeVector.inicio.y, texto->_fadeVector.delta.x, texto->_fadeVector.delta.y);
		guardarEn->agregarConFormato("%s<col>%f|%f|%f|%f|%f|%f|%f|%f</col>\r\n", espaciosBaseIzq, texto->_fadeColor.inicial.r, texto->_fadeColor.inicial.g, texto->_fadeColor.inicial.b, texto->_fadeColor.inicial.a, texto->_fadeColor.final.r, texto->_fadeColor.final.g, texto->_fadeColor.final.b, texto->_fadeColor.final.a);
		guardarEn->agregarConFormato("%s</fade>\r\n", espaciosBaseIzq);
		r = true;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return r;
}

bool AUEscenaTextoFade::interpretarXmlInterno(AUEscenaTextoFade* cargarEn, const AUDatosXML* datosXml, const sNodoXML* nodoXml){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaTextoFade::interpretarXmlInterno")
	bool r = false;
	if(AUEscenaTexto::interpretarXmlInterno(cargarEn, datosXml, nodoXml)){
		const sNodoXML* nodoFade		=  datosXml->nodoHijo("fade", nodoXml);
		if(nodoFade != NULL){
			const SI32 esAbs			= datosXml->nodoHijo("abs", 0, nodoFade);
			const SI32 esLocal			= datosXml->nodoHijo("loc", 0, nodoFade);
			NBPunto vecIni, vecDel;
			NBColorF colIni, colFin;
			const sNodoXML* nodoVec		= datosXml->nodoHijo("vec", nodoXml);
			const sNodoXML* nodoCol		= datosXml->nodoHijo("col", nodoXml);
			if(nodoVec != NULL){
				const char* cadenaCSV	= datosXml->cadenaValorDeNodo(nodoVec);
				SI32 tamCadenaCSV		= AUCadena8::tamano(cadenaCSV);
				SI32 posEnCadenaCSV		= 0;
				vecIni.x				= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0.0f);
				vecIni.y				= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0.0f);
				vecDel.x				= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0.0f);
				vecDel.y				= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0.0f);
			} else {
				NBPUNTO_ESTABLECER(vecIni, 0.0f, 0.0f)
				NBPUNTO_ESTABLECER(vecDel, 0.0f, 0.0f)
			}
			if(nodoCol != NULL){
				const char* cadenaCSV	= datosXml->cadenaValorDeNodo(nodoCol);
				SI32 tamCadenaCSV		= AUCadena8::tamano(cadenaCSV);
				SI32 posEnCadenaCSV		= 0;
				colIni.r				= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 1.0f);
				colIni.g				= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 1.0f);
				colIni.b				= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 1.0f);
				colIni.a				= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 1.0f);
				colFin.r				= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 1.0f);
				colFin.g				= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 1.0f);
				colFin.b				= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 1.0f);
				colFin.a				= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 1.0f);
			} else {
				NBCOLOR_ESTABLECER(colIni, 1.0f, 1.0f, 1.0f, 1.0f)
				NBCOLOR_ESTABLECER(colFin, 1.0f, 1.0f, 1.0f, 1.0f)
			}
			cargarEn->_fadeUsarValorAbs	= (esAbs != 0);
			cargarEn->_fadeVectEsLocal	= (esLocal != 0);
			cargarEn->_fadeVector.inicio= vecIni;
			cargarEn->_fadeVector.delta	= vecDel;
			cargarEn->_fadeColor.inicial= colIni;
			cargarEn->_fadeColor.final	= colFin;
			r = true;
		}
	}
	//}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return r;
}

bool AUEscenaTextoFade::agregarBitsInternosEn(AUEscenaTextoFade* texto, AUArchivo* guardarEn){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaTextoFade::agregarBitsInternosEn")
	bool r = false;
	if(AUEscenaTexto::agregarBitsInternosEn(texto, guardarEn)){
		const float versionDatos = 1.0f;
		const UI8 esAbs			= (texto->_fadeUsarValorAbs ? 1 : 0);
		const UI8 esLocal		= (texto->_fadeVectEsLocal ? 1 : 0);
		const NBPunto vecIni	= texto->_fadeVector.inicio;
		const NBPunto vecDel	= texto->_fadeVector.delta;
		const NBColorF colIni	= texto->_fadeColor.inicial;
		const NBColorF colFin	= texto->_fadeColor.final;
		guardarEn->escribir(&versionDatos, sizeof(versionDatos), 1, guardarEn);
		guardarEn->escribir(&esAbs, sizeof(esAbs), 1, guardarEn);
		guardarEn->escribir(&esLocal, sizeof(esLocal), 1, guardarEn);
		guardarEn->escribir(&vecIni, sizeof(vecIni), 1, guardarEn);
		guardarEn->escribir(&vecDel, sizeof(vecDel), 1, guardarEn);
		guardarEn->escribir(&colIni, sizeof(colIni), 1, guardarEn);
		guardarEn->escribir(&colFin, sizeof(colFin), 1, guardarEn);
		r = true;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return r;
}

bool AUEscenaTextoFade::interpretarBitsInternos(AUEscenaTextoFade* cargarEn, AUArchivo* cargarDe){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaTextoFade::interpretarBitsInternos")
	bool r = false;
	if(AUEscenaTexto::interpretarBitsInternos(cargarEn, cargarDe)){
		float versionDatos; cargarDe->leer(&versionDatos, sizeof(versionDatos), 1, cargarDe);
		UI8 esAbs;			cargarDe->leer(&esAbs, sizeof(esAbs), 1, cargarDe);
		UI8 esLocal;		cargarDe->leer(&esLocal, sizeof(esLocal), 1, cargarDe);
		NBPunto vecIni;		cargarDe->leer(&vecIni, sizeof(vecIni), 1, cargarDe);
		NBPunto vecDel;		cargarDe->leer(&vecDel, sizeof(vecDel), 1, cargarDe);
		NBColorF colIni;	cargarDe->leer(&colIni, sizeof(colIni), 1, cargarDe);
		NBColorF colFin;	cargarDe->leer(&colFin, sizeof(colFin), 1, cargarDe);
		cargarEn->_fadeUsarValorAbs		= (esAbs != 0);
		cargarEn->_fadeVectEsLocal		= (esLocal != 0);
		cargarEn->_fadeVector.inicio	= vecIni;
		cargarEn->_fadeVector.delta		= vecDel;
		cargarEn->_fadeColor.inicial	= colIni;
		cargarEn->_fadeColor.final		= colFin;
		r = true;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return r;
}

//
AUOBJMETODOS_CLASESID_UNICLASE(AUEscenaTextoFade)
AUOBJMETODOS_CLASESNOMBRES_UNICLASE(AUEscenaTextoFade, "AUEscenaTextoFade")
AUOBJMETODOS_CLONAR_THIS(AUEscenaTextoFade)






