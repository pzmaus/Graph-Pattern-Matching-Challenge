/**
 * @file backtrack.cc
 *
 */

#include "backtrack.h"

Backtrack::Backtrack() {}
Backtrack::~Backtrack() {}

void backtrack(const Graph &query, const Graph &data, std::vector<Vertex> query_dag[], const CandidateSet &cs, Vertex cur, std::vector<int> &u_visited, std::vector<bool> &v_visited, std::set<Vertex> &extendable, std::vector<Vertex> &candidates) {
  extendable.erase(cur);
  std::vector<Vertex> inserted;
  for(Vertex next : query_dag[cur]) {
    if(u_visited[next] == -1 && extendable.find(next) == extendable.end()) {
      extendable.insert(next);
      inserted.push_back(next);
    }
  }

  // std::cout << "current node : "<< cur << '\n';
  // std::cout << "- extendable : ";
  // for(Vertex ex : extendable) {
  //   std::cout << ex << ' ';
  // }
  // std::cout << '\n';

  for(Vertex cand : candidates) {
    // std::cout << "- selected candidate : " << cand << '\n';      
    u_visited[cur] = cand;
    v_visited[cand] = true;

    if(extendable.empty()) {
      std::cout << 'a';
      for(Vertex u = 0; u < query.GetNumVertices(); u++) {
        std::cout << ' ' << u_visited[u];
      }
      std::cout << '\n';
      u_visited[cur] = -1;
      v_visited[cand] = false;
      continue;
    }

    Vertex min_u = -1;
    std::vector<Vertex> min_final_v;

    for(Vertex u_next : extendable) {
      std::vector<Vertex> connected;
      size_t startOffset = query.GetNeighborStartOffset(u_next);
      size_t endOffset = query.GetNeighborEndOffset(u_next);
      for(size_t i = startOffset; i < endOffset; i++) {
        Vertex next = query.GetNeighbor(i);
        if (u_visited[next] != -1) {
          connected.push_back(next);
        }
      }

      // std::cout << "- - u_next : "<< u_next << '\n';
      // std::cout << "- - - connected: ";
      // for(Vertex c : connected) {
      //   std::cout << c << ' ';
      // }
      // std::cout << '\n';

      std::vector<Vertex> v_next;
      size_t candSize = cs.GetCandidateSize(u_next);
      for(size_t i = 0; i < candSize; i++) {
        Vertex cand = cs.GetCandidate(u_next, i);
        v_next.push_back(cand);
      }

      std::vector<Vertex> final_v;
      for(Vertex v : v_next) {
        bool check = true;
        for(Vertex connectedU : connected) {
          if(!data.IsNeighbor(v, u_visited[connectedU])) {
            check = false;
            break;
          }
        }
        if(check) final_v.push_back(v);
      }
      // std::cout << "- - - final_v : ";
      // for(Vertex c : final_v) {
      //   std::cout << c << ' ';
      // }
      // std::cout << '\n';


      if(min_u == -1 || final_v.size() < min_final_v.size()) {
        min_u = u_next;
        min_final_v = final_v;
      }
    }

    // std::cout << "- - next u : " << min_u << '\n';
    backtrack(query, data, query_dag, cs, min_u, u_visited, v_visited, extendable, min_final_v);

    u_visited[cur] = -1;
    v_visited[cand] = false;
  }

  extendable.insert(cur);
  for(Vertex v : inserted) {
    extendable.erase(v);
  }
}

void Backtrack::PrintAllMatches(const Graph &data, const Graph &query,
                                const CandidateSet &cs) {
  std::cout << "t " << query.GetNumVertices() << "\n";

  size_t numU = query.GetNumVertices();

  // get root from query graph
  double argmin = cs.GetCandidateSize(0) / (double) query.GetDegree(0);
  Vertex root = 0;
  for(Vertex u = 1; u < numU; u++) {
    int cini = cs.GetCandidateSize(u);
    int deg = query.GetDegree(u);
    double tmp = cini / (double) deg;
    if(tmp < argmin) {
      argmin = tmp;
      root = u;
    }
  }

  // bfs query graph
  std::vector<Vertex> adj[100000];
  bool visited[100000], letsmakeadj[100000];
  for(Vertex u = 0; u < numU; u++) {
    visited[u] = false;
    letsmakeadj[u] = false;
  }

  std::queue<Vertex> q;
  q.push(root);
  visited[root] = true;

  while(!q.empty()) {
    Vertex cur = q.front();
    q.pop();
    letsmakeadj[cur] = true;

    size_t startOffset = query.GetNeighborStartOffset(cur);
    size_t endOffset = query.GetNeighborEndOffset(cur);
    for(size_t i = startOffset; i < endOffset; i++) {
      Vertex next = query.GetNeighbor(i);
      if (!visited[next]) {
        visited[next] = true;
        q.push(next);
      }
      if(!letsmakeadj[next]) {
        adj[cur].push_back(next);
      }
    }
  }

  /*for(Vertex u = 0; u < numU; u++) {
    std::cout << u << ": ";
    for(Vertex v : adj[u]) std::cout << v << ' ';
    std::cout << '\n';
  }*/



  std::vector<int> u_visited;
  for(Vertex u = 0; u < numU; u++) u_visited.push_back(-1);
  std::vector<bool> v_visited;
  size_t numV = data.GetNumVertices();
  for(Vertex v = 0; v < numV; v++) v_visited.push_back(false);

  std::set<Vertex> extendable;
  extendable.insert(root);

  std::vector<Vertex> candidates;
  size_t rootCandSize = cs.GetCandidateSize(root);
  for(size_t i = 0; i < rootCandSize; i++) {
    Vertex cand = cs.GetCandidate(root, i);
    candidates.push_back(cand);
  }

  backtrack(query, data, adj, cs, root, u_visited, v_visited, extendable, candidates);
}
