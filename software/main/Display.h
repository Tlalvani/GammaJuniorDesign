class Display
{
private:
    TFT_HX8357 tft;

public:
    Display()
    {
        tft.init();
        tft.setRotation(3);
    }

    void print(String s)
    {
        tft.fillScreen(TFT_WHITE);
        tft.setCursor(0, 0, 2);
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        tft.setTextSize(3);
        tft.println(s);
    }

    void text(String s)
    {
        tft.println(s);
    }
};
