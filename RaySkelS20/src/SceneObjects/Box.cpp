#include <cmath>
#include <assert.h>

#include "Box.h"

bool Box::intersectLocal( const ray& r, isect& i ) const
{
	// YOUR CODE HERE:
    // Add box intersection code here.
	// it currently ignores all boxes and just returns false.
	BoundingBox box = ComputeLocalBoundingBox();

	double tMin, tMax;

	if (!box.intersect(r, tMin, tMax)) {
		return false;
	}

	//ray inside or on the box
	if (tMin < RAY_EPSILON) {
		tMin = tMax;
	}

	//ray outside the box
	if (tMax < RAY_EPSILON) {
		return false;
	}

	//ray tangent to the box
	if (abs(tMin - tMax) < RAY_EPSILON) {
		return false;
	}

	i.t = tMin;

	vec3f P = r.at(tMin);

	i.N = vec3f(0, 0, 0);


	//check all 6 sides of the box
	if (abs(P[0] - -0.5) < RAY_EPSILON) {
		i.N = vec3f(-1, 0, 0);
	}
	else if (abs(P[0] - 0.5) < RAY_EPSILON) {
		i.N = vec3f(1, 0, 0);
	}
	else if (abs(P[1] - -0.5) < RAY_EPSILON) {
		i.N = vec3f(0, -1, 0);
	}
	else if (abs(P[1] - 0.5) < RAY_EPSILON) {
		i.N = vec3f(0, 1, 0);
	}
	else if (abs(P[2] - -0.5) < RAY_EPSILON) {
		i.N = vec3f(0, 0, -1);
	}
	else if (abs(P[2] - 0.5) < RAY_EPSILON) {
		i.N = vec3f(0, 0, 1);
	}

	//flip normal if necessary
//	if (r.getDirection().dot(i.N) > 0) {
//		i.N = -i.N;
//	}

	i.obj = this;

	return true;
}
