#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"
#include <Keypad.h>



//Keyboard
const byte ROWS = 6; //four rows
const byte COLS = 10; //four columns
//define the cymbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
   {'X', 'K','L','M','N','O','P', 'Q','X','X'},
   {'a', 'b','c','d','e','f','g', 'h','i','j'},
   {'k', 'l','m','n','o','p','q', 'r','s','t'},
   {'0', '1','2','3','4','5','6', '7','8','9'},
   {'u', 'v','w','x','y','z','A', 'B','X','X'},
   {'C', 'D','E','F','G','H','I', 'J','X','X'}};
byte rowPins[ROWS] ={A0,A1,A2,A3,A4, A5}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {11, 10, 9, 8,7,6,5,4,3,2};         //connect to the column pinouts of the keypad - Strich von Diode

//Serial
char InputStringCharArray[100]; //InputBuffer
String InputString;
char terminationChar = '/';

//LEDS
//Location of Leds
uint8_t AUX[10][2] = {{1, 0},{1, 1},{1, 2},{1, 3},{1, 4},{1, 5},{1, 6},{1, 7},{1, 8},{1, 9}};
uint8_t PROGRAM[10][2] = {{2, 0},{2, 1},{2, 2},{2, 3},{2, 4},{2, 5},{2, 6},{2, 7},{2, 8},{2, 9}};
uint8_t PREVIEW[10][2] = {{3, 0},{3, 1},{3, 2},{3, 3},{3, 4},{3, 5},{3, 6},{3, 7},{3, 8},{3, 9}};    
uint8_t SPECIAL[10][2] = {{0, 1},{0, 2},{0, 2},{0, 3},{0, 4},{0, 5},{0, 6}, {0, 7},{0, 8},{0, 9}};
uint8_t FUNCTION[10][2] = {{5, 0},{0, 1},{0, 2},{0, 3},{0, 4},{0, 5},{0, 6},{0, 7},{0, 8},{0, 9}};
uint8_t KEYER[10][2] = {{0, 1},{0, 1},{0, 2},{0, 3},{0, 4},{0, 5},{0, 6},{0, 7},{0, 8},{0, 9}};

//------------don't adjust------------
//Serial
int currentChar = 0;
bool newInput = false;
//Keypad
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
//Matrix
Adafruit_LEDBackpack matrix = Adafruit_LEDBackpack();

//------------Program------------
void setup()
{
  Serial.begin(115200);
  matrix.begin(0x70);
  InputString = InputStringCharArray;
}

void loop()
{
  bool newInput = readSerial();
  if (newInput)
  {
    handleInput();
  }
  char customKey = customKeypad.getKey();
  if (customKey)
  {
    sendKeyPress(customKey);
  }
}

//------------Functions------------
bool readSerial()
{
  while (Serial.available())
  {
    InputStringCharArray[currentChar] = Serial.read();
    if (InputStringCharArray[currentChar] == terminationChar || InputStringCharArray[currentChar] == '\n')
    {
      InputStringCharArray[currentChar] = '\0';
      InputString = InputStringCharArray;
      Serial.println(InputString);
      currentChar = 0;
      return true;
    }
    else
    {
      currentChar++;
      if (currentChar >= sizeof(InputStringCharArray))
      {
        currentChar = 0;
        Serial.println("Error: String To Long");
      }
    }
  }
  return false;
}

void handleInput()
{
  if (strncmp(InputStringCharArray, "PREVIEW_", 8) == 0)
  {
    handleCommand(PREVIEW, 8, true);
  }
  else if(strncmp(InputStringCharArray, "PROGRAM_", 8) == 0)
  {
    handleCommand(PROGRAM, 8, true);
  }
  else if(strncmp(InputStringCharArray, "FUNCTION_", 10) == 0)
  {
    handleCommand(FUNCTION, 10, false);
  }
  else if(strncmp(InputStringCharArray, "SPECIAL_", 8) == 0)
  {
    handleCommand(SPECIAL, 8, false);
  }
  else if (strncmp(InputStringCharArray, "AUX_", 4) == 0)
  {
    handleCommand(AUX, 4, true);
  }
  else if (strncmp(InputStringCharArray, "KEYER_", 6) == 0)
  {
    handleCommand(KEYER, 6, false);
  }
  else
  {
    Serial.println(String("Error: Wrong String => ") + InputString);
    Serial.println(InputString);
  }
  matrix.writeDisplay();
}

