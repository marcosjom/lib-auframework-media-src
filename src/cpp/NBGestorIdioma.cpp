//
//  NBGestorIdioma.cpp
//  DodoTown
//
//  Created by Marcos Ortega on 23/08/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkMediaStdAfx.h"
#include "NBGestorIdioma.h"

bool											NBGestorIdioma::_gestorInicializado	= false;
ENIdioma										NBGestorIdioma::_priodidadIdiomas[ENIdioma_Conteo];
bool											NBGestorIdioma::_prioridadIdiomasEsManual	= false;
AUCadenaMutable8*								NBGestorIdioma::_strCompartidaDiccionarios	= NULL;
AUArregloNativoMutableP<STDiccMultiIdioma>*		NBGestorIdioma::_diccionarios	= NULL;
AUArregloNativoMutableP<STDialogoIdioma>*		NBGestorIdioma::_dialogos		= NULL;


bool NBGestorIdioma::inicializar(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorIdioma::inicializar")
	_gestorInicializado			= false;
	_prioridadIdiomasEsManual	= false;
	SI32 iPriodidad;
	for(iPriodidad=0; iPriodidad<ENIdioma_Conteo; iPriodidad++){
		_priodidadIdiomas[iPriodidad] = (ENIdioma)iPriodidad;
	}
	_strCompartidaDiccionarios = new(ENMemoriaTipo_General) AUCadenaMutable8();
	_strCompartidaDiccionarios->agregar('\0'); //El indice cero sera la cadena vacia
	_diccionarios	= new(ENMemoriaTipo_General) AUArregloNativoMutableP<STDiccMultiIdioma>();
	_dialogos		= new(ENMemoriaTipo_General) AUArregloNativoMutableP<STDialogoIdioma>();
	#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
	NBGestorIdioma::validarArregloPrioridadesIdiomas(_priodidadIdiomas);
	#endif
	_gestorInicializado			= true;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return true;
}

