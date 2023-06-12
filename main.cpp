#include <SFML/Graphics.hpp>

#include "Display.h"
#include "Field.h"
#include "Contour.h"

int main(int argc, char** argv)
{
	try {
		Display disp(800, 800, 4);
		disp.show();
	}
	catch (std::out_of_range e)
	{
		std::cout << e.what() << std::endl;
	}
	catch (...)
	{
		std::cout << "\n\nUNDEF THROW\n\n";
	}

	return 0;
}

/*
  - Non-square windows translate incorrectly from field to display
*/
