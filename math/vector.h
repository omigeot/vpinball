#pragma once

// 2D vector
class Vertex2D
{
public:
   float x;
   float y;

   inline Vertex2D() {}
   inline Vertex2D(const float _x, const float _y) : x(_x), y(_y) {}

   inline Vertex2D operator+ (const Vertex2D& v) const
   {
       return Vertex2D(x + v.x, y + v.y);
   }
   inline Vertex2D operator- (const Vertex2D& v) const
   {
       return Vertex2D(x - v.x, y - v.y);
   }
   inline Vertex2D operator- () const
   {
       return Vertex2D(-x, -y);
   }

   inline Vertex2D& operator+= (const Vertex2D& v)
   {
       x += v.x;
       y += v.y;
       return *this;
   }
   inline Vertex2D& operator-= (const Vertex2D& v)
   {
       x -= v.x;
       y -= v.y;
       return *this;
   }

   inline Vertex2D operator* (const float s) const
   {
       return Vertex2D(s*x, s*y);
   }
   inline friend Vertex2D operator* (const float s, const Vertex2D& v)
   {
       return Vertex2D(s*v.x, s*v.y);
   }
   inline Vertex2D operator/ (const float s) const
   {
       const float invs = 1.0f/s;
       return Vertex2D(x*invs, y*invs);
   }

   inline Vertex2D& operator*= (const float s)
   {
       x *= s;
       y *= s;
       return *this;
   }
   inline Vertex2D& operator/= (const float s)
   {
       const float invs = 1.0f/s;
       x *= invs;
       y *= invs;
       return *this;
   }

   inline float Dot(const Vertex2D &pv) const
   {
      return x*pv.x + y*pv.y;
   }

   inline float LengthSquared() const
   {
      return x*x + y*y;
   }

   inline float Length() const
   {
      return sqrtf(x*x + y*y);
   }

   inline void Normalize()
   {
      const float oneoverlength = 1.0f / Length();
      x *= oneoverlength;
      y *= oneoverlength;
   }

   inline void NormalizeSafe()
   {
       if (!IsZero())
           Normalize();
   }

   inline bool IsZero() const
   {
       return x == 0.f && y == 0.f;
   }
};


// 3D vector
class Vertex3Ds
{
public:
   float x,y,z;

   inline Vertex3Ds() {}
   inline Vertex3Ds(const float _x, const float _y, const float _z) : x(_x), y(_y), z(_z) {}

   inline void Set(const float a, const float b, const float c) {x=a; y=b; z=c;}

   inline Vertex3Ds operator+ (const Vertex3Ds& v) const
   {
       return Vertex3Ds(x + v.x, y + v.y, z + v.z);
   }
   inline Vertex3Ds operator- (const Vertex3Ds& v) const
   {
       return Vertex3Ds(x - v.x, y - v.y, z - v.z);
   }
   inline Vertex3Ds operator- () const
   {
       return Vertex3Ds(-x, -y, -z);
   }

   inline Vertex3Ds& operator+= (const Vertex3Ds& v)
   {
       x += v.x;
       y += v.y;
       z += v.z;
       return *this;
   }
   inline Vertex3Ds& operator-= (const Vertex3Ds& v)
   {
       x -= v.x;
       y -= v.y;
       z -= v.z;
       return *this;
   }

   inline Vertex3Ds operator* (const float s) const
   {
       return Vertex3Ds(s*x, s*y, s*z);
   }
   inline friend Vertex3Ds operator* (const float s, const Vertex3Ds& v)
   {
       return Vertex3Ds(s*v.x, s*v.y, s*v.z);
   }
   inline Vertex3Ds operator/ (const float s) const
   {
       const float invs = 1.0f/s;
       return Vertex3Ds(x*invs, y*invs, z*invs);
   }

   inline Vertex3Ds& operator*= (const float s)
   {
       x *= s;
       y *= s;
       z *= s;
       return *this;
   }
   inline Vertex3Ds& operator/= (const float s)
   {
       const float invs = 1.0f/s;
       x *= invs;
       y *= invs;
       z *= invs;
       return *this;
   }

