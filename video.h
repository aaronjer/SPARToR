
#ifndef SPARTOR_VIDEO_H_
#define SPARTOR_VIDEO_H_

#define VIDCAPS(caps,vidinfo) {                                                                               \
  sprintf(caps,"HW:%d WM:%d bHW:%d bHWCC:%d bHWA:%d bSW:%d bSWCC:%d bSWA:%d bFill:%d VMem:%d "                  \
               "bpp:%d Bpp:%d loss:%d-%d-%d-%d shift:%d-%d-%d-%d mask:%x-%x-%x-%x key:%d alpha:%d", \
          vidinfo->hw_available,vidinfo->wm_available,vidinfo->blit_hw,vidinfo->blit_hw_CC,                     \
          vidinfo->blit_hw_A,vidinfo->blit_sw,vidinfo->blit_sw_CC,vidinfo->blit_sw_A,                           \
          vidinfo->blit_fill,vidinfo->video_mem,                                                                \
          vidinfo->vfmt->BitsPerPixel,vidinfo->vfmt->BytesPerPixel,                                             \
          vidinfo->vfmt->Rloss,vidinfo->vfmt->Gloss,vidinfo->vfmt->Bloss,vidinfo->vfmt->Aloss,                  \
          vidinfo->vfmt->Rshift,vidinfo->vfmt->Gshift,vidinfo->vfmt->Bshift,vidinfo->vfmt->Ashift,              \
          vidinfo->vfmt->Rmask,vidinfo->vfmt->Gmask,vidinfo->vfmt->Bmask,vidinfo->vfmt->Amask,                  \
          vidinfo->vfmt->colorkey,vidinfo->vfmt->alpha); }


extern int drawhulls;
extern int showstats;
extern int fullscreen;
extern int screen_w;
extern int screen_h;
extern int desktop_w;
extern int desktop_h;
extern int video_reset;


void render();
void setvideo(int w,int h,int quiet);

#endif

