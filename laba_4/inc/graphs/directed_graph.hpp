#pragma once
 
#include <algorithm>
#include <cstddef>
#include <iterator>
#include <limits>
#include <map>
#include <ostream>
#include <set>
#include <utility>
 
#include "adjacent_iterator.hpp"
#include "graph_exceptions.hpp"
#include "graph_traits.hpp"
 
 
template <class T, class Traits = vertex_traits<T>>
class directed_graph {
public:
    using value_type = T;
    using reference = T&;
    using const_reference = const T&;
    using pointer = T*;
    using const_pointer = const T*;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using traits_type = Traits;
 
    using vertex_id = std::size_t;
    using edge_type = std::pair<vertex_id, vertex_id>; 
 
private:
    using vertex_map = std::map<vertex_id, T>;
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

    using adjacent_vertex_iterator = adjacent_iterator<edge_iterator>;
    using const_adjacent_vertex_iterator = adjacent_vertex_iterator;
    using reverse_adjacent_vertex_iterator = std::reverse_iterator<adjacent_vertex_iterator>;
    using const_reverse_adjacent_vertex_iterator = reverse_adjacent_vertex_iterator;

    directed_graph() = default;
    directed_graph(const directed_graph& other)
        : vertices_(other.vertices_), edges_(other.edges_), next_id_(other.next_id_) {}
    ~directed_graph() = default;
 
    directed_graph& operator=(const directed_graph& other) {
        if (this != &other) {
            directed_graph tmp(other);
            vertices_.swap(tmp.vertices_);
            edges_.swap(tmp.edges_);
            std::swap(next_id_, tmp.next_id_);
        }
        return *this;
    }

    bool empty() const { return vertices_.empty(); }
 
    void clear() {
        vertices_.clear();
        edges_.clear();
        next_id_ = 0;
    }

    bool has_vertex(vertex_id id) const { return vertices_.count(id) != 0; }
 
    bool has_edge(vertex_id from, vertex_id to) const {
        return edges_.count(edge_type(from, to)) != 0;
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
        if (!has_edge(from, to)) throw edge_not_found();
        return static_cast<size_type>(std::count_if(
            edges_.begin(), edges_.end(), [&](const edge_type& e) {
                if (e.first == from && e.second == to) return false;
                return e.first == from || e.first == to || e.second == from || e.second == to;
            }));
    }

    reference at(vertex_id id) {
        auto it = vertices_.find(id);
        if (it == vertices_.end()) throw vertex_not_found();
        return it->second;
    }
    const_reference at(vertex_id id) const {
        auto it = vertices_.find(id);
        if (it == vertices_.end()) throw vertex_not_found();
        return it->second;
    }
 
    vertex_iterator find_vertex(vertex_id id) { return vertices_.find(id); }
    const_vertex_iterator find_vertex(vertex_id id) const { return vertices_.find(id); }
    edge_iterator find_edge(vertex_id from, vertex_id to) const {
        return edges_.find(edge_type(from, to));
    }

    vertex_iterator add_vertex(const T& value) {
        return vertices_.emplace(next_id_++, value).first;
    }
 

    std::pair<edge_iterator, bool> add_edge(vertex_id from, vertex_id to) {
        require_vertex(from);
        require_vertex(to);
        return edges_.insert(edge_type(from, to));
    }
 

    void remove_vertex(vertex_id id) {
        auto it = vertices_.find(id);
        if (it == vertices_.end()) throw vertex_not_found();
        erase_vertex(it);
    }
 
    void remove_edge(vertex_id from, vertex_id to) {
        auto it = edges_.find(edge_type(from, to));
        if (it == edges_.end()) throw edge_not_found();
        edges_.erase(it);
    }

    vertex_iterator erase_vertex(const_vertex_iterator pos) {
        if (pos == vertices_.end()) throw invalid_iterator();
        const vertex_id id = pos->first;
        edges_.erase(out_begin(id), out_end(id));            
        for (auto e = edges_.begin(); e != edges_.end();) {  
            if (e->second == id) e = edges_.erase(e);
            else ++e;
        }
        return vertices_.erase(pos);
    }
 
