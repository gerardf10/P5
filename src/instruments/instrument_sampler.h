#pragma once
#include "instrument.h"
#include <vector>
#include <string>

namespace upc {
class InstrumentSampler : public Instrument {
public:
    InstrumentSampler(const std::string &params);
    void command(long cmd, long note, long vel) override;
    const std::vector<float> & synthesize() override;

private:
    std::vector<double> buffer;  // Loaded WAV samples
    size_t readIndex;            // Current playback index
    double A;                    // Amplitude from MIDI velocity
};
}
