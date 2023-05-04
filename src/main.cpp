#include <Arduino.h>

#include "AudioFileSourcePROGMEM.h"
#include "AudioFileSourceID3.h"
#include "AudioGeneratorMP3.h"
#include "AudioGeneratorWAV.h"
#include "AudioOutputI2S.h"

// VIOLA sample taken from https://ccrma.stanford.edu/~jos/pasp/Sound_Examples.html
// #include "viola.h"
#include "fly_64k.h"
// #include "cantina.h"
// #include "fly_short.h"

AudioGeneratorMP3 *mp3;
AudioFileSourcePROGMEM *file;
AudioOutputI2S *out;
AudioFileSourceID3 *id3;

// Called when a metadata event occurs (i.e. an ID3 tag, an ICY block, etc.
void MDCallback(void *cbData, const char *type, bool isUnicode, const char *string)
{
  (void)cbData;
  Serial.printf("ID3 callback for: %s = '", type);

  if (isUnicode) {
    string += 2;
  }
  
  while (*string) {
    char a = *(string++);
    if (isUnicode) {
      string++;
    }
    Serial.printf("%c", a);
  }
  Serial.printf("'\n");
  Serial.flush();
}

void setup()
{
  Serial.begin(115200);
  delay(1000);
  Serial.printf("WAV start\n");

  audioLogger = &Serial;
  // file = new AudioFileSourcePROGMEM(viola, sizeof(viola));
  file = new AudioFileSourcePROGMEM(rawData, sizeof(rawData));
  id3 = new AudioFileSourceID3(file);
  id3->RegisterMetadataCB(MDCallback, (void*)"ID3TAG");
  out = new AudioOutputI2S();
  out->SetPinout(4, 5, 18);
  // out->SetRate(44100);
  mp3 = new AudioGeneratorMP3();
  mp3->begin(id3, out);
}

void loop()
{
  if (mp3->isRunning()) {
    if (!mp3->loop()) mp3->stop();
  } else {
    Serial.printf("MP3 done\n");
    delay(1000);
  }
}
