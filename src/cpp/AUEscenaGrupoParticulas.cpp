//
//  AUEscenaGrupoParticulas.cpp
//  Gameplay_Mac
//
//  Created by Marcos Ortega Morales on 12/10/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkMediaStdAfx.h"
#include "AUEscenaGrupoParticulas.h"

AUEscenaGrupoParticulas::AUEscenaGrupoParticulas(float xMin, float yMin, float xMax, float yMax) : AUEscenaObjeto(), _gruposPorTexturaGL(this, 8), _gruposPorTexturaAU(this, 32), _datosAnimInmovil(this), _datosAnimLibre(this), _datosAnimHoja(this), _datosAnimGota(this), _datosAnimHumo(this) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaGrupoParticulas::AUEscenaGrupoParticulas")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaGrupoParticulas")
	_conteoParticulas					= 0;
	_cajaPersistenciaParticulas.xMin	= xMin;
	_cajaPersistenciaParticulas.xMax	= xMax;
	_cajaPersistenciaParticulas.yMin	= yMin;
	_cajaPersistenciaParticulas.yMax	= yMax;
	//Agregar un primer grupo para las texturas en estado "precarga"
	STParticulaGrupoGL nuevoGrupoGL;
	nuevoGrupoGL.idTexturaGL			= 0; //idTexturaGL=0 cuando esta en precarga y aun no se ha definido su atlasGL
	nuevoGrupoGL.conteoParticulas		= 0;
	nuevoGrupoGL.particulas				= new(this) AUArregloNativoMutableP<STParticulaProps>(128);
	_gruposPorTexturaGL.agregarElemento(nuevoGrupoGL);
	//
	_partsCajaLocalSucia				= true;
	NBCAJAAABB_INICIALIZAR(_partsCajaLocal);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

