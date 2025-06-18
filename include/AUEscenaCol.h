//
//  AUEscenaCol.h
//  lib-auframework-media
//
//  Created by Marcos Ortega on 28/12/18.
//

#ifndef AUEscenaCol_h
#define AUEscenaCol_h

#include "AUEscenaObjeto.h"
#include "AUEscenaContenedor.h"
#include "NBAnimador.h"
#include "AUEscenaSpriteElastico.h"
#include "AUEscenaContentI.h"

typedef struct STEscenaColDataRef_ {
	const AUEscenaObjeto*	objeto;
	const AUEscenaContentI*	interfaz;
	const char*				strTitulo;
} STEscenaColDataRef;

typedef struct STEscenaColData_ {
	AUEscenaObjeto*		objeto;
	AUEscenaContentI*	interfaz;
	AUCadena8*			strTitulo;
} STEscenaColData;

class AUEscenaCol: public AUEscenaContenedor, public NBAnimador {
public:
	AUEscenaCol();
	virtual ~AUEscenaCol();
	//
	float					colLeft() const;
	void					setColLeft(const float colLeft);
	float					colWidth() const;
	void					setColWidth(const float colWidth);
	//
	/*float					indiceScrollAlto() const;
	AUEscenaContenedor*		indiceScroll() const;
	AUEscenaSpriteElastico*	barraScroll() const;*/
	//
	float					marginTop() const;
	void					setMarginTop(const float height);
	float					marginBottom() const;
	void					setMarginBottom(const float height);
	//
	SI32					touchesCount() const;
	void					touchesPush();
	void					touchesPop();
	float					scrollSpeed() const;
	void					setScrollSpeed(const float speed);
	//
	//ToDo: Remove.
	//const AUArregloNativoP<STEscenaColData>*	pilaContenido() const;
	SI32					stackSize() const;
	SI32					stackHiddingCount() const;	//Contenidos previos al actual que se estan ocultando (accion push)
	SI32					stackRemovingCount() const;	//Contenidos posteriores al actual que se estan quitando (accion pop)
	void					empty();
	//
	STEscenaColData*		curContent() const;
	STEscenaColData*		topContent(const SI32 indiceDesdeFinal) const;
	void					push(const NBCajaAABB cajaEscena, AUEscenaObjeto* objAgregar, AUEscenaContentI* interfaz, const char* strTitulo, const bool animarEntradaNuevo, const bool animarSalidaUltimo, const bool intercambiarConUltimo);
	bool					pop(const NBCajaAABB cajaEscena, bool animarEntradaPenultimo, bool animarSalidaUltimo);
	void					scrollContenido(const NBCajaAABB cajaEscena, const float yTraslado);
	//
	void					tickAnimacion(float segsTranscurridos);
	//
	AUOBJMETODOS_CLASESID_DEFINICION
	AUOBJMETODOS_CLASESNOMBRES_DEFINICION
	AUOBJMETODOS_CLONAR_DEFINICION
protected:
	float					_colLeft;
	float					_colWidth;
	float					_indiceScrollAlto;
	AUEscenaContenedor*		_indiceScroll;
	AUEscenaSpriteElastico*	_barraScroll;
	//
	float					_marginTop;
	float					_marginBottom;
	//
	SI32					_touchesCountAntes;
	SI32					_touchesCount;
	float					_scrollAnimatingAlpha;
	bool					_scrollAnimating;
	bool					_scrollShow;
	float					_scrollSpeed;
	NBCajaAABB				_scrollSceneBox;
	//
	AUArregloNativoMutableP<STEscenaColData> _pilaContenido;
	SI32					_pilaCantOcultando;	//Contenidos previos al actual que se estan ocultando (accion push)
	SI32					_pilaCantQuitando;	//Contenidos posteriores al actual que se estan quitando (accion pop)
	//
	void					privEmpty();
	void					privScrollContent(const NBCajaAABB cajaEscena, const float yTraslado, STEscenaColData* datEnScroll);
};

#endif /* AUEscenaCol_h */
