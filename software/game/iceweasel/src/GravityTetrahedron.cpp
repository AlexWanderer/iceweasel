#include "iceweasel/GravityTetrahedron.h"
#include "iceweasel/GravityPoint.h"
#include "iceweasel/Math.h"

#include <Urho3D/Graphics/DebugRenderer.h>
#include <Urho3D/Math/Matrix2.h>

using namespace Urho3D;

// ----------------------------------------------------------------------------
GravityTetrahedron::GravityTetrahedron(const GravityPoint& p0,
                                       const GravityPoint& p1,
                                       const GravityPoint& p2,
                                       const GravityPoint& p3)
{
    vertex_[0] = p0;
    vertex_[1] = p1;
    vertex_[2] = p2;
    vertex_[3] = p3;

    transform_ = CalculateBarycentricTransformationMatrix();
}

// ----------------------------------------------------------------------------
bool GravityTetrahedron::PointLiesInside(const Vector4& bary) const
{
    return (
        bary.x_ >= 0.0f &&
        bary.y_ >= 0.0f &&
        bary.z_ >= 0.0f &&
        bary.w_ >= 0.0f
    );
}

// ----------------------------------------------------------------------------
Vector4 GravityTetrahedron::TransformToBarycentric(const Vector3& cartesian) const
{
    return transform_ * Vector4(cartesian, 1.0f);
}

// ----------------------------------------------------------------------------
Vector3 GravityTetrahedron::TransformToCartesian(const Vector4& barycentric) const
{
    return barycentric.x_ * vertex_[0].position_ +
            barycentric.y_ * vertex_[1].position_ +
            barycentric.z_ * vertex_[2].position_ +
            barycentric.w_ * vertex_[3].position_;
}

// ----------------------------------------------------------------------------
Vector3 GravityTetrahedron::GetVertexPosition(unsigned char vertexID) const
{
    assert(vertexID < 4);
    return vertex_[vertexID].position_;
}

// ----------------------------------------------------------------------------
Vector3 GravityTetrahedron::InterpolateGravity(const Vector4& barycentric) const
{
    return (
        vertex_[0].direction_ * barycentric.x_ +
        vertex_[1].direction_ * barycentric.y_ +
        vertex_[2].direction_ * barycentric.z_ +
        vertex_[3].direction_ * barycentric.w_
    ).Normalized() * (
        vertex_[0].forceFactor_ * barycentric.x_ +
        vertex_[1].forceFactor_ * barycentric.y_ +
        vertex_[2].forceFactor_ * barycentric.z_ +
        vertex_[3].forceFactor_ * barycentric.w_
    );
}

// ----------------------------------------------------------------------------
Matrix4 GravityTetrahedron::CalculateBarycentricTransformationMatrix() const
{
    // Barycentric transformation matrix
    // https://en.wikipedia.org/wiki/Barycentric_coordinate_system#Conversion_between_barycentric_and_Cartesian_coordinates
    return Matrix4(
        vertex_[0].position_.x_, vertex_[1].position_.x_, vertex_[2].position_.x_, vertex_[3].position_.x_,
        vertex_[0].position_.y_, vertex_[1].position_.y_, vertex_[2].position_.y_, vertex_[3].position_.y_,
        vertex_[0].position_.z_, vertex_[1].position_.z_, vertex_[2].position_.z_, vertex_[3].position_.z_,
        1, 1, 1, 1
    ).Inverse();
}

// ----------------------------------------------------------------------------
void GravityTetrahedron::DrawDebugGeometry(DebugRenderer* debug, bool depthTest, const Color& color)
{
    for(unsigned i = 0; i != 4; ++i)
    {
        for(unsigned j = i + 1; j != 4; ++j)
            debug->AddLine(
                Vector3(vertex_[i].position_.x_, vertex_[i].position_.y_, vertex_[i].position_.z_),
                Vector3(vertex_[j].position_.x_, vertex_[j].position_.y_, vertex_[j].position_.z_),
                color, depthTest
            );
    }

    //debug->AddSphere(Sphere(sphereCenter_, (vertex_[0] - sphereCenter_).Length()), Color::GRAY, depthTest);
}
