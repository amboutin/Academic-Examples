/********************************************************************************************
** Author: Aaron Boutin
** Date: 01/JUN/16
** Description: Book Class Implementation file.
********************************************************************************************/

#include "Book.hpp"
#include <string>

std::string Book::getAuthor()
{
	return author;
}

int Book::getCheckOutLength()
{
	return CHECK_OUT_LENGTH;
}

/*********************************************************************************************
** Description: Class constructor that passes arguments to base class constructor.
**
**********************************************************************************************/
Book::Book(std::string id, std::string title, std::string author) : LibraryItem(id, title)
{
	this->author = author;
}