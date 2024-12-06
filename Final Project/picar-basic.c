#include <stdio.h>
#include <unistd.h>
#include <wiringPi.h>
#include <softPwm.h>
#include <stdint.h>

// Define L298N GPIO pins
#define IN1 23  // Left motor direction pin
#define IN2 24  // Left motor direction pin
#define IN3 27  // Right motor direction pin
#define IN4 22  // Right motor direction pin
#define ENA 0   // Left motor speed PCA9685 port 0
#define ENB 1   // Right motor speed PCA9685 port 1

#define MOVE_SPEED 0x7FFF  // half of Max pulse length out of 0xFFFF

void changespeed(int speed);
void stopcar();
void backward();
void forward();
void turnRight();
void turnLeft();

int main() {
    wiringPiSetupGpio(); // Use BCM GPIO numbering
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);

    // Initialize PWM for motor speed control
    softPwmCreate(ENA, 0, 100); // Create PWM for left motor
    softPwmCreate(ENB, 0, 100); // Create PWM for right motor

    forward();
    sleep(1);
    stopcar();
    usleep(250000); // 0.25 seconds

    backward();
    sleep(1);
    stopcar();
    usleep(250000); // 0.25 seconds

    turnLeft();
    sleep(1);
    stopcar();
    usleep(250000); // 0.25 seconds

    turnRight();
    sleep(1);
    stopcar();
    usleep(250000); // 0.25 seconds

    printf("press Ctrl-C to quit...\n");
    sleep(2);

    return 0;
}

void changespeed(int speed) {
    softPwmWrite(ENA, speed);
    softPwmWrite(ENB, speed);
}

void stopcar() {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
    changespeed(0);
}

void backward() {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    changespeed(MOVE_SPEED);
}

void forward() {
    digitalWrite(IN2, HIGH);
    digitalWrite(IN1, LOW);
    digitalWrite(IN4, HIGH);
    digitalWrite(IN3, LOW);
    changespeed(MOVE_SPEED);
}

void turnRight() {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    changespeed(MOVE_SPEED);
}

void turnLeft() {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    changespeed(MOVE_SPEED);
}

