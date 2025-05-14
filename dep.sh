sudo dnf install llvm clang clang-tools-extra lldb

Build SDL3
sudo dnf install gcc git-core make cmake SDL3 SDL3-devel SDL3_image-devel \
alsa-lib-devel pulseaudio-libs-devel nas-devel pipewire-devel \
libX11-devel libXext-devel libXrandr-devel libXcursor-devel libXfixes-devel \
libXi-devel libXScrnSaver-devel dbus-devel ibus-devel \
systemd-devel mesa-libGL-devel libxkbcommon-devel mesa-libGLES-devel \
mesa-libEGL-devel vulkan-devel wayland-devel wayland-protocols-devel \
libdrm-devel mesa-libgbm-devel libdecor-devel \
pipewire-jack-audio-connection-kit-devel liburing-devel

Leak : valgrind --leak-check=full  --track-origins=no --log-file=memcheck.log build/Debug/hello
