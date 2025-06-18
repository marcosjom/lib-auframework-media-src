#ifndef AUDiccionarioTextoMulti_H_INCLUIDO
#define AUDiccionarioTextoMulti_H_INCLUIDO

#include "AUObjeto.h"
#include "AUCadenaMutable.h"
#include "AUArregloNativoMutableP.h"
#include "AUDiccionarioTextoMutable.h"
#include "NBGestorIdiomaDef.h"

class AUDiccionarioTextoMulti : public AUObjeto {
	public:
		//constructores y destructores
		AUDiccionarioTextoMulti(const ENIdioma* prioridadesIdiomas, const char* soloNombreRecurso);
		AUDiccionarioTextoMulti(AUDiccionarioTextoMulti* otraInstancia);
		virtual ~AUDiccionarioTextoMulti();
		//funciones de instancia
		const char*					textoParaLlave(const char* strLlave);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	protected:
		//propiedades
		char						_retornoPorDefecto[1];
		ENIdioma					_prioridadesIdiomas[ENIdioma_Conteo];
		AUDiccionarioTextoMutable*	_diccionariosPorIdioma[ENIdioma_Conteo];
};

#endif