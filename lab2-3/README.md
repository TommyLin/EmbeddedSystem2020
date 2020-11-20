# Lab 2-3

## Run command on a system reboot
`$ crontab -e` to ***e***dit cron
```
SHELL=/bin/bash
@reboot echo 0 > /sys/class/graphics/fbcon/cursor_blink
@reboot sleep 10 && /root/lab2-3/auto_play
```
`$ crontab -l` to ***l***ist current setting

`auto_play` will browse *.jpg file at /root/wallpapers after boot. Please put your wallpapares at /root/wallpapers.
