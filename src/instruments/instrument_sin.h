#pragma once
#include "instrument.h"
#include <vector>
#include "envelope_adsr.h"
#include "keyvalue.h"
namespace upc {
class InstrumentSin : public Instrument {
    unsigned int id;
    EnvelopeADSR adsr;
    float A;                     // Amplitude (from MIDI velocity)
    std::vector<double> tbl;      // One-cycle sine table
public:
    InstrumentSin(const std::string &params);
    void command(long cmd, long note, long vel=1);
    const std::vector<float> & synthesize();
    float step;                  // Step size (frequency)
    float phaseIndex;            // Current real-valued phase index
};
}