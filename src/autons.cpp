#include "liblvgl/draw/lv_draw.h"
#include "main.h"
#include "pros/rtos.h"
#include "autons.hpp"
#include <cmath>

void drive_forward_hard(double inches, int timeout_ms, bool forwards) {

	lemlib::Pose start = chassis.getPose();

	int start_time = pros::millis();

	int power = forwards ? 127 : -127;

	left_motors.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
	right_motors.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);

	while (pros::millis() - start_time < timeout_ms) {

		lemlib::Pose now = chassis.getPose();

		double traveled = std::hypot(now.x - start.x, now.y - start.y);

		if (traveled >= inches) {
			break;
		}

		left_motors.move(power);
		right_motors.move(power);

		pros::delay(10);
	}

	left_motors.brake();
	right_motors.brake();

	pros::delay(150);

	left_motors.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
	right_motors.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
}



void driverwall_auton() {

	chassis.setPose(0, 0, 0);
	drive_forward_hard(14, 1000);
	pros::delay(50);
	chassis.moveToPose(0,8,0, 800, {.forwards = false}); // Go forward more to ensure the toggle is sprung
	pros::delay(50);
	drive_forward_hard(5, 1000);
	pros::delay(50);
	chassis.moveToPose(0,-1,0,800, {.forwards = false}); // Go back to the wall and flip the toggle
	pros::delay(50);
	chassis.moveToPose(0,8,0,800); // Go forward so the toggle can be flipped again
	pros::delay(50);
	chassis.moveToPose(0,-1,0,800, {.forwards = false}); // Go back to the wall and flip the toggle again
	pros::delay(1000);
	chassis.setPose(0, 0, 0);
	pros::delay(50);
	chassis.moveToPose(0,13.5,0,800); // Go to the alliance goal y
	pros::delay(50);
	chassis.turnToHeading(90,500); // Turn to face the alliance goal
	pros::delay(50);
	lift_motors.move_velocity(-30); // Lift the lift so the pin can be scored in the alliance goal
	chassis.moveToPoint(10.5, 13.5, 800); // Move to the alliance goal x while the lift is lifting
	lift_motors.move_velocity(0); // Stop the lift
	pros::delay(50);
	chassis.moveToPoint(20, 13.5, 500); // Move forward to ensure the robot is aligned with the alliance goal
	pros::delay(500);
	piston.set_value(false); // Release the pin into the alliance goal
	pros::delay(200);
	chassis.setPose(0, 0, 0); // Reset the pose as the robot is in an aligned position
	pros::delay(50);
	chassis.moveToPose(0,-24,0,1000, {.forwards = false}); // Back up so that the robot can turn to face the first cup-pin
	chassis.turnToPoint(-18.5,-1,1000); // Turn to face the first cup-pin
	chassis.moveToPoint(-18.5,-1,2000); // Move to the first cup-pin
	pros::delay(600);
	chassis.moveToPoint(-19.8,2.8,2000, {.maxSpeed = 3}); // Move slowly to the first cup-pin so that the robot doesn't knock it over
	pros::delay(500);
	piston.set_value(true); // Grab the cup-pin
	pros::delay(800);
	lift_motors.move_velocity(-200); // Start lifting the cup-pin so it can be scored in the alliance goal
	chassis.moveToPoint(-24,7,1000); // Move to the alliance goal y while the lift is lifting
	pros::delay(300);
	chassis.turnToHeading(90,1000); // Turn to face the alliance goal
	pros::delay(300);
	chassis.moveToPoint(-8,7,1000, {.maxSpeed = 200}); // Move to the alliance goal x while the lift is lifting
	pros::delay(300);
	lift_motors.move_velocity(200); // Start lowering the lift so the cup-pin can be scored in the alliance goal
	pros::delay(300);
	chassis.moveToPoint(-4,7,1000, {.maxSpeed = 80}); // Move forward to ensure the robot is aligned with the alliance goal
	pros::delay(500);
	lift_motors.move_velocity(0); // Stop the lift
	pros::delay(300);
	piston.set_value(false); // Release the cup-pin onto the alliance goal
	pros::delay(300);
	chassis.setPose(0, 0, 0); // Reset the pose as the robot is in an aligned position
	pros::delay(300);
	chassis.moveToPoint(0,-22,1000, {.forwards = false}); // Back up so that the robot can turn to face the next cup-pin
	chassis.turnToHeading(-45,1000); // Turn to face the next cup-pin
	// Works until here, next part is to move to the next cup-pin and score it in the alliance goal if we have time, prioritize the sidewall auton before this
}

