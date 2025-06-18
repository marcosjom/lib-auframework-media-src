//
//  AUEscenaContentI.h
//  lib-auframework-media
//
//  Created by Marcos Ortega on 28/12/18.
//

#ifndef AUEscenaContentI_h
#define AUEscenaContentI_h

#include "AUAnimadorObjetoEscena.h"

class AUEscenaContentI {
	public:
		virtual bool	cargarMasContenido() = 0;
		virtual void	organizarContenido(const float anchoParaContenido, const float altoVisible, const bool notificarCambioAltura, AUAnimadorObjetoEscena* animator, const float secsAnim) = 0;
		virtual void	organizarSegunAltoVisible(const float yMaxVisible, const float yMinVisible, const float yMaxUsable, const float yMinUsable) = 0;
		//
		virtual bool	animando() = 0;
		virtual void	animarSalida(const float yMaxVisible, const float yMinVisible) = 0;
		virtual void	animarEntrada(const float yMaxVisible, const float yMinVisible) = 0;
};

#endif /* AUEscenaContentI_h */
