#define TFT_CS     10
#define TFT_RST    9
#define TFT_DC     8
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>
#include "sprite.c"

const int TICKS_PER_SECOND = 25;
const int SKIP_TICKS = 1000 / TICKS_PER_SECOND;
const int MAX_FRAMESKIP = 5;

const int trunk_w = 29;
const int trunk_l = 25;
const int leg_w = 29;
const int leg_l = 6;
int dino_y = 97;
int y;
int y_old;
int tree_pos = 80;
int dino_x = 35;
int type_old;

int type = 2;
int color;

double diff;
float u;
bool button_press = "false";
unsigned long next_game_tick;
unsigned long game_F;
unsigned long t1;
unsigned long t2;
unsigned long t3;

int loops;
float interpolation;

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

void button() {
  t2 = millis();
  u = 90.0;
  button_press = true;
  tone(7, 523,30);
  //Serial.println("Pressed");
}


void update_game() {
  t1 = millis();
  if (button_press) {
    diff = (abs(t1 - t2)) / 1000.00;
    y = u * diff - 35 * diff * diff;
    Serial.println(dino_y, 8);
    if (y <= 0 && diff > 1) {             //detect touch down
      u = 0;
      y = 0;
      t2 = 0;
      button_press = false;
      //Serial.println("Down");
    }
  }
  if (t3 < t1) {
    t3 = t1 + 3000;
    type = random(1, 3);

    if (type_old != type) {
      if (type_old == 1) {
        tft.fillRoundRect(tree_pos, 99, 4, 29, 1.5, tft.Color565( 0x80, 0x80, 0x80));
        tft.fillRoundRect(tree_pos - 10, 113, 20, 4, 1.5, tft.Color565( 0x80, 0x80, 0x80));
        tft.fillRoundRect(tree_pos - 10, 113 - 8, 4, 10, 1.5, tft.Color565( 0x80, 0x80, 0x80));
        tft.fillRoundRect(tree_pos + 8, 113 - 10, 4, 12, 1.5, tft.Color565( 0x80, 0x80, 0x80));
      } else if (type_old == 2 ) {
        tft.fillRoundRect(tree_pos, 108, 4, 20, 1.5, tft.Color565( 0x80, 0x80, 0x80));
        tft.fillRoundRect(tree_pos - 8, 115, 18, 4, 1.5, tft.Color565( 0x80, 0x80, 0x80));
        tft.fillRoundRect(tree_pos - 9, 115 - 8, 4, 10, 1.5, tft.Color565( 0x80, 0x80, 0x80));
        tft.fillRoundRect(tree_pos + 8, 115 - 8, 4, 10, 1.5, tft.Color565( 0x80, 0x80, 0x80));

        tft.fillRoundRect(tree_pos + 25, 108, 4, 20, 1.5, tft.Color565( 0x80, 0x80, 0x80));
        tft.fillRoundRect(tree_pos - 8 + 25, 115, 18, 4, 1.5, tft.Color565( 0x80, 0x80, 0x80));
        tft.fillRoundRect(tree_pos - 9 + 25, 115 - 8, 4, 10, 1.5, tft.Color565( 0x80, 0x80, 0x80));
        //tft.fillRoundRect(tree_pos+8+25,115-8,4,10,1.5,tft.Color565( 0x80, 0x80, 0x80));

      }
      type_old = type;
      tree_pos = 160;

    }
  }


}

