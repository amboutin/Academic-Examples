/********************************************************************************************
** Author: Aaron Boutin
** Date: 01/JUN/16
** Description: Movie Class Implementation file.
********************************************************************************************/

#include "Movie.hpp"
#include <string>

std::string Movie::getDirector()
{
	return director;
}

int Movie::getCheckOutLength()
{
	return CHECK_OUT_LENGTH;
}

/*********************************************************************************************
** Description: Class constructor that passes arguments to base class constructor.
**
**********************************************************************************************/
Movie::Movie(std::string id, std::string title, std::string director) : LibraryItem(id, title)
{
	this->director = director;
}