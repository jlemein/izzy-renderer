//
// Created by jlemein on 18-01-21.
//
#include <geo_mesh.h>
#include <geo_sceneloader.h>
#include <geo_camera.h>
#include <geo_light.h>
#include <geo_meshinstance.h>
#include <geo_transform.h>
#include <geo_scene.h>

#include <deque>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include <res_resourcemanager.h>
#include <spdlog/spdlog.h>
#include <filesystem>

using namespace lsw;
using namespace lsw::geo;

SceneLoader::SceneLoader(std::shared_ptr<res::ResourceManager> resourceManager)
        : m_resourceManager{resourceManager} {}

std::shared_ptr<res::Resource<geo::Texture>> SceneLoader::readDiffuseTexture(const geo::Scene &scene,
                                                                             const aiMaterial *aiMaterial_p,
                                                                             const Material &material) const {
    std::string diffusePath = material.diffuseTexturePath;

    if (diffusePath.empty() && aiMaterial_p->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
        aiString aiDiffusePath;
        aiTextureMapping textureMapping;
        aiMaterial_p->GetTexture(aiTextureType_DIFFUSE, 0, &aiDiffusePath, &textureMapping);
        diffusePath = aiDiffusePath.C_Str();
    }

    std::shared_ptr<res::Resource<geo::Texture>> texture{nullptr};
    if (!diffusePath.empty()) {
        // diffuse path is relative to scene file
        auto resolvedPath = scene.m_dir / diffusePath;
        texture = m_resourceManager->createResource<geo::Texture>(resolvedPath);
        spdlog::info("Material {}: loaded diffuse texture {}", material.name, resolvedPath.string());
    }

    return texture;
}

std::shared_ptr<res::Resource<geo::Texture>> SceneLoader::readSpecularTexture(const geo::Scene &scene,
                                                                              const aiMaterial *aiMaterial_p,
                                                                              const geo::Material &material) const {
    std::string specularPath = material.specularTexturePath;

    if (specularPath.empty() && aiMaterial_p->GetTextureCount(aiTextureType_SPECULAR) > 0) {
        aiString aiSpecularPath;
        aiTextureMapping textureMapping;
        aiMaterial_p->GetTexture(aiTextureType_SPECULAR, 0, &aiSpecularPath, &textureMapping);
        specularPath = aiSpecularPath.C_Str();
    }

    std::shared_ptr<res::Resource<geo::Texture>> texture{nullptr};
    if (!specularPath.empty()) {
        auto resolvedPath = scene.m_dir / specularPath;
        texture = m_resourceManager->createResource<geo::Texture>(resolvedPath);
        spdlog::info("Material {}: loaded specular texture {}", material.name, resolvedPath.string());
    }

    return texture;
}

std::shared_ptr<res::Resource<geo::Texture>> SceneLoader::readNormalTexture(const geo::Scene &scene,
                                                                            const aiMaterial *aiMaterial_p,
                                                                            const geo::Material &material) const {
    std::string normalPath = material.normalTexturePath;

    if (normalPath.empty() && aiMaterial_p->GetTextureCount(aiTextureType_NORMALS) > 0) {
        aiString aiNormalPath;
        aiTextureMapping textureMapping;
        aiMaterial_p->GetTexture(aiTextureType_NORMALS, 0, &aiNormalPath, &textureMapping);
        normalPath = aiNormalPath.C_Str();
    }

    std::shared_ptr<res::Resource<geo::Texture>> texture{nullptr};
    if (!normalPath.empty()) {
        auto resolvedPath = scene.m_dir / normalPath;
        texture = m_resourceManager->createResource<geo::Texture>(resolvedPath);
        spdlog::info("Material {}: loaded specular texture {}", material.name, resolvedPath.string());
    }

    return texture;
}

