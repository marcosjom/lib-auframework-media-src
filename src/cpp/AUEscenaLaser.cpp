
#include "AUFrameworkMediaStdAfx.h"
#include "AUEscenaLaser.h"

//-------------------------------------
//--- CONSTRUCTORES Y DESTRUCTORES ----
//-------------------------------------

AUEscenaLaser::AUEscenaLaser(const float alcanceLaser, const float anchoLaser) : AUEscenaObjeto(), _laserTrayectoriaEscena(this) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaLaser::AUEscenaLaser")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaLaser")
	#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
	_dbgLaserRegistradoComoRayo	= false;
	#endif
	_laserEscuchador			= NULL;
	_laserColisionIgnorarPrimera = false;
	_laserColisionaConFisica	= true;
	_laserColisionaConCuerdas	= true;
	_laserAlcance			= alcanceLaser;
	_laseAncho				= anchoLaser;
	_laserTexDesplazaVel.x	= 16.0f;
	_laserTexDesplazaVel.y	= 0.0f;
	_laserTexturaPatron		= NBGestorTexturas::texturaPatronDesdeArchivoPNG("Patrones/ptrnMadera02mitad.png"); _laserTexturaPatron->retener(NB_RETENEDOR_THIS);
	_laserTexDesplaza.x		= 0.0f;
	_laserTexDesplaza.y		= 0.0f;
	//
	_laserObstruccionFraccion = 0.0f;
	_colisionCuerdaConsumir	= NULL;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUEscenaLaser::AUEscenaLaser(AUEscenaLaser* otra) : AUEscenaObjeto(otra), _laserTrayectoriaEscena(this) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaLaser::AUEscenaLaser")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUEscenaLaser")
	NBASSERT(otra != NULL)
	#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
	_dbgLaserRegistradoComoRayo	= false;
	#endif
	_laserEscuchador			= NULL;
	_laserColisionIgnorarPrimera = otra->_laserColisionIgnorarPrimera;
	_laserColisionaConFisica	= otra->_laserColisionaConFisica;
	_laserColisionaConCuerdas	= otra->_laserColisionaConCuerdas;
	_laserAlcance			= otra->_laserAlcance;
	_laseAncho				= otra->_laseAncho;
	_laserTexDesplazaVel	= otra->_laserTexDesplazaVel;
	_laserTexturaPatron		= otra->_laserTexturaPatron; _laserTexturaPatron->retener(NB_RETENEDOR_THIS);
	_laserTexDesplaza.x		= 0.0f;
	_laserTexDesplaza.y		= 0.0f;
	//
	_laserObstruccionFraccion	= 0.0f;
	_colisionCuerdaConsumir	= NULL;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUEscenaLaser::~AUEscenaLaser(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaLaser::~AUEscenaLaser")
	NBASSERT(!_dbgLaserRegistradoComoRayo)
	if(_laserTexturaPatron != NULL) _laserTexturaPatron->liberar(NB_RETENEDOR_THIS); _laserTexturaPatron = NULL;
	if(_colisionCuerdaConsumir != NULL) _colisionCuerdaConsumir->cuerdaLiberarDeAnalisisCorte(); _colisionCuerdaConsumir = NULL;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}
 
//-------------------------------
//--- FUNCIONES DE INSTANCIA ----
//-------------------------------

float AUEscenaLaser::laserAlcance() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaLaser::laserAlcance")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _laserAlcance;
}

float AUEscenaLaser::laserAncho() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaLaser::laserAncho")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _laseAncho;
}

bool AUEscenaLaser::laserColisionIgnorarPrimera() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaLaser::laserColisionIgnorarPrimera")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _laserColisionIgnorarPrimera;
}

bool AUEscenaLaser::laserColisionaConFisica() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaLaser::laserColisionaConFisica")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _laserColisionaConFisica;
}

bool AUEscenaLaser::laserColisionaConCuerdas() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaLaser::laserColisionaConCuerdas")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _laserColisionaConCuerdas;
}

NBPunto AUEscenaLaser::laserVelDesplazaTex() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaLaser::laserVelDesplazaTex")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _laserTexDesplazaVel;
}

