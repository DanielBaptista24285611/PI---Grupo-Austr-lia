#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/keyboard.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>


//infos, sprite idle soldado!
// largura 896 / 7 = 128;
// altura 128 / 1 = 128;
float FPS = 20.0;
//bool controleDeTelas1 = true;
int tela2() {

	al_init();
	al_init_font_addon();
	al_init_ttf_addon();
	al_init_image_addon();
	al_install_keyboard();
	al_install_audio();
	al_init_acodec_addon();


	ALLEGRO_DISPLAY* display1 = al_create_display(1280, 720);
	al_set_window_position(display1, 0, 0);

	//ALLEGRO_FONT* font = al_create_builtin_font();
	ALLEGRO_FONT* font = al_load_font("./fonte/font.ttf", 60, 0);
	ALLEGRO_TIMER* timer = al_create_timer(1.0 / FPS);

	//IMAGENS
	//Sprites Soldado
	ALLEGRO_BITMAP* walkright = al_load_bitmap("./soldiersprites1/Soldier_1/walkright.png");
	ALLEGRO_BITMAP* walkleft = al_load_bitmap("./soldiersprites1/Soldier_1/walkleft.png");
	ALLEGRO_BITMAP* jump_image = al_load_bitmap("./soldiersprites1/Soldier_1/jump.png");
	ALLEGRO_BITMAP* idle = al_load_bitmap("./soldiersprites1/Soldier_1/idle.png");
	ALLEGRO_BITMAP* shot_image_right = al_load_bitmap("./soldiersprites1/Soldier_1/shot_1_right.png");
	ALLEGRO_BITMAP* shot_image_left = al_load_bitmap("./soldiersprites1/Soldier_1/shot_1_left.png");
	ALLEGRO_BITMAP* pegar_image = al_load_bitmap("./soldiersprites2/soldier_2/rest.png");
	ALLEGRO_BITMAP* bullet_right = al_load_bitmap("./soldiersprites1/teste/bullet_right.png");
	ALLEGRO_BITMAP* bullet_left = al_load_bitmap("./soldiersprites1/teste/bullet_left.png");
	//Backgrounds
	ALLEGRO_BITMAP* background_2 = al_load_bitmap("./backgrounds/backgroundfloresta2.jpg");
	//Obstáculos
	ALLEGRO_BITMAP* tronco3 = al_load_bitmap("./Obstaculos/wood_spike.png");
	//Consumíveis
	ALLEGRO_BITMAP* kitmedico = al_load_bitmap("./Consumiveis/Kitmedico1.png");
	ALLEGRO_BITMAP* kitmunicao = al_load_bitmap("./consumiveis/kitmunicao1.png");


	//AUDIOS
	//Soldado
	ALLEGRO_SAMPLE* tiro = al_load_sample("./Audios/metralhadora.mp3");

	//FILAS DE EVENTOS
	ALLEGRO_EVENT_QUEUE* event_queue = al_create_event_queue();
	al_register_event_source(event_queue, al_get_display_event_source(display1));
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_register_event_source(event_queue, al_get_keyboard_event_source());
	al_start_timer(timer);



	//Movimentações personagem soldado, X e Y.
	//int image1 = walkright;
	int posicao = 0;
	int pulo = 0;
	int velocy = 0;
	int velocup = 15;
	int plimit = 0;
	float frame = 0.f;
	float frame2 = 0.f;
	float frame4px = 0.f;
	float frameidle = 0.f;
	float frameright = 0.f;

	//Variáveis dos consumíveis
	int kitmedicoX = 600;
	int kitmunicaoX = 680;



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
		WALKING_SHOT_RIGHT,
		WALKING_SHOT_LEFT,
		JUMPING,
		IDLE,
		PEGAR
	}characterState;

	characterState jump_state = WALKING_RIGHT; //Manter estado atual

	while (true) {
		al_clear_to_color(al_map_rgb(255, 255, 255));
		//Backgrounds
		al_draw_bitmap(background_2, 0, 0, 0);
		//Obstáculos
		al_draw_bitmap(tronco3, 400, 516, 0);
		//Consumíveis
		al_draw_bitmap(kitmedico, kitmedicoX, 600, 0);
		al_draw_bitmap(kitmunicao, kitmunicaoX, 600, 0);

		ALLEGRO_EVENT event;
		al_wait_for_event(event_queue, &event);
		if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			break;
		}

		else if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
			return false;

		else if (event.keyboard.keycode == ALLEGRO_KEY_SPACE && !pulo) {
			pulo = 1;
			velocy = -velocup;
			plimit = pos_y - 140; //altura do pulo
			jump_state = JUMPING;
			mov = true;

		}
		else  if (mov == false) {
			jump_state = IDLE;
			posicao == 1;
		}

		switch (event.keyboard.keycode) {
		case ALLEGRO_KEY_LEFT:
			pos_x -= 5; jump_state = WALKING_LEFT;
			moving_left = true;
			mov = true;
			break;
		case ALLEGRO_KEY_RIGHT:
			pos_x += 5;
			jump_state = WALKING_RIGHT;
			moving_left = false;
			mov = true;
			break;
		case ALLEGRO_KEY_V:
			jump_state = PEGAR;
			if (pos_x > 465 && pos_x < 530 && pos_y + 128 > pos_y && pos_y < pos_y + 100) {

				kitmedicoX = -1000;

			}
			else if (pos_x > 560 && pos_x < 670 && pos_y + 128 > pos_y && pos_y < pos_y + 100) {
				kitmunicaoX = -1000;
			}
			//al_draw_bitmap(kitmedico, 600, 600, 0);

			//al_draw_bitmap(kitmunicao, 680, 600, 0);
			break;
		case ALLEGRO_KEY_B:
			if (moving_left == true) {
				jump_state = WALKING_SHOT_LEFT;
			}
			else {
				jump_state = WALKING_SHOT_RIGHT;
			}
			break;
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
		frame4px += 0.4f;
		if (frame4px > 4) {
			frame4px -= 4;
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

		case WALKING_SHOT_RIGHT:
			al_draw_bitmap_region(shot_image_right, 256 * (int)frame4px, current_frame_y, 256, 256, pos_x, pos_y, 0);
			break;

		case WALKING_SHOT_LEFT:
			al_draw_bitmap_region(shot_image_left, 256 * (int)frame4px, current_frame_y, 256, 256, pos_x, pos_y, 0);
			break;

		case JUMPING:if (moving_left) {
			al_draw_bitmap_region(jump_image, 256 * (int)frame, current_frame_y, 256, 256, pos_x, pos_y, ALLEGRO_FLIP_HORIZONTAL);
			mov = true;
			//Inverter imagem do pulo
		}
					else {
			al_draw_bitmap_region(jump_image, 256 * (int)frame2, current_frame_y, 256, 256, pos_x, pos_y, 0);
			mov = true;

		}
					break;
		case PEGAR:
			al_draw_bitmap_region(pegar_image, 256 * (int)frame4px, current_frame_y, 256, 256, pos_x, pos_y, 0);
			break;

		case IDLE:
			al_draw_bitmap_region(idle, 256 * (int)frameidle, current_frame_y, 256, 256, pos_x, pos_y, 0);
			break;
		default:
			break;
		}
		//Mudança de tela

		if (pos_x > 1280) {
			tela2();

		}


		al_flip_display();
	}
	//Destroi imagens
	//Sprites Soldado
	al_destroy_bitmap(walkright);
	al_destroy_bitmap(walkleft);
	al_destroy_bitmap(idle);
	al_destroy_bitmap(shot_image_right);
	al_destroy_bitmap(shot_image_left);
	al_destroy_bitmap(jump_image);
	al_destroy_bitmap(pegar_image);
	al_destroy_bitmap(bullet_right);
	al_destroy_bitmap(bullet_left);
	//Backgrounds
	al_destroy_bitmap(background_2);
	//Obstáculos
	al_destroy_bitmap(tronco3);
	//Consumíveis
	al_destroy_bitmap(kitmedico);
	al_destroy_bitmap(kitmunicao);

	//Destroi audios
	//Soldado
	al_destroy_sample(tiro);
	//destroi fonts,eventos e tela
	al_destroy_font(font);
	al_destroy_display(display1);
	al_destroy_event_queue(event_queue);

	return 0;

}

