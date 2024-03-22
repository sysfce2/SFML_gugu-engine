////////////////////////////////////////////////////////////////
// Header

#include "Gugu/Common.h"
#include "Gugu/Math/Vector2.h"

////////////////////////////////////////////////////////////////
// Includes

#include "Gugu/Math/MathUtility.h"
#include "Gugu/System/SystemUtility.h"

////////////////////////////////////////////////////////////////
// File Implementation

namespace gugu {

std::string ToString(const Vector2f& value)
{
    std::ostringstream os;
    os << "(" << ToString(value.x) << ", " << ToString(value.y) << ")";
    return os.str();
}

bool ApproxEqual(const Vector2f& left, const Vector2f& right, float epsilon)
{
    return gugu::ApproxEqual(left.x, right.x, epsilon) && gugu::ApproxEqual(left.y, right.y, epsilon);
}

bool ApproxIsInBounds(const sf::Vector2f& value, const sf::Vector2f& min, const sf::Vector2f& max, float epsilon)
{
    return ApproxSuperiorOrEqual(value.x, min.x, epsilon) && ApproxInferiorOrEqual(value.x, max.x, epsilon)
        && ApproxSuperiorOrEqual(value.y, min.y, epsilon) && ApproxInferiorOrEqual(value.y, max.y, epsilon);
}

bool ApproxDistanceCheck(const sf::Vector2f& left, const sf::Vector2f& right, float distance, float epsilon)
{
    if (ApproxInferiorOrEqual(std::abs(left.x - right.x), distance, epsilon) && ApproxInferiorOrEqual(std::abs(left.y - right.y), distance, epsilon))
    {
        return ApproxInferiorOrEqual(LengthSquare(right - left), Power2(distance), epsilon);
    }

    return false;
}

}   // namespace gugu
