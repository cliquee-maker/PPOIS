#include "../inc/directed_graph.hpp"

#include <algorithm>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

namespace {

using StringGraph = graph::DirectedGraph<std::string>;

std::string readLine(const std::string& prompt) {
    std::cout << prompt;
    std::string line;
    if (!std::getline(std::cin, line)) {
        return {};
    }
    return line;
}

int readNumber(const std::string& prompt, int minimum, int maximum) {
    while (true) {
        std::string line = readLine(prompt);
        if (line.empty() && !std::cin) {
            return -1;
        }
        try {
            std::size_t consumed = 0;
            int value = std::stoi(line, &consumed);
            if (consumed == line.size() && value >= minimum && value <= maximum) {
                return value;
            }
        } catch (const std::exception&) {
        }
        std::cout << "Enter a number from " << minimum << " to " << maximum
                  << ".\n";
    }
}

StringGraph::vertex_id readVertexId(const std::string& prompt) {
    while (true) {
        std::string line = readLine(prompt);
        if (line.empty() && !std::cin) {
            return std::numeric_limits<StringGraph::vertex_id>::max();
        }
        try {
            std::size_t consumed = 0;
            unsigned long long value = std::stoull(line, &consumed);
            if (consumed == line.size()) {
                return static_cast<StringGraph::vertex_id>(value);
            }
        } catch (const std::exception&) {
        }
        std::cout << "Enter a valid vertex ID.\n";
    }
}

void printVertices(const StringGraph& graph) {
    std::cout << "Vertices:";
    std::for_each(graph.cbegin(), graph.cend(),
                  [](const auto& item) {
                      std::cout << " [" << item.first << ": " << item.second << ']';
                  });
    std::cout << '\n';
}

void printVerticesReverse(const StringGraph& graph) {
    std::cout << "Vertices in reverse:";
    std::for_each(graph.rbegin(), graph.rend(),
                  [](const auto& item) {
                      std::cout << " [" << item.first << ": " << item.second << ']';
                  });
    std::cout << '\n';
}

void printEdges(const StringGraph& graph) {
    std::cout << "Edges:";
    std::for_each(graph.edges_begin(), graph.edges_end(),
                  [](const StringGraph::edge_type& edge) {
                      std::cout << " (" << edge.first << " -> " << edge.second
                                << ')';
                  });
    std::cout << '\n';
}

void printEdgesReverse(const StringGraph& graph) {
    std::cout << "Edges in reverse:";
    std::for_each(graph.edges_rbegin(), graph.edges_rend(),
                  [](const StringGraph::edge_type& edge) {
                      std::cout << " (" << edge.first << " -> " << edge.second
                                << ')';
                  });
    std::cout << '\n';
}

void printVertexDetails(const StringGraph& graph, StringGraph::vertex_id id) {
    std::cout << "Vertex [" << id << ": " << graph.at(id) << "]\n";
    std::cout << "Out-degree: " << graph.out_degree(id)
              << ", In-degree: " << graph.in_degree(id)
              << ", Total degree: " << graph.degree(id) << '\n';

    std::cout << "Incident edges:";
    for (auto it = graph.incident_begin(id);
         it != graph.incident_end(id); ++it) {
        std::cout << " (" << it->first << " -> " << it->second << ')';
    }
    std::cout << '\n';

    std::cout << "Incident edges in reverse:";
    for (auto it = graph.incident_rbegin(id);
         it != graph.incident_rend(id); ++it) {
        std::cout << " (" << it->first << " -> " << it->second << ')';
    }
    std::cout << '\n';

    std::cout << "Adjacent vertices:";
    for (auto it = graph.adjacent_begin(id);
         it != graph.adjacent_end(id); ++it) {
        std::cout << ' ' << *it;
    }
    std::cout << '\n';

    std::cout << "Adjacent vertices in reverse:";
    for (auto it = graph.adjacent_rbegin(id);
         it != graph.adjacent_rend(id); ++it) {
        std::cout << ' ' << *it;
    }
    std::cout << '\n';
}

void sortVertices(const StringGraph& graph) {
    std::vector<std::pair<StringGraph::vertex_id, std::string>> vertices(
        graph.cbegin(), graph.cend());
    std::sort(vertices.begin(), vertices.end(),
              [](const auto& left, const auto& right) {
                  return left.second < right.second;
              });
    std::cout << "Vertices sorted with std::sort:";
    for (const auto& [id, value] : vertices) {
        std::cout << " [" << id << ": " << value << ']';
    }
    std::cout << '\n';
}

void sortEdges(const StringGraph& graph) {
    std::vector<StringGraph::edge_type> edges(graph.edges_begin(),
                                              graph.edges_end());
    std::sort(edges.begin(), edges.end(),
              [](const StringGraph::edge_type& left,
                 const StringGraph::edge_type& right) {
                  if (left.first != right.first) {
                      return left.first < right.first;
                  }
                  return left.second < right.second;
              });
    std::cout << "Edges sorted with std::sort:";
    for (const StringGraph::edge_type& edge : edges) {
        std::cout << " (" << edge.first << " -> " << edge.second << ')';
    }
    std::cout << '\n';
}

void compareWithCopy(const StringGraph& graph) {
    StringGraph copy(graph);
    std::cout << "Copy == original: " << std::boolalpha << (copy == graph)
              << '\n';

    auto added = copy.add_vertex("copy-only");
    std::cout << "After adding vertex [" << added->first << ": "
              << added->second << "] to the copy:\n";
    std::cout << "  original:\n" << graph << '\n';
    std::cout << "  copy:\n" << copy << '\n';
    std::cout << "  copy != original: " << (copy != graph) << '\n';
    std::cout << "  original < copy: " << (graph < copy) << '\n';
    std::cout << "  original <= copy: " << (graph <= copy) << '\n';
    std::cout << "  original > copy: " << (graph > copy) << '\n';
    std::cout << "  original >= copy: " << (graph >= copy) << '\n';
}

void printMenu() {
    std::cout << "\n--- Directed Graph menu ---\n"
              << " 1. Add vertex\n"
              << " 2. Add edge\n"
              << " 3. Remove vertex\n"
              << " 4. Remove edge\n"
              << " 5. Show graph and counts\n"
              << " 6. Show vertex details\n"
              << " 7. Iterate vertices forward and reverse\n"
              << " 8. Iterate edges forward and reverse\n"
              << " 9. Sort vertices with std::sort\n"
              << "10. Sort edges with std::sort\n"
              << "11. Compare graph with its copy\n"
              << "12. Run automatic demonstration\n"
              << " 0. Exit\n";
}

void makeDemo(StringGraph& graph) {
    graph.clear();
    const auto a = graph.add_vertex("A")->first;
    const auto b = graph.add_vertex("B")->first;
    const auto c = graph.add_vertex("C")->first;
    const auto d = graph.add_vertex("D")->first;
    const auto e = graph.add_vertex("E")->first;

    graph.add_edge(a, b);
    graph.add_edge(a, c);
    graph.add_edge(b, d);
    graph.add_edge(c, d);
    graph.add_edge(d, e);
}

void runDemo(const StringGraph& graph) {
    std::cout << "\nGraph:\n" << graph << '\n';
    std::cout << "Vertex count: " << graph.vertex_count()
              << ", edge count: " << graph.edge_count() << '\n';
    printVertices(graph);
    printVerticesReverse(graph);
    printEdges(graph);
    printEdgesReverse(graph);
    sortVertices(graph);
    sortEdges(graph);
    if (graph.has_vertex(3)) {
        std::cout << "Degree of vertex 3: " << graph.degree(3) << '\n';
        printVertexDetails(graph, 3);
    }
    if (graph.has_edge(0, 1)) {
        std::cout << "Degree of edge (0 -> 1): " << graph.edge_degree(0, 1)
                  << '\n';
    }
    compareWithCopy(graph);
}

}  

