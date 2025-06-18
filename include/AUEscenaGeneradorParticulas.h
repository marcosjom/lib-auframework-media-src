//
//  AUEscenaGeneradorParticulas.h
//  Gameplay_Mac
//
//  Created by Marcos Ortega Morales on 12/10/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef AUESCENAGENERADORPARTICULAS_H_INCLUIDO
#define AUESCENAGENERADORPARTICULAS_H_INCLUIDO

#include "AUEscenaObjeto.h"
#include "NBAnimador.h"
#include "NBGestorTexturas.h"
#include "NBGestorAnimadores.h"
#include "AUEscenaGrupoParticulas.h"
#include "NBGestorEscena.h"

struct STParticulaGen {
	STParticulaGen(){
		tipoParticula					= 0;
		particulasPorSeg				= 1;
		textura							= NULL;
		probabilidadUsarEsta			= 1.0f;
		segundosVive					= 8.0f;
		segundosDesaparece				= 0.5f;
		escalaInicial					= 1.0f;
		velocidadBaseLanza				= 64.0f;
		absDeltaVelocidadLanzamiento	= 0.0f;
		absDeltaRadianesLanzamiento		= 0.0f;
		//Propio de particulas hojas
		factorResistenciaAire			= 0.5f; //tambien es propiedad de humo
		factorTrayectoriaIrregular		= 0.5f;
		//Propio de particulas gotas
		segsParaConsultarColision		= 0.15f;
		//Propio de particulas humo
		escalarParaDisipar				= 3.0f;
	}
	UI8				tipoParticula;
	UI8				particulasPorSeg;
	AUTextura*		textura;
	float			probabilidadUsarEsta;
	float			segundosVive;
	float			segundosDesaparece;
	float			escalaInicial;
	float			velocidadBaseLanza;
	float			absDeltaVelocidadLanzamiento;
	float			absDeltaRadianesLanzamiento;
	//Propio de particulas hojas
	float			factorResistenciaAire;
	float			factorTrayectoriaIrregular;
	//Propio de particulas gotas
	float			segsParaConsultarColision;
	//Propio de particulas humo
	float			escalarParaDisipar;
	//
	float			segundoVirtualUltimoLanzamiento;
	bool operator==(const STParticulaGen &otro) const {
		return (tipoParticula==otro.tipoParticula && textura==otro.textura);
	}
	bool operator!=(const STParticulaGen &otro) const {
		return !(tipoParticula==otro.tipoParticula && textura==otro.textura);
	}
};

class AUEscenaGeneradorParticulas: public AUEscenaObjeto, public NBAnimador {
	public:
		AUEscenaGeneradorParticulas();
		AUEscenaGeneradorParticulas(AUEscenaGeneradorParticulas* otraInstancia);
		virtual ~AUEscenaGeneradorParticulas();
		//
		AUEscenaGrupoParticulas* grupoParticulas();
		void				establecerGrupoParticulas(AUEscenaGrupoParticulas* grupoParticulas);
		//
		void				agregarParticulaLibreGenerar(AUTextura* textura, SI16 particsPorSeg, float probabilidadUsarEsta, float segsVive, float segsDesaparece, float escalaInicial, float velLanza, float margenVelLanza, float margenRadianes);
		void				agregarParticulaHojaGenerar(AUTextura* textura, SI16 particsPorSeg, float probabilidadUsarEsta, float segsVive, float segsDesaparece, float escalaInicial, float velLanza, float margenVelLanza, float margenRadianes, float factorResistenciaAire, float factorTrayectoriaIrregular);
		void				agregarParticulaGotaGenerar(AUTextura* textura, SI16 particsPorSeg, float probabilidadUsarEsta, float segsVive, float segsDesaparece, float escalaInicial, float velLanza, float margenVelLanza, float margenRadianes, float segsParaConsultarColision);
		void				agregarParticulaHumoGenerar(AUTextura* textura, SI16 particsPorSeg, float probabilidadUsarEsta, float segsVive, float escalaInicial, float velLanza, float margenVelLanza, float margenRadianes, float escalaDisipacion, float factorResistenciaAire);
		void				agregarParticulaInmovilGenerar(AUTextura* textura, SI16 particsPorSeg, float probabilidadUsarEsta, float segsVive, float segsDesaparece, float escalaInicial, float escalaDisipacion, float margenRadianes);
		void				actualizarParticulaGenerarEnIndice(UI32 iParticulaGen, const STParticulaGen &datosParticula);
		void				quitarParticulaGenerarEnIndice(UI32 iParticulaGen);
		const STParticulaGen datosParticulaEnIndice(UI32 iParticulaGen);
		AUArregloNativoP<STParticulaGen>*	datosParticulas();
		//
		virtual void		tickAnimacion(float segsTranscurridos);
		//
		virtual	NBCajaAABB	cajaAABBLocalCalculada();
		virtual PTRfunCmdsGL actualizarModeloGL(const NBPropRenderizado &propsRenderizado, const NBPropIluminacion &propsIluminacion, const NBPropHeredadasRender &propsHeredadas);
		//guardar y cargar en XML
		static bool			agregarXmlInternoEn(AUEscenaGeneradorParticulas* generador, AUCadenaLargaMutable8* guardarEn, const char* espaciosBaseIzq);
		static bool			interpretarXmlInterno(AUEscenaGeneradorParticulas* cargarEn, const AUDatosXML* datosXml, const sNodoXML* nodoXml);
		//guardar y cargar en binario
		static bool			agregarBitsInternosEn(AUEscenaGeneradorParticulas* generador, AUArchivo* guardarEn);
		static bool			interpretarBitsInternos(AUEscenaGeneradorParticulas* cargarEn, AUArchivo* cargarDe);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	protected:
		virtual void		agregadoEnEscena();
		virtual void		quitandoDeEscena();
	private:
		AUEscenaGrupoParticulas* _grupoParticulas;
		AUArregloNativoMutableP<STParticulaGen>	_datosParticulas;
};

#endif
