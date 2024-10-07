#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/keyboard.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <stdio.h>


//infos, sprite idle soldado!
// largura 896 / 7 = 128;
// altura 128 / 1 = 128;
float FPS = 20.0;
///bool controleDeTelas1 = true;

 int tela3() {
	al_init();
	if (!al_init()) {
		fprintf(stderr, "Falha ao inicializar Allegro.\n");
		return -1;
	}
	al_init_font_addon();
	if (!al_init_font_addon()) {
		fprintf(stderr, "Falha ao inicializar font addon.\n");
		return -1;
	}
	al_init_ttf_addon();
	if (!al_init_ttf_addon()) {
		fprintf(stderr, "Falha ao inicializar ttf addon.\n");
		return -1;
	}
	al_init_image_addon();
	if (!al_init_image_addon()) {
		fprintf(stderr, "Falha ao inicializar image addon.\n");
		return -1;
	}
	al_install_keyboard();
	if (!al_install_keyboard()) {
		fprintf(stderr, "Falha ao instalar keyboard.\n");
		return -1;
	}
	al_install_audio();
	if (!al_install_audio()) {
		fprintf(stderr, "Falha ao instalar audio.\n");
		return -1;
	}
	al_init_acodec_addon();

	if (!al_init_acodec_addon()) {
		fprintf(stderr, "Falha ao inicializar acodec audio.\n");
		return -1;
	}
	al_reserve_samples(1);



	ALLEGRO_DISPLAY* display = al_create_display(1280, 720);
	al_set_window_position(display, 200, 200);

	//ALLEGRO_FONT* font = al_create_builtin_font();
	ALLEGRO_FONT* font = al_load_font("./fonte/font.ttf", 60, 0);
	ALLEGRO_TIMER* timer = al_create_timer(1.0 / FPS);

	//IMAGENS
	//Sprites Soldado
	ALLEGRO_BITMAP* walkright = al_load_bitmap("./soldiersprites1/Soldier_1/walkright.png");
	ALLEGRO_BITMAP* walkleft = al_load_bitmap("./soldiersprites1/Soldier_1/walkleft.png");
	ALLEGRO_BITMAP* jump_image = al_load_bitmap("./soldiersprites1/Soldier_1/jump.png");
	ALLEGRO_BITMAP* idle_right = al_load_bitmap("./soldiersprites1/Soldier_1/idleRight.png");
	ALLEGRO_BITMAP* idle_left = al_load_bitmap("./soldiersprites1/Soldier_1/idleLeft.png");
	ALLEGRO_BITMAP* shot_image_right = al_load_bitmap("./soldiersprites1/Soldier_1/shot_1_right.png");
	ALLEGRO_BITMAP* shot_image_left = al_load_bitmap("./soldiersprites1/Soldier_1/shot_1_left.png");
	ALLEGRO_BITMAP* pegar_image = al_load_bitmap("./soldiersprites2/soldier_2/rest.png");
	//ARMAS
	ALLEGRO_BITMAP* bullet_right = al_load_bitmap("./soldiersprites1/teste/bullet_right.png");
	ALLEGRO_BITMAP* bullet_left = al_load_bitmap("soldiersprites1/teste/bullet_left.png");
	//Background
	ALLEGRO_BITMAP* background_3 = al_load_bitmap("./backgrounds/interior_base.jpeg");

	//AUDIOS
	//Soldado
	ALLEGRO_SAMPLE* tiro = al_load_sample("./Audios/metralhadora.mp3");

	//FILAS DE EVENTOS
	ALLEGRO_EVENT_QUEUE* event_queue = al_create_event_queue();
	al_register_event_source(event_queue, al_get_display_event_source(display));
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_register_event_source(event_queue, al_get_keyboard_event_source());
	al_start_timer(timer);
	if (!event_queue) {
		fprintf(stderr, "Falha ao criar a fila de eventos.\n");
		al_destroy_display(display);  // Limpa o que foi alocado antes de encerrar
		return -1;
	}

	//MovimentaÃ§Ãµes personagem soldado, X e Y.
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
	int pos_x = 0, pos_y = 380;
	int current_frame_y = 0;
	bool moving_left = false;
	bool mov = false;
	bool atirando = false;
	float pos_x_bullet = 0.1;

	//VariÃ¡veis dos consumÃ­veis
	int kitmedicoX = 600;
	int kitmunicaoX = 680;

	//TEMPORIZADOR
	float temporizador = 2.0; // Tempo em segundos para a execuÃ§Ã£o acontecer
	float tempo_decorrido = 0.0; // Tempo decorrido

	//colisão
	static bool inicializado = false;
	static bool bala_ativa = false;
	typedef enum {
		WALKING_RIGHT,
		WALKING_LEFT,
		WALKING_SHOT_RIGHT,
		WALKING_SHOT_LEFT,
		JUMPING,
		IDLE_LEFT,
		IDLE_RIGHT,
		PEGAR
	}characterState;

	characterState jump_state = WALKING_RIGHT; //Manter estado atual

	while (true) {
		al_clear_to_color(al_map_rgb(255, 255, 255));
		//Backgrounds
		al_draw_bitmap(background_3, 0, 0, 0);
		
		ALLEGRO_EVENT event;
		al_wait_for_event(event_queue, &event);
		if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			break;
		}
		else if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) { return false; }
		else if (event.keyboard.keycode == ALLEGRO_KEY_SPACE && !pulo) {
			pulo = 1;
			velocy = -velocup;
			plimit = pos_y - 140; //altura do pulo
			jump_state = JUMPING;
			mov = true;

		}		
		else  if (mov == false) {
			jump_state = IDLE_RIGHT;
		}

		if (pulo) {
			pos_y += velocy;
			velocy += 1;

			if (pos_y >= 380) {
				pos_y = 380;
				pulo = 0;
				velocy = 0;
				//Parar de pular ao encostar no chÃ£o
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

		case IDLE_LEFT:
			al_draw_bitmap_region(idle_left, 256 * (int)frameidle, current_frame_y, 256, 256, pos_x, pos_y, 0);
			break;

		case IDLE_RIGHT:
			al_draw_bitmap_region(idle_right, 256 * (int)frameidle, current_frame_y, 256, 256, pos_x, pos_y, 0);
			break;
		default:
			break;
		}
		if (event.type == ALLEGRO_EVENT_TIMER) {

			tempo_decorrido += 1.0 / FPS;

			if (tempo_decorrido >= temporizador) {
				if (moving_left) {
					jump_state = IDLE_LEFT;
				}
				else {
					jump_state = IDLE_RIGHT;
				}
				tempo_decorrido = 0.0;
			}
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
			
			break;
		case ALLEGRO_KEY_B:
			if (moving_left == true) {
				jump_state = WALKING_SHOT_LEFT;
				atirando = true;
			}
			else {
				jump_state = WALKING_SHOT_RIGHT;
				atirando = true;
			}
			break;

		}

		if (atirando) {
			// Inicializa a posição da bala apenas uma vez
			if (inicializado == false) {
				pos_x_bullet = pos_x + 160;
				bala_ativa = true;
				inicializado = true;
			}

			// Desenha a bala e atualiza sua posição se estiver ativa
			if (bala_ativa) {
				if (moving_left == false) {
					al_draw_bitmap(bullet_right, pos_x_bullet, pos_y + 167, 0);
					pos_x_bullet += 20.2f;
				}
				else {
					al_draw_bitmap(bullet_left, pos_x_bullet, pos_y + 167, 0);
					pos_x_bullet -= 20.2f;
				}
			}
		}
		else {
			inicializado = false; // Reseta quando não está atirando
		}

		if (pos_x_bullet > 1280) {
			atirando = false;
		}

		if (pos_x_bullet < 0) {
			atirando = false;
		}
		//MudanÃ§a de tela

		if (pos_x > 1280) {
			tela2();

		}


		al_flip_display();
	}
	//Destroi imagens
	//Sprites Soldado
	al_destroy_bitmap(walkright);
	al_destroy_bitmap(walkleft);
	al_destroy_bitmap(idle_right);
	al_destroy_bitmap(idle_left);
	al_destroy_bitmap(shot_image_right);
	al_destroy_bitmap(shot_image_left);
	al_destroy_bitmap(jump_image);
	al_destroy_bitmap(pegar_image);
	//Armas
	al_destroy_bitmap(bullet_right);
	al_destroy_bitmap(bullet_left);
	//Backgrounds
	al_destroy_bitmap(background_3);

	//Destroi audios
	//Soldado
	al_destroy_sample(tiro);
	//destroi fonts,eventos e tela
	al_destroy_font(font);
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);

	return 0;
}
//MEU
static int tela2() {


	al_init();
	if (!al_init()) {
		fprintf(stderr, "Falha ao inicializar Allegro.\n");
		return -1;
	}
	al_init_font_addon();
	if (!al_init_font_addon()) {
		fprintf(stderr, "Falha ao inicializar font addon.\n");
		return -1;
	}
	al_init_ttf_addon();
	if (!al_init_ttf_addon()) {
		fprintf(stderr, "Falha ao inicializar ttf addon.\n");
		return -1;
	}
	al_init_image_addon();
	if (!al_init_image_addon()) {
		fprintf(stderr, "Falha ao inicializar image addon.\n");
		return -1;
	}
	al_install_keyboard();
	if (!al_install_keyboard()) {
		fprintf(stderr, "Falha ao instalar keyboard.\n");
		return -1;
	}
	al_install_audio();
	if (!al_install_audio()) {
		fprintf(stderr, "Falha ao instalar audio.\n");
		return -1;
	}
	al_init_acodec_addon();

	if (!al_init_acodec_addon()) {
		fprintf(stderr, "Falha ao inicializar acodec audio.\n");
		return -1;
	}
	al_reserve_samples(1);



	ALLEGRO_DISPLAY* display = al_create_display(1280, 720);
	al_set_window_position(display, 200, 200);

	//ALLEGRO_FONT* font = al_create_builtin_font();
	ALLEGRO_FONT* font = al_load_font("./fonte/font.ttf", 60, 0);
	ALLEGRO_TIMER* timer = al_create_timer(1.0 / FPS);

	//IMAGENS
	//Sprites Soldado
	ALLEGRO_BITMAP* walkright = al_load_bitmap("./soldiersprites1/Soldier_1/walkright.png");
	ALLEGRO_BITMAP* walkleft = al_load_bitmap("./soldiersprites1/Soldier_1/walkleft.png");
	ALLEGRO_BITMAP* jump_image = al_load_bitmap("./soldiersprites1/Soldier_1/jump.png");
	ALLEGRO_BITMAP* idle_right = al_load_bitmap("./soldiersprites1/Soldier_1/idleRight.png");
	ALLEGRO_BITMAP* idle_left = al_load_bitmap("./soldiersprites1/Soldier_1/idleLeft.png");
	ALLEGRO_BITMAP* shot_image_right = al_load_bitmap("./soldiersprites1/Soldier_1/shot_1_right.png");
	ALLEGRO_BITMAP* shot_image_left = al_load_bitmap("./soldiersprites1/Soldier_1/shot_1_left.png");
	ALLEGRO_BITMAP* pegar_image = al_load_bitmap("./soldiersprites2/soldier_2/rest.png");
	//ARMAS
	ALLEGRO_BITMAP* bullet_right = al_load_bitmap("./soldiersprites1/teste/bullet_right.png");
	ALLEGRO_BITMAP* bullet_left = al_load_bitmap("soldiersprites1/teste/bullet_left.png");
	//Backgrounds
	ALLEGRO_BITMAP* background_2 = al_load_bitmap("./backgrounds/backgroundfloresta2.jpg");
	//ObstÃ¡culos
	ALLEGRO_BITMAP* tronco3 = al_load_bitmap("./Obstaculos/wood_spike.png");
	//ConsumÃ­veis
	ALLEGRO_BITMAP* kitmedico = al_load_bitmap("./Consumiveis/Kitmedico1.png");
	ALLEGRO_BITMAP* kitmunicao = al_load_bitmap("./consumiveis/kitmunicao1.png");
	//Estilizações
	ALLEGRO_BITMAP* Estilizacao_Alien = al_load_bitmap("./Estilizacoes/EstAliens.png");
	//ALLEGRO_BITMAP* sapo = al_load_bitmap("./Estilizacoes/sapo.png");


	//Entrada base
	ALLEGRO_BITMAP* fundoporta = al_load_bitmap("./base/montanha.png");
	ALLEGRO_BITMAP* porta = al_load_bitmap("./base/porta.png");





	//AUDIOS
	//Soldado
	ALLEGRO_SAMPLE* tiro = al_load_sample("./Audios/metralhadora.mp3");

	//FILAS DE EVENTOS
	ALLEGRO_EVENT_QUEUE* event_queue = al_create_event_queue();
	al_register_event_source(event_queue, al_get_display_event_source(display));
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_register_event_source(event_queue, al_get_keyboard_event_source());
	al_start_timer(timer);
	if (!event_queue) {
		fprintf(stderr, "Falha ao criar a fila de eventos.\n");
		al_destroy_display(display);  // Limpa o que foi alocado antes de encerrar
		return -1;
	}



	//MovimentaÃ§Ãµes personagem soldado, X e Y.
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
	int pos_x = 0, pos_y = 380;
	int current_frame_y = 0;
	bool moving_left = false;
	bool mov = false;
	bool atirando = false;
	float pos_x_bullet = 0.1;

	//VariÃ¡veis dos consumÃ­veis
	int kitmedicoX = 600;
	int kitmunicaoX = 680;

	//TEMPORIZADOR
	float temporizador = 2.0; // Tempo em segundos para a execuÃ§Ã£o acontecer
	float tempo_decorrido = 0.0; // Tempo decorrido

	//Variáveis das Estilizações alienigenas
	int EstAlien_pos_x = 200;
	int EstAlien_pos_y = 200;

	//Tronco e colisão
	int TRONCO_X_MIN = 320;
	int TRONCO_X_MAX = 350;
	int troncoX = 400;
	static bool inicializado = false;
	static bool bala_ativa = false;
	bool troncoVivo = true;
	//al_draw_rectangle(200.0, 200.0, 100.0, 100.0, al_map_rgb(0, 255, 0), 6.0);
	typedef enum {
		WALKING_RIGHT,
		WALKING_LEFT,
		WALKING_SHOT_RIGHT,
		WALKING_SHOT_LEFT,
		JUMPING,
		IDLE_LEFT,
		IDLE_RIGHT,
		PEGAR
	}characterState;

	characterState jump_state = WALKING_RIGHT; //Manter estado atual

	while (true) {
		al_clear_to_color(al_map_rgb(255, 255, 255));
		//Backgrounds
		al_draw_bitmap(background_2, 0, 0, 0);
		//Obstáculos
		al_draw_bitmap(tronco3, troncoX, 516, 0);
		//Entrada base
		al_draw_bitmap(fundoporta, 780, 186, 0);
		al_draw_bitmap(porta, 980, 340, 0);
		//Estilizações
		//Alien 1 x,y a
		al_draw_bitmap_region(Estilizacao_Alien, 0, 0, 300, 150, 760, 490, 0);
		//al_draw_bitmap_region(Estilizacao_Alien, 0, 0, 300, 150, 600, 500, 0);
		//Alien 3 y a
		//al_draw_bitmap_region(Estilizacao_Alien, 0, 300, 225, 200, 1000, 320, 0);


		//al_draw_bitmap_region(walkright, 256 * (int)frame, current_frame_y, 256, 256, pos_x, pos_y, 0);
		//Consumíveis
		//ObstÃ¡culos
		al_draw_bitmap(tronco3, 400, 516, 0);
		//ConsumÃ­veis
		//al_draw_bitmap(kitmedico, kitmedicoX, 600, 0);
		//al_draw_bitmap(kitmunicao, kitmunicaoX, 600, 0);

		ALLEGRO_EVENT event;
		al_wait_for_event(event_queue, &event);
		if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			break;
		}
		else if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) { return false; }
		else if (event.keyboard.keycode == ALLEGRO_KEY_SPACE && !pulo) {
			pulo = 1;
			velocy = -velocup;
			plimit = pos_y - 140; //altura do pulo
			jump_state = JUMPING;
			mov = true;

		}		//al_draw_bitmap(kitmedico, 600, 600, 0);//al_draw_bitmap(kitmunicao, 680, 600, 0);
		else  if (mov == false) {
			jump_state = IDLE_RIGHT;
		}

		if (pulo) {
			pos_y += velocy;
			velocy += 1;

			if (pos_y >= 380) {
				pos_y = 380;
				pulo = 0;
				velocy = 0;
				//Parar de pular ao encostar no chÃ£o
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

		case IDLE_LEFT:
			al_draw_bitmap_region(idle_left, 256 * (int)frameidle, current_frame_y, 256, 256, pos_x, pos_y, 0);
			break;

		case IDLE_RIGHT:
			al_draw_bitmap_region(idle_right, 256 * (int)frameidle, current_frame_y, 256, 256, pos_x, pos_y, 0);
			break;
		default:
			break;
		}
		if (event.type == ALLEGRO_EVENT_TIMER) {

			tempo_decorrido += 1.0 / FPS;

			if (tempo_decorrido >= temporizador) {
				if (moving_left) {
					jump_state = IDLE_LEFT;
				}
				else {
					jump_state = IDLE_RIGHT;
				}
				tempo_decorrido = 0.0;
			}
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
				atirando = true;
			}
			else {
				jump_state = WALKING_SHOT_RIGHT;
				atirando = true;
			}
			break;
		case ALLEGRO_KEY_R:
			if (pos_x > 1000) {
				tela3();

			}
			break;
		}

		if (atirando) {
			// Inicializa a posição da bala apenas uma vez
			if (inicializado == false) {
				pos_x_bullet = pos_x + 160;
				bala_ativa = true;
				inicializado = true;
			}

			// Desenha a bala e atualiza sua posição se estiver ativa
			if (bala_ativa) {
				if (moving_left == false) {
					al_draw_bitmap(bullet_right, pos_x_bullet, pos_y + 167, 0);
					pos_x_bullet += 20.2f;
				}
				else {
					al_draw_bitmap(bullet_left, pos_x_bullet, pos_y + 167, 0);
					pos_x_bullet -= 20.2f;
				}

				// Verifica se a bala atingiu o tronco
				if (troncoVivo) {
					if (pos_x_bullet > TRONCO_X_MIN && pos_x_bullet < TRONCO_X_MAX) {
						troncoX = -1000;
						bala_ativa = false;
						atirando = false;
						troncoVivo = false;
					}
				}
			}
		}
		else {
			inicializado = false; // Reseta quando não está atirando
		}

		if (pos_x_bullet > 1280) {
			atirando = false;
		}

		if (pos_x_bullet < 0) {
			atirando = false;
		}
		//MudanÃ§a de tela

		if (pos_x > 1280) {
			tela2();

		}


		al_flip_display();
	}
	//Destroi imagens
	//Sprites Soldado
	al_destroy_bitmap(walkright);
	al_destroy_bitmap(walkleft);
	al_destroy_bitmap(idle_right);
	al_destroy_bitmap(idle_left);
	al_destroy_bitmap(shot_image_right);
	al_destroy_bitmap(shot_image_left);
	al_destroy_bitmap(jump_image);
	al_destroy_bitmap(pegar_image);
	//Armas
	al_destroy_bitmap(bullet_right);
	al_destroy_bitmap(bullet_left);
	//Backgrounds
	al_destroy_bitmap(background_2);
	//ObstÃ¡culos
	al_destroy_bitmap(tronco3);
	//ConsumÃ­veis
	al_destroy_bitmap(kitmedico);
	al_destroy_bitmap(kitmunicao);

	//Destroi audios
	//Soldado
	al_destroy_sample(tiro);
	//destroi fonts,eventos e tela
	al_destroy_font(font);
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);


	return 0;
}

