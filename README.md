# speak_ros_voicevox_plugin

the VoiceVox plugin for speak_ros

## Install

```bash
mkdir -p speak_ros_ws/src
cd speak_ros_ws/src
git clone https://github.com/HansRobo/speak_ros
git clone https://github.com/HansRobo/speak_ros_voicevox_plugin
cd ..
source /opt/ros/humble/setup.bash
rosdep install -riy --from-paths src
colcon build --symlink-install
```
## How to Use

1. Execute VoiceVox

```bash
docker run --rm -it -p '127.0.0.1:50021:50021' voicevox/voicevox_engine:cpu-ubuntu20.04-latest
```

with GPU support
```bash
docker run --rm --gpus all -p '127.0.0.1:50021:50021' voicevox/voicevox_engine:nvidia-ubuntu20.04-latest
```

2. Execute speak_ros with VoiceVox plugin

```bash
source speak_ros_ws/install/setup.bash
ros2 run speak_ros speak_ros_node --ros-args -p plugin_name:=voicevox_plugin::VoiceVoxPlugin
```

3. Execute client

```bash
source speak_ros_ws/install/setup.bash
ros2 run speak_ros test_client "こんにちは"
```
