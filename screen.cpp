#include "screen.h"
#include "display.h"
#include "rc.h"
#include "controller.h"

Screen * Screen::instance = NULL;

char * on_off[] = {	(char *)STR_ON, (char *)STR_OFF};
char * camera[] = {	(char *)STR_ALWAYS_BACK, (char *)STR_ALWAYS_RIGHT, 
					(char *)STR_ALWAYS_LEFT, (char *)STR_FREE};

MenuItem::MenuItem(char * l, GLshort num_opts, char ** opts)
{
	label = l;
	count_options = num_opts;
	options = opts;
	current_option = 0;
}

MenuData::MenuData(GLshort mid, char * mtitle, char * bok, char * bcancel, GLshort size, SDL_Surface * bg)
{
	id = mid;
	items_count = size;
	title = mtitle;
	ok = bok;
	cancel = bcancel;
	items = new MenuItem*[size];
	bgimage = bg;
	subtitle = NULL;
}

MenuData::~MenuData()
{
	delete[] items;
}

GLint MenuData::GetHeight()
{
	return (title ? MENU_TITLE_SPACE : 0) +
			(subtitle ? MENU_SUBTITLE_SPACE : 0) +
			(ok || cancel ? MENU_BUTTON_SPACE : 0) +
			(MENU_ITEM_SPACE * ITEMS_SHOW_LIMIT);
}

GLvoid Screen::InitMenus()
{
	RcRegistry * rc = RcRegistry::getInstance();
	
	menu_templates[MENU_MAIN] = new MenuData(MENU_MAIN, 0, 0, 0, 3, rc->GetImage(IMAGE_MAIN_SCREEN));
	menu_templates[MENU_MAIN]->SetItem(MI_MAIN_PLAY, new MenuItem((char*) STR_PLAY, 0, 0));
	menu_templates[MENU_MAIN]->SetItem(MI_MAIN_OPTS, new MenuItem((char*) STR_CONFIG, 0, 0));
	menu_templates[MENU_MAIN]->SetItem(MI_MAIN_EXIT, new MenuItem((char*) STR_EXIT, 0, 0));
	
	menu_templates[MENU_PAUSE] = new MenuData(MENU_PAUSE, (char *)STR_PAUSE, 0, 0, 4, rc->GetImage(IMAGE_PAUSE_SCREEN));
	menu_templates[MENU_PAUSE]->SetItem(MI_PAUSE_TO_ROOM, new MenuItem((char*) STR_RETURN_TO_ROOM, 0, 0));
	menu_templates[MENU_PAUSE]->SetItem(MI_PAUSE_RESTART, new MenuItem((char*) STR_RESTART_ROOM, 0, 0));
	menu_templates[MENU_PAUSE]->SetItem(MI_PAUSE_TO_MAP, new MenuItem((char*) STR_BACK_TO_MAP, 0, 0));
	menu_templates[MENU_PAUSE]->SetItem(MI_PAUSE_TO_MAIN, new MenuItem((char*) STR_BACK_MAIN, 0, 0));
	
	menu_templates[MENU_CONFIG] = new MenuData(MENU_CONFIG, (char *)STR_CONFIG, (char *)STR_APPLY, (char *)STR_BACK, 5, rc->GetImage(IMAGE_PAUSE_SCREEN));
	menu_templates[MENU_CONFIG]->SetItem(MI_OPTS_FULLSCREEN, new MenuItem((char*) STR_FULLSCREEN, 2, on_off));
	menu_templates[MENU_CONFIG]->SetItem(MI_OPTS_CAMERA, new MenuItem((char*) STR_CAMERA, 4, camera));
	menu_templates[MENU_CONFIG]->SetItem(MI_OPTS_DRAW_WALLS, new MenuItem((char*) STR_DRAW_WALLS, 2, on_off));
	menu_templates[MENU_CONFIG]->SetItem(MI_OPTS_SOUND, new MenuItem((char*) STR_SOUND, 2, on_off));
	menu_templates[MENU_CONFIG]->SetItem(MI_OPTS_MUSIC, new MenuItem((char*) STR_MUSIC, 2, on_off));
	
	menu_templates[MENU_NEXT_ROOM] = new MenuData(MENU_NEXT_ROOM, (char *)STR_THERE_IS_NEXT_ROOM, 0, 0, 4, rc->GetImage(IMAGE_PAUSE_SCREEN));
	menu_templates[MENU_NEXT_ROOM]->SetItem(MI_NEXT_ROOM, new MenuItem((char*) STR_ENTER_THIS_ROOM, 0, 0));
	menu_templates[MENU_NEXT_ROOM]->SetItem(MI_NEXT_BACK, new MenuItem((char*) STR_RETURN_BACK, 0, 0));
	menu_templates[MENU_NEXT_ROOM]->SetItem(MI_NEXT_MAP, new MenuItem((char*) STR_BACK_TO_MAP, 0, 0));
	menu_templates[MENU_NEXT_ROOM]->SetItem(MI_NEXT_MAIN, new MenuItem((char*) STR_BACK_MAIN, 0, 0));
	
	state = SCREEN_NORMAL;
}

