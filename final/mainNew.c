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
#define SPEED 500
#define HIGH_SPEED 3000
#define MIN_SPEED 250
const float MOTOR_FACTOR = SPEED / 1000;

#define DEFAULT_HEAD_TURN_DELAY 200

#define MAX_DISTANCE 100.0 //cm
#define STOP_DISTANCE 10.0 //cm
const float DISTANCE_FACTOR = MAX_DISTANCE / 1000;


                                        
#define SERVO_PIN 15  //right motor speed pin ENB connect to PCA9685 port 1
#define LEFT 400 //ultrasonic sensor facing right
#define CENTER 280//ultrasonic sensor facing front
#define RIGHT 160 //ultrasonic sensor facing left
#define TRIG 28 //wPi#28=BCM GPIO#20=Physical pin#38
#define ECHO 29 //wPi#29=BCM GPIO#21=Physical pin#40
#define OBSTACLE 20
#define short_delay 200
#define long_delay  300
#define extra_long_delay 400

void setup() {

 pinMode(IN1,OUTPUT);
 pinMode(IN2,OUTPUT);
 pinMode(IN3,OUTPUT);
 pinMode(IN4,OUTPUT);
 pinMode(TRIG,OUTPUT);
 pinMode(ECHO,INPUT);
 
 digitalWrite(IN1,LOW);
 digitalWrite(IN2,LOW);
 digitalWrite(IN3,LOW);
 digitalWrite(IN4,LOW);

}


int distance() {
        //Send trig pulse
        digitalWrite(TRIG, HIGH);
        delayMicroseconds(10);
        digitalWrite(TRIG, LOW);
 
        //Wait for echo start
        while(digitalRead(ECHO) == LOW);
 
        //Wait for echo end
        long startTime = micros();
        while(digitalRead(ECHO) == HIGH);
        long travelTime = micros() - startTime;
 
        //Get distance in cm
        int distance = travelTime / 58;
         if (distance==0) distance=1000;
        
        return distance;
}

// function to set variable speed to motors
void setMotors(int fd, int current_distance) {
    
    float leftSpeed = SPEED;
    float rightSpeed = SPEED;
    
    
    printf("comparing distance to max\n");
    if(current_distance <= MAX_DISTANCE) {
        float magnitude = (float)(MAX_DISTANCE - current_distance) / DISTANCE_FACTOR;
        leftSpeed = SPEED - (magnitude * MOTOR_FACTOR);
        rightSpeed = SPEED - (magnitude * MOTOR_FACTOR);
    }

    printf("running limit checks\n");
    // lower limit check
    if(leftSpeed < MIN_SPEED) {
        leftSpeed = MIN_SPEED;
    }

    if(rightSpeed < MIN_SPEED) {
        rightSpeed = MIN_SPEED;
    }
    
    printf("checking stop distance\n");
    // check stop distance
    if(current_distance <= STOP_DISTANCE) leftSpeed = 0;
    if(current_distance <= STOP_DISTANCE) rightSpeed = 0;

    if(rightSpeed == 0 && leftSpeed == 0) {
        digitalWrite(IN1,LOW);
        digitalWrite(IN2,LOW);
        digitalWrite(IN3,LOW);
        digitalWrite(IN4,LOW); 
        pca9685PWMWrite(fd, ENA, 0, 0);
        pca9685PWMWrite(fd, ENB, 0, 0);
        printf("stopped motors\n");

    } else {

        printf("writing speed to motors\n");

        digitalWrite(IN1,LOW);
        printf("IN1 low\n");
        digitalWrite(IN2,HIGH);
        printf("IN2 high");
        digitalWrite(IN3,LOW);
        printf("IN3 low\n");
        digitalWrite(IN4,HIGH);
        printf("IN4 high\n");

        pca9685PWMWrite(fd, ENA, 0, leftSpeed);
        pca9685PWMWrite(fd, ENB, 0, rightSpeed);
        printf("move\n");
        
    }
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
        int current_distance = distance();
        printf("Distance is: %d\n", current_distance);
        delay(10);
        setMotors(fd, current_distance);
        
    }

}
