
#include "AUFrameworkMediaStdAfx.h"
#include "AUDialogoMutable.h"

//-------------------------------------
//--- CONSTRUCTORES Y DESTRUCTORES ----
//-------------------------------------

AUDialogoMutable::AUDialogoMutable() : AUDialogo(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUDialogoMutable::AUDialogoMutable")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUDialogoMutable")
	AU_GESTOR_PILA_LLAMADAS_POP
}

AUDialogoMutable::~AUDialogoMutable(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUDialogoMutable::~AUDialogoMutable")
	AU_GESTOR_PILA_LLAMADAS_POP
}

//

bool AUDialogoMutable::cargarXml(const char* rutaArchivo){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUDialogoMutable::cargarXml")
	bool exito = false;
	AUDatosXMLMutable* datosXml = new(ENMemoriaTipo_Temporal) AUDatosXMLMutable(); NB_DEFINE_NOMBRE_PUNTERO(datosXml, "AUDiccionarioTextoMutable::datosXml")
	if(datosXml->cargaDatosXMLDesdeArchivo(rutaArchivo)){
		const sNodoXML* nodoRaiz = datosXml->nodoHijo("dialogo");
		if(nodoRaiz != NULL){
			exito = this->interpretarXml(datosXml, nodoRaiz);
		}
	}
	datosXml->liberar(NB_RETENEDOR_THIS);
	AU_GESTOR_PILA_LLAMADAS_POP
	return exito;
}

