#include "game.h"
#include "utils.h"
#include "input.h"
#include "image.h"
#include "stage.h"

#include <cmath>

Game* Game::instance = NULL;

// Initialize stages
stage_bapt* bapt_stage = new stage_bapt();
stage_menu* menu_stage = new stage_menu();
stage_game_over* game_over_stage = new stage_game_over();

// Pick first stage
stage* current_stage = (stage*) menu_stage;

Synth::Sample* intro_song;
Synth::Sample* game_song;
Synth::Sample* water_sound;
Synth::Sample* game_over_sound;
Synth::Sample* transition_sound;
Synth::Sample* correct_sound;


Game::Game(int window_width, int window_height, SDL_Window* window)
{
	this->window_width = window_width;
	this->window_height = window_height;
	this->window = window;
	instance = this;
	must_exit = false;

	fps = 0;
	frame = 0;
	time = 0.0f;
	elapsed_time = 0.0f;
	
	Image* title = Image::Get("data/title_dynamic.tga");
	Image* sprite_water_tank = Image::Get("data/water_tank.tga");
	Image* sprite_babies = Image::Get("data/babies_water.tga");
	Image* sprite_buttons = Image::Get("data/buttons.tga");
	Image* time_bar = Image::Get("data/time_bar.tga");
	Image* game_over_screen = Image::Get("data/game_over.tga");
	Image* priest_arm = Image::Get("data/priest_arm.tga");
	Image* font = Image::Get("data/bitmap-font-white.tga");

	intro_song = synth.loadSample("data/music_title.wav");
	game_song = synth.loadSample("data/music_play_2.wav");
	water_sound = synth.loadSample("data/water.wav");
	game_over_sound = synth.loadSample("data/error.wav");
	transition_sound = synth.loadSample("data/transition.wav");
	correct_sound = synth.loadSample("data/correct.wav");
	

	//enableAudio(); //enable this line if you plan to add audio to your application
	synth.playSample(intro_song, 1.3, true);
	
}


//what to do when the image has to be draw
void Game::render(void)
{
	Image framebuffer(128, 128); //do not change framebuffer size
	current_stage->render(time, framebuffer);
	showFramebuffer(&framebuffer);
}


void Game::update(double seconds_elapsed)
{

	current_stage->update(seconds_elapsed, time);

	if (current_stage == (stage*)bapt_stage)
	{
		if (bapt_stage->is_correct())
		{
			synth.playSample(correct_sound, 1.5, false);
			synth.playSample(water_sound, 1.5, false);
		}
		if (bapt_stage->is_game_over())
		{
			current_stage = (stage*)game_over_stage;
		
			synth = Synth();
			synth.playSample(game_over_sound, 1.5, false);
			synth.playSample(intro_song, 1.3, false);
		}
	}
	else if (current_stage == (stage*)menu_stage)
	{
		if (Input::wasKeyPressed(SDL_SCANCODE_Z))
		{
			current_stage = (stage*)bapt_stage;
			synth = Synth();
			synth.playSample(transition_sound, 2, false);
			synth.playSample(game_song, 1.2, false);
		}	
	}
	else if (current_stage == (stage*)game_over_stage)
	{
		if (Input::wasKeyPressed(SDL_SCANCODE_Z))
		{
			delete bapt_stage; bapt_stage = NULL;
			bapt_stage = new stage_bapt();
			current_stage = (stage*)bapt_stage;
			synth = Synth();
			synth.playSample(transition_sound, 2, false);
			synth.playSample(game_song, 1.3, false);
		}
		if (Input::wasKeyPressed(SDL_SCANCODE_X))
		{
			delete bapt_stage; bapt_stage = NULL;
			bapt_stage = new stage_bapt();
			current_stage = (stage*)menu_stage;
			synth.playSample(transition_sound, 2, false);
		}
	}
	
}

//Keyboard event handler (sync input)
void Game::onKeyDown( SDL_KeyboardEvent event )
{
	switch(event.keysym.sym)
	{
		case SDLK_ESCAPE: must_exit = true; break; //ESC key, kill the app
	}
}

void Game::onKeyUp(SDL_KeyboardEvent event)
{
}

void Game::onGamepadButtonDown(SDL_JoyButtonEvent event)
{

}

void Game::onGamepadButtonUp(SDL_JoyButtonEvent event)
{

}

void Game::onMouseMove(SDL_MouseMotionEvent event)
{
}

void Game::onMouseButtonDown( SDL_MouseButtonEvent event )
{
}

void Game::onMouseButtonUp(SDL_MouseButtonEvent event)
{
}

void Game::onMouseWheel(SDL_MouseWheelEvent event)
{
}

void Game::onResize(int width, int height)
{
    std::cout << "window resized: " << width << "," << height << std::endl;
	glViewport( 0,0, width, height );
	window_width = width;
	window_height = height;
}

//sends the image to the framebuffer of the GPU
void Game::showFramebuffer(Image* img)
{
	static Image finalframe;

	if (window_width < img->width * 4 || window_height < img->height * 4)
	{
		finalframe = *img;
		finalframe.scale( window_width, window_height );
	}
	else
	{
		if (finalframe.width != window_width || finalframe.height != window_height)
		{
			finalframe.resize(window_width, window_height);
			finalframe.fill(Color::BLACK);
		}
		finalframe.drawImage(*img, (window_width - img->width * 4) * 0.5, (window_height - img->height * 4) * 0.5, img->width * 4, img->height * 4);
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	if (1) //flip
	{
		glRasterPos2f(-1, 1);
		glPixelZoom(1, -1);
	}

	glDrawPixels(finalframe.width, finalframe.height, GL_RGBA, GL_UNSIGNED_BYTE, finalframe.pixels);
}

//AUDIO STUFF ********************

SDL_AudioSpec audio_spec;

void AudioCallback(void*  userdata,
	Uint8* stream,
	int    len)
{
	static double audio_time = 0;

	memset(stream, 0, len);//clear
	if (!Game::instance)
		return;

	Game::instance->onAudio((float*)stream, len / sizeof(float), audio_time, audio_spec);
	audio_time += len / (double)audio_spec.freq;
}

void Game::enableAudio()
{
	SDL_memset(&audio_spec, 0, sizeof(audio_spec)); /* or SDL_zero(want) */
	audio_spec.freq = 48000;
	audio_spec.format = AUDIO_F32;
	audio_spec.channels = 1;
	audio_spec.samples = 1024;
	audio_spec.callback = AudioCallback; /* you wrote this function elsewhere. */
	if (SDL_OpenAudio(&audio_spec, &audio_spec) < 0) {
		fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
		exit(-1);
	}
	SDL_PauseAudio(0);
}

void Game::onAudio(float *buffer, unsigned int len, double time, SDL_AudioSpec& audio_spec)
{
	//fill the audio buffer using our custom retro synth
	synth.generateAudio(buffer, len, audio_spec);
}
