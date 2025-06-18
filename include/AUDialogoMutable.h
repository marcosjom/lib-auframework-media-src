#ifndef AUDialogoMutable_H_INCLUIDO
#define AUDialogoMutable_H_INCLUIDO

#include "AUDialogo.h"
#include "AUDatosXMLMutable.h"
#include "AUDatosCSV.h"
#include "NBGestorAnimaciones.h"

class AUDialogoMutable : public AUDialogo {
	public:
		//constructores y destructores
		AUDialogoMutable();
		virtual ~AUDialogoMutable();
		//
		bool				cargarXml(const char* rutaArchivo);
		bool				interpretarXml(const AUDatosXML* datosXml, const sNodoXML* nodoXml);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	protected:
		AUFuenteTextura*	privFuenteTexturaDesdeNodoXml(const AUDatosXML* datosXml, const sNodoXML* nodoFuente, AUCadenaMutable8* strTmpCompartida);
		void				privInterpretarParametrosEtiqueta(const char* listaParametros, STDialogoElem* datosElem, STDialogoImagen* datosImagen, STDialogoAnimacion* datosAnimacion);
};

#endif