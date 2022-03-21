/*
 * I and Q awaiting 16-bit SPI
 * SPI CLK frequency max 10MHz
 * CS active HIGH, MSB first, idle either high/low (SPI_MODE0) 
 * rising edge of SCLK: latch data in 
 * falling edge of SCLK: shift data out
 * 
 */
#include <SPI.h>

const int chipSelectPin = 10;   
uint16_t IQ[2] = {0, 0};  // array to store the I and Q values from the quick read function


void setup() {
  pinMode(chipSelectPin, OUTPUT);
  Serial.begin(115200);
  SPI.begin();
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
  delay(100);
  initialize();  
}


void loop() {
  while (read_two_bytes_from_register(0x05) != 1){ // Wait until ADC start is complete (Address: 0x05 / Value: 0x01)
    Serial.println("Waiting for ADC to be ready");
    delay(100);
  }
  quick_read(IQ);
  Serial.print("I: ");
  Serial.print(IQ[0]);
  Serial.print("\t Q: ");
  Serial.println(IQ[1]); 
  delay(50);
}


// ---------------------- WRITE FUNCTION ----------------------------------------------------

void write_two_bytes_to_register(uint8_t address, uint8_t data1, uint8_t data2){
  // this function writes two bytes into the address register 
  
  digitalWrite(chipSelectPin, HIGH);  // activate the sensor 
  
  SPI.transfer(address);              // choose address
  SPI.transfer(data1);                // send first byte
  SPI.transfer(data2);                // send second byte

  digitalWrite(chipSelectPin, LOW);   // deactivate the sensor 
}

// -------------------------------------------------------------------------------------------

// ------------------------ READ FUNCTION ----------------------------------------------------

uint16_t read_two_bytes_from_register(uint8_t address){
  // this function returns the two read bytes as a uint16_t  
  
  uint8_t val1 = 0;
  uint8_t val2 = 0;
  uint16_t back = 0;

  digitalWrite(chipSelectPin, HIGH); // activate the sensor 
  
  SPI.transfer(address);            // choose address 
  val1 = SPI.transfer(0x00);        // catch returned byte
  val2 = SPI.transfer(0x00);        // catch returned byte 

  digitalWrite(chipSelectPin, LOW); // deactivate the sensor 

  back = val1 << 8;                 // shift by 8 bits to left
  back = back | val2;               // OR operand 
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
// The "minimum settings", the "soft reset" and the "start sensing" have definitely to be made. 
// (Compare datasheet: SPI Initialization)
// With this initialization you shoud be able to read some I and Q values. 
// The additional settings are then used to make the I and Q values useful.
// Start experimenting with the PGA offset to get the I and Q values out of saturation. 
//
// (You can always read the status register (0x20) to see possible mistakes)
 
 void initialize() {
  
  // minimum settings 
  write_two_bytes_to_register(0x30, 0x00, 0x00);  // EDIV: set system CLK   (here: 4MHz)
  write_two_bytes_to_register(0x31, 0x00, 0x00);  // Set frequency          (here: 45.45kHz) 
  write_two_bytes_to_register(0x32, 0x00, 0x02);  // Set peak to peak       (here: 1V)
  write_two_bytes_to_register(0x33, 0x00, 0x01);  // Set MUX                (here: SEN0 line, output driver) 
  
  // additional settings 
  //write_two_bytes_to_register(0x34, 0x00, 0x00);  // Set VAR_SEN
  //write_two_bytes_to_register(0x35, 0x00, 0x00);  // Set Current Buffer Gain
  write_two_bytes_to_register(0x37, 0x00, 0x00);  // Set PGA gain           (here: 36)
  write_two_bytes_to_register(0x38, 0x00, 0x60);  // Set PGA I offset       (here: 0x60)
  write_two_bytes_to_register(0x39, 0x00, 0x90);  // Set PGA Q offset       (here: 0x90)
  
  // soft reset 
  write_two_bytes_to_register(0x3A, 0x00, 0x28);  // reset DSP and start ADC

  delay(100);
  
  // start sensing
  write_two_bytes_to_register(0x3A, 0x00, 0x2A);  // start continuous conversion  
 }






  
