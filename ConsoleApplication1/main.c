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
static int tela4() {
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
	//inimigo
	ALLEGRO_BITMAP* inimigoright = al_load_bitmap("./inimigos/inimigodireita.png");
	ALLEGRO_BITMAP* inimigoleft = al_load_bitmap("./inimigos/inimigoesquerda.png");
	//general
	ALLEGRO_BITMAP* generalright = al_load_bitmap("./inimigos/generaldireita.png");
	ALLEGRO_BITMAP* generalleft = al_load_bitmap("./inimigos/generalesquerda.png");
	//Background
	ALLEGRO_BITMAP* background_4 = al_load_bitmap("./base/sala_boss.jpg");
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
	float frame7px = 0.f;
	float frame4px = 0.f;
	float pos_x = 0.0, pos_y = 380.0;
	float soldier_height = 256.0;
	float soldier_width = 256.0;
	int current_frame_y = 0;
	bool moving_left = false;
	bool moving_up = false;
	bool moving_down = false;
	bool moving_right = false;
	bool mov = false;
	bool atirando = false;
	bool verdadeiro = true;
	bool jump_control = false;
	float pos_x_bullet = 0.1;

	//VariÃ¡veis dos consumÃ­veis
	int kitmedicoX = 600;
	int kitmunicaoX = 680;

	//VARIÁVEIS INIMIGO
	float frameinimigo = 0.f;
	int enemy_pos_x = 1200;
	int enemy_pos_y = 600;
	int enemy_velocity = 3; // Velocidade do inimigo
	bool enemy_moving_right = true; // Direção do movimento do inimigo
	bool enemy_alive = true; // Estado de vida do inimigo

	//CONTROLE ESTADO DO ENEMY
	typedef enum {
		INIMIGO_RIGHT,
		INIMIGO_LEFT
	} enemyState;

	enemyState current_enemy_state = INIMIGO_RIGHT;

	//TEMPORIZADOR PERSONAGEM ANDANDO!!
	float frame_rate = 0.15f; // Tempo em segundos para a execução acontecer
	float tempo_limite = 5.0f;
	float tempo_animacao = 0.0; // Tempo decorrido
	float tempo_animacao2 = 0.0;
	float tempo_animacao_enemy = 0.0;
	float tempo_control_finished = 0.0;
	int max_frame = 7;
	int max_frame2 = 4;
	int max_frame_enemy = 3;

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

	characterState jump_state = IDLE_RIGHT; //Manter estado atual

	while (true) {

		ALLEGRO_EVENT event;
		al_wait_for_event(event_queue, &event);



		if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			break;
		}
		else if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) { return false; }

		else if (event.keyboard.keycode == ALLEGRO_KEY_SPACE) {
			pulo = 1;
			velocy = -velocup;
			plimit = pos_y - 140; //altura do pulo
			jump_state = JUMPING;


		}

		else if (pulo) {
			pos_y += velocy;
			velocy += 1;

			if (pos_y >= 380) {
				pos_y = 380;
				pulo = 0;
				velocy = 0;
				jump_control = false;
				//Parar de pular ao encostar no chão
				if (moving_left) jump_state = WALKING_LEFT;
				else jump_state = WALKING_RIGHT;
			}
		}

		switch (event.keyboard.keycode) {
		case ALLEGRO_KEY_LEFT:
			jump_state = WALKING_LEFT;
			pos_x -= 5;
			moving_left = true;
			mov = true;
			break;
		case ALLEGRO_KEY_RIGHT:
			jump_state = WALKING_RIGHT;
			pos_x += 5;
			moving_left = false;
			mov = true;
			break;
		case ALLEGRO_KEY_UP:
			moving_up = true;
			pos_y -= 5;
			break;
		case ALLEGRO_KEY_DOWN:
			moving_down = true;
			pos_y += 5;
			break;
		case ALLEGRO_KEY_V:
			jump_state = PEGAR;
			if (pos_x > (kitmedicoX - 135) && pos_x < (kitmedicoX - 70) && pos_y + 128 > pos_y && pos_y < pos_y + 100) {

				kitmedicoX = -1000;

			}
			else if (pos_x > (kitmunicaoX - 120) && pos_x < (kitmunicaoX - 10) && pos_y + 128 > pos_y && pos_y < pos_y + 100) {
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
				al_play_sample(tiro, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
			}
			else {
				jump_state = WALKING_SHOT_RIGHT;
				atirando = true;
				pos_x_bullet = pos_x + 160;
				al_play_sample(tiro, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
			}
			break;
		}

		if (event.type == ALLEGRO_EVENT_TIMER) {

			tempo_animacao += 1.0 / FPS;

			if (tempo_animacao >= frame_rate) {
				frame7px += 1.0f;
				if (frame7px >= max_frame) {
					frame7px = 0; // Reinicia o quadro de animação
				}

				tempo_animacao = 0.0f;

			}
			tempo_animacao2 += 1.0 / FPS;
			if (tempo_animacao2 >= frame_rate) {
				frame4px += 1.0f;
				if (frame4px >= max_frame2) {
					frame4px = 0; // Reinicia o quadro de animação
				}

				tempo_animacao2 = 0.0f;

			}
			tempo_animacao_enemy += 1.0 / FPS;
			if (tempo_animacao_enemy >= frame_rate) {
				frameinimigo += 1.0f;
				if (frameinimigo >= max_frame_enemy) {
					frameinimigo = 0; // Reinicia o quadro de animação do inimigo
				}
				tempo_animacao_enemy = 0.0f;
			}
			al_clear_to_color(al_map_rgb(255, 255, 255));
			//Backgrounds
			al_draw_bitmap(background_4, 0, 0, 0);
			//Consumíveis
			//al_draw_bitmap(kitmedico, kitmedicoX, 650, 0);
			//al_draw_bitmap(kitmunicao, kitmunicaoX, 650, 0);

			switch (jump_state) {
			case WALKING_RIGHT:
				al_draw_bitmap_region(walkright, 256 * (int)frame7px, current_frame_y, 256, 256, pos_x, pos_y, 0);
				break;

			case WALKING_LEFT:
				al_draw_bitmap_region(walkleft, 256 * (int)frame7px, current_frame_y, 256, 256, pos_x, pos_y, 0);
				break;

			case WALKING_SHOT_RIGHT:
				al_draw_bitmap_region(shot_image_right, 256 * (int)frame4px, current_frame_y, 256, 256, pos_x, pos_y, 0);
				break;

			case WALKING_SHOT_LEFT:
				al_draw_bitmap_region(shot_image_left, 256 * (int)frame4px, current_frame_y, 256, 256, pos_x, pos_y, 0);
				break;

			case JUMPING:if (moving_left) {
				al_draw_bitmap_region(jump_image, 256 * (int)frame7px, current_frame_y, 256, 256, pos_x, pos_y, ALLEGRO_FLIP_HORIZONTAL);
				//Inverter imagem do pulo
			}
						else {
				al_draw_bitmap_region(jump_image, 256 * (int)frame7px, current_frame_y, 256, 256, pos_x, pos_y, 0);


			}
						break;
			case PEGAR:
				al_draw_bitmap_region(pegar_image, 256 * (int)frame4px, current_frame_y, 256, 256, pos_x, pos_y, 0);
				break;

			case IDLE_LEFT:
				al_draw_bitmap_region(idle_left, 256 * (int)frame7px, current_frame_y, 256, 256, pos_x, pos_y, 0);
				break;

			case IDLE_RIGHT:
				al_draw_bitmap_region(idle_right, 256 * (int)frame7px, current_frame_y, 256, 256, pos_x, pos_y, 0);
				break;
			default:
				break;
			}
			//Impede o mov de cancelar a animação de pulo, tiro e de pegar.
			if (!(jump_state == WALKING_SHOT_LEFT) && !(jump_state == WALKING_SHOT_RIGHT) && !(jump_state == JUMPING) && !(jump_state == PEGAR)) {
				if (moving_left) {
					jump_state = IDLE_LEFT;
				}
				else {
					jump_state = IDLE_RIGHT;
				}
			}
			mov = false;

			// Dentro do loop principal, na parte de colisão e atualização de estado
			if (pulo && velocy > 0) { // Se o personagem está caindo
				// Verificar se o personagem está em cima do inimigo
				if (pos_x + soldier_width > enemy_pos_x && pos_x < enemy_pos_x + 61 && // colisão em X
					pos_y + soldier_height > enemy_pos_y && pos_y + soldier_height < enemy_pos_y + 30) { // colisão em Y
					enemy_alive = false; // Inimigo morre
					if (!jump_control) {
					velocy = -velocup; // O personagem salta novamente
					jump_control = true;
					}
				}
			}

			// No código onde os inimigos são desenhados, adicionar uma condição para verificar se o inimigo está vivo
			if (enemy_alive) {
				if (enemy_moving_right) {
					enemy_pos_x += enemy_velocity; // Move para a direita
					current_enemy_state = INIMIGO_RIGHT;
					if (enemy_pos_x > 1280 - 183) { // 183 é a largura do sprite do inimigo
						enemy_moving_right = false; // Muda a direção para a esquerda
					}
				}
				else {
					enemy_pos_x -= enemy_velocity; // Move para a esquerda
					current_enemy_state = INIMIGO_LEFT;
					if (enemy_pos_x < 0) { // Verifica se atingiu o limite esquerdo
						enemy_moving_right = true; // Muda a direção para a direita
					}
				}

				// INIMIGO - renderiza apenas se estiver vivo
				switch (current_enemy_state) {
				case INIMIGO_RIGHT:
					al_draw_bitmap_region(inimigoleft, 61 * (int)frameinimigo, 0, 61, 64, enemy_pos_x, enemy_pos_y, 0);
					break;

				case INIMIGO_LEFT:
					al_draw_bitmap_region(inimigoright, 61 * (int)frameinimigo, 0, 61, 64, enemy_pos_x, enemy_pos_y, 0);
					break;
				default:
					break;
				}
			}
			else{
				tempo_control_finished += 1.0 / FPS;
				al_draw_text(font, al_map_rgb(255, 255, 0), 500, 200, 0, "VITÓRIA!");
				if (tempo_control_finished >= tempo_limite) {
					break;
				}
			}
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
			}
		}
		else {
			inicializado = false; // Reseta quando não está atirando
		}

		if (pos_x_bullet > 1280) {
			atirando = false;
		}
		else if (pos_x_bullet < 0) {
			atirando = false;
		}

		if (verdadeiro && pos_x + 75 > 200) {
			if (event.keyboard.keycode == ALLEGRO_KEY_RIGHT) {
				current_frame_y -= 3;
			}
		}
		else if (current_frame_y <= 0) {
			verdadeiro = false;
			if (event.keyboard.keycode == ALLEGRO_KEY_LEFT) {
				current_frame_y += 3;
			}
		}
		else {
			verdadeiro = true;
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
	al_destroy_bitmap(background_4);

	//Destroi audios
	//Soldado
	al_destroy_sample(tiro);
	//destroi fonts,eventos e tela
	al_destroy_font(font);
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);

	return 0;
}
static int tela3() {
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
	//Consumíveis
	ALLEGRO_BITMAP* kitmedico = al_load_bitmap("./Consumiveis/Kitmedico1.png");
	ALLEGRO_BITMAP* kitmunicao = al_load_bitmap("./consumiveis/kitmunicao1.png");
	//Background
	ALLEGRO_BITMAP* background_3 = al_load_bitmap("./backgrounds/fundo_base.jpg");

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
	float frame7px = 0.f;
	float frame4px = 0.f;
	int pos_x = 0, pos_y = 430;
	int current_frame_y = 0;
	bool moving_left = false;
	bool moving_up = false;
	bool moving_down = false;
	bool moving_right=false; 
	bool mov = false;
	bool atirando = false;
	float pos_x_bullet = 0.1;

	//VariÃ¡veis dos consumÃ­veis
	int kitmedicoX = 1080;
	int kitmunicaoX = 1160;

	//TEMPORIZADOR PERSONAGEM ANDANDO!!
	float frame_rate = 0.15f; // Tempo em segundos para a execução acontecer
	float tempo_animacao = 0.0; // Tempo decorrido
	float tempo_animacao2 = 0.0;
	int max_frame = 7;
	int max_frame2 = 4;

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

		ALLEGRO_EVENT event;
		al_wait_for_event(event_queue, &event);



		if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			break;
		}
		else if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) { return false; }

		else if (event.keyboard.keycode == ALLEGRO_KEY_SPACE) {
			pulo = 1;
			velocy = -velocup;
			plimit = pos_y - 140; //altura do pulo
			jump_state = JUMPING;


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

		switch (event.keyboard.keycode) {
		case ALLEGRO_KEY_LEFT:
			jump_state = WALKING_LEFT;
			pos_x -= 5;
			moving_left = true;
			mov = true;
			break;
		case ALLEGRO_KEY_RIGHT:
			jump_state = WALKING_RIGHT;
			pos_x += 5;
			moving_left = false;
			mov = true;
			break;
		case ALLEGRO_KEY_UP:
			moving_up = true;
			pos_y -= 5;
			break;
		case ALLEGRO_KEY_DOWN:
			moving_down = true;
			pos_y += 5;
			break;
		case ALLEGRO_KEY_V:
			jump_state = PEGAR;
			if (pos_x > (kitmedicoX  - 135) && pos_x < (kitmedicoX - 70) && pos_y + 128 > pos_y && pos_y < pos_y + 100) {

				kitmedicoX = -1000;

			}
			else if (pos_x > (kitmunicaoX - 120) && pos_x < (kitmunicaoX - 10) && pos_y + 128 > pos_y && pos_y < pos_y + 100) {
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
				al_play_sample(tiro, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
			}
			else {
				jump_state = WALKING_SHOT_RIGHT;
				atirando = true;
				pos_x_bullet = pos_x + 160;
				al_play_sample(tiro, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
			}
			break;
		}

		switch (event.keyboard.keycode) {
		case ALLEGRO_KEY_LEFT:
			moving_left = false;
			break;
		case ALLEGRO_KEY_RIGHT:
			moving_right = false;
			break;
		case ALLEGRO_KEY_UP:
			moving_up = false;
			break;
		case ALLEGRO_KEY_DOWN:
			moving_down = false;
			break;
		}


		// Movimentos diagonais
		if (moving_left && moving_up) {
			pos_x -= 4;
			pos_y -= 4;
		}
		else if (moving_right && moving_up) {
			pos_x += 4;
			pos_y -= 4;
		}
		else if (moving_left && moving_down) {
			pos_x -= 4;
			pos_y += 4;
		}
		else if (moving_right && moving_down) {
			pos_x += 4;
			pos_y += 4;
		}

		if (event.type == ALLEGRO_EVENT_TIMER) {

			tempo_animacao += 1.0 / FPS;

			if (tempo_animacao >= frame_rate) {
				frame7px += 1.0f;
				if (frame7px >= max_frame) {
					frame7px = 0; // Reinicia o quadro de animação
				}

				tempo_animacao = 0.0f;

			}
			tempo_animacao2 += 1.0 / FPS;
			if (tempo_animacao2 >= frame_rate) {
				frame4px += 1.0f;
				if (frame4px >= max_frame2) {
					frame4px = 0; // Reinicia o quadro de animação
				}

				tempo_animacao2 = 0.0f;

			}

			al_clear_to_color(al_map_rgb(255, 255, 255));
			//Backgrounds
			al_draw_bitmap(background_3, 0, 0, 0);
			//Consumíveis
			al_draw_bitmap(kitmedico, kitmedicoX, 650, 0);
			al_draw_bitmap(kitmunicao, kitmunicaoX, 650, 0);


			switch (jump_state) {
			case WALKING_RIGHT:
				al_draw_bitmap_region(walkright, 256 * (int)frame7px, current_frame_y, 256, 256, pos_x, pos_y, 0);
				break;

			case WALKING_LEFT:
				al_draw_bitmap_region(walkleft, 256 * (int)frame7px, current_frame_y, 256, 256, pos_x, pos_y, 0);
				break;

			case WALKING_SHOT_RIGHT:
				al_draw_bitmap_region(shot_image_right, 256 * (int)frame4px, current_frame_y, 256, 256, pos_x, pos_y, 0);
				break;

			case WALKING_SHOT_LEFT:
				al_draw_bitmap_region(shot_image_left, 256 * (int)frame4px, current_frame_y, 256, 256, pos_x, pos_y, 0);
				break;

			case JUMPING:if (moving_left) {
				al_draw_bitmap_region(jump_image, 256 * (int)frame7px, current_frame_y, 256, 256, pos_x, pos_y, ALLEGRO_FLIP_HORIZONTAL);
				//Inverter imagem do pulo
			}
						else {
				al_draw_bitmap_region(jump_image, 256 * (int)frame7px, current_frame_y, 256, 256, pos_x, pos_y, 0);


			}
						break;
			case PEGAR:
				al_draw_bitmap_region(pegar_image, 256 * (int)frame4px, current_frame_y, 256, 256, pos_x, pos_y, 0);
				break;

			case IDLE_LEFT:
				al_draw_bitmap_region(idle_left, 256 * (int)frame7px, current_frame_y, 256, 256, pos_x, pos_y, 0);
				break;

			case IDLE_RIGHT:
				al_draw_bitmap_region(idle_right, 256 * (int)frame7px, current_frame_y, 256, 256, pos_x, pos_y, 0);
				break;
			default:
				break;
			}
			//Impede o mov de cancelar a animação de pulo, tiro e de pegar.
			if (!(jump_state == WALKING_SHOT_LEFT) && !(jump_state == WALKING_SHOT_RIGHT) && !(jump_state == JUMPING) && !(jump_state == PEGAR)) {
				if (moving_left) {
					jump_state = IDLE_LEFT;
				}
				else {
					jump_state = IDLE_RIGHT;
				}
			}
			mov = false;
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
			}
		}
		else {
			inicializado = false; // Reseta quando não está atirando
		}

		if (pos_x_bullet > 1280) {
			atirando = false;
		}
		else if (pos_x_bullet < 0) {
			atirando = false;
		}
		//MudanÃ§a de tela

		if (pos_x > 777) {
	         tela4();
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
	//inimigo
    ALLEGRO_BITMAP* inimigoright = al_load_bitmap("./inimigos/inimigodireita.png");
    ALLEGRO_BITMAP* inimigoleft = al_load_bitmap("./inimigos/inimigoesquerda.png");
    //general
    ALLEGRO_BITMAP* generalright = al_load_bitmap("./inimigos/generaldireita.png");
    ALLEGRO_BITMAP* generalleft = al_load_bitmap("./inimigos/generalesquerda.png");
	//Estilizações
	ALLEGRO_BITMAP* Estilizacao_Alien = al_load_bitmap("./Estilizacoes/EstAliens.png");
	ALLEGRO_BITMAP* Estilizacao_Ossos = al_load_bitmap("./Estilizacoes/EstOssos.png");
	ALLEGRO_BITMAP* Rei = al_load_bitmap("./Estilizacoes/Rei_EstOssos.png");
	ALLEGRO_BITMAP* ossinhos = al_load_bitmap("./Estilizacoes/Ossinhos.png");
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
	float frame7px = 0.f;
	float frame4px = 0.f;
	int pos_x = 0, pos_y = 380;
	int current_frame_y = 0;
	bool moving_left = false;
	bool mov = false;
	bool atirando = false;
	float pos_x_bullet = 0.1;

	//VARIÁVEIS INIMIGO
float frameinimigo = 0.f;
int enemy_pos_x = 1200;
int enemy_pos_y = 600;
int enemy_velocity = 3; // Velocidade do inimigo
bool enemy_moving_right = true; // Direção do movimento do inimigo

//CONTROLE ESTADO DO ENEMY
typedef enum {
	INIMIGO_RIGHT,
	INIMIGO_LEFT
} enemyState;

enemyState current_enemy_state = INIMIGO_RIGHT;
//tamanho sprite general 
// x = 295 / 3 = 98
// y = 102 
// 
//VARIÁVEIS DO GENERAL
float framegeneral = 0.3f;
int gen_pos_x = 1050;
int gen_pos_y = 580;
int gen_velocity = 2; // Velocidade do generaL
bool gen_moving_right = true; // Direção do movimento do general

//CONTROLE DO ESTADO DO GENERAL
typedef enum {
	GEN_RIGHT,
	GEN_LEFT
} GenState;

GenState current_gen_state = GEN_RIGHT;
	//VariÃ¡veis dos consumÃ­veis
	int kitmedicoX = 600;
	int kitmunicaoX = 680;

	//TEMPORIZADOR PERSONAGEM ANDANDO!!
	float frame_rate = 0.15f; // Tempo em segundos para a execução acontecer
	float tempo_animacao = 0.0; // Tempo decorrido
	float tempo_animacao2 = 0.0;
	float tempo_animacao_enemy = 0.0;
	int max_frame = 7;
	int max_frame2 = 4;
	int max_frame_enemy = 3;

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

	characterState jump_state = IDLE_RIGHT; //Manter estado atual
	while (true) {
		ALLEGRO_EVENT event;
		al_wait_for_event(event_queue, &event);



		if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			break;
		}
		else if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) { return false; }

		else if (event.keyboard.keycode == ALLEGRO_KEY_SPACE) {
			pulo = 1;
			velocy = -velocup;
			plimit = pos_y - 140; //altura do pulo
			jump_state = JUMPING;


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


		if (pos_x + 75 > 320 && pos_x < 350 && pos_y + 128 > pos_y && pos_y < pos_y + 100) {
			al_draw_text(font, al_map_rgb(0, 0, 0), 640, 360, ALLEGRO_ALIGN_CENTER, "Teste");
		}


		switch (event.keyboard.keycode) {
		case ALLEGRO_KEY_LEFT:
			jump_state = WALKING_LEFT;
			pos_x -= 5;
			moving_left = true;
			mov = true;
			break;
		case ALLEGRO_KEY_RIGHT:
			jump_state = WALKING_RIGHT;
			pos_x += 5;
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
				al_play_sample(tiro, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
			}
			else {
				jump_state = WALKING_SHOT_RIGHT;
				atirando = true;
				pos_x_bullet = pos_x + 160;
				al_play_sample(tiro, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
			}
			break;
			case ALLEGRO_KEY_R:
	if (pos_x > 1000) {
		tela3();

	}
	break;
		}



		if (event.type == ALLEGRO_EVENT_TIMER) {

			tempo_animacao += 1.0 / FPS;

			if (tempo_animacao >= frame_rate) {
				frame7px += 1.0f;
				if (frame7px >= max_frame) {
					frame7px = 0; // Reinicia o quadro de animação
				}

				tempo_animacao = 0.0f;

			}
			tempo_animacao2 += 1.0 / FPS;
			if (tempo_animacao2 >= frame_rate) {
				frame4px += 1.0f;
				if (frame4px >= max_frame2) {
					frame4px = 0; // Reinicia o quadro de animação
				}

				tempo_animacao2 = 0.0f;

			}

			tempo_animacao_enemy += 1.0 / FPS;
			if (tempo_animacao_enemy >= frame_rate) {
				frameinimigo += 1.0f;
				if (frameinimigo >= max_frame_enemy) {
					frameinimigo = 0; // Reinicia o quadro de animação do inimigo
				}
				tempo_animacao_enemy = 0.0f;
			}

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
			//al_draw_bitmap_region(Estilizacao_Alien, 0, 300, 225, 200, 990, 375, 0);
			//al_draw_bitmap_region(Estilizacao_Alien, 225, 320, 190, 88, 1160, 410, 0);
			//Cobras
			al_draw_bitmap_region(Estilizacao_Alien, 320, 135, 127, 120, 240, 500, 0);
			al_draw_bitmap_region(Estilizacao_Alien, 447, 150, 165, 100, 360, 510, 0);
			//Ossos
			al_draw_bitmap(ossinhos, 1120, 410, 0);
			al_draw_bitmap(Rei, 1070, 390, 0);
			//al_draw_bitmap_region(walkright, 256 * (int)frame, current_frame_y, 256, 256, pos_x, pos_y, 0);
			//Consumíveis
			//ObstÃ¡culos
			al_draw_bitmap(tronco3, 400, 516, 0);
			//ConsumÃ­veis
			//al_draw_bitmap(kitmedico, kitmedicoX, 600, 0);
			//al_draw_bitmap(kitmunicao, kitmunicaoX, 600, 0);al_clear_to_color(al_map_rgb(255, 255, 255));
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
			//al_draw_bitmap_region(Estilizacao_Alien, 0, 300, 225, 200, 990, 375, 0);
			//al_draw_bitmap_region(Estilizacao_Alien, 225, 320, 190, 88, 1160, 410, 0);
			//Cobras
			al_draw_bitmap_region(Estilizacao_Alien, 320, 135, 127, 120, 240, 500, 0);
			al_draw_bitmap_region(Estilizacao_Alien, 447, 150, 165, 100, 360, 510, 0);
			//Ossos
			al_draw_bitmap(ossinhos, 1120, 410, 0);
			al_draw_bitmap(Rei, 1070, 390, 0);
			//al_draw_bitmap_region(walkright, 256 * (int)frame, current_frame_y, 256, 256, pos_x, pos_y, 0);
			//Consumíveis
			//ObstÃ¡culos
			al_draw_bitmap(tronco3, 400, 516, 0);
			//ConsumÃ­veis
			//al_draw_bitmap(kitmedico, kitmedicoX, 600, 0);
			//al_draw_bitmap(kitmunicao, kitmunicaoX, 600, 0);

			/*if (enemy_moving_right) {
				enemy_pos_x += enemy_velocity; // Move para a direita
				current_enemy_state = INIMIGO_RIGHT;
				if (enemy_pos_x > 1280 - 183) { // 183 é a largura do sprite do inimigo
					enemy_moving_right = false; // Muda a direção para a esquerda
				}
			}
			else {
				enemy_pos_x -= enemy_velocity; // Move para a esquerda
				current_enemy_state = INIMIGO_LEFT;
				if (enemy_pos_x < 0) { // Verifica se atingiu o limite esquerdo
					enemy_moving_right = true; // Muda a direção para a direita
				}
			}

			//INIMIGO
			switch (current_enemy_state) {
			case INIMIGO_RIGHT:
				al_draw_bitmap_region(inimigoleft, 61 * (int)frameinimigo, 0, 61, 64, enemy_pos_x, enemy_pos_y, 0);

				break;

			case INIMIGO_LEFT:
				al_draw_bitmap_region(inimigoright, 61 * (int)frameinimigo, 0, 61, 64, enemy_pos_x, enemy_pos_y, 0);
				break;
			default:
				break;
			}*/
			//Movimento GENERAL - deixei para caso ele precise se movimentar ou somente ser guarda costas
			switch (current_gen_state) {
			case GEN_RIGHT:
				al_draw_bitmap_region(generalright, 98 * (int)framegeneral, 0, 98, 102, gen_pos_x, gen_pos_y, 0);
				break;
			case GEN_LEFT:
				al_draw_bitmap_region(generalleft, 98 * (int)framegeneral, 0, 98, 102, gen_pos_x, gen_pos_y, 0);
				break;
			default:
				break;
			}

			switch (jump_state) {
			case WALKING_RIGHT:
				al_draw_bitmap_region(walkright, 256 * (int)frame7px, current_frame_y, 256, 256, pos_x, pos_y, 0);
				break;

			case WALKING_LEFT:
				al_draw_bitmap_region(walkleft, 256 * (int)frame7px, current_frame_y, 256, 256, pos_x, pos_y, 0);
				break;

			case WALKING_SHOT_RIGHT:
				al_draw_bitmap_region(shot_image_right, 256 * (int)frame4px, current_frame_y, 256, 256, pos_x, pos_y, 0);
				break;

			case WALKING_SHOT_LEFT:
				al_draw_bitmap_region(shot_image_left, 256 * (int)frame4px, current_frame_y, 256, 256, pos_x, pos_y, 0);
				break;

			case JUMPING:if (moving_left) {
				al_draw_bitmap_region(jump_image, 256 * (int)frame7px, current_frame_y, 256, 256, pos_x, pos_y, ALLEGRO_FLIP_HORIZONTAL);
				//Inverter imagem do pulo
			}
						else {
				al_draw_bitmap_region(jump_image, 256 * (int)frame7px, current_frame_y, 256, 256, pos_x, pos_y, 0);


			}
						break;
			case PEGAR:
				al_draw_bitmap_region(pegar_image, 256 * (int)frame4px, current_frame_y, 256, 256, pos_x, pos_y, 0);
				break;

			case IDLE_LEFT:
				al_draw_bitmap_region(idle_left, 256 * (int)frame7px, current_frame_y, 256, 256, pos_x, pos_y, 0);
				break;

			case IDLE_RIGHT:
				al_draw_bitmap_region(idle_right, 256 * (int)frame7px, current_frame_y, 256, 256, pos_x, pos_y, 0);
				break;
			default:
				break;
			}
			//Impede o mov de cancelar a animação de pulo, tiro e de pegar.
			if (!(jump_state == WALKING_SHOT_LEFT) && !(jump_state == WALKING_SHOT_RIGHT) && !(jump_state == JUMPING) && !(jump_state == PEGAR)) {
				if (moving_left) {
					jump_state = IDLE_LEFT;
				}
				else {
					jump_state = IDLE_RIGHT;
				}
			}
			mov = false;
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
		else if (pos_x_bullet < 0) {
			atirando = false;
		}
		//MudanÃ§a de tela

		if (pos_x < -60) {
			tela1();

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
        //inimigos
    al_destroy_bitmap(inimigoright);
	al_destroy_bitmap(inimigoleft);
    al_destroy_bitmap(generalright);
    al_destroy_bitmap(generalleft);
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
	if (!al_reserve_samples(1)) {
		fprintf(stderr, "Falha ao reservar amostras de áudio!\n");
		return -1;
	}


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

	ALLEGRO_SAMPLE* tiro = al_load_sample("./Audios/metralhadora.wav");

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


	//TELA 1
	//Movimentações personagem soldado, X e Y.
		//int image1 = walkright;

	int pulo = 0;
	int velocy = 0;
	int velocup = 15;
	int plimit = 0;
	float frame7px = 0.f;
	float frame4px = 0.f;
	int pos_x = 0, pos_y = 380;
	int current_frame_y = 0;
	bool moving_left = false;
	bool mov = false;
	bool atirando = false;
	bool verdadeiro = true;
	float pos_x_bullet = 0.1;
	//Variáveis dos consumíveis
	int kitmedicoX = 600;
	int kitmunicaoX = 680;

	//Variáveis das Estilizações alienigenas
	int EstAlien_pos_x = 60;
	int EstAlien_pos_y = 0;

	//TEMPORIZADOR PERSONAGEM ANDANDO!!
	float frame_rate = 0.15f; // Tempo em segundos para a execução acontecer
	float tempo_animacao = 0.0; // Tempo decorrido
	float tempo_animacao2 = 0.0;
	int max_frame = 7;
	int max_frame2 = 4;


	//Tronco e colisão
	int TRONCO_X_MIN = 320;
	int TRONCO_X_MAX = 350;
	int troncoX = 400;
	static bool inicializado = false;
	static bool bala_ativa = false;
	bool troncoVivo = true;

	bool shot_control = false;
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

	characterState jump_state = IDLE_RIGHT; //Manter estado atual


	while (true) {

		ALLEGRO_EVENT event;
		al_wait_for_event(event_queue, &event);



		if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			break;
		}
		else if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) { return false; }

		else if (event.keyboard.keycode == ALLEGRO_KEY_SPACE) {
			pulo = 1;
			velocy = -velocup;
			plimit = pos_y - 140; //altura do pulo
			jump_state = JUMPING;


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


		//al_draw_text(font, al_map_rgb(0, 0, 0), 230, 200, 0, "Hello World!");

		if (pos_x + 75 > 320 && pos_x < 350 && pos_y + 128 > pos_y && pos_y < pos_y + 100) {
			al_draw_text(font, al_map_rgb(0, 0, 0), 640, 360, ALLEGRO_ALIGN_CENTER, "Teste");
		}

		switch (event.keyboard.keycode) {
		case ALLEGRO_KEY_LEFT:
			jump_state = WALKING_LEFT;
			pos_x -= 5;
			moving_left = true;
			mov = true;
			break;
		case ALLEGRO_KEY_RIGHT:
			jump_state = WALKING_RIGHT;
			pos_x += 5;
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
				pos_x_bullet = pos_x - 80;
				al_play_sample(tiro, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
			}
			else {
				jump_state = WALKING_SHOT_RIGHT;
				atirando = true;
				pos_x_bullet = pos_x + 160;
				al_play_sample(tiro, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
			}
			break;
		}



		if (event.type == ALLEGRO_EVENT_TIMER) {

			tempo_animacao += 1.0 / FPS;

			if (tempo_animacao >= frame_rate) {
				frame7px += 1.0f;
				if (frame7px >= max_frame) {
					frame7px = 0; // Reinicia o quadro de animação
				}

				tempo_animacao = 0.0f;

			}
			tempo_animacao2 += 1.0 / FPS;
			if (tempo_animacao2 >= frame_rate) {
				frame4px += 1.0f;
				if (frame4px >= max_frame2) {
					frame4px = 0; // Reinicia o quadro de animação
				}

				tempo_animacao2 = 0.0f;

			}
			
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

			switch (jump_state) {
			case WALKING_RIGHT:
				al_draw_bitmap_region(walkright, 256 * (int)frame7px, current_frame_y, 256, 256, pos_x, pos_y, 0);
				break;

			case WALKING_LEFT:
				al_draw_bitmap_region(walkleft, 256 * (int)frame7px, current_frame_y, 256, 256, pos_x, pos_y, 0);
				break;

			case WALKING_SHOT_RIGHT:
				al_draw_bitmap_region(shot_image_right, 256 * (int)frame4px, current_frame_y, 256, 256, pos_x, pos_y, 0);
				break;

			case WALKING_SHOT_LEFT:
				al_draw_bitmap_region(shot_image_left, 256 * (int)frame4px, current_frame_y, 256, 256, pos_x, pos_y, 0);
				break;

			case JUMPING:if (moving_left) {
				al_draw_bitmap_region(jump_image, 256 * (int)frame7px, current_frame_y, 256, 256, pos_x, pos_y, ALLEGRO_FLIP_HORIZONTAL);
				//Inverter imagem do pulo
			}
						else {
				al_draw_bitmap_region(jump_image, 256 * (int)frame7px, current_frame_y, 256, 256, pos_x, pos_y, 0);


			}
						break;
			case PEGAR:
				al_draw_bitmap_region(pegar_image, 256 * (int)frame4px, current_frame_y, 256, 256, pos_x, pos_y, 0);
				break;

			case IDLE_LEFT:
				al_draw_bitmap_region(idle_left, 256 * (int)frame7px, current_frame_y, 256, 256, pos_x, pos_y, 0);
				break;

			case IDLE_RIGHT:
				al_draw_bitmap_region(idle_right, 256 * (int)frame7px, current_frame_y, 256, 256, pos_x, pos_y, 0);
				break;
			default:
				break;
			}
			//Impede o mov de cancelar a animação de pulo, tiro e de pegar.
			if (!(jump_state == WALKING_SHOT_LEFT) && !(jump_state == WALKING_SHOT_RIGHT) && !(jump_state == JUMPING) && !(jump_state == PEGAR)) {
				if (moving_left) {
					jump_state = IDLE_LEFT;
				}
				else {
					jump_state = IDLE_RIGHT;
				}	}
			mov = false;
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
		else if (pos_x_bullet < 0) {
			atirando = false;
		}
		al_draw_textf(font, al_map_rgb(0, 0, 0), 640, 360, ALLEGRO_ALIGN_CENTER, "%d", pos_y);

		if (event.keyboard.keycode == ALLEGRO_KEY_RIGHT && pos_x + 75 > 300) {
			pos_y += 3;
		}
		else if (event.keyboard.keycode == ALLEGRO_KEY_LEFT) {
			if (pos_y > 380) {
				pos_y -= 3;
			}
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
	ALLEGRO_FONT* font_realista = al_load_font("./fonte/airstrike.ttf", 40, 0);
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
		ALLEGRO_COLOR cor_padrao = al_map_rgb(255, 255, 255);
		ALLEGRO_COLOR cor_selecionada = al_map_rgb(000, 000, 000);

		al_draw_text(font_realista, controlador == 4 ? cor_selecionada : cor_padrao, 630, 176, ALLEGRO_ALIGN_CENTER, "JOGAR");
		al_draw_text(font_realista, controlador == 3 ? cor_selecionada : cor_padrao, 630, 320, ALLEGRO_ALIGN_CENTER, "OPCOES");
		al_draw_text(font_realista, controlador == 2 ? cor_selecionada : cor_padrao, 635, 467, ALLEGRO_ALIGN_CENTER, "TECLAS");
		al_draw_text(font_realista, controlador == 1 ? cor_selecionada : cor_padrao, 630, 618, ALLEGRO_ALIGN_CENTER, "CREDITOS");
		if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
			if (event.keyboard.keycode == ALLEGRO_KEY_UP) {
				controlador++;
				if (controlador > 4) {
					controlador = 1;
				}
			}
			else if (event.keyboard.keycode == ALLEGRO_KEY_DOWN) {
				controlador--;
				if (controlador < 1) {
					controlador = 4;
				}
			}
			else if (event.keyboard.keycode == ALLEGRO_KEY_ENTER) {
				switch (controlador) {
				case 4: tela1(); break;
				case 3: // Chamar função de opções
					break;
				case 2: // Chamar função de teclas
					break;
				case 1: // Chamar função de créditos
					break;
				}
			}
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
	tela1();
	//menu();
	return 0;
}
