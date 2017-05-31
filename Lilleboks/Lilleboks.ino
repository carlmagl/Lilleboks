#include <MFRC522.h>
#include <SPI.h>//include the SPI bus library

#define SS_PIN 10  //slave select pin
#define RST_PIN 5  //reset pin
MFRC522 mfrc522(SS_PIN, RST_PIN);        // iniate a MFRC522 reader object.
MFRC522::MIFARE_Key key;//create a MIFARE_Key struct named 'key', which will hold the card information

//----bruker0-----------------
String rfidbruker0 = "04 28 C4 D2 20 4B 80";
String mailBruker0 = "eksempel@live.no";
boolean bruker0Vasket = false;
int bruker0VaskeSone = 0;
int ledbruker0 = 7;
boolean prevStatebruker0 = false;
//-------------------------

//----bruker1---------------
String rfidbruker1 = "04 30 C4 D2 20 4B 80";
String mailBruker1 = "eksempel@live.no";
boolean bruker1Vasket = false;
int bruker1VaskeSone = 1;
int ledbruker1 = 6;
boolean prevStatebruker1 = false;
//-------------------------

//----bruker2----------------
String rfidbruker2 = "04 38 C4 D2 20 4B 80";
String mailBruker2 = "eksempel@live.no";
boolean bruker2Vasket = false;
int bruker2VaskeSone = 2;
int ledbruker2 = 5;
boolean prevStatebruker2 = false;
//---------------------------

//---bruker3------------------
String rfidbruker3 = "04 3F C3 D2 20 4B 80";
String mailBruker3 = "eksempel@live.no";
boolean bruker3Vasket = false;
int bruker3VaskeSone = 3;
int ledbruker3 = 4;
boolean prevStatebruker3 = false;
//--------------------------

//---bruker4------------------
String rfidbruker4 = "04 46 C2 D2 20 4B 80";
String mailBruker4 = "eksempel@live.no";
boolean bruker4Vasket = false;
int bruker4VaskeSone = 4;
int ledbruker4 = 3;
boolean prevStatebruker4 = false;
//--------------------------

//------Vaskesoner------
int badet = 0;
String meldingBad = "du skal denne uken vaske badet, husk å tømme sluket.";
int kjokken = 1;
String meldingKjokken = "du skal denne uken vaske kjokkenet, husk å vaske ovnen.";
int gang = 2;
String meldingGang = "du skal denne uken vaske gangen, husk under skohyllen.";
int sjekkVaskeutstyr = 3;
String meldingVaskeutstyr = "du skal denne uken sjekke at dere har nok vaskeutstyr, husk toalettpapir og såpe.";
int friUke = 4;
String fri = "du har denne uken fri, kos deg!";
//----------------------

//-----Knapp---------
int knappPlass = A0;
int knappState;             
int sisteKnappState = LOW;   
int antallKlikket = 0;
//--------------------- 

//-------Tilt-Switch---------
int switchState1 = 1;
int prevSwitchState1 = 0;
int switchState2 = 0;
int prevSwitchState2 = 1;
int switchPin1 = A1;
int switchPin2 = A2;
//--------------------------

unsigned long enUke = 604800000;
unsigned long naa = 0;
unsigned long forrigeTid;


unsigned long fredag = 432000000;
unsigned long naa2 = 0;
unsigned long forrigeTid2;

boolean harAngret = false;

void setup() {
  pinMode(ledbruker0, OUTPUT);
  pinMode(ledbruker1, OUTPUT);
  pinMode(ledbruker2, OUTPUT);
  pinMode(ledbruker3, OUTPUT);
  pinMode(ledbruker4, OUTPUT);
  digitalWrite(ledbruker0, LOW);
  digitalWrite(ledbruker1, LOW);
  digitalWrite(ledbruker2, LOW);
  digitalWrite(ledbruker3, LOW);
  digitalWrite(ledbruker4, LOW);
  
  pinMode(switchPin1, INPUT);
  pinMode(switchPin2, INPUT);
  pinMode(knappPlass, INPUT);
  
  
  
  //rfid
  Serial.begin(9600);        // Initialize serial communications with the PC
  SPI.begin();               // Init SPI bus
  mfrc522.PCD_Init();        // Init MFRC522 card (in case you wonder what PCD means: proximity coupling device)
  Serial.println("Scan a MIFARE Classic card");
        
  // Prepare the security key for the read and write functions - all six key bytes are set to 0xFF at chip delivery from the factory.
  // Since the cards in the kit are new and the keys were never defined, they are 0xFF
  // if we had a card that was programmed by someone else, we would need to know the key to be able to access it. This key would then need to be stored in 'key' instead
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;//keyByte is defined in the "MIFARE_Key" 'struct' definition in the .h file of the library
  }
  

}

