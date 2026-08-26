//
// Created by jlemein on 10-01-21.
//
#include <ecsu_relationshipsorter.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <entt/entt.hpp>
#include <izz_relationship.h>
#include <izz_relationshiputil.h>
using namespace izz;
using namespace testing;
using namespace izz::ecs;

class RelationshipSorterTest : public ::testing::Test {};

//
MATCHER_P(ViewEq, value, "") {
  if (arg.size() != value.size()) {
    return false;
  }

  auto leftIt = arg.begin();
  auto valueIt = value.begin();

  std::cout << "Actual: [";
  for (auto a : arg) {
    std::cout << static_cast<int>(a) << " ";
  }
  std::cout << "]\n";
  std::cout << "Expected: [";
  for (auto a : value) {
    std::cout << static_cast<int>(a) << " ";
  }
  std::cout << "]\n";

  while (leftIt != arg.end() || valueIt != value.end()) {
    auto actualId = static_cast<unsigned int>(*leftIt);
    auto expectedId = static_cast<unsigned int>(*valueIt);

    std::cout << "Value of view (expected): " << expectedId
              << " | value of list (actual): " << actualId << "\n";

    ++leftIt;
    ++valueIt;
    if (actualId != expectedId)
      return false;
  }

  return leftIt == arg.end() && valueIt == value.end();
}

TEST_F(RelationshipSorterTest, SortParents) {
  entt::registry r;

  // create 3 entitiess
  auto entities = std::vector<entt::entity>{6};
  r.create(entities.begin(), entities.end());
  r.insert<Relationship>(entities.begin(), entities.end());

  RelationshipUtil::MakeChildren(r, entities[0], {entities[1], entities[3]});
  RelationshipUtil::MakeChild(r, entities[1], entities[2]);
  RelationshipUtil::MakeChildren(r, entities[3], {entities[4], entities[5]});

  ecsu::RelationshipSorter::Sort<Relationship>(r);

  auto view = r.view<Relationship>();
  auto expectedOrder =
      std::vector<entt::entity>{entities[0], entities[1], entities[3],
                                entities[2], entities[4], entities[5]};

  ASSERT_THAT(view, ViewEq(expectedOrder));
}

TEST_F(RelationshipSorterTest, SortParents2) {
  entt::registry r;

  // create 3 entitiess
  auto entities = std::vector<entt::entity>{10};
  r.create(entities.begin(), entities.end());
  r.insert<Relationship>(entities.begin(), entities.end());

  RelationshipUtil::MakeChildren(r, entities[0], {entities[1], entities[3]});
  RelationshipUtil::MakeChild(r, entities[1], entities[2]);
  RelationshipUtil::MakeChildren(r, entities[3], {entities[4], entities[5]});
  RelationshipUtil::MakeChild(r, entities[5], entities[6]);
  RelationshipUtil::MakeChild(r, entities[4], entities[7]);
  RelationshipUtil::MakeChild(r, entities[2], entities[8]);

  /*
   *                0             9
   *               / \
   *              1   3
   *             /   / \
   *            2   4   5
   *            |   |   |
   *            8   7   6
   *
   * Expected breadth first order: 0, 9, 1, 3, 2, 4, 5, 6, 7, 8 **(not 8, 7, 6)
   * The addresses are used, so the order of adding determines also the order
   * of looping through the items
   */
  auto expectedOrder = std::vector<entt::entity>{
      entities[0], entities[9], entities[1], entities[3], entities[2],
      entities[4], entities[5], entities[6], entities[7], entities[8]};
  ecsu::RelationshipSorter::Sort<Relationship>(r);

  auto view = r.view<Relationship>();

  ASSERT_THAT(view, ViewEq(expectedOrder));
}