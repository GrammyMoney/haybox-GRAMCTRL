#ifndef _MODES_MELEE21BUTTONWASD_HPP
#define _MODES_MELEE21BUTTONWASD_HPP

#include "core/ControllerMode.hpp"
#include "core/socd.hpp"
#include "core/state.hpp"

typedef struct {
    bool crouch_walk_os = false;
} Melee21ButtonWASDOptions;

class Melee21ButtonWASD : public ControllerMode {
  public:
    Melee21ButtonWASD(socd::SocdType socd_type, Melee21ButtonWASDOptions options = {});

  protected:
    void UpdateDigitalOutputs(InputState &inputs, OutputState &outputs);
    void UpdateAnalogOutputs(InputState &inputs, OutputState &outputs);

  private:
    Melee21ButtonWASDOptions _options;
    bool _horizontal_socd;

    void HandleSocd(InputState &inputs);
};

#endif
