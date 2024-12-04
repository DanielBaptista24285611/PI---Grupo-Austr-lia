#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/keyboard.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <stdio.h>

#define MAX_INIMIGO 9

float FPS = 30.0;
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
		fprintf(stderr, "Falha ao inicializar o addon de vídeo.\n"); return -1;
	}
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
	recursos->walkleft = al_load_bitmap("./soldiersprites1/Soldier_1/walkleft.png");
	recursos->jump_image = al_load_bitmap("./soldiersprites1/Soldier_1/jump.png");
	recursos->idle_right = al_load_bitmap("./soldiersprites1/Soldier_1/idleRight.png");
	recursos->idle_left = al_load_bitmap("./soldiersprites1/Soldier_1/idleLeft.png");
	recursos->shot_image_right = al_load_bitmap("./soldiersprites1/Soldier_1/shot_1_right.png");
	recursos->shot_image_left = al_load_bitmap("./soldiersprites1/Soldier_1/shot_1_left.png");
	recursos->pegar_image = al_load_bitmap("./soldiersprites2/soldier_2/rest.png");
	recursos->bullet_right = al_load_bitmap("./soldiersprites1/teste/bullet_right.png");
	recursos->bullet_left = al_load_bitmap("soldiersprites1/teste/bullet_left.png");
	recursos->tiro = al_load_sample("./Audios/metralhadora.mp3");
	recursos->pause = al_load_bitmap("./backgrounds/pause.png");


	if (!recursos->walkright || !recursos->walkleft || !recursos->jump_image || !recursos->idle_right || !recursos->idle_left || !recursos->shot_image_right ||
		!recursos->shot_image_left || !recursos->pegar_image || !recursos->bullet_right || !recursos->bullet_left || !recursos->tiro || !recursos->pause) {

		return false;
	}
	return true;
}

void liberar_recursos(Recursos* recursos) {

	if (recursos->walkright)al_destroy_bitmap(recursos->walkright);
	if (recursos->walkleft)al_destroy_bitmap(recursos->walkleft);
	if (recursos->idle_right)al_destroy_bitmap(recursos->idle_right);
	if (recursos->idle_left)al_destroy_bitmap(recursos->idle_left);
	if (recursos->shot_image_right)al_destroy_bitmap(recursos->shot_image_right);
	if (recursos->shot_image_left)al_destroy_bitmap(recursos->shot_image_left);
	if (recursos->jump_image)al_destroy_bitmap(recursos->jump_image);
	if (recursos->pegar_image)al_destroy_bitmap(recursos->pegar_image);
	if (recursos->bullet_right)al_destroy_bitmap(recursos->bullet_right);
	if (recursos->bullet_left)al_destroy_bitmap(recursos->bullet_left);
	if (recursos->tiro)al_destroy_sample(recursos->tiro);
	if (recursos->pause)al_destroy_bitmap(recursos->pause);
}

typedef struct {
	int posicao, pulo, velocy, velocup, plimit, countholo, alternateControl,pegoumuni,pegoukit;
	float frame7px, frame4px, pos_x, pos_y, soldier_height, soldier_width;
	int current_frame_y;
	bool moving_left, moving_up, moving_down, moving_right, mov, atirando, jump_control;
	float pos_x_bullet, frame_rate, tempo_limite;
	float tempo_animacao, tempo_animacao2, tempo_control_finished;
	int max_frame, max_frame2;
	bool inicializado, bala_ativa, pulando, error;
	bool paused;
}Variaveis;

void Iniciar_variaveis(Variaveis* variaveis) {
	variaveis->posicao = 0, variaveis->pulo = 0, variaveis->velocy = 0, variaveis->velocup = 15, variaveis->plimit = 0, variaveis->alternateControl = 0, variaveis->pegoumuni = 0, variaveis->pegoukit = 0;
	variaveis->frame7px = 0.f, variaveis->frame4px = 0.f, variaveis->pos_x = 0.0, variaveis->pos_y = 380.0, variaveis->soldier_height = 256.0, variaveis->soldier_width = 256.0;
	variaveis->current_frame_y = 0;
	variaveis->moving_left = false, variaveis->moving_up = false, variaveis->moving_down = false, variaveis->moving_right = false, variaveis->mov = false, variaveis->atirando = false, variaveis->jump_control = false;
	variaveis->pos_x_bullet = 0.1;
	variaveis->frame_rate = 0.15f, variaveis->tempo_limite = 5.0f;
	variaveis->tempo_animacao = 0.0, variaveis->tempo_animacao2 = 0.0, variaveis->tempo_control_finished = 0.0;
	variaveis->max_frame = 7, variaveis->max_frame2 = 4;
	variaveis->inicializado = false, variaveis->bala_ativa = false, variaveis->pulando = false;
	variaveis->paused = false, variaveis->error = false;
}

int controlLife = 1;

void Interface_tela(bool error, ALLEGRO_BITMAP* barra_vida, int controlLife, ALLEGRO_BITMAP* est_interface, ALLEGRO_BITMAP* municao, ALLEGRO_BITMAP* sprite_arma, ALLEGRO_BITMAP* died) {
	ALLEGRO_COLOR transparencia = al_map_rgba(255, 255, 255, 128);
	if (error == false) {
		Variaveis variaveis;
		Iniciar_variaveis(&variaveis);

		al_draw_bitmap_region(est_interface, 160, 0, 270, 63, 330, 17, 0);
		al_draw_bitmap_region(est_interface, 265, 270, 160, 80, 0, 650, 0);
		al_draw_bitmap_region(municao, 100, 20, 120, 50, 390, 27, 0);
		al_draw_bitmap(sprite_arma, 30, 640, 0);
		al_draw_bitmap_region(barra_vida, 0, 0, 300, 55, 20, 30, 0);
		return;
	}
	else if (error == true) {
		switch (controlLife)
		{
		case 2:

			al_draw_bitmap_region(est_interface, 160, 0, 270, 63, 330, 17, 0);
			al_draw_bitmap_region(est_interface, 265, 270, 160, 80, 0, 650, 0);
			al_draw_bitmap_region(municao, 100, 20, 120, 50, 390, 27, 0);
			al_draw_bitmap(sprite_arma, 30, 640, 0);
			al_draw_bitmap_region(barra_vida, 0, 60, 300, 60, 20, 30, 0);
			return;
			break;
		case 3:
			al_draw_bitmap_region(est_interface, 160, 0, 270, 63, 330, 17, 0);
			al_draw_bitmap_region(est_interface, 265, 270, 160, 80, 0, 650, 0);
			al_draw_bitmap_region(municao, 100, 20, 120, 50, 390, 27, 0);
			al_draw_bitmap(sprite_arma, 30, 640, 0);
			al_draw_bitmap_region(barra_vida, 0, 130, 300, 55, 20, 30, 0);
			return;
			break;
		case 4:

			al_draw_bitmap_region(est_interface, 160, 0, 270, 63, 330, 17, 0);
			al_draw_bitmap_region(est_interface, 265, 270, 160, 80, 0, 650, 0);
			al_draw_bitmap_region(municao, 100, 20, 120, 50, 390, 27, 0);
			al_draw_bitmap(sprite_arma, 30, 640, 0);
			al_draw_bitmap_region(barra_vida, 0, 190, 300, 65, 20, 30, 0);
			return;
			break;
		case 5:
			al_draw_bitmap_region(est_interface, 160, 0, 270, 63, 330, 17, 0);
			al_draw_bitmap_region(est_interface, 265, 270, 160, 80, 0, 650, 0);
			al_draw_bitmap_region(municao, 100, 20, 120, 50, 390, 27, 0);
			al_draw_bitmap(sprite_arma, 30, 640, 0);
			al_draw_bitmap_region(barra_vida, 0, 260, 300, 55, 20, 30, 0);
			return;
			break;
		case 6:

			al_draw_tinted_bitmap(died, transparencia, 0, 0, 0);
			return;
			break;
		}
	}

	//Interface
	al_destroy_bitmap(barra_vida);
	al_destroy_bitmap(municao);
	al_destroy_bitmap(est_interface);
	al_destroy_bitmap(sprite_arma);
	al_destroy_bitmap(died);
}

