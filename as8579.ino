/*
 * I and Q awaiting 16-bit SPI
 * SPI CLK frequency max 10MHz
 * CS active HIGH, MSB first, idle either high/low (SPI_MODE1 or 2) 
 * rising edge of SCLK: latch data in 
 * falling edge of SCLK: shift data out
 * 
 */
#include <SPI.h>

// präprozessor 
#define EDIVread 0x20 
#define EDIVwrite 0x30

const int chipSelectPin = 10;
uint16_t IQ[2] = {0, 1}; // array to store the I and Q values from the quick read function

void setup() {
  pinMode(chipSelectPin, OUTPUT);
  Serial.begin(9600);
  SPI.begin();
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
  delay(10);
  initialize();
  
}

void loop() {
  if (read_two_bytes_from_register(0x02) == 0){ // Wait for ADC complete Bit (Address: 0x05 / Value: 0x01)
    Serial.print(IQ[0]);
    Serial.print("\t");
    Serial.println(IQ[1]);
    quick_read(IQ);
  }
  
  delay(1000); 
}




// ---------------------- WRITE FUNCTION ----------------------------------------------------

void write_two_bytes_to_register(uint8_t address, uint8_t data1, uint8_t data2){
  // this function writes the two bytes into the address register 
  
  digitalWrite(chipSelectPin, HIGH); // activates the sensor 
  
  SPI.transfer(address);            // choose address
  SPI.transfer(data1);       // send first byte
  SPI.transfer(data2);       // send second byte

  digitalWrite(chipSelectPin, LOW); // deactivates the sensor 
}

// -------------------------------------------------------------------------------------------

// ------------------------ READ FUNCTION ----------------------------------------------------

uint16_t read_two_bytes_from_register(uint8_t address){
  // this function returns the two read bytes in one variable 
  
  uint8_t val1 = 0;
  uint8_t val2 = 0;
  uint16_t back = 0;

  digitalWrite(chipSelectPin, HIGH); // activates the sensor 
  
  SPI.transfer(address);            // choose address 
  val1 = SPI.transfer(0x00);        // catch returned byte
  val2 = SPI.transfer(0x00);        // catch returned byte 

  digitalWrite(chipSelectPin, LOW); // deactivates the sensor 

  back = val1 << 8;
  back = back | val2;
  return back; 
}

// --------------------------------------------------------------------------------------------

// -----------------QUICK READ FUNCTION -------------------------------------------------------
// This function reads the I and Q values and stores them in the provided array IQ

void quick_read(uint16_t IQ[]){
  uint8_t I_data1 = 0;
  uint8_t I_data2 = 0;
  uint8_t Q_data1 = 0;
  uint8_t Q_data2 = 0;
  uint16_t I = 0;
  uint16_t Q = 0;
  
  digitalWrite(chipSelectPin, HIGH); // activates the sensor 
  
  SPI.transfer(0x90);
  I_data1 = SPI.transfer(0x00);
  I_data2 = SPI.transfer(0x00);
  Q_data1 = SPI.transfer(0x00);
  Q_data2 = SPI.transfer(0x00); 

  digitalWrite(chipSelectPin, LOW); // deactivates the sensor 

  // write I value into IQ array 
  I = I_data1 << 8;
  I = I | I_data2;
  IQ[0] = I;

  // write Q value into IQ array 
  Q = Q_data1 << 8;
  Q = Q | Q_data2;
  IQ[1] = Q;
}

// --------------------------------------------------------------------------------------------

// --------------INITIALIZE FUNCTION ----------------------------------------------------------
// This function should be called in the setup once to make the necessary settings. 
 
 void initialize() {
  write_two_bytes_to_register(0x30, 0x00, 0x00);  // EDIV to 4MHz
  write_two_bytes_to_register(0x31, 0x00, 0x03);  // Frequency to 125kHz
  write_two_bytes_to_register(0x32, 0x00, 0x02);  // peak to peak to 1V
  write_two_bytes_to_register(0x33, 0x00, 0x01);  // SEN0 chosen by MUX 
  write_two_bytes_to_register(0x3A, 0x00, 0x28);  // reset
  write_two_bytes_to_register(0x3A, 0x00, 0x2A);  // start continuous conversion  
 }






  
