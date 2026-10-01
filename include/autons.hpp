#pragma once
#include "lemlib/api.hpp"

extern lemlib::Chassis chassis;
extern pros::MotorGroup lift_motors;
extern pros::adi::Pneumatics piston;

void driverwall_auton();
void sidewall_auton();
void skills_auton();
void pid_lateral_auton();
void pid_angular_auton();