void loop() {
  if(sjekkUkeFerdig() || alleVasket()){
    nyVaskeUke();
  }
  if(sjekkFredag()){
    masPaaUvasket();
  }
  if(sjekkKnapp()){
    angreNyUke();
  }
  if(sjekkTilt()){
    nyVaskeUke();
  }
  rfidTing();
  if(!bruker0Vasket){
    //sendMail();
  }
}
boolean alleVasket(){
  return bruker0Vasket && bruker1Vasket && bruker2Vasket && bruker3Vasket && bruker4Vasket;
}
void rfidTing(){
  // Look for new cards
  if (!mfrc522.PICC_IsNewCardPresent()){
    return;
  }
  
  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  
  //Show UID on serial monitor
  Serial.print("UID tag :");
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();
  
  //hvis iden på klistremerket som er lest inn matcher og brukeren ikke har vasket blir led tent og vasket satt til true.
  if (content.substring(1) == rfidbruker0 && !bruker0Vasket){
    bruker0Vasket = true;
    digitalWrite(ledbruker0, HIGH);
    Serial.println("bruker0 har vasket.");
  }
  //hvis iden på klistremerket som er lest inn matcher og brukeren ikke har vasket blir led tent og vasket satt til true.
  else if(content.substring(1) == rfidbruker1 && !bruker1Vasket){
    bruker1Vasket = true;
    digitalWrite(ledbruker1, HIGH);
    Serial.println("bruker1 har vasket.");
  }
  //hvis iden på klistremerket som er lest inn matcher og brukeren ikke har vasket blir led tent og vasket satt til true.
 else if(content.substring(1) == rfidbruker2 && !bruker2Vasket){
    bruker2Vasket = true;
    digitalWrite(ledbruker2, HIGH);
    Serial.println("bruker2 har vasket.");
 }
 //hvis iden på klistremerket som er lest inn matcher og brukeren ikke har vasket blir led tent og vasket satt til true.
 else if(content.substring(1) == rfidbruker3 && !bruker3Vasket){
    bruker3Vasket = true;
    digitalWrite(ledbruker3, HIGH);
    Serial.println("bruker3 har vasket.");
 }
 //hvis iden på klistremerket som er lest inn matcher og brukeren ikke har vasket blir led tent og vasket satt til true.
 else if(content.substring(1) == rfidbruker4 && !bruker4Vasket){
    bruker4Vasket = true;
    digitalWrite(ledbruker4, HIGH);
    Serial.println("bruker4 har vasket.");
 }
 else   {
    Serial.println("Personen har allerede vasket");
  }
  
}
void angreNyUke(){
  if(!harAngret){
    //endrer vaskesone tilbake for bruker
    bruker0Vasket = prevStatebruker0;
    digitalWrite(ledbruker0, LOW);
    if(bruker0VaskeSone == 0){
      bruker0VaskeSone = 4;
    }else{
      bruker0VaskeSone--;
    }
    //endrer vaskesone tilbake for bruker
    bruker1Vasket = prevStatebruker1;
    digitalWrite(ledbruker1, LOW);
    if(bruker1VaskeSone = 0){
      bruker1VaskeSone = 4;
    }else{
      bruker1VaskeSone--;
    }
    //endrer vaskesone tilbake for bruker
    bruker2Vasket = prevStatebruker2;
    digitalWrite(ledbruker2, LOW);
    if(bruker2VaskeSone = 0){
      bruker2VaskeSone = 4;
    }else{
      bruker2VaskeSone--;
    }
    //endrer vaskesone tilbake for bruker
    bruker3Vasket = prevStatebruker3;
    digitalWrite(ledbruker3, LOW);
    if(bruker3VaskeSone = 0){
      bruker3VaskeSone = 4;
    }else{
      bruker3VaskeSone--;
    }
    //endrer vaskesone tilbake for bruker
    bruker4Vasket = prevStatebruker4;
    digitalWrite(ledbruker4, LOW);
    if(bruker4VaskeSone = 0){
      bruker4VaskeSone = 4;
    }else{
      bruker4VaskeSone--;
    } 
  }
}

