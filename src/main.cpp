#include <Arduino.h>

#include "AudioFileSourceID3.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2S.h"
#include "AudioFileSourceICYStream.h"
// #include "AudioFileSourceBuffer.h"

// Enter your WiFi setup here:
#ifndef STASSID
#define STASSID "BOM VPN"
#define STAPSK  "root@netgear:42069"
#endif

AudioGeneratorMP3 *mp3;
AudioFileSourceICYStream *file;
// AudioFileSourceBuffer *buff;
AudioOutputI2S *out;
AudioFileSourceID3 *id3;

const char* ssid = STASSID;
const char* password = STAPSK;

// Randomly picked URL
const char *URL="http://192.168.30.1:8000/bruvva2.mp3";

// Called when a metadata event occurs (i.e. an ID3 tag, an ICY block, etc.
void MDCallback(void *cbData, const char *type, bool isUnicode, const char *string)
{
  const char *ptr = reinterpret_cast<const char *>(cbData);
  (void) isUnicode; // Punt this ball for now
  // Note that the type and string may be in PROGMEM, so copy them to RAM for printf
  char s1[32], s2[64];
  strncpy_P(s1, type, sizeof(s1));
  s1[sizeof(s1)-1]=0;
  strncpy_P(s2, string, sizeof(s2));
  s2[sizeof(s2)-1]=0;
  Serial.printf("METADATA(%s) '%s' = '%s'\n", ptr, s1, s2);
  Serial.flush();
}

// Called when there's a warning or error (like a buffer underflow or decode hiccup)
void StatusCallback(void *cbData, int code, const char *string)
{
  const char *ptr = reinterpret_cast<const char *>(cbData);
  // Note that the string may be in PROGMEM, so copy it to RAM for printf
  char s1[64];
  strncpy_P(s1, string, sizeof(s1));
  s1[sizeof(s1)-1]=0;
  Serial.printf("STATUS(%s) '%d' = '%s'\n", ptr, code, s1);
  Serial.flush();
}

// similar to setup, but it will be called repeatedly if need be
// e.g. ran out of audio (no stream available) etc.
void init(){
  delay(1000);
  Serial.println("Connecting to WiFi");

  WiFi.disconnect();
  WiFi.softAPdisconnect(true);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  // Try forever
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("...Connecting to WiFi");
    delay(1000);
  }

  Serial.println("Connected");

  audioLogger = &Serial;

  file = new AudioFileSourceICYStream(URL);
  file->RegisterMetadataCB(MDCallback, (void*)"ICY");

  // Buffer the HTTP stream a bit
  // buff = new AudioFileSourceBuffer(file, 2048);
  // buff->RegisterStatusCB(StatusCallback, (void*)"buffer");

  // Note: On the AirM2M board, to use GPIO11, you must burn an efuse
  // $ pip instal esptool
  // $ espefuse.py -p /dev/ttyUSB0 burn_efuse VDD_SPI_AS_GPIO 1
  // See: https://github.com/chenxuuu/luatos-wiki/discussions/11#discussioncomment-3021045
  out = new AudioOutputI2S();
  out->SetPinout(6, 11, 7); // BCK, LCK, DIN

  mp3 = new AudioGeneratorMP3();
  mp3->RegisterStatusCB(StatusCallback, (void*)"mp3");
  mp3->begin(file, out);
}

void setup()
{
  Serial.begin(115200);
  init();
}

void loop()
{
  if (mp3->isRunning()) {
    if (!mp3->loop()) mp3->stop();
  } else {
    Serial.printf("MP3 done\n");
    delay(1000);
    init();
  }
}
