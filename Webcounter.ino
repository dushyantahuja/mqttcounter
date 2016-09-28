// This #include statement was automatically added by the Particle IDE.
#include "MQTT/MQTT.h"

// This #include statement was automatically added by the Particle IDE.
#include "FastLED/FastLED.h"
FASTLED_USING_NAMESPACE;

#define bitRead(value, bit) (((value) >> (bit)) & 0x01)

// Gradient palette "GPS_Nature_Grass_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/gps/tn/GPS-Nature-Grass.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 52 bytes of program space.

DEFINE_GRADIENT_PALETTE( GPS_Nature_Grass_gp ) {
    0, 109,203, 42,
   10,  83,139, 26,
   21,  60, 90, 14,
   31,  39, 91,  3,
   42,  22, 92,  1,
   53,  21, 75,  5,
   68,  20, 59, 21,
   83,  14, 49, 13,
  105,   9, 40,  7,
  133,   5, 27,  3,
  172,   3, 17,  1,
  205,   4, 14,  1,
  255,   5, 11,  1};
  
// Gradient palette "es_emerald_dragon_10_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/es/emerald_dragon/tn/es_emerald_dragon_10.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 12 bytes of program space.

DEFINE_GRADIENT_PALETTE( es_emerald_dragon_10_gp ) {
    0,   1, 150,  1,
  128,  79,213,  1,
  255, 1, 150, 1};




void callback(char* topic, byte* payload, unsigned int length);
void diplay_number(int n);
unsigned long number_array[10]={262143,63,2093560,2064895,1838655,2068423,2097095,511,2097151,1839103};


#define LED_PIN     5
#define COLOR_ORDER GRB
#define CHIPSET     WS2812B
#define NUM_LEDS    21

#define BRIGHTNESS  50
#define FRAMES_PER_SECOND 60

CRGB leds[NUM_LEDS];
CRGBPalette16 currentPalette;
TBlendType    currentBlending;
int sensorPin = A0;
int sensorValue = 0;

byte server[] = { 192,168,1,236 };
MQTT client(server, 1883, callback);
static uint8_t startIndex = 0;
char message_buff[100]; // this buffers our incoming messages so we can do something on certain commands
//unsigned long readTime;
unsigned int day;


void setup() {
    
  delay(3000); // sanity delay
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness( BRIGHTNESS );
  currentPalette = es_emerald_dragon_10_gp;
  // currentBlending = BLEND;
  client.connect("Photon1");
  if (client.isConnected()) {
        char myIPString[24];
        IPAddress myIP;
        myIP = WiFi.localIP();
        sprintf(myIPString, "%d.%d.%d.%d", myIP[0], myIP[1], myIP[2], myIP[3]);
        client.publish("photon1/test","Alive - subscribing to photon1/weeklycounter, publishing to photon1/LDR");
        client.publish("photon1/IP", myIPString);
        client.subscribe("photon1/weeklycounter");
    }
  // readTime = 0;
  day = EEPROM.read(1);
  if (day < 0 || day >6) { day = 0; EEPROM.write(1,0);}  // Invalid Value
  display_number(day);

}

void loop() {
    EVERY_N_SECONDS(30) { 
        sensorValue = analogRead(sensorPin);
        client.publish("photon1/LDR",String(sensorValue));
        }
    
    startIndex = startIndex + 1; 
    // random16_add_entropy( random(256));
    if (client.isConnected())
        client.loop();
    else
        reconnect();
    display_number(day);
    FastLED.show(); // display this frame
    FastLED.delay(1000 / FRAMES_PER_SECOND);
}



void callback(char* topic, byte* payload, unsigned int length) {
    char p[length + 1];
    memcpy(p, payload, length);
    p[length] = NULL;
    String message(p);

    if (message.equals("INC")) {   
        day++;
        if(day>6) day = 0;
        EEPROM.write(1,day);
        String dayString = String(day);
        client.publish("photon1/weeklycounter/status",dayString);
        display_number(day);
    }
    else if (message.equals("DEC")) {   
        if (day == 0) day = 6;
        else day--;
        EEPROM.write(1,day);
        String dayString = String(day);
        client.publish("photon1/weeklycounter/status",dayString);
        display_number(day);
    }
    // delay(1000);
}

#define COOLING  55
#define SPARKING 50

void display_number(int n){
    // static byte heat[NUM_LEDS];

  
    /*for( int i = 0; i < NUM_LEDS; i++) {
      heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / NUM_LEDS) + 2));
    }
    for( int k= NUM_LEDS - 1; k >= 2; k--) {
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
    }
    if( random8() < SPARKING ) {
      int y = random8(7);
      heat[y] = qadd8( heat[y], random8(160,255) );
    }
    for( int j = 0; j < NUM_LEDS; j++) {
        leds[j] = HeatColor( heat[j]);
    } */
    FillLEDsFromPaletteColors( startIndex);
    for(int l=0;l<NUM_LEDS;l++)
        if(bitRead(number_array[n],l)==0) leds[l]=CRGB::Black;
        //    else leds[l]=HeatColor( heat[l]);
    
     // FastLED.show(); 
}

void FillLEDsFromPaletteColors( uint8_t colorIndex)
{
  uint8_t brightness = 255;
  
  for( int i = 0; i < NUM_LEDS; i++) {
    leds[i] = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
    colorIndex += 3;
  }
}

void reconnect() {
  while (Spark.connected() == false) {
    Spark.connect();
    delay(1000);
  }//while (Spark.connected()
}//reconnect()
