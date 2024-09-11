#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
//#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>


//infos, sprite idle soldado!
// largura 896 / 7 = 128;
// altura 128 / 1 = 128;


float FPS = 30.0;
int main() {

	al_init();
	al_init_font_addon();
	//al_init_ttf_addon();
	al_init_image_addon();
		
	ALLEGRO_DISPLAY* display = al_create_display(1280, 720);
	al_set_window_position(display, 200, 200);

	ALLEGRO_FONT* font = al_create_builtin_font();
	ALLEGRO_TIMER* timer = al_create_timer(1.0 / FPS);	

	ALLEGRO_BITMAP* sprite = al_load_bitmap("./idle.png");

	ALLEGRO_EVENT_QUEUE* event_queue = al_create_event_queue();
	al_register_event_source(event_queue, al_get_display_event_source(display));
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_start_timer(timer);

	while (true) {
		ALLEGRO_EVENT event;
		al_wait_for_event(event_queue, &event);
		if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			break;
		}
		al_clear_to_color(al_map_rgb(255, 255, 255));
		al_draw_text(font, al_map_rgb(0, 0, 0), 230, 200, 0, "Hello World!");
		al_draw_bitmap(sprite, 0, 0,0);
		al_flip_display();
	}

	al_destroy_font(font);
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);

	return 0;

}