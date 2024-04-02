/*visauls
  XD-62B-TTP229-4x4-Keypad-16
  modified on 11 Nov 2020
  by Amir Mohammad Shojaee @ Electropeak
  Home

  based on forum.hobbycomponents.com examples
*/
#include <MIDI.h> // Add Midi Library
MIDI_CREATE_DEFAULT_INSTANCE();

#include <CurieBLE.h>

#define TXRX_BUF_LEN              20 //max number of bytes
#define RX_BUF_LEN                20 //max number of bytes
uint8_t rx_buf[RX_BUF_LEN];
int rx_buf_num, rx_state = 0;
uint8_t rx_temp_buf[20];
uint8_t outBufMidi[128];

//Buffer to hold 5 bytes of MIDI data. Note the timestamp is forced
uint8_t midiData[] = {0x80, 0x80, 0x00, 0x00, 0x00};
double sensorPin = A0;
double sensorValue = 0;  // variable to store the value coming from the sensor
double exprChanged_val = 0;
double normalize_value = 0;
double ratioVin = 0.12414446725;
int dicatorValue = 10;

//Loads up buffer with values for note On
void noteOn(char chan, char note, char vel) //channel 1
{
  midiData[2] = 0x90 + chan;
  midiData[3] = note;
  midiData[4] = vel;
}

//Loads up buffer with values for note On
void ctrlChange(char chan, char ctrlParamter, char vel) //channel 1
{
  midiData[2] = 0xB0 + chan;
  midiData[3] = ctrlParamter;
  midiData[4] = vel;
}
//Loads up buffer with values for note Off
void noteOff(char chan, char note) //channel 1
{
  midiData[2] = 0x80 + chan;
  midiData[3] = note;
  midiData[4] = 0;
}
unsigned long timer[8];
int switch_state[8];
int previous_Val[8];
int store_midi_value[8];
#define LED 13
int sensorVal[8];
int midivalue = 0;
bool BreakIteration = false;
//BLEPeripheral midiDevice; // create peripheral instance

BLEService midiSvc("03B80E5A-EDE8-4B33-A751-6CE34EC4C700"); // create service

// create switch characteristic and allow remote device to read and write
BLECharacteristic midiChar("7772E5DB-3868-4112-A1A9-F2669D106BF3",  BLEWrite | BLEWriteWithoutResponse | BLENotify | BLERead, 5);
void BLESetup()
{
  BLE.begin();
  // set the local name peripheral advertises
  BLE.setLocalName("Auxren");

  // set the UUID for the service this peripheral advertises
  BLE.setAdvertisedServiceUuid(midiSvc.uuid());

  // add service and characteristic

  midiSvc.addCharacteristic(midiChar);
  BLE.addService(midiSvc);

  // assign event handlers for connected, disconnected to peripheral
  BLE.setEventHandler(BLEConnected, midiDeviceConnectHandler);
  BLE.setEventHandler(BLEDisconnected, midiDeviceDisconnectHandler);

  // assign event handlers for characteristic
  midiChar.setEventHandler(BLEWritten, midiCharacteristicWritten);
  // set an initial value for the characteristic
  midiChar.setValue(midiData, 5);




}


/* Define the digital pins used for the clock and data */
#define SCL_PIN 10
#define SDO_PIN 11
#define DEBOUNCE 100
/* Used to store the key state */
byte Key;

void setup()
{


  MIDI.begin();

  pinMode(LED, OUTPUT);
  for (int i = 0; i <= 8; i++) {
    switch_state[i] = HIGH;
    previous_Val[i] = HIGH;
  }
  Serial.println(("initialise array"));


  pinMode(0, INPUT_PULLUP);
  pinMode(1, INPUT_PULLUP);
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);
  /* Initialise the serial interface */
  Serial.begin(115200);
  /* Configure the clock and data pins */
  pinMode(SCL_PIN, OUTPUT);
  pinMode(SDO_PIN, INPUT);
  BLESetup();
  // advertise the service
  BLE.advertise();
  Serial.println(("Bluetooth device active, waiting for connections..."));
  delay(5000);
  BlinkLed() ;
}


