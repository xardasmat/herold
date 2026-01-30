#include <piper.h>

#include <audio.hpp>
#include <demux.hpp>
#include <fstream>
#include <iostream>
#include <mux.hpp>

int main() {
  // std::clog << "opening test/der_tank_de.flac" << std::endl;
  // std::ifstream movie_audio_istream("test/der_tank_de.flac");
  // std::clog << "Demuxing test/der_tank_de.flac" << std::endl;
  // Demux movie_audio(movie_audio_istream);
  // auto movie_audio_decoder = movie_audio.GetDecoder(0);
  // AudioDecoder<int32_t> decoded_movie_audio(movie_audio_decoder);

  // std::clog << "extracting sound test/der_tank_de.flac" << std::endl;
  // int32_t total = 0;
  // int64_t index = 0;
  // while (auto packet = movie_audio.read()) {
  //   movie_audio_decoder.Write(*packet);
  //   while (auto sample = decoded_movie_audio.Read()) {
  //     // total += sample->sample(0) + sample->sample(1);
  //     // ++index;
  //     // if (index%10000000 == 0) std::clog << index/10000000 << std::endl;
  //     // // std::cout << sample->sample(0) << " " << sample->sample(1) <<
  //     std::endl;
  //     // std::cout.write((char*)&(sample->sample(0)), 8);
  //   }
  // }

  piper_synthesizer* synth = piper_create(
      "test/bezi.onnx", "test/bezi.onnx.json",
      "build/espeak_ng/src/espeak_ng_external-build/espeak-ng-data");

  const AVCodec* codec = avcodec_find_encoder(AV_CODEC_ID_MP3);
  AVCodecContext* ctx = avcodec_alloc_context3(codec);
  AVCodecParameters* params = avcodec_parameters_alloc();
  avcodec_parameters_from_context(params, ctx);
  avcodec_free_context(&ctx);
  params->sample_rate = 44100;
  params->bit_rate = 128000;
  params->format = AVSampleFormat::AV_SAMPLE_FMT_S16;
  params->bits_per_coded_sample = 16;
  params->block_align = 2;
  av_channel_layout_default(&params->ch_layout, 1);

  std::ofstream audio_stream("test/output.mp3", std::ios::binary);
  Mux audio_stream_mux(audio_stream, "mp3", {params});
  auto mux_encoder = audio_stream_mux.MakeEncoder(0);
  AudioEncoder<int16_t> mux_audio_encoder(mux_encoder);

  piper_synthesize_options options = piper_default_synthesize_options(synth);

  std::string text;
  std::getline(std::cin, text, '\0');

  piper_synthesize_start(synth, text.c_str(),
                         &options /* NULL for defaults */);

  piper_audio_chunk chunk;
  while (piper_synthesize_next(synth, &chunk) != PIPER_DONE) {
    for (int i = 0; i < chunk.num_samples; ++i) {
      AudioSample<int16_t> sample(1);
      sample.sample(0) = chunk.samples[i] * 32000;
      mux_audio_encoder.Write(sample);
      mux_audio_encoder.Write(sample);
      while (auto pkt = mux_encoder.Read()) {
        audio_stream_mux.Write(std::move(*pkt));
      }
    }
  }
  mux_audio_encoder.Flush();
  while (auto pkt = mux_encoder.Read()) {
    audio_stream_mux.Write(std::move(*pkt));
  }

  piper_free(synth);

  return 0;
}
