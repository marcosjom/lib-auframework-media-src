//
//  AUSimuladorLiquidos.cpp
//  Gameplay_Mac
//
//  Created by Marcos Ortega Morales on 12/10/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkMediaStdAfx.h"
#include "AUSimuladorLiquidos.h"

#ifndef CONFIG_NB_UNSUPPORT_BOX2D

/*
 Pruebas de rendimiento para diferente tamano de caja en la grilla:
 Ciclo a: 60FPS, 750 gotas, 0.6 radio liquido, 0.05 radio solido, 0.004 biscosidad
 tamCaja = (AU_SIMULADOR_LIQUIDOS_RADIO_GOTA_LIQUIDO * 2.0f): consumen 80% del tiempo, comportamiento realista (OK)
 tamCaja = (AU_SIMULADOR_LIQUIDOS_RADIO_GOTA_LIQUIDO / 1.0f): consumen 62% del tiempo, comportamiento realista (OK)
 tamCaja = (AU_SIMULADOR_LIQUIDOS_RADIO_GOTA_LIQUIDO / 1.33f): consumen 55% del tiempo, comportamiento realista (OK - IDEAL)
 tamCaja = (AU_SIMULADOR_LIQUIDOS_RADIO_GOTA_LIQUIDO / 1.5f): consumen 52% del tiempo, comportamiento realista (un poco tembloroso irregular)
 tamCaja = (AU_SIMULADOR_LIQUIDOS_RADIO_GOTA_LIQUIDO / 2.0f): consumen 48% del tiempo, comportamiento no-realista (muy tembloroso irregular)
 */

AUSimuladorLiquidos::AUSimuladorLiquidos(float x, float y, float ancho, float alto, float dimesionesCajas, AUEscenaContenedor* contenedorDebug) : AUEscenaObjeto(), _gotas(this, 256), _tmpRelaciones(this, 256*40) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUSimuladorLiquidos::AUSimuladorLiquidos")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUSimuladorLiquidos")
	_dimensionesCajasLiq = dimesionesCajas;
	_conteoRegistrosGotasRevisar = 0;
	//Determinar la cantidad de cajas enteras
	float cajasAltoF	= alto/_dimensionesCajasLiq;
	float cajasAnchoF	= ancho/_dimensionesCajasLiq;
	_filas				= cajasAltoF; if((float)_filas!=cajasAltoF) _filas++;
	_columnas			= cajasAnchoF; if((float)_columnas!=cajasAnchoF) _columnas++;
	NBASSERT(_columnas>0)
	NBASSERT(_filas>0)
	_filas				+= 2; //Agregar una fila vacia a cada extremo para reducir condicionales/validaciones en los ciclos que recorren las celdas
	_columnas			+= 2; //Agregar una fila vacia a cada extremo para reducir condicionales/validaciones en los ciclos que recorren las celdas
	PRINTF_INFO("SIMULACION DE LIQUIDOS: grilla de fils(%d) x cols(%d)\n", (SI32)_filas, (SI32)_columnas);
	//
	SI32 iGota;
	for(iGota=_gotas.conteo-1; iGota>=0; iGota--){
		_gotas.elemento[iGota].registroOcupado = false;
	}
	//
	_areaCajasLiq.x		= x-_dimensionesCajasLiq;	//-_dimensionesCajas, para incluir una caja vacia a la izquierda (y otra a la derecha)
	_areaCajasLiq.y		= y-_dimensionesCajasLiq;	//-_dimensionesCajas, para incluir una caja vacia a la abajo (y otra a la arriba)
	_areaCajasLiq.ancho	= (float)_columnas*_dimensionesCajasLiq;
	_areaCajasLiq.alto	= (float)_filas*_dimensionesCajasLiq;
	//
	_texGota			= NBGestorTexturas::texturaDesdeArchivo("Escenarios/liqGotaGris.png");		_texGota->retener(NB_RETENEDOR_THIS);
	_texGotaMed			= NBGestorTexturas::texturaDesdeArchivo("Escenarios/liqGotaMedGris.png");	_texGotaMed->retener(NB_RETENEDOR_THIS);
	_texGotaPeq			= NBGestorTexturas::texturaDesdeArchivo("Escenarios/liqGotaPeqGris.png");	_texGotaPeq->retener(NB_RETENEDOR_THIS);
	//
	_motorSumilacion		= new NBSimuladorLiquidosMotor1(_idMundoFisica, contenedorDebug, this, _areaCajasLiq.x, _areaCajasLiq.y, _columnas, _filas, _dimensionesCajasLiq); NB_DEFINE_NOMBRE_PUNTERO(_motorSumilacion, "AUSimuladorLiquidos::_motorSumilacion")
	//
	NBGestorFisica::agregarAnimadorAdicional(_idMundoFisica, this);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

