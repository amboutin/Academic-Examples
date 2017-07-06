/********************************************************************************************
** Author: Aaron Boutin
** Date: 01/JUN/16
** Description: Book Class Specification File
********************************************************************************************/

#ifndef BOOK_HPP
#define BOOK_HPP

#include "LibraryItem.hpp"
#include <string>

class Book : public LibraryItem
{
private:
	std::string author;
public:
	Book(std::string id, std::string title, std::string author);
	static const int CHECK_OUT_LENGTH = 21;
	std::string getAuthor();
	virtual int getCheckOutLength();
};

#endif
