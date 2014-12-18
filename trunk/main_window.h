// generated by Fast Light User Interface Designer (fluid) version 1.0302

#ifndef main_window_h
#define main_window_h
#include <FL/Fl.H>
#include "main_winApp.h"
#include <FL/Fl_Double_Window.H>
extern Fl_Double_Window *pMain_win;
#include <FL/Fl_Group.H>
extern Fl_Group *pCamera_group;
#include <FL/Fl_Input.H>
extern Fl_Input *pIP0_txtIn;
extern Fl_Input *pIP1_txtIn;
extern Fl_Input *pIP2_txtIn;
extern Fl_Input *pIP3_txtIn;
extern Fl_Input *pPort_txtIn;
extern Fl_Input *pAccount_txtIn;
extern Fl_Input *pPasswd_txtIn;
extern Fl_Group *pShooter_group;
#include <FL/Fl_Button.H>
extern void RunConnect(Fl_Button*, void*);
extern Fl_Button *pConnect_bttn;
#include <FL/Fl_Light_Button.H>
extern void Disconnect(Fl_Light_Button*, void*);
extern Fl_Light_Button *pClose_litBttn;
extern void ComputeScore(Fl_Light_Button*, void*);
extern Fl_Light_Button *pCalculate_litBttn;
extern MyGlWindow *pTarget_win;
Fl_Double_Window* make_window();
#endif
