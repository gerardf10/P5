#include <iostream>
#include <math.h>
#include "instrument_dumb.h"
#include "keyvalue.h"

#include <stdlib.h>

using namespace upc;
using namespace std;

InstrumentDumb::InstrumentDumb(const std::string &param) 
  : adsr(SamplingRate, param) {
  bActive = false;
  x.resize(BSIZE);

  /*
    You can use the class keyvalue to parse "param" and configure your instrument.
    Take a Look at keyvalue.h    
  */
  KeyValue kv(param);
  int N;

  if (!kv.to_int("N",N))
    N = 40; //default value
  
  //Create a tbl with one period of a sinusoidal wave
  tbl.resize(N);
  float phase = 0, step = 2 * 3.1415926 /(float) N;
  index = 0;
  for (int i=0; i < N ; ++i) {
    tbl[i] = sin(phase);
    phase += step;
  }
}


void InstrumentDumb::command(long cmd, long note, long vel) {
  if (cmd == 9) {		//'Key' pressed: attack begins
    bActive = true;
    adsr.start();
    index = 0;
	A = vel / 127.;
      float f0 = 440*pow(2, (note - 69.)/12.);
    phase = 2*3.14159*f0/SamplingRate;
    phase_act = 0;
  }
  else if (cmd == 8) {	//'Key' released: sustain ends, release begins
    adsr.stop();
  }
  else if (cmd == 0) {	//Sound extinguished without waiting for release to end
    adsr.end();
  }
}


const vector<float> & InstrumentDumb::synthesize() {
  if (!adsr.active()) {
    x.assign(x.size(), 0);
    bActive = false;
    return x;
  }
  else if (!bActive)
    return x;

  for (unsigned int i=0; i<x.size(); ++i) {
      phase_act += this->phase;
    while(phase_act > 2*3.14159){
      phase_act -= 2*3.14159;
    }
    index = (int) phase_act/(2*3.14159)*tbl.size();
    x[i] = A * tbl[index];
    if (index == tbl.size())
      index = 0;
  }
  adsr(x); //apply envelope to x and update internal status of ADSR

  return x;
}
