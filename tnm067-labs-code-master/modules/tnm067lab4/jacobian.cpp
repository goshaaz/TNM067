#include <modules/tnm067lab4/jacobian.h>

namespace inviwo {

namespace util {

mat2 jacobian(const ImageSampler& sampler, vec2 position, vec2 offset) {
    mat2 J(1); 
	auto dVx = (sampler.sample(position.x + offset.x, position.y) - sampler.sample(position.x - offset.x, position.y)) / (2.0 * offset.x);
	auto dVy = (sampler.sample(position.x, position.y + offset.y) - sampler.sample(position.x, position.y - offset.y)) / (2.0 * offset.y);
	J[0][0] = dVx.x;
	J[1][0] = dVy.x;
	J[0][1] = dVx.y;
	J[1][1] = dVy.y;
	return J;
}

}  // namespace util

}  // namespace inviwo
