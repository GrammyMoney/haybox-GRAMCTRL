#ifndef _MODES_FGCMODEWASD_HPP
#define _MODES_FGCMODEWASD_HPP

#include "core/ControllerMode.hpp"
#include "core/socd.hpp"
#include "core/state.hpp"

class FgcModeWASD : public ControllerMode {
  public:
    FgcModeWASD(socd::SocdType horizontal_socd, socd::SocdType vertical_socd);

  private:
    void UpdateDigitalOutputs(InputState &inputs, OutputState &outputs);
    void UpdateAnalogOutputs(InputState &inputs, OutputState &outputs);
};

#endif
