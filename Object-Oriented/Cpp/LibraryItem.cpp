/********************************************************************************************
** Author: Aaron Boutin
** Date: 01/JUN/16
** Description: LibraryItem Class Implementation file. Mostly all get and set methods.
********************************************************************************************/

#include "LibraryItem.hpp"
#include <string>

/*********************************************************************************************
** Description: Class constructor that initializes member pointers to point at nothing and sets
**				inital item location to the shelf.
**********************************************************************************************/
LibraryItem::LibraryItem(std::string idc, std::string t)
{
	idCode = idc;
	title = t;
	Patron* checkedOutBy = NULL;
	Patron* requestedBy = NULL;
	location = ON_SHELF;
}

std::string LibraryItem::getIdCode()
{
	return idCode;
}


std::string LibraryItem::getTitle()
{
	return title;
}

Locale LibraryItem::getLocation()
{
	return location;
}

void LibraryItem::setLocation(Locale loc)
{
	location = loc;
}

Patron* LibraryItem::getCheckedOutBy()
{
	return checkedOutBy;
}

void LibraryItem::setCheckedOutBy(Patron* memberCheckOut)
{
	checkedOutBy = memberCheckOut;
}

Patron* LibraryItem::getRequestedBy()
{
	return requestedBy;
}

void LibraryItem::setRequestedBy(Patron* memberReq)
{
	requestedBy = memberReq;
}

int LibraryItem::getDateCheckedOut()
{
	return dateCheckedOut;
}

void LibraryItem::setDateCheckedOut(int checkOutDate)
{
	dateCheckedOut = checkOutDate;
}