   inline void Normalize()
   {
      const float oneoverlength = 1.0f/sqrtf(x*x + y*y + z*z);
      x *= oneoverlength;
      y *= oneoverlength;
      z *= oneoverlength;
   }
   inline void Normalize(const float scalar)
   {
      const float oneoverlength = scalar/sqrtf(x*x + y*y + z*z);
      x *= oneoverlength;
      y *= oneoverlength;
      z *= oneoverlength;
   }

   inline void NormalizeSafe()
   {
       if (!IsZero())
           Normalize();
   }

   inline float Dot(const Vertex3Ds &pv) const
   {
      return x*pv.x + y*pv.y + z*pv.z;
   }
   inline float Dot(const Vertex3D &pv) const
   {
      return x*pv.x + y*pv.y + z*pv.z;
   }

   inline float LengthSquared() const
   {
      return x*x + y*y + z*z;
   }

   inline float Length() const
   {
      return sqrtf(x*x + y*y + z*z);
   }

   inline bool IsZero() const
   {
       return x == 0.f && y == 0.f && z == 0.f;
   }

   // access the x/y components as a 2D vector
   inline Vertex2D& xy()               { return *(reinterpret_cast<Vertex2D*>(&x)); }
   inline const Vertex2D& xy() const   { return *(reinterpret_cast<const Vertex2D*>(&x)); }
};


inline Vertex3Ds CrossProduct(const Vertex3Ds &pv1, const Vertex3Ds &pv2)
{
   return Vertex3Ds(pv1.y * pv2.z - pv1.z * pv2.y,
      pv1.z * pv2.x - pv1.x * pv2.z,
      pv1.x * pv2.y - pv1.y * pv2.x);
}

////////////////////////////////////////////////////////////////////////////////

inline Vertex3Ds GetRotatedAxis( const float angle, const Vertex3Ds &axis, const Vertex3Ds &temp)
{   
   Vertex3Ds u=axis;
   u.Normalize();

   const float sinAngle=sinf((float)(M_PI/180.0)*angle);
   const float cosAngle=cosf((float)(M_PI/180.0)*angle);
   const float oneMinusCosAngle=1.0f-cosAngle;

   Vertex3Ds rotMatrixRow0, rotMatrixRow1, rotMatrixRow2;

   rotMatrixRow0.x=u.x*u.x + cosAngle*(1.f-u.x*u.x);
   rotMatrixRow0.y=u.x*u.y*oneMinusCosAngle - sinAngle*u.z;
   rotMatrixRow0.z=u.x*u.z*oneMinusCosAngle + sinAngle*u.y;

   rotMatrixRow1.x=u.x*u.y*oneMinusCosAngle + sinAngle*u.z;
   rotMatrixRow1.y=u.y*u.y + cosAngle*(1.f-u.y*u.y);
   rotMatrixRow1.z=u.y*u.z*oneMinusCosAngle - sinAngle*u.x;

   rotMatrixRow2.x=u.x*u.z*oneMinusCosAngle - sinAngle*u.y;
   rotMatrixRow2.y=u.y*u.z*oneMinusCosAngle + sinAngle*u.x;
   rotMatrixRow2.z=u.z*u.z + cosAngle*(1.f-u.z*u.z);

   return Vertex3Ds(temp.Dot(rotMatrixRow0), temp.Dot(rotMatrixRow1), temp.Dot(rotMatrixRow2));
}