void sidewall_auton() {

	chassis.setPose(0, 0, 0);
	drive_forward_hard(14, 1000);
	pros::delay(50);
	chassis.moveToPose(0,8,0, 800, {.forwards = false}); // Go forward more to ensure the toggle is sprung
	pros::delay(50);
	drive_forward_hard(5, 1000);
	chassis.moveToPose(0,-1,0,800, {.forwards = false}); // Go back to the wall and flip the toggle
	pros::delay(50);
	chassis.moveToPose(0,8,0,800); // Go forward so the toggle can be flipped again
	pros::delay(50);
	chassis.moveToPose(0,-1,0,800, {.forwards = false}); // Go back to the wall and flip the toggle again
	pros::delay(1000);
	chassis.setPose(0, 0, 0);
	pros::delay(50);
	chassis.moveToPose(0,13.5,0,800); // Go to the alliance goal y
	pros::delay(50);
	lift_motors.move_velocity(-30); // Lift the lift so the pin can be scored in the alliance goal
	pros::delay(50);
	chassis.moveToPoint(-10.5, 13.5, 800); // Move to the alliance goal x while the lift is lifting
	lift_motors.move_velocity(0); // Stop the lift
	chassis.moveToPoint(-20, 13.5, 500); // Move forward to ensure the robot is aligned with the alliance goal
	pros::delay(500);
	piston.set_value(false); // Release the pin into the alliance goal
	pros::delay(200);
	chassis.setPose(0, 0, 0); // Reset the pose as the robot is in an aligned position
	pros::delay(50);
	chassis.moveToPose(0,-24,0,1000, {.forwards = false}); // Back up so that the robot can turn to face the first cup-pin
	chassis.turnToPoint(18.5,-1,1000); // Turn to face the first cup-pin
	chassis.moveToPoint(18.5,-1,2000); // Move to the first cup-pin
	pros::delay(200);
	chassis.moveToPoint(19.8,2.8,2000, {.maxSpeed = 3}); // Move slowly to the first cup-pin so that the robot doesn't knock it over
	pros::delay(500);
	piston.set_value(true); // Grab the cup-pin
	pros::delay(800);
	lift_motors.move_velocity(-200); // Start lifting the cup-pin so it can be scored in the alliance goal
	chassis.moveToPoint(24,7,1000); // Move to the alliance goal y while the lift is lifting
	pros::delay(300);
	chassis.turnToHeading(-90,1000); // Turn to face the alliance goal
	pros::delay(300);
	chassis.moveToPoint(8,7,1000, {.maxSpeed = 200}); // Move to the alliance goal x while the lift is lifting
	pros::delay(300);
	lift_motors.move_velocity(200); // Start lowering the lift so the cup-pin can be scored in the alliance goal
	pros::delay(300);
	chassis.moveToPoint(4,7,1000, {.maxSpeed = 80}); // Move forward to ensure the robot is aligned with the alliance goal
	pros::delay(500);
	lift_motors.move_velocity(0); // Stop the lift
	pros::delay(300);
	piston.set_value(false); // Release the cup-pin onto the alliance goal
	pros::delay(300);
	chassis.setPose(0, 0, 0); // Reset the pose as the robot is in an aligned position
	pros::delay(300);
	chassis.moveToPoint(0,-22,1000, {.forwards = false}); // Back up so that the robot can turn to face the next cup-pin
	chassis.turnToHeading(45,1000); // Turn to face the next cup-pin
}


