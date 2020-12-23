## Steps to build zImage and device drivers

### 1. Download(Clone) kernel source (for E9V3)
* Source: [congvue/e9v3](https://github.com/congvue/e9v3)
* `# git clone https://github.com/congvue/e9v3.git`

### 2. Add missing driver rtl8723bu (Realtek RTL8723BU Wireless Adapter)
* Source: [lwfinger/rtl8723bu](https://github.com/lwfinger/rtl8723bu)
* Clone source and put(link) source code to $(kernel)/drivers/net/wireless/rtlwifi
  ```
  # git clone https://github.com/lwfinger/rtl8723bu.git
  # ln -s rtl8723bu e9v3/drivers/net/wireless/rtlwifi/
  ```

### 3. Change kernel options
  ```
  # cp config_linux .config
  # make menuconfig
  ```

### 4. Build image and drivers
  ```
  export ARCH=arm
  export CROSS_COMPILE=arm-linux-gnueabihf-
  make dtbs
  make zImage
  ```
  Target files:
  - e9-sabresd.dtb => arch/arm/boot/dts/e9-sabresd.dtb
  - imx6q-sabresd.dtb => arch/arm/boot/dts/imx6q-sabresd.dtb
  - zImage => arch/arm/boot/zImage



## Package might required to build kernel
```
# sudo apt install libncurses5-dev libncursesw5-dev
# sudo apt install lzop
```
