# esphome configuration
some of my esphome configurations.

## tof people counter
this is an esphome version of a people counter sensor based on a vl53l1x time of flight sennsor. since the prrople counting only works when passing the door openinng complety, i also added kind of a light barrier sensor, that senses if someone is standing in the door openneing.

all the credit goes to @Lyr3x on whose [Roode v1.0](https://github.com/Lyr3x/Roode/tree/v1.0) project this is based.
@lyr3x reliable „smart home people counter“ uses a modified [STM32duino_Proximity_Gesture](https://github.com/stm32duino/Proximity_Gesture) library and an older version of the [vl53l1x-st-api-arduino](https://github.com/pololu/vl53l1x-st-api-arduino) library (vl53l1_api). both were first used in a proof of concept by „kabron“ described [here](https://community.st.com/s/question/0D50X0000A7VWoMSQW/is-vl53l1x-people-counting-source-code-available). 

i copied those libraries into the lib folder of the target folder, into which esphome/platformio is compiling it’s code.

i did not find a better way to include the libraries than putting them into the lib folder, but putting them there makes esphome find all neccessary files and compiles fine

if you rename the yaml configuration file, esphome will use another target folder and you’ll need to copy the ```lib``` folder into the new folder. you’ll also need to copy ```tof_people_counter/tof_vl53l1x_api.h``` into your esphome configuration folder.

## tof distance/toilet sensor
this is based on @jardous esphome [tof_vl53l1x](https://github.com/jardous/tof_vl53l1x) implementation. since i get timeout reading from time to time, i added a timeout sensor and a way to re-initialise the vl53l1x if too many timeouts happen.