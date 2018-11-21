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
             std::string const& path, int depth, std::shared_ptr<Node> const& origin);

        //Getter
        std::shared_ptr<Node> getParent() const;
        Node getChildren(std::string const& childName) const;
        std::list<std::shared_ptr<Node>> getChildrenList() const;
        std::string getName() const;
        std::string getPath() const;
        int getDepth() const;
        glm::fmat4 getLocalTransform() const;
        glm::fmat4 getWorldTransform() const;
        float getSpeed() const;
        glm::fvec3 getDistanceOrigin() const;
        float getRadius() const;
        float getSelfRotation() const;
        std::shared_ptr<Node> getOrigin() const;


        //Setter
        void setParent(std::shared_ptr<Node> const& parent);
        void setLocalTransform(glm::fmat4 const& localTransfrom);
        void setWorldTransform(glm::fmat4 const& localTransform);
        void setDistanceOrigin(glm::fvec3 const& distanceOrigin);
        void setSpeed(float speed);
        void setRadius(float radius);
        void setSelfRotation(float rotation);
        void setOrigin(std::shared_ptr<Node> const& origin);

        void addChildren(std::shared_ptr<Node> const& child);
        Node removeChildren(std::string const& childName);

        //kann überschrieben werden:
        virtual std::ostream& print(std::ostream& os) const;

        void setColor(glm::fvec3 const& color);
        glm::fvec3 getColor() const;
    
    
    protected:
        
        //Member
        std::shared_ptr<Node> parent_;
        std::list<std::shared_ptr<Node>> children_;
        std::string name_;
        std::string path_;
        int depth_;
        glm::fmat4 localTransform_;
        glm::fmat4 worldTransform_;
        float speed_;
        float selfRotation_; //speed of self rotation
        glm::fvec3 distanceOrigin_;
        float radius_;
        std::shared_ptr<Node> origin_;
        glm::fvec3 color_;
};

std::ostream& operator<<(std::ostream& os, Node const& n);

#endif