void display_game() {
  //check if the dino pose changed, if yes rewrite the old one with background and write the new one else only do leg animation
  //use game_F to switch between leg frames (odd/even)
  if (y_old != y) {
    drawBitmap(dino_x, dino_y - y, body, trunk_w, trunk_l, 0x07E0);
    drawBitmap(dino_x, dino_y - y + trunk_l, leg1, leg_w, leg_l, 0x07E0);
    tft.fillRect(dino_x, dino_y - y - 5, trunk_w + 5, trunk_l + 5, tft.Color565( 0x80, 0x80, 0x80)); //clearing extra bits
    //drawBitmap(dino_x, dino_y - y, body, 40, 35, tft.Color565( 0x80, 0x80, 0x80));
    tft.fillRect(dino_x, dino_y - y + trunk_l - 5, leg_w + 5, leg_l + 10, tft.Color565( 0x80, 0x80, 0x80));
    //drawBitmap(dino_x, dino_y - y+ 35, leg1, 40, 8, tft.Color565( 0x80, 0x80, 0x80));
    y_old = y;
    delay(2);
  } else
  {
    drawBitmap(dino_x, dino_y - y, body, trunk_w, trunk_l, 0x07E0);
    if (game_F % 3 == 0) {
      //drawBitmap(dino_x, dino_y - y+ 35, leg2, 40, 8, tft.Color565( 0x80, 0x80, 0x80));
      tft.fillRect(dino_x, dino_y - y + trunk_l, leg_w, leg_l, tft.Color565( 0x80, 0x80, 0x80));
      drawBitmap(dino_x, dino_y - y + trunk_l, leg1, leg_w, leg_l, 0x07E0);
      //delay(250);

    }
    else {
      tft.fillRect(dino_x, dino_y - y + trunk_l, leg_w, leg_l, tft.Color565( 0x80, 0x80, 0x80));
      drawBitmap(dino_x, dino_y - y + trunk_l, leg2, leg_w, leg_l, 0x07E0);
      //delay(250);

    }
    game_F++;
  }
  if (type == 1 ) {
    tft.fillRoundRect(tree_pos, 99, 4, 29, 1.5, tft.Color565( 0x80, 0x80, 0x80));
    tft.fillRoundRect(tree_pos - 10, 113, 20, 4, 1.5, tft.Color565( 0x80, 0x80, 0x80));
    tft.fillRoundRect(tree_pos - 10, 113 - 8, 4, 10, 1.5, tft.Color565( 0x80, 0x80, 0x80));
    tft.fillRoundRect(tree_pos + 8, 113 - 10, 4, 12, 1.5, tft.Color565( 0x80, 0x80, 0x80));
  } else if (type == 2 ) {
    tft.fillRoundRect(tree_pos, 108, 4, 20, 1.5, tft.Color565( 0x80, 0x80, 0x80));
    tft.fillRoundRect(tree_pos - 8, 115, 18, 4, 1.5, tft.Color565( 0x80, 0x80, 0x80));
    tft.fillRoundRect(tree_pos - 9, 115 - 8, 4, 10, 1.5, tft.Color565( 0x80, 0x80, 0x80));
    tft.fillRoundRect(tree_pos + 8, 115 - 8, 4, 10, 1.5, tft.Color565( 0x80, 0x80, 0x80));

    tft.fillRoundRect(tree_pos + 25, 108, 4, 20, 1.5, tft.Color565( 0x80, 0x80, 0x80));
    tft.fillRoundRect(tree_pos - 8 + 25, 115, 18, 4, 1.5, tft.Color565( 0x80, 0x80, 0x80));
    tft.fillRoundRect(tree_pos - 9 + 25, 115 - 8, 4, 10, 1.5, tft.Color565( 0x80, 0x80, 0x80));
    //tft.fillRoundRect(tree_pos+8+25,115-8,4,10,1.5,tft.Color565( 0x80, 0x80, 0x80));

  }

  if (tree_pos > -40) {
    tree_pos = tree_pos - 4;
    color = ST7735_GREEN;
  } else {
    color = tft.Color565( 0x80, 0x80, 0x80); // to avoid stray pixels due to border effects
  }



  if (type == 1) {
    tft.fillRoundRect(tree_pos, 99, 4, 29, 1.5, color);
    tft.fillRoundRect(tree_pos - 10, 113, 20, 4, 1.5, color);
    tft.fillRoundRect(tree_pos - 10, 113 - 8, 4, 10, 1.5, color);
    tft.fillRoundRect(tree_pos + 8, 113 - 10, 4, 12, 1.5, color);
  } else if (type == 2 ) {
    tft.fillRoundRect(tree_pos, 108, 4, 20, 1.5, color);
    tft.fillRoundRect(tree_pos - 8, 115, 18, 4, 1.5, color);
    tft.fillRoundRect(tree_pos - 9, 115 - 8, 4, 10, 1.5, color);
    tft.fillRoundRect(tree_pos + 8, 115 - 8, 4, 10, 1.5, color);

    tft.fillRoundRect(tree_pos + 25, 108, 4, 20, 1.5, color);
    tft.fillRoundRect(tree_pos - 8 + 25, 115, 18, 4, 1.5, color);
    tft.fillRoundRect(tree_pos - 9 + 25, 115 - 8, 4, 10, 1.5, color);
    //tft.fillRoundRect(tree_pos+8+25,115-8,4,10,1.5,ST7735_GREEN);

  }

}

void drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color) {
  int16_t i, j, byteWidth = (w + 7) / 8;
  uint8_t byte;

  for (j = 0; j < h; j++) {
    for (i = 0; i < w; i++) {
      if (i & 7) byte <<= 1;
      else      byte   = pgm_read_byte(bitmap + j * byteWidth + i / 8);
      if (byte & 0x80) tft.drawPixel(x + i, y + j, color);
    }
  }
}


void setup() {
  next_game_tick = millis();
  Serial.begin(9600);
  pinMode(2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2), button, FALLING);
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(3);
  tft.fillScreen(tft.Color565( 0x80, 0x80, 0x80));

  //drawBitmap(10, 97,body,trunk_w,trunk_l,0x07E0);
  //drawBitmap(10, 97+25,leg1,leg_w,leg_l,0x07E0);


}

void loop() {
  tft.fillRect(80, 2, 80, 15, tft.Color565( 0x80, 0x80, 0x80));
  tft.setCursor(80, 2);
  tft.setTextColor(ST7735_RED);
  tft.setTextSize(2);
  tft.print(millis() / 1000);
  update_game();
  display_game();


}
