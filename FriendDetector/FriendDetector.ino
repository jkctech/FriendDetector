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
#define LIST_SIZE 4     // Amount of people in your list
#define USE_LEDS true   // Do you want to make use of LEDs as outputs?
#define COOLDOWN 600000 // Last seen timeout to 10 minutes

// Names of people to track
const String names[LIST_SIZE] = {
  "Jeffrey",
  "Sister",
  "Mom",
  "Dad"
};

// Define their MAC addresses
const uint8_t macs[LIST_SIZE][ESPPL_MAC_LEN] = {
  {0x64, 0xA2, 0xF9, 0x0F, 0x8F, 0x4A},
  {0x88, 0x28, 0xB3, 0xD0, 0x25, 0x9F},
  {0xF8, 0x27, 0x93, 0x69, 0xBD, 0x58},
  {0x00, 0x27, 0x15, 0xD2, 0xE1, 0xFF}
};

// Pins per person (If USE_LEDS is set to false, don't bother about this)
const int pins[LIST_SIZE] = {
  D1, D2, D5, D6
};

// Timers to keep track of last seen
unsigned long timers[LIST_SIZE] = {
  0, 0, 0, 0
};

// Setup everything
void setup()
{
  Serial.begin(115200); // Setup serial

  // Set all pins for people to track
  for (int i = 0; i < LIST_SIZE; i++)
    pinMode(pins[i], OUTPUT);

  // Turn them all off (Just to be sure)
  for (int i = 0; i < LIST_SIZE; i++)
    digitalWrite(pins[i], LOW);

  esppl_init(cb); // Init esppl
}

// Main Loop
void loop() {
  esppl_sniffing_start(); // Start the sniffing >:)

  // Process every frame we can find
  while (true)
  {
    for (int i = ESPPL_CHANNEL_MIN; i <= ESPPL_CHANNEL_MAX; i++)
    {
      esppl_set_channel(i); // Scan on all available channels
      // Process them all but don't actually do anything in that loop
      while (esppl_process_frames()) {} 
    }
  }
}

// Function to compare mac addresses
bool maccmp(const uint8_t *mac1, const uint8_t *mac2)
{
  for (int i = 0; i < ESPPL_MAC_LEN; i++)
    if (mac1[i] != mac2[i])
      return false;
  return true;
}

// Function run by esppl
void cb(esppl_frame_info *info)
{
  unsigned long currentTime = millis(); // Current time
  
  // Iterate over list of persons
  for (int i = 0; i < LIST_SIZE; i++)
  {
    // If MAC address is receiver OR sender, person is found.
    if (maccmp(info->sourceaddr, macs[i]) || maccmp(info->receiveraddr, macs[i]))
    {
      // Print to serial
      Serial.printf("\nDetected %s (ID: %d, Pin: %d)", names[i].c_str(), i, pins[i]);

      // Do we use the leds? If so, turn them on
      if (USE_LEDS) { turnon(pins[i]); }
      timers[i] = currentTime; // Set last seen to now
    }
    else
    {
      // If needed, check last seen on user and act accordingly
      if(USE_LEDS && timers[i] != 0 && currentTime - timers[i] > COOLDOWN)
      {
        timers[i] = 0; // Set last seen to 0
        turnoff(pins[i]); // Turn their LED off
      }
    }
  }
}

// Turn on pin X
void turnon(int pin)
{
  digitalWrite(pin, HIGH);
}

// Turn off pin X
void turnoff(int pin)
{
  digitalWrite(pin, LOW);
}
