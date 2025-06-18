//
//  AUEscenaCol.cpp
//  lib-auframework-media
//
//  Created by Marcos Ortega on 28/12/18.
//

#include "AUFrameworkMediaStdAfx.h"
#include "AUEscenaCol.h"
//
#include "NBGestorAnimadores.h"

AUEscenaCol::AUEscenaCol() : AUEscenaContenedor()
, _colLeft(0.0f)
, _colWidth(0.0f)
, _indiceScrollAlto(0.0f)
, _indiceScroll(NULL)
, _barraScroll(NULL)
//
, _marginTop(0.0f)
, _marginBottom(0.0f)
//
, _scrollAnimatingAlpha(0.0f)
, _scrollAnimating(false)
, _scrollShow(false)
, _touchesCountAntes(0)
, _touchesCount(0)
, _scrollSpeed(0.0f)
//
, _pilaContenido(this)
, _pilaCantOcultando(0)
, _pilaCantQuitando(0)
{
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaCol")
	_indiceScroll	= new(this) AUEscenaContenedor();
	_indiceScroll->establecerMultiplicadorColor8(181, 181, 181, 0);
	_barraScroll	= new(this) AUEscenaSpriteElastico(NBGestorTexturas::texturaDesdeArchivoPNG("refranero/RefraneroUIKit.png#ui-scrollbar-mini"));
	_barraScroll->establecerRotacion(270.0f);
	_barraScroll->establecerMultiplicadorColor8(0, 0, 0, 0);
	//
	NBGestorAnimadores::agregarAnimador(this, this);
}

AUEscenaCol::~AUEscenaCol(){
	NBGestorAnimadores::quitarAnimador(this);
	//
	if(_indiceScroll != NULL) _indiceScroll->liberar(NB_RETENEDOR_THIS); _indiceScroll = NULL;
	if(_barraScroll != NULL) _barraScroll->liberar(NB_RETENEDOR_THIS); _barraScroll = NULL;
	//Liberar pila de contenido
	this->privEmpty();
}

//

void AUEscenaCol::privEmpty(){
	_pilaCantOcultando	= 0;
	_pilaCantQuitando	= 0;
	{
		UI16 i; const UI16 conteo = _pilaContenido.conteo;
		for(i = 0; i < conteo; i++){
			STEscenaColData* datos = _pilaContenido.elemPtr(i);
			if(datos->objeto != NULL){
				AUEscenaContenedor* cont = (AUEscenaContenedor*)datos->objeto->contenedor();
				if(cont != NULL) cont->quitarObjetoEscena(datos->objeto);
				datos->objeto->liberar(NB_RETENEDOR_THIS);
				datos->objeto = NULL;
			}
			if(datos->strTitulo != NULL){ datos->strTitulo->liberar(NB_RETENEDOR_THIS); datos->strTitulo = NULL; }
		}
		_pilaContenido.vaciar();
	}
}

//

float AUEscenaCol::colLeft() const{
	return _colLeft;
}

void AUEscenaCol::setColLeft(const float colLeft){
	_colLeft = colLeft;
}

float AUEscenaCol::colWidth() const {
	return _colWidth;
}

void AUEscenaCol::setColWidth(const float colWidth){
	_colWidth = colWidth;
}

//

/*float AUEscenaCol::indiceScrollAlto() const {
	return _indiceScrollAlto;
}

AUEscenaContenedor* AUEscenaCol::indiceScroll() const {
	return _indiceScroll;
}

AUEscenaSpriteElastico* AUEscenaCol::barraScroll() const {
	return _barraScroll;
}*/

//

float AUEscenaCol::marginTop() const {
	return _marginTop;
}

void AUEscenaCol::setMarginTop(const float height){
	_marginTop = height;
}

float AUEscenaCol::marginBottom() const {
	return _marginBottom;
}

void AUEscenaCol::setMarginBottom(const float height){
	_marginBottom = height;
}

//

