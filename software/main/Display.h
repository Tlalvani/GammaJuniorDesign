#include <TFT_HX8357.h> // Hardware-specific library
#include <SD.h>
#define RBUFF_SIZE 256

#define BLACK 0x0000
#define WHITE 0xFFFF

#define BU_BMP 1 // Temporarily flip the TFT coords for standard Bottom-Up bit maps
#define TD_BMP 0 // Draw inverted Top-Down bitmaps in standard coord frame
class Display
{
private:
  TFT_HX8357 tft;
  int line_render_delay = 0;
  int static_render_counter = 0;
  int cycles_per_render = 50;
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

public:
  Display()
  {
    tft.init();
    tft.setRotation(3);
    if (!SD.begin(53))
    {
      Serial.println("SD fail");
      return;
    }
    Serial.println("SD good");
  }

  void drawRAW(char *filename, int16_t x, int16_t y, int16_t rawWidth, int16_t rawHeight)
  {
    File rawFile;
    uint8_t sdbuffer[2 * RBUFF_SIZE]; // SD read pixel buffer (16 bits per pixel)
    Serial.println(filename);

    // Check file exists and open it
    if ((rawFile = SD.open(filename)) == NULL)
    {
      Serial.println(F(" File not found"));
      return;
    }

    // Prepare the TFT screen area to receive the data
    tft.setWindow(x, y, x + rawWidth - 1, y + rawHeight - 1);

    // Work out how many whole buffers to send
    uint16_t nr = ((long)rawHeight * rawWidth) / RBUFF_SIZE;
    while (nr--)
    {
      rawFile.read(sdbuffer, sizeof(sdbuffer));
      tft.pushColors(sdbuffer, RBUFF_SIZE);
    }

    // Send any partial buffer
    nr = ((long)rawHeight * rawWidth) % RBUFF_SIZE;
    if (nr)
    {
      rawFile.read(sdbuffer, nr << 1); // We load  2 x nr bytes
      tft.pushColors(sdbuffer, nr);    // We send nr 16 bit pixels
    }

    // Close the file
    rawFile.close();
  }

#define BUFF_SIZE 80

