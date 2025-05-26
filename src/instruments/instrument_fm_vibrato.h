// New InstrumentVibrato (sinusoidal carrier with vibrato parameter)
#pragma once
#include "instrument.h"
#include <vector>
#include <string>
#include "keyvalue.h"
#include "envelope_adsr.h"

namespace upc {

class InstrumentVibrato : public Instrument {
public:
    InstrumentVibrato(const std::string &params);
    void command(long cmd, long note, long vel) override;
    const std::vector<float> & synthesize() override;
    EnvelopeADSR adsr;
private:
    float f0;
    float fm;    // Vibrato frequency in Hz
    float I;     // Vibrato depth in semitones or Hz
    float phaseC;
    float phaseV;
};

} // namespace upc
