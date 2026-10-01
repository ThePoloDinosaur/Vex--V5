#include "main.h" // This is the main header file for PROS, which includes all necessary libraries and definitions for the robot's operation.
#include "lemlib/api.hpp"
#include "pros/adi.hpp"
#include "pros/imu.hpp"
#include "pros/misc.h"
#include "pros/motor_group.hpp"
#include "pros/rotation.hpp"
#include "pros/rtos.hpp"
#include "pros/screen.hpp"
#include <algorithm> // No idea what this does, but it was in the original code. I think it has something to do with the clamp function.


// ============================================================
// ROBOT HARDWARE
// ============================================================


// -------------------- DRIVETRAIN ----------------------------


pros::MotorGroup left_motors(
	{-1, 2, 3},
	pros::MotorGearset::blue
);


pros::MotorGroup right_motors(
	{11, -12, -13},
	pros::MotorGearset::blue
);


// -------------------- LIFT ----------------------------------


pros::MotorGroup lift_motors(
	{-9, 14},
	pros::MotorGearset::green
);


// -------------------- PNEUMATIC -----------------------------


pros::adi::Pneumatics piston(
	'H',
	true
);


// -------------------- IMU -----------------------------------


pros::Imu imu(
	(4),
);


// ----------------------Tracking-Wheels-----------------------
pros::Rotation vertical_encoder(4);


// -------------------- CONTROLLER ----------------------------


pros::Controller controller(
	pros::E_CONTROLLER_MASTER
);


// ============================================================
// LEMLIB PID SETTINGS
// ============================================================


// ------------------------------------------------------------
// LATERAL PID
// ------------------------------------------------------------
//
// These are starter values.
// We will tune them based on how your robot actually moves.
//
// kP = how strongly it reacts to distance error
// kI = accumulated error
// kD = how strongly it reacts to rate of change
//
//
//
// Set kP = 2, kI = 0, kD = 5
// Increase kP by 2 each time until it is fast and overshoots
// Increase kD by 5 each time until it doesn't overshoot
// Increase kI by 0.001 if it is barely undershooting
//
// ------------------------------------------------------------


lemlib::ControllerSettings lateral_controller(
	10,     // kP
	0,      // kI
	3,      // kD
	3,      // anti-windup
	1,      // small error range (inches)
	100,    // small error timeout (ms) <-- Adjusted from 10
	3,      // large error range (inches) <-- Re-separated from small error
	500,    // large error timeout (ms) <-- Adjusted from 50
	20      // maximum acceleration / slew
);


// ------------------------------------------------------------
// ANGULAR PID
// ------------------------------------------------------------
//
// Starter values based on LemLib's normal example setup.
//
// kP MUST NOT be 0 if you want useful turning control.
// ------------------------------------------------------------


lemlib::ControllerSettings angular_controller(
	2,      // kP
	0,      // kI
	10,     // kD
	3,      // anti-windup
	1,      // small error range (degrees)
	100,    // small error timeout (ms)
	3,      // large error range (degrees)
	500,    // large error timeout (ms)
	0       // maximum acceleration / slew
);


// ============================================================
// LEMLIB DRIVETRAIN
// ============================================================
//
// YOUR ROBOT:
//
// Drive motors:     6 blue motors
// Drive speed:      600 RPM
// Drive wheels:     2.75" omni wheels
// Track width:      31.7 cm = 12.48 inches
// Tracking wheels:  NONE
// IMU:              NONE
//
// LemLib recommends a horizontal drift of 2 when using omni
// wheels rather than traction wheels.
// ============================================================


lemlib::Drivetrain drivetrain(
	&left_motors,
	&right_motors,


	12.35,                          // track width (inches)


	lemlib::Omniwheel::NEW_275,    // 2.75" omni wheels


	450,                            // drivetrain RPM


	2                               // horizontal drift
);


// ============================================================
// ODOMETRY SENSORS
// ============================================================
//
// So every dedicated sensor is nullptr.
//
// LemLib will use the drivetrain motor encoders for its
// position estimate.
// ============================================================


