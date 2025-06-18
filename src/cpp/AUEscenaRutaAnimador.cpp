//
//  AUEscenaRutaAnimador.cpp
//  Gameplay_iOS
//
//  Created by Marcos Ortega Morales on 27/09/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkMediaStdAfx.h"
#include "AUEscenaRutaAnimador.h"

AUEscenaRutaAnimador::AUEscenaRutaAnimador() : AUEscenaRuta() {
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaRutaAnimador::AUEscenaRutaAnimador")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaRutaAnimador")
	_objAnimandoEnRuta				= NULL;
	_posicionRelInicial				= 0.0f;
	_iniciarAnimandoRuta			= false;
	_estadoAnimacion.animandoRuta	= false;
	//
	_modoTickAnimacionRuta			= ENRutaAnimModoTick_Animando;
	_segsTomaDesaparecer			= 0.33f;
	_segsTomaReaparecer				= 0.33f;
	_segsAcumDesaparecer			= 0.0f;
	_segsAcumReaparecer				= 0.0f;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

AUEscenaRutaAnimador::AUEscenaRutaAnimador(AUEscenaRutaAnimador* otra) : AUEscenaRuta(otra) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaRutaAnimador::AUEscenaRutaAnimador")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaRutaAnimador")
	_objAnimandoEnRuta						= NULL;
	_posicionRelInicial						= otra->_posicionRelInicial;		NBASSERT(_posicionRelInicial>=0.0f && _posicionRelInicial<=1.0f)
	_iniciarAnimandoRuta					= otra->_iniciarAnimandoRuta;
	//
	_modoTickAnimacionRuta					= ENRutaAnimModoTick_Animando;
	_segsTomaDesaparecer					= 0.33f;
	_segsTomaReaparecer						= 0.33f;
	_segsAcumDesaparecer					= 0.0f;
	_segsAcumReaparecer						= 0.0f;
	//
	_estadoAnimacion.animandoRuta			= false;
	_estadoAnimacion.velocidadRecorreRuta	= otra->_estadoAnimacion.velocidadRecorreRuta;
	_estadoAnimacion.modoRutaAnimacion		= otra->_estadoAnimacion.modoRutaAnimacion;
	_estadoAnimacion.repetirRuta			= otra->_estadoAnimacion.repetirRuta;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

