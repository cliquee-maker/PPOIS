#pragma once

#include <stdexcept>
#include <string>

namespace graph {

class GraphException : public std::runtime_error {
public:
    explicit GraphException(const std::string& message)
        : std::runtime_error(message) {}
};

class VertexNotFoundException : public GraphException {
public:
    explicit VertexNotFoundException(const std::string& message = "Vertex not found")
        : GraphException(message) {}
};

class EdgeNotFoundException : public GraphException {
public:
    explicit EdgeNotFoundException(const std::string& message = "Edge not found")
        : GraphException(message) {}
};

class DuplicateVertexException : public GraphException {
public:
    explicit DuplicateVertexException(const std::string& message = "Vertex already exists")
        : GraphException(message) {}
};

class DuplicateEdgeException : public GraphException {
public:
    explicit DuplicateEdgeException(const std::string& message = "Edge already exists")
        : GraphException(message) {}
};

class InvalidIteratorException : public GraphException {
public:
    explicit InvalidIteratorException(const std::string& message = "Invalid iterator")
        : GraphException(message) {}
};

}  