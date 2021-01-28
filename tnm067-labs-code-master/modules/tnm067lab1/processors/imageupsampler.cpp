#include <inviwo/core/util/logcentral.h>
#include <modules/opengl/texture/textureutils.h>
#include <modules/tnm067lab1/processors/imageupsampler.h>
#include <modules/tnm067lab1/utils/interpolationmethods.h>
#include <inviwo/core/datastructures/image/layerram.h>
#include <inviwo/core/datastructures/image/layerramprecision.h>
#include <inviwo/core/util/imageramutils.h>
#include <iostream>
#include <cmath>
#pragma optimize("", off)
namespace inviwo {

namespace detail {

template <typename T>
void upsample(ImageUpsampler::IntepolationMethod method, const LayerRAMPrecision<T>& inputImage,
              LayerRAMPrecision<T>& outputImage) {
    using F = typename float_type<T>::type;

    const size2_t inputSize = inputImage.getDimensions();
    const size2_t outputSize = outputImage.getDimensions();

    const T* inPixels = inputImage.getDataTyped();
    T* outPixels = outputImage.getDataTyped();

    auto inIndex = [&inputSize](auto pos) -> size_t {
        pos = glm::clamp(pos, decltype(pos)(0), decltype(pos)(inputSize - size2_t(1)));
        return pos.x + pos.y * inputSize.x;
    };
    auto outIndex = [&outputSize](auto pos) -> size_t {
        pos = glm::clamp(pos, decltype(pos)(0), decltype(pos)(outputSize - size2_t(1)));
        return pos.x + pos.y * outputSize.x;
    };

    util::forEachPixel(outputImage, [&](ivec2 outImageCoords) {
        // outImageCoords: Exact pixel coordinates in the output image currently writing to
        // inImageCoords: Relative coordinates of outImageCoords in the input image, might be
        // between pixels
        dvec2 inImageCoords =
            ImageUpsampler::convertCoordinate(outImageCoords, inputSize, outputSize);

        T finalColor(0);

        // DUMMY COLOR, remove or overwrite this bellow
        finalColor = inPixels[inIndex(
            glm::clamp(size2_t(outImageCoords), size2_t(0), size2_t(inputSize - size2_t(1))))];

        switch (method) {
            case ImageUpsampler::IntepolationMethod::PiecewiseConstant: {
                double posx, posy;
                if ((inImageCoords.x - floor(inImageCoords.x)) >= 0.5) {
                    posx = ceil(inImageCoords.x);
                } 
                else {
                    posx = floor(inImageCoords.x);
                }
                if ((inImageCoords.y - floor(inImageCoords.y)) >= 0.5) {
                    posy = ceil(inImageCoords.y);
                } else {
                    posy = floor(inImageCoords.y);
                }

                dvec2 pos(posx, posy);

                finalColor = inPixels[inIndex(pos)];
                break;
            }
            case ImageUpsampler::IntepolationMethod::Bilinear: {
             //   TNM067::Interpolation::bilinear()
                T color0(0);
                T color1(0);
                T color2(0);
                T color3(0);

                dvec2 pos0 = dvec2(floor(inImageCoords.x), floor(inImageCoords.y));
                dvec2 pos1 = dvec2(ceil(inImageCoords.x), floor(inImageCoords.y));
                dvec2 pos2 = dvec2(floor(inImageCoords.x), ceil(inImageCoords.y));
                dvec2 pos3 = dvec2(ceil(inImageCoords.x), ceil(inImageCoords.y));

                double x = inImageCoords.x - floor(inImageCoords.x);
                double y = inImageCoords.y - floor(inImageCoords.y);

                color0 = inPixels[inIndex(pos0)];
                color1 = inPixels[inIndex(pos1)];
                color2 = inPixels[inIndex(pos2)];
                color3 = inPixels[inIndex(pos3)];

                std::array<T, 4> arr{color0, color1, color2, color3};
                finalColor = TNM067::Interpolation::bilinear(arr, x, y);
                break;
            }
            case ImageUpsampler::IntepolationMethod::Quadratic: {
                T color0(0);
                T color1(0);
                T color2(0);
                T color3(0);
                T color4(0);
                T color5(0);
                T color6(0);
                T color7(0);
                T color8(0);


                dvec2 pos0 = dvec2(floor(inImageCoords.x), floor(inImageCoords.y));
                dvec2 pos1 = dvec2(ceil(inImageCoords.x), floor(inImageCoords.y));
                dvec2 pos2 = dvec2(ceil(inImageCoords.x)+1, floor(inImageCoords.y));
                dvec2 pos3 = dvec2(floor(inImageCoords.x), ceil(inImageCoords.y));
                dvec2 pos4 = dvec2(ceil(inImageCoords.x), ceil(inImageCoords.y));
                dvec2 pos5 = dvec2(ceil(inImageCoords.x)+1, ceil(inImageCoords.y));
                dvec2 pos6 = dvec2(floor(inImageCoords.x), ceil(inImageCoords.y)+1);
                dvec2 pos7 = dvec2(ceil(inImageCoords.x), ceil(inImageCoords.y)+1);
                dvec2 pos8 = dvec2(ceil(inImageCoords.x)+1, ceil(inImageCoords.y)+1);

                double x = (inImageCoords.x - floor(inImageCoords.x)) /
                           (ceil(inImageCoords.x) + 1 - floor(inImageCoords.x));
                double y = (inImageCoords.y - floor(inImageCoords.y)) /
                           (ceil(inImageCoords.y) + 1 - floor(inImageCoords.y));

                color0 = inPixels[inIndex(pos0)];
                color1 = inPixels[inIndex(pos1)];
                color2 = inPixels[inIndex(pos2)];
                color3 = inPixels[inIndex(pos3)];
                color4 = inPixels[inIndex(pos4)];
                color5 = inPixels[inIndex(pos5)];
                color6 = inPixels[inIndex(pos6)];
                color7 = inPixels[inIndex(pos7)];
                color8 = inPixels[inIndex(pos8)];

                std::array<T, 9> arr{color0, color1, color2, color3, color4, color5, color6, color7, color8};

                finalColor = TNM067::Interpolation::biQuadratic(arr, x, y);
                break;
            }
            case ImageUpsampler::IntepolationMethod::Barycentric: {
                T color0(0);
                T color1(0);
                T color2(0);
                T color3(0);

                dvec2 pos0 = dvec2(floor(inImageCoords.x), floor(inImageCoords.y));
                dvec2 pos1 = dvec2(ceil(inImageCoords.x), floor(inImageCoords.y));
                dvec2 pos2 = dvec2(floor(inImageCoords.x), ceil(inImageCoords.y));
                dvec2 pos3 = dvec2(ceil(inImageCoords.x), ceil(inImageCoords.y));

                color0 = inPixels[inIndex(pos0)];
                color1 = inPixels[inIndex(pos1)];
                color2 = inPixels[inIndex(pos2)];
                color3 = inPixels[inIndex(pos3)];
                
                double x = inImageCoords.x - floor(inImageCoords.x);
                double y = inImageCoords.y - floor(inImageCoords.y);

                std::array<T, 4> arr{color0, color1, color2, color3};

                finalColor = TNM067::Interpolation::barycentric(arr, x, y);

                break;
            }
            default:
                break;
        }

        outPixels[outIndex(outImageCoords)] = finalColor;
    });

}

}  // namespace detail

#pragma optimize("", on)

const ProcessorInfo ImageUpsampler::processorInfo_{
    "org.inviwo.imageupsampler",  // Class identifier
    "Image Upsampler",            // Display name
    "TNM067",                     // Category
    CodeState::Experimental,      // Code state
    Tags::None,                   // Tags
};
const ProcessorInfo ImageUpsampler::getProcessorInfo() const { return processorInfo_; }

ImageUpsampler::ImageUpsampler()
    : Processor()
    , inport_("inport", true)
    , outport_("outport", true)
    , interpolationMethod_("interpolationMethod", "Interpolation Method",
                           {
                               {"piecewiseconstant", "Piecewise Constant (Nearest Neighbor)",
                                IntepolationMethod::PiecewiseConstant},
                               {"bilinear", "Bilinear", IntepolationMethod::Bilinear},
                               {"quadratic", "Quadratic", IntepolationMethod::Quadratic},
                               {"barycentric", "Barycentric", IntepolationMethod::Barycentric},
                           }) {
    addPort(inport_);
    addPort(outport_);
    addProperty(interpolationMethod_);
}

void ImageUpsampler::process() {
    auto inputImage = inport_.getData();
    if (inputImage->getDataFormat()->getComponents() != 1) {
        LogError("The ImageUpsampler processor does only support single channel images");
    }

    auto inSize = inport_.getData()->getDimensions();
    auto outDim = outport_.getDimensions();

    auto outputImage = std::make_shared<Image>(outDim, inputImage->getDataFormat());
    outputImage->getColorLayer()->setSwizzleMask(inputImage->getColorLayer()->getSwizzleMask());
    outputImage->getColorLayer()
        ->getEditableRepresentation<LayerRAM>()
        ->dispatch<void, dispatching::filter::Scalars>([&](auto outRep) {
            auto inRep = inputImage->getColorLayer()->getRepresentation<LayerRAM>();
            detail::upsample(interpolationMethod_.get(), *(const decltype(outRep))(inRep), *outRep);
        });

    outport_.setData(outputImage);
}

dvec2 ImageUpsampler::convertCoordinate(ivec2 outImageCoords, [[maybe_unused]] size2_t inputSize,
                                        [[maybe_unused]] size2_t outputsize) {
    // TODO implement
    dvec2 c(outImageCoords);

    double x_ratio = double(inputSize[0]) / double(outputsize[0]);
    double y_ratio = double(inputSize[1]) / double(outputsize[1]);

    c[0] = x_ratio * outImageCoords[0];
    c[1] = y_ratio * outImageCoords[1];

    // TASK 5: Convert the outImageCoords to its coordinates in the input image

    return c;
}

}  // namespace inviwo