typedef struct {
	int x, y, velocidade;
	int pos_x, pos_y;
	int estado;
	bool movendoDireita, Vivo;
	int soldier_width, soldier_height;
} inimig;
void Iniciar_inimigos(inimig* inimigos) {
	srand(time(NULL));

	int quantidade2 = MAX_INIMIGO;
	for (int i = 0; i < quantidade2; i++) {

		inimigos[i].x = rand() % 1200 + 50; // Posição inicial X aleatória	
		inimigos[i].y = 600; // Posição  Y	
		inimigos[i].velocidade = rand() % 3 + 1; // Velocidade aleatória entre 1 e 3	
		inimigos[i].movendoDireita = rand() % 2; // Direção inicial aleatória	
		inimigos[i].Vivo = true; // Inimigo começa vivo	
	}

	inimigos->pos_x = 100, inimigos->pos_y = 380, inimigos->soldier_width = 50, inimigos->soldier_height = 50;

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
	ALLEGRO_BITMAP* vitoria = al_load_bitmap("./base/vitoria.png");

	//Estilizações
	ALLEGRO_BITMAP* Est_futurista = al_load_bitmap("./Estilizacoes/Est_futurista.png");
	ALLEGRO_BITMAP* Est_futurista_remove = al_load_bitmap("./Estilizacoes/Est_futurista_remove.png");
	ALLEGRO_BITMAP* assets_keys = al_load_bitmap("./Estilizacoes/assets_keys.png");
	ALLEGRO_BITMAP* barra_vida = al_load_bitmap("./Estilizacoes/barra_vida.png");
	ALLEGRO_BITMAP* municao = al_load_bitmap("./Estilizacoes/municao.png");
	ALLEGRO_BITMAP* est_interface = al_load_bitmap("./Estilizacoes/interface.png");
	ALLEGRO_BITMAP* sprite_arma = al_load_bitmap("./Estilizacoes/sprite_arma.png");
	//Tela
	ALLEGRO_BITMAP* died = al_load_bitmap("./backgrounds/died.png");
	ALLEGRO_BITMAP* holograma = al_load_bitmap("./backgrounds/holograma.jpg");
	ALLEGRO_BITMAP* hologramaexp = al_load_bitmap("./backgrounds/hologramaexp.jpg");
	//FONT
	ALLEGRO_FONT* font_realista = al_load_font("./fonte/Seagram.ttf", 28, 0);
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

	inimig inimigos[MAX_INIMIGO];
	Iniciar_inimigos(&inimigos);

	//VariÃ¡veis dos consumÃ­veis
	int kitmedicoX = 600, kitmunicaoX = 680;

	//VARIÁVEIS INIMIGO
	float frameinimigo = 0.f;
	int enemy_pos_x = 1200, enemy_pos_y = 600, enemy_velocity = 3;
	bool enemy_moving_right = true, enemy_alive = true;
	bool running = true;
	bool redraw = false;

	//CONTROLE ESTADO DO ENEMY
	typedef enum {
		INIMIGO_RIGHT, INIMIGO_LEFT
	} enemyState;

	enemyState current_enemy_state = INIMIGO_RIGHT;
	int LIMITE_ESQUERDO = 5;
	int LIMITE_DIREITO = 1100;
	//TEMPORIZADOR PERSONAGEM ANDANDO!!
	float tempo_animacao_enemy = 0.0;
	int max_frame_enemy = 3;

	typedef enum {
		WALKING_RIGHT, WALKING_LEFT, WALKING_SHOT_RIGHT, WALKING_SHOT_LEFT, JUMPING, IDLE_LEFT, IDLE_RIGHT, PEGAR
	}characterState;

	characterState jump_state = IDLE_RIGHT; //Manter estado atual

	while (true) {

		ALLEGRO_EVENT event;
		al_wait_for_event(event_queue, &event);

		if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			break;
		}

		else if (event.keyboard.keycode == ALLEGRO_KEY_SPACE && !variaveis.pulando) {
			if (event.keyboard.keycode == ALLEGRO_KEY_RIGHT) {
				variaveis.pos_x += 10;
			}
			else if (event.keyboard.keycode == ALLEGRO_KEY_LEFT) {
				variaveis.pos_x -= 10;
				break;
			}
			variaveis.pulo = 1;
			variaveis.velocy = -variaveis.velocup;
			variaveis.plimit = variaveis.pos_y - 140; // Altura máxima do pulo
			jump_state = JUMPING; // Prioriza o estado de pulo
			variaveis.pulando = true;
		}

		// Lógica de pulo
		else if (variaveis.pulo) {
			switch (event.keyboard.keycode) {
			case ALLEGRO_KEY_RIGHT:
				variaveis.pos_x += 10;
				break;
			case ALLEGRO_KEY_LEFT:
				variaveis.pos_x -= 10;
				break;
			}
			variaveis.pos_y += variaveis.velocy;
			variaveis.velocy += 1;


			if (variaveis.pos_y >= 380) { // Retorna ao chão
				variaveis.pos_y = 380;
				variaveis.pulo = 0;
				variaveis.velocy = 0;
				variaveis.pulando = false;

				// Define o estado após o pulo com base no movimento
				if (variaveis.moving_left) {
					jump_state = WALKING_LEFT;
				}
				else {
					jump_state = WALKING_RIGHT;
				}
			}
		}

		// Movimentação horizontal
		if (!variaveis.pulando) { // Só muda estado se não estiver pulando
			switch (event.keyboard.keycode) {
			case ALLEGRO_KEY_LEFT:
				if (variaveis.pos_x > LIMITE_ESQUERDO) {
					jump_state = WALKING_LEFT;
					variaveis.pos_x -= 5;
					variaveis.moving_left = true;
					variaveis.mov = true;
				}
				break;

			case ALLEGRO_KEY_RIGHT:
				if (variaveis.pos_x < LIMITE_DIREITO) {
					jump_state = WALKING_RIGHT;
					variaveis.pos_x += 5;
					variaveis.moving_left = false;
					variaveis.mov = true;
				}
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
				if (variaveis.moving_left) {
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

			Interface_tela(variaveis.error, barra_vida, controlLife, est_interface, municao, sprite_arma, died);
			switch (jump_state) {
			case WALKING_RIGHT: al_draw_bitmap_region(recursos.walkright, 256 * (int)variaveis.frame7px, variaveis.current_frame_y, 256, 256, variaveis.pos_x, variaveis.pos_y, 0);
				break;

			case WALKING_LEFT: al_draw_bitmap_region(recursos.walkleft, 256 * (int)variaveis.frame7px, variaveis.current_frame_y, 256, 256, variaveis.pos_x, variaveis.pos_y, 0);
				break;

			case WALKING_SHOT_RIGHT: al_draw_bitmap_region(recursos.shot_image_right, 256 * (int)variaveis.frame4px, variaveis.current_frame_y, 256, 256, variaveis.pos_x, variaveis.pos_y, 0);
				break;

			case WALKING_SHOT_LEFT: al_draw_bitmap_region(recursos.shot_image_left, 256 * (int)variaveis.frame4px, variaveis.current_frame_y, 256, 256, variaveis.pos_x, variaveis.pos_y, 0);
				break;

			case JUMPING:if (variaveis.moving_left) {
				al_draw_bitmap_region(recursos.jump_image, 256 * (int)variaveis.frame7px, variaveis.current_frame_y, 256, 256, variaveis.pos_x, variaveis.pos_y, ALLEGRO_FLIP_HORIZONTAL);
				//Inverter imagem do pulo
			}
						else {
				al_draw_bitmap_region(recursos.jump_image, 256 * (int)variaveis.frame7px, variaveis.current_frame_y, 256, 256, variaveis.pos_x, variaveis.pos_y, 0);
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
				else { jump_state = IDLE_RIGHT; }
			}
			variaveis.mov = false;

			if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
				running = false;
			}
			else if (event.type == ALLEGRO_EVENT_TIMER) {
				redraw = true;

				// Atualiza os inimigos
				for (int i = 0; i < MAX_INIMIGO; i++) {
					if (!inimigos[i].Vivo) continue;

					// Movimenta os inimigos (direção)
					if (inimigos[i].movendoDireita) {
						inimigos[i].x += inimigos[i].velocidade;
						inimigos[i].estado = INIMIGO_RIGHT; // Atualiza o estado para a direção correta
						if (inimigos[i].x > 1280 - 61) { // 61 é a largura do sprite
							inimigos[i].movendoDireita = false;
						}
					}
					else {
						inimigos[i].x -= inimigos[i].velocidade;
						inimigos[i].estado = INIMIGO_LEFT; // Atualiza o estado para a direção correta
						if (inimigos[i].x < 0) {
							inimigos[i].movendoDireita = true;
						}
					}
				}

				// Verifica colisão entre o pulo do jogador e os inimigos
				for (int i = 0; i < MAX_INIMIGO; i++) {
					if (inimigos[i].Vivo &&
						variaveis.pos_x + variaveis.soldier_width > inimigos[i].x && // Colisão em X
						variaveis.pos_x < inimigos[i].x + 61 &&
						variaveis.pos_y + variaveis.soldier_height > inimigos[i].y && // Colisão em Y
						variaveis.pos_y + variaveis.soldier_height < inimigos[i].y + 30) { // Apenas na parte superior do inimigo

						// Verifica se o soldado está caindo
						if (variaveis.velocy > 0) {
							inimigos[i].Vivo = false; // Elimina o inimigo ao colidir

							// Reinicia o pulo automaticamente
							variaveis.velocy = -variaveis.velocup; // Define a velocidade do novo pulo
							variaveis.jump_control = true; // Ativa o controle do pulo
						}
					}
				}

				// Verifica se todos os inimigos foram derrotados
				bool todosDerrotados = true;
				for (int i = 0; i < MAX_INIMIGO; i++) {
					if (inimigos[i].Vivo) {
						todosDerrotados = false;
						break;
					}
				}

				if (todosDerrotados) {
					variaveis.tempo_control_finished += 1.0 / FPS;
					al_draw_bitmap(vitoria, 0, 0, 0);
					if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
						menu();
					}
				}
			}

			// Renderiza os inimigos
			for (int i = 0; i < MAX_INIMIGO; i++) {
				if (inimigos[i].Vivo) {
					switch (inimigos[i].estado) {
					case INIMIGO_RIGHT:
						al_draw_bitmap_region(inimigoleft, 61 * (int)frameinimigo, 0, 61, 64, inimigos[i].x, inimigos[i].y, 0);
						break;
					case INIMIGO_LEFT:
						al_draw_bitmap_region(inimigoright, 61 * (int)frameinimigo, 0, 61, 64, inimigos[i].x, inimigos[i].y, 0);
						break;
					default:
						break;
					}
				}
			}
			float largura = 195.0;

			if (variaveis.pos_x > -50 && (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)) {

				variaveis.countholo = 1;

				bool alternate = true;
				int controlQuest2 = 1;

				// Loop principal do menu de alternativas
				while (alternate) {
					ALLEGRO_EVENT evento;
					al_wait_for_event(event_queue, &evento);

					if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
						alternate = false;
						break;
					}

					if (evento.type == ALLEGRO_EVENT_KEY_DOWN) {
						switch (evento.keyboard.keycode) {
						case ALLEGRO_KEY_UP:
							controlQuest2++;
							if (controlQuest2 > 2) {
								controlQuest2 = 1;
								largura -= 460; // Reset para a posição inicial
							}
							else {
								largura += 230; // Avança para a próxima posição
							}
							break;

						case ALLEGRO_KEY_DOWN:
							controlQuest2--;
							if (controlQuest2 < 1) {
								controlQuest2 = 2;
								largura += 460; // Reset para a última posição
							}
							else {
								largura -= 230; // Retorna à posição anterior
							}
							break;
						case ALLEGRO_KEY_ENTER:
							// Confirma a alternativa escolhida
							alternate = false;
							break;

						}
					}

					ALLEGRO_COLOR cor_padrao = al_map_rgb(41, 43, 43);
					ALLEGRO_COLOR cor_selecionada = al_map_rgb(215, 219, 218);

					al_draw_bitmap(recursos.pause, 210, 120, 0);
					al_draw_text(font_realista, cor_padrao, 630, 214, ALLEGRO_ALIGN_CENTER, "PAUSE");

					// Mostra as alternativas com destaque para a selecionada
					switch (controlQuest2) {
					case 1:
						al_draw_text(font_realista, cor_selecionada, 630, 323, ALLEGRO_ALIGN_CENTER, "CONTINUAR");
						al_draw_text(font_realista, cor_padrao, 630, 405, ALLEGRO_ALIGN_CENTER, "SAIR");

						break;
					case 2:
						al_draw_text(font_realista, cor_padrao, 630, 323, ALLEGRO_ALIGN_CENTER, "CONTINUAR");
						al_draw_text(font_realista, cor_selecionada, 630, 405, ALLEGRO_ALIGN_CENTER, "SAIR");
						break;
					}

					al_flip_display();
				}
				if (controlQuest2 == 2) {
					menu();
				}
				else if (controlQuest2 == 1) {
					return tela4();
				}
				al_flip_display();
				al_rest(2.0);

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

			if (variaveis.pos_x_bullet > 1280) { variaveis.atirando = false; }
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
		ALLEGRO_FONT* font = al_load_font("./fonte/font.ttf", 25, 0);
		ALLEGRO_TIMER* timer = al_create_timer(1.0 / FPS);
		//Consumíveis
		ALLEGRO_BITMAP* kitmedico = al_load_bitmap("./Consumiveis/Kitmedico1.png");
		ALLEGRO_BITMAP* kitmunicao = al_load_bitmap("./consumiveis/kitmunicao1.png");
		ALLEGRO_FONT* font_realista = al_load_font("./fonte/Seagram.ttf", 28, 0);
		//INIMIGO
		ALLEGRO_BITMAP* inimigo = al_load_bitmap("./inimigos/inimigodireita.png");
		//Background
		ALLEGRO_BITMAP* background_3 = al_load_bitmap("./backgrounds/fundo_base.jpg");

		//Estilizações
		ALLEGRO_BITMAP* Est_futurista = al_load_bitmap("./Estilizacoes/Est_futurista.png");
		ALLEGRO_BITMAP* Est_futurista_remove = al_load_bitmap("./Estilizacoes/Est_futurista_remove.png");
		ALLEGRO_BITMAP* assets_keys = al_load_bitmap("./Estilizacoes/assets_keys.png");
		ALLEGRO_BITMAP* barra_vida = al_load_bitmap("./Estilizacoes/barra_vida.png");
		ALLEGRO_BITMAP* municao = al_load_bitmap("./Estilizacoes/municao.png");
		ALLEGRO_BITMAP* est_interface = al_load_bitmap("./Estilizacoes/interface.png");
		ALLEGRO_BITMAP* sprite_arma = al_load_bitmap("./Estilizacoes/sprite_arma.png");
		//Tela
		ALLEGRO_BITMAP* died = al_load_bitmap("./backgrounds/died.png");
		ALLEGRO_BITMAP* holograma = al_load_bitmap("./backgrounds/holograma.jpg");
		ALLEGRO_BITMAP* hologramaexp = al_load_bitmap("./backgrounds/hologramaexp.jpg");
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

		typedef enum {
			INIMIGO_RIGHT, INIMIGO_LEFT
		} enemyState;

		enemyState current_enemy_state = INIMIGO_RIGHT;
		Variaveis variaveis;
		Iniciar_variaveis(&variaveis);

		inimig inimigos[MAX_INIMIGO];
		Iniciar_inimigos(&inimigos);

		int LIMITE_ESQUERDO = 5;
		int LIMITE_DIREITO = 1100;
		bool running = true;
		bool redraw = false;

		//VariÃ¡veis dos consumÃ­veis
		int kitmedicoX = 1080;
		int kitmunicaoX = 1160;

		typedef enum {
			WALKING_RIGHT, WALKING_LEFT, WALKING_SHOT_RIGHT, WALKING_SHOT_LEFT, JUMPING, IDLE_LEFT, IDLE_RIGHT, PEGAR
		}characterState;

		characterState jump_state = WALKING_RIGHT; //Manter estado atual

		while (true) {

			ALLEGRO_EVENT event;
			al_wait_for_event(event_queue, &event);

			if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
				break;
			}

			else if (event.keyboard.keycode == ALLEGRO_KEY_SPACE && !variaveis.pulando) {
				if (event.keyboard.keycode == ALLEGRO_KEY_RIGHT) {
					variaveis.pos_x += 10;
				}
				else if (event.keyboard.keycode == ALLEGRO_KEY_LEFT) {
					variaveis.pos_x -= 10;
					break;
				}
				variaveis.pulo = 1;
				variaveis.velocy = -variaveis.velocup;
				variaveis.plimit = variaveis.pos_y - 140; // Altura máxima do pulo
				jump_state = JUMPING; // Prioriza o estado de pulo
				variaveis.pulando = true;
			}

			// Lógica de pulo
			else if (variaveis.pulo) {
				switch (event.keyboard.keycode) {
				case ALLEGRO_KEY_RIGHT:
					variaveis.pos_x += 10;
					break;
				case ALLEGRO_KEY_LEFT:
					variaveis.pos_x -= 10;
					break;
				}
				variaveis.pos_y += variaveis.velocy;
				variaveis.velocy += 1;


				if (variaveis.pos_y >= 380) { // Retorna ao chão
					variaveis.pos_y = 380;
					variaveis.pulo = 0;
					variaveis.velocy = 0;
					variaveis.pulando = false;

					// Define o estado após o pulo com base no movimento
					if (variaveis.moving_left) {
						jump_state = WALKING_LEFT;
					}
					else {
						jump_state = WALKING_RIGHT;
					}
				}
			}

			// Movimentação horizontal
			if (!variaveis.pulando) { // Só muda estado se não estiver pulando
				switch (event.keyboard.keycode) {
				case ALLEGRO_KEY_LEFT:
					if (variaveis.pos_x > LIMITE_ESQUERDO) {
						jump_state = WALKING_LEFT;
						variaveis.pos_x -= 5;
						variaveis.moving_left = true;
						variaveis.mov = true;
					}
					break;

				case ALLEGRO_KEY_RIGHT:
					if (variaveis.pos_x < LIMITE_DIREITO) {
						jump_state = WALKING_RIGHT;
						variaveis.pos_x += 5;
						variaveis.moving_left = false;
						variaveis.mov = true;
					}
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
					if (variaveis.moving_left) {
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
			}

			switch (event.keyboard.keycode) {
			case ALLEGRO_KEY_LEFT: variaveis.moving_left = true;
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
			}


			


			al_clear_to_color(al_map_rgb(255, 255, 255));
			//Backgrounds
			al_draw_bitmap(background_3, 0, 0, 0);
			//Consumíveis
			al_draw_bitmap(kitmedico, kitmedicoX, 620, 0);
			al_draw_bitmap(kitmunicao, kitmunicaoX, 620, 0);

			switch (jump_state) {
			case WALKING_RIGHT: al_draw_bitmap_region(recursos.walkright, 256 * (int)variaveis.frame7px, variaveis.current_frame_y, 256, 256, variaveis.pos_x, variaveis.pos_y, 0);
				break;

			case WALKING_LEFT: al_draw_bitmap_region(recursos.walkleft, 256 * (int)variaveis.frame7px, variaveis.current_frame_y, 256, 256, variaveis.pos_x, variaveis.pos_y, 0);
				break;

			case WALKING_SHOT_RIGHT: al_draw_bitmap_region(recursos.shot_image_right, 256 * (int)variaveis.frame4px, variaveis.current_frame_y, 256, 256, variaveis.pos_x, variaveis.pos_y, 0);
				break;

			case WALKING_SHOT_LEFT: al_draw_bitmap_region(recursos.shot_image_left, 256 * (int)variaveis.frame4px, variaveis.current_frame_y, 256, 256, variaveis.pos_x, variaveis.pos_y, 0);
				break;

			case JUMPING:if (variaveis.moving_left) {
				al_draw_bitmap_region(recursos.jump_image, 256 * (int)variaveis.frame7px, variaveis.current_frame_y, 256, 256, variaveis.pos_x, variaveis.pos_y, ALLEGRO_FLIP_HORIZONTAL);
				//Inverter imagem do pulo
			}
						else {
				al_draw_bitmap_region(recursos.jump_image, 256 * (int)variaveis.frame7px, variaveis.current_frame_y, 256, 256, variaveis.pos_x, variaveis.pos_y, 0);
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

			if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
				running = false;
			}
			else if (event.type == ALLEGRO_EVENT_TIMER) {
				redraw = true;

				for (int i = 0; i < MAX_INIMIGO; i++) {
					if (!inimigos[i].Vivo) continue;

					if (inimigos[i].movendoDireita) {
						inimigos[i].x += inimigos[i].velocidade;
						inimigos[i].estado = INIMIGO_RIGHT; // Atualiza o estado para a direção correta
						if (inimigos[i].x > 1280 - 61) { // 61 é a largura do sprite
							inimigos[i].movendoDireita = false;
						}
					}
					else {
						inimigos[i].x -= inimigos[i].velocidade;
						inimigos[i].estado = INIMIGO_LEFT; // Atualiza o estado para a direção correta
						if (inimigos[i].x < 0) {
							inimigos[i].movendoDireita = true;
						}
					}
				}
			}
			bool todosDerotados = true;
			for (int i = 0; i < MAX_INIMIGO; i++) {
				if (variaveis.pulo) {
					if (inimigos[i].Vivo &&
						variaveis.pos_x < inimigos[i].x + 61 && variaveis.pos_x + variaveis.soldier_width > inimigos[i].x &&
						variaveis.pos_y + variaveis.soldier_height < inimigos[i].y + 30 && variaveis.pos_y + variaveis.soldier_height > inimigos[i].y) {

						if (variaveis.velocy > 0) {
							inimigos[i].Vivo = false; // Elimina o inimigo ao colidir

							// Reinicia o pulo automaticamente
							variaveis.velocy = -variaveis.velocup; // Define a velocidade do novo pulo
							variaveis.jump_control = true; // Ativa o controle do pulo
						}
					}
				}
			}
			for (int i = 0; i < MAX_INIMIGO; i++) {
				if (inimigos[i].Vivo) {
					todosDerotados = false;
					al_draw_bitmap(inimigo, inimigos[i].x, inimigos[i].y, 0);

				}
			}


			float largura = 195.0;

			if (variaveis.pos_x > -50 && (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)) {

				variaveis.countholo = 1;

				bool alternate = true;
				int controlQuest2 = 1;

				// Loop principal do menu de alternativas
				while (alternate) {
					ALLEGRO_EVENT evento;
					al_wait_for_event(event_queue, &evento);

					if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
						alternate = false;
						break;
					}

					if (evento.type == ALLEGRO_EVENT_KEY_DOWN) {
						switch (evento.keyboard.keycode) {
						case ALLEGRO_KEY_UP:
							controlQuest2++;
							if (controlQuest2 > 2) {
								controlQuest2 = 1;
								largura -= 460; // Reset para a posição inicial
							}
							else {
								largura += 230; // Avança para a próxima posição
							}
							break;

						case ALLEGRO_KEY_DOWN:
							controlQuest2--;
							if (controlQuest2 < 1) {
								controlQuest2 = 2;
								largura += 460; // Reset para a última posição
							}
							else {
								largura -= 230; // Retorna à posição anterior
							}
							break;
						case ALLEGRO_KEY_ENTER:
							// Confirma a alternativa escolhida
							alternate = false;
							break;

						}
					}

					ALLEGRO_COLOR cor_padrao = al_map_rgb(41, 43, 43);
					ALLEGRO_COLOR cor_selecionada = al_map_rgb(215, 219, 218);

					al_draw_bitmap(recursos.pause, 210, 120, 0);
					al_draw_text(font_realista, cor_padrao, 630, 214, ALLEGRO_ALIGN_CENTER, "PAUSE");

					// Mostra as alternativas com destaque para a selecionada
					switch (controlQuest2) {
					case 1:
						al_draw_text(font_realista, cor_selecionada, 630, 323, ALLEGRO_ALIGN_CENTER, "CONTINUAR");
						al_draw_text(font_realista, cor_padrao, 630, 405, ALLEGRO_ALIGN_CENTER, "SAIR");

						break;
					case 2:
						al_draw_text(font_realista, cor_padrao, 630, 323, ALLEGRO_ALIGN_CENTER, "CONTINUAR");
						al_draw_text(font_realista, cor_selecionada, 630, 405, ALLEGRO_ALIGN_CENTER, "SAIR");
						break;
					}

					al_flip_display();
				}
				if (controlQuest2 == 2) {
					menu();
				}
				else if (controlQuest2 == 1) {
					alternate = false;
				}
				al_flip_display();
				al_rest(2.0);

			}

			if (variaveis.pos_x > 634 && variaveis.pos_x < 950) {
				al_draw_text(font, al_map_rgb(255, 255, 255), 890, 370, ALLEGRO_ALIGN_CENTER, "Press ");
				al_draw_bitmap_region(assets_keys, 240, 110, 60, 40, 935, 367, 0);
			}
			Interface_tela(variaveis.error, barra_vida, controlLife, est_interface, municao, sprite_arma, died);
			if (variaveis.pos_x > 634 && (event.keyboard.keycode == ALLEGRO_KEY_R)) {
				variaveis.countholo = 1;

				float largura_original = al_get_bitmap_width(holograma);
				float altura_original = al_get_bitmap_height(holograma);

				// Definindo o tamanho final da tela (1280x720)
				float largura_tela = 1280.0f;
				float altura_tela = 720.0f;
				float largura = 195.0;
				int total_alternativas = 3;
				int alternativa_selecionada = 0;

				// Variáveis de escala
				float escala_x = largura_tela / largura_original;   // Fator de escala na direção X
				float escala_y = altura_tela / altura_original;     // Fator de escala na direção Y
				float escala = 1.0f; // Escala inicial de 1.0 (tamanho original da imagem)

				float tempo_inicial = al_get_time(); // Tempo inicial para controle de animação

				// Controle de loop
				bool abrindo = true;

				int menu_frame_x = 0, menu_frame_y = 0, menu_pos_x = 0, menu_pos_y = 0, controlQuest = 1, Alternativa = 0;
				// Loop principal
				while (abrindo) {
					// Controla o tempo de aumento da escala (1.5 segundos)

					float tempo_decorrido = al_get_time() - tempo_inicial;

					if (tempo_decorrido < 1.5) {
						// A escala aumenta gradualmente até atingir o tamanho final em 1.5 segundos
						escala = 1.0f + (escala_x - 1.0f) * (tempo_decorrido / 1.5f); // Crescimento suave
					}
					else {
						escala = escala_x; // A escala final é o valor correspondente ao tamanho da tela
					}
					// Calcula a posição para centralizar a imagem escalada
					float centro_x = (largura_tela - (largura_original * escala)) / 2; // Centraliza horizontalmente
					float centro_y = (altura_tela - (altura_original * escala)) / 2; // Centraliza verticalmente

					ALLEGRO_COLOR cor_padrao = al_map_rgb(41, 43, 43);
					ALLEGRO_COLOR cor_selecionada = al_map_rgb(215, 219, 218);
					ALLEGRO_COLOR Resposta = al_map_rgb(215, 219, 218);

					bool alternate = true;

					while (alternate) {
						ALLEGRO_EVENT evento;
						al_wait_for_event(event_queue, &evento);
						//HOLOGRAMA
						al_draw_scaled_bitmap(
							holograma,
							0, 0, // Coordenadas de origem na imagem
							largura_original, altura_original, // Dimensões da imagem original
							centro_x, centro_y, // Posição centralizada
							largura_original * escala, altura_original * escala, // Dimensões escaladas
							0 // Sem flags
						);
						if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
							alternate = false;
							break;
						}
						if (evento.type == ALLEGRO_EVENT_KEY_DOWN) {
							switch (evento.keyboard.keycode) {
							case ALLEGRO_KEY_RIGHT:
								controlQuest++;
								if (controlQuest > 3) {
									controlQuest = 1;
									largura -= 460; // Reset para a posição inicial
								}
								else {
									largura += 230; // Avança para a próxima posição
								}
								break;

							case ALLEGRO_KEY_LEFT:
								controlQuest--;
								if (controlQuest < 1) {
									controlQuest = 3;
									largura += 460; // Reset para a última posição
								}
								else {
									largura -= 230; // Retorna à posição anterior
								}
								break;
							case ALLEGRO_KEY_ENTER:
								// Confirma a alternativa escolhida
								alternate = false;
								break;

							case ALLEGRO_KEY_ESCAPE:
								// Sai do menu sem escolher
								alternate = false;
								controlQuest = 0; // Nenhuma alternativa selecionada
								break;
							}
						}

						// Desenha o menu de alternativas
						al_clear_to_color(al_map_rgb(0, 0, 0));

						al_draw_scaled_bitmap(
							holograma,
							0, 0, // Coordenadas de origem na imagem
							largura_original, altura_original, // Dimensões da imagem original
							centro_x, centro_y, // Posição centralizada
							largura_original * escala, altura_original * escala, // Dimensões escaladas
							0 // Sem flags
						);
						al_draw_bitmap_region(Est_futurista_remove, 150, 200, 100, 100, 960, 460, 0);
						al_draw_bitmap_region(Est_futurista_remove, 450, 120, 200, 110, 320, 300, 0);
						al_draw_bitmap_region(Est_futurista_remove, 450, 120, 200, 110, 550, 300, 0);
						al_draw_bitmap_region(Est_futurista_remove, 450, 120, 200, 110, 780, 300, 0);
						al_draw_bitmap_region(Est_futurista_remove, 450, 120, 200, 110, 540, 180, 0);
						al_draw_text(font, al_map_rgb(255, 255, 255), 630, 450, ALLEGRO_ALIGN_CENTER, "Parar abrir, é necessário");
						al_draw_text(font, al_map_rgb(255, 255, 255), 630, 480, ALLEGRO_ALIGN_CENTER, "desvendar o mistério!");

						//QUESTÃO
						al_draw_text(font, al_map_rgb(255, 255, 255), 620, 210, ALLEGRO_ALIGN_CENTER, "Log");
						al_draw_text(font, al_map_rgb(255, 255, 255), 648, 224, ALLEGRO_ALIGN_CENTER, "6");
						al_draw_text(font, al_map_rgb(255, 255, 255), 680, 212, ALLEGRO_ALIGN_CENTER, "36");

						// Mostra as alternativas com destaque para a selecionada
						switch (controlQuest) {
						case 1:
							al_draw_text(font_realista, cor_selecionada, 425, 340, ALLEGRO_ALIGN_CENTER, "5");
							al_draw_text(font_realista, cor_padrao, 655, 340, ALLEGRO_ALIGN_CENTER, "6");
							al_draw_text(font_realista, cor_padrao, 885, 340, ALLEGRO_ALIGN_CENTER, "2");
							break;
						case 2:
							al_draw_text(font_realista, cor_padrao, 425, 340, ALLEGRO_ALIGN_CENTER, "5");
							al_draw_text(font_realista, cor_selecionada, 655, 340, ALLEGRO_ALIGN_CENTER, "6");
							al_draw_text(font_realista, cor_padrao, 885, 340, ALLEGRO_ALIGN_CENTER, "2");
							break;
						case 3:
							al_draw_text(font_realista, cor_padrao, 425, 340, ALLEGRO_ALIGN_CENTER, "5");
							al_draw_text(font_realista, cor_padrao, 655, 340, ALLEGRO_ALIGN_CENTER, "6");
							al_draw_text(font_realista, cor_selecionada, 885, 340, ALLEGRO_ALIGN_CENTER, "2");
							break;
						}

						al_flip_display();

					}


					// Mostra o resultado da alternativa escolhida
					al_clear_to_color(al_map_rgb(0, 0, 0));
					if (!(controlLife == 5)) {
						al_draw_scaled_bitmap(
							holograma,
							0, 0, // Coordenadas de origem na imagem
							largura_original, altura_original, // Dimensões da imagem original
							centro_x, centro_y, // Posição centralizada
							largura_original * escala, altura_original * escala, // Dimensões escaladas
							0 // Sem flags
						);
					}
					if (controlQuest == 3) {
						al_draw_scaled_bitmap(
							holograma,
							0, 0, // Coordenadas de origem na imagem
							largura_original, altura_original, // Dimensões da imagem original
							centro_x, centro_y, // Posição centralizada
							largura_original * escala, altura_original * escala, // Dimensões escaladas
							0 // Sem flags
						);
						al_draw_text(font, al_map_rgb(255, 255, 255), 630, 330, ALLEGRO_ALIGN_CENTER, "Acesso concedido!");

						al_flip_display();
						al_rest(2.0);
					}
					else if ((controlQuest == 1 || controlQuest == 2) && !(controlLife == 5)) {
						al_draw_text(font, al_map_rgb(255, 255, 255), 630, 330, ALLEGRO_ALIGN_CENTER, "Acesso bloqueado!");
					}
					if (!(controlLife == 5)) {

						al_flip_display();
						al_rest(2.0);
					}

					if (controlQuest == 3) {
						tela4();
					}
					if (!(controlLife == 6))
					{
						variaveis.error = true;
						controlLife++;
						Interface_tela(variaveis.error, barra_vida, controlLife, est_interface, municao, sprite_arma, died);
						abrindo = false;
					}
					if (controlLife == 6) {
						al_clear_to_color(al_map_rgb(255, 255, 255));
						//Backgrounds
						al_draw_bitmap(background_3, 0, 0, 0);
						//Consumíveis
						al_draw_bitmap(kitmedico, kitmedicoX, 650, 0);
						al_draw_bitmap(kitmunicao, kitmunicaoX, 650, 0);
						Interface_tela(variaveis.error, barra_vida, controlLife, est_interface, municao, sprite_arma, died);
						al_flip_display();
						al_rest(3.0);
						menu();

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
							variaveis.pos_x_bullet += 80.0f;
						}
						else {
							al_draw_bitmap(recursos.bullet_left, variaveis.pos_x_bullet, variaveis.pos_y + 167, 0);
							variaveis.pos_x_bullet -= 80.0f;
						}
					}
				}
			}
			else {
				variaveis.inicializado = false; // Reseta quando não está atirando
			}

			if (variaveis.pos_x_bullet > 1280) {
				variaveis.atirando = false;
			}
			else if (variaveis.pos_x_bullet < 0) {
				variaveis.atirando = false;
			}

			al_flip_display();
		}

		//Backgrounds
		al_destroy_bitmap(background_3);
		//Estilizações
		al_destroy_bitmap(Est_futurista);
		al_destroy_bitmap(Est_futurista_remove);
		al_destroy_bitmap(assets_keys);
		//Interface
		al_destroy_bitmap(barra_vida);
		al_destroy_bitmap(municao);
		al_destroy_bitmap(est_interface);
		al_destroy_bitmap(sprite_arma);
		al_destroy_bitmap(died);
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
	ALLEGRO_FONT* font = al_load_font("./fonte/font.ttf", 25, 0);
	ALLEGRO_FONT* font_realista = al_load_font("./fonte/Seagram.ttf", 28, 0);
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
	ALLEGRO_BITMAP* Est_futurista = al_load_bitmap("./Estilizacoes/Est_futurista.png");
	ALLEGRO_BITMAP* Est_futurista_remove = al_load_bitmap("./Estilizacoes/Est_futurista_remove.png");
	ALLEGRO_BITMAP* Est_futurista_remove2 = al_load_bitmap("./Estilizacoes/Est_futurista_remove2.png");
	ALLEGRO_BITMAP* assets_keys = al_load_bitmap("./Estilizacoes/assets_keys.png");
	ALLEGRO_BITMAP* assets_keys_space = al_load_bitmap("./Estilizacoes/space_key.png");
	ALLEGRO_BITMAP* barra_vida = al_load_bitmap("./Estilizacoes/barra_vida.png");
	ALLEGRO_BITMAP* municao = al_load_bitmap("./Estilizacoes/municao.png");
	ALLEGRO_BITMAP* est_interface = al_load_bitmap("./Estilizacoes/interface.png");
	ALLEGRO_BITMAP* sprite_arma = al_load_bitmap("./Estilizacoes/sprite_arma.png");
	//Tela
	ALLEGRO_BITMAP* died = al_load_bitmap("./backgrounds/died.png");
	//Entrada base
	ALLEGRO_BITMAP* fundoporta = al_load_bitmap("./base/montanha.png");
	ALLEGRO_BITMAP* porta = al_load_bitmap("./base/porta.png");
	ALLEGRO_BITMAP* holograma = al_load_bitmap("./backgrounds/holograma.jpg");
	ALLEGRO_BITMAP* hologramaexp = al_load_bitmap("./backgrounds/hologramaexp.jpg");
	ALLEGRO_BITMAP* Robo = al_load_bitmap("./backgrounds/Robo.gif");

	ALLEGRO_COLOR cor_padrao = al_map_rgb(41, 43, 43);
	ALLEGRO_COLOR  cor_selecionada = al_map_rgb(215, 219, 218);


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
	int LIMITE_DIREITO = 950;
	int LIMITE_ESQUERDO = 5;
	//VARIÁVEIS INIMIGO
	float frameinimigo = 0.f;
	int enemy_pos_x = 1200;
	int enemy_pos_y = 600;
	int enemy_velocity = 3; // Velocidade do inimigo
	bool enemy_alive = true;
	bool enemy_moving_right = true; // Direção do movimento do inimigo
	//CONTROLE ESTADO DO ENEMY
	typedef enum {
		INIMIGO_RIGHT, INIMIGO_LEFT
	} enemyState;

	enemyState current_enemy_state = INIMIGO_RIGHT;
	//VARIÁVEIS DO GENERAL
	float framegeneral = 0.3f;
	int gen_pos_x = 1050;
	int gen_pos_y = 580;
	int gen_velocity = 2; // Velocidade do generaL
	bool gen_moving_right = true; // Direção do movimento do general

	//CONTROLE DO ESTADO DO GENERAL
	typedef enum {
		GEN_RIGHT, GEN_LEFT
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

	typedef enum {
		WALKING_RIGHT, WALKING_LEFT, WALKING_SHOT_RIGHT, WALKING_SHOT_LEFT, JUMPING, IDLE_LEFT, IDLE_RIGHT, PEGAR
	}WalkingState;

	WalkingState jump_state = IDLE_RIGHT; //Manter estado atual


	while (true) {
		ALLEGRO_EVENT event;
		al_wait_for_event(event_queue, &event);

		if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			break;
		}

		// Atualização do estado ao pressionar a tecla de pulo
		else if (event.keyboard.keycode == ALLEGRO_KEY_SPACE && !variaveis.pulando) {
			if (event.keyboard.keycode == ALLEGRO_KEY_RIGHT) {
				variaveis.pos_x += 10;
			}
			else if (event.keyboard.keycode == ALLEGRO_KEY_LEFT) {
				variaveis.pos_x -= 10;
				break;
			}
			variaveis.pulo = 1;
			variaveis.velocy = -variaveis.velocup;
			variaveis.plimit = variaveis.pos_y - 140; // Altura máxima do pulo
			jump_state = JUMPING; // Prioriza o estado de pulo
			variaveis.pulando = true;
		}

		// Lógica de pulo
		else if (variaveis.pulo) {
			switch (event.keyboard.keycode) {
			case ALLEGRO_KEY_RIGHT:
				variaveis.pos_x += 10;
				break;
			case ALLEGRO_KEY_LEFT:
				variaveis.pos_x -= 10;
				break;
			}
			variaveis.pos_y += variaveis.velocy;
			variaveis.velocy += 1;


			if (variaveis.pos_y >= 380) { // Retorna ao chão
				variaveis.pos_y = 380;
				variaveis.pulo = 0;
				variaveis.velocy = 0;
				variaveis.pulando = false;

				// Define o estado após o pulo com base no movimento
				if (variaveis.moving_left) {
					jump_state = WALKING_LEFT;
				}
				else {
					jump_state = WALKING_RIGHT;
				}
			}
		}


		// Movimentação horizontal
		if (!variaveis.pulando) { // Só muda estado se não estiver pulando
			switch (event.keyboard.keycode) {
			case ALLEGRO_KEY_LEFT:
				if (variaveis.pos_x > LIMITE_ESQUERDO) {
					jump_state = WALKING_LEFT;
					variaveis.pos_x -= 5;
					variaveis.moving_left = true;
					variaveis.mov = true;
				}
				break;

			case ALLEGRO_KEY_RIGHT:
				if (variaveis.pos_x < LIMITE_DIREITO) {
					jump_state = WALKING_RIGHT;
					variaveis.pos_x += 5;
					variaveis.moving_left = false;
					variaveis.mov = true;
				}
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
				if (variaveis.moving_left) {
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
			//Entrada base
			al_draw_bitmap(fundoporta, 780, 186, 0);
			al_draw_bitmap(porta, 980, 340, 0);
			al_draw_bitmap_region(Estilizacao_Alien, 0, 0, 300, 150, 760, 490, 0);
			if (enemy_alive) {
				al_draw_text(font, al_map_rgb(255, 255, 255), 725, 200, ALLEGRO_ALIGN_CENTER, "on the enemy");
				al_draw_text(font, al_map_rgb(255, 255, 255), 500, 200, ALLEGRO_ALIGN_CENTER, "Press ");
				al_draw_bitmap_region(assets_keys_space, 92.3, 220, 85, 120, 540, 198, 0);
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

			case JUMPING:if (variaveis.moving_left) {
				al_draw_bitmap_region(recursos.jump_image, 256 * (int)variaveis.frame7px, variaveis.current_frame_y, 256, 256, variaveis.pos_x, variaveis.pos_y, ALLEGRO_FLIP_HORIZONTAL);
				//Inverter imagem do pulo
			}
						else {
				al_draw_bitmap_region(recursos.jump_image, 256 * (int)variaveis.frame7px, variaveis.current_frame_y, 256, 256, variaveis.pos_x, variaveis.pos_y, 0);
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

		float largura = 195.0;

		if (variaveis.pos_x > -50 && (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)) {

			variaveis.countholo = 1;

			bool alternate = true;
			int controlQuest = 1;

			// Loop principal do menu de alternativas
			while (alternate) {
				ALLEGRO_EVENT evento;
				al_wait_for_event(event_queue, &evento);

				if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
					alternate = false;
					break;
				}

				if (evento.type == ALLEGRO_EVENT_KEY_DOWN) {
					switch (evento.keyboard.keycode) {
					case ALLEGRO_KEY_UP:
						controlQuest++;
						if (controlQuest > 2) {
							controlQuest = 1;
							largura -= 460; // Reset para a posição inicial
						}
						else {
							largura += 230; // Avança para a próxima posição
						}
						break;

					case ALLEGRO_KEY_DOWN:
						controlQuest--;
						if (controlQuest < 1) {
							controlQuest = 2;
							largura += 460; // Reset para a última posição
						}
						else {
							largura -= 230; // Retorna à posição anterior
						}
						break;
					case ALLEGRO_KEY_ENTER:
						// Confirma a alternativa escolhida
						alternate = false;
						break;

					}
				}

				// Desenha o menu de alternativas


				al_draw_bitmap(recursos.pause, 210, 120, 0);
				al_draw_text(font_realista, cor_padrao, 630, 214, ALLEGRO_ALIGN_CENTER, "PAUSE");

				ALLEGRO_COLOR cor_padrao = al_map_rgb(41, 43, 43);
				ALLEGRO_COLOR cor_selecionada = al_map_rgb(215, 219, 218);

				// Mostra as alternativas com destaque para a selecionada
				switch (controlQuest) {
				case 1:
					al_draw_text(font_realista, cor_selecionada, 630, 323, ALLEGRO_ALIGN_CENTER, "CONTINUAR");
					al_draw_text(font_realista, cor_padrao, 630, 405, ALLEGRO_ALIGN_CENTER, "SAIR");

					break;
				case 2:
					al_draw_text(font_realista, cor_padrao, 630, 323, ALLEGRO_ALIGN_CENTER, "CONTINUAR");
					al_draw_text(font_realista, cor_selecionada, 630, 405, ALLEGRO_ALIGN_CENTER, "SAIR");
					break;
				}

				al_flip_display();
			}

			// Mostra o resultado da alternativa escolhida
			if (controlQuest == 2) {
				menu();
			}
			else if (controlQuest == 1) {
				alternate = false;
			}
			al_flip_display();
			al_rest(2.0);
		}
		if (variaveis.pos_x > 940) {
			al_draw_text(font, al_map_rgb(255, 255, 255), 1140, 360, ALLEGRO_ALIGN_CENTER, "Press ");
			al_draw_bitmap_region(assets_keys, 240, 110, 60, 40, 1182, 357, 0);
		}
		Interface_tela(variaveis.error, barra_vida, controlLife, est_interface, municao, sprite_arma, died);

		if (variaveis.pos_x > -50 && (event.keyboard.keycode == ALLEGRO_KEY_R)) {
			variaveis.countholo = 1;

			float largura_original = al_get_bitmap_width(holograma);
			float altura_original = al_get_bitmap_height(holograma);

			// Definindo o tamanho final da tela (1280x720)
			float largura_tela = 1280.0f;
			float altura_tela = 720.0f;
			float largura = 195.0;

			int total_alternativas = 3;
			int alternativa_selecionada = 0;

			// Variáveis de escala
			float escala_x = largura_tela / largura_original;   // Fator de escala na direção X
			float escala_y = altura_tela / altura_original;     // Fator de escala na direção Y
			float escala = 1.0f; // Escala inicial de 1.0 (tamanho original da imagem)

			float tempo_inicial = al_get_time(); // Tempo inicial para controle de animação

			// Controle de loop
			bool abrindo = true;

			int menu_frame_x = 0, menu_frame_y = 0, menu_pos_x = 0, menu_pos_y = 0, controlQuest = 1, Alternativa = 0;
			// Loop principal
			while (abrindo) {
				// Controla o tempo de aumento da escala (1.5 segundos)

				float tempo_decorrido = al_get_time() - tempo_inicial;

				if (tempo_decorrido < 1.5) {
					// A escala aumenta gradualmente até atingir o tamanho final em 1.5 segundos
					escala = 1.0f + (escala_x - 1.0f) * (tempo_decorrido / 1.5f); // Crescimento suave
				}
				else {
					escala = escala_x; // A escala final é o valor correspondente ao tamanho da tela
				}

				// Calcula a posição para centralizar a imagem escalada
				float centro_x = (largura_tela - (largura_original * escala)) / 2; // Centraliza horizontalmente
				float centro_y = (altura_tela - (altura_original * escala)) / 2; // Centraliza verticalmente

				ALLEGRO_COLOR cor_padrao = al_map_rgb(41, 43, 43);
				ALLEGRO_COLOR cor_selecionada = al_map_rgb(215, 219, 218);
				ALLEGRO_COLOR Resposta = al_map_rgb(215, 219, 218);


				/*al_draw_text(font_realista, controlQuest == 1 ? cor_selecionada : cor_padrao, 425, 340, ALLEGRO_ALIGN_CENTER, "5");
				al_draw_text(font_realista,controlQuest == 2 ? cor_selecionada : cor_padrao, 655, 340, ALLEGRO_ALIGN_CENTER, "6");
				al_draw_text(font_realista,controlQuest == 3 ? cor_selecionada : cor_padrao, 885, 340, ALLEGRO_ALIGN_CENTER, "2");
				*/
				bool alternate = true;


				while (alternate) {
					ALLEGRO_EVENT evento;
					al_wait_for_event(event_queue, &evento);
					if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
						alternate = false;
						break;
					}

					if (evento.type == ALLEGRO_EVENT_KEY_DOWN) {
						switch (evento.keyboard.keycode) {
						case ALLEGRO_KEY_RIGHT:
							controlQuest++;
							if (controlQuest > 3) {
								controlQuest = 1;
								largura -= 460; // Reset para a posição inicial
							}
							else {
								largura += 230; // Avança para a próxima posição
							}
							break;

						case ALLEGRO_KEY_LEFT:
							controlQuest--;
							if (controlQuest < 1) {
								controlQuest = 3;
								largura += 460; // Reset para a última posição
							}
							else {
								largura -= 230; // Retorna à posição anterior
							}
							break;

						case ALLEGRO_KEY_ENTER:
							// Confirma a alternativa escolhida
							alternate = false;
							break;

						case ALLEGRO_KEY_ESCAPE:
							// Sai do menu sem escolher
							alternate = false;
							controlQuest = 0; // Nenhuma alternativa selecionada
							break;
						}
					}

					// Desenha o menu de alternativas
					al_clear_to_color(al_map_rgb(0, 0, 0));

					al_draw_scaled_bitmap(
						holograma,
						0, 0, // Coordenadas de origem na imagem
						largura_original, altura_original, // Dimensões da imagem original
						centro_x, centro_y, // Posição centralizada
						largura_original * escala, altura_original * escala, // Dimensões escaladas
						0 // Sem flags
					);

					al_draw_bitmap_region(Est_futurista_remove, 150, 200, 100, 100, 960, 460, 0);
					al_draw_bitmap_region(Est_futurista_remove, 450, 120, 200, 110, 320, 300, 0);
					al_draw_bitmap_region(Est_futurista_remove, 450, 120, 200, 110, 550, 300, 0);
					al_draw_bitmap_region(Est_futurista_remove, 450, 120, 200, 110, 780, 300, 0);
					al_draw_bitmap_region(Est_futurista_remove, 450, 120, 200, 110, 540, 180, 0);
					al_draw_text(font, al_map_rgb(255, 255, 255), 630, 450, ALLEGRO_ALIGN_CENTER, "Parar abrir, é necessário");
					al_draw_text(font, al_map_rgb(255, 255, 255), 630, 480, ALLEGRO_ALIGN_CENTER, "desvendar o mistério!");

					//QUESTÃO
					al_draw_text(font_realista, al_map_rgb(255, 255, 255), 642.5, 210, ALLEGRO_ALIGN_CENTER, "X + 14 = 20");

					ALLEGRO_COLOR cor_padrao = al_map_rgb(41, 43, 43);
					ALLEGRO_COLOR cor_selecionada = al_map_rgb(215, 219, 218);

					// Mostra as alternativas com destaque para a selecionada
					switch (controlQuest) {
					case 1:
						al_draw_text(font_realista, cor_selecionada, 420, 340, ALLEGRO_ALIGN_CENTER, "X = 34");
						al_draw_text(font_realista, cor_padrao, 655, 340, ALLEGRO_ALIGN_CENTER, "X = -6");
						al_draw_text(font_realista, cor_padrao, 880, 340, ALLEGRO_ALIGN_CENTER, "X = 6");
						break;
					case 2:
						al_draw_text(font_realista, cor_padrao, 420, 340, ALLEGRO_ALIGN_CENTER, "X = 34");
						al_draw_text(font_realista, cor_selecionada, 655, 340, ALLEGRO_ALIGN_CENTER, "X = -6");
						al_draw_text(font_realista, cor_padrao, 880, 340, ALLEGRO_ALIGN_CENTER, "X = 6");
						break;
					case 3:
						al_draw_text(font_realista, cor_padrao, 420, 340, ALLEGRO_ALIGN_CENTER, "X = 34");
						al_draw_text(font_realista, cor_padrao, 655, 340, ALLEGRO_ALIGN_CENTER, "X = -6");
						al_draw_text(font_realista, cor_selecionada, 880, 340, ALLEGRO_ALIGN_CENTER, "X = 6");
						break;
					}

					al_flip_display();
				}

				// Mostra o resultado da alternativa escolhida
				al_clear_to_color(al_map_rgb(0, 0, 0));
				if (!(controlLife == 5)) {
					al_draw_scaled_bitmap(
						holograma,
						0, 0, // Coordenadas de origem na imagem
						largura_original, altura_original, // Dimensões da imagem original
						centro_x, centro_y, // Posição centralizada
						largura_original * escala, altura_original * escala, // Dimensões escaladas
						0 // Sem flags
					);
				}
				if (controlQuest == 3) {
					al_draw_scaled_bitmap(
						holograma,
						0, 0, // Coordenadas de origem na imagem
						largura_original, altura_original, // Dimensões da imagem original
						centro_x, centro_y, // Posição centralizada
						largura_original * escala, altura_original * escala, // Dimensões escaladas
						0 // Sem flags
					);
					al_draw_text(font, al_map_rgb(255, 255, 255), 630, 330, ALLEGRO_ALIGN_CENTER, "Acesso concedido!");

					al_flip_display();
					al_rest(2.0);
				}
				else if ((controlQuest == 1 || controlQuest == 2) && !(controlLife == 5)) {
					al_draw_text(font, al_map_rgb(255, 255, 255), 630, 330, ALLEGRO_ALIGN_CENTER, "Acesso bloqueado!");
				}
				if (!(controlLife == 5)) {

					al_flip_display();
					al_rest(2.0);
				}
				if (controlQuest == 3) {
					tela3();
				}
				if (!(controlLife == 6))
				{
					variaveis.error = true;
					controlLife++;
					Interface_tela(variaveis.error, barra_vida, controlLife, est_interface, municao, sprite_arma, died);
					abrindo = false;
				}
				if (controlLife == 6) {
					al_clear_to_color(al_map_rgb(255, 255, 255));
					//Backgrounds
					al_draw_bitmap(background_2, 0, 0, 0);
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
					Interface_tela(variaveis.error, barra_vida, controlLife, est_interface, municao, sprite_arma, died);
					al_flip_display();
					al_rest(2.0);
					menu();
				}

			}
		}
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
					variaveis.pos_x_bullet += 80.0f;
				}
				else {
					al_draw_bitmap(recursos.bullet_left, variaveis.pos_x_bullet, variaveis.pos_y + 167, 0);
					variaveis.pos_x_bullet -= 80.0f;
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

		if (variaveis.pos_x_bullet > 1280) {
			variaveis.atirando = false;
		}
		else if (variaveis.pos_x_bullet < 0) {
			variaveis.atirando = false;
		}


		al_flip_display();
	}

	//Backgrounds
	al_destroy_bitmap(background_2);
	al_destroy_bitmap(fundoporta);
	al_destroy_bitmap(porta);
	al_destroy_bitmap(holograma);
	al_destroy_bitmap(hologramaexp);
	al_destroy_bitmap(Robo);
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
	//Estilizacao
	al_destroy_bitmap(Estilizacao_Alien);
	al_destroy_bitmap(Estilizacao_Ossos);
	al_destroy_bitmap(Est_futurista);
	al_destroy_bitmap(Est_futurista_remove);
	al_destroy_bitmap(ossinhos);
	al_destroy_bitmap(Rei); al_destroy_bitmap(assets_keys);
	//Interface
	al_destroy_bitmap(barra_vida);
	al_destroy_bitmap(municao);
	al_destroy_bitmap(est_interface);
	al_destroy_bitmap(sprite_arma);
	al_destroy_bitmap(died);

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
	ALLEGRO_FONT* font = al_load_font("./fonte/font.ttf", 25, 0);
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
	//Holograma
	ALLEGRO_BITMAP* holograma = al_load_bitmap("./backgrounds/holograma.jpg");
	ALLEGRO_BITMAP* hologramaexp = al_load_bitmap("./backgrounds/hologramaexp.jpg");
	ALLEGRO_BITMAP* Est_futurista = al_load_bitmap("./Estilizacoes/Est_futurista.png");
	ALLEGRO_BITMAP* Est_futurista_remove = al_load_bitmap("./Estilizacoes/Est_futurista_remove.png");
	ALLEGRO_BITMAP* assets_keys = al_load_bitmap("./Estilizacoes/assets_keys.png");
	ALLEGRO_BITMAP* assets_keys_space = al_load_bitmap("./Estilizacoes/space_key.png");
	ALLEGRO_BITMAP* setas_key = al_load_bitmap("./Estilizacoes/setas_key.png");
	ALLEGRO_BITMAP* barra_vida = al_load_bitmap("./Estilizacoes/barra_vida.png");
	ALLEGRO_BITMAP* municao = al_load_bitmap("./Estilizacoes/municao.png");
	ALLEGRO_BITMAP* est_interface = al_load_bitmap("./Estilizacoes/interface.png");
	ALLEGRO_BITMAP* sprite_arma = al_load_bitmap("./Estilizacoes/sprite_arma.png");
	//Tela
	ALLEGRO_BITMAP* died = al_load_bitmap("./backgrounds/died.png");
	//Inimigos
	ALLEGRO_BITMAP* drone = al_load_bitmap("./inimigos/drone.png");
	ALLEGRO_SAMPLE* tiro = al_load_sample("./Audios/metralhadora.wav");
	//FONTE
	ALLEGRO_FONT* font_realista = al_load_font("./fonte/Seagram.ttf", 28, 0);
	ALLEGRO_COLOR cor_padrao = al_map_rgb(41, 43, 43);
	ALLEGRO_COLOR  cor_selecionada = al_map_rgb(215, 219, 218);

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
	typedef enum {
		WALKING_RIGHT, WALKING_LEFT, WALKING_SHOT_RIGHT, WALKING_SHOT_LEFT, JUMPING, IDLE_LEFT, IDLE_RIGHT, PEGAR
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

		else if (event.keyboard.keycode == ALLEGRO_KEY_SPACE && !variaveis.pulando) {
			if (event.keyboard.keycode == ALLEGRO_KEY_RIGHT) {
				variaveis.pos_x += 10;
			}
			else if (event.keyboard.keycode == ALLEGRO_KEY_LEFT) {
				variaveis.pos_x -= 10;
				break;
			}
			variaveis.pulo = 1;
			variaveis.velocy = -variaveis.velocup;
			variaveis.plimit = variaveis.pos_y - 140; // Altura máxima do pulo
			jump_state = JUMPING; // Prioriza o estado de pulo
			variaveis.pulando = true;
		}

		// Lógica de pulo
		else if (variaveis.pulo) {
			switch (event.keyboard.keycode) {
			case ALLEGRO_KEY_RIGHT:
				variaveis.pos_x += 10;
				break;
			case ALLEGRO_KEY_LEFT:
				variaveis.pos_x -= 10;
				break;
			}
			variaveis.pos_y += variaveis.velocy;
			variaveis.velocy += 1;


			if (variaveis.pos_y >= 380) { // Retorna ao chão
				variaveis.pos_y = 380;
				variaveis.pulo = 0;
				variaveis.velocy = 0;
				variaveis.pulando = false;

				// Define o estado após o pulo com base no movimento
				if (variaveis.moving_left) {
					jump_state = WALKING_LEFT;
				}
				else {
					jump_state = WALKING_RIGHT;
				}
			}
		}

		// Movimentação horizontal
		if (!variaveis.pulando) { // Só muda estado se não estiver pulando
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
					variaveis.pegoukit = 1;
					if (variaveis.pegoumuni == 1) {
						variaveis.pegoukit = 2;
					}
					variaveis.error = false;
					Interface_tela(variaveis.error, barra_vida, controlLife, est_interface, municao, sprite_arma, died);

				}
				else if (variaveis.pos_x > 560 && variaveis.pos_x < 670 && variaveis.pos_y + 128 > variaveis.pos_y && variaveis.pos_y < variaveis.pos_y + 100) {
					kitmunicaoX = -1000;
					variaveis.pegoumuni = 1;
					if (variaveis.pegoukit == 1) {
						variaveis.pegoumuni = 2;
					}
					variaveis.error = false;
					Interface_tela(variaveis.error, barra_vida, controlLife, est_interface, municao, sprite_arma, died);

				}
				break;

			case ALLEGRO_KEY_B:
				if (variaveis.moving_left) {
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
			if (variaveis.pos_x >= 360 - 256 && variaveis.pos_x <= 570) {
				al_draw_text(font, al_map_rgb(255, 255, 255), 400, 450, ALLEGRO_ALIGN_CENTER, "Press ");
				al_draw_bitmap_region(assets_keys, 55, 0, 50, 65, 442, 425, 0);
			}
			al_draw_bitmap(tronco3, troncoX, 516, 0);
			//ESC
			al_draw_text(font, al_map_rgb(255, 255, 255), 500, 200, ALLEGRO_ALIGN_CENTER, "Press ");
			al_draw_bitmap_region(assets_keys_space, 47.5, 220, 40, 120, 545, 198, 0);
			al_draw_text(font, al_map_rgb(255, 255, 255), 650, 200, ALLEGRO_ALIGN_CENTER, "To pause");
			//SETAS
			al_draw_text(font, al_map_rgb(255, 255, 255), 100, 225, ALLEGRO_ALIGN_CENTER, "Press ");
			al_draw_bitmap(setas_key, 150, 197, 0);
			al_draw_text(font, al_map_rgb(255, 255, 255), 290, 225, ALLEGRO_ALIGN_CENTER, "To walk");

			if (variaveis.pegoukit < 2 || variaveis.pegoumuni < 2) {
				if (variaveis.pos_x > 465 && variaveis.pos_x < 670 && variaveis.pos_y + 128 > variaveis.pos_y && variaveis.pos_y < variaveis.pos_y + 1000) {
					al_draw_text(font, al_map_rgb(255, 255, 255), 650, 450, ALLEGRO_ALIGN_CENTER, "Press ");
					al_draw_bitmap_region(assets_keys, 150, 150, 50, 45, 695, 443, 0);
				}
			}if (variaveis.pegoukit >= 2 || variaveis.pegoumuni >= 2) {
				if (variaveis.pegoukit == 2) {
					variaveis.pegoumuni = 2;
				}
				else if (variaveis.pegoumuni == 2) {
					variaveis.pegoukit = 2;
				}
				al_draw_text(font, al_map_rgb(255, 255, 255), 750, 350, ALLEGRO_ALIGN_CENTER, "Press ");
				al_draw_bitmap_region(assets_keys_space, 92.3, 220, 85, 120, 795, 348, 0);
				al_draw_text(font, al_map_rgb(255, 255, 255), 805, 380, ALLEGRO_ALIGN_CENTER, "To jump ");

			}
			al_draw_bitmap(kitmedico, kitmedicoX, 600, 0);
			al_draw_bitmap(kitmunicao, kitmunicaoX, 600, 0);

			//Interface
			Interface_tela(variaveis.error, barra_vida, controlLife, est_interface, municao, sprite_arma, died);
			switch (jump_state) {
			case WALKING_RIGHT: al_draw_bitmap_region(recursos.walkright, 256 * (int)variaveis.frame7px, variaveis.current_frame_y, 256, 256, variaveis.pos_x, variaveis.pos_y, 0);
				break;

			case WALKING_LEFT: al_draw_bitmap_region(recursos.walkleft, 256 * (int)variaveis.frame7px, variaveis.current_frame_y, 256, 256, variaveis.pos_x, variaveis.pos_y, 0);
				break;

			case WALKING_SHOT_RIGHT: al_draw_bitmap_region(recursos.shot_image_right, 256 * (int)variaveis.frame4px, variaveis.current_frame_y, 256, 256, variaveis.pos_x, variaveis.pos_y, 0);
				break;

			case WALKING_SHOT_LEFT: al_draw_bitmap_region(recursos.shot_image_left, 256 * (int)variaveis.frame4px, variaveis.current_frame_y, 256, 256, variaveis.pos_x, variaveis.pos_y, 0);
				break;

			case JUMPING:if (variaveis.moving_left) {
				al_draw_bitmap_region(recursos.jump_image, 256 * (int)variaveis.frame7px, variaveis.current_frame_y, 256, 256, variaveis.pos_x, variaveis.pos_y, ALLEGRO_FLIP_HORIZONTAL);
				//Inverter imagem do pulo
			}
						else {
				al_draw_bitmap_region(recursos.jump_image, 256 * (int)variaveis.frame7px, variaveis.current_frame_y, 256, 256, variaveis.pos_x, variaveis.pos_y, 0);
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
		float largura = 195.0;
		int total_alternativas = 3;
		int alternativa_selecionada = 0;

		if (variaveis.pos_x > -50 && (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)) {

			variaveis.countholo = 1;

			bool alternate = true;
			int controlQuest = 1;

			// Loop principal do menu de alternativas
			while (alternate) {
				ALLEGRO_EVENT evento;
				al_wait_for_event(event_queue, &evento);

				if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
					alternate = false;
					break;
				}

				if (evento.type == ALLEGRO_EVENT_KEY_DOWN) {
					switch (evento.keyboard.keycode) {
					case ALLEGRO_KEY_UP:
						controlQuest++;
						if (controlQuest > 2) {
							controlQuest = 1;
							largura -= 460; // Reset para a posição inicial
						}
						else {
							largura += 230; // Avança para a próxima posição
						}
						break;

					case ALLEGRO_KEY_DOWN:
						controlQuest--;
						if (controlQuest < 1) {
							controlQuest = 2;
							largura += 460; // Reset para a última posição
						}
						else {
							largura -= 230; // Retorna à posição anterior
						}
						break;
					case ALLEGRO_KEY_ENTER:
						// Confirma a alternativa escolhida
						alternate = false;
						break;

					}
				}

				// Desenha o menu de alternativas


				al_draw_bitmap(recursos.pause, 210, 120, 0);
				al_draw_text(font_realista, cor_padrao, 630, 214, ALLEGRO_ALIGN_CENTER, "PAUSE");

				ALLEGRO_COLOR cor_padrao = al_map_rgb(41, 43, 43);
				ALLEGRO_COLOR cor_selecionada = al_map_rgb(215, 219, 218);

				// Mostra as alternativas com destaque para a selecionada
				switch (controlQuest) {
				case 1:
					al_draw_text(font_realista, cor_selecionada, 630, 323, ALLEGRO_ALIGN_CENTER, "CONTINUAR");
					al_draw_text(font_realista, cor_padrao, 630, 405, ALLEGRO_ALIGN_CENTER, "SAIR");

					break;
				case 2:
					al_draw_text(font_realista, cor_padrao, 630, 323, ALLEGRO_ALIGN_CENTER, "CONTINUAR");
					al_draw_text(font_realista, cor_selecionada, 630, 405, ALLEGRO_ALIGN_CENTER, "SAIR");
					break;
				}

				al_flip_display();
			}

			// Mostra o resultado da alternativa escolhida
			if (controlQuest == 2) {
				menu();
			}
			else if (controlQuest == 1) {
				alternate = false;
			}
			al_flip_display();
			al_rest(2.0);
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
					variaveis.pos_x_bullet += 80.0f;
				}
				else {
					al_draw_bitmap(recursos.bullet_left, variaveis.pos_x_bullet, variaveis.pos_y + 167, 0);
					variaveis.pos_x_bullet -= 80.0f;
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

		if (variaveis.pos_x_bullet > 1280) {
			variaveis.atirando = false;
		}
		else if (variaveis.pos_x_bullet < 0) {
			variaveis.atirando = false;
		}

		if (variaveis.pos_x > 1280) {
			tela2();
		}
		if (variaveis.pos_x < 0) variaveis.pos_x = 0; // Hitbox do inicio

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
	ALLEGRO_BITMAP* menu_opcoes_image2 = al_load_bitmap("./backgrounds/menu_opcoes_image2.png");
	ALLEGRO_BITMAP* menu_teclas = al_load_bitmap("./backgrounds/fundo-teclas.png");
	ALLEGRO_BITMAP* menu_creditos = al_load_bitmap("./backgrounds/fundo-creditos.png");
	ALLEGRO_BITMAP* botao_voltar = al_load_bitmap("./backgrounds/botao-voltar.png");
	ALLEGRO_BITMAP* creditos = al_load_bitmap("./backgrounds/creditos.png");
	ALLEGRO_BITMAP* help = al_load_bitmap("./backgrounds/help.jpg");
	//ALLEGRO_FONT* font = al_create_builtin_font();
	ALLEGRO_FONT* font_realista = al_load_font("./fonte/Seagram.ttf", 28, 0);
	ALLEGRO_FONT* font_nome_jogo = al_load_font("./fonte/Gimme.ttf", 30, 0);
	ALLEGRO_BITMAP* interrogacao = al_load_bitmap("./Estilizacoes/interrogacao.png");
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
		al_draw_bitmap(menu_opcoes_image, 450, 50, 0);
		al_draw_bitmap(menu_opcoes_image2, 450, 580, 0);

		ALLEGRO_EVENT event;
		al_wait_for_event(event_queue, &event);
		ALLEGRO_COLOR cor_padrao = al_map_rgb(41, 43, 43);
		ALLEGRO_COLOR cor_selecionada = al_map_rgb(215, 219, 218);
		ALLEGRO_COLOR credito = al_map_rgb(215, 219, 218);

		al_draw_text(font_nome_jogo, al_map_rgb(232, 237, 236), 620, 80, ALLEGRO_ALIGN_CENTER, "THE EQUATION WARRIOR");
		al_draw_text(font_realista, controlador == 4 ? cor_padrao : cor_selecionada, 635, 190, ALLEGRO_ALIGN_CENTER, "JOGAR");
		al_draw_text(font_realista, controlador == 3 ? cor_padrao : cor_selecionada, 634, 330, ALLEGRO_ALIGN_CENTER, "TECLAS");
		al_draw_text(font_realista, controlador == 2 ? cor_padrao : cor_selecionada, 630, 480, ALLEGRO_ALIGN_CENTER, "CREDITOS");
		al_draw_text(font_realista, controlador == 1 ? cor_padrao : cor_selecionada, 630, 610, ALLEGRO_ALIGN_CENTER, "HELP");

		if (controlador == 1) {
			al_draw_bitmap(interrogacao, 300, 82, 0);
			al_draw_bitmap(interrogacao, 910, 82, 0);

		}
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
					break;
				case 3:  estado_menu = 3;
				case 2: estado_menu = 2;
					break;
				case 1: estado_menu = 1;
					break;
				}
			}
		}
		else if (estado_menu == 3) {
			al_draw_bitmap(menu_teclas, 0, 0, 0);
			al_draw_text(font_realista, cor_padrao, 630, 30, ALLEGRO_ALIGN_CENTER, "TECLAS");

			if (event.keyboard.keycode == ALLEGRO_KEY_V) {
				estado_menu = 0;
			}
		}
		else if (estado_menu == 2) {
			al_draw_bitmap(menu_creditos, 0, 0, 0);
			//al_draw_text(creditos,credido, 500, 300, 0);
			al_draw_text(font_realista, cor_padrao, 610, 30, ALLEGRO_ALIGN_CENTER, "CREDITOS");
			if (event.keyboard.keycode == ALLEGRO_KEY_V) {
				estado_menu = 0;
			}
		}
		else if (estado_menu == 1) {
			al_draw_bitmap(help, 0, 0, 0);
			al_draw_text(font_realista, cor_padrao, 630, 30, ALLEGRO_ALIGN_CENTER, "HELP");

			if (event.keyboard.keycode == ALLEGRO_KEY_V) {
				estado_menu = 0;
			}
		}
		if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) { break; }

		al_flip_display();
	}
	//Destroi imagens
	al_destroy_bitmap(menu_image);
	al_destroy_bitmap(menu_opcoes_image);
	al_destroy_bitmap(menu_opcoes_image2);
	al_destroy_bitmap(menu_teclas);
	al_destroy_bitmap(menu_creditos);
	al_destroy_bitmap(help);
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
	//tela3();
	menu();
	return 0;
}
