#ifndef AUEscenaContenedorLimitado_H_INCLUIDO
#define AUEscenaContenedorLimitado_H_INCLUIDO

#include "AUEscenaContenedor.h"

class AUEscenaContenedorLimitado : public AUEscenaContenedor {
	public:
		AUEscenaContenedorLimitado();
		AUEscenaContenedorLimitado(AUEscenaContenedorLimitado* otraInstancia);
		virtual ~AUEscenaContenedorLimitado();
		//
		NBCajaAABB			limites() const;
		void				establecerLimites(const float xMin, const float xMax, const float yMin, const float yMax);
		void				establecerLimites(const NBCajaAABB limites);
		//
		virtual	NBCajaAABB	cajaAABBLocalCalculada();
		virtual void		actualizarArbolMatricesEscena(STNBSceneModelsResult* dst, const NBPropRenderizado &propsRenderizado, const NBPropHeredadasModelos &propsHeredadas);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	private:
		NBCajaAABB		_limites;
};
 
#endif
