#include <Arduino.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <LoRa.h>
#include <ACS712.h>
#include <SIM800L.h>
#include <EEPROM.h>

#ifndef MODEDEBUG
#define MODEDEBUG false
#endif
#ifndef MODESERVER
#define MODESERVER true
#endif
#ifndef MODEDEBUGSIM
#define MODEDEBUGSIM false
#endif
#define BAUDRATE 9600
#define LORABUFFER 33
#define LORATIMEOUT 1000
#define LORAFREQUENCY 433E6
#define NODEOFNUMBER 2
#define NODETHISADDRESS 66
#define NODEADDRESSSTRART 65
#define LEDSENDING 5
#define LEDRECEIVE 6
#define LEDADDRESS 1
#define LEDINDICATORGET 1
#define LEDINDICATORRSSI 2
#define LEDINDICATORDISABLE 10
#define ACS712PIN 14
#define ACS712FREQUENCY 50
#define SWITCHADDRESS 0
#define SIM800LPINTX 2
#define SIM800LPINRX 3
#define SIM800LPINRS 4
#define SIM800LBUFFER 99
#define SIM800LINTERVAL 50
#define SIM800LTIMEOUT 10000
#define SIM800LSETSAPN "internet"
#define SIM800LURLHOST "https://builtcom.000webhostapp.com/posts.php"
SIM800L *SIM;
ACS712 ACS712(ACS712_05B, ACS712PIN);

void timerBegin(void) {
  #if MODESERVER
  TCCR2A = 0x00;
  TCCR2B = 0x00;
  TCNT2  = 0x00;
  OCR2A  = 0xf9;
  TCCR2A |= (1 << WGM21);
  TCCR2B |= (1 << CS21);
  TIMSK2 |= (1 << OCIE2A);
  sei();
  #endif
}

void ledIndicator(uint8_t data) {
  for (uint8_t i = 0; i < 2; ++i) {
    digitalWrite(LEDSENDING, i ? 0 : data);
    digitalWrite(LEDRECEIVE, i ? 0 : !data);
    delayMicroseconds(50);
  }
}

uint8_t ledIndicatorValue(uint8_t value) {
  if (value) {
    EEPROM.write(LEDADDRESS, value);
  }
  return EEPROM.read(LEDADDRESS);
}

void ledIndicatorInterrupt(uint16_t timer) {
  static uint16_t runTimer = 0;
  ++runTimer;
  digitalWrite(LEDSENDING, runTimer < (timer / 2) ? 1 : 0);
  digitalWrite(LEDRECEIVE, runTimer < (timer / 2) ? 1 : 0);
  runTimer = runTimer > timer ? 0 : runTimer;
}

void simIsReset(void) {
  #if MODESERVER
  pinMode(SIM800LPINRS, 1);
  for (uint8_t i = 0; i < 3; ++i) {
    digitalWrite(SIM800LPINRS, !(i % 2));
    delay(100);
  }
  #endif
}

void simBeginInit(void) {
  #if MODESERVER
  SoftwareSerial *stream = new SoftwareSerial(SIM800LPINTX, SIM800LPINRX);
  stream->begin(BAUDRATE);
  #if MODEDEBUGSIM
  SIM = new SIM800L((Stream*)stream, 0, 200, 512, (Stream*)&Serial);
  #else
  SIM = new SIM800L((Stream*)stream, 0, 200, 512);
  #endif
  #endif
}

void simConnectGprs(void) {
  #if MODESERVER
  simIsReset();
  while (!SIM->isReady());
  while (!SIM->getSignal());
  while (!SIM->getRegistrationStatus());
  while (!SIM->setupGPRS(SIM800LSETSAPN));
  while (!SIM->connectGPRS());
  #endif
}

uint16_t simPushToServer(uint8_t size, float sensor[]) {
  #if MODESERVER
  ledIndicatorValue(LEDINDICATORRSSI);
  simConnectGprs();
  ledIndicatorValue(LEDINDICATORGET);
  uint8_t lenArray = 0;
  char URL[SIM800LBUFFER];
  String host = (String)SIM800LURLHOST;
  host += "?sensor=[";
  for (lenArray = 0; lenArray < size - 1; ++lenArray) {
    host += (String)sensor[lenArray];
    host += ",";
  }
  host += (String)sensor[lenArray];
  host += "]";
  host.toCharArray(URL, SIM800LBUFFER);
  uint16_t responsStatus = SIM->doGet(URL, SIM800LTIMEOUT);
  ledIndicatorValue(LEDINDICATORDISABLE);
  return responsStatus;
  #else
  return 0;
  #endif
}

void loraBeginInit(void) {
  boolean packetCallback = false;
  if (!LoRa.begin(LORAFREQUENCY)) {
    packetCallback = true;
  }
  #if MODEDEBUG
  Serial.print(F("LORA -> Begin: "));
  Serial.print(packetCallback ? F("failed") : F("success"));
  Serial.print(F("\n"));
  #endif
  while (packetCallback);
}

