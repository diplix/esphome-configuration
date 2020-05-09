#include "esphome.h"







static unsigned long loopCounter = 0;
static unsigned long loopCounterLast = 0;
static unsigned long loopCounterMax = 1;

class LoadCalculator : public PollingComponent, public Sensor {
 public:
  // constructor
  LoadCalculator() : PollingComponent(30000) {} // polling every 30s

  void update() override {
    if (loopCounter < 5000) { 
      loopCounterLast = loopCounter;
      loopCounter = 0;
      if (loopCounterLast > loopCounterMax)
        loopCounterMax = loopCounterLast;
      int load = (100 - (100 * loopCounterLast / loopCounterMax));
      /* if uptime > 2 min */
      if (id(uptime_sensor).state > 60) {
        publish_state(load);
      }
    }
    else {
      loopCounter = 0;
    }
  }
};

/*
esphome:
  # ...
  includes:
    - tof/loopcounter.h
  on_loop:
    then:
      - lambda: loopCounter++;

sensor:
  - platform: custom
    lambda: |-
      auto load_sensor = new LoadCalculator();
      App.register_component(load_sensor);
      return {load_sensor};
    sensors:
      - name: Load
        id: load
        accuracy_decimals: 0
        unit_of_measurement: "%"
        internal: false


*/