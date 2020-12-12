
### 1. Download kernel source (for E9V3)
* [congvue/e9v3](https://github.com/congvue/e9v3)

### 2. Add missing driver rtl8723bu - Realtek RTL8723BU Wireless Adapter
* [lwfinger/rtl8723bu](https://github.com/lwfinger/rtl8723bu)
Clone source and put source code to $(kernel)/drivers/net/wireless/rtlwifi

### 3. `# make menuconfig`

### 4. `# make`

## Package might required to build kernel
```
# sudo apt install libncurses5-dev libncursesw5-dev
# sudo apt install lzop
```