SI32 AUEscenaCol::touchesCount() const {
	return _touchesCount;
}

void AUEscenaCol::touchesPush(){
	_touchesCount++; NBASSERT(_touchesCount >= 0)
}

void AUEscenaCol::touchesPop(){
	_touchesCount--; NBASSERT(_touchesCount >= 0)
	if(_touchesCount < 0) _touchesCount = 0;
}

float AUEscenaCol::scrollSpeed() const {
	return _scrollSpeed;
}

void AUEscenaCol::setScrollSpeed(const float vel) {
	_scrollSpeed = vel;
}

//

//ToDo: Remove.
/*const AUArregloNativoP<STEscenaColData>* AUEscenaCol::pilaContenido() const {
	return &_pilaContenido;
}*/

SI32 AUEscenaCol::stackSize() const {
	return (SI32)_pilaContenido.conteo - _pilaCantQuitando;
}

SI32 AUEscenaCol::stackHiddingCount() const {
	return _pilaCantOcultando;
}

SI32 AUEscenaCol::stackRemovingCount() const {
	return _pilaCantQuitando;
}

void AUEscenaCol::empty(){
	this->privEmpty();
}

//

STEscenaColData* AUEscenaCol::curContent() const {
	STEscenaColData* r = NULL;
	if(_pilaCantQuitando < _pilaContenido.conteo){
		r = _pilaContenido.elemPtr(_pilaContenido.conteo - _pilaCantQuitando - 1);
	}
	return r;
}

STEscenaColData* AUEscenaCol::topContent(const SI32 indiceDesdeFinal) const {
	STEscenaColData* r = NULL;
	if((indiceDesdeFinal + _pilaCantQuitando) < _pilaContenido.conteo){
		r = _pilaContenido.elemPtr((SI32)_pilaContenido.conteo - _pilaCantQuitando - indiceDesdeFinal - 1);
	}
	return r;
}

void AUEscenaCol::push(const NBCajaAABB cajaEscena, AUEscenaObjeto* objAgregar, AUEscenaContentI* interfaz, const char* strTitulo, const bool animarEntradaNuevo, const bool animarSalidaUltimo, const bool intercambiarConUltimo){
	STEscenaColData* datosAct = NULL;
	if((_pilaContenido.conteo - _pilaCantQuitando) != 0){
		datosAct	= _pilaContenido.elemPtr(_pilaContenido.conteo - _pilaCantQuitando - 1);
	}
	//Limpiar indice de scroll
	_indiceScroll->vaciar();
	_indiceScrollAlto = 0.0f;
	//Quitar el objeto actual de escena
	if(datosAct != NULL){
		bool salidaEstaAnimando = false;
		if(animarSalidaUltimo){
			const NBPunto posObj = datosAct->objeto->traslacion();
			datosAct->interfaz->animarSalida(cajaEscena.yMax - posObj.y, cajaEscena.yMin - posObj.y);
			if(datosAct->interfaz->animando()){
				if(intercambiarConUltimo){
					//Encolar a quitando
					_pilaCantQuitando++;
				} else {
					//Encolar a ocultando
					_pilaCantOcultando++;
				}
				salidaEstaAnimando = true;
			}
		}
		if(!salidaEstaAnimando){
			AUEscenaContenedor* cont = (AUEscenaContenedor*)datosAct->objeto->contenedor();
			if(cont != NULL){
				cont->quitarObjetoEscena(datosAct->objeto);
			}
			if(intercambiarConUltimo){
				if(datosAct->objeto != NULL) datosAct->objeto->liberar(NB_RETENEDOR_THIS); datosAct->objeto = NULL;
				if(datosAct->strTitulo != NULL) datosAct->strTitulo->liberar(NB_RETENEDOR_THIS); datosAct->strTitulo = NULL;
				_pilaContenido.quitarElementoEnIndice(_pilaContenido.conteo - _pilaCantQuitando - 1);
			}
		}
	}
	//Agregar objeto a pila
	STEscenaColData datosNvo;
	datosNvo.objeto		= objAgregar; objAgregar->retener(NB_RETENEDOR_THIS);
	datosNvo.interfaz	= interfaz;
	datosNvo.strTitulo	= new(this) AUCadena8(strTitulo[0] != '\0' ? strTitulo : "Búsqueda");
	_pilaContenido.agregarElementoEnIndice(datosNvo, _pilaContenido.conteo - _pilaCantQuitando);
	this->agregarObjetoEscenaEnIndice(objAgregar, 0);
	//Organizar objeto
	interfaz->organizarContenido(_colWidth, 0.0f, false, NULL, 0.0f);
	objAgregar->establecerTraslacion(_colLeft, 0.0f);
	this->privScrollContent(cajaEscena, -100000.0f, _pilaContenido.elemPtr(_pilaContenido.conteo - _pilaCantQuitando - 1));
	//this->privOrganizarAccesoriosMostrar();
	if(animarEntradaNuevo){
		const NBPunto posObj = objAgregar->traslacion();
		interfaz->animarEntrada(cajaEscena.yMax - posObj.y, cajaEscena.yMin - posObj.y);
	}
	PRINTF_INFO("PUSH - tamPila(%d) quitando(%d).\n", _pilaContenido.conteo, _pilaCantQuitando);
	_scrollSceneBox = cajaEscena;
}

