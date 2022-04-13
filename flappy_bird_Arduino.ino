#include <MCUFRIEND_kbv.h>
#include <TouchScreen.h>

//color
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0
#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin
MCUFRIEND_kbv myGLCD;

//Touch screen presure threshold
#define MINPRESSURE 20
#define MAXPRESSURE 1000

const int16_t XP = 8, XM = A2, YP = A3, YM = 9; //320x480

const TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

#define GROUND 30
#define BIRD_POSITION   myGLCD.width()/3
int movingRate = 3, score = 0, highest_score = 0, xp = myGLCD.width(), yp = myGLCD.height()/2 - 50;
bool gameStarted = false, endGame = false, screenPressed = false;
void init_Game(){
  //blue background
  myGLCD.fillScreen(CYAN);
  //ground
  myGLCD.fillRect(0,myGLCD.height()- GROUND,myGLCD.width(),50,GREEN);
  //bird
  drawBird(BIRD_POSITION,false);
  //Text
  myGLCD.setCursor(120,myGLCD.height()/2 - 100);
  myGLCD.setTextColor(RED);
  myGLCD.setTextSize(3);
  myGLCD.println("Tap to start");
  myGLCD.setCursor(100,myGLCD.height()/2 - 50);
  myGLCD.print("Highest score:");
  myGLCD.print(highest_score);
  //wait first touch to start game
  while(!gameStarted){
    //wait first touch
    TSPoint tp = ts.getPoint();
    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);
    if(tp.z >MINPRESSURE && tp.z < MAXPRESSURE){
      gameStarted = true;  
    }
  }
  //delete text
  myGLCD.fillRect(100,myGLCD.height()/2 - 100, 300, 100,CYAN)
  //ground
  myGLCD.fillRect(0,myGLCD.height()- GROUND,myGLCD.width(),50,GREEN);
  myGLCD.setCursor(0,myGLCD.height()-GROUND + 5);
  myGLCD.setTextColor(WHITE);
  myGLCD.setTextSize(3);
  myGLCD.println("Score:0");
  //reset pillar position
  xp = myGLCD.width(); 
}

#define PILLAR_WIDTH    50
#define DISTANCE 100
void drawPillar(int x, int y){

    //delete pillar from previous frame
    //top pillar
    myGLCD.fillRect(x+PILLAR_WIDTH,0,movingRate+1,1 + y,CYAN);
    //bot pillar
    myGLCD.fillRect(x+PILLAR_WIDTH,y + DISTANCE - 1,movingRate+1,myGLCD.height() - y - DISTANCE - GROUND + 2,CYAN);

    //drawm pillar from current frame  
    //top pillar
    myGLCD.drawRect(x-1,0,PILLAR_WIDTH + 2,1+y,BLACK);
    myGLCD.fillRect(x,0,PILLAR_WIDTH,y,GREEN);
    //bot pillar
    myGLCD.drawRect(x-1,y + DISTANCE - 1,PILLAR_WIDTH + 2,myGLCD.height() - y - DISTANCE - GROUND + 2,BLACK);
    myGLCD.fillRect(x,y + DISTANCE,PILLAR_WIDTH,myGLCD.height() - y - DISTANCE - GROUND,GREEN);

}

int height_fly = myGLCD.height()/2;
void drawBird(int y,bool up){
  //delete bird from previous frame
  if(up){
    myGLCD.fillRect(myGLCD.width()/3-25,y + 10,55,22,CYAN);
  }
  else{
    myGLCD.fillRect(myGLCD.width()/3-25,y - 22,55,22,CYAN);  
  }
  //draw bird from current frame
  //body
  myGLCD.fillCircle(myGLCD.width()/3, y,18,YELLOW);
  myGLCD.drawCircle(myGLCD.width()/3, y,18,BLACK);
  //eye
  myGLCD.fillCircle(myGLCD.width()/3 + 9, y - 9,8,WHITE); 
  myGLCD.drawCircle(myGLCD.width()/3 + 9, y - 9,9,BLACK);
  myGLCD.fillCircle(myGLCD.width()/3 + 9, y - 9,1,BLACK); 
  //wing
  myGLCD.fillRoundRect(myGLCD.width()/3-25,y,20,7,3,WHITE);
  myGLCD.drawRoundRect(myGLCD.width()/3-25,y,20,8,3,BLACK);
  //neb
  myGLCD.fillRoundRect(myGLCD.width()/3+10,y + 6,15,4,2,RED);
  myGLCD.drawRoundRect(myGLCD.width()/3+10,y + 5,15,6,2,BLACK);
  myGLCD.fillRoundRect(myGLCD.width()/3+10,y+1,15,4,2,RED);
  myGLCD.drawRoundRect(myGLCD.width()/3+10,y,15,6,2,BLACK);
}

void GameOver(){
  bool continue_game=false;
  Serial.print("height: ");
  Serial.println(height_fly);
  myGLCD.setCursor(myGLCD.width()/2-8*10,myGLCD.height()/2);
  myGLCD.setTextColor(RED);
  myGLCD.setTextSize(3);//size 3: 16pixel/char wide
  myGLCD.println("Game Over");
  
  delay(1000);
  while(!continue_game){
    //wait first touch
    TSPoint tp = ts.getPoint();
    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);
    if(tp.z >MINPRESSURE && tp.z < MAXPRESSURE){
      continue_game = true;  
    }
  }
  init_Game();
  height_fly = myGLCD.height()/2;
  if(score > highest_score)highest_score = score;
  score = 0; 
  gameStarted = false;
}

void setup() {
  Serial.begin(115200);
  myGLCD.reset();

  //read ID of TFT
  uint16_t identifier = myGLCD.readID();
  Serial.print("Found LCD driver: ");
  Serial.println(identifier,HEX);
  myGLCD.begin(identifier);
  
  //turn wide screen
  myGLCD.setRotation(1);//1 or 3 is wide, 0 or 2 is tall
  
  //TFT properties
  Serial.print("tft width: ");
  Serial.println(myGLCD.width());
  Serial.print("tft height: ");
  Serial.println(myGLCD.height());
  
  init_Game();
}

void loop() {
  // determine position pillar
  if(xp <= -PILLAR_WIDTH)xp = myGLCD.width();
  else xp -= movingRate;
  drawPillar(xp,yp);

  //decrease height of bird 
  height_fly+=3;
  bool up = false;
  //touch to increase height of bird
  TSPoint tp = ts.getPoint();
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  if(tp.z >MINPRESSURE && tp.z < MAXPRESSURE){
    height_fly -= 15; 
    up = true;
  }
  drawBird(height_fly,up);
  if(height_fly <= 18 || height_fly >= myGLCD.height()-GROUND - 18){ //bird fall to ground or fly too high
    GameOver();
  }
  if(xp <= BIRD_POSITION + 18 && (xp >= BIRD_POSITION - PILLAR_WIDTH) && height_fly < yp + 18)GameOver(); //bird hit the top pillar 
  if(xp <= BIRD_POSITION + 18 && (xp >= BIRD_POSITION - PILLAR_WIDTH) && (height_fly > yp + DISTANCE - 18))GameOver(); //bird hit the pillar 
  
  //score
  if(xp + PILLAR_WIDTH <= 0){
    score++;
    myGLCD.fillRect(100,myGLCD.height()-GROUND + 5,60,GROUND,GREEN);
    myGLCD.setCursor(110,myGLCD.height()-GROUND + 5);
    myGLCD.setTextColor(WHITE);
    myGLCD.setTextSize(3);
    myGLCD.print(score);
    
    yp = 30 + rand() %(myGLCD.height()-DISTANCE-GROUND - 60);
  }
}
