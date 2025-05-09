#include "instrument_sampler.h"
#include <sndfile.h>  // Library for WAV
#include <iostream>

using namespace std;
using namespace upc;

InstrumentSampler::InstrumentSampler(const std::string &params)
: readIndex(0), A(0.0) {
    // Get filename from params, e.g. "file=sample.wav"
    size_t pos = params.find("file=");
    if (pos == std::string::npos) {
        cerr << "InstrumentSampler: no WAV file specified.\n";
        return;
    }
    string filename = params.substr(pos + 5);
    // Load WAV file into buffer (pseudo-code; actual error checking omitted)
    SF_INFO sfinfo;

    SNDFILE *snd = sf_open(filename.c_str(), SFM_READ, &sfinfo);
    if (!snd) {
        cerr << "InstrumentSampler: could not open " << filename << "\n";
        return;
    }
    buffer.resize(sfinfo.frames);
    sf_read_double(snd, buffer.data(), sfinfo.frames);
    sf_close(snd);
    // Initialize output buffer
    x.resize(BSIZE);
    bActive = true;
}


void InstrumentSampler::command(long cmd, long note, long vel) {
    if (cmd == 9) {      // Note On
        bActive = true;
        A = vel / 127.0;
        readIndex = 0;   // restart playback
    }
    else if (cmd == 8) { // Note Off
        bActive = false;
    }
    else if (cmd == 0) { // All notes off
        bActive = false;
    }
}

const std::vector<float> &InstrumentSampler::synthesize() {
    if (!bActive) {
        fill(x.begin(), x.end(), 0.0f);
        return x;
    }
    for (size_t i = 0; i < x.size(); ++i) {
        if (readIndex < buffer.size()) {
            x[i] = static_cast<float>(A * buffer[readIndex++]);
        } else {
            x[i] = 0.0f;
            bActive = false;  // stop when buffer is exhausted
        }
    }
    return x;
}
