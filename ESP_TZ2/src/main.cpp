#include "Arduino.h"
#include "WiFi.h"
#include "Audio.h"
#include <HardwareSerial.h>

#define I2S_DOUT  25
#define I2S_BCLK 27
#define I2S_LRC  26

HardwareSerial SerialPort(2);

Audio audio;

String ssid = "Mivart Studios";
String password = "M!vart.219";

char str[4];
char str_2[4];

int ledPin = 2;

// TZ2Comms Functions
void read_TZ2_serial()
{
  int i=0;

  if (SerialPort.available() > 0) 
  {
    //delay(100);
    while(SerialPort.available() && i<4) 
    
    {
      str[i++] = SerialPort.read();
    }
  }
}
void send_TZ2_serial()
{
  int value=4321; //this would be much more exciting if it was a sensor value
  
  itoa(value, str_2, 10); //Turn value into a character array
  SerialPort.write(str_2, 4);
}

//****************************************************************************************
//                                   A U D I O _ T A S K                                 *
//****************************************************************************************

struct audioMessage{
    uint8_t     cmd;
    const char* txt;
    uint32_t    value;
    uint32_t    ret;
} audioTxMessage, audioRxMessage;

enum : uint8_t { SET_VOLUME, GET_VOLUME, CONNECTTOHOST, CONNECTTOSD };

QueueHandle_t audioSetQueue = NULL;
QueueHandle_t audioGetQueue = NULL;

void CreateQueues(){
    audioSetQueue = xQueueCreate(10, sizeof(struct audioMessage));
    audioGetQueue = xQueueCreate(10, sizeof(struct audioMessage));
}

void audioTask(void *parameter) {
    CreateQueues();
    if(!audioSetQueue || !audioGetQueue){
        log_e("queues are not initialized");
        while(true){;}  // endless loop
    }

    struct audioMessage audioRxTaskMessage;
    struct audioMessage audioTxTaskMessage;

    audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    audio.setVolume(15); // 0...21

    while(true){
        if(xQueueReceive(audioSetQueue, &audioRxTaskMessage, 1) == pdPASS) {
            if(audioRxTaskMessage.cmd == SET_VOLUME){
                audioTxTaskMessage.cmd = SET_VOLUME;
                audio.setVolume(audioRxTaskMessage.value);
                audioTxTaskMessage.ret = 1;
                xQueueSend(audioGetQueue, &audioTxTaskMessage, portMAX_DELAY);
            }
            else if(audioRxTaskMessage.cmd == CONNECTTOHOST){
                audioTxTaskMessage.cmd = CONNECTTOHOST;
                audioTxTaskMessage.ret = audio.connecttohost(audioRxTaskMessage.txt);
                xQueueSend(audioGetQueue, &audioTxTaskMessage, portMAX_DELAY);
            }
            else if(audioRxTaskMessage.cmd == CONNECTTOSD){
                audioTxTaskMessage.cmd = CONNECTTOSD;
                audioTxTaskMessage.ret = audio.connecttoSD(audioRxTaskMessage.txt);
                xQueueSend(audioGetQueue, &audioTxTaskMessage, portMAX_DELAY);
            }
            else if(audioRxTaskMessage.cmd == GET_VOLUME){
                audioTxTaskMessage.cmd = GET_VOLUME;
                audioTxTaskMessage.ret = audio.getVolume();
                xQueueSend(audioGetQueue, &audioTxTaskMessage, portMAX_DELAY);
            }
            else{
                log_i("error");
            }
        }
        audio.loop();
        if (!audio.isRunning()) {
          sleep(1);
        }
    }
}

void audioInit() {
    xTaskCreatePinnedToCore(
        audioTask,             /* Function to implement the task */
        "audioplay",           /* Name of the task */
        5000,                  /* Stack size in words */
        NULL,                  /* Task input parameter */
        2 | portPRIVILEGE_BIT, /* Priority of the task */
        NULL,                  /* Task handle. */
        1                      /* Core where the task should run */
    );
}

audioMessage transmitReceive(audioMessage msg){
    xQueueSend(audioSetQueue, &msg, portMAX_DELAY);
    if(xQueueReceive(audioGetQueue, &audioRxMessage, portMAX_DELAY) == pdPASS){
        if(msg.cmd != audioRxMessage.cmd){
            log_e("wrong reply from message queue");
        }
    }
    return audioRxMessage;
}

void audioSetVolume(uint8_t vol){
    audioTxMessage.cmd = SET_VOLUME;
    audioTxMessage.value = vol;
    audioMessage RX = transmitReceive(audioTxMessage);
}

uint8_t audioGetVolume(){
    audioTxMessage.cmd = GET_VOLUME;
    audioMessage RX = transmitReceive(audioTxMessage);
    return RX.ret;
}

bool audioConnecttohost(const char* host){
    audioTxMessage.cmd = CONNECTTOHOST;
    audioTxMessage.txt = host;
    audioMessage RX = transmitReceive(audioTxMessage);
    return RX.ret;
}

bool audioConnecttoSD(const char* filename){
    audioTxMessage.cmd = CONNECTTOSD;
    audioTxMessage.txt = filename;
    audioMessage RX = transmitReceive(audioTxMessage);
    return RX.ret;
}

//****************************************************************************************
//                                   S E T U P                                           *
//****************************************************************************************

void setup() {
    Serial.begin(115200);
    SerialPort.begin(115200, SERIAL_8N1, 16, 17); 

    pinMode(ledPin, OUTPUT);

    WiFi.begin(ssid.c_str(), password.c_str());
    while (WiFi.status() != WL_CONNECTED) delay(1500);

    audioInit();

    audioConnecttohost("http://mp3.ffh.de/radioffh/hqlivestream.mp3");
    audioSetVolume(3);
    log_i("current volume is: %d", audioGetVolume());
}

//****************************************************************************************
//                                   L O O P                                             *
//****************************************************************************************

void loop(){
    // your own code here
    read_TZ2_serial();
    send_TZ2_serial();

    if (str == "1234")
    {
      digitalWrite(ledPin, HIGH);
    }


}
//*****************************************************************************************
//                                  E V E N T S                                           *
//*****************************************************************************************

void audio_info(const char *info){
    Serial.print("info        "); Serial.println(info);
}