std::shared_ptr<res::Resource<geo::Texture>> SceneLoader::readRoughnessTexture(const geo::Scene &scene,
                                                                               const aiMaterial *aiMaterial_p,
                                                                               const geo::Material &material) const {
    std::string roughnessPath = material.roughnessTexturePath;

    if (roughnessPath.empty() && aiMaterial_p->GetTextureCount(aiTextureType_DIFFUSE_ROUGHNESS) > 0) {
        aiString aiNormalPath;
        aiTextureMapping textureMapping;
        aiMaterial_p->GetTexture(aiTextureType_DIFFUSE_ROUGHNESS, 0, &aiNormalPath, &textureMapping);
        roughnessPath = aiNormalPath.C_Str();
    }

    std::shared_ptr<res::Resource<geo::Texture>> texture{nullptr};
    if (!roughnessPath.empty()) {
        auto resolvedPath = scene.m_dir / roughnessPath;
        texture = m_resourceManager->createResource<geo::Texture>(roughnessPath);
        spdlog::info("Material {}: loaded roughness texture {}", material.name, resolvedPath.string());
    }

    return texture;
}

void SceneLoader::readTextures(const geo::Scene &scene, const aiMaterial *aiMaterial_p, Material &material) {
    material.diffuseTexture = readDiffuseTexture(scene, aiMaterial_p, material);
    material.normalTexture = readNormalTexture(scene, aiMaterial_p, material);
    material.specularTexture = readSpecularTexture(scene, aiMaterial_p, material);
    material.roughnessTexture = readRoughnessTexture(scene, aiMaterial_p, material);

    // TODO: read remaining generic textures
    for (auto&[name, texturePath]: material.texturePaths) {
        auto p = texturePath;//scene.m_dir / texturePath;
        material.textures[name] = m_resourceManager->createResource<geo::Texture>(p);
    }
}

void SceneLoader::readMaterials(const aiScene *scene_p, Scene &scene) {
    for (int i = 0; i < scene_p->mNumMaterials; ++i) {
        aiMaterial *mat_p = scene_p->mMaterials[i];

        std::string name = mat_p->GetName().C_Str();
        auto material = m_resourceManager->createResource<geo::Material>(name);
        //    (*material)->name = mat_p->GetName().C_Str();

        spdlog::debug("Read material {} -- mapped to {} (vertex shader: {})", name, (*material)->name,
                      (*material)->vertexShader);

        readTextures(scene, mat_p, **material);

        // overwrite settings with scene file properties
        aiColor3D color(0.f, 0.f, 0.f);

        auto &mat = *material;
        if (!mat->hasDiffuse) {
            mat_p->Get(AI_MATKEY_COLOR_DIFFUSE, color);
            mat->diffuse.r = color.r;
            mat->diffuse.g = color.g;
            mat->diffuse.b = color.b;
        }
        std::cout << (*material)->name << " has diffuse: " << color.r << " " << color.g << " " << color.b << std::endl;

        if (!mat->hasSpecular) {
            mat_p->Get(AI_MATKEY_COLOR_SPECULAR, color);
            mat->specular.r = color.r;
            mat->specular.g = color.g;
            mat->specular.b = color.b;
        }
        std::cout << (*material)->name << " has specular: " << color.r << " " << color.g << " " << color.b << std::endl;

        if (!mat->hasAmbient) {
            mat_p->Get(AI_MATKEY_COLOR_AMBIENT, color);
            mat->ambient.r = color.r;
            mat->ambient.g = color.g;
            mat->ambient.b = color.b;
        }
        scene.m_materials.push_back(std::move(material));
    }
}

void SceneLoader::readMeshes(const aiScene *scene_p, Scene &scene) {
    scene.m_meshes.reserve(scene_p->mNumMeshes);

    for (unsigned int n = 0U; n < scene_p->mNumMeshes; ++n) {
        auto mesh = std::make_shared<Mesh>();
        const aiMesh *mesh_p = scene_p->mMeshes[n];

        mesh->name = mesh_p->mName.C_Str();
        mesh->polygonMode = PolygonMode::kTriangles;

        mesh->material = scene.m_materials[mesh_p->mMaterialIndex];
        spdlog::debug("Reading mesh {}: {} has material {} - {}", n, mesh_p->mName.C_Str(), (*mesh->material)->name,
                      (*mesh->material)->vertexShader);

        for (int i = 0; i < mesh_p->mNumVertices; ++i) {
            auto pVertex = mesh_p->mVertices[i];
            mesh->vertices.push_back(pVertex.x);
            mesh->vertices.push_back(pVertex.y);
            mesh->vertices.push_back(pVertex.z);

            auto pNormal = mesh_p->mNormals[i];
            mesh->normals.push_back(pNormal.x);
            mesh->normals.push_back(pNormal.y);
            mesh->normals.push_back(pNormal.z);

            aiVector3D ZERO3D = {0, 0, 0};
            auto uv = mesh_p->HasTextureCoords(0) ? mesh_p->mTextureCoords[0][i] : ZERO3D;
            mesh->uvs.push_back(uv.x);
            mesh->uvs.push_back(uv.y);
        }

        for (int f = 0; f < mesh_p->mNumFaces; ++f) {
            auto face = mesh_p->mFaces[f];
            mesh->indices.push_back(face.mIndices[0]);
            mesh->indices.push_back(face.mIndices[1]);
            mesh->indices.push_back(face.mIndices[2]);
        }

        scene.m_meshes.emplace_back(mesh);
    }
}