bool AUDialogoMutable::interpretarXml(const AUDatosXML* datosXml, const sNodoXML* nodoXml) {
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUDialogoMutable::interpretarXml")
	bool exito = true;
	privVaciar();
	//
	AUCadenaMutable8* strTmp		= new(ENMemoriaTipo_Temporal) AUCadenaMutable8();
	AUCadenaMutable8* strTmp2		= new(ENMemoriaTipo_Temporal) AUCadenaMutable8();
	AUCadenaMutable8* strID			= new(ENMemoriaTipo_Temporal) AUCadenaMutable8();
	AUCadenaMutable8* strAlinea		= new(ENMemoriaTipo_Temporal) AUCadenaMutable8();
	AUCadenaMutable8* strNomFuente	= new(ENMemoriaTipo_Temporal) AUCadenaMutable8();
	AUFuenteTextura* fuentePorDefecto = privFuenteTexturaDesdeNodoXml(datosXml, NULL, strTmp); NBASSERT(fuentePorDefecto != NULL)
	const sNodoXML* nodoElemento	= datosXml->nodoHijo("e", nodoXml);
	while(nodoElemento != NULL){
		NBColor8 color;					NBCOLOR_ESTABLECER(color, 0, 0, 0, 0);
		bool colorExplicito				= false;
		AUFuenteTextura* fuenteParaTexto = fuentePorDefecto;
		//
		datosXml->nodoHijo("id", strID, "", nodoElemento);
		//
		const sNodoXML* nodoColor		= datosXml->nodoHijo("c", nodoElemento);
		if(nodoColor != NULL){
			if(nodoColor->nodosHijos == NULL){
				//Formato nuevo CSV (mas compacto)
				const char* cadenaCSV	= datosXml->cadenaValorDeNodo(nodoColor);
				SI32 tamCadenaCSV		= AUCadena8::tamano(cadenaCSV);
				SI32 posEnCadenaCSV		= 0;
				color.r					= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 255);
				color.g					= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 255);
				color.b					= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 255);
				color.a					= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 255);
			} else {
				//Formato viejo XML (muy inflado)
				color.r					= datosXml->nodoHijo("r", 255, nodoColor);
				color.g					= datosXml->nodoHijo("g", 255, nodoColor);
				color.b					= datosXml->nodoHijo("b", 255, nodoColor);
				color.a					= datosXml->nodoHijo("a", 255, nodoColor);
			}
			colorExplicito				= true;
		}
		//
		STDialogoElem datosElem;
		datosElem.tipoElemento			= ENDialogoElemTipo_Conteo;
		datosElem.iElem					= 0;
		datosElem.iStrID				= 0; if(strID->tamano()!=0){ datosElem.iStrID = _strCompartida.tamano(); _strCompartida.agregar(strID->str(), strID->tamano()+1, strCopiarPlecasCero); }
		datosElem.alineacionH			= ENDialogoAlineacionH_Izquierda;
		datosElem.alineacionV			= ENDialogoAlineacionV_Arriba;
		datosElem.esPrimeroDeID			= true;
		datosElem.esUltimoDeID			= true;
		datosElem.colorExplicito		= colorExplicito;
		datosElem.color8				= color;
		//
		datosXml->nodoHijo("aH", strAlinea, "", nodoElemento);
		if(strAlinea->esIgual("cen")){
			datosElem.alineacionH		= ENDialogoAlineacionH_Centro;
		} else if(strAlinea->esIgual("der")){
			datosElem.alineacionH		= ENDialogoAlineacionH_Derecha;
		} else if(strAlinea->esIgual("izqLib")){
			datosElem.alineacionH		= ENDialogoAlineacionH_IzquierdaLibre;
		} else if(strAlinea->esIgual("derLib")){
			datosElem.alineacionH		= ENDialogoAlineacionH_DerechaLibre;
		} else {
			datosElem.alineacionH		= ENDialogoAlineacionH_Izquierda;
		}
		datosXml->nodoHijo("aV", strAlinea, "", nodoElemento);
		if(strAlinea->esIgual("cen")){
			datosElem.alineacionV		= ENDialogoAlineacionV_Centro;
		} else if(strAlinea->esIgual("aba")){
			datosElem.alineacionV		= ENDialogoAlineacionV_Abajo;
		} else {
			datosElem.alineacionV		= ENDialogoAlineacionV_Arriba;
		}
		//
		strTmp->vaciar();
		if(nodoElemento->nodosHijos == NULL){
			strTmp->establecer(datosXml->cadenaValorDeNodo(nodoElemento));
		} else {
			const sNodoXML* nodoDatos; XML_NODO_HIJO(datosXml, nodoDatos, "dt", "datos", nodoElemento, NULL)
			if(nodoDatos != NULL){
				if(nodoDatos->nodosHijos == NULL){
					strTmp->establecer(datosXml->cadenaValorDeNodo(nodoDatos));
				} else {
					datosXml->nodoHijo("texto", strTmp, "", nodoDatos);
					fuenteParaTexto = privFuenteTexturaDesdeNodoXml(datosXml, datosXml->nodoHijo("fuente", nodoDatos), strTmp);
				}
			}
		}
		NBASSERT(fuenteParaTexto != NULL)
		//Separar los segmentos resaltados y no resaltados
		SI32 iPrimerSubElementoDeId		= -1;	//Para poder marcar el primer y ultimo subelemento del strID actual
		SI32 iUltimoSubElementoDeId		= -1;	//Para poder marcar el primer y ultimo subelemento del strID actual
		//
		bool seccionActualReslatada = false;
		SI32 posPrimerCharDeTexto = 0;
		//Ignorar los primeros carracteres en blanco (para permitir un formato mas leible para humanos en el XML)
		while(posPrimerCharDeTexto<strTmp->tamano() && (strTmp->str()[posPrimerCharDeTexto]=='\n' || strTmp->str()[posPrimerCharDeTexto]=='\r' || strTmp->str()[posPrimerCharDeTexto]=='\t' || strTmp->str()[posPrimerCharDeTexto]==' ')){
			posPrimerCharDeTexto++;
		}
		//Ignorar los ultimos caracteres en blanco (para permitir un formato mas leible para humanos en el XML)
		if(strTmp->tamano()!=0){
			char ultimoChar = strTmp->str()[strTmp->tamano()-1];
			while(strTmp->tamano()!=0 && (ultimoChar=='\n' || ultimoChar=='\r' || ultimoChar=='\t' || ultimoChar==' ')){
				strTmp->quitarDerecha(1);
				if(strTmp->tamano()!=0) ultimoChar = strTmp->str()[strTmp->tamano()-1];
			}
		}
		//
		while(posPrimerCharDeTexto<strTmp->tamano()){
			/*
			 En una cadena de texto se pueden introducir propiedades en lugar de crear un nodo.
			 - Para texto reslatado, encerrar entre corchetes, ejemplo: Hola, soy [Marcos].
			 - Para introducir un separador al final del elemento, un punto encerrado entre llaves: Hola, soy Marcos.{.}
			 - Para introducir una animacion, propiedades separadas por punto_coma entre llaves: Hola, soy Marcos {anim:happyface.animacion}
			 - Para introducir una sprite/imagen, propiedades separadas por punto_coma entre llaves: Hola, soy Marcos {img:happyface.png}
			 Si se requieren establecer propiedades detalladas al objeto, se deben introducir en nodos <e> detallados (mas sucios pero flexibles)
			 */
			SI32 iAperturaResaltar		= strTmp->indiceDe('[', posPrimerCharDeTexto); if(iAperturaResaltar==-1) iAperturaResaltar = strTmp->tamano();
			SI32 iCierreResaltar		= strTmp->indiceDe(']', posPrimerCharDeTexto); if(iCierreResaltar==-1) iCierreResaltar = strTmp->tamano();
			SI32 iAperturaResaltar2		= strTmp->indiceDe('{', posPrimerCharDeTexto); if(iAperturaResaltar2==-1) iAperturaResaltar2 = strTmp->tamano();
			SI32 iCierreResaltar2		= strTmp->indiceDe('}', posPrimerCharDeTexto); if(iCierreResaltar2==-1) iCierreResaltar2 = strTmp->tamano();
			SI32 iSeparadorMasCercano	= iAperturaResaltar;
			if(iSeparadorMasCercano>iCierreResaltar)	iSeparadorMasCercano = iCierreResaltar;
			if(iSeparadorMasCercano>iAperturaResaltar2) iSeparadorMasCercano = iAperturaResaltar2;
			if(iSeparadorMasCercano>iCierreResaltar2)	iSeparadorMasCercano = iCierreResaltar2;
			NBASSERT(iSeparadorMasCercano>=posPrimerCharDeTexto)
			SI32 cantidadCaracteres		= iSeparadorMasCercano - posPrimerCharDeTexto; NBASSERT(cantidadCaracteres>=0)
			if(cantidadCaracteres>0){
				strTmp2->vaciar(); strTmp2->agregar(&strTmp->str()[posPrimerCharDeTexto], cantidadCaracteres);
				bool esMarcaEspecial	= (strTmp2->tamano()==2 && strTmp2->str()[0]=='\\');
				bool esEtiquetaImagen	= false; if(strTmp2->tamano()>4 && strTmp2->str()[0]=='i') esEtiquetaImagen = (strTmp2->indiceDe("img=")==0);
				bool esEtiquetaAnim		= false; if(strTmp2->tamano()>5 && strTmp2->str()[0]=='a') esEtiquetaAnim = (strTmp2->indiceDe("anim=")==0);
				if(esMarcaEspecial){
					NBASSERT(strTmp2->tamano()>=2)
					char charEspecial = strTmp2->str()[1];
					if(charEspecial=='.'){
						STDialogoElem datosSubElem	= datosElem;
						datosSubElem.tipoElemento	= ENDialogoElemTipo_SeparadorBloque;
						datosSubElem.iStrID			= 0;
						datosSubElem.iElem			= 0;
						datosSubElem.esPrimeroDeID	= true; //Es un subelemto con otro ID
						datosSubElem.esUltimoDeID	= true; //Es un subelemto con otro ID
						_arregloElementos.agregarElemento(datosSubElem);
					} else if(charEspecial=='n'){
						STDialogoElem datosSubElem	= datosElem;
						datosSubElem.tipoElemento	= ENDialogoElemTipo_SeparadorLinea;
						datosSubElem.iStrID			= 0;
						datosSubElem.iElem			= 0;
						datosSubElem.esPrimeroDeID	= true; //Es un subelemto con otro ID
						datosSubElem.esUltimoDeID	= true; //Es un subelemto con otro ID
						_arregloElementos.agregarElemento(datosSubElem);
					}
				} else if(esEtiquetaImagen){
					STDialogoElem datosSubElem = datosElem; datosSubElem.tipoElemento	= ENDialogoElemTipo_Imagen;
					STDialogoImagen datosDetalle;			datosDetalle.iURLRecurso	= 0;
					privInterpretarParametrosEtiqueta(strTmp2->str(), &datosSubElem, &datosDetalle, NULL);
					if(datosDetalle.iURLRecurso!=0){
						AUTextura* textura			= NBGestorTexturas::texturaDesdeArchivo(&_strCompartida.str()[datosDetalle.iURLRecurso]);
						if(textura != NULL){
							datosDetalle.objPlantilla	= new(this) AUEscenaSprite(textura);
							datosSubElem.iElem			= _arregloImagenes.conteo;
							if(datosSubElem.iStrID==datosElem.iStrID){
								if(iPrimerSubElementoDeId==-1) iPrimerSubElementoDeId = _arregloElementos.conteo;
								iUltimoSubElementoDeId = _arregloElementos.conteo;
								datosSubElem.esPrimeroDeID	= false; //Solo el primero conserva esta propiedad
								datosSubElem.esUltimoDeID	= false; //Solo el ultimo conserva esta propiedad
							} else {
								datosSubElem.esPrimeroDeID	= true; //Es un subelemto con otro ID
								datosSubElem.esUltimoDeID	= true; //Es un subelemto con otro ID
							}
							_arregloElementos.agregarElemento(datosSubElem);
							_arregloImagenes.agregarElemento(datosDetalle);
						}
					}
				} else if(esEtiquetaAnim){
					STDialogoElem datosSubElem = datosElem; datosSubElem.tipoElemento	= ENDialogoElemTipo_Animacion;
					STDialogoAnimacion datosDetalle;
					datosDetalle.iURLRecurso		= 0;
					datosDetalle.repetir			= true;
					datosDetalle.suavizar			= false;
					datosDetalle.factorVelocidad	= 1.0f;
					datosDetalle.angulo				= 0.0f;
					privInterpretarParametrosEtiqueta(strTmp2->str(), &datosSubElem, NULL, &datosDetalle);
					if(datosDetalle.iURLRecurso!=0){
						datosDetalle.objPlantilla	= NBGestorAnimaciones::animacionDesdeArchivo(this->tipoMemoriaResidencia(), &_strCompartida.str()[datosDetalle.iURLRecurso]);
						if(datosDetalle.objPlantilla != NULL){
							datosDetalle.objPlantilla->retener(NB_RETENEDOR_THIS);
							datosDetalle.objPlantilla->establecerRepetirAnimacion(datosDetalle.repetir);
							datosDetalle.objPlantilla->establecerSuavizarAnimacion(datosDetalle.suavizar);
							datosDetalle.objPlantilla->establecerFactorVelocidadReproduccion(datosDetalle.factorVelocidad);
							datosSubElem.iElem			= _arregloAnimaciones.conteo;
							if(datosSubElem.iStrID==datosElem.iStrID){
								if(iPrimerSubElementoDeId==-1) iPrimerSubElementoDeId = _arregloElementos.conteo;
								iUltimoSubElementoDeId = _arregloElementos.conteo;
								datosSubElem.esPrimeroDeID	= false; //Solo el primero conserva esta propiedad
								datosSubElem.esUltimoDeID	= false; //Solo el ultimo conserva esta propiedad
							} else {
								datosSubElem.esPrimeroDeID	= true; //Es un subelemto con otro ID
								datosSubElem.esUltimoDeID	= true; //Es un subelemto con otro ID
							}
							_arregloElementos.agregarElemento(datosSubElem);
							_arregloAnimaciones.agregarElemento(datosDetalle);
						}
					}
				} else {
					STDialogoElem datosSubElem = datosElem; datosSubElem.tipoElemento	= ENDialogoElemTipo_Texto;
					STDialogoTexto datosDetalle;
					datosDetalle.texFuente	= fuenteParaTexto; fuenteParaTexto->retener(NB_RETENEDOR_THIS);
					datosDetalle.iTexto		= _strCompartida.tamano(); _strCompartida.agregar(strTmp2->str(), strTmp2->tamano()+1, strCopiarPlecasCero);
					datosDetalle.resaltar	= seccionActualReslatada;
					datosSubElem.iElem		= _arregloTextos.conteo;
					if(datosSubElem.iStrID==datosElem.iStrID){
						if(iPrimerSubElementoDeId==-1) iPrimerSubElementoDeId = _arregloElementos.conteo;
						iUltimoSubElementoDeId = _arregloElementos.conteo;
						datosSubElem.esPrimeroDeID	= false; //Solo el primero conserva esta propiedad
						datosSubElem.esUltimoDeID	= false; //Solo el ultimo conserva esta propiedad
					} else {
						datosSubElem.esPrimeroDeID	= true; //Es un subelemto con otro ID
						datosSubElem.esUltimoDeID	= true; //Es un subelemto con otro ID
					}
					_arregloElementos.agregarElemento(datosSubElem);
					_arregloTextos.agregarElemento(datosDetalle);
				}
			}
			seccionActualReslatada = !seccionActualReslatada;
			posPrimerCharDeTexto = iSeparadorMasCercano + 1;
		}
		NBASSERT((iPrimerSubElementoDeId==-1 && iUltimoSubElementoDeId==-1) || (iPrimerSubElementoDeId!=-1 && iUltimoSubElementoDeId!=-1))
		if(iPrimerSubElementoDeId!=-1) _arregloElementos.elemento[iPrimerSubElementoDeId].esPrimeroDeID	= true; //Solo el primero conserva esta propiedad
		if(iUltimoSubElementoDeId!=-1) _arregloElementos.elemento[iUltimoSubElementoDeId].esUltimoDeID	= true;	//Solo el ultimo conserva esta propiedad
		//Siguiente elemento
		nodoElemento  = datosXml->nodoHijo("e", nodoXml, nodoElemento);
	}
	strNomFuente->liberar(NB_RETENEDOR_THIS);
	strAlinea->liberar(NB_RETENEDOR_THIS);
	strID->liberar(NB_RETENEDOR_THIS);
	strTmp2->liberar(NB_RETENEDOR_THIS);
	strTmp->liberar(NB_RETENEDOR_THIS);
	exito = true;
	AU_GESTOR_PILA_LLAMADAS_POP
	return exito;
}

