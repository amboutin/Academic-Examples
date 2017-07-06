/********************************************************************************************
** Author: Aaron Boutin
** Date: 01/JUN/16
** Description: Album Class Implementation file.
********************************************************************************************/

#include "Album.hpp"
#include <string>

std::string Album::getArtist()
{
	return artist;
}

int Album::getCheckOutLength()
{
	return CHECK_OUT_LENGTH;
}

/*********************************************************************************************
** Description: Class constructor that passes arguments to base class constructor.
**
**********************************************************************************************/
Album::Album(std::string id, std::string title, std::string artist) : LibraryItem(id, title)
{
	this->artist = artist;
}