void handleCommand(uint8_t location[][2], uint8_t position, bool turnRowOff)
{
  if(turnRowOff){
    
      matrix.displaybuffer[location[0][0]] =  0;
    
  }
  
  int positionX = InputStringCharArray[position] - '0';
  if (strncmp(&InputString[position + 2], "ON", 2) == 0)
  {
    //0000 0001 bitshift than OR Operation
    uint16_t bits = 1;
    bits = bits << location[positionX][1];
    matrix.displaybuffer[location[positionX][0]] |= bits;
    //DEBUG_________________________________________________
    //Turn on whole row
    //matrix.displaybuffer[PREVIEW[positionX][0]] = 65535;
    //turn on whole matrix
    //for(int i= 0; i< 8; i++){
    //matrix.displaybuffer[i]=65535;
    //}
  }
  else if(strncmp(&InputString[position+2], "OFF", 3) == 0)
  {
    //1111 1110 bitshift than AND Operation
    uint8_t bits = 1;
    bits = bits << location[positionX][1];
    bits = ~bits;
    matrix.displaybuffer[location[positionX][0]] &= bits;
  }
  else
  {
    Serial.println(String("Error: Wrong Command => ") + InputString);
  }
}

void sendKeyPress(char c)
{
  switch (c)
  {
  case 'a':
    Serial.println("AUX_0");
    break;
  case 'b':
    Serial.println("AUX_1");
    break;
  case 'c':
    Serial.println("AUX_2");
    break;
  case 'd':
    Serial.println("AUX_3");
    break;
  case 'e':
    Serial.println("AUX_4");
    break;
  case 'f':
    Serial.println("AUX_5");
    break;
  case 'g':
    Serial.println("AUX_6");
    break;
  case 'h':
    Serial.println("AUX_7");
    break;
  case 'i':
    Serial.println("AUX_8");
    break;
  case 'j':
    Serial.println("AUX_9");
    break;
  case 'k':
    Serial.println("PROGRAM_0");
    break;
  case 'l':
    Serial.println("PROGRAM_1");
    break;
  case 'm':
    Serial.println("PROGRAM_2");
    break;
  case 'n':
    Serial.println("PROGRAM_3");
    break;
  case 'o':
    Serial.println("PROGRAM_4");
    break;
  case 'p':
    Serial.println("PROGRAM_5");
    break;
  case 'q':
    Serial.println("PROGRAM_6");
    break;
  case 'r':
    Serial.println("PROGRAM_7");
    break;
  case 's':
    Serial.println("PROGRAM_8");
    break;
  case 't':
    Serial.println("PROGRAM_9");
    break; 
  case '0':
    Serial.println("PREVIEW_0");
    break;
  case '1':
    Serial.println("PREVIEW_1");
    break;
  case '2':
    Serial.println("PREVIEW_2");
    break;
  case '3':
    Serial.println("PREVIEW_3");
    break;
  case '4':
    Serial.println("PREVIEW_4");
    break;
  case '5':
    Serial.println("PREVIEW_5");
    break;
  case '6':
    Serial.println("PREVIEW_6");
    break;
  case '7':
    Serial.println("PREVIEW_7");
    break;
  case '8':
    Serial.println("PREVIEW_8");
    break;
  case '9':
    Serial.println("PREVIEW_9");
    break; 
  case 'u':
    Serial.println("FUNCTION_0");
    break;
  case 'v':
    Serial.println("FUNCTION_1");
    break;
  case 'w':
    Serial.println("FUNCTION_2");
    break;
  case 'x':
    Serial.println("FUNCTION_3");
    break;
  case 'y':
    Serial.println("FUNCTION_4");
    break;
  case 'z':
    Serial.println("FUNCTION_5");
    break;
  case 'A':
    Serial.println("FUNCTION_6");
    break;
  case 'B':
    Serial.println("FUNCTION_7");
    break;
  case 'C':
    Serial.println("KEYER_0");
    break;
  case 'D':
    Serial.println("KEYER_1");
    break;
  case 'E':
    Serial.println("KEYER_2");
    break;
  case 'F':
    Serial.println("KEYER_3");
    break;
  case 'G':
    Serial.println("KEYER_4");
    break;
  case 'H':
    Serial.println("KEYER_5");
    break;
  case 'I':
    Serial.println("KEYER_6");
    break;
  case 'J':
    Serial.println("KEYER_7");
    break;
  case 'K':
    Serial.println("CUT");
    break;
  case 'L':
    Serial.println("AUTO");
    break;
  case 'M':
    Serial.println("MENU");
    break;
  case 'N':
    Serial.println("FUNCTION_SHIFT");
    break;
  case 'O':
    Serial.println("PROGRAM_SHIFT");
    break;
  case 'P':
    Serial.println("PREVIEW_SHIFT");
    break;
  default:
    Serial.println(c);
    break;
  }
}
