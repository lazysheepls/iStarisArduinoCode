#include <SoftwareSerial.h>  //Do we still need it?
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>


/* RFID card reader Pin layout*/
const byte rxPin_SOFT = 4;
const byte txPin_SOFT = 5;
// set up a new serial object
SoftwareSerial RFIDSerial (rxPin_SOFT, txPin_SOFT);
unsigned char buffer[64];
int count=0;
boolean queryFlag = false;
boolean hold = false;

/* PIR Sensor Pin layout*/
#define PIR_MOTION_SENSOR 2  // Uno pin 2 for interrupt

/* Button Pin layout*/
#define PUSH_BUTTON 3       // Uno pin 3 for interrupt

/*Display Pin layout*/
// Software SPI (slower updates, more flexible pin options):
// pin 12 - Serial clock out (SCLK)
// pin 11 - Serial data out (DIN)
// pin 10 - Data/Command select (D/C)
// pin 9 - LCD chip select (CS)
// pin 8 - LCD reset (RST)
Adafruit_PCD8544 display = Adafruit_PCD8544(12, 11, 10, 9, 8);

// Welcome page
// Bitmap sizes for welcomePage
const int welcomePageWidthPages = 10*8;
const int welcomePageHeightPixels = 30;

static const unsigned char PROGMEM welcomePageBitmaps[] =
{
  B00000000, B00000000, B00000000, B11111111, B11000000, B00000000, B00000000, B00000000, B00000000, B00000000, //                         ##########                                           
  B00111111, B11000000, B00000000, B11111111, B11000000, B00000000, B00000000, B00000000, B00000000, B00000000, //   ########              ##########                                           
  B00111111, B11000000, B00000000, B11111111, B11000000, B00000000, B00000000, B00000000, B00000000, B00000000, //   ########              ##########                                           
  B00011111, B11000000, B00000000, B11110000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, //    #######              ####                                                 
  B00001111, B11000000, B00000000, B11110000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, //     ######              ####                                                 
  B00011111, B11000000, B00000000, B11110000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, //    #######              ####                                                 
  B00111111, B11000000, B00000000, B11110000, B00011111, B11000000, B00000000, B00000000, B00000000, B00000000, //   ########              ####       #######                                   
  B01111011, B11000000, B00111111, B11110000, B00010000, B00000000, B00000000, B00000000, B00000000, B00000000, //  #### ####        ##########       #                                         
  B11110011, B11000000, B00111111, B11110000, B00010000, B00000000, B00000000, B00000000, B00000000, B00000000, // ####  ####        ##########       #                                         
  B11100000, B00000000, B00111111, B11110000, B00010000, B00000000, B00000000, B00000000, B00000000, B00000000, // ###               ##########       #                                         
  B11000000, B00000000, B00111000, B00000000, B00010000, B00000000, B00000000, B00000000, B00000000, B00000000, // ##                ###              #                                         
  B10000000, B00000000, B00111000, B00000000, B00010000, B11000111, B11000000, B00000000, B01100000, B00000000, // #                 ###              #    ##   #####               ##          
  B00000000, B00000000, B00111000, B00000000, B00010000, B11001100, B01100110, B00000000, B01100000, B00000000, //                   ###              #    ##  ##   ##  ##          ##          
  B00000000, B00001111, B11111000, B00000000, B00010000, B00001100, B01100110, B00000000, B00000000, B00000000, //             #########              #        ##   ##  ##                      
  B00000000, B00001111, B11111000, B00000000, B00000000, B11001100, B00001111, B00111100, B01100111, B10000000, //             #########                   ##  ##      ####  ####   ##  ####    
  B00000000, B00001111, B11111000, B00000000, B00000000, B11000111, B00000110, B01100110, B01100111, B00000000, //             #########                   ##   ###     ##  ##  ##  ##  ###     
  B00000000, B00001111, B11111000, B00000000, B00000000, B11000001, B11000110, B00011110, B01100110, B00000000, //             #########                   ##     ###   ##    ####  ##  ##      
  B00000000, B00001110, B00000000, B00000000, B00000000, B11000000, B01100110, B00110110, B01100110, B00000000, //             ###                         ##       ##  ##   ## ##  ##  ##      
  B00000000, B00001110, B00000000, B00000000, B00000000, B11001100, B01100110, B01100110, B01100110, B00001000, //             ###                         ##  ##   ##  ##  ##  ##  ##  ##     #
  B00000000, B00001110, B00000000, B00000000, B00000000, B11001100, B01100110, B01100110, B01100110, B00001000, //             ###                         ##  ##   ##  ##  ##  ##  ##  ##     #
  B00000111, B11111110, B00000000, B00000000, B00000000, B11000111, B11000011, B00111110, B01100110, B00001000, //      ##########                         ##   #####    ##  #####  ##  ##     #
  B00000111, B11111110, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00001000, //      ##########                                                             #
  B00000111, B11111110, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00001000, //      ##########                                                             #
  B00000111, B10000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00001000, //      ####                                                                   #
  B00000111, B10000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00001000, //      ####                                                                   #
  B00000111, B10000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00001000, //      ####                                                                   #
  B00000111, B10000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000011, B11111000, //      ####                                                             #######
  B11111111, B10000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, // #########                                                                    
  B11111111, B10000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, // #########                                                                    
  B11111111, B10000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, // #########                                                                    
                                                                    
};


