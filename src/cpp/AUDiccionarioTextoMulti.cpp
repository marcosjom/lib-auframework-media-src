
#include "AUFrameworkMediaStdAfx.h"
#include "AUDiccionarioTextoMulti.h"

AUDiccionarioTextoMulti::AUDiccionarioTextoMulti(const ENIdioma* prioridadesIdiomas, const char* soloNombreRecurso) : AUObjeto() {
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUDiccionarioTextoMulti::AUDiccionarioTextoMulti")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUDiccionarioTextoMulti")
	//
	_retornoPorDefecto[0] = '\0';
	//
	SI32 iPrioridad;
	for(iPrioridad=0; iPrioridad<ENIdioma_Conteo; iPrioridad++){
		_prioridadesIdiomas[iPrioridad] = prioridadesIdiomas[iPrioridad];
	}
	//
	AUCadenaMutable8* strTmp = new(ENMemoriaTipo_Temporal) AUCadenaMutable8();
	char strIdIdioma[3];
	SI32 iIdioma;
	for(iIdioma=0; iIdioma<ENIdioma_Conteo; iIdioma++){
		_diccionariosPorIdioma[iIdioma] = NULL;
	}
	for(iIdioma=0; iIdioma<ENIdioma_Conteo; iIdioma++){
		strTmp->vaciar();
		if(iIdioma==ENIdioma_ES){
			strIdIdioma[0] = 'e'; strIdIdioma[1] = 's'; strIdIdioma[2] = '\0';
		} else if(iIdioma==ENIdioma_EN){
			strIdIdioma[0] = 'e'; strIdIdioma[1] = 'n'; strIdIdioma[2] = '\0';
		} else if(iIdioma==ENIdioma_FR){
			strIdIdioma[0] = 'f'; strIdIdioma[1] = 'r'; strIdIdioma[2] = '\0';
		} else if(iIdioma==ENIdioma_DE){
			strIdIdioma[0] = 'd'; strIdIdioma[1] = 'e'; strIdIdioma[2] = '\0';
		} else if(iIdioma==ENIdioma_IT){
			strIdIdioma[0] = 'i'; strIdIdioma[1] = 't'; strIdIdioma[2] = '\0';
		} else {
			NBASSERT(false) //Idioma no contemplado en esta clase
			strIdIdioma[0] = '\0';
		}
		strTmp->agregarConFormato("Diccionarios/%s/%s", strIdIdioma, soloNombreRecurso);
		AUDiccionarioTextoMutable* dicc = new(this) AUDiccionarioTextoMutable();
		if(!dicc->cargarXml(NBGestorArchivos::rutaHaciaRecursoEnPaquete(strTmp->str()))){
			PRINTF_WARNING("MULTI_IDIOMA: no se pudo cargar el diccionario '%s'\n", strTmp->str());
		#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
		} else if(iIdioma>0) {
			NBASSERT(_diccionariosPorIdioma[0] != NULL)
			SI32 iLLave, cantLlaves;
			//Identificar llaves que no existen en este diccionario
			cantLlaves = _diccionariosPorIdioma[0]->conteoLLaves();
			for(iLLave=0; iLLave<cantLlaves; iLLave++){
				const char* strLlave = _diccionariosPorIdioma[0]->llaveEnIndice(iLLave);
				SI32 indiceLlaveHermana = dicc->indiceParaLlave(strLlave);
				if(indiceLlaveHermana==-1){
					PRINTF_WARNING("MULTI_IDIOMA (%s): no existe '%s' en '%s'\n", strIdIdioma, strLlave, strTmp->str());
				}
			}
			//Identificar llaves nuevas en este diccionario
			cantLlaves = dicc->conteoLLaves();
			for(iLLave=0; iLLave<cantLlaves; iLLave++){
				const char* strLlave = dicc->llaveEnIndice(iLLave);
				SI32 indiceLlaveHermana = _diccionariosPorIdioma[0]->indiceParaLlave(strLlave);
				if(indiceLlaveHermana==-1){
					PRINTF_WARNING("MULTI_IDIOMA (%s): registra nueva '%s' en '%s'\n", strIdIdioma, strLlave, strTmp->str());
				}
			}
		#endif
		}
		_diccionariosPorIdioma[iIdioma] = dicc;
	}
	strTmp->liberar(NB_RETENEDOR_THIS);
	AU_GESTOR_PILA_LLAMADAS_POP
}

AUDiccionarioTextoMulti::AUDiccionarioTextoMulti(AUDiccionarioTextoMulti* otraInstancia){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUDiccionarioTextoMulti::AUDiccionarioTextoMulti")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUDiccionarioTextoMulti")
	//
	_retornoPorDefecto[0] = '\0';
	//
	SI32 iPrioridad;
	for(iPrioridad=0; iPrioridad<ENIdioma_Conteo; iPrioridad++){
		_prioridadesIdiomas[iPrioridad] = otraInstancia->_prioridadesIdiomas[iPrioridad];
	}
	//
	SI32 iIdioma;
	for(iIdioma=0; iIdioma<ENIdioma_Conteo; iIdioma++){
		_diccionariosPorIdioma[iIdioma] = otraInstancia->_diccionariosPorIdioma[iIdioma]; //El diccionario sera compartido
		_diccionariosPorIdioma[iIdioma]->retener(NB_RETENEDOR_THIS);
	}
	AU_GESTOR_PILA_LLAMADAS_POP
}

AUDiccionarioTextoMulti::~AUDiccionarioTextoMulti(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUDiccionarioTextoMulti::~AUDiccionarioTextoMulti")
	SI32 iIdioma;
	for(iIdioma=0; iIdioma<ENIdioma_Conteo; iIdioma++){
		NBASSERT(_diccionariosPorIdioma[iIdioma] != NULL)
		_diccionariosPorIdioma[iIdioma]->liberar(NB_RETENEDOR_THIS);
		_diccionariosPorIdioma[iIdioma] = NULL;
	}
	AU_GESTOR_PILA_LLAMADAS_POP
}

//

const char* AUDiccionarioTextoMulti::textoParaLlave(const char* strLlave){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUDiccionarioTextoMulti::textoParaLlave")
	const char* cadena = NULL;
	SI32 iPrioridad = 0;
	while(cadena == NULL && iPrioridad<ENIdioma_Conteo){
		cadena = _diccionariosPorIdioma[_prioridadesIdiomas[iPrioridad++]]->textoParaLlave(strLlave);
	}
	AU_GESTOR_PILA_LLAMADAS_POP
	return (cadena != NULL?cadena:_retornoPorDefecto);
}

//
AUOBJMETODOS_CLASESID_UNICLASE(AUDiccionarioTextoMulti)
AUOBJMETODOS_CLASESNOMBRES_UNICLASE(AUDiccionarioTextoMulti, "AUDiccionarioTextoMulti")
AUOBJMETODOS_CLONAR_THIS(AUDiccionarioTextoMulti)





