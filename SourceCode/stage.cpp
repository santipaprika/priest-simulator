#include "stage.h"
#include "image.h"
#include "input.h"

#include <random>

stage::stage()
{
}
stage::~stage()
{
}
void stage::render(const float& time, Image& framebuffer)
{

}
void stage::update(const double& dt, const float& time)
{
}

int correct;

stage_bapt::stage_bapt()
{
	posX = 0; posY = 128;
	time_arrive = 5000.0;
	returning = false;
	time_return = NULL;
	speed = 300;
	button = -1;

	buttons_side = 20;
	time_available = 3;
	wait_time = time_available;
	time_step = 0.1;
	for (int i=0; i<6; ++i)
		pressed_button[i] = 0;
	bar_position = 0;
	baptize = false;
	game_over = false;
	bapt_position = 0;
	bapt_time_position = 0;
	prev_bapt_time = 0;
	correct = 0;
	dt_sum = 0;
}

//Initialize rng seed
std::random_device rd;  //Will be used to obtain a seed for the random number engine
std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
std::uniform_int_distribution<> dis(0, 5);

Image* sprite_water_tank = Image::Get("data/water_tank.tga");
Image* sprite_babies = Image::Get("data/babies_water.tga");
Image* sprite_buttons = Image::Get("data/buttons.tga");
Image* time_bar = Image::Get("data/time_bar.tga");
Image* priest_arm = Image::Get("data/priest_arm.tga");
Image* font = Image::Get("data/bitmap-font-white.tga");
Image* game_over_screen = Image::Get("data/game_over.tga");

void stage_bapt::render(const float& time, Image& framebuffer)
{
	framebuffer.drawImage(*sprite_water_tank, 0, 0, Area(128 * (int(time * 5) % 2), 0, 128, 128));			//draws a scaled image
	framebuffer.drawImage(*sprite_babies, 0, posY, Area(128, 128 * (time_return != NULL), 128, 128));	//draws only a part of an image
	framebuffer.drawImage(*sprite_buttons, framebuffer.width / 2 - buttons_side / 2 + 2, posY + framebuffer.height / 2 - buttons_side / 2 - 3, Area(20 * button, 20*(time_return != NULL), 20, 20));
	framebuffer.drawImage(*time_bar, bar_position, 0, Area(0, 0, 128, 5));
	bapt_time_position = (int(time * speed / 30) % 3);
	if (bapt_time_position != prev_bapt_time && baptize && bapt_position < 2)
	{
		bapt_position += 1;
		prev_bapt_time = bapt_time_position;
	}
	framebuffer.drawImage(*priest_arm, 6 + 0.5*pow(-1,int(time*5)), 30 + 0.5*pow(-1, int(time * 7)), Area(128 * bapt_position * baptize, 0, 128, 128));
	framebuffer.drawText("Baptize the baby!", 7, 6, *font);				//draws some text using a bitmap font in an image (assuming every char is 7x9)
	framebuffer.drawText(toString(correct), 1, 118, *font);

	//send image to screen
	//return framebuffer;
}

void stage_bapt::update(const double& dt, const float& time)
{
	//float time_real = time * dt * 1000;
	if (game_over)
	{
		return;
	}
	if (posY > 128)
		dt_sum = 0;
	dt_sum += float(dt);
	posY = 128 - position_manager(dt_sum);



	//Read the keyboard state, to see all the keycodes: https://wiki.libsdl.org/SDL_Keycode
	if (Input::wasKeyPressed(SDL_SCANCODE_LEFT) | (Input::gamepads[0].direction & PAD_LEFT)) //if key up
	{
		pressed_button[2] = true;
	}
	if (Input::wasKeyPressed(SDL_SCANCODE_RIGHT) | (Input::gamepads[0].direction & PAD_RIGHT)) //if key down
	{
		pressed_button[3] = true;
	}
	if (Input::wasKeyPressed(SDL_SCANCODE_UP) | (Input::gamepads[0].direction & PAD_UP)) //if key up
	{
		pressed_button[4] = true;
	}
	if (Input::wasKeyPressed(SDL_SCANCODE_DOWN) | (Input::gamepads[0].direction & PAD_DOWN)) //if key down
	{
		pressed_button[5] = true;
	}
	if (Input::wasKeyPressed(SDL_SCANCODE_Z) | Input::gamepads[0].isButtonPressed(A_BUTTON)) //if key A was pressed
	{
		pressed_button[0] = true;
	}
	if (Input::wasKeyPressed(SDL_SCANCODE_X) | Input::gamepads[0].isButtonPressed(B_BUTTON)) //if key Z was pressed
	{
		pressed_button[1] = true;
	}
}

