#include "MyAudioSink.h"

MyAudioSink::MyAudioSink(){

}
/*
 * WAVEFORMATEX: https://docs.microsoft.com/en-us/windows/win32/api/mmeapi/ns-mmeapi-waveformatex
 * A structure wich will define the format of the waveform of the audio
 * will give the following data:
 * typedef struct tWAVEFORMATEX {
 * WORD  wFormatTag;
 * WORD  nChannels;
 * DWORD nSamplesPerSec;
 * DWORD nAvgBytesPerSec;
 * WORD  nBlockAlign;
 * WORD  wBitsPerSample;
 * WORD  cbSize;
 * }
 */
HRESULT MyAudioSink::SetFormat(WAVEFORMATEX *pwfx){
  std::cout << "samples per second " << pwfx->nSamplesPerSec <<'\n';
  return 0;
}

HRESULT MyAudioSink::CopyData(BYTE *pData, UINT32 numFramesAvailable, BOOL *bDone){
  BYTE data = 0;
  while (numFramesAvailable > 0) {
    data = *pData;
    std::cout<< "data" << data << '\n';
    pData++;
    --numFramesAvailable;
  }
  return 0;
}
