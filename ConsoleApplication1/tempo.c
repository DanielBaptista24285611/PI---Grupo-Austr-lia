
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <stdio.h>
#include <stdbool.h>

bool inicializar();

const int LARGURA_TELA = 640;
const int ALTURA_TELA = 480;

ALLEGRO_DISPLAY* janela = NULL;
ALLEGRO_EVENT_QUEUE* fila_eventos = NULL, * fila_contador = NULL;
ALLEGRO_TIMER* timer = NULL, * contador = 0;
ALLEGRO_FONT* fonte = NULL;

int main(void)
{
    bool sair = false;
    int r = 0, g = 0, b = 0;
    int min = 0, seg = 0;

    if (!inicializar())
    {
        return -1;
    }

    al_start_timer(timer);
    al_start_timer(contador);

    while (!sair)
    {
        if (!al_is_event_queue_empty(fila_eventos))
        {
            ALLEGRO_EVENT evento;
            al_wait_for_event(fila_eventos, &evento);

            if (evento.type == ALLEGRO_EVENT_TIMER)
            {
                if (++r >= 255) r = 0;
                if (++g >= 255) g = 0;
                if (++b >= 255) b = 0;
            }
            else if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
            {
                sair = true;
            }
        }

        if (!al_is_event_queue_empty(fila_contador))
        {
            ALLEGRO_EVENT evento;
            al_wait_for_event(fila_contador, &evento);

            if (evento.type == ALLEGRO_EVENT_TIMER)
            {
                seg++;
                if (seg == 60)
                {
                    min++;
                    seg = 0;
                }
            }
        }

        al_clear_to_color(al_map_rgb(r, g, b));
        al_draw_textf(fonte, al_map_rgb(255, 0, 0), LARGURA_TELA / 2, 200, ALLEGRO_ALIGN_CENTRE, "%d:%d", min, seg);
        al_flip_display();
    }

    al_destroy_timer(timer);
    al_destroy_event_queue(fila_eventos);
    al_destroy_display(janela);

    return 0;
}

bool inicializar()
{
    if (!al_init())
    {
        fprintf(stderr, "Falha ao inicializar Allegro.\n");
        return false;
    }

    al_init_font_addon();

    if (!al_init_ttf_addon())
    {
        fprintf(stderr, "Falha ao inicializar allegro_ttf.\n");
        return false;
    }

    janela = al_create_display(LARGURA_TELA, ALTURA_TELA);
    if (!janela)
    {
        fprintf(stderr, "Falha ao criar janela.\n");
        return false;
    }

    al_set_window_title(janela, "Testandos Timers...");

    fila_eventos = al_create_event_queue();
    if (!fila_eventos)
    {
        fprintf(stderr, "Falha ao criar fila de eventos.\n");
        al_destroy_display(janela);
        return false;
    }

    timer = al_create_timer(0.1);
    if (!timer)
    {
        fprintf(stderr, "Falha ao criar timer.\n");
        al_destroy_event_queue(fila_eventos);
        al_destroy_display(janela);
        return false;
    }

    fonte = al_load_font("font.ttf", 120, 0);
    if (!fonte)
    {
        fprintf(stderr, "Falha ao carregar fonte.\n");
        al_destroy_timer(timer);
        al_destroy_event_queue(fila_eventos);
        al_destroy_display(janela);
        return false;
    }

    contador = al_create_timer(1.0);
    if (!contador)
    {
        fprintf(stderr, "Falha ao criar timer.\n");
        al_destroy_font(fonte);
        al_destroy_timer(timer);
        al_destroy_event_queue(fila_eventos);
        al_destroy_display(janela);
        return false;
    }

    fila_contador = al_create_event_queue();
    if (!fila_contador)
    {
        fprintf(stderr, "Falha ao criar fila do contador.\n");
        al_destroy_timer(contador);
        al_destroy_font(fonte);
        al_destroy_timer(timer);
        al_destroy_event_queue(fila_eventos);
        al_destroy_display(janela);
        return false;
    }

    al_register_event_source(fila_eventos, al_get_display_event_source(janela));
    al_register_event_source(fila_eventos, al_get_timer_event_source(timer));
    al_register_event_source(fila_contador, al_get_timer_event_source(contador));

    return true;
}