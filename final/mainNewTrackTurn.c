#include "pca9685/pca9685.h"
#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define PIN_BASE 300
#define MAX_PWM 4096
#define HERTZ 50

#define ENA 0  //left motor speed pin ENA connect to PCA9685 port 0
#define ENB 1  //right motor speed pin ENB connect to PCA9685 port 1
#define IN1 4  //Left motor IN1 connect to wPi pin# 4 (Physical 16,BCM GPIO 23)
#define IN2 5  //Left motor IN2 connect to wPi pin# 5 (Physical 18,BCM GPIO 24)
#define IN3 2  //right motor IN3 connect to wPi pin# 2 (Physical 13,BCM GPIO 27)
#define IN4 3  //right motor IN4 connect to wPi pin# 3 (Physical 15,BCM GPIO 22)

//Default speed 2000
#define SPEED 3000
#define HIGH_SPEED 4000
#define MIN_SPEED 1500


                                        
// Servo constants
#define SERVO_PIN 15  //right motor speed pin ENB connect to PCA9685 port 1
#define DEFAULT_HEAD_TURN_DELAY 150
#define FULL_LEFT 400 //ultrasonic sensor facing right
#define SLIGHT_LEFT 340
#define CENTER 280 //ultrasonic sensor facing front
#define SLIGHT_RIGHT 220
#define FULL_RIGHT 160 //ultrasonic sensor facing left

#define HEAD_POSITIONS 5


#define TRIG 28 //wPi#28=BCM GPIO#20=Physical pin#38
#define ECHO 29 //wPi#29=BCM GPIO#21=Physical pin#40
#define TARGET_DISTANCE 20
#define short_delay 200
#define long_delay  300
#define extra_long_delay 400

void setup() {

 pinMode(TRIG,OUTPUT);
 pinMode(ECHO,INPUT);

}

double distance() {


        //Send trig pulse
        digitalWrite(TRIG, HIGH);
        delayMicroseconds(10);
        digitalWrite(TRIG, LOW);
 
        //Wait for echo start
        long timeoutStart = micros();
        while(digitalRead(ECHO) == LOW) {
            if(micros() - timeoutStart > 30000) {
                printf("Timeout waiting for echo start\n");
                return -1;
            }
		    
	    }
 
        //Wait for echo end
        long startTime = micros();
        while(digitalRead(ECHO) == HIGH);
        long travelTime = micros() - startTime;
 
        //Get distance in cm
        double distance = travelTime / 58;
         if (distance==0) distance=1000;
        
        return distance;
}

void turnHead(int fd) {
    double positions[5];


    int headPositions[5] = {
        FULL_LEFT, 
        SLIGHT_LEFT, 
        CENTER, 
        SLIGHT_RIGHT, 
        FULL_RIGHT
    };


    char headDirection[5][20] = {
        "FULL_LEFT", 
        "SLIGHT_LEFT", 
        "CENTER", 
        "SLIGHT_RIGHT", 
        "FULL_RIGHT"
    };
    
    for(int i = 0; i < 5; i++) {
        pca9685PWMWrite(fd, SERVO_PIN, 0, headPositions[i]);
        delay(DEFAULT_HEAD_TURN_DELAY);
        positions[i] = distance();

        printf("%s: %.2lfcm  ", headDirection[i], positions[i]);
        if (i ==4) {
            printf("\n");
        }
    }
    
    // pca9685PWMWrite(fd, SERVO_PIN, 0, FULL_LEFT);
    // delay(DEFAULT_HEAD_TURN_DELAY);
    // positions[0] = distance();

    // pca9685PWMWrite(fd, SERVO_PIN, 0, SLIGHT_LEFT);
    // delay(DEFAULT_HEAD_TURN_DELAY);
    // positions[1] = distance();

    // pca9685PWMWrite(fd, SERVO_PIN, 0, CENTER);
    // delay(DEFAULT_HEAD_TURN_DELAY);
    // positions[2] = distance();

    // pca9685PWMWrite(fd, SERVO_PIN, 0, SLIGHT_RIGHT);
    // delay(DEFAULT_HEAD_TURN_DELAY);
    // positions[3] = distance();

    // pca9685PWMWrite(fd, SERVO_PIN, 0, FULL_RIGHT);
    // delay(DEFAULT_HEAD_TURN_DELAY);
    // positions[4] = distance();

    // for (int i = 0; i < 5; i++) {
    //     printf("Distance: %.2lf   ", positions[i]);
    //     if (i == 4) {
    //         printf("\n");
    //     }
    // }

}


int main(void) {

    if(wiringPiSetup()==-1){
            printf("setup wiringPi failed!\n");
            printf("please check your setup\n");
            return -1;
        }
        setup();

        printf("Part 1: Object Tracking\n");

    int fd = pca9685Setup(PIN_BASE, 0x40, HERTZ);
    if (fd < 0)
    {
        printf("Error in setup\n");
        return fd;
    }

    pca9685PWMWrite(fd, SERVO_PIN, 0, CENTER);

   
    while(1) {
        
        delay(10);
        turnHead(fd);
        
    }
}
