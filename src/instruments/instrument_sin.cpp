#include "instrument_sin.h"
#include <cmath>
#include <algorithm>

using namespace std;
using namespace upc;

InstrumentSin::InstrumentSin(const std::string &params)
: adsr(SamplingRate, params),phaseIndex(0.0), A(0.0) {
    // Create sine table of N samples
    const int N = 1024;
    tbl.resize(N);
    double phase = 0.0;
    double stepTbl = 2.0 * 3.1415926 / N;
    for (int i = 0; i < N; ++i) {
        tbl[i] = sin(phase);
        phase += stepTbl;
    }
    // Prepare output buffer
    x.resize(BSIZE);
    bActive = false;
}



void InstrumentSin::command(long cmd, long note, long vel) {
    if (cmd == 9) {    // Note On
        bActive = true;
        adsr.start();
        // Compute step based on MIDI note
        double freq = pow(2.0, (note - 69) / 12.0) * 440.0;
        step = freq * tbl.size() / SamplingRate;
        A = vel / 127.0;
    }
    else if (cmd == 8) { // Note Off
        adsr.stop();
        bActive = false;
    }
    else if (cmd == 0) { // All notes off
        bActive = false;
        adsr.end();
    }
}
// En instrument_sin.cpp

const std::vector<float>& InstrumentSin::synthesize() {
    // Si la envolvente ya ha terminado, silencio total
    if (!adsr.active()) {
      x.assign(x.size(), 0.0f);
      bActive = false;
      return x;
    }
    // Si la voz no está activa, devolvemos silencio previo
    else if (!bActive) {
      return x;
    }
  
    // Rellenar un buffer ciclo a ciclo con interpolación lineal
    for (unsigned int i = 0; i < x.size(); ++i) {
      int idx = static_cast<int>(phaseIndex);
      float frac = phaseIndex - idx;
      int idx2 = (idx + 1) % tbl.size();
      float sample = (1.0f - frac) * tbl[idx] + frac * tbl[idx2];
      x[i] = A * sample;
      phaseIndex += step;
      if (phaseIndex >= tbl.size())
        phaseIndex -= tbl.size();
    }
  
    // Aplica el ADSR a todo el buffer y actualiza su estado
    adsr(x);
  
    return x;
  }
  