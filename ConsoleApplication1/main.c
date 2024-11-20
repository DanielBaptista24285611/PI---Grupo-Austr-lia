#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/keyboard.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <stdio.h>

float FPS = 20.0;
bool inicializar_Allegro() {
	if (!al_init()) {
		fprintf(stderr, "Falha ao inicializar Allegro.\n");
		return false;
	}
	al_init_font_addon(); al_init_ttf_addon(); al_init_image_addon();
	if (!al_init_font_addon() || !al_init_ttf_addon() || !al_init_image_addon()) {
		fprintf(stderr, "Falha ao inicializar  addon.\n");
		return false;
	}
	
	al_install_keyboard(); 	al_install_audio(); al_init_acodec_addon();
	if (!al_install_keyboard() || !al_install_audio() || !al_init_acodec_addon()) {
		fprintf(stderr, "Falha ao instalar keyboard.\n");
		return false;
	}
	
	if (!al_init_video_addon()) { 
		fprintf(stderr, "Falha ao inicializar o addon de vídeo.\n"); return -1; }
	al_reserve_samples(1);
	if (!al_reserve_samples(1)) {
		fprintf(stderr, "Falha ao reservar amostra de áudío.\n");
		return false;
	}
	return true;
}

typedef struct {
	ALLEGRO_BITMAP* walkright; ALLEGRO_BITMAP* walkleft; ALLEGRO_BITMAP* jump_image; ALLEGRO_BITMAP* idle_right; ALLEGRO_BITMAP* idle_left;
	ALLEGRO_BITMAP* shot_image_right; ALLEGRO_BITMAP* shot_image_left; ALLEGRO_BITMAP* pegar_image; ALLEGRO_BITMAP* bullet_right; ALLEGRO_BITMAP* bullet_left;
	ALLEGRO_SAMPLE* tiro; ALLEGRO_BITMAP* pause; ALLEGRO_SAMPLE* bodyfall; ALLEGRO_SAMPLE* footsteps;
   }Recursos;

bool carregar_recursos(Recursos* recursos) {
	recursos->walkright = al_load_bitmap("./soldiersprites1/Soldier_1/walkright.png");
	recursos-> walkleft = al_load_bitmap("./soldiersprites1/Soldier_1/walkleft.png");
	recursos->jump_image = al_load_bitmap("./soldiersprites1/Soldier_1/jump.png");
	recursos->idle_right = al_load_bitmap("./soldiersprites1/Soldier_1/idleRight.png");
	recursos->idle_left = al_load_bitmap("./soldiersprites1/Soldier_1/idleLeft.png");
	recursos->shot_image_right = al_load_bitmap("./soldiersprites1/Soldier_1/shot_1_right.png");
	recursos->shot_image_left = al_load_bitmap("./soldiersprites1/Soldier_1/shot_1_left.png");
	recursos->pegar_image = al_load_bitmap("./soldiersprites2/soldier_2/rest.png");
	recursos->bullet_right = al_load_bitmap("./soldiersprites1/teste/bullet_right.png");
	recursos->bullet_left = al_load_bitmap("soldiersprites1/teste/bullet_left.png");
	recursos ->tiro = al_load_sample("./Audios/metralhadora.mp3");
	recursos->pause= al_load_bitmap("./backgrounds/pause.png");


	if (!recursos->walkright || !recursos->walkleft || !recursos->jump_image || !recursos->idle_right || !recursos->idle_left || !recursos->shot_image_right ||
		!recursos->shot_image_left || !recursos->pegar_image || !recursos->bullet_right || !recursos->bullet_left || !recursos->tiro || !recursos->pause) {
		
return false;
	}
	return true;
}

void liberar_recursos(Recursos* recursos) {
	
	if(recursos->walkright)al_destroy_bitmap(recursos->walkright);
	if(recursos->walkleft)al_destroy_bitmap(recursos->walkleft);
	if(recursos->idle_right)al_destroy_bitmap(recursos->idle_right);
	if(recursos->idle_left)al_destroy_bitmap(recursos->idle_left);
	if(recursos->shot_image_right)al_destroy_bitmap(recursos->shot_image_right);
	if(recursos->shot_image_left)al_destroy_bitmap(recursos->shot_image_left);
	if(recursos->jump_image)al_destroy_bitmap(recursos->jump_image);
	if(recursos->pegar_image)al_destroy_bitmap(recursos->pegar_image);
	if(recursos->bullet_right)al_destroy_bitmap(recursos->bullet_right);
	if(recursos->bullet_left)al_destroy_bitmap(recursos->bullet_left);
	if(recursos->tiro)al_destroy_sample(recursos->tiro);
	if (recursos->pause)al_destroy_bitmap(recursos->pause);
}

typedef struct {
	int posicao, pulo, velocy, velocup, plimit;
	float frame7px, frame4px, pos_x, pos_y, soldier_height, soldier_width;
	int current_frame_y;
	bool moving_left , moving_up, moving_down , moving_right , mov , atirando, jump_control;
	float pos_x_bullet, frame_rate, tempo_limite;
	float tempo_animacao, tempo_animacao2, tempo_control_finished;
	int max_frame, max_frame2;
    bool inicializado, bala_ativa;
	bool paused;
}Variaveis;

void Iniciar_variaveis(Variaveis* variaveis) {
	variaveis->posicao = 0, variaveis->pulo = 0, variaveis->velocy = 0, variaveis->velocup = 15, variaveis->plimit = 0;
	variaveis->frame7px = 0.f, variaveis->frame4px = 0.f, variaveis->pos_x = 0.0, variaveis->pos_y = 380.0, variaveis->soldier_height = 256.0, variaveis->soldier_width = 256.0;
	variaveis->current_frame_y = 0;
	variaveis->moving_left = false, variaveis->moving_up = false, variaveis->moving_down = false, variaveis->moving_right = false, variaveis->mov = false, variaveis->atirando = false, variaveis->jump_control = false;
	variaveis->pos_x_bullet = 0.1;
	variaveis->frame_rate = 0.15f, variaveis->tempo_limite = 5.0f;
	variaveis->tempo_animacao = 0.0, variaveis->tempo_animacao2 = 0.0, variaveis->tempo_control_finished = 0.0;
	variaveis->max_frame = 7, variaveis->max_frame2 = 4;
	variaveis->inicializado = false, variaveis->bala_ativa = false;
	variaveis->paused = false;
}

