#ifndef INST_MULTINOTE
#define INST_MULTINOTE

#include <map>
#include <list>
#include <string>
#include "instrument.h"

namespace upc {
  class InstrumentMN: public upc::Instrument {
    typedef std::map<unsigned int, Instrument *> NoteInstruments;
    typedef NoteInstruments::iterator iterator;
    typedef NoteInstruments::const_iterator const_iterator;
    NoteInstruments note_instruments;
    std::string name, parameters;
  public:
    InstrumentMN(const std::string &s_name, const std::string & s_param);
    void command(long cmd, long note, long velocity=1) override;
    const std::vector<float> & synthesize() override;
    bool ready() const; // checks if initialization was OK
  };
}

#endif
