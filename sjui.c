

#include "sjui.h"

static SJUI_t SJUI;



void SJUI_Init()
{
    SJUIC_Init(&SJUI.root);
    SJUI.handle_max = 1;
    SJUI.shortcut_count = 1;
    SJUI.shortcut_alloc = 8;
    SJUI.shortcuts = malloc(sizeof(SJUICONTROL_t*)*SJUI.shortcut_alloc);
    SJUI.shortcuts[0] = &SJUI.root;
    SJUI.modal = 0;
}


void SJUI_Destroy()
{
    free(SJUI.shortcuts);
    SJUIC_Destroy(&SJUI.root);
}


void SJUI_Paint( SDL_Surface *screen )
{
    int i;
    int visible;
    SDL_Rect rect;
    SJUICONTROL_t *p, *q;

    for( i=0; i<SJUI.shortcut_count; i++ )
    {
        p = SJUI.shortcuts[i];
        visible = p->visible;
        rect = p->pos;

        q = p;
        while( q->parent )
        {
            q = q->parent;
            rect.x += q->pos.x;
            rect.y += q->pos.y;
            if( !q->visible )
                visible = 0;
        }

        if( visible )
        {
            SDL_FillRect(screen,&rect,0xEEEEEE);
            DrawSquare(screen,&rect,0x000000);
        }
    }
}


void SJUI_Focus(SJUI_HANDLE handle, int modal)
{
    ;
}


SJUI_HANDLE SJUI_NewControl(SJUI_HANDLE parent, int w, int h, int flags)
{
    int i;
    SJUICONTROL_t *p = SJUI_GetByHandle(parent);
    SJUICONTROL_t *q;
    if( p->child_count >= p->child_alloc )
        p->children = realloc( p->children, sizeof(SJUICONTROL_t*)*(p->child_alloc*=2) );
    q = p->children[p->child_count++] = malloc(sizeof(SJUICONTROL_t));
    SJUIC_Init(q);
    q->parent = p;
    q->flags = flags;
    q->handle = SJUI.handle_max++;
    q->pos.w = w;
    q->pos.h = h;
    if( SJUI.shortcut_count >= SJUI.shortcut_alloc )
        SJUI.shortcuts = realloc( SJUI.shortcuts, sizeof(SJUICONTROL_t*)*(SJUI.shortcut_alloc*=2) );
    SJUI.shortcuts[SJUI.shortcut_count++] = q;

    if( flags&SJUIF_EXTENDSV )
    {
        int max = 0;
        int temp;
        for( i=0; i<p->child_count; i++ )
        {
            temp = p->children[i]->pos.y + p->children[i]->pos.h;
            if( temp > max )
                max = temp;
        }
        q->pos.y = max;
    }

    if( flags&SJUIF_EXTENDSV || flags&SJUIF_EXTENDSH )
    {
        if( p->pos.h < q->pos.y+q->pos.h )
            p->pos.h = q->pos.y+q->pos.h;
        if( p->pos.w < q->pos.x+q->pos.w )
            p->pos.w = q->pos.x+q->pos.w;
    }

    return q->handle;
}


void SJUI_SetPos(SJUI_HANDLE handle,int x, int y)
{
    SJUICONTROL_t *p = SJUI_GetByHandle(handle);
    p->pos.x = x;
    p->pos.y = y;
}


void SJUI_SetSize(SJUI_HANDLE handle,int w, int h)
{
    SJUICONTROL_t *p = SJUI_GetByHandle(handle);
    p->pos.w = w;
    p->pos.h = h;
}


void SJUI_SetCaption(SJUI_HANDLE handle,const char *s)
{
    SJUICONTROL_t *p = SJUI_GetByHandle(handle);
    p->caption = realloc(p->caption,strlen(s)+1);
    strcpy(p->caption,s);
}


SJUI_HANDLE SJUI_NewMenu(SJUI_HANDLE parent)
{
    return 0;
}


SJUICONTROL_t *SJUI_GetByHandle(SJUI_HANDLE handle)
{
    int i;
    for( i=0; i<SJUI.shortcut_count; i++ )
        if( SJUI.shortcuts[i]->handle == handle )
            return SJUI.shortcuts[i];
    return &SJUI.root;
}


void SJUIC_Init(SJUICONTROL_t *p)
{
    p->handle = 0;
    p->parent = NULL;
    p->pos.x = 0;
    p->pos.y = 0;
    p->pos.w = 0;
    p->pos.h = 0;
    p->child_count = 0;
    p->child_alloc = 8;
    p->children = malloc(sizeof(SJUICONTROL_t*)*p->child_alloc);
    p->caption = NULL;
    p->visible = 1;
    p->interactive = 1;
    p->flags = 0;
}


void SJUIC_Destroy(SJUICONTROL_t *p)
{
    int i;
    for(i=0; i<p->child_count; i++)
    {
        SJUIC_Destroy(p->children[i]);
        free( p->children[i] );
    }
    free(p->children);
    free(p->caption);
}











void DrawSquare( SDL_Surface *surf, SDL_Rect *rect, unsigned int color )
{
	SDL_Rect edge;
	edge = *rect;
	edge.w = 1;
	SDL_FillRect(surf,&edge,color);
	edge.x += rect->w - 1;
	SDL_FillRect(surf,&edge,color);
	edge = *rect;
	edge.h = 1;
	SDL_FillRect(surf,&edge,color);
	edge.y += rect->h - 1;
	SDL_FillRect(surf,&edge,color);
}


