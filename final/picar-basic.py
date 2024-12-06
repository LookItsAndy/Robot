import time
# Import the PCA9685 module.
from board import SCL, SDA
import busio
# Import the PCA9685 module.
from adafruit_pca9685 import PCA9685
# Create the I2C bus interface.
i2c_bus = busio.I2C(SCL, SDA)

import RPi.GPIO as GPIO
# Initialise the PCA9685 using the default address (0x40).
pwm =  PCA9685(i2c_bus)

move_speed = 0x7FFF  # half of Max pulse length out of 0xFFFF

# Set frequency to 60hz, good for servos.
pwm.frequency = 60
GPIO.setmode(GPIO.BCM) # GPIO number  in BCM mode
GPIO.setwarnings(False)
#define L298N(Model-Pi motor drive board) GPIO pins
IN1 = 23  #Left motor direction pin
IN2 = 24  #Left motor direction pin
IN3 = 27  #Right motor direction pin
IN4 = 22  #Right motor direction pin
ENA = 0  #Left motor speed PCA9685 port 0
ENB = 1  #Right motor speed PCA9685 port 1

# Define motor control  pins as output
GPIO.setup(IN1, GPIO.OUT)   
GPIO.setup(IN2, GPIO.OUT) 
GPIO.setup(IN3, GPIO.OUT)   
GPIO.setup(IN4, GPIO.OUT) 

def changespeed(speed):
	pwm.channels[ENA].duty_cycle = speed
	pwm.channels[ENB].duty_cycle = speed


def stopcar():
	GPIO.output(IN1, GPIO.LOW)
	GPIO.output(IN2, GPIO.LOW)
	GPIO.output(IN3, GPIO.LOW)
	GPIO.output(IN4, GPIO.LOW)
	changespeed(0)


def backward():
	GPIO.output(IN1, GPIO.HIGH)
	GPIO.output(IN2, GPIO.LOW)
	GPIO.output(IN3, GPIO.HIGH)
	GPIO.output(IN4, GPIO.LOW)
	changespeed(move_speed)
 
	#following two lines can be removed if you want car make continuous movement without pause
	#time.sleep(0.25)  
	#stopcar()
	
def forward():
	GPIO.output(IN2, GPIO.HIGH)
	GPIO.output(IN1, GPIO.LOW)
	GPIO.output(IN4, GPIO.HIGH)
	GPIO.output(IN3, GPIO.LOW)
	changespeed(move_speed)
	#following two lines can be removed if you want car make continuous movement without pause
	#time.sleep(0.25)  
	#stopcar()
	
def turnRight():
	GPIO.output(IN1, GPIO.LOW)
	GPIO.output(IN2, GPIO.HIGH)
	GPIO.output(IN3, GPIO.HIGH)
	GPIO.output(IN4, GPIO.LOW)
	changespeed(move_speed)
	#following two lines can be removed if you want car make continuous movement without pause
	#time.sleep(0.25)  
	#stopcar()
	
def turnLeft():
	GPIO.output(IN1, GPIO.HIGH)
	GPIO.output(IN2, GPIO.LOW)
	GPIO.output(IN3, GPIO.LOW)
	GPIO.output(IN4, GPIO.HIGH)
	changespeed(move_speed)	
	#following two lines can be removed if you want car make continuous movement without pause
	#time.sleep(0.25)  
	#stopcar()
	
forward()
time.sleep(1)  
stopcar()
time.sleep(0.25)

backward()
time.sleep(1)  
stopcar()
time.sleep(0.25) 

turnLeft()
time.sleep(1)  
stopcar()
time.sleep(0.25)
	
turnRight()
time.sleep(1)  
stopcar()
time.sleep(0.25)

 



print('press Ctrl-C to quit...')
#while True:
# Move servo on channel O between extremes.

time.sleep(2)