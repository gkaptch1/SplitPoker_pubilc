This repo contains the prototype for Prof Rubin's multi-device online poker idea.

Build instructions can be found at http://wiki.holdingnuts.net/devel:setup

This project requires QT4 and the SDL framework.
If you download/have SDL2, you can just:
 - create a copy/link for /Library/Frameworks/SDL2.framework/ at /Library/Frameworks/SDL.framework/
 - change build/src/client/CMakeFiles/holdingnuts.dir/link.txt to look for the SDL2 framework

there is a server currently running at 128.220.247.211:40888