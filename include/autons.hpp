#pragma once
#include "lemlib/api.hpp"


extern lemlib::Chassis chassis;
extern pros::MotorGroup lift_motors;
extern pros::adi::Pneumatics piston;


const int LIFT_BOTTOM_MM = 66;
const int LIFT_TOLERANCE_MM = 3;


void move_lift_to(int target_mm, int timeout_ms);


void driverwall_auton();
void sidewall_auton();
void skills_auton();
void pid_lateral_auton();
void pid_angular_auton();