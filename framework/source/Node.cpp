#include "Node.hpp"

Node::Node():
    parent_(nullptr),
    children_(),
    name_("Node"), 
    path_(), //Was zur Hölle soll das denn sein? Der Pfad zum Knoten? Aber wie?
    depth_(0),
    localTransform_(1.0f), //Einheitsmatrix
    worldTransform_(1.0f),
    speed_(1.0f),
    distanceOrigin_(0.0f){}

Node::Node(std::shared_ptr<Node> const& parent, std::string const& name, 
           std::string const& path, int depth):
    parent_(parent),
    children_(),
    name_(name),
    path_(path),
    depth_(depth),
    localTransform_(1.0f),
    worldTransform_(1.0f),
    speed_(1.0f),
    distanceOrigin_(0.0f){}


std::shared_ptr<Node> Node::getParent() const{
    return parent_;
}

Node Node::getChildren(std::string const& childName) const{
    for(auto const& child: children_){
        if(child -> name_ == childName){
            return *child;
        }
    }
    return Node{}; //was returnt man denn, wenn nix dabei war? --> keine Ahnung vllt sowas?
}

std::list<std::shared_ptr<Node>> Node::getChildrenList() const{
    return children_;
}

std::string Node::getName() const{
    return name_;
}

std::string Node::getPath() const{
    return path_;
}

int Node::getDepth() const{
    return depth_;
}

glm::mat4 Node::getLocalTransform() const{
    return localTransform_;
}

glm::mat4 Node::getWorldTransform() const{
    return worldTransform_;
}

void Node::addChildren(std::shared_ptr<Node> const& child){
    children_.push_back(child);
}

Node Node::removeChildren(std::string const& childName){
    for(std::list<std::shared_ptr<Node>>::iterator it = children_.begin(); it != children_.end(); ++it){
        if((*it)-> name_ == childName){
            auto tmp = *(*it);
            children_.erase(it);
            return tmp;
        }
    }
    std::cout << "ERROR - " << childName << " does not exist in this List.";
}

void Node::setParent(std::shared_ptr<Node> const& parent){
    parent_ = parent;
}

void Node::setLocalTransform(glm::mat4 const& localTransform){
    localTransform_ = localTransform;
}

void Node::setWorldTransform(glm::mat4 const& worldTransform){
    worldTransform_ = worldTransform;
}

float Node::getSpeed() const{
    return speed_;
}

void Node::setSpeed(float speed){
    speed_ = speed;
}

float Node::getDistanceOrigin() const{
    return distanceOrigin_;
}

void Node::setDistanceOrigin(float distanceOrigin){
    distanceOrigin_ = distanceOrigin;
}

//Just for now and us, maybe that's helpful
std::ostream& Node::print(std::ostream& os) const{
    os << "name: " << name_ << "\n"
    << "path: " << path_ << "\n"
    << "Parent: " << parent_ << "\n" 
    << "children: ";
    for(auto const& i: children_){
        os << i -> name_ <<", ";
    } 
    os << "\n"
    << "depth: " << depth_ << "\n";
    // << "Local Transform: " << glm::to_string(localTransform_) << "\n"
    // << "World Transform: " << glm::to_string(worldTransform_) << "\n";
    return os;
}

std::ostream& operator<<(std::ostream& os, Node const& n){
    return n.print(os);
}



