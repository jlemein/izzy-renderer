//
// Created by jlemein on 08-08-21.
//
#include <gtest/gtest.h>
#include <geo_meshutil.h>
#include <geo_mesh.h>
#include <vector>
#include <geo_primitivefactory.h>

using namespace lsw::geo;

class MeshUtilTest : public testing::Test {
public:
};

TEST_F(MeshUtilTest, ShouldFailIfResourceNotExist) {
    Mesh mesh;

    mesh.vertices = {0.0F, 0.0F, 0.0F, 1.0F, 0.0F, 0.0F, 0.0F, 1.0F, 0.0F,    // triangle 1
                     1.0F, 0.0F, -1.0F, 1.0F, 1.0F, -1.0F, 1.0F, 1.0F, 0.0F}; // triangle 2
    mesh.uvs = {.0F, .0F, 1.F, 0.F, .0F, 1.F,     // triangle 1
                .0F, .0F, 1.F, .0F, .0F, 1.F};    // triangle 2
    mesh.indices = {0, 1, 2, 3, 4, 5};

    MeshUtil::GenerateTangents(mesh);
    EXPECT_EQ(mesh.tangents.size(), mesh.vertices.size());

    // first triangle
    glm::vec3 T1(1.0F, 0.0F, 0.0F);
    EXPECT_EQ(mesh.tangents[0], T1.x);
    EXPECT_EQ(mesh.tangents[1], T1.y);
    EXPECT_EQ(mesh.tangents[2], T1.z);

    // second triangle
    glm::vec3 T2(0.0F, 0.0F, -1.0F);
    EXPECT_EQ(mesh.tangents[3], T1.x);
    EXPECT_EQ(mesh.tangents[4], T1.y);
    EXPECT_EQ(mesh.tangents[5], T1.z);
}

TEST_F(MeshUtilTest, ShouldFillVertices) {
    auto box = lsw::geo::PrimitiveFactory::MakeBox();
    for (int i = 0; i<box.vertices.size(); i++) {
        if (i%3==0) std::cout << i/3 << ": ";
        std::cout << box.vertices.at(i) << " ";
        if (i%3 == 2) {
            std::cout << std::endl;
        }
    }
}

TEST_F(MeshUtilTest, ShouldFillVerticesFromIndex) {
    auto box = lsw::geo::PrimitiveFactory::MakeBox();
    for (int i = 0; i<box.indices.size(); i++) {
        auto vi = box.indices[i]*3;
        if (i%3==0) {
            std::cout << "triangle " << i/3 << ": \t";

        }

        for (int n=0; n<3; ++n) {
            std::cout << box.vertices.at(vi+n) << " ";
        }
        std::cout << " (T: ";
        for (int n=0; n<3; ++n) {
            std::cout << box.tangents.at(vi+n) << " ";
        }
        std::cout << ") \t";

        std::cout << " (B: ";
        for (int n=0; n<3; ++n) {
            std::cout << box.bitangents.at(vi+n) << " ";
        }
        std::cout << ") \t";

        if (i%3 == 2) {
            std::cout << std::endl;
        }
    }
}

TEST_F(MeshUtilTest, ShouldCreateBoxTangents) {
    auto box = lsw::geo::PrimitiveFactory::MakeBox();
    for (int t = 0; t<box.indices.size()/3; t+=3) {
        auto index = box.indices[t];

        auto uvIndex = index*2;
        std::cout << "UV1: [{" << box.uvs[uvIndex] << " " << box.uvs[uvIndex + 1] << "}, {" << box.uvs[uvIndex+2] << " " << box.uvs[uvIndex + 3] << "}, {" << box.uvs[uvIndex+4] << " " << box.uvs[uvIndex + 5] << "}]" << std::endl;

        for (int i=0; i<3; ++i) {
            auto Vx = box.vertices.at(index*3 + i*3);
            auto Vy = box.vertices.at(index*3 + i*3 + 1);
            auto Vz = box.vertices.at(index*3 + i*3 + 2);
            std::cout << "Vertex " << i << ": " <<  Vx << ", " << Vy << ", " << Vz << std::endl;
        }

        auto Tx = box.tangents[index*3];
        auto Ty = box.tangents[index*3+1];
        auto Tz = box.tangents[index*3+2];
        auto Bx = box.tangents[index*3];
        auto By = box.tangents[index*3+1];
        auto Bz = box.tangents[index*3+2];
        std::cout << "Tangent for t: " << t << ": " << Tx << ", " << Ty << ", " << Tz << std::endl;
//        std::cout << "Bitangent for t: " << t << ": " << Bx << ", " << By << ", " << Bz << std::endl;
    }
    for (int i = 0; i < box.uvs.size(); i += 6) {
        std::cout << "UV " << i / 6 << ": " << box.uvs[i] << " " << box.uvs[i + 1] << " " << std::endl;
    }
    for (int i = 0; i < box.tangents.size(); i += 9) {
        std::cout << "Vertex " << i / 9 << ": " << box.vertices[i] << " " << box.vertices[i + 1] << " "
                  << box.vertices[i + 2] << std::endl;

        std::cout << "Tangent " << i / 9 << ": " << box.tangents[i] << " " << box.tangents[i + 1] << " "
                  << box.tangents[i + 2] << std::endl;
    }
}
