
#include "AUFrameworkMediaStdAfx.h"
#include "AUEscenaLuzRadial.h"

//-------------------------------------
//--- CONSTRUCTORES Y DESTRUCTORES ----
//-------------------------------------

AUEscenaLuzRadial::AUEscenaLuzRadial() : AUEscenaObjeto() {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaLuzRadial::AUEscenaLuzRadial")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaLuzRadial")
	_registradoEnMotorIlum			= false;
	_alcanceLuz						= 64.0f;
	_produceSombras					= true;
	_tipoLuz						= ENFuenteIlumTipo_SoloMascara;
	_factorMultiplicadorAlcance		= 1.0f;
	_angulosMinMaxActivos			= false;
	_radianesMin					= 0.0f;
	_radianesMax					= PI;
	_texturaLuz						= NBGestorTexturas::texturaDesdeArchivo("Iluminacion/ilumRadial75.png"); _texturaLuz->retener(NB_RETENEDOR_THIS);
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUEscenaLuzRadial::AUEscenaLuzRadial(float alcanceLuz, bool produceSombras) : AUEscenaObjeto() {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaLuzRadial::AUEscenaLuzRadial")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaLuzRadial")
	_registradoEnMotorIlum			= false;
	_alcanceLuz						= alcanceLuz;
	_produceSombras					= produceSombras;
	_tipoLuz						= ENFuenteIlumTipo_SoloMascara;
	_factorMultiplicadorAlcance		= 1.0f;
	_angulosMinMaxActivos			= false;
	_radianesMin					= 0.0f;
	_radianesMax					= PI;
	_texturaLuz						= NBGestorTexturas::texturaDesdeArchivo("Iluminacion/ilumRadial75.png"); _texturaLuz->retener(NB_RETENEDOR_THIS);
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUEscenaLuzRadial::AUEscenaLuzRadial(AUEscenaLuzRadial* otra) : AUEscenaObjeto(otra) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaLuzRadial::AUEscenaLuzRadial")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaLuzRadial")
	_registradoEnMotorIlum			= false;
	_alcanceLuz						= otra->_alcanceLuz;
	_produceSombras					= otra->_produceSombras;
	_tipoLuz						= otra->_tipoLuz;
	_factorMultiplicadorAlcance		= otra->_factorMultiplicadorAlcance;
	_angulosMinMaxActivos			= otra->_angulosMinMaxActivos;
	_radianesMin					= otra->_radianesMin;
	_radianesMax					= otra->_radianesMax;
	_texturaLuz						= otra->_texturaLuz; _texturaLuz->retener(NB_RETENEDOR_THIS);
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUEscenaLuzRadial::~AUEscenaLuzRadial(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaLuzRadial::~AUEscenaLuzRadial")
	NBASSERT(!_registradoEnMotorIlum) //No debe quedar registrado en el motor
	if(_texturaLuz != NULL) _texturaLuz->liberar(NB_RETENEDOR_THIS); _texturaLuz = NULL;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}
 
//-------------------------------
//--- FUNCIONES DE INSTANCIA ----
//-------------------------------

float AUEscenaLuzRadial::alcanceLuz(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaLuzRadial::alcanceLuz")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _alcanceLuz;
}

float AUEscenaLuzRadial::factorMultiplicadorAlcance(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaLuzRadial::factorMultiplicadorAlcance")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _factorMultiplicadorAlcance;
}

float AUEscenaLuzRadial::radianesMinimo(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaLuzRadial::radianesMinimo")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _radianesMin;
}

float AUEscenaLuzRadial::radianesMaximo(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaLuzRadial::radianesMaximo")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _radianesMax;
}

ENFuenteIlumTipo AUEscenaLuzRadial::tipoLuz(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaLuzRadial::tipoLuz")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return (ENFuenteIlumTipo)_tipoLuz;
}

bool AUEscenaLuzRadial::produceSombras(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaLuzRadial::produceSombras")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _produceSombras;
}