bool AUEscenaCol::pop(const NBCajaAABB cajaEscena, bool animarEntradaPenultimo, bool animarSalidaUltimo){
	bool r = false;
	//Quitar penultimo
	bool quitandoAnimado = false;
	if((_pilaContenido.conteo - _pilaCantQuitando) > 1){
		//Animar entrada de objeto anterior
		STEscenaColData* datosAnt = _pilaContenido.elemPtr(_pilaContenido.conteo - _pilaCantQuitando - 2);
		datosAnt->interfaz->organizarContenido(_colWidth, 0.0f, false, NULL, 0.0f);
		this->privScrollContent(cajaEscena, 0.0f, datosAnt);
		this->agregarObjetoEscena(datosAnt->objeto);
		//
		const NBPunto posObj = datosAnt->objeto->traslacion();
		datosAnt->interfaz->animarEntrada(cajaEscena.yMax - posObj.y, cajaEscena.yMin - posObj.y);
		if(animarSalidaUltimo){
			if(datosAnt->interfaz->animando()){
				//Encolar a quitando
				_pilaCantQuitando++;
				quitandoAnimado = true;
			}
		}
	}
	//Quitar el ultimo elemento de la pila
	if((_pilaContenido.conteo - _pilaCantQuitando) > 0){
		if(!quitandoAnimado){
			//Quitar inmediatamente
			STEscenaColData* datosAct = _pilaContenido.elemPtr(_pilaContenido.conteo - _pilaCantQuitando - 1);
			AUEscenaContenedor* cont = (AUEscenaContenedor*)datosAct->objeto->contenedor();
			if(cont != NULL){
				cont->quitarObjetoEscena(datosAct->objeto);
			}
			if(datosAct->objeto != NULL) datosAct->objeto->liberar(NB_RETENEDOR_THIS); datosAct->objeto = NULL;
			if(datosAct->strTitulo != NULL) datosAct->strTitulo->liberar(NB_RETENEDOR_THIS); datosAct->strTitulo = NULL;
			_pilaContenido.quitarElementoEnIndice(_pilaContenido.conteo - _pilaCantQuitando - 1);
		}
		//
		r = true;
	}
	_scrollSceneBox = cajaEscena;
	return r;
}

void AUEscenaCol::scrollContenido(const NBCajaAABB cajaEscena, const float yTraslado){
	STEscenaColData* datEnScroll	= this->curContent();
	if(datEnScroll != NULL){
		this->privScrollContent(cajaEscena, yTraslado, datEnScroll);
	}
}

