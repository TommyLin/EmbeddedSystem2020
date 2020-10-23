## 調整LCD解析度
#### Step 1: Install `fbset`
You only need to install once.</br>
`$ apt install fbset`

#### Step 2: Change resolution
`$ fbset -fb /dev/fb0 -g 1024 600 1024 600 16`
