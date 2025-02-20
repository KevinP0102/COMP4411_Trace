//
// TraceUI.h
//
// Handles FLTK integration and other user interface tasks
//
#include <stdio.h>
#include <time.h>
#include <string.h>

#include <FL/fl_ask.h>

#include "TraceUI.h"
#include "../RayTracer.h"

static bool done;

//------------------------------------- Help Functions --------------------------------------------
TraceUI* TraceUI::whoami(Fl_Menu_* o)	// from menu item back to UI itself
{
	return ( (TraceUI*)(o->parent()->user_data()) );
}

//--------------------------------- Callback Functions --------------------------------------------
void TraceUI::cb_load_scene(Fl_Menu_* o, void* v) 
{
	TraceUI* pUI=whoami(o);
	
	char* newfile = fl_file_chooser("Open Scene?", "*.ray", NULL );

	if (newfile != NULL) {
		char buf[256];

		if (pUI->raytracer->loadScene(newfile)) {
			sprintf(buf, "Ray <%s>", newfile);
			done=true;	// terminate the previous rendering
		} else{
			sprintf(buf, "Ray <Not Loaded>");
		}

		pUI->m_mainWindow->label(buf);
	}
}

void TraceUI::cb_save_image(Fl_Menu_* o, void* v) 
{
	TraceUI* pUI=whoami(o);
	
	char* savefile = fl_file_chooser("Save Image?", "*.bmp", "save.bmp" );
	if (savefile != NULL) {
		pUI->m_traceGlWindow->saveImage(savefile);
	}
}

void TraceUI::cb_exit(Fl_Menu_* o, void* v)
{
	TraceUI* pUI=whoami(o);

	// terminate the rendering
	done=true;

	pUI->m_traceGlWindow->hide();
	pUI->m_mainWindow->hide();
}

void TraceUI::cb_exit2(Fl_Widget* o, void* v) 
{
	TraceUI* pUI=(TraceUI *)(o->user_data());
	
	// terminate the rendering
	done=true;

	pUI->m_traceGlWindow->hide();
	pUI->m_mainWindow->hide();
}

void TraceUI::cb_about(Fl_Menu_* o, void* v) 
{
	fl_message("RayTracer Project, FLTK version for CS 341 Spring 2002. Latest modifications by Jeff Maurer, jmaurer@cs.washington.edu");
}

void TraceUI::cb_sizeSlides(Fl_Widget* o, void* v)
{
	TraceUI* pUI=(TraceUI*)(o->user_data());
	
	pUI->m_nSize=int( ((Fl_Slider *)o)->value() ) ;
	int	height = (int)(pUI->m_nSize / pUI->raytracer->aspectRatio() + 0.5);
	pUI->m_traceGlWindow->resizeWindow( pUI->m_nSize, height );
}

void TraceUI::cb_depthSlides(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_nDepth=int( ((Fl_Slider *)o)->value() ) ;
}

void TraceUI::cb_DistASlides(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_nDistA = double(((Fl_Slider*)o)->value());
}

void TraceUI::cb_DistBSlides(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_nDistB = double(((Fl_Slider*)o)->value());
}

void TraceUI::cb_DistCSlides(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_nDistC = double(((Fl_Slider*)o)->value());
}

void TraceUI::cb_ThreshSlides(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_nThresh = double(((Fl_Slider*)o)->value());
}

void TraceUI::cb_softShadowSlides(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_nSoftShadow = int(((Fl_Slider*)o)->value());
}

void TraceUI::cb_glossyReflSlides(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_nGlossyRefl = int(((Fl_Slider*)o)->value());
}

void TraceUI::cb_motionBlurSlides(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_nMotionBlur = int(((Fl_Slider*)o)->value());
}

void TraceUI::cb_render(Fl_Widget* o, void* v)
{
	char buffer[256];

	TraceUI* pUI=((TraceUI*)(o->user_data()));
	
	if (pUI->raytracer->sceneLoaded()) {
		int width=pUI->getSize();
		int	height = (int)(width / pUI->raytracer->aspectRatio() + 0.5);
		pUI->m_traceGlWindow->resizeWindow( width, height );

		pUI->m_traceGlWindow->show();

		pUI->raytracer->traceSetup(width, height);
		
		// Save the window label
		const char *old_label = pUI->m_traceGlWindow->label();

		// start to render here	
		done=false;
		clock_t prev, now;
		prev=clock();
		
		pUI->m_traceGlWindow->refresh();
		Fl::check();
		Fl::flush();

		for (int y=0; y<height; y++) {
			for (int x=0; x<width; x++) {
				if (done) break;
				
				// current time
				now = clock();

				// check event every 1/2 second
				if (((double)(now-prev)/CLOCKS_PER_SEC)>0.5) {
					prev=now;

					if (Fl::ready()) {
						// refresh
						pUI->m_traceGlWindow->refresh();
						// check event
						Fl::check();

						if (Fl::damage()) {
							Fl::flush();
						}
					}
				}

				pUI->raytracer->tracePixel( x, y );
		
			}
			if (done) break;

			// flush when finish a row
			if (Fl::ready()) {
				// refresh
				pUI->m_traceGlWindow->refresh();

				if (Fl::damage()) {
					Fl::flush();
				}
			}
			// update the window label
			sprintf(buffer, "(%d%%) %s", (int)((double)y / (double)height * 100.0), old_label);
			pUI->m_traceGlWindow->label(buffer);
			
		}
		done=true;
		pUI->m_traceGlWindow->refresh();

		// Restore the window label
		pUI->m_traceGlWindow->label(old_label);		
	}
}

