#pragma once

#include<cmath>
#include<initializer_list>
#include<ostream>

#include"ht_logger.h"

namespace hobot{

using TMath = float;

inline constexpr float Abs(const float& v){
  if (v > 0) return v;
  else       return -v;
}

inline constexpr double Abs(const double& v){
  if (v > 0) return v;
  else       return -v;
}

inline constexpr int Abs(const int& v){
  if (v > 0) return v;
  else       return -v;
}


template<class T>
inline constexpr T Epsilon();

template<>
inline constexpr float Epsilon(){return 1e-10;}

template<>
inline constexpr double Epsilon(){return 1e-10;}

template<>
inline constexpr int Epsilon(){return 1;}

template<class T>
inline constexpr bool EpEq(const T& lhs, const T& rhs){
  return Abs(lhs-rhs) < Epsilon<T>();
}

template<int N, class T = TMath>
struct VecStorage{
  T data[N];
};

template<class T>
struct VecStorage<2, T>{
  union{
    T data[2];
    struct{
      union{
        T x, r;
      };
      union{
        T y, g;
      };
    };
  };
};

template<class T>
struct VecStorage<3, T>{
  union{
    T data[3];
    struct{
      union{
        T x, r;
      };
      union{
        T y, g;
      };
      union{
        T z, b;
      };
    };
  };
};

template<class T>
struct VecStorage<4, T>{
  union{
    T data[4];
    struct{
      union{
        T x, r;
      };
      union{
        T y, g;
      };
      union{
        T z, b;
      };
      union{
        T w, a;
      };
    };
  };
};

template<int N, class T = TMath>
struct Vec : public VecStorage<N, T>{
  using Type = T;

  constexpr Vec<N, T>(){
    for (int i = 0; i < N; i++){
      this->data[i] = 0;
    }
  }
  template<class U>
  explicit constexpr Vec<N, T>(const U& v){
    for (int i = 0; i < N; i++){
      this->data[i] = v;
    }
  }
  constexpr Vec<N, T>(T v[N]){
    for (int i = 0; i < N; i++){
      this->data[i] = v[i];
    }
  }

  template<class U1, class U2>
  constexpr Vec<N, T>(const U1& v1, const U2& v2) requires(N == 2){
    this->data[0] = v1;
    this->data[1] = v2;
  }

  template<class U1, class U2, class U3>
  constexpr Vec<N, T>(const U1& v1, const U2& v2, const U3& v3) requires(N == 3){
    this->data[0] = v1;
    this->data[1] = v2;
    this->data[2] = v3;
  }

  template<class U1, class U2, class U3, class U4>
  constexpr Vec<N, T>(const U1& v1, const U2& v2, const U3& v3, const U4& v4) requires(N == 4){
    this->data[0] = v1;
    this->data[1] = v2;
    this->data[2] = v3;
    this->data[3] = v4;
  }

  constexpr Vec<N, T>& operator+=(const Vec<N, T>& rhs){
    for (int i = 0; i < N; i++){
      this->data[i] += rhs.data[i];
    }
    return *this;
  }
  constexpr Vec<N, T>& operator-=(const Vec<N, T>& rhs){
    for (int i = 0; i < N; i++){
      this->data[i] -= rhs.data[i];
    }
    return *this;
  }
  constexpr Vec<N, T>& operator*=(const T& rhs){
    for (int i = 0; i < N; i++){
      this->data[i] *= rhs;
    }
    return *this;
  }
  constexpr Vec<N, T>& operator/=(const T& rhs){
    for (int i = 0; i < N; i++){
      this->data[i] /= rhs;
    }
    return *this;
  }

  constexpr friend Vec<N, T> operator+(const Vec<N, T>& lhs, const Vec<N, T>& rhs){
    Vec<N, T> res;
    for (int i = 0; i < N; i++){
      res.data[i] = lhs.data[i]+rhs.data[i];
    }
    return res;
  }
  constexpr friend Vec<N, T> operator-(const Vec<N, T>& lhs, const Vec<N, T>& rhs){
    Vec<N, T> res;
    for (int i = 0; i < N; i++){
      res.data[i] = lhs.data[i]-rhs.data[i];
    }
    return res;
  }

  constexpr friend Vec<N, T> operator*(const Vec<N, T>& lhs, const T& rhs){
    Vec<N, T> res;
    for (int i = 0; i < N; i++){
      res.data[i] = lhs.data[i]*rhs;
    }
    return res;
  }
  constexpr friend Vec<N, T> operator*(const T& lhs, const Vec<N, T>& rhs) {return rhs*lhs;} //Order

  constexpr friend Vec<N, T> operator/(const Vec<N, T>& lhs, const T& rhs){
    Vec<N, T> res;
    for (int i = 0; i < N; i++){
      res.data[i] = lhs.data[i]/rhs;
    }
    return res;
  }

