#ifndef SCENEGRAPH_HPP
#define SCENEGRAPH_HPP

#include "Node.hpp"
#include <glm/vec3.hpp>
#include <glm/glm.hpp>
#include <cmath>
#include <string>
#include <iostream>
#include <fstream>

class SceneGraph{

    public:

        SceneGraph();
        SceneGraph(std::string const& name, Node const& root);
        std::string getName() const;
        Node getRoot() const;
        std::string printGraph() const;
        std::ostream& print(std::ostream& os) const;

    private:

        void setName(std::string const& name);
        void setRoot(Node const& root);

        std::string name_;
        Node root_;
};

std::ostream& operator<<(std::ostream& os, SceneGraph const& sc);

#endif 