AUEscenaGrupoParticulas::~AUEscenaGrupoParticulas(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaGrupoParticulas::~AUEscenaGrupoParticulas")
	UI32 i, iConteo = _gruposPorTexturaAU.conteo;
	for(i=0; i<iConteo; i++){
		NBASSERT(_gruposPorTexturaAU.elemento[i].objTextura != NULL)
		_gruposPorTexturaAU.elemento[i].objTextura->liberar(NB_RETENEDOR_THIS);
	}
	iConteo = _gruposPorTexturaGL.conteo;
	for(i=0; i<iConteo; i++){
		if(_gruposPorTexturaGL.elemento[i].particulas != NULL){
			_gruposPorTexturaGL.elemento[i].particulas->liberar(NB_RETENEDOR_THIS);
		}
	}
	iConteo = _datosAnimGota.conteo;
	for(i=0; i<iConteo; i++){
		NBASSERT(_datosAnimGota.elemento[i].texturaImpacto != NULL)
		_datosAnimGota.elemento[i].texturaImpacto->liberar(NB_RETENEDOR_THIS);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

//

SI16 AUEscenaGrupoParticulas::privInlineRegistrarTextura(AUTextura* textura){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaGrupoParticulas::privInlineRegistrarTextura")
	NBASSERT(textura != NULL)
	NBASSERT(textura->idTexturaGL<32767) //Si falla no alcanza en un SI16
	UI32 iGrupo, iGrupoConteo; SI16 iGrupoEncontrado = -1;
	//Contabilizar en el grupoAU
	iGrupoConteo = _gruposPorTexturaAU.conteo;
	for(iGrupo=0; iGrupo<iGrupoConteo; iGrupo++){
		if(_gruposPorTexturaAU.elemento[iGrupo].objTextura==textura){
			_gruposPorTexturaAU.elemento[iGrupo].conteoParticulas++;
			iGrupoEncontrado = iGrupo;
			break;
		}
	}
	if(iGrupoEncontrado==-1){
		STParticulaGrupoAU nuevoGrupoAU;
		nuevoGrupoAU.objTextura			= textura; textura->retener(NB_RETENEDOR_THIS);
		nuevoGrupoAU.conteoParticulas	= 1;
		_gruposPorTexturaAU.agregarElemento(nuevoGrupoAU);
	}
	//Contabilizar en el grupoGL
	iGrupoConteo = _gruposPorTexturaGL.conteo;
	iGrupoEncontrado = -1;
	for(iGrupo=0; iGrupo<iGrupoConteo; iGrupo++){
		if(_gruposPorTexturaGL.elemento[iGrupo].idTexturaGL==textura->idTexturaGL){
			_gruposPorTexturaGL.elemento[iGrupo].conteoParticulas++;
			iGrupoEncontrado = iGrupo;
			break;
		}
	}
	if(iGrupoEncontrado==-1){
		STParticulaGrupoGL nuevoGrupoGL;
		nuevoGrupoGL.idTexturaGL		= textura->idTexturaGL;
		nuevoGrupoGL.conteoParticulas	= 1;
		nuevoGrupoGL.particulas			= new(this) AUArregloNativoMutableP<STParticulaProps>(128);
		//Reciclar registro
		iGrupoConteo = _gruposPorTexturaGL.conteo;
		for(iGrupo=0; iGrupo<iGrupoConteo; iGrupo++){
			if(_gruposPorTexturaGL.elemento[iGrupo].idTexturaGL==-1){
				_gruposPorTexturaGL.elemento[iGrupo] = nuevoGrupoGL;
				iGrupoEncontrado = iGrupo;
				break;
			}
		}
		//Agregar registro
		if(iGrupoEncontrado==-1){
			iGrupoEncontrado = _gruposPorTexturaGL.conteo;
			_gruposPorTexturaGL.agregarElemento(nuevoGrupoGL);
		}
	}
	NBASSERT(iGrupoEncontrado!=-1)
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return iGrupoEncontrado;
}

void AUEscenaGrupoParticulas::privInlineLiberarTextura(AUTextura* textura, SI16 idTexturaGL){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaGrupoParticulas::privInlineLiberarTextura")
	NBASSERT(textura != NULL)
	NBASSERT(idTexturaGL>=0 && idTexturaGL<32767) //Si falla no alcanza en un SI16
	#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
	SI32 iGrupoEncontrado = -1;
	#endif
	//
	//Quitar de grupoAU
	SI32 iGrupo, iGrupoConteo = _gruposPorTexturaAU.conteo;
	for(iGrupo=0; iGrupo<iGrupoConteo; iGrupo++){
		if(_gruposPorTexturaAU.elemento[iGrupo].objTextura==textura){
			#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
			NBASSERT(iGrupoEncontrado==-1)
			iGrupoEncontrado = iGrupo;
			#endif
			NBASSERT(_gruposPorTexturaAU.elemento[iGrupo].conteoParticulas>0)
			_gruposPorTexturaAU.elemento[iGrupo].conteoParticulas--;
			if(_gruposPorTexturaAU.elemento[iGrupo].conteoParticulas==0){
				#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
				UI32 iGrp, iGrpConteo = _gruposPorTexturaGL.conteo;
				for(iGrp=0; iGrp<iGrpConteo; iGrp++){
					AUArregloNativoP<STParticulaProps>* particulas = _gruposPorTexturaGL.elemento[iGrp].particulas;
					UI32 iPart, iPartConteo = particulas->conteo;
					for(iPart=0; iPart<iPartConteo; iPart++){
						NBASSERT(particulas->elemento[iPart].objTextura!=textura) //Si falla aun hay particulas que usan esta textura
					}
				}
				#endif
				NBASSERT(_gruposPorTexturaAU.elemento[iGrupo].objTextura != NULL)
				_gruposPorTexturaAU.elemento[iGrupo].objTextura->liberar(NB_RETENEDOR_THIS);
				_gruposPorTexturaAU.quitarElementoEnIndice(iGrupo--); iGrupoConteo--;
			}
		}
	}
	NBASSERT(iGrupoEncontrado!=-1)
	//
	//Quitar de grupoGL
	//
	#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
	iGrupoEncontrado = -1;
	#endif
	iGrupoConteo = _gruposPorTexturaGL.conteo;
	for(iGrupo=0; iGrupo<iGrupoConteo; iGrupo++){
		if(_gruposPorTexturaGL.elemento[iGrupo].idTexturaGL==idTexturaGL){
			#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
			NBASSERT(iGrupoEncontrado==-1)
			iGrupoEncontrado = iGrupo;
			#endif
			NBASSERT(_gruposPorTexturaGL.elemento[iGrupo].conteoParticulas>0)
			_gruposPorTexturaGL.elemento[iGrupo].conteoParticulas--;
			if(_gruposPorTexturaGL.elemento[iGrupo].conteoParticulas==0){
				#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
				AUArregloNativoP<STParticulaProps>* particulas = _gruposPorTexturaGL.elemento[iGrupo].particulas; NBASSERT(particulas != NULL)
				UI32 iPart, iPartConteo = particulas->conteo;
				for(iPart=0; iPart<iPartConteo; iPart++){
					NBASSERT(particulas->elemento[iPart].objTextura == NULL) //Si falla aun hay particulas que usan esta textura
				}
				#endif
				_gruposPorTexturaGL.elemento[iGrupo].particulas->vaciar();
				if(idTexturaGL!=0){ //idTexturaGL==0 es el grupo de las texturas en precarga (el grupo no debe reciclarse/eliminarse)
					_gruposPorTexturaGL.elemento[iGrupo].idTexturaGL = -1; 
				}
			}
		}
	}
	NBASSERT(iGrupoEncontrado!=-1)
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

//

STParticulaIndice AUEscenaGrupoParticulas::privInlineRegistrarParticula(AUTextura* textura, SI8 alineaTexH, SI8 alineaTexV, float posX, float posY, float escala, float radianes, float alpha){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaGrupoParticulas::privInlineRegistrarParticula")
	NBASSERT(textura != NULL);
	NBASSERT(escala>0.0f && escala<=100.0f)
	//
	STParticulaIndice indiceP;
	indiceP.iGrupoGL				= privInlineRegistrarTextura(textura);
	indiceP.iParticula				= -1;
	NBASSERT(indiceP.iGrupoGL>=0 && indiceP.iGrupoGL<(SI32)_gruposPorTexturaGL.conteo)
	NBASSERT(textura->estadoCarga() != ENTexturaEstadoCarga_Precargada || indiceP.iGrupoGL==0)
	NBASSERT(_gruposPorTexturaGL.elemento[indiceP.iGrupoGL].particulas != NULL)
	//
	STParticulaProps datosParticula;
	datosParticula.objTextura		= textura;
	datosParticula.texAlineacionH	= alineaTexH;
	datosParticula.texAlineacionV	= alineaTexV;
	datosParticula.alpha			= alpha;
	datosParticula.posicion.x		= posX;
	datosParticula.posicion.y		= posY;
	datosParticula.escala			= escala;
	datosParticula.radianes			= radianes;
	//
	AUArregloNativoMutableP<STParticulaProps>* particulas = _gruposPorTexturaGL.elemento[indiceP.iGrupoGL].particulas;
	UI32 i, iConteo = particulas->conteo;
	for(i=0; i<iConteo; i++){
		if(particulas->elemento[i].objTextura == NULL){
			particulas->elemento[i] = datosParticula;
			indiceP.iParticula = i;
			break;
		}
	}
	if(indiceP.iParticula==-1){
		indiceP.iParticula = particulas->conteo;
		particulas->agregarElemento(datosParticula);
	}
	NBASSERT(indiceP.iParticula>=0 && indiceP.iParticula<(SI32)particulas->conteo)
	_conteoParticulas++;
	_partsCajaLocalSucia = true;
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return indiceP;
}
void AUEscenaGrupoParticulas::privInlineLiberarParticula(SI16 iGrupoGL, SI16 iParticula, SI16 idTexturaGL){ //Se pasa explicito el idTexturaGL, para poder eliminar del grupo de precarga (indice 0, idTexturaGL 0) las texturas que fueron cargadas posteriormente
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaGrupoParticulas::privInlineLiberarParticula")
	NBASSERT(iGrupoGL>=0 && iGrupoGL<(SI32)_gruposPorTexturaGL.conteo)
	NBASSERT(_gruposPorTexturaGL.elemento[iGrupoGL].particulas != NULL)
	NBASSERT(iParticula>=0 && iParticula<(SI32)_gruposPorTexturaGL.elemento[iGrupoGL].particulas->conteo)
	NBASSERT(_gruposPorTexturaGL.elemento[iGrupoGL].particulas->elemento[iParticula].objTextura != NULL)
	AUTextura* objTextura = _gruposPorTexturaGL.elemento[iGrupoGL].particulas->elemento[iParticula].objTextura;
	_gruposPorTexturaGL.elemento[iGrupoGL].particulas->elemento[iParticula].objTextura = NULL;
	privInlineLiberarTextura(objTextura, idTexturaGL); //Se pasa explicito el idTexturaGL, para poder eliminar del grupo de precarga (indice 0, idTexturaGL 0) las texturas que fueron cargadas posteriormente
	_conteoParticulas--;
	_partsCajaLocalSucia = true;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaGrupoParticulas::agregarParticulaInmovil(AUTextura* textura, float segundosVive, float segundosDesaparece, float posX, float posY, float escala, float radianes, float escalaDesaparece){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaGrupoParticulas::agregarParticulaInmovil")
	NBASSERT(textura != NULL)
	STParticulaAnimInmovil datosAnimacion;
	datosAnimacion.indice		= privInlineRegistrarParticula(textura, 0, 0, posX, posY, escala, radianes, 1.0f);
	datosAnimacion.segundosViva	= 0.0f;
	datosAnimacion.segundosVive	= segundosVive;
	datosAnimacion.segundosDesapareciendo = 0.0f;
	datosAnimacion.segundosDesaparece = segundosDesaparece;
	datosAnimacion.escalaInicial	= escala;
	datosAnimacion.escalaDesaparece	= escalaDesaparece;
	_datosAnimInmovil.agregarElemento(datosAnimacion);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaGrupoParticulas::agregarParticulaLibre(AUTextura* textura, float segundosVive, float segundosDesaparece, float posX, float posY, float escala, float radianes, float velX, float velY, float velRadianes){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaGrupoParticulas::agregarParticulaLibre")
	NBASSERT(textura != NULL)
	STParticulaAnimLibre datosAnimacion;
	datosAnimacion.indice		= privInlineRegistrarParticula(textura, 0, 0, posX, posY, escala, radianes, 1.0f);
	datosAnimacion.segundosViva	= 0.0f;
	datosAnimacion.segundosVive	= segundosVive;
	datosAnimacion.segundosDesapareciendo = 0.0f;
	datosAnimacion.segundosDesaparece = segundosDesaparece;
	datosAnimacion.velLineal.x	= velX;
	datosAnimacion.velLineal.y	= velY;
	datosAnimacion.velRadAngular= velRadianes;
	_datosAnimLibre.agregarElemento(datosAnimacion);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaGrupoParticulas::agregarParticulaHoja(AUTextura* textura, float segundosVive, float segundosDesaparece, float posX, float posY, float escala, float radianes, float velX, float velY, float velRadianes, float factorResistenciaAire, float factorTrayectoriaIrregular){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaGrupoParticulas::agregarParticulaHoja")
	NBASSERT(textura != NULL)
	STParticulaAnimHoja datosAnimacion;
	datosAnimacion.indice				= privInlineRegistrarParticula(textura, 0, 0, posX, posY, escala, radianes, 1.0f);
	datosAnimacion.polarN				= 1+(rand()%7);
	datosAnimacion.polarD				= 1+(rand()%7);
	datosAnimacion.factorResistenciaAire = factorResistenciaAire;
	datosAnimacion.factorTrayectoriaIrregular = factorTrayectoriaIrregular;
	datosAnimacion.radianesPolar		= 0.0f;
	datosAnimacion.segundosViva			= 0.0f;
	datosAnimacion.segundosVive			= segundosVive;
	datosAnimacion.segundosDesapareciendo = 0.0f;
	datosAnimacion.segundosDesaparece = segundosDesaparece;
	datosAnimacion.velLinealGravedad.x	= 0.0f;
	datosAnimacion.velLinealGravedad.y	= 0.0f;
	datosAnimacion.velLinealViento.x	= velX;
	datosAnimacion.velLinealViento.y	= velY;
	datosAnimacion.posSinPolar.x		= posX;
	datosAnimacion.posSinPolar.y		= posY;
	datosAnimacion.posPolar.x			= 0.0f;
	datosAnimacion.posPolar.y			= 0.0f;
	datosAnimacion.velRadAngular		= velRadianes;
	_datosAnimHoja.agregarElemento(datosAnimacion);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaGrupoParticulas::agregarParticulaGota(AUTextura* texturaGota, AUTextura* texturaImpacto, float segundosDesaparece, float posX, float posY, float escala, float velX, float velY, float segsParaCalcularFisica){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaGrupoParticulas::agregarParticulaGota")
	NBASSERT(texturaGota != NULL)
	NBASSERT(texturaImpacto != NULL)
	STParticulaAnimGota datosAnimacion;
	float radianesDireccion;			NBPUNTO_RADIANES_VECTOR(radianesDireccion, 0.0f, 0.0f, velX, velY)
	datosAnimacion.indice				= privInlineRegistrarParticula(texturaGota, -1, 0, posX, posY, escala, radianesDireccion, 1.0f);
	datosAnimacion.impactada			= false;
	datosAnimacion.segundosDesapareciendo = 0.0f;
	datosAnimacion.segundosDesaparece	= segundosDesaparece;
	datosAnimacion.velLineal.x			= velX;
	datosAnimacion.velLineal.y			= velY;
	datosAnimacion.segsAcumSinCalcularFisica = 0.0f;
	datosAnimacion.segsparaCalcularFisica = segsParaCalcularFisica;
	datosAnimacion.texturaImpacto		= texturaImpacto; texturaImpacto->retener(NB_RETENEDOR_THIS);
	datosAnimacion.velLineal.x			= velX;
	datosAnimacion.velLineal.y			= velY;
	_datosAnimGota.agregarElemento(datosAnimacion);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaGrupoParticulas::agregarParticulaHumo(AUTextura* textura, float segundosDesaparece, float posX, float posY, float escala, float radianes, float velX, float velY, float velradianes, float escalarParaDisipar, float factorResistenciaAire){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaGrupoParticulas::agregarParticulaHoja")
	NBASSERT(textura != NULL)
	STParticulaAnimHumo datosAnimacion;
	datosAnimacion.indice				= privInlineRegistrarParticula(textura, 0, 0, posX, posY, escala, radianes, 1.0f);
	datosAnimacion.escalaInicial		= escala;
	datosAnimacion.escalaParaDisipacion	= escalarParaDisipar;
	datosAnimacion.factorResistenciaAire = factorResistenciaAire;
	datosAnimacion.segundosViva			= 0.0f;
	datosAnimacion.segundosVive			= segundosDesaparece;
	datosAnimacion.velLinealGravedad.x	= 0.0f;
	datosAnimacion.velLinealGravedad.y	= 0.0f;
	datosAnimacion.velLinealViento.x	= velX;
	datosAnimacion.velLinealViento.y	= velY;
	datosAnimacion.velRadAngular		= velradianes;
	_datosAnimHumo.agregarElemento(datosAnimacion);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

//

UI16 AUEscenaGrupoParticulas::conteoParticulasInmovil(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaGrupoParticulas::conteoParticulasInmovil")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _datosAnimInmovil.conteo;
}

UI16 AUEscenaGrupoParticulas::conteoParticulasLibres(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaGrupoParticulas::conteoParticulasLibres")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _datosAnimLibre.conteo;
}

UI16 AUEscenaGrupoParticulas::conteoParticulasHoja(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaGrupoParticulas::conteoParticulasHoja")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _datosAnimHoja.conteo;
}

UI16 AUEscenaGrupoParticulas::conteoParticulasGotas(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaGrupoParticulas::conteoParticulasGotas")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _datosAnimGota.conteo;
}

UI16 AUEscenaGrupoParticulas::conteoParticulasHumo(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaGrupoParticulas::conteoParticulasHumo")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return _datosAnimHumo.conteo;
}

//

void AUEscenaGrupoParticulas::tickAnimacion(float segsTranscurridos){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaGrupoParticulas::tickAnimacion")
	if(!this->_dormido){
		_partsCajaLocalSucia	= true;
		NBPunto vectorViento;
		NBPunto vectorGravedad;
		NBPUNTO_ESTABLECER(vectorViento, 0.0f, 0.0f);
		NBPUNTO_ESTABLECER(vectorGravedad, 0.0f, 0.0f);
		if(_idMundoFisica >= 0){
#			ifndef CONFIG_NB_UNSUPPORT_BOX2D
			vectorViento		= NBGestorFisica::vectorVientoEscena(_idMundoFisica);
			vectorGravedad		= NBGestorFisica::vectorGravedadEscena(_idMundoFisica);
#			endif
		}
		//
		SI32 i;
		//Texturas en precarga (actualizar a su grupo cuando finalicen su carga)
		//IMPORTANTE: esto debe realizarse antes de ejecutar las animaciones de particulas.
		NBASSERT(_gruposPorTexturaGL.elemento[0].idTexturaGL==0)
		AUArregloNativoMutableP<STParticulaProps>* particulasEnPrecarga = _gruposPorTexturaGL.elemento[0].particulas;
		for(i=particulasEnPrecarga->conteo-1; i>=0; i--){
			STParticulaProps datosParticula = particulasEnPrecarga->elemento[i];
			if(datosParticula.objTextura != NULL){
				const ENTexturaEstadoCarga loadState = datosParticula.objTextura->estadoCarga();
				if(loadState == ENTexturaEstadoCarga_Cargada || loadState == ENTexturaEstadoCarga_Cargando){
					datosParticula.objTextura->retener(NB_RETENEDOR_THIS);
					//Quitar del grupo de precarga
					privInlineLiberarParticula(0, i, 0);
					//Agregar al grupo ya cargada
					STParticulaIndice nuevoIndice = privInlineRegistrarParticula(datosParticula.objTextura, datosParticula.texAlineacionH, datosParticula.texAlineacionV, datosParticula.posicion.x, datosParticula.posicion.y, datosParticula.escala, datosParticula.radianes, datosParticula.alpha);
					//Actualizar indice de animador
					SI32 iAnimEncontrado = -1;
					UI32 iAnim, iAnimConteo = _datosAnimLibre.conteo;
					for(iAnim=0; iAnim<iAnimConteo; iAnim++){
						if(_datosAnimLibre.elemento[iAnim].indice.iGrupoGL==0 && _datosAnimLibre.elemento[iAnim].indice.iParticula==i){
							_datosAnimLibre.elemento[iAnim].indice = nuevoIndice;
							iAnimEncontrado = iAnim;
							break;
						}
					}
					iAnimConteo = _datosAnimHoja.conteo;
					for(iAnim=0; iAnim<iAnimConteo; iAnim++){
						if(_datosAnimHoja.elemento[iAnim].indice.iGrupoGL==0 && _datosAnimHoja.elemento[iAnim].indice.iParticula==i){
							_datosAnimHoja.elemento[iAnim].indice = nuevoIndice;
							iAnimEncontrado = iAnim;
							break;
						}
					}
					iAnimConteo = _datosAnimGota.conteo;
					for(iAnim=0; iAnim<iAnimConteo; iAnim++){
						if(_datosAnimGota.elemento[iAnim].indice.iGrupoGL==0 && _datosAnimGota.elemento[iAnim].indice.iParticula==i){
							_datosAnimGota.elemento[iAnim].indice = nuevoIndice;
							iAnimEncontrado = iAnim;
							break;
						}
					}
					iAnimConteo = _datosAnimHumo.conteo;
					for(iAnim=0; iAnim<iAnimConteo; iAnim++){
						if(_datosAnimHumo.elemento[iAnim].indice.iGrupoGL==0 && _datosAnimHumo.elemento[iAnim].indice.iParticula==i){
							_datosAnimHumo.elemento[iAnim].indice = nuevoIndice;
							iAnimEncontrado = iAnim;
							break;
						}
					}
					NBASSERT(iAnimEncontrado!=-1)
					//
					datosParticula.objTextura->liberar(NB_RETENEDOR_THIS);
				}
			}
		}
		//Particulas inmovil
		SI32 iConteo = _datosAnimInmovil.conteo;
		for(i=0; i<iConteo; i++){
			STParticulaAnimInmovil* datosAnim	= &_datosAnimInmovil.elemento[i];
			STParticulaProps* datosParticula	= &_gruposPorTexturaGL.elemento[datosAnim->indice.iGrupoGL].particulas->elemento[datosAnim->indice.iParticula];
			//
			if(!NBCAJAAABB_INTERSECTA_PUNTO(_cajaPersistenciaParticulas, datosParticula->posicion.x, datosParticula->posicion.y)){
				privInlineLiberarParticula(datosAnim->indice.iGrupoGL, datosAnim->indice.iParticula, datosParticula->objTextura->idTexturaGL);
				_datosAnimInmovil.quitarElementoEnIndice(i--); iConteo--;
			} else {
				datosAnim->segundosViva			+= segsTranscurridos;
				if(datosAnim->segundosViva>datosAnim->segundosVive){
					datosAnim->segundosDesapareciendo += segsTranscurridos;
					if(datosAnim->segundosDesapareciendo<datosAnim->segundosDesaparece){
						datosParticula->escala	= datosAnim->escalaInicial + ((datosAnim->escalaDesaparece-datosAnim->escalaInicial) * (datosAnim->segundosDesapareciendo / datosAnim->segundosDesaparece));
						datosParticula->alpha	= 1.0f - (datosAnim->segundosDesapareciendo / datosAnim->segundosDesaparece); //En teoria si 'segundosDesaparece' es cero nunca se ejecuta
					} else {
						privInlineLiberarParticula(datosAnim->indice.iGrupoGL, datosAnim->indice.iParticula, datosParticula->objTextura->idTexturaGL);
						_datosAnimInmovil.quitarElementoEnIndice(i--); iConteo--;
					}
				}
			}
		}
		//Particulas libres
		iConteo = _datosAnimLibre.conteo;
		for(i=0; i<iConteo; i++){
			STParticulaAnimLibre* datosAnim		= &_datosAnimLibre.elemento[i];
			STParticulaProps* datosParticula	= &_gruposPorTexturaGL.elemento[datosAnim->indice.iGrupoGL].particulas->elemento[datosAnim->indice.iParticula];
			datosParticula->posicion.x			+= datosAnim->velLineal.x * segsTranscurridos;
			datosParticula->posicion.y			+= datosAnim->velLineal.y * segsTranscurridos;
			datosParticula->radianes			+= datosAnim->velRadAngular * segsTranscurridos;
			//
			if(!NBCAJAAABB_INTERSECTA_PUNTO(_cajaPersistenciaParticulas, datosParticula->posicion.x, datosParticula->posicion.y)){
				privInlineLiberarParticula(datosAnim->indice.iGrupoGL, datosAnim->indice.iParticula, datosParticula->objTextura->idTexturaGL);
				_datosAnimLibre.quitarElementoEnIndice(i--); iConteo--;
			} else {
				datosAnim->velLineal.x			+= vectorGravedad.x * segsTranscurridos;
				datosAnim->velLineal.y			+= vectorGravedad.y * segsTranscurridos;
				datosAnim->segundosViva			+= segsTranscurridos;
				if(datosAnim->segundosViva>datosAnim->segundosVive){
					datosAnim->segundosDesapareciendo += segsTranscurridos;
					if(datosAnim->segundosDesapareciendo<datosAnim->segundosDesaparece){
						datosParticula->alpha	= 1.0f - (datosAnim->segundosDesapareciendo / datosAnim->segundosDesaparece); //En teoria si 'segundosDesaparece' es cero nunca se ejecuta
					} else {
						privInlineLiberarParticula(datosAnim->indice.iGrupoGL, datosAnim->indice.iParticula, datosParticula->objTextura->idTexturaGL);
						_datosAnimLibre.quitarElementoEnIndice(i--); iConteo--;
					}
				}
			}
		}
		//Particulas hojas
		iConteo = _datosAnimHoja.conteo;
		for(i=0; i<iConteo; i++){
			STParticulaAnimHoja* datosAnim		= &_datosAnimHoja.elemento[i];
			AUArregloNativoP<STParticulaProps>* particulas = _gruposPorTexturaGL.elemento[datosAnim->indice.iGrupoGL].particulas;
			STParticulaProps* datosParticula	= &particulas->elemento[datosAnim->indice.iParticula];
			datosAnim->posSinPolar.x			+= (datosAnim->velLinealGravedad.x+datosAnim->velLinealViento.x) * segsTranscurridos;
			datosAnim->posSinPolar.y			+= (datosAnim->velLinealGravedad.y+datosAnim->velLinealViento.y) * segsTranscurridos;
			//datosParticula->radianes			+= datosAnim->velRadAngular * segsTranscurridos;
			if(!NBCAJAAABB_INTERSECTA_PUNTO(_cajaPersistenciaParticulas, datosParticula->posicion.x, datosParticula->posicion.y)){
				privInlineLiberarParticula(datosAnim->indice.iGrupoGL, datosAnim->indice.iParticula, datosParticula->objTextura->idTexturaGL);
				_datosAnimHoja.quitarElementoEnIndice(i--); iConteo--;
			} else {
				//Aplicar velocidad gravedad
				if(datosAnim->velLinealGravedad.x<vectorGravedad.x){
					datosAnim->velLinealGravedad.x += ((vectorGravedad.x-datosAnim->velLinealGravedad.x) * (1.0f-datosAnim->factorResistenciaAire) * segsTranscurridos);
					if(datosAnim->velLinealGravedad.x>vectorGravedad.x) datosAnim->velLinealGravedad.x = vectorGravedad.x;
				} else {
					datosAnim->velLinealGravedad.x += ((vectorGravedad.x-datosAnim->velLinealGravedad.x) * (1.0f-datosAnim->factorResistenciaAire) * segsTranscurridos);
					if(datosAnim->velLinealGravedad.x<vectorGravedad.x) datosAnim->velLinealGravedad.x = vectorGravedad.x;
				}
				if(datosAnim->velLinealGravedad.y<vectorGravedad.y){
					datosAnim->velLinealGravedad.y += ((vectorGravedad.y-datosAnim->velLinealGravedad.y) * (1.0f-datosAnim->factorResistenciaAire) * segsTranscurridos);
					if(datosAnim->velLinealGravedad.y>vectorGravedad.y) datosAnim->velLinealGravedad.y = vectorGravedad.y;
				} else {
					datosAnim->velLinealGravedad.y += ((vectorGravedad.y-datosAnim->velLinealGravedad.y) * (1.0f-datosAnim->factorResistenciaAire) * segsTranscurridos);
					if(datosAnim->velLinealGravedad.y<vectorGravedad.y) datosAnim->velLinealGravedad.y = vectorGravedad.y;
				}
				//Aplicar velocidad viento
				if(datosAnim->velLinealViento.x<vectorViento.x){
					datosAnim->velLinealViento.x += ((vectorViento.x-datosAnim->velLinealViento.x) * datosAnim->factorResistenciaAire * segsTranscurridos * 4.0f);
					if(datosAnim->velLinealViento.x>vectorViento.x) datosAnim->velLinealViento.x = vectorViento.x;
				} else {
					datosAnim->velLinealViento.x += ((vectorViento.x-datosAnim->velLinealViento.x) * datosAnim->factorResistenciaAire * segsTranscurridos * 4.0f);
					if(datosAnim->velLinealViento.x<vectorViento.x) datosAnim->velLinealViento.x = vectorViento.x;
				}
				if(datosAnim->velLinealViento.y<vectorViento.y){
					datosAnim->velLinealViento.y += ((vectorViento.y-datosAnim->velLinealViento.y) * datosAnim->factorResistenciaAire * segsTranscurridos * 4.0f);
					if(datosAnim->velLinealViento.y>vectorViento.y) datosAnim->velLinealViento.y = vectorViento.y;
				} else {
					datosAnim->velLinealViento.y += ((vectorViento.y-datosAnim->velLinealViento.y) * datosAnim->factorResistenciaAire * segsTranscurridos * 4.0f);
					if(datosAnim->velLinealViento.y<vectorViento.y) datosAnim->velLinealViento.y = vectorViento.y;
				}
				//Aplicar movimiento polar
				NBPunto posPolarAnterior			= datosAnim->posPolar;
				if(datosAnim->factorTrayectoriaIrregular!=0.0f){
					datosAnim->radianesPolar		+= (PIx2 * segsTranscurridos);
					NBPUNTO_ESTABLECER(datosAnim->posPolar, sinf(datosAnim->polarN/datosAnim->polarD)*(32.0f*datosAnim->factorTrayectoriaIrregular), 0.0f);
					NBPUNTO_ROTAR_RADIANES(datosAnim->posPolar, datosAnim->posPolar, datosAnim->radianesPolar)
					datosParticula->posicion.x		= datosAnim->posSinPolar.x + datosAnim->posPolar.x;
					datosParticula->posicion.y		= datosAnim->posSinPolar.y + datosAnim->posPolar.y;
					NBPUNTO_RADIANES_VECTOR(datosParticula->radianes, posPolarAnterior.x, posPolarAnterior.y, datosAnim->posPolar.x, datosAnim->posPolar.y)
				} else {
					datosParticula->posicion.x		= datosAnim->posSinPolar.x;
					datosParticula->posicion.y		= datosAnim->posSinPolar.y;
				}
				//
				datosAnim->segundosViva			+= segsTranscurridos;
				if(datosAnim->segundosViva>datosAnim->segundosVive){
					datosAnim->segundosDesapareciendo += segsTranscurridos;
					if(datosAnim->segundosDesapareciendo<datosAnim->segundosDesaparece){
						datosParticula->alpha	= 1.0f - (datosAnim->segundosDesapareciendo / datosAnim->segundosDesaparece); //En teoria si 'segundosDesaparece' es cero nunca se ejecuta
					} else {
						privInlineLiberarParticula(datosAnim->indice.iGrupoGL, datosAnim->indice.iParticula, datosParticula->objTextura->idTexturaGL);
						_datosAnimHoja.quitarElementoEnIndice(i--); iConteo--;
					}
				}
			}
		}
		//Particulas gotas (lluvia)
		iConteo = _datosAnimGota.conteo;
		for(i=0; i<iConteo; i++){
			STParticulaAnimGota* datosAnim		= &_datosAnimGota.elemento[i];
			STParticulaProps* datosParticula	= &_gruposPorTexturaGL.elemento[datosAnim->indice.iGrupoGL].particulas->elemento[datosAnim->indice.iParticula];
			if(datosAnim->impactada){
				//Ya fue impactada
				datosAnim->segundosDesapareciendo += segsTranscurridos;
				if(datosAnim->segundosDesapareciendo<datosAnim->segundosDesaparece){
					datosParticula->alpha	= 1.0f - (datosAnim->segundosDesapareciendo / datosAnim->segundosDesaparece); //En teoria si 'segundosDesaparece' es cero nunca se ejecuta
				} else {
					privInlineLiberarParticula(datosAnim->indice.iGrupoGL, datosAnim->indice.iParticula, datosParticula->objTextura->idTexturaGL);
					datosAnim->texturaImpacto->liberar(NB_RETENEDOR_THIS);
					_datosAnimGota.quitarElementoEnIndice(i--); iConteo--;
				}
			} else {
				if(!NBCAJAAABB_INTERSECTA_PUNTO(_cajaPersistenciaParticulas, datosParticula->posicion.x, datosParticula->posicion.y)){
					privInlineLiberarParticula(datosAnim->indice.iGrupoGL, datosAnim->indice.iParticula, datosParticula->objTextura->idTexturaGL);
					datosAnim->texturaImpacto->liberar(NB_RETENEDOR_THIS);
					_datosAnimGota.quitarElementoEnIndice(i--); iConteo--;
				} else {
					datosAnim->segsAcumSinCalcularFisica += segsTranscurridos;
					if(datosAnim->velLineal.x!=0.0f ||  datosAnim->velLineal.y!=0.0f){
						NBPunto posActual = datosParticula->posicion;
						NBPunto posDestino; NBPUNTO_ESTABLECER(posDestino, posActual.x+(datosAnim->velLineal.x*segsTranscurridos), posActual.y+(datosAnim->velLineal.y*segsTranscurridos));
						bool colisionDetectada = false;
						if(datosAnim->segsAcumSinCalcularFisica>=datosAnim->segsparaCalcularFisica){
							datosAnim->segsAcumSinCalcularFisica = 0.0f;
#							ifndef CONFIG_NB_UNSUPPORT_BOX2D
							_resultadosColision.fixture = NULL;
							NBGestorFisica::interseccionesConLinea(_idMundoFisica, this, posActual.x/ESCALA_PIXELES_POR_METRO_MUNDO_FISICA, posActual.y/ESCALA_PIXELES_POR_METRO_MUNDO_FISICA, posDestino.x/ESCALA_PIXELES_POR_METRO_MUNDO_FISICA, posDestino.y/ESCALA_PIXELES_POR_METRO_MUNDO_FISICA);
							if(_resultadosColision.fixture != NULL){
								NBASSERT(datosAnim->texturaImpacto != NULL)
								STParticulaProps datosParticOrig = *datosParticula;
								privInlineLiberarParticula(datosAnim->indice.iGrupoGL, datosAnim->indice.iParticula, datosParticula->objTextura->idTexturaGL);
								//
								float radianes;			NBPUNTO_RADIANES_VECTOR(radianes, 0.0f, 0.0f, _resultadosColision.normal.x, _resultadosColision.normal.y) radianes += PI;
								datosAnim->indice		= privInlineRegistrarParticula(datosAnim->texturaImpacto, datosParticOrig.texAlineacionH, datosParticOrig.texAlineacionV, _resultadosColision.point.x*ESCALA_PIXELES_POR_METRO_MUNDO_FISICA, _resultadosColision.point.y*ESCALA_PIXELES_POR_METRO_MUNDO_FISICA, datosParticOrig.escala, radianes, datosParticOrig.alpha);
								datosAnim->velLineal.x	= 0.0f;
								datosAnim->velLineal.y	= 0.0f;
								datosAnim->impactada	= true;
								//
								colisionDetectada		= true;
							}
#							endif
						}
						if(!colisionDetectada){
							datosParticula->posicion.x	+= datosAnim->velLineal.x * segsTranscurridos;
							datosParticula->posicion.y	+= datosAnim->velLineal.y * segsTranscurridos;
							NBPUNTO_RADIANES_VECTOR(datosParticula->radianes, 0.0f, 0.0f, datosAnim->velLineal.x, datosAnim->velLineal.y)
							datosAnim->velLineal.x		+= (vectorGravedad.x+vectorViento.x) * segsTranscurridos;
							datosAnim->velLineal.y		+= (vectorGravedad.y+vectorViento.y) * segsTranscurridos;
						}
					}
				}
			}
		}
		//Particulas humo
		iConteo = _datosAnimHumo.conteo;
		for(i=0; i<iConteo; i++){
			STParticulaAnimHumo* datosAnim		= &_datosAnimHumo.elemento[i];
			STParticulaProps* datosParticula	= &_gruposPorTexturaGL.elemento[datosAnim->indice.iGrupoGL].particulas->elemento[datosAnim->indice.iParticula];
			datosParticula->posicion.x			+= (datosAnim->velLinealGravedad.x+datosAnim->velLinealViento.x) * segsTranscurridos;
			datosParticula->posicion.y			+= (datosAnim->velLinealGravedad.y+datosAnim->velLinealViento.y) * segsTranscurridos;
			datosParticula->radianes			+= (datosAnim->velRadAngular) * segsTranscurridos;
			//datosParticula->radianes			+= datosAnim->velRadAngular * segsTranscurridos;
			if(!NBCAJAAABB_INTERSECTA_PUNTO(_cajaPersistenciaParticulas, datosParticula->posicion.x, datosParticula->posicion.y)){
				privInlineLiberarParticula(datosAnim->indice.iGrupoGL, datosAnim->indice.iParticula, datosParticula->objTextura->idTexturaGL);
				_datosAnimHumo.quitarElementoEnIndice(i--); iConteo--;
			} else {
				NBPunto vGravedadHumo;
				vGravedadHumo.x		= -vectorGravedad.x * (1.0f-datosAnim->factorResistenciaAire);
				vGravedadHumo.y		= -vectorGravedad.y * (1.0f-datosAnim->factorResistenciaAire);
				//Aplicar velocidad gravedad
				if(datosAnim->velLinealGravedad.x<vGravedadHumo.x){
					datosAnim->velLinealGravedad.x += ((vGravedadHumo.x-datosAnim->velLinealGravedad.x) * segsTranscurridos);
					if(datosAnim->velLinealGravedad.x>vGravedadHumo.x) datosAnim->velLinealGravedad.x = vGravedadHumo.x;
				} else {
					datosAnim->velLinealGravedad.x += ((vGravedadHumo.x-datosAnim->velLinealGravedad.x) * segsTranscurridos);
					if(datosAnim->velLinealGravedad.x<vGravedadHumo.x) datosAnim->velLinealGravedad.x = vGravedadHumo.x;
				}
				if(datosAnim->velLinealGravedad.y<vGravedadHumo.y){
					datosAnim->velLinealGravedad.y += ((vGravedadHumo.y-datosAnim->velLinealGravedad.y) * segsTranscurridos);
					if(datosAnim->velLinealGravedad.y>vGravedadHumo.y) datosAnim->velLinealGravedad.y = vGravedadHumo.y;
				} else {
					datosAnim->velLinealGravedad.y += ((vGravedadHumo.y-datosAnim->velLinealGravedad.y) * segsTranscurridos);
					if(datosAnim->velLinealGravedad.y<vGravedadHumo.y) datosAnim->velLinealGravedad.y = vGravedadHumo.y;
				}
				//Aplicar velocidad viento
				if(datosAnim->velLinealViento.x<vectorViento.x){
					datosAnim->velLinealViento.x += ((vectorViento.x-datosAnim->velLinealViento.x) * datosAnim->factorResistenciaAire * segsTranscurridos * 4.0f);
					if(datosAnim->velLinealViento.x>vectorViento.x) datosAnim->velLinealViento.x = vectorViento.x;
				} else {
					datosAnim->velLinealViento.x += ((vectorViento.x-datosAnim->velLinealViento.x) * datosAnim->factorResistenciaAire * segsTranscurridos * 4.0f);
					if(datosAnim->velLinealViento.x<vectorViento.x) datosAnim->velLinealViento.x = vectorViento.x;
				}
				if(datosAnim->velLinealViento.y<vectorViento.y){
					datosAnim->velLinealViento.y += ((vectorViento.y-datosAnim->velLinealViento.y) * datosAnim->factorResistenciaAire * segsTranscurridos * 4.0f);
					if(datosAnim->velLinealViento.y>vectorViento.y) datosAnim->velLinealViento.y = vectorViento.y;
				} else {
					datosAnim->velLinealViento.y += ((vectorViento.y-datosAnim->velLinealViento.y) * datosAnim->factorResistenciaAire * segsTranscurridos * 4.0f);
					if(datosAnim->velLinealViento.y<vectorViento.y) datosAnim->velLinealViento.y = vectorViento.y;
				}
				//
				datosAnim->segundosViva			+= segsTranscurridos;
				if(datosAnim->segundosViva<datosAnim->segundosVive){
					float factorVivido		= (datosAnim->segundosViva / datosAnim->segundosVive);
					datosParticula->alpha	= 1.0f - factorVivido; //En teoria si 'segundosDesaparece' es cero nunca se ejecuta
					datosParticula->escala	= datosAnim->escalaInicial + (datosAnim->escalaInicial * datosAnim->escalaParaDisipacion * factorVivido);
				} else {
					privInlineLiberarParticula(datosAnim->indice.iGrupoGL, datosAnim->indice.iParticula, datosParticula->objTextura->idTexturaGL);
					_datosAnimHumo.quitarElementoEnIndice(i--); iConteo--;
				}
			}
		}
		_partsCajaLocalSucia = true;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

#ifndef CONFIG_NB_UNSUPPORT_BOX2D
float32 AUEscenaGrupoParticulas::ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float32 fraction){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaTexto::ReportFixture")
	_resultadosColision.fixture		= fixture;
	_resultadosColision.normal		= normal;
	_resultadosColision.point		= point;
	_resultadosColision.fraction	= fraction;
	AU_GESTOR_PILA_LLAMADAS_POP_2
	/// return -1: ignore this fixture and continue
	/// return 0: terminate the ray cast
	/// return fraction: clip the ray to this point
	/// return 1: don't clip the ray and continue
	return 0;
}
#endif

//

NBCajaAABB AUEscenaGrupoParticulas::cajaAABBLocalCalculada(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaTexto::cajaAABBLocalCalculada")
	if(_partsCajaLocalSucia){
		NBCAJAAABB_INICIALIZAR(_partsCajaLocal)
		UI32 iGrupo, iGrupoConteo = _gruposPorTexturaGL.conteo;
		for(iGrupo=0; iGrupo<iGrupoConteo; iGrupo++){
			AUArregloNativoMutableP<STParticulaProps>* particulas = _gruposPorTexturaGL.elemento[iGrupo].particulas;
			UI32 iPart, iPartConteo = particulas->conteo;
			for(iPart=0; iPart<iPartConteo; iPart++){
				if(particulas->elemento[iPart].objTextura != NULL){
					NBPunto pos = particulas->elemento[iPart].posicion;
					NBCAJAAABB_ENVOLVER_PUNTO(_partsCajaLocal, pos.x, pos.y)
				}
			}
		}
		_partsCajaLocalSucia = false;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _partsCajaLocal;
}

void AUEscenaGrupoParticulas::agregadoEnEscena(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaGrupoParticulas::agregadoEnEscena")
	NBGestorAnimadores::agregarAnimador(this, this);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaGrupoParticulas::quitandoDeEscena(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaGrupoParticulas::quitandoDeEscena")
	NBGestorAnimadores::quitarAnimador(this);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

PTRfunCmdsGL AUEscenaGrupoParticulas::actualizarModeloGL(const NBPropRenderizado &propsRenderizado, const NBPropIluminacion &propsIluminacion, const NBPropHeredadasRender &propsHeredadas){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaGrupoParticulas::actualizarModeloGL")
	NBASSERT(idEscena!=-1) //Solo si esta en escena
	//Conteo de todas las particulas a renderizar
	#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
	SI32 conteoParticulasAgregadas = 0;
	#endif
	SI32 conteoParticulasTotal					= _conteoParticulas;
	if(conteoParticulasTotal!=0){
		NBColor8 colorMultiplicado8;			NBCOLOR_ESTABLECER(colorMultiplicado8, _propiedades.color8.r*propsHeredadas.colorPadre.r, _propiedades.color8.g*propsHeredadas.colorPadre.g, _propiedades.color8.b*propsHeredadas.colorPadre.b, _propiedades.color8.a*propsHeredadas.colorPadre.a)
		STBloqueGL bloqueVerticesGL;
		bloqueVerticesGL.cantidadElementos		= conteoParticulasTotal * 4;
		bloqueVerticesGL.primerElemento			= (*propsRenderizado.funcReservarVertices)(ENVerticeGlTipo_MonoTextura, bloqueVerticesGL.cantidadElementos);
		NBVerticeGL* verticeGL					= &((*propsRenderizado.verticesGL1)[bloqueVerticesGL.primerElemento]);
		//Datos render
		const SI32 indiceDatosRender			= propsRenderizado.bytesDatosModelos->conteo; propsRenderizado.bytesDatosModelos->reservarRegistrosAlFinal(sizeof(STGrupoParticulasRender));
		const SI32 indiceArregloGrupos			= propsRenderizado.bytesDatosModelos->conteo; propsRenderizado.bytesDatosModelos->reservarRegistrosAlFinal(sizeof(STGrupoParticulasRenderGrupo) * _gruposPorTexturaGL.conteo);
		STGrupoParticulasRender* datosRender	= (STGrupoParticulasRender*)&(propsRenderizado.bytesDatosModelos->elemento[indiceDatosRender]);
		STGrupoParticulasRenderGrupo* arregloGrupos = (STGrupoParticulasRenderGrupo*)&(propsRenderizado.bytesDatosModelos->elemento[indiceArregloGrupos]);
		datosRender->cantGruposGL				= _gruposPorTexturaGL.conteo;
		//Retener todas las texturas de las particulas
		UI32 iGrupoAU, iGrupoAUConteo = _gruposPorTexturaAU.conteo;
		for(iGrupoAU=0; iGrupoAU<iGrupoAUConteo; iGrupoAU++){
			propsRenderizado.piscinaRetecionRender->agregarElemento(_gruposPorTexturaAU.elemento[iGrupoAU].objTextura);
		}
		//Arreglos de grupos
		NBASSERT(_gruposPorTexturaGL.conteo!=0)
		//
		UI32 iGrupoGL, iGrupoGLConteo			= _gruposPorTexturaGL.conteo;
		for(iGrupoGL=0; iGrupoGL<iGrupoGLConteo; iGrupoGL++){
			STParticulaGrupoGL grupo			= _gruposPorTexturaGL.elemento[iGrupoGL];
			arregloGrupos[iGrupoGL].idTexturaGL = (grupo.idTexturaGL!=-1) ? grupo.idTexturaGL : 0;
			arregloGrupos[iGrupoGL].cantModelos = 0;
			//
			AUArregloNativoMutableP<STParticulaProps>* particulas = _gruposPorTexturaGL.elemento[iGrupoGL].particulas;
			if(particulas->conteo == 0) continue;
			UI16 iPart, conteoParts				= particulas->conteo;
			for(iPart=0; iPart<conteoParts; iPart++){
				STParticulaProps* datosModelo	= &particulas->elemento[iPart];
				if(datosModelo->objTextura == NULL) continue;
				#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
				conteoParticulasAgregadas++;
				#endif
				arregloGrupos[iGrupoGL].cantModelos++;
				//
				float alphaParticula = colorMultiplicado8.a * datosModelo->alpha;
				NBTamano tamanoTextura	= datosModelo->objTextura->tamanoHD();
				NBPunto puntosLocales[4];
				if(datosModelo->texAlineacionH<0){
					puntosLocales[0].x = -tamanoTextura.ancho * datosModelo->escala;
					puntosLocales[1].x = 0.0f;
					puntosLocales[2].x = -tamanoTextura.ancho * datosModelo->escala;
					puntosLocales[3].x = 0.0f;
				} else if(datosModelo->texAlineacionH>0){
					puntosLocales[0].x = 0.0f;
					puntosLocales[1].x = tamanoTextura.ancho * datosModelo->escala;
					puntosLocales[2].x = 0.0f;
					puntosLocales[3].x = tamanoTextura.ancho * datosModelo->escala;
				} else {
					puntosLocales[0].x = tamanoTextura.ancho * datosModelo->escala / -2.0f;
					puntosLocales[1].x = tamanoTextura.ancho * datosModelo->escala / 2.0f;
					puntosLocales[2].x = tamanoTextura.ancho * datosModelo->escala / -2.0f;
					puntosLocales[3].x = tamanoTextura.ancho * datosModelo->escala / 2.0f;
				}
				if(datosModelo->texAlineacionV<0){
					puntosLocales[0].y = tamanoTextura.alto * datosModelo->escala;
					puntosLocales[1].y = tamanoTextura.alto * datosModelo->escala;
					puntosLocales[2].y = 0.0f;
					puntosLocales[3].y = 0.0f;
				} else if(datosModelo->texAlineacionV>0){
					puntosLocales[0].y = 0.0f;
					puntosLocales[1].y = 0.0f;
					puntosLocales[2].y = tamanoTextura.alto * datosModelo->escala;
					puntosLocales[3].y = tamanoTextura.alto * datosModelo->escala;
				} else {
					puntosLocales[0].y = tamanoTextura.alto * datosModelo->escala / -2.0f;
					puntosLocales[1].y = tamanoTextura.alto * datosModelo->escala / -2.0f;
					puntosLocales[2].y = tamanoTextura.alto * datosModelo->escala / 2.0f;
					puntosLocales[3].y = tamanoTextura.alto * datosModelo->escala / 2.0f;
				}
				//GOTA NORMAL
				NBPUNTO_ROTAR_RADIANES(verticeGL[0], puntosLocales[0], datosModelo->radianes);	verticeGL[0].x += datosModelo->posicion.x; verticeGL[0].y += datosModelo->posicion.y;
				NBPUNTO_ROTAR_RADIANES(verticeGL[1], puntosLocales[1], datosModelo->radianes);	verticeGL[1].x += datosModelo->posicion.x; verticeGL[1].y += datosModelo->posicion.y;
				NBPUNTO_ROTAR_RADIANES(verticeGL[2], puntosLocales[2], datosModelo->radianes);	verticeGL[2].x += datosModelo->posicion.x; verticeGL[2].y += datosModelo->posicion.y;
				NBPUNTO_ROTAR_RADIANES(verticeGL[3], puntosLocales[3], datosModelo->radianes);	verticeGL[3].x += datosModelo->posicion.x; verticeGL[3].y += datosModelo->posicion.y;
				NBCOLOR_ESTABLECER(verticeGL[0], colorMultiplicado8.r, colorMultiplicado8.g, colorMultiplicado8.b, alphaParticula)
				NBCOLOR_ESTABLECER(verticeGL[1], colorMultiplicado8.r, colorMultiplicado8.g, colorMultiplicado8.b, alphaParticula)
				NBCOLOR_ESTABLECER(verticeGL[2], colorMultiplicado8.r, colorMultiplicado8.g, colorMultiplicado8.b, alphaParticula)
				NBCOLOR_ESTABLECER(verticeGL[3], colorMultiplicado8.r, colorMultiplicado8.g, colorMultiplicado8.b, alphaParticula)
				datosModelo->objTextura->cargarCoordenadasTextura(verticeGL);
				verticeGL += 4;
			}
		}
		NBASSERT(conteoParticulasTotal > 0)
		NBASSERT(verticeGL == &((*propsRenderizado.verticesGL1)[bloqueVerticesGL.primerElemento + bloqueVerticesGL.cantidadElementos]))
		datosRender->bloqueIndicesGL			= NB_GESTOR_GL_DAME_ELEMS_PARA_TRIANGSTRIPS_4(ENVerticeGlTipo_MonoTextura, bloqueVerticesGL, conteoParticulasTotal);
	}
	NBASSERT(conteoParticulasAgregadas==conteoParticulasTotal)
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return (conteoParticulasTotal!=0 ? &AUEscenaGrupoParticulas::enviarComandosGL : NULL);
}

void AUEscenaGrupoParticulas::enviarComandosGL(UI8* punteroDatosModelo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaGrupoParticulas::enviarComandosGL")
	STGrupoParticulasRender* datosRender		= (STGrupoParticulasRender*)punteroDatosModelo;
	STGrupoParticulasRenderGrupo* arregloGrupos	= (STGrupoParticulasRenderGrupo*)(punteroDatosModelo + sizeof(STGrupoParticulasRender));
	NBASSERT(datosRender->bloqueIndicesGL.cantidadElementos>0)
	NBGestorGL::activarVerticesGL(ENVerticeGlTipo_MonoTextura); NBGESTORGL_DBG_NOMBRAR_ACTIVADOR_VERTICES_GL("AUEscenaGrupoParticulas")
	SI32 iGrupo, iIndice = datosRender->bloqueIndicesGL.primerElemento;
	for(iGrupo=0; iGrupo<datosRender->cantGruposGL; iGrupo++){
		if(arregloGrupos[iGrupo].cantModelos!=0){
			NBGestorGL::bindTexture(0, arregloGrupos[iGrupo].idTexturaGL);
			NB_GESTOR_GL_RENDER_TRIANGSTRIPS_4(iIndice, NB_GESTOR_GL_ELEMS_POR_TRIANGSTRIP * arregloGrupos[iGrupo].cantModelos); NBGESTORGL_DBG_NOMBRAR_CONVOCADOR_ACUMULAR_TRIANG_STRIPS_INDEP("AUEscenaGrupoParticulas")
			iIndice += (NB_GESTOR_GL_ELEMS_POR_TRIANGSTRIP * arregloGrupos[iGrupo].cantModelos);
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

//
AUOBJMETODOS_CLASESID_MULTICLASE(AUEscenaGrupoParticulas, AUEscenaObjeto)
AUOBJMETODOS_CLASESNOMBRES_MULTICLASE(AUEscenaGrupoParticulas, "AUEscenaGrupoParticulas", AUEscenaObjeto)
AUOBJMETODOS_CLONAR_NULL(AUEscenaGrupoParticulas)






