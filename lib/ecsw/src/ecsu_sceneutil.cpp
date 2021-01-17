//
// Created by jlemein on 13-01-21.
//
#include <ecsu_sceneutil.h>
#include <ecsw_scene.h>
#include <entt/entt.hpp>

#include <deque>
using namespace artifax::ecsu;

void SceneUtil::addToScene(entt::registry& registry, ecsw::Scene& m) {
  auto scene_p = m.get();

  std::deque<aiNode*> queue;
  queue.emplace_back(scene_p->mRootNode);

  while (!queue.empty()) {
    auto node = queue.front();
    queue.pop_front();

    //
    if (node->mNumMeshes > 0) {
      std::vector<entt::entity> entities;
      for (auto i=0U; i<node->mNumMeshes; ++i) {
        auto pMesh = scene_p->mMeshes[node->mMeshes[0]];

//        ecs::MeshInstance meshInstance;
//
//        auto e = makeEntity();
//        MeshLoader::(e, mesh);
      }

    }
  }

}