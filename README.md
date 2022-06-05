## Trinity: High-Performance Mobile Emulation through Graphics Projection
![version](https://img.shields.io/badge/Version-Beta-yellow "Beta") ![license](https://img.shields.io/badge/GuestOS-Androidx86-green "Android") ![license](https://img.shields.io/badge/Licence-GPLv2-blue.svg "Apache") ![status](https://github.com/TrinityEmulator/TrinityEmulator/actions/workflows/main.yml/badge.svg) [![DOI](https://zenodo.org/badge/479100313.svg)](https://zenodo.org/badge/latestdoi/479100313)

This is the Artifact README for Trinity---an Android emulator designed to simultaneously meet the goals of good compatibility, security and efficiency with our novel notion of graphics projection space.


<p float="left" align="center">
<img src=https://user-images.githubusercontent.com/96227984/162603121-267b3496-60cd-448a-89d3-8a6664b85f1e.gif width=400/>
<img src=https://user-images.githubusercontent.com/96227984/162574611-ed89b1a8-1ad8-4f65-8b85-d309fddbdc94.gif width=400/>
</p>

### 1. Getting Started with Trinity

* **Hardware Requirements**

    You should run Trinity on a *WinTel (Windows-x64 on Intel) machine, with an NVIDIA dedicated GPU installed with the latest driver (over version 497.09) if possible* (no dedicated GPU is also acceptable), as this hardware/software combination is much more tested than the others. **Minimal** hardware configurations include:
    - 4-core CPU
    - 8 GB memory
    - 1920x1080 display
    - 128 GB storage

* **Software Prerequisites**
  1. Ensure that you have turned on Intel VT in the BIOS settings. By default, this is turned on for most PCs.
  2. Install Intel HAXM (recommended version is [v7.6.5](https://github.com/intel/haxm/releases/download/v7.6.5/haxm-windows_v7_6_5.zip). Extract the downloaded ZIP file, and simply double-click `haxm-7.6.5-setup.exe` to install. 
  3. You should turn off Windows' Hyper-V if it's enabled ([how to determine Hyper-V's state](https://docs.microsoft.com/en-us/troubleshoot/windows-client/application-management/virtualization-apps-not-work-with-hyper-v#determine-whether-the-hyper-v-hypervisor-is-running)) as it inherently conflicts with Intel HAXM. Enter the following commands in Windows Terminal and **restart Windows**: 
      ```
      bcdedit /set hypervisorlaunchtype off
      DISM /Online /Disable-Feature:Microsoft-Hyper-V
      ```
      To turn Hyper-V back on later if you need, try:
      ```
      bcdedit /set hypervisorlaunchtype auto
      Enable-WindowsOptionalFeature -Online -FeatureName Microsoft-Hyper-V -All
      DISM /Online /Enable-Feature /All /FeatureName:Microsoft-Hyper-V
      ```

* **Running the Released Binary**

    We provide a packaged binary [here](https://github.com/TrinityEmulator/TrinityEmulator/releases/tag/Trinity-Release). Download and extract the ZIP file, double-click the executable `Trinity.exe` in the extracted folder to run. Press `Enter` when the boot option is `Run Android-x86 without installation`.

### 2. Detailed Instructions    
#### 2.1. Guest OS Installation

Running without installation allows you to quickly see what is Trinity capable of. But it also makes the virtual storage volatile (i.e., the next boot will erase all data) and small-size (up to only 8 GB available space).

Before you can fully enjoy Trinity, you may want to install the Android-x86 image during system boot. Refer to our [wiki](https://github.com/TrinityEmulator/TrinityEmulator/wiki/Guest-OS-Installation-Guide) for more detailed instructions.  

#### 2.2. Usages and Problem Resolving
Use Trinity as you use your mobile phones. Trinity's guest OS is packed with [@OpenGApps](https://github.com/opengapps/opengapps), therefore you can install any apps from the Google Play Store we host, even ARM-based ones! Other important notes are listed below.

* For establishing `ADB` connection with Trinity, see [here](https://github.com/TrinityEmulator/TrinityEmulator/wiki/Advanced-Usages#adb-connection).

* If Trinity freezes on certain machines (perhaps during your first boot without installation), see [here](https://github.com/TrinityEmulator/TrinityEmulator/wiki/Advanced-Usages#shutdown-and-force-shutdown). 

* If Trinity cannot function properly after installation, see [here](https://github.com/TrinityEmulator/TrinityEmulator/wiki/Advanced-Usages#formatting-the-disk).

* If Trinity cannot start properly, send us the `log.txt` file in the root directory of the binary.

#### 2.3. Result Reproducing

We provide 1) our own measurement data and script to reproduce the figures in our paper, and 2) detailed guides and videos for running our experiments independently.

Please go to our [wiki page](https://github.com/TrinityEmulator/TrinityEmulator/wiki#reproducing-results) for more details.

### 3. Artifact Claims

* **Reproducibility.** To reproduce results similar to that shown in our paper, hardware configurations of your evaluation machines are of vital importance. This is because our key results of graphics benchmarks test the ***extreme performance*** of the evaluated emulators. In particular, even if the hardware configurations are exactly the same, the running states of the host machines (e.g., CPU/GPU occupation and heat level) can also impact the results. We recommend you check your hardware settings before evaluation as suggested [here](https://github.com/TrinityEmulator/TrinityEmulator/wiki/Graphics-Benchmark#experimental-setup).

* **Stability.** Trinity is not yet stable, especially the version we release is that of the paper submission time to prevent recent fixes/improvements from affecting the evaluation results. Therefore, freezing and crashing can sometimes happen, normally a system reboot can resolve the issues. 

The following parts of README are not important to the running of Trinity and result reproducing, feel free to skip them if you do not wish to build Trinity from scratch.


### 4. Code Organization
Trinity is based on QEMU 5.0. Most of the code here remains consistent with the upstream QEMU, while our modifications are mostly decoupled modules. Therefore it's easy to upgrade to higher versions of QEMU (tested also on QEMU 5.1). We detail our modifications as follows:

|  Module  |  Purpose  |
|  ----  | ----  |
| `hw/direct-express` | This additional module implements the `data teleporting` mechanism described in our paper, which is a general-purpose data transfering pipeline between the guest and host. Leveraging a specially designed packet protocol, one can realize fast bidirectional data delivering under considerable system and data dynamics. The module here mostly serves the purpose of retrieving data from the guest and dispatches the data to a designated thread specified by the data packet. |
| `hw/express-gpu` | This module implements the host rendering engine that leverages the shadow contexts and resource handles in the `graphics projection space` to manage real contexts and resources at the host, while drawing actual frames on a window. It also provides host-side hints for realizing `flow control`. |

Other minor changes to the vanilla QEMU includ the general keyboard mapping and input device to achieve cross-platform compatibility.

### 5. Build

We use git submodule to hold some of the essential modules. Thus, after `git clone`, you should also run `git submodule update --init --recursive` at the repo's root directory.

#### Windows
* **Environment & Dependencies**

    The building of Trinity on Windows (tested on 64-bit Win 10/11 Home/Professional/LTSC) should be performed under the `MSYS2` environment. Your environment setup should include:

   1. Download and install MSYS2 following the instructions at [https://www.msys2.org/](https://www.msys2.org/). ***CRITICAL NOTE*: DO NOT INSTALL MSYS AT PATH THAT CONTAINS A SPACE, e.g., C:/Program Files**.
   2. Open the **MSYS2 MinGW x64** terminal (***CRITICAL NOTE*: NOT THE MSYS2 MSYS TERMINAL**) and install dependencies using:
   
      ``pacman -S base-devel git mingw-w64-x86_64-binutils mingw-w64-x86_64-crt-git mingw-w64-x86_64-headers-git mingw-w64-x86_64-gcc-libs mingw-w64-x86_64-gcc mingw-w64-x86_64-gdb mingw-w64-x86_64-make mingw-w64-x86_64-tools-git mingw-w64-x86_64-pkg-config mingw-w64-x86_64-winpthreads-git mingw-w64-x86_64-libwinpthread-git mingw-w64-x86_64-winstorecompat-git mingw-w64-x86_64-libmangle-git mingw-w64-x86_64-pixman mingw-w64-x86_64-SDL2 mingw-w64-x86_64-glib2 mingw-w64-x86_64-capstone mingw-w64-x86_64-glfw``
   3. Additional environmental adjustments (also executed at the MSYS terminal):
   
      ``cp /mingw64/bin/ar.exe /mingw64/bin/x86_64-w64-mingw32-ar.exe & cp /mingw64/bin/ranlib.exe /mingw64/bin/x86_64-w64-mingw32-ranlib.exe & cp /mingw64/bin/windres.exe /mingw64/bin/x86_64-w64-mingw32-windres.exe & cp /mingw64/bin/objcopy.exe /mingw64/bin/x86_64-w64-mingw32-objcopy.exe  & cp /mingw64/bin/nm.exe /mingw64/bin/x86_64-w64-mingw32-nm.exe & cp /mingw64/bin/strip.exe /mingw64/bin/x86_64-w64-mingw32-strip.exe``
* **Configure**

    `cd` to the root directory of the repo at the MSYS terminal (note that you *cannot* use a typical Windows path such as `C:/XXX` for `cd`, instead you can simply drag and drop the repo to the terminal to acquire its path in MSYS), input the following to configure:
    `./configure --cross-prefix=x86_64-w64-mingw32- --disable-gtk --enable-sdl  --target-list=x86_64-softmmu --disable-werror`
* **Compile**

    After the configuration, simply type `make install -j$(nproc)`.

* **Run the Built Trinity**

    We provide a proxy executable for running Trinity. Therefore, all you need to do is: 
    Download the guest system’s images (`Android_x86_64.iso` and `hda.img`) and the executable (`Trinity.exe`) packed in [the Trinity.zip file](https://github.com/TrinityEmulator/TrinityEmulator/releases/tag/Trinity-Release), put them at the root directory of the repo, and execute the executable at the **MSYS2 MinGW x64** terminal (directly click on the file may not work due to missing dlls) to run Trinity.

### 6. Developing
To debug the code, you can use the GDB provided by MSYS2. You may need to examine which GDB is used by checking the output of configuration. The last few lines should contain the location of the used GDB. Normally, it should be `/mingw64/bin/gdb-multiarch.exe` or `/mingw64/bin/gdb.exe`
### 7. Licensing 
Our code is under the GPLv2 license.

### 8. Reference
Trinity's paper is to appear in OSDI'22. If you use Trinity in your work, please reference it using
```
@inproceedings {di2022trinity,
    author = {Di Gao and Hao Lin and Zhenhua Li and Chengen Huang and Liangyi Gong and Feng Qian and Yunhao Liu and Tianyin Xu},
    title = {Trinity: Desirable Mobile Emulation through Graphics Projection},
    booktitle = {16th {USENIX} Symposium on Operating Systems Design and Implementation ({OSDI} 22)},
    year = {2022},
    publisher = {{USENIX} Association}
  }
```
