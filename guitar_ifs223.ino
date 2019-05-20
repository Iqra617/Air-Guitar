#include <Wire.h>            //library to communicate with I2C devices
#include <Adafruit_Sensor.h> //sensor library
#include <Adafruit_ADXL343.h> //Adafruit's accelerometer library
#include <Audio.h>
#include <SD.h>
#include <SPI.h>
#include <SerialFlash.h>

#define NOTE_E4  329.63
#define NOTE_F4  349.23
#define NOTE_C4  261.63
#define NOTE_B3  246.94
#define NOTE_G3  196.00
#define NOTE_A3  220.00
#define NOTE_E3  164.81
#define NOTE_F3  174.61
#define NOTE_D3  146.83
#define NOTE_C3  130.81
#define NOTE_A2  110.00
#define NOTE_B2  123.47
#define NOTE_G2   98.00


int16_t pos_x=0;        //x co-ordinate for accelerometer's position on x axis
int16_t pos_y=0;        //y co-ordinate for accelerometer's position on y axis
int16_t pos_z=0;        //z co-ordinate for accelerometer's position on z axis

const int hand_delay = 220;

Adafruit_ADXL343 accel = Adafruit_ADXL343(12345);   // declare an object of the class  Adafruit_ADXL343



// according to http://www.guitar-chords.org.uk/
// and http://www.8notes.com/guitar_chord_chart/c.asp
const float Cmajor[6] = {      0, NOTE_C3, NOTE_E3, NOTE_G3, NOTE_C4, NOTE_E4};  // C - E - G
const float Gmajor[6] = {NOTE_G2, NOTE_B2, NOTE_D3, NOTE_G3, NOTE_B3, NOTE_E4};  // G - B - D
const float Fmajor[6] = {      0, NOTE_A2, NOTE_F3, NOTE_A3, NOTE_C4, NOTE_F4};  // F - A - C
const float Aminor[6] = {      0, NOTE_A2, NOTE_E3, NOTE_A3, NOTE_C4, NOTE_E4};  // A - C - E

AudioSynthKarplusStrong  string1;
AudioSynthKarplusStrong  string2;
AudioSynthKarplusStrong  string3;
AudioSynthKarplusStrong  string4;
AudioSynthKarplusStrong  string5;
AudioSynthKarplusStrong  string6;
AudioMixer4              mixer1;
AudioMixer4              mixer2;
AudioControlSGTL5000     sgtl5000_1;
AudioOutputI2S           i2s1;
AudioConnection          patchCord1(string1, 0, mixer1, 0);
AudioConnection          patchCord2(string2, 0, mixer1, 1);
AudioConnection          patchCord3(string3, 0, mixer1, 2);
AudioConnection          patchCord4(string4, 0, mixer1, 3);
AudioConnection          patchCord5(mixer1, 0, mixer2, 0);
AudioConnection          patchCord6(string5, 0, mixer2, 1);
AudioConnection          patchCord7(string6, 0, mixer2, 2);
AudioConnection          patchCord8(mixer2, 0, i2s1, 0);
AudioConnection          patchCord9(mixer2, 0, i2s1, 1);





void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Accelerometer Test"); Serial.println("");

  /* Initialise the sensor */
  if(!accel.begin())
  {
    // There was a problem detecting the ADXL343
    Serial.println("No ADXL343 detected ... Check your wiring!");
    while(1);
  }

  // Set the range , The lower range gives more resolution for slow movements, the higher range is good for high speed tracking
  //sensitivity level cab be set to either +-2g, +-4g, +-8g or +-16g.
  
  accel.setRange(ADXL343_RANGE_4_G);
  
  // accel.setRange(ADXL343_RANGE_2_G);
  //accel.setRange(ADXL343_RANGE_16_G);      
   //accel.setRange(ADXL343_RANGE_8_G);

  AudioMemory(15);
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.6);
  mixer1.gain(0, 0.15);
  mixer1.gain(1, 0.15);
  mixer1.gain(2, 0.15);
  mixer1.gain(3, 0.15);
  mixer2.gain(1, 0.15);
  mixer2.gain(2, 0.15);
  delay(700);
}


void loop() {
  // put your main code here, to run repeatedly:
  pos_x = accel.getX();    //Gets the most recent X axis value. 
  pos_y=accel.getY();      //Gets the most recent Y axis value. 
  pos_z=accel.getZ();       //Gets the most recent Z axis value. 
  Serial.println(pos_x);
  Serial.println(pos_y);
  Serial.println(pos_z);
  delay(200);

  const float *chord;

  // each time through the loop, play a different chord depending on the (x,y,z coordinates)
  if ( (pos_x<0) && (pos_y<0) && (pos_z < 0 )  ) {
    chord = Cmajor;
    Serial.println("C major");
  } else if ( (pos_x>0) && (pos_y<0) && (pos_z < 0)  ) {
    chord = Gmajor;
    Serial.println("G major");
  } else if ( (pos_x<0) && (pos_y>0) && (pos_z < 0 )) {
    chord = Fmajor;
    Serial.println("F major");
  } else if((pos_x>0) && (pos_y>0) && (pos_z > 0 )) {    //(pos_x<0) && (pos_y<0) && (pos_z > 0 
    chord = Aminor;
    Serial.println("A minor");
  }

  // then strum the string up and down
  strum_up(chord, 1.0);
  delay(hand_delay);
  delay(hand_delay);
  strum_dn(chord, 0.8);
  delay(hand_delay);
  delay(hand_delay);

}
void strum_up(const float *chord, float velocity)
{
  if (chord[0] > 20.0) string1.noteOn(chord[0], velocity);
  delay(hand_delay);
  if (chord[1] > 20.0) string2.noteOn(chord[1], velocity);
  delay(hand_delay);
  if (chord[2] > 20.0) string3.noteOn(chord[2], velocity);
  delay(hand_delay);
  if (chord[3] > 20.0) string4.noteOn(chord[3], velocity);
  delay(hand_delay);
  if (chord[4] > 20.0) string5.noteOn(chord[4], velocity);
  delay(hand_delay);
  if (chord[5] > 20.0) string6.noteOn(chord[5], velocity);
  delay(hand_delay);
}

void strum_dn(const float *chord, float velocity)
{
  if (chord[5] > 20.0) string1.noteOn(chord[5], velocity);
  delay(hand_delay);
  if (chord[4] > 20.0) string2.noteOn(chord[4], velocity);
  delay(hand_delay);
  if (chord[3] > 20.0) string3.noteOn(chord[3], velocity);
  delay(hand_delay);
  if (chord[2] > 20.0) string4.noteOn(chord[2], velocity);
  delay(hand_delay);
  if (chord[1] > 20.0) string5.noteOn(chord[1], velocity);
  delay(hand_delay);
  if (chord[0] > 20.0) string6.noteOn(chord[0], velocity);
  delay(hand_delay);
}
