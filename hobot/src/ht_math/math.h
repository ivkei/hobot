#pragma once

#include<cmath>
#include<initializer_list>
#include<ostream>

#include"ht_logger.h"

namespace hobot{

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

template<int N, class T = float>
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

template<int N, class T = float>
struct Vec : public VecStorage<N>{
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
};

using Vec2 = Vec<2>;
using Vec3 = Vec<3>;
using Vec4 = Vec<4>;

//Mat support
#include"glm/glm.hpp"
using Mat4 = glm::mat4;
using Mat3 = glm::mat3;
using Mat2 = glm::mat2;

inline constexpr Vec2 operator*(const Mat2& lhs, const Vec2& rhs){
  return rhs.x*(Vec2(lhs[0][0], lhs[0][1])) + rhs.y*(Vec2(lhs[1][0], lhs[1][1]));
}

//TODO: own structs, with vec multiplication

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