void SceneLoader::readHierarchy(const aiScene *scene_p, Scene &scene) {
    using ChildParent = std::pair<aiNode *, std::shared_ptr<SceneNode>>;
    std::deque<ChildParent> queue;

    queue.push_back(std::make_pair<>(scene_p->mRootNode, nullptr));

    // Queue is filled with entities, starting with root node.
    // Every time a node is processed.

    while (!queue.empty()) {
        auto[node_p, parent_p] = queue.front();
        queue.pop_front();

        auto node = std::make_shared<SceneNode>();
        if (parent_p != nullptr) {
            parent_p->children.push_back(node);
        } else {
            scene.m_rootNode = node;
        }

        node->name = std::string{node_p->mName.C_Str()};

        // assimp stores the transformations in
        std::memcpy(&node->transform[0][0], &node_p->mTransformation.a1, 16 * sizeof(ai_real));
        node->transform = glm::transpose(node->transform);

        // loops through all mesh instances of this node
        node->meshInstances.reserve(node_p->mNumMeshes);
        for (int i = 0U; i < node_p->mNumMeshes; ++i) {
            auto meshInstance = std::make_shared<MeshInstance>();

            auto mesh = scene.m_meshes[node_p->mMeshes[i]];

            for (int k = 0; k < node_p->mMetaData->mNumProperties; ++k) {
                std::cout << "\t\t" << node_p->mMetaData->mKeys[k].C_Str() << std::endl;
            }
            meshInstance->mesh = mesh;
            meshInstance->name = std::string{node_p->mName.C_Str()};
            meshInstance->material = mesh->material;
            spdlog::info("Mesh with name {}, has material {} -- vs: {}", mesh->name, (*mesh->material)->name,
                         (*mesh->material)->vertexShader);
            meshInstance->transform = glm::mat4(1.0F);

            node->meshInstances.push_back(meshInstance);
        }

        // register name
        scene.registerSceneNode(node);

        // add the children for the next round
        for (int i = 0; i < node_p->mNumChildren; ++i) {
            queue.push_back(std::make_pair<>(node_p->mChildren[i], node));
        }
    }
}

Scene::SceneNodeIterable Scene::getSceneNodesByName(const std::string &name) {
    return m_sceneNodes.count(name) > 0 ? m_sceneNodes.at(name) : SceneNodeIterable{};
}

void Scene::registerSceneNode(std::shared_ptr<SceneNode> node) {
    m_sceneNodes[node->name].push_back(node);

    if (m_sceneNodes.at(node->name).size() > 1) {
        spdlog::warn("Node with name '{}' has more than 1 entry. Is this correct?", node->name);
    }
}

void SceneLoader::readCameras(const aiScene *scene_p, Scene &scene) {
    for (int i = 0; i < scene_p->mNumCameras; ++i) {
        auto aiCamera_p = scene_p->mCameras[i];

        auto camera = std::make_shared<geo::Camera>();
        camera->name = aiCamera_p->mName.C_Str();
        camera->fovx = aiCamera_p->mHorizontalFOV;
        camera->aspect = aiCamera_p->mAspect;
        camera->position = glm::vec3(aiCamera_p->mPosition.x, aiCamera_p->mPosition.y, aiCamera_p->mPosition.z);
        camera->lookAt = glm::vec3(aiCamera_p->mLookAt.x, aiCamera_p->mLookAt.y, aiCamera_p->mLookAt.z);
        camera->up = glm::vec3(aiCamera_p->mUp.x, aiCamera_p->mUp.y, aiCamera_p->mUp.z);
        camera->near = aiCamera_p->mClipPlaneNear;
        camera->far = aiCamera_p->mClipPlaneFar;
        // not sure about this
        camera->fovy = camera->fovx / camera->aspect;

        auto cameraNodes = scene.getSceneNodesByName(camera->name);
        if (cameraNodes.empty()) {
            scene.m_rootNode->cameras.emplace_back(camera);
        } else {
            for (auto node: cameraNodes) {
                node->cameras.emplace_back(camera);
            }
        }

        scene.cameras().emplace_back(camera);
    }
}

