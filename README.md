## Trinity: Desirable Mobile Emulation through Graphics Projection
![version](https://img.shields.io/badge/Version-Beta-yellow "Beta")
![license](https://img.shields.io/badge/GuestOS-Androidx86-green "Android")
![license](https://img.shields.io/badge/Licence-GPLv2-blue.svg "Apache")

This is the repo for holding the code of Trinity---an Android emulator designed to simultaneously meet the goals of good compatibility, security and efficiency with our novel notion of graphics projection space.

### Code Organization
Trinity is based on QEMU 5.0. Most of the code here remains consistent with the upstream QEMU, while our modifications are mostly decoupled modules. Therefore it's easy to upgrade to higher versions of QEMU (tested also on QEMU 5.1). We detail our modifications as follows:

|  Module  |  Purpose  |
|  ----  | ----  |
| `hw/direct-express` | This additional module implements the `data teleporting` mechanism described in our paper, which is a general-purpose data transfering pipeline between the guest and host. Leveraging a specially designed packet protocol, one can realize fast bidirectional data delivering under considerable system and data dynamics. The module here mostly serves the purpose of retrieving data from the guest and dispatches the data to a designated thread specified by the data packet. |
| `hw/express-gpu` | This module implements the host rendering engine that leverages the shadow contexts and resource handles in the `graphics projection space` to manage real contexts and resources at the host, while drawing actual frames on a window. It also provides host-side hints for realizing `flow control`. |

Other minor changes to the vanilla QEMU includ the general keyboard mapping and input device to achieve cross-platform compatibility.

### Build

We use git submodule to hold some of the essential modules. Thus, after `git clone`, you should also run `git submodule update --init --recursive` at the repo's root directory.

#### Windows
* **Environment & Dependencies**

    The building of Trinity on Windows (tested on 64-bit Win 10/11 Home/Professional/LTSC) should be performed under the `MSYS2` environment. Your environment setup should include:

   1. Download and install MSYS2 following the instructions at [https://www.msys2.org/](https://www.msys2.org/)
   2. Open the MSYS2 MinGW 64-bit terminal and install dependencies using:
   
      ``pacman -S base-devel git mingw-w64-x86_64-binutils mingw-w64-x86_64-crt-git mingw-w64-x86_64-headers-git mingw-w64-x86_64-gcc-libs mingw-w64-x86_64-gcc mingw-w64-x86_64-gdb mingw-w64-x86_64-make mingw-w64-x86_64-tools-git mingw-w64-x86_64-pkg-config mingw-w64-x86_64-winpthreads-git mingw-w64-x86_64-libwinpthread-git mingw-w64-x86_64-winstorecompat-git mingw-w64-x86_64-libmangle-git mingw-w64-x86_64-pixman mingw-w64-x86_64-SDL2 mingw-w64-x86_64-glib2 mingw-w64-x86_64-capstone mingw-w64-x86_64-glfw``
   3. Additional environmental adjustments (also executed at the MSYS terminal):
   
      ``cp /mingw64/bin/ar.exe /mingw64/bin/x86_64-w64-mingw32-ar.exe & cp /mingw64/bin/ranlib.exe /mingw64/bin/x86_64-w64-mingw32-ranlib.exe & cp /mingw64/bin/windres.exe /mingw64/bin/x86_64-w64-mingw32-windres.exe & cp /mingw64/bin/objcopy.exe /mingw64/bin/x86_64-w64-mingw32-objcopy.exe``
* **Configure**

    `cd` to the root directory of the repo at the MSYS terminal, input the following to configure:
    `./configure --cross-prefix=x86_64-w64-mingw32- --disable-gtk --enable-sdl  --target-list=x86_64-softmmu --disable-werror`
* **Compile**

    After the configuration, simply type `make -j${#threads}`, where `#threads` is the number of threads for compiling.
### Install & Run

* **Install**

    Installation is optional. If you want to install, type `make install` to do so. However, you can run fine without installation.

* **Run An Emulator**
  * Prerequisites
    1. Ensure that you have turned on Intel VT in the BIOS settings.
    2. Install Intel HAXM (instructions [here](https://github.com/intel/haxm/wiki/Installation-Instructions-on-Windows).
    And then you are good to go!

  * Running
  
    We provide an automatic script for running Trinity. Therefore, all you need to do is download the [guest system’s ISO](https://drive.google.com/drive/folders/1-2s3oKei5XgpkhVPKF8quxxWTwWctm5-?usp=sharing) and [Batch file](https://drive.google.com/drive/folders/1-2s3oKei5XgpkhVPKF8quxxWTwWctm5-?usp=sharing) we provide, put them at the root directory of the repo, and double-click on the Batch file `run.cmd` to execute Trinity.

    During booting, you may see an option to run without installation or to install provided by the Android-x86 system we host. The former allows you to quickly enjoy the journey but makes the virtual storage volatile (i.e., the next boot will erase all data), while the latter may involve more complex configurations (you can find them [here](https://www.android-x86.org/installhowto.html)).
### Developing
To debug the code, you can use the GDB provided by MSYS2. You may need to examine which GDB is used by checking the output of configuration. The last few lines should contain the location of the used GDB. Normally, it should be `/mingw64/bin/gdb-multiarch.exe` or `/mingw64/bin/gdb.exe`
### Licensing 
Our code is under the GPLv2 license.
