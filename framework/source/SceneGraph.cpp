#include "SceneGraph.hpp"

SceneGraph::SceneGraph():
    name_{"Scene Graph"},
    root_{}{}

SceneGraph::SceneGraph(std::string const& name, Node const& root):
    name_ {name},
    root_{root}{}

std::string SceneGraph::printGraph() const{
    //see below function print() in line 33
    return 0;
}

void SceneGraph::setName(std::string const& name){
    name_ = name;
}

void SceneGraph::setRoot(Node const& root){
    root_ = root;
}

Node SceneGraph::getRoot() const{
    return root_;
}

std::string SceneGraph::getName() const{
    return name_;
}

std::ostream& SceneGraph::print(std::ostream& os) const{
    os << "Name: " << name_ << "\n" << "Root: " << root_ << "\n";
    return os;
}

std::ostream& operator<<(std::ostream& os, SceneGraph const& sc){
    return sc.print(os);
}