#pragma once
#include "instrument.h"
#include <vector>
#include "envelope_adsr.h"
namespace upc {
class InstrumentSin : public Instrument {
public:
    InstrumentSin(const std::string &params);
    void command(long cmd, long note, long vel) override;
    const std::vector<float> & synthesize() override;
    EnvelopeADSR adsr;
private:
    std::vector<double> tbl;      // One-cycle sine table
    double phaseIndex;            // Current real-valued phase index
    double step;                  // Step size (frequency)
    double A;                     // Amplitude (from MIDI velocity)
};
}
