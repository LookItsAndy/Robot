#include "pca9685/pca9685.h"
#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define PIN_BASE 300
#define MAX_PWM 4096
#define HERTZ 50

/*
 * wiringPi C library use different GPIO pin number system from BCM pin numberwhich are often used by Python, 
 * you can lookup BCM/wPi/Physical pin relation by following Linux command : gpio readall
 */
#define ENA 0  //left motor speed pin ENA connect to PCA9685 port 0
#define ENB 1  //right motor speed pin ENB connect to PCA9685 port 1
#define IN1 4  //Left motor IN1 connect to wPi pin# 4 (Physical 16,BCM GPIO 23)
#define IN2 5  //Left motor IN2 connect to wPi pin# 5 (Physical 18,BCM GPIO 24)
#define IN3 2  //right motor IN3 connect to wPi pin# 2 (Physical 13,BCM GPIO 27)
#define IN4 3  //right motor IN4 connect to wPi pin# 3 (Physical 15,BCM GPIO 22)

//Default speed 2000
#define SPEED 2000
#define HIGH_SPEED 3000
#define LOW_SPEED 1500
#define DEFAULT_HEAD_TURN_DELAY 1000

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

int avoid = 1; // While loop for avoiding objects
int track = 1; // While loop for tracking object
int SL=(LEFT+CENTER)/2;
int SR=(RIGHT+CENTER)/2;

int stsAvoid0=0;
int stsAvoid1=0;
int stsAvoid2=0;
char valAvoid[3];

int stsTrack0=0;
int stsTrack1=0;
int stsTrack2=0;
char valTrack[3];

// initialize  IN1,IN2,IN3,IN4 
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

void go_back(int fd,int speed){
    digitalWrite(IN1,HIGH);
    digitalWrite(IN2,LOW);
    digitalWrite(IN3,HIGH);
    digitalWrite(IN4,LOW); 
    pca9685PWMWrite(fd, ENA, 0, speed);
    pca9685PWMWrite(fd, ENB, 0, speed);
}

void go_advance(int fd,int speed){
    digitalWrite(IN1,LOW);
    digitalWrite(IN2,HIGH);
    digitalWrite(IN3,LOW);
    digitalWrite(IN4,HIGH); 
    pca9685PWMWrite(fd, ENA, 0, speed);
    pca9685PWMWrite(fd, ENB, 0, speed);
}

void go_left(int fd,int left_speed,int right_speed){
    digitalWrite(IN1,HIGH);
    digitalWrite(IN2,LOW);
    digitalWrite(IN3,LOW);
    digitalWrite(IN4,HIGH); 
    pca9685PWMWrite(fd, ENA, 0, left_speed);
    pca9685PWMWrite(fd, ENB, 0, right_speed);
}

void go_right(int fd,int left_speed,int right_speed){
    digitalWrite(IN1,LOW);
    digitalWrite(IN2,HIGH);
    digitalWrite(IN3,HIGH);
    digitalWrite(IN4,LOW); 
    pca9685PWMWrite(fd, ENA, 0, left_speed);
    pca9685PWMWrite(fd, ENB, 0, right_speed);
}

void stop_car(int fd){
    digitalWrite(IN1,LOW);
    digitalWrite(IN2,LOW);
    digitalWrite(IN3,LOW);
    digitalWrite(IN4,LOW); 
    pca9685PWMWrite(fd, ENA, 0, 0);
    pca9685PWMWrite(fd, ENB, 0, 0);
}

