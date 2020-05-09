#include "esphome.h"

#include <Wire.h>
//#include "vl53l1_api.h"
#include <vl53l1_api.h>
#include <tof_gestures.h>
#include <tof_gestures_DIRSWIPE_1.h>

/* todo: make configurable */
#define XSHUT   14 // D15 // D3 // 5 // D3 D5
//#define INT     2 // D2 // D4 // 17 // D7

#define dev1_sel  digitalWrite(XSHUT, HIGH);
#define dev1_desel  digitalWrite(XSHUT, LOW);

VL53L1_Dev_t    VL53L1_sensor;
VL53L1_DEV      Dev = &VL53L1_sensor;

VL53L1_UserRoi_t  roiConfig1 = { 10, 15, 15, 0 }; //TopLeftX, TopLeftY, BotRightX, BotRightY
VL53L1_UserRoi_t  roiConfig2 = { 0, 15, 5, 0 }; //TopLeftX, TopLeftY, BotRightX, BotRightY

//VL53L1_UserRoi_t  roiConfig1 = { 12, 15, 15, 0 };
//VL53L1_UserRoi_t  roiConfig2 = { 0, 15, 3, 0 };

Gesture_DIRSWIPE_1_Data_t gestureDirSwipeData;
int VL53L1_status, i, distance[2] = { 0, 0 };
int left = 0, right = 0, cnt = 0, oldcnt;
//long timeMark = 0, DisplayUpdateTime = 0;
uint8_t peopleCount = 0; //default state: nobody is inside the room
static int resetCounter = 0;
boolean lastTrippedState = 0;

//static int num_timeouts = 0;
double people, distance_avg;

//class MyCustomSensor : public PollingComponent, public Sensor {
//class MyCustomSensor : public Component, public Sensor {
class PeopleCountSensor : public Component, public Sensor {
 public:
  // constructor
  //MyCustomSensor() : PollingComponent(15000) {} // polling every 15s
  //MyCustomSensor() : PollingComponent(3000) {} // polling every 3s
  Sensor *people_sensor = new Sensor();
  Sensor *distance_sensor = new Sensor();

  void setup() override {
    // This will be called by App.setup()
    Wire.begin();
    Wire.setClock(400000);
    //Serial.begin(115200);
  
    pinMode(XSHUT, OUTPUT);
  
    delay(100);
  
    dev1_sel
  
    Dev->I2cDevAddr = 0x52;
  
    //Serial.print(F("\n\rDevice data  "));
    ESP_LOGD("VL53L1X custom sensor", "Getting Device data");
    uint16_t wordData;
    VL53L1_RdWord(Dev, 0x010F, &wordData);
    //Serial.printf("DevAddr: 0x%X VL53L1X: 0x%X\n\r", Dev->I2cDevAddr, wordData);
    ESP_LOGD("VL53L1X custom sensor", "DevAddr: 0x%X VL53L1X: 0x%X", Dev->I2cDevAddr, wordData);
    
    delay(1000);
  
    // with modified DIRDWIPE lib
    tof_gestures_initDIRSWIPE_1(800, 0, 1000, false, &gestureDirSwipeData);
    // with NOT modified DIRDWIPE lib
    //  tof_gestures_initDIRSWIPE_1(800, 0, 1000, &gestureDirSwipeData);
  
    VL53L1_software_reset(Dev);
      
    //Serial.println(F("Autonomous Ranging Test"));
    ESP_LOGD("VL53L1X custom sensor", "Autonomous Ranging Test");
    VL53L1_status += VL53L1_WaitDeviceBooted(Dev);
    VL53L1_status += VL53L1_DataInit(Dev);
    VL53L1_status += VL53L1_StaticInit(Dev);
    VL53L1_status += VL53L1_SetDistanceMode(Dev, VL53L1_DISTANCEMODE_LONG);
    //VL53L1_status += VL53L1_SetDistanceMode(Dev, VL53L1_DISTANCEMODE_SHORT);
    VL53L1_status += VL53L1_SetMeasurementTimingBudgetMicroSeconds(Dev, 10000);  // 73Hz
    VL53L1_status += VL53L1_SetInterMeasurementPeriodMilliSeconds(Dev, 15); 
    if (VL53L1_status) {
      //Serial.printf("StartMeasurement failed status: %d\n\r", VL53L1_status);
      ESP_LOGE("VL53L1X custom sensor", "StartMeasurement failed status: %d", VL53L1_status);
      }
    
    ESP_LOGI("VL53L1X custom sensor", "Starting measurements");
    VL53L1_StartMeasurement(Dev);


    /*
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
    */
  }

