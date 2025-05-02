#include "instrument_fm.h"
#include <cmath>

using namespace std;
using namespace upc;

InstrumentFM::InstrumentFM(const std::string &params)
: adsr(SamplingRate, params),phaseC(0.0), phaseM(0.0), f0(0.0),
  modFreq(0.0), I(0.0), A(0.0),  N1(1), N2(1)  {
    // Parse parameters I, N1, N2 from params string
    size_t pos;
    if ((pos = params.find("I=")) != std::string::npos) {
        I = std::stod(params.substr(pos + 2));
    }
    size_t p;
    if ((p = params.find("N1=")) != std::string::npos)
        N1 = std::stoi(params.substr(p+3));
    if ((p = params.find("N2=")) != std::string::npos)
        N2 = std::stoi(params.substr(p+3));
    // Create carrier sine table of N samples
    if (N1 < 1) N1 = 1;
    if (N2 < 1) N2 = 1;
    // ... (build operator sine table code, omitted for brevity)
    // Initialize output buffer
    const int N = 1024;
    tbl.resize(N);
    double phase = 0.0;
    double stepTbl = 2.0 * 3.1415926 / N;
    for (int i = 0; i < N; ++i) {
        tbl[i] = std::sin(phase);
        phase += stepTbl;
    }
    x.resize(BSIZE);
    bActive = false;
}


void InstrumentFM::command(long cmd, long note, long vel) {
    if (cmd == 9) {       // Note On
        bActive = true;
        adsr.start();
        // Compute base frequency f0 and modulator frequency based on note and params
        f0 = pow(2.0, (note - 69) / 12.0) * 440.0;
        double localFreq = pow(2.0, I / 12.0) * f0;
        modFreq = localFreq;
        phaseC = 0.0;
        phaseM = 0.0;
        A = vel / 127.0;
    }
    else if (cmd == 8) {  // Note Off
        bActive = false;
        adsr.stop();
    }
    else if (cmd == 0) {  // All notes off
        bActive = false;
        adsr.end();
    }
}
// En instrument_fm.cpp

const std::vector<float>& InstrumentFM::synthesize() {
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
  
    // Variables locales de fase y pasos
    double localPhaseC = phaseC;
    double localPhaseM = phaseM;
    double stepC = f0 * tbl.size() / SamplingRate;
    double stepM = modFreq * tbl.size() / SamplingRate;
  
    // Síntesis FM por muestra
    for (unsigned int i = 0; i < x.size(); ++i) {
      // Interpolación del modulador
      int m1 = static_cast<int>(localPhaseM);
      float mf = localPhaseM - m1;
      int m2 = (m1 + 1) % tbl.size();
      float modSample = (1.0f - mf) * tbl[m1] + mf * tbl[m2];
  
      // Portadora modulada
      double phaseWithMod = localPhaseC + I * modSample;
      int c1 = static_cast<int>(phaseWithMod) % tbl.size();
      float cf = phaseWithMod - static_cast<int>(phaseWithMod);
      int c2 = (c1 + 1) % tbl.size();
      float carSample = (1.0f - cf) * tbl[c1] + cf * tbl[c2];
  
      x[i] = static_cast<float>(A * carSample);
  
      // Avanzar fases
      localPhaseC += stepC;
      if (localPhaseC >= tbl.size()) localPhaseC -= tbl.size();
      localPhaseM += stepM;
      if (localPhaseM >= tbl.size()) localPhaseM -= tbl.size();
    }
  
    // Guardar fases para el siguiente bloque
    phaseC = localPhaseC;
    phaseM = localPhaseM;
  
    // Aplica el ADSR a todo el buffer y actualiza su estado
    adsr(x);
  
    return x;
  }
  