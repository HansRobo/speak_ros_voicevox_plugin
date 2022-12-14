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
  const std::string file_name)
{
  std::string generated_file_path = output_directory / (file_name + ".wav");

  std::string base_url = "http://" + host_name + ":50021";

  auto audio_query_task = pplx::create_task([=] {
                            web::http::client::http_client_config config;
                            web::http::client::http_client client(base_url, config);
                            return client.request(
                              web::http::methods::POST,
                              web::http::uri_builder(U("/audio_query"))
                                .append_query(U("text"), input_text)
                                .append_query(U("speaker"), speaker_id)
                                .to_string(),
                              web::json::value().serialize(), "application/json");
                          }).then([](web::http::http_response response) {
    if (response.status_code() == web::http::status_codes::OK) {
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

        web::http::client::http_client_config config;
        web::http::client::http_client client(base_url, config);
        return client.request(
          web::http::methods::POST,
          web::http::uri_builder(U("/synthesis"))
            .append_query(U("speaker"), speaker_id)
            .to_string(),
          audio_query_response.body(), "application/json");
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

std::vector<speak_ros::Parameter> voicevox_plugin::VoiceVoxPlugin::getParametersDefault() const
{
  return {
    // clang-format off
      {"speaker_id", "voicevox speaker id", "1"},
      {"host_name", "host of voicevox engine", "localhost"}
    // clang-format on
  };
}

void voicevox_plugin::VoiceVoxPlugin::importParameters(
  const std::unordered_map<std::string, std::string> & parameters)
{
  speaker_id = parameters.at("speaker_id");
  host_name = parameters.at("host_name");
}
