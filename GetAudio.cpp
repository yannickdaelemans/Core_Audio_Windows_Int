#include <windows.h>
#include <Audiopolicy.h>
#include <Mmdeviceapi.h>
#include <stdio.h>
#include <iostream>
#include <system_error>
//#include <functiondiscoverykeys.h>
#include <Functiondiscoverykeys_devpkey.h>
#include <setupapi.h>
#include <initguid.h>  // Put this in to get rid of linker errors.
#include <devpkey.h>  // Property keys defined here are now defined inline.
#include <propsys.h>

#include "MyAudioSink.h"

#pragma comment(lib, "uuid.lib")

//-----------------------------------------------------------
// Record an audio stream from the default audio capture
// device. The RecordAudioStream function allocates a shared
// buffer big enough to hold one second of PCM audio data.
// The function uses this buffer to stream data from the
// capture device. The main loop runs every 1/2 second.
//-----------------------------------------------------------

// REFERENCE_TIME time units per second and per millisecond
#define REFTIMES_PER_SEC  10000000
#define REFTIMES_PER_MILLISEC  10000

#define EXIT_ON_ERROR(hres)  \
              if (FAILED(hres)) { goto Exit; }
#define SAFE_RELEASE(punk)  \
              if ((punk) != NULL)  \
                { (punk)->Release(); (punk) = NULL; }

const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
const IID IID_IAudioClient = __uuidof(IAudioClient);
const IID IID_IAudioCaptureClient = __uuidof(IAudioCaptureClient);

HRESULT RecordAudioStreamBLE(MyAudioSink *pMySink, LPWSTR pwszID, INT32 packetLengthCompare)
{

    HRESULT hr;
    REFERENCE_TIME hnsRequestedDuration = REFTIMES_PER_SEC;
    REFERENCE_TIME hnsActualDuration;
    UINT32 bufferFrameCount;
    UINT32 numFramesAvailable;
    IMMDeviceEnumerator *pEnumerator = NULL;
    IMMDevice *pDevice = NULL;
    IAudioClient *pAudioClient = NULL;
    IAudioCaptureClient *pCaptureClient = NULL;
    WAVEFORMATEX *pwfx = NULL;
    UINT32 packetLength = 0;
    BOOL bDone = FALSE;
    BYTE *pData;
    DWORD flags;

    hr = CoInitialize(0);

    hr = CoCreateInstance(
           CLSID_MMDeviceEnumerator, NULL,
           CLSCTX_ALL, IID_IMMDeviceEnumerator,
           (void**)&pEnumerator);
    EXIT_ON_ERROR(hr)

    hr = pEnumerator->GetDevice(pwszID, &pDevice);
    EXIT_ON_ERROR(hr)

    hr = pDevice->Activate(IID_IAudioClient, CLSCTX_ALL,
                          NULL, (void**)&pAudioClient);
    EXIT_ON_ERROR(hr)

    hr = pAudioClient->GetMixFormat(&pwfx);
    EXIT_ON_ERROR(hr)

    hr = pAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED,
                                  0, hnsRequestedDuration,
                                  0, pwfx, NULL);
    EXIT_ON_ERROR(hr)

    // Get the size of the allocated buffer.
    hr = pAudioClient->GetBufferSize(&bufferFrameCount);
    EXIT_ON_ERROR(hr)

    hr = pAudioClient->GetService(IID_IAudioCaptureClient,
                                  (void**)&pCaptureClient);
    EXIT_ON_ERROR(hr)
    printf("The buffer size is: %d\n", bufferFrameCount);

    // Notify the audio sink which format to use.
    hr = pMySink->SetFormat(pwfx);
    EXIT_ON_ERROR(hr)

    // Calculate the actual duration of the allocated buffer.
    hnsActualDuration = (double)REFTIMES_PER_SEC * packetLengthCompare / pwfx->nSamplesPerSec;

    hr = pAudioClient->Reset();
    EXIT_ON_ERROR(hr)

    hr = pAudioClient->Start();  // Start recording.
    EXIT_ON_ERROR(hr)

    // Each loop fills about half of the shared buffer.
    while (bDone == FALSE)
    {
        // Sleep for half the buffer duration.
        Sleep(hnsActualDuration/REFTIMES_PER_MILLISEC/2);

        hr = pCaptureClient->GetNextPacketSize(&packetLength);
        EXIT_ON_ERROR(hr)

        printf("packet size = %d\n", packetLength);
        while (packetLength < packetLengthCompare)
        {
            // Get the available data in the shared buffer.
            hr = pCaptureClient->GetBuffer( &pData,
                                            &numFramesAvailable,
                                            &flags, NULL, NULL);
            EXIT_ON_ERROR(hr)

            if (flags & AUDCLNT_BUFFERFLAGS_SILENT)
            {
                pData = NULL;  // Tell CopyData to write silence.
            }

            // Copy the available capture data to the audio sink.
            hr = pMySink->CopyData(pData, numFramesAvailable, &bDone);
            EXIT_ON_ERROR(hr)

            hr = pCaptureClient->ReleaseBuffer(numFramesAvailable);
            EXIT_ON_ERROR(hr)

            hr = pCaptureClient->GetNextPacketSize(&packetLength);
            EXIT_ON_ERROR(hr)
        }
    }

    hr = pAudioClient->Stop();  // Stop recording.
    EXIT_ON_ERROR(hr)

