#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>

#include "lcd.h"
#include "usart.h"
#include "DS3231_rtc.h"


// macros
#define SET_PIN_HIGH(port, pin) port |= (1 << pin)
#define SET_PIN_LOW(port, pin) port &= ~(1 << pin)
#define READ_PIN(port_input_addr, pin) (1<<pin) & port_input_addr
#define RIGHT_BUTTON PORTD2
#define LEFT_BUTTON PORTD3
#define BUTTON_PORT PIND
#define BUTTON_DELAY 75
#define ERROR 100
#define PARAMETERS 6

// functions
void init(void); // initialise all peripherals for watch
void main_menu(void); // main menu
void binary_time(void); // displays current time in binary
void settings(void); // calibrate time settings
char* format_weekday(uint8_t input); // return day of week as string 
void print_state(uint16_t input); // debug output for printing numbers to terminal
uint8_t check_button_states(); // check button states
void page_select_control(); // select which page you are on
void decimal_to_binary(uint8_t input, char binary_buffer[]);

// global variables
char buffer[20]; // buffer for formatting characters into SSD1306 RAM
rtc_time time; // store time and date
int8_t page_select = 0; // counter to select which page to select 
uint8_t parameter_select = 0; // variable to hold which time parameter is selcted
uint8_t parameter_increment = 0; // increment selected paremeter

int main(){
    init();
    //USARTSendString("Watch debug terminal");
    
    while(1){
      page_select_control();
      
      // check which page has been selected
      if(page_select == 0){
        main_menu();
      }
      if(page_select == 1){
        binary_time();
      }
      if(page_select == 2){
        settings();
      }
      
      // write to display and clear screen buffer
      lcd_display();
      _delay_ms(10);
      lcd_clear_buffer();
      
    }
    //*/


}

void main_menu(void){
  get_rtc_time(&time);
  lcd_drawRect(0, 0, 127, 63, WHITE); // set up border
  lcd_drawRect(3, 3, 124, 60, WHITE); // set up border 
  uint8_t y_pos = 1;
  uint8_t x_pos = 4;

  // print time
  lcd_gotoxy(x_pos, y_pos);
  sprintf(buffer, "Time: %d-%d-%d", time.hours, time.minutes, time.seconds);
  lcd_puts(buffer);

  // print date
  lcd_gotoxy(x_pos, y_pos+2);
  sprintf(buffer, "Date: %d-%d-%d", time.year, time.month, time.date);
  lcd_puts(buffer);

  // print day of week
  lcd_gotoxy(x_pos, y_pos+4);
  sprintf(buffer, "Day: %s", format_weekday( time.weekday ) );
  lcd_puts(buffer);
}

void binary_time(void){
  get_rtc_time(&time);
  lcd_drawRect(0, 0, 127, 63, WHITE); // set up border
  lcd_drawRect(3, 3, 124, 60, WHITE); // set up border
  uint8_t y_pos = 1;
  uint8_t x_pos = 4;
  
  
  char binary_hour_string[] = "00000000";
  lcd_gotoxy(x_pos, y_pos);
  decimal_to_binary( time.hours, binary_hour_string );
  sprintf(buffer, "Hour: %s", binary_hour_string);
  lcd_puts(buffer);

  char binary_min_string[] = "00000000";
  lcd_gotoxy(x_pos, y_pos+2);
  decimal_to_binary( time.minutes, binary_min_string );
  sprintf(buffer, "Min:  %s", binary_min_string);
  lcd_puts(buffer);

  char binary_sec_string[] = "00000000";
  lcd_gotoxy(x_pos, y_pos+4);
  decimal_to_binary( time.seconds, binary_sec_string );
  sprintf(buffer, "Sec:  %s", binary_sec_string);
  lcd_puts(buffer);
  
}

void decimal_to_binary(uint8_t input, char binary_buffer[]){
    // counter for binary array
    int i = 0;
    while (input > 0) {
 
        // storing remainder in binary array
        if(input % 2){
          binary_buffer[i] = '1';
        }
        else{
          binary_buffer[i] = '0';
        }
       
        input = input / 2;
        i++;
    }
 
    // reverse binary string to be MSB
    binary_buffer = strrev( binary_buffer );
}