inline void RotateAround(const Vertex3Ds &pvAxis, Vertex3D_NoTex * const pvPoint, const int count, const float angle)
{
    const float rsin = sinf(angle);
    const float rcos = cosf(angle);

    // Matrix for rotating around an arbitrary vector

    float matrix[3][3];
    matrix[0][0] = pvAxis.x*pvAxis.x + rcos*(1.0f-pvAxis.x*pvAxis.x);
    matrix[1][0] = pvAxis.x*pvAxis.y*(1.0f-rcos) - pvAxis.z*rsin;
    matrix[2][0] = pvAxis.z*pvAxis.x*(1.0f-rcos) + pvAxis.y*rsin;

    matrix[0][1] = pvAxis.x*pvAxis.y*(1.0f-rcos) + pvAxis.z*rsin;
    matrix[1][1] = pvAxis.y*pvAxis.y + rcos*(1.0f-pvAxis.y*pvAxis.y);
    matrix[2][1] = pvAxis.y*pvAxis.z*(1.0f-rcos) - pvAxis.x*rsin;

    matrix[0][2] = pvAxis.z*pvAxis.x*(1.0f-rcos) - pvAxis.y*rsin;
    matrix[1][2] = pvAxis.y*pvAxis.z*(1.0f-rcos) + pvAxis.x*rsin;
    matrix[2][2] = pvAxis.z*pvAxis.z + rcos*(1.0f-pvAxis.z*pvAxis.z);

    for (int i=0; i<count; ++i)
    {
        const float result[3] = {
            matrix[0][0]*pvPoint[i].x + matrix[0][1]*pvPoint[i].y + matrix[0][2]*pvPoint[i].z,
            matrix[1][0]*pvPoint[i].x + matrix[1][1]*pvPoint[i].y + matrix[1][2]*pvPoint[i].z,
            matrix[2][0]*pvPoint[i].x + matrix[2][1]*pvPoint[i].y + matrix[2][2]*pvPoint[i].z};

            pvPoint[i].x = result[0];
            pvPoint[i].y = result[1];
            pvPoint[i].z = result[2];

            const float resultn[3] = {
                matrix[0][0]*pvPoint[i].nx + matrix[0][1]*pvPoint[i].ny + matrix[0][2]*pvPoint[i].nz,
                matrix[1][0]*pvPoint[i].nx + matrix[1][1]*pvPoint[i].ny + matrix[1][2]*pvPoint[i].nz,
                matrix[2][0]*pvPoint[i].nx + matrix[2][1]*pvPoint[i].ny + matrix[2][2]*pvPoint[i].nz};

                pvPoint[i].nx = resultn[0];
                pvPoint[i].ny = resultn[1];
                pvPoint[i].nz = resultn[2];
    }
}

inline void RotateAround(const Vertex3Ds &pvAxis, Vertex3D_NoTex2 * const pvPoint, const int count, const float angle)
{
    const float rsin = sinf(angle);
    const float rcos = cosf(angle);

    // Matrix for rotating around an arbitrary vector

    float matrix[3][3];
    matrix[0][0] = pvAxis.x*pvAxis.x + rcos*(1.0f-pvAxis.x*pvAxis.x);
    matrix[1][0] = pvAxis.x*pvAxis.y*(1.0f-rcos) - pvAxis.z*rsin;
    matrix[2][0] = pvAxis.z*pvAxis.x*(1.0f-rcos) + pvAxis.y*rsin;

    matrix[0][1] = pvAxis.x*pvAxis.y*(1.0f-rcos) + pvAxis.z*rsin;
    matrix[1][1] = pvAxis.y*pvAxis.y + rcos*(1.0f-pvAxis.y*pvAxis.y);
    matrix[2][1] = pvAxis.y*pvAxis.z*(1.0f-rcos) - pvAxis.x*rsin;

    matrix[0][2] = pvAxis.z*pvAxis.x*(1.0f-rcos) - pvAxis.y*rsin;
    matrix[1][2] = pvAxis.y*pvAxis.z*(1.0f-rcos) + pvAxis.x*rsin;
    matrix[2][2] = pvAxis.z*pvAxis.z + rcos*(1.0f-pvAxis.z*pvAxis.z);

    for (int i=0; i<count; ++i)
    {
        const float result[3] = {
            matrix[0][0]*pvPoint[i].x + matrix[0][1]*pvPoint[i].y + matrix[0][2]*pvPoint[i].z,
            matrix[1][0]*pvPoint[i].x + matrix[1][1]*pvPoint[i].y + matrix[1][2]*pvPoint[i].z,
            matrix[2][0]*pvPoint[i].x + matrix[2][1]*pvPoint[i].y + matrix[2][2]*pvPoint[i].z};

            pvPoint[i].x = result[0];
            pvPoint[i].y = result[1];
            pvPoint[i].z = result[2];

            const float resultn[3] = {
                matrix[0][0]*pvPoint[i].nx + matrix[0][1]*pvPoint[i].ny + matrix[0][2]*pvPoint[i].nz,
                matrix[1][0]*pvPoint[i].nx + matrix[1][1]*pvPoint[i].ny + matrix[1][2]*pvPoint[i].nz,
                matrix[2][0]*pvPoint[i].nx + matrix[2][1]*pvPoint[i].ny + matrix[2][2]*pvPoint[i].nz};

                pvPoint[i].nx = resultn[0];
                pvPoint[i].ny = resultn[1];
                pvPoint[i].nz = resultn[2];
    }
}

