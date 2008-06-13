/*
** SJUI - SuperJer's User Interface
** v 0.1.fresh
**
** A very simple SDL Gooey by SuperJer
**
** Email = superjer@superjer.com
** Web   = http://www.superjer.com/
**
**
**
*/

#ifndef __SJUI_H__
#define __SJUI_H__


#include <string.h>
#include <SDL/SDL.h>

#include "font.h"



#define SJUIF_EXTENDSV   0x1
#define SJUIF_EXTENDSH   0x2
#define SJUIF_STRETCHFIT 0x4


typedef int SJUI_HANDLE;


typedef struct SJUICONTROL
{
    SJUI_HANDLE handle;
    struct SJUICONTROL *parent;
    struct SJUICONTROL **children;
    SDL_Rect pos;
    int child_count;
    int child_alloc;
    char *caption;
    int visible;
    int interactive;
    int flags;
} SJUICONTROL_t;

void SJUIC_Init(SJUICONTROL_t *);
void SJUIC_Destroy(SJUICONTROL_t *);



typedef struct
{
    int handle_max;
    SJUICONTROL_t root;
    SJUICONTROL_t **shortcuts;
    int shortcut_count;
    int shortcut_alloc;
    SJUI_HANDLE modal;
} SJUI_t;

void SJUI_Init();
void SJUI_Destroy();

void SJUI_Paint(SDL_Surface *screen);
void SJUI_Focus(SJUI_HANDLE handle,int modal);
void SJUI_CheckIn();
int SJUI_MouseTo(int x, int y);
int SJUI_MouseDown(int button);
int SJUI_MouseUp(int button);
int SJUI_KeyDown(int key);
int SJUI_KeyUp(int key);
void SJUI_SetPos(SJUI_HANDLE handle,int x, int y);
void SJUI_SetSize(SJUI_HANDLE handle,int w, int h);
void SJUI_SetCaption(SJUI_HANDLE handle,const char *s);

SJUI_HANDLE SJUI_NewControl(SJUI_HANDLE parent,int w,int h,int flags);
SJUI_HANDLE SJUI_NewMenu(SJUI_HANDLE parent);
SJUICONTROL_t *SJUI_GetByHandle(SJUI_HANDLE parent);





void DrawSquare( SDL_Surface *surf, SDL_Rect *rect, unsigned int color );


#endif



