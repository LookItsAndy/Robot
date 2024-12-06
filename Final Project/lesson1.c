/*  ___   ___  ___  _   _  ___   ___   ____ ___  ____  
 * / _ \ /___)/ _ \| | | |/ _ \ / _ \ / ___) _ \|    \ 
 *| |_| |___ | |_| | |_| | |_| | |_| ( (__| |_| | | | |
 * \___/(___/ \___/ \__  |\___/ \___(_)____)___/|_|_|_|
 *                  (____/ 
 * Raspberry Pi Robot Car V2.0 Lesson 1: Basic Installation and Movement in C language
 * Tutorial URL https://osoyoo.com/?p=37327
 *
 * CopyRight www.osoyoo.com
 * 
 */
#include "pca9685/pca9685.h"
#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>

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
#define SPEED 2000

// initialize  IN1,IN2,IN3,IN4 
void setup(){
 pinMode(IN1,OUTPUT);
 pinMode(IN2,OUTPUT);
 pinMode(IN3,OUTPUT);
 pinMode(IN4,OUTPUT);
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
void go_left(int fd,int speed){
    digitalWrite(IN1,HIGH);
    digitalWrite(IN2,LOW);
    digitalWrite(IN3,LOW);
    digitalWrite(IN4,HIGH); 
    pca9685PWMWrite(fd, ENA, 0, speed);
    pca9685PWMWrite(fd, ENB, 0, speed);
}
void go_right(int fd,int speed){
    digitalWrite(IN1,LOW);
    digitalWrite(IN2,HIGH);
    digitalWrite(IN3,HIGH);
    digitalWrite(IN4,LOW); 
    pca9685PWMWrite(fd, ENA, 0, speed);
    pca9685PWMWrite(fd, ENB, 0, speed);
}
void stop_car(int fd){
    digitalWrite(IN1,LOW);
    digitalWrite(IN2,LOW);
    digitalWrite(IN3,LOW);
    digitalWrite(IN4,LOW); 
    pca9685PWMWrite(fd, ENA, 0, 0);
    pca9685PWMWrite(fd, ENB, 0, 0);
}
int main(void)
{
    if(wiringPiSetup()==-1){
        printf("setup wiringPi failed!\n");
        printf("please check your setup\n");
        return -1;
    }
	setup();
	
   // 
	printf("Lesson 1: Basic Movement\n");
 
	// Setup with pinbase 300 and i2c location 0x40
	int fd = pca9685Setup(PIN_BASE, 0x40, HERTZ);
	if (fd < 0)
	{
		printf("Error in setup\n");
		return fd;
	}
 
	go_advance(fd,SPEED);
	delay(1000);
	go_back(fd,SPEED);
	delay(1000);
	go_left(fd,SPEED);
	delay(1000);
	go_right(fd,SPEED);
	delay(1000);
	go_right(fd,SPEED);
	delay(1000);
	stop_car(fd);
 
	return 0;
}
