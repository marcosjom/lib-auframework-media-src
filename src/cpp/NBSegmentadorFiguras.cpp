//
//  NBSegmentadorFiguras.cpp
//  AUFramework
//
//  Created by Marcos Ortega on 22/11/12.
//
//

#include "AUFrameworkMediaStdAfx.h"
#include "NBSegmentadorFiguras.h"

AUArregloNativoMutableP<NBPunto>* NBSegmentadorFiguras::_figuras[2];
AUArregloMutable*	NBSegmentadorFiguras::_cacheFiguras;
UI16				NBSegmentadorFiguras::_indiceFiguraActual;
UI16				NBSegmentadorFiguras::_cacheConteoFigurasUsadas;

#ifdef CONFIG_NB_SEGMENTADOR_FIGURAS_ESCRIBIR_LOG
AUCadenaLargaMutable8*	NBSegmentadorFiguras::_dbgStr;
#endif

void NBSegmentadorFiguras::inicializar(){
	_cacheConteoFigurasUsadas	= 0;
	_indiceFiguraActual			= 0;
	_figuras[0]					= new(ENMemoriaTipo_General) AUArregloNativoMutableP<NBPunto>(NBSEGMENTADORFIGURAS_CACHE_VERTICES_INICIALES);
	_figuras[1]					= new(ENMemoriaTipo_General) AUArregloNativoMutableP<NBPunto>(NBSEGMENTADORFIGURAS_CACHE_VERTICES_INICIALES);
	_cacheFiguras				= new(ENMemoriaTipo_General) AUArregloMutable(NBSEGMENTADORFIGURAS_CACHE_FIGURAS_INICIALES);
	UI16 iFig;
	for(iFig=0; iFig<NBSEGMENTADORFIGURAS_CACHE_FIGURAS_INICIALES; iFig++){
		AUArregloNativoMutableP<NBPunto>* nuevosVertices = new(ENMemoriaTipo_General) AUArregloNativoMutableP<NBPunto>(NBSEGMENTADORFIGURAS_CACHE_VERTICES_INICIALES);
		_cacheFiguras->agregarElemento(nuevosVertices);
		nuevosVertices->liberar(NB_RETENEDOR_NULL);
	}
	#ifdef CONFIG_NB_SEGMENTADOR_FIGURAS_ESCRIBIR_LOG
	_dbgStr						= new(ENMemoriaTipo_General) AUCadenaLargaMutable8();
	#endif
}

void NBSegmentadorFiguras::finalizar(){
	if(_figuras[0] != NULL)	_figuras[0]->liberar(NB_RETENEDOR_NULL);		_figuras[0] = NULL;
	if(_figuras[1] != NULL)	_figuras[1]->liberar(NB_RETENEDOR_NULL);		_figuras[1] = NULL;
	if(_cacheFiguras != NULL)	_cacheFiguras->liberar(NB_RETENEDOR_NULL);	_cacheFiguras = NULL;
	_cacheConteoFigurasUsadas = 0;
	#ifdef CONFIG_NB_SEGMENTADOR_FIGURAS_ESCRIBIR_LOG
	if(_dbgStr != NULL) _dbgStr->liberar(NB_RETENEDOR_NULL); _dbgStr = NULL;
	#endif
}

AUArregloNativoMutableP<NBPunto>** NBSegmentadorFiguras::cacheFiguras(){
	return (AUArregloNativoMutableP<NBPunto>**)_cacheFiguras->elemento;
}

UI16 NBSegmentadorFiguras::cacheConteoFiguras(){
	return _cacheConteoFigurasUsadas;
}

void NBSegmentadorFiguras::privCacheVaciar(){
	_cacheConteoFigurasUsadas = 0;
}

AUArregloNativoMutableP<NBPunto>* NBSegmentadorFiguras::privCacheReservarNuevaFigura(){
	AUArregloNativoMutableP<NBPunto>* arrVertices = NULL;
	if(_cacheConteoFigurasUsadas<_cacheFiguras->conteo){
		arrVertices = (AUArregloNativoMutableP<NBPunto>*) _cacheFiguras->elemento[_cacheConteoFigurasUsadas];
		arrVertices->vaciar();
	} else {
		arrVertices = new(ENMemoriaTipo_General) AUArregloNativoMutableP<NBPunto>(NBSEGMENTADORFIGURAS_CACHE_VERTICES_INICIALES);
		_cacheFiguras->agregarElemento(arrVertices);
		arrVertices->liberar(NB_RETENEDOR_NULL);
	}
	_cacheConteoFigurasUsadas++;
	return arrVertices;
}

