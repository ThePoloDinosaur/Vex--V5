#include "liblvgl/draw/lv_draw.h"
#include "main.h"
#include "autons.hpp"


void driverwall_auton() {


	chassis.setPose(0, 0, 0);
	chassis.moveToPose(0,20,0, 1000, {.maxSpeed = 200});
	chassis.moveToPose(0,-1,0,1000, {.forwards = false, .maxSpeed = 200});
	chassis.moveToPose(0,10,0,1000, {.maxSpeed = 200});
	chassis.moveToPose(0,-1,0,1000, {.forwards = false, .maxSpeed = 200});
	chassis.moveToPose(0,13.5,0,1000, {.maxSpeed = 200});
	chassis.turnToHeading(90,1000);
	lift_motors.move_velocity(-200);
	pros::delay(200);
	lift_motors.move_velocity(0);
	chassis.moveToPoint(10.5, 13.5, 1000);
	pros::delay(800);
	chassis.moveToPoint(20, 13.5, 500);
	piston.set_value(false);
	pros::delay(300);
	chassis.setPose(0, 0, 0);
	pros::delay(300);
	chassis.moveToPose(0,-24,0,1000, {.forwards = false, .maxSpeed = 200});
	pros::delay(300);
	chassis.turnToPoint(-19.6,0.5,1000);
	pros::delay(300);
	chassis.moveToPoint(-19.6,0.5,2000, {.maxSpeed = 200});
	pros::delay(2000);
	chassis.moveToPoint(-24,7,500, {.maxSpeed = 40});
	/*
	pros::delay(1000);
	piston.set_value(true);
	pros::delay(1000);
	lift_motors.move_velocity(-200);
	pros::delay(4500);
	lift_motors.move_velocity(0);
	chassis.moveToPoint(-24,7,1000, {.maxSpeed = 200});
	pros::delay(300);
	chassis.turnToHeading(90,1000);
	pros::delay(300);
	chassis.moveToPoint(0,7,1000, {.maxSpeed = 200});
	*/

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