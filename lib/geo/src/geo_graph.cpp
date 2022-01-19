////
//// Created by jeffrey on 08-01-22.
////
//#include <geo_graph.h>
//using namespace izz::geo;
//
///**
//   * Retrieves the maximum depth of the graph. That means: the longest chain of nodes connected.
//   **/
////  int maxDepth();
//MaterialId EffectGraph::finalNode() {
//  return m_nodes[0];
//}
//
//auto begin() const noexcept {
//  return m_nodes.begin();
//}
//
//auto end() const noexcept {
//  return m_nodes.end();
//}
//
//
//int getNodeCount() { return m_nodeCount; }
//
//void add(MaterialId from, MaterialId to) {
//  int i, j;
//
//  // if material id 'from' is not in m_nodes, then add it, store the location in 'i'.
//  if (i = std::find(std::begin(m_nodes), std::end(m_nodes), from); i == std::end(m_nodes)) {
//    i = m_nodes[m_nodeCount++] = from;
//  }
//
//  // if material id 'to' is not in m_nodes, then add it, store the location in 'j'.
//  if (j = std::find(std::begin(m_nodes), std::end(m_nodes), from); i == std::end(m_nodes)) {
//    j = m_nodes[m_nodeCount++] = from;
//  }
//
//  m_isConnected[i][j] = true;
//}
//
//void add(MaterialId id) {
//  if (int i = std::find(std::begin(m_nodes), std::end(m_nodes), id); i == std::end(m_nodes)) {
//    m_nodes[m_nodeCount++] = id;
//  }
//}
//
//
//
//
//EffectGraph() {
//  memset(m_isConnected, 0, sizeof(m_isConnected));
//  memset(m_nodes, nullptr, sizeof(m_nodes));
//}
//
//EffectGraph(MaterialId id)
//  : EffectGraph() {
//  add(id);
//}
//
//MaterialId operator[](std::string key) {
//  return m_nodes[m_keys.at(key)];
//}
//
//MaterialId operator[](int i) {
//  return m_nodes[i];
//}