int stage_bapt::random_button()
{
	int rand_button = dis(gen);
	if (rand_button > 1)	// give more chances to 0 and 1 (A and B buttons)
		rand_button = dis(gen)*(rand_button > 1);

	return rand_button;
}

int stage_bapt::position_manager(const float& time)
{
	if (button < 0)
	{
	button = random_button();
	}

	bar_position = int((time_arrive - time) / time_available * 128)*returning*(time_return == NULL);

	if (time_return == NULL && (time - time_arrive > wait_time))
	{
		time_return = time;
	}
	posY = int(time * speed) * !returning + 128 * returning - (time - time_arrive > wait_time)*int((time - time_return) * speed);

	if (pressed_button[button])
	{
		posY = 129;
		pressed_button[button] = false;
		wait_time = 0.0;
		baptize = true;
		correct += 1;
	}

	game_over = true * (time - time_arrive > time_available);
	for (int button_aux = 0; button_aux < 6; button_aux++)
	{
		if (pressed_button[button_aux] && button_aux != button)
			game_over = true;
	}
	

	if (is_correct())
	{
		time_arrive = time;
		returning = true;
		posY = 128;

	}

	if (returning)
	{	
	/*	if (posY < 100)
			baptize = false;*/
		if (posY < 0)
		{
			time_arrive = 500000.0;
			time_return = NULL;
			returning = false;
			button = -1;
			time_available *= (1 - time_step);
			speed *= (1 + time_step);
			wait_time = time_available;
			for (int i = 0; i < 6; i++)
			{
				pressed_button[i] = 0;
			}
			baptize = false;
			bapt_position = 0;
			bapt_time_position = 0;
			prev_bapt_time = 0;
		}
	}

	

	return posY;
}

bool stage_bapt::is_correct()
{
	return posY >= 129;
}

bool stage_bapt::is_game_over()
{
	return game_over;
}

void stage_menu::render(const float& time, Image& framebuffer)
{
	Image* title = Image::Get("data/title_dynamic.tga");
	framebuffer.drawImage(*title, 0, 0, Area(128 * (int(time * 4) % 2), 0, 128, 128));
}

void stage_menu::update(const double& dt, const float& time)
{

}

void stage_game_over::render(const float& time, Image& framebuffer)
{
	
	if (int(time * 4) % 2 == 0)
	{
		framebuffer.drawImage(*sprite_water_tank, 0, 0, Area(0, 0, 128, 128));
		framebuffer.multiplyByColor(Color(50, 50, 50));
		framebuffer.drawImage(*game_over_screen, 0, 0, Area(0, 0, 128, 128));
	}
	if (int(time * 4) % 2 == 1)
	{
		framebuffer.drawImage(*sprite_water_tank, 0, 0, Area(128, 0, 128, 128));
		framebuffer.multiplyByColor(Color(50, 50, 50));
		framebuffer.drawImage(*game_over_screen, 0, 0, Area(128, 0, 128, 128));
	}

	framebuffer.drawText("You baptized ", 22, 56, *font);
	framebuffer.drawText(toString(correct) + " babies!", 34 - 2 * (correct > 9), 68, *font);
	
}

void stage_game_over::update(const double& dt, const float& time)
{

}