lemlib::OdomSensors sensors(
	vertical_encoder,    // vertical tracking wheel 1
	nullptr,    // vertical tracking wheel 2
	nullptr,    // horizontal tracking wheel 1
	nullptr,    // horizontal tracking wheel 2
	&imu        // IMU
);


// ============================================================
// LEMLIB CHASSIS
// ============================================================


lemlib::Chassis chassis(
	drivetrain,
	lateral_controller,
	angular_controller,
	sensors
);


// ============================================================
// AUTONOMOUS MODES
// ============================================================


enum Auton {
	RED_LEFT,
	RED_RIGHT,
	BLUE_LEFT,
	BLUE_RIGHT,
	SKILLS
};


// Default auton
Auton selected_auton = RED_LEFT;


// ============================================================
// GUI COLORS
// ============================================================


const int BLACK      = 0x000000;
const int WHITE      = 0xFFFFFF;


const int DARK_GRAY  = 0x202020;
const int GRAY       = 0x404040;
const int LIGHT_GRAY = 0x808080;


const int RED        = 0xE53935;
const int BLUE       = 0x2979FF;
const int GREEN      = 0x32CD75;


const int RED_DARK   = 0x551515;
const int BLUE_DARK  = 0x112C58;
const int GREEN_DARK = 0x164D2C;


// ============================================================
// SCREEN SIZE
// ============================================================


constexpr int SCREEN_WIDTH  = 480;
constexpr int SCREEN_HEIGHT = 240;


// ============================================================
// BUTTON STRUCTURE
// ============================================================


struct Button {
	int x1;
	int y1;
	int x2;
	int y2;
};


// ============================================================
// BUTTON LOCATIONS
// ============================================================


const Button red_left_button = {
	20, 65, 225, 112
};


const Button blue_left_button = {
	255, 65, 460, 112
};


const Button red_right_button = {
	20, 120, 225, 167
};


const Button blue_right_button = {
	255, 120, 460, 167
};


const Button skills_button = {
	95, 176, 385, 216
};


// ============================================================
// DRAW BUTTON
// ============================================================


void draw_button(
	const Button& button,
	int fill_color,
	int border_color
) {


	// Fill
	pros::screen::set_pen(fill_color);


	pros::screen::fill_rect(
		button.x1,
		button.y1,
		button.x2,
		button.y2
	);


	// Border
	pros::screen::set_pen(border_color);


	pros::screen::draw_rect(
		button.x1,
		button.y1,
		button.x2,
		button.y2
	);
}


// ============================================================
// DRAW CENTERED BUTTON TEXT
// ============================================================


void draw_button_text(
	const Button& button,
	const char* text
) {


	int center_x =
		(button.x1 + button.x2) / 2;


	int center_y =
		(button.y1 + button.y2) / 2;


	pros::screen::set_pen(WHITE);


	pros::screen::print(
		pros::E_TEXT_MEDIUM_CENTER,
		center_x,
		center_y - 7,
		"%s",
		text
	);
}


// ============================================================
// GET AUTON NAME
// ============================================================


const char* get_auton_name() {


	switch (selected_auton) {


		case RED_LEFT:
			return "RED LEFT";


		case RED_RIGHT:
			return "RED RIGHT";


		case BLUE_LEFT:
			return "BLUE LEFT";


		case BLUE_RIGHT:
			return "BLUE RIGHT";


		case SKILLS:
			return "SKILLS";
	}


	return "UNKNOWN";
}


// ============================================================
// DRAW AUTON SELECTOR
// ============================================================


