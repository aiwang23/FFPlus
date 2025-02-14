//
// Created by 10484 on 25-1-1.
//

#include "utils.h"

#include <cstdio>

#if defined(WIN32) || defined(_WIN32) || defined(_WIN32_) || defined(WIN64) || defined(_WIN64) || defined(_WIN64_) // win



#include <Windows.h>
#include <dshow.h>

#elif defined(__linux__)	// linux

#endif

extern "C" {
#include <libavutil/error.h>
#include <libavutil/pixdesc.h>
}

constexpr int MAX_FRIENDLY_NAME_LENGTH = 128;
constexpr int MAX_MONIKER_NAME_LENGTH = 256;

typedef struct _TDeviceName
{
	WCHAR FriendlyName[MAX_FRIENDLY_NAME_LENGTH];
	WCHAR MonikerName[MAX_MONIKER_NAME_LENGTH];
} TDeviceName;

std::string rsCode2Msg(int rsCode)
{
	char err_s[128] = {0};
	av_strerror(rsCode, err_s, 128);

	std::string msg;
	msg.resize(128);
	strncpy(msg.data(), err_s, 128);
	return msg;
}

void printErrMsg(const char *funName, int line, int rsCode)
{
	fprintf(stderr, "[PrintErrMsg](%s:%d) %s\n", funName, line, rsCode2Msg(rsCode).c_str());
	// char err_s[128] = {0};
	// av_strerror(rsCode, err_s, 128);
	// fprintf(stderr, "[PrintErrMsg](%s:%d) %s\n", funName, line, err_s);
}


static HRESULT GetAudioVideoInputDevices(std::vector<TDeviceName> &vectorDevices, REFGUID guidValue)
{
	TDeviceName name;
	HRESULT hr;

	// 初始化
	vectorDevices.clear();

	// 初始化COM
	hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

	if (FAILED(hr))
	{
		return hr;
	}

	// 创建系统设备枚举器实例
	ICreateDevEnum *pSysDevEnum = NULL;
	hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum,
	                      (void **) &pSysDevEnum);
	if (FAILED(hr))
	{
		CoUninitialize();
		return hr;
	}

	// 获取设备类枚举器

	IEnumMoniker *pEnumCat = NULL;
	hr = pSysDevEnum->CreateClassEnumerator(guidValue, &pEnumCat, 0);
	if (hr == S_OK)
	{
		// 枚举设备名称
		IMoniker *pMoniker = NULL;
		ULONG cFetched;
		while (pEnumCat->Next(1, &pMoniker, &cFetched) == S_OK)
		{
			IPropertyBag *pPropBag;
			hr = pMoniker->BindToStorage(NULL, NULL, IID_IPropertyBag, (void **) &pPropBag);
			if (SUCCEEDED(hr))
			{
				// 获取设备友好名
				VARIANT varName;
				VariantInit(&varName);
				hr = pPropBag->Read(L"FriendlyName", &varName, NULL);
				if (SUCCEEDED(hr))
				{
					StringCchCopy(reinterpret_cast<STRSAFE_LPSTR>(name.FriendlyName),
					              MAX_FRIENDLY_NAME_LENGTH, reinterpret_cast<STRSAFE_LPSTR>(varName.bstrVal));
					// 获取设备Moniker名
					LPOLESTR pOleDisplayName = reinterpret_cast<LPOLESTR>(CoTaskMemAlloc(MAX_MONIKER_NAME_LENGTH * 2));
					if (pOleDisplayName != NULL)
					{
						hr = pMoniker->GetDisplayName(NULL, NULL, &pOleDisplayName);

						if (SUCCEEDED(hr))
						{
							StringCchCopy(reinterpret_cast<STRSAFE_LPSTR>(name.MonikerName),
							              MAX_MONIKER_NAME_LENGTH, reinterpret_cast<STRSAFE_LPSTR>(pOleDisplayName));
							vectorDevices.push_back(name);
						}

						CoTaskMemFree(pOleDisplayName);
					}
				}
				VariantClear(&varName);
				pPropBag->Release();
			}

			pMoniker->Release();
		} //  End for While

		pEnumCat->Release();
	}

	pSysDevEnum->Release();
	CoUninitialize();
	return hr;
}

static char *WCharToChar(WCHAR *s)
{
	int w_nlen = WideCharToMultiByte(CP_ACP, 0, s, -1, NULL, 0, NULL, FALSE);
	char *ret = new char[w_nlen];
	memset(ret, 0, w_nlen);
	WideCharToMultiByte(CP_ACP, 0, s, -1, ret, w_nlen, NULL, FALSE);
	return ret;
}

std::vector<deviceInfo> findAudioDevices()
{
	std::vector<deviceInfo> audio_infos;

	std::vector<TDeviceName> audioDevices;
	GetAudioVideoInputDevices(audioDevices, CLSID_AudioInputDeviceCategory);

	for (TDeviceName device_info: audioDevices)
	{
		deviceInfo audiodevice;
		snprintf(audiodevice.vedioname, 128, "%s", WCharToChar(device_info.FriendlyName));
		snprintf(audiodevice.vediodesc, 256, "%s", WCharToChar(device_info.MonikerName));
		audio_infos.push_back(audiodevice);
	}

	return audio_infos;
}

std::vector<deviceInfo> findVideoDevices()
{
	std::vector<deviceInfo> video_infos;

	std::vector<TDeviceName> videoDevices;
	GetAudioVideoInputDevices(videoDevices, CLSID_VideoInputDeviceCategory);

	for (TDeviceName device_info: videoDevices)
	{
		deviceInfo audiodevice;
		snprintf(audiodevice.vedioname, 128, "%s", WCharToChar(device_info.FriendlyName));
		snprintf(audiodevice.vediodesc, 256, "%s", WCharToChar(device_info.MonikerName));
		video_infos.push_back(audiodevice);
	}

	return video_infos;
}

const AVCodec *findffDeCodec(AVCodecID codec_id)
{
	return avcodec_find_decoder(codec_id);
}

const AVCodec *findffDeCodec(const char *codec_name)
{
	return avcodec_find_decoder_by_name(codec_name);
}

const AVCodec *findffEnCodec(AVCodecID codec_id)
{
	return avcodec_find_encoder(codec_id);
}

const AVCodec *findffEnCodec(const char *codec_name)
{
	return avcodec_find_encoder_by_name(codec_name);
}

const AVInputFormat *findffInputFormat(const char *format_name)
{
	return av_find_input_format(format_name);
}

const AVOutputFormat *findffOutputFormat(const char *format_name)
{
	return av_guess_format(format_name, nullptr, nullptr);
}

const char *sampleFormat2Str(AVSampleFormat sample_fmt)
{
	return av_get_sample_fmt_name(sample_fmt);
}

const char *pixFormat2Str(AVPixelFormat pix_fmt)
{
	return av_get_pix_fmt_name(pix_fmt);
}

dictionary::dictionary()
= default;

dictionary::~dictionary()
{
	free();
}

int dictionary::setOpt(const char *key, const char *value, int flags)
{
	return av_dict_set(&dictionary_, key, value, flags);
}

const char *dictionary::getOpt(const char *key)
{
	const AVDictionaryEntry *entry = av_dict_get(dictionary_, key, nullptr, 0);
	return entry->value;
}

int dictionary::count()
{
	return av_dict_count(dictionary_);
}

void dictionary::free()
{
	av_dict_free(&dictionary_);
}

AVDictionary **dictionary::ffDictionaryPtr()
{
	return dictionary_ ? &dictionary_ : nullptr;
}
