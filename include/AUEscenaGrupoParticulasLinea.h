//
//  AUEscenaGrupoParticulasLinea.h
//  Gameplay_Mac
//
//  Created by Marcos Ortega Morales on 12/10/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef AUEscenaGrupoParticulasLineaLINEA_H_INCLUIDO
#define AUEscenaGrupoParticulasLineaLINEA_H_INCLUIDO

#include "AUEscenaGrupoParticulas.h"
#include "NBGestorTexturas.h"
#include "NBGestorPersonajes.h"

struct STGrupoParticulasLineaRender {
	UI32		idTexturaGL;
	UI32		cantModelos;
	STBloqueGL	bloqueIndicesGL;
};

class AUEscenaGrupoParticulasLinea: public AUEscenaObjeto, public NBAnimador, public IPersonajeVictimario {
	public:
		AUEscenaGrupoParticulasLinea();
		AUEscenaGrupoParticulasLinea(AUEscenaGrupoParticulasLinea* otraInstancia);
		virtual ~AUEscenaGrupoParticulasLinea();
		//
		UI16				conteoParticulasHumo();
		float				longitudHaciaDerecha();
		float				particulasPorSegundoPorMetro();
		float				segsDesaparecen();
		float				velocidadBurbuja();
		float				escalaInicial();
		float				escalaParaDisipar();
		//
		void				establecerLongitudHaciaDerecha(float longitud);
		void				establecerParticulasPorSegundoPorMetro(float particulasPorSegundoPorMetro);
		void				establecerSegsDesaparecen(float segsDesaparecen);
		void				establecerVelocidadBurbuja(float velocidadBurbuja);
		void				establecerEscalaInicial(float escalaInicial);
		void				establecerEscalaParaDisipar(float escalaParaDisipar);
		//
		virtual bool		personajeDanoHaciaVictima(const NBCajaAABB &cajaEscenaVictima, STPersonajeDano* guardarDanoEn);
		//
		virtual void		tickAnimacion(float segsTranscurridos);
		//
		virtual	NBCajaAABB	cajaAABBLocalCalculada();
		virtual PTRfunCmdsGL actualizarModeloGL(const NBPropRenderizado &propsRenderizado, const NBPropIluminacion &propsIluminacion, const NBPropHeredadasRender &propsHeredadas);
		static void			enviarComandosGL(UI8* punteroDatosModelo);
		//guardar y cargar en XML
		static bool			agregarXmlInternoEn(AUEscenaGrupoParticulasLinea* lineaParts, AUCadenaLargaMutable8* guardarEn, const char* espaciosBaseIzq);
		static bool			interpretarXmlInterno(AUEscenaGrupoParticulasLinea* cargarEn, const AUDatosXML* datosXml, const sNodoXML* nodoXml);
		//guardar y cargar en binario
		static bool			agregarBitsInternosEn(AUEscenaGrupoParticulasLinea* lineaParts, AUArchivo* guardarEn);
		static bool			interpretarBitsInternos(AUEscenaGrupoParticulasLinea* cargarEn, AUArchivo* cargarDe);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	protected:
		virtual void		agregadoEnEscena();
		virtual void		quitandoDeEscena();
	private:
		bool				_partsCajaLocalSucia;
		NBCajaAABB			_partsCajaLocal;
		AUTextura*			_texturaParticula;
		NBTamano			_texturaTamano;
		float				_segsAcumSinGenerar;
		//Propiedades compartidas de animacion
		float				_longitudHaciaDerecha;
		float				_particulasPorSegundoPorMetro;
		float				_segsDesaparecen;
		float				_velocidadBurbuja;
		float				_escalaInicial;
		float				_escalaParaDisipar;
		//
		AUArregloNativoMutableP<NBPropiedadesEnEscena>	_datosParticulas;
		AUArregloNativoMutableP<STParticulaAnimHumo>	_datosAnimHumo;
		void				privAgregarParticulaHumo(float posX, float posY);
};

#endif
