/********************************************************************************************
** Author: Aaron Boutin
** Date: 01/JUN/16
** Description: Album Class Specification File
********************************************************************************************/

#ifndef ALBUM_HPP
#define ALBUM_HPP

#include "LibraryItem.hpp"
#include <string>

class Album : public LibraryItem
{
private:
	std::string artist;
public:
	Album(std::string id, std::string title, std::string artist);
	static const int CHECK_OUT_LENGTH = 14;
	std::string getArtist();
	virtual int getCheckOutLength();
};
#endif
