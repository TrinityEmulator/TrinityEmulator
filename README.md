## Trinity: Desirable Mobile Emulation through Graphics Projection
![version](https://img.shields.io/badge/Version-Beta-yellow "Beta") ![license](https://img.shields.io/badge/GuestOS-Androidx86-green "Android") ![license](https://img.shields.io/badge/Licence-GPLv2-blue.svg "Apache") ![status](https://github.com/TrinityEmulator/TrinityEmulator/actions/workflows/main.yml/badge.svg)

This is the repo for holding the code of Trinity---an Android emulator designed to simultaneously meet the goals of good compatibility, security and efficiency with our novel notion of graphics projection space.

<div align=center>
<img src=https://user-images.githubusercontent.com/96227984/162603121-267b3496-60cd-448a-89d3-8a6664b85f1e.gif width=45%>
<img src=https://user-images.githubusercontent.com/96227984/162574611-ed89b1a8-1ad8-4f65-8b85-d309fddbdc94.gif width=45%>
</div>

### Code Organization
Trinity is based on QEMU 5.0. Most of the code here remains consistent with the upstream QEMU, while our modifications are mostly decoupled modules. Therefore it's easy to upgrade to higher versions of QEMU (tested also on QEMU 5.1). We detail our modifications as follows:

|  Module  |  Purpose  |
|  ----  | ----  |
| `hw/direct-express` | This additional module implements the `data teleporting` mechanism described in our paper, which is a general-purpose data transfering pipeline between the guest and host. Leveraging a specially designed packet protocol, one can realize fast bidirectional data delivering under considerable system and data dynamics. The module here mostly serves the purpose of retrieving data from the guest and dispatches the data to a designated thread specified by the data packet. |
| `hw/express-gpu` | This module implements the host rendering engine that leverages the shadow contexts and resource handles in the `graphics projection space` to manage real contexts and resources at the host, while drawing actual frames on a window. It also provides host-side hints for realizing `flow control`. |

Other minor changes to the vanilla QEMU includ the general keyboard mapping and input device to achieve cross-platform compatibility.

### Run Trinity

* **Hardware Requirements**

    We highly recommend you run Trinity on a *WinTel (Windows on Intel) machine with an NVIDIA dedicated GPU installed with the latest driver (over version 497.09) if possible*, as this hardware/software setting is more tested than others. Other **minimal** requirements include:
    - 4-core CPU
    - 4 GB memory
    - 1920x1080 display
    - 64 GB storage

* **Software Prerequisites**
  1. Ensure that you have turned on Intel VT in the BIOS settings.
  2. Install Intel HAXM (recommended version is [v7.6.5](https://github.com/intel/haxm/releases/tag/v7.6.5), install instructions [here](https://github.com/intel/haxm/wiki/Installation-Instructions-on-Windows)). 
  3. And then you are good to go!

* **Running the Released Binary**

    We provide a packaged binary [here](https://github.com/TrinityEmulator/TrinityEmulator/releases/download/Trinity-init-release/Trinity.zip). Download and extract the ZIP file, double-click the executable `Trinity.exe` in the extracted folder to run.
### Guest OS Installation

Before you can enjoy Trinity, you may see options to run without installation or to install the Android-x86 image during system boot. 
    
  - The former allows you to quickly enjoy the journey but makes the virtual storage volatile (i.e., the next boot will erase all data) and small-size (up to only 8 GB available space). Press `Enter` on this option to run without installation.
    
  - The latter may involve more complex configurations. Refer to our [wiki](https://github.com/TrinityEmulator/TrinityEmulator/wiki/Guest-OS-Installation-Guide) for more details.

### Usages
Use Trinity as you use your mobile phones. Trinity's guest OS is packed with [@OpenGApps](https://github.com/opengapps/opengapps), therefore you can install any apps from the Google Play Store we host, even ARM-based ones! For advanced developing and problem resolving techniques, refer to our [wiki](https://github.com/TrinityEmulator/TrinityEmulator/wiki/Advanced-Usages).

### Result Reproducing

As an academic project, it's essential for readers of our paper to be able to reproduce our claimed results. We provide 1) our own measurement data and script to reproduce the figures in our paper, and 2) detailed guides for running our experiments independently.

Please go to our [wiki page](https://github.com/TrinityEmulator/TrinityEmulator/wiki#reproducing-results) for more details.

### Build

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
    Download the guest system’s images (`Android_x86_64.iso` and `hda.img`) and the executable (`Trinity.exe`) we release [here](https://github.com/TrinityEmulator/TrinityEmulator/releases/tag/Trinity-init-release), put them at the root directory of the repo, and execute the Batch script at the **MSYS2 MinGW x64** terminal (directly click on the file may not work due to missing dlls) to run Trinity.

### Developing
To debug the code, you can use the GDB provided by MSYS2. You may need to examine which GDB is used by checking the output of configuration. The last few lines should contain the location of the used GDB. Normally, it should be `/mingw64/bin/gdb-multiarch.exe` or `/mingw64/bin/gdb.exe`
### Licensing 
Our code is under the GPLv2 license.
