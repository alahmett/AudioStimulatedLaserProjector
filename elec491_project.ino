//Audio Stimulated Laser Projector
//Ahmet Al-Barış Hasan Aras
//ELEC491
//Senior Design Project
void sendto (float x_target, float y_target); // sends laser point to desired position on coordinate system
void do_pwm(); // pwm signals with varying duty cycle, scaling is handled in this function
void hold(long length);
void laser_on(); // turn-on laser, need to draw precisely
void laser_off(); //turn-off laser, need to draw precisely

String state; 

float x_level; // position of speaker X
float y_level; // position of speaker Y
float move_scale = 2.4; //scale the movement

int laser_pin = 12; // laser pin on arduino
int x_speaker_pin = 5; // speaker X pin on arduino
int y_speaker_pin = 6; // spekaer Y pin on arduino

float exp_move = 5;    // laser movement is controlled by exp_move
                       //if it increases then the drawing will be slower
float min_move = 0.5;  // laser point distance to desired target
    
float laser_toggle_delay = 11;   //delay before toggling laser on/off
float line_delay = 8;            //expected line delays during process    

// captured linear range is between 20 and 190 (duty cycles)
//min values for nonlinear response
float x_min_value = 20; 
float y_min_value = 20 ; 
//max value for nonlinear response
float x_max_value = 190; 
float y_max_value = 190; 
                                                

void setup()   {               
  Serial.begin(9600);
  pinMode(laser_pin, OUTPUT);   
  pinMode(x_speaker_pin, OUTPUT);  
  pinMode(y_speaker_pin, OUTPUT);

  //default Arduino PWM frequency
  setPwmFrequency(x_speaker_pin, 1);
  setPwmFrequency(y_speaker_pin, 1);
  
}

 
void loop()
{


  if(Serial.available() > 0){ // Checks whether data is comming from the serial port
     state = Serial.readString(); // Reads the data from the serial port
 }

if(state.length() <= 5 && state != "STAR"){  //if user do not write STAR and write a string, then draw
  draw_string(state, 1, 1, 1);
}
/*else if(state.length() > 5){
  marquee(state,5);
}*/
else if(state.length() <= 5 && state == "STAR"){//if the user write STAR, then draw a Star
star();
}
 
}


void star()
{
  laser_on();
       move (13,24);
       move (-30,-18);
       move (34,0);
       move (-30,18);
       move (13,-24);
}


void move (float x_movedir, float y_movedir) //moves the laser to target, relative to current position
//vectoral addition logic

{
    
  x_movedir = x_movedir * move_scale; // scales to better image
  y_movedir = y_movedir * move_scale; // scales to better image
  
  sendto (x_level + x_movedir, y_level + y_movedir); // sends laser to X and Y targets
  
}

void sendto (float x_target, float y_target) 
{
       // main function to adjust positions
       //x_level is depend on duty cycle 
       //y_level is depend on duty cycle
       //depend on duty cycle, mirror behaves differently
       //target is depend on command, but movement is depend on previous position
       //provides movement between positions
    
    float x_move;
    float y_move;
           
    float temp_exp_move = exp_move; // controls the movement

   while (fabs(x_target - x_level) > min_move || fabs(y_target - y_level) > min_move)
   {
  
       x_move = (x_target - x_level) / temp_exp_move;  //controls the movement
                                           //movement between two point
                                           //current level=x_level, target=x_target
                                          
       y_move = (y_target - y_level) / temp_exp_move; //controls the movement
       
        x_level = x_level + x_move; // X position for desired target
        y_level = y_level + y_move;  // Y position for desired target

        do_pwm(); // sendto provides the position,PWM Signals do the rests

    }

  x_level = x_target; // current X  position after movement
  y_level = y_target;  //current Y position after movement
  do_pwm();
  hold(line_delay);

}

void laser_on() //turn on Laser
{
  hold(laser_toggle_delay);
  digitalWrite(laser_pin, HIGH); 
}

void laser_off() //turn off laser 
{
  hold(laser_toggle_delay);
  digitalWrite(laser_pin, LOW);
}

void do_pwm()
{
    //actual PWM code
   
    float x_adjust;
    float y_adjust;
   
    // handles scaling, maintains 0-255 ranges,
    x_adjust = x_min_value + (x_level * ((x_max_value - x_min_value) / 255)); 
    y_adjust = y_min_value + ((y_level) * ((y_max_value - y_min_value) / 255));
   
    if (x_adjust < 1) x_adjust = 1;
    if (y_adjust < 1) y_adjust = 1;
    if (x_adjust > x_max_value) x_adjust = x_max_value;
    if (y_adjust > y_max_value) y_adjust = y_max_value;
   
    analogWrite(y_speaker_pin, y_adjust); //send signal to Y speaker
    analogWrite(x_speaker_pin, x_adjust); //send signal to X speaker

}


