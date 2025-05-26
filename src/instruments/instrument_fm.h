#pragma once
#include "instrument.h"
#include <vector>
#include <string>
#include "keyvalue.h"
#include "envelope_adsr.h"
namespace upc {
class InstrumentFM : public Instrument {
public:
    InstrumentFM(const std::string &params);
    void command(long cmd, long note, long vel) override;
    const std::vector<float> & synthesize() override;
    EnvelopeADSR adsr;            // ADSR envelope for amplitude control
private:
    std::vector<double> tbl;      // One-cycle sine table (carrier)
    double phaseC;                // Carrier phase
    double phaseM;                // Modulator phase
    double f0;                    // Base frequency of the note
    double modFreq;               // Modulator frequency
    float I;                     // Depth in semitones
    double A;                     // Amplitude (from velocity)
    int N1, N2;              // Number of samples for carrier and modulator
};
}
