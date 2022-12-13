# speak_ros_voicevox_plugin

the VoiceVox plugin for speak_ros

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

TBD

3. Execute client

```bash
ros2 run speak_ros test_client "こんにちは"
```
