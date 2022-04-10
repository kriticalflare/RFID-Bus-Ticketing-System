#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal.h>
#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);
LiquidCrystal lcd(6, 7, 5, 4, 3, 2);

/* Create an instance of MFRC522 */
/* Create an instance of MIFARE_Key */
MFRC522::MIFARE_Key key;

/* Set the block to which we want to write data */
/* Be aware of Sector Trailer Blocks */
int blockNum = 2;
/* Create an array of 16 Bytes and fill it with data */
/* This is the actual data which is going to be written into the card */
// byte blockData[16] = { "200" };
// byte blockData[]    = {
//     0xC8, 0x00, 0x00, 0x00,
//     0x00, 0x00, 0x00, 0x00,
//     0x00, 0x00, 0x00, 0x00,
//     0x00, 0x00, 0x00, 0x00
// };

byte blockData[]    = {
    200, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0
};

int amt = 0;

// byte dataBlock[] = { highByte(amt), lowByte(amt) };

/* Create another array to read data from Block */
/* Legthn of buffer should be 2 Bytes more than the size of Block (16 Bytes) */
byte bufferLen = 18;
byte readBlockData[18];

MFRC522::StatusCode status;

void setup() {
  /* Initialize serial communications with the PC */
  Serial.begin(9600);
  /* Initialize SPI bus */
  SPI.begin();
  /* Initialize MFRC522 Module */
  mfrc522.PCD_Init();
  Serial.println();
  Serial.println("Scan your Bus Card to buy a ticket");
  lcd.begin(16, 2);
  lcd.print("Scan Bus Card");
}

void loop() {
  /* Prepare the ksy for authentication */
  /* All keys are set to FFFFFFFFFFFFh at chip delivery from the factory */
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
  /* Look for new cards */
  /* Reset the loop if no new card is present on RC522 Reader */
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  /* Select one of the cards */
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }
 
  Serial.print("\n");
  Serial.println("**Card Detected**");
 
  /* Read data from the same block */
  Serial.print("\n");
  Serial.println("Reading from Bus Card...");
  ReadDataFromBlock(blockNum, readBlockData);
  /* If you want to print the full memory dump, uncomment the next line */
  //mfrc522.PICC_DumpToSerial(&(mfrc522.uid));

  /* Print the data read from block */
  Serial.print("\n");
  Serial.print("Current balance:");
  Serial.print(" --> ");
  // dump_byte_array(readBlockData, bufferLen);
  amt=readBlockData[0]; 
  Serial.print(amt);
  Serial.println("\n");

  amt -= 10;
  blockData[0] = amt;
  WriteDataToBlock(blockNum, blockData);
  Serial.print("\n");
  Serial.println("Deducting Fare from balance...");

  ReadDataFromBlock(blockNum, readBlockData);
  amt=readBlockData[0]; 
  Serial.println();
  Serial.print("New Balance:");
  Serial.print(" --> ");
  amt=readBlockData[0]; 
  Serial.print(amt);
  Serial.println("\n");
  delay(1000);
  lcd.clear();
  lcd.begin(16, 2);
  lcd.print("Fare Deducted");
  delay(2000);
  lcd.clear();
  lcd.begin(16, 2);
  String display = "Balance ";
  lcd.print(display);
  delay(2000);
  lcd.clear();
  lcd.begin(16, 2);
  lcd.print(amt);
}

void dump_byte_array(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
}


void WriteDataToBlock(int blockNum, byte blockData[]) {
  /* Authenticating the desired data block for write access using Key A */
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockNum, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print("Authentication failed for Write: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  } else {
    Serial.println("Authentication success");
  }


  /* Write data to the block */
  status = mfrc522.MIFARE_Write(blockNum, blockData, 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.print("Writing to Block failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  } else {
    Serial.println("Data was written into Card successfully");
  }
}

void ReadDataFromBlock(int blockNum, byte readBlockData[]) {
  /* Authenticating the desired data block for Read access using Key A */
  byte status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockNum, &key, &(mfrc522.uid));

  if (status != MFRC522::STATUS_OK) {
    Serial.print("Authentication failed for Read: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  } else {
    Serial.println("Authentication success");
  }

  /* Reading data from the Block */
  status = mfrc522.MIFARE_Read(blockNum, readBlockData, &bufferLen);
  if (status != MFRC522::STATUS_OK) {
    Serial.print("Reading failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  } else {
    Serial.println("Card was read successfully");
  }
}