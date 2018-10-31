#ifndef NODE_HPP
#define NODE_HPP

#include <glm/vec3.hpp>
#include <glm/glm.hpp>
#include <cmath>
#include <string>
#include <list>
#include <iostream>
#include <fstream>
#include "glm/ext.hpp"
#include <memory>

class Node{

    public:
        //Constructors
        Node();
        Node(std::shared_ptr<Node> const& parent, std::string const& name,
             std::string const& path, int depth);

        //Getter
        std::shared_ptr<Node> getParent() const;
        Node getChildren(std::string const& childName) const;
        std::list<std::shared_ptr<Node>> getChildrenList() const;
        std::string getName() const;
        std::string getPath() const;
        int getDepth() const;
        glm::mat4 getLocalTransform() const;
        glm::mat4 getWorldTransform() const;

        //Setter
        void setParent(std::shared_ptr<Node> const& parent);
        void setLocalTransform(glm::mat4 const& localTransfrom);
        void setWorldTransform(glm::mat4 const& worldTransform);

        void addChildren(std::shared_ptr<Node> const& child);
        Node removeChildren(std::string const& childName);

        float getSpeed() const;
        void setSpeed(float speed);

        float getDistanceOrigin() const;
        void setDistanceOrigin(float distanceOrigin);

        //kann überschrieben werden:
        virtual std::ostream& print(std::ostream& os) const;
    
    protected:
        
        //Member
        std::shared_ptr<Node> parent_;
        std::list<std::shared_ptr<Node>> children_;
        std::string name_;
        std::string path_;
        int depth_;
        glm::mat4 localTransform_;
        glm::mat4 worldTransform_;
        float speed_;
        float distanceOrigin_;
};

std::ostream& operator<<(std::ostream& os, Node const& n);

#endif
