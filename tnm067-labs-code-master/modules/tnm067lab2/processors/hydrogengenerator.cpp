#include <modules/tnm067lab2/processors/hydrogengenerator.h>
#include <inviwo/core/datastructures/volume/volume.h>
#include <inviwo/core/util/volumeramutils.h>
#include <modules/base/algorithm/dataminmax.h>
#include <inviwo/core/util/indexmapper.h>
#include <inviwo/core/datastructures/volume/volumeram.h>
#include <modules/base/algorithm/dataminmax.h>
#include <cmath>

namespace inviwo {

const ProcessorInfo HydrogenGenerator::processorInfo_{
    "org.inviwo.HydrogenGenerator",  // Class identifier
    "Hydrogen Generator",            // Display name
    "TNM067",                        // Category
    CodeState::Stable,               // Code state
    Tags::CPU,                       // Tags
};

const ProcessorInfo HydrogenGenerator::getProcessorInfo() const { return processorInfo_; }

HydrogenGenerator::HydrogenGenerator()
    : Processor(), volume_("volume"), size_("size_", "Volume Size", 16, 4, 256) {
    addPort(volume_);
    addProperty(size_);
}

void HydrogenGenerator::process() {
    auto vol = std::make_shared<Volume>(size3_t(size_), DataFloat32::get());

    auto ram = vol->getEditableRepresentation<VolumeRAM>();
    auto data = static_cast<float*>(ram->getData());
    util::IndexMapper3D index(ram->getDimensions());

    util::forEachVoxel(*ram, [&](const size3_t& pos) {
        vec3 cartesian = idTOCartesian(pos);
        data[index(pos)] = static_cast<float>(eval(cartesian));
    });

    auto minMax = util::volumeMinMax(ram);
    vol->dataMap_.dataRange = vol->dataMap_.valueRange = dvec2(minMax.first.x, minMax.second.x);

    volume_.setData(vol);
}

vec3 HydrogenGenerator::cartesianToSphereical(vec3 cartesian) {
    double r = sqrt(pow((cartesian.x), 2) + pow(cartesian.y,2) + pow(cartesian.z, 2));
    double theta;
    if (r == 0) {
        theta = 0;
    } else {
        theta = acos(cartesian.z / r);
    }
    double phi;
    if (cartesian.x == 0) {
        if (cartesian.y >= 0) {
            if (cartesian.y == 0) {
                phi = 0;
            } else {
              phi = 3.141593 / 2;
            }
        } else {
            phi = -3.141593 / 2;
        }
    } else {
        if (cartesian.x <= 0) {
            phi = -3.141593 + atan(cartesian.y / cartesian.x);
            if (phi < -3.141593) {
                phi = 2 * 3.141593 + phi;
            }
        } else {
            phi = atan(cartesian.y / cartesian.x);
        }
    }
    vec3 sph{r,theta,phi};  // TODO implement the conversion
    return sph;
}

double HydrogenGenerator::eval(vec3 cartesian) {
    vec3 sph = HydrogenGenerator::cartesianToSphereical(cartesian);

    double r = sph.x;
    double theta = sph.y;
    double phi = sph.z;
    double yellow = 1 / (81 * sqrt(6 * 3.141593));
    double red = 1 / 1;
    double blue = (1 * pow(r, 2)) / 1;
    double green = exp(-r / 3);
    double purple = (3 * pow(cos(theta), 2)) - 1;
    double funcval = yellow*red*blue*green*purple;  // TODO implement this
    double density = pow(abs(funcval), 2);
    return density;
}

vec3 HydrogenGenerator::idTOCartesian(size3_t pos) {
    vec3 p(pos);
    p /= size_ - 1;
    return p * (36.0f) - 18.0f;
}

}  // namespace inviwo
