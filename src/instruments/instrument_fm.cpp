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
        I = std::stod(params.substr(pos));
    }
    size_t p;
    if ((p = params.find("N1=")) != std::string::npos)
        N1 = std::stoi(params.substr(p));
    if ((p = params.find("N2=")) != std::string::npos)
        N2 = std::stoi(params.substr(p));
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



const std::vector<float>& InstrumentFM::synthesize() {
    // 1) If the envelope is fully done, just return silence forever
    double twoPi = 2.0 * 3.14159265358979323846;
    if (!adsr.active()) {
        x.assign(x.size(), 0.0f);
        return x;
    }

    // 2) Always regenerate the FM voice, even during release

    double stepC = twoPi * f0      / SamplingRate;
    double stepM = twoPi * modFreq / SamplingRate;

    for (size_t i = 0; i < x.size(); ++i) {
        // modulator
        double modS = std::sin(phaseM);
        phaseM += stepM;
        if (phaseM >= twoPi) phaseM -= twoPi;

        // carrier with FM
        double instP = phaseC + I * modS;
        float carS = std::sin(instP);

        // raw amplitude × velocity
        x[i] = A * carS;

        // advance base phase
        phaseC += stepC;
        if (phaseC >= twoPi) phaseC -= twoPi;
    }

    // 3) Apply the ADSR to *this* freshly‑generated buffer
    adsr(x);

    return x;
}