#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
bool NBSegmentadorFiguras::figuraEsConvexaContrareloj(const NBPunto* vertices, const UI16 conteoVertices){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("NBSegmentadorFiguras::figuraEsConvexaContrareloj")
	bool exito = (conteoVertices>=3); NBASSERT(conteoVertices>=3)
	NBPunto verticeIniSegAnt = vertices[conteoVertices-2];
	NBPunto verticeFinSegAnt = vertices[conteoVertices-1];
	SI32 iVert; NBPunto vector1, vector2;
	for(iVert=0; iVert<conteoVertices; iVert++){
		NBPunto verticeActual = vertices[iVert];
		NBPUNTO_ESTABLECER(vector1, verticeFinSegAnt.x-verticeIniSegAnt.x, verticeFinSegAnt.y-verticeIniSegAnt.y)
		NBPUNTO_ESTABLECER(vector2, verticeActual.x-verticeIniSegAnt.x, verticeActual.y-verticeIniSegAnt.y)
		float productoCruz	= NBPUNTO_PRODUCTO_VECTORIAL(vector1.x, vector1.y, vector2.x, vector2.y);
		if(productoCruz<=0.0f){
			PRINTF_INFO("Figura no es convexa-contrareloj\n");
			NBPunto verticeIniSegAnt = vertices[conteoVertices-2];
			NBPunto verticeFinSegAnt = vertices[conteoVertices-1];
			for(iVert=0; iVert<conteoVertices; iVert++){
				NBPunto verticeActual = vertices[iVert];
				NBPUNTO_ESTABLECER(vector1, verticeFinSegAnt.x-verticeIniSegAnt.x, verticeFinSegAnt.y-verticeIniSegAnt.y)
				NBPUNTO_ESTABLECER(vector2, verticeActual.x-verticeIniSegAnt.x, verticeActual.y-verticeIniSegAnt.y)
				PRINTF_INFO("Vertice %d) (%.4f, %.4f) (prodCruz: %.8f)\n", iVert, vertices[iVert].x, vertices[iVert].y, NBPUNTO_PRODUCTO_VECTORIAL(vector1.x, vector1.y, vector2.x, vector2.y));
				verticeIniSegAnt	= verticeFinSegAnt;
				verticeFinSegAnt	= verticeActual;
			}
			NBASSERT(false)
			exito = false;
			break;
		}
		verticeIniSegAnt	= verticeFinSegAnt;
		verticeFinSegAnt	= verticeActual;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return exito;
}
#endif

#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
bool NBSegmentadorFiguras::figuraNoRepiteVertices(const NBPunto* vertices, const UI16 conteoVertices){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("NBSegmentadorFiguras::figuraNoRepiteVertices")
	bool exito = (conteoVertices>=3); NBASSERT(conteoVertices>=3)
	NBPunto verticeAnterior = vertices[conteoVertices-1];
	UI16 iVert;
	for(iVert=0; iVert<conteoVertices; iVert++){
		NBPunto verticeActual = vertices[iVert];
		if(verticeAnterior==verticeActual){
			PRINTF_INFO("Figura tiene vertices repetidos\n");
			NBPunto vAnt = vertices[conteoVertices-1];
			for(iVert=0; iVert<conteoVertices; iVert++){ PRINTF_INFO("Vertice %d) (%.4f, %.4f)%s\n", iVert, vertices[iVert].x, vertices[iVert].y, vAnt==vertices[iVert]?" REPETIDO":""); vAnt = vertices[iVert]; }
			NBASSERT(false)
			exito = false;
			break;
		}
		verticeAnterior	= verticeActual;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return exito;
}
#endif

void NBSegmentadorFiguras::privAgregarOrejasNoConvexasDeFigura(AUArregloNativoMutableP<NBPunto>* figura){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("NBSegmentadorFiguras::privAgregarOrejasNoConvexasDeFigura")
	NBASSERT(figura->conteo>=3)
	NBASSERT(NBSegmentadorFiguras::figuraNoRepiteVertices(figura->elemento, figura->conteo))
	//
	NBPunto* arrVerts	= figura->elemento;	//Esta variable debe actualizarse cada vez que se modifique el arreglo
	UI16 conteoVerts	= figura->conteo;	//Esta variable debe actualizarse cada vez que se modifique el arreglo
	SI16 iVertAnt2		= conteoVerts - 2;
	SI32 iVertAnt1		= conteoVerts - 1;
	float prodCruzAnt	= 1000.0f;
	float prodCruzAct	= 1000.0f;
	SI16 iVert			= 0;
	SI16 dbgConteoVertsRecorridos = 0; //Para evitar ciclos infinitos
	NBPunto vectSegAnt, vectVertAct; bool figuraModificada;
	do {
		figuraModificada	=	false;
		NBPUNTO_ESTABLECER(vectSegAnt, arrVerts[iVertAnt1].x-arrVerts[iVertAnt2].x, arrVerts[iVertAnt1].y-arrVerts[iVertAnt2].y);
		NBPUNTO_ESTABLECER(vectVertAct, arrVerts[iVert].x-arrVerts[iVertAnt2].x, arrVerts[iVert].y-arrVerts[iVertAnt2].y);
		prodCruzAct			= NBPUNTO_PRODUCTO_VECTORIAL(vectSegAnt.x, vectSegAnt.y, vectVertAct.x, vectVertAct.y);
		if(prodCruzAct==0.0f){
			//Son colineales (quitar el vertice de enmedio)
			figura->quitarElementoEnIndice(iVertAnt1);
			figuraModificada = true;
		} else if(prodCruzAnt<0.0f && prodCruzAct>0.0f){
			//Es una oreja convexa, justo despues de un segmento no-convexo (extirpar)
			AUArregloNativoMutableP<NBPunto>* subFiguraOreja = NBSegmentadorFiguras::privCacheReservarNuevaFigura();
			subFiguraOreja->agregarElemento(arrVerts[iVertAnt2]);
			subFiguraOreja->agregarElemento(arrVerts[iVertAnt1]);
			subFiguraOreja->agregarElemento(arrVerts[iVert]);
			NBASSERT(NBSegmentadorFiguras::figuraNoRepiteVertices(subFiguraOreja->elemento, subFiguraOreja->conteo))
			NBASSERT(NBSegmentadorFiguras::figuraEsConvexaContrareloj(subFiguraOreja->elemento, subFiguraOreja->conteo))
			//Quitar de figura original el vertice exclusivo de la oreja
			figura->quitarElementoEnIndice(iVertAnt1);
			figuraModificada = true;
		}
		if(figuraModificada){
			arrVerts		= figura->elemento;	//Esta variable debe actualizarse cada vez que se modifique el arreglo
			conteoVerts		= figura->conteo;	//Esta variable debe actualizarse cada vez que se modifique el arreglo
			iVertAnt2		= conteoVerts - 2;
			iVertAnt1		= conteoVerts - 1;
			prodCruzAnt		= 1000.0f;
			prodCruzAct		= 1000.0f;
			iVert			= 0;
			dbgConteoVertsRecorridos = 0;
		} else {
			iVertAnt2		= iVertAnt1;
			iVertAnt1		= iVert;
			iVert++;		if(iVert>=conteoVerts) iVert -= conteoVerts;
			prodCruzAnt		= prodCruzAct;
			dbgConteoVertsRecorridos++;
			if(dbgConteoVertsRecorridos>=conteoVerts*2) break;
		}
		NBASSERT(figura->elemento==arrVerts)
		NBASSERT((SI32)figura->conteo==conteoVerts)
		NBASSERT(figura->conteo>2)
		#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
		if(dbgConteoVertsRecorridos>conteoVerts){
			PRINTF_ERROR("EXTIRPANDO-OREJA: Se han recorrido %d de %d vertices (no quedan segmentos convexos):\n", dbgConteoVertsRecorridos, conteoVerts);
			NBPunto p2	= arrVerts[conteoVerts-2];
			NBPunto p1	= arrVerts[conteoVerts-1];
			UI16 dbgVert;
			for(dbgVert=0; dbgVert<conteoVerts; dbgVert++){
				NBPunto p = arrVerts[dbgVert];
				NBPunto vec1; NBPUNTO_ESTABLECER(vec1, p1.x-p2.x, p1.y-p2.y);
				NBPunto vec2; NBPUNTO_ESTABLECER(vec2, p.x-p2.x, p.y-p2.y);
				PRINTF_INFO("%d) [%.4f, %.4f] prodCurz(%.8f)\n", dbgVert, p.x, p.y, NBPUNTO_PRODUCTO_VECTORIAL(vec1.x, vec1.y, vec2.x, vec2.y));
				p2 = p1;
				p1 = p;
			}
		}
		#endif
	} while(iVert!=0 || figuraModificada || prodCruzAnt<=0.0f || prodCruzAct<=0.0f);
	//NBASSERT(NBSegmentadorFiguras::figuraNoRepiteVertices(figura->elemento, figura->conteo))
	//NBASSERT(NBSegmentadorFiguras::figuraEsConvexaContrareloj(figura->elemento, figura->conteo))
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

bool NBSegmentadorFiguras::sombraEsCubiertaPorOtraYCubrirOtrasDosVertices(NBFuenteSombra* arrOtrasSombras, const UI16 conteoOtrasSombras, const NBPunto* bibVerticesIntermediosSombras, const NBPunto &vectorMasIzquierda, const NBPunto &vectorMasDerecha){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("NBSegmentadorFiguras::sombraEsCubiertaPorOtraYCubrirOtrasDosVertices")
	NBASSERT(NBPUNTO_PRODUCTO_VECTORIAL(vectorMasDerecha.x, vectorMasDerecha.y, vectorMasIzquierda.x, vectorMasIzquierda.y)>0.0f) //No se admiten lineas (==0.0f) ni orden invertido (<0.0f)
	bool cubiertaPorOtraSombra					= false;
	#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
	UI16 conteoOtrasSombrasCubiertas			= 0;
	#endif
	UI16 iOtraSombra;
	for(iOtraSombra=0; iOtraSombra<conteoOtrasSombras; iOtraSombra++){
		NBFuenteSombra* otraSombra			= &arrOtrasSombras[iOtraSombra];
		if(!otraSombra->sombraCubiertaPorOtra){
			const float prodCruzMinVsOtraMin	= NBPUNTO_PRODUCTO_VECTORIAL(vectorMasDerecha.x, vectorMasDerecha.y, otraSombra->vectorAnguloMin.x, otraSombra->vectorAnguloMin.y);
			const float prodCruzMaxVsOtraMin	= NBPUNTO_PRODUCTO_VECTORIAL(vectorMasIzquierda.x, vectorMasIzquierda.y, otraSombra->vectorAnguloMin.x, otraSombra->vectorAnguloMin.y);
			const float prodCruzMaxVsOtraMax	= NBPUNTO_PRODUCTO_VECTORIAL(vectorMasIzquierda.x, vectorMasIzquierda.y, otraSombra->vectorAnguloMax.x, otraSombra->vectorAnguloMax.y);
			const float prodCruzMinVsOtraMax	= NBPUNTO_PRODUCTO_VECTORIAL(vectorMasDerecha.x, vectorMasDerecha.y, otraSombra->vectorAnguloMax.x, otraSombra->vectorAnguloMax.y);
			//Verificar si ESTA sombra es totalmente cubierta por la OTRA
			if(!cubiertaPorOtraSombra && prodCruzMinVsOtraMin<=0.0f && prodCruzMinVsOtraMax>=0.0f && prodCruzMaxVsOtraMin<=0.0f && prodCruzMaxVsOtraMax>=0.0f){
				bool tieneVerticesFueraDeOtra	= false;
				NBPunto verticeOtraAnt			= otraSombra->vectorAnguloMax;
				if(otraSombra->conteoVerticesAdicionalSombra!=0){
					const NBPunto* verticeOtra			= &bibVerticesIntermediosSombras[otraSombra->iPrimerVerticeAdicionalSombra];
					const NBPunto* verticesOtraSigUlt	= verticeOtra + otraSombra->conteoVerticesAdicionalSombra;
					while(verticeOtra<verticesOtraSigUlt){
						if(NBPUNTO_PRODUCTO_VECTORIAL(verticeOtra->x - verticeOtraAnt.x, verticeOtra->y - verticeOtraAnt.y, vectorMasDerecha.x - verticeOtraAnt.x, vectorMasDerecha.y - verticeOtraAnt.y)<0.0f ||
						   NBPUNTO_PRODUCTO_VECTORIAL(verticeOtra->x - verticeOtraAnt.x, verticeOtra->y - verticeOtraAnt.y, vectorMasIzquierda.x - verticeOtraAnt.x, vectorMasIzquierda.y - verticeOtraAnt.y)<0.0f){
							tieneVerticesFueraDeOtra = true;
							break;
						}
						verticeOtraAnt = *verticeOtra;
						verticeOtra++;
					}
				}
				if(NBPUNTO_PRODUCTO_VECTORIAL(otraSombra->vectorAnguloMin.x - verticeOtraAnt.x, otraSombra->vectorAnguloMin.y - verticeOtraAnt.y, vectorMasDerecha.x - verticeOtraAnt.x, vectorMasDerecha.y - verticeOtraAnt.y)<0.0f ||
				   NBPUNTO_PRODUCTO_VECTORIAL(otraSombra->vectorAnguloMin.x - verticeOtraAnt.x, otraSombra->vectorAnguloMin.y - verticeOtraAnt.y, vectorMasIzquierda.x - verticeOtraAnt.x, vectorMasIzquierda.y - verticeOtraAnt.y)<0.0f){
					tieneVerticesFueraDeOtra = true;
				}
				if(!tieneVerticesFueraDeOtra){
					cubiertaPorOtraSombra		= true; //PRINTF_INFO("Nueva sombra(dos vertices) es totalmente cubierta por otra existente(%d vertices)\n", otraSombra->conteoVerticesAdicionalSombra);
					break;
					//Nueva sombra esta cubierta. No es necesario continuar validando.
					//En teroria, todas las sombras cubiertas por esta deberian ya estar cubiertas/apagadas por la sombra que cubre a esta nueva.
				}
			}
			//Verificar si la OTRA sombra es totalmente cubierta por ESTA
			if(prodCruzMinVsOtraMin>=0.0f && prodCruzMinVsOtraMax>=0.0f && prodCruzMaxVsOtraMin<=0.0f && prodCruzMaxVsOtraMax<=0.0f){
				bool otraTieneVerticeFuera		= false;
				NBPunto vectorSeg;				NBPUNTO_ESTABLECER(vectorSeg, vectorMasDerecha.x - vectorMasIzquierda.x, vectorMasDerecha.y - vectorMasIzquierda.y)
				if(NBPUNTO_PRODUCTO_VECTORIAL(vectorSeg.x, vectorSeg.y, otraSombra->vectorAnguloMin.x - vectorMasIzquierda.x, otraSombra->vectorAnguloMin.y - vectorMasIzquierda.y)<0.0f ||
				   NBPUNTO_PRODUCTO_VECTORIAL(vectorSeg.x, vectorSeg.y, otraSombra->vectorAnguloMax.x - vectorMasIzquierda.x, otraSombra->vectorAnguloMax.y - vectorMasIzquierda.y)<0.0f){
					otraTieneVerticeFuera	= true;
				} else if(otraSombra->conteoVerticesAdicionalSombra!=0){
					const NBPunto* verticeOtra			= &bibVerticesIntermediosSombras[otraSombra->iPrimerVerticeAdicionalSombra];
					const NBPunto* verticesOtraSigUlt	= verticeOtra + otraSombra->conteoVerticesAdicionalSombra;
					while(verticeOtra<verticesOtraSigUlt){
						if(NBPUNTO_PRODUCTO_VECTORIAL(vectorSeg.x, vectorSeg.y, verticeOtra->x - vectorMasIzquierda.x, verticeOtra->y - vectorMasIzquierda.y)<0.0f){
							otraTieneVerticeFuera	= true;
							break;
						}
						verticeOtra++;
					}
				}
				if(!otraTieneVerticeFuera){
					otraSombra->sombraCubiertaPorOtra = true; //PRINTF_INFO("Otra sombra(%d vertices) es totalmente cubierta por nueva(dos vertices)\n", otraSombra->conteoVerticesAdicionalSombra);
					#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
					conteoOtrasSombrasCubiertas++;
					#endif
				}
			}
		}
	}
	NBASSERT(!(cubiertaPorOtraSombra && conteoOtrasSombrasCubiertas!=0)) //Si falla, hay sombras cubiertas por esta que no fueron marcadas por la sombra que cubre a esta
																		//(se supone que la sombra que cubre a esta tambien cubre al subconjunto)
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return cubiertaPorOtraSombra;
}

bool NBSegmentadorFiguras::sombraEsCubiertaPorOtraYCubrirOtrasMultiVertices(NBFuenteSombra* arrOtrasSombras, const UI16 conteoOtrasSombras, const NBPunto* bibVerticesIntermediosSombras, const NBPunto &vectorMasIzquierda, const NBPunto &vectorMasDerecha, const NBPunto* verticesNuevaSombra, const UI16 conteoVerticesNuevaSombra){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("NBSegmentadorFiguras::sombraEsCubiertaPorOtraYCubrirOtrasDosVertices")
	NBASSERT(NBPUNTO_PRODUCTO_VECTORIAL(vectorMasDerecha.x, vectorMasDerecha.y, vectorMasIzquierda.x, vectorMasIzquierda.y)>0.0f) //No se admiten lineas (==0.0f) ni orden invertido (<0.0f)
	NBASSERT(conteoVerticesNuevaSombra!=0)
	bool cubiertaPorOtraSombra					= false;
	#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
	UI16 conteoOtrasSombrasCubiertas			= 0;
	#endif
	const NBPunto* verticeSigUlt				= verticesNuevaSombra + conteoVerticesNuevaSombra;
	UI16 iOtraSombra;
	for(iOtraSombra=0; iOtraSombra<conteoOtrasSombras; iOtraSombra++){
		NBFuenteSombra* otraSombra				= &arrOtrasSombras[iOtraSombra];
		if(!otraSombra->sombraCubiertaPorOtra){
			const NBPunto* arrVerticesOtra		= &bibVerticesIntermediosSombras[otraSombra->iPrimerVerticeAdicionalSombra];
			const NBPunto* verticesOtraSigUlt	= arrVerticesOtra + otraSombra->conteoVerticesAdicionalSombra;
			const float prodCruzMinVsOtraMin	= NBPUNTO_PRODUCTO_VECTORIAL(vectorMasDerecha.x, vectorMasDerecha.y, otraSombra->vectorAnguloMin.x, otraSombra->vectorAnguloMin.y);
			const float prodCruzMaxVsOtraMin	= NBPUNTO_PRODUCTO_VECTORIAL(vectorMasIzquierda.x, vectorMasIzquierda.y, otraSombra->vectorAnguloMin.x, otraSombra->vectorAnguloMin.y);
			const float prodCruzMaxVsOtraMax	= NBPUNTO_PRODUCTO_VECTORIAL(vectorMasIzquierda.x, vectorMasIzquierda.y, otraSombra->vectorAnguloMax.x, otraSombra->vectorAnguloMax.y);
			const float prodCruzMinVsOtraMax	= NBPUNTO_PRODUCTO_VECTORIAL(vectorMasDerecha.x, vectorMasDerecha.y, otraSombra->vectorAnguloMax.x, otraSombra->vectorAnguloMax.y);
			//Verificar si ESTA sombra es totalmente cubierta por la OTRA
			if(!cubiertaPorOtraSombra && prodCruzMinVsOtraMin<=0.0f && prodCruzMinVsOtraMax>=0.0f && prodCruzMaxVsOtraMin<=0.0f && prodCruzMaxVsOtraMax>=0.0f){
				bool tieneVerticesFueraDeOtra	= false;
				NBPunto verticeOtraAnt			= otraSombra->vectorAnguloMax;
				const NBPunto* verticeOtra		= arrVerticesOtra;
				while(verticeOtra<verticesOtraSigUlt){
					NBPunto vectorSeg;			NBPUNTO_ESTABLECER(vectorSeg, verticeOtra->x - verticeOtraAnt.x, verticeOtra->y - verticeOtraAnt.y)
					if(NBPUNTO_PRODUCTO_VECTORIAL(vectorSeg.x, vectorSeg.y, vectorMasIzquierda.x - verticeOtraAnt.x, vectorMasIzquierda.y - verticeOtraAnt.y)<0.0f ||
					   NBPUNTO_PRODUCTO_VECTORIAL(vectorSeg.x, vectorSeg.y, vectorMasDerecha.x - verticeOtraAnt.x, vectorMasDerecha.y - verticeOtraAnt.y)<0.0f){
						tieneVerticesFueraDeOtra = true;
						break;
					} else {
						const NBPunto* vertice	= verticesNuevaSombra;
						while(vertice<verticeSigUlt){
							if(NBPUNTO_PRODUCTO_VECTORIAL(vectorSeg.x, vectorSeg.y, vertice->x - verticeOtraAnt.x, vertice->y - verticeOtraAnt.y)<0.0f){
								tieneVerticesFueraDeOtra = true;
								break;
							}
							vertice++;
						}
						if(tieneVerticesFueraDeOtra) break;
					}
					verticeOtraAnt = *verticeOtra;
					verticeOtra++;
				}
				if(!tieneVerticesFueraDeOtra){
					NBPunto vectorSeg;			NBPUNTO_ESTABLECER(vectorSeg, otraSombra->vectorAnguloMin.x - verticeOtraAnt.x, otraSombra->vectorAnguloMin.y - verticeOtraAnt.y)
					if(NBPUNTO_PRODUCTO_VECTORIAL(vectorSeg.x, vectorSeg.y, vectorMasIzquierda.x - verticeOtraAnt.x, vectorMasIzquierda.y - verticeOtraAnt.y)<0.0f ||
					   NBPUNTO_PRODUCTO_VECTORIAL(vectorSeg.x, vectorSeg.y, vectorMasDerecha.x - verticeOtraAnt.x, vectorMasDerecha.y - verticeOtraAnt.y)<0.0f){
						tieneVerticesFueraDeOtra = true;
						break;
					} else {
						const NBPunto* vertice	= verticesNuevaSombra;
						while(vertice<verticeSigUlt){
							if(NBPUNTO_PRODUCTO_VECTORIAL(vectorSeg.x, vectorSeg.y, vertice->x - verticeOtraAnt.x, vertice->y - verticeOtraAnt.y)<0.0f){
								tieneVerticesFueraDeOtra = true;
								break;
							}
							vertice++;
						}
						if(tieneVerticesFueraDeOtra) break;
					}
				}
				if(!tieneVerticesFueraDeOtra){
					cubiertaPorOtraSombra		= true; //PRINTF_INFO("Nueva sombra(%d vertices) es totalmente cubierta por otra existente(%d vertices)\n", conteoVerticesNuevaSombra, otraSombra->conteoVerticesAdicionalSombra);
					break;
					//Nueva sombra esta cubierta. No es necesario continuar validando.
					//En teroria, todas las sombras cubiertas por esta deberian ya estar cubiertas/apagadas por la sombra que cubre a esta nueva.
				}
			}
			//Verificar si la OTRA sombra es totalmente cubierta por ESTA
			if(prodCruzMinVsOtraMin>=0.0f && prodCruzMinVsOtraMax>=0.0f && prodCruzMaxVsOtraMin<=0.0f && prodCruzMaxVsOtraMax<=0.0f){
				bool otraTieneVerticeFuera		= false;
				NBPunto verticeAnt				= vectorMasIzquierda;
				const NBPunto* vertice			= verticesNuevaSombra;
				while(vertice<verticeSigUlt){
					NBPunto vectorSeg;			NBPUNTO_ESTABLECER(vectorSeg, vertice->x - verticeAnt.x, vertice->y - verticeAnt.y)
					if(NBPUNTO_PRODUCTO_VECTORIAL(vectorSeg.x, vectorSeg.y, otraSombra->vectorAnguloMin.x - verticeAnt.x, otraSombra->vectorAnguloMin.y - verticeAnt.y)<0.0f ||
					   NBPUNTO_PRODUCTO_VECTORIAL(vectorSeg.x, vectorSeg.y, otraSombra->vectorAnguloMax.x - verticeAnt.x, otraSombra->vectorAnguloMax.y - verticeAnt.y)<0.0f){
						otraTieneVerticeFuera	= true;
						break;
					} else {
						const NBPunto* verticeOtra	= arrVerticesOtra;
						while(verticeOtra<verticesOtraSigUlt){
							if(NBPUNTO_PRODUCTO_VECTORIAL(vectorSeg.x, vectorSeg.y, verticeOtra->x - verticeAnt.x, verticeOtra->y - verticeAnt.y)<0.0f){
								otraTieneVerticeFuera	= true;
								break;
							}
							verticeOtra++;
						}
						if(otraTieneVerticeFuera) break;
					}
					verticeAnt = *vertice;
					vertice++;
				}
				if(!otraTieneVerticeFuera){
					NBPunto vectorSeg;			NBPUNTO_ESTABLECER(vectorSeg, vectorMasDerecha.x - verticeAnt.x, vectorMasDerecha.y - verticeAnt.y)
					if(NBPUNTO_PRODUCTO_VECTORIAL(vectorSeg.x, vectorSeg.y, otraSombra->vectorAnguloMin.x - verticeAnt.x, otraSombra->vectorAnguloMin.y - verticeAnt.y)<0.0f ||
					   NBPUNTO_PRODUCTO_VECTORIAL(vectorSeg.x, vectorSeg.y, otraSombra->vectorAnguloMax.x - verticeAnt.x, otraSombra->vectorAnguloMax.y - verticeAnt.y)<0.0f){
						otraTieneVerticeFuera	= true;
						break;
					} else {
						const NBPunto* verticeOtra	= arrVerticesOtra;
						while(verticeOtra<verticesOtraSigUlt){
							if(NBPUNTO_PRODUCTO_VECTORIAL(vectorSeg.x, vectorSeg.y, verticeOtra->x - verticeAnt.x, verticeOtra->y - verticeAnt.y)<0.0f){
								otraTieneVerticeFuera	= true;
								break;
							}
							verticeOtra++;
						}
						if(otraTieneVerticeFuera) break;
					}
				}
				if(!otraTieneVerticeFuera){
					otraSombra->sombraCubiertaPorOtra = true; //PRINTF_INFO("Otra sombra(%d vertices) es totalmente cubierta por nueva(%d vertices)\n", otraSombra->conteoVerticesAdicionalSombra, conteoVerticesNuevaSombra);
					#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
					conteoOtrasSombrasCubiertas++;
					#endif
				}
			}
		}
	}
	NBASSERT(!(cubiertaPorOtraSombra && conteoOtrasSombrasCubiertas!=0)) //Si falla, hay sombras cubiertas por esta que no fueron marcadas por la sombra que cubre a esta
	//(se supone que la sombra que cubre a esta tambien cubre al subconjunto)
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return cubiertaPorOtraSombra;
}

void NBSegmentadorFiguras::producirSombraDosVertices(NBFuenteIluminacionRender* arrLuces, const UI16 conteoLues, AUArregloNativoMutableP<NBFuenteSombra>** arregloSombrasPorLuces, AUArregloNativoMutableP<NBPunto>* bibVerticesIntermediosSombras, const NBPunto verticeEscenaIzq, const NBPunto &verticeEscenaDer, AUEscenaObjeto* objOriginaSombra, AUTextura* texturaSombra){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("NBSegmentadorFiguras::producirSombraDosVertices")
	//PRINTF_INFO("Sombra de dos vertices\n");
	AUObjeto* agrupadorSombras	= objOriginaSombra->agrupadorSombras();
	//Definir la caja de sombra
	NBCajaAABB cajaEscenaSombra;
	NBCAJAAABB_INICIALIZAR_CON_PUNTO(cajaEscenaSombra, verticeEscenaIzq.x, verticeEscenaIzq.y)
	NBCAJAAABB_ENVOLVER_PUNTO_SIGUIENTE(cajaEscenaSombra, verticeEscenaDer.x, verticeEscenaDer.y)
	//Recorrer figuras de sombras
	UI16 iIlum; NBPunto vectorActual[2];
	for(iIlum=0; iIlum<conteoLues; iIlum++){
		NBFuenteIluminacionRender* fuenteIlum					= &arrLuces[iIlum];
		AUArregloNativoMutableP<NBFuenteSombra>* arregloSombras	= arregloSombrasPorLuces[iIlum];
		//Validar que produce sombras, no esta obstruida, el agrupador de sombras no concuerda y las cajas luz y sombra se intersectan
		if(!fuenteIlum->produceSombras || fuenteIlum->obstruida || fuenteIlum->objetoAgrupaSombras==agrupadorSombras || NBCAJAAABB_NO_INTERSECTA_CAJA(fuenteIlum->cajaEscenaLuz, cajaEscenaSombra)) continue;
		//Validar si la sombra obstruye/tapa a la luz (la luz esta dentro de la figura)
		//(solo validar si el centro de la luz es colineal con la linea sombra, es necesario?)
		//
		//Validar que todos los vertices de la sombra esten a la derecha del vector limite de la luz
		if(fuenteIlum->limitesFormaActivos){
			NBPUNTO_ESTABLECER(vectorActual[0], verticeEscenaIzq.x - fuenteIlum->posicionEscenaLuz.x - fuenteIlum->posIniVecLimiteForma.x, verticeEscenaIzq.y - fuenteIlum->posicionEscenaLuz.y - fuenteIlum->posIniVecLimiteForma.y)
			NBPUNTO_ESTABLECER(vectorActual[1], verticeEscenaDer.x - fuenteIlum->posicionEscenaLuz.x - fuenteIlum->posIniVecLimiteForma.x, verticeEscenaDer.y - fuenteIlum->posicionEscenaLuz.y - fuenteIlum->posIniVecLimiteForma.y)
			if(NBPUNTO_PRODUCTO_VECTORIAL(fuenteIlum->vecUnitLimiteForma.x, fuenteIlum->vecUnitLimiteForma.y, vectorActual[0].x, vectorActual[0].y)>=0.0f || NBPUNTO_PRODUCTO_VECTORIAL(fuenteIlum->vecUnitLimiteForma.x, fuenteIlum->vecUnitLimiteForma.y, vectorActual[1].x, vectorActual[1].y)>=0.0f){
				continue;
			}
		}
		//
		NBPunto vectorMasIzquierda, vectorMasDerecha;
		NBPUNTO_ESTABLECER(vectorActual[0], verticeEscenaIzq.x - fuenteIlum->posicionEscenaLuz.x, verticeEscenaIzq.y - fuenteIlum->posicionEscenaLuz.y)
		NBPUNTO_ESTABLECER(vectorActual[1], verticeEscenaDer.x - fuenteIlum->posicionEscenaLuz.x, verticeEscenaDer.y - fuenteIlum->posicionEscenaLuz.y)
		float prodCruzVectores = NBPUNTO_PRODUCTO_VECTORIAL(vectorActual[0].x, vectorActual[0].y, vectorActual[1].x, vectorActual[1].y);
		if(prodCruzVectores>0.0f){
			vectorMasIzquierda	= vectorActual[1];
			vectorMasDerecha	= vectorActual[0];
			NBASSERT(NBPUNTO_PRODUCTO_VECTORIAL(vectorMasDerecha.x, vectorMasDerecha.y, vectorMasIzquierda.x, vectorMasIzquierda.y) > 0.0f)
		} else if(prodCruzVectores<0.0f){
			vectorMasIzquierda	= vectorActual[0];
			vectorMasDerecha	= vectorActual[1];
			NBASSERT(NBPUNTO_PRODUCTO_VECTORIAL(vectorMasDerecha.x, vectorMasDerecha.y, vectorMasIzquierda.x, vectorMasIzquierda.y) > 0.0f)
		} else if(prodCruzVectores==0.0f){
			continue; //Omitir sombras que producen solo una linea
		}
		//Agregar los vertices de la sombra proyectada ante la luz
		NBFuenteSombra proyeccionSombra;
		proyeccionSombra.objOriginaSombra		= objOriginaSombra;
		proyeccionSombra.objAgrupaSombras		= objOriginaSombra->agrupadorSombras();
		proyeccionSombra.vectorAnguloMin		= vectorMasDerecha;
		proyeccionSombra.vectorAnguloMax		= vectorMasIzquierda;
		proyeccionSombra.iPrimerVerticeAdicionalSombra	= 0;
		proyeccionSombra.conteoVerticesAdicionalSombra	= 0;
		proyeccionSombra.cajaEscenaSombra		= cajaEscenaSombra;
		proyeccionSombra.sombraCubiertaPorOtra	= NBSegmentadorFiguras::sombraEsCubiertaPorOtraYCubrirOtrasDosVertices(arregloSombras->elemento, arregloSombras->conteo, bibVerticesIntermediosSombras->elemento, proyeccionSombra.vectorAnguloMax, proyeccionSombra.vectorAnguloMin);
		NBASSERT(NBSegmentadorFiguras::debugValidarSombra(&bibVerticesIntermediosSombras->elemento[proyeccionSombra.iPrimerVerticeAdicionalSombra], proyeccionSombra.conteoVerticesAdicionalSombra, proyeccionSombra.vectorAnguloMin, proyeccionSombra.vectorAnguloMax));
		arregloSombras->agregarElemento(proyeccionSombra);
		#ifdef CONFIG_NB_RECOPILAR_ESTADISTICAS_DE_ESCENA_CUERPOS
		//*propsIluminacion.debugPrtConteoTotalFigurasSombras		+= 1;
		//*propsIluminacion.debugPrtConteoVerticesSombras			+= conteoVerticesSombra;
		#endif
	}
	#ifdef CONFIG_NB_RECOPILAR_ESTADISTICAS_DE_ESCENA_CUERPOS
	/**propsIluminacion.debugPrtConteoCuerpos			+= 1;
	 *propsIluminacion.debugPtrConteoTotalFigurasFisica	+= _fijaciones->conteo;
	 SI32 iFijacion;
	 for(iFijacion=0; iFijacion<_fijaciones->conteo; iFijacion++){
	 *propsIluminacion.debugPrtConteoVerticesFisica	+= _fijaciones->elemento[iFijacion].vertices->conteo;
	 }*/
	#endif
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void NBSegmentadorFiguras::producirSombraMultiVertices(NBFuenteIluminacionRender* arrLuces, const UI16 conteoLues, AUArregloNativoMutableP<NBFuenteSombra>** arregloSombrasPorLuces, AUArregloNativoMutableP<NBPunto>* bibVerticesIntermediosSombras, const NBPunto* verticesSombra, const UI16 conteoVerticesSombra, AUEscenaObjeto* objOriginaSombra, const bool verticesSombraSonContrareloj, AUTextura* texturaSombra){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("NBSegmentadorFiguras::producirSombraMultiVertices")
	NBASSERT(conteoVerticesSombra>1)
	NBASSERT(conteoVerticesSombra>2) //PRINTF_INFO("Sombra de mas-de-dos vertices\n");
	AUObjeto* agrupadorSombras	= objOriginaSombra->agrupadorSombras();
	//Definir la caja de sombra
	NBCajaAABB cajaEscenaSombra; NBCAJAAABB_INICIALIZAR_CON_PUNTO(cajaEscenaSombra, verticesSombra[0].x, verticesSombra[0].y)
	SI16 iPunto; for(iPunto=1; iPunto<conteoVerticesSombra; iPunto++){ NBCAJAAABB_ENVOLVER_PUNTO_SIGUIENTE(cajaEscenaSombra, verticesSombra[iPunto].x, verticesSombra[iPunto].y) }
	if(cajaEscenaSombra.xMin==cajaEscenaSombra.xMax){ cajaEscenaSombra.xMin -= 4.0f; cajaEscenaSombra.xMax += 4.0f; }
	if(cajaEscenaSombra.yMin==cajaEscenaSombra.yMax){ cajaEscenaSombra.yMin -= 4.0f; cajaEscenaSombra.yMax += 4.0f; }
	//Determinar el orden en que seran recorridos los vertices de la figura sombra
	const SI16 indicePrimerVertice			= verticesSombraSonContrareloj ? 0 : conteoVerticesSombra-1;
	const SI16 indiceSiguienteUltimoVertice	= verticesSombraSonContrareloj ? conteoVerticesSombra : -1;
	const SI16 avanceVertices				= verticesSombraSonContrareloj ? 1 : -1;
	//Recorrer figuras de sombras
	UI16 iIlum;
	for(iIlum=0; iIlum<conteoLues; iIlum++){
		NBFuenteIluminacionRender* fuenteIlum					= &arrLuces[iIlum];
		AUArregloNativoMutableP<NBFuenteSombra>* arregloSombras	= arregloSombrasPorLuces[iIlum];
		//Validar que produce sombras, no esta obstruida, el agrupador de sombras no concuerda y las cajas luz y sombra se intersectan
		if(!fuenteIlum->produceSombras || fuenteIlum->obstruida || fuenteIlum->objetoAgrupaSombras==agrupadorSombras || NBCAJAAABB_NO_INTERSECTA_CAJA(fuenteIlum->cajaEscenaLuz, cajaEscenaSombra)) continue;
		//Validar si la sombra obstruye/tapa a la luz (la luz esta dentro de la figura)
		if(conteoVerticesSombra>2){
			bool luzEstaDentroDeFigura			= true;
			UI16 iPuntoAnterior					= indicePrimerVertice;
			for(iPunto=indicePrimerVertice+avanceVertices; iPunto!=indiceSiguienteUltimoVertice; iPunto+=avanceVertices){
				if(!NBPUNTO_ESTA_IZQUIERDA_VECTOR(verticesSombra[iPuntoAnterior].x, verticesSombra[iPuntoAnterior].y, verticesSombra[iPunto].x, verticesSombra[iPunto].y, fuenteIlum->posicionEscenaLuz.x, fuenteIlum->posicionEscenaLuz.y)){
					luzEstaDentroDeFigura		= false; break;
				}
				iPuntoAnterior					= iPunto;
			}
			if(luzEstaDentroDeFigura && NBPUNTO_ESTA_IZQUIERDA_VECTOR(verticesSombra[iPuntoAnterior].x, verticesSombra[iPuntoAnterior].y, verticesSombra[indicePrimerVertice].x, verticesSombra[indicePrimerVertice].y, fuenteIlum->posicionEscenaLuz.x, fuenteIlum->posicionEscenaLuz.y)){
				//fuenteIlum->obstruida = true; //PENDIENTE: obstruir???
				continue;
			}
		}
		//Validar que todos los vertices de la sombra esten a la derecha del vector limite de la luz
		if(fuenteIlum->limitesFormaActivos){
			bool algunVerticeIzqLimite = false;
			for(iPunto=indicePrimerVertice+avanceVertices; iPunto!=indiceSiguienteUltimoVertice; iPunto+=avanceVertices){
				NBPunto vectorActual; NBPUNTO_ESTABLECER(vectorActual, verticesSombra[iPunto].x - fuenteIlum->posicionEscenaLuz.x - fuenteIlum->posIniVecLimiteForma.x, verticesSombra[iPunto].y - fuenteIlum->posicionEscenaLuz.y - fuenteIlum->posIniVecLimiteForma.y)
				if(NBPUNTO_PRODUCTO_VECTORIAL(fuenteIlum->vecUnitLimiteForma.x, fuenteIlum->vecUnitLimiteForma.y, vectorActual.x, vectorActual.y)>=0.0f){
					algunVerticeIzqLimite = true;
					break;
				}
			}
			if(algunVerticeIzqLimite) continue;
		}
		//
		SI16 iVerticeMasIzquierda, iVerticeMasDerecha;
		NBPunto vectorMasIzquierda, vectorMasDerecha;
		//Determinar el primero y ultimo vertice que forma la sombra antihoraria
		iVerticeMasIzquierda	= indicePrimerVertice;
		iVerticeMasDerecha		= indicePrimerVertice;
		NBPUNTO_ESTABLECER(vectorMasIzquierda, verticesSombra[indicePrimerVertice].x - fuenteIlum->posicionEscenaLuz.x, verticesSombra[indicePrimerVertice].y - fuenteIlum->posicionEscenaLuz.y)
		vectorMasDerecha		= vectorMasIzquierda;
		for(iPunto=indicePrimerVertice+avanceVertices; iPunto!=indiceSiguienteUltimoVertice; iPunto+=avanceVertices){
			NBPunto vectorActual;	NBPUNTO_ESTABLECER(vectorActual, verticesSombra[iPunto].x - fuenteIlum->posicionEscenaLuz.x, verticesSombra[iPunto].y - fuenteIlum->posicionEscenaLuz.y)
			float prodVectorConIzq	= NBPUNTO_PRODUCTO_VECTORIAL(vectorMasIzquierda.x, vectorMasIzquierda.y, vectorActual.x, vectorActual.y);
			float prodVectorConDer	= NBPUNTO_PRODUCTO_VECTORIAL(vectorMasDerecha.x, vectorMasDerecha.y, vectorActual.x, vectorActual.y);
			if(prodVectorConIzq>0.0f){
				iVerticeMasIzquierda	= iPunto;
				vectorMasIzquierda		= vectorActual;
			}
			if(prodVectorConDer<0.0f){
				iVerticeMasDerecha		= iPunto;
				vectorMasDerecha		= vectorActual;
			}
		}
		//Omitir sombras que producen solo una linea
		if(NBPUNTO_PRODUCTO_VECTORIAL(vectorMasDerecha.x, vectorMasDerecha.y, vectorMasIzquierda.x, vectorMasIzquierda.y)<=0.0f) continue;
		//Determinar y agregar los vertices de la sombra proyectada ante la luz
		NBFuenteSombra proyeccionSombra;
		proyeccionSombra.objOriginaSombra		= objOriginaSombra;
		proyeccionSombra.objAgrupaSombras		= objOriginaSombra->agrupadorSombras();
		proyeccionSombra.vectorAnguloMin		= vectorMasDerecha;
		proyeccionSombra.vectorAnguloMax		= vectorMasIzquierda;
		proyeccionSombra.iPrimerVerticeAdicionalSombra	= bibVerticesIntermediosSombras->conteo;
		proyeccionSombra.cajaEscenaSombra		= cajaEscenaSombra;
		//Agregar los vertices de la figura
		iPunto									= iVerticeMasIzquierda + avanceVertices; if(iPunto<0) iPunto = conteoVerticesSombra - 1; if(iPunto==conteoVerticesSombra) iPunto = 0;
		while(iPunto!=iVerticeMasDerecha){
			NBPunto vLocal; NBPUNTO_ESTABLECER(vLocal, verticesSombra[iPunto].x - fuenteIlum->posicionEscenaLuz.x, verticesSombra[iPunto].y - fuenteIlum->posicionEscenaLuz.y)
			bibVerticesIntermediosSombras->agregarElemento(vLocal);
			//Siguiente
			iPunto += avanceVertices; if(iPunto<0) iPunto = conteoVerticesSombra - 1; if(iPunto==conteoVerticesSombra) iPunto = 0;
		}
		//
		proyeccionSombra.conteoVerticesAdicionalSombra	= bibVerticesIntermediosSombras->conteo - proyeccionSombra.iPrimerVerticeAdicionalSombra;
		if(proyeccionSombra.conteoVerticesAdicionalSombra==0){
			proyeccionSombra.sombraCubiertaPorOtra	= NBSegmentadorFiguras::sombraEsCubiertaPorOtraYCubrirOtrasDosVertices(arregloSombras->elemento, arregloSombras->conteo, bibVerticesIntermediosSombras->elemento, proyeccionSombra.vectorAnguloMax, proyeccionSombra.vectorAnguloMin);
		} else {
			proyeccionSombra.sombraCubiertaPorOtra	= NBSegmentadorFiguras::sombraEsCubiertaPorOtraYCubrirOtrasMultiVertices(arregloSombras->elemento, arregloSombras->conteo, bibVerticesIntermediosSombras->elemento, proyeccionSombra.vectorAnguloMax, proyeccionSombra.vectorAnguloMin, &bibVerticesIntermediosSombras->elemento[proyeccionSombra.iPrimerVerticeAdicionalSombra], proyeccionSombra.conteoVerticesAdicionalSombra);
		}
		NBASSERT(NBSegmentadorFiguras::debugValidarSombra(&bibVerticesIntermediosSombras->elemento[proyeccionSombra.iPrimerVerticeAdicionalSombra], proyeccionSombra.conteoVerticesAdicionalSombra, proyeccionSombra.vectorAnguloMin, proyeccionSombra.vectorAnguloMax));
		arregloSombras->agregarElemento(proyeccionSombra);
		#ifdef CONFIG_NB_RECOPILAR_ESTADISTICAS_DE_ESCENA_CUERPOS
		//*propsIluminacion.debugPrtConteoTotalFigurasSombras		+= 1;
		//*propsIluminacion.debugPrtConteoVerticesSombras			+= conteoVerticesSombra;
		#endif
	}
	#ifdef CONFIG_NB_RECOPILAR_ESTADISTICAS_DE_ESCENA_CUERPOS
	/**propsIluminacion.debugPrtConteoCuerpos			+= 1;
	 *propsIluminacion.debugPtrConteoTotalFigurasFisica	+= _fijaciones->conteo;
	 SI32 iFijacion;
	 for(iFijacion=0; iFijacion<_fijaciones->conteo; iFijacion++){
	 *propsIluminacion.debugPrtConteoVerticesFisica	+= _fijaciones->elemento[iFijacion].vertices->conteo;
	 }*/
	#endif
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
bool NBSegmentadorFiguras::debugValidarSombra(const NBPunto* verticesEscenaSombra, const SI16 conteoVerticesEscenaSombra, const NBPunto &vectorAngMin, const NBPunto &vectorAngMax){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("NBSegmentadorFiguras::debugValidarSombra")
	bool exito = (NBPUNTO_PRODUCTO_VECTORIAL(vectorAngMin.x, vectorAngMin.y, vectorAngMax.x, vectorAngMax.y)>0.0f);
	NBASSERT(vectorAngMin.x!=0.0f || vectorAngMin.y!=0.0f)
	NBASSERT(vectorAngMax.x!=0.0f || vectorAngMax.y!=0.0f)
	NBASSERT(NBPUNTO_PRODUCTO_VECTORIAL(vectorAngMin.x, vectorAngMin.y, vectorAngMax.x, vectorAngMax.y)>0.0f)
	//
	NBPunto vertIniSeg; NBPUNTO_ESTABLECER(vertIniSeg, vectorAngMax.x+vectorAngMax.x, vectorAngMax.y+vectorAngMax.y)
	NBPunto vertFinSeg	= vectorAngMax;
	SI16 iVert, iVertFin = conteoVerticesEscenaSombra + 1;
	for(iVert=0; iVert<=iVertFin; iVert++){
		NBPunto vertAct;
		if(iVert<conteoVerticesEscenaSombra){
			vertAct	= verticesEscenaSombra[iVert];
		} else if(iVert==conteoVerticesEscenaSombra){
			vertAct = vectorAngMin;
		} else {
			NBPUNTO_ESTABLECER(vertAct, vectorAngMin.x+vectorAngMin.x, vectorAngMin.y+vectorAngMin.y)
		}
		NBPunto vec1;	NBPUNTO_ESTABLECER(vec1, vertFinSeg.x-vertIniSeg.x, vertFinSeg.y-vertIniSeg.y);
		NBPunto vec2;	NBPUNTO_ESTABLECER(vec2, vertAct.x-vertIniSeg.x, vertAct.y-vertIniSeg.y);
		float prodpCruz	= NBPUNTO_PRODUCTO_VECTORIAL(vec1.x, vec1.y, vec2.x, vec2.y);
		if(prodpCruz<=0.0f){
			exito = false; NBASSERT(false);
			break;
		}
		vertIniSeg	= vertFinSeg;
		vertFinSeg	= vertAct;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return exito;
}
#endif

//
//SEGMENTACION DE FIGURA
//

AUArregloNativoMutableP<NBPunto>* NBSegmentadorFiguras::figuraActual(){
	return _figuras[_indiceFiguraActual];
}

void NBSegmentadorFiguras::cargarFiguraActual(const NBPunto* vertices4FiguraConvexaContrareloj){
	NBASSERT(NBSegmentadorFiguras::figuraEsConvexaContrareloj(vertices4FiguraConvexaContrareloj, 4))
	AUArregloNativoMutableP<NBPunto>* figuraActual = _figuras[_indiceFiguraActual];
	figuraActual->vaciar();
	figuraActual->agregarElementos(vertices4FiguraConvexaContrareloj, 4);
	NBASSERT(NBSegmentadorFiguras::figuraNoRepiteVertices(figuraActual->elemento, figuraActual->conteo));
	NBASSERT(NBSegmentadorFiguras::figuraEsConvexaContrareloj(figuraActual->elemento, figuraActual->conteo))
}

void NBSegmentadorFiguras::limitarCajaSegunVectoresAngulos(const NBPunto &vectorAngMin, const NBPunto &vectorAngMax){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("NBSegmentadorFiguras::limitarCajaSegunVectoresAngulos")
	//Se asume que:
	// - La figura actual es la caja completa convexa-contrareloj de la luz (sin limitar).
	AUArregloNativoMutableP<NBPunto>* figuraActual	= _figuras[_indiceFiguraActual];
	AUArregloNativoMutableP<NBPunto>* figuraNueva	= _figuras[_indiceFiguraActual==0?1:0];
	const NBPunto* arrVertFig	= figuraActual->elemento;
	const SI16 conteoVertFig	= figuraActual->conteo;
	NBASSERT(conteoVertFig==4) //Se espera que sea la figura inicial
	NBASSERT(NBSegmentadorFiguras::figuraNoRepiteVertices(figuraActual->elemento, figuraActual->conteo));
	NBASSERT(NBSegmentadorFiguras::figuraEsConvexaContrareloj(figuraActual->elemento, figuraActual->conteo));
	#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
	float debugModulo;
	debugModulo = NBPUNTO_DISTANCIA_VECTOR(vectorAngMin.x, vectorAngMin.y); NBASSERT(debugModulo>0.9999f && debugModulo<1.0001f)
	debugModulo = NBPUNTO_DISTANCIA_VECTOR(vectorAngMax.x, vectorAngMax.y); NBASSERT(debugModulo>0.9999f && debugModulo<1.0001f)
	#endif
	//Determinar las intersecciones con los vectores angulos
	SI16 iVertInterseccion[2]		= {-1, -1};
	float avanceSegInterseccion[2]	= {-1.0f, -1.0f};
	NBPunto posInterseccion[2];
	float relAvanceVec[2][4], relAvanceSeg[2][4], relDenom[2][4]; NBASSERT(conteoVertFig==4) //Se espera que sea la figura inicial
	UI16 iVertAnt				= conteoVertFig - 1;
	NBPunto posVertAnt			= arrVertFig[iVertAnt];
	UI16 iVert;
	for(iVert=0; iVert<conteoVertFig; iVert++){
		NBPunto posVertAct		= arrVertFig[iVert];
		if(iVertInterseccion[0]==-1){
			NBLINEA_INSTERSECCION_RELATIVOS(relAvanceVec[0][iVert], relAvanceSeg[0][iVert], relDenom[0][iVert], 0.0f, 0.0f, vectorAngMin.x, vectorAngMin.y, posVertAnt.x, posVertAnt.y, posVertAct.x, posVertAct.y)
			if(relDenom[0][iVert]!=0.0f && relAvanceVec[0][iVert]>0.0f && relAvanceSeg[0][iVert]>=0.0f && (relAvanceSeg[0][iVert]<=1.0f || (relAvanceSeg[0][iVert]<1.001f && avanceSegInterseccion[0]<0.0f))){
				if(relAvanceSeg[0][iVert]<=1.0f || avanceSegInterseccion[0]<0.0f){ //Solo se adoptan valores "cercanos a 1.0f" si aun no hay una valor menor-o-igual a uno
					avanceSegInterseccion[0]				= relAvanceSeg[0][iVert];
					iVertInterseccion[0]					= iVertAnt;
					if(relAvanceSeg[0][iVert]<0.001f)		posInterseccion[0] = posVertAnt;
					else if(relAvanceSeg[0][iVert]>0.999f)	posInterseccion[0] = posVertAct;
					else {
						posInterseccion[0].x		= posVertAnt.x + ((posVertAct.x - posVertAnt.x) * relAvanceSeg[0][iVert]);
						posInterseccion[0].y		= posVertAnt.y + ((posVertAct.y - posVertAnt.y) * relAvanceSeg[0][iVert]);
					}
				}
			}
		}
		if(iVertInterseccion[1]==-1){
			NBLINEA_INSTERSECCION_RELATIVOS(relAvanceVec[1][iVert], relAvanceSeg[1][iVert], relDenom[1][iVert], 0.0f, 0.0f, vectorAngMax.x, vectorAngMax.y, posVertAnt.x, posVertAnt.y, posVertAct.x, posVertAct.y)
			if(relDenom[1][iVert]!=0.0f && relAvanceVec[1][iVert]>0.0f && relAvanceSeg[1][iVert]>=0.0f && (relAvanceSeg[1][iVert]<=1.0f || (relAvanceSeg[1][iVert]<1.001f && avanceSegInterseccion[1]<0.0f))){ //Solo se adoptan valores "cercanos a 1.0f" si aun no hay una valor menor-o-igual a uno
				avanceSegInterseccion[1]		= relAvanceSeg[1][iVert];
				iVertInterseccion[1]			= iVertAnt;
				if(relAvanceSeg[1][iVert]<0.001f)		posInterseccion[1] = posVertAnt;
				else if(relAvanceSeg[1][iVert]>0.999f)	posInterseccion[1] = posVertAct;
				else {
					posInterseccion[1].x		= posVertAnt.x + ((posVertAct.x - posVertAnt.x) * relAvanceSeg[1][iVert]);
					posInterseccion[1].y		= posVertAnt.y + ((posVertAct.y - posVertAnt.y) * relAvanceSeg[1][iVert]);
				}
			}
		}
		//Siguiente
		posVertAnt	= posVertAct;
		iVertAnt	= iVert;
	}
	NBASSERT(iVertInterseccion[0]!=-1 && iVertInterseccion[1]!=-1)
	//NBASSERT(avanceSegInterseccion[0]>=0.0f && avanceSegInterseccion[0]<=1.0f) //Esto no es un error a validar, solo lo uso para detectar cuando fue necesario utilizar aproximaciones
	//NBASSERT(avanceSegInterseccion[1]>=0.0f && avanceSegInterseccion[1]<=1.0f) //Esto no es un error a validar, solo lo uso para detectar cuando fue necesario utilizar aproximaciones
	//Generar nueva figura resultante
	if(iVertInterseccion[0]!=-1 && iVertInterseccion[1]!=-1){
		if(posInterseccion[0]==posInterseccion[1]){
			//Caso, en el que el angulo es muy pequeno (conviertiendo a la luz en una linea)
			figuraActual->vaciar();
		} else {
			NBASSERT(posInterseccion[0]!=posInterseccion[1])
			NBPunto posCero; NBPUNTO_ESTABLECER(posCero, 0.0f, 0.0f)
			//Agregar el vertice centro
			figuraNueva->vaciar();
			figuraNueva->agregarElemento(posCero);
			//Agregar la interseccion con el AngMin
			NBASSERT(posInterseccion[0]!=posCero) //Nunca deberia concordar con la posicion de la luz
			figuraNueva->agregarElemento(posInterseccion[0]);
			//Agregar todos los vertices entre las intersecciones con AngMin y AngMax
			iVert = iVertInterseccion[0];
			while(iVert!=iVertInterseccion[1]) {
				iVert++; if(iVert>=conteoVertFig) iVert = 0;
				NBASSERT(arrVertFig[iVert]!=posCero) //Nunca deberia concordar con la posicion de la luz
				if(figuraNueva->elemento[figuraNueva->conteo-1]!=arrVertFig[iVert]) figuraNueva->agregarElemento(arrVertFig[iVert]);
			}
			//Agregar la interseccion con el AngMax
			NBASSERT(posInterseccion[1]!=posCero) //Nunca deberia concordar con la posicion de la luz
			if(figuraNueva->elemento[figuraNueva->conteo-1]!=posInterseccion[1]) figuraNueva->agregarElemento(posInterseccion[1]);
			//
			NBASSERT(figuraNueva->conteo>2)
			NBASSERT(NBSegmentadorFiguras::figuraNoRepiteVertices(figuraNueva->elemento, figuraNueva->conteo));
			NBASSERT(NBSegmentadorFiguras::figuraEsConvexaContrareloj(figuraNueva->elemento, figuraNueva->conteo));
			_indiceFiguraActual = _indiceFiguraActual==0?1:0;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void NBSegmentadorFiguras::extraerProcionesRangosSombrasAFigura(const STRangoSombra* arregloSombras, const UI16 conteoSombras){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("NBSegmentadorFiguras::extraerProcionesRangosSombrasAFigura")
	AUArregloNativoMutableP<NBPunto>* figuraInicial	= _figuras[_indiceFiguraActual];
	NBASSERT(NBSegmentadorFiguras::figuraNoRepiteVertices(figuraInicial->elemento, figuraInicial->conteo));
	PRINTF_INFO("%d sombras reflejadas en espejo\n", conteoSombras);
	//
	NBPunto posCero;				NBPUNTO_ESTABLECER(posCero, 0.0f, 0.0f)
	const NBPunto* arrVertFig		= figuraInicial->elemento;
	const UI16 conteoVertFig		= figuraInicial->conteo;
	//Buscar el vertice de la figura que concuerda con el centro de la luz (en este momento deberia ser uno solo)
	SI16 iVert, iVerticeOrigen		= -1;
	for(iVert=0; iVert<conteoVertFig; iVert++){
		if(arrVertFig[iVert].x==0.0f && arrVertFig[iVert].y==0.0f){
			iVerticeOrigen			= iVert;
			break;
		}
	}
	NBASSERT(iVerticeOrigen!=-1)
	//Recorrer todos los rangos de sombras preagrupadas
	if(iVerticeOrigen!=-1){
		UI16 iRangoSombra;
		for(iRangoSombra=0; iRangoSombra<conteoSombras; iRangoSombra++){
			if(arregloSombras[iRangoSombra].vectorAngMin==arregloSombras[iRangoSombra].vectorAngMax) continue;
			NBASSERT(NBPUNTO_PRODUCTO_VECTORIAL(arregloSombras[iRangoSombra].vectorAngMin.x, arregloSombras[iRangoSombra].vectorAngMin.y, arregloSombras[iRangoSombra].vectorAngMax.x, arregloSombras[iRangoSombra].vectorAngMax.y)>0.0f)
			//
			AUArregloNativoMutableP<NBPunto>* figuraActual	= _figuras[_indiceFiguraActual];
			AUArregloNativoMutableP<NBPunto>* figuraNueva	= _figuras[_indiceFiguraActual==0?1:0];
			const NBPunto* arrVertFig		= figuraActual->elemento; NBASSERT(arrVertFig[iVerticeOrigen]==posCero)
			const UI16 conteoVertFig		= figuraActual->conteo;
			SI16 iVertAntesOrigen			= iVerticeOrigen - 1; if(iVertAntesOrigen<0) iVertAntesOrigen += conteoVertFig;
			SI16 iVertDespuesOrigen			= iVerticeOrigen + 1; if(iVertDespuesOrigen>=conteoVertFig) iVertDespuesOrigen -= conteoVertFig;
			//Determinar las intersecciones con los vectores angulos
			SI16 iVertInterseccion[2]		= {-1, -1};
			//float avanceInterseccion[2];
			NBPunto posInterseccion[2];
			UI16 iVertAnt					= conteoVertFig - 1;
			NBPunto posVertAnt				= arrVertFig[iVertAnt];
			SI16 iVert; float relAvanceVec, relAvanceSeg, relDenom;
			for(iVert=0; iVert<conteoVertFig; iVert++){
				NBPunto posVertAct		= arrVertFig[iVert];
				if(posVertAnt!=posCero && posVertAct!=posCero){ //se ignoran todos los segmentos que tienen como origen o destino el centro
					if(iVertInterseccion[0]==-1){
						NBLINEA_INSTERSECCION_RELATIVOS(relAvanceVec, relAvanceSeg, relDenom, 0.0f, 0.0f, arregloSombras[iRangoSombra].vectorAngMin.x, arregloSombras[iRangoSombra].vectorAngMin.y, posVertAnt.x, posVertAnt.y, posVertAct.x, posVertAct.y)
						if(relDenom!=0.0f && relAvanceVec>0.0f && relAvanceSeg>=0.0f && relAvanceSeg<=1.0f){
							iVertInterseccion[0]	= iVertAnt;
							if(relAvanceSeg<0.001f){
								//avanceInterseccion[0]	= 0.0f;
								posInterseccion[0]		= posVertAnt;
							} else if(relAvanceSeg>0.999f){
								//avanceInterseccion[0]	= 1.0f;
								posInterseccion[0]		= posVertAct;
							} else {
								//avanceInterseccion[0]	= relAvanceSeg;
								posInterseccion[0].x	= posVertAnt.x + ((posVertAct.x - posVertAnt.x) * relAvanceSeg);
								posInterseccion[0].y	= posVertAnt.y + ((posVertAct.y - posVertAnt.y) * relAvanceSeg);
							}
							if(iVertInterseccion[1]!=-1) break;
						}
					}
					if(iVertInterseccion[1]==-1){
						NBLINEA_INSTERSECCION_RELATIVOS(relAvanceVec, relAvanceSeg, relDenom, 0.0f, 0.0f, arregloSombras[iRangoSombra].vectorAngMax.x, arregloSombras[iRangoSombra].vectorAngMax.y, posVertAnt.x, posVertAnt.y, posVertAct.x, posVertAct.y)
						if(relDenom!=0.0f && relAvanceVec>0.0f && relAvanceSeg>=0.0f && relAvanceSeg<=1.0f){
							iVertInterseccion[1]	= iVertAnt;
							if(relAvanceSeg<0.001f){
								//avanceInterseccion[1]	= 0.0f;
								posInterseccion[1]		= posVertAnt;
							} else if(relAvanceSeg>0.999f){
								//avanceInterseccion[1]	= 1.0f;
								posInterseccion[1]		= posVertAct;
							} else {
								//avanceInterseccion[1]	= relAvanceSeg;
								posInterseccion[1].x	= posVertAnt.x + ((posVertAct.x - posVertAnt.x) * relAvanceSeg);
								posInterseccion[1].y	= posVertAnt.y + ((posVertAct.y - posVertAnt.y) * relAvanceSeg);
							}
							if(iVertInterseccion[0]!=-1) break;
						}
					}
				}
				//Siguiente
				posVertAnt	= posVertAct;
				iVertAnt	= iVert;
			}
			//PRINTF_INFO("Rango %d, Intersecciones(%d, %d)\n", iRangoSombra, iVertInterseccion[0], iVertInterseccion[1]);
			//NBASSERT(iVertInterseccion[0]!=-1 && iVertInterseccion[1]!=-1) //Pendiente: si alguno es -1, entonces considerar como que la sombra cubre desde el extremo de la figura
			//NBASSERT(posInterseccion[0]!=posOrigenLuz && posInterseccion[1]!=posOrigenLuz)
			if(iVertInterseccion[0]!=-1 && iVertInterseccion[1]!=-1){
				if(posInterseccion[0]!=posInterseccion[1]){ //Ignorar los casos en el que la interseccion solo forma una linea-sin-ancho como sombra
					figuraNueva->vaciar();
					//Agregar el vertice origen
					figuraNueva->agregarElemento(posCero);
					//Agregar todos los vertices siguiente al origen hasta el que produce la interseccion AngMin
					SI16 iVertN = iVerticeOrigen;
					do {
						iVertN++; if(iVertN>=conteoVertFig) iVertN -= conteoVertFig;
						if(figuraNueva->elemento[figuraNueva->conteo-1]!=arrVertFig[iVertN]){
							figuraNueva->agregarElemento(arrVertFig[iVertN]);
						}
					} while(iVertN!=iVertInterseccion[0]);
					//Agregar la interseccion AngMin
					if(figuraNueva->elemento[figuraNueva->conteo-1]!=posInterseccion[0]){
						figuraNueva->agregarElemento(posInterseccion[0]);
					}
					//Agregar el vertice origen
					figuraNueva->agregarElemento(posCero);
					//Agregar la interseccion AngMax
					if(figuraNueva->elemento[figuraNueva->conteo-1]!=posInterseccion[1]){
						figuraNueva->agregarElemento(posInterseccion[1]);
					}
					//Agregar todos los vertices siguiente a la interseccion AngMax hasta uno antes que iVerticeOrigen
					iVertN = iVertInterseccion[1];
					do {
						iVertN++; if(iVertN>=conteoVertFig) iVertN -= conteoVertFig;
						if(figuraNueva->elemento[figuraNueva->conteo-1]!=arrVertFig[iVertN]){
							figuraNueva->agregarElemento(arrVertFig[iVertN]);
						}
					} while(iVertN!=iVertAntesOrigen);
					NBASSERT(figuraNueva->conteo>=5) //6
					NBASSERT(figuraNueva->elemento[figuraNueva->conteo-1]!=posCero)
					NBASSERT(figuraNueva->elemento[0]==posCero)
					NBASSERT(figuraNueva->elemento[1]==arrVertFig[iVertDespuesOrigen])
					NBASSERT(figuraNueva->elemento[figuraNueva->conteo-1]==arrVertFig[iVertAntesOrigen])
					//Siguiente figura y rangoSombra
					iVerticeOrigen	= 0;
					_indiceFiguraActual = _indiceFiguraActual==0?1:0;
				}
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void NBSegmentadorFiguras::dividirEnPorcionesDerechaLinea(const NBPunto &posVector, const NBPunto &vectUnitCorte){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("NBSegmentadorFiguras::dividirEnPorcionesDerechaLinea")
	#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
	float debugModulo				= NBPUNTO_DISTANCIA_VECTOR(vectUnitCorte.x, vectUnitCorte.y); NBASSERT(debugModulo>0.9999f && debugModulo<1.0001f)
	#endif
	AUArregloNativoMutableP<NBPunto>* figuraActual = _figuras[_indiceFiguraActual];
	//NBASSERT(NBSegmentadorFiguras::figuraNoRepiteVertices(figuraActual->elemento, figuraActual->conteo)); //Esta validacion no se respeta, si la figura-luz heredo sombras-preagrupadas se repite el vertice(0.0f, 0.0f) varias veces
	const NBPunto* arrVertFig		= figuraActual->elemento;
	const UI16 conteoVertFig		= figuraActual->conteo;
	//Reiniciar cache
	_cacheConteoFigurasUsadas		= 0;
	//Buscar cualquier vertice completamente a la izquierda del vector limite
	NBPunto vectVertice;
	float prodVectInicio;
	SI16 iVert, iVertInicio			= -1;
	for(iVert=0; iVert<conteoVertFig; iVert++){
		NBPUNTO_ESTABLECER(vectVertice, arrVertFig[iVert].x - posVector.x, arrVertFig[iVert].y - posVector.y)
		prodVectInicio		= NBPUNTO_PRODUCTO_VECTORIAL(vectUnitCorte.x, vectUnitCorte.y, vectVertice.x, vectVertice.y);
		if(prodVectInicio>0.0f){
			iVertInicio = iVert;
			break;
		}
	}
	// Nota: es valido el caso que toda la luz este a la derecha del vector limite.
	// (luces producidas por espejos, cuya validacion ha fallado)
	if(iVertInicio!=-1){
		//Recorrer todos los vertices identificando cambio de izquierda a derecha (y viceversa) del vector corte
		//Identificar las intersecciones
		bool dentroAreaValida	= false;
		SI16 iVertAct			= iVertInicio;
		float prodVectAct		= prodVectInicio;
		AUArregloNativoMutableP<NBPunto>* subFiguraActual = NULL;
		do {
			//Siguiente vertice
			float prodVectAnt	= prodVectAct;
			SI16 iVertAnt		= iVertAct;
			iVertAct++;			if(iVertAct>=conteoVertFig) iVertAct = 0;
			NBPUNTO_ESTABLECER(vectVertice, arrVertFig[iVertAct].x - posVector.x, arrVertFig[iVertAct].y - posVector.y)
			prodVectAct			= NBPUNTO_PRODUCTO_VECTORIAL(vectUnitCorte.x, vectUnitCorte.y, vectVertice.x, vectVertice.y);
			//PRINTF_INFO("ProdVect ant(%.4f) desp(%.4f)  ---- posVector(%.3f, %.3f) VerticeAnt(%.3f, %.3f) VerticeAct(%.3f, %.3f)\n", prodVectAnt, prodVecAct, posVector.x, posVector.y, arrVertFig[iVertAnt].x, arrVertFig[iVertAnt].y, arrVertFig[iVertAct].x, arrVertFig[iVertAct].y);
			if(prodVectAnt!=prodVectAct && !((prodVectAnt<0.0f && prodVectAct<0.0f) || (prodVectAnt>0.0f && prodVectAct>0.0f))){
				float avanceRelEnSegFig = prodVectAnt / (prodVectAnt - prodVectAct);
				NBPunto posInterseccion;
				if(avanceRelEnSegFig!=0.0f && avanceRelEnSegFig!=1.0f){
					NBPUNTO_ESTABLECER(posInterseccion, arrVertFig[iVertAnt].x + ((arrVertFig[iVertAct].x - arrVertFig[iVertAnt].x) * avanceRelEnSegFig), arrVertFig[iVertAnt].y + ((arrVertFig[iVertAct].y - arrVertFig[iVertAnt].y)) * avanceRelEnSegFig);
				} else if(avanceRelEnSegFig==1.0f){
					posInterseccion = arrVertFig[iVertAct];
				} else {
					NBASSERT(avanceRelEnSegFig==0.0f)
					posInterseccion = arrVertFig[iVertAnt];
				}
				dentroAreaValida	= !dentroAreaValida;
				if(dentroAreaValida){
					if(subFiguraActual != NULL){
						NBASSERT(subFiguraActual->conteo>=3)
						//NBASSERT(NBSegmentadorFiguras::figuraNoRepiteVertices(subFiguraActual->elemento, subFiguraActual->conteo));
						NBSegmentadorFiguras::privAgregarOrejasNoConvexasDeFigura(subFiguraActual);
					}
					//Iniciar nueva figura
					subFiguraActual = NBSegmentadorFiguras::privCacheReservarNuevaFigura();
				}
				subFiguraActual->agregarElemento(posInterseccion);
			}
			if(dentroAreaValida){
				NBASSERT(subFiguraActual != NULL)
				subFiguraActual->agregarElemento(arrVertFig[iVertAct]);
			}
		} while(iVertAct!=iVertInicio);
		NBASSERT(!dentroAreaValida) //Si falla, no termina fuera del area valida (y deberia)
		if(subFiguraActual != NULL){
			NBASSERT(subFiguraActual->conteo>=3)
			//NBASSERT(NBSegmentadorFiguras::figuraNoRepiteVertices(subFiguraActual->elemento, subFiguraActual->conteo));
			NBSegmentadorFiguras::privAgregarOrejasNoConvexasDeFigura(subFiguraActual);
		}
	} else {
		//La figura original no tiene ningun vertice a la izquieda del vector limite
		//La figura queda intacta (todos estan a la derecha)
		AUArregloNativoMutableP<NBPunto>* subFiguraActual = NBSegmentadorFiguras::privCacheReservarNuevaFigura();
		subFiguraActual->agregarElementos(arrVertFig, conteoVertFig);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

SI16 NBSegmentadorFiguras::privIndiceVerticeCompletamenteFueraDeSombraDosVertices(const NBPunto* arrVerticesFig, const UI16 conteoVerticesFig, const NBPunto &vecAngMinSombra, const NBPunto &vecAngMaxSombra){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("NBSegmentadorFiguras::privIndiceVerticeCompletamenteFueraDeSombraDosVertices")
	NBASSERT(conteoVerticesFig>2)
	NBASSERT(NBPUNTO_PRODUCTO_VECTORIAL(vecAngMinSombra.x, vecAngMinSombra.y, vecAngMaxSombra.x, vecAngMaxSombra.y)>0.0f)
	UI16 iVertFig;
	for(iVertFig=0; iVertFig<conteoVerticesFig; iVertFig++) {
		const NBPunto vectorVertice = arrVerticesFig[iVertFig];
		//Determinar que no esta dentro de la luz comparando con vectorSombraAngMin y vectorSombraAngMax
		//Y determinar si no-esta-a-la-izquierda del segmento sombra
		if(NBPUNTO_PRODUCTO_VECTORIAL(vecAngMinSombra.x, vecAngMinSombra.y, vectorVertice.x, vectorVertice.y)<0.0f ||
		   NBPUNTO_PRODUCTO_VECTORIAL(vecAngMaxSombra.x, vecAngMaxSombra.y, vectorVertice.x, vectorVertice.y)>0.0f ||
		   NBPUNTO_PRODUCTO_VECTORIAL(vecAngMinSombra.x - vecAngMaxSombra.x, vecAngMinSombra.y - vecAngMaxSombra.y, vectorVertice.x - vecAngMaxSombra.x, vectorVertice.y - vecAngMaxSombra.y)<0.0f){
			//PRINTF_INFO("Productos vectorial (%.4f, %.4f)\n", NBPUNTO_PRODUCTO_VECTORIAL(datosSombra->vectorAnguloMin.x, datosSombra->vectorAnguloMin.y, vecDesdeOrigenLuz.x, vecDesdeOrigenLuz.y), NBPUNTO_PRODUCTO_VECTORIAL(datosSombra->vectorAnguloMax.x, datosSombra->vectorAnguloMax.y, vecDesdeOrigenLuz.x, vecDesdeOrigenLuz.y));
			AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO_3(iVertFig)
		}
	}
	NBASSERT(false) //Nunca deberia llegar a este punto
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return -1;
}

SI16 NBSegmentadorFiguras::privIndiceVerticeCompletamenteFueraDeSombraMultiVertices(const NBPunto* arrVerticesFig, const UI16 conteoVerticesFig, const NBPunto &vecAngMinSombra, const NBPunto &vecAngMaxSombra, const NBPunto* arrVertSmb, const NBPunto* vertSmbSigUlt){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("NBSegmentadorFiguras::privIndiceVerticeCompletamenteFueraDeSombraMultiVertices")
	NBASSERT(conteoVerticesFig>2)
	NBASSERT(arrVertSmb<vertSmbSigUlt) //Si falla, debio llamarse a la version optimizada para sombras-de-dos-vertices
	NBASSERT(NBPUNTO_PRODUCTO_VECTORIAL(vecAngMinSombra.x, vecAngMinSombra.y, vecAngMaxSombra.x, vecAngMaxSombra.y)>0.0f)
	UI16 iVertFig;
	for(iVertFig=0; iVertFig<conteoVerticesFig; iVertFig++) {
		const NBPunto vectorVertice = arrVerticesFig[iVertFig];
		//Determinar que no-esta dentro de la luz comparando con vectorSombraAngMin y vectorSombraAngMax
		if(NBPUNTO_PRODUCTO_VECTORIAL(vecAngMinSombra.x, vecAngMinSombra.y, vectorVertice.x, vectorVertice.y)<0.0f ||
			 NBPUNTO_PRODUCTO_VECTORIAL(vecAngMaxSombra.x, vecAngMaxSombra.y, vectorVertice.x, vectorVertice.y)>0.0f){
			//PRINTF_INFO("Productos vectorial (%.4f, %.4f)\n", NBPUNTO_PRODUCTO_VECTORIAL(datosSombra->vectorAnguloMin.x, datosSombra->vectorAnguloMin.y, vecDesdeOrigenLuz.x, vecDesdeOrigenLuz.y), NBPUNTO_PRODUCTO_VECTORIAL(datosSombra->vectorAnguloMax.x, datosSombra->vectorAnguloMax.y, vecDesdeOrigenLuz.x, vecDesdeOrigenLuz.y));
			AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO_3(iVertFig)
		}
		//El vertice esta entre los angulos, ahora determinar si no-esta-a-la-izquierda de los segmentos sombras
		NBPunto posVertSombAnt	= vecAngMaxSombra;
		const NBPunto* vertSmb	= arrVertSmb;
		while(vertSmb<vertSmbSigUlt){
			if(NBPUNTO_PRODUCTO_VECTORIAL(vertSmb->x - posVertSombAnt.x, vertSmb->y - posVertSombAnt.y, vectorVertice.x - posVertSombAnt.x, vectorVertice.y - posVertSombAnt.y)<0.0f){
				//PRINTF_INFO("Prod vectorial (%.4f)\n", NBPUNTO_PRODUCTO_VECTORIAL(vectorSombra.x, vectorSombra.y, vectorVertFig.x, vectorVertFig.y));
				AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO_3(iVertFig)
			}
			//siguiente
			posVertSombAnt = *vertSmb;
			vertSmb++;
		}
		if(NBPUNTO_PRODUCTO_VECTORIAL(vecAngMinSombra.x - posVertSombAnt.x, vecAngMinSombra.y - posVertSombAnt.y, vectorVertice.x - posVertSombAnt.x, vectorVertice.y - posVertSombAnt.y)<0.0f){
			//PRINTF_INFO("Prod vectorial (%.4f)\n", NBPUNTO_PRODUCTO_VECTORIAL(vectorSombra.x, vectorSombra.y, vectorVertFig.x, vectorVertFig.y));
			AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO_3(iVertFig)
		}
	}
	NBASSERT(false) //Nunca deberia llegar a este punto
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return -1;
}

bool NBSegmentadorFiguras::privVerticeEstaCompletamenteFueraDeSombraDosVertices(const NBPunto &vectorVertice, const NBPunto &vecAngMinSombra, const NBPunto &vecAngMaxSombra){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("NBSegmentadorFiguras::privVerticeEstaCompletamenteFueraDeSombraDosVertices")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return (NBPUNTO_PRODUCTO_VECTORIAL(vecAngMinSombra.x, vecAngMinSombra.y, vectorVertice.x, vectorVertice.y)<0.0f ||
			NBPUNTO_PRODUCTO_VECTORIAL(vecAngMaxSombra.x, vecAngMaxSombra.y, vectorVertice.x, vectorVertice.y)>0.0f ||
			NBPUNTO_PRODUCTO_VECTORIAL(vecAngMinSombra.x - vecAngMaxSombra.x, vecAngMinSombra.y - vecAngMaxSombra.y, vectorVertice.x - vecAngMaxSombra.x, vectorVertice.y - vecAngMaxSombra.y)<0.0f);
}

bool NBSegmentadorFiguras::privVerticeEstaCompletamenteFueraDeSombraMultiVertices(const NBPunto &vectorVertice, const NBPunto &vecAngMinSombra, const NBPunto &vecAngMaxSombra, const NBPunto* arrVertSmb, const NBPunto* vertSmbSigUlt){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("NBSegmentadorFiguras::privVerticeEstaCompletamenteFueraDeSombraMultiVertices")
	NBASSERT(arrVertSmb<vertSmbSigUlt) //Si falla, el arreglo esta vacio, mejor utilizar la version optimizada para sombra-sin-vertices-intermedios
	if(NBPUNTO_PRODUCTO_VECTORIAL(vecAngMinSombra.x, vecAngMinSombra.y, vectorVertice.x, vectorVertice.y)<0.0f ||
	   NBPUNTO_PRODUCTO_VECTORIAL(vecAngMaxSombra.x, vecAngMaxSombra.y, vectorVertice.x, vectorVertice.y)>0.0f){
		AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO_3(true)
	}
	NBPunto posAnterior		= vecAngMaxSombra;
	while(arrVertSmb<vertSmbSigUlt){
		if(NBPUNTO_PRODUCTO_VECTORIAL(arrVertSmb->x - posAnterior.x, arrVertSmb->y - posAnterior.y, vectorVertice.x - posAnterior.x, vectorVertice.y - posAnterior.y)<0.0f){
			AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO_3(true)
		}
		posAnterior = *arrVertSmb;
		arrVertSmb++;
	}
	if(NBPUNTO_PRODUCTO_VECTORIAL(vecAngMinSombra.x - posAnterior.x, vecAngMinSombra.y - posAnterior.y, vectorVertice.x - posAnterior.x, vectorVertice.y - posAnterior.y)<0.0f){
		AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO_3(true)
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return false;
}

UI16 NBSegmentadorFiguras::privBuscarUnaInterseccionConSombraDosVerticesOrdenInversoVsSegmento(NBSegFigInterseccionSombra* guardarEn, const NBPunto &iniSegFig, const NBPunto &finSegFig, const NBPunto &vecAngMinSombra, const NBPunto &vecAngMaxSombra, const NBPunto &posFinVectorSmbAngMin, const NBPunto &posFinVectorSmbAngMax){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("NBSegmentadorFiguras::privBuscarUnaInterseccionConSombraDosVerticesOrdenInversoVsSegmento")
	NBASSERT(posFinVectorSmbAngMin.x==vecAngMinSombra.x*2.0f && posFinVectorSmbAngMin.y==vecAngMinSombra.y*2.0f)
	NBASSERT(posFinVectorSmbAngMax.x==vecAngMaxSombra.x*2.0f && posFinVectorSmbAngMax.y==vecAngMaxSombra.y*2.0f)
	bool primerInterEncontrada = false;
	NBSegFigInterseccionSombra interseccionTmp;
	float diffHaciaSegSmbMasCercano, diffHaciaSegSmbMasCercanoTmp;
	float relIntersSegFigOrig, relIntersSegSmbOrig;
	float denom;
	//Detectar interseccion con el segmentoAngMin
	NBLINEA_INSTERSECCION_RELATIVOS(relIntersSegSmbOrig, relIntersSegFigOrig, denom, vecAngMinSombra.x, vecAngMinSombra.y, posFinVectorSmbAngMin.x, posFinVectorSmbAngMin.y, iniSegFig.x, iniSegFig.y, finSegFig.x, finSegFig.y)
	if(denom!=0.0f && relIntersSegFigOrig>=0.0f && relIntersSegFigOrig<=1.0f){
		interseccionTmp.iVertSombra				= 0;
		interseccionTmp.avanceEnSegSombra		= relIntersSegSmbOrig;
		interseccionTmp.avanceEnSegFigura		= relIntersSegFigOrig;
		interseccionTmp.precisaEnSombra			= (interseccionTmp.avanceEnSegSombra>=0.0f); //Proyectado al infinito
		if(interseccionTmp.precisaEnSombra){
			*guardarEn							= interseccionTmp;
			AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO_3(1) //La interseccion es precisa, retornar (solo se continua si la interseccion actual es una aproximacion)
		}
		primerInterEncontrada					= true;
		*guardarEn								= interseccionTmp;
		diffHaciaSegSmbMasCercano				= 0.0f - relIntersSegSmbOrig; NBASSERT(diffHaciaSegSmbMasCercano>0.0f)
	}
	//Detectar interseccion con los segmentos internos de sombra (en orden horario)
	NBLINEA_INSTERSECCION_RELATIVOS(relIntersSegSmbOrig, relIntersSegFigOrig, denom, vecAngMaxSombra.x, vecAngMaxSombra.y, vecAngMinSombra.x, vecAngMinSombra.y, iniSegFig.x, iniSegFig.y, finSegFig.x, finSegFig.y)
	if(denom!=0.0f && relIntersSegFigOrig>=0.0f && relIntersSegFigOrig<=1.0f){
		interseccionTmp.iVertSombra				= -1;
		interseccionTmp.avanceEnSegSombra		= relIntersSegSmbOrig;
		interseccionTmp.avanceEnSegFigura		= relIntersSegFigOrig;
		interseccionTmp.precisaEnSombra			= interseccionTmp.avanceEnSegSombra>=0.0f && interseccionTmp.avanceEnSegSombra<=1.0f;
		if(interseccionTmp.precisaEnSombra){
			*guardarEn							= interseccionTmp;
			AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO_3(1) //La interseccion es precisa, retornar (solo se continua si la interseccion actual es una aproximacion)
		}
		diffHaciaSegSmbMasCercanoTmp			= 0.0f - relIntersSegSmbOrig; if(diffHaciaSegSmbMasCercanoTmp<0.0f) diffHaciaSegSmbMasCercanoTmp = relIntersSegSmbOrig - 1.0f; NBASSERT(diffHaciaSegSmbMasCercanoTmp>0.0f)
		if(!primerInterEncontrada || diffHaciaSegSmbMasCercanoTmp<diffHaciaSegSmbMasCercano){
			primerInterEncontrada				= true;
			*guardarEn							= interseccionTmp;
			diffHaciaSegSmbMasCercano			= diffHaciaSegSmbMasCercanoTmp;
		}
	}
	//Detectar interseccion con el segmentoAngMax
	NBLINEA_INSTERSECCION_RELATIVOS(relIntersSegSmbOrig, relIntersSegFigOrig, denom, vecAngMaxSombra.x, vecAngMaxSombra.y, posFinVectorSmbAngMax.x, posFinVectorSmbAngMax.y, iniSegFig.x, iniSegFig.y, finSegFig.x, finSegFig.y)
	if(denom!=0.0f && relIntersSegFigOrig>=0.0f && relIntersSegFigOrig<=1.0f){
		interseccionTmp.iVertSombra				= -2;
		interseccionTmp.avanceEnSegSombra		= relIntersSegSmbOrig;
		interseccionTmp.avanceEnSegFigura		= relIntersSegFigOrig;
		interseccionTmp.precisaEnSombra			= interseccionTmp.avanceEnSegSombra>=0.0f; //Proyectado al infinito
		if(interseccionTmp.precisaEnSombra){
			*guardarEn							= interseccionTmp;
			AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO_3(1) //La interseccion es precisa, retornar (solo se continua si la interseccion actual es una aproximacion)
		}
		diffHaciaSegSmbMasCercanoTmp			= 0.0f - relIntersSegSmbOrig; NBASSERT(diffHaciaSegSmbMasCercanoTmp>0.0f)
		if(!primerInterEncontrada || diffHaciaSegSmbMasCercanoTmp<diffHaciaSegSmbMasCercano){
			primerInterEncontrada				= true;
			*guardarEn							= interseccionTmp;
			diffHaciaSegSmbMasCercano			= diffHaciaSegSmbMasCercanoTmp;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return 0;
}

UI16 NBSegmentadorFiguras::privBuscarUnaInterseccionConSombraMultiVerticesOrdenInversoVsSegmento(NBSegFigInterseccionSombra* guardarEn, const NBPunto &iniSegFig, const NBPunto &finSegFig, const NBPunto &vecAngMinSombra, const NBPunto &vecAngMaxSombra, const NBPunto &posFinVectorSmbAngMin, const NBPunto &posFinVectorSmbAngMax, const NBPunto* arrVertSmb, const SI16 conteoVertSmb){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("NBSegmentadorFiguras::privBuscarUnaInterseccionConSombraMultiVerticesOrdenInversoVsSegmento")
	NBASSERT(conteoVertSmb!=0) //Si falla, entonces debio utilizarse la opcion optimizada para sombra-dos-vertices
	NBASSERT(posFinVectorSmbAngMin.x==vecAngMinSombra.x*2.0f && posFinVectorSmbAngMin.y==vecAngMinSombra.y*2.0f)
	NBASSERT(posFinVectorSmbAngMax.x==vecAngMaxSombra.x*2.0f && posFinVectorSmbAngMax.y==vecAngMaxSombra.y*2.0f)
	bool primerInterEncontrada = false;
	NBSegFigInterseccionSombra interseccionTmp;
	float diffHaciaSegSmbMasCercano, diffHaciaSegSmbMasCercanoTmp;
	float relIntersSegFigOrig, relIntersSegSmbOrig;
	float denom;
	NBLINEA_INSTERSECCION_RELATIVOS(relIntersSegSmbOrig, relIntersSegFigOrig, denom, vecAngMinSombra.x, vecAngMinSombra.y, posFinVectorSmbAngMin.x, posFinVectorSmbAngMin.y, iniSegFig.x, iniSegFig.y, finSegFig.x, finSegFig.y)
	if(denom!=0.0f && relIntersSegFigOrig>=0.0f && relIntersSegFigOrig<=1.0f){
		interseccionTmp.iVertSombra					= conteoVertSmb;
		interseccionTmp.avanceEnSegSombra			= relIntersSegSmbOrig;
		interseccionTmp.avanceEnSegFigura			= relIntersSegFigOrig;
		interseccionTmp.precisaEnSombra				= interseccionTmp.avanceEnSegSombra>=0.0f; //Proyectado al infinito
		if(interseccionTmp.precisaEnSombra){
			*guardarEn								= interseccionTmp;
			AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO_3(1) //La interseccion es precisa, retornar (solo se continua si la interseccion actual es una aproximacion)
		}
		primerInterEncontrada						= true;
		*guardarEn									= interseccionTmp;
		diffHaciaSegSmbMasCercano					= 0.0f - relIntersSegSmbOrig; NBASSERT(diffHaciaSegSmbMasCercano>0.0f)
	}
	//Detectar interseccion con el segmento que termina en el AngMin
	NBPunto vertSombSig	= arrVertSmb[conteoVertSmb-1]; NBASSERT(conteoVertSmb>0)
	NBLINEA_INSTERSECCION_RELATIVOS(relIntersSegSmbOrig, relIntersSegFigOrig, denom, vertSombSig.x, vertSombSig.y, vecAngMinSombra.x, vecAngMinSombra.y, iniSegFig.x, iniSegFig.y, finSegFig.x, finSegFig.y)
	if(denom!=0.0f && relIntersSegFigOrig>=0.0f && relIntersSegFigOrig<=1.0f){
		interseccionTmp.iVertSombra					= conteoVertSmb - 1;
		interseccionTmp.avanceEnSegSombra			= relIntersSegSmbOrig;
		interseccionTmp.avanceEnSegFigura			= relIntersSegFigOrig;
		interseccionTmp.precisaEnSombra				= interseccionTmp.avanceEnSegSombra>=0.0f && interseccionTmp.avanceEnSegSombra<=1.0f;
		if(interseccionTmp.precisaEnSombra){
			*guardarEn								= interseccionTmp;
			AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO_3(1) //La interseccion es precisa, retornar (solo se continua si la interseccion actual es una aproximacion)
		}
		diffHaciaSegSmbMasCercanoTmp				= 0.0f - relIntersSegSmbOrig; if(diffHaciaSegSmbMasCercanoTmp<0.0f) diffHaciaSegSmbMasCercanoTmp = relIntersSegSmbOrig - 1.0f; NBASSERT(diffHaciaSegSmbMasCercanoTmp>0.0f)
		if(!primerInterEncontrada || diffHaciaSegSmbMasCercanoTmp<diffHaciaSegSmbMasCercano){
			primerInterEncontrada					= true;
			*guardarEn								= interseccionTmp;
			diffHaciaSegSmbMasCercano				= diffHaciaSegSmbMasCercanoTmp;
		}
	}
	//Detectar interseccion con los segmentos internos de sombra (en orden horario)
	SI16 iVertSombAnt;
	for(iVertSombAnt=conteoVertSmb-2; iVertSombAnt>=0; iVertSombAnt--){
		NBLINEA_INSTERSECCION_RELATIVOS(relIntersSegSmbOrig, relIntersSegFigOrig, denom, arrVertSmb[iVertSombAnt].x, arrVertSmb[iVertSombAnt].y, vertSombSig.x, vertSombSig.y, iniSegFig.x, iniSegFig.y, finSegFig.x, finSegFig.y)
		if(denom!=0.0f && relIntersSegFigOrig>=0.0f && relIntersSegFigOrig<=1.0f){
			interseccionTmp.iVertSombra				= iVertSombAnt;
			interseccionTmp.avanceEnSegSombra		= relIntersSegSmbOrig;
			interseccionTmp.avanceEnSegFigura		= relIntersSegFigOrig;
			interseccionTmp.precisaEnSombra			= interseccionTmp.avanceEnSegSombra>=0.0f && interseccionTmp.avanceEnSegSombra<=1.0f;
			if(interseccionTmp.precisaEnSombra){
				*guardarEn							= interseccionTmp;
				AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO_3(1) //La interseccion es precisa, retornar (solo se continua si la interseccion actual es una aproximacion)
			}
			diffHaciaSegSmbMasCercanoTmp			= 0.0f - relIntersSegSmbOrig; if(diffHaciaSegSmbMasCercanoTmp<0.0f) diffHaciaSegSmbMasCercanoTmp = relIntersSegSmbOrig - 1.0f; NBASSERT(diffHaciaSegSmbMasCercanoTmp>0.0f)
			if(!primerInterEncontrada || diffHaciaSegSmbMasCercanoTmp<diffHaciaSegSmbMasCercano){
				primerInterEncontrada				= true;
				*guardarEn							= interseccionTmp;
				diffHaciaSegSmbMasCercano			= diffHaciaSegSmbMasCercanoTmp;
			}
		}
		vertSombSig = arrVertSmb[iVertSombAnt];
	}
	//Detectar interseccion con el segmento que inicia en AngMax
	NBLINEA_INSTERSECCION_RELATIVOS(relIntersSegSmbOrig, relIntersSegFigOrig, denom, vecAngMaxSombra.x, vecAngMaxSombra.y, vertSombSig.x, vertSombSig.y, iniSegFig.x, iniSegFig.y, finSegFig.x, finSegFig.y)
	if(denom!=0.0f && relIntersSegFigOrig>=0.0f && relIntersSegFigOrig<=1.0f){
		interseccionTmp.iVertSombra					= -1;
		interseccionTmp.avanceEnSegSombra			= relIntersSegSmbOrig;
		interseccionTmp.avanceEnSegFigura			= relIntersSegFigOrig;
		interseccionTmp.precisaEnSombra				= interseccionTmp.avanceEnSegSombra>=0.0f && interseccionTmp.avanceEnSegSombra<=1.0f;
		if(interseccionTmp.precisaEnSombra){
			*guardarEn								= interseccionTmp;
			AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO_3(1) //La interseccion es precisa, retornar (solo se continua si la interseccion actual es una aproximacion)
		}
		diffHaciaSegSmbMasCercanoTmp				= 0.0f - relIntersSegSmbOrig; if(diffHaciaSegSmbMasCercanoTmp<0.0f) diffHaciaSegSmbMasCercanoTmp = relIntersSegSmbOrig - 1.0f; NBASSERT(diffHaciaSegSmbMasCercanoTmp>0.0f)
		if(!primerInterEncontrada || diffHaciaSegSmbMasCercanoTmp<diffHaciaSegSmbMasCercano){
			primerInterEncontrada					= true;
			*guardarEn								= interseccionTmp;
			diffHaciaSegSmbMasCercano				= diffHaciaSegSmbMasCercanoTmp;
		}
	}
	//Detectar interseccion con el segmentoAngMaxMax
	NBLINEA_INSTERSECCION_RELATIVOS(relIntersSegSmbOrig, relIntersSegFigOrig, denom, vecAngMaxSombra.x, vecAngMaxSombra.y, posFinVectorSmbAngMax.x, posFinVectorSmbAngMax.y, iniSegFig.x, iniSegFig.y, finSegFig.x, finSegFig.y)
	if(denom!=0.0f && relIntersSegFigOrig>=0.0f && relIntersSegFigOrig<=1.0f){
		interseccionTmp.iVertSombra					= -2;
		interseccionTmp.avanceEnSegSombra			= relIntersSegSmbOrig;
		interseccionTmp.avanceEnSegFigura			= relIntersSegFigOrig;
		interseccionTmp.precisaEnSombra				= interseccionTmp.avanceEnSegSombra>=0.0f; //Proyectado al infinito
		if(interseccionTmp.precisaEnSombra){
			*guardarEn								= interseccionTmp;
			AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO_3(1) //La interseccion es precisa, retornar (solo se continua si la interseccion actual es una aproximacion)
		}
		diffHaciaSegSmbMasCercanoTmp				= 0.0f - relIntersSegSmbOrig; NBASSERT(diffHaciaSegSmbMasCercanoTmp>0.0f)
		if(!primerInterEncontrada || diffHaciaSegSmbMasCercanoTmp<diffHaciaSegSmbMasCercano){
			primerInterEncontrada					= true;
			*guardarEn								= interseccionTmp;
			diffHaciaSegSmbMasCercano				= diffHaciaSegSmbMasCercanoTmp;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return 0;
}

//
//
//

UI16 NBSegmentadorFiguras::privBuscarHastaDosInterseccionesConSombraDosVerticesOrdenInversoVsSegmento(NBSegFigInterseccionSombra* guardarEn, const NBPunto &iniSegFig, const NBPunto &finSegFig, const NBPunto &vecAngMinSombra, const NBPunto &vecAngMaxSombra, const NBPunto &posFinVectorSmbAngMin, const NBPunto &posFinVectorSmbAngMax){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("NBSegmentadorFiguras::privBuscarHastaDosInterseccionesConSombraDosVerticesOrdenInversoVsSegmento")
	NBASSERT(posFinVectorSmbAngMin.x==vecAngMinSombra.x*2.0f && posFinVectorSmbAngMin.y==vecAngMinSombra.y*2.0f)
	NBASSERT(posFinVectorSmbAngMax.x==vecAngMaxSombra.x*2.0f && posFinVectorSmbAngMax.y==vecAngMaxSombra.y*2.0f)
	UI16 cantEncontradas	= 0;
	UI16 iRegistrarTmp;
	float diffHaciaSegSmbMasCercano[2] = {0.0f, 0.0f}, diffHaciaSegSmbMasCercanoTmp = 0.0f;
	NBSegFigInterseccionSombra interseccionTmp;
	float relIntersSegFigOrig, relIntersSegSmbOrig;
	float denom;
	//bool interseccionEncontrada;
	//float prodVecSegSmbVsIniOtro, prodVecSegSmbVsFinOtro, prodVecSegFigVsIniOtro, prodVecSegFigVsFinOtro;
	guardarEn[0].avanceEnSegSombra	= guardarEn[0].avanceEnSegFigura = -1.0f;
	guardarEn[1].avanceEnSegSombra	= guardarEn[1].avanceEnSegFigura = -1.0f;
	//Detectar interseccion con el segmentoAngMin
	NBLINEA_INSTERSECCION_RELATIVOS(relIntersSegSmbOrig, relIntersSegFigOrig, denom, vecAngMinSombra.x, vecAngMinSombra.y, posFinVectorSmbAngMin.x, posFinVectorSmbAngMin.y, iniSegFig.x, iniSegFig.y, finSegFig.x, finSegFig.y)
	if(denom!=0.0f && relIntersSegFigOrig>=0.0f && relIntersSegFigOrig<=1.0f){
		interseccionTmp.iVertSombra						= 0;
		interseccionTmp.avanceEnSegSombra				= relIntersSegSmbOrig;
		interseccionTmp.avanceEnSegFigura				= relIntersSegFigOrig;
		interseccionTmp.precisaEnSombra					= (interseccionTmp.avanceEnSegSombra>=0.0f); //Proyectado al infinito
		diffHaciaSegSmbMasCercanoTmp					= 0.0f - relIntersSegSmbOrig; NBASSERT(interseccionTmp.precisaEnSombra || diffHaciaSegSmbMasCercanoTmp>0.0f)
		//
		diffHaciaSegSmbMasCercano[0]					= diffHaciaSegSmbMasCercanoTmp;
		guardarEn[0]									= interseccionTmp;
		cantEncontradas++;
	}
	//Detectar interseccion con los segmentos internos de sombra (en orden horario)
	NBLINEA_INSTERSECCION_RELATIVOS(relIntersSegSmbOrig, relIntersSegFigOrig, denom, vecAngMaxSombra.x, vecAngMaxSombra.y, vecAngMinSombra.x, vecAngMinSombra.y, iniSegFig.x, iniSegFig.y, finSegFig.x, finSegFig.y)
	if(denom!=0.0f && relIntersSegFigOrig>=0.0f && relIntersSegFigOrig<=1.0f){
		interseccionTmp.iVertSombra						= -1;
		interseccionTmp.avanceEnSegSombra				= relIntersSegSmbOrig;
		interseccionTmp.avanceEnSegFigura				= relIntersSegFigOrig;
		interseccionTmp.precisaEnSombra					= (interseccionTmp.avanceEnSegSombra>=0.0f && interseccionTmp.avanceEnSegSombra<=1.0f);
		diffHaciaSegSmbMasCercanoTmp					= 0.0f - relIntersSegSmbOrig; if(diffHaciaSegSmbMasCercanoTmp<0.0f) diffHaciaSegSmbMasCercanoTmp = relIntersSegSmbOrig - 1.0f; NBASSERT(interseccionTmp.precisaEnSombra || diffHaciaSegSmbMasCercanoTmp>0.0f)
		//
		if(cantEncontradas<2){
			diffHaciaSegSmbMasCercano[cantEncontradas]	= diffHaciaSegSmbMasCercanoTmp;
			guardarEn[cantEncontradas]					= interseccionTmp;
			cantEncontradas++;
		} else {
			iRegistrarTmp								= (cantEncontradas==1 && !guardarEn[0].precisaEnSombra && (interseccionTmp.precisaEnSombra || diffHaciaSegSmbMasCercanoTmp<diffHaciaSegSmbMasCercano[0])) ? 0 : (cantEncontradas==2 && !guardarEn[1].precisaEnSombra && (interseccionTmp.precisaEnSombra || diffHaciaSegSmbMasCercanoTmp<diffHaciaSegSmbMasCercano[1]))? 1 : 2;
			if(iRegistrarTmp<2){
				diffHaciaSegSmbMasCercano[iRegistrarTmp]= diffHaciaSegSmbMasCercanoTmp;
				guardarEn[iRegistrarTmp]				= interseccionTmp;
				if(guardarEn[0].precisaEnSombra && guardarEn[1].precisaEnSombra){
					AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO_3(cantEncontradas) //La intersecciones son precisas, retornar (solo se continua si la interseccion actual es una aproximacion)
				}
			}
		}
	}
	//Detectar interseccion con el segmentoAngMax
	NBLINEA_INSTERSECCION_RELATIVOS(relIntersSegSmbOrig, relIntersSegFigOrig, denom, vecAngMaxSombra.x, vecAngMaxSombra.y, posFinVectorSmbAngMax.x, posFinVectorSmbAngMax.y, iniSegFig.x, iniSegFig.y, finSegFig.x, finSegFig.y)
	if(denom!=0.0f && relIntersSegFigOrig>=0.0f && relIntersSegFigOrig<=1.0f){
		interseccionTmp.iVertSombra						= -2;
		interseccionTmp.avanceEnSegSombra				= relIntersSegSmbOrig;
		interseccionTmp.avanceEnSegFigura				= relIntersSegFigOrig;
		interseccionTmp.precisaEnSombra					= (interseccionTmp.avanceEnSegSombra>=0.0f); //Proyectado al infinito
		diffHaciaSegSmbMasCercanoTmp					= 0.0f - relIntersSegSmbOrig; NBASSERT(interseccionTmp.precisaEnSombra || diffHaciaSegSmbMasCercanoTmp>0.0f)
		//
		if(cantEncontradas<2){
			diffHaciaSegSmbMasCercano[cantEncontradas]	= diffHaciaSegSmbMasCercanoTmp;
			guardarEn[cantEncontradas]					= interseccionTmp;
			cantEncontradas++;
		} else {
			iRegistrarTmp								= (cantEncontradas==1 && !guardarEn[0].precisaEnSombra && (interseccionTmp.precisaEnSombra || diffHaciaSegSmbMasCercanoTmp<diffHaciaSegSmbMasCercano[0])) ? 0 : (cantEncontradas==2 && !guardarEn[1].precisaEnSombra && (interseccionTmp.precisaEnSombra || diffHaciaSegSmbMasCercanoTmp<diffHaciaSegSmbMasCercano[1]))? 1 : 2;
			if(iRegistrarTmp<2){
				diffHaciaSegSmbMasCercano[iRegistrarTmp]= diffHaciaSegSmbMasCercanoTmp;
				guardarEn[iRegistrarTmp]				= interseccionTmp;
				if(guardarEn[0].precisaEnSombra && guardarEn[1].precisaEnSombra){
					AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO_3(cantEncontradas) //La intersecciones son precisas, retornar (solo se continua si la interseccion actual es una aproximacion)
				}
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return cantEncontradas;
}

UI16 NBSegmentadorFiguras::privBuscarHastaDosInterseccionesConSombraMultiVerticesOrdenInversoVsSegmento(NBSegFigInterseccionSombra* guardarEn, const NBPunto &iniSegFig, const NBPunto &finSegFig, const NBPunto &vecAngMinSombra, const NBPunto &vecAngMaxSombra, const NBPunto &posFinVectorSmbAngMin, const NBPunto &posFinVectorSmbAngMax, const NBPunto* arrVertSmb, const SI16 conteoVertSmb){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("NBSegmentadorFiguras::privBuscarHastaDosInterseccionesConSombraMultiVerticesOrdenInversoVsSegmento")
	NBASSERT(conteoVertSmb!=0) //Si falla, entonces debio utilizarse la opcion optimizada para sombra-dos-vertices
	NBASSERT(posFinVectorSmbAngMin.x==vecAngMinSombra.x*2.0f && posFinVectorSmbAngMin.y==vecAngMinSombra.y*2.0f)
	NBASSERT(posFinVectorSmbAngMax.x==vecAngMaxSombra.x*2.0f && posFinVectorSmbAngMax.y==vecAngMaxSombra.y*2.0f)
	guardarEn[0].avanceEnSegSombra	= guardarEn[0].avanceEnSegFigura = -1.0f;
	guardarEn[1].avanceEnSegSombra	= guardarEn[1].avanceEnSegFigura = -1.0f;
	UI16 cantEncontradas	= 0;
	UI16 iRegistrarTmp;
	float diffHaciaSegSmbMasCercano[2] = {0.0f, 0.0f}, diffHaciaSegSmbMasCercanoTmp = 0.0f;
	NBSegFigInterseccionSombra interseccionTmp;
	float relIntersSegFigOrig, relIntersSegSmbOrig;
	float denom;
	//Detectar interseccion con el segSmbAngMinMin
	NBLINEA_INSTERSECCION_RELATIVOS(relIntersSegSmbOrig, relIntersSegFigOrig, denom, vecAngMinSombra.x, vecAngMinSombra.y, posFinVectorSmbAngMin.x, posFinVectorSmbAngMin.y, iniSegFig.x, iniSegFig.y, finSegFig.x, finSegFig.y)
	if(denom!=0.0f && relIntersSegFigOrig>=0.0f && relIntersSegFigOrig<=1.0f){
		interseccionTmp.iVertSombra						= conteoVertSmb;
		interseccionTmp.avanceEnSegSombra				= relIntersSegSmbOrig;
		interseccionTmp.avanceEnSegFigura				= relIntersSegFigOrig;
		interseccionTmp.precisaEnSombra					= (interseccionTmp.avanceEnSegSombra>=0.0f); //Proyectado al infinito
		diffHaciaSegSmbMasCercanoTmp					= 0.0f - relIntersSegSmbOrig; NBASSERT(interseccionTmp.precisaEnSombra || diffHaciaSegSmbMasCercanoTmp>0.0f)
		//
		diffHaciaSegSmbMasCercano[0]					= diffHaciaSegSmbMasCercanoTmp;
		guardarEn[0]									= interseccionTmp;
		cantEncontradas++;
	}
	//Detectar interseccion con el segmento que termina en el AngMin
	NBPunto vertSombSig	= arrVertSmb[conteoVertSmb-1]; NBASSERT(conteoVertSmb>0)
	NBLINEA_INSTERSECCION_RELATIVOS(relIntersSegSmbOrig, relIntersSegFigOrig, denom, vertSombSig.x, vertSombSig.y, vecAngMinSombra.x, vecAngMinSombra.y, iniSegFig.x, iniSegFig.y, finSegFig.x, finSegFig.y)
	if(denom!=0.0f && relIntersSegFigOrig>=0.0f && relIntersSegFigOrig<=1.0f){
		interseccionTmp.iVertSombra			= conteoVertSmb - 1;
		interseccionTmp.avanceEnSegSombra	= relIntersSegSmbOrig;
		interseccionTmp.avanceEnSegFigura	= relIntersSegFigOrig;
		interseccionTmp.precisaEnSombra		= (interseccionTmp.avanceEnSegSombra>=0.0f && interseccionTmp.avanceEnSegSombra<=1.0f);
		diffHaciaSegSmbMasCercanoTmp		= 0.0f - relIntersSegSmbOrig; if(diffHaciaSegSmbMasCercanoTmp<0.0f) diffHaciaSegSmbMasCercanoTmp = relIntersSegSmbOrig - 1.0f; NBASSERT(interseccionTmp.precisaEnSombra || diffHaciaSegSmbMasCercanoTmp>0.0f)
		//
		if(cantEncontradas<2){
			diffHaciaSegSmbMasCercano[cantEncontradas]	= diffHaciaSegSmbMasCercanoTmp;
			guardarEn[cantEncontradas]					= interseccionTmp;
			cantEncontradas++;
		} else {
			iRegistrarTmp								= (cantEncontradas==1 && !guardarEn[0].precisaEnSombra && (interseccionTmp.precisaEnSombra || diffHaciaSegSmbMasCercanoTmp<diffHaciaSegSmbMasCercano[0])) ? 0 : (cantEncontradas==2 && !guardarEn[1].precisaEnSombra && (interseccionTmp.precisaEnSombra || diffHaciaSegSmbMasCercanoTmp<diffHaciaSegSmbMasCercano[1]))? 1 : 2;
			if(iRegistrarTmp<2){
				diffHaciaSegSmbMasCercano[iRegistrarTmp]= diffHaciaSegSmbMasCercanoTmp;
				guardarEn[iRegistrarTmp]				= interseccionTmp;
				if(guardarEn[0].precisaEnSombra && guardarEn[1].precisaEnSombra){
					AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO_3(cantEncontradas) //La intersecciones son precisas, retornar (solo se continua si la interseccion actual es una aproximacion)
				}
			}
		}
	}
	//Detectar interseccion con los segmentos internos de sombra (en orden horario)
	SI16 iVertSombAnt;
	for(iVertSombAnt=conteoVertSmb-2; iVertSombAnt>=0; iVertSombAnt--){
		NBLINEA_INSTERSECCION_RELATIVOS(relIntersSegSmbOrig, relIntersSegFigOrig, denom, arrVertSmb[iVertSombAnt].x, arrVertSmb[iVertSombAnt].y, vertSombSig.x, vertSombSig.y, iniSegFig.x, iniSegFig.y, finSegFig.x, finSegFig.y)
		if(denom!=0.0f && relIntersSegFigOrig>=0.0f && relIntersSegFigOrig<=1.0f){
			interseccionTmp.iVertSombra					= iVertSombAnt;
			interseccionTmp.avanceEnSegSombra			= relIntersSegSmbOrig;
			interseccionTmp.avanceEnSegFigura			= relIntersSegFigOrig;
			interseccionTmp.precisaEnSombra				= (interseccionTmp.avanceEnSegSombra>=0.0f && interseccionTmp.avanceEnSegSombra<=1.0f);
			diffHaciaSegSmbMasCercanoTmp				= 0.0f - relIntersSegSmbOrig; if(diffHaciaSegSmbMasCercanoTmp<0.0f) diffHaciaSegSmbMasCercanoTmp = relIntersSegSmbOrig - 1.0f; NBASSERT(interseccionTmp.precisaEnSombra || diffHaciaSegSmbMasCercanoTmp>0.0f)
			//
			if(cantEncontradas<2){
				diffHaciaSegSmbMasCercano[cantEncontradas]	= diffHaciaSegSmbMasCercanoTmp;
				guardarEn[cantEncontradas]					= interseccionTmp;
				cantEncontradas++;
			} else {
				iRegistrarTmp								= (cantEncontradas==1 && !guardarEn[0].precisaEnSombra && (interseccionTmp.precisaEnSombra || diffHaciaSegSmbMasCercanoTmp<diffHaciaSegSmbMasCercano[0])) ? 0 : (cantEncontradas==2 && !guardarEn[1].precisaEnSombra && (interseccionTmp.precisaEnSombra || diffHaciaSegSmbMasCercanoTmp<diffHaciaSegSmbMasCercano[1]))? 1 : 2;
				if(iRegistrarTmp<2){
					diffHaciaSegSmbMasCercano[iRegistrarTmp]= diffHaciaSegSmbMasCercanoTmp;
					guardarEn[iRegistrarTmp]				= interseccionTmp;
					if(guardarEn[0].precisaEnSombra && guardarEn[1].precisaEnSombra){
						AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO_3(cantEncontradas) //La intersecciones son precisas, retornar (solo se continua si la interseccion actual es una aproximacion)
					}
				}
			}
		}
		vertSombSig = arrVertSmb[iVertSombAnt];
	}
	//Detectar interseccion con el segmento que inicia en AngMax
	NBLINEA_INSTERSECCION_RELATIVOS(relIntersSegSmbOrig, relIntersSegFigOrig, denom, vecAngMaxSombra.x, vecAngMaxSombra.y, vertSombSig.x, vertSombSig.y, iniSegFig.x, iniSegFig.y, finSegFig.x, finSegFig.y)
	if(denom!=0.0f && relIntersSegFigOrig>=0.0f && relIntersSegFigOrig<=1.0f){
		interseccionTmp.iVertSombra						= -1;
		interseccionTmp.avanceEnSegSombra				= relIntersSegSmbOrig;
		interseccionTmp.avanceEnSegFigura				= relIntersSegFigOrig;
		interseccionTmp.precisaEnSombra					= (interseccionTmp.avanceEnSegSombra>=0.0f && interseccionTmp.avanceEnSegSombra<=1.0f);
		diffHaciaSegSmbMasCercanoTmp					= 0.0f - relIntersSegSmbOrig; if(diffHaciaSegSmbMasCercanoTmp<0.0f) diffHaciaSegSmbMasCercanoTmp = relIntersSegSmbOrig - 1.0f; NBASSERT(interseccionTmp.precisaEnSombra || diffHaciaSegSmbMasCercanoTmp>0.0f)
		//
		if(cantEncontradas<2){
			diffHaciaSegSmbMasCercano[cantEncontradas]	= diffHaciaSegSmbMasCercanoTmp;
			guardarEn[cantEncontradas]					= interseccionTmp;
			cantEncontradas++;
		} else {
			iRegistrarTmp								= (cantEncontradas==1 && !guardarEn[0].precisaEnSombra && (interseccionTmp.precisaEnSombra || diffHaciaSegSmbMasCercanoTmp<diffHaciaSegSmbMasCercano[0])) ? 0 : (cantEncontradas==2 && !guardarEn[1].precisaEnSombra && (interseccionTmp.precisaEnSombra || diffHaciaSegSmbMasCercanoTmp<diffHaciaSegSmbMasCercano[1]))? 1 : 2;
			if(iRegistrarTmp<2){
				diffHaciaSegSmbMasCercano[iRegistrarTmp]= diffHaciaSegSmbMasCercanoTmp;
				guardarEn[iRegistrarTmp]				= interseccionTmp;
				if(guardarEn[0].precisaEnSombra && guardarEn[1].precisaEnSombra){
					AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO_3(cantEncontradas) //La intersecciones son precisas, retornar (solo se continua si la interseccion actual es una aproximacion)
				}
			}
		}
	}
	//Detectar interseccion con el segmentoAngMaxMax
	NBLINEA_INSTERSECCION_RELATIVOS(relIntersSegSmbOrig, relIntersSegFigOrig, denom, vecAngMaxSombra.x, vecAngMaxSombra.y, posFinVectorSmbAngMax.x, posFinVectorSmbAngMax.y, iniSegFig.x, iniSegFig.y, finSegFig.x, finSegFig.y)
	if(denom!=0.0f && relIntersSegFigOrig>=0.0f && relIntersSegFigOrig<=1.0f){
		interseccionTmp.iVertSombra						= -2;
		interseccionTmp.avanceEnSegSombra				= relIntersSegSmbOrig;
		interseccionTmp.avanceEnSegFigura				= relIntersSegFigOrig;
		interseccionTmp.precisaEnSombra					= (interseccionTmp.avanceEnSegSombra>=0.0f); //Proyectado al infinito
		diffHaciaSegSmbMasCercanoTmp					= 0.0f - relIntersSegSmbOrig; NBASSERT(interseccionTmp.precisaEnSombra || diffHaciaSegSmbMasCercanoTmp>0.0f)
		//
		if(cantEncontradas<2){
			diffHaciaSegSmbMasCercano[cantEncontradas]	= diffHaciaSegSmbMasCercanoTmp;
			guardarEn[cantEncontradas]					= interseccionTmp;
			cantEncontradas++;
		} else {
			iRegistrarTmp								= (cantEncontradas==1 && !guardarEn[0].precisaEnSombra && (interseccionTmp.precisaEnSombra || diffHaciaSegSmbMasCercanoTmp<diffHaciaSegSmbMasCercano[0])) ? 0 : (cantEncontradas==2 && !guardarEn[1].precisaEnSombra && (interseccionTmp.precisaEnSombra || diffHaciaSegSmbMasCercanoTmp<diffHaciaSegSmbMasCercano[1]))? 1 : 2;
			if(iRegistrarTmp<2){
				diffHaciaSegSmbMasCercano[iRegistrarTmp]= diffHaciaSegSmbMasCercanoTmp;
				guardarEn[iRegistrarTmp]				= interseccionTmp;
				if(guardarEn[0].precisaEnSombra && guardarEn[1].precisaEnSombra){
					AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO_3(cantEncontradas) //La intersecciones son precisas, retornar (solo se continua si la interseccion actual es una aproximacion)
				}
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return cantEncontradas;
}

void NBSegmentadorFiguras::extraerPorcionesSombrasAFigura(const NBFuenteSombra* arregloSombras, const UI16 conteoSombras, const NBPunto* bibVertsSombras, const UI16 tamBibVertsSombras){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("NBSegmentadorFiguras::extraerPorcionesSombrasAFigura")
	// --------------------------------------------------------
	// Importante:
	// - Se asume que la figuraActual es contrareloj (no necesariamente convexa)
	// - Se asume que las figuras sombras son contrareloj y convexas
	// - Se asume que todas las figuras sombras son proyecciones desde el origen de la luz.
	// - El vertice (0, 0) es el unico permitido repetirse.
	// Por ende:
	// - Para cada segmentoFiguraActual solo puede existir una interseccion con cualquier segmento sombra.
	// - Cada segmento sombra puede colision con mas de un segmentoFiguraActual.
	// --------------------------------------------------------
	NBASSERT(NBSegmentadorFiguras::figuraEsConvexaContrareloj(_figuras[_indiceFiguraActual]->elemento, _figuras[_indiceFiguraActual]->conteo))	//La entrada es convexa-contrareloj, el resultado es deforme
	NBASSERT(NBSegmentadorFiguras::figuraNoRepiteVertices(_figuras[_indiceFiguraActual]->elemento, _figuras[_indiceFiguraActual]->conteo))		//La entrada es convexa-contrareloj, el resultado es deforme
	#ifdef CONFIG_NB_SEGMENTADOR_FIGURAS_ESCRIBIR_LOG
	_dbgStr->vaciar();
	_dbgStr->agregarConFormato("\n----INICIO DE PROCESO DE FIGURA ----\n");
	#endif
	// --------------------------------------------------------
	// Recorrer cada figura sombra (figuraNueva = figuraActual - figuraSombra)
	// --------------------------------------------------------
	const UI16 cantSombras = conteoSombras; //if(cantSombras==0) PRINTF_INFO("Sombras: %d\n", cantSombras);
	UI16 iSombra;
	for(iSombra=0; iSombra<cantSombras; iSombra++){
		const NBFuenteSombra* datosSombra	= &arregloSombras[iSombra]; NBASSERT(datosSombra->conteoVerticesAdicionalSombra==0 || datosSombra->iPrimerVerticeAdicionalSombra+datosSombra->conteoVerticesAdicionalSombra<=tamBibVertsSombras)
		if(!datosSombra->sombraCubiertaPorOtra){ //Ignorar las sombras que son cubiertas por otras (ahorrando procedimiento inecesario)
			AUArregloNativoMutableP<NBPunto>* figuraActual	= _figuras[_indiceFiguraActual];			//Almacenar en dos arreglos distintos, simplifica el proceso y permite "revertir" resultados errados
			AUArregloNativoMutableP<NBPunto>* figuraNueva	= _figuras[_indiceFiguraActual==0?1:0];		//Almacenar en dos arreglos distintos, simplifica el proceso y permite "revertir" resultados errados
			const NBPunto* arrVerticesFig					= figuraActual->elemento;
			//const NBPunto* verticeFigSigUlt				= figuraActual->elemento + figuraActual->conteo;
			const UI16 conteoVerticesFig					= figuraActual->conteo;
			const NBPunto* arrVertSmb						= &bibVertsSombras[datosSombra->iPrimerVerticeAdicionalSombra];
			const NBPunto* vertSmbSigUlt					= &bibVertsSombras[datosSombra->iPrimerVerticeAdicionalSombra + datosSombra->conteoVerticesAdicionalSombra];
			const UI16 conteoVertSmb						= datosSombra->conteoVerticesAdicionalSombra;
			const NBPunto vectorSmbAngMin					= datosSombra->vectorAnguloMin;
			const NBPunto vectorSmbAngMax					= datosSombra->vectorAnguloMax;
			NBPunto posFinVectorSmbAngMin;					NBPUNTO_ESTABLECER(posFinVectorSmbAngMin, vectorSmbAngMin.x * 2.0f, vectorSmbAngMin.y * 2.0f);
			NBPunto posFinVectorSmbAngMax;					NBPUNTO_ESTABLECER(posFinVectorSmbAngMax, vectorSmbAngMax.x * 2.0f, vectorSmbAngMax.y * 2.0f);
			figuraNueva->vaciar(); NBASSERT(NBSegmentadorFiguras::figuraNoRepiteVertices(arrVerticesFig, conteoVerticesFig));
			#ifdef CONFIG_NB_SEGMENTADOR_FIGURAS_ESCRIBIR_LOG
			if(conteoVertSmb==0)	_dbgStr->agregarConFormato("\n----Sombra tipo DOS_VERTICES\n");
			else					_dbgStr->agregarConFormato("\n----Sombra tipo N%d_VERTICES_ADICIONALES\n", conteoVertSmb);
			#endif
			// --------------------------------------------------------
			// Identificar cualquier vertice que no este dentro de la sombra (debe existir alguno)
			// El proceso posterior se debe iniciar desde fuera de la sombra.
			// --------------------------------------------------------
			SI16 iPrimerVerticeFueraSombra;
			if(arrVertSmb==vertSmbSigUlt){
				iPrimerVerticeFueraSombra = NBSegmentadorFiguras::privIndiceVerticeCompletamenteFueraDeSombraDosVertices(arrVerticesFig, conteoVerticesFig, vectorSmbAngMin, vectorSmbAngMax);
			} else {
				iPrimerVerticeFueraSombra = NBSegmentadorFiguras::privIndiceVerticeCompletamenteFueraDeSombraMultiVertices(arrVerticesFig, conteoVerticesFig, vectorSmbAngMin, vectorSmbAngMax, arrVertSmb, vertSmbSigUlt);
			}
			NBASSERT(iPrimerVerticeFueraSombra!=-1)
			if(iPrimerVerticeFueraSombra!=-1){ //En teoria, esto nunca deberia de suceder (por seguridad es mejor dejar la condicional)
				#ifdef CONFIG_NB_SEGMENTADOR_FIGURAS_ESCRIBIR_LOG
				_dbgStr->agregarConFormato("FIGURA:\n");
				UI16 iV;
				for(iV=0; iV<conteoVerticesFig; iV++) _dbgStr->agregarConFormato("VertFig #%d (%.4f, %.4f)%s\n", iV, arrVerticesFig[iV].x, arrVerticesFig[iV].y, iV==iPrimerVerticeFueraSombra?" [PRIMER VERTICE FUERA DE SOMBRA]":"");
				_dbgStr->agregarConFormato("SOMBRA:\n");
				_dbgStr->agregarConFormato("AngMax (%.4f, %.4f)\n", vectorSmbAngMax.x, vectorSmbAngMax.y);
				for(iV=0; iV<conteoVertSmb; iV++) _dbgStr->agregarConFormato("Smb #%d (%.4f, %.4f)\n", iV, arrVertSmb[iV].x, arrVertSmb[iV].y);
				_dbgStr->agregarConFormato("AngMin (%.4f, %.4f)\n", vectorSmbAngMin.x, vectorSmbAngMin.y);
				_dbgStr->agregarConFormato("PROCESO:\n");
				#endif
				// --------------------------------------------------------
				// Recorrer segmento por segmento la figura (en sentido directo/antihorario).
				// En dependencia de la posicion de cada extremo de segmento:
				// Desde FUERA-de-sombra hacia DENTRO-de-sombra (o viceversa):
				//   - una interseccion esperada
				// Desde FUERA-de-sombra hacia FUERA-de-sombra:
				//   - cero o dos intersecciones esperadas
				// Desde DENTRO-de-sombra hacia DENTRO-de-sombra:
				//   - cero intersecciones esperadas.
				// --------------------------------------------------------
				bool fueraDeSombraAnt				= true;
				bool intersEstaDentroDeSombra		= false;
				SI16 iVerticeSmbEntradaSombra		= -1;
				SI16 iVertFigActual					= iPrimerVerticeFueraSombra;
				const SI16 iVertFigInicial			= iPrimerVerticeFueraSombra;
				//Variables para evitar vertices duplicados
				bool registradoPrimerVertice		= false;
				NBPunto primerVerticeAgregado;		NBPUNTO_ESTABLECER(primerVerticeAgregado, 0.0f, 0.0f)
				NBPunto ultimoVerticeAgregado;		NBPUNTO_ESTABLECER(ultimoVerticeAgregado, 0.0f, 0.0f)
				//PRINTF_INFO("\nProcesando figura\n");
				do {
					UI16 iVertFigAnterior			= iVertFigActual;
					iVertFigActual++;				if(iVertFigActual>=conteoVerticesFig) iVertFigActual -= conteoVerticesFig;
					UI16 iVertFigPostActual			= iVertFigActual + 1; if(iVertFigPostActual>=conteoVerticesFig) iVertFigPostActual -= conteoVerticesFig;
					const NBPunto vertFigAnterior	= arrVerticesFig[iVertFigAnterior];
					const NBPunto vertFigActual		= arrVerticesFig[iVertFigActual];
					#ifdef CONFIG_NB_SEGMENTADOR_FIGURAS_ESCRIBIR_LOG
					_dbgStr->agregarConFormato("Segmento figura de %d (%.4f, %.4f) hacia %d (%.4f, %.4f) distancia(%.4f)\n", iVertFigAnterior, arrVerticesFig[iVertFigAnterior].x, arrVerticesFig[iVertFigAnterior].y, iVertFigActual, arrVerticesFig[iVertFigActual].x, arrVerticesFig[iVertFigActual].y, NBPUNTO_DISTANCIA(arrVerticesFig[iVertFigAnterior].x, arrVerticesFig[iVertFigAnterior].y, arrVerticesFig[iVertFigActual].x, arrVerticesFig[iVertFigActual].y));
					#endif
					//
					bool fueraDeSombraSig;
					if(arrVertSmb==vertSmbSigUlt){
						fueraDeSombraSig			= NBSegmentadorFiguras::privVerticeEstaCompletamenteFueraDeSombraDosVertices(vertFigActual, vectorSmbAngMin, vectorSmbAngMax);
					} else {
						fueraDeSombraSig			= NBSegmentadorFiguras::privVerticeEstaCompletamenteFueraDeSombraMultiVertices(vertFigActual, vectorSmbAngMin, vectorSmbAngMax, arrVertSmb, vertSmbSigUlt);
					}
					//---------------------------------------------
					// -- Buscar las intersecciones con este segmento.
					// -- Se predice la cantidades posibles con base a si los vertices extremos del segmento estan dentro o fuera de la sombra.
					// -- Si no se encuentra una interseccion precisa, entonces se usa la aproximacion mas cercana a un segmentoSombra.
					//---------------------------------------------
					NBSegFigInterseccionSombra intersEncontradas[2];
					UI16 conteoIntersEncontradas	= 0;
					if(fueraDeSombraAnt!=fueraDeSombraSig){
						//Se espera encontrar UNA interseccion (desde FUERA-de-sombra hacia DENTRO-de-sombra (o viceversa))
						if(arrVertSmb==vertSmbSigUlt)	conteoIntersEncontradas	= NBSegmentadorFiguras::privBuscarUnaInterseccionConSombraDosVerticesOrdenInversoVsSegmento(intersEncontradas, vertFigAnterior, vertFigActual, vectorSmbAngMin, vectorSmbAngMax, posFinVectorSmbAngMin, posFinVectorSmbAngMax);
						else							conteoIntersEncontradas	= NBSegmentadorFiguras::privBuscarUnaInterseccionConSombraMultiVerticesOrdenInversoVsSegmento(intersEncontradas, vertFigAnterior, vertFigActual, vectorSmbAngMin, vectorSmbAngMax, posFinVectorSmbAngMin, posFinVectorSmbAngMax, arrVertSmb, conteoVertSmb);
						#ifdef CONFIG_NB_SEGMENTADOR_FIGURAS_ESCRIBIR_LOG
						_dbgStr->agregarConFormato("    1 - Atraviesa FRONTERA sombra (antes: %s despues: %s) interseccion(smb %d, %.9f, fig %.9f)\n", fueraDeSombraAnt?"FUERA":"DENTRO", fueraDeSombraSig?"FUERA":"DENTRO", intersEncontradas[0].iVertSombra, intersEncontradas[0].avanceEnSegSombra, intersEncontradas[0].avanceEnSegFigura);
						if(conteoIntersEncontradas!=1) PRINTF_INFO("\n%s\n", _dbgStr->str());
						#endif
						//NBASSERT(conteoIntersEncontradas==1)
						//NBASSERT(intersEncontradas[0].avanceEnSegSombra>=0.0f && (intersEncontradas[0].iVertSombra==-2 || intersEncontradas[0].iVertSombra==conteoVertSmb || intersEncontradas[0].avanceEnSegSombra<=1.0f) && intersEncontradas[0].avanceEnSegFigura>=0.0f && intersEncontradas[0].avanceEnSegFigura<=1.0f)
					} else if(fueraDeSombraAnt){
						//Se espera NINGUNA o DOS intersecciones (desde FUERA-de-sombra hacia FUERA-de-sombra)
						if(arrVertSmb==vertSmbSigUlt)	conteoIntersEncontradas = NBSegmentadorFiguras::privBuscarHastaDosInterseccionesConSombraDosVerticesOrdenInversoVsSegmento(intersEncontradas, vertFigAnterior, vertFigActual, vectorSmbAngMin, vectorSmbAngMax, posFinVectorSmbAngMin, posFinVectorSmbAngMax);
						else							conteoIntersEncontradas = NBSegmentadorFiguras::privBuscarHastaDosInterseccionesConSombraMultiVerticesOrdenInversoVsSegmento(intersEncontradas, vertFigAnterior, vertFigActual, vectorSmbAngMin, vectorSmbAngMax, posFinVectorSmbAngMin, posFinVectorSmbAngMax, arrVertSmb, conteoVertSmb);
						#ifdef CONFIG_NB_SEGMENTADOR_FIGURAS_ESCRIBIR_LOG
						UI16 cantOriginalEncontradas	= conteoIntersEncontradas;
						if(conteoIntersEncontradas==0) _dbgStr->agregarConFormato("    0 - Permanece FUERA DE SOMBRA SIN interseccion\n");
						else if(conteoIntersEncontradas==1) _dbgStr->agregarConFormato("    1 - Permanece FUERA DE SOMBRA con UNA interseccion (smb %d, %.9f, fig %.9f)\n", intersEncontradas[0].iVertSombra, intersEncontradas[0].avanceEnSegSombra, intersEncontradas[0].avanceEnSegFigura);
						else if(conteoIntersEncontradas==2) _dbgStr->agregarConFormato("    2 - Permanece FUERA DE SOMBRA con DOS interseccion (smb %d, %.9f, fig %.9f), interseccion(smb %d, %.9f, fig %.9f)\n", intersEncontradas[0].iVertSombra, intersEncontradas[0].avanceEnSegSombra, intersEncontradas[0].avanceEnSegFigura, intersEncontradas[1].iVertSombra, intersEncontradas[1].avanceEnSegSombra, intersEncontradas[1].avanceEnSegFigura);
						#endif
						UI16 conteoInterseccionesPrecisas = 0, conteoInterseccionesAproximadas = 0;
						if(conteoIntersEncontradas!=0){
							if(intersEncontradas[0].avanceEnSegSombra>=0.0f && (intersEncontradas[0].iVertSombra==-2 || intersEncontradas[0].iVertSombra==conteoVertSmb || intersEncontradas[0].avanceEnSegSombra<=1.0f) && intersEncontradas[0].avanceEnSegFigura>=0.0f && intersEncontradas[0].avanceEnSegFigura<=1.0f) conteoInterseccionesPrecisas++;
							else conteoInterseccionesAproximadas++;
						}
						if(conteoIntersEncontradas>0){
							if(intersEncontradas[1].avanceEnSegSombra>=0.0f && (intersEncontradas[1].iVertSombra==-2 || intersEncontradas[1].iVertSombra==conteoVertSmb || intersEncontradas[1].avanceEnSegSombra<=1.0f) && intersEncontradas[1].avanceEnSegFigura>=0.0f && intersEncontradas[1].avanceEnSegFigura<=1.0f) conteoInterseccionesPrecisas++;
							else conteoInterseccionesAproximadas++;
						}
						#ifdef CONFIG_NB_SEGMENTADOR_FIGURAS_ESCRIBIR_LOG
						if(conteoInterseccionesPrecisas!=0 && conteoInterseccionesPrecisas+conteoInterseccionesAproximadas!=2) PRINTF_INFO("\n%s\n", _dbgStr->str());
						#endif
						NBASSERT(conteoInterseccionesPrecisas==0 || conteoInterseccionesPrecisas+conteoInterseccionesAproximadas==2)
						conteoIntersEncontradas = (conteoInterseccionesPrecisas==2?2:0);
						#ifdef CONFIG_NB_SEGMENTADOR_FIGURAS_ESCRIBIR_LOG
						if(cantOriginalEncontradas!=0 && conteoIntersEncontradas==0) _dbgStr->agregarConFormato("    IGNORADA(s) xxxxxxxxx\n");
						#endif
						NBASSERT(conteoIntersEncontradas!=2 || intersEncontradas[0].iVertSombra>intersEncontradas[1].iVertSombra) //Si falla, las dos intersecciones encontradas no estan ordenadas en sentido horario (y deberian)
					}
					#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
					else {
						//Se espera NINGUNA interseccion (desde DENTRO-de-sombra hacia DENTRO-de-sombra)
						if(arrVertSmb==vertSmbSigUlt)	conteoIntersEncontradas		= NBSegmentadorFiguras::privBuscarHastaDosInterseccionesConSombraDosVerticesOrdenInversoVsSegmento(intersEncontradas, vertFigAnterior, vertFigActual, vectorSmbAngMin, vectorSmbAngMax, posFinVectorSmbAngMin, posFinVectorSmbAngMax);
						else							conteoIntersEncontradas		= NBSegmentadorFiguras::privBuscarHastaDosInterseccionesConSombraMultiVerticesOrdenInversoVsSegmento(intersEncontradas, vertFigAnterior, vertFigActual, vectorSmbAngMin, vectorSmbAngMax, posFinVectorSmbAngMin, posFinVectorSmbAngMax, arrVertSmb, conteoVertSmb);
						#ifdef CONFIG_NB_SEGMENTADOR_FIGURAS_ESCRIBIR_LOG
						if(conteoIntersEncontradas==0)		_dbgStr->agregarConFormato("    0 - Permanece DENTRO DE SOMBRA SIN intersecciones\n");
						else if(conteoIntersEncontradas==1)	_dbgStr->agregarConFormato("    1 - Permanece DENTRO DE SOMBRA con UNA interseccion (smb %d, %.9f, fig %.9f) [ADVERTENCIA, deberia ser cero]\n", intersEncontradas[0].iVertSombra, intersEncontradas[0].avanceEnSegSombra, intersEncontradas[0].avanceEnSegFigura);
						else if(conteoIntersEncontradas==2)	_dbgStr->agregarConFormato("    2 - Permanece DENTRO DE SOMBRA con DOS interseccion (smb %d, %.9f, fig %.9f), interseccion(smb %d, %.9f, fig %.9f) [ADVERTENCIA, deberia ser cero]\n", intersEncontradas[0].iVertSombra, intersEncontradas[0].avanceEnSegSombra, intersEncontradas[0].avanceEnSegFigura, intersEncontradas[1].iVertSombra, intersEncontradas[1].avanceEnSegSombra, intersEncontradas[1].avanceEnSegFigura);
						if(conteoIntersEncontradas!=0)		_dbgStr->agregarConFormato("    IGNORADA(s) xxxxxxxxx\n");
						#endif
						conteoIntersEncontradas = 0;
					}
					#endif
					NBASSERT(conteoIntersEncontradas!=2 || intersEncontradas[0].iVertSombra>intersEncontradas[1].iVertSombra) //Si falla, las dos intersecciones encontradas no estan ordenadas en sentido horario (y deberian)
					NBSegFigInterseccionSombra* interSig	= &intersEncontradas[0];
					NBSegFigInterseccionSombra* interSigUlt	= &intersEncontradas[conteoIntersEncontradas];
					NBPunto posInterseccion;
					while(interSig<interSigUlt){
						if(interSig->avanceEnSegFigura!=0.0f && interSig->avanceEnSegFigura!=1.0f){
							NBPUNTO_ESTABLECER(posInterseccion, vertFigAnterior.x + ((vertFigActual.x - vertFigAnterior.x) * interSig->avanceEnSegFigura), vertFigAnterior.y + ((vertFigActual.y - vertFigAnterior.y) * interSig->avanceEnSegFigura))
						} else if(interSig->avanceEnSegFigura==1.0f){
							posInterseccion = vertFigActual;
						} else {
							NBASSERT(interSig->avanceEnSegFigura==0.0f)
							posInterseccion = vertFigAnterior;
						}
						if(!intersEstaDentroDeSombra){
							//Entrando a area sombra
							//
							iVerticeSmbEntradaSombra		= interSig->iVertSombra;
							if(!registradoPrimerVertice || (posInterseccion!=primerVerticeAgregado && posInterseccion!=ultimoVerticeAgregado)){
								ultimoVerticeAgregado		= posInterseccion; if(!registradoPrimerVertice){ primerVerticeAgregado = ultimoVerticeAgregado; registradoPrimerVertice = true; }
								figuraNueva->agregarElemento(posInterseccion);
							}
						} else {
							// --------------------------------------------------------
							//Saliendo de area sombra
							// --------------------------------------------------------
							//Agregar los vertices de la sombra
							SI16 iVertSmb		= iVerticeSmbEntradaSombra;	//El vertice AngMin (no forma parte de los vertices intermedios)
							if(iVertSmb==conteoVertSmb){
								if(!registradoPrimerVertice || (vectorSmbAngMin!=primerVerticeAgregado && vectorSmbAngMin!=ultimoVerticeAgregado)){
									ultimoVerticeAgregado	= vectorSmbAngMin; if(!registradoPrimerVertice){ primerVerticeAgregado = ultimoVerticeAgregado; registradoPrimerVertice = true; }
									figuraNueva->agregarElemento(vectorSmbAngMin);
								}
								iVertSmb--;
							}
							SI16 iVertSmbAntUlt	= interSig->iVertSombra; if(iVertSmbAntUlt==-2) iVertSmbAntUlt = -1; //Los vertices intermedios
							for(; iVertSmb>iVertSmbAntUlt; iVertSmb--){
								if(!registradoPrimerVertice || (arrVertSmb[iVertSmb]!=primerVerticeAgregado && arrVertSmb[iVertSmb]!=ultimoVerticeAgregado)){
									ultimoVerticeAgregado	= arrVertSmb[iVertSmb]; if(!registradoPrimerVertice){ primerVerticeAgregado = ultimoVerticeAgregado; registradoPrimerVertice = true; }
									figuraNueva->agregarElemento(arrVertSmb[iVertSmb]);
								}
							}
							if(interSig->iVertSombra==-2){ //El vertice AngMax (no forma parte de los vertices intermedios)
								if(!registradoPrimerVertice || (vectorSmbAngMax!=primerVerticeAgregado && vectorSmbAngMax!=ultimoVerticeAgregado)){
									ultimoVerticeAgregado	= vectorSmbAngMax; if(!registradoPrimerVertice){ primerVerticeAgregado = ultimoVerticeAgregado; registradoPrimerVertice = true; }
									figuraNueva->agregarElemento(vectorSmbAngMax);
								}
								iVertSmb--;
							}
							//Agregar la nueva interseccion
							if(!registradoPrimerVertice || (posInterseccion!=primerVerticeAgregado && posInterseccion!=ultimoVerticeAgregado)){
								ultimoVerticeAgregado	= posInterseccion; if(!registradoPrimerVertice){ primerVerticeAgregado = ultimoVerticeAgregado; registradoPrimerVertice = true; }
								figuraNueva->agregarElemento(posInterseccion);
							}
						}
						intersEstaDentroDeSombra = !intersEstaDentroDeSombra;
						interSig++;
					}
					if(fueraDeSombraSig){
						if(!registradoPrimerVertice || (vertFigActual!=primerVerticeAgregado && vertFigActual!=ultimoVerticeAgregado)){
							ultimoVerticeAgregado = vertFigActual; if(!registradoPrimerVertice){ primerVerticeAgregado = ultimoVerticeAgregado; registradoPrimerVertice = true; }
							figuraNueva->agregarElemento(vertFigActual);
						}
					}
					fueraDeSombraAnt	= fueraDeSombraSig;
				} while(iVertFigActual!=iVertFigInicial);
				#ifdef CONFIG_NB_SEGMENTADOR_FIGURAS_ESCRIBIR_LOG
				if(intersEstaDentroDeSombra || figuraNueva->conteo<3){
					PRINTF_INFO("\n\nERROR procesando figura\n\n%s\n", _dbgStr->str());
				}
				#endif
				//NBASSERT(!intersEstaDentroDeSombra)
				//NBASSERT(figuraNueva->conteo>2)
				if(!intersEstaDentroDeSombra && figuraNueva->conteo>2){
					NBASSERT(NBSegmentadorFiguras::figuraNoRepiteVertices(figuraNueva->elemento, figuraNueva->conteo));
					_indiceFiguraActual = (_indiceFiguraActual==0?1:0); //Intercambiar figuras
				}
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

//
// Color en un punto
//

NBColor NBSegmentadorFiguras::calcularColoresAnteLucesConsiderandoSombras(NBFuenteIluminacionRender* fuentesLuz, AUArregloNativoMutableP<NBFuenteSombra>** sombrasPorFuenteLuz, const UI16 conteoIlum, const NBPunto* arrVerticesSombras, const AUEscenaObjeto* objAgrupadorSombras, const NBPunto &punto, const NBCajaAABB &cajaVerticesGL){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("NBSegmentadorFiguras::calcularColoresAnteLucesConsiderandoSombras")
	NBColor colorAcum; colorAcum.r = colorAcum.g = colorAcum.b = 0.0f; colorAcum.a = 1.0f;
	//Procesar con cada luz
	bool dentroDeFiguraSombra;
	float influencia, influenciaX, influenciaY, distanciaX, distanciaY;
	NBPunto vectorX, vectorY, vectorVerticeDesdeLuz, vectorDesdeLimiteForma;
	UI16 iSombra, conteoSombras;
	NBFuenteSombra* arrSombras;
	NBFuenteIluminacionRender* fuenteIlum;
	AUArregloNativoMutableP<NBFuenteSombra>* arregloSombras;
	UI16 iIlum;
	for(iIlum=0; iIlum<conteoIlum; iIlum++){
		fuenteIlum		= &fuentesLuz[iIlum]; NBASSERT(!(fuenteIlum->limitesFormaActivos && !fuenteIlum->limitesAngulosActivos))
		influencia		= 0.0f;
		if(!fuenteIlum->obstruida && fuenteIlum->cajaEscenaLuz.xMin!=fuenteIlum->cajaEscenaLuz.xMax && fuenteIlum->cajaEscenaLuz.yMin!=fuenteIlum->cajaEscenaLuz.yMax && NBCAJAAABB_INTERSECTA_CAJA(fuenteIlum->cajaEscenaLuz, cajaVerticesGL)){
			//Validar que esta dentro de la figura-caja-escena de la luz
			NBPUNTO_ESTABLECER(vectorVerticeDesdeLuz, punto.x - fuenteIlum->posicionEscenaLuz.x, punto.y - fuenteIlum->posicionEscenaLuz.y)
			NBPUNTO_ESTABLECER(vectorX, vectorVerticeDesdeLuz.x - fuenteIlum->posLocalVectorAvanceX.x, vectorVerticeDesdeLuz.y - fuenteIlum->posLocalVectorAvanceX.y);
			NBPUNTO_ESTABLECER(vectorY, vectorVerticeDesdeLuz.x - fuenteIlum->posLocalVectorAvanceY.x, vectorVerticeDesdeLuz.y - fuenteIlum->posLocalVectorAvanceY.y);
			distanciaX	= NBPUNTO_PRODUCTO_VECTORIAL(fuenteIlum->vecUnitAvanceX.x, fuenteIlum->vecUnitAvanceX.y, vectorX.x, vectorX.y);
			distanciaY	= NBPUNTO_PRODUCTO_VECTORIAL(fuenteIlum->vecUnitAvanceY.x, fuenteIlum->vecUnitAvanceY.y, vectorY.x, vectorY.y);
			if(distanciaX>0.0f && distanciaX<fuenteIlum->maxAvanceX && distanciaY>0.0f && distanciaY<fuenteIlum->maxAvanceY){
				bool dentroDelimitesForma = true, dentroDeLimitesAngulos = true;
				//Validar que esta a la izquierda de la forma limite de luz
				if(fuenteIlum->limitesFormaActivos){
					NBPUNTO_ESTABLECER(vectorDesdeLimiteForma, vectorVerticeDesdeLuz.x - fuenteIlum->posIniVecLimiteForma.x, vectorVerticeDesdeLuz.y - fuenteIlum->posIniVecLimiteForma.y)
					dentroDelimitesForma = NBPUNTO_PRODUCTO_VECTORIAL(fuenteIlum->vecUnitLimiteForma.x, fuenteIlum->vecUnitLimiteForma.y, vectorDesdeLimiteForma.x, vectorDesdeLimiteForma.y)<0.0f;
				}
				//Validar que esta entre los angulos limites de luz
				if(fuenteIlum->limitesAngulosActivos){
					dentroDeLimitesAngulos = (NBPUNTO_PRODUCTO_VECTORIAL(fuenteIlum->vecUnitAnguloMinDesdeLuz.x, fuenteIlum->vecUnitAnguloMinDesdeLuz.y, vectorVerticeDesdeLuz.x, vectorVerticeDesdeLuz.y)>0.0f
											  && NBPUNTO_PRODUCTO_VECTORIAL(fuenteIlum->vecUnitAnguloMaxDesdeLuz.x, fuenteIlum->vecUnitAnguloMaxDesdeLuz.y, vectorVerticeDesdeLuz.x, vectorVerticeDesdeLuz.y)<0.0f);
				}
				if(dentroDelimitesForma && dentroDeLimitesAngulos){
					//Validar que no es cubierto por una sombra
					dentroDeFiguraSombra = false;
					arregloSombras	= sombrasPorFuenteLuz[iIlum];
					arrSombras		= arregloSombras->elemento;
					conteoSombras	= arregloSombras->conteo; NBASSERT(conteoSombras<1024)
					if(conteoSombras!=0){
						for(iSombra=0; iSombra<conteoSombras && !dentroDeFiguraSombra; iSombra++){
							NBFuenteSombra* datosSombra			= &arrSombras[iSombra];
							//Validar que la sombra no esta cubierta por otra y que esta entre los angulos de la sombra
							if(datosSombra->sombraCubiertaPorOtra ||
							   NBPUNTO_PRODUCTO_VECTORIAL(datosSombra->vectorAnguloMin.x, datosSombra->vectorAnguloMin.y, vectorVerticeDesdeLuz.x, vectorVerticeDesdeLuz.y)<=0.0f ||
							   NBPUNTO_PRODUCTO_VECTORIAL(datosSombra->vectorAnguloMax.x, datosSombra->vectorAnguloMax.y, vectorVerticeDesdeLuz.x, vectorVerticeDesdeLuz.y)>=0.0f) continue;
							//Validar que esta a la izquierda los segmentos sombra
							NBPunto posAnterior, vectorSegSombra, vectorComparar;
							dentroDeFiguraSombra				= true;
							posAnterior							= datosSombra->vectorAnguloMax;
							if(datosSombra->conteoVerticesAdicionalSombra!=0){
								const NBPunto* puntosSombra			= &arrVerticesSombras[datosSombra->iPrimerVerticeAdicionalSombra];
								const UI16 conteoPuntosSombras		= datosSombra->conteoVerticesAdicionalSombra;
								UI16 iVertSomb;
								for(iVertSomb=0; iVertSomb<conteoPuntosSombras; iVertSomb++){
									NBPUNTO_ESTABLECER(vectorSegSombra, puntosSombra[iVertSomb].x - posAnterior.x, puntosSombra[iVertSomb].y - posAnterior.y)
									NBPUNTO_ESTABLECER(vectorComparar, vectorVerticeDesdeLuz.x - posAnterior.x, vectorVerticeDesdeLuz.y - posAnterior.y)
									if(NBPUNTO_PRODUCTO_VECTORIAL(vectorSegSombra.x, vectorSegSombra.y, vectorComparar.x, vectorComparar.y)<0.0f){
										dentroDeFiguraSombra		= false;
										break;
									}
									posAnterior = puntosSombra[iVertSomb];
								}
							}
							NBPUNTO_ESTABLECER(vectorSegSombra, datosSombra->vectorAnguloMin.x - posAnterior.x, datosSombra->vectorAnguloMin.y - posAnterior.y)
							NBPUNTO_ESTABLECER(vectorComparar, vectorVerticeDesdeLuz.x - posAnterior.x, vectorVerticeDesdeLuz.y - posAnterior.y)
							if(NBPUNTO_PRODUCTO_VECTORIAL(vectorSegSombra.x, vectorSegSombra.y, vectorComparar.x, vectorComparar.y)<0.0f){
								dentroDeFiguraSombra		= false;
							}
						}
					}
					if(!dentroDeFiguraSombra){
						//Calcular la influencia de la luz (asumiendo una forma ovala)
						influenciaX = ((distanciaX / fuenteIlum->maxAvanceX) - 0.5f) * 2.0f; NBASSERT(influenciaX>=-1.0f && influenciaX<=1.0f)
						influenciaY = ((distanciaY / fuenteIlum->maxAvanceY) - 0.5f) * 2.0f; NBASSERT(influenciaY>=-1.0f && influenciaY<=1.0f)
						influencia	= 1.0f - NBPUNTO_DISTANCIA_VECTOR(influenciaX, influenciaY);
						colorAcum.r += (fuenteIlum->colorLuzF.r * influencia);
						colorAcum.g += (fuenteIlum->colorLuzF.g * influencia);
						colorAcum.b += (fuenteIlum->colorLuzF.b * influencia);
					}
				}
			}
		}
		NBASSERT(colorAcum.a==1.0f)
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return colorAcum;
}

