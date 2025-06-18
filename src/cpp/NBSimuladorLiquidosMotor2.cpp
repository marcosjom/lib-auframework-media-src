//
//  NBSimuladorLiquidosMotor2.cpp
//  Gameplay_Mac
//
//  Created by Nicaragua Binary on 05/07/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkMediaStdAfx.h"
#include "NBSimuladorLiquidosMotor2.h"

NBSimuladorLiquidosMotor2::NBSimuladorLiquidosMotor2(NBSimuladorLiquidosMotorEscuchadorI* escuchador, float x, float y, UI16 columnas, UI16 filas, float dimesionesCajas) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("NBSimuladorLiquidosMotor2::NBSimuladorLiquidosMotor2")
	_escuchador						= escuchador;
	_tamGrillaDensidad				= columnas>filas?columnas:filas; NBASSERT(_tamGrillaDensidad>0);
	_filas							= _tamGrillaDensidad; NBASSERT(_filas>0)
	_columnas						= _tamGrillaDensidad; NBASSERT(_columnas>0)
	_conteoRegistrosGotasRevisar	= 0;
	_dimensionesCajasLiq			= dimesionesCajas;
	_areaCajasLiq.x					= x;
	_areaCajasLiq.y					= y;
	_areaCajasLiq.ancho				= (float)_columnas * _dimensionesCajasLiq;
	_areaCajasLiq.alto				= (float)_filas * _dimensionesCajasLiq;
	_gotas							= new AUArregloNativoMutableP<STLiquidoGotaDensidad>(256); NB_DEFINE_NOMBRE_PUNTERO(_gotas, "NBSimuladorLiquidosMotor2::_gotas")
	//
	SI32 tamArreglosGrillas			= (_tamGrillaDensidad+2)*(_tamGrillaDensidad+2);
	_u								= new float[tamArreglosGrillas];	NB_DEFINE_NOMBRE_PUNTERO(_u, "NBSimuladorLiquidosMotor2::_u") //fuerza			
	_v								= new float[tamArreglosGrillas];	NB_DEFINE_NOMBRE_PUNTERO(_v, "NBSimuladorLiquidosMotor2::_v") //velocidad
	_dens							= new float[tamArreglosGrillas];	NB_DEFINE_NOMBRE_PUNTERO(_dens, "NBSimuladorLiquidosMotor2::_dens") //densidad
	_u_prev							= new float[tamArreglosGrillas];	NB_DEFINE_NOMBRE_PUNTERO(_u_prev, "NBSimuladorLiquidosMotor2::_u_prev") //fuerza previa
	_v_prev							= new float[tamArreglosGrillas];	NB_DEFINE_NOMBRE_PUNTERO(_v_prev, "NBSimuladorLiquidosMotor2::_v_prev") //velocidad previa
	_dens_prev						= new float[tamArreglosGrillas];	NB_DEFINE_NOMBRE_PUNTERO(_dens_prev, "NBSimuladorLiquidosMotor2::_dens_prev") //densidad previa
	memset(_u, 0, sizeof(float)*tamArreglosGrillas);
	memset(_v, 0, sizeof(float)*tamArreglosGrillas);
	memset(_dens, 0, sizeof(float)*tamArreglosGrillas);
	memset(_u_prev, 0, sizeof(float)*tamArreglosGrillas);
	memset(_v_prev, 0, sizeof(float)*tamArreglosGrillas);
	memset(_dens_prev, 0, sizeof(float)*tamArreglosGrillas);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

