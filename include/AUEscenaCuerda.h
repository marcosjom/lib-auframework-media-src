//
//  AUEscenaCuerda.h
//  Gameplay_Mac
//
//  Created by Nicaragua Binary on 10/04/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef AUESCENACUERDA_H_INCLUIDO
#define AUESCENACUERDA_H_INCLUIDO

#include "NBPunto.h"
#include "NBTamano.h"
#include "NBColor.h"
#include "NBCajaAABB.h"
#include "AUEscenaContenedor.h"
#include "NBAnimador.h"
#include "NBGestorAnimadores.h"
#include "AUEscenaSprite.h"
#include "NBGestorEscena.h"
#include "AUEscenaCuerdaDef.h"
#include "AUDatosXML.h"

#define AUESCENACUERDA_BIT_ANCLA_INI_ACTIVA			1
#define AUESCENACUERDA_BIT_ANCLA_FIN_ACTIVA			2
#define AUESCENACUERDA_BIT_PUEDE_CORTARSE			4
#define AUESCENACUERDA_BIT_VERTICES_SON_LOCALES		8

class AUEscenaCuerda;

class IEscuchadorCuerda {
	public:
		virtual ~IEscuchadorCuerda(){
			//
		}
		virtual void cuerdaCortada(AUEscenaCuerda* objCuerda, UI16 iSegmentoCorte) = 0;
};

struct STCuerdaRender {
	UI32		idTextura;
	STBloqueGL	bloqueIndicesGL;
};

