#include "instrument_fm.h"
#include <cmath>
using namespace std;
using namespace upc;

InstrumentFM::InstrumentFM(const std::string &params)
: adsr(SamplingRate, params), phaseC(0), phaseM(0), f0(0), modFreq(0), I(0), A(0), N1(1), N2(1) {
    KeyValue kv(params);
    kv.to_float("I", I);
    kv.to_int("N1", N1);
    kv.to_int("N2", N2);
    if (N1 < 1) N1 = 1;
    if (N2 < 1) N2 = 1;
    // Build one-cycle sine table
    const int TABLE_SIZE = 1024;
    tbl.resize(TABLE_SIZE);
    double phase = 0.0;
    double step = 2.0 * 3.14159 / TABLE_SIZE;
    for (int i = 0; i < TABLE_SIZE; ++i) {
        tbl[i] = sin(phase);
        phase += step;
    }
    x.resize(BSIZE);
    bActive = false;
}

void InstrumentFM::command(long cmd, long note, long vel) {
    if (cmd == 9) {
        bActive = true;
        adsr.start();
        f0 = pow(2.0, (note - 69) / 12.0) * 440.0;
        // Per original formula: apply I transposition before ratio
        double localFreq = pow(2.0, I / 12.0) * f0;
        modFreq = localFreq * (double)N1 / (double)N2;
        A = vel / 127.0f;

    } else if (cmd == 8) {
        adsr.stop();
    } else if (cmd == 0) {
        adsr.end();
    }
}

const vector<float>& InstrumentFM::synthesize() {
    const double twoPi = 2.0 * 3.14159;
    if (!adsr.active()) {
        x.assign(x.size(), 0.0f);
        bActive = false;
        return x;
    }
    double stepC = twoPi * f0 / SamplingRate;
    double stepM = twoPi * modFreq / SamplingRate;

    for (size_t i = 0; i < x.size(); ++i) {
        // Modulator
        double modS = sin(phaseM);
        phaseM += stepM;
        if (phaseM >= twoPi) phaseM -= twoPi;
        // Carrier with FM depth I
        double instP = phaseC + I * modS;
        float carS = sin(instP);
        x[i] = A * carS;
        phaseC += stepC;
        if (phaseC >= twoPi) phaseC -= twoPi;
    }
    adsr(x);
    return x;
}