    edge_iterator erase_edge(const_edge_iterator pos) {
        if (pos == edges_.end()) throw invalid_iterator();
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
    const_reverse_vertex_iterator crbegin() const { return rbegin(); }
    const_reverse_vertex_iterator crend() const { return rend(); }
 
    edge_iterator edges_begin() const { return edges_.begin(); }
    edge_iterator edges_end() const { return edges_.end(); }
    const_edge_iterator edges_cbegin() const { return edges_.cbegin(); }
    const_edge_iterator edges_cend() const { return edges_.cend(); }
    reverse_edge_iterator edges_rbegin() const { return reverse_edge_iterator(edges_end()); }
    reverse_edge_iterator edges_rend() const { return reverse_edge_iterator(edges_begin()); }
    const_reverse_edge_iterator edges_crbegin() const { return edges_rbegin(); }
    const_reverse_edge_iterator edges_crend() const { return edges_rend(); }

    incident_edge_iterator incident_begin(vertex_id id) const { require_vertex(id); return out_begin(id); }
    incident_edge_iterator incident_end(vertex_id id) const { require_vertex(id); return out_end(id); }
    const_incident_edge_iterator incident_cbegin(vertex_id id) const { return incident_begin(id); }
    const_incident_edge_iterator incident_cend(vertex_id id) const { return incident_end(id); }
    reverse_incident_edge_iterator incident_rbegin(vertex_id id) const {
        return reverse_incident_edge_iterator(incident_end(id));
    }
    reverse_incident_edge_iterator incident_rend(vertex_id id) const {
        return reverse_incident_edge_iterator(incident_begin(id));
    }
    const_reverse_incident_edge_iterator incident_crbegin(vertex_id id) const { return incident_rbegin(id); }
    const_reverse_incident_edge_iterator incident_crend(vertex_id id) const { return incident_rend(id); }

    adjacent_vertex_iterator adjacent_begin(vertex_id id) const {
        return adjacent_vertex_iterator(incident_begin(id));
    }
    adjacent_vertex_iterator adjacent_end(vertex_id id) const {
        return adjacent_vertex_iterator(incident_end(id));
    }
    const_adjacent_vertex_iterator adjacent_cbegin(vertex_id id) const { return adjacent_begin(id); }
    const_adjacent_vertex_iterator adjacent_cend(vertex_id id) const { return adjacent_end(id); }
    reverse_adjacent_vertex_iterator adjacent_rbegin(vertex_id id) const {
        return reverse_adjacent_vertex_iterator(adjacent_end(id));
    }
    reverse_adjacent_vertex_iterator adjacent_rend(vertex_id id) const {
        return reverse_adjacent_vertex_iterator(adjacent_begin(id));
    }
    const_reverse_adjacent_vertex_iterator adjacent_crbegin(vertex_id id) const { return adjacent_rbegin(id); }
    const_reverse_adjacent_vertex_iterator adjacent_crend(vertex_id id) const { return adjacent_rend(id); }

    bool operator==(const directed_graph& o) const {
        return vertices_.size() == o.vertices_.size() && edges_ == o.edges_ &&
               std::equal(vertices_.begin(), vertices_.end(), o.vertices_.begin(),
                          [](const auto& a, const auto& b) {
                              return a.first == b.first && Traits::equal(a.second, b.second);
                          });
    }
    bool operator!=(const directed_graph& o) const { return !(*this == o); }

    bool operator<(const directed_graph& o) const {
        auto vless = [](const auto& a, const auto& b) {
            if (a.first != b.first) return a.first < b.first;
            return Traits::less(a.second, b.second);
        };
        if (std::lexicographical_compare(vertices_.begin(), vertices_.end(),
                                         o.vertices_.begin(), o.vertices_.end(), vless))
            return true;
        if (std::lexicographical_compare(o.vertices_.begin(), o.vertices_.end(),
                                         vertices_.begin(), vertices_.end(), vless))
            return false;
        return edges_ < o.edges_;
    }
    bool operator>(const directed_graph& o) const { return o < *this; }
    bool operator<=(const directed_graph& o) const { return !(o < *this); }
    bool operator>=(const directed_graph& o) const { return !(*this < o); }
 
    friend std::ostream& operator<<(std::ostream& os, const directed_graph& g) {
        os << "vertices:";
        std::for_each(g.begin(), g.end(), [&os](const auto& v) {
            os << " [" << v.first << ": ";
            Traits::print(os, v.second);
            os << ']';
        });
        os << "\nedges:";
        std::for_each(g.edges_begin(), g.edges_end(), [&os](const edge_type& e) {
            os << " (" << e.first << "->" << e.second << ')';
        });
        return os << '\n';
    }
 
private:
    void require_vertex(vertex_id id) const {
        if (!has_vertex(id)) throw vertex_not_found();
    }

    edge_iterator out_begin(vertex_id id) const { return edges_.lower_bound(edge_type(id, 0)); }
    edge_iterator out_end(vertex_id id) const {
        return edges_.upper_bound(edge_type(id, std::numeric_limits<vertex_id>::max()));
    }
 
    vertex_map vertices_;
    edge_set edges_;
    vertex_id next_id_ = 0;
};
 
 