#pragma once

////////////////////////////////////////////////////////////////
// Includes

#include "Gugu/Math/Vector2.h"

////////////////////////////////////////////////////////////////
// File Declarations

namespace gugu {

class UDim
{
public:

    float relative;
    float absolute;

    UDim();
    UDim(const UDim& _kRight);
    UDim(float _fRel, float _fAbs);

    UDim& operator+=(float _fAbs);
    const UDim operator+(float _fAbs) const;

    float GetComputedDimension(float _fReference) const;

    static const UDim ZERO;
    static const UDim HALF;
    static const UDim FULL;
};

class UDim2
{
public:

    UDim x;
    UDim y;

    UDim2();
    UDim2(const UDim2& _kRight);
    UDim2(const UDim& _X, const UDim& _Y);
    UDim2(float _fRelX, float _fRelY);
    UDim2(float _fRelX, float _fAbsX, float _fRelY, float _fAbsY);  //TODO: I would prefer to use xrel/yrel/xabs/yabs, but I would probably break a lot of use cases (I could force a ctor with (vector2, vector2) ?).

    bool operator==(const UDim2& right);

    UDim2& operator+=(const Vector2f& _kAbs);
    const UDim2 operator+(const Vector2f& _kAbs) const;

    Vector2f GetComputedDimension(float _fReferenceX, float _fReferenceY) const;
    Vector2f GetComputedDimension(const Vector2f& _kReferenceSize) const;
    Vector2f GetPixelAlignedComputedDimension(float _fReferenceX, float _fReferenceY) const;
    Vector2f GetPixelAlignedComputedDimension(const Vector2f& _kReferenceSize) const;

    static const UDim2 POSITION_TOP_LEFT;
    static const UDim2 POSITION_TOP_CENTER;
    static const UDim2 POSITION_TOP_RIGHT;
    static const UDim2 POSITION_CENTER_LEFT;
    static const UDim2 POSITION_CENTER;
    static const UDim2 POSITION_CENTER_RIGHT;
    static const UDim2 POSITION_BOTTOM_LEFT;
    static const UDim2 POSITION_BOTTOM_CENTER;
    static const UDim2 POSITION_BOTTOM_RIGHT;

    static const UDim2 SIZE_ZERO;
    static const UDim2 SIZE_SQUARE_QUARTER;
    static const UDim2 SIZE_HORIZONTAL_HALF;
    static const UDim2 SIZE_VERTICAL_HALF;
    static const UDim2 SIZE_FULL;

    static const UDim2 ZERO;
};

}   // namespace gugu
