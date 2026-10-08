#include "main.h" // This is the main header file for PROS, which includes all necessary libraries and definitions for the robot's operation.
#include "lemlib/api.hpp"
#include "pros/adi.hpp"
#include "pros/imu.hpp"
#include "pros/misc.h"
#include "pros/motor_group.hpp"
#include "pros/rotation.hpp"
#include "pros/rtos.hpp"
#include "pros/screen.hpp"
#include "autons.hpp"
#include <cstring>
#include <cmath>


struct DebugPrint {
	DebugPrint(const char* text) {
		printf("%s\n", text);
	}
};


DebugPrint debug_a("A: program started");

// ============================================================
// ROBOT HARDWARE
// ============================================================


// -------------------- DRIVETRAIN ----------------------------


pros::MotorGroup left_motors({-1, 2, 3}, pros::MotorGearset::blue);


pros::MotorGroup right_motors({11, -12, -13}, pros::MotorGearset::blue);


// -------------------- LIFT ----------------------------------


pros::MotorGroup lift_motors({-9, 14}, pros::MotorGearset::green);

pros::Distance distance_sensor(6);


bool lift_reading_valid(int lift_mm) {

	return lift_mm != PROS_ERR && lift_mm < 9999;
}


void move_lift_to(int target_mm, int timeout_ms) {

	bool going_to_bottom = target_mm <= LIFT_BOTTOM_MM;

	int start_time = pros::millis();

	while (pros::millis() - start_time < timeout_ms) {

		int current_mm = distance_sensor.get();

		if (!lift_reading_valid(current_mm)) {
			break;
		}

		if (going_to_bottom) {

			if (current_mm <= LIFT_BOTTOM_MM) {
				break;
			}

			lift_motors.move_velocity(200);
		}

		else {

			int error = target_mm - current_mm;

			if (std::abs(error) <= LIFT_TOLERANCE_MM) {
				break;
			}

			int speed = std::abs(error) < 30 ? 60 : 200;

			if (error > 0) {
				lift_motors.move_velocity(-speed);
			}

			else {
				lift_motors.move_velocity(speed);
			}	
		}

		pros::delay(20);
	}

	lift_motors.move_velocity(0);
}


// -------------------- PNEUMATIC -----------------------------


pros::adi::Pneumatics piston('H', true);


// -------------------- IMU -----------------------------------


pros::Imu imu(16);


// ----------------------Tracking-Wheels-----------------------
pros::Rotation vertical_sensor(-4);


lemlib::TrackingWheel vertical_tracking_wheel(
	&vertical_sensor,
	lemlib::Omniwheel::NEW_2,0
);


// -------------------- CONTROLLER ----------------------------


pros::Controller controller(pros::E_CONTROLLER_MASTER);


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
	9,   // kP - Lowered significantly to prevent violent launching
	0.001,     // kI
	0,    // kD - Adjusted relative to the new kP
	3,     // anti-windup
	1.5,     // small error range (inches)
	100,   // small error timeout (ms)
	5.5,     // large error range (inches) <-- Increased to give it space to slow down
	500,   // large error timeout (ms)
	20	     // maximum acceleration / slew <-- Lowered to smooth out the initial start
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
	13,     // kD
	3,      // anti-windup
	1,      // small error range (degrees)
	100,    // small error timeout (ms)
	1,      // large error range (degrees)
	500,    // large error timeout (ms)
	0       // maximum acceleration / slew
);


// ============================================================
// LEMLIB DRIVETRAIN
// ============================================================
//
// YOUR ROBOT:
//
// Drive motors:     4 blue motors, 2 half motors
// Drive speed:      450 RPM
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
	&vertical_tracking_wheel,    // vertical tracking wheel 1
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


DebugPrint debug_b("B: hardware setup done");


// ============================================================
// AUTONOMOUS MODES
// ============================================================


enum Auton {
	DRIVERWALL,
	SIDEWALL,
	SKILLS,
	PID_LATERAL,
	PID_ANGULAR,
	ONLY_TOGGLE,
	PLACEHOLDER_2,
	PLACEHOLDER_3,
	PLACEHOLDER_4
};


// Default auton
Auton selected_auton = DRIVERWALL;


bool on_more_screen = false;


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

const Button driverwall_button = {20, 65, 225, 150};

const Button sidewall_button = {255, 65, 460, 150};

const Button skills_button = {20, 158, 225, 198};

const Button more_button = {255, 158, 460, 198};

const Button back_button = {8, 8, 88, 42};

const Button lateral_button = {15, 58, 158, 124};

const Button angular_button = {168, 58, 311, 124};