void NBGestorIdioma::finalizar(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorIdioma::finalizar")
	UI32 iDicc, iDiccConteo = _diccionarios->conteo;
	for(iDicc=0; iDicc<iDiccConteo; iDicc++){
		NBASSERT(_diccionarios->elemento[iDicc].diccionario != NULL)
		NBASSERT(_diccionarios->elemento[iDicc].diccionario->conteoReferencias() == 1) //Solo retenido por el gestor
		_diccionarios->elemento[iDicc].diccionario->liberar(NB_RETENEDOR_NULL);
		_diccionarios->elemento[iDicc].diccionario = NULL;
	}
	_diccionarios->liberar(NB_RETENEDOR_NULL);
	_diccionarios = NULL;
	//
	UI32 iDialg, iDialgConteo = _dialogos->conteo;
	for(iDialg=0; iDialg<iDialgConteo; iDialg++){
		NBASSERT(_dialogos->elemento[iDialg].dialogo != NULL)
		NBASSERT(_dialogos->elemento[iDialg].dialogo->conteoReferencias() == 1) //Solo retenido por el gestor
		_dialogos->elemento[iDialg].dialogo->liberar(NB_RETENEDOR_NULL);
		_dialogos->elemento[iDialg].dialogo = NULL;
	}
	_dialogos->liberar(NB_RETENEDOR_NULL);
	_dialogos = NULL;
	//
	_strCompartidaDiccionarios->liberar(NB_RETENEDOR_NULL);
	_strCompartidaDiccionarios = NULL;
	_gestorInicializado = false;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

bool NBGestorIdioma::gestorInicializado(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorIdioma::gestorInicializado")
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return _gestorInicializado;
}

void NBGestorIdioma::liberarRecursosSinUso(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorIdioma::liberarRecursosSinUso")
	//Diccionarios en cache
	SI32 iDicc;
	for(iDicc=_diccionarios->conteo-1; iDicc>=0; iDicc--){
		NBASSERT(_diccionarios->elemento[iDicc].diccionario != NULL)
		if(_diccionarios->elemento[iDicc].diccionario->conteoReferencias() == 1){ //Solo retenido por el gestor
			_diccionarios->elemento[iDicc].diccionario->liberar(NB_RETENEDOR_NULL);
			_diccionarios->elemento[iDicc].diccionario = NULL;
			_diccionarios->quitarElementoEnIndice(iDicc);
		}
	}
	SI32 iDialg;
	for(iDialg=_dialogos->conteo-1; iDialg>=0; iDialg--){
		NBASSERT(_dialogos->elemento[iDialg].dialogo != NULL)
		if(_dialogos->elemento[iDialg].dialogo->conteoReferencias() == 1){ //Solo retenido por el gestor
			_dialogos->elemento[iDialg].dialogo->liberar(NB_RETENEDOR_NULL);
			_dialogos->elemento[iDialg].dialogo = NULL;
			_dialogos->quitarElementoEnIndice(iDialg);
		}
	}
	if(_diccionarios->conteo == 0 && _dialogos->conteo == 0){
		_strCompartidaDiccionarios->vaciar();
		_strCompartidaDiccionarios->agregar('\0'); //El indice cero sera la cadena vacia
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

ENIdioma NBGestorIdioma::idiomaPrioritario(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorIdioma::idiomaPrioritario")
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return _priodidadIdiomas[0];
}

const ENIdioma* NBGestorIdioma::idiomasPorPrioridad(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorIdioma::idiomasPorPrioridad")
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return _priodidadIdiomas;
}

bool NBGestorIdioma::prioridadIdiomasEsManual(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorIdioma::prioridadIdiomasEsManual")
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return _prioridadIdiomasEsManual;
}

void NBGestorIdioma::establecerPrioridadIdioma(SI32 priodidadP, ENIdioma idiomaP){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorIdioma::establecerIdiomaActual")
	NBASSERT(priodidadP>=0 && priodidadP<ENIdioma_Conteo)
	if(priodidadP<0) priodidadP = 0;
	if(priodidadP>=ENIdioma_Conteo) priodidadP = ENIdioma_Conteo - 1;
	//
	ENIdioma priodidadesAnteriores[ENIdioma_Conteo];
	SI32 iPriodidad;
	for(iPriodidad=0; iPriodidad<ENIdioma_Conteo; iPriodidad++){
		priodidadesAnteriores[iPriodidad]  = _priodidadIdiomas[iPriodidad];
	}
	#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
	NBGestorIdioma::validarArregloPrioridadesIdiomas(priodidadesAnteriores);
	#endif
	//
	SI32 iPriodidadN = 0;
	for(iPriodidad=0; iPriodidad<ENIdioma_Conteo; iPriodidad++){
		if(iPriodidadN==priodidadP) _priodidadIdiomas[iPriodidadN++] = idiomaP;
		if(priodidadesAnteriores[iPriodidad]==idiomaP){
			//ignorar este espacio
		} else {
			_priodidadIdiomas[iPriodidadN++] = priodidadesAnteriores[iPriodidad];
		}
	}
	if(iPriodidadN==priodidadP) _priodidadIdiomas[iPriodidadN++] = idiomaP;
	//
	#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
	NBGestorIdioma::validarArregloPrioridadesIdiomas(_priodidadIdiomas);
	#endif
	NBASSERT(iPriodidadN==ENIdioma_Conteo)
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

void NBGestorIdioma::validarArregloPrioridadesIdiomas(ENIdioma* arregloPrioridades){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorIdioma::validarArregloPrioridadesIdiomas")
	SI32 iPriodidad;
	bool idiomaEncontrado[ENIdioma_Conteo];
	for(iPriodidad=0; iPriodidad<ENIdioma_Conteo; iPriodidad++){
		idiomaEncontrado[iPriodidad] = false;
	}
	for(iPriodidad=0; iPriodidad<ENIdioma_Conteo; iPriodidad++){
		NBASSERT(idiomaEncontrado[_priodidadIdiomas[iPriodidad]]==false) //Si falla, el idioma esta registrado dos veces
		idiomaEncontrado[_priodidadIdiomas[iPriodidad]] = true;
	}
	for(iPriodidad=0; iPriodidad<ENIdioma_Conteo; iPriodidad++){
		NBASSERT(idiomaEncontrado[iPriodidad]==true) //Si falla, el idioma no esta en la lista de prioridades
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

void NBGestorIdioma::establecerPrioridadIdiomasEsManual(bool prioridadIdiomasEsManual){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorIdioma::establecerPrioridadIdiomasEsManual")
	_prioridadIdiomasEsManual = prioridadIdiomasEsManual;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

//

AUDiccionarioTextoMulti* NBGestorIdioma::diccionario(const char* nombreRecurso){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorIdioma::diccionario")
	AUDiccionarioTextoMulti* diccMulti = NULL;
	//Buscar en la biblioteca actual
	UI32 iDicc, iDiccConteo = _diccionarios->conteo;
	for(iDicc=0; iDicc<iDiccConteo; iDicc++){
		NBASSERT(_diccionarios->elemento[iDicc].diccionario != NULL)
		if(AUCadena8::esIgual(nombreRecurso, &(_strCompartidaDiccionarios->str()[_diccionarios->elemento[iDicc].iNombreRecurso]))){
			diccMulti = _diccionarios->elemento[iDicc].diccionario;
			//PRINTF_INFO("NBGestorIdioma, OPTIMIZACION diccionarios '%s' cargados desde cache\n", nombreRecurso);
			break;
		}
	}
	//Cargar nuevo diccionario
	if(diccMulti == NULL) {
		diccMulti = new(ENMemoriaTipo_General) AUDiccionarioTextoMulti(_priodidadIdiomas, nombreRecurso);
		STDiccMultiIdioma datosDiccionario;
		datosDiccionario.diccionario	= diccMulti;
		datosDiccionario.iNombreRecurso	= _strCompartidaDiccionarios->tamano(); _strCompartidaDiccionarios->agregar(nombreRecurso, AUCadena8::tamano(nombreRecurso)+1, strCopiarPlecasCero);
		_diccionarios->agregarElemento(datosDiccionario);
		//PRINTF_INFO("NBGestorIdioma, diccionarios '%s' cargados desde archivos\n", nombreRecurso);
	}
	NBASSERT(diccMulti != NULL)
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return diccMulti;
}


AUDialogo* NBGestorIdioma::dialogo(const char* nombreRecurso){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorIdioma::dialogo")
	AUDialogoMutable* dialogooo = NULL;
	//Buscar en la biblioteca actual
	UI32 iDialg, iDialgConteo = _dialogos->conteo;
	for(iDialg=0; iDialg<iDialgConteo; iDialg++){
		NBASSERT(_dialogos->elemento[iDialg].dialogo != NULL)
		if(AUCadena8::esIgual(nombreRecurso, &(_strCompartidaDiccionarios->str()[_dialogos->elemento[iDialg].iNombreRecurso])) && _dialogos->elemento[iDialg].idioma==_priodidadIdiomas[0]){
			dialogooo = _dialogos->elemento[iDialg].dialogo;
			//PRINTF_INFO("NBGestorIdioma, OPTIMIZACION dialogo '%s' cargado desde cache\n", nombreRecurso);
			break;
		}
	}
	//Cargar nuevo dialogo
	if(dialogooo == NULL) {
		AUDialogoMutable* dialogo = new(ENMemoriaTipo_General) AUDialogoMutable();
		bool datosCargados = false;
		AUCadenaMutable8* strTmp = new(ENMemoriaTipo_Temporal) AUCadenaMutable8();
		SI32 iPrioridad;
		for(iPrioridad=0; iPrioridad<ENIdioma_Conteo && !datosCargados; iPrioridad++){
			strTmp->vaciar();
			if(_priodidadIdiomas[iPrioridad]==ENIdioma_ES){
				strTmp->agregarConFormato("Dialogos/es/%s", nombreRecurso);
			} else if(_priodidadIdiomas[iPrioridad]==ENIdioma_EN){
				strTmp->agregarConFormato("Dialogos/en/%s", nombreRecurso);
			} else if(_priodidadIdiomas[iPrioridad]==ENIdioma_FR){
				strTmp->agregarConFormato("Dialogos/fr/%s", nombreRecurso);
			} else if(_priodidadIdiomas[iPrioridad]==ENIdioma_DE){
				strTmp->agregarConFormato("Dialogos/de/%s", nombreRecurso);
			} else if(_priodidadIdiomas[iPrioridad]==ENIdioma_IT){
				strTmp->agregarConFormato("Dialogos/it/%s", nombreRecurso);
			} else {
				NBASSERT(false) //Idioma no contemplado
			}
			if(dialogo->cargarXml(NBGestorArchivos::rutaHaciaRecursoEnPaquete(strTmp->str()))){
				datosCargados = true;
			}
		}
		strTmp->liberar(NB_RETENEDOR_NULL);
		if(datosCargados){
			STDialogoIdioma datosDialogo;
			datosDialogo.dialogo		= dialogo; dialogo->retener(NB_RETENEDOR_NULL);
			datosDialogo.idioma			= _priodidadIdiomas[0];
			datosDialogo.iNombreRecurso	= _strCompartidaDiccionarios->tamano(); _strCompartidaDiccionarios->agregar(nombreRecurso, AUCadena8::tamano(nombreRecurso)+1, strCopiarPlecasCero);
			_dialogos->agregarElemento(datosDialogo);
			//PRINTF_INFO("NBGestorIdioma, diccionarios '%s' cargados desde archivos\n", nombreRecurso);
			dialogooo = dialogo;
		}
		dialogo->liberar(NB_RETENEDOR_NULL);
		dialogo = NULL;
	}
	NBASSERT(dialogooo != NULL)
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return dialogooo;
}


