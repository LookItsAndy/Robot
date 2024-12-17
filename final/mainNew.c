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


// SPEED 2000 , factor divider is 1000 
// SPEED divider should be based on how many digits SPEED has
const double MOTOR_FACTOR = SPEED / 100;

#define MAX_DISTANCE 50.0 //cm
#define STOP_DISTANCE 20.0 //cm
const double DISTANCE_FACTOR = MAX_DISTANCE / 100;

const double L_MOTOR_FACTOR = 0.815;
const double R_MOTOR_FACTOR = 1.0;
const double L_MOTOR_FACTOR_THRESHOLD = 8000.0;
const double R_MOTOR_FACTOR_THRESHOLD = 8000.0;

double current_distance = 0.00;
double leftTurnFactor = 0.0;
double rightTurnFactor = 0.0;

                                        
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



double distance() {


        //delay(100);
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

void turnRobot (int direction) {

    //full left
    if (direction == 0) {
        leftTurnFactor = 0.75;
        rightTurnFactor = 1;
    }
    //slight left
    else if(direction == 1) {
        leftTurnFactor = 0.9;
        rightTurnFactor = 1;
        
    }
    //center
    else if (direction == 2) {
        leftTurnFactor = 1.0;
        rightTurnFactor = 1.0;
    }
    //slight right
    else if (direction == 3) {
        leftTurnFactor = 1;
        rightTurnFactor = 0.9;
    }
    //full right
    else if (direction == 4) {
        leftTurnFactor = 1;
        rightTurnFactor = 0.75;
    } else
    {
        leftTurnFactor = 0;
        rightTurnFactor = 0;
    }
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

    int intHeadDirection[5] = {
        0, //full left
        1, //slight left
        2, //center
        3, //slight right
        4 //full right
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

    for(int i = 0; i < 5; i++) {
        double min = 0.0;
        double temp = 0.0;
        int direction;

        if (i == 0) {
            min = positions[i];
            direction = intHeadDirection[i];
        }

        temp = positions[i];

        if (temp < min) {
            min = temp;
            direction = intHeadDirection[i];
        }

        turnRobot(direction);
    }
}





// function to set variable speed to motors
void setMotors(int fd, double current_distance) {
    delay(20);
    double leftSpeed = SPEED;
    double rightSpeed = SPEED;
    
    
    printf("comparing distance to max\n");
    if(current_distance <= MAX_DISTANCE) {
        double magnitude = (double)(MAX_DISTANCE - current_distance) / DISTANCE_FACTOR;
        leftSpeed = SPEED - (magnitude * MOTOR_FACTOR);
        rightSpeed = SPEED - (magnitude * MOTOR_FACTOR);

        if(leftSpeed < MIN_SPEED) {
            leftSpeed = MIN_SPEED;
        }

        if(rightSpeed < MIN_SPEED) {
            rightSpeed = MIN_SPEED;
        }
    } else {
        rightSpeed = 0;
        leftSpeed = 0;
    }

    //printf("running limit checks\n");
    // lower limit check
    
    // add in motor compensation
    /*if (leftSpeed <= L_MOTOR_FACTOR_THRESHOLD) {
        leftSpeed *= L_MOTOR_FACTOR;

    }

    
    if (rightSpeed <= R_MOTOR_FACTOR_THRESHOLD) {
        rightSpeed *= R_MOTOR_FACTOR;

    }*/


    
    // check stop distance
    if(current_distance <= STOP_DISTANCE && current_distance >= 10) leftSpeed = 0; 
    if(current_distance <= STOP_DISTANCE  && current_distance >= 10) rightSpeed = 0;

    if(current_distance < 10) {
        leftSpeed = -leftSpeed;   
        rightSpeed = -rightSpeed;
    }


    if(rightSpeed == 0 && leftSpeed == 0) {
        digitalWrite(IN1,LOW);
        digitalWrite(IN2,LOW);
        digitalWrite(IN3,LOW);
        digitalWrite(IN4,LOW); 
        pca9685PWMWrite(fd, ENA, 0, 0);
        pca9685PWMWrite(fd, ENB, 0, 0);
	    //printf("LEFT_SPEED: %f \n", leftSpeed);
	   // printf("RIGHT_SPEED: %f \n", rightSpeed);
        

    } else if(rightSpeed < 0 && leftSpeed < 0) {

        digitalWrite(IN1,HIGH);
        digitalWrite(IN2,LOW);
        digitalWrite(IN3,HIGH);
        digitalWrite(IN4,LOW); 
        leftSpeed *= L_MOTOR_FACTOR;
        pca9685PWMWrite(fd, ENA, 0, leftSpeed);
        pca9685PWMWrite(fd, ENB, 0, rightSpeed);


    }   else {

        

        digitalWrite(IN1,LOW);
        //printf("IN1 low\n");
        digitalWrite(IN2,HIGH);
        //printf("IN2 high\n");
        digitalWrite(IN3,LOW);
        //printf("IN3 low\n");
        digitalWrite(IN4,HIGH);
	//printf("IN4 high\n");
	
	    //printf("LEFT_SPEED: %f \n", leftSpeed);
	    //printf("RIGHT_SPEED: %f \n", rightSpeed);

        leftSpeed *= L_MOTOR_FACTOR;

        leftSpeed *= leftTurnFactor;
        rightSpeed *= rightTurnFactor;
	
	
	
        pca9685PWMWrite(fd, ENA, 0, leftSpeed);
        pca9685PWMWrite(fd, ENB, 0, rightSpeed);

        
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
        current_distance = distance();
        printf("Distance is: %.2lf\n", current_distance);
        delay(10);
        turnHead(fd);
	    //printf("before calling setMotor main\n");
        setMotors(fd, current_distance);
	    //printf("\n-=-=-=-=-=-setMotors is called-=-=-=-=-=--=-\n");
        
    }
	
}
