#ifndef VAR_H
    #define	VAR_H

    void varInit(void);

    char getState(void);
    void setState(char newState);

    int getTime(void);
    void setTime(int newTime);


    int getAlarmLevel_H(void);
    void setAlarmLevel_H(int);
    int getAlarmLevel_L(void);
    void setAlarmLevel_L(int);

    int getSensorLevel(void);
    void setSensorLevel(int newSensorLevel);
    float getSensorLevel_V(void);


    char getLanguage(void);
    void setLanguage(char newLanguage);

    int getSeconds(void);
    void setSeconds(int newSeconds);
    int getMinutes(void);
    void setMinutes(int newMinutes);
    int getHours(void);
    void setHours(int newHours);

    void ajustaTempo(int*, int*, int*);

    /* Persistência em EEPROM: chamadas explicitamente nos pontos
     * de confirmação da máquina de estados (EV_ENTER / EV_UP relevantes) */
    void salvarIdiomaEEPROM(void);
    void salvarAlarmesEEPROM(void);
    void salvarHorarioEEPROM(void);
#endif
