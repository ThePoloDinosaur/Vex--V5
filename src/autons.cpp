#include "main.h"
#include "autons.hpp"


void driverwall_auton() {


	chassis.setPose(0, 0, 0);
	chassis.moveToPoint(0,24,3000);
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
    pros::delay(2000);
    chassis.turnToHeading(180,1000);
    chassis.waitUntilDone();
    pros::delay(2000);
    chassis.turnToHeading(270,1000);
    chassis.waitUntilDone();
    pros::delay(2000);
    chassis.turnToHeading(0,1000);
}