AUEscenaRutaAnimador::~AUEscenaRutaAnimador(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaRutaAnimador::~AUEscenaRutaAnimador")
	if(_objAnimandoEnRuta != NULL) _objAnimandoEnRuta->liberar(NB_RETENEDOR_THIS); _objAnimandoEnRuta = NULL;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaRutaAnimador::actualizarObjetosSegunCopia(AUEscenaRutaAnimador* objOriginal, AUArreglo* capaObjetosViejos, AUArreglo* capaObjetosNuevos){
	//Objeto animar
	AUEscenaObjeto* objCabezaOriginal = objOriginal->objAnimandoEnRuta();
	SI32 iObjetoOriginal = -1; if(objCabezaOriginal != NULL) iObjetoOriginal = capaObjetosViejos->indiceDe(objCabezaOriginal);
	if(iObjetoOriginal!=-1){
		NBASSERT(iObjetoOriginal>=0 && iObjetoOriginal<capaObjetosNuevos->conteo)
		NBASSERT(capaObjetosNuevos->elemento[iObjetoOriginal]->esClase(AUEscenaObjeto::idTipoClase))
		this->establecerObjAnimarEnRuta((AUEscenaObjeto*)capaObjetosNuevos->elemento[iObjetoOriginal]);
	}
}

bool AUEscenaRutaAnimador::animandoRuta(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaRutaAnimador::animandoRuta")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _estadoAnimacion.animandoRuta;
}

bool AUEscenaRutaAnimador::iniciarAnimandoRuta(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaRutaAnimador::iniciarAnimandoRuta")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _iniciarAnimandoRuta;
}

bool AUEscenaRutaAnimador::repetirRuta(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaRutaAnimador::repetirRuta")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _estadoAnimacion.repetirRuta;
}

float AUEscenaRutaAnimador::posicionRelativaInicial(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaRutaAnimador::posicionRelativaInicial")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _posicionRelInicial;
}

float AUEscenaRutaAnimador::velRecorreRuta(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaRutaAnimador::velRecorreRuta")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _estadoAnimacion.velocidadRecorreRuta;
}

ENRutaAnimModo AUEscenaRutaAnimador::modoRutaAnimacion(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaRutaAnimador::modoRutaAnimacion")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return (ENRutaAnimModo)_estadoAnimacion.modoRutaAnimacion;
}

AUEscenaObjeto* AUEscenaRutaAnimador::objAnimandoEnRuta(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaRutaAnimador::objAnimandoEnRuta")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _objAnimandoEnRuta;
}

void AUEscenaRutaAnimador::moverHaciaAvanceRelativo(float avanceRelativo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaRutaAnimador::moverHaciaAvanceRelativo")
	moverHaciaAvanceRelativo(avanceRelativo, _estadoAnimacion);
	//Animar posicion de objeto animado
	if(_objAnimandoEnRuta != NULL){
		NBMatriz matrizEscena		= this->matrizLocal(); //Asumiendo que la ruta y el objetoAnimar comparten el mismo contenedor
		NBPunto posEscena; NBMATRIZ_MULTIPLICAR_PUNTO(posEscena, matrizEscena,_estadoAnimacion.posLocalEnAnimacion.x, _estadoAnimacion.posLocalEnAnimacion.y)
		_objAnimandoEnRuta->establecerTraslacion(posEscena);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaRutaAnimador::moverHaciaAvanceRelativo(float avanceRelativo, STRutaAnimEstado &estadoAnim){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaRutaAnimador::moverHaciaAvanceRelativo")
	NBASSERT(avanceRelativo>=0.0f && avanceRelativo<=1.0f)
	UI16 conteoVerts = _verticesRuta.conteo;
	if(conteoVerts>1){
		float velocidadRecorreRutaAbs	= estadoAnim.velocidadRecorreRuta; if(velocidadRecorreRutaAbs<0.0f) velocidadRecorreRutaAbs = -velocidadRecorreRutaAbs;
		if(avanceRelativo==0.0f){
			estadoAnim.indiceSegmentoActual		= 0;
			estadoAnim.avanceSobreSementoActual	= 0.0f;
			estadoAnim.avanceSobreRutaTotal		= 0.0f;
			estadoAnim.segsAcumEnSegmentoActual	= 0.0f;
			estadoAnim.segsDuraSegmentoActual	= this->privInlineSegundosDuracionSegmento(_verticesRuta.elemento[0].longitudSegmento, velocidadRecorreRutaAbs*_verticesRuta.elemento[0].factorVelocidad, velocidadRecorreRutaAbs*_verticesRuta.elemento[1].factorVelocidad);
			estadoAnim.segsEsperaPendientes		= _verticesRuta.elemento[estadoAnim.indiceSegmentoActual].segundossEspera;
			NBPUNTO_ESTABLECER(estadoAnim.posLocalEnAnimacion, _verticesRuta.elemento[0].posicion.x, _verticesRuta.elemento[0].posicion.y);
		} else if(avanceRelativo==1.0f){
			estadoAnim.indiceSegmentoActual		= _verticesRuta.conteo-2;
			estadoAnim.avanceSobreSementoActual	= _verticesRuta.elemento[estadoAnim.indiceSegmentoActual].longitudSegmento;
			estadoAnim.avanceSobreRutaTotal		= _longitudTotalRecorrido;
			estadoAnim.segsDuraSegmentoActual	= this->privInlineSegundosDuracionSegmento(_verticesRuta.elemento[estadoAnim.indiceSegmentoActual].longitudSegmento, velocidadRecorreRutaAbs*_verticesRuta.elemento[estadoAnim.indiceSegmentoActual].factorVelocidad, velocidadRecorreRutaAbs*_verticesRuta.elemento[estadoAnim.indiceSegmentoActual+1].factorVelocidad);
			estadoAnim.segsAcumEnSegmentoActual	= estadoAnim.segsDuraSegmentoActual;
			estadoAnim.segsEsperaPendientes		= _verticesRuta.elemento[estadoAnim.indiceSegmentoActual+1].segundossEspera;
			NBPUNTO_ESTABLECER(estadoAnim.posLocalEnAnimacion, _verticesRuta.elemento[estadoAnim.indiceSegmentoActual+1].posicion.x, _verticesRuta.elemento[estadoAnim.indiceSegmentoActual+1].posicion.y);
		} else {
			// Formulas de la cinematica
			// Movimiento rectilineo uniforme
			//	x1 = x0 + v * t;
			// Movimiento rectilineo uniformemente acelerado
			//	x1 = x0 + (v0 * t) + (1/2 * a * t * t)
			//	v1 = v0 + a * t
			//	v1 * v1 = v0 * v0 + 2 * a * (x1 - x0)
			float avanceFaltante = _longitudTotalRecorrido * avanceRelativo;
			estadoAnim.avanceSobreRutaTotal		= 0.0f;
			conteoVerts--; //El ultimo vertice no se toma en cuenta
			for(estadoAnim.indiceSegmentoActual=0; estadoAnim.indiceSegmentoActual<conteoVerts; estadoAnim.indiceSegmentoActual++){
				STRutaVertice* vIni = &_verticesRuta.elemento[estadoAnim.indiceSegmentoActual];
				if(vIni->longitudSegmento<avanceFaltante){
					avanceFaltante		-= vIni->longitudSegmento;
					estadoAnim.avanceSobreRutaTotal += vIni->longitudSegmento;
				} else {
					break;
				}
			}
			if(estadoAnim.indiceSegmentoActual==conteoVerts) estadoAnim.indiceSegmentoActual = conteoVerts - 1;
			STRutaVertice* vIni	= &_verticesRuta.elemento[estadoAnim.indiceSegmentoActual];
			STRutaVertice* vFin	= &_verticesRuta.elemento[estadoAnim.indiceSegmentoActual+1];
			float avanceRelSobreSegmento			= avanceFaltante / vIni->longitudSegmento; if(avanceRelSobreSegmento>1.0f) avanceRelSobreSegmento = 1.0f;
			estadoAnim.avanceSobreSementoActual		= avanceFaltante;
			estadoAnim.avanceSobreRutaTotal			+= avanceFaltante;
			estadoAnim.segsDuraSegmentoActual		= this->privInlineSegundosDuracionSegmento(vIni->longitudSegmento, velocidadRecorreRutaAbs*vIni->factorVelocidad, velocidadRecorreRutaAbs*vFin->factorVelocidad);
			estadoAnim.segsAcumEnSegmentoActual		= estadoAnim.segsDuraSegmentoActual * avanceRelSobreSegmento;
			estadoAnim.segsEsperaPendientes			= 0.0f;
			float avanceEnTiempoAcum				= this->privInlineAvanceEnSegmentoSegunTiempo(vIni->longitudSegmento, velocidadRecorreRutaAbs*vIni->factorVelocidad, velocidadRecorreRutaAbs*vFin->factorVelocidad, estadoAnim.segsAcumEnSegmentoActual);
			float avanceRelSegunTiempo				= avanceEnTiempoAcum / vIni->longitudSegmento;
			NBPUNTO_ESTABLECER(estadoAnim.posLocalEnAnimacion, vIni->posicion.x+((vFin->posicion.x-vIni->posicion.x)*avanceRelSegunTiempo), vIni->posicion.y+((vFin->posicion.y-vIni->posicion.y)*avanceRelSegunTiempo));
		}
		/*} else if(_tipoRuta==ENRutaTipo_Circulo){
			NBPunto centroCircun			= _verticesRuta.elemento[0].posicion;
			float radio						= _verticesRuta.elemento[1].longitudSegmento;
			float radianesMin				= _verticesRuta.elemento[1].radianesSegmento;
			float radianesMax				= radianesMin + PIx2; if(conteoVerts>2) radianesMax = _verticesRuta.elemento[2].radianesSegmento; if(radianesMin>radianesMax) radianesMin -= PIx2;
			float proprocionCircunferencia	= (radianesMax - radianesMin) / PIx2;
			float radianesFinal				= radianesMin + (proprocionCircunferencia * PIx2 * avanceRelativo);
			float perimetroCircunferencia	= (PIx2 * radio);
			_indiceSegmentoActual			= 0;
			_segsDuraSegmentoActual			= (perimetroCircunferencia * proprocionCircunferencia) / _velocidadRecorreRuta; if(_segsDuraSegmentoActual<0.0f) _segsDuraSegmentoActual = -_segsDuraSegmentoActual;
			_segsAcumEnSegmentoActual		= _segsDuraSegmentoActual * avanceRelativo;
			//
			NBPunto posFinal; NBPUNTO_OBTENER_VECTOR_ROTADO(posFinal, radio, radianesFinal);
			NBPUNTO_ESTABLECER(_posLocalEnAnimacion, posFinal.x+centroCircun.x, posFinal.y+centroCircun.y);
		} */
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaRutaAnimador::establecerPosicionRelativaInicial(float posicionRelativaInicial){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaRutaAnimador::establecerSegsParaDestruir")
	NBASSERT(posicionRelativaInicial>=0.0f && posicionRelativaInicial<=1.0f)
	_posicionRelInicial = posicionRelativaInicial;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaRutaAnimador::establecerVelRecorreRuta(float velRecorreRuta){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaRutaAnimador::establecerSegsParaRenovar")
	_estadoAnimacion.velocidadRecorreRuta = velRecorreRuta;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaRutaAnimador::establecerModoRutaAnimacion(ENRutaAnimModo modoRutaAnimacion){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaRutaAnimador::establecerModoRutaAnimacion")
	NBASSERT(modoRutaAnimacion>=0 && modoRutaAnimacion<ENRutaAnimModo_Conteo)
	_estadoAnimacion.modoRutaAnimacion = modoRutaAnimacion;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaRutaAnimador::reanudarAnimacionRuta(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaRutaAnimador::reanudarAnimacionRuta")
	_estadoAnimacion.animandoRuta = true;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaRutaAnimador::detenerAnimacionRuta(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaRutaAnimador::detenerAnimacionRuta")
	_estadoAnimacion.animandoRuta = false;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaRutaAnimador::establecerIniciarAnimandoRuta(bool iniciarAnimandoRuta){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaRutaAnimador::establecerIniciarAnimandoRuta")
	_iniciarAnimandoRuta = iniciarAnimandoRuta;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaRutaAnimador::establecerRepetirRuta(bool repetirRuta){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaRutaAnimador::establecerRepetirRuta")
	_estadoAnimacion.repetirRuta = repetirRuta;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaRutaAnimador::establecerObjAnimarEnRuta(AUEscenaObjeto* objAnimarEnRuta){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaRutaAnimador::establecerObjAnimarEnRuta")
	if(objAnimarEnRuta != NULL) objAnimarEnRuta->retener(NB_RETENEDOR_THIS);
	if(_objAnimandoEnRuta != NULL) _objAnimandoEnRuta->liberar(NB_RETENEDOR_THIS);
	_objAnimandoEnRuta = objAnimarEnRuta;
	this->moverHaciaAvanceRelativo(this->posicionRelativaInicial());
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaRutaAnimador::agregadoEnEscena(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaRutaAnimador::agregadoEnEscena")
	this->moverHaciaAvanceRelativo(_posicionRelInicial);
	NBGestorAnimadores::agregarAnimador(this, this);
	NBASSERT(debugVeritificarCacheDeRuta())
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaRutaAnimador::agregadoEnEscenaConHijosNotificados(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaRutaAnimador::agregadoEnEscenaConHijosNotificados")
	NBASSERT(debugVeritificarCacheDeRuta())
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaRutaAnimador::quitandoDeEscena(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaRutaAnimador::quitandoDeEscena")
	NBGestorAnimadores::quitarAnimador(this);
	NBASSERT(debugVeritificarCacheDeRuta())
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaRutaAnimador::tickAnimacionRuta(float segsTranscurridos, STRutaAnimEstado &estadoAnim){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaRutaAnimador::tickAnimacion")
	// Formulas de la cinematica
	// Movimiento rectilineo uniforme
	//	x1 = x0 + v * t;
	// Movimiento rectilineo uniformemente acelerado
	//	x1 = x0 + (v0 * t) + (1/2 * a * t * t)
	//	v1 = v0 + a * t
	//	v1 * v1 = v0 * v0 + 2 * a * (x1 - x0)
	//
	if(_verticesRuta.conteo>1){
		float segundosAcum					= segsTranscurridos;
		float velocidadAbs					= estadoAnim.velocidadRecorreRuta; if(velocidadAbs<0.0f) velocidadAbs = -velocidadAbs;
		while(estadoAnim.animandoRuta && segundosAcum!=0.0f){
			if(estadoAnim.segsEsperaPendientes!=0.0f){
				//
				//CONSUMIR ESPERA
				//
				NBASSERT(estadoAnim.segsEsperaPendientes>0.0f)
				if(segundosAcum<estadoAnim.segsEsperaPendientes){
					estadoAnim.segsEsperaPendientes	-= segundosAcum;
					segundosAcum = 0.0f;
				} else {
					segundosAcum			-= estadoAnim.segsEsperaPendientes;
					estadoAnim.segsEsperaPendientes	= 0.0f;
					if(estadoAnim.modoRutaAnimacion==ENRutaAnimModo_Ida){
						float velocidadRuta				= estadoAnim.velocidadRecorreRuta; if((estadoAnim.mascPropsAnimacion & AUESCENARUTAANIMADOR_BIT_ANIMANDO_ORDEN_INVERSO)!=0) velocidadRuta = -velocidadRuta;
						if((velocidadRuta<0.0f && estadoAnim.indiceSegmentoActual==0 && estadoAnim.avanceSobreSementoActual==0.0f) || (velocidadRuta>=0.0f && estadoAnim.indiceSegmentoActual==_verticesRuta.conteo-2 && estadoAnim.avanceSobreSementoActual!=0.0f)){
							estadoAnim.animandoRuta		= estadoAnim.repetirRuta;
							if(estadoAnim.repetirRuta){
								this->moverHaciaAvanceRelativo(velocidadRuta<0.0f?1.0f:0.0f, estadoAnim);
								estadoAnim.consumirMovimientoBrusco = true;
							}
						}
					}
				}
			} else {
				if((estadoAnim.mascPropsAnimacion & AUESCENARUTAANIMADOR_BIT_ANIMANDO_SEGMNT_CICLO)!=0){
					//
					//ANIMAR SEGMENTO DE CICLO (INI A FIN o veceversa)
					//
					float faltanteParaDestino = estadoAnim.cicloLongitudTotal - estadoAnim.cicloLongitudRecorrida;
					if(faltanteParaDestino<64.0f){
						estadoAnim.cicloVelActual += (estadoAnim.cicloVelFinal - velocidadAbs) * segundosAcum;
						if(estadoAnim.cicloVelActual<(0.10f * velocidadAbs)) estadoAnim.cicloVelActual = (0.10f * velocidadAbs);
					} else {
						estadoAnim.cicloVelActual += (velocidadAbs - estadoAnim.cicloVelActual) * segundosAcum;
						if(estadoAnim.cicloVelActual>velocidadAbs) estadoAnim.cicloVelActual = velocidadAbs;
					}
					estadoAnim.cicloLongitudRecorrida	+= estadoAnim.cicloVelActual * segundosAcum;
					if(estadoAnim.cicloLongitudRecorrida>=estadoAnim.cicloLongitudTotal){
						estadoAnim.cicloLongitudRecorrida		= estadoAnim.cicloLongitudTotal;
						estadoAnim.mascPropsAnimacion			&= ~AUESCENARUTAANIMADOR_BIT_ANIMANDO_SEGMNT_CICLO;
						if((estadoAnim.mascPropsAnimacion & AUESCENARUTAANIMADOR_BIT_ANIMANDO_CICLO_HACIA_FIN)!=0){
							estadoAnim.indiceSegmentoActual		= _verticesRuta.conteo-2;
							estadoAnim.avanceSobreSementoActual	= _verticesRuta.elemento[estadoAnim.indiceSegmentoActual].longitudSegmento;
							estadoAnim.avanceSobreRutaTotal		= _longitudTotalRecorrido;
							estadoAnim.segsEsperaPendientes		= _verticesRuta.elemento[_verticesRuta.conteo-1].segundossEspera;
							estadoAnim.segsDuraSegmentoActual	= this->privInlineSegundosDuracionSegmento(_verticesRuta.elemento[estadoAnim.indiceSegmentoActual].longitudSegmento, velocidadAbs*_verticesRuta.elemento[estadoAnim.indiceSegmentoActual].factorVelocidad, velocidadAbs*_verticesRuta.elemento[estadoAnim.indiceSegmentoActual+1].factorVelocidad);
							estadoAnim.segsAcumEnSegmentoActual	= estadoAnim.segsDuraSegmentoActual;
						} else {
							estadoAnim.indiceSegmentoActual		= 0;
							estadoAnim.avanceSobreSementoActual	= 0.0f;
							estadoAnim.avanceSobreRutaTotal		= 0.0f;
							estadoAnim.segsEsperaPendientes		= _verticesRuta.elemento[estadoAnim.indiceSegmentoActual].segundossEspera;
							estadoAnim.segsDuraSegmentoActual	= this->privInlineSegundosDuracionSegmento(_verticesRuta.elemento[estadoAnim.indiceSegmentoActual].longitudSegmento, velocidadAbs*_verticesRuta.elemento[estadoAnim.indiceSegmentoActual].factorVelocidad, velocidadAbs*_verticesRuta.elemento[estadoAnim.indiceSegmentoActual+1].factorVelocidad);
							estadoAnim.segsAcumEnSegmentoActual	= 0.0f;
						}
					}
					segundosAcum = 0.0f;
					//Actualizar posicion de objeto
					NBPUNTO_ESTABLECER(estadoAnim.posLocalEnAnimacion, estadoAnim.cicloPosIni.x+(estadoAnim.cicloVectorUnit.x*estadoAnim.cicloLongitudRecorrida), estadoAnim.cicloPosIni.y+(estadoAnim.cicloVectorUnit.y*estadoAnim.cicloLongitudRecorrida));
				} else {
					//
					//ANIMAR DENTRO DE RUTA
					//
					float velocidadRuta				= estadoAnim.velocidadRecorreRuta; if((estadoAnim.mascPropsAnimacion & AUESCENARUTAANIMADOR_BIT_ANIMANDO_ORDEN_INVERSO)!=0) velocidadRuta = -velocidadRuta;
					if(velocidadRuta<0.0f){
						//Recorrer en orden inverso
						if(segundosAcum<estadoAnim.segsAcumEnSegmentoActual){
							estadoAnim.segsAcumEnSegmentoActual -= segundosAcum;
							segundosAcum = 0.0f;
						} else if(estadoAnim.indiceSegmentoActual==0){
							if(estadoAnim.modoRutaAnimacion==ENRutaAnimModo_Ida){
								estadoAnim.segsEsperaPendientes		= _verticesRuta.elemento[estadoAnim.indiceSegmentoActual].segundossEspera;
								estadoAnim.avanceSobreSementoActual	= 0.0f;
								estadoAnim.segsAcumEnSegmentoActual	= 0.0f;
								if(estadoAnim.segsEsperaPendientes==0.0f){
									segundosAcum						= 0.0f;
									estadoAnim.animandoRuta				= estadoAnim.repetirRuta;
									if(estadoAnim.repetirRuta){
										this->moverHaciaAvanceRelativo(1.0f, estadoAnim);
										estadoAnim.consumirMovimientoBrusco = true;
									}
								}
							} else if(estadoAnim.modoRutaAnimacion==ENRutaAnimModo_Ciclo){
								estadoAnim.segsEsperaPendientes		= _verticesRuta.elemento[estadoAnim.indiceSegmentoActual].segundossEspera;
								estadoAnim.mascPropsAnimacion		|= AUESCENARUTAANIMADOR_BIT_ANIMANDO_SEGMNT_CICLO;
								estadoAnim.mascPropsAnimacion		|= AUESCENARUTAANIMADOR_BIT_ANIMANDO_CICLO_HACIA_FIN;
								estadoAnim.cicloPosIni				= _verticesRuta.elemento[estadoAnim.indiceSegmentoActual].posicion;
								NBPunto cicloPosFinal				= _verticesRuta.elemento[_verticesRuta.conteo-1].posicion;
								NBPUNTO_ESTABLECER(estadoAnim.cicloVectorUnit, cicloPosFinal.x-estadoAnim.cicloPosIni.x, cicloPosFinal.y-estadoAnim.cicloPosIni.y)
								estadoAnim.cicloLongitudTotal		= NBPUNTO_DISTANCIA_VECTOR(estadoAnim.cicloVectorUnit.x, estadoAnim.cicloVectorUnit.y);
								estadoAnim.cicloVectorUnit.x		/= estadoAnim.cicloLongitudTotal;
								estadoAnim.cicloVectorUnit.y		/= estadoAnim.cicloLongitudTotal;
								estadoAnim.cicloLongitudRecorrida	= 0.0f;
								estadoAnim.cicloVelActual			= velocidadAbs * _verticesRuta.elemento[estadoAnim.indiceSegmentoActual].factorVelocidad;
								estadoAnim.cicloVelFinal			= velocidadAbs * _verticesRuta.elemento[_verticesRuta.conteo-1].factorVelocidad;
							} else { //ENRutaAnimModo_IdaVuelta
								estadoAnim.segsEsperaPendientes		= _verticesRuta.elemento[estadoAnim.indiceSegmentoActual].segundossEspera;
								AUESCENARUTAANIMADOR_INVERTIR_BIT_EN_MASCARA(estadoAnim.mascPropsAnimacion, AUESCENARUTAANIMADOR_BIT_ANIMANDO_ORDEN_INVERSO)
								segundosAcum = 0.0f;
							}
						} else {
							estadoAnim.indiceSegmentoActual--;
							segundosAcum						-= estadoAnim.segsAcumEnSegmentoActual;
							estadoAnim.segsEsperaPendientes		= _verticesRuta.elemento[estadoAnim.indiceSegmentoActual+1].segundossEspera;
							estadoAnim.segsDuraSegmentoActual	= this->privInlineSegundosDuracionSegmento(_verticesRuta.elemento[estadoAnim.indiceSegmentoActual].longitudSegmento, velocidadAbs*_verticesRuta.elemento[estadoAnim.indiceSegmentoActual].factorVelocidad, velocidadAbs*_verticesRuta.elemento[estadoAnim.indiceSegmentoActual+1].factorVelocidad);
							estadoAnim.segsAcumEnSegmentoActual	= estadoAnim.segsDuraSegmentoActual;
						}
					} else {
						//Recorrer en orden directo
						float segsFaltanParaFinal = estadoAnim.segsDuraSegmentoActual - estadoAnim.segsAcumEnSegmentoActual;
						if(segundosAcum<segsFaltanParaFinal){
							estadoAnim.segsAcumEnSegmentoActual += segundosAcum;
							segundosAcum = 0.0f;
						} else if(estadoAnim.indiceSegmentoActual==_verticesRuta.conteo-2){
							if(estadoAnim.modoRutaAnimacion==ENRutaAnimModo_Ida){
								estadoAnim.segsEsperaPendientes		= _verticesRuta.elemento[estadoAnim.indiceSegmentoActual+1].segundossEspera;
								estadoAnim.avanceSobreSementoActual	= _verticesRuta.elemento[estadoAnim.indiceSegmentoActual].longitudSegmento;
								estadoAnim.segsAcumEnSegmentoActual	= estadoAnim.segsDuraSegmentoActual;
								if(estadoAnim.segsEsperaPendientes==0.0f){
									segundosAcum						= 0.0f;
									estadoAnim.animandoRuta				= estadoAnim.repetirRuta;
									if(estadoAnim.repetirRuta){
										this->moverHaciaAvanceRelativo(0.0f, estadoAnim);
										estadoAnim.consumirMovimientoBrusco = true;
									}
								}
							} else if(estadoAnim.modoRutaAnimacion==ENRutaAnimModo_Ciclo){
								estadoAnim.segsEsperaPendientes		= _verticesRuta.elemento[estadoAnim.indiceSegmentoActual+1].segundossEspera;
								estadoAnim.mascPropsAnimacion		|= AUESCENARUTAANIMADOR_BIT_ANIMANDO_SEGMNT_CICLO;
								estadoAnim.mascPropsAnimacion		&= ~AUESCENARUTAANIMADOR_BIT_ANIMANDO_CICLO_HACIA_FIN;
								estadoAnim.cicloPosIni				= _verticesRuta.elemento[_verticesRuta.conteo-1].posicion;
								NBPunto cicloPosFinal				= _verticesRuta.elemento[0].posicion;
								NBPUNTO_ESTABLECER(estadoAnim.cicloVectorUnit, cicloPosFinal.x-estadoAnim.cicloPosIni.x, cicloPosFinal.y-estadoAnim.cicloPosIni.y)
								estadoAnim.cicloLongitudTotal			= NBPUNTO_DISTANCIA_VECTOR(estadoAnim.cicloVectorUnit.x, estadoAnim.cicloVectorUnit.y);
								estadoAnim.cicloVectorUnit.x			/= estadoAnim.cicloLongitudTotal;
								estadoAnim.cicloVectorUnit.y			/= estadoAnim.cicloLongitudTotal;
								estadoAnim.cicloLongitudRecorrida		= 0.0f;
								estadoAnim.cicloVelActual				= velocidadAbs * _verticesRuta.elemento[_verticesRuta.conteo-1].factorVelocidad;
								estadoAnim.cicloVelFinal				= velocidadAbs * _verticesRuta.elemento[0].factorVelocidad;
							} else { //ENRutaAnimModo_IdaVuelta
								estadoAnim.segsEsperaPendientes		= _verticesRuta.elemento[estadoAnim.indiceSegmentoActual+1].segundossEspera;
								AUESCENARUTAANIMADOR_INVERTIR_BIT_EN_MASCARA(estadoAnim.mascPropsAnimacion, AUESCENARUTAANIMADOR_BIT_ANIMANDO_ORDEN_INVERSO)
								segundosAcum = 0.0f;
							}
						} else {
							estadoAnim.indiceSegmentoActual++;
							segundosAcum							-= segsFaltanParaFinal;
							estadoAnim.segsEsperaPendientes			= _verticesRuta.elemento[estadoAnim.indiceSegmentoActual].segundossEspera;
							estadoAnim.segsDuraSegmentoActual		= this->privInlineSegundosDuracionSegmento(_verticesRuta.elemento[estadoAnim.indiceSegmentoActual].longitudSegmento, velocidadAbs*_verticesRuta.elemento[estadoAnim.indiceSegmentoActual].factorVelocidad, velocidadAbs*_verticesRuta.elemento[estadoAnim.indiceSegmentoActual+1].factorVelocidad);
							estadoAnim.segsAcumEnSegmentoActual		= 0.0f;
						}
					}
					//Actualizar posicion de objeto
					STRutaVertice* vIni				= &_verticesRuta.elemento[estadoAnim.indiceSegmentoActual];
					STRutaVertice* vFin				= &_verticesRuta.elemento[estadoAnim.indiceSegmentoActual+1];
					float avanceEnTiempoAcum		= this->privInlineAvanceEnSegmentoSegunTiempo(vIni->longitudSegmento, velocidadAbs*vIni->factorVelocidad, velocidadAbs*vFin->factorVelocidad, estadoAnim.segsAcumEnSegmentoActual);
					float avanceRelSegunTiempo		= avanceEnTiempoAcum / vIni->longitudSegmento;
					estadoAnim.avanceSobreSementoActual = avanceEnTiempoAcum;
					estadoAnim.avanceSobreRutaTotal	= _verticesRuta.elemento[estadoAnim.indiceSegmentoActual].longitudSegmentosAnteriores + avanceEnTiempoAcum;
					NBPUNTO_ESTABLECER(estadoAnim.posLocalEnAnimacion, vIni->posicion.x+((vFin->posicion.x-vIni->posicion.x)*avanceRelSegunTiempo), vIni->posicion.y+((vFin->posicion.y-vIni->posicion.y)*avanceRelSegunTiempo));
				}
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaRutaAnimador::tickAnimacion(float segsTranscurridos){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaRutaAnimador::tickAnimacion")
	if(this->_dormido){ AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO_3() }
	NBMatriz matrizEscena		= this->matrizLocal(); //Asumiendo que la ruta y el objetoAnimar comparten el mismo contenedor
	NBPunto posEscena;			NBMATRIZ_MULTIPLICAR_PUNTO(posEscena, matrizEscena,_estadoAnimacion.posLocalEnAnimacion.x, _estadoAnimacion.posLocalEnAnimacion.y)
	switch (_modoTickAnimacionRuta) {
		case ENRutaAnimModoTick_Animando:
			//ANIMAR SOBRE RUTA
			tickAnimacionRuta(segsTranscurridos, _estadoAnimacion);
			if(_estadoAnimacion.consumirMovimientoBrusco){
				_modoTickAnimacionRuta						= ENRutaAnimModoTick_Desapareciendo;
				_segsAcumDesaparecer						= 0.0f;
				_estadoAnimacion.consumirMovimientoBrusco	= false;
			} else if(_objAnimandoEnRuta != NULL) {
				_objAnimandoEnRuta->establecerTraslacion(posEscena);
			}
			break;
		case ENRutaAnimModoTick_Desapareciendo:
			//ANIMAR DESPARAICION DE OBJETO
			_segsAcumDesaparecer += segsTranscurridos;
			if(_objAnimandoEnRuta != NULL){
				if(_segsAcumDesaparecer<_segsTomaDesaparecer){
					_objAnimandoEnRuta->establecerMultiplicadorAlpha8(255.0f * (1.0f - (_segsAcumDesaparecer / _segsTomaDesaparecer)));
				} else {
					AUEscenaContenedor* contenedorAgregar	= (AUEscenaContenedor*)_objAnimandoEnRuta->contenedor();
					SI32 indiceEnContenedor;
					if(contenedorAgregar != NULL){
						indiceEnContenedor = contenedorAgregar->hijos()->indiceDe(_objAnimandoEnRuta); NBASSERT(indiceEnContenedor!=-1)
						contenedorAgregar->quitarObjetoEscenaEnIndice(indiceEnContenedor); NBASSERT(_objAnimandoEnRuta->conteoReferencias()>0)
					}
					_objAnimandoEnRuta->establecerMultiplicadorAlpha8(0);
					_objAnimandoEnRuta->establecerTraslacion(posEscena);
					if(contenedorAgregar != NULL){ contenedorAgregar->agregarObjetoEscenaEnIndice(_objAnimandoEnRuta, indiceEnContenedor); }
					_modoTickAnimacionRuta					= ENRutaAnimModoTick_Apareciendo;
					_segsAcumReaparecer						= 0.0f;
				}
			}
			break;
		case ENRutaAnimModoTick_Apareciendo:
			//ANIMAR REAPARICION DE OBJETO
			_segsAcumReaparecer								+= segsTranscurridos;
			if(_objAnimandoEnRuta != NULL){
				if(_segsAcumReaparecer<_segsTomaReaparecer){
					_objAnimandoEnRuta->establecerMultiplicadorAlpha8(255.0f * (_segsAcumReaparecer / _segsTomaReaparecer));
				} else {
					_objAnimandoEnRuta->establecerMultiplicadorAlpha8(255);
					_modoTickAnimacionRuta					= ENRutaAnimModoTick_Animando;
				}
			}
			break;
		default:
			NBASSERT(false)
			break;
	}
	NBASSERT(_estadoAnimacion.consumirMovimientoBrusco==false)
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

//

bool AUEscenaRutaAnimador::agregarXmlInternoEn(AUEscenaRutaAnimador* rutaAnim, AUCadenaLargaMutable8* guardarEn, const char* espaciosBaseIzq, AUArreglo* arregloObjetosParaID){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AULanzallamas::agregarXmlInternoEn")
	//guardarEn->agregarConFormato("%s<vXML>1</vXML>\r\n", espaciosBaseIzq);
	guardarEn->agregarConFormato("%s<eOb>\r\n", espaciosBaseIzq);
	AUEscenaRuta::agregarXmlInternoEn(rutaAnim, guardarEn, espaciosBaseIzq);
	guardarEn->agregarConFormato("%s</eOb>\r\n", espaciosBaseIzq);
	guardarEn->agregarConFormato("%s<vel>%f</vel>\r\n", espaciosBaseIzq, rutaAnim->_estadoAnimacion.velocidadRecorreRuta);
	guardarEn->agregarConFormato("%s<rep>%d</rep>\r\n", espaciosBaseIzq, rutaAnim->_estadoAnimacion.repetirRuta?1:0);
	guardarEn->agregarConFormato("%s<pIni>%f</pIni>\r\n", espaciosBaseIzq, rutaAnim->_posicionRelInicial);
	guardarEn->agregarConFormato("%s<mod>%d</mod>\r\n", espaciosBaseIzq, rutaAnim->_estadoAnimacion.modoRutaAnimacion);
	guardarEn->agregarConFormato("%s<iniA>%d</iniA>\r\n", espaciosBaseIzq, rutaAnim->_iniciarAnimandoRuta?1:0);
	//
	if(rutaAnim->_objAnimandoEnRuta != NULL) guardarEn->agregarConFormato("%s<idObj>%d</idObj>\r\n", espaciosBaseIzq, (SI32)arregloObjetosParaID->indiceDe(rutaAnim->_objAnimandoEnRuta));
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return true;
	
}

bool AUEscenaRutaAnimador::interpretarXmlInterno(AUEscenaRutaAnimador* cargarEn, const AUDatosXML* datosXml, const sNodoXML* nodoXml, AUArregloNativoP<AUEscenaObjeto*>* arregloObjetos, AUArregloNativoP<SI32>* arregloIDs){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AULanzallamas::interpretarXmlInterno")
	bool exito = false;
	//int versionXML = datosXml->nodoHijo("vXML", 0, nodoXml);
	//if(versionXML==1){
	const sNodoXML* nodoEscenaObj;			XML_NODO_HIJO(datosXml, nodoEscenaObj, "eOb", "escObj", nodoXml, NULL)
	if(nodoEscenaObj != NULL){
		exito = AUEscenaRuta::interpretarXmlInterno(cargarEn, datosXml, nodoEscenaObj);
		if(exito){
			cargarEn->_posicionRelInicial						= datosXml->nodoHijo("pIni", 0.0f, nodoXml); NBASSERT(cargarEn->_posicionRelInicial>=0.0f && cargarEn->_posicionRelInicial<=1.0f)
			cargarEn->_estadoAnimacion.velocidadRecorreRuta		= datosXml->nodoHijo("vel", 128.0f, nodoXml);
			cargarEn->_estadoAnimacion.repetirRuta				= datosXml->nodoHijo("rep", false, nodoXml);
			cargarEn->_estadoAnimacion.modoRutaAnimacion		= datosXml->nodoHijo("mod", (SI32)ENRutaAnimModo_IdaVuelta, nodoXml);
			cargarEn->_iniciarAnimandoRuta						= datosXml->nodoHijo("iniA", true, nodoXml);
			SI32 indiceObjeto					= arregloIDs->indiceDe(datosXml->nodoHijo("idObj", -1, nodoXml));
			if(indiceObjeto!=-1) cargarEn->establecerObjAnimarEnRuta(arregloObjetos->elemento[indiceObjeto]);
			NBASSERT(cargarEn->_posicionRelInicial>=0.0f && cargarEn->_posicionRelInicial<=1.0f)
		}
	}
	//}
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return exito;
}

bool AUEscenaRutaAnimador::agregarBitsInternosEn(AUEscenaRutaAnimador* rutaAnim, AUArchivo* guardarEn, AUArreglo* arregloObjetosParaID){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaRutaAnimador::agregarBitsInternosEn")
	bool exito = false;
	//
	float versionDatos = 1.0f;
	guardarEn->escribir(&versionDatos, sizeof(versionDatos), 1, guardarEn);
	if(AUEscenaRuta::agregarBitsInternosEn(rutaAnim, guardarEn)){
		guardarEn->escribir(&rutaAnim->_posicionRelInicial, sizeof(rutaAnim->_posicionRelInicial), 1, guardarEn);
		guardarEn->escribir(&rutaAnim->_iniciarAnimandoRuta, sizeof(rutaAnim->_iniciarAnimandoRuta), 1, guardarEn);
		guardarEn->escribir(&rutaAnim->_estadoAnimacion.velocidadRecorreRuta, sizeof(rutaAnim->_estadoAnimacion.velocidadRecorreRuta), 1, guardarEn);
		guardarEn->escribir(&rutaAnim->_estadoAnimacion.repetirRuta, sizeof(rutaAnim->_estadoAnimacion.repetirRuta), 1, guardarEn);
		guardarEn->escribir(&rutaAnim->_estadoAnimacion.modoRutaAnimacion, sizeof(rutaAnim->_estadoAnimacion.modoRutaAnimacion), 1, guardarEn);
		SI32 iObjAnim = -1; if(rutaAnim->_objAnimandoEnRuta != NULL) iObjAnim = arregloObjetosParaID->indiceDe(rutaAnim->_objAnimandoEnRuta);
		guardarEn->escribir(&iObjAnim, sizeof(iObjAnim), 1, guardarEn);
		exito = true;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return exito;
}

bool AUEscenaRutaAnimador::interpretarBitsInternos(AUEscenaRutaAnimador* cargarEn, AUArchivo* cargarDe, AUArreglo* arregloObjetosParaID){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaRutaAnimador::interpretarBitsInternos")
	bool exito = false;
	float versionDatos;
	cargarDe->leer(&versionDatos, sizeof(versionDatos), 1, cargarDe);
	if(versionDatos==1.0f){
		if(AUEscenaRuta::interpretarBitsInternos(cargarEn, cargarDe)){
			cargarDe->leer(&cargarEn->_posicionRelInicial, sizeof(cargarEn->_posicionRelInicial), 1, cargarDe); NBASSERT(cargarEn->_posicionRelInicial>=0.0f && cargarEn->_posicionRelInicial<=1.0f)
			cargarDe->leer(&cargarEn->_iniciarAnimandoRuta, sizeof(cargarEn->_iniciarAnimandoRuta), 1, cargarDe);
			cargarDe->leer(&cargarEn->_estadoAnimacion.velocidadRecorreRuta, sizeof(cargarEn->_estadoAnimacion.velocidadRecorreRuta), 1, cargarDe);
			cargarDe->leer(&cargarEn->_estadoAnimacion.repetirRuta, sizeof(cargarEn->_estadoAnimacion.repetirRuta), 1, cargarDe);
			cargarDe->leer(&cargarEn->_estadoAnimacion.modoRutaAnimacion, sizeof(cargarEn->_estadoAnimacion.modoRutaAnimacion), 1, cargarDe);
			SI32 iObjAnim; cargarDe->leer(&iObjAnim, sizeof(iObjAnim), 1, cargarDe);
			if(iObjAnim>=0 && iObjAnim<arregloObjetosParaID->conteo) cargarEn->establecerObjAnimarEnRuta((AUEscenaObjeto*)arregloObjetosParaID->elemento[iObjAnim]);
			exito = true;
			NBASSERT(cargarEn->_posicionRelInicial>=0.0f && cargarEn->_posicionRelInicial<=1.0f)
		}
	} else {
		PRINTF_ERROR("AUEscenaRutaAnimador, la version (%f) de datos binarios no es interpretable por este ejecutable\n", versionDatos);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return exito;
}

//
AUOBJMETODOS_CLASESID_MULTICLASE(AUEscenaRutaAnimador, AUEscenaRuta)
AUOBJMETODOS_CLASESNOMBRES_MULTICLASE(AUEscenaRutaAnimador, "AUEscenaRutaAnimador", AUEscenaRuta)
AUOBJMETODOS_CLONAR_THIS(AUEscenaRutaAnimador)













