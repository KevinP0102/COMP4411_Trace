#include <cmath>

#include "light.h"
#include "../ui/TraceUI.h"

extern TraceUI* traceUI;

double DirectionalLight::distanceAttenuation( const vec3f& P ) const
{
	// distance to light is infinite, so f(di) goes to 0.  Return 1.
	return 1.0;
}


vec3f DirectionalLight::shadowAttenuation( const vec3f& P ) const
{
    // YOUR CODE HERE:
    // You should implement shadow-handling code here.

	vec3f d = -orientation;
	ray shadowRay(P, d);

	vec3f c = color;
	isect i;
	if (scene->intersect(shadowRay, i)) {
		c = i.getMaterial().kt;
	}


    return c;
}

vec3f DirectionalLight::getColor( const vec3f& P ) const
{
	// Color doesn't depend on P 
	return color;
}

vec3f DirectionalLight::getDirection( const vec3f& P ) const
{
	return -orientation;
}

double PointLight::distanceAttenuation( const vec3f& P ) const
{
	// YOUR CODE HERE

	// You'll need to modify this method to attenuate the intensity 
	// of the light based on the distance between the source and the 
	// point P.  For now, I assume no attenuation and just return 1.0
	
	double constantTerm = traceUI->getDistA();
	double linearTerm = traceUI->getDistB();
	double quadraticTerm = traceUI->getDistC();

	double distance = (position - P).length();
	double attenuation = 1.0 / (constantTerm + linearTerm * distance + quadraticTerm * distance * distance);
	if (attenuation < 0.0) {
		return 0.0;
	}
	else if (attenuation > 1.0) {
		return 1.0;
	}
	else {
		return attenuation;
	}

}

vec3f PointLight::getColor( const vec3f& P ) const
{
	// Color doesn't depend on P 
	return color;
}

vec3f PointLight::getDirection( const vec3f& P ) const
{
	return (position - P).normalize();
}


vec3f PointLight::shadowAttenuation(const vec3f& P) const
{
    // YOUR CODE HERE:
    // You should implement shadow-handling code here.

	vec3f d = (position - P).normalize();
	ray shadowRay(P, d);

	vec3f c = color;
	isect i;
	if (scene->intersect(shadowRay, i)) {
		if ((shadowRay.at(i.t) - P).length() < (position - P).length()) {
			c = i.getMaterial().kt;
		}
	}

    return c;
}
