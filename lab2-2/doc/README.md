# Lab 2-2

### How to disable cursor
`$ echo 0 > /sys/class/graphics/fbcon/cursor_blink`

### Set LCD upside down
`$ echo 1 > /sys/class/graphics/fb0/rotate`

### Rstore LCD direction
`$ echo 0 > /sys/class/graphics/fb0/rotate`
