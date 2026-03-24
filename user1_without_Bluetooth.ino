#include <SPI.h>
#include <LoRa.h>

byte localAddress = 0xAA;   // CHANGE per device
byte destination  = 0xBB;   // CHANGE per device

byte msgCount = 0;

String outgoing;
String incoming;

void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.println("LoRa Serial Chat");

  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa init failed!");
    while (true);
  }

  Serial.println("LoRa init OK");
}

void loop() {

  // -------- SEND FROM SERIAL --------
  if (Serial.available()) {
    String msg = Serial.readStringUntil('\n');
    sendMessage(msg);
  }

  // -------- RECEIVE FROM LORA --------
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    receiveMessage();
  }
}

// ---------- SEND FUNCTION ----------
void sendMessage(String outgoing) {
  LoRa.beginPacket();
  LoRa.write(destination);   // receiver
  LoRa.write(localAddress);  // sender
  LoRa.write(msgCount);      
  LoRa.write(outgoing.length());
  LoRa.print(outgoing);
  LoRa.endPacket();

  Serial.print("Sent: ");
  Serial.println(outgoing);

  msgCount++;
}

// ---------- RECEIVE FUNCTION ----------
void receiveMessage() {
  byte recipient = LoRa.read();
  byte sender    = LoRa.read();
  byte msgId     = LoRa.read();
  byte msgLength = LoRa.read();

  String incoming = "";

  while (LoRa.available()) {
    incoming += (char)LoRa.read();
  }

  // Filter messages
  if (recipient != localAddress && recipient != 0xFF) {
    return;
  }

  Serial.print("Received from 0x");
  Serial.print(sender, HEX);
  Serial.print(": ");
  Serial.println(incoming);
}
