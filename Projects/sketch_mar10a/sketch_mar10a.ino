
#define wire00 25
#define wire01 26
#define wire02 27
#define wire03 14
#define INVALID_GPIO_NUMBER 0xFF
#define INVALID_WIRE_NUMBER 0xFE


void SetAllWireModesAsInput(void);
void SetExclusiveAsOutput(byte wireNo );
bool isValidWireNo(byte wireNo);
void SetWireMode(byte wireNo, byte mode);
void ClearConnections(void);


#define CONNECTION_NOTSET -1

#define maxwires 4 
#define connectionRows 2
byte wires[maxwires]{wire00, wire01, wire02, wire03 };
byte connections[maxwires][maxwires];

 
void setup() 
{
   Serial.begin(115200);
   SetAllWireModesAsInput();
   ClearConnections();
   Serial.println("Go");
}
 
 byte i =0;
void loop() 
{
  ClearConnections();
  FillConnectedMatrix();
  PrintConnections();
  delay(500);
}

bool SetWire(byte wireNo, byte status)
{
  digitalWrite(GetGpio(wireNo), status);
  return true;
}

byte GetWire(byte wireNo)
{
  return digitalRead(GetGpio(wireNo));
}

bool isValidWireNo(byte wireNo)
{
  return wireNo < maxwires; 
}
byte GetGpio(byte wireNo)
{
  if (!isValidWireNo( wireNo)) return INVALID_WIRE_NUMBER;
  return wires[wireNo];
}


void SetExclusiveAsOutput(byte wireNo )
{
  byte mode = OUTPUT;
  SetAllWireModesAsInput();
  
  if (!isValidWireNo(wireNo)) return;
   SetWireMode(wireNo, mode);
}

void SetWireMode(byte wireNo, byte mode)
{
  if (!isValidWireNo(wireNo)) return;
  pinMode(GetGpio(wireNo), mode);
}

void SetAllWireModesAsInput()
{
  byte mode = INPUT_PULLDOWN;
  for (int wireNo = 0; wireNo < maxwires;wireNo++)
  {
    byte gpio = GetGpio(wireNo);
     pinMode(gpio, mode);
    //digitalWrite(gpio, LOW);
   
  }
}

void FillConnectedMatrix()
{
  for (byte row = 0; row <  maxwires; row ++)
  {
    for (byte col = 0; col <  maxwires; col ++)
    {
      if (Connected(row,col))
      {
        connections[row][col] =row;
      }
      else
      {
        connections[row][col] =CONNECTION_NOTSET;
      }
    }
  }
}

void SetResultTable()
{
  for (byte row = 0; row <  maxwires; row ++)
  {
    byte cCount = ConnectionCount(row);
    if (cCount > 0)
  }
}


byte ConnectionCount(byte wire)
{
  byte connections = 0;
  for (byte row = 0; row <  maxwires; row ++)
  {
    if (connections[row][wire] != CONNECTION_NOTSET) 
      connections++;
  }
  return connections-1; // 
}

bool Connected(byte wireA, byte wireB)
{
   SetExclusiveAsOutput(wireA);
   byte wireAState = HIGH;
   SetWire(wireA, wireAState);
   byte wireBState = GetWire(wireB);
   return  wireAState == wireBState;
}


void PrintConnections()
{
  for (byte row = 0; row< maxwires; row ++)
  {
    for (byte wire = 0; wire <maxwires; wire ++)
    {
      Serial.print((char)(connections[row][wire]+(byte)'1'));
    }
    Serial.println();
  }
  Serial.println();
}

void ClearConnections()
{
  for (byte row = 0; row < maxwires; row ++)
  {
    for (byte wire = 0; wire < maxwires; wire ++)
    {
      connections[row][wire] = CONNECTION_NOTSET;
    }
  }
}

