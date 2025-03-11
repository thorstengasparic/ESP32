
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


#define maxwires 4 
byte wires[maxwires]{wire00, wire01, wire02, wire03 };
 
void setup() 
{
   SetAllWireModesAsInput();
}
 
 byte i =0;
void loop() 
{
 SetExclusiveAsOutput(i);
 
 SetWire(i,HIGH);
 
 delay(500);
  //SetAllWireModesAsInput();
  SetWire(i,LOW);
 delay(500);
 i++;
 if (i >= maxwires) i =0;
}

bool SetWire(byte wireNo, byte status)
{
  digitalWrite(GetGpio(wireNo), status);
  return true;
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

