#ifndef TRACKER_H_
# define TRACKER_H_

// thread safety
#include <atomic>

//openCV headers
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "Position.hh"

class Tracker
{
protected:

  cv::VideoCapture	_cap;

  Position<>		_pos; //center of the found object
  std::atomic<double>	_area; // area of the found object
  int			_iLastX;
  int			_iLastY;

  cv::Mat		_imgLines;

  int			_iLowH;
  int			_iHighH;
  int			_iLowS;
  int			_iHighS;
  int			_iLowV;
  int			_iHighV;

public:

			explicit Tracker(std::string const & color = "red");
			~Tracker() = default;

			Tracker(const Tracker &) = delete ;
			Tracker(Tracker &&) = delete ;

  Tracker		&operator=(const Tracker &) = delete;
  Tracker		&operator=(Tracker &&) = delete;

  Position<> const &	position() const {return this->_pos;}
  double		area() const {return this->_area;}

  void			setColor(std::string const &);

  ///////DO THE IMG PROC
  void			run();
  void			runShape();

private:

  void			init_config(std::string const &);

  void			find_object(cv::Mat const & imgThresholded);

  void			thresholder(const cv::Mat&, cv::Mat&) const;

  void          setLabel(cv::Mat& im, const std::string & label, std::vector<cv::Point>& contour);

};

#endif /* !TRACKER_H_ */
