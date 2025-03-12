
#include <stdio.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);




  
#define GpioWire00 1
#define GpioWire01 2
#define GpioWire02 3
#define GpioWire03 4
#define INVALID_GPIO_NUMBER 0xFF
#define INVALID_WIRE_NUMBER 0xFE
#define CONNECTION_NOTSET 0
#define CONNECTION_SET  1
#define maxwires 4 
#define connectionRows 2
class WireMatrix
{
  public:
    WireMatrix(byte gpio01, byte gpio02, byte gpio03,byte gpio04);
    bool SetWire(byte wireNo, byte status);
    byte GetWire(byte wireNo);
    bool isValidWireNo(byte wireNo);
    byte GetGpio(byte wireNo);
    void SetExclusiveAsOutput(byte wireNo );
    void SetWireMode(byte wireNo, byte mode);
    void SetAllWireModesAsInput();
    void FillWireConnectedMatrix();
    bool IsPhysicalConnected(byte wireA, byte wireB);
    bool IsSetInMatrix(byte wireA, byte wireB);
    void SetResultTable();
    byte ConnectionCount(byte wire);
    void PrintConnections();
    void ClearConnections();
    byte* GetModel(void);

  private:
    byte wires[maxwires];
    byte resultModel[maxwires+1];
    byte connections[maxwires][maxwires];
};

WireMatrix matrix(GpioWire00, GpioWire01, GpioWire02, GpioWire03);
void View(char* model); 
void setup() 
{
  Serial.begin(115200);

  Wire.begin(9,8); // seesd 
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.display();
  delay(20); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();

  // Draw a single pixel in white
  display.drawPixel(10, 10, SSD1306_WHITE);
display.display();
Serial.println("Go");

}
 
void loop() 
{
  matrix.ClearConnections();
  matrix.FillWireConnectedMatrix();
  View((char*)matrix.GetModel());
  matrix.PrintConnections();
  delay(10);
}

 byte i =0;
 int step=1;

void View(char* model)
{
  char outstr[255] ;

  sprintf(outstr, "%c|%c|%c|%c", model[0], model[1], model[2], model[3] );
  display.clearDisplay();
  display.setTextSize(3); // Draw 2X-scale text
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println(F(outstr));
  
  display.setTextSize(1); // Draw 2X-scale text
  display.setCursor(25, SCREEN_HEIGHT-8);
  display.println(F("Hobbywerkerei"));

  display.drawPixel(i, SCREEN_HEIGHT-10, SSD1306_WHITE);  
  display.display();
  i += step;
  if (i > 128) step=-1;
  if (i <= 0) step= 1;
  

}


WireMatrix::WireMatrix(byte gpio01, byte gpio02, byte gpio03,byte gpio04)
{
  wires[0] = gpio01;
  wires[1] = gpio02;
  wires[2] = gpio03;
  wires[3] = gpio04;
  SetAllWireModesAsInput();
  ClearConnections();
}

bool WireMatrix::SetWire(byte wireNo, byte status)
{
  digitalWrite(GetGpio(wireNo), status);
  return true;
}

byte WireMatrix::GetWire(byte wireNo)
{
  return digitalRead(GetGpio(wireNo));
}

bool WireMatrix::isValidWireNo(byte wireNo)
{
  return wireNo < maxwires; 
}
byte WireMatrix::GetGpio(byte wireNo)
{
  if (!isValidWireNo( wireNo)) return INVALID_WIRE_NUMBER;
  return wires[wireNo];
}

void WireMatrix::SetExclusiveAsOutput(byte wireNo )
{
  SetAllWireModesAsInput();
  
  if (!isValidWireNo(wireNo)) return;
   SetWireMode(wireNo, OUTPUT);
}

void WireMatrix::SetWireMode(byte wireNo, byte mode)
{
  if (!isValidWireNo(wireNo)) return;
  pinMode(GetGpio(wireNo), mode);
}

void WireMatrix::SetAllWireModesAsInput()
{
  for (int wireNo = 0; wireNo < maxwires;wireNo++)
  {
    byte gpio = GetGpio(wireNo);
     pinMode(gpio, INPUT_PULLDOWN);
  }
}

void WireMatrix::FillWireConnectedMatrix()
{
  for (byte row = 0; row <  maxwires; row ++)
  {
    for (byte col = 0; col <  maxwires; col ++)
    {
      if (IsPhysicalConnected(row,col))
      {
        connections[row][col] =CONNECTION_SET;
      }
      else
      {
        connections[row][col] =CONNECTION_NOTSET;
      }
    }
  }
}

byte*  WireMatrix::GetModel()
{
  #define markernotset 0
  byte marker  = markernotset;
  for (byte wire = 0; wire <  maxwires; wire ++)
  {
    resultModel[wire] = marker;
  }
  
  for (byte wire = 0; wire <  maxwires; wire ++)
  {
    if (resultModel[wire] == markernotset) 
    {
      if (ConnectionCount(wire) >= 1){
        marker++;  
        for (byte row = 0; row <  maxwires; row ++)
        { 
            if (IsSetInMatrix(wire, row ))
            {
              resultModel[row] = marker;
            }
        }
      }
    }
  }

  for (byte wire = 0; wire <  maxwires; wire ++)
  {
    char disp = '0';
    switch (resultModel[wire])
    {
      case 0: disp = '-';
              break;
      case 1: disp = 222;
              break;
      case 2: disp = 219;
              break;
      default:
        disp = '?';
    }
    resultModel[wire] = disp;
  }
  return resultModel;
}

bool WireMatrix::IsSetInMatrix(byte wireA, byte wireB)
{
  bool directionAB = ((byte)connections[wireA][wireB] != (byte)CONNECTION_NOTSET);
  bool directionBA = ((byte)connections[wireB][wireA] != (byte)CONNECTION_NOTSET); 
  return directionAB && directionBA;
}

byte WireMatrix::ConnectionCount(byte wire)
{
  byte connections = 0;
  for (byte row = 0; row <  maxwires; row ++)
  {
    if (IsSetInMatrix(row, wire)) 
      connections++;
  }
  return connections-1; // 
}

bool WireMatrix::IsPhysicalConnected(byte wireA, byte wireB)
{
   SetExclusiveAsOutput(wireA);
   byte wireAState = HIGH;
   SetWire(wireA, wireAState);
   byte wireBState = GetWire(wireB);
   return  wireAState == wireBState;
}

void WireMatrix::PrintConnections()
{
  for (byte row = 0; row< maxwires; row ++)
  {
    for (byte wire = 0; wire <maxwires; wire ++)
    {
      Serial.print((char)(connections[row][wire]+(byte)'0'));
    }
    Serial.println();
  }

  Serial.println("---------------");
  byte* model =  GetModel();
  Serial.print((char)('0'+model[0]));
  Serial.print((char)('0'+model[1]));
  Serial.print((char)('0'+model[2]));
  Serial.print((char)('0'+model[3]));
  Serial.println("");
  Serial.println("---------------");
  Serial.println();
}

void WireMatrix::ClearConnections()
{
  for (byte row = 0; row < maxwires; row ++)
  {
    for (byte wire = 0; wire < maxwires; wire ++)
    {
      connections[row][wire] = CONNECTION_NOTSET;
    }
  }
}

