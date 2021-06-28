#pragma once

#include "image.h"

class stage
{
public:
	stage();
	~stage();
	virtual void render(const float& time, Image& framebuffer);
	virtual void update(const double& dt, const float& time);
};

class stage_bapt : public stage
{
	int posX; int posY;
	float time_arrive;
	bool returning;
	float time_return;
	int speed;
	int button;

	int buttons_side;
	float time_available;
	float wait_time;
	float time_step;
	bool pressed_button[6];
	int bar_position;
	bool baptize;
	bool game_over;
	int bapt_position;
	int bapt_time_position;
	int prev_bapt_time;
	float dt_sum;

public : stage_bapt();
	void render(const float& time, Image& framebuffer);
	void update(const double& dt, const float& time);
	int random_button();
	int position_manager(const float& time);
public : bool is_game_over();
		 bool is_correct();
};

class stage_menu : public stage
{
	void render(const float& time, Image& framebuffer);
	void update(const double& dt, const float& time);
};

class stage_game_over : public stage
{
	void render(const float& time, Image& framebuffer);
	void update(const double& dt, const float& time);
};