const Button only_toggle_button = {321, 58, 464, 124};

const Button placeholder_2_button = {15, 132, 158, 198};

const Button placeholder_3_button = {168, 132, 311, 198};

const Button placeholder_4_button = {321, 132, 464, 198};

// ============================================================
// DRAW BUTTON
// ============================================================

void draw_button(const Button& button, int fill_color, int border_color) {


	// Fill
	pros::screen::set_pen(fill_color);

	pros::screen::fill_rect(button.x1, button.y1, button.x2, button.y2);

	// Border
	pros::screen::set_pen(border_color);

	pros::screen::draw_rect(button.x1, button.y1, button.x2, button.y2);
}


void print_centered(pros::text_format_e_t format, int char_width, int center_x, int y, const char* text) {

	int text_width = std::strlen(text) * char_width;

	pros::screen::print(format, center_x - text_width / 2, y, "%s", text);
}


// ============================================================
// DRAW CENTERED BUTTON TEXT
// ============================================================


void draw_button_text(const Button& button, const char* text) {

	int center_x = (button.x1 + button.x2) / 2;

	int center_y = (button.y1 + button.y2) / 2;

	pros::screen::set_pen(WHITE);

	print_centered(pros::E_TEXT_MEDIUM, 10, center_x, center_y - 7, text);
}


// ============================================================
// GET AUTON NAME
// ============================================================


const char* get_auton_name() {


	switch (selected_auton) {


		case DRIVERWALL:
			return "DRIVER WALL";


		case SIDEWALL:
			return "SIDE WALL";


		case SKILLS:
			return "SKILLS";


		case PID_LATERAL:
			return "PID LATERAL";


		case PID_ANGULAR:
			return "PID ANGULAR";


		case ONLY_TOGGLE:
			return "ONLY TOGGLE";


		case PLACEHOLDER_2:
			return "PLACEHOLDER 2";


		case PLACEHOLDER_3:
			return "PLACEHOLDER 3";


		case PLACEHOLDER_4:
			return "PLACEHOLDER 4";
	}


	return "UNKNOWN";
}


bool more_auton_selected() {

	return selected_auton != DRIVERWALL && selected_auton != SIDEWALL && selected_auton != SKILLS;
}


void draw_battery() {


	pros::screen::set_pen(DARK_GRAY);

	pros::screen::fill_rect(405, 10, 479, 40);

	int battery = pros::battery::get_capacity();

	pros::screen::set_pen(LIGHT_GRAY);

	pros::screen::print(pros::E_TEXT_LARGE, 420, 18, "%d%%", battery);
}


void draw_selected_bar() {

	pros::screen::set_pen(GRAY);

	pros::screen::fill_rect(15, 224, 465, 239);

	pros::screen::set_pen(WHITE);

	pros::screen::print(pros::E_TEXT_SMALL, 25, 229, "SELECTED: %s", get_auton_name());
}


void draw_pose() {

	lemlib::Pose pose = chassis.getPose();

	int lift_mm = distance_sensor.get();

	char text[96];

	if (lift_mm >= 9999 || lift_mm == PROS_ERR) {

		snprintf(text, sizeof(text), "X: %.2f, Y: %.2f, Heading: %.2f, Lift: --", pose.x, pose.y, pose.theta);
	}

	else {

		snprintf(text, sizeof(text), "X: %.2f, Y: %.2f, Heading: %.2f, Lift: %d mm", pose.x, pose.y, pose.theta, lift_mm);
	}

	pros::screen::set_pen(GRAY);

	pros::screen::fill_rect(15, 204, 465, 220);

	pros::screen::set_pen(WHITE);

	pros::screen::print(pros::E_TEXT_SMALL, 25, 209, "%s", text);
}


// ============================================================
// DRAW AUTON SELECTOR
// ============================================================