  //void update() override {
  void loop() override {

    static VL53L1_RangingMeasurementData_t RangingData;
    int gesture_code;
  
    VL53L1_status = VL53L1_SetUserROI(Dev, &roiConfig1);
    VL53L1_status = VL53L1_WaitMeasurementDataReady(Dev);
    if (!VL53L1_status) VL53L1_status = VL53L1_GetRangingMeasurementData(Dev, &RangingData);  //4mS
    VL53L1_clear_interrupt_and_enable_next_range(Dev, VL53L1_DEVICEMEASUREMENTMODE_SINGLESHOT);  //2mS
    if (VL53L1_status == 0) {distance[0] = RangingData.RangeMilliMeter;}
  
    VL53L1_status = VL53L1_SetUserROI(Dev, &roiConfig2);
    VL53L1_status = VL53L1_WaitMeasurementDataReady(Dev);
    if (!VL53L1_status) VL53L1_status = VL53L1_GetRangingMeasurementData(Dev, &RangingData);  //4mS
    VL53L1_clear_interrupt_and_enable_next_range(Dev, VL53L1_DEVICEMEASUREMENTMODE_SINGLESHOT);  //2mS
    if (VL53L1_status == 0) {distance[1] = RangingData.RangeMilliMeter;}
  
    gesture_code = tof_gestures_detectDIRSWIPE_1(distance[0], distance[1], &gestureDirSwipeData); //0mS
    //Serial.printf("%d,%d,%d\n\r", distance[0], distance[1], cnt);
    if (gesture_code != 0) {
      //Serial.printf("gesture code: %d\n\r", gesture_code);
      ESP_LOGD("VL53L1X custom sensor", "Gesture code: %d", gesture_code);
    }

  switch (gesture_code)
  {
    case GESTURES_SWIPE_LEFT_RIGHT:
      if (cnt > 0)
        cnt--;
      right = 1;
      dispUpdate();
      right = 0;
      break;
    case GESTURES_SWIPE_RIGHT_LEFT:
      cnt++;
      left = 1;
      dispUpdate();
      left = 0;
      break;
    default:
      break;
  }
  
  if (resetCounter == 1) { 
    resetCounter = 0;
    sendCounter(-1); 
  }
    /*
    uint16_t mm = tof_sensor.read();
    
    if (!tof_sensor.timeoutOccurred()) {
      publish_state(mm);
      num_timeouts = 0;
    } else {
      ESP_LOGE("VL53L1X custom sensor", "Timeout during read().");
      num_timeouts += 1;
    }
    */
  }

  inline void dispUpdate() {  // 33mS
    // left = rein
    if (left) {
      //Serial.println("--->");
      ESP_LOGD("VL53L1X custom sensor", "--->");
      sendCounter(1);
    }
    // right = raus
    if (right) { 
      //Serial.println("<---");
      ESP_LOGD("VL53L1X custom sensor", "<---");
      sendCounter(0);
    }
    //Serial.println(cnt);
    ESP_LOGD("VL53L1X custom sensor", "Count: %d", cnt);
  }

  void sendCounter(int inout)
  {
      if (inout == 1) { peopleCount++; }
      else if (inout == 0)
      {
          if (peopleCount > 0) { peopleCount--; }
      }
      else if (inout == -1)
      {
          peopleCount = 0;
      }
      //client.publish(counter_topic, String(peopleCount).c_str(), true);
      ESP_LOGI("VL53L1X custom sensor", "Sending people count: %d", peopleCount);
      people_sensor->publish_state(peopleCount);
      //distance_sensor->publish_state(distance_avg);
  
      //Serial.print("PeopleCounter: ");
      //Serial.println(peopleCount);
  }


};

/*
sensor:
- platform: custom
  lambda: |-
    auto energy_meter = new EnergyMeterSensor();
    App.register_component(energy_meter);
    return {energy_meter->power_sensor, energy_meter->elapsedkWh_sensor};
  sensors:
  - name: "Bieżące zużycie energii"
    unit_of_measurement: W
    accuracy_decimals: 2
  - name: "Łączne zużycie energii"
    unit_of_measurement: kWh
    accuracy_decimals: 2

*/