void draw_auton_gui() {


	// --------------------------------------------------------
	// BACKGROUND
	// --------------------------------------------------------


	pros::screen::set_pen(BLACK);


	pros::screen::fill_rect(
		0,
		0,
		SCREEN_WIDTH - 1,
		SCREEN_HEIGHT - 1
	);


	// --------------------------------------------------------
	// HEADER
	// --------------------------------------------------------


	pros::screen::set_pen(DARK_GRAY);


	pros::screen::fill_rect(
		0,
		0,
		479,
		49
	);


	// Header title
	pros::screen::set_pen(WHITE);


	pros::screen::print(
		pros::E_TEXT_LARGE_CENTER,
		240,
		8,
		"AUTON SELECTOR"
	);


	// --------------------------------------------------------
	// BATTERY
	// --------------------------------------------------------


	int battery =
		pros::battery::get_capacity();


	pros::screen::set_pen(LIGHT_GRAY);


	pros::screen::print(
		pros::E_TEXT_SMALL,
		420,
		18,
		"%d%%",
		battery
	);


	// --------------------------------------------------------
	// RED LEFT BUTTON
	// --------------------------------------------------------


	draw_button(
		red_left_button,


		selected_auton == RED_LEFT
			? RED
			: RED_DARK,


		RED
	);


	draw_button_text(
		red_left_button,
		"RED LEFT"
	);


	// --------------------------------------------------------
	// BLUE LEFT BUTTON
	// --------------------------------------------------------


	draw_button(
		blue_left_button,


		selected_auton == BLUE_LEFT
			? BLUE
			: BLUE_DARK,


		BLUE
	);


	draw_button_text(
		blue_left_button,
		"BLUE LEFT"
	);


	// --------------------------------------------------------
	// RED RIGHT BUTTON
	// --------------------------------------------------------


	draw_button(
		red_right_button,


		selected_auton == RED_RIGHT
			? RED
			: RED_DARK,


		RED
	);


	draw_button_text(
		red_right_button,
		"RED RIGHT"
	);


	// --------------------------------------------------------
	// BLUE RIGHT BUTTON
	// --------------------------------------------------------


	draw_button(
		blue_right_button,


		selected_auton == BLUE_RIGHT
			? BLUE
			: BLUE_DARK,


		BLUE
	);


	draw_button_text(
		blue_right_button,
		"BLUE RIGHT"
	);


	// --------------------------------------------------------
	// SKILLS BUTTON
	// --------------------------------------------------------


	draw_button(
		skills_button,


		selected_auton == SKILLS
			? GREEN
			: GREEN_DARK,


		GREEN
	);


	draw_button_text(
		skills_button,
		"SKILLS"
	);


	// --------------------------------------------------------
	// SELECTED BAR
	// --------------------------------------------------------


	pros::screen::set_pen(GRAY);


	pros::screen::fill_rect(
		15,
		224,
		465,
		239
	);


	pros::screen::set_pen(WHITE);


	pros::screen::print(
		pros::E_TEXT_SMALL,
		25,
		229,
		"SELECTED: %s",
		get_auton_name()
	);
}


// ============================================================
// CHECK BUTTON COLLISION
// ============================================================


bool inside_button(
	const Button& button,
	int x,
	int y
) {


	return (
		x >= button.x1 &&
		x <= button.x2 &&
		y >= button.y1 &&
		y <= button.y2
	);
}


// ============================================================
// AUTON SELECTOR TASK
// ============================================================


void auton_selector_task() {


	int last_press_count = 0;


	while (true) {


		pros::screen_touch_status_s_t touch =
			pros::screen::touch_status();


		// ----------------------------------------------------
		// HANDLE NEW TOUCH
		// ----------------------------------------------------


		if (
			touch.touch_status == pros::E_TOUCH_PRESSED &&
			touch.press_count != last_press_count
		) {


			last_press_count =
				touch.press_count;


			int x = touch.x;
			int y = touch.y;


			// ------------------------------------------------
			// RED LEFT
			// ------------------------------------------------


			if (
				inside_button(
					red_left_button,
					x,
					y
				)
			) {


				selected_auton = RED_LEFT;


				draw_auton_gui();
			}


			// ------------------------------------------------
			// BLUE LEFT
			// ------------------------------------------------


			else if (
				inside_button(
					blue_left_button,
					x,
					y
				)
			) {


				selected_auton = BLUE_LEFT;


				draw_auton_gui();
			}


			// ------------------------------------------------
			// RED RIGHT
			// ------------------------------------------------


			else if (
				inside_button(
					red_right_button,
					x,
					y
				)
			) {


				selected_auton = RED_RIGHT;


				draw_auton_gui();
			}


			// ------------------------------------------------
			// BLUE RIGHT
			// ------------------------------------------------


			else if (
				inside_button(
					blue_right_button,
					x,
					y
				)
			) {


				selected_auton = BLUE_RIGHT;


				draw_auton_gui();
			}


			// ------------------------------------------------
			// SKILLS
			// ------------------------------------------------


			else if (
				inside_button(
					skills_button,
					x,
					y
				)
			) {


				selected_auton = SKILLS;


				draw_auton_gui();
			}
		}


		// ----------------------------------------------------
		// UPDATE BATTERY DISPLAY
		// ----------------------------------------------------


		draw_auton_gui();


		pros::delay(250);
	}
}


