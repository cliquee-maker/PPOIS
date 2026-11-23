#pragma once

#include "exceptions.hpp"

#include <algorithm>
#include <cstddef>
#include <functional>
#include <iterator>
#include <limits>
#include <map>
#include <ostream>
#include <set>
#include <utility>

namespace graph {

template <typename T>
class DirectedGraph {
public:
    using value_type = T;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using difference_type = std::ptrdiff_t;
    using size_type = std::size_t;
    using vertex_id = std::size_t;
    using edge_type = std::pair<vertex_id, vertex_id>;

private:
    using vertex_map = std::map<vertex_id, value_type>;
    using edge_set = std::set<edge_type>;

public:
    using vertex_iterator = typename vertex_map::iterator;
    using const_vertex_iterator = typename vertex_map::const_iterator;
    using reverse_vertex_iterator = std::reverse_iterator<vertex_iterator>;
    using const_reverse_vertex_iterator = std::reverse_iterator<const_vertex_iterator>;

    using edge_iterator = typename edge_set::const_iterator;
    using const_edge_iterator = typename edge_set::const_iterator;
    using reverse_edge_iterator = std::reverse_iterator<edge_iterator>;
    using const_reverse_edge_iterator = std::reverse_iterator<const_edge_iterator>;

    using incident_edge_iterator = edge_iterator;
    using const_incident_edge_iterator = const_edge_iterator;
    using reverse_incident_edge_iterator = reverse_edge_iterator;
    using const_reverse_incident_edge_iterator = const_reverse_edge_iterator;

    class adjacent_vertex_iterator {
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = const value_type*;
        using reference = const value_type&;

        adjacent_vertex_iterator() = default;
        explicit adjacent_vertex_iterator(incident_edge_iterator it, const vertex_map* vmap)
            : it_(it), vmap_(vmap) {}

        reference operator*() const {
            if (vmap_ == nullptr) {
                throw InvalidIteratorException();
            }
            auto found = vmap_->find(it_->second);
            if (found == vmap_->end()) {
                throw VertexNotFoundException();
            }
            return found->second;
        }

        pointer operator->() const {
            return &(**this);
        }

        adjacent_vertex_iterator& operator++() { ++it_; return *this; }
        adjacent_vertex_iterator operator++(int) { auto tmp = *this; ++it_; return tmp; }
        adjacent_vertex_iterator& operator--() { --it_; return *this; }
        adjacent_vertex_iterator operator--(int) { auto tmp = *this; --it_; return tmp; }

        bool operator==(const adjacent_vertex_iterator& other) const {
            return it_ == other.it_ && vmap_ == other.vmap_;
        }
        bool operator!=(const adjacent_vertex_iterator& other) const {
            return !(*this == other);
        }

    private:
        incident_edge_iterator it_{};
        const vertex_map* vmap_ = nullptr;
    };

    using const_adjacent_vertex_iterator = adjacent_vertex_iterator;
    using reverse_adjacent_vertex_iterator = std::reverse_iterator<adjacent_vertex_iterator>;
    using const_reverse_adjacent_vertex_iterator = reverse_adjacent_vertex_iterator;

    DirectedGraph() = default;
    DirectedGraph(const DirectedGraph&) = default;
    DirectedGraph& operator=(const DirectedGraph&) = default;
    ~DirectedGraph() = default;

    bool empty() const { return vertices_.empty(); }

    void clear() {
        vertices_.clear();
        edges_.clear();
        next_id_ = 0;
    }

    bool has_vertex(vertex_id id) const { return vertices_.count(id) != 0; }

    bool has_edge(vertex_id from, vertex_id to) const {
        return edges_.count({from, to}) != 0;
    }

    size_type vertex_count() const { return vertices_.size(); }
    size_type edge_count() const { return edges_.size(); }

    size_type out_degree(vertex_id id) const {
        require_vertex(id);
        return static_cast<size_type>(std::distance(out_begin(id), out_end(id)));
    }

    size_type in_degree(vertex_id id) const {
        require_vertex(id);
        return static_cast<size_type>(std::count_if(
            edges_.begin(), edges_.end(), [id](const edge_type& e) { return e.second == id; }));
    }

    size_type degree(vertex_id id) const { return out_degree(id) + in_degree(id); }

    size_type edge_degree(vertex_id from, vertex_id to) const {
        require_vertex(from);
        require_vertex(to);
        if (!has_edge(from, to)) throw EdgeNotFoundException();
        return out_degree(from) + in_degree(to);
    }

    const value_type& at(vertex_id id) const {
        auto it = vertices_.find(id);
        if (it == vertices_.end()) throw VertexNotFoundException();
        return it->second;
    }

    value_type& at(vertex_id id) {
        auto it = vertices_.find(id);
        if (it == vertices_.end()) throw VertexNotFoundException();
        return it->second;
    }

    vertex_iterator add_vertex(const value_type& value) {
        return vertices_.emplace(next_id_++, value).first;
    }

    std::pair<edge_iterator, bool> add_edge(vertex_id from, vertex_id to) {
        require_vertex(from);
        require_vertex(to);
        return edges_.insert({from, to});
    }