void SceneLoader::readLights(const aiScene *aiScene, geo::Scene &scene) {
    for (int i = 0; i < aiScene->mNumLights; ++i) {
        auto aiLight = aiScene->mLights[i];
        auto light = std::make_shared<Light>();
        light->name = std::string{aiLight->mName.C_Str()};

        // diffuse color encodes wattage
        light->diffuseColor = glm::vec3(aiLight->mColorDiffuse.r, aiLight->mColorDiffuse.g, aiLight->mColorDiffuse.b);
        // TODO: we use blender standard. 1000W is considered intensity 1 - this is chosen arbitrarily.
        //  investigate how to improve, maybe HDR eventually
        light->intensity = glm::length(light->diffuseColor);
        light->diffuseColor = glm::normalize(light->diffuseColor);

        light->specularColor = glm::vec3(aiLight->mColorSpecular.r, aiLight->mColorSpecular.g,
                                         aiLight->mColorSpecular.b);
        light->ambientColor = glm::vec3(aiLight->mColorAmbient.r, aiLight->mColorAmbient.g, aiLight->mColorAmbient.b);

        light->upVector = glm::vec3{aiLight->mUp.x, aiLight->mUp.y, aiLight->mUp.z};
        light->attenuationQuadratic = aiLight->mAttenuationQuadratic;
        light->position = glm::vec3(aiLight->mPosition.x, aiLight->mPosition.y, aiLight->mPosition.z);
        light->radius = glm::vec2(aiLight->mSize.x, aiLight->mSize.y);

        switch (aiLight->mType) {
            case aiLightSource_POINT:
                light->type = Light::Type::POINT_LIGHT;
                break;

            case aiLightSource_DIRECTIONAL:
                light->type = Light::Type::DIRECTIONAL_LIGHT;
                light->position = glm::vec3(aiLight->mDirection.x, aiLight->mDirection.y, aiLight->mDirection.z);
                light->position *= -1;
                break;

            case aiLightSource_AMBIENT:
                light->type = Light::Type::AMBIENT_LIGHT;
                break;

            case aiLightSource_SPOT:
                light->type = Light::Type::SPOT_LIGHT;
                break;

            default:
                std::cerr << "Unknown light source type. Ignoring light source type.\n";
                continue;
        }

        // light belongs to a transform node
        auto lightNodes = scene.getSceneNodesByName(light->name);
        if (lightNodes.empty()) {
            scene.m_rootNode->lights.emplace_back(light);
        } else {
            for (auto node: lightNodes) {
                node->lights.emplace_back(light);
            }
        }

        scene.m_lights.emplace_back(light);
    }
}

void readTextures(const aiScene *scene_p, Scene &scene) {
    for (int i = 0; i < scene_p->mNumTextures; ++i) {
        auto aiTexture = scene_p->mTextures[i];
        std::cerr << "Could not load texture: " << aiTexture->mFilename.C_Str() << ": not yet implemented\n";
    }
}

std::unique_ptr<res::IResource> SceneLoader::createResource(const std::string &path) {
    geo::Scene scene;
    scene.m_path = path;
    scene.m_dir = scene.m_path.parent_path();

    Assimp::Importer m_importer;
    const aiScene *aiScene_p{nullptr};

    aiScene_p = m_importer.ReadFile(path, aiProcess_Triangulate);
    if (!aiScene_p) {
        throw std::runtime_error(fmt::format("Failed to load scene file: {}", path));
    }

    readMaterials(aiScene_p, scene);
    //  readEmbeddedTextures(aiScene_p, scene);

    readMeshes(aiScene_p, scene);
    readHierarchy(aiScene_p, scene);

    // should come after reading of hierarchy
    readLights(aiScene_p, scene);
    readCameras(aiScene_p, scene);

    return std::make_unique<res::Resource<geo::Scene>>(std::move(scene));
}