// ============================================================
// INITIALIZE
// ============================================================


void initialize() {


	// Set screen colors
	pros::screen::set_eraser(BLACK);


	pros::screen::erase();


	// Draw selector
	draw_auton_gui();


	// --------------------------------------------------------
	// Create touchscreen task
	// --------------------------------------------------------


	static pros::Task selector_task(
		auton_selector_task


		
	);


	// --------------------------------------------------------
	// Initialize LemLib
	// --------------------------------------------------------
	//
	// You have no IMU, so there is nothing to calibrate there.
	// This still initializes the chassis/odometry system.
	//
	chassis.calibrate(false);
}


// ============================================================
// DISABLED
// ============================================================


void disabled() {


	left_motors.move(0);
	right_motors.move(0);
	lift_motors.move(0);
}


// ============================================================
// AUTONOMOUS
// ============================================================


void autonomous() {


	// --------------------------------------------------------
	// Set starting position
	// --------------------------------------------------------

	


/*
	// --------------------------------------------------------
	// SELECTED AUTON
	// --------------------------------------------------------


	switch (selected_auton) {


		// ====================================================
		// RED LEFT
		// ====================================================


		case RED_LEFT:


			// TEST:
			// Drive forward 24 inches


			chassis.turnToHeading(90, 100000);


			break;


		// ====================================================
		// RED RIGHT
		// ====================================================


		case RED_RIGHT:


			// TEST:
			// Drive forward 24 inches




			break;


		// ====================================================
		// BLUE LEFT
		// ====================================================


		case BLUE_LEFT:


			// TEST:
			// Drive forward 24 inches


			chassis.turnToHeading(90, 100000);


			break;


		// ====================================================
		// BLUE RIGHT
		// ====================================================


		case BLUE_RIGHT:


			// TEST:
			// Drive forward 24 inches


			chassis.moveToPoint(
				0,
				24,
				3000
			);


			break;


		// ====================================================
		// SKILLS
		// ====================================================


		case SKILLS:


			// TEST:
			// Drive forward 24 inches


			chassis.moveToPoint(
				0,
				24,
				3000
			);


			break;
	}
	*/
}


// ============================================================
// DRIVER CONTROL
// ============================================================


void opcontrol() {


	bool piston_last = false;


	while (true) {


		// ====================================================
		// LIFT
		// ====================================================


		if (
			controller.get_digital(
				pros::E_CONTROLLER_DIGITAL_L2
			)
		) {


			lift_motors.move_velocity(200);
		}


		else if (
			controller.get_digital(
				pros::E_CONTROLLER_DIGITAL_L1
			)
		) {
			lift_motors.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
			lift_motors.move_velocity(-200);
		}


		else {


			lift_motors.move_velocity(0);
			lift_motors.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
		}


		// ====================================================
		// ARCADE DRIVE
		// ====================================================


		int leftY =
			controller.get_analog(
				pros::E_CONTROLLER_ANALOG_LEFT_Y
			);


		int rightX =
			controller.get_analog(
				pros::E_CONTROLLER_ANALOG_RIGHT_X
			);


		chassis.arcadeDrive(
			leftY,
			rightX
		);


		// ====================================================
		// PNEUMATIC TOGGLE
		// ====================================================


		bool piston_pressed =
			controller.get_digital(
				pros::E_CONTROLLER_DIGITAL_B
			);


		// Toggle only on the initial button press
		if (
			piston_pressed &&
			!piston_last
		) {


			piston.toggle();
		}


		piston_last =
			piston_pressed;


		// ====================================================
		// LOOP DELAY
		// ====================================================


		pros::delay(25);
	}
}