void AUEscenaCol::privScrollContent(const NBCajaAABB cajaEscena, const float yTraslado, STEscenaColData* datEnScroll){
	AUEscenaObjeto* objEnScroll	= datEnScroll->objeto;
	float* indiceCentroAlto = &_indiceScrollAlto;
	AUEscenaContenedor* indiceEnScroll = _indiceScroll;
	AUEscenaSpriteElastico* barraEnScroll = _barraScroll;
	//
	if(objEnScroll != NULL){
		//Determinar espacio para scrollear
		NBCajaAABB cajaCont		= objEnScroll->cajaAABBLocal();
		const float yMin		= cajaEscena.yMax - _marginTop  - 0.0f; //Se asume que todas las capas de contenido tienen su yMax = 0.0f;
		float yMax				= cajaEscena.yMin - cajaCont.yMin + _marginBottom;
		//Evaluar scroll en Y
		const NBPunto posOrig	= objEnScroll->traslacion();
		NBPunto posDst			= posOrig;
		{
			posDst.y			+= yTraslado;
			if(posDst.y > yMax){
				if(datEnScroll != NULL){
					datEnScroll->interfaz->cargarMasContenido();
					//Actualizar por si ha crecido.
					cajaCont	= objEnScroll->cajaAABBLocal();
					yMax		= cajaEscena.yMin - cajaCont.yMin + _marginBottom;
				}
				if(posDst.y > yMax){
					//No ha crecido, detener scroll.
					posDst.y	= yMax;
					_scrollSpeed = 0.0f;
				}
			}
			if(posDst.y < yMin){
				posDst.y = yMin;
				_scrollSpeed = 0.0f;
			}
			objEnScroll->establecerTraslacion(posDst);
			datEnScroll->interfaz->organizarSegunAltoVisible(cajaEscena.yMax - posDst.y, cajaEscena.yMin - posDst.y, cajaEscena.yMax - posDst.y - _marginTop, cajaEscena.yMin - posDst.y + _marginBottom);
			//Barra de scroll
			if(barraEnScroll != NULL){
				const NBCajaAABB cajaCont	= objEnScroll->cajaAABBLocal(); //Volver a obtener el tamano (el objeto pudo haber cargado mas contenido)
				const float altoContenido	= -cajaCont.yMin;
				const float altoVisible		= cajaEscena.yMax - cajaEscena.yMin - _marginTop - _marginBottom;
				if(altoContenido <= altoVisible){
					barraEnScroll->establecerVisible(false);
				} else {
					barraEnScroll->establecerVisible(true);
					const AUTextura* texScroll	= barraEnScroll->textura();
					const NBTamano texScrollSz	= texScroll->tamanoHD();
					float altoBarraScroll = altoVisible * (altoVisible / altoContenido);
					if(altoBarraScroll < texScrollSz.ancho){
						altoBarraScroll = texScrollSz.ancho;
					}
					//
					const float yMax			= cajaEscena.yMin - cajaCont.yMin + _marginBottom;
					const float yMin			= cajaEscena.yMax - _marginTop  - 0.0f; //Se asume que todas las capas de contenido tienen su yMax = 0.0f;
					const float avanceRelScroll	= (posDst.y - yMin) / (yMax - yMin);
					//
					barraEnScroll->redimensionar(0.0f, 0.0f, altoBarraScroll, -texScrollSz.alto);
					barraEnScroll->establecerTraslacion(barraEnScroll->traslacion().x, cajaEscena.yMax - _marginTop - ((altoVisible - altoBarraScroll) * avanceRelScroll));
				}
			}
			//Indice en scroll
			/*if(indiceEnScroll != NULL && indiceCentroAlto != NULL){
				if(objEnScroll->esClase(AUEscenaCategorias::idTipoClase)){
					const float altoVisible = cajaEscena.yMax - cajaEscena.yMin - _marginTop - _marginBottom;
					if(*indiceCentroAlto != altoVisible){
						indiceEnScroll->vaciar();
						((AUEscenaCategorias*)objEnScroll)->agregarLetrasDeIndiceEn(indiceEnScroll, altoVisible);
						*indiceCentroAlto = altoVisible;
					}
				}
				indiceEnScroll->establecerTraslacion(indiceEnScroll->traslacion().x, cajaEscena.yMax - _marginTop);
			}*/
		}
		//Scrollear accesorios
		/*const float yTrasladoReal = posDst.y - posOrig.y;
		 if(yTrasladoReal != 0.0f){
		 if(contenidoEnScroll == ENLobbyPanel_MainLeft){
		 {
		 const NBCajaAABB cajaBarraSup	= _capaBarraSup->cajaAABBLocal();
		 const float altoBaraSup			= cajaBarraSup.yMax - cajaBarraSup.yMin;
		 const float yMaxBarra			= cajaEscena.yMax + altoBaraSup - _ultAltoNoUsableSuperior;
		 const float yMinBarra			= cajaEscena.yMax;
		 NBPunto posBarraSup				= _capaBarraSup->traslacion();
		 if(moverAccesorios) posBarraSup.y += yTrasladoReal;
		 if(posBarraSup.y > yMaxBarra) posBarraSup.y = yMaxBarra;
		 if(posBarraSup.y < yMinBarra) posBarraSup.y = yMinBarra;
		 _capaBarraSup->establecerTraslacion(posBarraSup);
		 //Transparencia
		 const float yRelBarra = (posBarraSup.y - yMinBarra) / (yMaxBarra - yMinBarra);
		 if(yRelBarra > 0.60f){
		 //Not visible
		 _capaBarraSup->establecerMultiplicadorColor8DeContenido(255, 255, 255, 0);
		 } else if(yRelBarra < 0.20f){
		 //Fully visible
		 _capaBarraSup->establecerMultiplicadorColor8DeContenido(255, 255, 255, 255);
		 } else {
		 //Animate transparency between the way
		 _capaBarraSup->establecerMultiplicadorColor8DeContenido(255, 255, 255, 255.0f * (1.0f - ((yRelBarra - 0.20f) / 0.40f)));
		 }
		 }
		 {
		 const NBCajaAABB cajaBarraInf	= _capaBarraInf->cajaAABBLocal();
		 const float altoBaraInf			= cajaBarraInf.yMax - cajaBarraInf.yMin;
		 NBPunto posBarraInf				= _capaBarraInf->traslacion();
		 const float yMaxBarra			= cajaEscena.yMin;
		 const float yMinBarra			= cajaEscena.yMin - (altoBaraInf * 1.0f);
		 if(moverAccesorios) posBarraInf.y += yTrasladoReal;
		 if(posBarraInf.y > yMaxBarra) posBarraInf.y = yMaxBarra;
		 if(posBarraInf.y < yMinBarra) posBarraInf.y = yMinBarra;
		 _capaBarraInf->establecerTraslacion(posBarraInf);
		 }
		 }
		 }*/
	}
	_scrollSceneBox = cajaEscena;
}

