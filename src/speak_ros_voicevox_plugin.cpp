// Copyright 2022 Kotaro Yoshimoto All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <cpprest/filestream.h>
#include <cpprest/http_client.h>

#include "speak_ros_voicevox_plugin/voicevox_plugin.hpp"

std::filesystem::path voicevox_plugin::VoiceVoxPlugin::generateSoundFile(
    const std::string input_text, const std::filesystem::path output_directory,
    const std::string file_name) {

  updateParameters();

  std::string generated_file_path = output_directory / (file_name + ".wav");

  std::string base_url = "http://" + host_name + ":50021";

  auto audio_query_task =
      pplx::create_task([=] {
        web::http::client::http_client_config config;
        web::http::client::http_client client(base_url, config);
        web::json::value body;

        body[U("accent_phrases")] = web::json::value::array();
        body[U("speedScale")] = web::json::value::number(speed_scale);
        body[U("pitchScale")] = web::json::value::number(pitch_scale);
        body[U("volumeScale")] = web::json::value::number(volume_scale);
        body[U("prePhonemeLength")] =
            web::json::value::number(pre_phoneme_length);
        body[U("postPhonemeLength")] =
            web::json::value::number(post_phoneme_length);
        body[U("outputSamplingRate")] =
            web::json::value::number(output_sampling_rate);
        body[U("outputStereo")] = web::json::value::string(output_stereo);

        return client.request(web::http::methods::POST,
                              web::http::uri_builder(U("/audio_query"))
                                  .append_query(U("text"), input_text)
                                  .append_query(U("speaker"), speaker)
                                  .to_string(),
                              web::json::value().serialize(),
                              U("application/json"));
      }).then([](web::http::http_response response) {
        if (response.status_code() == web::http::status_codes::OK) {
          std::cout << "audio_query succeeded" << std::endl;
          std::cout << response.to_string() << std::endl;
          return response;
        }
        throw std::runtime_error("network error");
      });

  audio_query_task.wait();
  auto audio_query_response = audio_query_task.get();

  using concurrency::streams::ostream;
  auto file_stream = std::make_shared<ostream>();

  pplx::task<void> synthesis_task =
      concurrency::streams::fstream::open_ostream(generated_file_path)
          .then([&](ostream out_file) {
            *file_stream = out_file;

            web::json::value body = audio_query_response.extract_json().get();
            body[U("speedScale")] = web::json::value::number(speed_scale);
            body[U("pitchScale")] = web::json::value::number(pitch_scale);
            body[U("volumeScale")] = web::json::value::number(volume_scale);
            body[U("prePhonemeLength")] =
                web::json::value::number(pre_phoneme_length);
            body[U("postPhonemeLength")] =
                web::json::value::number(post_phoneme_length);
            body[U("outputSamplingRate")] =
                web::json::value::number(output_sampling_rate);
            body[U("outputStereo")] = web::json::value::string(output_stereo);

            web::http::client::http_client_config config;
            web::http::client::http_client client(base_url, config);
            return client.request(web::http::methods::POST,
                                  web::http::uri_builder(U("/synthesis"))
                                      .append_query(U("speaker"), speaker)
                                      .to_string(),
                                  body.serialize(), "application/json");
          })
          .then([&](web::http::http_response response) {
            if (response.status_code() == web::http::status_codes::OK) {
              // write out the audio
              return response.body().read_to_end(file_stream->streambuf());
            }
            throw std::runtime_error("network error");
          })
          .then([=](size_t) {
            // close the file stream
            return file_stream->close();
          });

  synthesis_task.wait();

  return std::filesystem::path(generated_file_path);
}

std::vector<speak_ros::Parameter>
voicevox_plugin::VoiceVoxPlugin::getParametersDefault() const {
  return {
      // clang-format off
      {"speaker", "[number/integer] voicevox speaker id", 1},
      {"host_name", "[string] host of voicevox engine", "localhost"},
      {"speedScale", "[number] voice speed. larger is faster", 2.0},
      {"pitchScale", "[number] voice pitch. larger is higher", 0.0},
      {"intensityScale", "[number] intensity scale. larger is more intense", 1.0},
      {"volumeScale", "[number] volume scale. larger is more volume", 1.0},
      {"prePhonemeLength", "[number] pre phoneme length [sec]", 0.1},
      {"postPhonemeLength", "[number] post phoneme length [sec]", 0.1},
      {"outputSamplingRate", "[number] output sampling rate [Hz]", 24000},
      {"outputStereo", "[bool] output stereo", "true"}
      // clang-format on
  };
}

void voicevox_plugin::VoiceVoxPlugin::importParameters(
    const std::unordered_map<
        std::string, std::variant<int, double, std::string>> &parameters) {
  speaker = std::get<int>(parameters.at("speaker"));
  host_name = std::get<std::string>(parameters.at("host_name"));
  speed_scale = std::get<double>(parameters.at("speedScale"));
  pitch_scale = std::get<double>(parameters.at("pitchScale"));
  intensity_scale = std::get<double>(parameters.at("intensityScale"));
  volume_scale = std::get<double>(parameters.at("volumeScale"));
  pre_phoneme_length = std::get<double>(parameters.at("prePhonemeLength"));
  post_phoneme_length = std::get<double>(parameters.at("postPhonemeLength"));
  output_sampling_rate = std::get<int>(parameters.at("outputSamplingRate"));
  output_stereo = std::get<std::string>(parameters.at("outputStereo"));
}