Screen::~Screen()
{
	GLshort i;
	
	if (menu_templates[MENU_MAIN]) {
		for (i = 0; i < menu_templates[MENU_MAIN]->CountItems(); i++) {
			delete  menu_templates[MENU_MAIN]->GetItem(i);
		}
		delete menu_templates[MENU_MAIN];
	}
	if (menu_templates[MENU_PAUSE]) {
		for (i = 0; i < menu_templates[MENU_PAUSE]->CountItems(); i++) {
			delete  menu_templates[MENU_PAUSE]->GetItem(i);
		}
		delete menu_templates[MENU_PAUSE];
	}
	if (menu_templates[MENU_CONFIG]) {
		for (i = 0; i < menu_templates[MENU_CONFIG]->CountItems(); i++) {
			delete  menu_templates[MENU_CONFIG]->GetItem(i);
		}
		delete menu_templates[MENU_CONFIG];
	}
}

GLvoid Screen::RenderMainMenu()
{
	if (repaint) {
		repaint = GL_FALSE;
		
		SDL_Surface * screen = SDL_GetVideoSurface();
		SDL_Rect rect;
		SDL_Color item_color = {MAIN_MENU_ITEM_COLOR},
				  active_item_color = {MAIN_MENU_ACTIVE_ITEM_COLOR},
				  vesrion_color = {VERSION_COLOR};
		TTF_Font *items_font = RcRegistry::getInstance()->GetFont(FONT_TITLE),
				 *version_font = RcRegistry::getInstance()->GetFont(FONT_VERSION);
		MenuItem * item;
		Display::getInstance()->LigthModel(1.0f);
		
		SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
		if (menu->GetBackground()) {
			RenderBgImage(screen, menu->GetBackground());
		}
		GLint i, count, x = MAIN_MENU_SX, y = MAIN_MENU_SY;
		count = menu->CountItems();
		for (i = 0; i < count; i++) {
			item = menu->GetItem(i);
			if (i == selected_item) {
				RenderText(item->GetLabel(), items_font, active_item_color, x, y, ALIGN_CENTER, GL_FALSE);
			} else {
				RenderText(item->GetLabel(), items_font, item_color, x, y, ALIGN_CENTER, GL_FALSE);
			}
			y += MAIN_MENU_ITEM_SPACE;
		}
		
		RenderText((char*)VERSION_STR, version_font, vesrion_color, VERSION_SX, screen->h - VERSION_SY, ALIGN_LEFT, GL_FALSE);
	
		SDL_UpdateRect(screen, 0, 0, screen->w, screen->h);
		SDL_GL_SwapBuffers();
		
		Display::getInstance()->LigthModel(LIGHT_MODEL_AMBIENT);
	} else {
		SDL_Delay(50);
	}
}

