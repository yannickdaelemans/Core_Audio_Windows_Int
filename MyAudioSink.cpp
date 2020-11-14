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

  if(pwfx->cbSize >= 22){
    WAVEFORMATEXTENSIBLE *waveFormatExtensible = reinterpret_cast<WAVEFORMATEXTENSIBLE *>(pwfx);
    if(waveFormatExtensible->SubFormat == KSDATAFORMAT_SUBTYPE_IEEE_FLOAT){
        printf("the variable type is a float\n");
    }
    if(waveFormatExtensible->SubFormat == KSDATAFORMAT_SUBTYPE_PCM){
        printf("the term is a PCM\n");
    }
  }

  return 0;
}



HRESULT MyAudioSink::CopyData(BYTE *pData, UINT32 numFramesAvailable, BOOL *bDone){
  BYTE data = 0;
  UINT32 num = 10;

  while (num > 0) {
    union{
      float f;
      BYTE b[4];
    } src;

    data = *pData;
    int numSize = 0;
    INT32 tData = *pData;
    BYTE* dataBegin = pData;
    //printf("%d\n", data );
    //numSize = blocksize/channels -1;
    while (numSize <= blocksize/channels -1 ){

        src.b[numSize] = *pData;
        numSize++;
        pData++;
    }
    num --;
    BYTE* dataEnd = pData;
    FLOAT fData = *((float*)&src);
    //FLOAT fData;
    /*
    std::copy(reinterpret_cast<const BYTE*>(dataBegin),
              reinterpret_cast<const BYTE*>(dataEnd),
              reinterpret_cast<BYTE*>(&fData));
              */
    printf("%f\n", fData );
  }

  return 0;
}