void hold(long length)
{
    long loop2 = 0;
        length = length * 2;  
       
        while (loop2 < length)
        {
            delay (1); 
            loop2 ++;
        }
}

void draw_string(String text, int x, int y, int count) //in order to draw string
{

  for (int loop = 0; loop < count; loop ++)
  {
    sendto (x,y);
    int i = 0;
    while (text.charAt(i) != '\0')
    {
       draw_letter(text.charAt(i)); 
       i ++;
    }
  }
  
}

void draw_letter(byte letter)  
//draw all defined letters
//letters are defined according to vectoral addition logic
//laser is turn on or turn off according to desired drawing
//It is not possible to draw 'A' without turn off the laser
{
  
  float temp_x_level = x_level;
  float temp_y_level = y_level;
  

  
  laser_off();
  
  switch (letter) //letters are defined 
  {

case 'A':        
        move (0,16); //laser is at top of 'A'
        laser_on();
        move (7,-16); // draw the left side
        move (7,16);  // draw right side
        laser_off();  // turn off the laser to draw middle line
        move (-10,-7); // laser is at edge of that line
        laser_on(); // turn on laser to draw
        move (9,0); 
        break;

      //same logic to define all letters
      // since the image is in reversed direction,calculatios are according to that
      // When we make the defined vectoral addition, we will obtain the reverse A 
case 'B': 
        laser_on();
        move (0,16);
        move (11,-3);
        move (-7,-5);
        move (7,-5);
        move (-11,-3);
        break;
case 'C' : 
        move (11,16);
        laser_on();
        move (-11,-2);
        move (0,-12);
        move (11,-2);
        break;
case 'D': 
        laser_on();
        move (0,16);
        move (9,-4);
        move (0,-8);
        move (-9,-4);
        break;
case 'E' : 

        move (10,0);
        laser_on();  
        move (-10,0);
        move (0,14);
        move (10,2);
        laser_off();
        move (-6,-10);
        laser_on();
        move (-6,0);
        break;
case 'F':         
        move (10,0);
        laser_on();  
        move (-10,0);
        move (0,16);
        laser_off();
        move (0,-8);
        laser_on();
        move (5,0);
        break;
case 'G': 

        move (9,8);
        laser_on();
        move (4,0);
        move (-1,8);
        move (-12,-2);
        move (0,-12);
        move (9,-2);
       break;
case 'H': 
        move (2,0);
        laser_on();
        move (0,16);
        laser_off();
        move (0,-8);
        laser_on();
        move (9,0);
        laser_off();
        move (0,8);
        laser_on();
        move (0,-16);
        break;
case 'I':    
        move (5,0);
        laser_on();
        move (0,16);
        break;
case 'J':    
        move (0,8);
        laser_on();
        move (2,8);
        move (9,0);
        move (0,-16);
        break;
case 'K':    
        laser_on();
        move (0,16);
        laser_off();
        move (11,0);
        laser_on();
        move (-9,-8);
        move (9,-8);
        break;
case 'L':    
        laser_on();
        move (0,16);
        move (11,0);
        break;
case 'M':    
        move (0,16);
        laser_on();
        move (0,-16);
        move (7,8);
        move (7,-8);
        move (0,16);
        break;
case 'N':    
        move (0,16);
        laser_on();
        move (0,-16);
        move (11,16);
        move (0,-16);
        break;
case 'O':      
        laser_on();
        move (11,0);
        move (0,16);
        move (-11,0);
        move (0,-16);
        break;
case 'P':      
        move (0,16);
        laser_on();
        move (0,-16);
        move (11,4);
        move (-11,4);
        break;
case 'Q':      

        move (6,0);
        laser_on();
        move (-6,8);
        move (6,8);
        move (6,-8);
        move (-6,-8);
        laser_off();
        move (0,7);
        laser_on();
        move (7,7);  
        break;
case 'R':      
        move (0,16);
        laser_on();
        move (0,-16);
        move (11,3);
        move (-11,5);
        move (11,10);
        break;
case 'S':      
        move (11,0);
        laser_on();
        move (-11,4);
        move (11,6);
        move (-11,6);
        break;
case 'T':      
       laser_on();
       move (13,0);
       laser_off();
       move (-8,0);
       laser_on();
       move (0,16);
       break;
case 'U':      
        laser_on();
        move (2,16);
        move (9,0);
        move (2,-16);
        break;
case 'V':      
        laser_on();
        move (5,16);
        move (5,-16);
        break;
case 'W':      
        laser_on();
        move (3,16);
        move (4,-8);
        move (4,8);
        move (3,-16);
        break;
case 'X':      
        laser_on();
        move (12,16);
        laser_off();
        move (-12,0);
        laser_on();
        move (12,-16);
        break;
case 'Y':      
        laser_on();
        move (6,7);
        move (0,9);
        laser_off();
        move (0,-9);
        laser_on();
        move (6,-7);
        break;
case 'Z':      
        laser_on();
        move (12,0);
        move (-12,16);
        move (12,0);
        break;
case ' ':    
        break;

case '0':      
        laser_on();
        move (11,0);
        move (0,16);
        move (-11,0);
        move (0,-16);
        break;
case '1':    
        move (5,0);
        laser_on();
        move (0,16);
        break;
 case '2':    
        laser_on();
        move (12,2);
        move (-12,14);
        move (12,0);
        break;
case '3':    
        laser_on();
        move (12,3);
        move (-7,6);
        move (7,6);
        move (-12,3);
        break;
case '4':    
        laser_on();
        move (0,7);
        move (12,0);
        laser_off();
        move (0,-7);
        laser_on();
        move (0,16);
        break;
case '5':    
        move (0,16);
        laser_on();
        move (12,-5);
        move (0,-5);
        move (-12,0);
        move (0,-6);
        move (12,0);
        break;
 case '6': 
        move (0,6);
        laser_on();
        move (12,3);
        move (0,6);
        move (-12,1);
        move (0,-16);
        move (10,0);
       break;
case '7': 
        laser_on();
        move (12,0);
        move (-10,16);
        break;
case '8': 
        move (0,2);
        laser_on();
        move (12,12);
        move (-6, 2);
        move (-6,-2);
        move (12,-12);
        move (-6, -2);
        move (-6, 2);
       break;
case '9': 
        move (12,6);
        laser_on();
        move (-12,-3);
        move (0,-3);
        move (12,4);
        move (0,12);
       break;


  }

  laser_off();
  sendto (temp_x_level + (18 * move_scale) , temp_y_level); //in order to draw on X axis
  // by changing Y level, and holding X level constant, we can draw on Y axis.
  
}


