#include <Arduino.h>

#include "AudioFileSourcePROGMEM.h"
#include "AudioGeneratorWAV.h"
#include "AudioOutputI2S.h"

// VIOLA sample taken from https://ccrma.stanford.edu/~jos/pasp/Sound_Examples.html
// #include "viola.h"
// #include "fly_64k.h"
// #include "cantina.h"
#include "fly_short.h"

AudioGeneratorWAV *wav;
AudioFileSourcePROGMEM *file;
AudioOutputI2S *out;

void setup()
{
  Serial.begin(115200);
  delay(1000);
  Serial.printf("WAV start\n");

  audioLogger = &Serial;
  // file = new AudioFileSourcePROGMEM(viola, sizeof(viola));
  file = new AudioFileSourcePROGMEM(rawData, sizeof(rawData));
  out = new AudioOutputI2S();
  out->SetPinout(4, 5, 18);
  // out->SetRate(44100);
  wav = new AudioGeneratorWAV();
  wav->begin(file, out);
}

void loop()
{
  if (wav->isRunning()) {
    if (!wav->loop()) wav->stop();
  } else {
    Serial.printf("WAV done\n");
    delay(1000);
  }
}
