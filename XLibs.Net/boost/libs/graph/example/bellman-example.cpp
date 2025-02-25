//=======================================================================
// Copyright 2001 Jeremy G. Siek, Andrew Lumsdaine, Lie-Quan Lee, 
//
// This file is part of the Boost Graph Library
//
// You should have received a copy of the License Agreement for the
// Boost Graph Library along with the software; see the file LICENSE.
// If not, contact Office of Research, Indiana University,
// Bloomington, IN 47405.
//
// Permission to modify the code and to distribute the code is
// granted, provided the text of this NOTICE is retained, a notice if
// the code was modified is included with the above COPYRIGHT NOTICE
// and with the COPYRIGHT NOTICE in the LICENSE file, and that the
// LICENSE file is distributed with the modified code.
//
// LICENSOR MAKES NO REPRESENTATIONS OR WARRANTIES, EXPRESS OR IMPLIED.
// By way of example, but not limitation, Licensor MAKES NO
// REPRESENTATIONS OR WARRANTIES OF MERCHANTABILITY OR FITNESS FOR ANY
// PARTICULAR PURPOSE OR THAT THE USE OF THE LICENSED SOFTWARE COMPONENTS
// OR DOCUMENTATION WILL NOT INFRINGE ANY PATENTS, COPYRIGHTS, TRADEMARKS
// OR OTHER RIGHTS.
//=======================================================================
#include <boost/config.hpp>
#include <vector>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/bellman_ford_shortest_paths.hpp>

using namespace boost;

template < typename Graph, typename ParentMap > 
struct edge_writer
{
  edge_writer(const Graph & g, const ParentMap & p)
  : m_g(g), m_parent(p)
  {
  }

  template < typename Edge >
    void operator() (std::ostream & out, const Edge & e) const
  {
    out << "[label=\"" << get(edge_weight, m_g, e) << "\"";
    typename graph_traits < Graph >::vertex_descriptor
      u = source(e, m_g), v = target(e, m_g);
    if (m_parent[v] == u)
        out << ", color=\"black\"";
    else
        out << ", color=\"grey\"";
      out << "]";
  }
  const Graph & m_g;
  ParentMap m_parent;
};
template < typename Graph, typename Parent >
edge_writer < Graph, Parent >
make_edge_writer(const Graph & g, const Parent & p)
{
  return edge_writer < Graph, Parent > (g, p);
}

int
main()
{
  enum { u, v, x, y, z, N };
  char name[] = { 'u', 'v', 'x', 'y', 'z' };
  typedef std::pair < int, int >E;
  const int n_edges = 10;
  E edge_array[] = { E(u, y), E(u, x), E(u, v), E(v, u),
      E(x, y), E(x, v), E(y, v), E(y, z), E(z, u), E(z,x) };
  int weight[n_edges] = { -4, 8, 5, -2, 9, -3, 7, 2, 6, 7 };

  typedef adjacency_list < vecS, vecS, directedS,
    no_property, property < edge_weight_t, int > > Graph;
#if defined(BOOST_MSVC) && BOOST_MSVC <= 1300
  // VC++ can't handle the iterator constructor
  Graph g(N);
  for (std::size_t j = 0; j < n_edges; ++j)
    add_edge(edge_array[j].first, edge_array[j].second, g);
#else
  Graph g(edge_array, edge_array + n_edges, N);
#endif
  graph_traits < Graph >::edge_iterator ei, ei_end;
  property_map<Graph, edge_weight_t>::type weight_pmap = get(edge_weight, g);
  int i = 0;
  for (tie(ei, ei_end) = edges(g); ei != ei_end; ++ei, ++i)
    weight_pmap[*ei] = weight[i];

  std::vector<int> distance(N, (std::numeric_limits < short >::max)());
  std::vector<std::size_t> parent(N);
  for (i = 0; i < N; ++i)
    parent[i] = i;
  distance[z] = 0;

#if defined(BOOST_MSVC) && BOOST_MSVC <= 1300
  bool r = bellman_ford_shortest_paths
    (g, int(N), weight_pmap, &parent[0], &distance[0], 
     closed_plus<int>(), std::less<int>(), default_bellman_visitor());
#else
  bool r = bellman_ford_shortest_paths
    (g, int (N), weight_map(weight_pmap).distance_map(&distance[0]).
     predecessor_map(&parent[0]));
#endif

  if (r)
    for (i = 0; i < N; ++i)
      std::cout << name[i] << ": " << std::setw(3) << distance[i]
        << " " << name[parent[i]] << std::endl;
  else
    std::cout << "negative cycle" << std::endl;

  std::ofstream dot_file("figs/bellman-eg.dot");
  dot_file << "digraph D {\n"
    << "  rankdir=LR\n"
    << "  size=\"5,3\"\n"
    << "  ratio=\"fill\"\n"
    << "  edge[style=\"bold\"]\n" << "  node[shape=\"circle\"]\n";

  {
    for (tie(ei, ei_end) = edges(g); ei != ei_end; ++ei) {
      graph_traits < Graph >::edge_descriptor e = *ei;
      graph_traits < Graph >::vertex_descriptor
        u = source(e, g), v = target(e, g);
      // VC++ doesn't like the 3-argument get function, so here
      // we workaround by using 2-nested get()'s.
      dot_file << name[u] << " -> " << name[v]
        << "[label=\"" << get(get(edge_weight, g), e) << "\"";
      if (parent[v] == u)
        dot_file << ", color=\"black\"";
      else
        dot_file << ", color=\"grey\"";
      dot_file << "]";
    }
  }
  dot_file << "}";
  return EXIT_SUCCESS;
}