void AUEscenaLaser::establecerLaserEscuchador(IEscenaLaserEscuchador* laserEscuchador){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaLaser::establecerLaserEscuchador")
	_laserEscuchador = laserEscuchador;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaLaser::establecerLaserAlcance(const float alcance){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaLaser::establecerLaserAlcance")
	_laserAlcance = alcance;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaLaser::establecerLaserAncho(const float ancho){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaLaser::establecerLaserAncho")
	_laseAncho = ancho;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaLaser::establecerLaserVelDesplazaTex(const float velX, const float velY){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaLaser::establecerLaserVelDesplazaTex")
	_laserTexDesplazaVel.x = velX;
	_laserTexDesplazaVel.y = velY;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaLaser::establecerLaserVelDesplazaTex(const NBPunto &velDesplazaTex){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaLaser::establecerLaserVelDesplazaTex")
	_laserTexDesplazaVel = velDesplazaTex;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaLaser::establecerLaserColisionIgnorarPrimera(const bool ignorarPrimeraColision){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaLaser::establecerLaserColisionIgnorarPrimera")
	_laserColisionIgnorarPrimera = ignorarPrimeraColision;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaLaser::establecerLaserColisionaConFisica(const bool colisionaFisica) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaLaser::establecerLaserColisionaConFisica")
	_laserColisionaConFisica = colisionaFisica;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaLaser::establecerLaserColisionaConCuerdas(const bool colisionaCuerdas) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaLaser::establecerLaserColisionaConCuerdas")
	if(_laserColisionaConCuerdas!=colisionaCuerdas){
		_laserColisionaConCuerdas = colisionaCuerdas;
		if(idEscena!=-1){
			if(colisionaCuerdas){
				NBASSERT(!_dbgLaserRegistradoComoRayo)
				NBGestorEscena::agregarConsumidorCuerdas(idEscena, idCapa, this);
				#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
				_dbgLaserRegistradoComoRayo	= true;
				#endif
			} else {
				NBASSERT(_dbgLaserRegistradoComoRayo)
				NBGestorEscena::quitarConsumidorCuerdas(idEscena, idCapa, this);
				#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
				_dbgLaserRegistradoComoRayo	= false;
				#endif
			}
			NBASSERT(colisionaCuerdas == _dbgLaserRegistradoComoRayo)
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

//

NBCajaAABB AUEscenaLaser::cajaAABBLocalCalculada(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaLaser::cajaAABBLocalCalculada")
	NBCajaAABB cajaLocal;
	if(_laserTrayectoriaEscena.conteo==0){
		NBCAJAAABB_INICIALIZAR_CON_PUNTO(cajaLocal, 0, 0)
	} else {
		NBMatriz matrizEscenaInversa;	NBMATRIZ_INVERSA(matrizEscenaInversa, _cacheObjEscena.matrizEscena)
		NBCajaAABB cajaEscena; NBCAJAAABB_INICIALIZAR_CON_PUNTO(cajaEscena, _laserTrayectoriaEscena.elemento[0].posEscena.x, _laserTrayectoriaEscena.elemento[0].posEscena.y)
		UI16 iPos;
		for(iPos=1; iPos<_laserTrayectoriaEscena.conteo; iPos++){
			NBCAJAAABB_ENVOLVER_PUNTO_SIGUIENTE(cajaEscena, _laserTrayectoriaEscena.elemento[iPos].posEscena.x, _laserTrayectoriaEscena.elemento[iPos].posEscena.y)
		}
		NBMATRIZ_MULTIPLICAR_CAJAAABB(cajaLocal, matrizEscenaInversa, cajaEscena)
	}
	if(cajaLocal.xMin==cajaLocal.xMax){ cajaLocal.xMin -= 8.0f; cajaLocal.xMax += 8.0f; }
	if(cajaLocal.yMin==cajaLocal.yMax){ cajaLocal.yMin -= 8.0f; cajaLocal.yMax += 8.0f; }
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return cajaLocal;
}

void AUEscenaLaser::agregadoEnEscena(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaLaser::agregadoEnEscena")
	NBASSERT(!_dbgLaserRegistradoComoRayo)
	NBGestorEscena::agregarConsumidorEspejos(this->idEscena, this->idCapa, this);
	NBGestorEscena::agregarConsumidorPreModeloGL(this->idEscena, this->idCapa, this);
	NBGestorAnimadores::agregarAnimador(this, this);
	if(_laserColisionaConCuerdas){
		NBGestorEscena::agregarConsumidorCuerdas(this->idEscena, this->idCapa, this);
		#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
		_dbgLaserRegistradoComoRayo	= true;
		#endif
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaLaser::quitandoDeEscena(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaLaser::quitandoDeEscena")
	if(_laserColisionaConCuerdas){
		NBASSERT(_dbgLaserRegistradoComoRayo)
		NBGestorEscena::quitarConsumidorCuerdas(this->idEscena, this->idCapa, this);
		#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
		_dbgLaserRegistradoComoRayo	= false;
		#endif
	}
	NBGestorAnimadores::quitarAnimador(this);
	NBGestorEscena::quitarConsumidorPreModeloGL(this->idEscena, this->idCapa, this);
	NBGestorEscena::quitarConsumidorEspejos(this->idEscena, this->idCapa, this);
	NBASSERT(!_dbgLaserRegistradoComoRayo)
	if(_colisionCuerdaConsumir != NULL) _colisionCuerdaConsumir->cuerdaLiberarDeAnalisisCorte(); _colisionCuerdaConsumir = NULL;
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUEscenaLaser::consumirEspejos(const NBFuenteEspejoRender* arrEspejos, const UI16 conteoEspejos){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaLaser::consumirEspejos")
	_laserTrayectoriaEscena.vaciar();
	if(_laserAlcance>0.0f){
		NBPunto posEscenaIniRayo;		NBMATRIZ_MULTIPLICAR_PUNTO(posEscenaIniRayo, _cacheObjEscena.matrizEscena, 0.0f, 0.0f)
		NBPunto posEscenaFinRayo;		NBMATRIZ_MULTIPLICAR_PUNTO(posEscenaFinRayo, _cacheObjEscena.matrizEscena, _laserAlcance, 0.0f)
		NBPunto vectorUnitRayo;			NBPUNTO_ESTABLECER(vectorUnitRayo, posEscenaFinRayo.x-posEscenaIniRayo.x, posEscenaFinRayo.y-posEscenaIniRayo.y)
		float longitudRestanteRayo		= NBPUNTO_DISTANCIA_VECTOR(vectorUnitRayo.x, vectorUnitRayo.y); NBASSERT(longitudRestanteRayo!=0.0f)
		vectorUnitRayo.x				/= longitudRestanteRayo;
		vectorUnitRayo.y				/= longitudRestanteRayo;
		STEscenaLaserPos datosPosLaser;
		datosPosLaser.posEscena			= posEscenaIniRayo;
		datosPosLaser.moduloSegmento	= 0.0f;
		datosPosLaser.vecUnitSegmento	= vectorUnitRayo;
		datosPosLaser.vecUnitHaciaIzq.x = -vectorUnitRayo.y;
		datosPosLaser.vecUnitHaciaIzq.y = vectorUnitRayo.x;
		_laserTrayectoriaEscena.agregarElemento(datosPosLaser);
		SI16 iUltimoEspejoColisionado	= -1;
		bool reflejado, bloqueado; UI16 conteoVecesReflejado = 0;
		do {
			reflejado	= false;
			bloqueado	= false;
			SI16 iEspejoColisionado = -1;
			bool desdeIzquierdaEspejo = false;
			float denom, relAvanceRayo, relAvanceEspejo, relAvanceRayoRefejoMin = 2.0f;
			UI16 iEspejo;
			for(iEspejo=0; iEspejo<conteoEspejos; iEspejo++){
				if(iEspejo!=iUltimoEspejoColisionado){ //Para evitar una interseccion con el espejo-origen del rayo
					NBLINEA_INSTERSECCION_RELATIVOS(relAvanceRayo, relAvanceEspejo, denom, posEscenaIniRayo.x, posEscenaIniRayo.y, posEscenaFinRayo.x, posEscenaFinRayo.y, arrEspejos[iEspejo].limitesEscenaEspejo[0].x, arrEspejos[iEspejo].limitesEscenaEspejo[0].y, arrEspejos[iEspejo].limitesEscenaEspejo[1].x, arrEspejos[iEspejo].limitesEscenaEspejo[1].y)
					if(denom!=0.0f && relAvanceRayo>0.0f && relAvanceRayo<1.0f && relAvanceEspejo>0.0f && relAvanceEspejo<1.0f){
						//El rayo intersecta con el espejo
						if(relAvanceRayoRefejoMin>relAvanceRayo){
							NBPunto vectorDesdeIniEspejo; NBPUNTO_ESTABLECER(vectorDesdeIniEspejo, posEscenaIniRayo.x - arrEspejos[iEspejo].limitesEscenaEspejo[0].x, posEscenaIniRayo.y - arrEspejos[iEspejo].limitesEscenaEspejo[0].y)
							desdeIzquierdaEspejo	= (NBPUNTO_PRODUCTO_VECTORIAL(arrEspejos[iEspejo].vecUnitEspejo.x, arrEspejos[iEspejo].vecUnitEspejo.y, vectorDesdeIniEspejo.x, vectorDesdeIniEspejo.y)>0.0f);
							iEspejoColisionado		= iEspejo;
							relAvanceRayoRefejoMin	= relAvanceRayo;
						}
					}
				}
			}
			if(iEspejoColisionado!=-1){
				iUltimoEspejoColisionado		= iEspejoColisionado;
				NBASSERT(relAvanceRayoRefejoMin>=0.0f && relAvanceRayoRefejoMin<=1.0f)
				NBASSERT(iEspejoColisionado<conteoEspejos)
				NBPUNTO_ESTABLECER(posEscenaIniRayo, posEscenaIniRayo.x + ((posEscenaFinRayo.x - posEscenaIniRayo.x) * relAvanceRayoRefejoMin), posEscenaIniRayo.y + ((posEscenaFinRayo.y - posEscenaIniRayo.y) * relAvanceRayoRefejoMin))
				float avanceEnEspejo, avanceIzq; NBPunto vectorReflejar;
				NBPUNTO_VECTOR_REFLEJADO(vectorUnitRayo, arrEspejos[iEspejoColisionado].vecUnitEspejo, vectorUnitRayo, avanceIzq, avanceEnEspejo)
				datosPosLaser.posEscena			= posEscenaIniRayo;
				datosPosLaser.vecUnitSegmento	= vectorUnitRayo;
				datosPosLaser.moduloSegmento	= longitudRestanteRayo * relAvanceRayoRefejoMin;
				if(desdeIzquierdaEspejo){
					datosPosLaser.vecUnitHaciaIzq.x	= -arrEspejos[iEspejoColisionado].vecUnitEspejo.x;
					datosPosLaser.vecUnitHaciaIzq.y	= -arrEspejos[iEspejoColisionado].vecUnitEspejo.y;
					longitudRestanteRayo		*= (1.0f - relAvanceRayoRefejoMin);
					NBPUNTO_POSICION_REFLEJADA(posEscenaFinRayo, arrEspejos[iEspejoColisionado].limitesEscenaEspejo[0], arrEspejos[iEspejoColisionado].vecUnitEspejo, posEscenaFinRayo, vectorReflejar, avanceIzq, avanceEnEspejo)
					conteoVecesReflejado++;
					bloqueado					= (conteoVecesReflejado>3);
					reflejado					= !bloqueado;
				} else {
					datosPosLaser.vecUnitHaciaIzq.x	= arrEspejos[iEspejoColisionado].vecUnitEspejo.x;
					datosPosLaser.vecUnitHaciaIzq.y	= arrEspejos[iEspejoColisionado].vecUnitEspejo.y;
					bloqueado					= true;
				}
				_laserTrayectoriaEscena.agregarElemento(datosPosLaser);
			}
		} while(reflejado);
		if(!bloqueado){
			datosPosLaser.posEscena			= posEscenaFinRayo;
			datosPosLaser.moduloSegmento	= longitudRestanteRayo;
			datosPosLaser.vecUnitSegmento	= vectorUnitRayo;
			datosPosLaser.vecUnitHaciaIzq.x = -vectorUnitRayo.y;
			datosPosLaser.vecUnitHaciaIzq.y = vectorUnitRayo.x;
			_laserTrayectoriaEscena.agregarElemento(datosPosLaser);
		}
		_laserObstruccionFraccion			= _laserTrayectoriaEscena.conteo - 1;
		NBASSERT(_laserTrayectoriaEscena.conteo>=2)
		NBASSERT(_laserObstruccionFraccion>0.0f && _laserObstruccionFraccion<_laserTrayectoriaEscena.conteo)
		
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaLaser::consumirCuerdas(ICuerdaCortable** arrCuerdas, const UI16 conteoCuerdas){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaLaser::consumirCuerdas")
	if(_colisionCuerdaConsumir != NULL) _colisionCuerdaConsumir->cuerdaLiberarDeAnalisisCorte(); _colisionCuerdaConsumir = NULL;
	const STEscenaLaserPos* arrPosEscena	= _laserTrayectoriaEscena.elemento;
	const UI16 conteoPosEscena				= _laserTrayectoriaEscena.conteo;
	SI16 iCuerdaCorteMasCercano				= -1;
	UI16 iCuerda; STCuerdaCorte corteTmp;
	for(iCuerda=0; iCuerda<conteoCuerdas; iCuerda++){
		if(arrCuerdas[iCuerda]->cuerdaAnalizarSiRayoCorta(arrPosEscena, conteoPosEscena, &corteTmp)){
			if(iCuerdaCorteMasCercano==-1 || corteTmp.rayoAvanceRelEnRayo<_colisionCuerdaDatos.rayoAvanceRelEnRayo){
				_colisionCuerdaDatos		= corteTmp;
				iCuerdaCorteMasCercano		= iCuerda;
			}
		}
	}
	if(iCuerdaCorteMasCercano!=-1){
		_colisionCuerdaConsumir				= arrCuerdas[iCuerdaCorteMasCercano];
		_colisionCuerdaConsumir->cuerdaRetenerParaAnalisisCorte();
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUEscenaLaser::consumirAntesDeCrearModeloGL(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaLaser::consumirAntesDeCrearModeloGL")
	NBASSERT(_laserObstruccionFraccion==_laserTrayectoriaEscena.conteo-1)
	//Inicializar obstruccion
	float obstruccionLimite		= _laserTrayectoriaEscena.conteo - 1;
	float obstruccionFisica		= obstruccionLimite;
	float obstruccionCuerda		= _colisionCuerdaConsumir == NULL? obstruccionLimite : _colisionCuerdaDatos.rayoAvanceRelEnRayo; NBASSERT(_colisionCuerdaConsumir == NULL || (_colisionCuerdaDatos.rayoAvanceRelEnRayo>0.0f && _colisionCuerdaDatos.rayoAvanceRelEnRayo<_laserTrayectoriaEscena.conteo))
	//Detectar colision con fisica
	/*b2Body*/void* cuerpoObstructor		= NULL;
	NBPunto cuerpoPosColEscena, cuerpoNormalCol;
	if(_idMundoFisica!=-1 && _laserTrayectoriaEscena.conteo>1){
		_colisionFraccionBase	= 0.0f;
		_colisionFisicaDatos[0].fraccion	= _laserTrayectoriaEscena.conteo - 1;
		_colisionFisicaDatos[1].fraccion	= _laserTrayectoriaEscena.conteo - 1;
		_colisionFisicaDatos[0].cuerpo		= NULL;
		_colisionFisicaDatos[1].cuerpo		= NULL;
#		ifndef CONFIG_NB_UNSUPPORT_BOX2D
		UI16 indiceDatosCol		= _laserColisionIgnorarPrimera ? 1 : 0;
		b2Body** ptrCuerpoCol	= (b2Body**)&_colisionFisicaDatos[indiceDatosCol].cuerpo;
		NBPunto posEscenaAnt	= _laserTrayectoriaEscena.elemento[0].posEscena;
		UI16 iPosEscena;
		for(iPosEscena=1; iPosEscena<_laserTrayectoriaEscena.conteo; iPosEscena++){
			NBPunto posEscenaAct = _laserTrayectoriaEscena.elemento[iPosEscena].posEscena;
			NBGestorFisica::interseccionesConLinea(_idMundoFisica, this, posEscenaAnt.x/ESCALA_PIXELES_POR_METRO_MUNDO_FISICA, posEscenaAnt.y/ESCALA_PIXELES_POR_METRO_MUNDO_FISICA, posEscenaAct.x/ESCALA_PIXELES_POR_METRO_MUNDO_FISICA, posEscenaAct.y/ESCALA_PIXELES_POR_METRO_MUNDO_FISICA);
			if(*ptrCuerpoCol != NULL){
				cuerpoObstructor		= *ptrCuerpoCol;
				obstruccionFisica		= _colisionFisicaDatos[indiceDatosCol].fraccion;
				NBPUNTO_ESTABLECER(cuerpoPosColEscena, _colisionFisicaDatos[indiceDatosCol].posFisica.x * ESCALA_PIXELES_POR_METRO_MUNDO_FISICA, _colisionFisicaDatos[indiceDatosCol].posFisica.y * ESCALA_PIXELES_POR_METRO_MUNDO_FISICA)
				NBPUNTO_ESTABLECER(cuerpoNormalCol, _colisionFisicaDatos[indiceDatosCol].normal.x, _colisionFisicaDatos[indiceDatosCol].normal.y)
				break;
			}
			posEscenaAnt		= posEscenaAct;
			_colisionFraccionBase++;
		}
#		endif
	}
	//Determinar la obstruccion mas cercana
	if(obstruccionCuerda<obstruccionFisica){
		//Hubo obstruccion por cuerda
		NBASSERT(_colisionCuerdaConsumir != NULL)
		NBASSERT(obstruccionCuerda>0.0f && obstruccionCuerda<_laserTrayectoriaEscena.conteo)
		_laserObstruccionFraccion		= obstruccionCuerda;
		if(_laserEscuchador != NULL)	_laserEscuchador->laserEscenaTickObstruccionPorCuerda(this, 1.0f/60.0f /*temporal, resolver*/, _colisionCuerdaConsumir, _colisionCuerdaDatos.rayoPosEscenaColision, _colisionCuerdaDatos);
	} else if(obstruccionFisica<obstruccionCuerda){
		//Hubo obstruccion por cuerpo con fisica
		NBASSERT(cuerpoObstructor != NULL)
		NBASSERT(obstruccionFisica>0.0f && obstruccionFisica<_laserTrayectoriaEscena.conteo)
		_laserObstruccionFraccion		= obstruccionFisica;
		if(_laserEscuchador != NULL)	_laserEscuchador->laserEscenaTickObstruccionPorFisica(this, 1.0f/60.0f /*temporal, resolver*/, cuerpoObstructor, cuerpoPosColEscena, cuerpoNormalCol);
	}
	//
	if(_colisionCuerdaConsumir != NULL) _colisionCuerdaConsumir->cuerdaLiberarDeAnalisisCorte(); _colisionCuerdaConsumir = NULL;
	NBASSERT(_laserObstruccionFraccion>0.0f && _laserObstruccionFraccion<_laserTrayectoriaEscena.conteo)
	NBASSERT(_laserTrayectoriaEscena.conteo>=2)
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

#ifndef CONFIG_NB_UNSUPPORT_BOX2D
float32 AUEscenaLaser::ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float32 fraction){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUEscenaLaser::ReportFixture")
	if(!fixture->IsSensor()){
		fraction					+= _colisionFraccionBase;
		if(_colisionFisicaDatos[0].fraccion>fraction){
			_colisionFisicaDatos[1]				= _colisionFisicaDatos[0];
			_colisionFisicaDatos[0].posFisica.x	= point.x;
			_colisionFisicaDatos[0].posFisica.y	= point.y;
			_colisionFisicaDatos[0].normal.x	= normal.x;
			_colisionFisicaDatos[0].normal.y	= normal.y;
			_colisionFisicaDatos[0].fraccion	= fraction;
			_colisionFisicaDatos[0].cuerpo		= fixture->GetBody();
		} else if(_colisionFisicaDatos[1].fraccion>fraction) {
			_colisionFisicaDatos[1].posFisica.x	= point.x;
			_colisionFisicaDatos[1].posFisica.y	= point.y;
			_colisionFisicaDatos[1].normal.x	= normal.x;
			_colisionFisicaDatos[1].normal.y	= normal.y;
			_colisionFisicaDatos[1].fraccion	= fraction;
			_colisionFisicaDatos[1].cuerpo		= fixture->GetBody();
		}
	}
	NBASSERT(_colisionFisicaDatos[0].fraccion<=_colisionFisicaDatos[1].fraccion)
	AU_GESTOR_PILA_LLAMADAS_POP_3
	/// return -1: ignore this fixture and continue
	/// return 0: terminate the ray cast
	/// return fraction: clip the ray to this point
	/// return 1: don't clip the ray and continue
	return -1.0f;
}
#endif

void AUEscenaLaser::tickAnimacion(float segsTranscurridos){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaLaser::tickAnimacion")
	NBASSERT(_laserTrayectoriaEscena.conteo>=2)
	//Animar textura de laser
	if(_laserTexturaPatron != NULL){
		NBTamano tamHDTex		= _laserTexturaPatron->tamanoHD();
		_laserTexDesplaza.x		-= _laserTexDesplazaVel.x * segsTranscurridos; while(_laserTexDesplaza.x>tamHDTex.ancho) _laserTexDesplaza.x -= tamHDTex.ancho; while(_laserTexDesplaza.x<-tamHDTex.ancho) _laserTexDesplaza.x += tamHDTex.ancho;
		_laserTexDesplaza.y		-= _laserTexDesplazaVel.y * segsTranscurridos; while(_laserTexDesplaza.y>tamHDTex.alto) _laserTexDesplaza.y -= tamHDTex.alto; while(_laserTexDesplaza.y<-tamHDTex.alto) _laserTexDesplaza.y += tamHDTex.alto;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

PTRfunCmdsGL AUEscenaLaser::actualizarModeloGL(const NBPropRenderizado &propsRenderizado, const NBPropIluminacion &propsIluminacion, const NBPropHeredadasRender &propsHeredadas){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaLaser::actualizarModeloGL")
	NBASSERT(idEscena!=-1) //Solo si esta en escena
	//Retorno temprano
	if(_laserTrayectoriaEscena.conteo<2){ AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO_2(NULL) }
	//
	NBASSERT(_laserTrayectoriaEscena.conteo>=2)
	NBASSERT(_laserObstruccionFraccion>0.0f && _laserObstruccionFraccion<_laserTrayectoriaEscena.conteo)
	float porcionUltimoSegmento				= _laserObstruccionFraccion - (float)((SI32)_laserObstruccionFraccion);
	UI16 conteoSegmentos					= _laserObstruccionFraccion + (porcionUltimoSegmento!=0.0f?1:0);
	if(conteoSegmentos!=0){
		UI16 conteoSegmentoCompletos		= _laserObstruccionFraccion;
		STBloqueGL bloqueVerticesGL;
		bloqueVerticesGL.cantidadElementos	= conteoSegmentos * 4;
		bloqueVerticesGL.primerElemento		= (*propsRenderizado.funcReservarVertices)(ENVerticeGlTipo_MonoTextura, bloqueVerticesGL.cantidadElementos);
		NBVerticeGL* verticesGL				= &((*propsRenderizado.verticesGL1)[bloqueVerticesGL.primerElemento]);
		float mitadAnchoLaser				= _laseAncho * 0.5f;
		//ACTUALIZAR VERTICES
		NBColor8 colorMultiplicado8;		NBCOLOR_ESTABLECER(colorMultiplicado8, _propiedades.color8.r*propsHeredadas.colorPadre.r, _propiedades.color8.g*propsHeredadas.colorPadre.g, _propiedades.color8.b*propsHeredadas.colorPadre.b, _propiedades.color8.a*propsHeredadas.colorPadre.a);
		STEscenaLaserPos posEnLaser			= _laserTrayectoriaEscena.elemento[0];
		float distanciaIzq					= NBPUNTO_PRODUCTO_VECTORIAL(posEnLaser.vecUnitSegmento.x, posEnLaser.vecUnitSegmento.y, posEnLaser.vecUnitHaciaIzq.x, posEnLaser.vecUnitHaciaIzq.y); NBASSERT(distanciaIzq>0.0f) //Debe apuntar hacia la izquierda y no ser colineal
		float avanceEnVectorIzq				= mitadAnchoLaser / distanciaIzq;
		NBPunto posIzqAnt;					NBPUNTO_ESTABLECER(posIzqAnt, posEnLaser.posEscena.x + (posEnLaser.vecUnitHaciaIzq.x * avanceEnVectorIzq), posEnLaser.posEscena.y + (posEnLaser.vecUnitHaciaIzq.y * avanceEnVectorIzq))
		NBPunto posDerAnt;					NBPUNTO_ESTABLECER(posDerAnt, posEnLaser.posEscena.x - (posEnLaser.vecUnitHaciaIzq.x * avanceEnVectorIzq), posEnLaser.posEscena.y - (posEnLaser.vecUnitHaciaIzq.y * avanceEnVectorIzq))
		NBTamano tamTexturaHD;				if(_laserTexturaPatron != NULL) tamTexturaHD = _laserTexturaPatron->tamanoHD(); else tamTexturaHD.ancho = tamTexturaHD.alto = 256.0f;
		NBTamano tamTexturaHDInv;			NBTAMANO_ESTABLECER(tamTexturaHDInv, 1.0f / tamTexturaHD.ancho, 1.0f / tamTexturaHD.alto)
		UI16 iPos;
		for(iPos=1; iPos<=conteoSegmentoCompletos; iPos++){
			NBASSERT(iPos<_laserTrayectoriaEscena.conteo)
			posEnLaser						= _laserTrayectoriaEscena.elemento[iPos];
			distanciaIzq					= NBPUNTO_PRODUCTO_VECTORIAL(posEnLaser.vecUnitSegmento.x, posEnLaser.vecUnitSegmento.y, posEnLaser.vecUnitHaciaIzq.x, posEnLaser.vecUnitHaciaIzq.y); NBASSERT(distanciaIzq>0.0f) //Debe apuntar hacia la izquierda y no ser colineal
			avanceEnVectorIzq				= mitadAnchoLaser / distanciaIzq;
			NBPunto posIzqAct;				NBPUNTO_ESTABLECER(posIzqAct, posEnLaser.posEscena.x + (posEnLaser.vecUnitHaciaIzq.x * avanceEnVectorIzq), posEnLaser.posEscena.y + (posEnLaser.vecUnitHaciaIzq.y * avanceEnVectorIzq))
			NBPunto posDerAct;				NBPUNTO_ESTABLECER(posDerAct, posEnLaser.posEscena.x - (posEnLaser.vecUnitHaciaIzq.x * avanceEnVectorIzq), posEnLaser.posEscena.y - (posEnLaser.vecUnitHaciaIzq.y * avanceEnVectorIzq))
			//
			NBPUNTO_ESTABLECER(verticesGL[0], posIzqAnt.x, posIzqAnt.y);
			//NBPUNTO_ESTABLECER(verticesGL[0].tex, 0.0f, 0.0f);
			NBPUNTO_ESTABLECER(verticesGL[0].tex, _laserTexDesplaza.x * tamTexturaHDInv.ancho, (_laserTexDesplaza.y + mitadAnchoLaser) * tamTexturaHDInv.alto);
			NBCOLOR_ESTABLECER(verticesGL[0], colorMultiplicado8.r, colorMultiplicado8.g, colorMultiplicado8.b, colorMultiplicado8.a);
			//
			NBPUNTO_ESTABLECER(verticesGL[1], posDerAnt.x, posDerAnt.y);
			//NBPUNTO_ESTABLECER(verticesGL[1].tex, 0.0f, 0.0f);
			NBPUNTO_ESTABLECER(verticesGL[1].tex, _laserTexDesplaza.x * tamTexturaHDInv.ancho, (_laserTexDesplaza.y - mitadAnchoLaser) * tamTexturaHDInv.alto);
			NBCOLOR_ESTABLECER(verticesGL[1], colorMultiplicado8.r, colorMultiplicado8.g, colorMultiplicado8.b, colorMultiplicado8.a);
			//
			NBPUNTO_ESTABLECER(verticesGL[2], posDerAct.x, posDerAct.y);
			//NBPUNTO_ESTABLECER(verticesGL[2].tex, 0.0f, 0.0f);
			NBPUNTO_ESTABLECER(verticesGL[2].tex, (_laserTexDesplaza.x + posEnLaser.moduloSegmento) * tamTexturaHDInv.ancho, (_laserTexDesplaza.y + mitadAnchoLaser) * tamTexturaHDInv.alto);
			NBCOLOR_ESTABLECER(verticesGL[2], colorMultiplicado8.r, colorMultiplicado8.g, colorMultiplicado8.b, colorMultiplicado8.a);
			//
			NBPUNTO_ESTABLECER(verticesGL[3], posIzqAct.x, posIzqAct.y);
			//NBPUNTO_ESTABLECER(verticesGL[3].tex, 0.0f, 0.0f);
			NBPUNTO_ESTABLECER(verticesGL[3].tex, (_laserTexDesplaza.x + posEnLaser.moduloSegmento) * tamTexturaHDInv.ancho, (_laserTexDesplaza.y - mitadAnchoLaser) * tamTexturaHDInv.alto);
			NBCOLOR_ESTABLECER(verticesGL[3], colorMultiplicado8.r, colorMultiplicado8.g, colorMultiplicado8.b, colorMultiplicado8.a);
			//
			verticesGL						+= 4;
			posIzqAnt						= posIzqAct;
			posDerAnt						= posDerAct;
		}
		//Ultimo segmento parcial
		if(porcionUltimoSegmento!=0.0f){
			NBASSERT(iPos<_laserTrayectoriaEscena.conteo)
			posEnLaser						= _laserTrayectoriaEscena.elemento[iPos];
			distanciaIzq					= NBPUNTO_PRODUCTO_VECTORIAL(posEnLaser.vecUnitSegmento.x, posEnLaser.vecUnitSegmento.y, posEnLaser.vecUnitHaciaIzq.x, posEnLaser.vecUnitHaciaIzq.y); NBASSERT(distanciaIzq>0.0f) //Debe apuntar hacia la izquierda y no ser colineal
			avanceEnVectorIzq				= mitadAnchoLaser / distanciaIzq;
			NBPunto posAnterior				= _laserTrayectoriaEscena.elemento[iPos-1].posEscena;
			NBPunto posFinalPorcion;		NBPUNTO_ESTABLECER(posFinalPorcion, posAnterior.x + ((posEnLaser.posEscena.x - posAnterior.x) * porcionUltimoSegmento), posAnterior.y + ((posEnLaser.posEscena.y - posAnterior.y) * porcionUltimoSegmento))
			NBPunto posIzqAct;				NBPUNTO_ESTABLECER(posIzqAct, posFinalPorcion.x + (posEnLaser.vecUnitHaciaIzq.x * avanceEnVectorIzq), posFinalPorcion.y + (posEnLaser.vecUnitHaciaIzq.y * avanceEnVectorIzq))
			NBPunto posDerAct;				NBPUNTO_ESTABLECER(posDerAct, posFinalPorcion.x - (posEnLaser.vecUnitHaciaIzq.x * avanceEnVectorIzq), posFinalPorcion.y - (posEnLaser.vecUnitHaciaIzq.y * avanceEnVectorIzq))
			//
			NBPUNTO_ESTABLECER(verticesGL[0], posIzqAnt.x, posIzqAnt.y);
			//NBPUNTO_ESTABLECER(verticesGL[0].tex, 0.0f, 0.0f);
			NBPUNTO_ESTABLECER(verticesGL[0].tex, _laserTexDesplaza.x * tamTexturaHDInv.ancho, (_laserTexDesplaza.y + mitadAnchoLaser) * tamTexturaHDInv.alto);
			NBCOLOR_ESTABLECER(verticesGL[0], colorMultiplicado8.r, colorMultiplicado8.g, colorMultiplicado8.b, colorMultiplicado8.a);
			//
			NBPUNTO_ESTABLECER(verticesGL[1], posDerAnt.x, posDerAnt.y);
			//NBPUNTO_ESTABLECER(verticesGL[1].tex, 0.0f, 0.0f);
			NBPUNTO_ESTABLECER(verticesGL[1].tex, _laserTexDesplaza.x * tamTexturaHDInv.ancho, (_laserTexDesplaza.y - mitadAnchoLaser) * tamTexturaHDInv.alto);
			NBCOLOR_ESTABLECER(verticesGL[1], colorMultiplicado8.r, colorMultiplicado8.g, colorMultiplicado8.b, colorMultiplicado8.a);
			//
			NBPUNTO_ESTABLECER(verticesGL[2], posDerAct.x, posDerAct.y);
			//NBPUNTO_ESTABLECER(verticesGL[2].tex, 0.0f, 0.0f);
			NBPUNTO_ESTABLECER(verticesGL[2].tex, (_laserTexDesplaza.x + posEnLaser.moduloSegmento) * tamTexturaHDInv.ancho, (_laserTexDesplaza.y + mitadAnchoLaser) * tamTexturaHDInv.alto);
			NBCOLOR_ESTABLECER(verticesGL[2], colorMultiplicado8.r, colorMultiplicado8.g, colorMultiplicado8.b, colorMultiplicado8.a);
			//
			NBPUNTO_ESTABLECER(verticesGL[3], posIzqAct.x, posIzqAct.y);
			//NBPUNTO_ESTABLECER(verticesGL[3].tex, 0.0f, 0.0f);
			NBPUNTO_ESTABLECER(verticesGL[3].tex, (_laserTexDesplaza.x + posEnLaser.moduloSegmento) * tamTexturaHDInv.ancho, (_laserTexDesplaza.y - mitadAnchoLaser) * tamTexturaHDInv.alto);
			NBCOLOR_ESTABLECER(verticesGL[3], colorMultiplicado8.r, colorMultiplicado8.g, colorMultiplicado8.b, colorMultiplicado8.a);
			//
			verticesGL						+= 4;
		}
		//
		NBASSERT(conteoSegmentos > 0)
		const SI32 posicionDatos			= propsRenderizado.bytesDatosModelos->conteo; propsRenderizado.bytesDatosModelos->reservarRegistrosAlFinal(sizeof(STEscenaLaserRender));
		STEscenaLaserRender* propsRender	= (STEscenaLaserRender*)&(propsRenderizado.bytesDatosModelos->elemento[posicionDatos]);
		propsRender->conteoSegmentos		= conteoSegmentos;
		propsRender->bloqueIndicesGL		= NB_GESTOR_GL_DAME_ELEMS_PARA_TRIANGSTRIPS_4(ENVerticeGlTipo_MonoTextura, bloqueVerticesGL, conteoSegmentos);
		if(_laserTexturaPatron != NULL){
			propsRender->idTexturaGL = _laserTexturaPatron->idTexturaGL; propsRenderizado.piscinaRetecionRender->agregarElemento(_laserTexturaPatron);
		} else {
			//required on some systems where texture-0 will not be drawn
			propsRender->idTexturaGL = NBGestorTexturas::texturaBlanca->idTexturaGL;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return (conteoSegmentos !=0 ? &AUEscenaLaser::enviarComandosGL : NULL);
}

void AUEscenaLaser::enviarComandosGL(UI8* punteroDatosModelo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUEscenaLaser::enviarComandosGL")
	STEscenaLaserRender* propsRender = (STEscenaLaserRender*)punteroDatosModelo;
	NBASSERT(propsRender->bloqueIndicesGL.cantidadElementos>0)
	NBASSERT(propsRender->idTexturaGL>=0)
	NBGestorGL::activarVerticesGL(ENVerticeGlTipo_MonoTextura); NBGESTORGL_DBG_NOMBRAR_ACTIVADOR_VERTICES_GL("AUEscenaLaser")
	NBGestorGL::bindTexture(0, propsRender->idTexturaGL);
	NB_GESTOR_GL_RENDER_TRIANGSTRIPS_4(propsRender->bloqueIndicesGL.primerElemento, propsRender->bloqueIndicesGL.cantidadElementos); NBGESTORGL_DBG_NOMBRAR_CONVOCADOR_ACUMULAR_TRIANG_STRIPS_INDEP("AUEscenaLaser")
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

//
AUOBJMETODOS_CLASESID_MULTICLASE(AUEscenaLaser, AUEscenaObjeto)
AUOBJMETODOS_CLASESNOMBRES_MULTICLASE(AUEscenaLaser, "AUEscenaLaser", AUEscenaObjeto)
AUOBJMETODOS_CLONAR_THIS(AUEscenaLaser)









