#ifndef CLIPPING_H
#define CLIPPING_H

#include <vector>
#include <string>
#include "effect.h"

namespace upc {

class Clipping: public upc::Effect {
public:
    Clipping(const std::string &param = "");
    void command(unsigned int comm);
    void operator()(std::vector<float> &samples);

private:
    float threshold; // Llindar (% del valor màxim absolut)
};

} // namespace upc

#endif // CLIPPING_H