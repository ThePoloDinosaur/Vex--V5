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


void draw_pose() {

	lemlib::Pose pose = chassis.getPose();

	char text[64];

	snprintf(text, sizeof(text), "X: %.2f, Y: %.2f, Heading: %.2f", pose.x, pose.y, pose.theta);

	pros::screen::set_pen(GRAY);

	pros::screen::fill_rect(180, 224, 465, 239);

	int text_width = std::strlen(text) * 7;

	pros::screen::set_pen(WHITE);

	pros::screen::print(pros::E_TEXT_SMALL, 460 - text_width, 229, "%s", text);
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

	draw_pose();
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


		pose_loops++;


		if (pose_loops >= 5) {


			pose_loops = 0;


			draw_pose();
		}


		pros::delay(20);
	}
}