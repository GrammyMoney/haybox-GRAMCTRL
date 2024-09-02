#ifndef _MODES_MELEE21BUTTON_HPP
#define _MODES_MELEE21BUTTON_HPP

#include "core/ControllerMode.hpp"
#include "core/socd.hpp"
#include "core/state.hpp"

typedef struct {
    bool crouch_walk_os = false;
} Melee21ButtonOptions;

class Melee21Button : public ControllerMode {
  public:
    Melee21Button(socd::SocdType socd_type, Melee21ButtonOptions options = {});

  protected:
    void UpdateDigitalOutputs(InputState &inputs, OutputState &outputs);
    void UpdateAnalogOutputs(InputState &inputs, OutputState &outputs);

  private:
    Melee21ButtonOptions _options;
    bool _horizontal_socd;

    void HandleSocd(InputState &inputs);
};

#endif
