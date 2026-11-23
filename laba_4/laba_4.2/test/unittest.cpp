#include <UnitTest++/UnitTest++.h>

#include "../inc/directed_graph.hpp"

#include <cstddef>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

using graph::DirectedGraph;
using graph::EdgeNotFoundException;
using graph::InvalidIteratorException;
using graph::VertexNotFoundException;

namespace {

using IntGraph = DirectedGraph<int>;
using Id = IntGraph::vertex_id;
using Edge = IntGraph::edge_type;

std::vector<Id> vertex_ids(const IntGraph& graph) {
    std::vector<Id> ids;
    for (auto it = graph.begin(); it != graph.end(); ++it) {
        ids.push_back(it->first);
    }
    return ids;
}

std::vector<Edge> edges(const IntGraph& graph) {
    std::vector<Edge> result;
    for (auto it = graph.edges_begin(); it != graph.edges_end(); ++it) {
        result.push_back(*it);
    }
    return result;
}

} 

SUITE(DirectedGraphTests)
{
    TEST(DefaultConstructorCreatesEmptyGraph)
    {
        IntGraph graph;

        CHECK(graph.empty());
        CHECK_EQUAL(0, static_cast<int>(graph.vertex_count()));
        CHECK_EQUAL(0, static_cast<int>(graph.edge_count()));
        CHECK(graph.begin() == graph.end());
        CHECK(graph.edges_begin() == graph.edges_end());
    }

    TEST(AddVertexAssignsSequentialIdsAndStoresValues)
    {
        IntGraph graph;

        auto first = graph.add_vertex(10);
        auto second = graph.add_vertex(20);

        CHECK(!graph.empty());
        CHECK_EQUAL(2, static_cast<int>(graph.vertex_count()));
        CHECK_EQUAL(0, static_cast<int>(first->first));
        CHECK_EQUAL(10, first->second);
        CHECK_EQUAL(1, static_cast<int>(second->first));
        CHECK_EQUAL(20, second->second);

        CHECK(graph.has_vertex(0));
        CHECK(graph.has_vertex(1));
        CHECK(!graph.has_vertex(2));
        CHECK_EQUAL(10, graph.at(0));
        CHECK_EQUAL(20, graph.at(1));
    }

    TEST(AddEdgeConnectsExistingVertices)
    {
        IntGraph graph;
        graph.add_vertex(10);
        graph.add_vertex(20);

        auto result = graph.add_edge(0, 1);

        CHECK(result.second);
        CHECK(result.first != graph.edges_end());
        CHECK_EQUAL(0, static_cast<int>(result.first->first));
        CHECK_EQUAL(1, static_cast<int>(result.first->second));
        CHECK_EQUAL(1, static_cast<int>(graph.edge_count()));
        CHECK(graph.has_edge(0, 1));
        CHECK(!graph.has_edge(1, 0));
    }

    TEST(ClearRemovesAllVerticesAndEdges)
    {
        IntGraph graph;
        graph.add_vertex(10);
        graph.add_vertex(20);
        graph.add_edge(0, 1);

        graph.clear();

        CHECK(graph.empty());
        CHECK_EQUAL(0, static_cast<int>(graph.vertex_count()));
        CHECK_EQUAL(0, static_cast<int>(graph.edge_count()));
        CHECK(graph.begin() == graph.end());
        CHECK(graph.edges_begin() == graph.edges_end());

        auto vertex = graph.add_vertex(30);
        CHECK_EQUAL(0, static_cast<int>(vertex->first));
        CHECK(graph.has_vertex(0));
        CHECK_EQUAL(30, graph.at(0));
    }

    TEST(AddEdgeThrowsForMissingEndpoints)
    {
        IntGraph graph;
        graph.add_vertex(10);

        CHECK_THROW(graph.add_edge(0, 1), VertexNotFoundException);
        CHECK_THROW(graph.add_edge(1, 0), VertexNotFoundException);
        CHECK_THROW(graph.add_edge(5, 6), VertexNotFoundException);

        CHECK_EQUAL(0, static_cast<int>(graph.edge_count()));
    }

    TEST(DuplicateEdgeIsNotInserted)
    {
        IntGraph graph;
        graph.add_vertex(10);
        graph.add_vertex(20);

        auto first = graph.add_edge(0, 1);
        auto duplicate = graph.add_edge(0, 1);

        CHECK(first.second);
        CHECK(!duplicate.second);
        CHECK_EQUAL(1, static_cast<int>(graph.edge_count()));
        CHECK(graph.has_edge(0, 1));
    }

    TEST(AtProvidesMutableAndConstAccess)
    {
        IntGraph graph;
        graph.add_vertex(10);

        const IntGraph& const_graph = graph;
        CHECK_EQUAL(10, const_graph.at(0));

        graph.at(0) = 42;
        CHECK_EQUAL(42, graph.at(0));
        CHECK_EQUAL(42, const_graph.at(0));
    }

    TEST(AtThrowsForMissingVertex)
    {
        IntGraph graph;
        graph.add_vertex(10);

        const IntGraph& const_graph = graph;

        CHECK_THROW(graph.at(1), VertexNotFoundException);
        CHECK_THROW(const_graph.at(1), VertexNotFoundException);
    }

    TEST(HasVertexAndHasEdgeReportPresence)
    {
        IntGraph graph;
        graph.add_vertex(10);
        graph.add_vertex(20);

        CHECK(graph.has_vertex(0));
        CHECK(graph.has_vertex(1));
        CHECK(!graph.has_vertex(2));
        CHECK(!graph.has_edge(0, 1));

        graph.add_edge(0, 1);

        CHECK(graph.has_edge(0, 1));
        CHECK(!graph.has_edge(1, 0));
        CHECK(!graph.has_edge(100, 200));
    }

    TEST(RemoveVertexDeletesAllIncidentEdgesAndCanEmptyGraph)
    {
        IntGraph graph;
        graph.add_vertex(10); 
        graph.add_vertex(20); 
        graph.add_vertex(30); 
        graph.add_vertex(40); 
        graph.add_edge(0, 1);
        graph.add_edge(0, 2);
        graph.add_edge(1, 2);
        graph.add_edge(3, 0);
        graph.add_edge(2, 0);

        graph.remove_vertex(0);

        CHECK(!graph.has_vertex(0));
        CHECK_EQUAL(3, static_cast<int>(graph.vertex_count()));
        CHECK_EQUAL(1, static_cast<int>(graph.edge_count()));
        CHECK(graph.has_edge(1, 2));

        graph.remove_vertex(1);
        CHECK_EQUAL(0, static_cast<int>(graph.edge_count()));

        graph.remove_vertex(2);
        graph.remove_vertex(3);

        CHECK(graph.empty());
        CHECK_EQUAL(0, static_cast<int>(graph.vertex_count()));
        CHECK_EQUAL(0, static_cast<int>(graph.edge_count()));
    }

    TEST(RemoveVertexAndRemoveEdgeThrowWhenMissing)
    {
        IntGraph graph;
        graph.add_vertex(10);
        graph.add_vertex(20);

        CHECK_THROW(graph.remove_vertex(5), VertexNotFoundException);
        CHECK_THROW(graph.remove_edge(0, 1), EdgeNotFoundException);

        CHECK_EQUAL(2, static_cast<int>(graph.vertex_count()));
        CHECK_EQUAL(0, static_cast<int>(graph.edge_count()));
    }

    TEST(RemoveEdgeDeletesOnlyRequestedEdge)
    {
        IntGraph graph;
        graph.add_vertex(10);
        graph.add_vertex(20);
        graph.add_vertex(30);

        graph.add_edge(0, 1);
        graph.add_edge(0, 2);

        graph.remove_edge(0, 1);

        CHECK_EQUAL(1, static_cast<int>(graph.edge_count()));
        CHECK(!graph.has_edge(0, 1));
        CHECK(graph.has_edge(0, 2));
        CHECK_EQUAL(3, static_cast<int>(graph.vertex_count()));
    }

    TEST(EraseVertexByIteratorRemovesVertexAndReturnsNext)
    {
        IntGraph graph;
        graph.add_vertex(100);
        graph.add_vertex(200);
        graph.add_vertex(300);
        graph.add_edge(0, 1);
        graph.add_edge(1, 2);

        auto next = graph.erase_vertex(graph.begin());

        CHECK(next != graph.end());
        CHECK_EQUAL(1, static_cast<int>(next->first));
        CHECK(!graph.has_vertex(0));
        CHECK_EQUAL(2, static_cast<int>(graph.vertex_count()));
        CHECK_EQUAL(1, static_cast<int>(graph.edge_count()));
        CHECK(graph.has_edge(1, 2));
    }

    TEST(EraseVertexByEndIteratorThrows)
    {
        IntGraph graph;
        graph.add_vertex(10);

        CHECK_THROW(graph.erase_vertex(graph.cend()), InvalidIteratorException);
    }

    TEST(EraseEdgeByIteratorRemovesEdgeAndEndThrows)
    {
        IntGraph graph;
        graph.add_vertex(10);
        graph.add_vertex(20);
        graph.add_vertex(30);
        graph.add_edge(0, 1);
        graph.add_edge(0, 2);

        auto next = graph.erase_edge(graph.edges_begin());

        CHECK(next != graph.edges_end());
        CHECK_EQUAL(0, static_cast<int>(next->first));
        CHECK_EQUAL(2, static_cast<int>(next->second));
        CHECK_EQUAL(1, static_cast<int>(graph.edge_count()));
        CHECK(!graph.has_edge(0, 1));
        CHECK_THROW(graph.erase_edge(graph.edges_end()), InvalidIteratorException);
    }

    TEST(CopyConstructorCreatesEqualIndependentGraph)
    {
        IntGraph graph;
        graph.add_vertex(10);
        graph.add_vertex(20);
        graph.add_vertex(30);
        graph.add_edge(0, 1);
        graph.add_edge(1, 2);

        IntGraph copy(graph);

        CHECK(copy == graph);
        CHECK_EQUAL(3, static_cast<int>(copy.vertex_count()));
        CHECK_EQUAL(2, static_cast<int>(copy.edge_count()));
        CHECK_EQUAL(10, copy.at(0));

        copy.at(0) = 99;
        CHECK(copy != graph);
        CHECK_EQUAL(10, graph.at(0));

        copy.add_vertex(40);
        CHECK_EQUAL(3, static_cast<int>(graph.vertex_count()));
        CHECK_EQUAL(4, static_cast<int>(copy.vertex_count()));
    }

    TEST(AssignmentOperatorCreatesEqualIndependentGraph)
    {
        IntGraph graph;
        graph.add_vertex(10);
        graph.add_vertex(20);
        graph.add_edge(0, 1);

        IntGraph other;
        other.add_vertex(500);

        other = graph;

        CHECK(other == graph);
        CHECK_EQUAL(2, static_cast<int>(other.vertex_count()));
        CHECK_EQUAL(1, static_cast<int>(other.edge_count()));
        CHECK(other.has_vertex(0));
        CHECK_EQUAL(10, other.at(0));

        other.add_vertex(30);
        CHECK(!graph.has_vertex(2));
        CHECK(other.has_vertex(2));
        CHECK_EQUAL(3, static_cast<int>(other.vertex_count()));
    }

    TEST(SelfAssignmentKeepsGraphIntact)
    {
        IntGraph graph;
        graph.add_vertex(10);
        graph.add_vertex(20);
        graph.add_edge(0, 1);

        IntGraph& alias = graph;
        graph = alias;

        CHECK_EQUAL(2, static_cast<int>(graph.vertex_count()));
        CHECK_EQUAL(1, static_cast<int>(graph.edge_count()));
        CHECK(graph.has_edge(0, 1));
        CHECK_EQUAL(10, graph.at(0));
        CHECK_EQUAL(20, graph.at(1));
    }

    TEST(EqualityOperatorsCompareStructure)
    {
        IntGraph first;
        first.add_vertex(10);
        first.add_vertex(20);
        first.add_edge(0, 1);

        IntGraph same;
        same.add_vertex(10);
        same.add_vertex(20);
        same.add_edge(0, 1);

        CHECK(first == same);
        CHECK(!(first != same));

        same.add_edge(1, 0);
        CHECK(first != same);

        IntGraph different_values;
        different_values.add_vertex(10);
        different_values.add_vertex(21);
        different_values.add_edge(0, 1);
        CHECK(first != different_values);

        IntGraph different_ids;
        different_ids.add_vertex(20);
        different_ids.add_vertex(10);
        different_ids.add_edge(0, 1);
        CHECK(first != different_ids);
    }

    TEST(RelationalOperatorsOrderGraphs)
    {
        IntGraph small;
        small.add_vertex(1);

        IntGraph large;
        large.add_vertex(1);
        large.add_vertex(2);

        CHECK(small < large);
        CHECK(large > small);
        CHECK(small <= large);
        CHECK(large >= small);
        CHECK(!(large < small));
        CHECK(!(small > large));

        CHECK(small <= small);
        CHECK(small >= small);
        CHECK(!(small < small));

        IntGraph with_edge(large);
        with_edge.add_edge(0, 1);

        CHECK(large < with_edge);
        CHECK(with_edge > large);
        CHECK(large <= with_edge);
        CHECK(with_edge >= large);
    }

    TEST(OutInAndTotalDegreesAreCorrect)
    {
        IntGraph graph;
        graph.add_vertex(10);
        graph.add_vertex(20); 
        graph.add_vertex(30); 
        graph.add_vertex(40); 

        graph.add_edge(0, 1);
        graph.add_edge(0, 2);
        graph.add_edge(0, 3);
        graph.add_edge(1, 0);
        graph.add_edge(2, 0);

        CHECK_EQUAL(3, static_cast<int>(graph.out_degree(0)));
        CHECK_EQUAL(2, static_cast<int>(graph.in_degree(0)));
        CHECK_EQUAL(5, static_cast<int>(graph.degree(0)));

        CHECK_EQUAL(1, static_cast<int>(graph.out_degree(1)));
        CHECK_EQUAL(1, static_cast<int>(graph.in_degree(1)));
        CHECK_EQUAL(2, static_cast<int>(graph.degree(1)));

        CHECK_EQUAL(0, static_cast<int>(graph.out_degree(3)));
        CHECK_EQUAL(1, static_cast<int>(graph.in_degree(3)));
        CHECK_EQUAL(1, static_cast<int>(graph.degree(3)));
    }

    TEST(DegreeThrowsForMissingVertex)
    {
        IntGraph graph;
        graph.add_vertex(10);

        CHECK_THROW(graph.out_degree(1), VertexNotFoundException);
        CHECK_THROW(graph.in_degree(1), VertexNotFoundException);
        CHECK_THROW(graph.degree(1), VertexNotFoundException);
    }

    TEST(EdgeDegreeIsSumOfEndpointDegrees)
    {
        IntGraph graph;
        graph.add_vertex(10); 
        graph.add_vertex(20); 
        graph.add_vertex(30); 
        graph.add_vertex(40);

        graph.add_edge(0, 1);
        graph.add_edge(0, 2);
        graph.add_edge(0, 3);
        graph.add_edge(1, 0);
        graph.add_edge(2, 0);

        CHECK_EQUAL(4, static_cast<int>(graph.edge_degree(0, 1)));
        CHECK_EQUAL(3, static_cast<int>(graph.edge_degree(1, 0)));
        CHECK_EQUAL(4, static_cast<int>(graph.edge_degree(0, 2)));
    }

    TEST(EdgeDegreeThrowsForMissingEdgeOrVertex)
    {
        IntGraph graph;
        graph.add_vertex(10);
        graph.add_vertex(20);

        CHECK_THROW(graph.edge_degree(0, 1), EdgeNotFoundException);
        CHECK_THROW(graph.edge_degree(0, 5), VertexNotFoundException);
        CHECK_THROW(graph.edge_degree(5, 0), VertexNotFoundException);
    }

    TEST(VertexIteratorTraversesInIdOrderAndIsBidirectional)
    {
        IntGraph graph;
        graph.add_vertex(100);
        graph.add_vertex(200);
        graph.add_vertex(300);
        graph.add_edge(0, 2);

        std::vector<Id> ids;
        for (auto it = graph.begin(); it != graph.end(); ++it) {
            it->second += 1;
        }

        CHECK(vertex_ids(graph) == std::vector<Id>({0, 1, 2}));
        CHECK_EQUAL(101, graph.at(0));
        CHECK_EQUAL(201, graph.at(1));
        CHECK_EQUAL(301, graph.at(2));

        auto it = graph.end();
        --it;
        CHECK_EQUAL(2, static_cast<int>(it->first));
        ++it;
        CHECK(it == graph.end());

        auto post = it--;
        CHECK(post == graph.end());
        CHECK_EQUAL(2, static_cast<int>(it->first));
    }

    TEST(ReverseAndConstVertexIterationWork)
    {
        IntGraph graph;
        graph.add_vertex(10);
        graph.add_vertex(20);
        graph.add_vertex(30);

        const IntGraph& const_graph = graph;

        CHECK(const_graph.cbegin() == const_graph.begin());
        CHECK(const_graph.cend() == const_graph.end());

        std::vector<Id> forward;
        for (auto it = const_graph.begin(); it != const_graph.end(); ++it) {
            forward.push_back(it->first);
        }
        CHECK(forward == std::vector<Id>({0, 1, 2}));

        std::vector<Id> backward;
        for (auto it = const_graph.rbegin(); it != const_graph.rend(); ++it) {
            backward.push_back(it->first);
        }
        CHECK(backward == std::vector<Id>({2, 1, 0}));
    }

    TEST(EdgeIteratorTraversesInLexicographicOrderAndReverse)
    {
        IntGraph graph;
        graph.add_vertex(10);
        graph.add_vertex(20);
        graph.add_vertex(30);

        graph.add_edge(1, 2);
        graph.add_edge(0, 2);
        graph.add_edge(0, 1);
        graph.add_edge(1, 0);

        std::vector<Edge> forward = edges(graph);
        CHECK(forward == std::vector<Edge>({
            Edge(0, 1), Edge(0, 2), Edge(1, 0), Edge(1, 2)
        }));

        std::vector<Edge> backward;
        for (auto it = graph.edges_rbegin(); it != graph.edges_rend(); ++it) {
            backward.push_back(*it);
        }
        CHECK(backward == std::vector<Edge>({
            Edge(1, 2), Edge(1, 0), Edge(0, 2), Edge(0, 1)
        }));
    }

    TEST(IncidentEdgeIteratorTraversesOutgoingEdges)
    {
        IntGraph graph;
        graph.add_vertex(10);
        graph.add_vertex(20);
        graph.add_vertex(30);
        graph.add_vertex(40);

        graph.add_edge(0, 1);
        graph.add_edge(0, 2);
        graph.add_edge(0, 3);
        graph.add_edge(2, 0);
        graph.add_edge(1, 0);

        std::vector<Edge> outgoing;
        for (auto it = graph.incident_begin(0); it != graph.incident_end(0); ++it) {
            outgoing.push_back(*it);
        }

        CHECK(outgoing == std::vector<Edge>({
            Edge(0, 1), Edge(0, 2), Edge(0, 3)
        }));
        CHECK_EQUAL(static_cast<int>(outgoing.size()),
                    static_cast<int>(graph.out_degree(0)));

        std::vector<Edge> reverse_outgoing;
        for (auto it = graph.incident_rbegin(0); it != graph.incident_rend(0); ++it) {
            reverse_outgoing.push_back(*it);
        }

        CHECK(reverse_outgoing == std::vector<Edge>({
            Edge(0, 3), Edge(0, 2), Edge(0, 1)
        }));

        CHECK_THROW(graph.incident_begin(99), VertexNotFoundException);
        CHECK_THROW(graph.incident_end(99), VertexNotFoundException);
    }

    TEST(AdjacentVertexIteratorAndReverseTraversal)
    {
        IntGraph graph;
        graph.add_vertex(10); 
        graph.add_vertex(20); 
        graph.add_vertex(30); 
        graph.add_vertex(40); 

        graph.add_edge(0, 1);
        graph.add_edge(0, 2);
        graph.add_edge(0, 3);
        graph.add_edge(2, 0);

        std::vector<int> adjacent;
        for (auto it = graph.adjacent_begin(0); it != graph.adjacent_end(0); ++it) {
            adjacent.push_back(*it);
        }

        CHECK(adjacent == std::vector<int>({20, 30, 40}));
        CHECK_EQUAL(20, *graph.adjacent_begin(0).operator->());

        std::vector<int> reverse_adjacent;
        for (auto it = graph.adjacent_rbegin(0); it != graph.adjacent_rend(0); ++it) {
            reverse_adjacent.push_back(*it);
        }

        CHECK(reverse_adjacent == std::vector<int>({40, 30, 20}));

        auto it = graph.adjacent_end(0);
        --it;
        CHECK_EQUAL(40, *it);
        ++it;
        CHECK(it == graph.adjacent_end(0));

        CHECK_THROW(graph.adjacent_begin(99), VertexNotFoundException);
        CHECK_THROW(graph.adjacent_end(99), VertexNotFoundException);
    }

    TEST(OutputOperatorPrintsVerticesAndEdges)
    {
        DirectedGraph<std::string> graph;
        graph.add_vertex("Alice");
        graph.add_vertex("Bob");
        graph.add_edge(0, 1);

        std::ostringstream stream;
        stream << graph;
        const std::string output = stream.str();

        CHECK(output.find("Vertices:") != std::string::npos);
        CHECK(output.find("[0: Alice]") != std::string::npos);
        CHECK(output.find("[1: Bob]") != std::string::npos);
        CHECK(output.find("Edges:") != std::string::npos);
        CHECK(output.find("(0->1)") != std::string::npos);
    }
}
