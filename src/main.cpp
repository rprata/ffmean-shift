#include "includes/Camera.h"

Camera c;

int main (int argc, char ** argv)
{
	if (argc < 1)
	{
		fprintf(stderr, "Utilize: %s <device>\n", argv[0]);
		return -1;
	}

	std::string device(argv[1]);
	
	if (!c.SetupCamera(device))
		return -1;

	if (!c.StartCamera())
		return -1;
	
	return 0;
}