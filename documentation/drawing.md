# Drawing
## How does drawing work?
Because this project uses Wayland, there is a buffer (``pixel``, which is accessible anywhere where the ``window_handler.h`` header file is imported) that contains ``width * height * 4`` values which is read in ``BGRA`` for each pixel. This buffer is read every time the screen is drawn to the filled completely with the ``background`` color (also located in the ``window_handler.h`` file). Due to this buffer's huge size, there are premade [drawing functions](#drawing-functions) that can do the heavy lifting for you.
## Render Event
Located in the ``window_handler.h`` file is a type definition for a function point called ``RenderEventListener``, which takes in one parameter, that being the current size of the screen, as well as a function called ``appendRenderListener``. This function takes in a ``RenderEventListener`` and adds it to a list of other functions to be called every frame.
## Example
If I wanted to render ``Hello, World!`` on the top-left of the screen for example, I would first make the function. This function must return ``void`` and take in one parameter of type ``struct Size``. That would look like:
```C
void drawMessage(struct Size currentSize) {
    //A nice green color
    Pixel color = {
        0xFF, //A
        0x12, //R
        0x7B, //G
        0x27  //B
    };
    //Drawing text on the screen
    drawText(0 /*x pos*/, 0 /*y pos*/, "Hello, World!" /*string to print*/, color /*color of the string*/, currentSize /*current size of the screen*/);
}
```
Then, to tell the draw loop to call this function, I would call ``appendRenderListener``. For this example we will call this in the main function (located in ``main.c``) but this can be put in any function **as long as it's called at some point**. After adding out render listener, the main function now looks like:
```C
int main() {
    appendRenderListener(drawMessage);
    return runWindow();
}
```
If you are following this tutorial exactly, then you ``main.c`` file should now look like this:
```C
#ifndef LIVEBG
#define LIVEBG
#include "headers/liveBG.h"
#endif

void drawMessage(struct Size currentSize) {
    Pixel color = {
        0xFF,
        0x12,
        0x7B,
        0x27
    };
    drawText(0, 0, "Hello, World!", color, currentSize);
}

int main() {
    appendRenderListener(drawMessage);
    return runWindow();
}
```
And now you should see ``Hello, World!`` printed in green at the top corner of you screen!
## Drawing Functions
_There aren't many drawing functions as of right now but more are coming!_

```C
void drawChar(int x, int y, char character, Pixel color, struct Size size)
```
Takes in an x position, y position, character, color, and the current screen size. This function will then render the character on the screen using an 8x16 pixel font, with the top-left most pixel of the character being rendered at the x and y position.
```C
void drawText(int x, int y, const unsigned char* str, Pixel color, struct Size size)
```
Takes in an x position, y position, string, color, and the current screen size. This function will then render the string on the screen an 8x16 pixel font, with the top-left most pixel of the first character being rendered at the x and y position. In addition, there is support for the ``'\n'`` character. If this is found in the string, the x position of the next character is placed and the x parameter and the y position is moved down.
```C
void drawRect(int x, int y, int w, int h, struct Color color, struct Size size)
```
Takes in an x position, y position, width, height, color, and the current screen size. This function will then render a rectangle with the width and height specified and with the top-left corner being located at the x and y position.
```C
void drawCircle(int cx, int cy, double r, struct Color color, struct Size size)
```
Takes in the x and y position of the center of the circle, the radius, color, and current screen size. This function will then render a circle with the radius and color specified and centered at the ``cx`` and ``cy`` position.
## Congrats!
At this time you've learned all there is to know! LiveBG is supposed to be as flexible as possible to your needs for it, hence why this tutorial is so short. If you would like to see a very complex example of a working background then go **[here](./CRIM50N_html.c)** (this file would replace ``main.c``). Happy coding!