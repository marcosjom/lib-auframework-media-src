
#include "AUFrameworkMediaStdAfx.h"
#include "AUEscenaLuzEspejo.h"

//-------------------------------------
//--- CONSTRUCTORES Y DESTRUCTORES ----
//-------------------------------------

AUEscenaLuzEspejo::AUEscenaLuzEspejo() : AUEscenaObjeto() {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaLuzEspejo::AUEscenaLuzEspejo")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaLuzEspejo")
	_registradoEnMotorIlum			= false;
	_factorReflejaLuz				= 1.0f;
	_limitesEspejo[0].x				= -16.0f;
	_limitesEspejo[0].y				= 0.0f;
	_limitesEspejo[1].x				= 16.0f;
	_limitesEspejo[1].y				= 0.0f;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUEscenaLuzEspejo::AUEscenaLuzEspejo(AUEscenaLuzEspejo* otra) : AUEscenaObjeto(otra) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaLuzEspejo::AUEscenaLuzEspejo")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaLuzEspejo")
	_registradoEnMotorIlum			= false;
	_factorReflejaLuz				= otra->_factorReflejaLuz;
	_limitesEspejo[0]				= otra->_limitesEspejo[0];
	_limitesEspejo[1]				= otra->_limitesEspejo[1];
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUEscenaLuzEspejo::~AUEscenaLuzEspejo(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaLuzEspejo::~AUEscenaLuzEspejo")
	NBASSERT(!_registradoEnMotorIlum) //No debe quedar registrado en el motor
	AU_GESTOR_PILA_LLAMADAS_POP_2
}
 
//-------------------------------
//--- FUNCIONES DE INSTANCIA ----
//-------------------------------

float AUEscenaLuzEspejo::factorReflejaLuz(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaLuzEspejo::factorReflejaLuz")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _factorReflejaLuz;
}

NBPunto AUEscenaLuzEspejo::limiteEspejoInicial(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaLuzEspejo::limiteEspejoInicial")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _limitesEspejo[0];
}

NBPunto AUEscenaLuzEspejo::limiteEspejoFinal(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaLuzEspejo::limiteEspejoFinal")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _limitesEspejo[1];
}

