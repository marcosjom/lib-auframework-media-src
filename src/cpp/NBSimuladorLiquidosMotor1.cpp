//
//  NBSimuladorLiquidosMotor1.cpp
//  Gameplay_Mac
//
//  Created by Nicaragua Binary on 05/07/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkMediaStdAfx.h"
#include "NBSimuladorLiquidosMotor1.h"

#ifndef CONFIG_NB_UNSUPPORT_BOX2D

NBSimuladorLiquidosMotor1::NBSimuladorLiquidosMotor1(SI8 mundoFisica, AUEscenaContenedor* contenedorDebug, NBSimuladorLiquidosMotorEscuchadorI* escuchador, float x, float y, UI16 columnas, UI16 filas, float dimesionesCajas) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("NBSimuladorLiquidosMotor1::NBSimuladorLiquidosMotor1")
	_mundoFisica					= mundoFisica;
	_contenedorInfoDebug			= contenedorDebug;
	_escuchador						= escuchador;
	_filas							= filas; NBASSERT(_filas>0)
	_columnas						= columnas; NBASSERT(_columnas>0)
	_conteoRegistrosGotasRevisar	= 0;
	_consultaColision.gotaConsultando = NULL;
	_dimensionesCajasLiq			= dimesionesCajas * AU_MOTOR_LIQUIDOS_1_MUTIPLICADOR;
	_areaCajasLiq.x					= x * AU_MOTOR_LIQUIDOS_1_MUTIPLICADOR;
	_areaCajasLiq.y					= y * AU_MOTOR_LIQUIDOS_1_MUTIPLICADOR;
	_areaCajasLiq.ancho				= (float)_columnas * _dimensionesCajasLiq;
	_areaCajasLiq.alto				= (float)_filas * _dimensionesCajasLiq;
	_gotas							= new AUArregloNativoMutableP<STLiquidoGotaProps>(256); NB_DEFINE_NOMBRE_PUNTERO(_gotas, "NBSimuladorLiquidosMotor1::_gotas")
	_tmpRelaciones					= new AUArregloNativoMutableP<STGotaRelacion>(256*40); NB_DEFINE_NOMBRE_PUNTERO(_tmpRelaciones, "NBSimuladorLiquidosMotor1::_tmpRelaciones")
	_debugFiguras					= new AUArregloMutable(256); NB_DEFINE_NOMBRE_PUNTERO(_debugFiguras, "NBSimuladorLiquidosMotor1::_debugFiguras")
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

