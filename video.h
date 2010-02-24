
#ifndef SPARTOR_VIDEO_H_
#define SPARTOR_VIDEO_H_


extern int drawhulls;
extern int showstats;
extern int fullscreen;
extern int screen_w;
extern int screen_h;
extern int desktop_w;
extern int desktop_h;
extern int video_reset;


void render();
void setvideo(int w,int h,Uint32 flags);

#endif