byte commandByte;
byte noteByte;
byte velocityByte;





/* Main program */
void loop()
{

  do {
    if (Serial.available()) {
      commandByte = Serial.read();//read first byte
      noteByte = Serial.read();//read next byte
      velocityByte = Serial.read();//read final byte
      if (commandByte == 144)

      {


        noteOn(0, int(noteByte), int(velocityByte)); //loads up midiData buffer
        midiChar.setValue(midiData, 5);//midiData); //posts 5 bytes // If it is a Program Change,
        BlinkLed();  // blink the LED a number of times
        //                                            // correponding to the program number
        //                                            // (0 to 127, it can last a while..)

      }

      if (commandByte == 128)
      {
        noteOff(0, int(noteByte)); //loads up midiData buffer
        midiChar.setValue(midiData, 5);//midiData); //posts 5 bytes // If it is a Program Change,
        BlinkLed();  // blink the LED a number of times
        //                                            // correponding to the program number
        //                                            // (0 to 127, it can last a while..)


      }


    }
  }
  while (Serial.available() > 2);//when at least three bytes available


  /* Read the current state of the keypad */

  BLE.poll();
  delay(1);


  sensorValue = analogRead(sensorPin);
  //midi for expr pedal
  if (abs(exprChanged_val - sensorValue) > 50)
  {


    exprChanged_val = sensorValue;
    if (exprChanged_val < 50)
    {
      exprChanged_val = 100; //for the dictactor to stat

    }
    if (exprChanged_val > 950)

    {
      exprChanged_val = 1024;

    }
    normalize_value =  (int)abs(constrain(exprChanged_val * ratioVin, 0, 127));
    if (dicatorValue != normalize_value) {
      Serial.print("sensor : ");
      Serial.print(sensorValue);
      Serial.print("  ctrl :");
      Serial.println(normalize_value);
      BlinkLed();
      //delay(100);
      dicatorValue = normalize_value;
      ctrlChange(0, 100, normalize_value); //loads up midiData buffer
      midiChar.setValue(midiData, 5);//midiData); //posts 5 bytes

    }

  }







}//end loop

void BlinkLed()         // Basic blink function
{

  digitalWrite(LED, HIGH);
  delay(1);
  digitalWrite(LED, LOW);
  //delay(5);

}
/**Breakiteratyion on pressed another key*/
bool F_breakIteration(void) {

  for (int i = 0; i <= 7; i++)
  {
    sensorVal[i] = digitalRead(i);
    if (previous_Val[i] != sensorVal[i]) {
      return true;
      Serial.println("Break iterartion");
    }
  }
  return false;
}
/* Read the state of the keypad */
byte Read_Keypad(void)
{
  byte Count;
  byte Key_State = 0;

  /* Pulse the clock pin 16 times (one for each key of the keypad)
     and read the state of the data pin on each pulse */
  for (Count = 1; Count <= 16; Count++)
  {
    digitalWrite(SCL_PIN, LOW);

    /* If the data pin is low (active low mode) then store the
       current key number */
    if (!digitalRead(SDO_PIN))
      Key_State = Count;

    digitalWrite(SCL_PIN, HIGH);
  }

  return Key_State;
}




void midiDeviceConnectHandler(BLEDevice central) {
  // central connected event handler
  Serial.print("Connected event, central: ");
  Serial.println(central.address());
}

void midiDeviceDisconnectHandler(BLEDevice central) {
  // central disconnected event handler
  Serial.print("Disconnected event, central: ");
  Serial.println(central.address());
}

void midiCharacteristicWritten(BLEDevice central, BLECharacteristic characteristic) {
  // central wrote new value to characteristic, update LED
  Serial.print("Characteristic event, written: ");
}