int tela1() {

	al_init();
	al_init_font_addon();
	al_init_ttf_addon();
	al_init_image_addon();
	al_install_keyboard();
	al_install_audio();
	al_init_acodec_addon();


	ALLEGRO_DISPLAY* display1 = al_create_display(1280, 720);
	al_set_window_position(display1, 0, 0);

	//ALLEGRO_FONT* font = al_create_builtin_font();
	ALLEGRO_FONT* font = al_load_font("./fonte/font.ttf", 60, 0);
	ALLEGRO_TIMER* timer = al_create_timer(1.0 / FPS);

	//IMAGENS
	//Sprites Soldado
	ALLEGRO_BITMAP* walkright = al_load_bitmap("./soldiersprites1/Soldier_1/walkright.png");
	ALLEGRO_BITMAP* walkleft = al_load_bitmap("./soldiersprites1/Soldier_1/walkleft.png");
	ALLEGRO_BITMAP* jump_image = al_load_bitmap("./soldiersprites1/Soldier_1/jump.png");
	ALLEGRO_BITMAP* idle = al_load_bitmap("./soldiersprites1/Soldier_1/idle.png");
	ALLEGRO_BITMAP* shot_image_right = al_load_bitmap("./soldiersprites1/Soldier_1/shot_1_right.png");
	ALLEGRO_BITMAP* shot_image_left = al_load_bitmap("./soldiersprites1/Soldier_1/shot_1_left.png");
	ALLEGRO_BITMAP* pegar_image = al_load_bitmap("./soldiersprites2/soldier_2/rest.png");
	ALLEGRO_BITMAP* bullet_right = al_load_bitmap("./soldiersprites1/teste/bullet_right.png");
	ALLEGRO_BITMAP* bullet_left = al_load_bitmap("./soldiersprites1/teste/bullet_left.png");
	//Backgrounds
	ALLEGRO_BITMAP* background = al_load_bitmap("./backgrounds/backgroundfloresta.jpeg");
	//Obstáculos
	ALLEGRO_BITMAP* tronco3 = al_load_bitmap("./Obstaculos/wood_spike.png");
	//Consumíveis
	ALLEGRO_BITMAP* kitmedico = al_load_bitmap("./Consumiveis/Kitmedico1.png");
	ALLEGRO_BITMAP* kitmunicao = al_load_bitmap("./consumiveis/kitmunicao1.png");


	//AUDIOS
	//Soldado
	ALLEGRO_SAMPLE* tiro = al_load_sample("./Audios/metralhadora.mp3");

	//FILAS DE EVENTOS
	ALLEGRO_EVENT_QUEUE* event_queue = al_create_event_queue();
	al_register_event_source(event_queue, al_get_display_event_source(display1));
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_register_event_source(event_queue, al_get_keyboard_event_source());
	al_start_timer(timer);



	//Movimentações personagem soldado, X e Y.
	//int image1 = walkright;
	int posicao = 0;
	int pulo = 0;
	int velocy = 0;
	int velocup = 15;
	int plimit = 0;
	float frame = 0.f;
	float frame2 = 0.f;
	float frame4px = 0.f;
	float frameidle = 0.f;
	float frameright = 0.f;
	float pos_x = 0.1, pos_y = 380;
	int current_frame_y = 0;
	bool atirando = false;
	float pos_x_bullet = pos_x;
	//Variáveis dos consumíveis
	int kitmedicoX = 600;
	int kitmunicaoX = 680;

	int pos_x2 = 0, pos_y2 = 0;
	int current_frame_y2 = 0;
	bool moving_left = false;
	bool mov = false;
	//al_draw_rectangle(200.0, 200.0, 100.0, 100.0, al_map_rgb(0, 255, 0), 6.0);
	typedef enum {
		WALKING_RIGHT,
		WALKING_LEFT,
		WALKING_SHOT_RIGHT,
		WALKING_SHOT_LEFT,
		JUMPING,
		IDLE,
		PEGAR
	}characterState;

	characterState jump_state = WALKING_RIGHT; //Manter estado atual

	while (true) {
		al_clear_to_color(al_map_rgb(255, 255, 255));
		//Backgrounds
		al_draw_bitmap(background, 0, 0, 0);
		//Obstáculos
		al_draw_bitmap(tronco3, 400, 516, 0);
		//Consumíveis
		al_draw_bitmap(kitmedico, kitmedicoX, 600, 0);
		al_draw_bitmap(kitmunicao, kitmunicaoX, 600, 0);

		ALLEGRO_EVENT event;
		al_wait_for_event(event_queue, &event);
		if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			break;
		}

		else if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
			return false;
				
		else if (event.keyboard.keycode == ALLEGRO_KEY_SPACE && !pulo) {
			pulo = 1;
			velocy = -velocup;
			plimit = pos_y - 140; //altura do pulo
			jump_state = JUMPING;
			mov = true;

		}
		else  if (mov == false) {
			jump_state = IDLE;
			posicao == 1;
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
		frame4px += 0.4f;
		if (frame4px > 4) {
			frame4px -= 4;
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

		case WALKING_SHOT_RIGHT:
			al_draw_bitmap_region(shot_image_right, 256 * (int)frame4px, current_frame_y, 256, 256, pos_x, pos_y, 0);
			break;

		case WALKING_SHOT_LEFT:
			al_draw_bitmap_region(shot_image_left, 256 * (int)frame4px, current_frame_y, 256, 256, pos_x, pos_y, 0);
			break;

		case JUMPING:if (moving_left) {
			al_draw_bitmap_region(jump_image, 256 * (int)frame, current_frame_y, 256, 256, pos_x, pos_y, ALLEGRO_FLIP_HORIZONTAL);
			mov = true;
			//Inverter imagem do pulo
		}
					else {
			al_draw_bitmap_region(jump_image, 256 * (int)frame2, current_frame_y, 256, 256, pos_x, pos_y, 0);
			mov = true;

		}
					break;
		case PEGAR:
			al_draw_bitmap_region(pegar_image, 256 * (int)frame4px, current_frame_y, 256, 256, pos_x, pos_y, 0);
			break;

		case IDLE:
			al_draw_bitmap_region(idle, 256 * (int)frameidle, current_frame_y, 256, 256, pos_x, pos_y, 0);
			break;
		default:
			break;
		}

		switch (event.keyboard.keycode) {
		case ALLEGRO_KEY_LEFT:
			pos_x -= 5; jump_state = WALKING_LEFT;
			moving_left = true;
			mov = true;
			break;
		case ALLEGRO_KEY_RIGHT:
			pos_x += 5;
			jump_state = WALKING_RIGHT;
			moving_left = false;
			mov = true;
			break;
		case ALLEGRO_KEY_V:
			jump_state = PEGAR;
			if (pos_x > 465 && pos_x < 530 && pos_y + 128 > pos_y && pos_y < pos_y + 100) {

				kitmedicoX = -1000;

			}
			else if (pos_x > 560 && pos_x < 670 && pos_y + 128 > pos_y && pos_y < pos_y + 100) {
				kitmunicaoX = -1000;
			}
			//al_draw_bitmap(kitmedico, 600, 600, 0);

			//al_draw_bitmap(kitmunicao, 680, 600, 0);
			break;
		case ALLEGRO_KEY_B:
			if (moving_left == true) {
				jump_state = WALKING_SHOT_LEFT;

			}
			else {
				jump_state = WALKING_SHOT_RIGHT;
				atirando = true;
			}
			break;
		}

		if (atirando) {
			al_draw_bitmap(bullet_right, pos_x_bullet + 160, pos_y + 167, 0);
			pos_x_bullet *= 1.2f;
		}
		if (pos_x_bullet > 1280) {
			atirando = false;
		}
		//Mudança de tela

		if (pos_x > 1280) {
			tela2();

		}
		al_flip_display();
	}
	//Destroi imagens
	//Sprites Soldado
	al_destroy_bitmap(walkright);
	al_destroy_bitmap(walkleft);
	al_destroy_bitmap(idle);
	al_destroy_bitmap(shot_image_right);
	al_destroy_bitmap(shot_image_left);
	al_destroy_bitmap(jump_image);
	al_destroy_bitmap(pegar_image);
	al_destroy_bitmap(bullet_right);
	al_destroy_bitmap(bullet_left);
	//Backgrounds
	al_destroy_bitmap(background);
	//Obstáculos
	al_destroy_bitmap(tronco3);
	//Consumíveis
	al_destroy_bitmap(kitmedico);
	al_destroy_bitmap(kitmunicao);

	//Destroi audios
	//Soldado
	al_destroy_sample(tiro);
	//destroi fonts,eventos e tela
	al_destroy_font(font);
	al_destroy_display(display1);
	al_destroy_event_queue(event_queue);

	return 0;

}
int main()
{
	tela1();
	return 0;
}