static int tela4() {
	al_init();

	if (!inicializar_Allegro()) {
		return -1;
	}
	Recursos recursos;
	if (!carregar_recursos(&recursos)) {
		fprintf(stderr, "Fallha ao carregar bitmaps.");
		return -1;
	}
		
	ALLEGRO_DISPLAY* display = al_create_display(1280, 720);
	al_set_window_position(display, 200, 200);

	//ALLEGRO_FONT* font = al_create_builtin_font();
	ALLEGRO_FONT* font = al_load_font("./fonte/font.ttf", 60, 0);
	ALLEGRO_TIMER* timer = al_create_timer(1.0 / FPS);
	//inimigo
	ALLEGRO_BITMAP* inimigoright = al_load_bitmap("./inimigos/inimigodireita.png");
	ALLEGRO_BITMAP* inimigoleft = al_load_bitmap("./inimigos/inimigoesquerda.png");
	//general
	ALLEGRO_BITMAP* generalright = al_load_bitmap("./inimigos/generaldireita.png");
	ALLEGRO_BITMAP* generalleft = al_load_bitmap("./inimigos/generalesquerda.png");
	//Background
	ALLEGRO_BITMAP* background_4 = al_load_bitmap("./base/sala_boss.jpg");
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
	Variaveis variaveis;
	Iniciar_variaveis(&variaveis);

	//VariÃ¡veis dos consumÃ­veis
	int kitmedicoX = 600, kitmunicaoX = 680;

	//VARIÁVEIS INIMIGO
	float frameinimigo = 0.f;
	int enemy_pos_x = 1200, enemy_pos_y = 600, enemy_velocity = 3; 
	bool enemy_moving_right = true, enemy_alive = true; 

	//CONTROLE ESTADO DO ENEMY
	typedef enum { INIMIGO_RIGHT, INIMIGO_LEFT
	} enemyState;

	enemyState current_enemy_state = INIMIGO_RIGHT;

	//TEMPORIZADOR PERSONAGEM ANDANDO!!
	float tempo_animacao_enemy = 0.0;
	int max_frame_enemy = 3;

	typedef enum { WALKING_RIGHT, WALKING_LEFT, WALKING_SHOT_RIGHT, WALKING_SHOT_LEFT, JUMPING, IDLE_LEFT, IDLE_RIGHT, PEGAR
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
			variaveis.pulo = 1;
			variaveis.velocy = -variaveis.velocup;
			variaveis.plimit = variaveis.pos_y - 140; //altura do pulo
			jump_state = JUMPING;
		}

		else if (variaveis.pulo) {
			variaveis.pos_y += variaveis.velocy;
			variaveis.velocy += 1;

			if (variaveis.pos_y >= 380) {
				variaveis.pos_y = 380;
				variaveis.pulo = 0;
				variaveis.velocy = 0;
				variaveis.jump_control = false;
				//Parar de pular ao encostar no chão
				if (variaveis.moving_left) jump_state = WALKING_LEFT;
				else jump_state = WALKING_RIGHT;
			}
		}

		switch (event.keyboard.keycode) {
		case ALLEGRO_KEY_LEFT:
			jump_state = WALKING_LEFT;
			variaveis.pos_x -= 5;
			variaveis.moving_left = true;
			variaveis.mov = true;
			//al_play_sample(recursos.footsteps, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
			break;
		case ALLEGRO_KEY_RIGHT:
			jump_state = WALKING_RIGHT;
			variaveis.pos_x += 5;
			variaveis.moving_left = false;
			variaveis.mov = true;
			//al_play_sample(recursos.footsteps, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
			break;
		case ALLEGRO_KEY_UP:
			variaveis.moving_up = true;
			variaveis.pos_y -= 5;
			break;
		case ALLEGRO_KEY_DOWN:
			variaveis.moving_down = true;
			variaveis.pos_y += 5;
			break;
		case ALLEGRO_KEY_V:
			jump_state = PEGAR;
			if (variaveis.pos_x > (kitmedicoX - 135) && variaveis.pos_x < (kitmedicoX - 70) && variaveis.pos_y + 128 > variaveis.pos_y && variaveis.pos_y < variaveis.pos_y + 100) {
				kitmedicoX = -1000;
			}
			else if (variaveis.pos_x > (kitmunicaoX - 120) && variaveis.pos_x < (kitmunicaoX - 10) && variaveis.pos_y + 128 > variaveis.pos_y && variaveis.pos_y < variaveis.pos_y + 100) {
				kitmunicaoX = -1000;
			}
			break;
		case ALLEGRO_KEY_B:
			if (variaveis.moving_left == true) {
				jump_state = WALKING_SHOT_LEFT;
				variaveis.atirando = true;
				variaveis.pos_x_bullet = variaveis.pos_x - 160;
				al_play_sample(recursos.tiro, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
			}
			else {
				jump_state = WALKING_SHOT_RIGHT;
				variaveis.atirando = true;
				variaveis.pos_x_bullet = variaveis.pos_x + 160;
				al_play_sample(recursos.tiro, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
			}
			break;
		case ALLEGRO_KEY_ESCAPE:
			variaveis.paused = !variaveis.paused;
			break;
		}
		

		if (event.type == ALLEGRO_EVENT_TIMER) {
			if (variaveis.paused) {
				al_clear_to_color(al_map_rgb(0, 0, 0));
				al_draw_bitmap(recursos.pause, 0, 0, 0);
				al_flip_display();
			return;
			}
			
			variaveis.tempo_animacao += 1.0 / FPS;

			if (variaveis.tempo_animacao >= variaveis.frame_rate) {
				variaveis.frame7px += 1.0f;
				if (variaveis.frame7px >= variaveis.max_frame) {
					variaveis.frame7px = 0; // Reinicia o quadro de animação
				}
				variaveis.tempo_animacao = 0.0f;
			}
			variaveis.tempo_animacao2 += 1.0 / FPS;
			if (variaveis.tempo_animacao2 >= variaveis.frame_rate) {
				variaveis.frame4px += 1.0f;
				if (variaveis.frame4px >= variaveis.max_frame2) {
					variaveis.frame4px = 0; // Reinicia o quadro de animação
				}
				variaveis.tempo_animacao2 = 0.0f;
			}
			tempo_animacao_enemy += 1.0 / FPS;
			if (tempo_animacao_enemy >= variaveis.frame_rate) {
				frameinimigo += 1.0f;
				if (frameinimigo >= max_frame_enemy) {
					frameinimigo = 0; // Reinicia o quadro de animação do inimigo
				}
				tempo_animacao_enemy = 0.0f;
			}

			al_clear_to_color(al_map_rgb(255, 255, 255));
			//Backgrounds
			al_draw_bitmap(background_4, 0, 0, 0);

			switch (jump_state) {
			case WALKING_RIGHT: al_draw_bitmap_region(recursos.walkright, 256 * (int)variaveis.frame7px, variaveis.current_frame_y, 256, 256, variaveis.pos_x, variaveis.pos_y, 0);
				break;

			case WALKING_LEFT: al_draw_bitmap_region(recursos.walkleft, 256 * (int)variaveis.frame7px, variaveis.current_frame_y, 256, 256, variaveis.pos_x, variaveis.pos_y, 0);
				break;

			case WALKING_SHOT_RIGHT: al_draw_bitmap_region(recursos.shot_image_right, 256 * (int)variaveis.frame4px, variaveis.current_frame_y, 256, 256, variaveis.pos_x, variaveis.pos_y, 0);
				break;

			case WALKING_SHOT_LEFT: al_draw_bitmap_region(recursos.shot_image_left, 256 * (int)variaveis.frame4px, variaveis.current_frame_y, 256, 256, variaveis.pos_x, variaveis.pos_y, 0);
				break;

			case JUMPING:if (variaveis.moving_left) { al_draw_bitmap_region(recursos.jump_image, 256 * (int)variaveis.frame7px, variaveis.current_frame_y, 256, 256, variaveis.pos_x, variaveis.pos_y, ALLEGRO_FLIP_HORIZONTAL);
				//Inverter imagem do pulo
			}
						else { al_draw_bitmap_region(recursos.jump_image, 256 * (int)variaveis.frame7px, variaveis.current_frame_y, 256, 256, variaveis.pos_x, variaveis.pos_y, 0);
			}
						break;
			case PEGAR:	al_draw_bitmap_region(recursos.pegar_image, 256 * (int)variaveis.frame4px, variaveis.current_frame_y, 256, 256, variaveis.pos_x, variaveis.pos_y, 0);
				break;

			case IDLE_LEFT: al_draw_bitmap_region(recursos.idle_left, 256 * (int)variaveis.frame7px, variaveis.current_frame_y, 256, 256, variaveis.pos_x, variaveis.pos_y, 0);
				break;

			case IDLE_RIGHT: al_draw_bitmap_region(recursos.idle_right, 256 * (int)variaveis.frame7px, variaveis.current_frame_y, 256, 256, variaveis.pos_x, variaveis.pos_y, 0);
				break;
			default:
				break;
			}
			//Impede o mov de cancelar a animação de pulo, tiro e de pegar.
			if (!(jump_state == WALKING_SHOT_LEFT) && !(jump_state == WALKING_SHOT_RIGHT) && !(jump_state == JUMPING) && !(jump_state == PEGAR)) {
				if (variaveis.moving_left) { jump_state = IDLE_LEFT; }
				else { jump_state = IDLE_RIGHT;	}
			}
			variaveis.mov = false;

			// Dentro do loop principal, na parte de colisão e atualização de estado
			if (variaveis.pulo && variaveis.velocy > 0) { // Se o personagem está caindo
				// Verificar se o personagem está em cima do inimigo
				if (variaveis.pos_x + variaveis.soldier_width > enemy_pos_x && variaveis.pos_x < enemy_pos_x + 61 && // colisão em X
					variaveis.pos_y + variaveis.soldier_height > enemy_pos_y && variaveis.pos_y + variaveis.soldier_height < enemy_pos_y + 30) { // colisão em Y
					enemy_alive = false; // Inimigo morre
					if (!variaveis.jump_control) {
						variaveis.velocy = -variaveis.velocup; // O personagem salta novamente
						variaveis.jump_control = true;
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
				case INIMIGO_RIGHT: al_draw_bitmap_region(inimigoleft, 61 * (int)frameinimigo, 0, 61, 64, enemy_pos_x, enemy_pos_y, 0);
					break;

				case INIMIGO_LEFT: al_draw_bitmap_region(inimigoright, 61 * (int)frameinimigo, 0, 61, 64, enemy_pos_x, enemy_pos_y, 0);
					break;
				default:
					break;
				}
			}
			else {
				variaveis.tempo_control_finished += 1.0 / FPS;
				al_draw_text(font, al_map_rgb(255, 255, 0), 500, 200, 0, "VITÓRIA!");
				if (variaveis.tempo_control_finished >= variaveis.tempo_limite) {
					break;
				}
			}
		}

		if (variaveis.atirando) {
			// Inicializa a posição da bala apenas uma vez
			if (variaveis.inicializado == false) {
				variaveis.bala_ativa = true;
				variaveis.inicializado = true;
			}

			// Desenha a bala e atualiza sua posição se estiver ativa
			if (variaveis.bala_ativa) {
				if (variaveis.moving_left == false) {
					al_draw_bitmap(recursos.bullet_right, variaveis.pos_x_bullet, variaveis.pos_y + 167, 0);
					variaveis.pos_x_bullet += 20.2f;
				}
				else {
					al_draw_bitmap(recursos.bullet_left, variaveis.pos_x_bullet, variaveis.pos_y + 167, 0);
					variaveis.pos_x_bullet -= 20.2f;
				}
			}
		}
		else { variaveis.inicializado = false; }

		if (variaveis.pos_x_bullet > 1280) { variaveis.atirando = false;}
		else if (variaveis.pos_x_bullet < 0) { variaveis.atirando = false; }

		al_flip_display();
	}
	
	//Backgrounds
	al_destroy_bitmap(background_4);
	//destroi fonts,eventos e tela
	al_destroy_font(font);
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);


	return 0;
}
static int tela3() {
	al_init();
	if (!inicializar_Allegro()) {
		return -1;
	}
	Recursos recursos;
	if (!carregar_recursos(&recursos)) {
		fprintf(stderr, "Fallha ao carregar bitmaps.");
		return -1;
	}

	ALLEGRO_DISPLAY* display = al_create_display(1280, 720);
	al_set_window_position(display, 200, 200);

	//ALLEGRO_FONT* font = al_create_builtin_font();
	ALLEGRO_FONT* font = al_load_font("./fonte/font.ttf", 60, 0);
	ALLEGRO_TIMER* timer = al_create_timer(1.0 / FPS);
	//Consumíveis
	ALLEGRO_BITMAP* kitmedico = al_load_bitmap("./Consumiveis/Kitmedico1.png");
	ALLEGRO_BITMAP* kitmunicao = al_load_bitmap("./consumiveis/kitmunicao1.png");
	//Background
	ALLEGRO_BITMAP* background_3 = al_load_bitmap("./backgrounds/fundo_base.jpg");
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
	Variaveis variaveis;
	Iniciar_variaveis(&variaveis);
	
	//VariÃ¡veis dos consumÃ­veis
	int kitmedicoX = 1080;
	int kitmunicaoX = 1160;

	typedef enum { WALKING_RIGHT, WALKING_LEFT, WALKING_SHOT_RIGHT, WALKING_SHOT_LEFT, JUMPING, IDLE_LEFT, IDLE_RIGHT, PEGAR
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
			variaveis.pulo = 1;
			variaveis.velocy = -variaveis.velocup;
			variaveis.plimit = variaveis.pos_y - 140; //altura do pulo
			jump_state = JUMPING;
		}

		else if (variaveis.pulo) {
			variaveis.pos_y += variaveis.velocy;
			variaveis.velocy += 1;

			if (variaveis.pos_y >= 380) {
				variaveis.pos_y = 380;
				variaveis.pulo = 0;
				variaveis.velocy = 0;
				//Parar de pular ao encostar no chão
				if (variaveis.moving_left) jump_state = WALKING_LEFT;
				else jump_state = WALKING_RIGHT;
			}
		}

		switch (event.keyboard.keycode) {
		case ALLEGRO_KEY_LEFT:
			jump_state = WALKING_LEFT;
			variaveis.pos_x -= 5;
			variaveis.moving_left = true;
			variaveis.mov = true;
			break;
		case ALLEGRO_KEY_RIGHT:
			jump_state = WALKING_RIGHT;
			variaveis.pos_x += 5;
			variaveis.moving_left = false;
			variaveis.mov = true;
			break;
		case ALLEGRO_KEY_UP:
			variaveis.moving_up = true;
			variaveis.pos_y -= 5;
			break;
		case ALLEGRO_KEY_DOWN:
			variaveis.moving_down = true;
			variaveis.pos_y += 5;
			break;
		case ALLEGRO_KEY_V:
			jump_state = PEGAR;
			if (variaveis.pos_x > (kitmedicoX - 135) && variaveis.pos_x < (kitmedicoX - 70) && variaveis.pos_y + 128 > variaveis.pos_y && variaveis.pos_y < variaveis.pos_y + 100) {
				kitmedicoX = -1000;
			}
			else if (variaveis.pos_x > (kitmunicaoX - 120) && variaveis.pos_x < (kitmunicaoX - 10) && variaveis.pos_y + 128 > variaveis.pos_y && variaveis.pos_y < variaveis.pos_y + 100) {
				kitmunicaoX = -1000;
			}
			break;
		case ALLEGRO_KEY_B:
			if (variaveis.moving_left == true) {
				jump_state = WALKING_SHOT_LEFT;
				variaveis.atirando = true;
				variaveis.pos_x_bullet = variaveis.pos_x - 160;
				al_play_sample(recursos.tiro, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
			}
			else {
				jump_state = WALKING_SHOT_RIGHT;
				variaveis.atirando = true;
				variaveis.pos_x_bullet = variaveis.pos_x + 160;
				al_play_sample(recursos.tiro, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
			}
			break;
		}

		switch (event.keyboard.keycode) {
		case ALLEGRO_KEY_LEFT: variaveis.moving_left = false;
			break;
		case ALLEGRO_KEY_RIGHT: variaveis.moving_right = false;
			break;
		case ALLEGRO_KEY_UP: variaveis.moving_up = false;
			break;
		case ALLEGRO_KEY_DOWN: variaveis.moving_down = false;
			break;
		}

		// Movimentos diagonais
		if (variaveis.moving_left && variaveis.moving_up) {
			variaveis.pos_x -= 4;
			variaveis.pos_y -= 4;
		}
		else if (variaveis.moving_right && variaveis.moving_up) {
			variaveis.pos_x += 4;
			variaveis.pos_y -= 4;
		}
		else if (variaveis.moving_left && variaveis.moving_down) {
			variaveis.pos_x -= 4;
			variaveis.pos_y += 4;
		}
		else if (variaveis.moving_right && variaveis.moving_down) {
			variaveis.pos_x += 4;
			variaveis.pos_y += 4;
		}

		if (event.type == ALLEGRO_EVENT_TIMER) {

			variaveis.tempo_animacao += 1.0 / FPS;

			if (variaveis.tempo_animacao >= variaveis.frame_rate) {
				variaveis.frame7px += 1.0f;
				if (variaveis.frame7px >= variaveis.max_frame) {
					variaveis.frame7px = 0; // Reinicia o quadro de animação
				}
				variaveis.tempo_animacao = 0.0f;
			}
			variaveis.tempo_animacao2 += 1.0 / FPS;
			if (variaveis.tempo_animacao2 >= variaveis.frame_rate) {
				variaveis.frame4px += 1.0f;
				if (variaveis.frame4px >= variaveis.max_frame2) {
					variaveis.frame4px = 0; // Reinicia o quadro de animação
				}
				variaveis.tempo_animacao2 = 0.0f;
			}

			al_clear_to_color(al_map_rgb(255, 255, 255));
			//Backgrounds
			al_draw_bitmap(background_3, 0, 0, 0);
			//Consumíveis
			al_draw_bitmap(kitmedico, kitmedicoX, 650, 0);
			al_draw_bitmap(kitmunicao, kitmunicaoX, 650, 0);

			switch (jump_state) {
			case WALKING_RIGHT: al_draw_bitmap_region(recursos.walkright, 256 * (int)variaveis.frame7px, variaveis.current_frame_y, 256, 256, variaveis.pos_x, variaveis.pos_y, 0);
				break;

			case WALKING_LEFT: al_draw_bitmap_region(recursos.walkleft, 256 * (int)variaveis.frame7px, variaveis.current_frame_y, 256, 256, variaveis.pos_x, variaveis.pos_y, 0);
				break;

			case WALKING_SHOT_RIGHT: al_draw_bitmap_region(recursos.shot_image_right, 256 * (int)variaveis.frame4px, variaveis.current_frame_y, 256, 256, variaveis.pos_x, variaveis.pos_y, 0);
				break;

			case WALKING_SHOT_LEFT: al_draw_bitmap_region(recursos.shot_image_left, 256 * (int)variaveis.frame4px, variaveis.current_frame_y, 256, 256, variaveis.pos_x, variaveis.pos_y, 0);
				break;

			case JUMPING:if (variaveis.moving_left) { al_draw_bitmap_region(recursos.jump_image, 256 * (int)variaveis.frame7px, variaveis.current_frame_y, 256, 256, variaveis.pos_x, variaveis.pos_y, ALLEGRO_FLIP_HORIZONTAL);
				//Inverter imagem do pulo
			}
						else { al_draw_bitmap_region(recursos.jump_image, 256 * (int)variaveis.frame7px, variaveis.current_frame_y, 256, 256, variaveis.pos_x, variaveis.pos_y, 0);
			}
						break;
			case PEGAR: al_draw_bitmap_region(recursos.pegar_image, 256 * (int)variaveis.frame4px, variaveis.current_frame_y, 256, 256, variaveis.pos_x, variaveis.pos_y, 0);
				break;

			case IDLE_LEFT: al_draw_bitmap_region(recursos.idle_left, 256 * (int)variaveis.frame7px, variaveis.current_frame_y, 256, 256, variaveis.pos_x, variaveis.pos_y, 0);
				break;

			case IDLE_RIGHT: al_draw_bitmap_region(recursos.idle_right, 256 * (int)variaveis.frame7px, variaveis.current_frame_y, 256, 256, variaveis.pos_x, variaveis.pos_y, 0);
				break;
			default:
				break;
			}
			//Impede o mov de cancelar a animação de pulo, tiro e de pegar.
			if (!(jump_state == WALKING_SHOT_LEFT) && !(jump_state == WALKING_SHOT_RIGHT) && !(jump_state == JUMPING) && !(jump_state == PEGAR)) {
				if (variaveis.moving_left) {
					jump_state = IDLE_LEFT;
				}
				else {
					jump_state = IDLE_RIGHT;
				}
			}
			variaveis.mov = false;
		}

		if (variaveis.atirando) {
			// Inicializa a posição da bala apenas uma vez
			if (variaveis.inicializado == false) {
				variaveis.bala_ativa = true;
				variaveis.inicializado = true;
			}
			// Desenha a bala e atualiza sua posição se estiver ativa
			if (variaveis.bala_ativa) {
				if (variaveis.moving_left == false) {
					al_draw_bitmap(recursos.bullet_right, variaveis.pos_x_bullet, variaveis.pos_y + 167, 0);
					variaveis.pos_x_bullet += 20.2f;
				}
				else {
					al_draw_bitmap(recursos.bullet_left, variaveis.pos_x_bullet, variaveis.pos_y + 167, 0);
					variaveis.pos_x_bullet -= 20.2f;
				}
			}
		}
		else { variaveis.inicializado = false; // Reseta quando não está atirando
		}

		if (variaveis.pos_x_bullet > 1280) {	variaveis.atirando = false;
		}
		else if (variaveis.pos_x_bullet < 0) { variaveis.atirando = false;
		}
		//MudanÃ§a de tela

		if (variaveis.pos_x > 777) { tela4();
		}

		al_flip_display();
	}	
	
	//Backgrounds
	al_destroy_bitmap(background_3);
	//destroi fonts,eventos e tela
	al_destroy_font(font);
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);

	return 0;
}

static int tela2() {
	al_init();
	if (!inicializar_Allegro()) {
		return -1;
	}
	Recursos recursos;
	if (!carregar_recursos(&recursos)) {
		fprintf(stderr, "Fallha ao carregar bitmaps.");
		return -1;
	}

	ALLEGRO_DISPLAY* display = al_create_display(1280, 720);
	al_set_window_position(display, 200, 200);

	//ALLEGRO_FONT* font = al_create_builtin_font();
	ALLEGRO_FONT* font = al_load_font("./fonte/font.ttf", 60, 0);
	ALLEGRO_TIMER* timer = al_create_timer(1.0 / FPS);;
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
	ALLEGRO_BITMAP* holograma = al_load_bitmap("./backgrounds/holograma.jpg");
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
	Variaveis variaveis;
	Iniciar_variaveis(&variaveis);
	
	//VARIÁVEIS INIMIGO
	float frameinimigo = 0.f;
	int enemy_pos_x = 1200;
	int enemy_pos_y = 600;
	int enemy_velocity = 3; // Velocidade do inimigo
	bool enemy_moving_right = true; // Direção do movimento do inimigo
	//CONTROLE ESTADO DO ENEMY
	typedef enum { INIMIGO_RIGHT, INIMIGO_LEFT
	} enemyState;

	enemyState current_enemy_state = INIMIGO_RIGHT;
	//VARIÁVEIS DO GENERAL
	float framegeneral = 0.3f;
	int gen_pos_x = 1050;
	int gen_pos_y = 580;
	int gen_velocity = 2; // Velocidade do generaL
	bool gen_moving_right = true; // Direção do movimento do general

	//CONTROLE DO ESTADO DO GENERAL
	typedef enum { GEN_RIGHT, GEN_LEFT
	} GenState;

	GenState current_gen_state = GEN_RIGHT;
	int kitmedicoX = 600, kitmunicaoX = 680;
	float tempo_animacao_enemy = 0.0;
	int max_frame_enemy = 3;

	//Variáveis das Estilizações alienigenas
	int EstAlien_pos_x = 200;
	int EstAlien_pos_y = 200;

	//Tronco e colisão
	int TRONCO_X_MIN = 320;
	int TRONCO_X_MAX = 350;
	int troncoX = 400;
	bool troncoVivo = true;

	typedef enum { WALKING_RIGHT, WALKING_LEFT, WALKING_SHOT_RIGHT, WALKING_SHOT_LEFT, JUMPING, IDLE_LEFT, IDLE_RIGHT, PEGAR
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
			variaveis.pulo = 1;
			variaveis.velocy = -variaveis.velocup;
			variaveis.plimit = variaveis.pos_y - 140; //altura do pulo
			jump_state = JUMPING;
		}

		else if (variaveis.pulo) {
			variaveis.pos_y += variaveis.velocy;
			variaveis.velocy += 1;

			if (variaveis.pos_y >= 380) {
				variaveis.pos_y = 380;
				variaveis.pulo = 0;
				variaveis.velocy = 0;
				//Parar de pular ao encostar no chão
				if (variaveis.moving_left) jump_state = WALKING_LEFT;
				else jump_state = WALKING_RIGHT;
			}
		}

		if (variaveis.pos_x + 75 > 320 && variaveis.pos_x < 350 && variaveis.pos_y + 128 > variaveis.pos_y && variaveis.pos_y < variaveis.pos_y + 100) {
			al_draw_text(font, al_map_rgb(0, 0, 0), 640, 360, ALLEGRO_ALIGN_CENTER, "Teste");
		}

		switch (event.keyboard.keycode) {
		case ALLEGRO_KEY_LEFT:
			jump_state = WALKING_LEFT;
			variaveis.pos_x -= 5;
			variaveis.moving_left = true;
			variaveis.mov = true;
			break;
		case ALLEGRO_KEY_RIGHT:
			jump_state = WALKING_RIGHT;
			variaveis.pos_x += 5;
			variaveis.moving_left = false;
			variaveis.mov = true;
			break;
		case ALLEGRO_KEY_V:
			jump_state = PEGAR;
			if (variaveis.pos_x > 465 && variaveis.pos_x < 530 && variaveis.pos_y + 128 > variaveis.pos_y && variaveis.pos_y < variaveis.pos_y + 100) {
				kitmedicoX = -1000;
			}
			else if (variaveis.pos_x > 560 && variaveis.pos_x < 670 && variaveis.pos_y + 128 > variaveis.pos_y && variaveis.pos_y < variaveis.pos_y + 100) {
				kitmunicaoX = -1000;
			}
			break;
		case ALLEGRO_KEY_B:
			if (variaveis.moving_left == true) {
				jump_state = WALKING_SHOT_LEFT;
				variaveis.atirando = true;
				variaveis.pos_x_bullet = variaveis.pos_x - 160;
				al_play_sample(recursos.tiro, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
			}
			else {
				jump_state = WALKING_SHOT_RIGHT;
				variaveis.atirando = true;
				variaveis.pos_x_bullet = variaveis.pos_x + 160;
				al_play_sample(recursos.tiro, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
			}
			break;
		}
		if (event.type == ALLEGRO_EVENT_TIMER) {

			variaveis.tempo_animacao += 1.0 / FPS;

			if (variaveis.tempo_animacao >= variaveis.frame_rate) {
				variaveis.frame7px += 1.0f;
				if (variaveis.frame7px >= variaveis.max_frame) {
					variaveis.frame7px = 0; // Reinicia o quadro de animação
				}
				variaveis.tempo_animacao = 0.0f;
			}
			variaveis.tempo_animacao2 += 1.0 / FPS;
			if (variaveis.tempo_animacao2 >= variaveis.frame_rate) {
				variaveis.frame4px += 1.0f;
				if (variaveis.frame4px >= variaveis.max_frame2) {
					variaveis.frame4px = 0; // Reinicia o quadro de animação
				}
				variaveis.tempo_animacao2 = 0.0f;
			}

			tempo_animacao_enemy += 1.0 / FPS;
			if (tempo_animacao_enemy >= variaveis.frame_rate) {
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
			//Cobras
			al_draw_bitmap_region(Estilizacao_Alien, 320, 135, 127, 120, 240, 500, 0);
			al_draw_bitmap_region(Estilizacao_Alien, 447, 150, 165, 100, 360, 510, 0);
			//Ossos
			al_draw_bitmap(ossinhos, 1120, 410, 0);
			al_draw_bitmap(Rei, 1070, 390, 0);
			//ObstÃ¡culos
			al_draw_bitmap(tronco3, 400, 516, 0);
			al_draw_bitmap(tronco3, troncoX, 516, 0);
			//Entrada base
			al_draw_bitmap(fundoporta, 780, 186, 0);
			al_draw_bitmap(porta, 980, 340, 0);
			al_draw_bitmap_region(Estilizacao_Alien, 0, 0, 300, 150, 760, 490, 0);
	
			switch (current_gen_state) {
			case GEN_RIGHT: al_draw_bitmap_region(generalright, 98 * (int)framegeneral, 0, 98, 102, gen_pos_x, gen_pos_y, 0);
				break;
			case GEN_LEFT: al_draw_bitmap_region(generalleft, 98 * (int)framegeneral, 0, 98, 102, gen_pos_x, gen_pos_y, 0);
				break;
			default:
				break;
			}
			

			switch (jump_state) {
			case WALKING_RIGHT: al_draw_bitmap_region(recursos.walkright, 256 * (int)variaveis.frame7px, variaveis.current_frame_y, 256, 256, variaveis.pos_x, variaveis.pos_y, 0);
				break;

			case WALKING_LEFT: al_draw_bitmap_region(recursos.walkleft, 256 * (int)variaveis.frame7px, variaveis.current_frame_y, 256, 256, variaveis.pos_x, variaveis.pos_y, 0);
				break;

			case WALKING_SHOT_RIGHT: al_draw_bitmap_region(recursos.shot_image_right, 256 * (int)variaveis.frame4px, variaveis.current_frame_y, 256, 256, variaveis.pos_x, variaveis.pos_y, 0);
				break;

			case WALKING_SHOT_LEFT: al_draw_bitmap_region(recursos.shot_image_left, 256 * (int)variaveis.frame4px, variaveis.current_frame_y, 256, 256, variaveis.pos_x, variaveis.pos_y, 0);
				break;

			case JUMPING:if (variaveis.moving_left) { al_draw_bitmap_region(recursos.jump_image, 256 * (int)variaveis.frame7px, variaveis.current_frame_y, 256, 256, variaveis.pos_x, variaveis.pos_y, ALLEGRO_FLIP_HORIZONTAL);
				//Inverter imagem do pulo
			}
						else { al_draw_bitmap_region(recursos.jump_image, 256 * (int)variaveis.frame7px, variaveis.current_frame_y, 256, 256, variaveis.pos_x, variaveis.pos_y, 0);
			}
						break;
			case PEGAR: al_draw_bitmap_region(recursos.pegar_image, 256 * (int)variaveis.frame4px, variaveis.current_frame_y, 256, 256, variaveis.pos_x, variaveis.pos_y, 0);
				break;

			case IDLE_LEFT: al_draw_bitmap_region(recursos.idle_left, 256 * (int)variaveis.frame7px, variaveis.current_frame_y, 256, 256, variaveis.pos_x, variaveis.pos_y, 0);
				break;

			case IDLE_RIGHT: al_draw_bitmap_region(recursos.idle_right, 256 * (int)variaveis.frame7px, variaveis.current_frame_y, 256, 256, variaveis.pos_x, variaveis.pos_y, 0);
				break;
			default:
				break;
			}
			//Impede o mov de cancelar a animação de pulo, tiro e de pegar.
			if (!(jump_state == WALKING_SHOT_LEFT) && !(jump_state == WALKING_SHOT_RIGHT) && !(jump_state == JUMPING) && !(jump_state == PEGAR)) {
				if (variaveis.moving_left) {
					jump_state = IDLE_LEFT;
				}
				else { jump_state = IDLE_RIGHT;
				}
			}
			variaveis.mov = false;
		}
		if (variaveis.pos_x > 1000) {
			//tela3();
			al_draw_bitmap(holograma, 0, 0, 0);
		}

		if (variaveis.atirando) {
			// Inicializa a posição da bala apenas uma vez
			if (variaveis.inicializado == false) {
				variaveis.bala_ativa = true;
				variaveis.inicializado = true;
			}

			// Desenha a bala e atualiza sua posição se estiver ativa
			if (variaveis.bala_ativa) {
				if (variaveis.moving_left == false) {
					al_draw_bitmap(recursos.bullet_right, variaveis.pos_x_bullet, variaveis.pos_y + 167, 0);
					variaveis.pos_x_bullet += 20.2f;
				}
				else {
					al_draw_bitmap(recursos.bullet_left, variaveis.pos_x_bullet, variaveis.pos_y + 167, 0);
					variaveis.pos_x_bullet -= 20.2f;
				}

				// Verifica se a bala atingiu o tronco
				if (troncoVivo) {
					if (variaveis.pos_x_bullet > TRONCO_X_MIN && variaveis.pos_x_bullet < TRONCO_X_MAX) {
						troncoX = -1000;
						variaveis.bala_ativa = false;
						variaveis.atirando = false;
						troncoVivo = false;
					}
				}
			}
		}
		else { variaveis.inicializado = false; // Reseta quando não está atirando
		}

		if (variaveis.pos_x_bullet > 1280) { variaveis.atirando = false;
		}
		else if (variaveis.pos_x_bullet < 0) { variaveis.atirando = false;
		}
		//MudanÃ§a de tela
		if (variaveis.pos_x < -60) { tela1();
		}

		al_flip_display();
	}
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
	//destroi fonts,eventos e tela
	al_destroy_font(font);
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);

	return 0;
}

static int tela1() {
	al_init();
	if (!inicializar_Allegro()) {
		return -1;
	}
	Recursos recursos;
	if (!carregar_recursos(&recursos)) {
		fprintf(stderr, "Fallha ao carregar bitmaps.");
		return -1;
	}

	ALLEGRO_DISPLAY* display1 = al_create_display(1280, 720);
	al_set_window_position(display1, 200, 200);

	//ALLEGRO_FONT* font = al_create_builtin_font();
	ALLEGRO_FONT* font = al_load_font("./fonte/font.ttf", 60, 0);
	ALLEGRO_TIMER* timer = al_create_timer(1.0 / FPS);
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
	ALLEGRO_SAMPLE* tiro = al_load_sample("./Audios/metralhadora.wav");
	//FONTE
	ALLEGRO_FONT* font_realista = al_load_font("./fonte/Seagram.ttf", 28, 0);
	ALLEGRO_COLOR cor_padrao = al_map_rgb(41, 43, 43);
	ALLEGRO_COLOR  cor_selecionada= al_map_rgb(215, 219, 218);
	
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
	
	Variaveis variaveis;
	Iniciar_variaveis(&variaveis);

	//Variáveis dos consumíveis
	int kitmedicoX = 600, kitmunicaoX = 680;

	//Variáveis das Estilizações alienigenas
	int EstAlien_pos_x = 60;
	int EstAlien_pos_y = 0;

	//Tronco e colisão
	int TRONCO_X_MIN = 320;
	int TRONCO_X_MAX = 350;
	int troncoX = 400;
	float tempo_pausa = 0.0f;
	float DURACAO_PAUSA = 3.0f;
	bool troncoVivo = true, shot_control = false;
	typedef enum { WALKING_RIGHT, WALKING_LEFT, WALKING_SHOT_RIGHT, WALKING_SHOT_LEFT, JUMPING, IDLE_LEFT, IDLE_RIGHT, PEGAR
	}characterState;

	characterState jump_state = IDLE_RIGHT; //Manter estado atual
	bool exit_game = false;

	while (!exit_game) {

		ALLEGRO_EVENT event;
		al_wait_for_event(event_queue, &event);

		if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			break;
		}
		//else if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) { return false; }

		else if (event.keyboard.keycode == ALLEGRO_KEY_SPACE) {
			variaveis.pulo = 1;
			variaveis.velocy = -variaveis.velocup;
			variaveis.plimit = variaveis.pos_y - 140; //altura do pulo
			jump_state = JUMPING;
		}

		else if (variaveis.pulo) {
			variaveis.pos_y += variaveis.velocy;
			variaveis.velocy += 1;

			if (variaveis.pos_y >= 380) {
				variaveis.pos_y = 380;
				variaveis.pulo = 0;
				variaveis.velocy = 0;
				//Parar de pular ao encostar no chão
				if (variaveis.moving_left) jump_state = WALKING_LEFT;
				else jump_state = WALKING_RIGHT;
			}
		}
		

		if (variaveis.pos_x + 75 > 320 && variaveis.pos_x < 350 && variaveis.pos_y + 128 > variaveis.pos_y && variaveis.pos_y < variaveis.pos_y + 100) {
			al_draw_text(font, al_map_rgb(0, 0, 0), 640, 360, ALLEGRO_ALIGN_CENTER, "Teste");
		}

		switch (event.keyboard.keycode) {
		case ALLEGRO_KEY_LEFT:
			jump_state = WALKING_LEFT;
			variaveis.pos_x -= 5;
			variaveis.moving_left = true;
			variaveis.mov = true;
			break;
		case ALLEGRO_KEY_RIGHT:
			jump_state = WALKING_RIGHT;
			variaveis.pos_x += 5;
			variaveis.moving_left = false;
			variaveis.mov = true;
			break;
		case ALLEGRO_KEY_V:
			jump_state = PEGAR;
			if (variaveis.pos_x > 465 && variaveis.pos_x < 530 && variaveis.pos_y + 128 > variaveis.pos_y && variaveis.pos_y < variaveis.pos_y + 100) {
				kitmedicoX = -1000;
			}
			else if (variaveis.pos_x > 560 && variaveis.pos_x < 670 && variaveis.pos_y + 128 > variaveis.pos_y && variaveis.pos_y < variaveis.pos_y + 100) {
				kitmunicaoX = -1000;
			}
			break;
		case ALLEGRO_KEY_B:
			if (variaveis.moving_left == true) {
				jump_state = WALKING_SHOT_LEFT;
				variaveis.atirando = true;
				variaveis.pos_x_bullet = variaveis.pos_x - 160;
				al_play_sample(tiro, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
			}
			else {
				jump_state = WALKING_SHOT_RIGHT;
				variaveis.atirando = true;
				variaveis.pos_x_bullet = variaveis.pos_x + 160;
				al_play_sample(tiro, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
			}
			break;
		case ALLEGRO_KEY_ESCAPE:
			variaveis.paused = !variaveis.paused;
			break;
		}
		int controlador = 1;
		if (variaveis.paused) {

			al_draw_bitmap(recursos.pause, 210, 120, 0);
			al_draw_text(font_realista, cor_padrao, 630, 214, ALLEGRO_ALIGN_CENTER, "PAUSE");
			al_draw_text(font_realista, controlador == 2 ? cor_padrao : cor_selecionada, 630, 323, ALLEGRO_ALIGN_CENTER, "CONTINUAR");
			al_draw_text(font_realista, controlador == 1 ? cor_padrao : cor_selecionada, 630, 405, ALLEGRO_ALIGN_CENTER, "SAIR");

			al_flip_display();
			continue;
		}
		if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
			if (event.keyboard.keycode == ALLEGRO_KEY_UP) {
				controlador++;
				if (controlador == 2) {
					controlador = 1;
				}
			}
			else if (event.keyboard.keycode == ALLEGRO_KEY_DOWN) {
				controlador--;
				if (controlador ==1) {
					controlador = 2;
				}
			}
			else if (event.keyboard.keycode == ALLEGRO_KEY_ENTER) {
				switch (controlador) {
				case 2: tela1(); break;
				case 1: menu();break;
				}
			}
		}
		
		

		if (event.type == ALLEGRO_EVENT_TIMER) {
			
			variaveis.tempo_animacao += 1.0 / FPS;

			if (variaveis.tempo_animacao >= variaveis.frame_rate) {
				variaveis.frame7px += 1.0f;
				if (variaveis.frame7px >= variaveis.max_frame) {
					variaveis.frame7px = 0; // Reinicia o quadro de animação
				}
				variaveis.tempo_animacao = 0.0f;
			}
			variaveis.tempo_animacao2 += 1.0 / FPS;
			if (variaveis.tempo_animacao2 >= variaveis.frame_rate) {
				variaveis.frame4px += 1.0f;
				if (variaveis.frame4px >= variaveis.max_frame2) {
					variaveis.frame4px = 0; // Reinicia o quadro de animação
				}
				variaveis.tempo_animacao2 = 0.0f;
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
			case WALKING_RIGHT: al_draw_bitmap_region(recursos.walkright, 256 * (int)variaveis.frame7px, variaveis.current_frame_y, 256, 256, variaveis.pos_x, variaveis.pos_y, 0);
				break;

			case WALKING_LEFT: al_draw_bitmap_region(recursos.walkleft, 256 * (int)variaveis.frame7px, variaveis.current_frame_y, 256, 256, variaveis.pos_x, variaveis.pos_y, 0);
				break;

			case WALKING_SHOT_RIGHT: al_draw_bitmap_region(recursos.shot_image_right, 256 * (int)variaveis.frame4px, variaveis.current_frame_y, 256, 256, variaveis.pos_x, variaveis.pos_y, 0);
				break;

			case WALKING_SHOT_LEFT: al_draw_bitmap_region(recursos.shot_image_left, 256 * (int)variaveis.frame4px, variaveis.current_frame_y, 256, 256, variaveis.pos_x, variaveis.pos_y, 0);
				break;

			case JUMPING:if (variaveis.moving_left) { al_draw_bitmap_region(recursos.jump_image, 256 * (int)variaveis.frame7px, variaveis.current_frame_y, 256, 256, variaveis.pos_x, variaveis.pos_y, ALLEGRO_FLIP_HORIZONTAL);
				//Inverter imagem do pulo
			}
						else { al_draw_bitmap_region(recursos.jump_image, 256 * (int)variaveis.frame7px, variaveis.current_frame_y, 256, 256, variaveis.pos_x, variaveis.pos_y, 0);
			}
						break;
			case PEGAR: al_draw_bitmap_region(recursos.pegar_image, 256 * (int)variaveis.frame4px, variaveis.current_frame_y, 256, 256, variaveis.pos_x, variaveis.pos_y, 0);
				break;

			case IDLE_LEFT: al_draw_bitmap_region(recursos.idle_left, 256 * (int)variaveis.frame7px, variaveis.current_frame_y, 256, 256, variaveis.pos_x, variaveis.pos_y, 0);
				break;

			case IDLE_RIGHT: al_draw_bitmap_region(recursos.idle_right, 256 * (int)variaveis.frame7px, variaveis.current_frame_y, 256, 256, variaveis.pos_x, variaveis.pos_y, 0);
				break;
			default:
				break;
			}
			//Impede o mov de cancelar a animação de pulo, tiro e de pegar.
			if (!(jump_state == WALKING_SHOT_LEFT) && !(jump_state == WALKING_SHOT_RIGHT) && !(jump_state == JUMPING) && !(jump_state == PEGAR)) {
				if (variaveis.moving_left) { jump_state = IDLE_LEFT;
				}
				else { jump_state = IDLE_RIGHT;
				}
			}
			variaveis.mov = false;
		}

		if (variaveis.atirando) {
			// Inicializa a posição da bala apenas uma vez
			if (variaveis.inicializado == false) {
				variaveis.bala_ativa = true;
				variaveis.inicializado = true;
			}
			// Desenha a bala e atualiza sua posição se estiver ativa
			if (variaveis.bala_ativa) {
				if (variaveis.moving_left == false) {
					al_draw_bitmap(recursos.bullet_right, variaveis.pos_x_bullet, variaveis.pos_y + 167, 0);
					variaveis.pos_x_bullet += 20.2f;
				}
				else {
					al_draw_bitmap(recursos.bullet_left, variaveis.pos_x_bullet, variaveis.pos_y + 167, 0);
					variaveis.pos_x_bullet -= 20.2f;
				}

				// Verifica se a bala atingiu o tronco
				if (troncoVivo) {
					if (variaveis.pos_x_bullet > TRONCO_X_MIN && variaveis.pos_x_bullet < TRONCO_X_MAX) {
						troncoX = -1000;
						variaveis.bala_ativa = false;
						variaveis.atirando = false;
						troncoVivo = false;
					}
				}
			}
		}
		else {
			variaveis.inicializado = false; // Reseta quando não está atirando
		}

		if (variaveis.pos_x_bullet > 1280) { variaveis.atirando = false;
		}
		else if (variaveis.pos_x_bullet < 0) { variaveis.atirando = false;
		}

		if (variaveis.pos_x > 1280) { tela2();
		}
		if (variaveis.pos_x < 0) variaveis.pos_x = 0; // Hitbox do inicio
		// Mecânica de escada
		if (event.keyboard.keycode == ALLEGRO_KEY_RIGHT && variaveis.pos_x + 75 < 225) { variaveis.pos_y += 3;
		}
		else if (event.keyboard.keycode == ALLEGRO_KEY_LEFT) {
			if (variaveis.pos_y > 380) { variaveis.pos_y -= 3;
			}
		}

		al_flip_display();
	}
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
	if (!inicializar_Allegro()) {
		return -1;
	}
	
	ALLEGRO_DISPLAY* display1 = al_create_display(1280, 720);
	al_set_window_position(display1, 200, 200);

	//IMAGENS
	ALLEGRO_BITMAP* menu_image = al_load_bitmap("./backgrounds/backgroundmenu.png");
	ALLEGRO_BITMAP* menu_opcoes_image = al_load_bitmap("./backgrounds/menu_opcoes_image.png");
	ALLEGRO_BITMAP* menu_teclas = al_load_bitmap("./backgrounds/fundo-teclas.png");
	ALLEGRO_BITMAP* menu_creditos = al_load_bitmap("./backgrounds/fundo-creditos.png");
	ALLEGRO_BITMAP* botao_voltar = al_load_bitmap("./backgrounds/botao-voltar.png");
	ALLEGRO_BITMAP* creditos = al_load_bitmap("./backgrounds/creditos.png");
	//ALLEGRO_FONT* font = al_create_builtin_font();
	ALLEGRO_FONT* font_realista = al_load_font("./fonte/Seagram.ttf", 28, 0);
	ALLEGRO_FONT* font_nome_jogo = al_load_font("./fonte/Gimme.ttf", 30, 0);
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
	int menu_frame_x = 0, menu_frame_y = 0, menu_pos_x = 0, menu_pos_y = 0, controlador = 1, estado_menu = 0;

	while (true) {
		//Background menu
		al_draw_bitmap(menu_image, 0, 0, 0);
		//Opções
		al_draw_bitmap(menu_opcoes_image, 450, 150, 0);

		ALLEGRO_EVENT event;
		al_wait_for_event(event_queue, &event);
		ALLEGRO_COLOR cor_padrao = al_map_rgb(41, 43, 43);
		ALLEGRO_COLOR cor_selecionada = al_map_rgb(215, 219, 218);
		ALLEGRO_COLOR credito = al_map_rgb(215, 219, 218);

		al_draw_text(font_nome_jogo, al_map_rgb(232, 237, 236), 620, 100, ALLEGRO_ALIGN_CENTER, "QUANTIC SOLDIER");
		al_draw_text(font_realista, controlador == 3 ? cor_padrao : cor_selecionada, 635, 285, ALLEGRO_ALIGN_CENTER, "JOGAR");
		al_draw_text(font_realista, controlador == 2 ? cor_padrao : cor_selecionada, 634, 430, ALLEGRO_ALIGN_CENTER, "TECLAS");
		al_draw_text(font_realista, controlador == 1 ? cor_padrao: cor_selecionada, 630, 576, ALLEGRO_ALIGN_CENTER, "CREDITOS");
		if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
			if (event.keyboard.keycode == ALLEGRO_KEY_UP) {
				controlador++;
				if (controlador > 3) { controlador = 1;
				}
			}
			else if (event.keyboard.keycode == ALLEGRO_KEY_DOWN) {
				controlador--;
				if (controlador < 1) { controlador = 3;
				}
			}
			else if (event.keyboard.keycode == ALLEGRO_KEY_ENTER) {
				switch (controlador) {
				case 3: tela1(); break;
				case 2: estado_menu = 1;
					break;
				case 1: estado_menu = 2;
					break;
				}
			}
		} else if (estado_menu == 1) {
			al_draw_bitmap(menu_teclas, 0, 0, 0);
			al_draw_text(font_realista,  cor_padrao, 630, 30, ALLEGRO_ALIGN_CENTER, "TECLAS");

			if ( event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
				estado_menu = 0;
			}
		} else if (estado_menu == 2) {
			al_draw_bitmap(menu_creditos, 0, 0, 0); 
			//al_draw_text(creditos,credido, 500, 300, 0);
			al_draw_text(font_realista, cor_padrao, 610, 30, ALLEGRO_ALIGN_CENTER, "CREDITOS");
			if ( event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
				estado_menu = 0;
			}
		}

		if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) { break; }

		al_flip_display();
	}
	//Destroi imagens
	al_destroy_bitmap(menu_image);
	al_destroy_bitmap(menu_opcoes_image);
	al_destroy_bitmap(menu_teclas);
	al_destroy_bitmap(menu_creditos);
	al_destroy_bitmap(botao_voltar);
	//destroi fonts,eventos,tela e timer
	al_destroy_font(font_realista);
	al_destroy_display(display1);
	al_destroy_event_queue(event_queue);
	al_destroy_timer(timer);
	return 0;
}
int main()
{
	menu();
	return 0;
}
