//
//  NBGestorAnimadores.h
//  DodoTown
//
//  Created by Marcos Ortega on 23/08/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef NBGESTORIDIOMA_H_INCLUIDO
#define NBGESTORIDIOMA_H_INCLUIDO

#include "NBGestorPilaLlamadas.h"
#include "AUCadenaMutable.h"
#include "AUArregloMutable.h"
#include "AUArregloNativoMutableP.h"
#include "AUArchivo.h"
#include "NBGestorArchivos.h"
#include "NBGestorIdiomaDef.h"
#include "AUDiccionarioTextoMulti.h"
#include "AUDialogoMutable.h"

struct STDiccMultiIdioma {
	AUDiccionarioTextoMulti* diccionario;	//Objeto diccionario
	SI32 iNombreRecurso;					//Indice del nombre del recurso en la cadena compartida
	bool operator==(const STDiccMultiIdioma &otro) const {
		return (diccionario==otro.diccionario && iNombreRecurso==otro.iNombreRecurso);
	}
	bool operator!=(const STDiccMultiIdioma &otro) const {
		return !(diccionario==otro.diccionario && iNombreRecurso==otro.iNombreRecurso);
	}
};

struct STDialogoIdioma {
	AUDialogoMutable* dialogo;			//Objeto dialogo
	SI32 iNombreRecurso;		//Indice del nombre del recurso en la cadena compartida
	ENIdioma idioma;
	bool operator==(const STDialogoIdioma &otro) const {
		return (dialogo==otro.dialogo && iNombreRecurso==otro.iNombreRecurso && idioma==otro.idioma);
	}
	bool operator!=(const STDialogoIdioma &otro) const {
		return !(dialogo==otro.dialogo && iNombreRecurso==otro.iNombreRecurso && idioma==otro.idioma);
	}
};
		
class NBGestorIdioma {
	public:
		static bool				inicializar();
		static void				finalizar();
		static bool				gestorInicializado();
		//
		static void				liberarRecursosSinUso();
		//
		static ENIdioma			idiomaPrioritario();
		static const ENIdioma*	idiomasPorPrioridad();
		static bool				prioridadIdiomasEsManual();
		static void				establecerPrioridadIdioma(SI32 priodidad, ENIdioma idioma);
		static void				establecerPrioridadIdiomasEsManual(bool prioridadIdiomasEsManual);
		//
		static void				validarArregloPrioridadesIdiomas(ENIdioma* arregloPrioridades);
		//
		static AUDiccionarioTextoMulti* diccionario(const char* nombreRecurso);
		static AUDialogo*		dialogo(const char* nombreRecurso);
	private:
		static bool				_gestorInicializado;
		static ENIdioma			_priodidadIdiomas[ENIdioma_Conteo];
		static bool				_prioridadIdiomasEsManual; //Determina si el idioma actual fue establecido por el usuario o automaticamente definido
		static AUArregloNativoMutableP<STDiccMultiIdioma>*	_diccionarios;
		static AUArregloNativoMutableP<STDialogoIdioma>*	_dialogos;
		static AUCadenaMutable8* _strCompartidaDiccionarios;
};

#endif
