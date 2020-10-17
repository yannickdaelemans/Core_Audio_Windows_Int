#include <windows.h>
#include <iostream>

class MyAudioSink {
public:
  MyAudioSink();
  HRESULT SetFormat(WAVEFORMATEX *pwfx);
  HRESULT CopyData(BYTE *pData, UINT32 numFramesAvailable, BOOL *bDone);

};
