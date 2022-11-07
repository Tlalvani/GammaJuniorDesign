/*

 Example sketch for TFT_HX9357 library.

 No fonts are needed.
 
 Draws a 3d rotating cube on the TFT screen.
 
 Original code was found at http://forum.freetronics.com/viewtopic.php?f=37&t=5495
 
 */

#define BLACK 0x0000
#define WHITE 0xFFFF

#include <TFT_HX8357.h> // Hardware-specific library

TFT_HX8357 tft = TFT_HX8357();       // Invoke custom library



struct Point2d
{
  int x;
  int y;
};

int LinestoRender; // lines to render.
int OldLinestoRender; // lines to render just in case it changes. this makes sure the old lines all get erased.


struct Line2d
{
  Point2d p0;
  Point2d p1;
};

Line2d Render[3];
Line2d ORender[3];

/***********************************************************************************************************************************/
void setup() {

  tft.init();

  h = tft.height();
  w = tft.width();

  tft.setRotation(1);

  tft.fillScreen(TFT_BLACK);

  drawLines();
}

/***********************************************************************************************************************************/
void loop() {

  for (int i = 0; i < LinestoRender ; i++)
  {
    ORender[i] = Render[i]; // stores the old line segment so we can delete it later.
    scroll_line();
  }

  RenderImage(); // go draw it!

  delay(9); // Delay to reduce loop rate (reduces flicker caused by aliasing with TFT screen refresh rate)
}

/***********************************************************************************************************************************/
void RenderImage( void)
{
  // renders all the lines after erasing the old ones.
  // in here is the only code actually interfacing with the OLED. so if you use a different lib, this is where to change it.

  for (int i = 0; i < OldLinestoRender; i++ )
  {
    tft.drawLine(ORender[i].p0.x, ORender[i].p0.y, ORender[i].p1.x, ORender[i].p1.y, BLACK); // erase the old lines.
  }


  for (int i = 0; i < LinestoRender; i++ )
  {
    uint16_t color = TFT_BLUE;
    tft.drawLine(Render[i].p0.x, Render[i].p0.y, Render[i].p1.x, Render[i].p1.y, color);
  }
  OldLinestoRender = LinestoRender;
}

void scroll_line(struct Line2d *ret) {

  int rx0, rx1, ry0, ry1;
  int SCROLL_OFFSET = 40;
  

  //Test if overflow
  if (ret.y0 > 480) {
    ry0 = 200;
    ry1 = 160;

  } else if (ret.y0 < 480) {
    ry0 = (ret.y0 + SCROLL_OFFSET);
    ry1 = ret.y1 + SCROLL_OFFSET;

  }

  
  // ret->p0.x = rx0;
  ret->p0.y = ry0;

  // ret->p1.x = rx1;
  ret->p1.y = ry1;

}

/***********************************************************************************************************************************/


/***********************************************************************************************************************************/
// line segments to draw a cube. basically p0 to p1. p1 to p2. p2 to p3 so on.
void drawLines() {
  Lines[0].p0.x = 400;
  Lines[0].p0.y = 318;
  Lines[0].p0.x = 304;
  Lines[0].p0.y = 153;

  Lines[1].p0.x = 129;
  Lines[1].p0.y = 318;
  Lines[1].p1.x = 208;
  Lines[1].p1.y = 154;

  Lines[2].p0.x = 258;
  Lines[2].p0.y = 200;
  Lines[2].p1.x = 258;
  Lines[2].p1.y = 160;


  LinestoRender = 2;
  OldLinestoRender = LinestoRender;

}