  void drawBMP(char *filename, int x, int y, boolean flip)
  {
    if ((x >= tft.width()) || (y >= tft.height()))
      return;
    File bmpFile;
    int16_t bmpWidth, bmpHeight; // Image W+H in pixels
    // uint8_t  bmpDepth;            // Bit depth (must be 24) but we dont use this
    uint32_t bmpImageoffset;           // Start address of image data in file
    uint32_t rowSize;                  // Not always = bmpWidth; may have padding
    uint8_t sdbuffer[3 * BUFF_SIZE];   // SD read pixel buffer (8 bits each R+G+B per pixel)
    uint16_t tftbuffer[BUFF_SIZE];     // TFT pixel out buffer (16-bit per pixel)
    uint8_t sd_ptr = sizeof(sdbuffer); // sdbuffer pointer (so BUFF_SIZE must be less than 86)
    boolean goodBmp = false;           // Flag set to true on valid header parse
    int16_t w, h, row, col;            // to store width, height, row and column
    // uint8_t  r, g, b;   // brg encoding line concatenated for speed so not used
    uint8_t rotation;    // to restore rotation
    uint8_t tft_ptr = 0; // buffer pointer

    // Check file exists and open it
    Serial.println(filename);
    if ((bmpFile = SD.open(filename)) == NULL)
    {
      Serial.println(F(" File not found")); // Can comment out if not needed
      return;
    }

    // Parse BMP header to get the information we need
    if (read16(bmpFile) == 0x4D42)
    {                                   // BMP file start signature check
      read32(bmpFile);                  // Dummy read to throw away and move on
      read32(bmpFile);                  // Read & ignore creator bytes
      bmpImageoffset = read32(bmpFile); // Start of image data
      read32(bmpFile);                  // Dummy read to throw away and move on
      bmpWidth = read32(bmpFile);       // Image width
      bmpHeight = read32(bmpFile);      // Image height

      // if (read16(bmpFile) == 1) {
      //  Only proceed if we pass a bitmap file check
      //  Number of image planes -- must be '1', depth 24 and 0 (uncompressed format)
      if ((read16(bmpFile) == 1) && (read16(bmpFile) == 24) && (read32(bmpFile) == 0))
      {
        // goodBmp = true; // Supported BMP format -- proceed!
        //  BMP rows are padded (if needed) to 4-byte boundary
        rowSize = (bmpWidth * 3 + 3) & ~3;
        // Crop area to be loaded
        w = bmpWidth;
        h = bmpHeight;

        // We might need to alter rotation to avoid tedious pointer manipulation
        // Save the current value so we can restore it later
        rotation = tft.getRotation();
        // Use TFT SGRAM coord rotation if flip is set for 25% faster rendering
        if (flip)
          tft.setRotation((rotation + (flip << 2)) % 8); // Value 0-3 mapped to 4-7

        // We might need to flip and calculate new y plot coordinate
        // relative to top left corner as well...
        switch (rotation)
        {
        case 0:
          if (flip)
            y = tft.height() - y - h;
          break;
        case 1:
          y = tft.height() - y - h;
          break;
          break;
        case 2:
          if (flip)
            y = tft.height() - y - h;
          break;
          break;
        case 3:
          y = tft.height() - y - h;
          break;
          break;
        }

        // Set TFT address window to image bounds
        // Currently, image will not draw or will be corrputed if it does not fit
        // TODO -> efficient clipping, but I don't need it to be idiot proof ;-)
        tft.setWindow(x, y, x + w - 1, y + h - 1);

        // Finally we are ready to send rows of pixels, writing like this avoids slow 32 bit multiply
        for (uint32_t pos = bmpImageoffset; pos < bmpImageoffset + h * rowSize; pos += rowSize)
        {
          // Seek if we need to on boundaries and arrange to dump buffer and start again
          if (bmpFile.position() != pos)
          {
            bmpFile.seek(pos);
            sd_ptr = sizeof(sdbuffer);
          }

          // Fill the pixel buffer and plot
          for (col = 0; col < w; col++)
          { // For each column...
            // Time to read more pixel data?
            if (sd_ptr >= sizeof(sdbuffer))
            {
              // Push tft buffer to the display
              if (tft_ptr)
              {
                // Here we are sending a uint16_t array to the function
                tft.pushColors(tftbuffer, tft_ptr);
                tft_ptr = 0; // tft_ptr and sd_ptr are not always in sync...
              }
              // Finally reading bytes from SD Card
              bmpFile.read(sdbuffer, sizeof(sdbuffer));
              sd_ptr = 0; // Set buffer index to start
            }
            // Convert pixel from BMP 8+8+8 format to TFT compatible 16 bit word
            // Blue 5 bits, green 6 bits and red 5 bits (16 bits total)
            // Is is a long line but it is faster than calling a library fn for this
            tftbuffer[tft_ptr] = (sdbuffer[sd_ptr++] >> 3);
            tftbuffer[tft_ptr] |= ((sdbuffer[sd_ptr++] & 0xFC) << 3);
            tftbuffer[tft_ptr] |= ((sdbuffer[sd_ptr++] & 0xF8) << 8);
            tft_ptr++;
          } // Next row
        }   // All rows done

        // Write any partially full buffer to TFT
        if (tft_ptr)
          tft.pushColors(tftbuffer, tft_ptr);

      } // End of bitmap access
    }   // End of bitmap file check
    //}     // We can close the file now

    bmpFile.close();
    // if(!goodBmp) Serial.println(F("BMP format not recognized."));
    tft.setRotation(rotation); // Put back original rotation
  }

  /***************************************************************************************
  ** Function name:           Support functions for drawBMP()
  ** Descriptions:            Read 16 and 32-bit types from the SD card file
  ***************************************************************************************/

  // BMP data is stored little-endian, Arduino is little-endian too.
  // May need to reverse subscript order if porting elsewhere.

  uint16_t read16(File &f)
  {
    uint16_t result;
    ((uint8_t *)&result)[0] = f.read(); // LSB
    ((uint8_t *)&result)[1] = f.read(); // MSB
    return result;
  }