GLvoid Screen::RenderMenu()
{
	if (repaint) {
		repaint = GL_FALSE;
		
		SDL_Surface * screen = SDL_GetVideoSurface();
		SDL_Rect rect;
		TTF_Font *title_font = RcRegistry::getInstance()->GetFont(FONT_TITLE),
				 *subtitle_font = RcRegistry::getInstance()->GetFont(FONT_MENUITEMS);;
		SDL_Color title_color = {MENU_TITLE_COLOR},
				  subtitle_color = {MENU_SUBTITLE_COLOR};
		
		Display::getInstance()->LigthModel(1.0f);
		
		SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
		if (menu->GetBackground()) {
			RenderBgImage(screen, menu->GetBackground());
		}
		GLint x = GLint(screen->w / 2), y = GLint((screen->h - menu->GetHeight()) / 2);
		if (menu->GetTitle()) {
			RenderText(menu->GetTitle(), title_font, title_color, x, y, ALIGN_CENTER, GL_FALSE);
		}
		if (menu->GetSubtitle()) {
			y += MENU_SUBTITLE_SPACE;
			RenderText(menu->GetSubtitle(), subtitle_font, subtitle_color, x, y, ALIGN_CENTER, GL_FALSE);
			y += MENU_TITLE_SPACE;
		} else {
			y += MENU_TITLE_SPACE;
		}
		RenderItems(screen, x, y);
		RenderButtons(screen, x, y);
		SDL_UpdateRect(screen, 0, 0, screen->w, screen->h);
		SDL_GL_SwapBuffers();
		
		Display::getInstance()->LigthModel(LIGHT_MODEL_AMBIENT);
	} else {
		SDL_Delay(50);
	}
}

GLvoid Screen::RenderText(char *text, TTF_Font *font, SDL_Color color, GLint x, GLint y, GLshort align, GLboolean drawcursor)
{
	SDL_Surface *layer, *screen;
	SDL_Rect rect, crect, cursor_l = {GI_RECT_CURSOR_L}, cursor_r = {GI_RECT_CURSOR_R};
	
	layer = TTF_RenderUTF8_Blended(font, text, color);
	rect.x = x;
	rect.y = y;
	rect.w = layer->w;
	rect.h = layer->h;
	if (align == ALIGN_RIGHT) {
		rect.x -= layer->w;
	} else if (align == ALIGN_CENTER) {
		rect.x -= (GLint) (layer->w / 2);
	}
	
	screen = SDL_GetVideoSurface();
	
	if (drawcursor) {
		RenderGUI(rect.x - (MENU_CURSOR_PAD + cursor_l.w), rect.y + MENU_CURSOR_SY, cursor_l);
		RenderGUI(rect.x + MENU_CURSOR_PAD + rect.w, rect.y + MENU_CURSOR_SY, cursor_r);
	}
	
	SDL_BlitSurface(layer, 0, screen, &rect);
	SDL_FreeSurface(layer);
}

inline GLvoid Screen::RenderBgImage(SDL_Surface * screen, SDL_Surface * img) 
{
	SDL_Rect rect;
	rect.x = GLint((screen->w - img->w) / 2);
	rect.y = GLint((screen->h - img->h) / 2);
	rect.w = img->w;
	rect.h = img->w;
	SDL_BlitSurface(img, 0, screen, &rect);	
}

inline GLvoid Screen::RenderButtons(SDL_Surface * screen, GLint x, GLint &y) 
{
	SDL_Color button_color = {MENU_BUTTON_COLOR},
			  active_button_color = {MENU_ACTIVE_BUTTON_COLOR};
	TTF_Font *buttons_font = RcRegistry::getInstance()->GetFont(FONT_MENUITEMS);
	SDL_Rect cur = {GI_RECT_CURSOR_L};
	if (menu->GetOk() || menu->GetCancel()) {
		y += MENU_BUTTON_SPACE;
		if (menu->GetOk() && menu->GetCancel()) {
			GLint ok_w, cancel_w, h;
			TTF_SizeUTF8(buttons_font, menu->GetOk(), &ok_w, &h);
			TTF_SizeUTF8(buttons_font, menu->GetCancel(), &cancel_w, &h);
			x -= GLint((ok_w + cancel_w) / 2) + (MENU_CURSOR_PAD + cur.w);
			if (selected_item == menu->CountItems()) {
				RenderText(menu->GetOk(), buttons_font, active_button_color, x, y, ALIGN_LEFT, GL_TRUE);
			} else {
				RenderText(menu->GetOk(), buttons_font, button_color, x, y, ALIGN_LEFT, GL_FALSE);
			}
			x += ok_w + cancel_w + (MENU_CURSOR_PAD + cur.w) * 2;
			if (selected_item == menu->CountItems() + 1) {
				RenderText(menu->GetCancel(), buttons_font, active_button_color, x, y, ALIGN_RIGHT, GL_TRUE);
			} else {
				RenderText(menu->GetCancel(), buttons_font, button_color, x, y, ALIGN_RIGHT, GL_FALSE);
			}
		} else if (selected_item == menu->CountItems()) {
			RenderText(menu->GetOk() ? menu->GetOk() : menu->GetCancel(), buttons_font, active_button_color, x, y, ALIGN_CENTER, GL_TRUE);
		} else {
			RenderText(menu->GetOk() ? menu->GetOk() : menu->GetCancel(), buttons_font, button_color, x, y, ALIGN_CENTER, GL_FALSE);
		}
	}
}

