// Setup pin assignments based on board
// Part of our group's setup guide: https://docs.google.com/spreadsheets/d/1Kk39XogCKNj4P-L1mcQ60H9ZAvij4BNzWpTyQpcqGX0/edit#gid=730310102

#ifdef EGIZMO
  #define GPS_RX 16
  #define GPS_TX 17
  #define OLED_SCL 22
  #define OLED_SDA 21
  #define LORA_INTERRUPT 2
  #define LORA_RESET 14
  #define LORA_CHIP_SELECT 5
  #define LORA_SERIAL_CLOCK 18
  #define LORA_COPI 23
  #define LORA_CIPO 19
  #define BTN_1 13
  #define BTN_2 33
  #define BTN_3 25
  #define BTN_4 26
  #define BTN_5 27
#endif

#ifdef LILYGO
  #define GPS_RX 34
  #define GPS_TX 12
  #define OLED_SCL 22
  #define OLED_SDA 21
  #define LORA_INTERRUPT 26
  #define LORA_RESET 23
  #define LORA_CHIP_SELECT 18
  #define LORA_SERIAL_CLOCK 5
  #define LORA_COPI 27
  #define LORA_CIPO 19
  #define BTN_1 38
  #define BTN_2 13
  #define BTN_3 14
  #define BTN_4 25
  #define BTN_5 15
#endif