bool AUEscenaLuzRadial::angulosMinMaxActivos(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaLuzRadial::angulosMinMaxActivos")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _angulosMinMaxActivos;
}

AUTextura* AUEscenaLuzRadial::texturaLuz(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaLuzRadial::texturaLuz")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _texturaLuz;
}

void AUEscenaLuzRadial::establecerAlcanceLuz(float alcance){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaLuzRadial::establecerAlcanceLuz")
	_alcanceLuz = alcance; this->privInlineActualizarLuzEnGestor();
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaLuzRadial::establecerFactorMultiplicadorAlcance(float factorMultiplicadorAlcance){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaLuzRadial::establecerFactorMultiplicadorAlcance")
	if(factorMultiplicadorAlcance<1.0f) factorMultiplicadorAlcance = 1.0f;
	_factorMultiplicadorAlcance = factorMultiplicadorAlcance; this->privInlineActualizarLuzEnGestor();
	AU_GESTOR_PILA_LLAMADAS_POP_2
}


void AUEscenaLuzRadial::establecerRadianesMinimo(float radianes){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaLuzRadial::establecerRadianesMinimo")
	_radianesMin = radianes; this->privInlineActualizarLuzEnGestor();
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaLuzRadial::establecerRadianesMaximo(float radianes){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaLuzRadial::establecerRadianesMaximo")
	_radianesMax = radianes; this->privInlineActualizarLuzEnGestor();
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaLuzRadial::establecerTipoLuz(ENFuenteIlumTipo tipoLuz){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaLuzRadial::establecerTipoLuz")
	_tipoLuz = tipoLuz; this->privInlineActualizarLuzEnGestor();
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaLuzRadial::establecerProducirSombras(bool producirSombras){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaLuzRadial::establecerProducirSombras")
	_produceSombras = producirSombras; this->privInlineActualizarLuzEnGestor();
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaLuzRadial::establecerAngulosMinMaxActivos(bool angulosMinMaxActivos){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaLuzRadial::establecerAngulosMinMaxActivos")
	_angulosMinMaxActivos = angulosMinMaxActivos; this->privInlineActualizarLuzEnGestor();
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaLuzRadial::establecerTexturaLuz(AUTextura* textura){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaLuzRadial::establecerTexturaLuz")
	NBASSERT(textura != NULL)
	if(textura != NULL) textura->retener(NB_RETENEDOR_THIS);
	if(_texturaLuz != NULL) _texturaLuz->liberar(NB_RETENEDOR_THIS);
	_texturaLuz = textura;
	this->privInlineActualizarLuzEnGestor();
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

NBCajaAABB AUEscenaLuzRadial::cajaAABBLocalCalculada(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaLuzRadial::cajaAABBLocalCalculada")
	NBCajaAABB cajaLocal; NBCAJAAABB_INICIALIZAR(cajaLocal);
	NBCAJAAABB_ENVOLVER_PUNTO(cajaLocal, -8.0f, -8.0f);
	NBCAJAAABB_ENVOLVER_PUNTO(cajaLocal, 8.0f, 8.0f);
	/*//Solo se calcula la caja entre angulos limitados cuando se esta en modo debug
	if(!_angulosMinMaxActivos){
		NBCAJAAABB_ENVOLVER_PUNTO(cajaLocal, -_alcanceLuz, -_alcanceLuz);
		NBCAJAAABB_ENVOLVER_PUNTO(cajaLocal, _alcanceLuz, _alcanceLuz);
	} else {
		float radAct	= _radianesMin;
		float radAvance	= (_radianesMax - _radianesMin) / 4.0f;
		NBPunto posAct;
		NBCAJAAABB_ENVOLVER_PUNTO(cajaLocal, 0.0f, 0.0f)
		NBPUNTO_OBTENER_VECTOR_ROTADO(posAct, _alcanceLuz, radAct) NBCAJAAABB_ENVOLVER_PUNTO(cajaLocal, posAct.x, posAct.y) radAct += radAvance;
		NBPUNTO_OBTENER_VECTOR_ROTADO(posAct, _alcanceLuz, radAct) NBCAJAAABB_ENVOLVER_PUNTO(cajaLocal, posAct.x, posAct.y) radAct += radAvance;
		NBPUNTO_OBTENER_VECTOR_ROTADO(posAct, _alcanceLuz, radAct) NBCAJAAABB_ENVOLVER_PUNTO(cajaLocal, posAct.x, posAct.y) radAct += radAvance;
		NBPUNTO_OBTENER_VECTOR_ROTADO(posAct, _alcanceLuz, radAct) NBCAJAAABB_ENVOLVER_PUNTO(cajaLocal, posAct.x, posAct.y) radAct += radAvance;
		NBPUNTO_OBTENER_VECTOR_ROTADO(posAct, _alcanceLuz, radAct) NBCAJAAABB_ENVOLVER_PUNTO(cajaLocal, posAct.x, posAct.y)
	}*/
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return cajaLocal;
}

void AUEscenaLuzRadial::agregadoEnEscena(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaLuzRadial::agregadoEnEscena")
	NBASSERT(!_registradoEnMotorIlum)
	NBGestorEscena::agregarFuenteIluminacion(this->idEscena, this, this->_agrupadorSombras, _texturaLuz, _alcanceLuz*_factorMultiplicadorAlcance, (ENFuenteIlumTipo)_tipoLuz, _produceSombras, _angulosMinMaxActivos, _radianesMin, (_radianesMax-_radianesMin<=PI)?_radianesMax-_radianesMin:PI);
	_registradoEnMotorIlum = true;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaLuzRadial::quitandoDeEscena(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaLuzRadial::quitandoDeEscena")
	NBASSERT(_registradoEnMotorIlum)
	NBGestorEscena::quitarFuenteIluminacion(this->idEscena, this);
	_registradoEnMotorIlum = false;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

PTRfunCmdsGL AUEscenaLuzRadial::actualizarModeloGL(const NBPropRenderizado &propsRenderizado, const NBPropIluminacion &propsIluminacion, const NBPropHeredadasRender &propsHeredadas){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaLuzRadial::actualizarModeloGL")
	NBASSERT(idEscena!=-1) //Solo si esta en escena
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return NULL;
}

//


bool AUEscenaLuzRadial::agregarXmlInternoEn(AUEscenaLuzRadial* luzRadial, AUCadenaLargaMutable8* guardarEn, const char* espaciosBaseIzq){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaLuzRadial::agregarXmlInternoEn")
	//guardarEn->agregarConFormato("%s<vXML>1</vXML>\r\n", espaciosBaseIzq);
	guardarEn->agregarConFormato("%s<eOb>\r\n", espaciosBaseIzq);
	AUEscenaObjeto::agregarXmlInternoEn(luzRadial, guardarEn, espaciosBaseIzq);
	guardarEn->agregarConFormato("%s</eOb>\r\n", espaciosBaseIzq);
	guardarEn->agregarConFormato("%s<alcanceLuz>%f</alcanceLuz>\r\n", espaciosBaseIzq, luzRadial->_alcanceLuz);
	guardarEn->agregarConFormato("%s<factMultAlc>%f</factMultAlc>\r\n", espaciosBaseIzq, luzRadial->_factorMultiplicadorAlcance);
	guardarEn->agregarConFormato("%s<tipLuz>%d</tipLuz>\r\n", espaciosBaseIzq, luzRadial->_tipoLuz);
	guardarEn->agregarConFormato("%s<produceSombras>%d</produceSombras>\r\n", espaciosBaseIzq, luzRadial->_produceSombras?1:0);
	guardarEn->agregarConFormato("%s<angsAct>%d</angsAct>\r\n", espaciosBaseIzq, luzRadial->_angulosMinMaxActivos?1:0);
	guardarEn->agregarConFormato("%s<radMin>%f</radMin>\r\n", espaciosBaseIzq, luzRadial->_radianesMin);
	guardarEn->agregarConFormato("%s<radMax>%f</radMax>\r\n", espaciosBaseIzq, luzRadial->_radianesMax);
	if(luzRadial->_texturaLuz != NULL){
		const char* nombreTextura = NBGestorTexturas::nombreRecursoTextura(luzRadial->_texturaLuz);
		if(nombreTextura != NULL){
			guardarEn->agregarConFormato("%s<tex>%s</tex>\r\n", espaciosBaseIzq, nombreTextura);
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return true;

}

bool AUEscenaLuzRadial::interpretarXmlInterno(AUEscenaLuzRadial* cargarEn, const AUDatosXML* datosXml, const sNodoXML* nodoXml){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaLuzRadial::interpretarXmlInterno")
	bool exito = false;
	//int versionXML = datosXml->nodoHijo("vXML", 0, nodoXml);
	//if(versionXML==1){
		const sNodoXML* nodoEscenaObj;		XML_NODO_HIJO(datosXml, nodoEscenaObj, "eOb", "escObj", nodoXml, NULL)
		if(nodoEscenaObj != NULL){
			exito = AUEscenaObjeto::interpretarXmlInterno(cargarEn, datosXml, nodoEscenaObj);
			cargarEn->_alcanceLuz						= datosXml->nodoHijo("alcanceLuz", 0.0f, nodoXml);
			cargarEn->_factorMultiplicadorAlcance		= datosXml->nodoHijo("factMultAlc", 1.0f, nodoXml); if(cargarEn->_factorMultiplicadorAlcance<1.0f) cargarEn->_factorMultiplicadorAlcance = 1.0f;
			cargarEn->_tipoLuz							= datosXml->nodoHijo("tipLuz", ENFuenteIlumTipo_SoloMascara, nodoXml); if(cargarEn->_tipoLuz>=ENFuenteIlumTipo_Conteo) cargarEn->_tipoLuz = ENFuenteIlumTipo_SoloMascara;
			cargarEn->_produceSombras					= datosXml->nodoHijo("produceSombras", true, nodoXml);
			cargarEn->_angulosMinMaxActivos				= datosXml->nodoHijo("angsAct", false, nodoXml);
			cargarEn->_radianesMin						= datosXml->nodoHijo("radMin", GRADOS_A_RADIANES(0.0f), nodoXml);
			cargarEn->_radianesMax						= datosXml->nodoHijo("radMax", GRADOS_A_RADIANES(359.9f), nodoXml);
			//
			AUCadenaMutable8* nomTextura = new(ENMemoriaTipo_Temporal) AUCadenaMutable8();
			datosXml->nodoHijo("tex", nomTextura, "", nodoXml);
			if(nomTextura->tamano()!=0) cargarEn->establecerTexturaLuz(NBGestorTexturas::texturaDesdeArchivo(nomTextura->str()));
			nomTextura->liberar(NB_RETENEDOR_NULL);
			//
			cargarEn->privInlineActualizarLuzEnGestor();
		}
	//}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

bool AUEscenaLuzRadial::agregarBitsInternosEn(AUEscenaLuzRadial* luzRadial, AUArchivo* guardarEn){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaLuzRadial::agregarBitsInternosEn")
	bool exito = false;
	//
	float versionDatos = 1.0f;
	guardarEn->escribir(&versionDatos, sizeof(versionDatos), 1, guardarEn);
	if(AUEscenaObjeto::agregarBitsInternosEn(luzRadial, guardarEn)){
		guardarEn->escribir(&luzRadial->_alcanceLuz, sizeof(luzRadial->_alcanceLuz), 1, guardarEn);
		guardarEn->escribir(&luzRadial->_tipoLuz, sizeof(luzRadial->_tipoLuz), 1, guardarEn);
		guardarEn->escribir(&luzRadial->_produceSombras, sizeof(luzRadial->_produceSombras), 1, guardarEn);
		guardarEn->escribir(&luzRadial->_angulosMinMaxActivos, sizeof(luzRadial->_angulosMinMaxActivos), 1, guardarEn);
		guardarEn->escribir(&luzRadial->_radianesMin, sizeof(luzRadial->_radianesMin), 1, guardarEn);
		guardarEn->escribir(&luzRadial->_radianesMax, sizeof(luzRadial->_radianesMax), 1, guardarEn);
		//
		const char* nombreTextura = NULL;
		SI32 tamanoNombreTextura = 0;
		if(luzRadial->_texturaLuz != NULL) nombreTextura = NBGestorTexturas::nombreRecursoTextura(luzRadial->_texturaLuz);
		if(nombreTextura == NULL){
			guardarEn->escribir(&tamanoNombreTextura, sizeof(tamanoNombreTextura), 1, guardarEn);
		} else {
			tamanoNombreTextura = AUCadenaMutable8::tamano(nombreTextura);
			guardarEn->escribir(&tamanoNombreTextura, sizeof(tamanoNombreTextura), 1, guardarEn);
			guardarEn->escribir(nombreTextura, sizeof(char), tamanoNombreTextura, guardarEn);
		}
		exito = true;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

bool AUEscenaLuzRadial::interpretarBitsInternos(AUEscenaLuzRadial* cargarEn, AUArchivo* cargarDe){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaLuzRadial::interpretarBitsInternos")
	bool exito = false;
	float versionDatos;
	cargarDe->leer(&versionDatos, sizeof(versionDatos), 1, cargarDe);
	if(versionDatos==1.0f){
		if(AUEscenaObjeto::interpretarBitsInternos(cargarEn, cargarDe)){
			cargarDe->leer(&cargarEn->_alcanceLuz, sizeof(cargarEn->_alcanceLuz), 1, cargarDe);
			cargarDe->leer(&cargarEn->_tipoLuz, sizeof(cargarEn->_tipoLuz), 1, cargarDe); if(cargarEn->_tipoLuz>=ENFuenteIlumTipo_Conteo) cargarEn->_tipoLuz = ENFuenteIlumTipo_SoloMascara;
			cargarDe->leer(&cargarEn->_produceSombras, sizeof(cargarEn->_produceSombras), 1, cargarDe);
			cargarDe->leer(&cargarEn->_angulosMinMaxActivos, sizeof(cargarEn->_angulosMinMaxActivos), 1, cargarDe);
			cargarDe->leer(&cargarEn->_radianesMin, sizeof(cargarEn->_radianesMin), 1, cargarDe);
			cargarDe->leer(&cargarEn->_radianesMax, sizeof(cargarEn->_radianesMax), 1, cargarDe);
			//
			SI32 tamanoNombreTextura;
			cargarDe->leer(&tamanoNombreTextura, sizeof(tamanoNombreTextura), 1, cargarDe);
			if(tamanoNombreTextura>0){
				char* nombreTextura = (char*)NBGestorMemoria::reservarMemoria(tamanoNombreTextura+1, ENMemoriaTipo_Temporal); NB_DEFINE_NOMBRE_PUNTERO(nombreTextura, "AUEscenaSprite::nombreTextura") //+1 es el "\0"
				cargarDe->leer(nombreTextura, sizeof(char), tamanoNombreTextura, cargarDe);
				nombreTextura[tamanoNombreTextura] = '\0';
				cargarEn->establecerTexturaLuz(NBGestorTexturas::texturaDesdeArchivo(nombreTextura));
				NBGestorMemoria::liberarMemoria(nombreTextura);
			}
			//
			cargarEn->privInlineActualizarLuzEnGestor();
			exito = true;
		}
	} else {
		PRINTF_ERROR("AUEscenaLuzRadial, la version (%f) de datos binarios no es interpretable por este ejecutable\n", versionDatos);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}


//

AUOBJMETODOS_CLASESID_MULTICLASE(AUEscenaLuzRadial, AUEscenaObjeto)
AUOBJMETODOS_CLASESNOMBRES_MULTICLASE(AUEscenaLuzRadial, "AUEscenaLuzRadial", AUEscenaObjeto)
AUOBJMETODOS_CLONAR_THIS(AUEscenaLuzRadial)




