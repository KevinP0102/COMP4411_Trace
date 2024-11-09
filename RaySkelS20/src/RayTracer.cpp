// The main ray tracer.

#include <Fl/fl_ask.h>

#include "RayTracer.h"
#include "scene/light.h"
#include "scene/material.h"
#include "scene/ray.h"
#include "fileio/read.h"
#include "fileio/parse.h"
#include "ui/TraceUI.h"

extern TraceUI* traceUI;
extern std::vector<vec3f> distributedRays(vec3f ray, double radius, int count);

// Trace a top-level ray through normalized window coordinates (x,y)
// through the projection plane, and out into the scene.  All we do is
// enter the main ray-tracing method, getting things started by plugging
// in an initial ray weight of (0.0,0.0,0.0) and an initial recursion depth of 0.
vec3f RayTracer::trace( Scene *scene, double x, double y )
{
    ray r( vec3f(0,0,0), vec3f(0,0,0) );
    scene->getCamera()->rayThrough( x,y,r );

	stack<Material> materialStack = stack<Material>();
	materialStack.push(Material::worldMaterial());

	vec3f color = traceRay( scene, r, vec3f(1.0,1.0,1.0), 0, materialStack).clamp();

	if (traceUI->getMotionBlur()) {

		vec3f ray = r.getDirection();
		vec3f up = vec3f(0, 1, 0);

		if ((ray.normalize() - up).length() < RAY_EPSILON) {
			up = vec3f(1, 0, 0);
		}

		vec3f right = ray.cross(up);
		up = right.cross(ray);

		vec3f blur = ray;

		for (int i = 0; i < 9; i++) {
			blur += right * 0.003;
			class::ray blurRay(r.getPosition(), blur.normalize());

			color += traceRay(scene, blurRay, vec3f(1.0, 1.0, 1.0), 0, materialStack).clamp();
		}

		color = color / 10;

	}

	return color;
}

// Do recursive ray tracing!  You'll want to insert a lot of code here
// (or places called from here) to handle reflection, refraction, etc etc.
vec3f RayTracer::traceRay( Scene *scene, const ray& r, 
	const vec3f& thresh, int depth, stack<Material> materials )
{
	isect i;
	
	if (depth > traceUI->getDepth()) {
		return vec3f(0, 0, 0);
	}

	if( scene->intersect( r, i ) ) {
		// YOUR CODE HERE

		// An intersection occured!  We've got work to do.  For now,
		// this code gets the material for the surface that was intersected,
		// and asks that material to provide a color for the ray.  

		// This is a great place to insert code for recursive ray tracing.
		// Instead of just returning the result of shade(), add some
		// more steps: add in the contributions from reflected and refracted
		// rays.

		const Material& m = i.getMaterial();
		vec3f I = m.shade(scene, r, i);

		if (traceUI->getThresh() > I.length()) {
			return I;
		}

		vec3f P = r.at(i.t);
		vec3f N = i.N;
		vec3f V = r.getDirection();

		if (&materials.top() == &m) {
			N = -N;
		}

		vec3f L = V - 2 * (N * V) * N;

		vec3f reflectionColor;
		ray reflectionRay(P, L);

		reflectionColor = 
			prod(traceRay(scene, reflectionRay, thresh, depth + 1, materials), m.kr);

		if (traceUI->getGlossyRefl()) {

			std::vector<vec3f> rays = distributedRays(L, 0.02, 39);

			for (const vec3f& r : rays) {
				ray reflectionRay(P, r);
				reflectionColor += 
					prod(traceRay(scene, reflectionRay, thresh, max(depth+1, traceUI->getDepth()), materials), m.kr);
			}

			reflectionColor = reflectionColor / (rays.size()+1);
		}

		I = I + reflectionColor;



		//refraction
		if (m.kt.length() > 0) {

			double n1 = materials.top().index;
			double n2;

			if (&materials.top() == &m) { //inside
				materials.pop();
				n2 = materials.top().index;
			}
			else { //outside
				materials.push(m);
				n2 = m.index;
			}

			reflectionRay = ray(P, calculateRefractedRay(V, N, n1, n2));

			vec3f refractionColor = traceRay(scene, reflectionRay, thresh, depth + 1, materials);
			I = I + prod(refractionColor, m.kt);

		}

		return I;
	
	} else {
		// No intersection.  This ray travels to infinity, so we color
		// it according to the background color, which in this (simple) case
		// is just black.

		return vec3f( 0.0, 0.0, 0.0 );
	}
}

