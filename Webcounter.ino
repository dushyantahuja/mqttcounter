// This #include statement was automatically added by the Particle IDE.
#include "MQTT/MQTT.h"

// This #include statement was automatically added by the Particle IDE.
#include "FastLED/FastLED.h"
FASTLED_USING_NAMESPACE;

#define bitRead(value, bit) (((value) >> (bit)) & 0x01)


void callback(char* topic, byte* payload, unsigned int length);
void diplay_number(int n);
unsigned long number_array[10]={262143,63,2093560,2064895,1838655,2068423,2097095,511,2097151,1839103};


#define LED_PIN     5
#define COLOR_ORDER GRB
#define CHIPSET     WS2812B
#define NUM_LEDS    21

#define BRIGHTNESS  100
#define FRAMES_PER_SECOND 60

CRGB leds[NUM_LEDS];
CRGBPalette16 currentPalette;
TBlendType    currentBlending;

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
  currentPalette = RainbowColors_p;
  // currentBlending = BLEND;
  client.connect("Photon1");
  if (client.isConnected()) {
        client.publish("Photon-Test","Alive");
        client.subscribe("photon1/weeklycounter");
    }
  // readTime = 0;
  day = EEPROM.read(1);
  if (day < 0 || day >7) { day = 0; EEPROM.write(1,0);}  // Invalid Value
  display_number(day);

}

void loop() {
    
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
        if(day>7) day = 0;
        EEPROM.write(1,day);
        String dayString = String(day);
        client.publish("photon1/weeklycounter/status",dayString);
        display_number(day);
    }
    else if (message.equals("DEC")) {   
        if (day == 0) day = 7;
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
    static byte heat[NUM_LEDS];

  
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
