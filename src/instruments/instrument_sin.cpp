#include "instrument_sin.h"
#include <math.h>
#include <iostream>

using namespace std;
using namespace upc;

InstrumentSin::InstrumentSin(const std::string &params)
: adsr(SamplingRate, params) {
   
    x.resize(BSIZE);
    bActive = false;
    
    // Create sine table of N samples
    KeyValue kv(params); // Los parametros definidos en dumb.orc
    int N;
    if (!kv.to_int("N",N)) // Si el fichero no incluia el parametro N
      N = 40;
    tbl.resize(N);
    float phase = 0;
    float stepTbl = 2 * 3.1415926 / (float) N;
    for (int i = 0; i < N; ++i) {
        tbl[i] = sin(phase);
        phase += stepTbl;
    }
    id = 0;
}



void InstrumentSin::command(long cmd, long note, long vel) {
    if (cmd == 9) {    // Note On
        bActive = true;
        adsr.start();
        // Compute step based on MIDI note
        float freq = pow(2.0, (note - 69.0) / 12.0) * 440.0;
        step = freq * 2.0 * 3.1415926 / SamplingRate;
        A = vel / 127.0;
        phaseIndex = 0;
        id = 0;
    }
    else if (cmd == 8) { // Note Off
        adsr.stop();
    }
    else if (cmd == 0) { // All notes off
        adsr.end();
    }
}
// En instrument_sin.cpp

const std::vector<float>& InstrumentSin::synthesize() {
  // If ADSR has finished, return silence and deactivate voice
  if (!adsr.active()) {
      x.assign(x.size(), 0.0f);
      bActive = false;
      return x;
  }
  else if (!bActive)
    return x;
  // If voice not active yet, return previous buffer (silence or release tail)
  // but allow ADSR to process release phase
  
  // Fill buffer with interpolated sine samples
  for (unsigned int i = 0; i < x.size(); ++i) {
      phaseIndex += step;
      while (phaseIndex>2*3.1415926){
        phaseIndex -= 2*3.1415926;
      }
      id = (int) phaseIndex / (2*3.1415926)*tbl.size();
      x[i] = A * tbl[id];
  }

  // Apply ADSR envelope to entire buffer
  adsr(x);

  return x;
}

  