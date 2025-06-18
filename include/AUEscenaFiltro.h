//
//  AUEscenaFiltro.h
//  EditorCuerpos
//
//  Created by Nicaragua Binary on 24/12/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef AUEscenaFiltro_H_INCLUIDO
#define AUEscenaFiltro_H_INCLUIDO

#include "AUObjeto.h"
#include "AUEscenaObjeto.h"
#include "AUEscenaContenedor.h"
#include "AUArreglo.h"
#include "AUArregloMutable.h"
#include "AUArregloNativo.h"
#include "AUArregloNativoMutable.h"
#include "NBCajaAABB.h"
#include "AUCadenaMutable.h"
#include "AUDatosXML.h"
#include "AUDatosCSV.h"

class AUEscenaFiltro : public AUEscenaObjeto {
	public:
		AUEscenaFiltro();
		AUEscenaFiltro(AUEscenaFiltro* otraInstancia);
		virtual ~AUEscenaFiltro();
		//
		void						actualizarObjetosEnFiltroSegunInstancia(AUEscenaFiltro* otraInstancia, AUArreglo* hijosOriginales, AUArreglo* hijosDestino);
		//
		SI32						conteoObjetosEnFiltro();
		//
		void						agregarObjetoFiltrado(AUEscenaObjeto* objetoFiltrar);
		void						vaciarObjetosFiltrados();
		//
		virtual void				establecerDormido(bool dormido);
		//
		virtual	NBCajaAABB			cajaAABBLocalCalculada();
		virtual PTRfunCmdsGL		actualizarModeloGL(const NBPropRenderizado &propsRenderizado, const NBPropIluminacion &propsIluminacion, const NBPropHeredadasRender &propsHeredadas);
		//guardar y cargar en XML
		static bool					interpretarXmlInterno(AUEscenaFiltro* cargarEn, const AUDatosXML* datosXml, const sNodoXML* nodoXml, AUArregloNativoP<AUEscenaObjeto*>* arregloObjetos, AUArregloNativoP<SI32>* arregloIDs);
		//guardar y cargar en binario
		static bool					agregarBitsInternosEn(AUEscenaFiltro* filtro, AUArchivo* guardarEn, AUArreglo* arregloObjetosParaID);
		static bool					interpretarBitsInternos(AUEscenaFiltro* cargarEn, AUArchivo* cargarDe, AUArreglo* arregloObjetosParaID);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	private:
		AUArregloMutable			_objetosEnFiltro;
};

#endif
