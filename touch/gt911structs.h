#ifndef GT911STRUCTS_H_INCLUDED
#define GT911STRUCTS_H_INCLUDED

#include "hardware.h"

typedef struct {
  // 0x8140-0x814A 
  char productId[4];
  uint16_t fwId;
  uint16_t xResolution;
  uint16_t yResolution;
  uint8_t vendorId;
} GTInfo;

typedef struct {
  // 0x814F-0x8156, ... 0x8176 (5 points) 
  uint8_t trackId;
  uint16_t x;
  uint16_t y;
  uint16_t area;
  uint8_t reserved;
} GTPoint;

typedef struct {
  uint8_t touch; // Threshold of touch grow out of nothing
  uint8_t leave; // Threshold of touch decrease to nothing
} GTLevelConfig;

typedef struct {
  uint8_t txGain;
  uint8_t rxGain;
  uint8_t dumpShift;
  GTLevelConfig level;
  uint8_t control;     //Pen mode escape time out period (Unit: Sec)
} GTStylusConfig;

typedef struct {
  uint16_t hoppingBitFreq;
  uint8_t hoppingFactor;
} GTFreqHoppingConfig;

typedef struct {
  // Key position: 0-255 valid 
  // 0 means no touch, it means independent touch key when 4 of the keys are 8 multiples
  uint8_t pos1;
  uint8_t pos2;
  uint8_t pos3;
  uint8_t pos4;
  uint8_t area;
  GTLevelConfig level;
  uint8_t sens12;
  uint8_t sens34;
  uint8_t restrain;
} GTKeyConfig;

typedef struct {
  // start at 0x8047
  uint8_t configVersion;
  uint16_t xResolution;
  uint16_t yResolution;  
  // 0x804C
  uint8_t touchNumber;   // 3:0 Touch No.: 1~10
  
  // 7:6 Reserved, 5:4 Stretch rank, 3 X2Y, 2 Sito
  // 1:0 INT trig method: 00-rising, 01-falling, 02-low level, 03-high level enquiry
  uint8_t moduleSwitch1;   
  uint8_t moduleSwitch2;  // bit0 TouchKey
  uint8_t shakeCount;     // 3:0 Finger shake count
  // 0x8050
  // 7:6 First filter, 5:0 Normal filter (filtering value of original coordinate window, coefficiency is 1)
  uint8_t filter;
  uint8_t largeTouch;
  uint8_t noiseReduction;
  GTLevelConfig screenLevel;

  uint8_t lowPowerControl;  // Time to low power consumption (0~15s)
  uint8_t refreshRate;      // Coordinate report rate (Cycle: 5+N ms)
  uint8_t xThreshold; //res
  uint8_t yThreshold; //res
  uint8_t xSpeedLimit; //res
  uint8_t ySpeedLimit; //res
  uint8_t vSpace; // 4bit top/bottom  (coefficient 32)
  uint8_t hSpace; // 4bit left/right 
  //0x805D-0x8061
  uint8_t stretchRate; //Level of weak stretch (Strtch X/16 Pitch)
  uint8_t stretchR0;   // Interval 1 coefficient
  uint8_t stretchR1;   // Interval 2 coefficient
  uint8_t stretchR2;   // Interval 3 coefficient
  uint8_t stretchRM;   // All intervals base number
   
  uint8_t drvGroupANum;
  uint8_t drvGroupBNum;
  uint8_t sensorNum;
  uint8_t freqAFactor;
  uint8_t freqBFactor;
  // 0x8067
  uint16_t pannelBitFreq;      //Baseband of Driver group A\B (1526HZ<baseband<14600Hz)
  uint16_t pannelSensorTime;  //res
  uint8_t pannelTxGain;
  uint8_t pannelRxGain;
  uint8_t pannelDumpShift;
  uint8_t drvFrameControl;
  // 0x806F - 0x8071
  uint8_t NC_2[3];
  GTStylusConfig stylusConfig;
  // 0x8078-0x8079
  uint8_t NC_3[2];
  uint8_t freqHoppingStart; // Frequency hopping start frequency (Unit: 2KHz, 50 means 100KHz )
  uint8_t freqHoppingEnd;   // Frequency hopping stop frequency (Unit: 2KHz, 150 means 300KHz )
  uint8_t noiseDetectTims; 
  uint8_t hoppingFlag;
  uint8_t hoppingThreshold;  

  uint8_t noiseThreshold;  
  uint8_t NC_4[2];
  // 0x8082
  GTFreqHoppingConfig hoppingSegments[5];
  // 0x8091
  uint8_t NC_5[2];
  GTKeyConfig keys;
} GTConfig;
#endif /* GT911STRUCTS_H_INCLUDED */