void AUEscenaCol::tickAnimacion(float segsTranscurridos){
	//Ocultar contenido con salida animada (push)
	if(_pilaCantOcultando != 0){
		SI32 i; const SI32 conteo = _pilaContenido.conteo - _pilaCantQuitando - 1; //-1 to ignore the current content
		for(i = 0; i < conteo; i++){
			STEscenaColData* datCont = _pilaContenido.elemPtr(i);
			if(!datCont->interfaz->animando()){
				AUEscenaContenedor* cont = (AUEscenaContenedor*)datCont->objeto->contenedor();
				if(cont != NULL){
					cont->quitarObjetoEscena(datCont->objeto);
					_pilaCantOcultando--;
					//PRINTF_INFO("Ocultando contenido #%d de %d despues de animacion.\n", (i+1), _pilaContenido.conteo);
				}
			}
		}
		NBASSERT(_pilaCantOcultando >= 0)
	}
	//Quitar contenido con salida animada (pop)
	if(_pilaCantQuitando != 0){
		STEscenaColData* datContAct = _pilaContenido.elemPtr(_pilaContenido.conteo - _pilaCantQuitando - 1);
		if(!datContAct->interfaz->animando()){
			SI32 i; const SI32 ultQuitar = _pilaContenido.conteo - _pilaCantQuitando;
			for(i = _pilaContenido.conteo - 1; i >= ultQuitar; i--){
				STEscenaColData* datCont = _pilaContenido.elemPtr(i);
				if(!datCont->interfaz->animando()){
					//PRINTF_INFO("Quitando contenido #%d de %d despues de animacion.\n", (i+1), _pilaContenido.conteo);
					//Quitar inmediatamente
					AUEscenaContenedor* cont = (AUEscenaContenedor*)datCont->objeto->contenedor();
					if(cont != NULL){
						cont->quitarObjetoEscena(datCont->objeto);
					}
					if(datCont->objeto != NULL) datCont->objeto->liberar(NB_RETENEDOR_THIS); datCont->objeto = NULL;
					if(datCont->strTitulo != NULL) datCont->strTitulo->liberar(NB_RETENEDOR_THIS); datCont->strTitulo = NULL;
					_pilaContenido.quitarElementoEnIndice(i);
					_pilaCantQuitando--;
				}
			}
		}
		NBASSERT(_pilaCantQuitando >= 0)
	}
	//Animar scroll
	const SI32 tchBefore	= _touchesCountAntes;
	const float velBefore	= _scrollSpeed;
	if(_touchesCount == 0 && _scrollSpeed != 0.0f){
		if(_scrollSpeed > 0.0f){
			_scrollSpeed -= 32.0f * segsTranscurridos; if(_scrollSpeed < 0.0f) _scrollSpeed = 0.0f;
		} else {
			_scrollSpeed += 32.0f * segsTranscurridos; if(_scrollSpeed > 0.0f) _scrollSpeed = 0.0f;
		}
		this->scrollContenido(_scrollSceneBox, _scrollSpeed);
	}
	//
	const SI32 tchAfter		= _touchesCount;
	const float velAfter	= _scrollSpeed;
	if((velBefore == 0.0f && tchBefore == 0) && (velAfter != 0.0f || tchAfter != 0)){
		//Show scroll-bar
		_scrollShow = true;
		_scrollAnimating = true;
		_scrollAnimatingAlpha = 0.0f;
	} else if((velBefore != 0.0f || tchBefore != 0) && (velAfter == 0.0f && tchAfter == 0)){
		//Hide scroll-bar
		_scrollShow = false;
		_scrollAnimating = true;
		_scrollAnimatingAlpha = 0.5f;
	}
	if(_scrollAnimating){
		if(_scrollShow){
			_scrollAnimatingAlpha += (segsTranscurridos * 3.0f);
			if(_scrollAnimatingAlpha > 0.5f){
				_scrollAnimatingAlpha = 0.5f;
				_scrollAnimating = false;
			}
		} else {
			_scrollAnimatingAlpha -= (segsTranscurridos * 3.0f);
			if(_scrollAnimatingAlpha < 0.0f){
				_scrollAnimatingAlpha = 0.0f;
				_scrollAnimating = false;
			}
		}
		_indiceScroll->establecerMultiplicadorAlpha8(255.0f * _scrollAnimatingAlpha * 2.0f);
		_barraScroll->establecerMultiplicadorAlpha8(255.0f * _scrollAnimatingAlpha);
	}
	//
	_touchesCountAntes = _touchesCount;
}

//

AUOBJMETODOS_CLASESID_UNICLASE(AUEscenaCol)
AUOBJMETODOS_CLASESNOMBRES_UNICLASE(AUEscenaCol, "AUEscenaCol")
AUOBJMETODOS_CLONAR_NULL(AUEscenaCol)
