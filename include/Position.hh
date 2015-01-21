#ifndef POSITION_H_
# define POSITION_H_

#include <cmath>
#include <sstream>
#include <atomic>

//thread-safe Position class
template <typename T = int, typename U = double>
class Position
{
public:

  Position(int x = 0, int y = 0):_x(x), _y(y)
  {}

  ~Position() = default;

  Position(const Position & oth) = default;
  Position(Position && oth) = default;

  Position & operator=(const Position & oth) = default;
  Position & operator=(Position && oth) = default;

  inline T x() const
  {
    return this->_x;
  }

  inline void x(const T val)
  {
    this->_x = val;
  }

  inline T y() const
  {
    return this->_y;
  }

  inline void y(const T val)
  {
    this->_y = val;
  }

  void set(const T x, const T y)
  {
    this->x(x);
    this->y(y);
  }

  std::string inline toString() const
  {
    return std::string("(" + std::to_string(this->_x) + ", " + std::to_string(this->_y) +")");
  }

  static inline U distance(Position const & a, Position const & b)
  {
    return std::sqrt(pow(b._x - a._x, 2) + pow(b._y - a._y, 2));
  }

  static inline Position midpoint(Position const & a, Position const & b)
  {
    return (Position((a._x + b._x) / 2, (a._y + b._y) / 2));
  }

  private:

  std::atomic<T> _x;
  std::atomic<T> _y;
};

#endif /* !POSITION_H_ */
