#include "event.h"
#include "output.h"
#include "../programa.h"
#include "../hardware/Serial.h"
#include "../hardware/ENCODER.h"

static int evAnterior = EV_NOEVENT;
static int ev = EV_NOEVENT;

void eventInit(void) {
    iniciaTeclado();
    iniciaEncoder();
}

unsigned int eventRead(void) {
    ev = EV_NOEVENT;
    int sensor_Level    = getSensorLevel();
    int leituraEncoder  = leEncoder();

    if(leituraEncoder == 1) {
        ev = EV_RIGHT;
    }
    if(leituraEncoder == -1) {
        ev = EV_LEFT;
    }

    if(lerBotaoMeio()) {
        ev = EV_ENTER;
    }

    if(lerBotaoCima()) {
        ev = EV_UP;
    }

    if(lerBotaoBaixo()) {
        ev = EV_DOWN;
    }

    if(leBotaoEncoder()) {
        ev = EV_CANCEL;
    }

    if(ev == EV_NOEVENT &&
       (sensor_Level < getAlarmLevel_L() || sensor_Level > getAlarmLevel_H())) {
        ev = EV_OUT_OF_RANGE;
    }

    if (ev == EV_NOEVENT) {
        evAnterior = EV_NOEVENT;
        return EV_NOEVENT;
    }

    if (ev == evAnterior) {
        return EV_NOEVENT;
    }

    evAnterior = ev;
    LCD_clear();
    return ev;
}