void AUEscenaLuzEspejo::establecerFactorReflejaLuz(float factorReflejaLuz){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaLuzEspejo::establecerAlcanceLuz")
	_factorReflejaLuz = factorReflejaLuz; this->privInlineActualizarLuzEnGestor();
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaLuzEspejo::establecerLimiteEspejoInicial(float xLocal, float yLocal){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaLuzEspejo::establecerAlcanceLuz")
	_limitesEspejo[0].x = xLocal; _limitesEspejo[0].y = yLocal; this->privInlineActualizarLuzEnGestor();
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaLuzEspejo::establecerLimiteEspejoFinal(float xLocal, float yLocal){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaLuzEspejo::establecerAlcanceLuz")
	_limitesEspejo[1].x = xLocal; _limitesEspejo[1].y = yLocal; this->privInlineActualizarLuzEnGestor();
	AU_GESTOR_PILA_LLAMADAS_POP_2
}
													
void AUEscenaLuzEspejo::establecerLimiteEspejoInicial(NBPunto posLocal){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaLuzEspejo::establecerAlcanceLuz")
	_limitesEspejo[0] = posLocal; this->privInlineActualizarLuzEnGestor();
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaLuzEspejo::establecerLimiteEspejoFinal(NBPunto posLocal){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaLuzEspejo::establecerAlcanceLuz")
	_limitesEspejo[1] = posLocal; this->privInlineActualizarLuzEnGestor();
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

NBCajaAABB AUEscenaLuzEspejo::cajaAABBLocalCalculada(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaLuzEspejo::cajaAABBLocalCalculada")
	NBCajaAABB cajaLocal; NBCAJAAABB_INICIALIZAR(cajaLocal);
	NBCAJAAABB_ENVOLVER_PUNTO(cajaLocal, _limitesEspejo[0].x, _limitesEspejo[0].y);
	NBCAJAAABB_ENVOLVER_PUNTO(cajaLocal, _limitesEspejo[1].x, _limitesEspejo[1].y);
	//Brindar un poco de volumen
	if(cajaLocal.xMin==cajaLocal.xMax){ cajaLocal.xMin -= 4.0f; cajaLocal.xMax += 4.0f; }
	if(cajaLocal.yMin==cajaLocal.yMax){ cajaLocal.yMin -= 4.0f; cajaLocal.yMax += 4.0f; }
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return cajaLocal;
}

void AUEscenaLuzEspejo::agregadoEnEscena(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaLuzEspejo::agregadoEnEscena")
	NBASSERT(!_registradoEnMotorIlum)
	NBGestorEscena::agregarFuenteEspejo(this->idEscena, this, _factorReflejaLuz, _limitesEspejo[0], _limitesEspejo[1]);
	_registradoEnMotorIlum = true;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaLuzEspejo::quitandoDeEscena(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaLuzEspejo::quitandoDeEscena")
	NBASSERT(_registradoEnMotorIlum)
	NBGestorEscena::quitarFuenteEspejo(this->idEscena, this);
	_registradoEnMotorIlum = false;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

PTRfunCmdsGL AUEscenaLuzEspejo::actualizarModeloGL(const NBPropRenderizado &propsRenderizado, const NBPropIluminacion &propsIluminacion, const NBPropHeredadasRender &propsHeredadas){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaLuzEspejo::actualizarModeloGL")
	NBASSERT(idEscena!=-1) //Solo si esta en escena
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return NULL;
}

//


bool AUEscenaLuzEspejo::agregarXmlInternoEn(AUEscenaLuzEspejo* luzEspejo, AUCadenaLargaMutable8* guardarEn, const char* espaciosBaseIzq){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaLuzEspejo::agregarXmlInternoEn")
	//guardarEn->agregarConFormato("%s<vXML>1</vXML>\r\n", espaciosBaseIzq);
	guardarEn->agregarConFormato("%s<eOb>\r\n", espaciosBaseIzq);
	AUEscenaObjeto::agregarXmlInternoEn(luzEspejo, guardarEn, espaciosBaseIzq);
	guardarEn->agregarConFormato("%s</eOb>\r\n", espaciosBaseIzq);
	guardarEn->agregarConFormato("%s<factRefl>%f</factRefl>\r\n", espaciosBaseIzq, luzEspejo->_factorReflejaLuz);
	guardarEn->agregarConFormato("%s<lIni>%f|%f</lIni>\r\n", espaciosBaseIzq, luzEspejo->_limitesEspejo[0].x, luzEspejo->_limitesEspejo[0].y);
	guardarEn->agregarConFormato("%s<lFin>%f|%f</lFin>\r\n", espaciosBaseIzq, luzEspejo->_limitesEspejo[1].x, luzEspejo->_limitesEspejo[1].y);
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return true;

}

bool AUEscenaLuzEspejo::interpretarXmlInterno(AUEscenaLuzEspejo* cargarEn, const AUDatosXML* datosXml, const sNodoXML* nodoXml){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaLuzEspejo::interpretarXmlInterno")
	bool exito = false;
	//int versionXML = datosXml->nodoHijo("vXML", 0, nodoXml);
	//if(versionXML==1){
		const sNodoXML* nodoEscenaObj;		XML_NODO_HIJO(datosXml, nodoEscenaObj, "eOb", "escObj", nodoXml, NULL)
		if(nodoEscenaObj != NULL){
			exito = AUEscenaObjeto::interpretarXmlInterno(cargarEn, datosXml, nodoEscenaObj);
			cargarEn->_factorReflejaLuz					= datosXml->nodoHijo("factRefl", 1.0f, nodoXml);
			const sNodoXML* nodoLIni					= datosXml->nodoHijo("lIni", nodoXml);
			if(nodoLIni != NULL){
				//Formato nuevo CSV (mas compacto)
				const char* cadenaCSV			= datosXml->cadenaValorDeNodo(nodoLIni);
				SI32 tamCadenaCSV				= AUCadena8::tamano(cadenaCSV);
				SI32 posEnCadenaCSV				= 0;
				cargarEn->_limitesEspejo[0].x	= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, -16.0f);
				cargarEn->_limitesEspejo[0].y	= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0.0f);
			} else {
				cargarEn->_limitesEspejo[0].x	= -16.0f;
				cargarEn->_limitesEspejo[0].y	= 0.0f;
			}
			const sNodoXML* nodoLFin					= datosXml->nodoHijo("lFin", nodoXml);
			if(nodoLFin != NULL){
				//Formato nuevo CSV (mas compacto)
				const char* cadenaCSV			= datosXml->cadenaValorDeNodo(nodoLFin);
				SI32 tamCadenaCSV				= AUCadena8::tamano(cadenaCSV);
				SI32 posEnCadenaCSV				= 0;
				cargarEn->_limitesEspejo[1].x	= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 16.0f);
				cargarEn->_limitesEspejo[1].y	= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0.0f);
			} else {
				cargarEn->_limitesEspejo[1].x	= 16.0f;
				cargarEn->_limitesEspejo[1].y	= 0.0f;
			}
			cargarEn->privInlineActualizarLuzEnGestor();
		}
	//}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

bool AUEscenaLuzEspejo::agregarBitsInternosEn(AUEscenaLuzEspejo* luzEspejo, AUArchivo* guardarEn){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaLuzEspejo::agregarBitsInternosEn")
	bool exito = false;
	//
	float versionDatos = 1.0f;
	guardarEn->escribir(&versionDatos, sizeof(versionDatos), 1, guardarEn);
	if(AUEscenaObjeto::agregarBitsInternosEn(luzEspejo, guardarEn)){
		guardarEn->escribir(&luzEspejo->_factorReflejaLuz, sizeof(luzEspejo->_factorReflejaLuz), 1, guardarEn);
		guardarEn->escribir(&luzEspejo->_limitesEspejo[0], sizeof(luzEspejo->_limitesEspejo[0]), 1, guardarEn);
		guardarEn->escribir(&luzEspejo->_limitesEspejo[1], sizeof(luzEspejo->_limitesEspejo[1]), 1, guardarEn);
		exito = true;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

bool AUEscenaLuzEspejo::interpretarBitsInternos(AUEscenaLuzEspejo* cargarEn, AUArchivo* cargarDe){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaLuzEspejo::interpretarBitsInternos")
	bool exito = false;
	float versionDatos;
	cargarDe->leer(&versionDatos, sizeof(versionDatos), 1, cargarDe);
	if(versionDatos==1.0f){
		if(AUEscenaObjeto::interpretarBitsInternos(cargarEn, cargarDe)){
			cargarDe->leer(&cargarEn->_factorReflejaLuz, sizeof(cargarEn->_factorReflejaLuz), 1, cargarDe);
			cargarDe->leer(&cargarEn->_limitesEspejo[0], sizeof(cargarEn->_limitesEspejo[0]), 1, cargarDe);
			cargarDe->leer(&cargarEn->_limitesEspejo[1], sizeof(cargarEn->_limitesEspejo[1]), 1, cargarDe);
			cargarEn->privInlineActualizarLuzEnGestor();
			exito = true;
		}
	} else {
		PRINTF_ERROR("AUEscenaLuzEspejo, la version (%f) de datos binarios no es interpretable por este ejecutable\n", versionDatos);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}


//

AUOBJMETODOS_CLASESID_MULTICLASE(AUEscenaLuzEspejo, AUEscenaObjeto)
AUOBJMETODOS_CLASESNOMBRES_MULTICLASE(AUEscenaLuzEspejo, "AUEscenaLuzEspejo", AUEscenaObjeto)
AUOBJMETODOS_CLONAR_THIS(AUEscenaLuzEspejo)