void loraPacketProccess(void) {
  #if MODESERVER
  float arrayOfSensor[NODEOFNUMBER + 1];
  for (uint8_t address = NODEADDRESSSTRART; address <= NODEADDRESSSTRART + NODEOFNUMBER; ++address) {
    if (address == NODEADDRESSSTRART) {
      String valueSensor = (String)ACS712.getCurrentAC(ACS712FREQUENCY);
      float sensor = valueSensor.toFloat();
      arrayOfSensor[address - NODEADDRESSSTRART] = sensor;
    } else {
      #if MODEDEBUG
      Serial.print(F("LORA -> Send: "));
      Serial.print((char)address);
      #endif
      ledIndicator(1);
      LoRa.beginPacket();
      LoRa.print((char)address);
      LoRa.endPacket();
      ledIndicator(0);
      uint32_t timeMillis = millis();
      int8_t buffer, packetBuffer = -1;
      char packetReceived[LORABUFFER], sensorReceived[LORABUFFER], null = '\0';
      while (true) {
        if (LoRa.parsePacket()) {
          while (LoRa.available()) {
            packetReceived[++packetBuffer] = (char)LoRa.read();
          }
          packetReceived[++packetBuffer] = null;
          break;
        }
        if (millis() - timeMillis > LORATIMEOUT) {
          break;
        }
      }
      if (packetReceived[0] == (char)address) {
        if (packetReceived[1] != null) {
          for (buffer = 1; packetReceived[buffer] != null; ++buffer) {
            sensorReceived[buffer - 1] = packetReceived[buffer];
          }
          sensorReceived[buffer - 1] = null;
        }
        String valueSensor = sensorReceived;
        float sensor = valueSensor.toFloat();
        arrayOfSensor[address - NODEADDRESSSTRART] = sensor;
        #if MODEDEBUG
        Serial.print(F(" Recv: "));
        Serial.print(sensorReceived);
        Serial.print(F(" Flt: "));
        Serial.print(sensor);
        Serial.print(F(" Arr: "));
        Serial.print(arrayOfSensor[address - NODETHISADDRESS]);
        #endif
      }
      #if MODEDEBUG
      Serial.print(F("\n"));
      #endif
    }
  }
  static uint16_t timeInterval = 0;
  if (timeInterval < SIM800LINTERVAL) {
    ++timeInterval;
  } else {
    timeInterval = 0;
    if (simPushToServer(NODEOFNUMBER + 1, arrayOfSensor) == 200) {
      String respons = (String)SIM->getDataReceived();
      uint8_t responsReceived = respons.toInt();
      EEPROM.write(SWITCHADDRESS, responsReceived);
      #if MODEDEBUG
      Serial.print(F("SIM800L -> Data received: "));
      Serial.println(responsReceived);
      delay(5000);
      #endif
    }
  }
  #else
  #if MODEDEBUG
  boolean packetCallback = false;
  #endif
  int8_t packetBuffer = -1;
  char packetReceived[LORABUFFER], null = '\0';
  if (LoRa.parsePacket()) {
    while (LoRa.available()) {
      packetReceived[++packetBuffer] = (char)LoRa.read();
    }
    packetReceived[++packetBuffer] = null;
    if (packetReceived[0] == (char)NODETHISADDRESS) {
      #if MODEDEBUG
      packetCallback = true;
      #endif
      ledIndicator(1);
      LoRa.beginPacket();
      LoRa.print((char)NODETHISADDRESS);
      LoRa.print(ACS712.getCurrentAC(ACS712FREQUENCY));
      LoRa.endPacket();
      ledIndicator(0);
    }
    #if MODEDEBUG
    Serial.print(F("LORA -> Recv: "));
    Serial.print(packetReceived);
    if (packetCallback) {
      Serial.print(F(" Send: "));
      Serial.print((char)NODETHISADDRESS);
      Serial.print(ACS712.getCurrentAC(ACS712FREQUENCY));
    }
    Serial.print(F("\n"));
    #endif
  }
  #endif
}

void mcuInitialize(void) {
  timerBegin();
  ledIndicatorValue(LEDINDICATORDISABLE);
  pinMode(LEDSENDING, 1);
  pinMode(LEDRECEIVE, 1);
  digitalWrite(LEDSENDING, 1);
  digitalWrite(LEDRECEIVE, 1);
  #if MODEDEBUG || MODEDEBUGSIM
  Serial.begin(BAUDRATE);
  #endif
  ACS712.calibrate();
  loraBeginInit();
  simBeginInit();
}

void setup(void) {
  mcuInitialize();
}

void loop(void) {
  loraPacketProccess();
}

ISR(TIMER2_COMPA_vect) {
  if (ledIndicatorValue(false) == LEDINDICATORGET) {
    ledIndicatorInterrupt(3000);
  }
  if (ledIndicatorValue(false) == LEDINDICATORRSSI) {
    ledIndicatorInterrupt(1000);
  }
}