static int tela1() {
	al_init();
	if (!al_init()) {
		fprintf(stderr, "Falha ao inicializar Allegro! Código: %d\n", al_get_errno());
		return -1;
	}
	al_init_font_addon();
	if (!al_init_font_addon()) {
		fprintf(stderr, "Falha ao inicializar font addon.\n");
		return -1;
	}
	al_init_ttf_addon();
	if (!al_init_ttf_addon()) {
		fprintf(stderr, "Falha ao inicializar ttf addon.\n");
		return -1;
	}
	al_init_image_addon();
	if (!al_init_image_addon()) {
		fprintf(stderr, "Falha ao inicializar image addon.\n");
		return -1;
	}
	al_install_keyboard();
	if (!al_install_keyboard()) {
		fprintf(stderr, "Falha ao instalar keyboard.\n");
		return -1;
	}
	al_install_audio();
	if (!al_install_audio()) {
		fprintf(stderr, "Falha ao instalar audio.\n");
		return -1;
	}
	al_init_acodec_addon();
	if (!al_init_acodec_addon()) {
		fprintf(stderr, "Falha ao inicializar acodec audio.\n");
		return -1;
	}


	ALLEGRO_DISPLAY* display1 = al_create_display(1280, 720);
	al_set_window_position(display1, 200, 200);

	//ALLEGRO_FONT* font = al_create_builtin_font();
	ALLEGRO_FONT* font = al_load_font("./fonte/font.ttf", 60, 0);
	ALLEGRO_TIMER* timer = al_create_timer(1.0 / FPS);


	//IMAGENS
	//Sprites Soldado
	ALLEGRO_BITMAP* walkright = al_load_bitmap("./soldiersprites1/Soldier_1/walkright.png");
	ALLEGRO_BITMAP* walkleft = al_load_bitmap("./soldiersprites1/Soldier_1/walkleft.png");
	ALLEGRO_BITMAP* jump_image = al_load_bitmap("./soldiersprites1/Soldier_1/jump.png");
	ALLEGRO_BITMAP* idle_right = al_load_bitmap("./soldiersprites1/Soldier_1/idleRight.png");
	ALLEGRO_BITMAP* idle_left = al_load_bitmap("./soldiersprites1/Soldier_1/idleLeft.png");
	ALLEGRO_BITMAP* shot_image_right = al_load_bitmap("./soldiersprites1/Soldier_1/shot_1_right.png");
	ALLEGRO_BITMAP* shot_image_left = al_load_bitmap("./soldiersprites1/Soldier_1/shot_1_left.png");
	ALLEGRO_BITMAP* pegar_image = al_load_bitmap("./soldiersprites2/soldier_2/rest.png");
	//ARMAS
	ALLEGRO_BITMAP* bullet_right = al_load_bitmap("./soldiersprites1/teste/bullet_right.png");
	ALLEGRO_BITMAP* bullet_left = al_load_bitmap("soldiersprites1/teste/bullet_left.png");
	//Backgrounds
	ALLEGRO_BITMAP* background = al_load_bitmap("./backgrounds/backgroundfloresta.jpeg");
	//Obstáculos
	ALLEGRO_BITMAP* tronco3 = al_load_bitmap("./Obstaculos/wood_spike.png");
	//Consumíveis
	ALLEGRO_BITMAP* kitmedico = al_load_bitmap("./Consumiveis/Kitmedico1.png");
	ALLEGRO_BITMAP* kitmunicao = al_load_bitmap("./consumiveis/kitmunicao1.png");
	//Estilizações
	ALLEGRO_BITMAP* EstAliens = al_load_bitmap("./Estilizacoes/EstAliens.png");
	ALLEGRO_BITMAP* EstOssos = al_load_bitmap("./Estilizacoes/EstOssos.png");

	//Inimigos
	ALLEGRO_BITMAP* drone = al_load_bitmap("./inimigos/drone.png");

	//AUDIOS
	//Soldado
	ALLEGRO_SAMPLE* tiro = al_load_sample("./Audios/metralhadora.mp3");

	//FILAS DE EVENTOS
	ALLEGRO_EVENT_QUEUE* event_queue = al_create_event_queue();


	al_register_event_source(event_queue, al_get_display_event_source(display1));
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_register_event_source(event_queue, al_get_keyboard_event_source());
	al_start_timer(timer);
	if (!event_queue) {
		fprintf(stderr, "Falha ao criar a fila de eventos.\n");
		al_destroy_display(display1);  // Limpa o que foi alocado antes de encerrar
		return -1;
	}



	//Movimentações personagem soldado, X e Y.
	//int image1 = walkright;

	int pulo = 0;
	int velocy = 0;
	int velocup = 15;
	int plimit = 0;
	float frame = 0.f;
	float frame2 = 0.f;
	float frame4px = 0.f;
	float frameidle = 0.f;
	float frameright = 0.f;
	int pos_x = 0, pos_y = 380;
	int current_frame_y = 0;
	bool moving_left = false;
	bool mov = false;
	bool atirando = false;
	float pos_x_bullet = 0.1;
	//Variáveis dos consumíveis
	int kitmedicoX = 600;
	int kitmunicaoX = 680;

	//Variáveis das Estilizações alienigenas
	int EstAlien_pos_x = 60;
	int EstAlien_pos_y = 0;

	//TEMPORIZADOR
	float temporizador = 2.0; // Tempo em segundos para a execução acontecer
	float tempo_decorrido = 0.0; // Tempo decorrido

	//Tronco e colisão
	int TRONCO_X_MIN = 320;
	int TRONCO_X_MAX = 350;
	int troncoX = 400;
	static bool inicializado = false;
	static bool bala_ativa = false;
	bool troncoVivo = true;
	//al_draw_rectangle(200.0, 200.0, 100.0, 100.0, al_map_rgb(0, 255, 0), 6.0);
	typedef enum {
		WALKING_RIGHT,
		WALKING_LEFT,
		WALKING_SHOT_RIGHT,
		WALKING_SHOT_LEFT,
		JUMPING,
		IDLE_LEFT,
		IDLE_RIGHT,
		PEGAR
	}characterState;

	characterState jump_state = WALKING_RIGHT; //Manter estado atual


	while (true) {
		al_clear_to_color(al_map_rgb(255, 255, 255));
		//Backgrounds
		al_draw_bitmap(background, 0, 0, 0);
		//Obstáculos
		al_draw_bitmap(tronco3, troncoX, 516, 0);
		//Consumíveis
		al_draw_bitmap(kitmedico, kitmedicoX, 600, 0);
		al_draw_bitmap(kitmunicao, kitmunicaoX, 600, 0);
		//Inimigos
		al_draw_bitmap(drone, 500, 200, 0);

		ALLEGRO_EVENT event;
		al_wait_for_event(event_queue, &event);


		if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			break;
		}
		else if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) { return false; }

		else if (event.keyboard.keycode == ALLEGRO_KEY_SPACE && !pulo) {
			pulo = 1;
			velocy = -velocup;
			plimit = pos_y - 140; //altura do pulo
			jump_state = JUMPING;
			mov = true;

		}
		else if (mov == false) {
			jump_state = IDLE_RIGHT;
		}
		else if (pulo) {
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

		case IDLE_LEFT:
			al_draw_bitmap_region(idle_left, 256 * (int)frameidle, current_frame_y, 256, 256, pos_x, pos_y, 0);
			break;

		case IDLE_RIGHT:
			al_draw_bitmap_region(idle_right, 256 * (int)frameidle, current_frame_y, 256, 256, pos_x, pos_y, 0);
			break;
		default:
			break;
		}
		if (event.type == ALLEGRO_EVENT_TIMER) {

			tempo_decorrido += 1.0 / FPS;

			if (tempo_decorrido >= temporizador) {
				if (moving_left) {
					jump_state = IDLE_LEFT;
				}
				else {
					jump_state = IDLE_RIGHT;
				}

				tempo_decorrido = 0.0;
			}
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
				atirando = true;
				pos_x_bullet = pos_x - 160;
			}
			else {
				jump_state = WALKING_SHOT_RIGHT;
				atirando = true;
				pos_x_bullet = pos_x + 160;
			}
			break;
		}

		if (atirando) {
			// Inicializa a posição da bala apenas uma vez
			if (inicializado == false) {
				bala_ativa = true;
				inicializado = true;
			}

			// Desenha a bala e atualiza sua posição se estiver ativa
			if (bala_ativa) {
				if (moving_left == false) {
					al_draw_bitmap(bullet_right, pos_x_bullet, pos_y + 167, 0);
					pos_x_bullet += 20.2f;
				}
				else {
					al_draw_bitmap(bullet_left, pos_x_bullet, pos_y + 167, 0);
					pos_x_bullet -= 20.2f;
				}

				// Verifica se a bala atingiu o tronco
				if (troncoVivo) {
					if (pos_x_bullet > TRONCO_X_MIN && pos_x_bullet < TRONCO_X_MAX) {
						troncoX = -1000;
						bala_ativa = false;
						atirando = false;
						troncoVivo = false;
					}
				}
			}
		}
		else {
			inicializado = false; // Reseta quando não está atirando
		}

		if (pos_x_bullet > 1280) {
			atirando = false;
		}

		if (pos_x_bullet < 0) {
			atirando = false;
		}

		//MudanÃ§a de tela

		if (pos_x > 1280) {
			tela2();

		}


		al_flip_display();
	}
	//Destroi imagens
	//Sprites Soldado
	al_destroy_bitmap(walkright);
	al_destroy_bitmap(walkleft);
	al_destroy_bitmap(idle_right);
	al_destroy_bitmap(idle_left);
	al_destroy_bitmap(shot_image_right);
	al_destroy_bitmap(shot_image_left);
	al_destroy_bitmap(jump_image);
	al_destroy_bitmap(pegar_image);
	//Armas
	al_destroy_bitmap(bullet_right);
	al_destroy_bitmap(bullet_left);
	//Backgrounds
	al_destroy_bitmap(background);
	//ObstÃ¡culos
	al_destroy_bitmap(tronco3);
	//ConsumÃ­veis
	al_destroy_bitmap(kitmedico);
	al_destroy_bitmap(kitmunicao);
	//Estilizações
	al_destroy_bitmap(EstAliens);
	al_destroy_bitmap(EstOssos);

	//Destroi audios
	//Soldado
	al_destroy_sample(tiro);
	//destroi fonts,eventos,tela e timer
	al_destroy_font(font);
	al_destroy_display(display1);
	al_destroy_event_queue(event_queue);
	al_destroy_timer(timer);


	return 0;

}
static int menu() {
	al_init();
	if (!al_init()) {
		fprintf(stderr, "Falha ao inicializar Allegro! Código: %d\n", al_get_errno());
		return -1;
	}
	al_init_font_addon();
	if (!al_init_font_addon()) {
		fprintf(stderr, "Falha ao inicializar font addon.\n");
		return -1;
	}
	al_init_ttf_addon();
	if (!al_init_ttf_addon()) {
		fprintf(stderr, "Falha ao inicializar ttf addon.\n");
		return -1;
	}
	al_init_image_addon();
	if (!al_init_image_addon()) {
		fprintf(stderr, "Falha ao inicializar image addon.\n");
		return -1;
	}
	al_install_keyboard();
	if (!al_install_keyboard()) {
		fprintf(stderr, "Falha ao instalar keyboard.\n");
		return -1;
	}
	al_install_audio();
	if (!al_install_audio()) {
		fprintf(stderr, "Falha ao instalar audio.\n");
		return -1;
	}
	al_init_acodec_addon();
	if (!al_init_acodec_addon()) {
		fprintf(stderr, "Falha ao inicializar acodec audio.\n");
		return -1;
	}

	ALLEGRO_DISPLAY* display1 = al_create_display(1280, 720);
	al_set_window_position(display1, 200, 200);

	//IMAGENS
	ALLEGRO_BITMAP* menu_image = al_load_bitmap("./backgrounds/backgroundmenu.png");
	ALLEGRO_BITMAP* menu_opcoes_image = al_load_bitmap("./backgrounds/menu_opcoes_image.png");

	//ALLEGRO_FONT* font = al_create_builtin_font();
	ALLEGRO_FONT* font_realista = al_load_font("./fonte/airstrike.ttf", 60, 0);
	ALLEGRO_TIMER* timer = al_create_timer(1.0 / FPS);

	//FILAS DE EVENTOS
	ALLEGRO_EVENT_QUEUE* event_queue = al_create_event_queue();

	al_register_event_source(event_queue, al_get_display_event_source(display1));
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_register_event_source(event_queue, al_get_keyboard_event_source());
	al_start_timer(timer);
	if (!event_queue) {
		fprintf(stderr, "Falha ao criar a fila de eventos.\n");
		al_destroy_display(display1);  // Limpa o que foi alocado antes de encerrar
		return -1;
	}
	//Variáveis
	int menu_frame_x = 0;
	int menu_frame_y = 0;

	int menu_pos_x = 0;
	int menu_pos_y = 0;

	int controlador = 1;

	while (true) {
		//Background menu
		al_draw_bitmap(menu_image, 0, 0, 0);
		//Opções
		al_draw_bitmap(menu_opcoes_image, 450, 50, 0);

		ALLEGRO_EVENT event;
		al_wait_for_event(event_queue, &event);

		if (event.keyboard.keycode == ALLEGRO_KEY_UP) {
			controlador += 1;
		}
		if (controlador == 4) {
			al_draw_text(font_realista, al_map_rgb(250, 237, 39), 625, 176, ALLEGRO_ALIGN_CENTER, "JOGAR");

		}
		if (controlador == 4 && event.keyboard.keycode == ALLEGRO_KEY_ENTER) {
			tela1();
		}
		else if (controlador == 3) {
			al_draw_text(font_realista, al_map_rgb(250, 237, 39), 625, 320, ALLEGRO_ALIGN_CENTER, "OPCOES");

		}
		else if (controlador == 2) {
			al_draw_text(font_realista, al_map_rgb(250, 237, 39), 625, 467, ALLEGRO_ALIGN_CENTER, "TECLAS");

		}
		else  if (controlador == 1) {
			al_draw_text(font_realista, al_map_rgb(250, 237, 39), 625, 618, ALLEGRO_ALIGN_CENTER, "CREDITOS");

		}
		else if (controlador == 5) {
			controlador = 1;
		}
		//al_draw_bitmap_region(menu_opcoes_image, menu_frame_x, menu_frame_y,800, 800, menu_pos_x, menu_pos_y,0);

	//Mudança de tela
		al_flip_display();
	}
	//Destroi imagens
	al_destroy_bitmap(menu_image);
	al_destroy_bitmap(menu_opcoes_image);
	//destroi fonts,eventos,tela e timer
	al_destroy_font(font_realista);
	al_destroy_display(display1);
	al_destroy_event_queue(event_queue);
	al_destroy_timer(timer);
	return 0;
}
int main()
{    
	//tela1();
	menu();
	return 0;
}
