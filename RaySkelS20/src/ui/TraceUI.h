//
// rayUI.h
//
// The header file for the UI part
//

#ifndef __rayUI_h__
#define __rayUI_h__

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Button.H>

#include <FL/fl_file_chooser.H>		// FLTK file chooser

#include "TraceGLWindow.h"

class TraceUI {
public:
	TraceUI();

	// The FLTK widgets
	Fl_Window*			m_mainWindow;
	Fl_Menu_Bar*		m_menubar;

	Fl_Slider*			m_sizeSlider;
	Fl_Slider*			m_depthSlider;
	Fl_Slider*			m_DistASlider;
	Fl_Slider*			m_DistBSlider;
	Fl_Slider*			m_DistCSlider;
	Fl_Slider*			m_ThreshSlider;

	Fl_Slider*			m_softShadowSlider;
	Fl_Slider*			m_glossyReflSlider;	
	Fl_Slider*			m_motionBlurSlider;

	Fl_Button*			m_renderButton;
	Fl_Button*			m_stopButton;

	TraceGLWindow*		m_traceGlWindow;

	// member functions
	void show();

	void		setRayTracer(RayTracer *tracer);

	int			getSize();
	int			getDepth();
	double		getDistA();
	double		getDistB();
	double 		getDistC();
	double		getThresh();

	bool 	    getSoftShadow();
	bool		getGlossyRefl();
	bool		getMotionBlur();

private:
	RayTracer*	raytracer;

	int			m_nSize;
	int			m_nDepth;
	double		m_nDistA = 0.25;
	double		m_nDistB = 0.05;
	double		m_nDistC = 0.025;
	double		m_nThresh = 0.05;

	int			m_nSoftShadow = 0;
	int			m_nGlossyRefl = 0;
	int			m_nMotionBlur = 0;

// static class members
	static Fl_Menu_Item menuitems[];

	static TraceUI* whoami(Fl_Menu_* o);

	static void cb_load_scene(Fl_Menu_* o, void* v);
	static void cb_save_image(Fl_Menu_* o, void* v);
	static void cb_exit(Fl_Menu_* o, void* v);
	static void cb_about(Fl_Menu_* o, void* v);

	static void cb_exit2(Fl_Widget* o, void* v);

	static void cb_sizeSlides(Fl_Widget* o, void* v);
	static void cb_depthSlides(Fl_Widget* o, void* v);
	static void cb_DistASlides(Fl_Widget* o, void* v);
	static void cb_DistBSlides(Fl_Widget* o, void* v);
	static void cb_DistCSlides(Fl_Widget* o, void* v);
	static void cb_ThreshSlides(Fl_Widget* o, void* v);

	static void cb_softShadowSlides(Fl_Widget* o, void* v);
	static void cb_glossyReflSlides(Fl_Widget* o, void* v);
	static void cb_motionBlurSlides(Fl_Widget* o, void* v);

	static void cb_render(Fl_Widget* o, void* v);
	static void cb_stop(Fl_Widget* o, void* v);
};

#endif
