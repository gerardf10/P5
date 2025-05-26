#include "instrument_fm_vibrato.h"
#include <cmath>
using namespace std;
using namespace upc;

InstrumentVibrato::InstrumentVibrato(const string &params)
: adsr(SamplingRate, params), f0(0), fm(5.0), I(0.1), phaseC(0), phaseV(0) {
    KeyValue kv(params);
    kv.to_float("fm", fm);
    kv.to_float("I", I);
 
    x.resize(BSIZE);
    bActive = false;
}

void InstrumentVibrato::command(long cmd, long note, long vel) {
    if (cmd == 9) {
        bActive = true;
        adsr.start();
        f0 = pow(2.0, (note - 69) / 12.0) * 440.0;
        phaseC = phaseV = 0.0;
    } else if (cmd == 8) {
        adsr.stop();
    } else if (cmd == 0) {
        adsr.end();
    }
}

const vector<float>& InstrumentVibrato::synthesize() {
    const double twoPi = 2.0 * 3.14159;
    if (!adsr.active()) {
        fill(x.begin(), x.end(), 0.0f);
        bActive = false;
        return x;
    }
    double stepC = twoPi * f0 / SamplingRate;
    double stepV = twoPi * fm / SamplingRate;

    for (size_t i = 0; i < x.size(); ++i) {
        // Vibrato modulator
        double vibS = sin(phaseV);
        phaseV += stepV;
        if (phaseV >= twoPi) phaseV -= twoPi;
        // Phase modulation like FM
        double instP = phaseC + I * vibS;
        x[i] = sin(instP);
        // advance carrier phase
        phaseC += stepC;
        if (phaseC >= twoPi) phaseC -= twoPi;
    }
    adsr(x);
    return x;
}