inline GLvoid Screen::RenderItems(SDL_Surface * screen, GLint x, GLint &y)
{
	SDL_Color item_color = {MENU_ITEM_COLOR},
			  active_item_color = {MENU_ACTIVE_ITEM_COLOR},
			  opt_label_color = {MENU_OPT_LABEL_COLOR};
	TTF_Font *items_font = RcRegistry::getInstance()->GetFont(FONT_MENUITEMS);
	GLshort i, j;
	SDL_Rect r_down = {GI_RECT_ARROW_DOWN},
			 r_up = {GI_RECT_ARROW_UP},
			 cur = {GI_RECT_CURSOR_L};
	MenuItem * item;
	
	if (items_offset > 0) {
		RenderGUI(x - GLint(r_up.w / 2), y - r_up.h - 1, r_up);
	}
	
	for (j = 0; j < ITEMS_SHOW_LIMIT; j++) {
		i = j + items_offset;
		item = menu->GetItem(i);
		if (! item) {
			// buttons
			break;
		}
		if (item->isOption()) {
			RenderText(item->GetLabel(), items_font, opt_label_color, x - MENU_CURSOR_PAD, y, ALIGN_RIGHT, GL_FALSE);
			if (i == selected_item) {
				RenderText(item->GetOptionLabel(), items_font, active_item_color, x + MENU_CURSOR_PAD + cur.w, y, ALIGN_LEFT, GL_TRUE);
			} else {
				RenderText(item->GetOptionLabel(), items_font, item_color, x + MENU_CURSOR_PAD + cur.w, y, ALIGN_LEFT, GL_FALSE);
			}
		} else {
			if (i == selected_item) {
				RenderText(item->GetLabel(), items_font, active_item_color, x, y, ALIGN_CENTER, GL_TRUE);
			} else {
				RenderText(item->GetLabel(), items_font, item_color, x, y, ALIGN_CENTER, GL_FALSE);
			}
		}
		y += MENU_ITEM_SPACE;
	}
	
	if ((items_offset + ITEMS_SHOW_LIMIT) < menu->CountItems()) {
		RenderGUI(x - GLint(r_down.w / 2), y - 8, r_down);
	}
}

GLvoid Screen::RenderGUI(GLint x, GLint y, SDL_Rect r)
{
	SDL_Surface * gui = RcRegistry::getInstance()->GetImage(IMAGE_GUI_ITEMS);
	SDL_Surface * screen = SDL_GetVideoSurface();
	SDL_Rect rect;
	rect.x = x;
	rect.y = y;
	rect.w = r.w;
	rect.h = r.h;
	SDL_BlitSurface(gui, &r, screen, &rect);
}

