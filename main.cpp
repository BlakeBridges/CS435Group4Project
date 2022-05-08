
#include "mbed.h"
#include <cstdio>
#include <cmath>
#include <string>
#include "NHD_0216HZ.h"




//RGB LED stuff

#define RED_LED D2
#define GREEN_LED D4
#define BLUE_LED D3

BusOut out_bus(RED_LED, GREEN_LED, BLUE_LED);



 
//Thermometer stuff
I2C i2c(D14, D15);
const int temp_addr = 0x90;
char tempSensorCommands[2] = {0x51, 0xAA};
char MSByte;
char LSByte;
char values[2] = {MSByte, LSByte};


// I dont think we need any of the serial stuff but I'm keeping it here

// Serial serial(D1, D0);

// //Home Cursor
// void homecursor()
// {
//     char homecursor[] = {0x1B, '[', 'H', 0};
//     serial.printf(homecursor);
// }

// void clearscreen()
// {
//     char clearscreen[] = {0x1B, '[', '2', 'J', 0};
//     serial.printf(clearscreen);
// }

//Prototypes
void display_temps(float celcius, int fahrenheit);
void ControlLED_BusIO (int x);

//colors for the RGB LED
enum colors{
    white = 0b111,
    blue = 0b100,
    purple = 0b101,
    red = 0b001,
    yellow = 0b011,
    green = 0b010,
    cyan = 0b110,
    black = 0b000,
};

int main()
{
    // clearscreen();
    init_spi();
    init_lcd();

    while(1) {
        // serial.baud(9600);
        // homecursor();

        //initializes the thermometer
        i2c.write(temp_addr, &tempSensorCommands[0], 1);
        wait_us(500000);

		//Write the Read Temperature command to the sensor
        i2c.write(temp_addr, &tempSensorCommands[1], 1);

		//Read the 16-bit temperature data into values
        i2c.read(temp_addr, values, 2);
		
		//Convert temperature to Celsius
        float celcius = (float((values[0] << 8) | values[1]) / 256);

        //Convert to Farenheit
        int fahrenheit = round((celcius*1.8)+32);
        
        //display temps on the LCD display
        display_temps(celcius, fahrenheit);
        
        //Update color of the LED
        ControlLED_BusIO(fahrenheit);

    }
}

//Print to LCD commands put into a function
void display_temps(float celcius, int fahrenheit)
{
    //Print Fahrenheit to the LCD
        char fah[10];
        set_cursor(0, 0);
        print_lcd("Fahrenheit: ");
        sprintf(fah, "%d",fahrenheit); //Converts the int to a string somehow
        print_lcd(fah);

        //Print Celcius to the LCD
        char cel[10];
        set_cursor(0, 1);
        print_lcd("Celcius: ");
        sprintf(cel, "%5.1f",celcius);
        print_lcd(cel);
}

/*
    Switch case for the different colors
    takes in the temp in Fahrenheit and then
    switches to different colors based on the
    ranges below

*/
void ControlLED_BusIO (int x){
    switch(x)
    {
        case -67 ... 49: //basically we dont support these
            out_bus = black;     
            break;
        case 50 ... 70: 
            out_bus = purple;      
            break;
        case 71 ... 90: //6-7
            out_bus = blue; 
            break;
        case 91 ... 110: //8-11
           out_bus = cyan; 
            break;
        case 111 ... 130: //12-14
            out_bus = green; 
            break;
        case 131 ... 150:  
            out_bus = yellow; 
            break;
        case 151 ... 170:
            out_bus = red; 
            break;
        case 171 ... 257: //maybe could make this go all the way to the max temp but idk
            out_bus = white; 
            break;
        default:
            printf("error!!");
        break;
    }
}