boolean sjekkUkeFerdig(){
  forrigeTid = naa;
  naa = millis();
  return naa - forrigeTid >= enUke;
}
boolean sjekkFredag(){
  forrigeTid2 = naa;
  naa2 = millis();
  return naa2 - forrigeTid2 >= fredag;
}
boolean sjekkKnapp(){
  return digitalRead(knappPlass);
  //returner om knappen er trykket inn
}
void masPaaUvasket(){
  if(sjekkKnapp()){
    if(!bruker0Vasket){
      //send mail med vaskesone
      Serial.println("bruker0 har ikke vasket");
    }
    //hvis personen ikke har vasket, send mail
    if(!bruker1Vasket){
      //send mail med vaskesone
      Serial.println("bruker1 har ikke vasket");
    }
    //hvis personen ikke har vasket, send mail
    if(!bruker3Vasket){
      //send mail med vaskesone
      Serial.println("bruker3 har ikke vasket");
    }
    //hvis personen ikke har vasket, send mail
    if(!bruker2Vasket){
      //send mail med vaskesone
      Serial.println("bruker2 har ikke vasket");
    }
    //hvis personen ikke har vasket, send mail
    if(!bruker4Vasket){
      //send mail med vaskesone
      Serial.println("bruker4 har ikke vasket");
    }
    //hvis personen ikke har vasket, send mail
  } 
}

void nyVaskeUke(){
  if(harAngret){
    prevStatebruker0 = false;
    prevStatebruker1 = false;
    prevStatebruker2 = false;
    prevStatebruker3 = false;
    prevStatebruker4 = false;
    harAngret = false;
  }
  //resetter alle tidligere states hvis det har blitt angret.

  //skifter vaskesone for bruker0 og skrur av led
  prevStatebruker0 = bruker0Vasket;
  bruker0Vasket = false;
  digitalWrite(ledbruker0, LOW);
  if(bruker0VaskeSone == 4){
    bruker0VaskeSone = 0;
  }else{
    bruker0VaskeSone++;
  }
  //sendMail til bruker0 her
  
  //skifter vaskesone for bruker1 og skrur av led
  prevStatebruker1 = bruker1Vasket;
  bruker1Vasket = false;
  digitalWrite(ledbruker1, LOW);
  if(bruker1VaskeSone = 4){
    bruker1VaskeSone = 0;
  }else{
    bruker1VaskeSone++;
  }
  //sendMail til bruker1 her
  
  //skifter vaskesone for bruker2 og skrur av led
  prevStatebruker2 = bruker2Vasket;
  bruker2Vasket = false;
  digitalWrite(ledbruker2, LOW);
  if(bruker2VaskeSone = 4){
    bruker2VaskeSone = 0;
  }else{
    bruker2VaskeSone++;
  }
  //sendMail til bruker2 her
  
  //skifter vaskesone for bruker3 og skrur av led
  prevStatebruker3 = bruker3Vasket;
  bruker3Vasket = false;
  digitalWrite(ledbruker3, LOW);
  if(bruker3VaskeSone = 4){
    bruker3VaskeSone = 0;
  }else{
    bruker3VaskeSone++;
  }
  //sendMail til bruker3 her
  
  //skifter vaskesone for bruker4 og skrur av led
  prevStatebruker4 = bruker4Vasket;
  bruker4Vasket = false;
  digitalWrite(ledbruker4, LOW);
  if(bruker4VaskeSone = 4){
    bruker4VaskeSone = 0;
  }else{
    bruker4VaskeSone++;
  }
  //sendMail til bruker4 her
}
boolean sjekkTilt(){
  
  switchState1 = digitalRead(switchPin1);
  switchState2 = digitalRead(switchPin2);
  //leser av begge tilt switchene
  
  return switchState1 == prevSwitchState1 && switchState2 == prevSwitchState2;
  //sjekker om begge tilt switchen er i motsatt state av det de vanligvis er
}


