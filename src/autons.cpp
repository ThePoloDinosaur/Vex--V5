#include "liblvgl/draw/lv_draw.h"
#include "main.h"
#include "pros/rtos.h"
#include "autons.hpp"


void driverwall_auton() {


	chassis.setPose(0, 0, 0);
	chassis.moveToPose(0,8,0, 800, {.maxSpeed = 200});
	chassis.moveToPose(0,-1,0,800, {.forwards = false, .maxSpeed = 200});
	chassis.moveToPose(0,8,0,800, {.maxSpeed = 200});
	chassis.moveToPose(0,-1,0,800, {.forwards = false, .maxSpeed = 200});
	chassis.moveToPose(0,13.5,0,800, {.maxSpeed = 200});
	chassis.turnToHeading(90,500);
	chassis.moveToPoint(10.5, 13.5, 800);
	chassis.moveToPoint(20, 13.5, 500);
	piston.set_value(false);
	pros::delay(50);
	chassis.setPose(0, 0, 0);
	pros::delay(50);
	chassis.moveToPose(0,-24,0,1000, {.forwards = false, .maxSpeed = 200});
	chassis.turnToPoint(-18,-1,1000);
	chassis.moveToPoint(-18,-1,2000, {.maxSpeed = 200});
	pros::delay(200);
	chassis.moveToPoint(-19.8,2.8,2000, {.maxSpeed = 3});
	pros::delay(500);
	piston.set_value(true);
	pros::delay(800);
	lift_motors.move_velocity(-200);
	pros::delay(1500);
	lift_motors.move_velocity(0);
	chassis.moveToPoint(-24,7,1000, {.maxSpeed = 200});
	pros::delay(300);
	chassis.turnToHeading(90,1000);
	pros::delay(300);
	chassis.moveToPoint(-8,7,1000, {.maxSpeed = 80});
	pros::delay(1000);
	piston.set_value(false);
	pros::delay(300);
	chassis.setPose(0, 0, 0);
	pros::delay(300);
	chassis.moveToPoint(0,-22,1000, {.forwards = false, .maxSpeed = 200});
	chassis.turnToHeading(-45,1000);
	
}

void sidewall_auton() {


	chassis.setPose(0, 0, 0);
	chassis.moveToPoint(0,24,3000);
}


void skills_auton() {


	chassis.setPose(0, 0, 0);
	chassis.moveToPoint(0,24,3000);
}


void pid_lateral_auton() {


	chassis.setPose(0, 0, 0);
	chassis.moveToPoint(0,48,5000);

}


void pid_angular_auton() {


	chassis.setPose(0, 0, 0);
	chassis.turnToHeading(90,1000);
    chassis.waitUntilDone();
    pros::delay(4500);
    chassis.turnToHeading(180,1000);
    chassis.waitUntilDone();
    pros::delay(4500);
    chassis.turnToHeading(270,1000);
    chassis.waitUntilDone();
    pros::delay(4500);
    chassis.turnToHeading(0,1000);
}