  uint32_t read32(File &f)
  {
    uint32_t result;
    ((uint8_t *)&result)[0] = f.read(); // LSB
    ((uint8_t *)&result)[1] = f.read();
    ((uint8_t *)&result)[2] = f.read();
    ((uint8_t *)&result)[3] = f.read(); // MSB
    return result;
  }
  void print(String s, int x, int y, int size, uint16_t color)
  {
  
    tft.setRotation(3);
    // tft.setTextColor(TFT_WHITE, TFT_BLACK);   
    tft.setTextDatum(4);

    // Length (with one extra character for the null terminator)
    int str_len = s.length() + 1; 
    
    // Prepare the character array (the buffer) 
    char char_array[str_len];
    
    // Copy it over 
    s.toCharArray(char_array, str_len);
    tft.setCursor(x, y);  
    tft.setTextColor(color);  
    tft.setTextSize(size);  
    tft.print(char_array);
    // tft.drawString(char_array, x, y, size);
  }

  void reset(){
    tft.fillScreen(TFT_BLACK);
  }

  void image(char * s)
  {
    tft.setRotation(1);
    drawBMP(s, 0, 0, 0);
  }

  void setScreen() {

    
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

    Render[4].p0.x = 240;
    Render[4].p0.y = 200;
    Render[4].p1.x = 240;
    Render[4].p1.y = 230;

    Render[5].p0.x = 240;
    Render[5].p0.y = 250;
    Render[5].p1.x = 240;
    Render[5].p1.y = 280;

    Render[6].p0.x = 240;
    Render[6].p0.y = 300;
    Render[6].p1.x = 240;
    Render[6].p1.y = 330;

    // this.print("Hit the start button!", 240, 90, 4)


    LinestoRender = 7;
  }

void updateScreen(int forceCycle) {
 line_render_delay++;

  for (int i = 0; i < LinestoRender ; i++)
  {
    ORender[i] = Render[i]; // stores the old line segment so we can delete it later.
  if (i > 2 && line_render_delay == 50) {
      scroll_line(&Render[i]);
     }
  }

  if (line_render_delay == 50) line_render_delay = 0;

  //Force render delay to remove flicker
  static_render_counter++;
  if (forceCycle == 1) static_render_counter = 0;
  if (static_render_counter % cycles_per_render == 0) {
      RenderRoad(); 
      static_render_counter = 0;
  }
}

  void scroll_line(struct Line2d *ret) {

   int rx0, rx1, ry0, ry1;
   int SCROLL_OFFSET = 20;
   Serial.print("hello");
  
    Serial.print(ret->p0.y);
   //Test if overflow
   if (ret->p0.y >= 320) {
     ry0 = 150 + (ret->p0.y - 320);
     ry1 = ry0 + 30;

   } else if (ret->p0.y < 320) {
     ry0 = (ret->p0.y + SCROLL_OFFSET);
     ry1 = ret->p1.y + SCROLL_OFFSET;

   }

  
   // ret->p0.x = rx0;
   ret->p0.y = ry0;

   // ret->p1.x = rx1;
   ret->p1.y = ry1;

 }

  void RenderRoad()
{
  // renders all the lines after erasing the old ones.
  // in here is the only code actually interfacing with the OLED. so if you use a different lib, this is where to change it.

  //Slow rendering components kept in front part of lines array, will render every n cycles. Fixes
  //flickering

  for (int i = 0; i < LinestoRender; i++ )
  {
    tft.drawLine(ORender[i].p0.x, ORender[i].p0.y, ORender[i].p1.x, ORender[i].p1.y, BLACK); // erase the old lines.
    tft.drawLine(Render[i].p0.x, Render[i].p0.y, Render[i].p1.x, Render[i].p1.y, WHITE);
  }


  // for (int i = 0; i < LinestoRender; i++ )
  // {
  //   uint16_t color = TFT_WHITE;
  //   tft.drawLine(Render[i].p0.x, Render[i].p0.y, Render[i].p1.x, Render[i].p1.y, color);
  // }

}

};
