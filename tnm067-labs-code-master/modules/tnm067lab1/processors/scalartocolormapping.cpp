#include <modules/tnm067lab1/utils/scalartocolormapping.h>


namespace inviwo {

void ScalarToColorMapping::clearColors() { baseColors_.clear(); }
void ScalarToColorMapping::addBaseColors(vec4 color) { baseColors_.push_back(color); }
#pragma optimize("", off)
vec4 ScalarToColorMapping::sample(float t) const {
    if (baseColors_.size() == 0) return vec4(t);
    if (baseColors_.size() == 1) return vec4(baseColors_[0]);

    // Implement here:
    // Interpolate colors in baseColors_
    // return the right values

    if (t <= 0) return vec4(baseColors_.front());
    if (t >= 1) return vec4(baseColors_.back());

    float R, G, B;

    float interval = float(1) / float((baseColors_.size() - 1));

    float rightside = interval;
    
    float leftside = 0.0f;

    float tnew;

    for (int i = 0; i < baseColors_.size()-1; i++) {
        if (t > leftside && t <= rightside) {
            tnew = (t-leftside) / (rightside-leftside);
            R = baseColors_[i][0] + (baseColors_[i + 1][0] - baseColors_[i][0]) * tnew;
            G = baseColors_[i][1] + (baseColors_[i + 1][1] - baseColors_[i][1]) * tnew;
            B = baseColors_[i][2] + (baseColors_[i + 1][2] - baseColors_[i][2]) * tnew;
            vec4 finalColor(R, G, B, 1);
            return finalColor;
            break;
        }
        leftside = leftside + interval;
        rightside = rightside + interval;
    }
}
#pragma optimize("", on)
}  // namespace inviwo