  constexpr Vec<N, T>(const Vec<N, T>& rhs) = default;
  constexpr Vec<N, T>& operator=(const Vec<N, T>& rhs) = default;
  constexpr Vec<N, T>(Vec<N, T>&& rhs) = default;
  constexpr Vec<N, T>& operator=(Vec<N, T>&& rhs) = default;
  constexpr ~Vec<N, T>() = default;

  //Additionally, deals with narrowing cast problems
  template<class U>
  constexpr Vec<N, T>(const std::initializer_list<U>& rhs){
    HT_LOG_ASSERT(rhs.size()==N, "Attempt to initialize Vec of size ", N, " with ", rhs.size(), " elements!");
    for (int i = 0; i < N; i++){
      this->data[i] = *(rhs.begin()+i);
    }
  }

  constexpr Vec<N, T>(const std::initializer_list<Vec<2, T>>& rhs) requires(N == 4){
    HT_LOG_ASSERT(rhs.size()==2, "Attempt to initialize Vec", N, " with ", rhs.size(), " Vec2's!");
    this->data[0] = (rhs.begin())->x;
    this->data[1] = (rhs.begin())->y;
    this->data[2] = (rhs.begin()+1)->x;
    this->data[3] = (rhs.begin()+1)->y;
  }

  constexpr friend bool operator==(const Vec<N, T>& lhs, const Vec<N, T>& rhs){
    for (int i = 0; i < N; i++){
      if (!EpEq<T>(lhs.data[i], rhs.data[i])) return false;
    }
    return true;
  }

  constexpr Vec<N, T> operator-() const{
    return (-1)*(*this);
  }

  friend std::ostream& operator<<(std::ostream& os, const Vec<N, T>& rhs){
    os << "(";
    for (int i = 0; i < N; i++){
      os << rhs.data[i];
      if (i != N-1) os << ", ";
    }
    os << ")";
    return os;
  }

  constexpr T& operator[](int n){
    return this->data[n];
  }
  constexpr const T& operator[](int n) const{
    return this->data[n];
  }
};

using Vec2 = Vec<2>;
using Vec3 = Vec<3>;
using Vec4 = Vec<4>;

//Square mat
//Column-major!
template<int N, class T = TMath>
struct Mat{
  T data[N*N];

  constexpr Mat<N, T>(){
    for (int i = 0; i < N*N; i++){
      this->data[i] = 0;
    }
  }
  template<class U> //Diagonal
  explicit constexpr Mat<N, T>(const U& d) : Mat<N, T>(){
    for (int i = 0; i < N; i++){
      this->data[i*N+i] = d;
    }
  }

  constexpr Mat<N, T>(T v[N*N]){
    for (int i = 0; i < N*N; i++){
      this->data[i] = v[i];
    }
  }

  constexpr Mat<N, T>& operator*=(const T& rhs){
    for (int i = 0; i < N*N; i++){
      this->data[i] *= rhs;
    }
    return *this;
  }
  constexpr Mat<N, T>& operator/=(const T& rhs){
    for (int i = 0; i < N*N; i++){
      this->data[i] /= rhs;
    }
    return *this;
  }

  //0-indexed
  constexpr Vec<N, T>& operator[](int n){
    return *reinterpret_cast<Vec<N, T>*>((data+(n*N)));
  }

  constexpr const Vec<N, T>& operator[](int n) const{
    return *reinterpret_cast<const Vec<N, T>*>((data+(n*N)));
  }

  constexpr friend Vec<N, T> operator*(const Mat<N, T>& lhs, const Vec<N, T>& rhs){
    Vec<N, T> res;
    for (int i = 0; i < N; i++){
      res += rhs[i]*lhs[i];
    }
    return res;
  }

  constexpr Mat<N, T>& operator*=(const Mat<N, T>& rhs){
    for (int i = 0; i < N; i++){
      this->operator[](i) = rhs*this->operator[](i);
    }
  }

  constexpr friend Mat<N, T> operator*(const Mat<N, T>& lhs, const Mat<N, T>& rhs){
    Mat<N, T> res;
    for (int i = 0; i < N; i++){
      res[i] = rhs[i]*lhs;
    }
    return res;
  }

  constexpr friend Mat<N, T> operator*(const Mat<N, T>& lhs, const T& rhs){
    Mat<N, T> res;
    for (int i = 0; i < N*N; i++){
      res.data[i] = lhs.data[i]*rhs;
    }
    return res;
  }
  constexpr friend Mat<N, T> operator*(const T& lhs, const Mat<N, T>& rhs) {return rhs*lhs;} //Order

  constexpr friend Mat<N, T> operator/(const Mat<N, T>& lhs, const T& rhs){
    Mat<N, T> res;
    for (int i = 0; i < N*N; i++){
      res.data[i] = lhs.data[i]/rhs;
    }
    return res;
  }

  constexpr Mat<N, T>(const Mat<N, T>& rhs) = default;
  constexpr Mat<N, T>& operator=(const Mat<N, T>& rhs) = default;
  constexpr Mat<N, T>(Mat<N, T>&& rhs) = default;
  constexpr Mat<N, T>& operator=(Mat<N, T>&& rhs) = default;
  constexpr ~Mat<N, T>() = default;