//PWM adjustment for Arduino
//reference is Arduino home page


/**
 * Divides a given PWM pin frequency by a divisor.
 * 
 * The resulting frequency is equal to the base frequency divided by
 * the given divisor:
 *   - Base frequencies:
 *      o The base frequency for pins 3, 9, 10, and 11 is 31250 Hz.
 *      o The base frequency for pins 5 and 6 is 62500 Hz.
 *   - Divisors:
 *      o The divisors available on pins 5, 6, 9 and 10 are: 1, 8, 64,
 *        256, and 1024.
 *      o The divisors available on pins 3 and 11 are: 1, 8, 32, 64,
 *        128, 256, and 1024.
 * 
 * PWM frequencies are tied together in pairs of pins. If one in a
 * pair is changed, the other is also changed to match:
 *   - Pins 5 and 6 are paired.
 *   - Pins 9 and 10 are paired.
 *   - Pins 3 and 11 are paired.
 * 
 * Note that this function will have side effects on anything else
 * that uses timers:
 *   - Changes on pins 3, 5, 6, or 11 may cause the delay() and
 *     millis() functions to stop working. Other timing-related
 *     functions may also be affected.
 *   - Changes on pins 9 or 10 will cause the Servo library to function
 *     incorrectly.
 * 
 * Thanks to macegr of the Arduino forums for his documentation of the
 * PWM frequency divisors. His post can be viewed at:
 *   http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1235060559/0#4
 */
void setPwmFrequency(int pin, int divisor) {
  byte mode;
  if(pin == 5 || pin == 6 || pin == 9 || pin == 10) {
    switch(divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 64: mode = 0x03; break;
      case 256: mode = 0x04; break;
      case 1024: mode = 0x05; break;
      default: return;
    }
    if(pin == 5 || pin == 6) {
      TCCR0B = TCCR0B & 0b11111000 | mode;
    } else {
      TCCR1B = TCCR1B & 0b11111000 | mode;
    }
  } else if(pin == 3 || pin == 11) {
    switch(divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 32: mode = 0x03; break;
      case 64: mode = 0x04; break;
      case 128: mode = 0x05; break;
      case 256: mode = 0x06; break;
      case 1024: mode = 0x7; break;
      default: return;
    }
    TCCR2B = TCCR2B & 0b11111000 | mode;
  }
}

