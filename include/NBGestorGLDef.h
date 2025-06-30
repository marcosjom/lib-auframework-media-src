//
//  NBGestorGLDef.h
//  AUFramework
//
//  Created by Marcos Ortega on 07/12/12.
//
//

#ifndef AUFramework_NBGestorGLDef_h
#define AUFramework_NBGestorGLDef_h

struct STBloqueGL {
	UI32	primerElemento;				//verticeGL o indiceGL
	UI32	cantidadElementos;			//cantidad
	bool operator==(const STBloqueGL &otro) const {
		return (primerElemento == otro.primerElemento && cantidadElementos == otro.cantidadElementos);
	}
	bool operator!=(const STBloqueGL &otro) const {
		return !(primerElemento == otro.primerElemento && cantidadElementos == otro.cantidadElementos);
	}
};

struct STCoordTex {
	GLfloat x;
	GLfloat y;
	//
	bool operator==(const STCoordTex &otro) const {
		return (x==otro.x && y==otro.y);
	}
	bool operator!=(const STCoordTex &otro) const {
		return !(x==otro.x && y==otro.y);
	}
};

//Optimizacion: es importante que esta estructura mida bytes multiplo de 4 bytes.
struct NBVerticeGL0 {
	//coordenadas de vertice
	GLfloat		x;
	GLfloat		y;
	//color de vertice
	GLubyte		r;
	GLubyte		g;
	GLubyte		b;
	GLubyte		a;
	//
	bool operator==(const NBVerticeGL0 &otro) const {
		return (x==otro.x && y==otro.y && r==otro.r && g==otro.g && b==otro.b && a==otro.a);
	}
	bool operator!=(const NBVerticeGL0 &otro) const {
		return !(x==otro.x && y==otro.y && r==otro.r && g==otro.g && b==otro.b && a==otro.a);
	}
};

//Optimizacion: es importante que esta estructura mida bytes multiplo de 4 bytes.
struct NBVerticeGL : public NBVerticeGL0 {
	//coordenadas de textura
	STCoordTex	tex;
};

//Optimizacion: es importante que esta estructura mida bytes multiplo de 4 bytes.
struct NBVerticeGL2 : public NBVerticeGL {
	//coordenadas de segunda textura
	STCoordTex	tex2;
};

//Optimizacion: es importante que esta estructura mida bytes multiplo de 4 bytes.
struct NBVerticeGL3 : public NBVerticeGL2 {
	//coordenadas de tercera textura
	STCoordTex	tex3;
};

// Administrador de vertices/indices (VAO)

struct STVerticesDatos {
	//Arreglo de vertices
	BYTE*	verticesDatos;
	UI32	verticesCantBytes;
	#ifdef CONFIG_NB_GESTOR_ESCENAS_MODELOS_MEDIANTE_INDICES
	//Arreglo de indices
	BYTE*	indicesDatos;
	UI32	indicesCantBytes;
	#endif
};

typedef void (*PTRfuncObtenerDatosVao)(void* param, const SI32 iVao, STVerticesDatos* guardarDatosEn);

#endif