NBSimuladorLiquidosMotor2::~NBSimuladorLiquidosMotor2(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("NBSimuladorLiquidosMotor2::~NBSimuladorLiquidosMotor2")
	_gotas->liberar(NB_RETENEDOR_THIS); _gotas = NULL;
	delete _u;
	delete _v;
	delete _dens;
	delete _u_prev;
	delete _v_prev;
	delete _dens_prev;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

//

NBRectangulo NBSimuladorLiquidosMotor2::areaGrillaDensidades(){
	return _areaCajasLiq;
}

SI32 NBSimuladorLiquidosMotor2::dimGrillaDensidad(){
	return _tamGrillaDensidad;
}

NBPunto NBSimuladorLiquidosMotor2::posicionGota(UI16 indice){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("NBSimuladorLiquidosMotor2::posicionGota")
	NBASSERT(indice<_conteoRegistrosGotasRevisar)
	NBASSERT(_gotas->elemento[indice].registroOcupado)
	NBPunto pos = _gotas->elemento[indice].posLiq;
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return pos;
}

void NBSimuladorLiquidosMotor2::gotaAgregada(UI16 indice, float xPos, float yPos, float xVel, float yVel, float radioLiquido, float radioFisico, float biscosidad){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("NBSimuladorLiquidosMotor2::gotaAgregada")
	STLiquidoGotaDensidad datosGota;
	datosGota.registroOcupado	= true;
	//Propiedades
	datosGota.posLiq.x			= xPos;				//En coordenadas de grilla de simuladorLiq (aplicado el Multiplicador)
	datosGota.posLiq.y			= yPos;				//En coordenadas de grilla de simuladorLiq (aplicado el Multiplicador)
	datosGota.velLiq.x			= 0.0f/*xVel*/;		//En coordenadas de grilla de simuladorLiq (aplicado el Multiplicador)
	datosGota.velLiq.y			= 0.0f/*yVel*/;		//En coordenadas de grilla de simuladorLiq (aplicado el Multiplicador)
	datosGota.radioLiquido		= radioLiquido;		//Multiplicado por AU_MOTOR_LIQUIDOS_1_MUTIPLICADOR
	datosGota.radioSolido		= radioFisico;
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
	} else {
		NBASSERT(false) //El indice no es valido
	}
	//Agregar densidad
	SI32 indiceCol				= (datosGota.posLiq.x-_areaCajasLiq.x)/_dimensionesCajasLiq;
	SI32 indiceFil				= (datosGota.posLiq.y-_areaCajasLiq.y)/_dimensionesCajasLiq;
	SI32 N						= _tamGrillaDensidad;
	NBASSERT(indiceCol>=0)
	NBASSERT(indiceCol<_tamGrillaDensidad)
	NBASSERT(indiceFil>=0)
	NBASSERT(indiceFil<_tamGrillaDensidad)
	_dens_prev[IX(indiceCol, indiceFil)] += 0.02f;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void NBSimuladorLiquidosMotor2::gotaQuitada(UI16 indice){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("NBSimuladorLiquidosMotor2::gotaQuitada")
	NBASSERT(indice<_conteoRegistrosGotasRevisar)
	_gotas->elemento[indice].registroOcupado	= 0;
	_gotas->elemento[indice].coordCelda.x		= 0;
	_gotas->elemento[indice].coordCelda.y		= 0;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void NBSimuladorLiquidosMotor2::simularFuerzasLiquidos(float segundosTranscurridos){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("NBSimuladorLiquidosMotor2::simularFuerzasLiquidos")
	//get_from_UI (_dens_prev, _u_prev, _v_prev );
	float visc = 0.04f;
	float diff = 0.1f;
	vel_step(_tamGrillaDensidad, _u, _v, _u_prev, _v_prev, visc, segundosTranscurridos);
	dens_step(_tamGrillaDensidad, _dens, _dens_prev, _u, _v, diff, segundosTranscurridos);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

float NBSimuladorLiquidosMotor2::densidadEnCelda(SI32 x, SI32 y){
	SI32 N = _tamGrillaDensidad;
	return _dens[IX(x, y)];
}

void NBSimuladorLiquidosMotor2::vel_step ( int N, float * u, float * v, float * u0, float * v0, float visc, float dt ){
	add_source ( N, u, u0, dt ); add_source ( N, v, v0, dt );
	SWAP ( u0, u ); diffuse ( N, 1, u, u0, visc, dt );
	SWAP ( v0, v ); diffuse ( N, 2, v, v0, visc, dt );
	project ( N, u, v, u0, v0 );
	SWAP ( u0, u ); SWAP ( v0, v );
	advect ( N, 1, u, u0, u0, v0, dt ); advect ( N,  2, v, v0, u0, v0, dt );
	project ( N, u, v, u0, v0 );
}

void NBSimuladorLiquidosMotor2::dens_step ( int N, float * x, float * x0,  float * u, float * v, float diff, float dt ){
	add_source ( N, x, x0, dt );
	SWAP ( x0, x ); diffuse ( N, 0, x, x0, diff, dt );
	SWAP ( x0, x ); advect ( N, 0, x, x0, u, v, dt );
}

void NBSimuladorLiquidosMotor2::add_source ( int N, float * x, float * s, float dt ){
	int i, size=(N+2)*(N+2);
	for ( i=0 ; i<size ; i++ ) x[i] += dt*s[i];
}
			   
void NBSimuladorLiquidosMotor2::diffuse ( int N, int b, float * x, float * x0, float diff, float dt ){
	int i, j, k;
	float a=dt*diff*N*N;
	for ( k=0 ; k<20 ; k++ ) {
		for ( i=1 ; i<=N ; i++ ) {
			for ( j=1 ; j<=N ; j++ ) {
				x[IX(i,j)] = (x0[IX(i,j)] + a*(x[IX(i-1,j)]+x[IX(i+1,j)]+
											   x[IX(i,j-1)]+x[IX(i,j+1)]))/(1+4*a);
			}
		}
		set_bnd ( N, b, x );
	}
}

void NBSimuladorLiquidosMotor2::advect ( int N, int b, float * d, float * d0, float * u, float * v, float dt ){
	int i, j, i0, j0, i1, j1;
	float x, y, s0, t0, s1, t1, dt0;
	dt0 = dt*N;
	for ( i=1 ; i<=N ; i++ ) {
		for ( j=1 ; j<=N ; j++ ){ 
			x = i-dt0*u[IX(i,j)]; y = j-dt0*v[IX(i,j)];
			if (x<0.5) x=0.5; if (x>N+0.5) x=N+ 0.5; i0=(int)x; i1=i0+1;
			if (y<0.5) y=0.5; if (y>N+0.5) y=N+ 0.5; j0=(int)y; j1=j0+1;
			s1 = x-i0; s0 = 1-s1; t1 = y-j0; t0 = 1-t1;
			d[IX(i,j)] = s0*(t0*d0[IX(i0,j0)]+t1*d0[IX(i0,j1)])+s1*(t0*d0[IX(i1,j0)]+t1*d0[IX(i1,j1)]);
		}
	}
	set_bnd ( N, b, d );
}

void NBSimuladorLiquidosMotor2::project ( int N, float * u, float * v, float * p, float * div ){
	int i, j, k;
	float h;
	h = 1.0/N;
	for ( i=1 ; i<=N ; i++ ) {
		for ( j=1 ; j<=N ; j++ ) {
			div[IX(i,j)] = -0.5*h*(u[IX(i+1,j)]-u[IX(i-1,j)]+
								   v[IX(i,j+1)]-v[IX(i,j-1)]);
			p[IX(i,j)] = 0;
		}
	}
	set_bnd ( N, 0, div ); set_bnd ( N, 0, p );
	for ( k=0 ; k<20 ; k++ ) {
		for ( i=1 ; i<=N ; i++ ) {
			for ( j=1 ; j<=N ; j++ ) {
				p[IX(i,j)] = (div[IX(i,j)]+p[IX(i-1,j)]+p[IX(i+1,j)]+
							  p[IX(i,j-1)]+p[IX(i,j+1)])/4;
			}
		}
		set_bnd ( N, 0, p );
	}
	for ( i=1 ; i<=N ; i++ ) {
		for ( j=1 ; j<=N ; j++ ) {
			u[IX(i,j)] -= 0.5*(p[IX(i+1,j)]-p[IX(i-1,j)])/h;
			v[IX(i,j)] -= 0.5*(p[IX(i,j+1)]-p[IX(i,j-1)])/h;
		}
	}
	set_bnd ( N, 1, u ); set_bnd ( N, 2, v );
}

void NBSimuladorLiquidosMotor2::set_bnd ( int N, int b, float * x ){
	int i;
	for ( i=1 ; i<=N ; i++ ) {
		x[IX(0  ,i)] = b==1 ? -x[IX(1,i)] : x[IX(1,i)];
		x[IX(N+1,i)] = b==1 ? -x[IX(N,i)] : x[IX(N,i)];
		x[IX(i,0  )] = b==2 ? -x[IX(i,1)] : x[IX(i,1)];
		x[IX(i,N+1)] = b==2 ? -x[IX(i,N)] : x[IX(i,N)];
	}
	x[IX(0  ,0  )] = 0.5*(x[IX(1,0  )]+x[IX(0  ,1)]);
	x[IX(0  ,N+1)] = 0.5*(x[IX(1,N+1)]+x[IX(0  ,N )]);
	x[IX(N+1,0  )] = 0.5*(x[IX(N,0  )]+x[IX(N+1,1)]);
	x[IX(N+1,N+1)] = 0.5*(x[IX(N,N+1)]+x[IX(N+1,N )]);
}