  //Additionally, deals with narrowing cast problems
  template<class U>
  constexpr Mat<N, T>(const std::initializer_list<U>& rhs){
    HT_LOG_ASSERT(rhs.size()==N*N, "Mat", N, " is initalized with ", rhs.size(), " values!");
    for (int i = 0; i < N*N; i++){
      this->data[i] = *(rhs.begin()+i);
    }
  }

  constexpr friend bool operator==(const Mat<N, T>& lhs, const Mat<N, T>& rhs){
    for (int i = 0; i < N*N; i++){
      if (!EpEq<T>(lhs.data[i], rhs.data[i])) return false;
    }
    return true;
  }

  constexpr Mat<N, T> operator-() const{
    return (-1)*(*this);
  }

  friend std::ostream& operator<<(std::ostream& os, const Mat<N, T>& rhs){
    os << "[";
    for (int i = 0; i < N*N; i++){
      os << rhs.data[i];
      if (i != N*N-1) os << ", ";
    }
    os << "]";
    return os;
  }
};

using Mat4 = Mat<4>;
using Mat3 = Mat<3>;
using Mat2 = Mat<2>;

template<int N, class T = TMath>
inline constexpr Mat<N, T> Identity(){
  return Mat<N, T>(1);
}

//Projection matrix
template<int N, class T = TMath>
inline constexpr Mat<N, T> Orthographic();

//Projection matrix
//Maps 
template<class T = TMath>
inline constexpr Mat<4, T> Orthographic(const Vec<2, T>& x, const Vec<2, T>& y, const Vec<2, T>& z){
  return {2.0f/(x.y-x.x), 0, 0, 0, 0, 2.0f/(y.y-y.x), 0, 0, 0, 0, -2.0f/(z.y-z.x), 0, -(x.x+x.y)/(x.y-x.x), -(y.x+y.y)/(y.y-y.x), -(z.x+z.y)/(z.y-z.x), 1};
  //OpenGL by default is right-handed
  //This turns the actual viewport left-handed
  //TODO: right?
}

template<int N, class T = TMath>
inline constexpr Mat<N, T> Perspective();

//Projects
template<class T = TMath>
inline constexpr Mat<4, T> Perspective(const T& fov, const T& aspectRatio, const Vec<2, T>& z){
  //TODO
}

template<int N, class T = TMath>
inline constexpr Mat<N, T> Translation();

template<class T = TMath>
inline constexpr Mat<4, T> Translation(const Vec3& vec){
  //TODO
}

template<int N, class T = TMath>
inline constexpr Mat<N, T> Rotation();

template<class T = TMath>
inline constexpr Mat<4, T> Rotation(const T& angle, const Vec<3, T>& axis){
  //TODO
}

template<class T = TMath>
inline constexpr Mat<2, T> Rotation(const T& angle){
  return {std::cos(angle), std::sin(angle), -std::sin(angle), std::cos(angle)};
}

template<int N, class T = TMath>
inline constexpr Mat<N, T> Dilation();

template<class T = TMath>
inline constexpr Mat<4, T> Dilation(const Vec<3, T>& scale){
  //TODO
}

template<int N, class T = TMath>
inline constexpr Vec<N, T> Translate();

template<class T = TMath>
inline constexpr Vec<4, T> Translate(const Vec<3, T>& translation, const Vec<4, T>& vec){
  return Translation(translation)*vec;
}

template<int N, class T = TMath>
inline constexpr Vec<N, T> Rotate();

template<class T = TMath>
inline constexpr Vec<4, T> Rotate(const T& angle, const Vec<3, T>& axis, const Vec<4, T>& vec){
  return Rotation(angle, axis)*vec;
}

template<class T = TMath>
inline constexpr Vec<2, T> Rotate(const T& angle, const Vec<2, T>& vec){
  return Rotation(angle)*vec;
}

template<int N, class T = TMath>
inline constexpr Vec<N, T> Dilate();

template<class T = TMath>
inline constexpr Vec<4, T> Dilate(const Vec<3, T>& scale, const Vec<4, T>& vec){
  return Dilation(scale)*vec;
}

//PI
template<class T>
T PI();
template<>
inline constexpr float PI<float>(){
  return 0x1.921fb6p+1f;
}
template<>
inline constexpr double PI<double>(){
  return 0x1.921fb54442d18p+1;
}

template<class T>
inline constexpr T Exp(T v){
  return std::exp(v);
}

template<class T, class U>
inline constexpr T Lerp(const T& v1, const T& v2, const U& v3){
  return v2*v3 + v1*(1-v3);
}

template<int N, class T>
inline constexpr T Abs(const Vec<N, T>& v){
  T sum = 0;

  for (int i = 0; i < N; i++){
    sum += v.data[i]*v.data[i];
  }

  return std::sqrt(sum);
}

template<int N, class T>
inline constexpr Vec<N, T> Norm(const Vec<N, T>& v){
  return v/Abs(v);
}

}
