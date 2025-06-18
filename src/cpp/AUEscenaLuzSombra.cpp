//
//  AUEscenaLuzSombra.cpp
//  Gameplay_iOS
//
//  Created by Marcos Ortega on 25/09/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkMediaStdAfx.h"
#include "AUEscenaLuzSombra.h"

AUEscenaLuzSombra::AUEscenaLuzSombra(AUArregloNativoP<NBPunto>* puntosFigura, AUTextura* texturaSombra) : AUEscenaObjeto(), _puntosFiguraSombra(this, puntosFigura) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaLuzSombra::AUEscenaLuzSombra")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaLuzSombra")
	_registradoEnMotorSombras		= false;
	_texturaSombra					= texturaSombra; if(_texturaSombra != NULL) _texturaSombra->retener(NB_RETENEDOR_THIS);
	this->privActualizarCajaLocalFiguraSombra();
	#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
	NBASSERT(_puntosFiguraSombra.conteo>1)
	if(_puntosFiguraSombra.conteo>2){
		NBASSERT(NBSegmentadorFiguras::figuraEsConvexaContrareloj(_puntosFiguraSombra.elemento, _puntosFiguraSombra.conteo))
	}
	#endif
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

AUEscenaLuzSombra::AUEscenaLuzSombra(NBPunto* puntosFigura, UI16 conteoPuntos, AUTextura* texturaSombra) : AUEscenaObjeto(), _puntosFiguraSombra(this, puntosFigura, conteoPuntos) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaLuzSombra::AUEscenaLuzSombra")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaLuzSombra")
	_registradoEnMotorSombras		= false;
	_texturaSombra					= texturaSombra; if(_texturaSombra != NULL) _texturaSombra->retener(NB_RETENEDOR_THIS);
	this->privActualizarCajaLocalFiguraSombra();
	#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
	NBASSERT(_puntosFiguraSombra.conteo>1)
	if(_puntosFiguraSombra.conteo>2){
		NBASSERT(NBSegmentadorFiguras::figuraEsConvexaContrareloj(_puntosFiguraSombra.elemento, _puntosFiguraSombra.conteo))
	}
	#endif
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

AUEscenaLuzSombra::AUEscenaLuzSombra(AUEscenaLuzSombra* otraInstancia) : AUEscenaObjeto(), _puntosFiguraSombra(this, &otraInstancia->_puntosFiguraSombra) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaLuzSombra::AUEscenaLuzSombra")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaLuzSombra")
	_registradoEnMotorSombras		= false;
	_texturaSombra					= otraInstancia->_texturaSombra; if(_texturaSombra != NULL) _texturaSombra->retener(NB_RETENEDOR_THIS);
	this->privActualizarCajaLocalFiguraSombra();
	NBASSERT(NBSegmentadorFiguras::figuraEsConvexaContrareloj(_puntosFiguraSombra.elemento, _puntosFiguraSombra.conteo))
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

