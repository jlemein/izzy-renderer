//
// Created by jeffrey on 08-01-22.
//
#include <geo_graph.h>
#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
using namespace izz::geo;

class GraphTest : public testing::Test {};

TEST_F(GraphTest, Iterators) {
  Graph<8, int, const char*> g;

  g["node1"] = 10;
  g.connect("node1", std::pair<const char*, int>{"node2", 20});
  g.connect("node1", std::pair<const char*, int>{"node3", 120});
  g.connect("node1", std::pair<const char*, int>{"node4", 220});

  g["node5"] = 5;
  g["node6"] = 6;
  g["node7"] = 7;
  g.connect("node5", "node4");
  g.connect("node6", "node4");
  g.connect("node7", "node4");

  std::cout << "TO node4 " << std::endl;
  for (auto n : g.connections_to("node4")) {
    std::cout << g.name(n) << " -> node4" << std::endl;
  }

  std::cout << "FROM node 1" << std::endl;
  for (auto n : g.connections("node1")) {
    std::cout << "node1 -> " << g.name(n) << std::endl;
  }

//  auto it = g.connectionsTo(10).begin();
//  std::cout << "First connection is: " << *it << std::endl;
//  std::cout << "Second connection is: " << *(++it) << std::endl;
//  ++it;

//  std::cout << (it == g.connectionsTo(10).end() ? "true" : "false") << std::endl;
}

/**
 * Creates a sample graph for multi pass render effect. This simulates complex order of operations.
 * P0 -------> P2 -> P5
 * P0 -> P1 -> P2
 * P4 -------------> P5
 */
TEST_F(GraphTest, SortByDepth) {
  Graph<> g;
  g["pass1"] = 1;
  g["pass2"] = 2;
  g["pass3"] = 3;
  g["pass4"] = 4;
  g["pass5"] = 5;

  g.sortByDepth();

  EXPECT_EQ(g["pass1"].depth, 0);
  EXPECT_EQ(g["pass2"].depth, 0);
  EXPECT_EQ(g["pass3"].depth, 0);

  // let's shuffle things up a bit
  g.connect("pass1", "pass2");
  g.connect("pass2", "pass3");
  g.connect("pass3", "pass5");
  g.connect("pass4", "pass5");

  g.sortByDepth();

  EXPECT_EQ(g["pass1"].depth, 0);
  EXPECT_EQ(g["pass2"].depth, 1);
  EXPECT_EQ(g["pass3"].depth, 2);
  EXPECT_EQ(g["pass4"].depth, 0);
  EXPECT_EQ(g["pass5"].depth, 3);

  // print out sorted by depth
  for (int i=0; i<g.nodeSize(); ++i) {
    std::cout << "Pass " << g.sortedByDepth(i).data << " = " <<  g.sortedByDepth(i).depth << std::endl;
  }
}