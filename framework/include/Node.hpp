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

#include "pixel_data.hpp"
#include "texture_loader.hpp"
#include "structs.hpp"

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
        virtual pixel_data getTexture() const;
        virtual void setTexture(std::string const& texPath);
        virtual std::string getTexpath() const;
        virtual void setTexPath(std::string const& texPath);
        virtual texture_object getTextureObject() const;
        virtual void setTextureObject(texture_object const& textureObject);

        virtual std::vector<std::string> getTexpaths() const;
        virtual void setTexPaths(std::vector<std::string> const& texPaths);
        virtual std::map<std::string, pixel_data> getTextures() const;
        virtual void setTextures(std::vector<std::string> const& texPaths);

        //Normalmapping
        virtual std::string getNormalTexpath() const;
        virtual void setNormalTexPath(std::string const& normalTexPath);

        virtual pixel_data getNormalTexture() const;
        virtual void setNormalTexture(std::string const& normalTexPath);

        virtual texture_object getNormalTextureObject() const;
        virtual void setNormalTextureObject(texture_object const& normalTextureObject);

        virtual bool hasNormapMapping() const;
        virtual void setHasNormalMapping(bool normalMapping);

        // virtual int getPlanetID() const;
        // virtual void setPlanetID(int id);

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