GLvoid Screen::RenderMap()
{
	if (repaint) {
		repaint = GL_FALSE;
		
		SDL_Surface * screen = SDL_GetVideoSurface(), *pic;
		SDL_Rect rect, crect, 
				 rect_lock = {GI_RECT_LOCK},
				 rect_nokey = {GI_RECT_KEY0},
				 rect_keys[] = {{GI_RECT_KEY1}, {GI_RECT_KEY2}, {GI_RECT_KEY3}, {GI_RECT_KEY4}},
				 cursor = {GI_RECT_MAP_CURSOR};
		TTF_Font *title_font = RcRegistry::getInstance()->GetFont(FONT_TITLE),
				 *text_font = RcRegistry::getInstance()->GetFont(FONT_MENUITEMS);
		SDL_Color title_color = {MENU_TITLE_COLOR},
				  text_color = {MENU_ITEM_COLOR};
		SDL_Rect r_down = {GI_RECT_ARROW_DOWN},
				 r_up = {GI_RECT_ARROW_UP};
		GLshort i, j, k;
		
		Display::getInstance()->LigthModel(1.0f);
		
		SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
		if (screen_image) {
			RenderBgImage(screen, screen_image);
		}
		GLint x = GLint(screen->w / 2), y = GLint((screen->h - (MAP_HEIGHT + MAP_TITLE_SPACE + MAP_SPACING * 2)) / 2), top;
		RenderText((char*) STR_MAP, title_font, title_color, x, y, ALIGN_CENTER, GL_FALSE);
		y += MAP_TITLE_SPACE + MAP_SPACING;
		top = y;
		
		if (items_offset > 0) {
			RenderGUI(x - GLint(r_up.w / 2), y - r_up.h - 5, r_up);
		}
		
		for (j = 0; j < MAP_ITEMS_SHOW_LIMIT; j++) {
			i = j + items_offset;
			pic = map_items[i].picture;
			rect.x = GLint(screen->w / (MAP_COLS + 1)) * (j % MAP_COLS + 1) - GLint(pic->w / 2);
			rect.y = top + GLint(j / MAP_COLS) * MAP_ROW_H;
			rect.w = pic->w;
			rect.h = pic->h;
			if (i == selected_item) {
				RenderGUI(rect.x - GLint((cursor.w - rect.w) / 2), rect.y - GLint((cursor.h - rect.h) / 2), cursor);
			}
			SDL_BlitSurface(pic, 0, screen, &rect);
			if (map_items[i].locked) {
				RenderGUI(rect.x + GLint((rect.w - rect_lock.w) / 2), rect.y + GLint((rect.h - rect_lock.h) / 2), rect_lock);
			} else {
				y = rect.y + rect.h - (rect_nokey.h + MAP_KEY_PADDING) - MAP_KEYS_SY;
				x = rect.x + rect.w - (rect_nokey.w + MAP_KEY_PADDING) * map_items[i].exit_count - MAP_KEYS_SX;
				for (k = 0; k < map_items[i].exit_count; k++) {
					if (map_items[i].keys[k]) {
						RenderGUI(x, y, rect_keys[k]);
					} else {
						RenderGUI(x, y, rect_nokey);
					}
					x += rect_nokey.w + MAP_KEY_PADDING;
				}
			}
		}
		
		x = GLint(screen->w / 2);
		y = top + MAP_ROW_H * MAP_ROWS + MAP_SPACING;
		
		if ((items_offset + MAP_ITEMS_SHOW_LIMIT) < map_items_count) {
			RenderGUI(x - GLint(r_down.w / 2), y - (MAP_SPACING + 5), r_down);
		}
		
		RenderText(map_items[selected_item].szTitle, text_font, text_color, x, y, ALIGN_CENTER, GL_FALSE);
		
		SDL_UpdateRect(screen, 0, 0, screen->w, screen->h);
		SDL_GL_SwapBuffers();
		
		Display::getInstance()->LigthModel(LIGHT_MODEL_AMBIENT);
	} else {
		SDL_Delay(50);
	}
}

GLvoid Screen::CursorUp()
{
	if (selected_item > 0) {
		selected_item--;
		if (selected_item < items_offset) {
			items_offset = selected_item;
		}
		SetRepaint();
	}
}

GLvoid Screen::CursorDown()
{
	if ((menu->GetOk() && menu->GetCancel() && selected_item < (menu->CountItems() + 1)) ||
		((menu->GetOk() || menu->GetCancel()) && selected_item < menu->CountItems()) ||
		(selected_item < (menu->CountItems() - 1)))
	{
		selected_item++;
		if (selected_item < menu->CountItems()) {
			if (selected_item >= items_offset + ITEMS_SHOW_LIMIT) {
				items_offset++;
			}
		}
		SetRepaint();
	}
}

