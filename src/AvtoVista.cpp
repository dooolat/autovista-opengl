#include "AvtoVista.h"

#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace {
struct MeshData { std::vector<float> v; std::vector<std::uint32_t> i; };

MeshData cylinderData(unsigned int segments=64) {
    MeshData m; constexpr float half=0.5f, tau=6.28318530718f;
    const float step=tau/static_cast<float>(segments);
    for (unsigned int k=0;k<segments;++k) {
        const float a=k*step,b=(k+1)*step;
        const float x0=std::cos(a),z0=std::sin(a),x1=std::cos(b),z1=std::sin(b);
        const auto n=static_cast<std::uint32_t>(m.v.size()/3);
        m.v.insert(m.v.end(),{x0,-half,z0,x0,half,z0,x1,half,z1,x1,-half,z1});
        m.i.insert(m.i.end(),{n,n+1,n+2,n,n+2,n+3});
    }
    auto center=static_cast<std::uint32_t>(m.v.size()/3);
    m.v.insert(m.v.end(),{0,half,0,0,-half,0});
    const auto bottom=center+1;
    for (unsigned int k=0;k<segments;++k) {
        const float a=k*step,b=(k+1)*step;
        auto p=static_cast<std::uint32_t>(m.v.size()/3);
        m.v.insert(m.v.end(),{std::cos(a),half,std::sin(a),std::cos(b),half,std::sin(b),
                               std::cos(a),-half,std::sin(a),std::cos(b),-half,std::sin(b)});
        m.i.insert(m.i.end(),{center,p+1,p,bottom,p+2,p+3});
    }
    return m;
}

MeshData plusData() {
    MeshData m;
    constexpr float l=.72f,w=.11f;
    m.v={-l,0,-w,l,0,-w,l,0,w,-l,0,w,-w,0,-l,w,0,-l,w,0,l,-w,0,l};
    m.i={0,1,2,0,2,3,4,5,6,4,6,7};
    return m;
}

MeshData cubeData() {
    MeshData m;
    m.v={-.5f,-.5f,-.5f,.5f,-.5f,-.5f,.5f,.5f,-.5f,-.5f,.5f,-.5f,
         -.5f,-.5f,.5f,.5f,-.5f,.5f,.5f,.5f,.5f,-.5f,.5f,.5f};
    m.i={0,2,1,0,3,2,4,5,6,4,6,7,0,1,5,0,5,4,
         3,7,6,3,6,2,0,4,7,0,7,3,1,2,6,1,6,5};
    return m;
}
}

bool AvtoVistaScene::Mesh::create(const std::vector<float>& v,const std::vector<std::uint32_t>& i) {
    if(v.empty()||i.empty()) return false;
    indexCount=static_cast<GLsizei>(i.size());
    glGenVertexArrays(1,&vao); glGenBuffers(1,&vbo); glGenBuffers(1,&ebo);
    glBindVertexArray(vao); glBindBuffer(GL_ARRAY_BUFFER,vbo);
    glBufferData(GL_ARRAY_BUFFER,static_cast<GLsizeiptr>(v.size()*sizeof(float)),v.data(),GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,static_cast<GLsizeiptr>(i.size()*sizeof(std::uint32_t)),i.data(),GL_STATIC_DRAW);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),nullptr); glEnableVertexAttribArray(0);
    glBindVertexArray(0); return vao&&vbo&&ebo;
}
void AvtoVistaScene::Mesh::draw() const { glBindVertexArray(vao); glDrawElements(GL_TRIANGLES,indexCount,GL_UNSIGNED_INT,nullptr); }
void AvtoVistaScene::Mesh::destroy() {
    if(ebo) glDeleteBuffers(1,&ebo); if(vbo) glDeleteBuffers(1,&vbo); if(vao) glDeleteVertexArrays(1,&vao);
    vao=vbo=ebo=0; indexCount=0;
}
bool AvtoVistaScene::initialize() {
    auto c=cylinderData(),p=plusData(),b=cubeData();
    return cylinder_.create(c.v,c.i)&&plus_.create(p.v,p.i)&&cube_.create(b.v,b.i);
}
void AvtoVistaScene::update(float dt) {
    if(std::isfinite(dt)&&dt>0) angle_=std::fmod(angle_+dt*.45f,6.28318530718f);
}
void AvtoVistaScene::render(GLint modelLoc,GLint colorLoc) const {
    const glm::mat4 turn=glm::rotate(glm::mat4(1),angle_,glm::vec3(0,1,0));
    auto draw=[&](const Mesh& mesh,const glm::mat4& model,const glm::vec3& color) {
        glUniformMatrix4fv(modelLoc,1,GL_FALSE,glm::value_ptr(model));
        glUniform3fv(colorLoc,1,glm::value_ptr(color)); mesh.draw();
    };
    const auto disk=glm::translate(turn,glm::vec3(0,-.08f,0))*glm::scale(glm::mat4(1),glm::vec3(2,.28f,2));
    draw(cylinder_,disk,{.012f,.014f,.018f});
    draw(plus_,glm::translate(turn,glm::vec3(0,.062f,0)),{.95f,.12f,.04f});
    const auto cube=glm::translate(turn,glm::vec3(.82f,.395f,0))*glm::scale(glm::mat4(1),glm::vec3(.65f));
    draw(cube_,cube,{.08f,.28f,.92f});
}
void AvtoVistaScene::destroy() { cube_.destroy(); plus_.destroy(); cylinder_.destroy(); }
