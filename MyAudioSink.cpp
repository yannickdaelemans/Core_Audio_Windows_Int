#include "MyAudioSink.h"

int blocksize;
int channels;

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
  std::cout << "amount of channels " << pwfx->nChannels <<'\n';
  std::cout << "block allignment " << pwfx->nBlockAlign <<'\n';
  std::cout << "bits per sample " << pwfx->wBitsPerSample <<'\n';
  std::cout << "extra info size " << pwfx->cbSize <<'\n';

  blocksize = pwfx->nBlockAlign;
  channels = pwfx->nChannels;
  return 0;
}

HRESULT MyAudioSink::CopyData(BYTE *pData, UINT32 numFramesAvailable, BOOL *bDone){
  BYTE data = 0;
  UINT32 num = numFramesAvailable;

  while (num > 0) {
    //printf("%d\n", data );
    //std::cout<< data << '\n';
    data = *pData;
    //pData++;
    //num --;

    int numSize = 0;
    UINT32 tData = *pData;
    //printf("%d\n", data );

    while (numSize < blocksize/channels){
        tData <<=8;
        tData |= *pData;

        numSize ++;
        num --;
        pData++;
    }

    printf("%d\n", tData );
  }

/*
  while (numFramesAvailable > 0) {
    //printf("%d\n", data );
    //std::cout<< data << '\n';
    UINT32* data_pointer = (UINT32*) pData;
    data = *data_pointer;
    data_pointer++;
    numFramesAvailable -= 4;

    //std::cout<< data << '\n';
    printf("%u\n", data);
    //int num = 0;
    /*
    while (num < blocksize/channels){
        printf("%d", );
    }
  } */
  return 0;
}
