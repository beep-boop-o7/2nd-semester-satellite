#include "TMAG5170.hpp"
#include <krnl.h>

TMAG5170 myTMAG;

int magnetorquerSem;

void taskSensor(float *Bx,float *By,float *Bz) {

    k_wait(magnetorquerSem, 0);

    k_sleep(1000);  //ÆNDRE VÆRDI NÅR VI KENDER DEN

*Bx = myTMAG.readX();
*By = myTMAG.readY();
*Bz = myTMAG.readZ(true);
}


void callUpSemHallEffect(void) {
    k_signal(magnetorquerSem);
}

void startUpTMAG() {

  myTMAG.attachSPI(10, 10e6);
  myTMAG.init();
  myTMAG.setConversionAverage(CONV_AVG_32x);  //MULIGT AT ÆNDRE VÆRDIEN
  myTMAG.enableMagneticChannel(true, true, true);
  myTMAG.setMagneticRange(X_RANGE_25mT, Y_RANGE_25mT, Z_RANGE_25mT);  //MULIGT AT ÆNDRE VÆRDIEN

}