void draw_auton_gui() {


	pros::screen::set_pen(BLACK);

	pros::screen::fill_rect(0, 50, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1);

	// --------------------------------------------------------
	// HEADER
	// --------------------------------------------------------

	pros::screen::set_pen(DARK_GRAY);

	pros::screen::fill_rect(0, 0, 479, 49);

	// Header title
	pros::screen::set_pen(WHITE);

	print_centered(pros::E_TEXT_LARGE, 20, 240, 8, "AUTON SELECTOR");

	// --------------------------------------------------------
	// BATTERY
	// --------------------------------------------------------

	draw_battery();

	// --------------------------------------------------------
	// DRIVER WALL BUTTON
	// --------------------------------------------------------

	draw_button(driverwall_button, selected_auton == DRIVERWALL ? RED : RED_DARK, RED);

	draw_button_text(driverwall_button, "DRIVER WALL");

	// --------------------------------------------------------
	// SIDE WALL BUTTON
	// --------------------------------------------------------

	draw_button(sidewall_button, selected_auton == SIDEWALL ? BLUE : BLUE_DARK, BLUE);

	draw_button_text(sidewall_button, "SIDE WALL");

	// --------------------------------------------------------
	// SKILLS BUTTON
	// --------------------------------------------------------

	draw_button(skills_button, selected_auton == SKILLS ? GREEN : GREEN_DARK, GREEN);

	draw_button_text(skills_button, "SKILLS");

	// --------------------------------------------------------
	// MORE BUTTON
	// --------------------------------------------------------

	draw_button(more_button, more_auton_selected() ? LIGHT_GRAY : GRAY, LIGHT_GRAY);

	draw_button_text(more_button, "MORE");

	// --------------------------------------------------------
	// SELECTED BAR
	// --------------------------------------------------------

	draw_selected_bar();

	draw_pose();
}


void draw_more_button(const Button& button, Auton auton, const char* text) {

	draw_button(button, selected_auton == auton ? LIGHT_GRAY : GRAY, LIGHT_GRAY);

	draw_button_text(button, text);
}


void draw_more_gui() {

	pros::screen::set_pen(BLACK);

	pros::screen::fill_rect(0, 50, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1);

	pros::screen::set_pen(DARK_GRAY);

	pros::screen::fill_rect(0, 0, 479, 49);

	pros::screen::set_pen(WHITE);

	print_centered(pros::E_TEXT_LARGE, 20, 240, 8, "MORE");

	draw_battery();

	draw_button(back_button, GRAY, LIGHT_GRAY);

	draw_button_text(back_button, "< BACK");

	draw_more_button(lateral_button, PID_LATERAL, "PID LATERAL");

	draw_more_button(angular_button, PID_ANGULAR, "PID ANGULAR");

	draw_more_button(only_toggle_button, ONLY_TOGGLE, "ONLY TOGGLE");

	draw_more_button(placeholder_2_button, PLACEHOLDER_2, "PLACEHOLDER 2");

	draw_more_button(placeholder_3_button, PLACEHOLDER_3, "PLACEHOLDER 3");

	draw_more_button(placeholder_4_button, PLACEHOLDER_4, "PLACEHOLDER 4");

	draw_selected_bar();

	draw_pose();
}


// ============================================================
// CHECK BUTTON COLLISION
// ============================================================


bool inside_button(const Button& button, int x, int y) {

	return x >= button.x1 && x <= button.x2 && y >= button.y1 && y <= button.y2;
}


// ============================================================
// AUTON SELECTOR TASK
// ============================================================

void auton_selector_task() {


	int last_press_count = 0;


	int battery_loops = 0;


	int pose_loops = 0;

	pros::delay(100);


	draw_auton_gui();


	while (true) {


		pros::screen_touch_status_s_t touch = pros::screen::touch_status();


		// ----------------------------------------------------
		// HANDLE NEW TOUCH
		// ----------------------------------------------------


		if (touch.touch_status == pros::E_TOUCH_PRESSED && touch.press_count != last_press_count) {


			last_press_count = touch.press_count;


			int x = touch.x;
			int y = touch.y;


			if (on_more_screen) {


				if (inside_button(back_button, x, y)) {


					on_more_screen = false;


					draw_auton_gui();
				}


				else if (inside_button(lateral_button, x, y)) {


					selected_auton = PID_LATERAL;


					draw_more_gui();
				}


				else if (inside_button(angular_button, x, y)) {


					selected_auton = PID_ANGULAR;


					draw_more_gui();
				}


				else if (inside_button(only_toggle_button, x, y)) {


					selected_auton = ONLY_TOGGLE;


					draw_more_gui();
				}


				else if (inside_button(placeholder_2_button, x, y)) {


					selected_auton = PLACEHOLDER_2;


					draw_more_gui();
				}


				else if (inside_button(placeholder_3_button, x, y)) {


					selected_auton = PLACEHOLDER_3;


					draw_more_gui();
				}


				else if (inside_button(placeholder_4_button, x, y)) {


					selected_auton = PLACEHOLDER_4;


					draw_more_gui();
				}
			}


			else {


				// ------------------------------------------------
				// DRIVER WALL
				// ------------------------------------------------


				if (inside_button(driverwall_button, x, y)) {


					selected_auton = DRIVERWALL;


					draw_auton_gui();
				}


				// ------------------------------------------------
				// SIDE WALL
				// ------------------------------------------------


				else if (inside_button(sidewall_button, x, y)) {


					selected_auton = SIDEWALL;


					draw_auton_gui();
				}


				// ------------------------------------------------
				// SKILLS
				// ------------------------------------------------


				else if (inside_button(skills_button, x, y)) {


					selected_auton = SKILLS;


					draw_auton_gui();
				}


				// ------------------------------------------------
				// MORE
				// ------------------------------------------------


				else if (inside_button(more_button, x, y)) {


					on_more_screen = true;


					draw_more_gui();
				}
			}
		}


		// ----------------------------------------------------
		// UPDATE BATTERY DISPLAY
		// ----------------------------------------------------


		battery_loops++;


		if (battery_loops >= 50) {


			battery_loops = 0;


			draw_battery();
		}


		pose_loops++;


		if (pose_loops >= 5) {


			pose_loops = 0;


			draw_pose();
		}


		pros::delay(20);
	}
}