    void remove_vertex(vertex_id id) {
        auto it = vertices_.find(id);
        if (it == vertices_.end()) throw VertexNotFoundException();
        erase_vertex(it);
    }

    void remove_edge(vertex_id from, vertex_id to) {
        auto it = edges_.find({from, to});
        if (it == edges_.end()) throw EdgeNotFoundException();
        edges_.erase(it);
    }

    vertex_iterator erase_vertex(const_vertex_iterator pos) {
        if (pos == vertices_.end()) throw InvalidIteratorException();
        const vertex_id id = pos->first;
        edges_.erase(out_begin(id), out_end(id));
        for (auto e = edges_.begin(); e != edges_.end();) {
            if (e->second == id) e = edges_.erase(e);
            else ++e;
        }
        return vertices_.erase(pos);
    }

    edge_iterator erase_edge(const_edge_iterator pos) {
        if (pos == edges_.end()) throw InvalidIteratorException();
        return edges_.erase(pos);
    }

    vertex_iterator begin() { return vertices_.begin(); }
    vertex_iterator end() { return vertices_.end(); }
    const_vertex_iterator begin() const { return vertices_.begin(); }
    const_vertex_iterator end() const { return vertices_.end(); }
    const_vertex_iterator cbegin() const { return vertices_.cbegin(); }
    const_vertex_iterator cend() const { return vertices_.cend(); }
    reverse_vertex_iterator rbegin() { return reverse_vertex_iterator(end()); }
    reverse_vertex_iterator rend() { return reverse_vertex_iterator(begin()); }
    const_reverse_vertex_iterator rbegin() const { return const_reverse_vertex_iterator(end()); }
    const_reverse_vertex_iterator rend() const { return const_reverse_vertex_iterator(begin()); }

    edge_iterator edges_begin() const { return edges_.begin(); }
    edge_iterator edges_end() const { return edges_.end(); }
    const_edge_iterator edges_cbegin() const { return edges_.cbegin(); }
    const_edge_iterator edges_cend() const { return edges_.cend(); }
    reverse_edge_iterator edges_rbegin() const { return reverse_edge_iterator(edges_end()); }
    reverse_edge_iterator edges_rend() const { return reverse_edge_iterator(edges_begin()); }

    incident_edge_iterator incident_begin(vertex_id id) const { require_vertex(id); return out_begin(id); }
    incident_edge_iterator incident_end(vertex_id id) const { require_vertex(id); return out_end(id); }
    reverse_incident_edge_iterator incident_rbegin(vertex_id id) const { return reverse_incident_edge_iterator(incident_end(id)); }
    reverse_incident_edge_iterator incident_rend(vertex_id id) const { return reverse_incident_edge_iterator(incident_begin(id)); }

    adjacent_vertex_iterator adjacent_begin(vertex_id id) const {
        return adjacent_vertex_iterator(incident_begin(id), &vertices_);
    }
    adjacent_vertex_iterator adjacent_end(vertex_id id) const {
        return adjacent_vertex_iterator(incident_end(id), &vertices_);
    }
    reverse_adjacent_vertex_iterator adjacent_rbegin(vertex_id id) const {
        return reverse_adjacent_vertex_iterator(adjacent_end(id));
    }
    reverse_adjacent_vertex_iterator adjacent_rend(vertex_id id) const {
        return reverse_adjacent_vertex_iterator(adjacent_begin(id));
    }

    bool operator==(const DirectedGraph& other) const {
        return vertices_ == other.vertices_ && edges_ == other.edges_;
    }
    bool operator!=(const DirectedGraph& other) const { return !(*this == other); }
    bool operator<(const DirectedGraph& other) const {
        if (vertices_ != other.vertices_) return vertices_ < other.vertices_;
        return edges_ < other.edges_;
    }
    bool operator>(const DirectedGraph& other) const { return other < *this; }
    bool operator<=(const DirectedGraph& other) const { return !(other < *this); }
    bool operator>=(const DirectedGraph& other) const { return !(*this < other); }

    friend std::ostream& operator<<(std::ostream& os, const DirectedGraph& g) {
        os << "Vertices: ";
        std::for_each(g.begin(), g.end(), [&os](const auto& pair) {
            os << "[" << pair.first << ": " << pair.second << "] ";
        });
        os << "\nEdges: ";
        std::for_each(g.edges_begin(), g.edges_end(), [&os](const auto& edge) {
            os << "(" << edge.first << "->" << edge.second << ") ";
        });
        return os;
    }

private:
    void require_vertex(vertex_id id) const {
        if (!has_vertex(id)) throw VertexNotFoundException();
    }

    edge_iterator out_begin(vertex_id id) const {
        return edges_.lower_bound({id, 0});
    }
    edge_iterator out_end(vertex_id id) const {
        return edges_.upper_bound({id, std::numeric_limits<vertex_id>::max()});
    }

    vertex_map vertices_;
    edge_set edges_;
    vertex_id next_id_ = 0;
};

}