// Selfie Image
// Bitmap sizes for selfieTime
const int selfieTimeWidthPages = 7*8;
const int selfieTimeHeightPixels = 42;

static const unsigned char PROGMEM selfieBMP[] = {
  B01111000, B00000100, B10000000, B00001111, B10000000, B00000000, B00100100, //  ####        #  #           #####                 #  #
  B10000100, B00000101, B00100000, B00000010, B00100000, B00000000, B00100100, // #    #       # #  #           #   #               #  #
  B10000000, B00000101, B00000000, B00000010, B00000000, B00000000, B00100100, // #            # #              #                   #  #
  B01111000, B11100101, B10100111, B00000010, B00101111, B11000111, B00100100, //  ####   ###  # ## #  ###      #   # ######   ###  #  #
  B00000101, B00010101, B00101000, B10000010, B00101001, B00101000, B10100100, //      # #   # # #  # #   #     #   # #  #  # #   # #  #
  B00000101, B11110101, B00101111, B10000010, B00101001, B00101111, B10000000, //      # ##### # #  # #####     #   # #  #  # #####     
  B10000101, B00000101, B00101000, B00000010, B00101001, B00101000, B00100100, // #    # #     # #  # #         #   # #  #  # #     #  #
  B01111000, B11100101, B00100111, B00000010, B00101001, B00100111, B00000000, //  ####   ###  # #  #  ###      #   # #  #  #  ###      
  B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, //                                                       
  B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, //                                                       
  B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, //                                                       
  B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, //                                                       
  B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, //                                                       
  B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, //                                                       
  B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, //                                                       
  B00000000, B00000000, B00000001, B11111111, B11000000, B00000000, B00000000, //                        ###########                    
  B00000000, B00000000, B00000011, B11111111, B11110000, B00000000, B00000000, //                       ##############                  
  B00000000, B00000000, B00000011, B00000000, B01110000, B00000000, B00000000, //                       ##         ###                  
  B00000000, B00000011, B11110111, B00000100, B00110000, B00000000, B00000000, //               ###### ###     #    ##                  
  B00000000, B00111111, B11111111, B00001000, B00111111, B11100000, B00000000, //           ##############    #     #########           
  B00000000, B01010011, B00011111, B11111111, B11111111, B11010000, B00000000, //          # #  ##   ####################### #          
  B00000000, B01100000, B10111010, B01111111, B10111111, B11110000, B00000000, //          ##     # ### #  ######## ##########          
  B00000000, B01100000, B10111111, B11111111, B11111111, B11110000, B00000000, //          ##     # ##########################          
  B00000000, B01100000, B10001111, B10000000, B01111111, B11110000, B00000000, //          ##     #   #####        ###########          
  B00000000, B01100000, B10111110, B11011110, B00011111, B11110000, B00000000, //          ##     # ##### ## ####    #########          
  B00000000, B01100010, B10011101, B11111111, B10001111, B11110000, B00000000, //          ##   # #  ### ##########   ########          
  B00000000, B01100000, B10111001, B11111111, B11000111, B00110000, B00000000, //          ##     # ###  ###########   ###  ##          
  B00000000, B01100000, B11111011, B11111100, B00110011, B00110000, B00000000, //          ##     ##### ########    ##  ##  ##          
  B00000000, B01100000, B11111011, B01111000, B00111011, B00110000, B00000000, //          ##     ##### ## ####     ### ##  ##          
  B00000000, B01101000, B11110010, B00011100, B00010011, B10110000, B00000000, //          ## #   ####  #    ###     #  ### ##          
  B00000000, B01100111, B11110100, B00110010, B00011111, B11110000, B00000000, //          ##  ####### #    ##  #    #########          
  B00000000, B01101110, B11110100, B00110011, B00011011, B11110000, B00000000, //          ## ### #### #    ##  ##   ## ######          
  B00000000, B01101000, B11110010, B00011110, B00110111, B11110000, B00000000, //          ## #   ####  #    ####   ## #######          
  B00000000, B01101001, B11010010, B00000000, B00111111, B11110000, B00000000, //          ## #  ### #  #           ##########          
  B00000000, B01100000, B11111111, B00000100, B01111111, B11110000, B00000000, //          ##     ########     #   ###########          
  B00000000, B01100000, B11111111, B10000110, B11101110, B10110000, B00000000, //          ##     #########    ## ### ### # ##          
  B00000000, B01100000, B10111100, B01100011, B11111111, B10110000, B00000000, //          ##     # ####   ##   ########### ##          
  B00000000, B01111100, B10111110, B11011110, B10111111, B11110000, B00000000, //          #####  # ##### ## #### # ##########          
  B00000000, B01110100, B11011111, B10000001, B11111111, B11110000, B00000000, //          ### #  ## ######      #############          
  B00000000, B01100000, B11111011, B11111111, B10111111, B11110000, B00000000, //          ##     ##### ########### ##########          
  B00000000, B01111111, B11111100, B11111111, B11111111, B11110000, B00000000, //          #############  ####################          
  B00000000, B00111111, B11111111, B11111111, B11111111, B11100000, B00000000, //           #################################           
};
  