vec3f RayTracer::calculateRefractedRay(vec3f i, vec3f n, double n1, double n2) {
	if (abs(abs(n * i) - 1) < RAY_EPSILON)
		return i;

	double sinAngleI = sqrt(1 - pow(n * i, 2));
	double sinAngleT = (n1 / n2) * sinAngleI;

	double angleI = asin(sinAngleI);
	double angleT = asin(sinAngleT);

	double sinDiff = sin(abs(angleI - angleT));
	double d;

	if (n1 == n2) {
		return i;
	}

	if (n1 > n2) {

		double critical = n2 / n1;

		if (critical - sinAngleI > RAY_EPSILON) {

			double sinAlpha = sin(3.14159 - angleT);
			d = sinAlpha / sinDiff;

			return -(-i * d + (-n)).normalize();
		}

		return vec3f(0.0, 0.0, 0.0); //total internal reflection
	}

	d = sinAngleT / sinDiff;
	return (i * d + (-n)).normalize();
}

RayTracer::RayTracer() : distribution(0.0, 1.0)
{
	buffer = NULL;
	buffer_width = buffer_height = 256;
	scene = NULL;

	m_bSceneLoaded = false;
}


RayTracer::~RayTracer()
{
	delete [] buffer;
	delete scene;
}

void RayTracer::getBuffer( unsigned char *&buf, int &w, int &h )
{
	buf = buffer;
	w = buffer_width;
	h = buffer_height;
}

double RayTracer::aspectRatio()
{
	return scene ? scene->getCamera()->getAspectRatio() : 1;
}

bool RayTracer::sceneLoaded()
{
	return m_bSceneLoaded;
}

bool RayTracer::loadScene( char* fn )
{
	try
	{
		scene = readScene( fn );
	}
	catch( ParseError pe )
	{
		fl_alert( "ParseError: %s\n", pe );
		return false;
	}

	if( !scene )
		return false;
	
	buffer_width = 256;
	buffer_height = (int)(buffer_width / scene->getCamera()->getAspectRatio() + 0.5);

	bufferSize = buffer_width * buffer_height * 3;
	buffer = new unsigned char[ bufferSize ];
	
	// separate objects into bounded and unbounded
	scene->initScene();
	
	// Add any specialized scene loading code here
	
	m_bSceneLoaded = true;

	return true;
}

void RayTracer::traceSetup( int w, int h )
{
	if( buffer_width != w || buffer_height != h )
	{
		buffer_width = w;
		buffer_height = h;

		bufferSize = buffer_width * buffer_height * 3;
		delete [] buffer;
		buffer = new unsigned char[ bufferSize ];
	}
	memset( buffer, 0, w*h*3 );
}

void RayTracer::traceLines( int start, int stop )
{
	vec3f col;
	if( !scene )
		return;

	if( stop > buffer_height )
		stop = buffer_height;

	for( int j = start; j < stop; ++j )
		for( int i = 0; i < buffer_width; ++i )
			tracePixel(i,j);
}

void RayTracer::tracePixel( int i, int j )
{
	vec3f col;

	if( !scene )
		return;

	double x = double(i)/double(buffer_width);
	double y = double(j)/double(buffer_height);

	col = trace( scene,x,y );

	unsigned char *pixel = buffer + ( i + j * buffer_width ) * 3;

	pixel[0] = (int)( 255.0 * col[0]);
	pixel[1] = (int)( 255.0 * col[1]);
	pixel[2] = (int)( 255.0 * col[2]);
}