#ifndef STATEMACHINE_H
    #define	STATEMACHINE_H

    //estados da maquina de Estados
    enum {
    	STATE_ALARME,
        STATE_ALARME_L,
		STATE_ALARME_H,
        STATE_HORAS,
		STATE_MUDAR_HORAS,
        STATE_IDIOMA,
		STATE_MUDAR_IDIOMA,
		STATE_SENSOR,
		STATE_OUT_OF_RANGE,
        STATE_FIM,
    };

    void iniciaSM(void);
    void smLoop(void);
#endif
