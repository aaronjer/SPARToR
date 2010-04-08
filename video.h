
#ifndef SPARTOR_VIDEO_H_
#define SPARTOR_VIDEO_H_


extern int v_drawhulls;
extern int v_showstats;
extern int v_usealpha;
extern int v_fullscreen;


void videoinit();
void render();
void setvideo(int w,int h,int go_full,int quiet);
void setvideosoon(int w,int h,int go_full,int delay);

#endif

