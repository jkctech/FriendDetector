// =======================================================
// 
// Modified version of RicardoOliveira's FriendDetector:
// https://github.com/RicardoOliveira/FriendDetector
// 
// Modified by: JKCTech
// Date: 26-03-2019
// 
// =======================================================

#include "esppl_functions.h"

// Settings
#define LIST_SIZE 4
#define USE_LEDS true
#define cooldown 3000 // Last seen timeout to alot of packets

// Names
static String names[LIST_SIZE] = {
  "Me",
  "Sister",
  "Mom",
  "Dad"
};

// Define their MAC addresses
static uint8_t macs[LIST_SIZE][ESPPL_MAC_LEN] = {
  {0x64, 0xA2, 0xF9, 0x0F, 0x8F, 0x4A},
  {0x88, 0x28, 0xB3, 0xD0, 0x25, 0x9F},
  {0xF8, 0x27, 0x93, 0x69, 0xBD, 0x58},
  {0x00, 0x27, 0x15, 0xD2, 0xE1, 0xFF}
};

// Pins per person
static int pins[LIST_SIZE] = {
  D1, D2, D5, D6
};

// Timers to keep track of last seen
int timers[LIST_SIZE] = {
  0, 0, 0, 0
};

// Setup some stuff
void setup() {
  // Setup serial
  Serial.begin(115200);

  // Set all pins for people to track
  for (int i = 0; i < LIST_SIZE; i++)
    pinMode(pins[i], OUTPUT);

  // Turn them all off
  for (int i = 0; i < LIST_SIZE; i++)
    digitalWrite(pins[i], LOW);

  // Init esppl
  esppl_init(cb);
}

// Main Loop
void loop() {
  // Start the sniffing >:)
  esppl_sniffing_start();

  // Process every frame we can find
  while (true) {
    for (int i = ESPPL_CHANNEL_MIN; i <= ESPPL_CHANNEL_MAX; i++) {
      esppl_set_channel(i);
      while (esppl_process_frames()) {}
    }
  }
}

// Function to compare mac addresses
bool maccmp(uint8_t *mac1, uint8_t *mac2) {
  for (int i = 0; i < ESPPL_MAC_LEN; i++)
    if (mac1[i] != mac2[i])
      return false;
  return true;
}

// Function to detect
void cb(esppl_frame_info *info) {
  // Iterate over list of persons
  for (int i = 0; i < LIST_SIZE; i++) {
    // If MAC address is receiver OR sender, person is found.
    if (maccmp(info->sourceaddr, macs[i]) || maccmp(info->receiveraddr, macs[i])) {
      // Set LED for person ON & print
      turnon(pins[i]);
      Serial.printf("\nDetected %s (%d) [%d]", names[i].c_str(), i, pins[i]);

      // Reset timers
      timers[i] = cooldown;
    } else {
      if(timers[i] > 0)
        timers[i]--;
      else
        turnoff(pins[i]);
    }
  }
}

void turnon(int pin) {
  digitalWrite(pin, HIGH);
}

void turnoff(int pin) {
  digitalWrite(pin, LOW);
}

void turnalloff() {
  for (int i = 0; i < LIST_SIZE; i++)
    digitalWrite(pins[i], LOW);
}
