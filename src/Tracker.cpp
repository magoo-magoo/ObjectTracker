#include <iostream>
#include <stdexcept>
#include <thread>
#include <cmath>

#include "Tracker.hh"


void Tracker::setColor(std::string const & color)
{
  if (color == "green")
    {
      this->_iLowH = 50;
      this->_iHighH = 70;
      return ;
    }
  if (color == "blue")
    {
      this->_iLowH = 110;
      this->_iHighH = 130;
      return ;
    }
  if (color == "red")
    return ;

  throw std::logic_error("Color given to tracker is unsupported");
}

void Tracker::init_config(std::string const & color)
{
  this->_iLowH = 170;
  this->_iHighH = 179;
  this->_iLowS = 150;
  this->_iHighS = 255;
  this->_iLowV = 60;
  this->_iHighV = 255;

  this->setColor(color);

  // "Control" window with trackbar
  cv::namedWindow("Control", CV_WINDOW_AUTOSIZE);
  cv::createTrackbar("LowH", "Control", &this->_iLowH, 179); //Hue (0 - 179)
  cv::createTrackbar("HighH", "Control", &this->_iHighH, 179);

  cv::createTrackbar("LowS", "Control", &this->_iLowS, 255); //Saturation (0 - 255)
  cv::createTrackbar("HighS", "Control", &this->_iHighS, 255);

  cv::createTrackbar("LowV", "Control", &this->_iLowV, 255);//Value (0 - 255)
  cv::createTrackbar("HighV", "Control", &this->_iHighV, 255);
}

Tracker::Tracker(std::string const & color)
  :_cap(0), //capture the video from webcam
   _pos(-1, -1), _iLastX(-1), _iLastY(-1), _imgLines()
{
  if (!this->_cap.isOpened())  // if not success, exit program
    {
      throw std::runtime_error("Cannot open the web cam");
    }

  //Set config
  this->init_config(color);

  //Capture a temporary image from the camera
  cv::Mat imgTmp;
  this->_cap.read(imgTmp);

  //Create a black image with the size as the camera output
  this->_imgLines = cv::Mat::zeros(imgTmp.size(), CV_8UC3);;
}

void	Tracker::find_object(cv::Mat const & imgThresholded)
{
  //Calculate the moments of the thresholded image
  cv::Moments oMoments = moments(imgThresholded);

  double dM01 = oMoments.m01;
  double dM10 = oMoments.m10;
  this->_area = oMoments.m00;

  // if the area <= 10000 object is not distinct from noise
  if (this->_area > 10000)
    {
      //calculate the position of the ball
      this->_pos.x(static_cast<int>(dM10 / this->_area));
      this->_pos.y(static_cast<int>(dM01 / this->_area));

      if (this->_iLastX >= 0 && this->_iLastY >= 0
	  && this->_pos.x() >= 0 && this->_pos.y() >= 0)
	{
	  //Draw a red line from the previous point to the current point
	  cv::line(this->_imgLines, cv::Point(this->_pos.x(), this->_pos.y()),
		   cv::Point(this->_iLastX, this->_iLastY), cv::Scalar(0,0,255), 2);
	}

      this->_iLastX = this->_pos.x();
      this->_iLastY = this->_pos.y();
      return ;
    }

  this->_pos.x(-1);
  this->_pos.y(-1);
}

void Tracker::run()
{
  while (true)
    {
      cv::Mat imgOriginal;

      if (!this->_cap.read(imgOriginal)) //if not success, break loop
        {
	  throw std::runtime_error("Cannot read a frame from video stream");
        }

      cv::Mat imgHSV;

      //Convert the captured frame from BGR to HSV
      cv::cvtColor(imgOriginal, imgHSV, cv::COLOR_BGR2HSV);

      cv::Mat imgThresholded;

      //Threshold the image
      cv::inRange(imgHSV, cv::Scalar(this->_iLowH, this->_iLowS, this->_iLowV),
		  cv::Scalar(this->_iHighH, this->_iHighS, this->_iHighV), imgThresholded);

      //morphological opening (removes small objects from the foreground)
      cv::erode(imgThresholded, imgThresholded,
		getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)));
      cv::dilate( imgThresholded, imgThresholded,
		  getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)));

      //morphological closing (removes small holes from the foreground)
      cv::dilate( imgThresholded, imgThresholded,
		  getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)));
      cv::erode(imgThresholded, imgThresholded,
		getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)));

      //find the object in the thresholded image
      this->find_object(imgThresholded);

      cv::imshow("Thresholded Image", imgThresholded); //show the thresholded image

      imgOriginal = imgOriginal + this->_imgLines;
      imshow("Original", imgOriginal); //show the original image

      if (cv::waitKey(30) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
	{
	  std::cout << "esc key is pressed by user" << std::endl;
	  break;
	}
    }
}

