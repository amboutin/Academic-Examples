/********************************************************************************************
** Author: Aaron Boutin
** Date: 01/JUN/16
** Description: Movie Class Specification File
********************************************************************************************/

#ifndef MOVIE_HPP
#define MOVIE_HPP

#include "LibraryItem.hpp"
#include <string>

class Movie: public LibraryItem
{
private:
	std::string director;
public:
	Movie(std::string id, std::string title, std::string director);
	static const int CHECK_OUT_LENGTH = 7;
	std::string getDirector();
	virtual int getCheckOutLength();
};

#endif