AUEscenaLuzSombra::~AUEscenaLuzSombra(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaLuzSombra::~AUEscenaLuzSombra")
	NBASSERT(!_registradoEnMotorSombras) //No debe estar registrado
	if(_texturaSombra != NULL) _texturaSombra->retener(NB_RETENEDOR_THIS);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaLuzSombra::privActualizarCajaLocalFiguraSombra(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaLuzSombra::privActualizarCajaLocalFiguraSombra")
	NBCAJAAABB_INICIALIZAR(_cacheCajaLocalFiguraSombra)
	NBCAJAAABB_ENVOLVER_PUNTOS(_cacheCajaLocalFiguraSombra, _puntosFiguraSombra.elemento, _puntosFiguraSombra.conteo)
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

AUTextura* AUEscenaLuzSombra::texturaSombra(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaLuzSombra::establecerTexturaSombra")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _texturaSombra;
}

void AUEscenaLuzSombra::establecerTexturaSombra(AUTextura* textura){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaLuzSombra::establecerTexturaSombra")
	if(textura != NULL) textura->retener(NB_RETENEDOR_THIS);
	if(_texturaSombra != NULL) _texturaSombra->liberar(NB_RETENEDOR_THIS);
	_texturaSombra = textura;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaLuzSombra::agregadoEnEscena(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaLuzSombra::agregadoEnEscena")
	NBASSERT(!_registradoEnMotorSombras)
	NBGestorEscena::agregarProductorSombras(this->idEscena, this->idCapa, this);
	_registradoEnMotorSombras = true;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaLuzSombra::quitandoDeEscena(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaLuzSombra::quitandoDeEscena")
	NBASSERT(_registradoEnMotorSombras)
	NBGestorEscena::quitarProductorSombras(this->idEscena, this->idCapa, this);
	_registradoEnMotorSombras = false;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaLuzSombra::producirSombras2(const NBPropIluminacion &propsIluminacion){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaLuzSombra::producirSombras")
	if(!this->_dormido && _puntosFiguraSombra.conteo>1){
		if(_puntosFiguraSombra.conteo==2){
			//Optimizacion cuando la figura solo tiene dos vertices
			NBPunto verticesSombra[2];
			NBMATRIZ_MULTIPLICAR_PUNTO(verticesSombra[0], _cacheObjEscena.matrizEscena, _puntosFiguraSombra.elemento[0].x, _puntosFiguraSombra.elemento[0].y);
			NBMATRIZ_MULTIPLICAR_PUNTO(verticesSombra[1], _cacheObjEscena.matrizEscena, _puntosFiguraSombra.elemento[1].x, _puntosFiguraSombra.elemento[1].y);
			NBSegmentadorFiguras::producirSombraDosVertices(propsIluminacion.fuentesIluminacion, propsIluminacion.conteoFuentesIluminacion, propsIluminacion.sombrasPorFuenteIluminacion, propsIluminacion.sombrasVerticesIntermedios, verticesSombra[0], verticesSombra[1], this, this->_texturaSombra);
		} else {
			//Convertir los verticesLocales a verticesEscena
			NBASSERT(_puntosFiguraSombra.conteo<=16) //Si falla, entonces se rebalsara el buffer
			NBPunto verticesSombra[16];
			UI16 iVerticeSombra, conteoVerticesSombra = _puntosFiguraSombra.conteo;
			for(iVerticeSombra=0; iVerticeSombra<conteoVerticesSombra; iVerticeSombra++){
				NBMATRIZ_MULTIPLICAR_PUNTO(verticesSombra[iVerticeSombra], _cacheObjEscena.matrizEscena, _puntosFiguraSombra.elemento[iVerticeSombra].x, _puntosFiguraSombra.elemento[iVerticeSombra].y);
			}
			//Determinar el sentido de la figura escena
			NBPunto vectVertice1; NBPUNTO_ESTABLECER(vectVertice1, verticesSombra[1].x-verticesSombra[0].x, verticesSombra[1].y-verticesSombra[0].y)
			NBPunto vectVertice2; NBPUNTO_ESTABLECER(vectVertice2, verticesSombra[2].x-verticesSombra[0].x, verticesSombra[2].y-verticesSombra[0].y)
			float prodCruz		= NBPUNTO_PRODUCTO_VECTORIAL(vectVertice1.x, vectVertice1.y, vectVertice2.x, vectVertice2.y);
			NBSegmentadorFiguras::producirSombraMultiVertices(propsIluminacion.fuentesIluminacion, propsIluminacion.conteoFuentesIluminacion, propsIluminacion.sombrasPorFuenteIluminacion, propsIluminacion.sombrasVerticesIntermedios, verticesSombra, conteoVerticesSombra, this, prodCruz>=0.0f, this->_texturaSombra);
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

NBCajaAABB AUEscenaLuzSombra::cajaAABBLocalCalculada(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaLuzSombra::cajaAABBLocalCalculada")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _cacheCajaLocalFiguraSombra;
}

PTRfunCmdsGL AUEscenaLuzSombra::actualizarModeloGL(const NBPropRenderizado &propsRenderizado, const NBPropIluminacion &propsIluminacion, const NBPropHeredadasRender &propsHeredadas){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaLuzSombra::actualizarModeloGL")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return NULL;
}

//

bool AUEscenaLuzSombra::agregarXmlInternoEn(AUEscenaLuzSombra* sombra, AUCadenaLargaMutable8* guardarEn, const char* espaciosBaseIzq){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaLuzSombra::agregarXmlInternoEn")
	//guardarEn->agregarConFormato("%s<vXML>1</vXML>\r\n", espaciosBaseIzq);
	guardarEn->agregarConFormato("%s<eOb>\r\n", espaciosBaseIzq);
	AUEscenaObjeto::agregarXmlInternoEn(sombra, guardarEn, espaciosBaseIzq);
	guardarEn->agregarConFormato("%s</eOb>\r\n", espaciosBaseIzq);
	//Puntos
	guardarEn->agregarConFormato("%s<pts>\r\n", espaciosBaseIzq);
	UI16 iPnt, conteoPnt = sombra->_puntosFiguraSombra.conteo;
	for(iPnt=0; iPnt<conteoPnt; iPnt++){
		guardarEn->agregarConFormato("%s<p>%f|%f</p>\r\n", espaciosBaseIzq, sombra->_puntosFiguraSombra.elemento[iPnt].x, sombra->_puntosFiguraSombra.elemento[iPnt].y);
	}
	guardarEn->agregarConFormato("%s</pts>\r\n", espaciosBaseIzq);
	//Textura sombra
	if(sombra->_texturaSombra != NULL){
		const char* nombreTextura = NBGestorTexturas::nombreRecursoTextura(sombra->_texturaSombra);
		if(nombreTextura != NULL){
			guardarEn->agregarConFormato("%s<tex>%s</tex>\r\n", espaciosBaseIzq, nombreTextura);
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return true;
	
}

bool AUEscenaLuzSombra::interpretarXmlInterno(AUEscenaLuzSombra* cargarEn, const AUDatosXML* datosXml, const sNodoXML* nodoXml){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaLuzSombra::interpretarXmlInterno")
	bool exito = false;
	//int versionXML = datosXml->nodoHijo("vXML", 0, nodoXml);
	//if(versionXML==1){
	const sNodoXML* nodoEscenaObj;			XML_NODO_HIJO(datosXml, nodoEscenaObj, "eOb", "escObj", nodoXml, NULL)
	if(nodoEscenaObj != NULL){
		exito = AUEscenaObjeto::interpretarXmlInterno(cargarEn, datosXml, nodoEscenaObj);
		//Puntos
		const sNodoXML* nodoPuntos	= datosXml->nodoHijo("pts", nodoXml);
		if(nodoPuntos != NULL){
			const sNodoXML* nodoPto	= datosXml->nodoHijo("p", nodoPuntos);
			while(nodoPto != NULL){
				NBPunto punto;
				const char* cadenaCSV	= datosXml->cadenaValorDeNodo(nodoPto);
				SI32 tamCadenaCSV		= AUCadena8::tamano(cadenaCSV);
				SI32 posEnCadenaCSV		= 0;
				punto.x					= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0.0f);
				punto.y					= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0.0f);
				cargarEn->_puntosFiguraSombra.agregarElemento(punto);
				//Siguiente
				nodoPto	= datosXml->nodoHijo("p", nodoPuntos, nodoPto);
			}
		}
		//Textura sombra
		AUCadenaMutable8* nomTextura = new(ENMemoriaTipo_Temporal) AUCadenaMutable8();
		datosXml->nodoHijo("tex", nomTextura, "", nodoXml);
		if(nomTextura->tamano()!=0){
			cargarEn->establecerTexturaSombra(NBGestorTexturas::texturaDesdeArchivo(nomTextura->str()));
		}
		nomTextura->liberar(NB_RETENEDOR_NULL);
	}
	//}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

bool AUEscenaLuzSombra::agregarBitsInternosEn(AUEscenaLuzSombra* sombra, AUArchivo* guardarEn){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaLuzSombra::agregarBitsInternosEn")
	bool exito = false;
	//
	float versionDatos = 1.0f;
	guardarEn->escribir(&versionDatos, sizeof(versionDatos), 1, guardarEn);
	if(AUEscenaObjeto::agregarBitsInternosEn(sombra, guardarEn)){
		//Puntos
		sombra->_puntosFiguraSombra.agregarBitsInternosEn(guardarEn);
		//Textura sombra
		const char* nombreTextura = NULL;
		SI32 tamanoNombreTextura = 0;
		if(sombra->_texturaSombra != NULL) nombreTextura = NBGestorTexturas::nombreRecursoTextura(sombra->_texturaSombra);
		if(nombreTextura == NULL){
			guardarEn->escribir(&tamanoNombreTextura, sizeof(tamanoNombreTextura), 1, guardarEn);
		} else {
			tamanoNombreTextura = AUCadenaMutable8::tamano(nombreTextura);
			guardarEn->escribir(&tamanoNombreTextura, sizeof(tamanoNombreTextura), 1, guardarEn);
			guardarEn->escribir(nombreTextura, sizeof(char), tamanoNombreTextura, guardarEn);
		}
		exito = true;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

bool AUEscenaLuzSombra::interpretarBitsInternos(AUEscenaLuzSombra* cargarEn, AUArchivo* cargarDe){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaLuzSombra::interpretarBitsInternos")
	bool exito = false;
	float versionDatos;
	cargarDe->leer(&versionDatos, sizeof(versionDatos), 1, cargarDe);
	if(versionDatos==1.0f){
		if(AUEscenaObjeto::interpretarBitsInternos(cargarEn, cargarDe)){
			//Puntos
			cargarEn->_puntosFiguraSombra.interpretarBitsInternos(cargarDe);
			//Textura sombra
			SI32 tamanoNombreTextura;
			cargarDe->leer(&tamanoNombreTextura, sizeof(tamanoNombreTextura), 1, cargarDe);
			if(tamanoNombreTextura>0){
				char* nombreTextura = (char*)NBGestorMemoria::reservarMemoria(tamanoNombreTextura+1, ENMemoriaTipo_Temporal); NB_DEFINE_NOMBRE_PUNTERO(nombreTextura, "AUEscenaLuzSombra::nombreTextura") //+1 es el "\0"
				cargarDe->leer(nombreTextura, sizeof(char), tamanoNombreTextura, cargarDe);
				nombreTextura[tamanoNombreTextura] = '\0';
				cargarEn->establecerTexturaSombra(NBGestorTexturas::texturaDesdeArchivo(nombreTextura));
				NBGestorMemoria::liberarMemoria(nombreTextura);
			}
			exito = true;
		}
	} else {
		PRINTF_ERROR("AUEscenaLuzSombra, la version (%f) de datos binarios no es interpretable por este ejecutable\n", versionDatos);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

//
AUOBJMETODOS_CLASESID_MULTICLASE(AUEscenaLuzSombra, AUEscenaObjeto)
AUOBJMETODOS_CLASESNOMBRES_MULTICLASE(AUEscenaLuzSombra, "AUEscenaLuzSombra", AUEscenaObjeto)
AUOBJMETODOS_CLONAR_THIS(AUEscenaLuzSombra)







