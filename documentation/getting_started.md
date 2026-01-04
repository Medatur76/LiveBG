# Getting Started
## Installation
Follow the instructions **[here](../README.md#installation)** on how to properlly install this project.
## Requirements
To double check everything, make sure that your projects contains these files from this repository
```
.
├── headers/
│   ├── font_8x16.h
│   ├── layer-shell-unstable-v1-client-protocol.h
│   ├── window_handler.h
│   └── xdg-shell-client-protocol.h
├── include/
│   ├── font_8x16.c
│   ├── layer-shell-unstable-v1-protocol.c
│   ├── window_handler.c
│   └── xdg-shell-protocol.c
└── main.c 
```
Its recommened copying the ``main.c`` file but we will be going over how to make the whole file so its not a requirement
## Main file
Your main file can be named anything but for this tutorial it will be refered to as ``main.c``. If you copied this file from the repository you can skip this part.  

Inside ``main.c``, there are two big requirements that need fulfilling.  
First is a single import that allows your whole project to work.  

Inside your ``main.c`` you must have:
```C
#ifndef WINDOW_HANDLER
#define WINDOW_HANDLER
#include "headers/window_handler.h"
#endif
```
This allows us to get all of the functions required for the generation and manipulation of the background window.  

The second requirement is a main function that sets up our window.  

Inside ``main.c``, after the imports, add the main function which should look something like this:
```C
int main() {
    return runWindow();
}
```
``runWindow()`` doesnt have to be the return value of the main function, however if the function recieves certain errors, an error code will be returned so this is just an easy way on handling it.  

### Congrats!
If you run ``run.sh`` (or compile and run the program on your own) you will see a couple log messages. Now if you minimize all you windows you will find a black box covering most your screen!  

Now you are ready to move ontop **[Drawing](./drawing.md)**!