NBSimuladorLiquidosMotor1::~NBSimuladorLiquidosMotor1(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("NBSimuladorLiquidosMotor1::~NBSimuladorLiquidosMotor1")
	_gotas->liberar(NB_RETENEDOR_THIS); _gotas = NULL;
	_tmpRelaciones->liberar(NB_RETENEDOR_THIS); _tmpRelaciones = NULL;
	_debugFiguras->liberar(NB_RETENEDOR_THIS); _debugFiguras = NULL;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

//

NBPunto NBSimuladorLiquidosMotor1::posicionGota(UI16 indice){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("NBSimuladorLiquidosMotor1::posicionGota")
	NBASSERT(indice<_conteoRegistrosGotasRevisar)
	NBASSERT(_gotas->elemento[indice].registroOcupado)
	NBPunto pos = _gotas->elemento[indice].posLiq;
	pos.x /= AU_MOTOR_LIQUIDOS_1_MUTIPLICADOR;
	pos.y /= AU_MOTOR_LIQUIDOS_1_MUTIPLICADOR;
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return pos;
}

void NBSimuladorLiquidosMotor1::gotaAgregada(UI16 indice, float xPos, float yPos, float xVel, float yVel, float radioLiquido, float radioFisico, float biscosidad){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("NBSimuladorLiquidosMotor1::gotaAgregada")
	STLiquidoGotaProps datosGota;
	datosGota.registroOcupado	= true;
	//Propiedades
	datosGota.posLiq.x			= xPos * AU_MOTOR_LIQUIDOS_1_MUTIPLICADOR;				//En coordenadas de grilla de simuladorLiq (aplicado el Multiplicador)
	datosGota.posLiq.y			= yPos * AU_MOTOR_LIQUIDOS_1_MUTIPLICADOR;				//En coordenadas de grilla de simuladorLiq (aplicado el Multiplicador)
	datosGota.velLiq.x			= xVel * AU_MOTOR_LIQUIDOS_1_MUTIPLICADOR;				//En coordenadas de grilla de simuladorLiq (aplicado el Multiplicador)
	datosGota.velLiq.y			= yVel * AU_MOTOR_LIQUIDOS_1_MUTIPLICADOR;				//En coordenadas de grilla de simuladorLiq (aplicado el Multiplicador)
	datosGota.radioLiquido		= radioLiquido * AU_MOTOR_LIQUIDOS_1_MUTIPLICADOR;		//Multiplicado por AU_MOTOR_LIQUIDOS_1_MUTIPLICADOR
	datosGota.radioSolido		= radioFisico * AU_MOTOR_LIQUIDOS_1_MUTIPLICADOR;
	datosGota.biscosidad		= biscosidad;
	//Temporales de simulacion
	datosGota.coordCelda.x		= 1;			//Si diferente de cero entonces el registro esta ocupado
	datosGota.coordCelda.y		= 1;			//Si diferente de cero entonces el registro esta ocupado
	datosGota.cambioLiq.x		= 0.0f;			//En coordenadas de grilla de simuladorLiq (aplicado el Multiplicador)
	datosGota.cambioLiq.y		= 0.0f;			//En coordenadas de grilla de simuladorLiq (aplicado el Multiplicador)
	datosGota.p					= 0.0f;			//Presion acumulada
	datosGota.pCercano			= 0.0f;			//Presion mas cercano
	if(indice<_gotas->conteo){
		NBASSERT(!_gotas->elemento[indice].registroOcupado)
		_gotas->elemento[indice] = datosGota;
		if(_conteoRegistrosGotasRevisar<=indice) _conteoRegistrosGotasRevisar = indice+1;
	} else if(indice==_gotas->conteo){
		_gotas->agregarElemento(datosGota);
		_conteoRegistrosGotasRevisar = _gotas->conteo;
		//FIGURA DEBUG
		//AUEscenaFigura* debugFigura = new AUEscenaFigura(ENEscenaFiguraTipo_Linea); NB_DEFINE_NOMBRE_PUNTERO(debugFigura, "NBSimuladorLiquidosMotor1::debugFigura")
		//debugFigura->establecerVisible(false);
		//debugFigura->agregarCoordenadaLocal(0.0f, 0.0f);
		//debugFigura->agregarCoordenadaLocal(1.0f, 0.0f);
		//_debugFiguras->agregarElemento(debugFigura);
		//_contenedorInfoDebug->agregarObjetoEscena(debugFigura);
		//debugFigura->liberar(NB_RETENEDOR_THIS);
	} else {
		NBASSERT(false) //El indice no es valido
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void NBSimuladorLiquidosMotor1::gotaQuitada(UI16 indice){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("NBSimuladorLiquidosMotor1::gotaQuitada")
	NBASSERT(indice<_conteoRegistrosGotasRevisar)
	_gotas->elemento[indice].registroOcupado	= false;
	_gotas->elemento[indice].coordCelda.x		= 0;
	_gotas->elemento[indice].coordCelda.y		= 0;
	//
	//AUEscenaFigura* figDebug = (AUEscenaFigura*)_debugFiguras->elemento[indice];
	//figDebug->establecerVisible(false);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void NBSimuladorLiquidosMotor1::simularFuerzasLiquidos(float segundosTranscurridos){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("NBSimuladorLiquidosMotor1::simularFuerzasLiquidos")
	UI32 iGota, iRel;
	SI16 ultimaFila				= _filas-1;
	SI16 ultimaColumna			= _columnas-1;
	//Preparar registros
	for(iGota=0; iGota<_conteoRegistrosGotasRevisar; iGota++){	
		STLiquidoGotaProps* gota = &(_gotas->elemento[iGota]);
		if(!gota->registroOcupado) continue;
		SI32 indiceCol			= (gota->posLiq.x-_areaCajasLiq.x)/_dimensionesCajasLiq;
		SI32 indiceFil			= (gota->posLiq.y-_areaCajasLiq.y)/_dimensionesCajasLiq;
		if(indiceCol>0 && indiceCol<ultimaColumna && indiceFil>0 && indiceFil<ultimaFila){
			gota->coordCelda.x	= indiceCol;
			gota->coordCelda.y	= indiceFil;
			gota->cambioLiq.x	= 0.0f;
			gota->cambioLiq.y	= 0.0f;
			gota->p				= 0.0f;
			gota->pCercano		= 0.0f;
		} else {
			//PRINTF_INFO("Quitando gota %d desde motor (fuera de limites) col(%d) fil(%d) ultCol(%d) ultFil(%d)\n", iGota, indiceCol, indiceFil, ultimaColumna, ultimaFila);
			//eliminar gotas fuera de la grilla
			gota->registroOcupado = false;
			//Determinar el nuevo indice maximo (si es necesario)
			if(_conteoRegistrosGotasRevisar==(iGota+1)){
				_conteoRegistrosGotasRevisar = 0;
				SI32 iBsq;
				for(iBsq=iGota-1; iBsq>=0 && _conteoRegistrosGotasRevisar==0; iBsq--){
					if(_gotas->elemento[iBsq].registroOcupado){
						_conteoRegistrosGotasRevisar = (iBsq+1);
					}
				}
			}
			if(_escuchador != NULL){
				_escuchador->gotaQuitadaDeMotorPorSalirDeLimites(iGota);
			}
		}
	}
	//Precachear relaciones
	_tmpRelaciones->vaciar();
	for(iGota=0; iGota<_conteoRegistrosGotasRevisar; iGota++){
		STLiquidoGotaProps* gota = &(_gotas->elemento[iGota]);
		if(gota->coordCelda.x==0) continue; //saltar registro vacio
		UI16 xCeldaIzq	= gota->coordCelda.x-1;
		UI16 xCeldDer	= gota->coordCelda.x+1;
		UI16 yCeldaAba	= gota->coordCelda.y-1;
		UI16 yCeldArr	= gota->coordCelda.y+1;
		SI16 iOtraGota;
		for(iOtraGota=iGota+1; iOtraGota<_conteoRegistrosGotasRevisar; iOtraGota++){
			STLiquidoGotaProps* otraGota	= &(_gotas->elemento[iOtraGota]);
			if(otraGota->coordCelda.x==0) continue; //saltar registro vacio
			if(otraGota->coordCelda.x<xCeldaIzq || otraGota->coordCelda.x>xCeldDer || otraGota->coordCelda.y<yCeldaAba || otraGota->coordCelda.y>yCeldArr) continue; //saltar gota fuera de las 9 celdas alrededor
			float diffX			= otraGota->posLiq.x - gota->posLiq.x;
			float diffY			= otraGota->posLiq.y - gota->posLiq.y;
			float sumaCuadrados	= (diffX*diffX)+(diffY*diffY);
			if(sumaCuadrados>=gota->radioLiquido*gota->radioLiquido && sumaCuadrados>=otraGota->radioLiquido*otraGota->radioLiquido) continue; //Ignorar gotas demasiado lejanas
			STGotaRelacion datosRelacion;
			datosRelacion.iGotaA				= iGota;
			datosRelacion.iGotaB				= iOtraGota;
			datosRelacion.distanciaMutua		= sqrtf(sumaCuadrados); if(datosRelacion.distanciaMutua<b2_epsilon) datosRelacion.distanciaMutua = (gota->radioLiquido>otraGota->radioLiquido?gota->radioLiquido:otraGota->radioLiquido) - 0.01f;
			_tmpRelaciones->agregarElemento(datosRelacion);
		}
	}
	//PRINTF_INFO("%d relaciones\n", (SI32)_tmpRelaciones->conteo);
	//Acumular presiones en base a relaciones
	for(iRel=0; iRel<_tmpRelaciones->conteo; iRel++){
		STGotaRelacion datosRelacion	= _tmpRelaciones->elemento[iRel];
		STLiquidoGotaProps* gotaA			= &(_gotas->elemento[datosRelacion.iGotaA]);
		if(datosRelacion.distanciaMutua<gotaA->radioLiquido){
			float unoMenosQA			= 1.0f - (datosRelacion.distanciaMutua / gotaA->radioLiquido);
			gotaA->p					= (gotaA->p			+ unoMenosQA * unoMenosQA);
			gotaA->pCercano				= (gotaA->pCercano	+ unoMenosQA * unoMenosQA * unoMenosQA);
		}
		STLiquidoGotaProps* gotaB			= &(_gotas->elemento[datosRelacion.iGotaB]);
		if(datosRelacion.distanciaMutua<gotaB->radioLiquido){
			float unoMenosQB			= 1.0f - (datosRelacion.distanciaMutua / gotaB->radioLiquido);
			gotaB->p					= (gotaB->p			+ unoMenosQB * unoMenosQB);
			gotaB->pCercano				= (gotaB->pCercano	+ unoMenosQB * unoMenosQB * unoMenosQB);
		}
	}
	//Aplicar presiones
	for(iGota=0; iGota<_conteoRegistrosGotasRevisar; iGota++){
		STLiquidoGotaProps* gota		= &(_gotas->elemento[iGota]);
		gota->p							= (gota->p - 5.0f) / 2.0f;	//presion en terminos normales
		gota->pCercano					= gota->pCercano / 2.0f;	//presion en terminos de particulas cercanas
	}
	//Acumular cambios en base a presiones
	for(iRel=0; iRel<_tmpRelaciones->conteo; iRel++){
		STGotaRelacion datosRelacion	= _tmpRelaciones->elemento[iRel];
		STLiquidoGotaProps* gota		= &(_gotas->elemento[datosRelacion.iGotaA]);
		STLiquidoGotaProps* otraGota	= &(_gotas->elemento[datosRelacion.iGotaB]);
		NBPunto posGotaA				= gota->posLiq;
		NBPunto posGotaB				= otraGota->posLiq;
		float dx_AB						= 0.0f;
		float dy_AB						= 0.0f;
		float dx_BA						= 0.0f;
		float dy_BA						= 0.0f;
		if(datosRelacion.distanciaMutua<gota->radioLiquido){
			float oneminusqA			= 1.0f-(datosRelacion.distanciaMutua/gota->radioLiquido);
			float difX_AB				= posGotaB.x - posGotaA.x;
			float difY_AB				= posGotaB.y - posGotaA.y;
			float factorA				= oneminusqA * (gota->p + gota->pCercano * oneminusqA) / (2.0f*datosRelacion.distanciaMutua);
			float relvx_AB				= otraGota->velLiq.x - gota->velLiq.x;
			float relvy_AB				= otraGota->velLiq.y - gota->velLiq.y;
			dx_AB						= difX_AB * factorA;
			dy_AB						= difY_AB * factorA;
			factorA						= gota->biscosidad * oneminusqA * segundosTranscurridos;
			dx_AB						-= relvx_AB * factorA;
			dy_AB						-= relvy_AB * factorA;
			
		}
		if(datosRelacion.distanciaMutua<gota->radioLiquido){
			float oneminusqB			= 1.0f-(datosRelacion.distanciaMutua/otraGota->radioLiquido);
			float difX_BA				= posGotaA.x - posGotaB.x;
			float difY_BA				= posGotaA.y - posGotaB.y;
			float factorB				= oneminusqB * (otraGota->p + otraGota->pCercano * oneminusqB) / (2.0f*datosRelacion.distanciaMutua);
			float relvx_BA				= gota->velLiq.x - otraGota->velLiq.x;
			float relvy_BA				= gota->velLiq.y - otraGota->velLiq.y;
			dx_BA						= difX_BA * factorB;
			dy_BA						= difY_BA * factorB;
			factorB						= otraGota->biscosidad * oneminusqB * segundosTranscurridos;
			dx_BA						-= relvx_BA * factorB;
			dy_BA						-= relvy_BA * factorB;
		}
		otraGota->cambioLiq.x			+= dx_AB;
		otraGota->cambioLiq.y			+= dy_AB;
		otraGota->cambioLiq.x			-= dx_BA;
		otraGota->cambioLiq.y			-= dy_BA;
		gota->cambioLiq.x				-= dx_AB;
		gota->cambioLiq.y				-= dy_AB;
		gota->cambioLiq.x				+= dx_BA;
		gota->cambioLiq.y				+= dy_BA;
	}
	//Detectar colision y aplicar cambios
	for(iGota=0; iGota<_conteoRegistrosGotasRevisar; iGota++){
		if(!_gotas->elemento[iGota].registroOcupado) continue;		//saltar registro vacio
		STLiquidoGotaProps* gota			= &(_gotas->elemento[iGota]);
		gota->velLiq.x						+= gota->cambioLiq.x / segundosTranscurridos;
		gota->velLiq.y						+= (gota->cambioLiq.y / segundosTranscurridos) - (9.8f*AU_MOTOR_LIQUIDOS_1_MUTIPLICADOR*segundosTranscurridos);
		NBPunto posDestinoPorVel;
		posDestinoPorVel.x					= gota->posLiq.x + (gota->velLiq.x * segundosTranscurridos);
		posDestinoPorVel.y					= gota->posLiq.y + (gota->velLiq.y * segundosTranscurridos);
		//
		if(gota->posLiq!=posDestinoPorVel){
			_consultaColision.iGota				= iGota;
			_consultaColision.gotaConsultando	= gota;
			_consultaColision.conteoColisiones	= 0;
			_consultaColision.fraccionColisionMasCercana = 1.01f;
			_consultaColision.posInicioLiq		= gota->posLiq;
			_consultaColision.posDestinoLiq		= posDestinoPorVel;
			NBGestorFisica::interseccionesConLinea(_mundoFisica, this, _consultaColision.posInicioLiq.x/AU_MOTOR_LIQUIDOS_1_MUTIPLICADOR, _consultaColision.posInicioLiq.y/AU_MOTOR_LIQUIDOS_1_MUTIPLICADOR, _consultaColision.posDestinoLiq.x/AU_MOTOR_LIQUIDOS_1_MUTIPLICADOR, _consultaColision.posDestinoLiq.y/AU_MOTOR_LIQUIDOS_1_MUTIPLICADOR);
			//if(_consultaColision.conteoColisiones>1) PRINTF_INFO("%d colisiones detectadas con gota\n", _consultaColision.conteoColisiones);
			if(_consultaColision.conteoColisiones!=0){
				gota->posLiq.x					= _consultaColision.posProductoColision.x * AU_MOTOR_LIQUIDOS_1_MUTIPLICADOR;
				gota->posLiq.y					= _consultaColision.posProductoColision.y * AU_MOTOR_LIQUIDOS_1_MUTIPLICADOR;
			} else {
				gota->posLiq.x					= posDestinoPorVel.x;
				gota->posLiq.y					= posDestinoPorVel.y;
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

float32 NBSimuladorLiquidosMotor1::ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float32 fraction){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUSimuladorLiquidos::ReportFixture")
	float32 r = 1.0f;
	_consultaColision.conteoColisiones++;
	NBASSERT(fraction>=0.0f && fraction<=1.0f)
	//PRINTF_INFO("Colision en fraccion %f Longitud de vector normal de colision: %f\n", fraction, normal.Length());
	if(r<_consultaColision.fraccionColisionMasCercana){
		_consultaColision.posProductoColision.x = point.x + (normal.x * _consultaColision.gotaConsultando->radioSolido/AU_MOTOR_LIQUIDOS_1_MUTIPLICADOR);
		_consultaColision.posProductoColision.y = point.y + (normal.y * _consultaColision.gotaConsultando->radioSolido/AU_MOTOR_LIQUIDOS_1_MUTIPLICADOR);
		//Modificar la velocidad en base a la normal de colision
		//ALGORITMO AJUSTADO CON RESTITUCION: desvia la velocidad en base al impacto y aplica restitucion
		//
		NBPunto velActual;
		velActual.x = _consultaColision.gotaConsultando->velLiq.x / AU_MOTOR_LIQUIDOS_1_MUTIPLICADOR;
		velActual.y = _consultaColision.gotaConsultando->velLiq.y / AU_MOTOR_LIQUIDOS_1_MUTIPLICADOR;
		float restitucion = 0.25f;
		float velBias = -restitucion * NBPUNTO_PRODUCTO_PUNTO(normal.x, normal.y, -velActual.x, -velActual.y);
		float proyeccionNormalSObreVelocidad = NBPUNTO_PRODUCTO_PUNTO(-velActual.x, -velActual.y, normal.x, normal.y);
		float lambda = -1.0f * (proyeccionNormalSObreVelocidad - velBias);
		velActual.x -= lambda * normal.x;
		velActual.y -= lambda * normal.y;
		_consultaColision.gotaConsultando->velLiq.x = velActual.x * AU_MOTOR_LIQUIDOS_1_MUTIPLICADOR;
		_consultaColision.gotaConsultando->velLiq.y = velActual.y * AU_MOTOR_LIQUIDOS_1_MUTIPLICADOR;
		//
		//Figura debug de colision
		//AUEscenaFigura* figDebug = (AUEscenaFigura*)_debugFiguras->elemento[_consultaColision.iGota];
		//figDebug->establecerVisible(true);
		//figDebug->moverVerticeHacia(0, point.x*ESCALA_PIXELES_POR_METRO_MUNDO_FISICA, point.y*ESCALA_PIXELES_POR_METRO_MUNDO_FISICA);
		//figDebug->moverVerticeHacia(1, point.x*ESCALA_PIXELES_POR_METRO_MUNDO_FISICA+velActual.x*ESCALA_PIXELES_POR_METRO_MUNDO_FISICA, point.y*ESCALA_PIXELES_POR_METRO_MUNDO_FISICA+velActual.y*ESCALA_PIXELES_POR_METRO_MUNDO_FISICA);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return r;
}


#endif //#ifndef CONFIG_NB_UNSUPPORT_BOX2D

