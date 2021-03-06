// AudioCodingModule.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "modules/audio_coding/include/audio_coding_module.h"
#include "rtc_base/logging.h"
#include "modules/video_coding/frame_buffer.h"
#include "modules/include/module_common_types.h"
#include "api/audio_codecs/audio_format.h"
#include "api/audio_codecs/builtin_audio_encoder_factory.h"

using webrtc::AudioCodingModule;
using webrtc::AudioPacketizationCallback;
using webrtc::FrameType;
using webrtc::RTPFragmentationHeader;

class PacketizationCallback : public AudioPacketizationCallback {
	// 通过 AudioPacketizationCallback 继承
	virtual int32_t SendData(FrameType frame_type, uint8_t payload_type, uint32_t timestamp, const uint8_t * payload_data, size_t payload_len_bytes, const RTPFragmentationHeader * fragmentation) override
	{
		RTC_LOG(INFO) << __FILE__;
		return int32_t();
	}
};

int main()
{
	// 创建AudioCodingModule实例
	auto ACM = AudioCodingModule::Create();

	// 获取支持的音频编解码器
	int num = ACM->NumberOfCodecs();
	RTC_LOG(INFO) << "NumberOfCodecs : " << num;

	// 输出支持的音频编解码器
	for (int i = 0; i < num; i++) {
		webrtc::CodecInst c;
		ACM->Codec(i, &c);
		RTC_LOG(INFO) << "Codec : " << c;
	}

	// 获取opus的索引
	int index = ACM->Codec("opus", 48000,2);
	if (index < 0) {
		RTC_LOG(LS_ERROR) << "opus 48000 2 ";
		return -1;
	}

	// 获取opus的编码参数信息
	RTC_LOG(INFO) << "index of codecs : " << index;
	webrtc::CodecInst c;
	ACM->Codec(index, &c);

	ACM->RegisterSendCodec(c);

	std::shared_ptr<PacketizationCallback> pCallback(new PacketizationCallback());
	ACM->RegisterTransportCallback(pCallback.get());

	// 创建编码器工厂
	rtc::scoped_refptr<webrtc::AudioEncoderFactory>  pAudioEF = webrtc::CreateBuiltinAudioEncoderFactory();
	auto encoders = pAudioEF->GetSupportedEncoders();
	for (auto encoder : encoders) {
		RTC_LOG(INFO) << "Encoder : " << encoder;
	}
	
	// 查询更加详细的音频编码信息
	webrtc::SdpAudioFormat sdp(c.plname, c.plfreq, c.channels);
	pAudioEF->QueryAudioEncoder(sdp);
	RTC_LOG(INFO) << "sdp : " << sdp;


	// 创建编码器实例
	auto pAudioEncoder = pAudioEF->MakeAudioEncoder(c.pltype, sdp);

	system("pause");
    return 0;
}

