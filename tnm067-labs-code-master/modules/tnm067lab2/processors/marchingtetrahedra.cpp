#include <modules/tnm067lab2/processors/marchingtetrahedra.h>
#include <inviwo/core/datastructures/geometry/basicmesh.h>
#include <inviwo/core/datastructures/volume/volumeram.h>
#include <inviwo/core/util/indexmapper.h>
#include <inviwo/core/util/assertion.h>
#include <inviwo/core/network/networklock.h>

namespace inviwo {

size_t MarchingTetrahedra::HashFunc::max = 1;

const ProcessorInfo MarchingTetrahedra::processorInfo_{
    "org.inviwo.MarchingTetrahedra",  // Class identifier
    "Marching Tetrahedra",            // Display name
    "TNM067",                         // Category
    CodeState::Stable,                // Code state
    Tags::None,                       // Tags
};
const ProcessorInfo MarchingTetrahedra::getProcessorInfo() const { return processorInfo_; }

MarchingTetrahedra::MarchingTetrahedra()
    : Processor()
    , volume_("volume")
    , mesh_("mesh")
    , isoValue_("isoValue", "ISO value", 0.5f, 0.0f, 1.0f) {

    addPort(volume_);
    addPort(mesh_);

    addProperty(isoValue_);

    isoValue_.setSerializationMode(PropertySerializationMode::All);

    volume_.onChange([&]() {
        if (!volume_.hasData()) {
            return;
        }
        NetworkLock lock(getNetwork());
        float iso = (isoValue_.get() - isoValue_.getMinValue()) /
                    (isoValue_.getMaxValue() - isoValue_.getMinValue());
        const auto vr = volume_.getData()->dataMap_.valueRange;
        isoValue_.setMinValue(static_cast<float>(vr.x));
        isoValue_.setMaxValue(static_cast<float>(vr.y));
        isoValue_.setIncrement(static_cast<float>(glm::abs(vr.y - vr.x) / 50.0));
        isoValue_.set(static_cast<float>(iso * (vr.y - vr.x) + vr.x));
        isoValue_.setCurrentStateAsDefault();
    });
}

void MarchingTetrahedra::process() {
    auto volume = volume_.getData()->getRepresentation<VolumeRAM>();
    MeshHelper mesh(volume_.getData());

    const auto& dims = volume->getDimensions();
    MarchingTetrahedra::HashFunc::max = dims.x * dims.y * dims.z;

    const float iso = isoValue_.get();

    util::IndexMapper3D index(dims);

    const static size_t tetrahedraIds[6][4] = {{0, 1, 2, 5}, {1, 3, 2, 5}, {3, 2, 5, 7},
                                               {0, 2, 4, 5}, {6, 4, 2, 5}, {6, 7, 5, 2}};

    size3_t pos{};
    for (pos.z = 0; pos.z < dims.z - 1; ++pos.z) {
        for (pos.y = 0; pos.y < dims.y - 1; ++pos.y) {
            for (pos.x = 0; pos.x < dims.x - 1; ++pos.x) {
                // Step 1: create current cell
                // Use volume->getAsDouble to query values from the volume
                // Spatial position should be between 0 and 1
                // The voxel index should be the 1D-index for the voxel
                double dimx = dims.x - 1;
                double dimy = dims.y - 1;
                double dimz = dims.z - 1;

                Voxel v0, v1, v2, v3, v4, v5, v6, v7;
                vec3 spatpos0, spatpos1, spatpos2, spatpos3, spatpos4, spatpos5, spatpos6, spatpos7;

                spatpos0 = vec3{pos.x / dimx, pos.y / dimy, pos.z / dimz};
                spatpos1 = vec3{(pos.x + 1) / dimx, pos.y / dimy, pos.z / dimz};
                spatpos2 = vec3{pos.x / dimx, (pos.y + 1) / dimy, pos.z / dimz};
                spatpos3 = vec3{(pos.x + 1) / dimy, (pos.y + 1) / dimy, pos.z / dimz};
                spatpos4 = vec3{pos.x / dimx, pos.y / dimy, (pos.z + 1) / dimz};
                spatpos5 = vec3{(pos.x + 1) / dimx, (pos.y) / dimy, (pos.z + 1) / dimz};
                spatpos6 = vec3{pos.x / dimx, (pos.y + 1) / dimy, (pos.z + 1) / dimz};
                spatpos7 = vec3{(pos.x + 1) / dimx, (pos.y + 1) / dimy, (pos.z + 1) / dimz};

                v0.pos = vec3{pos.x, pos.y, pos.z};
                v1.pos = vec3{(pos.x + 1), pos.y, pos.z};
                v2.pos = vec3{pos.x, (pos.y+1), pos.z};
                v3.pos = vec3{(pos.x+1), (pos.y + 1), pos.z};
                v4.pos = vec3{pos.x, pos.y, (pos.z+1)};
                v5.pos = vec3{(pos.x+1), (pos.y), (pos.z+1)};
                v6.pos = vec3{pos.x, (pos.y + 1), (pos.z+1)};
                v7.pos = vec3{(pos.x+1), (pos.y + 1), (pos.z+1)};

                v0.value = volume->getAsDouble(v0.pos);
                v1.value = volume->getAsDouble(v1.pos);
                v2.value = volume->getAsDouble(v2.pos);
                v3.value = volume->getAsDouble(v3.pos);
                v4.value = volume->getAsDouble(v4.pos);
                v5.value = volume->getAsDouble(v5.pos);
                v6.value = volume->getAsDouble(v6.pos);
                v7.value = volume->getAsDouble(v7.pos);        

                v0.index = index(v0.pos);
                v1.index = index(v1.pos);
                v2.index = index(v2.pos);
                v3.index = index(v3.pos);
                v4.index = index(v4.pos);
                v5.index = index(v5.pos);
                v6.index = index(v6.pos);
                v7.index = index(v7.pos);

                v0.pos = spatpos0;
                v1.pos = spatpos1;
                v2.pos = spatpos2;
                v3.pos = spatpos3;
                v4.pos = spatpos4;
                v5.pos = spatpos5;
                v6.pos = spatpos6;
                v7.pos = spatpos7;
                
                Cell c;
                c.voxels[0] = v0;
                c.voxels[1] = v1;
                c.voxels[2] = v2;
                c.voxels[3] = v3;
                c.voxels[4] = v4;
                c.voxels[5] = v5;
                c.voxels[6] = v6;
                c.voxels[7] = v7;

                // Step 2: Subdivide cell into tetrahedra (hint: use tetrahedraIds)
                Tetrahedra t0 = Tetrahedra{c.voxels[0], c.voxels[1], c.voxels[2], c.voxels[5]};
                Tetrahedra t1 = Tetrahedra{c.voxels[1], c.voxels[3], c.voxels[2], c.voxels[5]};
                Tetrahedra t2 = Tetrahedra{c.voxels[3], c.voxels[2], c.voxels[5], c.voxels[7]};
                Tetrahedra t3 = Tetrahedra{c.voxels[0], c.voxels[2], c.voxels[4], c.voxels[5]};
                Tetrahedra t4 = Tetrahedra{c.voxels[6], c.voxels[4], c.voxels[2], c.voxels[5]};
                Tetrahedra t5 = Tetrahedra{c.voxels[6], c.voxels[7], c.voxels[5], c.voxels[2]};

                std::vector<Tetrahedra> tetrahedras;
                Tetrahedra tetra;
                for (int i = 0; i < 6; i++) {
                    for (int j = 0; j < 4; j++) {
                        tetra.voxels[j] = c.voxels[tetrahedraIds[i][j]];
                    }
                    tetrahedras.push_back(tetra);
                }
                for (const Tetrahedra& tetrahedra : tetrahedras) {
                    // Step three: Calculate for tetra case index
                    
                    int caseId = 0;
                    for (int i = 0; i < 4; i++) {
                        if (tetrahedra.voxels[i].value < iso) {
                            caseId += 1 * pow(2, 3 - i);
                        }
                    }


                    vec3 interpValPos0, interpValPos1, interpValPos2, interpValPos3;

                    vec3 pos0 = tetrahedra.voxels[0].pos;
                    vec3 pos1 = tetrahedra.voxels[1].pos;
                    vec3 pos2 = tetrahedra.voxels[2].pos;
                    vec3 pos3 = tetrahedra.voxels[3].pos;
                    double val0 = tetrahedra.voxels[0].value;
                    double val1 = tetrahedra.voxels[1].value;
                    double val2 = tetrahedra.voxels[2].value;
                    double val3 = tetrahedra.voxels[3].value;
                    size_t idx0 = tetrahedra.voxels[0].index;
                    size_t idx1 = tetrahedra.voxels[1].index;
                    size_t idx2 = tetrahedra.voxels[2].index;
                    size_t idx3 = tetrahedra.voxels[3].index;

                    // step four: Extract triangles
                    if (caseId == 0 || caseId == 15) {
                        //no triangles added
                    }

                    if (caseId == 1 || caseId == 14) {
                        interpValPos0 = pos0 + (pos1 - pos0) * (iso - val0) / (val1 - val0);
                        interpValPos1 = pos0 + (pos3 - pos0) * (iso - val0) / (val3 - val0);
                        interpValPos2 = pos0 + (pos2 - pos0) * (iso - val0) / (val2 - val0);

                        std::uint32_t id0 = mesh.addVertex(interpValPos0, idx0, idx1);
                        std::uint32_t id1 = mesh.addVertex(interpValPos1, idx0, idx3);
                        std::uint32_t id2 = mesh.addVertex(interpValPos2, idx0, idx2);
                        
                        if (caseId == 1) {
                            mesh.addTriangle(id0, id1, id2);
                        } else {
                            mesh.addTriangle(id0, id2, id1);
                        }
                    }
                    
                    if (caseId == 2 || caseId == 13) {
                        interpValPos0 = pos1 + (pos3 - pos1) * (iso - val1) / (val3 - val1);
                        interpValPos1 = pos1 + (pos2 - pos1) * (iso - val1) / (val2 - val1);
                        interpValPos2 = pos1 + (pos0 - pos1) * (iso - val1) / (val0 - val1);

                        std::uint32_t id0 = mesh.addVertex(interpValPos0, idx1, idx3);
                        std::uint32_t id1 = mesh.addVertex(interpValPos1, idx1, idx2);
                        std::uint32_t id2 = mesh.addVertex(interpValPos2, idx1, idx0);

                        if (caseId == 2) {
                            mesh.addTriangle(id0, id1, id2);
                        } else {
                            mesh.addTriangle(id0, id2, id1);
                        }
                    }
                    
                    
                    if (caseId == 3 || caseId == 12) {
                        interpValPos0 = pos1 + (pos2 - pos1) * (iso - val1) / (val2 - val1);
                        interpValPos1 = pos1 + (pos3 - pos1) * (iso - val1) / (val3 - val1);
                        interpValPos2 = pos0 + (pos3 - pos0) * (iso - val0) / (val3 - val0);
                        interpValPos3 = pos0 + (pos2 - pos0) * (iso - val0) / (val2 - val0);

                        std::uint32_t id0 = mesh.addVertex(interpValPos0, idx1, idx2);
                        std::uint32_t id1 = mesh.addVertex(interpValPos1, idx1, idx3);
                        std::uint32_t id2 = mesh.addVertex(interpValPos2, idx0, idx3);
                        std::uint32_t id3 = mesh.addVertex(interpValPos3, idx0, idx2);

                        if (caseId == 3) {
                            mesh.addTriangle(id0, id1, id2);
                            mesh.addTriangle(id0, id2, id3);
                        } else {
                            mesh.addTriangle(id2, id1, id0);
                            mesh.addTriangle(id3, id2, id0);
                        }

                    }
                    

                    if (caseId == 4 || caseId == 11) {
                        interpValPos0 = pos2 + (pos0 - pos2) * (iso - val2) / (val0 - val2);
                        interpValPos1 = pos2 + (pos1 - pos2) * (iso - val2) / (val1 - val2);
                        interpValPos2 = pos2 + (pos3 - pos2) * (iso - val2) / (val3 - val2);

                        std::uint32_t id0 = mesh.addVertex(interpValPos0, idx2, idx0);
                        std::uint32_t id1 = mesh.addVertex(interpValPos1, idx2, idx1);
                        std::uint32_t id2 = mesh.addVertex(interpValPos2, idx2, idx3);

                        if (caseId == 4) {
                            mesh.addTriangle(id0, id1, id2);
                        } else {
                            mesh.addTriangle(id0, id2, id1);
                        }
                    }
                    

                    if (caseId == 5 || caseId == 10) {
                        
                        interpValPos0 = pos0 + (pos3 - pos0) * (iso - val0) / (val3 - val0);
                        interpValPos1 = pos0 + (pos1 - pos0) * (iso - val0) / (val1 - val0);
                        interpValPos2 = pos2 + (pos1 - pos2) * (iso - val2) / (val1 - val2);
                        interpValPos3 = pos2 + (pos3 - pos2) * (iso - val2) / (val3 - val2);

                        std::uint32_t id0 = mesh.addVertex(interpValPos0, idx0, idx3);
                        std::uint32_t id1 = mesh.addVertex(interpValPos1, idx0, idx1);
                        std::uint32_t id2 = mesh.addVertex(interpValPos2, idx2, idx1);
                        std::uint32_t id3 = mesh.addVertex(interpValPos3, idx2, idx3);

                        if (caseId == 5) {
                            mesh.addTriangle(id0, id1, id2);
                            mesh.addTriangle(id0, id2, id3);
                        } else {
                            mesh.addTriangle(id2, id1, id0);
                            mesh.addTriangle(id3, id2, id0);
                        }
                        
                    }

                    if (caseId == 6 || caseId == 9) {
                        interpValPos0 = pos0 + (pos2 - pos0) * (iso - val0) / (val2 - val0);
                        interpValPos1 = pos0 + (pos1 - pos0) * (iso - val0) / (val1 - val0);
                        interpValPos2 = pos1 + (pos3 - pos1) * (iso - val1) / (val3 - val1);
                        interpValPos3 = pos2 + (pos3 - pos2) * (iso - val2) / (val3 - val2);

                        std::uint32_t id0 = mesh.addVertex(interpValPos0, idx0, idx2);
                        std::uint32_t id1 = mesh.addVertex(interpValPos1, idx0, idx1);
                        std::uint32_t id2 = mesh.addVertex(interpValPos2, idx2, idx3);
                        std::uint32_t id3 = mesh.addVertex(interpValPos3, idx2, idx3);

                        if (caseId == 9) {
                            mesh.addTriangle(id0, id1, id2);
                            mesh.addTriangle(id0, id2, id3);
                        } else {
                            mesh.addTriangle(id0, id2, id1);
                            mesh.addTriangle(id0, id3, id2);
                        }
                        
                    }

                    if (caseId == 7 || caseId == 8) {
                        
                        interpValPos0 = pos3 + (pos0 - pos3) * (iso - val3) / (val0 - val3);
                        interpValPos1 = pos3 + (pos1 - pos3) * (iso - val3) / (val1 - val3);
                        interpValPos2 = pos3 + (pos2 - pos3) * (iso - val3) / (val2 - val3);

                        std::uint32_t id0 = mesh.addVertex(interpValPos0, idx3, idx0);
                        std::uint32_t id1 = mesh.addVertex(interpValPos1, idx3, idx1);
                        std::uint32_t id2 = mesh.addVertex(interpValPos2, idx3, idx2);

                        if (caseId == 8) {
                            mesh.addTriangle(id0, id1, id2);
                        } else {
                            mesh.addTriangle(id0, id2, id1);
                        }
                        
                    }
                    
                }
            }
        }
    }

    mesh_.setData(mesh.toBasicMesh());
}

MarchingTetrahedra::MeshHelper::MeshHelper(std::shared_ptr<const Volume> vol)
    : edgeToVertex_()
    , vertices_()
    , mesh_(std::make_shared<BasicMesh>())
    , indexBuffer_(mesh_->addIndexBuffer(DrawType::Triangles, ConnectivityType::None)) {
    mesh_->setModelMatrix(vol->getModelMatrix());
    mesh_->setWorldMatrix(vol->getWorldMatrix());
}

void MarchingTetrahedra::MeshHelper::addTriangle(size_t i0, size_t i1, size_t i2) {
    IVW_ASSERT(i0 != i1, "i0 and i1 should not be the same value");
    IVW_ASSERT(i0 != i2, "i0 and i2 should not be the same value");
    IVW_ASSERT(i1 != i2, "i1 and i2 should not be the same value");

    indexBuffer_->add(static_cast<glm::uint32_t>(i0));
    indexBuffer_->add(static_cast<glm::uint32_t>(i1));
    indexBuffer_->add(static_cast<glm::uint32_t>(i2));

    const auto a = std::get<0>(vertices_[i0]);
    const auto b = std::get<0>(vertices_[i1]);
    const auto c = std::get<0>(vertices_[i2]);

    const vec3 n = glm::normalize(glm::cross(b - a, c - a));
    std::get<1>(vertices_[i0]) += n;
    std::get<1>(vertices_[i1]) += n;
    std::get<1>(vertices_[i2]) += n;
}

std::shared_ptr<BasicMesh> MarchingTetrahedra::MeshHelper::toBasicMesh() {
    for (auto& vertex : vertices_) {
        std::get<0>(vertex) = glm::normalize(std::get<0>(vertex));
    }
    mesh_->addVertices(vertices_);
    return mesh_;
}

std::uint32_t MarchingTetrahedra::MeshHelper::addVertex(vec3 pos, size_t i, size_t j) {
    IVW_ASSERT(i != j, "i and j should not be the same value");
    if (j < i) std::swap(i, j);

    auto [edgeIt, inserted] = edgeToVertex_.try_emplace(std::make_pair(i, j), vertices_.size());
    if (inserted) {
        vertices_.push_back({pos, vec3(0, 0, 0), pos, vec4(0.7f, 0.7f, 0.7f, 1.0f)});
    }
    return static_cast<std::uint32_t>(edgeIt->second);
}

}  // namespace inviwo