int distance() {
        //Send trig pulse
        digitalWrite(TRIG, HIGH);
        delayMicroseconds(20);
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

void scan_surroundings_avoid(int fd) {

    pca9685PWMWrite(fd, SERVO_PIN, 0, LEFT);
    delay(DEFAULT_HEAD_TURN_DELAY);
    if (distance()<OBSTACLE) stsAvoid0=1;
    else stsAvoid0=0;
    valAvoid[0]='0'+stsAvoid0;
    
    pca9685PWMWrite(fd, SERVO_PIN, 0, CENTER);
    delay(DEFAULT_HEAD_TURN_DELAY);
    if (distance()<OBSTACLE) stsAvoid1=1;
    else stsAvoid1=0;
    valAvoid[1]='0'+stsAvoid1;
    
    pca9685PWMWrite(fd, SERVO_PIN, 0, RIGHT);
    delay(DEFAULT_HEAD_TURN_DELAY);
    if (distance()<OBSTACLE) stsAvoid2=1;
    else stsAvoid2=0;
    valAvoid[2]='0'+stsAvoid2;
            
}

void scan_surroundings_track(int fd) {

    pca9685PWMWrite(fd, SERVO_PIN, 0, LEFT);
    delay(DEFAULT_HEAD_TURN_DELAY);
    if (distance()>OBSTACLE) stsTrack0=1;
    else stsTrack0=0;
    valTrack[0]='0'+stsTrack0;
    
    pca9685PWMWrite(fd, SERVO_PIN, 0, CENTER);
    delay(DEFAULT_HEAD_TURN_DELAY);
    if (distance()>OBSTACLE) stsTrack1=1;
    else stsTrack1=0;
    valTrack[1]='0'+stsTrack1;
    
    pca9685PWMWrite(fd, SERVO_PIN, 0, RIGHT);
    delay(DEFAULT_HEAD_TURN_DELAY);
    if (distance()>OBSTACLE) stsTrack2=1;
    else stsTrack2=0;
    valTrack[2]='0'+stsTrack2;
            
}

int main(void) 
{
    if(wiringPiSetup()==-1){
        printf("setup wiringPi failed!\n");
        printf("please check your setup\n");
        return -1;
    }
	setup();

    printf("Part 1: Object Tracking");

    // Setup with pinbase 300 and i2c location 0x40
	int fd = pca9685Setup(PIN_BASE, 0x40, HERTZ);
	if (fd < 0)
	{
		printf("Error in setup\n");
		return fd;
	}
    // Orientates Ultra sonic sensor left and waits 1 second
    pca9685PWMWrite(fd, SERVO_PIN, 0, LEFT);
	delay(1000);

    // Orientates Ultra Sonic Sensor center.
	pca9685PWMWrite(fd, SERVO_PIN, 0, CENTER);
	delay(1000);
    // 
	pca9685PWMWrite(fd, SERVO_PIN, 0, RIGHT);
	delay(1000);
	pca9685PWMWrite(fd, SERVO_PIN, 0, CENTER);
	delay(1000);
    // extern void pca9685PWMWrite(int fd, int pin, int on, int off);
    while (1)
	{
        while (avoid) {

            scan_surroundings_avoid(fd);

            if (strcmp("100",valAvoid)==0)
            {	
                printf("100 slight right\n");
                go_right(fd,SPEED,0);
                delay(long_delay);  
                stop_car(fd);
                delay(short_delay);
            }
            if (strcmp("001",valAvoid)==0)
            {
                printf("100 slight left\n");
                go_left(fd,0,SPEED);
                delay(long_delay);  
                stop_car(fd);
                delay(short_delay);		
            }
            if (strcmp("110",valAvoid)==0)
            {
                printf("110 sharp right\n");
                go_right(fd,HIGH_SPEED,LOW_SPEED);
                delay(long_delay);  
                stop_car(fd);
                delay(short_delay);	
            }
            if (strcmp("011",valAvoid)==0)
            {
                printf("011 sharp left\n");
                go_left(fd,LOW_SPEED,HIGH_SPEED);
                delay(long_delay);  
                stop_car(fd);
                delay(short_delay);	
            }  
            if (strcmp("111",valAvoid)==0 || strcmp("101",valAvoid)==0 || strcmp("010",valAvoid)==0  )
            {
                printf("%s sharp turn back\n",valAvoid);
                go_left(fd,HIGH_SPEED,HIGH_SPEED);
                delay(extra_long_delay);  
                stop_car(fd);
                delay(short_delay);	
            }        
            if (strcmp("000",valAvoid)==0)
            {
                avoid = 0;
                track = 1;
            }
         
        }

        while (track) {
            scan_surroundings_avoid(fd);
            
            if (strcmp("100", valAvoid) == 0 || strcmp("001", valAvoid) == 0 || strcmp("110", valAvoid) == 0 || strcmp("011", valAvoid) == 0 ||
            strcmp("111", valAvoid) == 0 || strcmp("101", valAvoid) == 0 || strcmp("010", valAvoid) == 0) {

                track = 0;
                avoid = 1;

            }

	    scan_surroundings_track(fd);

	    if (distance()>1)
{
		printf("Object in view");
}


            if (strcmp("100", valTrack) == 0) {
                
                printf("Tracking: [100] slight left\n");
                go_left(fd, 0, SPEED);
                delay(long_delay);
                stop_car(fd);
                delay(short_delay);
        
            }
            if (strcmp("001", valTrack) == 0) {
                
                printf("Tracking: [001] slight right\n");
                go_right(fd, SPEED, 0);
                delay(long_delay);
                stop_car(fd);
                delay(short_delay);
        
            }
            if (strcmp("110", valTrack) == 0) {

                printf("Tracking: [110] sharp left\n");
                go_left(fd,LOW_SPEED, HIGH_SPEED);
                delay(long_delay);
                stop_car(fd);
                delay(short_delay);
            }
            if (strcmp("011", valTrack) == 0 ){ 

                printf("Tracking: [011] sharp right\n");
                go_right(fd, HIGH_SPEED, LOW_SPEED);
                delay(long_delay);
                stop_car(fd);
                delay(short_delay);
            }
            if (strcmp("111", valTrack) == 0 || strcmp("101", valTrack) == 0 || strcmp("010", valTrack) == 0) {
                
                printf("Tracking: [111, 101, 010] forward\n");
                go_advance(fd, SPEED);
                delay(long_delay);
                stop_car(fd);
                delay(short_delay);
            }

        }
	 
	}

}