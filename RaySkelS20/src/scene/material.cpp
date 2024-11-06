#include "ray.h"
#include "material.h"
#include "light.h"

// Apply the phong model to this point on the surface of the object, returning
// the color of that point.
vec3f Material::shade( Scene *scene, const ray& r, const isect& i ) const
{
	// YOUR CODE HERE

	// For now, this method just returns the diffuse color of the object.
	// This gives a single matte color for every distinct surface in the
	// scene, and that's it.  Simple, but enough to get you started.
	// (It's also inconsistent with the phong model...)

	// Your mission is to fill in this method with the rest of the phong
	// shading model, including the contributions of all the light sources.
    // You will need to call both distanceAttenuation() and shadowAttenuation()
    // somewhere in your code in order to compute shadows and light falloff.
	
	vec3f ambientLight = vec3f(0.1, 0.1, 0.1);

	vec3f I = ke + prod(ka, ambientLight);

	vec3f P = r.at(i.t);
	vec3f N = i.N;
	vec3f V = r.getDirection();

	for (Scene::cliter it = scene->beginLights(); it != scene->endLights(); it++) {
		Light* light = *it;
		vec3f L = light->getDirection(P);
		vec3f lightColor = light->getColor(P);

		vec3f diffuse = std::max(0.0, N * L) * kd;
		vec3f R = L - 2 * (N * L) * N;
		R.normalize();
		vec3f specular = std::pow(std::max(0.0, R * V), shininess*128) * ks;

		vec3f shadow = light->shadowAttenuation(P);
		double distance = light->distanceAttenuation(P);

		I = I + prod(prod(diffuse + specular, lightColor) * distance, shadow);
	}

	return I;
}
