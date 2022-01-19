//
// Created by jeffrey on 08-01-22.
//
#include <geo_graph.h>
#include <gtest/gtest.h>
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