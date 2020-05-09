#include "esphome.h"

#include <Wire.h>
#include <VL53L1X.h>

VL53L1X tof_sensor;

static int num_timeouts = 0;

class MyCustomSensor : public PollingComponent, public Sensor {
 public:
  // constructor
  //MyCustomSensor() : PollingComponent(15000) {} // polling every 15s
  MyCustomSensor() : PollingComponent(3000) {} // polling every 3s

  void setup() override {
    // This will be called by App.setup()
    Wire.begin();
    Wire.setClock(400000); // use 400 kHz I2C

    tof_sensor.setTimeout(500);
    tof_sensor.setAddress(0x29);
    if (!tof_sensor.init()) {
      ESP_LOGE("VL53L1X custom sensor", "Failed to detect and initialize sensor!");
      while (1);
    }

    tof_sensor.setDistanceMode(VL53L1X::Long);
    tof_sensor.setMeasurementTimingBudget(50000);

    tof_sensor.startContinuous(50);
  }

  void update() override {
    uint16_t mm = tof_sensor.read();
    
    if (!tof_sensor.timeoutOccurred()) {
      publish_state(mm);
      num_timeouts = 0;
    } else {
      ESP_LOGE("VL53L1X custom sensor", "Timeout during read().");
      num_timeouts += 1;
    }
    if (num_timeouts > 80 ) {
      num_timeouts = 0;
      ESP_LOGE("VL53L1X custom sensor", "more than 100 timouts - trying to reset");
      reset_sensor();
    }
  }

  void reset_sensor() {
    tof_sensor.writeReg(VL53L1X::SOFT_RESET, 0x00);
    delay(100);
    tof_sensor.writeReg(VL53L1X::SOFT_RESET, 0x01);
    delay(100),

    tof_sensor.setTimeout(500);
    tof_sensor.setAddress(0x29);
    
    if (!tof_sensor.init()) {
      ESP_LOGE("VL53L1X custom sensor", "Failed to detect and initialize sensor after trying to reset sensor");
      while (1);
    }

    tof_sensor.setDistanceMode(VL53L1X::Long);
    tof_sensor.setMeasurementTimingBudget(50000);

    tof_sensor.startContinuous(50);

  }

};