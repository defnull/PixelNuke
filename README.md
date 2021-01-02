# Features

- [x] Displays pixels on a screen
  - [x] Lots of pixels per second (even on a raspberry pi)
  - [x] Scales nicely for different screen sizes
  - [x] Switches monitor at runtime
  - [x] Switches window/fullscreen at runtime
  - [x] Draws multiple layers (pixel layer and gui overlay)
  - [ ] Takes screenshots (on demand or every N seconds)
- [x] Network accessible
  - [x] Simple ASCII network protocol (``PX 23 42 ff8800 \n``)
  - [ ] Efficient binary network protocol
  - [ ] Pub/sub channels for text messages between clients
  - [ ] Live stream for pixel change
- [ ] Scriptable with lua (LuaJIT)
  - [ ] Script custom network commands, hooks and event handlers with lua (admin only)
  - [ ] Upload user scripts to draw pixels (time-limited and sandboxed)
- [ ] Builds and runs on:
  - [x] Linux (x86_64, armhf)
  - [ ] OS-X
  - [ ] Windows
  - [ ] Android

# Keyboard commands
    q / ESC     Quit
    c           Clear screen (with black)
    F11         Toggle fullscreen mode
    F12         Switch monitor in fullscreen mode

# Building from source

    $ sudo apt-get install make g++ pkg-config libevent-dev libglfw3-dev libglew-dev
    $ make
    $ make run

