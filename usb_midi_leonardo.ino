#include "MIDIUSB.h"




const int VxPIN = A0;
const int VyPIN = A1;
const int ResonancePIN = A2;
const int CutoffPIN = A3;


int pitchNote;
int lastPitchVal;

int resonanceNote;
int lastResonanceVal;

int cutoffNote;
int lastCutoffVal;

int btnState;
int range = 2;


void setup() {
  // put your setup code here, to run once:

  pinMode(VxPIN, INPUT_PULLUP);
  pinMode(VyPIN, INPUT_PULLUP);
  pinMode(ResonancePIN, INPUT_PULLUP);
  pinMode(CutoffPIN, INPUT_PULLUP);
  Serial.begin(115200);

}

void loop() {
  // put your main code here, to run repeatedly:
  int pitchVal = analogRead(VyPIN);
  int resVal = analogRead(ResonancePIN);
  int cutoffVal = analogRead(CutoffPIN);

  bool pitchSame = pitchVal < lastPitchVal + range && pitchVal > lastPitchVal - range;
  bool resSame = resVal < lastResonanceVal + range && resVal > lastResonanceVal - range;
  bool cutOffSame = cutoffVal < lastCutoffVal + range && cutoffVal > lastCutoffVal - range;


  
    
  pitchNote = map(pitchVal, 0, 1023, 2500, 13400); //16530);
  resonanceNote = map(resVal, 0, 1023, 0, 127);
  cutoffNote = map(cutoffVal, 0, 1023, 0, 127);

  char buffer[70]; 
    sprintf(buffer, "Pitch: %d (%d), Res: %d (%d), Cutoff: %d (%d)", pitchVal, pitchNote, resVal, resonanceNote, cutoffVal, cutoffNote);
    //sprintf(buffer, "pitchVal: %d, lastPitchVal: %d", pitchVal, lastPitchVal);
    Serial.println(buffer);

  if (!pitchSame)
  {
    sendMidi(0xE0, pitchNote);
     lastPitchVal = pitchVal;    
  }

  if (!resSame){
    //sendMidi(71, resonanceNote);
    sendCmd(71, resonanceNote);
    lastResonanceVal = resVal;
  }

  if (!cutOffSame){
    //sendMidi(74, cutoffNote);
    sendCmd(74, cutoffNote);
    lastCutoffVal = cutoffVal;
  }

  
}


void sendMidi(byte cmd, int val)
{
  int shiftedValue = val << 1; // shift so top bit of lsb is in msb
  byte msb = highByte(shiftedValue); // get the high bits
  byte lsb = lowByte(shiftedValue) >> 1; // get the low 7 bits and shift right
  
  midiCommand(cmd, lsb, msb);
  
}

// send a 3-byte midi message
void midiCommand(byte cmd, byte data1, byte  data2) {
  // First parameter is the event type (top 4 bits of the command byte).
  // Second parameter is command byte combined with the channel.
  // Third parameter is the first data byte
  // Fourth parameter second data byte

  midiEventPacket_t midiMsg = {cmd >> 4, cmd, data1, data2};
  MidiUSB.sendMIDI(midiMsg);
  MidiUSB.flush();
}

void sendCmd(byte cmd, byte value) {
  // Status byte: 0xB0 (Control Change) + channel
  // Controller number: 71 (Resonance)
  // Value: 0-127
  midiEventPacket_t controlChange = {0x0B, 0xB0 | 1, cmd, value};
  MidiUSB.sendMIDI(controlChange);
  MidiUSB.flush();
}
