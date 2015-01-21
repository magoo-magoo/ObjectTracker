#include <iostream>
#include <thread>
#include "Tracker.hh"

int main(int argc, char **argv)
{
  try {

    Tracker tracker("red");

    if (argc != 1)
      tracker.setColor(argv[1]);

    std::thread th(&Tracker::run, &tracker); // launch thread to execute run()


    while (true)
      {
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        if (tracker.position().x() == -1 or // if no object on screen
	    tracker.position().y() == -1) {
	  std::cout << "No object detected" << std::endl;
	  continue;
        }

        std::cout << "object's position is " << tracker.position().toString()
		  << ". Size: " << tracker.area() << std::endl;

        if (!th.joinable()) {
	  break;
        }
      }
  }
  catch (std::exception &e) {
    std::cerr << "A problem occured: " << e.what() << std::endl;
    return (-1);
  }
  return (0);
}
