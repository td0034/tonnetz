/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//includes_and_inits

#include <FastLED.h>
#include <Encoder.h>
#include <Bounce.h>
#include <MIDI.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define ORCHESTRA_MEMBERS   91

// serial3 = pin 14(TX) and pin 15(rx) for communication with ESP32
#define HWSERIAL Serial3

/*
//FastLED WS2811 strip
#define NUM_LEDS_PER_STRIP  91
#define NUM_STRIPS          1
#define NUM_LEDS            NUM_LEDS_PER_STRIP * NUM_STRIPS
#define BRIGHTNESS          100
#define LAST_VISIBLE_LED    90
*/

//depending on the library version used, the below or above code needs to be used to configure fastLED
#define NUM_LEDS            91
#define DATA_PIN            3
#define BRIGHTNESS          250
#define LAST_VISIBLE_LED    90

CRGB leds[ NUM_LEDS ];

// I2C OLED Display
#define SCREEN_WIDTH        128   // OLED display width, in pixels
#define SCREEN_HEIGHT       64    // OLED display height, in pixels
#define OLED_RESET          -1    // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS      0x3C  // I2C address

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//LOGO
#define LOGO_HEIGHT         64
#define LOGO_WIDTH          64

// set up hardware MIDI
MIDI_CREATE_INSTANCE(HardwareSerial, Serial4, MIDI);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//pin_declarations for teensy 4.0

//fastLED 
#define DATA_PIN            3          
//const int WS28_3 =          0;
const int WS28_2 =          4;
const int WS28_1 =          3;

// rotary encoders E1 and E2 
const int E1A =             2;
const int E1B =             1;
const int E2A =             6;
const int E2B =             5;

//DMX Serial Port 2
//const int RX_DMX =          7;
//const int TX_DMX =          8;

//Multiplexers
const int MX_S3 =           9;
const int MX_S2 =           10;
const int MX_S1 =           11;
const int MX_S0 =           12;
const int MX1_D =           8;
//const int MX2_D =           21;
//const int MX3_D =           20;
//const int MX4_D =           15;
//const int MX5_D =           14;

//MIDI Serial Port 4
//const int MIDI_IN =         16;
const int MIDI_OUT =        17;

//Potentiometers
const int POT_L =           A6;
const int POT_R =           A7;
const int POT_C =           A8;
const int POT_D =           A9;


int randomNumber = 1;
int randomNumber2 = 1;

//const int test =            A4;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//initialise peripherals 
Encoder parameter_selector (E1A, E1B);
int oldPosition1 =                0;
long position1 =                  -999;  
signed int selectorVariable =     0;

Encoder value_selector (E2A, E2B);
int oldPosition_2 =               0;
long position_2 =                 -999;  
signed int selectorVariable_2 =   0;

bool button_values[16] =          {0};
bool last_button_values[16] =     {0};
bool switches[16][100] =          {0};

int pot1 =                        0;
int pot2 =                        0;
int pot3 =                        0;
int pot4 =                        0;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//declare data types
struct parameter 
{
  signed int value; 
  signed int max_value;
  signed int min_value;
} max_voices, min_voices, root_note, root_octave, conductor_period, loop_length, loop_status, left_dial, right_dial;

signed int parameterValue[11] = {0};

struct virtual_orchestra
{
  unsigned int midi_no[];
  unsigned int velocity[];
  unsigned int hue[];
  unsigned int bright[];
  unsigned int coor[];
  bool on[0];
} tonnetz_struct;

// tonnetz variables
const uint8_t kMatrixWidth = 13;
const uint8_t kMatrixHeight = 13;
int orchestraMembers = ORCHESTRA_MEMBERS;
int sizeX = 11;
int tonnetz[11][11];
int midinetz[11][11];
int orchestra[13][13];
int hue = 0;
int midiOffset = 0;
int note = 0;
unsigned int total_playing = 0;
unsigned int max_notes = 0;
unsigned int midi_offset = 36;

int midi_staging_area[11][11] = {0};
int prev_midi_staging_area[11][11] = {0};

int scale0 = 13;
int scale1 = 0;
int scale2 = 0;
int scale3 = 0;
int scale4 = 0;
int scale5 = 0;
int scale6 = 0;
int scale7 = 0;
int scale8_ = 0;
int scale9 = 0;
int scale10 = 0;
int scale11 = 0;

//velocity and brightness params
int bright = 100;
int velocity = 99; 

//loop variables
int loopXY[2][2][999];
int maxLoopLength = 999;
int loopLength = 240;
int loopPosition = 0;
bool recordPlay = 0;

// timing variables
unsigned long currentMillis = 0;
unsigned long beatLength = 500;         // 500ms = 2Hz = 120BPM = 120/60 (b/s)
unsigned long beatLength_previous = 0;
int beatCount = 1;
int beatsPerBar = 40;
int barCount = 0;
int barsPerMeasure = 4;
int measureCount = 0;
float BPM = 120.0;

// ESP Comms
int incomingByte;
char str[4];
char str_2[4];

// MIDI timing

unsigned long currentMillisConductor = 0;
unsigned long conductorPeriod = 10;
unsigned long conductorPeriod_previous = 0;

bool chord_A[12] = {0};                 // define empty chord arrays for chord progressions
bool chord_B[12] = {0};  
bool chord_C[12] = {0};  

//button status arrays
bool scale[12][100] = {0};      // array of scales for 100 scenes (scale changes)
bool control[4] = {0};
unsigned int scene = 0;
bool loopScenes = 0;

int controlPin[] = 
  {
    MX_S0, MX_S1, MX_S2, MX_S3
  };
  