AUSimuladorLiquidos::~AUSimuladorLiquidos(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUSimuladorLiquidos::~AUSimuladorLiquidos")
	NBGestorFisica::quitarAnimadorAdicional(_idMundoFisica, this);
	//liberar texturas de gotas
	_texGota->liberar(NB_RETENEDOR_THIS);
	_texGotaMed->liberar(NB_RETENEDOR_THIS);
	_texGotaPeq->liberar(NB_RETENEDOR_THIS);
	//liberar gotas
	SI16 iGota;
	for(iGota=_gotas.conteo-1; iGota>=0; iGota--){
		if(_gotas.elemento[iGota].registroOcupado) {
			this->eliminarGota(iGota);
		}
	}
	//Liberar motor
	if(_motorSumilacion != NULL){
		delete _motorSumilacion;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

/*void AUSimuladorLiquidos::indicesCajaCalculados(float xPos, float yPos, int* guardaIndiceColumnaEn, int* guardaIndiceFilaEn){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUSimuladorLiquidos::indicesCajaCalculados")
	int rCol = -1, rFil = -1;
	if(xPos<_inicioCajas.x || xPos>=_finCajas.x || yPos<_inicioCajas.y || yPos>=_finCajas.y){
		rCol = -1; rFil = -1;
	} else {
		rCol = (xPos - _inicioCajas.x) / _dimensionesCajas;
		rFil = (yPos - _inicioCajas.y) / _dimensionesCajas;
	}
	*guardaIndiceColumnaEn 	= rCol;
	*guardaIndiceFilaEn 	= rFil;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}*/

/*void AUSimuladorLiquidos::indicesCajaBuscados(float xPos, float yPos, int* guardaIndiceColumnaEn, int* guardaIndiceFilaEn){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUSimuladorLiquidos::indicesCajaBuscados")
	NBPuntoI indices;
	int rCol = -1, rFil = -1;
	int iCol;
	for(iCol=0; iCol<_columnas->conteo && rCol==-1; iCol++){
		NBGrillaAgrupadoraColumna columna = _columnas->elemento[iCol];
		if(xPos>=columna.x && xPos<(columna.x+columna.ancho)){
			rCol = iCol;
			int iCaja;
			for(iCaja=0; iCaja<columna.cajas->conteo && rFil==-1; iCaja++){
				NBGrillaAgrupadoraCaja caja = columna.cajas->elemento[iCaja];
				if(yPos>=caja.y && yPos<(caja.y+caja.alto)){
					rFil = iCaja;
				}
			}
		}
	}
	//
	*guardaIndiceColumnaEn 	= rCol;
	*guardaIndiceFilaEn 	= rFil;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}*/

UI16 AUSimuladorLiquidos::conteoGotas(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUSimuladorLiquidos::conteoGotas")
	//PENDIENTE: implementar una variable de acumulacion previamente actualizada en lugar de este ciclo
	UI16 conteoGotas = 0;
	UI32 iGota;
	for(iGota=0; iGota<_conteoRegistrosGotasRevisar; iGota++){
		NBASSERT(iGota<_gotas.conteo)
		if(_gotas.elemento[iGota].registroOcupado) {
			conteoGotas++;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return conteoGotas;
}

NBPunto AUSimuladorLiquidos::posicionGotaEnMundoFisica(UI16 iGota){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUSimuladorLiquidos::posicionGota")
	NBASSERT(iGota<_conteoRegistrosGotasRevisar);
	NBASSERT(_gotas.elemento[iGota].registroOcupado)
	NBPunto posicion = _motorSumilacion->posicionGota(iGota);
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return posicion;
}

UI16 AUSimuladorLiquidos::creaGota(float posX, float posY, float velX, float velY, ENTuberiaLiquidoTipo tipoLiquido, float radioLiquido, float radioSolido, float biscosidad){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUSimuladorLiquidos::creaGota")
	NBASSERT(tipoLiquido>=0 && tipoLiquido<ENTuberiaLiquidoTipo_Conteo)
	UI16 rIGota = 0;
	//CUERPO DE GOTA
	/*b2BodyDef defCuerpo; 
	b2CircleShape defForma;
	b2MassData defMasa;
	b2FixtureDef defpropiedadesCuerpo;
	defCuerpo.type							= b2_dynamicBody;
	defCuerpo.position.Set(posX, posY);
	defCuerpo.fixedRotation					= true;
	defCuerpo.allowSleep					= false;
	cuerpoGota								= NBGestorFisica::crearCuerpo(_idMundoFisica, &defCuerpo);
	//FIJACION DE GOTA
	//Producir una diferencia aleatoria en el radio solido para que las gotas 
	//que colisionan con otro objeto empujen a sus vecinas en direcciones distintas al horizontal o vertical
	float factorTamanoSolido = 0.9f + (0.1f * ((float)(rand() % 100)/100.0f));
	defForma.m_radius						= radioSolido * factorTamanoSolido; 
	defpropiedadesCuerpo.shape 				= &defForma;	//circulo
	defpropiedadesCuerpo.density 			= 1.0f; 		//a probar
	defpropiedadesCuerpo.restitution 		= 0.4f;			//a probar
	defpropiedadesCuerpo.friction			= 0.0f;			//orignalmente 0.0f
	defpropiedadesCuerpo.filter.groupIndex 	= -10;			//evita colision entre gotas 
	defMasa.mass							= 0.01f;
	defMasa.I								= 1.0f;
	cuerpoGota->SetMassData(&defMasa);
	cuerpoGota->CreateFixture(&defpropiedadesCuerpo);*/
	//
	SI32							iFil = (posY-_areaCajasLiq.y)/_dimensionesCajasLiq;
	SI32							iCol = (posX-_areaCajasLiq.x)/_dimensionesCajasLiq;
	STLiquidoGota					gotaNueva;
	gotaNueva.registroOcupado		= true;
	//Propiedades
	gotaNueva.tipoLiq				= tipoLiquido;
	gotaNueva.radioLiquido			= radioLiquido;
	//Temporal
	gotaNueva.vectEscenaEstela.x	= 0.0f;
	gotaNueva.vectEscenaEstela.y	= 0.0f;
	gotaNueva.posEscenaAnterior.x	= posX * ESCALA_PIXELES_POR_METRO_MUNDO_FISICA;
	gotaNueva.posEscenaAnterior.y	= posY * ESCALA_PIXELES_POR_METRO_MUNDO_FISICA;
	//
	SI32 indiceAgregado = -1;
	UI32 iGota;
	for(iGota=0; iGota<_gotas.conteo; iGota++){
		if(!_gotas.elemento[iGota].registroOcupado) {
			_gotas.elemento[iGota] = gotaNueva;
			indiceAgregado = iGota;
			break;
		}
	}
	if(indiceAgregado==-1){
		_gotas.agregarElemento(gotaNueva);
		indiceAgregado = _gotas.conteo-1;
	}
	_motorSumilacion->gotaAgregada(indiceAgregado, posX, posY, velX, velY, radioLiquido, radioSolido, biscosidad);
	rIGota = indiceAgregado; NBASSERT(rIGota==indiceAgregado)
	NBASSERT(rIGota<_gotas.conteo)
	if(iCol>0 && iCol<(_columnas-1) && iFil>0 && iFil<(_filas-1)){ //Las celdas de los bordes deben permanecer vacias 
		//PRINTF_INFO("GOTA CREADA EN INDINCE %d\n", indiceAgregado);
	} else {
		PRINTF_WARNING("se ha creado gota fuera de la grilla (sera eliminada imediatamente) (%f, %f)\n", posX, posY);
	}
	if(_conteoRegistrosGotasRevisar<=indiceAgregado){
		_conteoRegistrosGotasRevisar = (indiceAgregado+1);
	}
	//
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return rIGota;
}

/*b2Body* AUSimuladorLiquidos::gotaEnCoordenada(float posX, float posY){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUSimuladorLiquidos::gotaEnCoordenada")
    b2Body* cuerpoGota = NULL;
    SI32 iGota;
	for(iGota=0; iGota<_gotas.conteo && cuerpoGota == NULL; iGota++){
		if(_gotas.elemento[iGota].cuerpoFisica) {
			b2Body* cuerpo = _gotas.elemento[iGota].cuerpoFisica;
            b2Vec2 posicion = cuerpo->GetWorldCenter();
            float distancia = NBPUNTO_DISTANCIA(posicion.x, posicion.y, posX, posY);
            if(distancia>-AU_SIMULADOR_LIQUIDOS_RADIO_GOTA_FISICO && distancia<AU_SIMULADOR_LIQUIDOS_RADIO_GOTA_FISICO){
                cuerpoGota = cuerpo;
            }
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
    return cuerpoGota;
}*/

void AUSimuladorLiquidos::eliminarGota(UI16 iGota){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUSimuladorLiquidos::eliminarGota")
	NBASSERT(iGota<_gotas.conteo)
	STLiquidoGota* gota = &(_gotas.elemento[iGota]);
	NBASSERT(gota->registroOcupado) //Se ha intentado eliminar un registro libre
	//PRINTF_INFO("Quitando gota %d desde simulador (orden)\n", iGota);
	//quitar cuerpo, sprite, animadorFisica y desactivar registro
	gota->registroOcupado	= false;
	_motorSumilacion->gotaQuitada(iGota);
	//descativar registro
	//Determinar el nuevo indice maximo (si es necesario)
	if(_conteoRegistrosGotasRevisar==(iGota+1)){
		_conteoRegistrosGotasRevisar = 0;
		SI32 iBsq;
		for(iBsq=iGota-1; iBsq>=0 && _conteoRegistrosGotasRevisar==0; iBsq--){
			if(_gotas.elemento[iBsq].registroOcupado){
				_conteoRegistrosGotasRevisar = (iBsq+1);
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUSimuladorLiquidos::gotaQuitadaDeMotorPorSalirDeLimites(UI16 indice){
	NBASSERT(false) //Estoy intentando evitar esta situacion, de preferencia que esto lo controlen las tuberiasLiquidas y el gameplay
	NBASSERT(indice<_gotas.conteo)
	STLiquidoGota* gota = &(_gotas.elemento[indice]);
	NBASSERT(gota->registroOcupado) //Se ha intentado eliminar un registro libre
	gota->registroOcupado	= false;
	//Determinar el nuevo indice maximo (si es necesario)
	if(_conteoRegistrosGotasRevisar==(indice+1)){
		_conteoRegistrosGotasRevisar = 0;
		SI32 iBsq;
		for(iBsq=indice-1; iBsq>=0 && _conteoRegistrosGotasRevisar==0; iBsq--){
			if(_gotas.elemento[iBsq].registroOcupado){
				_conteoRegistrosGotasRevisar = (iBsq+1);
			}
		}
	}
}

void AUSimuladorLiquidos::simularFisicaCuerpos(float segundosTranscurridos){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUSimuladorLiquidos::simularFisicaCuerpos")
	_motorSumilacion->simularFuerzasLiquidos(segundosTranscurridos);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

NBCajaAABB AUSimuladorLiquidos::cajaAABBLocalCalculada(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaTexto::cajaAABBLocalCalculada")
	NBCajaAABB cajaLocal; NBCAJAAABB_INICIALIZAR(cajaLocal)
	SI32 iGota;
	for(iGota=_conteoRegistrosGotasRevisar-1; iGota>=0; iGota--){
		if(_gotas.elemento[iGota].registroOcupado){
			NBPunto pos = _motorSumilacion->posicionGota(iGota);
			pos.x = pos.x * ESCALA_PIXELES_POR_METRO_MUNDO_FISICA;
			pos.y = pos.y * ESCALA_PIXELES_POR_METRO_MUNDO_FISICA;
			NBCAJAAABB_ENVOLVER_PUNTO(cajaLocal, pos.x, pos.y)
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return cajaLocal;
}

void AUSimuladorLiquidos::agregadoEnEscena(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUSimuladorLiquidos::agregadoEnEscena")
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUSimuladorLiquidos::quitandoDeEscena(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUSimuladorLiquidos::quitandoDeEscena")
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

PTRfunCmdsGL AUSimuladorLiquidos::actualizarModeloGL(const NBPropRenderizado &propsRenderizado, const NBPropIluminacion &propsIluminacion, const NBPropHeredadasRender &propsHeredadas){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUSimuladorLiquidos::actualizarModeloGL")
	NBASSERT(idEscena!=-1) //Solo si esta en escena
	//Conteo de gotas vivas
	SI32 iGota, conteoGotasVivas = 0; //Optimizacion, llevar este conteo en una variable previamente actualizado
	for(iGota=_conteoRegistrosGotasRevisar-1; iGota>=0; iGota--){
		if(_gotas.elemento[iGota].registroOcupado) conteoGotasVivas++;
	}
	if(conteoGotasVivas!=0){
		NBColor colorMultiplicado; NBCOLOR_MULTIPLICAR_CON_UI8(colorMultiplicado, propsHeredadas.colorPadre, _propiedades.color8);
		STBloqueGL bloqueVerticesGL;
		bloqueVerticesGL.cantidadElementos	= conteoGotasVivas * 4 * 3;
		bloqueVerticesGL.primerElemento		= (*propsRenderizado.funcReservarVertices)(ENVerticeGlTipo_MonoTextura, bloqueVerticesGL.cantidadElementos);
		NBColor8 colorGota;
		SI32 iVerticeGL		= 0;
		SI32 iVerticeGLMed	= (conteoGotasVivas * 4);
		SI32 iVerticeGLPeq	= (conteoGotasVivas * 4 * 2);
		for(iGota=_conteoRegistrosGotasRevisar-1; iGota>=0; iGota--){
			if(_gotas.elemento[iGota].registroOcupado){
				STLiquidoGota* datosGota = &(_gotas.elemento[iGota]);
				if(datosGota->tipoLiq==ENTuberiaLiquidoTipo_AguaPura){
					NBCOLOR_ESTABLECER(colorGota, 0*colorMultiplicado.r, 0*colorMultiplicado.g, 200*colorMultiplicado.b, 255*colorMultiplicado.a)
				} else if(datosGota->tipoLiq==ENTuberiaLiquidoTipo_LiqVerde){
					NBCOLOR_ESTABLECER(colorGota, 0*colorMultiplicado.r, 180*colorMultiplicado.g, 0*colorMultiplicado.b, 255*colorMultiplicado.a)
				} else if(datosGota->tipoLiq==ENTuberiaLiquidoTipo_LiqNegro){
					NBCOLOR_ESTABLECER(colorGota, 25*colorMultiplicado.r, 25*colorMultiplicado.b, 25*colorMultiplicado.b, 255*colorMultiplicado.a)
				} else {
					NBASSERT(false) //Tipo de liquido no valido o incluido
				}
				//
				float radioLiquido	= (datosGota->radioLiquido*ESCALA_PIXELES_POR_METRO_MUNDO_FISICA*0.53f)/2.0f;
				float radioLiquidoMed	= radioLiquido/2.0f;
				float radioLiquidoPeq	= radioLiquidoMed/2.0f;
				//
				NBPunto pos = _motorSumilacion->posicionGota(iGota);
				pos.x = pos.x * ESCALA_PIXELES_POR_METRO_MUNDO_FISICA;
				pos.y = pos.y * ESCALA_PIXELES_POR_METRO_MUNDO_FISICA;
				NBPunto vecEstela		 = datosGota->vectEscenaEstela;
				NBPunto vecEstelaVel; NBPUNTO_ESTABLECER(vecEstelaVel, (datosGota->posEscenaAnterior.x-pos.x)*30.0f, (datosGota->posEscenaAnterior.y-pos.y)*30.0f)
				vecEstela.x += (vecEstelaVel.x-vecEstela.x) * 0.1f;
				vecEstela.y += (vecEstelaVel.y-vecEstela.y) * 0.1f;
				datosGota->posEscenaAnterior.x = pos.x;
				datosGota->posEscenaAnterior.y = pos.y;
				//b2Vec2 vel = datosGota->cuerpoFisica->GetLinearVelocity();
				//vel.x *= ESCALA_PIXELES_POR_METRO_MUNDO_FISICA/3.0f;
				//vel.y *= ESCALA_PIXELES_POR_METRO_MUNDO_FISICA/3.0f;
				///*if(movGota.x>radioLiquido || movGota.x<-radioLiquido || movGota.y>radioLiquido || movGota.y<-radioLiquido)*/ PRINTF_INFO("Cambio de gota (%f, %f)\n", movGota.x, movGota.y);
				//float factorReduccion;
				NBPunto trasladoMed; NBPUNTO_ESTABLECER(trasladoMed, 0.0f, 0.0f /*vecEstela.x, vecEstela.y*/)
				//if(trasladoMed.x>radioLiquido){		factorReduccion = radioLiquido/trasladoMed.x;		trasladoMed.x *= factorReduccion; trasladoMed.y *= factorReduccion; }
				//if(trasladoMed.x<-radioLiquido){	factorReduccion = -radioLiquido/trasladoMed.x;		trasladoMed.x *= factorReduccion; trasladoMed.y *= factorReduccion; }
				//if(trasladoMed.y>radioLiquido){		factorReduccion = radioLiquido/trasladoMed.y;		trasladoMed.x *= factorReduccion; trasladoMed.y *= factorReduccion; }
				//if(trasladoMed.y<-radioLiquido){	factorReduccion = -radioLiquido/trasladoMed.y;		trasladoMed.x *= factorReduccion; trasladoMed.y *= factorReduccion; }
				NBPunto trasladoPeq = trasladoMed;
				//if(trasladoPeq.x>radioLiquidoMed){	factorReduccion = radioLiquidoMed/trasladoPeq.x;	trasladoPeq.x *= factorReduccion; trasladoPeq.y *= factorReduccion; }
				//if(trasladoPeq.x<-radioLiquidoMed){ factorReduccion = -radioLiquidoMed/trasladoPeq.x;	trasladoPeq.x *= factorReduccion; trasladoPeq.y *= factorReduccion; }
				//if(trasladoPeq.y>radioLiquidoMed){	factorReduccion = radioLiquidoMed/trasladoPeq.y;	trasladoPeq.x *= factorReduccion; trasladoPeq.y *= factorReduccion; }
				//if(trasladoPeq.y<-radioLiquidoMed){ factorReduccion = -radioLiquidoMed/trasladoPeq.y;	trasladoPeq.x *= factorReduccion; trasladoPeq.y *= factorReduccion; }
				//
				NBPunto pMinGota;		NBPUNTO_ESTABLECER(pMinGota, pos.x-radioLiquido, pos.y-radioLiquido)
				NBPunto pMaxGota;		NBPUNTO_ESTABLECER(pMaxGota, pos.x+radioLiquido, pos.y+radioLiquido)
				NBPunto pMinGotaMed;	NBPUNTO_ESTABLECER(pMinGotaMed, pos.x-radioLiquidoMed+trasladoMed.x, pos.y-radioLiquidoMed+trasladoMed.y)
				NBPunto pMaxGotaMed;	NBPUNTO_ESTABLECER(pMaxGotaMed, pos.x+radioLiquidoMed+trasladoMed.x, pos.y+radioLiquidoMed+trasladoMed.y)
				NBPunto pMinGotaPeq;	NBPUNTO_ESTABLECER(pMinGotaPeq, pos.x-radioLiquidoPeq+trasladoMed.x+trasladoPeq.x, pos.y-radioLiquidoPeq+trasladoMed.y+trasladoPeq.y)
				NBPunto pMaxGotaPeq;	NBPUNTO_ESTABLECER(pMaxGotaPeq, pos.x+radioLiquidoPeq+trasladoMed.x+trasladoPeq.x, pos.y+radioLiquidoPeq+trasladoMed.y+trasladoPeq.y)
				//GOTA PEQUENA
				NBVerticeGL* verticesGLPeq = &((*propsRenderizado.verticesGL1)[bloqueVerticesGL.primerElemento+iVerticeGLPeq]);
				NBMATRIZ_MULTIPLICAR_PUNTO(verticesGLPeq[0], _cacheObjEscena.matrizEscena, pMinGotaPeq.x, pMinGotaPeq.y);
				NBMATRIZ_MULTIPLICAR_PUNTO(verticesGLPeq[1], _cacheObjEscena.matrizEscena, pMaxGotaPeq.x, pMinGotaPeq.y);
				NBMATRIZ_MULTIPLICAR_PUNTO(verticesGLPeq[2], _cacheObjEscena.matrizEscena, pMinGotaPeq.x, pMaxGotaPeq.y);
				NBMATRIZ_MULTIPLICAR_PUNTO(verticesGLPeq[3], _cacheObjEscena.matrizEscena, pMaxGotaPeq.x, pMaxGotaPeq.y);
				NBCOLOR_ESTABLECER(verticesGLPeq[0], colorGota.r, colorGota.g, colorGota.b, colorGota.a)
				NBCOLOR_ESTABLECER(verticesGLPeq[1], colorGota.r, colorGota.g, colorGota.b, colorGota.a)
				NBCOLOR_ESTABLECER(verticesGLPeq[2], colorGota.r, colorGota.g, colorGota.b, colorGota.a)
				NBCOLOR_ESTABLECER(verticesGLPeq[3], colorGota.r, colorGota.g, colorGota.b, colorGota.a)
				_texGotaMed->cargarCoordenadasTextura(verticesGLPeq);
				iVerticeGLPeq	+= 4;
				//GOTA MEDIANA
				NBVerticeGL* verticesGLMed = &((*propsRenderizado.verticesGL1)[bloqueVerticesGL.primerElemento+iVerticeGLMed]);
				NBMATRIZ_MULTIPLICAR_PUNTO(verticesGLMed[0], _cacheObjEscena.matrizEscena, pMinGotaMed.x, pMinGotaMed.y);
				NBMATRIZ_MULTIPLICAR_PUNTO(verticesGLMed[1], _cacheObjEscena.matrizEscena, pMaxGotaMed.x, pMinGotaMed.y);
				NBMATRIZ_MULTIPLICAR_PUNTO(verticesGLMed[2], _cacheObjEscena.matrizEscena, pMinGotaMed.x, pMaxGotaMed.y);
				NBMATRIZ_MULTIPLICAR_PUNTO(verticesGLMed[3], _cacheObjEscena.matrizEscena, pMaxGotaMed.x, pMaxGotaMed.y);
				NBCOLOR_ESTABLECER(verticesGLMed[0], colorGota.r, colorGota.g, colorGota.b, colorGota.a)
				NBCOLOR_ESTABLECER(verticesGLMed[1], colorGota.r, colorGota.g, colorGota.b, colorGota.a)
				NBCOLOR_ESTABLECER(verticesGLMed[2], colorGota.r, colorGota.g, colorGota.b, colorGota.a)
				NBCOLOR_ESTABLECER(verticesGLMed[3], colorGota.r, colorGota.g, colorGota.b, colorGota.a)
				_texGotaMed->cargarCoordenadasTextura(verticesGLMed);
				iVerticeGLMed	+= 4;
				//GOTA NORMAL
				NBVerticeGL* verticesGL = &((*propsRenderizado.verticesGL1)[bloqueVerticesGL.primerElemento+iVerticeGL]);
				NBMATRIZ_MULTIPLICAR_PUNTO(verticesGL[0], _cacheObjEscena.matrizEscena, pMinGota.x, pMinGota.y);
				NBMATRIZ_MULTIPLICAR_PUNTO(verticesGL[1], _cacheObjEscena.matrizEscena, pMaxGota.x, pMinGota.y);
				NBMATRIZ_MULTIPLICAR_PUNTO(verticesGL[2], _cacheObjEscena.matrizEscena, pMinGota.x, pMaxGota.y);
				NBMATRIZ_MULTIPLICAR_PUNTO(verticesGL[3], _cacheObjEscena.matrizEscena, pMaxGota.x, pMaxGota.y);
				NBCOLOR_ESTABLECER(verticesGL[0], colorGota.r, colorGota.g, colorGota.b, colorGota.a)
				NBCOLOR_ESTABLECER(verticesGL[1], colorGota.r, colorGota.g, colorGota.b, colorGota.a)
				NBCOLOR_ESTABLECER(verticesGL[2], colorGota.r, colorGota.g, colorGota.b, colorGota.a)
				NBCOLOR_ESTABLECER(verticesGL[3], colorGota.r, colorGota.g, colorGota.b, colorGota.a)
				_texGota->cargarCoordenadasTextura(verticesGL);
				iVerticeGL		+= 4;
			}
		}
		//Datos render
		NBASSERT((conteoGotasVivas * 3) > 0)
		const SI32 indiceDatosRender	= propsRenderizado.bytesDatosModelos->conteo; propsRenderizado.bytesDatosModelos->reservarRegistrosAlFinal(sizeof(STLiquidoRender));
		STLiquidoRender* datosRender	= (STLiquidoRender*)&(propsRenderizado.bytesDatosModelos->elemento[indiceDatosRender]);
		datosRender->cantModelos		= conteoGotasVivas;
		datosRender->idTexturaGota		= _texGota->idTexturaGL;	propsRenderizado.piscinaRetecionRender->agregarElemento(_texGota);
		datosRender->idTexturaGotaMed	= _texGotaMed->idTexturaGL;	propsRenderizado.piscinaRetecionRender->agregarElemento(_texGotaMed);
		datosRender->idTexturaGotaPeq	= _texGotaPeq->idTexturaGL;	propsRenderizado.piscinaRetecionRender->agregarElemento(_texGotaPeq);
		datosRender->bloqueIndicesGL	= NB_GESTOR_GL_DAME_ELEMS_PARA_TRIANGSTRIPS_4(ENVerticeGlTipo_MonoTextura, bloqueVerticesGL, conteoGotasVivas * 3);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return (conteoGotasVivas!=0 ? &AUSimuladorLiquidos::enviarComandosGL : NULL);
}

void AUSimuladorLiquidos::enviarComandosGL(UI8* punteroDatosModelo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUSimuladorLiquidos::enviarComandosGL")
	STLiquidoRender* datosRender	= (STLiquidoRender*)punteroDatosModelo;
	NBGestorGL::activarVerticesGL(ENVerticeGlTipo_MonoTextura); NBGESTORGL_DBG_NOMBRAR_ACTIVADOR_VERTICES_GL("AUSimuladorLiquidos")
	UI32 primerElemento = datosRender->bloqueIndicesGL.primerElemento;
	NBGestorGL::bindTexture(0, datosRender->idTexturaGota);
	NB_GESTOR_GL_RENDER_TRIANGSTRIPS_4(primerElemento, datosRender->cantModelos * NB_GESTOR_GL_ELEMS_POR_TRIANGSTRIP); primerElemento += datosRender->cantModelos * NB_GESTOR_GL_ELEMS_POR_TRIANGSTRIP; NBGESTORGL_DBG_NOMBRAR_CONVOCADOR_ACUMULAR_TRIANG_STRIPS_INDEP("AUSimuladorLiquidos")
	NBGestorGL::bindTexture(0, datosRender->idTexturaGotaMed);
	NB_GESTOR_GL_RENDER_TRIANGSTRIPS_4(primerElemento, datosRender->cantModelos * NB_GESTOR_GL_ELEMS_POR_TRIANGSTRIP); primerElemento += datosRender->cantModelos * NB_GESTOR_GL_ELEMS_POR_TRIANGSTRIP; NBGESTORGL_DBG_NOMBRAR_CONVOCADOR_ACUMULAR_TRIANG_STRIPS_INDEP("AUSimuladorLiquidos")
	NBGestorGL::bindTexture(0, datosRender->idTexturaGotaPeq);
	NB_GESTOR_GL_RENDER_TRIANGSTRIPS_4(primerElemento, datosRender->cantModelos * NB_GESTOR_GL_ELEMS_POR_TRIANGSTRIP); primerElemento += datosRender->cantModelos * NB_GESTOR_GL_ELEMS_POR_TRIANGSTRIP; NBGESTORGL_DBG_NOMBRAR_CONVOCADOR_ACUMULAR_TRIANG_STRIPS_INDEP("AUSimuladorLiquidos")
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

/*void AUSimuladorLiquidos::privSimularFisicaLiquidosComparacionBruta(float segundosTranscurridos){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUSimuladorLiquidos::privSimularFisicaLiquidosComparacionBruta")
	float multiplicador = AU_SIMULADOR_LIQUIDOS_RADIO_IDEAL / AU_SIMULADOR_LIQUIDOS_RADIO_GOTA_LIQUIDO;
	//Actualizar cache de gotas y grillas
	//clock_t tINi = clock();
	SI16 ultimaFila = _filas-1;
	SI16 ultimaColumna = _columnas-1;
	SI16 iReg;
	for(iReg=0; iReg<_conteoRegistrosGotasRevisar; iReg++){
		NBASSERT(iReg>=0 && iReg<_gotas.conteo)
		STLiquidoGota* gota		= &_gotas.elemento[iReg];
		if(gota->cuerpoFisica){
			b2Vec2 posicion 	= gota->cuerpoFisica->GetWorldCenter();					//optimizar, evitar el llamado a la funcion
			b2Vec2 velocidad 	= gota->cuerpoFisica->GetLinearVelocity();				//optimizar, evitar el llamado a la funcion
			gota->x				= multiplicador * posicion.x;
			gota->y				= multiplicador * posicion.y;
			gota->vx			= multiplicador * velocidad.x;
			gota->vy			= multiplicador * velocidad.y;
			gota->cambioX 		= 0.0f;
			gota->cambioY 		= 0.0f;
			SI16 indiceCol		= (posicion.x-_areaCajas.x)/_dimensionesCajas;
			SI16 indiceFil		= (posicion.y-_areaCajas.y)/_dimensionesCajas;
			if(!(indiceCol>0 && indiceCol<ultimaColumna && indiceFil>0 && indiceFil<ultimaFila)){
				//eliminar gotas fuera de la grilla
				this->privEliminarGotaEnIndice(iReg);
			}
		}
	}
	//PRINTF_INFO("%.2f%% CPU organizando caja de gotas\n", (float)(100.0f*(float)(clock()-tINi)/(float)CLOCKS_PER_SEC));
	//debug_validarGrillaAgrupadora(); //ADVERTENCIA, afecta el rendimiento si se deja activada esta validacion debug
	//calcular las fuerzas producidas por los liquidos
	//tINi = clock();
	//SI32 conteoComparacionesGotas = 0;
	float radioIdealAlCuadrado = (AU_SIMULADOR_LIQUIDOS_RADIO_IDEAL*AU_SIMULADOR_LIQUIDOS_RADIO_IDEAL);
	for(iReg=0; iReg<_conteoRegistrosGotasRevisar; iReg++){
		NBASSERT(iReg>=0 && iReg<_gotas.conteo)
		STLiquidoGota* gota	= &_gotas.elemento[iReg];
		if(gota->cuerpoFisica){
			_tmpGotasVecinas.vaciar();
			float p				= 0.0f;
			float pCercano		= 0.0f;
			//Recorrer todas las gotas que siguen
			SI16 iReg2;
			for(iReg2=0; iReg2<_conteoRegistrosGotasRevisar; iReg2++){
				NBASSERT(iReg2>=0 && iReg2<_gotas.conteo)
				STLiquidoGota* otraGota	= &_gotas.elemento[iReg2];
				float difX				= otraGota->x - gota->x;
				float difY				= otraGota->y - gota->y;
				float sumaCuadrados		= (difX * difX + difY * difY);
				if(sumaCuadrados!=0.0f && sumaCuadrados<radioIdealAlCuadrado){
					float estaDistancia	= sqrtf(sumaCuadrados); if(estaDistancia<b2_epsilon) estaDistancia = AU_SIMULADOR_LIQUIDOS_RADIO_IDEAL - 0.01f;
					float unoMenosQ		= 1.0f - (estaDistancia / AU_SIMULADOR_LIQUIDOS_RADIO_IDEAL);
					p					= (p + unoMenosQ * unoMenosQ);
					pCercano			= (pCercano + unoMenosQ * unoMenosQ * unoMenosQ);
					otraGota->tmpDistanciaConEsteVecino = estaDistancia;
					_tmpGotasVecinas.agregarElemento(otraGota);
				}
				//conteoComparacionesGotas++;
			}
			//
			float presion				= (p - 5.0f) / 2.0f;	//presion en terminos normales
			float presionCercano		= pCercano / 2.0f;		//presion en terminos de particulas cercanas
			float cambioX				= 0.0f;
			float cambioY				= 0.0f;
			//recorrer vecinos y calcular presion
			SI32 iVecina;
			for(iVecina=_tmpGotasVecinas.conteo-1; iVecina>=0; iVecina--){
				STLiquidoGota* otraGota	= _tmpGotasVecinas.elemento[iVecina];
				float difX				= otraGota->x - gota->x;
				float difY				= otraGota->y - gota->y;
				float q					= otraGota->tmpDistanciaConEsteVecino / AU_SIMULADOR_LIQUIDOS_RADIO_IDEAL;
				float oneminusq			= 1.0f-q;
				float factor			= oneminusq * (presion + presionCercano * oneminusq) / (2.0f*otraGota->tmpDistanciaConEsteVecino);
				float dx				= difX * factor;
				float dy				= difY * factor;
				float relvx				= otraGota->vx - gota->vx;
				float relvy				= otraGota->vy - gota->vy;
				factor					= AU_SIMULADOR_LIQUIDOS_BISCOSIDAD_GOTA * oneminusq * segundosTranscurridos;
				dx						-= relvx * factor;
				dy						-= relvy * factor;
				otraGota->cambioX		+= dx;
				otraGota->cambioY		+= dy;
				cambioX					-= dx;
				cambioY					-= dy;
			}
			gota->cambioX += cambioX;
			gota->cambioY += cambioY;
		}
	}
	//PRINTF_INFO("%d comparaciones con gotas\n", conteoComparacionesGotas);
	//PRINTF_INFO("%.2f%% CPU acumulando fuerzas de gotas\n", (float)(100.0f*(float)(clock()-tINi)/(float)CLOCKS_PER_SEC));
	//aplicar las fuerzas resultantes
	//tINi = clock();
	for(iReg=0; iReg<_conteoRegistrosGotasRevisar; iReg++){ //iGota++???
		NBASSERT(iReg>=0 && iReg<_gotas.conteo)
		STLiquidoGota* gota = &(_gotas.elemento[iReg]);
		if(gota->cuerpoFisica){
			b2Vec2 velocidad 	= gota->cuerpoFisica->GetLinearVelocity();				//optimizar, evitar el llamado a la funcion
			velocidad.x 		+= gota->cambioX / (multiplicador * segundosTranscurridos);
			velocidad.y 		+= gota->cambioY / (multiplicador * segundosTranscurridos);
			gota->cuerpoFisica->SetLinearVelocity(velocidad);
		}
	}
	//PRINTF_INFO("%.2f%% CPU aplicando fuerzas de gotas\n", (float)(100.0f*(float)(clock()-tINi)/(float)CLOCKS_PER_SEC));
	AU_GESTOR_PILA_LLAMADAS_POP_3
}*/

/*void AUSimuladorLiquidos::privSimularFisicaLiquidosComparacionBrutaAcumulando(float segundosTranscurridos){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUSimuladorLiquidos::privSimularFisicaLiquidosComparacionBrutaAcumulada")
	float multiplicador = AU_SIMULADOR_LIQUIDOS_RADIO_IDEAL / AU_SIMULADOR_LIQUIDOS_RADIO_GOTA_LIQUIDO;
	//Actualizar cache de gotas y grillas
	//clock_t tINi = clock();
	SI16 ultimaFila = _filas-1;
	SI16 ultimaColumna = _columnas-1;
	SI16 iReg;
	for(iReg=0; iReg<_conteoRegistrosGotasRevisar; iReg++){
		STLiquidoGota* gota		= &_gotas.elemento[iReg];
		if(gota->cuerpoFisica){
			b2Vec2 posicion 	= gota->cuerpoFisica->GetWorldCenter();					//optimizar, evitar el llamado a la funcion
			b2Vec2 velocidad 	= gota->cuerpoFisica->GetLinearVelocity();				//optimizar, evitar el llamado a la funcion
			gota->x				= multiplicador * posicion.x;
			gota->y				= multiplicador * posicion.y;
			gota->vx			= multiplicador * velocidad.x;
			gota->vy			= multiplicador * velocidad.y;
			gota->cambioX 		= 0.0f;
			gota->cambioY 		= 0.0f;
			gota->p				= 0.0f;
			gota->pCercano		= 0.0f;
			gota->tmpGotasVecinas->vaciar();
			SI16 indiceCol		= (posicion.x-_areaCajas.x)/_dimensionesCajas;
			SI16 indiceFil		= (posicion.y-_areaCajas.y)/_dimensionesCajas;
			if(!(indiceCol>0 && indiceCol<ultimaColumna && indiceFil>0 && indiceFil<ultimaFila)){
				//eliminar gotas fuera de la grilla
				this->privEliminarGotaEnIndice(iReg);
			}
		}
	}
	//PRINTF_INFO("%.2f%% CPU organizando caja de gotas\n", (float)(100.0f*(float)(clock()-tINi)/(float)CLOCKS_PER_SEC));
	//debug_validarGrillaAgrupadora(); //ADVERTENCIA, afecta el rendimiento si se deja activada esta validacion debug
	//calcular las fuerzas producidas por los liquidos
	//tINi = clock();
	SI32 maxGotasVecinas = 0; //SI32 conteoComparacionesGotas = 0;
	float radioIdealAlCuadrado = (AU_SIMULADOR_LIQUIDOS_RADIO_IDEAL*AU_SIMULADOR_LIQUIDOS_RADIO_IDEAL);
	for(iReg=0; iReg<_conteoRegistrosGotasRevisar; iReg++){
		STLiquidoGota* gota	= &_gotas.elemento[iReg];
		if(gota->cuerpoFisica){
			//Recorrer todas las gotas que siguen
			SI16 iReg2;
			for(iReg2=iReg+1; iReg2<_conteoRegistrosGotasRevisar; iReg2++){
				STLiquidoGota* otraGota	= &_gotas.elemento[iReg2];
				float difX				= otraGota->x - gota->x;
				float difY				= otraGota->y - gota->y;
				float sumaCuadrados		= (difX * difX + difY * difY);
				if(sumaCuadrados!=0.0f && sumaCuadrados<radioIdealAlCuadrado){
					float estaDistancia	= sqrtf(sumaCuadrados); if(estaDistancia<b2_epsilon) estaDistancia = AU_SIMULADOR_LIQUIDOS_RADIO_IDEAL - 0.01f;
					float unoMenosQ		= 1.0f - (estaDistancia / AU_SIMULADOR_LIQUIDOS_RADIO_IDEAL);
					gota->p				= (gota->p + unoMenosQ * unoMenosQ);
					gota->pCercano		= (gota->pCercano + unoMenosQ * unoMenosQ * unoMenosQ);
					otraGota->p			= (otraGota->p + unoMenosQ * unoMenosQ);
					otraGota->pCercano	= (otraGota->pCercano + unoMenosQ * unoMenosQ * unoMenosQ);
					STGotaConVecino datosConVecino;
					datosConVecino.distanciaVecina = estaDistancia;
					datosConVecino.punteroGotaVecina = otraGota;
					gota->tmpGotasVecinas->agregarElemento(datosConVecino);
					datosConVecino.punteroGotaVecina = gota;
					otraGota->tmpGotasVecinas->agregarElemento(datosConVecino);
				}
				//conteoComparacionesGotas++;
			}
			if(maxGotasVecinas<gota->tmpGotasVecinas->conteo) maxGotasVecinas = gota->tmpGotasVecinas->conteo;
			//
			float presion				= (gota->p - 5.0f) / 2.0f;	//presion en terminos normales
			float presionCercano		= gota->pCercano / 2.0f;	//presion en terminos de particulas cercanas
			float cambioX				= 0.0f;
			float cambioY				= 0.0f;
			//recorrer vecinos y calcular presion
			SI32 iIndice;
			for(iIndice=gota->tmpGotasVecinas->conteo-1; iIndice>=0; iIndice--){
				STGotaConVecino datosConVecino = gota->tmpGotasVecinas->elemento[iIndice];
				STLiquidoGota* otraGota	= datosConVecino.punteroGotaVecina;
				float difX				= otraGota->x - gota->x;
				float difY				= otraGota->y - gota->y;
				float q					= datosConVecino.distanciaVecina / AU_SIMULADOR_LIQUIDOS_RADIO_IDEAL;
				float oneminusq			= 1.0f-q;
				float factor			= oneminusq * (presion + presionCercano * oneminusq) / (2.0f*datosConVecino.distanciaVecina);
				float dx				= difX * factor;
				float dy				= difY * factor;
				float relvx				= otraGota->vx - gota->vx;
				float relvy				= otraGota->vy - gota->vy;
				factor					= AU_SIMULADOR_LIQUIDOS_BISCOSIDAD_GOTA * oneminusq * segundosTranscurridos;
				dx						-= relvx * factor;
				dy						-= relvy * factor;
				otraGota->cambioX		+= dx;
				otraGota->cambioY		+= dy;
				cambioX					-= dx;
				cambioY					-= dy;
			}
			gota->cambioX += cambioX;
			gota->cambioY += cambioY;
		}
	}
	//PRINTF_INFO("%d comparaciones con gotas (%d max vecinas)\n", conteoComparacionesGotas, maxGotasVecinas);
	//PRINTF_INFO("%.2f%% CPU acumulando fuerzas de gotas\n", (float)(100.0f*(float)(clock()-tINi)/(float)CLOCKS_PER_SEC));
	//aplicar las fuerzas resultantes
	//tINi = clock();
	for(iReg=0; iReg<_conteoRegistrosGotasRevisar; iReg++){ //iGota++???
		STLiquidoGota* gota = &(_gotas.elemento[iReg]);
		if(gota->registroEnUso){
			NBASSERT(gota->iCuadrante.x>=0 && gota->iCuadrante.x<_columnas && gota->iCuadrante.y>=0 && gota->iCuadrante.y<_filas)
			b2Vec2 velocidad 	= gota->cuerpoFisica->GetLinearVelocity();				//optimizar, evitar el llamado a la funcion
			velocidad.x 		+= gota->cambioX / (multiplicador * segundosTranscurridos);
			velocidad.y 		+= gota->cambioY / (multiplicador * segundosTranscurridos);
			gota->cuerpoFisica->SetLinearVelocity(velocidad);
		}
	}
	//PRINTF_INFO("%.2f%% CPU aplicando fuerzas de gotas\n", (float)(100.0f*(float)(clock()-tINi)/(float)CLOCKS_PER_SEC));
	AU_GESTOR_PILA_LLAMADAS_POP_3
}*/

/*void AUSimuladorLiquidos::privSimularFisicaLiquidosComparandoConCeldasVecinas(float segundosTranscurridos) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUSimuladorLiquidos::privSimularFisicaLiquidosComparandoConCeldasVecinas")
	float multiplicador = AU_SIMULADOR_LIQUIDOS_RADIO_IDEAL / AU_SIMULADOR_LIQUIDOS_RADIO_GOTA_LIQUIDO;
	//Actualizar cache de gotas y grillas
	//clock_t tINi = clock();
	//Vaciar las cajas de indices (que ayudan a reducir las comparaciones entre gotas)
	SI16 iArr;
	for(iArr=_indicesGotasPorCaja.conteo-1; iArr>=0; iArr--){
		((AUArregloNativoMutableP<STLiquidoGota*>*)_indicesGotasPorCaja.elemento[iArr])->vaciar();
	}
	SI16 ultimaFila = _filas-1;
	SI16 ultimaColumna = _columnas-1;
	SI16 iReg;
	for(iReg=0; iReg<_conteoRegistrosGotasRevisar; iReg++){
		STLiquidoGota* gota			= &_gotas.elemento[iReg];
		STLiquidoGotaPos* gotaPos	= &_gotasPos.elemento[iReg];
		STLiquidoGotaProps* gotaProps = &_gotasProps.elemento[iReg];
		if(!gota->cuerpoFisica) continue;
		b2Vec2 posicion 	= gota->cuerpoFisica->GetWorldCenter();					//optimizar, evitar el llamado a la funcion
		b2Vec2 velocidad 	= gota->cuerpoFisica->GetLinearVelocity();				//optimizar, evitar el llamado a la funcion
		gotaPos->posicion.x	= multiplicador * posicion.x;
		gotaPos->posicion.y	= multiplicador * posicion.y;
		gotaProps->vx		= multiplicador * velocidad.x;
		gotaProps->vy		= multiplicador * velocidad.y;
		gotaProps->cambioX 	= 0.0f;
		gotaProps->cambioY 	= 0.0f;
		SI16 indiceCol		= (posicion.x-_areaCajas.x)/_dimensionesCajas;
		SI16 indiceFil		= (posicion.y-_areaCajas.y)/_dimensionesCajas;
		if(indiceCol>0 && indiceCol<ultimaColumna && indiceFil>0 && indiceFil<ultimaFila){
			//agregar a la caja correspondiente
			gotaPos->iCelda	= (_columnas*(indiceFil))+(indiceCol); NBASSERT(gotaPos->iCelda>=0 && gotaPos->iCelda<_indicesGotasPorCaja.conteo)
			((AUArregloNativoMutableP<UI16>*)_indicesGotasPorCaja.elemento[(_columnas*(indiceFil-1))+(indiceCol-1)])->agregarElemento(iReg);
			((AUArregloNativoMutableP<UI16>*)_indicesGotasPorCaja.elemento[(_columnas*(indiceFil-1))+(indiceCol)])->agregarElemento(iReg);
			((AUArregloNativoMutableP<UI16>*)_indicesGotasPorCaja.elemento[(_columnas*(indiceFil-1))+(indiceCol+1)])->agregarElemento(iReg);
			((AUArregloNativoMutableP<UI16>*)_indicesGotasPorCaja.elemento[(_columnas*(indiceFil))+(indiceCol-1)])->agregarElemento(iReg);
			((AUArregloNativoMutableP<UI16>*)_indicesGotasPorCaja.elemento[(_columnas*(indiceFil))+(indiceCol)])->agregarElemento(iReg);
			((AUArregloNativoMutableP<UI16>*)_indicesGotasPorCaja.elemento[(_columnas*(indiceFil))+(indiceCol+1)])->agregarElemento(iReg);
			((AUArregloNativoMutableP<UI16>*)_indicesGotasPorCaja.elemento[(_columnas*(indiceFil+1))+(indiceCol-1)])->agregarElemento(iReg);
			((AUArregloNativoMutableP<UI16>*)_indicesGotasPorCaja.elemento[(_columnas*(indiceFil+1))+(indiceCol)])->agregarElemento(iReg);
			((AUArregloNativoMutableP<UI16>*)_indicesGotasPorCaja.elemento[(_columnas*(indiceFil+1))+(indiceCol+1)])->agregarElemento(iReg);
		} else {
			//eliminar gotas fuera de la grilla
			this->privEliminarGotaEnIndice(iReg);
		}
	}
	//PRINTF_INFO("%.4f%% CPU organizando caja de gotas\n", (float)(100.0f*(float)(clock()-tINi)/(float)CLOCKS_PER_SEC));
	//debug_validarGrillaAgrupadora(); //ADVERTENCIA, afecta el rendimiento si se deja activada esta validacion debug
	//calcular las fuerzas producidas por los liquidos
	//tINi = clock();
	//SI16 conteoComparacionesGotas = 0, conteoComparacionesEfectivas = 0;
	float radioIdealAlCuadrado = (AU_SIMULADOR_LIQUIDOS_RADIO_IDEAL*AU_SIMULADOR_LIQUIDOS_RADIO_IDEAL);
	for(iReg=0; iReg<_conteoRegistrosGotasRevisar; iReg++){
		STLiquidoGota* gota	= &_gotas.elemento[iReg];
		STLiquidoGotaPos* gotaPos	= &_gotasPos.elemento[iReg];
		STLiquidoGotaProps* gotaProps	= &_gotasProps.elemento[iReg];
		if(!gota->cuerpoFisica) continue;
		//recorrer cuadrantes vecinos, actualizar distancias y precalcular presion
		SI16 iIndice;
		STLiquidoGotaPos* otraGota;
		float difX, difY, sumaCuadrados, estaDistancia, unoMenosQ;
		float p = 0.0f;
		float pCercano = 0.0f;
		AUArregloNativoMutableP<UI16>* indicesGotasVecinas = (AUArregloNativoMutableP<UI16>*)_indicesGotasPorCaja.elemento[gotaPos->iCelda];
		for(iIndice=indicesGotasVecinas->conteo-1; iIndice>=0; iIndice--){
			//conteoComparacionesGotas++;
			otraGota			= &_gotasPos.elemento[indicesGotasVecinas->elemento[iIndice]];
			if(gotaPos==otraGota) continue;
			difX				= otraGota->posicion.x - gotaPos->posicion.x;
			difY				= otraGota->posicion.y - gotaPos->posicion.y;
			sumaCuadrados		= (difX * difX) + (difY * difY);
			otraGota->tmpDistanciaConEsteVecino = 0.0f;
			//
			if(sumaCuadrados>=radioIdealAlCuadrado) continue;
			estaDistancia		= sqrtf(sumaCuadrados); if(estaDistancia<b2_epsilon) estaDistancia = AU_SIMULADOR_LIQUIDOS_RADIO_IDEAL - 0.01f;
			unoMenosQ			= 1.0f - (estaDistancia / AU_SIMULADOR_LIQUIDOS_RADIO_IDEAL);
			p					= (p + unoMenosQ * unoMenosQ);
			pCercano			= (pCercano + unoMenosQ * unoMenosQ * unoMenosQ);
			otraGota->tmpDistanciaConEsteVecino = estaDistancia;
			//conteoComparacionesEfectivas++;
		}
		//
		float presion				= (p - 5.0f) / 2.0f;	//presion en terminos normales
		float presionCercano		= pCercano / 2.0f;		//presion en terminos de particulas cercanas
		float cambioX				= 0.0f;
		float cambioY				= 0.0f;
		//recorrer vecinos y calcular presion
		for(iIndice=indicesGotasVecinas->conteo-1; iIndice>=0; iIndice--){
			STLiquidoGotaPos* otraGota	=  &_gotasPos.elemento[indicesGotasVecinas->elemento[iIndice]];
			STLiquidoGotaProps* otraGotaProps	=  &_gotasProps.elemento[indicesGotasVecinas->elemento[iIndice]];
			if(otraGota->tmpDistanciaConEsteVecino==0.0f) continue;
			float difX				= otraGota->posicion.x - gotaPos->posicion.x;
			float difY				= otraGota->posicion.y - gotaPos->posicion.y;
			float q					= otraGota->tmpDistanciaConEsteVecino / AU_SIMULADOR_LIQUIDOS_RADIO_IDEAL;
			float oneminusq			= 1.0f-q;
			float factor			= oneminusq * (presion + presionCercano * oneminusq) / (2.0f*otraGota->tmpDistanciaConEsteVecino);
			float dx				= difX * factor;
			float dy				= difY * factor;
			float relvx				= otraGotaProps->vx - gotaProps->vx;
			float relvy				= otraGotaProps->vy - gotaProps->vy;
			factor					= AU_SIMULADOR_LIQUIDOS_BISCOSIDAD_GOTA * oneminusq * segundosTranscurridos;
			dx						-= relvx * factor;
			dy						-= relvy * factor;
			otraGotaProps->cambioX	+= dx;
			otraGotaProps->cambioY	+= dy;
			cambioX					-= dx;
			cambioY					-= dy;
		}
		//PRINTF_INFO("Gota interactua con %d vecinas\n", (SI32)_tmpGotasVecinas.conteo);
		gotaProps->cambioX += cambioX;
		gotaProps->cambioY += cambioY;
		//_gotas.elemento[iReg] = gota;
	}
	//PRINTF_INFO("%d comparaciones con gotas (%.1f%% efectivas)\n", conteoComparacionesGotas, 100.0f*(float)conteoComparacionesEfectivas/(float)conteoComparacionesGotas);
	//PRINTF_INFO("%.4f%% CPU acumulando fuerzas de gotas\n", (float)(100.0f*(float)(clock()-tINi)/(float)CLOCKS_PER_SEC));
	//aplicar las fuerzas resultantes
	//tINi = clock();
	for(iReg=0; iReg<_conteoRegistrosGotasRevisar; iReg++){ //iGota++???
		STLiquidoGota* gota = &(_gotas.elemento[iReg]);
		STLiquidoGotaProps* gotaProps	= &_gotasProps.elemento[iReg];
		if(!gota->cuerpoFisica) continue;
		b2Vec2 velocidad 	= gota->cuerpoFisica->GetLinearVelocity();				//optimizar, evitar el llamado a la funcion
		velocidad.x 		+= gotaProps->cambioX / (multiplicador * segundosTranscurridos);
		velocidad.y 		+= gotaProps->cambioY / (multiplicador * segundosTranscurridos);
		gota->cuerpoFisica->SetLinearVelocity(velocidad);
	}
	//PRINTF_INFO("%.4f%% CPU aplicando fuerzas de gotas\n", (float)(100.0f*(float)(clock()-tINi)/(float)CLOCKS_PER_SEC));
	AU_GESTOR_PILA_LLAMADAS_POP_3
}*/

/*void AUSimuladorLiquidos::privSimularFisicaLiquidosComparandoConCeldasVecinasAcumulando(float segundosTranscurridos){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUSimuladorLiquidos::privSimularFisicaLiquidosComparandoConCeldasVecinasAcumulando")
	float multiplicador = AU_SIMULADOR_LIQUIDOS_RADIO_IDEAL / AU_SIMULADOR_LIQUIDOS_RADIO_GOTA_LIQUIDO;
	//Actualizar cache de gotas y grillas
	//clock_t tINi = clock();
	//Vaciar las cajas de indices (que ayudan a reducir las comparaciones entre gotas)
	SI16 iArr;
	for(iArr=_indicesGotasPorCaja.conteo-1; iArr>=0; iArr--){
		((AUArregloNativoMutableP<UI16>*)_indicesGotasPorCaja.elemento[iArr])->vaciar();
	}
	//Formatear todas las gotas y agregar en sus 9 cajas adyacentes
	SI16 ultimaFila				= _filas-1;
	SI16 ultimaColumna			= _columnas-1;
	SI16 iGota;
	for(iGota=0; iGota<_conteoRegistrosGotasRevisar; iGota++){
		NBASSERT(iGota>=0 && iGota<_gotas.conteo);
		STLiquidoGota* gota		= &_gotas.elemento[iGota];
		if(!gota->cuerpoFisica) continue;
		b2Vec2 posicion 	= gota->cuerpoFisica->GetWorldCenter();					//optimizar, evitar el llamado a la funcion
		b2Vec2 velocidad 	= gota->cuerpoFisica->GetLinearVelocity();				//optimizar, evitar el llamado a la funcion
		gota->x				= multiplicador * posicion.x;
		gota->y				= multiplicador * posicion.y;
		gota->vx			= multiplicador * velocidad.x;
		gota->vy			= multiplicador * velocidad.y;
		gota->cambioX 		= 0.0f;
		gota->cambioY 		= 0.0f;
		gota->p				= 0.0f;
		gota->pCercano		= 0.0f;
		SI16 indiceCol		= (posicion.x-_areaCajas.x)/_dimensionesCajas;
		SI16 indiceFil		= (posicion.y-_areaCajas.y)/_dimensionesCajas;
		if(indiceCol>0 && indiceCol<ultimaColumna && indiceFil>0 && indiceFil<ultimaFila){
			//agregar a las 9 cajas correspondientes
			gota->iCelda	= (_columnas*(indiceFil))+(indiceCol); NBASSERT(gota->iCelda>=0 && gota->iCelda<_indicesGotasPorCaja.conteo)
			((AUArregloNativoMutableP<UI16>*)_indicesGotasPorCaja.elemento[(_columnas*(indiceFil-1))+(indiceCol-1)])->agregarElemento(iGota);
			((AUArregloNativoMutableP<UI16>*)_indicesGotasPorCaja.elemento[(_columnas*(indiceFil-1))+(indiceCol)])->agregarElemento(iGota);
			((AUArregloNativoMutableP<UI16>*)_indicesGotasPorCaja.elemento[(_columnas*(indiceFil-1))+(indiceCol+1)])->agregarElemento(iGota);
			((AUArregloNativoMutableP<UI16>*)_indicesGotasPorCaja.elemento[(_columnas*(indiceFil))+(indiceCol-1)])->agregarElemento(iGota);
			((AUArregloNativoMutableP<UI16>*)_indicesGotasPorCaja.elemento[(_columnas*(indiceFil))+(indiceCol)])->agregarElemento(iGota);
			((AUArregloNativoMutableP<UI16>*)_indicesGotasPorCaja.elemento[(_columnas*(indiceFil))+(indiceCol+1)])->agregarElemento(iGota);
			((AUArregloNativoMutableP<UI16>*)_indicesGotasPorCaja.elemento[(_columnas*(indiceFil+1))+(indiceCol-1)])->agregarElemento(iGota);
			((AUArregloNativoMutableP<UI16>*)_indicesGotasPorCaja.elemento[(_columnas*(indiceFil+1))+(indiceCol)])->agregarElemento(iGota);
			((AUArregloNativoMutableP<UI16>*)_indicesGotasPorCaja.elemento[(_columnas*(indiceFil+1))+(indiceCol+1)])->agregarElemento(iGota);
		} else {
			//eliminar gotas fuera de la grilla
			this->privEliminarGotaEnIndice(iGota);
		}
	}
	//PRINTF_INFO("%.2f%% CPU organizando caja de gotas\n", (float)(100.0f*(float)(clock()-tINi)/(float)CLOCKS_PER_SEC));
	//debug_validarGrillaAgrupadora(); //ADVERTENCIA, afecta el rendimiento si se deja activada esta validacion debug
	//calcular las fuerzas producidas por los liquidos
	//tINi = clock();
	//SI16 conteoComparacionesGotas = 0;
	//Identificar las relaciones  entre las gotas adyacentes
	_tmpRelaciones.vaciar();
	float radioIdealAlCuadrado = (AU_SIMULADOR_LIQUIDOS_RADIO_IDEAL*AU_SIMULADOR_LIQUIDOS_RADIO_IDEAL);
	for(iGota=0; iGota<_conteoRegistrosGotasRevisar; iGota++){
		NBASSERT(iGota>=0 && iGota<_gotas.conteo);
		STLiquidoGota* gota	= &_gotas.elemento[iGota];
		if(!gota->cuerpoFisica) continue;
		AUArregloNativoMutableP<UI16>* indicesGotasVecinas = (AUArregloNativoMutableP<UI16>*)_indicesGotasPorCaja.elemento[gota->iCelda]; 
		SI32 iIndice;
		for(iIndice=indicesGotasVecinas->conteo-1; iIndice>=0; iIndice--){
			SI32 iVecina						= indicesGotasVecinas->elemento[iIndice]; NBASSERT(iVecina>=0 && iVecina<_gotas.conteo)
			if(iVecina<=iGota) continue;		//Ignorar las gotas previas (se asume que ya se han acumulado en los ciclos anteriores)
			STLiquidoGota* otraGota				= &_gotas.elemento[iVecina];
			float difX							= otraGota->x - gota->x;
			float difY							= otraGota->y - gota->y;
			float sumaCuadrados					= (difX * difX) + (difY * difY);
			//
			if(sumaCuadrados>=radioIdealAlCuadrado) continue;//Ignorar gotas demasiado lejanas
			float distanciaMutua			= sqrtf(sumaCuadrados); if(distanciaMutua<b2_epsilon) distanciaMutua = AU_SIMULADOR_LIQUIDOS_RADIO_IDEAL - 0.01f;
			float unoMenosQ					= 1.0f - (distanciaMutua / AU_SIMULADOR_LIQUIDOS_RADIO_IDEAL);
			gota->p							= (gota->p				+ unoMenosQ * unoMenosQ);
			gota->pCercano					= (gota->pCercano		+ unoMenosQ * unoMenosQ * unoMenosQ);
			otraGota->p						= (otraGota->p			+ unoMenosQ * unoMenosQ);
			otraGota->pCercano				= (otraGota->pCercano	+ unoMenosQ * unoMenosQ * unoMenosQ);
			STGotaRelacion datosRelacion;
			datosRelacion.iGotaA			= iGota;
			datosRelacion.iGotaB			= iVecina;
			datosRelacion.distanciaMutua	= distanciaMutua;
			_tmpRelaciones.agregarElemento(datosRelacion);
		}
	}
	//Calcular presiones
	for(iGota=0; iGota<_conteoRegistrosGotasRevisar; iGota++){
		NBASSERT(iGota>=0 && iGota<_gotas.conteo);
		STLiquidoGota* gota	= &_gotas.elemento[iGota];
		if(gota->cuerpoFisica){
			gota->p			= (gota->p - 5.0f) / 2.0f;	//presion en terminos normales
			gota->pCercano	= gota->pCercano / 2.0f;	//presion en terminos de particulas cercanas
		}
	}
	//Acumular los cambios
	SI32 iRel;
	for(iRel=_tmpRelaciones.conteo-1; iRel>=0; iRel--){
		NBASSERT(iRel>=0 && iRel<_tmpRelaciones.conteo);
		STGotaRelacion datosRelacion = _tmpRelaciones.elemento[iRel];
		STLiquidoGota* gota		= &_gotas.elemento[datosRelacion.iGotaA];
		STLiquidoGota* otraGota	= &_gotas.elemento[datosRelacion.iGotaB];
		float difX_AB			= otraGota->x - gota->x;
		float difY_AB			= otraGota->y - gota->y;
		float difX_BA			= gota->x - otraGota->x;
		float difY_BA			= gota->y - otraGota->y;
		float q					= datosRelacion.distanciaMutua / AU_SIMULADOR_LIQUIDOS_RADIO_IDEAL;
		float oneminusq			= 1.0f-q;
		float factorA			= oneminusq * (gota->p + gota->pCercano * oneminusq) / (2.0f*datosRelacion.distanciaMutua);
		float factorB			= oneminusq * (otraGota->p + otraGota->pCercano * oneminusq) / (2.0f*datosRelacion.distanciaMutua);
		float dx_AB				= difX_AB * factorA;
		float dy_AB				= difY_AB * factorA;
		float dx_BA				= difX_BA * factorB;
		float dy_BA				= difY_BA * factorB;
		float relvx_AB			= otraGota->vx - gota->vx;
		float relvy_AB			= otraGota->vy - gota->vy;
		float relvx_BA			= gota->vx - otraGota->vx;
		float relvy_BA			= gota->vy - otraGota->vy;
		factorA					= AU_SIMULADOR_LIQUIDOS_BISCOSIDAD_GOTA * oneminusq * segundosTranscurridos;
		factorB					= AU_SIMULADOR_LIQUIDOS_BISCOSIDAD_GOTA * oneminusq * segundosTranscurridos;
		dx_AB					-= relvx_AB * factorA;
		dy_AB					-= relvy_AB * factorA;
		dx_BA					-= relvx_BA * factorB;
		dy_BA					-= relvy_BA * factorB;
		otraGota->cambioX		+= (dx_AB - dx_BA);
 		otraGota->cambioY		+= (dy_AB - dy_BA);
		gota->cambioX			+= (dx_BA - dx_AB);
		gota->cambioY			+= (dy_BA - dy_AB);
	}
	//Aplicar cambios
	for(iGota=0; iGota<_conteoRegistrosGotasRevisar; iGota++){
		STLiquidoGota* gota = &(_gotas.elemento[iGota]);
		if(!gota->cuerpoFisica) continue;
		b2Vec2 velocidad 	= gota->cuerpoFisica->GetLinearVelocity();				//optimizar, evitar el llamado a la funcion
		velocidad.x 		+= gota->cambioX / (multiplicador * segundosTranscurridos);
		velocidad.y 		+= gota->cambioY / (multiplicador * segundosTranscurridos);
		gota->cuerpoFisica->SetLinearVelocity(velocidad);
	}
	//PRINTF_INFO("%.2f%% CPU aplicando fuerzas de gotas\n", (float)(100.0f*(float)(clock()-tINi)/(float)CLOCKS_PER_SEC));
	AU_GESTOR_PILA_LLAMADAS_POP_3
}*/

//
AUOBJMETODOS_CLASESID_UNICLASE(AUSimuladorLiquidos)
AUOBJMETODOS_CLASESNOMBRES_UNICLASE(AUSimuladorLiquidos, "AUSimuladorLiquidos")
AUOBJMETODOS_CLONAR_NULL(AUSimuladorLiquidos)


/*
 http://jbox2d.svn.sourceforge.net/viewvc/jbox2d/trunk/src/org/jbox2d/testbed/tests/LiquidTest.java?view=markup
private void applyLiquidConstraint(float deltaT) {
	/ *
	 * Unfortunately, this simulation method is not actually scale
	 * invariant, and it breaks down for rad < ~3 or so.  So we need
	 * to scale everything to an ideal rad and then scale it back after.
	 * /
	final float idealRad = 50.0f;
	float multiplier = idealRad / rad;
	
	float[] xchange = new float[liquid.length];
	float[] ychange = new float[liquid.length];
	Arrays.fill(xchange,0.0f);
	Arrays.fill(ychange, 0.0f);
	
	float[] xs = new float[liquid.length];
	float[] ys = new float[liquid.length];
	float[] vxs = new float[liquid.length];
	float[] vys = new float[liquid.length];
	for (int i=0; i<liquid.length; ++i) {
		xs[i] = multiplier*liquid[i].m_sweep.c.x;
		ys[i] = multiplier*liquid[i].m_sweep.c.y;
		vxs[i] = multiplier*liquid[i].m_linearVelocity.x;
		vys[i] = multiplier*liquid[i].m_linearVelocity.y;
	}
	
	for(int i = 0; i < liquid.length; i++) {
		// Populate the neighbor list from the 9 proximate cells
		ArrayList<Integer> neighbors = new ArrayList<Integer>();
		int hcell = hashX(liquid[i].m_sweep.c.x);
		int vcell = hashY(liquid[i].m_sweep.c.y);
		for(int nx = -1; nx < 2; nx++) {
			for(int ny = -1; ny < 2; ny++) {
				int xc = hcell + nx;
				int yc = vcell + ny;
				if(xc > -1 && xc < hashWidth && yc > -1 && yc < hashHeight && hash[xc][yc].size() > 0) {
					for(int a = 0; a < hash[xc][yc].size(); a++) {
						Integer ne = (Integer)hash[xc][yc].get(a);
						if(ne != null && ne.intValue() != i) neighbors.add(ne);
					}
				}
			}
		}
		
		// Particle pressure calculated by particle proximity
		// Pressures = 0 iff all particles within range are idealRad distance away
		float[] vlen = new float[neighbors.size()];
		float p = 0.0f;
		float pnear = 0.0f;
		for(int a = 0; a < neighbors.size(); a++) {
			Integer n = (Integer)neighbors.get(a);
			int j = n.intValue();
			float vx = xs[j]-xs[i];//liquid[j].m_sweep.c.x - liquid[i].m_sweep.c.x;
			float vy = ys[j]-ys[i];//liquid[j].m_sweep.c.y - liquid[i].m_sweep.c.y;
			
			//early exit check
			if(vx > -idealRad && vx < idealRad && vy > -idealRad && vy < idealRad) {
				float vlensqr = (vx * vx + vy * vy);
				//within idealRad check
				if(vlensqr < idealRad*idealRad) {
					vlen[a] = (float)Math.sqrt(vlensqr);
					if (vlen[a] < Settings.EPSILON) vlen[a] = idealRad-.01f;
					float oneminusq = 1.0f-(vlen[a] / idealRad);
					p = (p + oneminusq*oneminusq);
					pnear = (pnear + oneminusq*oneminusq*oneminusq);
				} else {
					vlen[a] = Float.MAX_VALUE;
				}
			}
		}
		
		// Now actually apply the forces
		//System.out.println(p);
		float pressure = (p - 5F) / 2.0F; //normal pressure term
		float presnear = pnear / 2.0F; //near particles term
		float changex = 0.0F;
		float changey = 0.0F;
		for(int a = 0; a < neighbors.size(); a++) {
			Integer n = (Integer)neighbors.get(a);
			int j = n.intValue();
			float vx = xs[j]-xs[i];//liquid[j].m_sweep.c.x - liquid[i].m_sweep.c.x;
			float vy = ys[j]-ys[i];//liquid[j].m_sweep.c.y - liquid[i].m_sweep.c.y;
			if(vx > -idealRad && vx < idealRad && vy > -idealRad && vy < idealRad) {
				if(vlen[a] < idealRad) {
					float q = vlen[a] / idealRad;
					float oneminusq = 1.0f-q;
					float factor = oneminusq * (pressure + presnear * oneminusq) / (2.0F*vlen[a]);
					float dx = vx * factor;
					float dy = vy * factor;
					float relvx = vxs[j] - vxs[i];
					float relvy = vys[j] - vys[i];
					factor = visc * oneminusq * deltaT;
					dx -= relvx * factor;
					dy -= relvy * factor;
					//liquid[j].m_xf.position.x += dx;// *deltaT*deltaT;
					//liquid[j].m_xf.position.y += dy;// *deltaT*deltaT;
					//liquid[j].m_linearVelocity.x += dx;/// deltaT;//deltaT;
					//liquid[j].m_linearVelocity.y += dy;/// deltaT;//deltaT;
					xchange[j] += dx;
					ychange[j] += dy;
					changex -= dx;
					changey -= dy;
				}
			}
		}
		//liquid[i].m_xf.position.x += changex;// *deltaT*deltaT;
		//liquid[i].m_xf.position.y += changey;// *deltaT*deltaT;
		//liquid[i].m_linearVelocity.x += changex;///deltaT;//deltaT;
		//liquid[i].m_linearVelocity.y += changey;///deltaT;//deltaT;
		xchange[i] += changex;
		ychange[i] += changey;
	}
	//multiplier *= deltaT;
	for (int i=0; i<liquid.length; ++i) {
		liquid[i].m_xf.position.x += xchange[i] / multiplier;
		liquid[i].m_xf.position.y += ychange[i] / multiplier;
		liquid[i].m_linearVelocity.x += xchange[i] / (multiplier*deltaT);
		liquid[i].m_linearVelocity.y += ychange[i] / (multiplier*deltaT);
	}
	
}
 */

#endif //#ifndef CONFIG_NB_UNSUPPORT_BOX2D