void skills_auton() {

	chassis.setPose(0, 0, 0);
	drive_forward_hard(14, 1000);
	pros::delay(50);
	chassis.moveToPose(0,8,0, 800, {.forwards = false}); // Go forward more to ensure the toggle is sprung
	pros::delay(50);
	drive_forward_hard(5, 1000);
	pros::delay(50);
	chassis.moveToPose(0,-1,0,800, {.forwards = false}); // Go back to the wall and flip the toggle
	pros::delay(50);
	chassis.moveToPose(0,8,0,800); // Go forward so the toggle can be flipped again
	pros::delay(50);
	chassis.moveToPose(0,-1,0,800, {.forwards = false}); // Go back to the wall and flip the toggle again
	pros::delay(1000);
	chassis.setPose(0, 0, 0);
	pros::delay(50);
	chassis.moveToPose(0,13.5,0,800); // Go to the alliance goal y
	pros::delay(50);
	chassis.turnToHeading(90,500); // Turn to face the alliance goal
	pros::delay(50);
	lift_motors.move_velocity(-30); // Lift the lift so the pin can be scored in the alliance goal
	chassis.moveToPoint(10.5, 13.5, 800); // Move to the alliance goal x while the lift is lifting
	lift_motors.move_velocity(0); // Stop the lift
	pros::delay(50);
	chassis.moveToPoint(20, 13.5, 500); // Move forward to ensure the robot is aligned with the alliance goal
	pros::delay(500);
	piston.set_value(false); // Release the pin into the alliance goal
	pros::delay(200);
	chassis.setPose(0, 0, 0); // Reset the pose as the robot is in an aligned position
	pros::delay(50);
	chassis.moveToPose(0,-24,0,1000, {.forwards = false}); // Back up so that the robot can turn to face the first cup-pin
	chassis.turnToPoint(-18.5,-1,1000); // Turn to face the first cup-pin
	chassis.moveToPoint(-18.5,-1,2000); // Move to the first cup-pin
	pros::delay(600);
	chassis.moveToPoint(-19.8,2.8,2000, {.maxSpeed = 3}); // Move slowly to the first cup-pin so that the robot doesn't knock it over
	pros::delay(500);
	piston.set_value(true); // Grab the cup-pin
	pros::delay(800);
	lift_motors.move_velocity(-200); // Start lifting the cup-pin so it can be scored in the alliance goal
	chassis.moveToPoint(-24,7,1000); // Move to the alliance goal y while the lift is lifting
	pros::delay(300);
	chassis.turnToHeading(90,1000); // Turn to face the alliance goal
	pros::delay(300);
	chassis.moveToPoint(-8,7,1000, {.maxSpeed = 200}); // Move to the alliance goal x while the lift is lifting
	pros::delay(300);
	lift_motors.move_velocity(200); // Start lowering the lift so the cup-pin can be scored in the alliance goal
	pros::delay(300);
	chassis.moveToPoint(-4,7,1000, {.maxSpeed = 80}); // Move forward to ensure the robot is aligned with the alliance goal
	pros::delay(500);
	lift_motors.move_velocity(0); // Stop the lift
	pros::delay(300);
	piston.set_value(false); // Release the cup-pin onto the alliance goal
	pros::delay(300);
	chassis.setPose(0, 0, 0); // Reset the pose as the robot is in an aligned position
	pros::delay(300);
	chassis.moveToPoint(0,-22,1000, {.forwards = false}); // Back up so that the robot can turn to face the next cup-pin
	pros::delay(300);
	move_lift_to(53,1000);
	pros::delay(300);
	chassis.turnToHeading(-45,1000); // Turn to face the next cup-pin
	pros::delay(300);
	chassis.moveToPoint(-20,-2,1000); // Go to the next cup-pin
	pros::delay(300);
	chassis.moveToPoint(-22,0,1000, {.maxSpeed = 3}); // Move slowly to the next cup-pin so that the robot doesn't knock it over
	pros::delay(300);
	piston.set_value(true); // Grab the cup-pin
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

void only_toggle_auton() {
	chassis.setPose(0, 0, 0);
	drive_forward_hard(14, 1000);
	pros::delay(50);
	chassis.moveToPose(0,8,0, 800, {.forwards = false}); // Go forward more to ensure the toggle is sprung
	pros::delay(50);
	drive_forward_hard(5, 1000);
	pros::delay(50);
	chassis.moveToPose(0,-1,0,800, {.forwards = false}); // Go back to the wall and flip the toggle
	pros::delay(50);
	chassis.moveToPose(0,8,0,800); // Go forward so the toggle can be flipped again
	pros::delay(50);
	chassis.moveToPose(0,-1,0,800, {.forwards = false}); // Go back to the wall and flip the toggle again
	pros::delay(50);
	chassis.moveToPose(0,13.5,0,800); // Go to the alliance goal y
}

void placeholder_2_auton() {
	chassis.setPose(0, 0, 0);

}

void placeholder_3_auton() {
	chassis.setPose(0, 0, 0);
}


void placeholder_4_auton() {
	chassis.setPose(0, 0, 0);	
}