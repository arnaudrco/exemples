/* 

arnaud RCO
start pour enregister un pixel ou effacer un pixel
déplacement avec les flèches de direction

 */
#include <Gamer.h>
#define pin1 2

Gamer gamer;

int gameScreen[] = {
  0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,
};

int xPos = 0;
int yPos = 0;
int vis = 0;

void bip(int d)// durée d
{       

       digitalWrite(pin1, HIGH);   delay(d);  
             digitalWrite(pin1, LOW);
 
}
void setup()
{          
  pinMode(pin1, OUTPUT);
  bip(4);   
  gamer.begin();
}

void loop() 
{
  gamer.clear();
  vis = vis==0 ? 1 : 0;
  gamer.display[yPos][xPos] = vis;
  printGameScreen();
  getInput();
  delay(200);
}

void printGameScreen() {
  int scrPos = 0;
  for(int col=0; col<8; col++) {
    for(int row=0; row<8; row++) {
      gamer.display[row][col] = (row==yPos and col==xPos) ? vis : gameScreen[scrPos];
      scrPos++;
    }
  }
  gamer.updateDisplay();
}

void getInput() {
  if(gamer.isPressed(LEFT) and yPos >0) { 
    yPos -= 1;
     bip(5); 
  }
  else if(gamer.isPressed(RIGHT) and yPos <7) { 
    yPos += 1;
     bip(6); 
  }
  else if(gamer.isPressed(UP) and xPos >0) { 
    xPos -= 1;
     bip(7); 
  }
  else if(gamer.isPressed(DOWN) and xPos <7) { 
    xPos += 1;
     bip(8); 
  }
  else if(gamer.isPressed(START)) {
    if(gameScreen[(8*xPos)+yPos] == 0) {
      gameScreen[(8*xPos)+yPos] = 1;      bip(9); 
    }
    else {
      gameScreen[(8*xPos)+yPos] = 0;      bip(10); 
    }
  }
}
