#include <SFML/Graphics.hpp>

#include "Display.h"
#include "Field.h"
#include "Contour.h"

int main(int argc, char** argv)
{
	try {
		Display disp(1200, 600, 4);
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
  - Neative averages are not combine correctly
  - Non-square windows translate incorrectly from field to display
*/