void Tracker::thresholder(const cv::Mat & imgOriginal, cv::Mat & imgThresholded) const
{
  cv::Mat imgHSV;

  //Convert the captured frame from BGR to HSV
  cv::cvtColor(imgOriginal, imgHSV, cv::COLOR_BGR2HSV);

  //Threshold the image
  cv::inRange(imgHSV, cv::Scalar(this->_iLowH, this->_iLowS, this->_iLowV),
	      cv::Scalar(this->_iHighH, this->_iHighS, this->_iHighV), imgThresholded);

  //morphological opening (removes small objects from the foreground)
  cv::erode(imgThresholded, imgThresholded,
	    getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)));
  cv::dilate( imgThresholded, imgThresholded,
	      getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)));

  //morphological closing (removes small holes from the foreground)
  cv::dilate( imgThresholded, imgThresholded,
	      getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)));
  cv::erode(imgThresholded, imgThresholded,
	    getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)));
}


void Tracker::setLabel(cv::Mat& im, const std::string & label, std::vector<cv::Point>& contour)
{
  int fontface = cv::FONT_HERSHEY_SIMPLEX;
  double scale = 0.4;
  int thickness = 1;
  int baseline = 0;

  cv::Size text = cv::getTextSize(label, fontface, scale, thickness, &baseline);
  cv::Rect r = cv::boundingRect(contour);

  cv::Point pt(r.x + ((r.width - text.width) / 2), r.y + ((r.height + text.height) / 2));
  cv::rectangle(im, pt + cv::Point(0, baseline), pt + cv::Point(text.width, -text.height), CV_RGB(255,255,255), CV_FILLED);
  cv::putText(im, label, pt, fontface, scale, CV_RGB(0,0,0), thickness, 8);
}

/*
void Tracker::runShape()
{
  while (true)
    {
      cv::Mat imgOriginal;

      // read a new frame from video
      bool bSuccess = this->_cap.read(imgOriginal);

      if (!bSuccess) //if not success, break loop
        {
	  throw std::runtime_error("Cannot read a frame from video stream");
        }


      cv::Mat imgThresholded;
      //this->thresholder(imgOriginal, imgThresholded);

	// // Convert to grayscale
	// cv::Mat gray;
      //      cv::cvtColor(imgThresholded, gray, CV_HSV2GRAY);

	// Use Canny instead of threshold to catch squares with gradient shading
	cv::Mat bw;
	cv::Canny(imgThresholded, bw, 0, 50, 5);

	// Find contours
	std::vector<std::vector<cv::Point> > contours;
	cv::findContours(bw.clone(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

	std::vector<cv::Point> approx;
	cv::Mat dst = imgOriginal.clone();

	for (int i = 0; i < static_cast<int>(contours.size()); i++)
	{
		// Approximate contour with accuracy proportional
		// to the contour perimeter
		cv::approxPolyDP(cv::Mat(contours[i]), approx, cv::arcLength(cv::Mat(contours[i]), true)*0.02, true);

		// Skip small or non-convex objects
		if (std::fabs(cv::contourArea(contours[i])) < 100 || !cv::isContourConvex(approx))
			continue;

		if (approx.size() == 3)
		{
			setLabel(dst, "TRI", contours[i]);    // Triangles
		}
		else if (approx.size() >= 4 && approx.size() <= 6)
		{
			// Number of vertices of polygonal curve
			int vtc = approx.size();

			// Get the cosines of all corners
			std::vector<double> cos;
			for (int j = 2; j < vtc+1; j++)
				cos.push_back(angle(approx[j%vtc], approx[j-2], approx[j-1]));

			// Sort ascending the cosine values
			std::sort(cos.begin(), cos.end());

			// Get the lowest and the highest cosine
			double mincos = cos.front();
			double maxcos = cos.back();

			// Use the degrees obtained above and the number of vertices
			// to determine the shape of the contour
			if (vtc == 4 && mincos >= -0.1 && maxcos <= 0.3)
				setLabel(dst, "RECT", contours[i]);
			else if (vtc == 5 && mincos >= -0.34 && maxcos <= -0.27)
				setLabel(dst, "PENTA", contours[i]);
			else if (vtc == 6 && mincos >= -0.55 && maxcos <= -0.45)
				setLabel(dst, "HEXA", contours[i]);
		}
		else
		{
			// Detect and label circles
			double area = cv::contourArea(contours[i]);
			cv::Rect r = cv::boundingRect(contours[i]);
			int radius = r.width / 2;

			if (std::abs(1 - ((double)r.width / r.height)) <= 0.2 &&
			    std::abs(1 - (area / (CV_PI * std::pow(radius, 2)))) <= 0.2)
				setLabel(dst, "CIR", contours[i]);
		}
	}

	cv::imshow("imgThresholded", imgThresholded);
	cv::imshow("imgOrignal", imgOriginal);
	cv::imshow("dst", dst);


	if ((cv::waitKey(30) & 0xFF) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
	{
	  std::cout << "esc key is pressed by user" << std::endl;
	  break;
	}
    }
}

*/


/**
 * Helper function to find a cosine of angle between vectors
 * from pt0->pt1 and pt0->pt2
 */
/*
static double angle(cv::Point& pt1, cv::Point& pt2, cv::Point& pt0)
{
	double dx1 = pt1.x - pt0.x;
	double dy1 = pt1.y - pt0.y;
	double dx2 = pt2.x - pt0.x;
	double dy2 = pt2.y - pt0.y;
	return (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}
*/