void setup() {
  // serial port for display
  Serial.begin(9600);
  RFIDSerial.begin(9600);

  displayInit();
  PIRInit();
  PushButtonInit();
}

// Define varibles
int incomingByte =0;
int incomingNum = 0;   // For serial input to the display
int displayNum=0;     // Number sending to nokia display
String incomingData;

void loop() {
  /*For Display*/
  while(Serial.available()>0){
    incomingData = String(Serial.read());   // Convert the input data to ascii
    Serial.println(incomingData);
    
    // Check for serial input: For display
    incomingNum = incomingData.toInt();
    if(incomingNum>=48 && incomingNum<=57){ // Check if the input is from 0~9
      displayNum = incomingNum-48;
      displayNokia(displayNum);
    }
    else{
      displayWelcomePage();
    }
  }
  /*For RDIF reader*/
   while(RFIDSerial.available()){
      buffer[count++]=RFIDSerial.read();
      if(count == 64){
        Serial.print("RFIDdata:");   //When it read the RFID data, printed through serial line
        Serial.write(buffer,count);
        clearBufferArray();
        count = 0;
        break;
      }
   }
}
/**********************************/
void displayInit(void){
  // Display setting
  display.begin();
  display.setContrast(50);
  display.clearDisplay();

  // Set text size
  display.setTextSize(6);
  display.setTextColor(BLACK);

  // Display welcome page
  displayWelcomePage();
}
/**********************************/
void PIRInit(void){
  pinMode(PIR_MOTION_SENSOR, INPUT);
  attachInterrupt(digitalPinToInterrupt(PIR_MOTION_SENSOR), UpdataPIR, RISING);
}
void UpdataPIR(){// Interrupt service routing ISR for PIR sensor
    Serial.println("PIRon");
}
/**********************************/
void PushButtonInit(void){
  pinMode(PUSH_BUTTON,OUTPUT);
  attachInterrupt(digitalPinToInterrupt(PUSH_BUTTON), UpdataPUSHBUTTON, RISING);
}
void UpdataPUSHBUTTON(void){
  Serial.println("BTon");
}
/**********************************/
void displayNokia(int inNum){
    display.clearDisplay();
    // Reminder to take selfie
    if(inNum == 9){
      display.clearDisplay();
      display.drawBitmap(14, 4,  selfieBMP, selfieTimeWidthPages, selfieTimeWidthPages, 1);
      display.display();
    }
    // Normal display from 0-8
    else{
      display.setCursor(24,3);
      display.println(String(inNum));
      display.display();
    }
}
void displayWelcomePage(void){
  display.clearDisplay();
  display.drawBitmap(1, 9,  welcomePageBitmaps, welcomePageWidthPages, welcomePageHeightPixels, 1);
  display.display();
}
/**********************************/
void clearBufferArray(){
  for (int i=0;i<count;i++)
    buffer[i]=NULL;
}
