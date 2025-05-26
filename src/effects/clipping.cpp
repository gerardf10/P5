#include "clipping.h"
#include "keyvalue.h"
#include <math.h>
#include <cstdio>
#include <stdlib.h>
#include <iostream>

using namespace upc;
using namespace std;

Clipping::Clipping(const std::string &param) {
    KeyValue args(param);
    if (!args.to_float("T", threshold))
        threshold = 70.0f;  // Valor per defecte
}

void Clipping::command(unsigned int) {
    // No s'utilitza cap comanda en aquesta implementació
}

void Clipping::operator()(std::vector<float> &samples) {
    if (samples.empty()) return;

    float peak = 0.0f;
    for (float val : samples) {
        float abs_val = std::abs(val);
        if (abs_val > peak)
            peak = abs_val;
    }

    if (peak == 0.0f) return;

    float limit = (threshold * 0.01f) * peak;

    for (float &val : samples) {
        if (val > limit)
            val = limit;
        else if (val < -limit)
            val = -limit;
    }
}