AUFuenteTextura* AUDialogoMutable::privFuenteTexturaDesdeNodoXml(const AUDatosXML* datosXml, const sNodoXML* nodoFuente, AUCadenaMutable8* strTmpCompartida){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUDialogoMutable::privFuenteTexturaDesdeNodoXml")
	AUFuenteTextura* fuente = NULL;
	strTmpCompartida->vaciar();
	if(nodoFuente != NULL){
		datosXml->nodoHijo("nombre", strTmpCompartida, "", nodoFuente);
		bool negrilla	= datosXml->nodoHijo("neg", false, nodoFuente);
		bool cursiva	= datosXml->nodoHijo("cur", false, nodoFuente);
		SI32 tamano		= datosXml->nodoHijo("tam", 32, nodoFuente);
		if(strTmpCompartida->tamano()>0){
			//Buscar en el arreglo de fuentes
			SI32 iFuente;
			for(iFuente=0; iFuente<_bibliotecaFuentes.conteo && fuente == NULL; iFuente++){
				AUFuenteTextura* fuenteTextura = (AUFuenteTextura*) _bibliotecaFuentes.elemento[iFuente];
				if(strTmpCompartida->esIgual(fuenteTextura->familia()) && negrilla == fuenteTextura->esNegrilla() && cursiva == fuenteTextura->esCursiva() && tamano == fuenteTextura->tamanoEscena()){
					fuente = fuenteTextura;
					//PRINTF_INFO("DIALOGO Reutilizando fuente textura en indice %d ('%s')\n", iFuente, strTmpCompartida->str());
				}
			}
			//Buscar a traves del gestor
			if(fuente == NULL){
				fuente = NBGestorTexturas::fuenteTextura(strTmpCompartida->str(), tamano, negrilla, cursiva);
				if(fuente != NULL) _bibliotecaFuentes.agregarElemento(fuente);
				//PRINTF_INFO("DIALOGO Fuente agregada desde XML ('%s')\n", strTmpCompartida->str());
			}
		}
	}
	if(fuente == NULL){
		fuente = NBGestorTexturas::fuenteTextura("Allatuq", 32, false, false);
		if(fuente != NULL) _bibliotecaFuentes.agregarElemento(fuente);
		//PRINTF_INFO("DIALOGO Fuente por defecto agregada ('HELLO I LIKE YOU')\n");
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return fuente;
}

void AUDialogoMutable::privInterpretarParametrosEtiqueta(const char* listaParametros, STDialogoElem* datosElem, STDialogoImagen* datosImagen, STDialogoAnimacion* datosAnimacion){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUDialogoMutable::privInterpretarParametrosEtiqueta")
	NBASSERT(datosElem != NULL)
	AUCadenaMutable8* strTmp = new(ENMemoriaTipo_Temporal) AUCadenaMutable8();
	AUCadenaMutable8* strTmp2 = new(ENMemoriaTipo_Temporal) AUCadenaMutable8();
	//
	const char* cadenaCSV	= listaParametros;
	SI32 tamCadenaCSV		= AUCadena8::tamano(cadenaCSV);
	SI32 posEnCadenaCSV		= 0;
	while(posEnCadenaCSV<tamCadenaCSV){
		strTmp->vaciar();
		AUDatosCSV::valorHastaSeparador(cadenaCSV, ';', tamCadenaCSV, &posEnCadenaCSV, strTmp, "");
		if(strTmp->tamano()!=0){
			SI32 posSeparador = strTmp->indiceDe('=');
			if(posSeparador>0){
				strTmp2->vaciar(); strTmp2->agregar(strTmp->str(), posSeparador);
				const char* strNombre = strTmp2->str();
				const char* strValor = &(strTmp->str()[posSeparador+1]);
				//Ignorar los espacios a la izquierda del nombre
				while(*strNombre!='\0' && (*strNombre==' ' || *strNombre=='\n' || *strNombre=='\r' || *strNombre=='\t')){
					strNombre++;
				}
				//Procesar
				if(AUCadena8::esIgual(strNombre, "id")){
					datosElem->iStrID = _strCompartida.tamano(); _strCompartida.agregar(strValor, AUCadena8::tamano(strValor)+1, strCopiarPlecasCero);
				} else if(AUCadena8::esIgual(strNombre, "color")){
					SI32 tamCadenaColor		= AUCadena8::tamano(strValor);
					SI32 posEnCadenaColor	= 0;
					datosElem->color8.r		= AUDatosCSV::valorHastaSeparador(strValor, '|', tamCadenaColor, &posEnCadenaColor, 255);
					datosElem->color8.g		= AUDatosCSV::valorHastaSeparador(strValor, '|', tamCadenaColor, &posEnCadenaColor, 255);
					datosElem->color8.b		= AUDatosCSV::valorHastaSeparador(strValor, '|', tamCadenaColor, &posEnCadenaColor, 255);
					datosElem->color8.a		= AUDatosCSV::valorHastaSeparador(strValor, '|', tamCadenaColor, &posEnCadenaColor, 255);
					datosElem->colorExplicito = true;
				} else if(AUCadena8::esIgual(strNombre, "img")){
					if(datosImagen != NULL){
						datosImagen->iURLRecurso	= _strCompartida.tamano(); _strCompartida.agregar(strValor, AUCadena8::tamano(strValor)+1, strCopiarPlecasCero);
					}
				} else if(AUCadena8::esIgual(strNombre, "anim")){
					if(datosAnimacion != NULL){
						datosAnimacion->iURLRecurso	= _strCompartida.tamano(); _strCompartida.agregar(strValor, AUCadena8::tamano(strValor)+1, strCopiarPlecasCero);
					}
				} else if(AUCadena8::esIgual(strNombre, "aH")){
					if(AUCadena8::esIgual(strValor, "izq")){
						datosElem->alineacionH		= ENDialogoAlineacionH_Izquierda;
					} else if(AUCadena8::esIgual(strValor, "cen")){
						datosElem->alineacionH		= ENDialogoAlineacionH_Centro;
					} else if(AUCadena8::esIgual(strValor, "der")){
						datosElem->alineacionH		= ENDialogoAlineacionH_Derecha;
					} else if(AUCadena8::esIgual(strValor, "izqLib")){
						datosElem->alineacionH		= ENDialogoAlineacionH_IzquierdaLibre;
					} else if(AUCadena8::esIgual(strValor, "derLib")){
						datosElem->alineacionH		= ENDialogoAlineacionH_DerechaLibre;
					}
				} else if(AUCadena8::esIgual(strNombre, "aV")){
					if(AUCadena8::esIgual(strValor, "arr")){
						datosElem->alineacionV		= ENDialogoAlineacionV_Arriba;
					} else if(AUCadena8::esIgual(strValor, "cen")){
						datosElem->alineacionV		= ENDialogoAlineacionV_Centro;
					} else if(AUCadena8::esIgual(strValor, "aba")){
						datosElem->alineacionV		= ENDialogoAlineacionV_Abajo;
					}
				} else if(AUCadena8::esIgual(strNombre, "rep")){
					if(datosAnimacion != NULL) datosAnimacion->repetir = (strValor[0]!=0);
				} else if(AUCadena8::esIgual(strNombre, "suav")){
					if(datosAnimacion != NULL) datosAnimacion->suavizar = (strValor[0]!=0);
				} else if(AUCadena8::esIgual(strNombre, "vel")){
					if(datosAnimacion != NULL) datosAnimacion->factorVelocidad = AUNumericoP<float, char>::aDecimalSiEsValido(strValor, 1.0f);
				} else if(AUCadena8::esIgual(strNombre, "rot")){
					if(datosAnimacion != NULL) datosAnimacion->angulo = AUNumericoP<float, char>::aDecimalSiEsValido(strValor, 0.0f);
				}
			}
		}
	}
	//
	strTmp2->liberar(NB_RETENEDOR_THIS);
	strTmp->liberar(NB_RETENEDOR_THIS);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

//
AUOBJMETODOS_CLASESID_MULTICLASE(AUDialogoMutable, AUDialogo)
AUOBJMETODOS_CLASESNOMBRES_MULTICLASE(AUDialogoMutable, "AUDialogoMutable", AUDialogo)
AUOBJMETODOS_CLONAR_NULL(AUDialogoMutable)