class AUEscenaCuerda : public AUEscenaObjeto, public NBAnimador, public ICuerdaCortable {
	public:
		AUEscenaCuerda(ENCuerdaTipo tipoCuerda, AUTextura* texturaSegmentos, float longitudCuerdaEscena, float masaCadaSegmento, float adicionalSegmentos);
		AUEscenaCuerda(AUEscenaCuerda* otraInstancia);
		virtual ~AUEscenaCuerda();
		//
		AUEscenaCuerda*				crearCuerdaAPartirDeSegmento(UI16 iSegmentoInicial);
		void						quitarSegmentosDespuesDeInclusive(UI16 iSegmentoInicial);
		void						inicializarCuerdaSegunLinea(const float xFisicaIni, const float yFisicaIni, const float xFisicaFin, const float yFisicaFin);
		void						agregarSegmentosDeCuerda(AUEscenaCuerda* otraCuerda, UI16 iPrimerSegmento, UI16 iUltimoSegmento);
		void						agregarSegmentosDeCuerdaOrdenInverso(AUEscenaCuerda* otraCuerda, UI16 iPrimerSegmento, UI16 iUltimoSegmento);
		//
		ENCuerdaTipo				tipoCuerda();
		AUTextura*					texturaSegmentosCuerda();
		IEscuchadorCuerda*			escuchadorCuerda();
		void*						unionCuerda(); //b2RopeJoint
		float						adicionalSegmentos();
		float						masaSegmentos();
		float						factorEstiramiento();
		float						segsNecesariosParaCorte();
		float						k2();
		float						k3();
		UI16						indicePrimerSegmentoHabilitado();
		UI16						indiceUltimoSegmentoHabilitado();
		void						establecerUnionCuerda(void* unionCuerdaOpcional); //b2RopeJoint*
		void						establecerEscuchadorCuerda(IEscuchadorCuerda* escuchador);
		//
		bool						anclaIniHabilitada();
		bool						anclaFinHabilitada();
		NBPunto						posAnclaIni();
		NBPunto						posAnclaFin();
		float						longitudHabilitadaCuerda();
		NBPunto						posLocalPrimerVerticeHabilitado();
		NBPunto						posLocalUltimoVerticeHabilitado();
		//
		void						establecerMundoFisica(const SI32 iMundoFisica);
		void						establecerTipoCuerda(ENCuerdaTipo tipo);
		void						establecerTexturaSegmentos(AUTextura* textura);
		void						establecerAdicionalSegmentos(float longitudAdicional);
		void						establecerLongitudCuerdaHabilitada(float longitudHabilitadaCuerdaEscena, float desplazamientoAlInicio);
		void						establecerMasaSegmentos(float masa);
		void						establecerFactorEstiramiento(float factor);
		void						establecerSegsNecesariosParaCorte(float segundos);
		void						establecerK2(float k2);
		void						establecerK3(float k3);
		void						habilitarAnclaCuerdaIni(float xLocalAncla, float yLocalAncla);
		void						habilitarAnclaCuerdaFin(float xLocalAncla, float yLocalAncla);
		void						deshabilitarAnclaCuerdaIni();
		void						deshabilitarAnclaCuerdaFin();
		//
		virtual bool				cuerdaAnalizarSiRayoCorta(const STEscenaLaserPos* arrPosEscena, const UI16 conteoPosEscena, STCuerdaCorte* guardarCorteEn);
		virtual void				cuerdaConsumirCorteDeRayo(const UI16 &iSegmentoCorte, const float factorCorteRayo, const float segsTranscurridos);
		virtual void				cuerdaRetenerParaAnalisisCorte();
		virtual void				cuerdaLiberarDeAnalisisCorte();
		//
		virtual	NBCajaAABB			cajaAABBLocalCalculada();
		virtual PTRfunCmdsGL		actualizarModeloGL(const NBPropRenderizado &propsRenderizado, const NBPropIluminacion &propsIluminacion, const NBPropHeredadasRender &propsHeredadas);
		static void					enviarComandosGL(UI8* punteroDatosModelo);
		//
		virtual void				tickAnimacion(float segsTranscurridos);
		//guardar y cargar en XML
		static bool					agregarXmlInternoEn(AUEscenaCuerda* cuerda, AUCadenaLargaMutable8* guardarEn, const char* espaciosBaseIzq);
		static bool					interpretarXmlInterno(AUEscenaCuerda* cargarEn, const AUDatosXML* datosXml, const sNodoXML* nodoXml);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	protected:
		UI8							_tipoCuerda;
		UI8							_conteoSegmentos;
		UI8							_mascaraPropsCuerda;
		bool						_registradoComoCortable;
		bool						_cuerdaCajaLocalSucia;
		NBCajaAABB					_cuerdaCajaLocal;
		float						_adicionalEntreSegmentos;	//Para las cadenas de dos texturas
		float						_masaCadaSegmento;
		float						_longitudHabilitadaCuerda;
		float						_desplazamientoCuerdaInicio;
		float						_segsNecesariosParaCorte;	//Segundos necesarios para cortar la cuerda con laser factor 1.0f
		AUTextura*					_texSegmentos;
		void*						_unionCuerdaOpcional; //b2RopeJoint*
		IEscuchadorCuerda*			_escuchadorCuerda;
		//Anclaje de ambos extremos de la cuerda
		NBPunto						_anclaIniPos;
		NBPunto						_anclaFinPos;
		//Datos de simulacion
		SI32						_iMundoFisica;							//Gravedad
		float						_damping;								//Flexibilidad
		float						_k2;									//Constante 1
		float						_k3;									//Constante 2
		UI32						_indicePrimerSegmentoHabilitado;
		UI32						_indiceUltimoSegmentoHabilitado;
		//Datos de cuerda
		AUArregloNativoMutableP<STCuerdaVertice>		_datosVertices;
		AUArregloNativoMutableP<STCuerdaSegmento>		_datosSegmentos;
		//
		virtual void				agregadoEnEscena();
		virtual void				quitandoDeEscena();
		//
		void						privInicializarCuerda(SI32 cantVertices, float longitudSegmentos, float masaSegmentos);
#		ifndef CONFIG_NB_UNSUPPORT_BOX2D
		void						privStep(float segsTransucurridos, UI32 iteraciones);
		void						privSolveC2();
		void						privSolveC3();
#		endif
		float 						distanciaTotalSegmentosHabilitados();
		void 						anclarVertice(UI32 indiceVertice, float posX, float posY);
		void 						desanclarVertice(UI32 indiceVertice, float masaVertice);
	
};

#endif
