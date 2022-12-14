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

#ifndef SPEAK_ROS_VOICEVOX_PLUGIN__VOICEVOX_PLUGIN_HPP
#define SPEAK_ROS_VOICEVOX_PLUGIN__VOICEVOX_PLUGIN_HPP

#include <speak_ros/speak_ros_plugin_base.hpp>

namespace voicevox_plugin
{
class VoiceVoxPlugin : public speak_ros::SpeakROSPluginBase
{
public:
  VoiceVoxPlugin() : speak_ros::SpeakROSPluginBase() {}

  std::string getPluginName() const override { return "voicevox_plugin"; }

  std::filesystem::path generateSoundFile(
    const std::string input_text, const std::filesystem::path output_directory,
    const std::string file_name) override;

  std::vector<speak_ros::Parameter> getParametersDefault() const override;

  void importParameters(const std::unordered_map<std::string, std::string> & parameters) override;

  std::string speaker_id;
  std::string host_name;
};
}  // namespace voicevox_plugin

#include <pluginlib/class_list_macros.hpp>
PLUGINLIB_EXPORT_CLASS(voicevox_plugin::VoiceVoxPlugin, speak_ros::SpeakROSPluginBase)

#endif  // SPEAK_ROS_VOICEVOX_PLUGIN__VOICEVOX_PLUGIN_HPP