int main() {
    StringGraph graph;
    makeDemo(graph);

    std::cout << "Generic directed graph container.\n";
    std::cout << "Initial graph:\n" << graph << '\n';

    while (true) {
        printMenu();
        const int choice = readNumber("Choice: ", 0, 12);
        if (choice < 0) {
            break;
        }

        try {
            switch (choice) {
                case 0:
                    std::cout << "Goodbye.\n";
                    return 0;
                case 1: {
                    const std::string vertex = readLine("Vertex value: ");
                    const auto it = graph.add_vertex(vertex);
                    std::cout << "Vertex [" << it->first << ": " << it->second
                              << "] added.\n";
                    break;
                }
                case 2: {
                    const auto from = readVertexId("Source vertex ID: ");
                    const auto to = readVertexId("Target vertex ID: ");
                    const auto [it, inserted] = graph.add_edge(from, to);
                    if (inserted) {
                        std::cout << "Edge (" << it->first << " -> "
                                  << it->second << ") added.\n";
                    } else {
                        std::cout << "Edge (" << from << " -> " << to
                                  << ") already exists.\n";
                    }
                    break;
                }
                case 3: {
                    const auto id = readVertexId("Vertex ID: ");
                    graph.remove_vertex(id);
                    std::cout << "Vertex " << id << " removed.\n";
                    break;
                }
                case 4: {
                    const auto from = readVertexId("Source vertex ID: ");
                    const auto to = readVertexId("Target vertex ID: ");
                    graph.remove_edge(from, to);
                    std::cout << "Edge (" << from << " -> " << to
                              << ") removed.\n";
                    break;
                }
                case 5:
                    std::cout << graph << '\n';
                    std::cout << "Vertex count: " << graph.vertex_count()
                              << ", edge count: " << graph.edge_count()
                              << ", empty: " << std::boolalpha
                              << graph.empty() << '\n';
                    break;
                case 6: {
                    const auto id = readVertexId("Vertex ID: ");
                    printVertexDetails(graph, id);
                    break;
                }
                case 7:
                    printVertices(graph);
                    printVerticesReverse(graph);
                    break;
                case 8:
                    printEdges(graph);
                    printEdgesReverse(graph);
                    break;
                case 9:
                    sortVertices(graph);
                    break;
                case 10:
                    sortEdges(graph);
                    break;
                case 11:
                    compareWithCopy(graph);
                    break;
                case 12:
                    runDemo(graph);
                    break;
                default:
                    break;
            }
        } catch (const std::exception& error) {
            std::cout << "Error: " << error.what() << '\n';
        }
    }

    return 0;
}