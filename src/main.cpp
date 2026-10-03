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
	PID_ANGULAR
};


// Default auton
Auton selected_auton = DRIVERWALL;


bool on_pid_screen = false;


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

const Button driverwall_button = {20, 65, 225, 167};

const Button sidewall_button = {255, 65, 460, 167};

const Button skills_button = {20, 176, 225, 216};

const Button pid_tuning_button = {255, 176, 460, 216};

const Button back_button = {8, 8, 88, 42};

const Button lateral_button = {20, 65, 225, 167};

const Button angular_button = {255, 65, 460, 167};

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
	}


	return "UNKNOWN";
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

	draw_button(pid_tuning_button, selected_auton == PID_LATERAL || selected_auton == PID_ANGULAR ? LIGHT_GRAY : GRAY, LIGHT_GRAY);

	draw_button_text(pid_tuning_button, "PID TUNING");

	// --------------------------------------------------------
	// SELECTED BAR
	// --------------------------------------------------------

	draw_selected_bar();

	// Print current position for debugging
	const auto pose = chassis.getPose();
	pros::screen::set_pen(WHITE);
	pros::screen::print(pros::E_TEXT_SMALL, 10, 220, "X: %.2f, Y: %.2f, Heading: %.2f", pose.x, pose.y, pose.theta);
	
}


void draw_pid_gui() {

	pros::screen::set_pen(BLACK);

	pros::screen::fill_rect(0, 50, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1);

	pros::screen::set_pen(DARK_GRAY);

	pros::screen::fill_rect(0, 0, 479, 49);

	pros::screen::set_pen(WHITE);

	print_centered(pros::E_TEXT_LARGE, 20, 240, 8, "PID TUNING");

	draw_battery();

	draw_button(back_button, GRAY, LIGHT_GRAY);

	draw_button_text(back_button, "< BACK");

	draw_button(lateral_button, selected_auton == PID_LATERAL ? LIGHT_GRAY : GRAY, LIGHT_GRAY);

	draw_button_text(lateral_button, "LATERAL");

	draw_button(angular_button, selected_auton == PID_ANGULAR ? LIGHT_GRAY : GRAY, LIGHT_GRAY);

	draw_button_text(angular_button, "ANGULAR");

	draw_selected_bar();
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


			if (on_pid_screen) {


				if (inside_button(back_button, x, y)) {


					on_pid_screen = false;


					draw_auton_gui();
				}


				else if (inside_button(lateral_button, x, y)) {


					selected_auton = PID_LATERAL;


					draw_pid_gui();
				}


				else if (inside_button(angular_button, x, y)) {


					selected_auton = PID_ANGULAR;


					draw_pid_gui();
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


				else if (inside_button(pid_tuning_button, x, y)) {


					on_pid_screen = true;


					draw_pid_gui();
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


		pros::delay(20);
	}
}

pros::Task* selector_task = nullptr;
// ============================================================
// INITIALIZE
// ============================================================


void initialize() {
	lift_motors.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);

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


		case PID_LATERAL:


			pid_lateral_auton();


			break;


		case PID_ANGULAR:


			pid_angular_auton();


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


			lift_motors.move_velocity(200);
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