Exit:
    printf("%s\n", hr);
    CoTaskMemFree(pwfx);
    SAFE_RELEASE(pEnumerator)
    SAFE_RELEASE(pDevice)
    SAFE_RELEASE(pAudioClient)
    SAFE_RELEASE(pCaptureClient)

    return hr;
}


IMMDeviceCollection* listAudioEndpoints(){
    HRESULT hr = S_OK;
    IMMDeviceEnumerator *pEnumerator = NULL;
    IMMDeviceCollection *pCollection = NULL;
    IMMDevice *pEndpoint = NULL;
    IPropertyStore *pProps = NULL;
    LPWSTR pwszID = NULL;
    DWORD *pdwState = NULL;

    hr = CoInitialize(0);

    hr = CoCreateInstance(
           CLSID_MMDeviceEnumerator, NULL,
           CLSCTX_ALL, IID_IMMDeviceEnumerator,
           (void**)&pEnumerator);
    EXIT_ON_ERROR(hr);

    hr = pEnumerator->EnumAudioEndpoints(eAll, DEVICE_STATE_ACTIVE /*| DEVICE_STATE_UNPLUGGED | DEVICE_STATE_DISABLED*/, &pCollection);

    UINT count;
    hr = pCollection->GetCount(&count);

    if(count == 0){
        printf("no endpoints were detected");
    }
    //otherwise go through the list
    for(ULONG nr = 0; nr < count; nr++){
        hr = pCollection->Item(nr, &pEndpoint);

        hr = pEndpoint->GetId(&pwszID);
        EXIT_ON_ERROR(hr)

        hr = pEndpoint->OpenPropertyStore(
                           STGM_READ, &pProps);

        EXIT_ON_ERROR(hr)

        static PROPERTYKEY key;

        GUID IDevice_FriendlyName = { 0xa45c254e, 0xdf1c, 0x4efd, { 0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0 } };
        key.pid = 14;
        key.fmtid = IDevice_FriendlyName;
        PROPVARIANT varName;
        // Initialize container for property value.
        PropVariantInit(&varName);

        // Get the endpoint's friendly-name property.
        hr = pProps->GetValue(
                        key, &varName);
        EXIT_ON_ERROR(hr)

        // Print endpoint friendly name and endpoint ID.
        printf("Endpoint %d: \"%S\" (%S)\n",
                nr, varName.pwszVal, pwszID);

        CoTaskMemFree(pwszID);
        pwszID = NULL;
        PropVariantClear(&varName);
        SAFE_RELEASE(pProps)
        SAFE_RELEASE(pEndpoint)
    }

    SAFE_RELEASE(pEnumerator)

    Exit:
        CoTaskMemFree(pwszID);
        SAFE_RELEASE(pEnumerator)
        SAFE_RELEASE(pProps)


    return pCollection;
}

LPWSTR GetIDpEndpoint(IMMDeviceCollection *pCollection){
    IMMDevice *pEndpoint = NULL;
    LPWSTR pwszID = NULL;
    HRESULT hr;
    ULONG number;

    std::cout << "Which input would you like?";
    std::cin >> number;

    hr = pCollection->Item(number, &pEndpoint);
    hr = pEndpoint->GetId(&pwszID);

    SAFE_RELEASE(pEndpoint)
    SAFE_RELEASE(pCollection)

    return pwszID;
}

INT32 GetSampleSizeBuffer(){
  INT32 number;

  std::cout << "Which sample size of the buffer would you like? ";
  std::cin >> number;

  return number;
}



int main(int argc, char const *argv[]) {
  MyAudioSink *pMiceSink;
  MyAudioSink *pBLESink;
  IMMDeviceCollection *pCollection = listAudioEndpoints();
  LPWSTR pwszID = GetIDpEndpoint(pCollection);
  INT32 sampleSize = GetSampleSizeBuffer();
  HRESULT hr = RecordAudioStreamBLE(pBLESink, pwszID, sampleSize);
  return 0;
}

/*
 * Make list of audio endpoints
 * Make user choose which endpoint he wants
 * Get the ID from the endpoint
 * use IMMDevice::GetId   To get ID from
 * use this ID to make the device interface
 * use IMMDeviceEnumerator::GetDevice with the above ID
 * work further from the example above
 */
