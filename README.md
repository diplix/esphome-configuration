# esphome configuration
 my esphome configurations

# tof people counter
this is a esphome version of a people counter sensor based on a vl53l1x time of flight sennsor.

all the credit goes to @Lyr3x on whose [Roode v1.0](https://github.com/Lyr3x/Roode/tree/v1.0) project this is based.
@lyr3x reliable „smart home people counter“ uses a modified [STM32duino_Proximity_Gesture](https://github.com/stm32duino/Proximity_Gesture) library and an older version of the [vl53l1x-st-api-arduino](https://github.com/pololu/vl53l1x-st-api-arduino) library (vl53l1_api). both were first used in a proof of concept by „kabron“ described [here](https://community.st.com/s/question/0D50X0000A7VWoMSQW/is-vl53l1x-people-counting-source-code-available).

i did not find a better way to include the libraries than putting them in the folder into which eshome compiles. putting them into the lib folder esphome finnds all the neccessary files.