void TraceUI::cb_stop(Fl_Widget* o, void* v)
{
	done=true;
}

void TraceUI::show()
{
	m_mainWindow->show();
}

void TraceUI::setRayTracer(RayTracer *tracer)
{
	raytracer = tracer;
	m_traceGlWindow->setRayTracer(tracer);
}

int TraceUI::getSize()
{
	return m_nSize;
}

int TraceUI::getDepth()
{
	return m_nDepth;
}

double TraceUI::getDistA()
{
	return m_nDistA;
}

double TraceUI::getDistB()
{
	return m_nDistB;
}

double TraceUI::getDistC()
{
	return m_nDistC;
}

double TraceUI::getThresh()
{
	return m_nThresh;
}

bool TraceUI::getSoftShadow()
{
	return m_nSoftShadow;
}

bool TraceUI::getGlossyRefl()
{
	return m_nGlossyRefl;
}

bool TraceUI::getMotionBlur()
{
	return m_nMotionBlur;
}

// menu definition
Fl_Menu_Item TraceUI::menuitems[] = {
	{ "&File",		0, 0, 0, FL_SUBMENU },
		{ "&Load Scene...",	FL_ALT + 'l', (Fl_Callback *)TraceUI::cb_load_scene },
		{ "&Save Image...",	FL_ALT + 's', (Fl_Callback *)TraceUI::cb_save_image },
		{ "&Exit",			FL_ALT + 'e', (Fl_Callback *)TraceUI::cb_exit },
		{ 0 },

	{ "&Help",		0, 0, 0, FL_SUBMENU },
		{ "&About",	FL_ALT + 'a', (Fl_Callback *)TraceUI::cb_about },
		{ 0 },

	{ 0 }
};