int muxChannel[16][4] = 
  { 
    {0, 0, 0, 0},
    {1, 0, 0, 0},
    {0, 1, 0, 0},
    {1, 1, 0, 0},
    {0, 0, 1, 0},
    {1, 0, 1, 0},
    {0, 1, 1, 0},
    {1, 1, 1, 0},
    {0, 0, 0, 1},
    {1, 0, 0, 1},
    {0, 1, 0, 1},
    {1, 1, 0, 1},
    {0, 0, 1, 1},
    {1, 0, 1, 1},
    {0, 1, 1, 1},
    {1, 1, 1, 1}
  };

// Splashscreen 'hex', 64x64px https://javl.github.io/image2cpp/
const unsigned char hex64 [] PROGMEM = {
  0x00, 0x00, 0x00, 0x03, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xf0, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x3f, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xfe, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x01, 0xfc, 0x3f, 0x80, 0x00, 0x00, 0x00, 0x00, 0x03, 0xf0, 0x0f, 0xc0, 0x00, 0x00, 
  0x00, 0x00, 0x0f, 0xc0, 0x03, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x80, 0x01, 0xfc, 0x00, 0x00, 
  0x00, 0x00, 0xfe, 0x00, 0x00, 0x7f, 0x00, 0x00, 0x00, 0x03, 0xf8, 0x00, 0x00, 0x1f, 0xc0, 0x00, 
  0x00, 0x07, 0xf0, 0x00, 0x00, 0x0f, 0xe0, 0x00, 0x00, 0x1f, 0x80, 0x00, 0x00, 0x01, 0xf8, 0x00, 
  0x00, 0x7f, 0x00, 0x00, 0x00, 0x00, 0xfe, 0x00, 0x01, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x80, 
  0x03, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xc0, 0x0f, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x07, 0xf0, 
  0x0f, 0x80, 0x00, 0x00, 0x00, 0x00, 0x01, 0xf0, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 
  0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 
  0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 
  0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 
  0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 
  0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 
  0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 
  0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 
  0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 
  0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 
  0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 
  0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 
  0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 
  0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 
  0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 
  0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x0f, 0x80, 0x00, 0x00, 0x00, 0x00, 0x01, 0xf0, 
  0x0f, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x07, 0xf0, 0x03, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xc0, 
  0x01, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x80, 0x00, 0x7f, 0x00, 0x00, 0x00, 0x00, 0xfe, 0x00, 
  0x00, 0x1f, 0x80, 0x00, 0x00, 0x01, 0xf8, 0x00, 0x00, 0x07, 0xf0, 0x00, 0x00, 0x0f, 0xe0, 0x00, 
  0x00, 0x03, 0xf8, 0x00, 0x00, 0x1f, 0xc0, 0x00, 0x00, 0x00, 0xfe, 0x00, 0x00, 0x7f, 0x00, 0x00, 
  0x00, 0x00, 0x3f, 0x80, 0x01, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xc0, 0x03, 0xf0, 0x00, 0x00, 
  0x00, 0x00, 0x03, 0xf0, 0x0f, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x01, 0xfc, 0x3f, 0x80, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x7f, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xfc, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x0f, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xc0, 0x00, 0x00, 0x00
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//setup_function

void setup() {
  // put your setup code here, to run once:
  MIDI.begin();
  usbMIDI.setHandleStart(myStart);
  usbMIDI.setHandleStop(myStop);
  usbMIDI.setHandleContinue(myContinue);
  usbMIDI.setHandleClock(myClock); 

  // multiplexer select and data pins
  pinMode(MX_S0, OUTPUT);
  pinMode(MX_S1, OUTPUT);
  pinMode(MX_S2, OUTPUT);
  pinMode(MX_S3, OUTPUT);
  
  pinMode(MX1_D, INPUT);
  
  digitalWrite(MX_S0, LOW);
  digitalWrite(MX_S1, LOW);
  digitalWrite(MX_S2, LOW);
  digitalWrite(MX_S3, LOW);

  Serial.begin(9600);
  
  //setup hardware Serial for ESP Comms
  HWSERIAL.begin(115200);

  init_screen();
  
  splash_screen();

  default_parameter_values();

  //initialise_orchestra();

  //delay(1000);
  FastLED.addLeds<WS2811, DATA_PIN, RGB>(leds, NUM_LEDS);
  //FastLED.addLeds<NUM_STRIPS, WS2811, DATA_PIN, RGB>(leds, NUM_LEDS_PER_STRIP);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.clear();

  //delay(10000);

  setUpTonnetz();
  printTonnetz(); 

  //display all leds to verify lights on startup
  HEX_TNZ_ALL_ON();

  //leds[0] = CRGB::White;
  
  FastLED.show();
  delay(10000);
  FastLED.clear();
  FastLED.show();

  //set default parameters
  parameterValue[4] = 1;
  parameterValue[5] = barsPerMeasure;
  parameterValue[6] = beatLength;
  parameterValue[7] = beatsPerBar;
  parameterValue[8] = loopLength;


}

////////////////*/////////////*////*/////////////*////////////////////////////////*///////////////////*/*////
//main loop

void loop() 
{
  // put your main code here, to run repeatedly:

  update_button_values();
  check_selector();
  check_selector_2();
  check_potentios();
  update_screen();
  count_beats_and_bars();
  read_ESP_serial();
  send_ESP_serial();
  
////////// F / U / N / C / T / I / O / N ////// T / E / S / T / B / E / D //////////// - START

/// - next up


/// - 1
/// Add: beat length,  to adjustable parameter in the screen menu
/// - 2
/// create a quantised mode to trigger whole chords evert beat/bar/etc.
/// - 3
/// mix between quantised and random
/// - 4

  
  currentMillisConductor = millis();
  if (currentMillisConductor - conductorPeriod_previous >= conductor_period.value)
  {
    conductorPeriod_previous = currentMillisConductor;

    COUNT_ORCHESTRA_MEMBERS();

    if (total_playing <= max_voices.value)
    {
      if (recordPlay == 0)
      { 
      //Serial.println("Recording Note On");
      HEX_TNZ_NEIGHBOURS_SCALE_ON(loopPosition);     
      }// tell a random orchestra member to play
      
      if (recordPlay == 1)
      {
        //Serial.print("Playing sample :");
        //Serial.println(loopPosition);
        HEX_TNZ_NEIGHBOURS_SCALE_LOOP_ON(loopPosition);
      }
    }

    if (recordPlay == 0)
    { 
      //Serial.println("Recording Note Off");
      HEX_TNZ_NEIGHBOURS_OFF(loopPosition);
    }// tell a random orchestra member to stop
    
    if (recordPlay == 1)
    {
      HEX_TNZ_NEIGHBOURS_LOOP_OFF(loopPosition);
    }

    COUNT_ORCHESTRA_MEMBERS();  
                                                        
    if (total_playing < min_voices.value)             
    {
      tickleOrchestra();                                // tickle a random orchestra member if none are playing
    }
    
    if (loopPosition >= loopLength)
    {
      //recordPlay = 1;
      loopPosition = 0;
    }

    //Serial.println(recordPlay);
    

    loopPosition = loopPosition + 1;
    
  
    
  }

  FastLED.show();                                      



////////// F / U / N / C / T / I / O / N ////// T / E / S / T / B / E / D //////////// - END


//////// tests
//FLASH_TNZ_ALL_ON();

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//functons

void init_screen()
{
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) 
  {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
}

void splash_screen()
{
  display.clearDisplay();
  display.drawBitmap(
    (display.width()  - LOGO_WIDTH ) / 2,
    (display.height() - LOGO_HEIGHT) / 2,
    hex64, LOGO_WIDTH, LOGO_HEIGHT, 1);
  display.display();
  delay(1000);
}

bool readMux(int channel) 
{
  //loop through the 4 mux select lines
  for (int i = 0; i < 4; i ++) 
  {
    digitalWrite(controlPin[i], muxChannel[channel][i]);
  }
  //read the value at the SIG pin
  bool val = digitalRead(MX1_D); //return the value
  return val;
}

void update_button_values()
{
  for (int i = 0; i < 16; i ++) 
  {
    // read buttons via mux
    button_values[i] = readMux(i);
    // toggle button status if the active low button is not equal to previous status
    if ((button_values[i] != last_button_values[i]) && (button_values[i] == 0))
    {
      switches[i][scene] = !switches[i][scene];
      //if(i==8){root_octave.value++;}
      if(i==13){recordPlay = !recordPlay;}
      if(i==8){root_octave.value--;}
      if(i==15){RESET();}
      //if(i==14){recordPlay = !recordPlay;}
      
      if(i==14)
      {
        //scene+=1;
        //if (scene > 4){scene=0;}

        loopScenes = !loopScenes;
      }
    }
    last_button_values[i] = button_values[i];
  }
  // reorganise button order 
  sort_buttons();
}

void print_button_values()
{
  for (int i = 0; i < 12; i ++) 
  {
    Serial.print(scale[i][scene]);
  }
  Serial.print(", ");
  for (int i = 0; i < 4; i ++) 
  {
    Serial.print(control[i]);
  }
  Serial.println(" ");
}

void gorge(void) 
{
  display.clearDisplay();
  display.setTextSize(2);               // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);  // Draw white text
  display.setCursor(10, 0);             // Start at top-left corner
  display.cp437(true);                  // Use full 256 char 'Code Page 437' font
  display.println(F("Hi Gorge"));
  display.display();
  delay(1000);
}

void check_selector()
{
  long newPosition1 = parameter_selector.read();
  if(newPosition1 != position1)
  {
    position1 = newPosition1;

    if (position1 - oldPosition1 > 3){selectorVariable++;oldPosition1 = position1;}
    if (oldPosition1 - position1 > 3){selectorVariable--;oldPosition1 = position1;}
    if (selectorVariable<0){selectorVariable=10;}
    if (selectorVariable>10){selectorVariable=0;}
    
    selectorVariable_2 = parameterValue[selectorVariable];
  }
}

void check_selector_2()
{   
  long newPosition_2 = value_selector.read();       
  if(newPosition_2 != position_2)
  {
    position_2 = newPosition_2;

    if (position_2 - oldPosition_2 > 3){selectorVariable_2 = parameterValue[selectorVariable] + 1;oldPosition_2 = position_2;}
    if (oldPosition_2 - position_2 > 3){selectorVariable_2 = parameterValue[selectorVariable] - 1;oldPosition_2 = position_2;}
    if (selectorVariable_2<-9999){selectorVariable_2=-9999;}
    if (selectorVariable_2>9999){selectorVariable_2=9999;}
    
    parameterValue[selectorVariable] = selectorVariable_2;
    update_parameter_values();

  }
}

void check_potentios()
{
  pot1 = analogRead(POT_L);
  bright = map(pot1, 0, 1023, 255, 0);
  FastLED.setBrightness(bright);
  pot1 = map(pot1, 0, 1023, 127, 0);
  velocity = pot1;
  pot2 = analogRead(POT_R);
  pot2 = map(pot2, 0, 1023, 127, 0);
  pot3 = analogRead(POT_C);
  pot3 = map(pot3, 0, 1023, 127, 0);
  pot4 = analogRead(POT_D);
  pot4 = map(pot4, 0, 1023, 127, 0);
}

void update_screen()
{
  display.clearDisplay();
  display.setTextSize(1);                         // 2:1 pixel scale
  display.setTextColor(SSD1306_WHITE);            // Draw white text
  display.setCursor(0, 0);                        // (X, Y) location, (0, 0) = top left corner
  display.cp437(true);                            // Use full 256 char 'Code Page 437' font
  
  //display.println(parameterPointer);
  //display.println(midiOffset);
  //print_note();

  // set cursor from 0,0 to 128, 64 (x, y) - 10 in y direction is roughly a new line for .setTextSize(1)
  display.setCursor(0, 0);
  //display.println(selectorVariable);
  display.setCursor(0, 0);
  print_parameter(selectorVariable);
  display.setCursor(0, 10);
  //display.println(selectorVariable_2);
  //display.println(parameterValue[selectorVariable]);
  display.setCursor(31, 50);
  display.print(scale[1][scene]);
  display.print(" ");
  display.print(scale[3][scene]);
  display.print("   ");
  display.print(scale[6][scene]);
  display.print(" ");
  display.print(scale[8][scene]);
  display.print(" ");
  display.print(scale[10][scene]);
  display.setCursor(25, 54);
  display.print(scale[0][scene]);
  display.print(" ");
  display.print(scale[2][scene]);
  display.print(" ");
  display.print(scale[4][scene]);
  display.print(" ");
  display.print(scale[5][scene]);
  display.print(" ");
  display.print(scale[7][scene]);
  display.print(" ");
  display.print(scale[9][scene]);
  display.print(" ");
  display.print(scale[11][scene]);
  display.print(" ");

  display.setCursor(0, 30);
  display.print(beatCount);
  display.setCursor(0, 40);
  display.print(barCount);
  display.setCursor(0, 50);
  display.print(loopPosition);

  display.setCursor(35, 10);
  display.println("AMP");
  display.setCursor(60, 10);
  display.println("TBC");
  display.setCursor(85, 10);
  display.println("TBC");
  display.setCursor(110, 10);
  display.println("TBC");
  
  display.setCursor(35, 20);
  display.println(pot1);
  display.setCursor(60, 20);
  display.println(pot2);
  display.setCursor(85, 20);
  display.println(pot3);
  display.setCursor(110, 20);
  display.println(pot4);

  display.setCursor(35, 30);
  display.println("RST");
  display.setCursor(60, 30);
  display.println("SCN");
  display.setCursor(85, 30);
  display.println("LUP");
  display.setCursor(110, 30);
  //display.println("TBC");
  display.println(str);

  display.setCursor(35, 40);
  display.println("");
  display.setCursor(60, 40);
  display.println(loopScenes);
  display.setCursor(85, 40);
  display.println(recordPlay);
  display.setCursor(110, 40);
  display.println(0);
  
  
  
  /*
  display.setCursor(0, 10);     
  print_parameter();
  if(selectorVariable !=0)
  {
    display.setCursor(0, 20);
    display.println("~");
  }
  display.setCursor(10, 20);  
  //print_parameter_value();
  //print_scale();
  //print_loop_status();
  //print_loop_position();

  */
  display.display();
}

void print_parameter(int parameter)
{
  if(parameter==0)
  {
    display.println("Max Voices:");
    display.setCursor(0, 10);
    display.println(max_voices.value);
  }
  if(parameter==1)
  {
    display.println("Min Voices:");
    display.setCursor(0, 10);
    display.println(min_voices.value);
  }
  if(parameter==2)
  {
    display.println("Root Note:");
    display.setCursor(0, 10);
    if(root_note.value%12 == 0)display.println("C");
    if(root_note.value%12 == 1)display.println("C#");
    if(root_note.value%12 == 2)display.println("D"); 
    if(root_note.value%12 == 3)display.println("Eb"); 
    if(root_note.value%12 == 4)display.println("E"); 
    if(root_note.value%12 == 5)display.println("F");
    if(root_note.value%12 == 6)display.println("F#");  
    if(root_note.value%12 == 7)display.println("G"); 
    if(root_note.value%12 == 8)display.println("Ab"); 
    if(root_note.value%12 == 9)display.println("A"); 
    if(root_note.value%12 == 10)display.println("Bb"); 
    if(root_note.value%12 == 11)display.println("B");
  }
  if(parameter==10)
  {
    display.println("Root Octave:");
    display.setCursor(0, 10);
    display.println(root_octave.value);
  }
  if(parameter==3)
  {
    display.println("Conductor Period:");
    display.setCursor(0, 10);
    display.println(conductor_period.value);
  }
  if(parameter==4)
  {
    display.println("Scene:");
    display.setCursor(0, 10);
    display.println(scene);
  }
  
  if(parameter==5)
  {
    display.println("Bars per loop");
    display.setCursor(0, 10);
    display.println(barsPerMeasure);
  }
  if(parameter==6)
  {
    display.println("Beat Length:");
    display.setCursor(0, 10);
    display.println(beatLength);
    
  }
  if(parameter==7)
  {
    display.println("Beats per Bar:");
    display.setCursor(0, 10);
    display.println(beatsPerBar);
  }
  if(parameter==8)
  {
    display.println("Loop length");
    display.setCursor(0, 10);
    display.println(loopLength);
  }
}

void sort_buttons()
{
  // scale buttons
  scale[0][scene]=switches[8][scene];     // C
  scale[1][scene]=switches[1][scene];     // C#
  scale[2][scene]=switches[2][scene];     // ...
  scale[3][scene]=switches[3][scene];
  scale[4][scene]=switches[4][scene];
  scale[5][scene]=switches[6][scene];
  scale[6][scene]=switches[5][scene];
  scale[7][scene]=switches[7][scene];
  scale[8][scene]=switches[0][scene];
  scale[9][scene]=switches[9][scene];
  scale[10][scene]=switches[10][scene];   // ...
  scale[11][scene]=switches[11][scene];   // B
  // control buttons 
  control[0]=switches[15][0];   // top left
  control[1]=switches[14][0];
  control[2]=switches[13][0];
  control[3]=switches[12][0];  // bottom right
}

void default_parameter_values()
{
  max_voices.value = 0;
  min_voices.value = 0;
  root_note.value = 0; 
  root_octave.value = 3; 
  conductor_period.value = 50;
  loop_length.value = 500;
  loop_status.value = 0;
  left_dial.value = 0;
  right_dial.value = 0;
}

void update_parameter_values()
{
  max_voices.value = parameterValue[0];
  if(min_voices.value > max_voices.value)
  {
    parameterValue[1] = max_voices.value;
  }
  min_voices.value = parameterValue[1];
  if(min_voices.value == 0)
  {
    min_voices.value = 1;
  }
  root_note.value = parameterValue[2];
  conductor_period.value = parameterValue[3];
  scene = parameterValue[4];
  barsPerMeasure = parameterValue[5];
  beatLength = parameterValue[6];
  beatsPerBar = parameterValue[7];
  loopLength = parameterValue[8];
}

void initialise_orchestra()
{
  tonnetz_struct.midi_no[orchestraMembers] =   {0};
  tonnetz_struct.velocity[orchestraMembers] =  {0};
  tonnetz_struct.hue[orchestraMembers] =       {0};
  tonnetz_struct.bright[orchestraMembers] =    {0};
  tonnetz_struct.coor[sizeX] =                 {0};
  tonnetz_struct.on[orchestraMembers] =        {0};
}

uint8_t XY (uint8_t x, uint8_t y)                         // maps the light index numbers (0 to 90) to a hexagonal grid
{
  // any out of bounds address maps to the first hidden pixel
  if ( (x >= kMatrixWidth) || (y >= kMatrixHeight) ) 
  {
    return (LAST_VISIBLE_LED + 1);
  }

  const uint8_t XYTable[] = 
  {

    103,  104,  116,  117,  125,  126,  130,  131,  137,  138,  148,  149,  168, 
    102,  0,    12,   13,   29,   30,   50,   132,  136,  139,  147,  150,  167, 
    101,  1,    11,   14,   28,   31,   49,   51,   135,  140,  146,  151,  166, 
    100,  2,    10,   15,   27,   32,   48,   52,   69,   141,  145,  152,  165, 
    99,   3,    9,    16,   26,   33,   47,   53,   68,   70,   144,  153,  164, 
    98,   4,    8,    17,   25,   34,   46,   54,   67,   71,   84,   154,  163, 
    97,   5,    7,    18,   24,   35,   45,   55,   66,   72,   83,   85,   162, 
    96,   105,  6,    19,   23,   36,   44,   56,   65,   73,   82,   86,   161, 
    95,   106,  115,  20,   22,   37,   43,   57,   64,   74,   81,   87,   160, 
    94,   107,  114,  118,  21,   38,   42,   58,   63,   75,   80,   88,   159, 
    93,   108,  113,  119,  124,  39,   41,   59,   62,   76,   79,   89,   158, 
    92,   109,  112,  120,  123,  127,  40,   60,   61,   77,   78,   90,   157, 
    91,   110,  111,  121,  122,  128,  129,  133,  134,  142,  143,  155,  156 
  
  };

  uint8_t i = (y * kMatrixWidth) + x;
  uint8_t j = XYTable[i];
  return j;
}

void setUpTonnetz()                                       // sets up an array with midi equivalents of Eulers Tonnetz grid
{                                                         // 
  for(int i = 0; i < sizeX; i = i + 1)                    // consider different chords.. 3 and 4 give minor and major respectively, why not sus2,4 dim etc.?
  {
    for(int j = 0; j < sizeX; j = j + 1)
    {
      
      tonnetz[i][j] = i * 3 + j * 4;                      // simplicity! see above comment for choice of "3" & "4" literals
      midinetz[i][j] = tonnetz[i][j];
      tonnetz[i][j] = tonnetz[i][j] % 12;               // western keyboard wraps around every 12 notes
      
      usbMIDI.sendNoteOff(i*3, velocity, 1);                    // turn off all midi notes at startup to avoid hanging notes
      usbMIDI.sendNoteOff(j*4, velocity, 1);

      MIDI.sendNoteOff(i*3, velocity, 1);                       // turn off all midi notes at startup to avoid hanging notes
      MIDI.sendNoteOff(j*4, velocity, 1);
    }
  }
}


void printTonnetz()
{
  for(int i = 0; i < sizeX; i = i + 1)
  {
    for(int j = 0; j < sizeX; j = j + 1)
    {
      Serial.print(tonnetz[i][j]);
      Serial.print("\t");  
    }
    Serial.println();
  }
}

void hueFromMidi(int x, int y)
{
  note = tonnetz[x-1][y-1] + midiOffset;
  if (note % 12 == 0) {hue = 76;}      // C
  if (note % 12 == 7) {hue = 103;}     // G
  if (note % 12 == 2) {hue = 120;}     // D
  if (note % 12 == 9) {hue = 140;}     // A
  if (note % 12 == 4) {hue = 178;}     // E
  if (note % 12 == 11){hue = 191;}     // B
  if (note % 12 == 6) {hue = 213;}     // F#
  if (note % 12 == 1) {hue = 234;}     // C#
  if (note % 12 == 8) {hue = 0;}       // G#
  if (note % 12 == 3) {hue = 21;}      // D#
  if (note % 12 == 10){hue = 43;}      // A#
  if (note % 12 == 5) {hue = 64;}      // F
}

void hueFromMidiFifths(int x, int y)
{
  note = tonnetz[x-1][y-1] + midiOffset;
  if (note % 12 == 0) {hue = 85;}      // C
  if (note % 12 == 7) {hue = 106;}     // G
  if (note % 12 == 2) {hue = 128;}     // D
  if (note % 12 == 9) {hue = 149;}     // A
  if (note % 12 == 4) {hue = 170;}     // E
  if (note % 12 == 11){hue = 191;}     // B
  if (note % 12 == 6) {hue = 213;}     // F#
  if (note % 12 == 1) {hue = 234;}     // C#
  if (note % 12 == 8) {hue = 0;}       // G#
  if (note % 12 == 3) {hue = 22;}      // D#
  if (note % 12 == 10){hue = 43;}       // A#
  if (note % 12 == 5) {hue = 64;}      // F
}

void hueFromMidiOg(int x, int y)
{
  note = tonnetz[x-1][y-1] + midiOffset;
  if (note % 12 == 0){hue = 85;}      // C
  if (note % 12 == 1){hue = 234;}     // C#
  if (note % 12 == 2){hue = 128;}     // ...
  if (note % 12 == 3){hue = 22;}
  if (note % 12 == 4){hue = 170;}
  if (note % 12 == 5){hue = 64;}
  if (note % 12 == 6){hue = 213;}
  if (note % 12 == 7){hue = 106;}
  if (note % 12 == 8){hue = 0;}
  if (note % 12 == 9){hue = 149;}
  if (note % 12 == 10){hue = 43;}     // ...
  if (note % 12 == 11){hue = 191;}    // B
}

void HEX_TNZ_ON(int x, int y)
{
  hueFromMidi(x,y);
  leds[XY(x,y)]= CHSV(hue,255,BRIGHTNESS);
}

void HEX_TNZ_ALL_ON()
{
  for(int yl = 0; yl < 13; yl = yl + 1)
  {   
   for(int xl = 0; xl < 13; xl = xl + 1)
   {  
      if (XY(xl,yl)<91)
      {
        HEX_TNZ_ON(xl,yl);
      }      
   }
  }
}

// MIDI routines
void myStart() {
}

void myStop() {
}

void myContinue() {
}

void myClock() {
}

void FLASH_TNZ_ALL_ON_BEAT_COUNTING()
{
 currentMillis = millis();
  if (currentMillis - beatLength_previous >= beatLength)
  {
    beatLength_previous = currentMillis;
    beatCount += 1;
    if (beatCount % 2)
    {
      HEX_TNZ_ALL_ON();
      FastLED.show();
    }
    else
    {
      FastLED.clear();
      FastLED.show();
    }
  }
}

void FLASH_TNZ_ALL_ON()
{
  if (beatCount % 2)
  {
    HEX_TNZ_ALL_ON();
    FastLED.show();
  }
  else
  {
    FastLED.clear();
    FastLED.show();
  }
}

void count_beats_and_bars()
{
  currentMillis = millis();
  if (currentMillis - beatLength_previous >= beatLength)
  {
    beatLength_previous = currentMillis;
    beatCount += 1;
    //FastLED.show();
    //PLAY_MIDI();
  
    if (beatCount > beatsPerBar)
    {
      barCount += 1;
      //FastLED.show();
      //PLAY_MIDI();
      beatCount = 1;
      if(loopScenes==1)
        {
          scene=barCount-1;
          soft_RESET();
        }
    }
    if (barCount > barsPerMeasure)
    {
      barCount = 1;
    }
    else
    {
    }
  }
}


void COUNT_ORCHESTRA_MEMBERS()
{
  total_playing = 0;
  for (int i = 1; i < 13; i = i + 1)
  {
    for (int j = 1; j < 13; j = j + 1)
    { 
      if (XY(i,j)<91)
      {
        total_playing = total_playing + orchestra[i][j]; // working out how many people playing
      }
    }      
  }
  //Serial.println(total_playing); 
}

void selectScale()
{
  if (scale[0][scene] == 1)
  {
    scale0 = 0;
  }
  else
  {
    scale0 = 13;
  }
  if (scale[1][scene] == 1)
  {
    scale1 = 1;
  }
  else
  {
    scale1 = 13;
  }
  if (scale[2][scene] == 1)
  {
    scale2 = 2;
  }
  else
  {
    scale2 = 13;
  }
  if (scale[3][scene] == 1)
  {
    scale3 = 3;
  }
  else
  {
    scale3 = 13;
  }
  if (scale[4][scene] == 1)
  {
    scale4 = 4;
  }
  else
  {
    scale4 = 13;
  }
  if (scale[5][scene] == 1)
  {
    scale5 = 5;
  }
  else
  {
    scale5 = 13;
  }
  if (scale[6][scene] == 1)
  {
    scale6 = 6;
  }
  else
  {
    scale6 = 13;
  }
  if (scale[7][scene] == 1)
  {
    scale7 = 7;
  }
  else
  {
    scale7 = 13;
  }
  if (scale[8][scene] == 1)
  {
    scale8_ = 8;
  }
  else
  {
    scale8_ = 13;
  }
  if (scale[9][scene] == 1)
  {
    scale9 = 9;
  }
  else
  {
    scale9 = 13;
  }
  if (scale[10][scene] == 1)
  {
    scale10 = 10;
  }
  else
  {
    scale10 = 13;
  }
  if (scale[11][scene] == 1)
  {
    scale11 = 11;
  }
  else
  {
    scale11 = 13;
  }
}

void tickleOrchestra()
{
  int init_x = random(0,12);
  int init_y = random(0,12);

  if (XY(init_x,init_y)<91)
  {
    selectScale();                                        // only tickle an orchestra member if they are within the current scale
    if (
    (midinetz[init_x-1][init_y-1] % 12 == scale0) ||
    (midinetz[init_x-1][init_y-1] % 12 == scale1) ||
    (midinetz[init_x-1][init_y-1] % 12 == scale2) ||
    (midinetz[init_x-1][init_y-1] % 12 == scale3) ||
    (midinetz[init_x-1][init_y-1] % 12 == scale4) ||
    (midinetz[init_x-1][init_y-1] % 12 == scale5) ||
    (midinetz[init_x-1][init_y-1] % 12 == scale6) ||
    (midinetz[init_x-1][init_y-1] % 12 == scale7) ||
    (midinetz[init_x-1][init_y-1] % 12 == scale8_) ||
    (midinetz[init_x-1][init_y-1] % 12 == scale9) ||
    (midinetz[init_x-1][init_y-1] % 12 == scale10) ||
    (midinetz[init_x-1][init_y-1] % 12 == scale11)
       )
    {
      orchestra[init_x][init_y] = 1;
    }
  }
}

void HEX_TNZ_NEIGHBOURS_SCALE_ON(int loopPosition)
{
  int xD2 = random(1,12);
  int yD2 = random(1,12);

  //randomiser();
  //randomiser2();
  
  //conductorSelector();
  //int xD2 = randomNumber;
  //int yD2 = randomNumber2;


  loopXY[0][0][loopPosition] = xD2;
  loopXY[1][0][loopPosition] = yD2;

  if (XY(xD2,yD2)<91)
  {
    if (orchestra[xD2][yD2] == 0)
    {
      if (
      (orchestra[xD2-1][yD2] == 1) ||     // A, is the note 3 semitones below on?
      (orchestra[xD2+1][yD2] == 1) ||     // D, is the note 3 semitones above on?
      (orchestra[xD2-1][yD2-1] == 1) ||   // B, is the note 7 semitones below on?
      (orchestra[xD2+1][yD2+1] == 1) ||   // E, is the note 7 semitones above on?
      (orchestra[xD2][yD2+1] == 1) ||     // C, is the note 4 semitones below on?
      (orchestra[xD2][yD2-1] == 1) ||     // F, is the note 4 semitones above on?
      (orchestra[xD2-1][yD2+1] == 1) ||   // B, is the note 1 semitone below on?
      (orchestra[xD2+1][yD2+2] == 1) ||   // 11 semitones above (extended neighbours)
      (orchestra[xD2+2][yD2+1] == 1) ||   // 10 semitones above
      (orchestra[xD2-1][yD2-2] == 1) ||   // 11 semitones below
      (orchestra[xD2-2][yD2-1] == 1) ||   // 10 semitones below
      (orchestra[xD2+1][yD2-1] == 1)      // E, is the note 1 semitone above on?
         )
      {
        selectScale();
        if (
        (midinetz[xD2-1][yD2-1] % 12 == scale0) ||
        (midinetz[xD2-1][yD2-1] % 12 == scale1) ||
        (midinetz[xD2-1][yD2-1] % 12 == scale2) ||
        (midinetz[xD2-1][yD2-1] % 12 == scale3) ||
        (midinetz[xD2-1][yD2-1] % 12 == scale4) ||
        (midinetz[xD2-1][yD2-1] % 12 == scale5) ||
        (midinetz[xD2-1][yD2-1] % 12 == scale6) ||
        (midinetz[xD2-1][yD2-1] % 12 == scale7) ||
        (midinetz[xD2-1][yD2-1] % 12 == scale8_) ||
        (midinetz[xD2-1][yD2-1] % 12 == scale9) ||
        (midinetz[xD2-1][yD2-1] % 12 == scale10) ||
        (midinetz[xD2-1][yD2-1] % 12 == scale11)
           )
        {
          HEX_TNZ_ON(xD2, yD2);
          HEX_TNZ_MIDI_ON(xD2, yD2);
          orchestra[xD2][yD2] = 1;
        }
      }
    }
  }
}

void HEX_TNZ_NEIGHBOURS_OFF(int loopPosition)
{
  int xD3 = random(1,12);
  int yD3 = random(1,12);

  loopXY[0][0][loopPosition] = xD3;
  loopXY[0][1][loopPosition] = yD3;

  if (XY(xD3,yD3)<91)
  {
    if (orchestra[xD3][yD3] == 1)
    {
      //HEX_TNZ_OFF(xD3, yD3);
      HEX_TNZ_WHITE(xD3, yD3);
      HEX_TNZ_MIDI_OFF(xD3, yD3);
      orchestra[xD3][yD3] = 0;
    }
  }
}

void HEX_TNZ_NEIGHBOURS_SCALE_LOOP_ON(int loopPosition)
{
  int xD2 = loopXY[0][0][loopPosition];
  int yD2 = loopXY[0][1][loopPosition];

  if (XY(xD2,yD2)<91)
  {
    if (orchestra[xD2][yD2] == 0)
    {
      if (
      (orchestra[xD2-1][yD2] == 1) ||     // A, is the note 3 semitones below on?
      (orchestra[xD2+1][yD2] == 1) ||     // D, is the note 3 semitones above on?
      (orchestra[xD2-1][yD2-1] == 1) ||   // B, is the note 7 semitones below on?
      (orchestra[xD2+1][yD2+1] == 1) ||   // E, is the note 7 semitones above on?
      (orchestra[xD2][yD2+1] == 1) ||     // C, is the note 4 semitones below on?
      (orchestra[xD2][yD2-1] == 1) ||     // F, is the note 4 semitones above on?
      (orchestra[xD2-1][yD2+1] == 1) ||   // B, is the note 1 semitone below on?
      (orchestra[xD2+1][yD2+2] == 1) ||   // 11 semitones above (extended neighbours)
      (orchestra[xD2+2][yD2+1] == 1) ||   // 10 semitones above
      (orchestra[xD2-1][yD2-2] == 1) ||   // 11 semitones below
      (orchestra[xD2-2][yD2-1] == 1) ||   // 10 semitones below
      (orchestra[xD2+1][yD2-1] == 1)      // E, is the note 1 semitone above on?
         )
      {
        selectScale();
        if (
        (midinetz[xD2-1][yD2-1] % 12 == scale0) ||
        (midinetz[xD2-1][yD2-1] % 12 == scale1) ||
        (midinetz[xD2-1][yD2-1] % 12 == scale2) ||
        (midinetz[xD2-1][yD2-1] % 12 == scale3) ||
        (midinetz[xD2-1][yD2-1] % 12 == scale4) ||
        (midinetz[xD2-1][yD2-1] % 12 == scale5) ||
        (midinetz[xD2-1][yD2-1] % 12 == scale6) ||
        (midinetz[xD2-1][yD2-1] % 12 == scale7) ||
        (midinetz[xD2-1][yD2-1] % 12 == scale8_) ||
        (midinetz[xD2-1][yD2-1] % 12 == scale9) ||
        (midinetz[xD2-1][yD2-1] % 12 == scale10) ||
        (midinetz[xD2-1][yD2-1] % 12 == scale11)
           )
        {
          HEX_TNZ_ON(xD2, yD2);
          HEX_TNZ_MIDI_ON(xD2, yD2);
          orchestra[xD2][yD2] = 1;
        }
      }
    }
  }
}

void HEX_TNZ_NEIGHBOURS_LOOP_OFF(int loopPosition)
{
  int xD3 = loopXY[0][0][loopPosition];
  int yD3 = loopXY[1][0][loopPosition];

  if (XY(xD3,yD3)<91)
  {
    if (orchestra[xD3][yD3] == 1)
    {
      //HEX_TNZ_OFF(xD3, yD3);
      HEX_TNZ_WHITE(xD3, yD3);
      HEX_TNZ_MIDI_OFF(xD3, yD3);
      orchestra[xD3][yD3] = 0;
    }
  }
}

void HEX_TNZ_MIDI_ON(int x, int y)
{
  usbMIDI.sendNoteOn(midinetz[x-1][y-1] + midi_offset, velocity, 1);
  MIDI.sendNoteOn(midinetz[x-1][y-1] + midi_offset, velocity, 1);
}

void HEX_TNZ_MIDI_OFF(int x, int y)
{
  usbMIDI.sendNoteOff(midinetz[x-1][y-1] + midi_offset, velocity, 1);
  MIDI.sendNoteOff(midinetz[x-1][y-1] + midi_offset, velocity, 1);
}

void HEX_TNZ_WHITE(int x, int y)
{
  leds[XY(x,y)]= CRGB(pot2, pot2, pot2);
}

void HEX_TNZ_MIDI_ON_SYNC(int x, int y)
{
  midi_staging_area[x][y] = 1;
}

void HEX_TNZ_MIDI_OFF_SYNC(int x, int y)
{
  midi_staging_area[x][y] = 0;
}

void PLAY_MIDI()
{
  for (int i = 0; i<11; i++)
  {
    for (int j = 0; j<11; j++)
    {
      if ((midi_staging_area[i][j] == 1) && (prev_midi_staging_area[i][j] == 0))
      {
        usbMIDI.sendNoteOn(midinetz[i-1][j-1] + midi_offset, velocity, 1);
        MIDI.sendNoteOn(midinetz[i-1][j-1] + midi_offset, velocity, 1);
        prev_midi_staging_area[i][j] = 1;
      }
      else
      {
        usbMIDI.sendNoteOff(midinetz[i-1][j-1] + midi_offset, velocity, 1);
        MIDI.sendNoteOff(midinetz[i-1][j-1] + midi_offset, velocity, 1);
        prev_midi_staging_area[i][j] = 0;
      }
    }
  }
}

/// USER FUNCTIONS for assigning to push buttons /// -------------

void RESET()
{
  //clear orchestra
  for (int i = 1; i < 13; i = i + 1)
  {
    for (int j = 1; j < 13; j = j + 1)
    { 
      orchestra[i][j] = 0;
    }
  }
  
  //clear midi
  for (int i = 0; i < 127; i = i + 1)
  {
    //for (int j = 0; j < 12; j = j + 1)
    //{ 
    usbMIDI.sendNoteOff(i, velocity, 1);
    MIDI.sendNoteOff(i, velocity, 1);
    //}
  }
  
  //clear lights
  FastLED.clear();

  //set recording back to on
  //recordPlay = 0;
}

void soft_RESET()
{
  // this reset doesnt clear orchestra, only the notes and the lights!
  
  //clear midi
  for (int i = 0; i < 127; i = i + 1)
  {
    //for (int j = 0; j < 12; j = j + 1)
    //{ 
    usbMIDI.sendNoteOff(i, velocity, 1);
    MIDI.sendNoteOff(i, velocity, 1);
    //}
  }
  
  //clear lights
  FastLED.clear();

  //set recording back to on
  //recordPlay = 0;
}

void randomiser()
{
  randomNumber++;
  if (randomNumber > 12)
  {
    randomNumber = 1;
  }
}
void randomiser2()
{
  randomNumber2++;
  if (randomNumber2 > 12)
  {
    randomNumber2 = 1;
  }
}

void conductorSelector()
{
  randomNumber++;
  if (randomNumber > 12)
  {
    randomNumber = 1;
    
    randomNumber2++;
    
    if (randomNumber2 > 12)
    {
    randomNumber2 = 1;
    }
  }
  
}

void read_ESP_serial()
{
  int i=0;

  if (HWSERIAL.available() > 0) 
  {
    //delay(100);
    while(Serial3.available() && i<4) 
    
    {
      str[i++] = HWSERIAL.read();
    }
  }
}
void send_ESP_serial()
{
  int value=1234; //this would be much more exciting if it was a sensor value
  
  itoa(value, str_2, 10); //Turn value into a character array
  HWSERIAL.write(str_2, 4);
}
