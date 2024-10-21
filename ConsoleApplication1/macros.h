#pragma 
//TELA 1
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

bool shot_control = false;