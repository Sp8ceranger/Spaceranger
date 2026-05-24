#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 64, &Wire);

#define UP_BUTTON 2
#define DOWN_BUTTON 3

int menu = 1;
bool ok = false;
int up = 0;
int down = 0;

const unsigned long PADDLE_RATE = 4;
const unsigned long BALL_RATE = 32;
const uint8_t PADDLE_HEIGHT = 12;
const uint8_t SCORE_LIMIT = 5;

bool game_over, win;

uint8_t player_score, mcu_score;
uint8_t ball_x = 53, ball_y = 26;
uint8_t ball_dir_x = 1, ball_dir_y = 1;

unsigned long ball_update;
unsigned long paddle_update;

const uint8_t MCU_X = 12;
uint8_t mcu_y = 16;

const uint8_t PLAYER_X = 115;
uint8_t player_y = 26;

unsigned long chrono = 0;
bool v1 = false;
int recordPong = 0;
unsigned long lastChronoUpdate = 0;

const unsigned char pong_image_noir [] PROGMEM = {
	// 'Sans titre, 23x18px
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x10, 0x00, 0x00, 0x10, 0x00, 0x00, 0x10, 
	0x00, 0x08, 0x10, 0x00, 0x08, 0x10, 0x10, 0x08, 0x10, 0x00, 0x08, 0x00, 0x00, 0x08, 0x00, 0x00, 
	0x08, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0xff, 0xff, 0xfe, 0x20, 0x00, 0x08, 0x50, 0x00, 0x14, 
	0x50, 0x00, 0x14, 0x20, 0x00, 0x08
};
const unsigned char snake_image [] PROGMEM = {
	// 'snake, 23x18px
	0x00, 0x00, 0x00, 0x18, 0x00, 0x20, 0x23, 0x13, 0x24, 0x22, 0x94, 0xa8, 0x12, 0x94, 0xb0, 0x0a, 
	0x57, 0xa8, 0x0a, 0x34, 0xa4, 0x32, 0x14, 0xa0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xf8, 0x00, 0x00, 0x08, 0x00, 0x00, 0x0f, 0x10, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
const unsigned char dodje_image [] PROGMEM = {
	// 'dodje, 23x18px
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x00, 0x00, 0xa0, 0x00, 0x00, 
	0xe0, 0x08, 0x02, 0xff, 0xff, 0xfe
};

bool jeu = false;

void setup() {
  // put your setup code here, to run once:
  pinMode(UP_BUTTON, INPUT_PULLUP);
  pinMode(DOWN_BUTTON, INPUT_PULLUP);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.drawRect(0, 0, 128, 64, WHITE);

  display.fillRect(16, 16, 25, 20, WHITE);
  display.drawBitmap(17, 17, pong_image_noir, 23, 18, SSD1306_WHITE);
  display.fillRect(46, 16, 25, 20, WHITE);
  display.drawBitmap(47, 17, snake_image, 23, 18, SSD1306_WHITE);
  display.fillRect(77, 17, 25, 20, WHITE);
  display.drawBitmap(77, 17, dodje_image, 23, 18, SSD1306_WHITE);

  display.setTextSize(2);
  display.setTextColor(WHITE, BLACK);


  // Sous la case 1
  display.setCursor(22, 40);
  display.print("1");

  // Sous la case 2
  display.setCursor(52, 40);
  display.print("2");

  // Sous la case 3
  display.setCursor(82, 40);
  display.print("3");

  display.display();
  unsigned long start = millis();
  while(millis() - start < 2000);
  ball_update = millis();
  paddle_update = ball_update;

  Serial.begin(9600);
  

}

void loop() {
  // put your main code here, to run repeatedly:
  up = digitalRead(UP_BUTTON);
  down = digitalRead(DOWN_BUTTON);
  if (jeu == false) {
    display.clearDisplay();
    afficherMenu();
    if (up == LOW) {
      menu++;
    }
    if (down == LOW) {
      if (ok == true) {ok = false;}
      if (ok == false) {ok = true;}
      delay(100);
    } 
    if (menu == 1) {
      display.fillRect(17, 17, 23, 18, BLACK); // gauche
      display.drawBitmap(17, 17, pong_image_noir, 23, 18, SSD1306_WHITE);
      display.fillRect(47, 17, 23, 18, WHITE); // milieu
      display.drawBitmap(47, 17, snake_image, 23, 18, SSD1306_BLACK);
      display.fillRect(77, 17, 23, 18, WHITE);
      display.drawBitmap(77, 17, dodje_image, 23, 18, SSD1306_BLACK);
    }
    if (menu == 2) {
      display.fillRect(47, 17, 23, 18, BLACK); // milieu
      display.drawBitmap(47, 17, snake_image, 23, 18, SSD1306_WHITE);
      display.fillRect(17, 17, 23, 18, WHITE); //gauche
      display.drawBitmap(17, 17, pong_image_noir, 23, 18, SSD1306_BLACK);
      display.fillRect(77, 17, 23, 18, WHITE);
      display.drawBitmap(77, 17, dodje_image, 23, 18, SSD1306_BLACK);
      
    }
    if (menu == 3) {
      display.fillRect(47, 17, 23, 18, WHITE); // milieu
      display.drawBitmap(47, 17, snake_image, 23, 18, SSD1306_BLACK);
      display.fillRect(17, 17, 23, 18, WHITE); // gauche
      display.drawBitmap(17, 17, pong_image_noir, 23, 18, SSD1306_BLACK);
      display.fillRect(77, 17, 23, 18, BLACK);
      display.drawBitmap(77, 17, dodje_image, 23, 18, SSD1306_WHITE);
      
    }
    if (menu == 4) {menu = 1;}

    if (menu == 1 && ok == true) {
      Serial.println("case gauche [pong] (1) ");
      ok = false;
      jeu = true;
      display.clearDisplay();
      display.setTextSize(1);
      display.drawRect(0, 0, 128, 54, WHITE);
      startPong();
    }
    else if (menu == 2 && ok == true) {
      Serial.println("case milieu (2)");
      ok = false;
      jeu2_snake();
    }
    else if (menu == 3 && ok == true) {
      Serial.println("case droite (3)");
      ok = false;
      jeu3_dodge();
    }
    display.display();
    delay(100);
  }
}

void afficherMenu () {
  display.fillRect(16, 16, 25, 20, WHITE);
  display.drawBitmap(17, 17, pong_image_noir, 23, 18, SSD1306_WHITE);
  display.fillRect(46, 16, 25, 20, WHITE);
  display.fillRect(76, 16, 25, 20, WHITE);

  display.setTextSize(2);
  display.setTextColor(WHITE, BLACK);


  // Sous la case 1
  display.setCursor(22, 40);
  display.print("1");

  // Sous la case 2
  display.setCursor(52, 40);
  display.print("2");

  // Sous la case 3
  display.setCursor(82, 40);
  display.print("3");

  display.setTextSize(1);
  display.setCursor(0, 55);
  display.print("2 + 3 = by Copilot");

  display.display();
}

void startPong () {
  if (millis() - lastChronoUpdate >= 1000) {
       chrono++;
       lastChronoUpdate = millis();
    }
    if (chrono > recordPong) {
        // recordPong = String(chrono / 60) + " min " + String(chrono % 60) + "s";
        recordPong = chrono;
    }
    // display.print()
    bool update_needed = false;
    unsigned long time = millis();

    static bool up_state = false;
    static bool down_state = false;
    
    up_state |= (digitalRead(UP_BUTTON) == LOW);
    down_state |= (digitalRead(DOWN_BUTTON) == LOW);

    if(time > ball_update) 
    {
        uint8_t new_x = ball_x + ball_dir_x;
        uint8_t new_y = ball_y + ball_dir_y;

        // Check if we hit the vertical walls
        if(new_x == 0 || new_x == 127) 
        {
            ball_dir_x = -ball_dir_x;
            new_x += ball_dir_x + ball_dir_x;

            if (new_x < 64)
            {
                player_scoreTone();
                player_score++;
            }
            else
            {
                mcu_scoreTone();
                mcu_score++;
            }

            if (player_score == SCORE_LIMIT || mcu_score == SCORE_LIMIT)
            {
                win = player_score > mcu_score;
                game_over = true;
            }
        }

        // Check if we hit the horizontal walls.
        if(new_y == 0 || new_y == 53) 
        {
            wallTone();
            ball_dir_y = -ball_dir_y;
            new_y += ball_dir_y + ball_dir_y;
        }

        // Check if we hit the CPU paddle
        if(new_x == MCU_X && new_y >= mcu_y && new_y <= mcu_y + PADDLE_HEIGHT) 
        {
            mcuPaddleTone();
            ball_dir_x = -ball_dir_x;
            new_x += ball_dir_x + ball_dir_x;
        }

        // Check if we hit the player paddle
        if(new_x == PLAYER_X && new_y >= player_y && new_y <= player_y + PADDLE_HEIGHT)
        {
            playerPaddleTone();
            ball_dir_x = -ball_dir_x;
            new_x += ball_dir_x + ball_dir_x;
        }

        display.drawPixel(ball_x, ball_y, BLACK);
        display.drawPixel(new_x, new_y, WHITE);
        ball_x = new_x;
        ball_y = new_y;

        ball_update += BALL_RATE;

        update_needed = true;
    }

    if(time > paddle_update) 
    {
        paddle_update += PADDLE_RATE;

        // CPU paddle
        display.drawFastVLine(MCU_X, mcu_y, PADDLE_HEIGHT, BLACK);
        const uint8_t half_paddle = PADDLE_HEIGHT >> 1;

        if(mcu_y + half_paddle > ball_y)
        {
            int8_t dir = ball_x > MCU_X ? -1 : 1;
            mcu_y += dir;
        }

        if(mcu_y + half_paddle < ball_y)
        {
            int8_t dir = ball_x > MCU_X ? 1 : -1;
            mcu_y += dir;
        }

        if(mcu_y < 1) 
        {
            mcu_y = 1;
        }

        if(mcu_y + PADDLE_HEIGHT > 53)
        {
            mcu_y = 53 - PADDLE_HEIGHT;
        }

        // Player paddle
        display.drawFastVLine(MCU_X, mcu_y, PADDLE_HEIGHT, WHITE);
        display.drawFastVLine(PLAYER_X, player_y, PADDLE_HEIGHT, BLACK);

        if(up_state) 
        {
            player_y -= 1;
        }

        if(down_state) 
        {
            player_y += 1;
        }

        up_state = down_state = false;

        if(player_y < 1) 
        {
            player_y = 1;
        }

        if(player_y + PADDLE_HEIGHT > 53) 
        {
            player_y = 53 - PADDLE_HEIGHT;
        }

        display.drawFastVLine(PLAYER_X, player_y, PADDLE_HEIGHT, WHITE);

        update_needed = true;
    }

    if(update_needed)
    {
        if (game_over)
        {
            const char* text = win ? "YOU WIN!!" : "YOU LOSE!";
            display.clearDisplay();
            display.setCursor(40, 18);
            display.print(text);
            display.setCursor(40, 33);
            // display.print("SCORE : " + String(chrono / 60) + " min " + String(chrono % 60) + "s");
            display.print("SCORE : " + String(chrono));
            display.setCursor(40, 48);
            display.print("RECORD : " + String(recordPong));
            display.display();

            delay(5000);

            display.clearDisplay();
            ball_x = 53;
            ball_y = 26;
            ball_dir_x = 1;
            ball_dir_y = 1;
            mcu_y = 16;
            player_y = 26;
            mcu_score = 0;
            player_score = 0;
            game_over = false;
            chrono = 0;
            jeu = false;
        }

        display.setTextColor(WHITE, BLACK);
        display.setCursor(0, 56);
        display.print(mcu_score);
        display.setCursor(122, 56);
        display.print(player_score);
        display.setCursor(41, 56);
        display.print(String(chrono / 60) + " min " + String(chrono % 60) + "s");
        
        display.display();
    }
}

void playerPaddleTone()
{
    tone(11, 250, 25);
    delay(25);
    noTone(11);
}

void mcuPaddleTone()
{
    tone(11, 225, 25);
    delay(25);
    noTone(11);
}

void wallTone()
{
    tone(11, 200, 25);
    delay(25);
    noTone(11);
}

void player_scoreTone()
{
    tone(11, 200, 25);
    delay(50);
    noTone(11);
    delay(25);
    tone(11, 250, 25);
    delay(25);
    noTone(11);
}

void mcu_scoreTone()
{
    tone(11, 250, 25);
    delay(25);
    noTone(11);
    delay(25);
    tone(11, 200, 25);
    delay(25);
    noTone(11);
}

void drawCourt() 
{
    display.drawRect(0, 0, 128, 54, WHITE);
}

void jeu3_dodge() {
  int playerY = 32;
  int obsX = 128;
  int obsY = random(0, 56);

  while (true) {
    display.clearDisplay();

    // Contrôles
    if (digitalRead(UP_BUTTON) == LOW) playerY -= 2;
    if (digitalRead(DOWN_BUTTON) == LOW) playerY += 2;

    // Limites
    if (playerY < 0) playerY = 0;
    if (playerY > 56) playerY = 56;

    // Déplacement obstacle
    obsX -= 3;
    if (obsX < -10) {
      obsX = 128;
      obsY = random(0, 56);
    }

    // Collision
    if (obsX < 15 && obsX > 0 && abs(playerY - obsY) < 10) {
      display.setCursor(20, 30);
      display.print("GAME OVER");
      display.display();
      delay(1500);
      return;
    }

    // Dessin
    display.fillRect(5, playerY, 8, 8, WHITE); // joueur
    display.fillRect(obsX, obsY, 10, 10, WHITE); // obstacle

    display.display();
    delay(40);
  }
}

void jeu2_snake() {
  int snakeY = 32;
  int fruitX = 100;
  int fruitY = random(5, 59);

  while (true) {
    display.clearDisplay();

    // Lecture boutons
    if (digitalRead(UP_BUTTON) == LOW) snakeY -= 2;
    if (digitalRead(DOWN_BUTTON) == LOW) snakeY += 2;

    // Limites
    if (snakeY < 0 || snakeY > 63) {
      display.setCursor(20, 30);
      display.print("GAME OVER");
      display.display();
      delay(1500);
      return; // Retour au menu
    }

    // Déplacement du fruit
    fruitX -= 2;
    if (fruitX < 0) {
      fruitX = 120;
      fruitY = random(5, 59);
    }

    // Collision
    if (abs(snakeY - fruitY) < 4 && fruitX < 10) {
      display.setCursor(20, 30);
      display.print("BONUS !");
      display.display();
      delay(500);
      fruitX = 120;
      fruitY = random(5, 59);
    }

    // Dessin
    display.fillRect(5, snakeY, 8, 4, WHITE); // serpent
    display.fillRect(fruitX, fruitY, 4, 4, WHITE); // fruit

    display.display();
    delay(40);
  }
}
