# How to install mpg123 on E9V3
* Please make sure E9V3 is connected to ethernet

## :cake: mpg123
### Install mpg123
```
# apt -y install mpg123
```

### Play mp3 in a folder
```
# mpg123 /root/mp3/spring/*
```

## :cake: Volume Control
### Install volume control utility "amixer" from alsa-utils
```
# apt -y install alsa-utils
```

### List sound devices
```
# amixer scontrols
Simple mixer control 'Headphone',0
Simple mixer control 'Headphone Mux',0
Simple mixer control 'Headphone Playback ZC',0
Simple mixer control 'PCM',0
Simple mixer control 'Mic',0
Simple mixer control 'Capture',0
Simple mixer control 'Capture Attenuate Switch (-6dB)',0
Simple mixer control 'Capture Mux',0
Simple mixer control 'Capture ZC',0
```
### Set volume (command line)
```
# amixer sset 'Headphone' 80%
```

### Get volume (command line)
```
# amixer sget 'Headphone'
Simple mixer control 'Headphone',0
  Capabilities: pvolume
  Playback channels: Front Left - Front Right
  Limits: Playback 0 - 127
  Mono:
  Front Left: Playback 127 [100%] [-13.40dB]
  Front Right: Playback 127 [100%] [-13.40dB]
```

## Reference:
1. [Set volume from terminal](https://unix.stackexchange.com/questions/32206/set-volume-from-terminal)
