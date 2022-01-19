//
// Created by jeffrey on 08-01-22.
//
#pragma once

#include <fmt/format.h>
#include <array>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>

namespace izz {
namespace geo {

template <int N, typename T, bool From>
class Connections;

/**
 * @brief A graph that is written for the material system specifically.
 * @tparam T
 * @tparam n
 * @tparam M edge metadata. If you want to store weights (float) or metadata in other data types.
 */
template <int N=16, typename T = int, typename K = const char*, typename M=int>
class Graph {
  template <int, typename, bool>
  friend class node_iterator;
  template <int, typename, bool>
  friend class Connections;

 private:
  M m_edges[N][N];
  bool m_isConnected[N][N];  /// m_isConnected[i][j] indicates if node[i] is connected to node[j].
  std::array<T, N> m_nodes;

  std::unordered_map<K, int> m_keys;
  int m_nodeCount{0U};

 public:
  /**
   * Retrieves the maximum depth of the graph. That means: the longest chain of nodes connected.
   **/
  //  int maxDepth();

  T finalNode() {
    return m_nodes[0];
  }

  auto nodes() const noexcept {
    return m_nodes;
  }

  auto nodeSize() const noexcept {
    return m_nodeCount;
  }

  bool isConnected(T from, T to) {
    auto from_it = std::find(std::begin(m_nodes), std::end(m_nodes), from);
    auto to_it = std::find(std::begin(m_nodes), std::end(m_nodes), to);

    if (from_it == m_nodes.end() || to_it == m_nodes.end()) {
      return false;
    }

    int ifrom = from_it - m_nodes.begin();
    int ito = to_it - m_nodes.begin();

    return m_isConnected[ifrom][ito];
  }

  K name(int node_index) {
    auto it = std::find_if(std::begin(m_keys), std::end(m_keys),
                           [node_index](auto&& p) { return p.second == node_index; });

    if (it == std::end(m_keys)) {
      throw std::runtime_error(fmt::format("Cannot retrieve name for node index {}.", node_index));
    }

    return it->first;
  }

  Connections<N, T, false> connections_to(K key) {
    if (!m_keys.contains(key)) {
      throw std::runtime_error("No value found");
    }

    return Connections<N, T, false>(this, m_keys.at(key));
  }

  Connections<N, T, true> connections(K key) {
    if (!m_keys.contains(key)) {
      throw std::runtime_error("No value found");
    }

    return Connections<N, T, true>(this, m_keys.at(key));
  }

  auto begin() const noexcept {
    return m_nodes.begin();
  }

  auto end() const noexcept {
    return m_nodes.end();
  }

  int getNodeCount() {
    return m_nodeCount;
  }

  T& operator[](K key) {
    if (m_keys.contains(key)) {
      return m_nodes[m_keys.at(key)];
    } else {
      if (m_nodeCount + 1 > N) {
        throw std::runtime_error("Graph has no space left to store more nodes.");
      }

      auto index = m_nodeCount++;
      m_keys[key] = index;
      return m_nodes[index];
    }
  }

  void add(K key, T value) {
    if (m_keys.contains(key)) {
      throw std::runtime_error(fmt::format("Cannot add key '{}' to graph. Already exists.", key));
    }

    if (m_nodeCount + 1 > N) {
      throw std::runtime_error("Graph has no space left to store more nodes.");
    }

    auto index = m_nodeCount++;
    m_keys[key] = index;
    m_nodes[index] = value;
  }

  void add(std::pair<K, T> keyValue) {
    add(keyValue.first, keyValue.second);
  }

  void add_connect(std::pair<K, T> from, std::pair<K, T> to) {
    add(from.first, from.second);
    add(to.first, to.second);

    m_isConnected[m_keys[from.first]][m_keys[to.first]] = true;
  }

  void connect(K keyFrom, std::pair<K, T> to, M metadata = M()) {
    add(to.first, to.second);

    auto index_from = m_keys[keyFrom];
    auto index_to = m_keys[to.first];
    m_isConnected[index_from][index_to] = true;
    m_edges[index_from][index_to] = metadata;
  }

  void connect(K from, K to, M metadata = M()) {
    if (!m_keys.contains(from)) {
      m_keys[from] = m_nodeCount++;
    }
    if (!m_keys.contains(to)) {
      m_keys[to] = m_nodeCount++;
    }

    m_isConnected[m_keys[from]][m_keys[to]] = true;
    m_edges[m_keys[from]][m_keys[to]] = metadata;
  }

  void add(T from, T to) {
    int i, j;

    auto it = std::find(std::begin(m_nodes), std::end(m_nodes), from);

    // if material id 'from' is not in m_nodes, then add it, store the location in 'i'.
    if (it == std::end(m_nodes)) {
      i = m_nodeCount;
      m_nodes[m_nodeCount++] = from;
    }

    it = std::find(std::begin(m_nodes), std::end(m_nodes), to);
    // if material id 'to' is not in m_nodes, then add it, store the location in 'j'.
    if (it == std::end(m_nodes)) {
      j = m_nodeCount;
      m_nodes[m_nodeCount++] = to;
    }

    m_isConnected[i][j] = true;
  }

  void add(T id) {
    if (int i = std::find(std::begin(m_nodes), std::end(m_nodes), id); i == std::end(m_nodes)) {
      m_nodes[m_nodeCount++] = id;
    }
  }

  Graph() {
    memset(m_isConnected, 0, sizeof(m_isConnected));
//    memset(m_nodes, nullptr, sizeof(m_nodes));
  }
};

template <int N, typename T, bool Reverse = false>
class node_iterator {
 public:
  using iterator_category = std::forward_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using value_type = T;
  using pointer = T*;
  using reference = T&;

  node_iterator(Graph<N, T>* graph, int node, int index = 0)
    : m_graph{graph}
    , m_node{node}
    , m_index{index} {}

  value_type operator*() const {
    return m_index;
  }

  pointer operator->() {
    return &m_index;
  }

  node_iterator& operator++() {
    if constexpr (Reverse == false) {
      while (m_index < N && !m_graph->m_isConnected[m_node][++m_index]) {
      }
    } else {
      while (m_index < N && !m_graph->m_isConnected[++m_index][m_node]) {
      }
    }

    return *this;
  }

  //    node_iterator operator++(int) { node_iterator tmp = *this; ++(*this); return tmp; }

  friend bool operator==(const node_iterator& a, const node_iterator& b) {
    return a.m_graph == b.m_graph && a.m_node == b.m_node && a.m_index == b.m_index;
  };
  friend bool operator!=(const node_iterator& a, const node_iterator& b) {
    return a.m_graph != b.m_graph || a.m_node != b.m_node || a.m_index != b.m_index;
  };

 private:
  Graph<N, T>* m_graph;
  int m_node;
  int m_index = 0;
};

template <int N, typename T, bool From>
class Connections {
 public:
  Connections(Graph<N, T>* graph, int node)
    : m_graph{graph}
    , m_node{node} {}

  auto begin() const noexcept {
    // find first connection
    int i;
    if constexpr (From) {
      for (i = 0; i < N; ++i) {
        if (m_graph->m_isConnected[m_node][i]) break;
      }
    } else {
      for (i = 0; i < N; ++i) {
        if (m_graph->m_isConnected[i][m_node]) break;
      }
    }

    return node_iterator<N, T, !From>(m_graph, m_node, i);
  }

  auto end() const noexcept {
    return node_iterator<N, T, !From>(m_graph, m_node, N);
  }

 private:
  Graph<N, T>* m_graph;
  int m_node;
};

}  // namespace geo
}  // namespace izz