inline void RotateAround(const Vertex3Ds &pvAxis, Vertex3Ds * const pvPoint, const int count, const float angle)
{
   const float rsin = sinf(angle);
   const float rcos = cosf(angle);

   // Matrix for rotating around an arbitrary vector

   float matrix[3][3];
   matrix[0][0] = pvAxis.x*pvAxis.x + rcos*(1.0f-pvAxis.x*pvAxis.x);
   matrix[1][0] = pvAxis.x*pvAxis.y*(1.0f-rcos) - pvAxis.z*rsin;
   matrix[2][0] = pvAxis.z*pvAxis.x*(1.0f-rcos) + pvAxis.y*rsin;

   matrix[0][1] = pvAxis.x*pvAxis.y*(1.0f-rcos) + pvAxis.z*rsin;
   matrix[1][1] = pvAxis.y*pvAxis.y + rcos*(1.0f-pvAxis.y*pvAxis.y);
   matrix[2][1] = pvAxis.y*pvAxis.z*(1.0f-rcos) - pvAxis.x*rsin;

   matrix[0][2] = pvAxis.z*pvAxis.x*(1.0f-rcos) - pvAxis.y*rsin;
   matrix[1][2] = pvAxis.y*pvAxis.z*(1.0f-rcos) + pvAxis.x*rsin;
   matrix[2][2] = pvAxis.z*pvAxis.z + rcos*(1.0f-pvAxis.z*pvAxis.z);

   for (int i=0; i<count; ++i)
   {
      const float result[3] = {
         matrix[0][0]*pvPoint[i].x + matrix[0][1]*pvPoint[i].y + matrix[0][2]*pvPoint[i].z,
         matrix[1][0]*pvPoint[i].x + matrix[1][1]*pvPoint[i].y + matrix[1][2]*pvPoint[i].z,
         matrix[2][0]*pvPoint[i].x + matrix[2][1]*pvPoint[i].y + matrix[2][2]*pvPoint[i].z};

      pvPoint[i].x = result[0];
      pvPoint[i].y = result[1];
      pvPoint[i].z = result[2];
   }
}

inline Vertex3Ds RotateAround(const Vertex3Ds &pvAxis, const Vertex2D &pvPoint, const float angle)
{
   const float rsin = sinf(angle);
   const float rcos = cosf(angle);

   // Matrix for rotating around an arbitrary vector

   float matrix[3][2];
   matrix[0][0] = pvAxis.x*pvAxis.x + rcos*(1.0f-pvAxis.x*pvAxis.x);
   matrix[1][0] = pvAxis.x*pvAxis.y*(1.0f-rcos) - pvAxis.z*rsin;
   matrix[2][0] = pvAxis.z*pvAxis.x*(1.0f-rcos) + pvAxis.y*rsin;

   matrix[0][1] = pvAxis.x*pvAxis.y*(1.0f-rcos) + pvAxis.z*rsin;
   matrix[1][1] = pvAxis.y*pvAxis.y + rcos*(1.0f-pvAxis.y*pvAxis.y);
   matrix[2][1] = pvAxis.y*pvAxis.z*(1.0f-rcos) - pvAxis.x*rsin;

   return Vertex3Ds(matrix[0][0]*pvPoint.x + matrix[0][1]*pvPoint.y,
      matrix[1][0]*pvPoint.x + matrix[1][1]*pvPoint.y,
      matrix[2][0]*pvPoint.x + matrix[2][1]*pvPoint.y);
}