TraceUI::TraceUI() {
	// init.
	m_nDepth = 0;
	m_nSize = 150;
	m_mainWindow = new Fl_Window(100, 40, 330, 260, "Ray <Not Loaded>");
		m_mainWindow->user_data((void*)(this));	// record self to be used by static callback functions
		// install menu bar
		m_menubar = new Fl_Menu_Bar(0, 0, 320, 25);
		m_menubar->menu(menuitems);

		// install slider depth
		m_depthSlider = new Fl_Value_Slider(10, 30, 180, 20, "Depth");
		m_depthSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_depthSlider->type(FL_HOR_NICE_SLIDER);
        m_depthSlider->labelfont(FL_COURIER);
        m_depthSlider->labelsize(12);
		m_depthSlider->minimum(0);
		m_depthSlider->maximum(10);
		m_depthSlider->step(1);
		m_depthSlider->value(m_nDepth);
		m_depthSlider->align(FL_ALIGN_RIGHT);
		m_depthSlider->callback(cb_depthSlides);

		// install slider size
		m_sizeSlider = new Fl_Value_Slider(10, 55, 180, 20, "Size");
		m_sizeSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_sizeSlider->type(FL_HOR_NICE_SLIDER);
        m_sizeSlider->labelfont(FL_COURIER);
        m_sizeSlider->labelsize(12);
		m_sizeSlider->minimum(64);
		m_sizeSlider->maximum(512);
		m_sizeSlider->step(1);
		m_sizeSlider->value(m_nSize);
		m_sizeSlider->align(FL_ALIGN_RIGHT);
		m_sizeSlider->callback(cb_sizeSlides);

		m_DistASlider = new Fl_Value_Slider(10, 80, 180, 20, "Dist Attenuation A");
		m_DistASlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_DistASlider->type(FL_HOR_NICE_SLIDER);
		m_DistASlider->labelfont(FL_COURIER);
		m_DistASlider->labelsize(12);
		m_DistASlider->minimum(0);
		m_DistASlider->maximum(1);
		m_DistASlider->step(0.01);
		m_DistASlider->value(m_nDistA);
		m_DistASlider->align(FL_ALIGN_RIGHT);
		m_DistASlider->callback(cb_DistASlides);

		m_DistBSlider = new Fl_Value_Slider(10, 105, 180, 20, "Dist Attenuation B");
		m_DistBSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_DistBSlider->type(FL_HOR_NICE_SLIDER);
		m_DistBSlider->labelfont(FL_COURIER);
		m_DistBSlider->labelsize(12);
		m_DistBSlider->minimum(0);
		m_DistBSlider->maximum(1 * 0.1);
		m_DistBSlider->step(0.01 * 0.1);
		m_DistBSlider->value(m_nDistB);
		m_DistBSlider->align(FL_ALIGN_RIGHT);
		m_DistBSlider->callback(cb_DistBSlides);

		m_DistCSlider = new Fl_Value_Slider(10, 130, 180, 20, "Dist Attenuation C");
		m_DistCSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_DistCSlider->type(FL_HOR_NICE_SLIDER);
		m_DistCSlider->labelfont(FL_COURIER);
		m_DistCSlider->labelsize(12);
		m_DistCSlider->minimum(0);
		m_DistCSlider->maximum(1 * 0.05);
		m_DistCSlider->step(0.01 * 0.05);
		m_DistCSlider->value(m_nDistC);
		m_DistCSlider->align(FL_ALIGN_RIGHT);
		m_DistCSlider->callback(cb_DistCSlides);

		m_ThreshSlider = new Fl_Value_Slider(10, 155, 180, 20, "Threshold");
		m_ThreshSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_ThreshSlider->type(FL_HOR_NICE_SLIDER);
		m_ThreshSlider->labelfont(FL_COURIER);
		m_ThreshSlider->labelsize(12);
		m_ThreshSlider->minimum(0);
		m_ThreshSlider->maximum(1);
		m_ThreshSlider->step(0.01);
		m_ThreshSlider->value(m_nThresh);
		m_ThreshSlider->align(FL_ALIGN_RIGHT);
		m_ThreshSlider->callback(cb_ThreshSlides);

		m_softShadowSlider = new Fl_Value_Slider(10, 180, 180, 20, "Soft Shadow");
		m_softShadowSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_softShadowSlider->type(FL_HOR_NICE_SLIDER);
		m_softShadowSlider->labelfont(FL_COURIER);
		m_softShadowSlider->labelsize(12);
		m_softShadowSlider->minimum(0);
		m_softShadowSlider->maximum(1);
		m_softShadowSlider->step(1);
		m_softShadowSlider->value(m_nSoftShadow);
		m_softShadowSlider->align(FL_ALIGN_RIGHT);
		m_softShadowSlider->callback(cb_softShadowSlides);

		m_glossyReflSlider = new Fl_Value_Slider(10, 205, 180, 20, "Glossy Reflection");
		m_glossyReflSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_glossyReflSlider->type(FL_HOR_NICE_SLIDER);
		m_glossyReflSlider->labelfont(FL_COURIER);
		m_glossyReflSlider->labelsize(12);
		m_glossyReflSlider->minimum(0);
		m_glossyReflSlider->maximum(1);
		m_glossyReflSlider->step(1);
		m_glossyReflSlider->value(m_nGlossyRefl);
		m_glossyReflSlider->align(FL_ALIGN_RIGHT);
		m_glossyReflSlider->callback(cb_glossyReflSlides);

		m_motionBlurSlider = new Fl_Value_Slider(10, 230, 180, 20, "Motion Blur");
		m_motionBlurSlider->user_data((void*)(this));	// record self to be used by static callback functions
		m_motionBlurSlider->type(FL_HOR_NICE_SLIDER);
		m_motionBlurSlider->labelfont(FL_COURIER);
		m_motionBlurSlider->labelsize(12);
		m_motionBlurSlider->minimum(0);
		m_motionBlurSlider->maximum(1);
		m_motionBlurSlider->step(1);
		m_motionBlurSlider->value(m_nMotionBlur);
		m_motionBlurSlider->align(FL_ALIGN_RIGHT);
		m_motionBlurSlider->callback(cb_motionBlurSlides);

		m_renderButton = new Fl_Button(240, 27, 70, 25, "&Render");
		m_renderButton->user_data((void*)(this));
		m_renderButton->callback(cb_render);

		m_stopButton = new Fl_Button(240, 55, 70, 25, "&Stop");
		m_stopButton->user_data((void*)(this));
		m_stopButton->callback(cb_stop);

		m_mainWindow->callback(cb_exit2);
		m_mainWindow->when(FL_HIDE);
    m_mainWindow->end();

	// image view
	m_traceGlWindow = new TraceGLWindow(100, 150, m_nSize, m_nSize, "Rendered Image");
	m_traceGlWindow->end();
	m_traceGlWindow->resizable(m_traceGlWindow);
}