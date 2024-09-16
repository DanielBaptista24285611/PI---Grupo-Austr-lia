#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
//#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/keyboard.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>


//infos, sprite idle soldado!
// largura 896 / 7 = 128;
// altura 128 / 1 = 128;


float FPS = 20.0;
int main() {

	al_init();
	al_init_font_addon();
	//al_init_ttf_addon();
	al_init_image_addon();
	al_install_keyboard();
	al_install_audio();
	al_init_acodec_addon();


	ALLEGRO_DISPLAY* display = al_create_display(1280, 720);
	al_set_window_position(display, 200, 200);

	ALLEGRO_FONT* font = al_create_builtin_font();
	ALLEGRO_TIMER* timer = al_create_timer(1.0 / FPS);

	//IMAGENS
	ALLEGRO_BITMAP* walkright = al_load_bitmap("./walkright.png");
	ALLEGRO_BITMAP* walkleft = al_load_bitmap("./walkleft.png");
	ALLEGRO_BITMAP* background = al_load_bitmap("./backgroundfloresta.jpeg");
	ALLEGRO_BITMAP* jump_image = al_load_bitmap("./jump.png");
	ALLEGRO_BITMAP* idle = al_load_bitmap("./idle.png");
	ALLEGRO_BITMAP* shot_image = al_load_bitmap("./shot_1.png");
	//ALLEGRO_BITMAP* tronco = al_load_bitmap("./tronco-de-madeira.png");
    //ALLEGRO_BITMAP* tronco2 = al_load_bitmap("./tronco2.png");
	ALLEGRO_BITMAP* tronco3 = al_load_bitmap("./wood_spike.png");


	//AUDIOS
	ALLEGRO_SAMPLE* tiro = al_load_sample("./metralhadora.mp3");

	//FILAS DE EVENTOS
	ALLEGRO_EVENT_QUEUE* event_queue = al_create_event_queue();
	al_register_event_source(event_queue, al_get_display_event_source(display));
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_register_event_source(event_queue, al_get_keyboard_event_source());
	al_start_timer(timer);



	//Movimentações personagem soldado, X e Y.
	//int image1 = walkright;
	int pulo = 0;
	int velocy = 0;
	int velocup = 15;
	int plimit = 0;
	float frame = 0.f;
	float frame2 = 0.f;
	float frameshot = 0.f;
	float frameidle = 0.f;

	int pos_x = 0, pos_y = 380;
	int current_frame_y = 0;
		
	int pos_x2 = 0, pos_y2 = 0;
	int current_frame_y2 = 0;
	bool moving_left = false;
	bool mov = false;
	//al_draw_rectangle(200.0, 200.0, 100.0, 100.0, al_map_rgb(0, 255, 0), 6.0);
	typedef enum {
		WALKING_RIGHT,
		WALKING_LEFT,
		WALKING_SHOT,
		JUMPING,
		IDLE
	}characterState;

	characterState jump_state = WALKING_RIGHT; //Manter estado atual

	while (true) {
		al_clear_to_color(al_map_rgb(255, 255, 255));
		al_draw_bitmap(background, 0, 0, 0);
		al_draw_bitmap(tronco3, 400, 516, 0);
		ALLEGRO_EVENT event;
		al_wait_for_event(event_queue, &event);
		if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			break;
		}
		else if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) return false;
		else if (event.keyboard.keycode == ALLEGRO_KEY_RIGHT) {
			pos_x += 5;
			jump_state = WALKING_RIGHT;
			moving_left = false;
			mov = true;
		}
		else if (event.keyboard.keycode == ALLEGRO_KEY_LEFT) {
			pos_x -= 5; jump_state = WALKING_LEFT;
			moving_left = true;
			mov = true;
		}
		else if (event.keyboard.keycode == ALLEGRO_KEY_B) {
			jump_state = WALKING_SHOT;
			moving_left = false;
			mov = true;
			//ADICIONAR SOM DO TIRO!!!	
			//al_play_sample(tiro, 1, 0, 1, ALLEGRO_PLAYMODE_LOOP, NULL);
		}

		else if (event.keyboard.keycode == ALLEGRO_KEY_SPACE && !pulo) {
			pulo = 1;
			velocy = -velocup;
			plimit = pos_y - 140; //altura do pulo
			jump_state = JUMPING;
			mov = true;

		}
		else  if (mov == false) {
			jump_state = IDLE;
		}

		if (pulo) {
			pos_y += velocy;
			velocy += 1;

			if (pos_y >= 380) {
				pos_y = 380;
				pulo = 0;
				velocy = 0;
				//Parar de pular ao encostar no chão
				if (moving_left) jump_state = WALKING_LEFT;
				else jump_state = WALKING_RIGHT;
			}
		}
		frame += 0.7f;
		if (frame > 7) {
			frame -= 7;
		}
		frame2 += 0.7f;
		if (frame2 > 7) {
			frame2 -= 7;
		}
		frameshot += 0.4f;
		if (frameshot > 4) {
			frameshot -= 4;
		}
		frameidle += 0.7f;
		if (frameidle > 7) {
			frameidle -= 7;
		}
		//al_draw_text(font, al_map_rgb(0, 0, 0), 230, 200, 0, "Hello World!");



if (pos_x + 75 > 320 && pos_x < 350 && pos_y + 128 > pos_y && pos_y < pos_y + 100) {
	al_draw_text(font, al_map_rgb(0, 0, 0), 640, 360, ALLEGRO_ALIGN_CENTER, "Teste");
}

		switch (jump_state) {
		case WALKING_RIGHT:
			al_draw_bitmap_region(walkright, 256 * (int)frame, current_frame_y, 256, 256, pos_x, pos_y, 0);
			break;

		case WALKING_LEFT:
			al_draw_bitmap_region(walkleft, 256 * (int)frame, current_frame_y, 256, 256, pos_x, pos_y, 0);
			break;
		case WALKING_SHOT:
			al_draw_bitmap_region(shot_image, 256 * (int)frameshot, current_frame_y, 256, 256, pos_x, pos_y, 0);
			break;

		case JUMPING:if (moving_left) {
			al_draw_bitmap_region(jump_image, 256 * (int)frame, current_frame_y, 256, 256, pos_x,pos_y, ALLEGRO_FLIP_HORIZONTAL);
			mov = true;
			//Inverter imagem do pulo
		}
					else{
			al_draw_bitmap_region(jump_image, 256 * (int)frame2, current_frame_y, 256, 256, pos_x,pos_y, 0);
			mov = true;

		}
					break;

		case IDLE:
			al_draw_bitmap_region(idle, 256 * (int)frameidle, current_frame_y, 256, 256, pos_x, pos_y, 0);
			break;
		default:
			break;
		}

		al_flip_display();
	}
	//Destroi imagens
	al_destroy_bitmap(background);
	al_destroy_bitmap(idle);
	al_destroy_bitmap(shot_image);
	al_destroy_bitmap(walkright);
	al_destroy_bitmap(walkleft);
	al_destroy_bitmap(jump_image);
	//Destroi audios
	al_destroy_sample(tiro);
	//destroi fonts,eventos e tela
	al_destroy_font(font);
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);

	return 0;

}