pros::Task* selector_task = nullptr;
// ============================================================
// INITIALIZE
// ============================================================


void initialize() {
	lift_motors.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);

	printf("1: initialize started\n");

	// --------------------------------------------------------
	// Initialize LemLib
	// --------------------------------------------------------

	chassis.calibrate(true);
	vertical_sensor.reset_position();

	printf("2: after calibrate\n");

	// Set screen colors
	pros::screen::set_eraser(BLACK);

	pros::screen::erase();

	// Draw selector
	draw_auton_gui();

	printf("3: gui drawn\n");

	// --------------------------------------------------------
	// Create touchscreen task
	// --------------------------------------------------------

	selector_task = new pros::Task(auton_selector_task);

	printf("4: task started\n");
	printf(pros::Imu(16).is_calibrating() ? "IMU is calibrating\n" : "IMU is not calibrating\n");
	printf("IMU heading: %.2f\n", pros::Imu(16).get_heading());
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
	// GUI
	// --------------------------------------------------------



	// --------------------------------------------------------
	// SELECTED AUTON
	// --------------------------------------------------------


	switch (selected_auton) {


		// ====================================================
		// DRIVER WALL
		// ====================================================


		case DRIVERWALL:


			driverwall_auton();


			break;


		// ====================================================
		// SIDE WALL
		// ====================================================


		case SIDEWALL:


			sidewall_auton();


			break;


		// ====================================================
		// SKILLS
		// ====================================================


		case SKILLS:


			skills_auton();


			break;


		// ====================================================
		// MORE PAGE
		// ====================================================


		case PID_LATERAL:


			pid_lateral_auton();


			break;


		case PID_ANGULAR:


			pid_angular_auton();


			break;


		case ONLY_TOGGLE:


			only_toggle_auton();


			break;


		case PLACEHOLDER_2:


			placeholder_2_auton();


			break;


		case PLACEHOLDER_3:


			placeholder_3_auton();


			break;


		case PLACEHOLDER_4:


			placeholder_4_auton();


			break;
	}

}


// ============================================================
// DRIVER CONTROL
// ============================================================


void opcontrol() {


	printf("5: opcontrol started\n");


	bool piston_last = false;


	while (true) {


		// ====================================================
		// LIFT
		// ====================================================


		if (controller.get_digital(pros::E_CONTROLLER_DIGITAL_L2)) {

			int lift_mm = distance_sensor.get();

			bool override_limit = controller.get_digital(pros::E_CONTROLLER_DIGITAL_RIGHT);

			if (!override_limit && lift_reading_valid(lift_mm) && lift_mm <= LIFT_BOTTOM_MM) {
				lift_motors.move_velocity(0);
			}

			else {
				lift_motors.move_velocity(200);
			}
		}


		else if (controller.get_digital(pros::E_CONTROLLER_DIGITAL_L1)) {
			lift_motors.move_velocity(-200);
		}


		else {
			lift_motors.move_velocity(0);
		}


		// ====================================================
		// ARCADE DRIVE
		// ====================================================


		int leftY = controller.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y);


		int rightX = controller.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_X);


		chassis.arcade(leftY, rightX);


		// ====================================================
		// PNEUMATIC TOGGLE
		// ====================================================


		bool piston_pressed = controller.get_digital(pros::E_CONTROLLER_DIGITAL_B);


		// Toggle only on the initial button press
		if (piston_pressed && !piston_last) {


			piston.toggle();
		}


		piston_last = piston_pressed;


		// ====================================================
		// LOOP DELAY
		// ====================================================


		pros::delay(25);
	}
}