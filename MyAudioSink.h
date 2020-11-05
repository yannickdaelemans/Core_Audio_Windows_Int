#include <windows.h>
#include <iostream>
#include <mmsystem.h>
#include <MMDeviceAPI.h>
#include <AudioClient.h>
#include <AudioPolicy.h>
#include <mmreg.h>

class MyAudioSink {
public:
  // methods
  MyAudioSink();
  HRESULT SetFormat(WAVEFORMATEX *pwfx);
  HRESULT CopyData(BYTE *pData, UINT32 numFramesAvailable, BOOL *bDone);

};
