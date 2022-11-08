/*

 Example sketch for TFT_HX9357 library.

 No fonts are needed.
 
 Draws a 3d rotating cube on the TFT screen.
 
 Original code was found at http://forum.freetronics.com/viewtopic.php?f=37&t=5495
 
 */

#define BLACK 0x0000
#define WHITE 0xFFFF

#include <TFT_HX8357.h> // Hardware-specific library
// #include "Free_Fonts.h"

TFT_HX8357 tft = TFT_HX8357();       // Invoke custom library


int16_t h;
int16_t w;

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

Line2d Render[10];
Line2d ORender[10];
int line_render_delay = 0;
int static_render_counter=0;
int static_render_index = 0;
int cycles_per_render = 50;

/***********************************************************************************************************************************/
void setup() {

  tft.init();

  h = tft.height();
  w = tft.width();

  tft.setRotation(3);

  tft.fillScreen(TFT_BLACK);

  drawLines();
}

/***********************************************************************************************************************************/
void loop() {



  for (int i = 0; i < LinestoRender ; i++)
  {
    ORender[i] = Render[i]; // stores the old line segment so we can delete it later.
    line_render_delay++;
     if (i == 3 & line_render_delay == 200) {
       scroll_line(&Render[i]);
       line_render_delay = 0;
     }
   
  }

  print("Need for Speed");

  RenderImage(); // go draw it!

//  delay(1); // Delay to reduce loop rate (reduces flicker caused by aliasing with TFT screen refresh rate)
}

/***********************************************************************************************************************************/
void RenderImage( void)
{
  // renders all the lines after erasing the old ones.
  // in here is the only code actually interfacing with the OLED. so if you use a different lib, this is where to change it.

  //Slow rendering components kept in front part of lines array, will render every n cycles. Fixes
  //flickering
  static_render_counter++;
  if (static_render_counter == cycles_per_render) {
    static_render_index = 0;
  }

  for (int i = static_render_index; i < OldLinestoRender; i++ )
  {
    tft.drawLine(ORender[i].p0.x, ORender[i].p0.y, ORender[i].p1.x, ORender[i].p1.y, BLACK); // erase the old lines.
  }


  for (int i = static_render_index; i < LinestoRender; i++ )
  {
    uint16_t color = TFT_WHITE;
    tft.drawLine(Render[i].p0.x, Render[i].p0.y, Render[i].p1.x, Render[i].p1.y, color);
  }

  static_render_index = OldLinestoRender;
  OldLinestoRender = LinestoRender;
}

 void scroll_line(struct Line2d *ret) {

   int rx0, rx1, ry0, ry1;
   int SCROLL_OFFSET = 40;
  

   //Test if overflow
   if (ret->p0.y > 320) {
     ry0 = 150;
     ry1 = 180;

   } else if (ret->p0.y < 320) {
     ry0 = (ret->p0.y + SCROLL_OFFSET);
     ry1 = ret->p1.y + SCROLL_OFFSET;

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
  // Render[0].p0.x = 400;
  // Render[0].p0.y = 0;
  // Render[0].p1.x = 300;
  // Render[0].p1.y = 150;

  //  Render[1].p0.x = 80;
  //  Render[1].p0.y = 0;
  //  Render[1].p1.x = 180;
  //  Render[1].p1.y = 150;

  //  Render[2].p0.x = 480;
  //  Render[2].p0.y = 150;
  //  Render[2].p1.x = 0;
  //  Render[2].p1.y = 150;

  //  Render[3].p0.x = 240;
  //  Render[3].p0.y = 150;
  //  Render[3].p1.x = 240;
  //  Render[3].p1.y = 120;

  Render[0].p0.x = 80;
  Render[0].p0.y = 320;
  Render[0].p1.x = 180;
  Render[0].p1.y = 150;

   Render[1].p0.x = 400;
   Render[1].p0.y = 320;
   Render[1].p1.x = 300;
   Render[1].p1.y = 150;

   Render[2].p0.x = 480;
   Render[2].p0.y = 150;
   Render[2].p1.x = 0;
   Render[2].p1.y = 150;

   Render[3].p0.x = 240;
   Render[3].p0.y = 150;
   Render[3].p1.x = 240;
   Render[3].p1.y = 180;


  LinestoRender = 4;
  OldLinestoRender = LinestoRender;

}

void print(String s)
  {
  
    
    tft.setTextColor(TFT_WHITE, TFT_BLACK);   
    tft.setTextDatum(4);
    int x = 240;
    int y = 120;

    // Length (with one extra character for the null terminator)
    int str_len = s.length() + 1; 
    
    // Prepare the character array (the buffer) 
    char char_array[str_len];
    
    // Copy it over 
    s.toCharArray(char_array, str_len);
    tft.drawCentreString(char_array, x, y, 4);
  }

  void reset(){
    tft.fillScreen(TFT_BLACK);
  }