GLvoid Screen::PrevMapRow()
{
	if (selected_item > 0) {
		selected_item -= MAP_COLS;
		if (selected_item < 0) {
			selected_item = 0;
		}
		if (selected_item < items_offset) {
			items_offset = MAP_COLS * GLshort(selected_item / MAP_COLS);
		}
		SetRepaint();
	}
}

GLvoid Screen::NextMapRow()
{
	if (selected_item < (map_items_count - 1)) {
		selected_item += MAP_COLS;
		if (selected_item >= map_items_count) {
			selected_item = map_items_count - 1;
		}
		if (selected_item >= items_offset + MAP_ITEMS_SHOW_LIMIT) {
			items_offset += MAP_COLS;
		}
		SetRepaint();
	}
}

GLvoid Screen::PrevMapItem()
{
	if (selected_item > 0) {
		selected_item--;
		if (selected_item < items_offset) {
			items_offset = MAP_COLS * GLshort(selected_item / MAP_COLS);
		}
		SetRepaint();
	}
}

GLvoid Screen::NextMapItem()
{
	if (selected_item < (map_items_count - 1)) {
		selected_item++;
		if (selected_item >= items_offset + MAP_ITEMS_SHOW_LIMIT) {
			items_offset += MAP_COLS;
		}
		SetRepaint();
	}
}

LevelPreview * Screen::GetMapItem()
{
	return &map_items[selected_item];
}

GLvoid Screen::SelectItem()
{
	MenuItem * item = menu->GetItem(selected_item);
	if (item && item->isOption()) {
		item->ToggleOption();
		SetRepaint();
	} else {
		Controller::getInstance()->HandleMenuCommand(menu->GetId() << 8 | selected_item);
	}
}

inline GLvoid Screen::Reset()
{
	SetRepaint();
	selected_item = 0;
	items_offset = 0;
}

GLvoid Screen::PrepareMap(LevelPreview * levels, GLushort count)
{
	Reset();
	map_items_count = count;
	map_items = levels;
	screen_image = RcRegistry::getInstance()->GetImage(IMAGE_PAUSE_SCREEN);
}

GLvoid Screen::PrepareMenu(GLshort id)
{
	Reset();
	if (id < COUNT_MENUS) {
		menu = menu_templates[id];
	}
}

GLvoid Screen::Render()
{
	if (repaint && state != SCREEN_FADED) {
		SDL_Surface * video = SDL_GetVideoSurface();
		Display::getInstance()->LigthModel(1.0f);
		SDL_FillRect(video, NULL, SDL_MapRGB(video->format, 0, 0, 0));
		if (screen_image) {
			if (state != SCREEN_NORMAL) {
				Fade(screen_image);
			}
			RenderBgImage(video, screen_image);
		}
		SDL_UpdateRect(video, 0, 0, video->w, video->h);
		SDL_GL_SwapBuffers();
		Display::getInstance()->LigthModel(LIGHT_MODEL_AMBIENT);
	} else {
		SDL_Delay(50);
	}
}

GLvoid Screen::Fade(SDL_Surface * surface)
{
	SDL_SetAlpha(surface, SDL_SRCALPHA, Uint8(255.0 * transparency));
	if (state == SCREEN_FADE_OUT) {
		transparency -= LIGHT_SWITCH_SPEED * Controller::getInstance()->GetTimePast();
		if (transparency <= 0.0f) {
			transparency = 0.0f;
			state = SCREEN_FADED;
		}
	} else if (state == SCREEN_FADE_IN) {
		transparency += LIGHT_SWITCH_SPEED * Controller::getInstance()->GetTimePast();
		if (transparency >= 1.0f) {
			transparency = 1.0f;
			state = SCREEN_NORMAL;
			SDL_SetAlpha(surface, 0, 0);
		}
	}
}

GLvoid Screen::SetCursorPosition(GLshort i)
{
	selected_item = i;
	while (selected_item >= items_offset + ITEMS_SHOW_LIMIT) {
		items_offset++;
	}
}

GLvoid Screen::SetMapCursorPosition(GLshort i)
{
	selected_item = i;
	while (selected_item >= items_offset + MAP_ITEMS_SHOW_LIMIT) {
		items_offset += MAP_COLS;
	}
}