void settings(void){
  lcd_drawRect(0, 0, 127, 63, WHITE); // set up border
  lcd_drawRect(3, 3, 124, 60, WHITE); // set up border
  uint8_t y_pos = 1;
  uint8_t x_pos = 6;

  lcd_gotoxy(x_pos, y_pos);
  lcd_puts("Settings");
  lcd_gotoxy(x_pos+1, y_pos+2);

  switch ( parameter_select )
  {
  case 0:
    lcd_gotoxy(x_pos, y_pos+2);
    sprintf(buffer, "Hour: < %d >", time.hours);
    lcd_puts(buffer);
    if(parameter_increment){
      time.hours++;
      if(time.hours > 23){
        time.hours = 1;
      }
    }
    break;
    
  case 1:
    lcd_gotoxy(x_pos-2, y_pos+2);
    sprintf(buffer, "Minute: < %d >", time.minutes);
    lcd_puts(buffer);
    if(parameter_increment){
      time.minutes++;
      if(time.minutes > 59){
        time.minutes = 1;
      }
    }
    break;

  case 2:
    lcd_gotoxy(x_pos-2, y_pos+2);
    sprintf(buffer, "Second: < %d >", time.seconds);
    lcd_puts(buffer);
    if(parameter_increment){
      time.seconds++;
      if(time.seconds > 59){
        time.seconds = 1;
      }
    }
    break;

  case 3:
    lcd_gotoxy(x_pos, y_pos+2);
    sprintf(buffer, "Date: < %d >", time.date);
    lcd_puts(buffer);
    if(parameter_increment){
      time.date++;
      if(time.month == 1 || time.month == 3 || time.month == 5 || time.month == 7 || time.month == 8 || time.month == 10 || time.month == 12){
        if(time.date > 31){
          time.date = 1;
        }
      }
      if(time.month == 4 || time.month == 6 || time.month == 9 || time.month == 11){
        if(time.date > 30){
          time.date = 1;
        }
      }
      else{
        if(time.date > 29){
          time.date = 1;
        }
      }
    }
    break;

  case 4:
    lcd_gotoxy(x_pos-1, y_pos+2);
    sprintf(buffer, "Month: < %d >", time.month);
    lcd_puts(buffer);
    if(parameter_increment){
      time.month++;
      if(time.month > 12){
        time.month = 1;
      }
    }
    break;

  case 5:
    lcd_gotoxy(x_pos-1, y_pos+2);
    sprintf(buffer, "Year: < %d >", time.year);
    lcd_puts(buffer);
    if(parameter_increment){
      time.year++;
      if(time.year > 2030){
        time.year = 2020;
      }
    }
    break;

  case 6:
    lcd_gotoxy(x_pos-4, y_pos+2);
    sprintf(buffer, "Day: < %s >", format_weekday(time.weekday) );
    lcd_puts(buffer);
    if(parameter_increment){
      time.weekday++;
      if(time.weekday > 7){
        time.weekday = 1;
      }
    }
    break;
  
  default:
    break;
  }

  set_rtc_date(&time);
  set_rtc_time(&time);
  set_rtc_weekday(&time);

}

void init(void){
  // enable pins connected to buttons as inputs
  SET_PIN_LOW(DDRD, RIGHT_BUTTON);
  SET_PIN_LOW(DDRD, LEFT_BUTTON);

  // enable pull up for input pins
  SET_PIN_HIGH(PORTD, RIGHT_BUTTON);
  SET_PIN_HIGH(PORTD, LEFT_BUTTON);


  //USARTInit(9600);
  rtc_init();
  lcd_init(LCD_DISP_ON);
  lcd_clrscr();
  lcd_set_contrast(0x00);
}

char* format_weekday(uint8_t input){
  switch (input)
  {
  case 1:
    return "Monday";
    break;
  
  case 2:
    return "Tuesday";
    break;

  case 3:
    return "Wednesday";
    break;
  
  case 4:
    return "Thursday";
    break;

  case 5:
    return "Friday";
    break;

  case 6:
    return "Saturday";
    break;

  case 7:
    return "Sunday";
    break;

  default:
    return "ERROR";
    break;

  }


}

uint8_t check_button_states(){
  uint8_t right_state = bit_is_set(BUTTON_PORT, RIGHT_BUTTON);
  uint8_t left_state = bit_is_set(BUTTON_PORT, LEFT_BUTTON);
  

  if( right_state == 0 && left_state == 0 ){
    //USARTSendString("Both buttons pressed");
    _delay_ms(BUTTON_DELAY);
    return 0;
  }

  else{
    if( right_state == 0 ){
    //USARTSendString("Right button pressed");
    _delay_ms(BUTTON_DELAY);
    return 1;
    }

    if( left_state == 0){
    //USARTSendString("Left button pressed");
    _delay_ms(BUTTON_DELAY);
    return 2;
    }

    else{
      return ERROR;
    }
    
  }

}

void page_select_control(){
  uint8_t state = check_button_states();
  
  if(page_select == 2){
    switch (state)
    {
    case 0:
      page_select++;
      break;

    case 1:
      parameter_select++;
      break;

    case 2:
      parameter_increment = 1;
      break;
    
    default:
      parameter_increment = 0;
      break;
    }

    if(parameter_select > PARAMETERS){
      parameter_select = 0;
    }

  }
  else {
    parameter_select = 0;
    switch (state)
    {
    case 1:
      page_select++;
      break;
    
    case 2:
      page_select--;
      break;

    default:
      break;
    }

    if(page_select < 0){
      page_select = 2;
    }

    if(page_select > 2){
      page_select = 0;
    }
  }
}

// for debugging 
void print_state(uint16_t input){
    sprintf(buffer, "%d", input);
    USARTSendString(buffer);
}
