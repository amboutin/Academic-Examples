/********************************************************************************************
** Author: Aaron Boutin
** Date: 01/JUN/16
** Description: Patron Class Implementation file.
********************************************************************************************/

#include "Patron.hpp"
#include <string>

/*********************************************************************************************
** Description: Class constructor that initializes member variables.
**
**********************************************************************************************/
Patron::Patron(std::string idn, std::string n)
{
	idNum = idn;
	name = n;
}


std::string Patron::getIdNum()
{
	return idNum;
}


std::string Patron::getName()
{
	return this->name;
}


std::vector<LibraryItem*> Patron::getCheckedOutItems()
{
	return checkedOutItems;
}

/*********************************************************************************************
** Description: Adds a pointer to a LibraryItem to its member vector.
**
**********************************************************************************************/
void Patron::addLibraryItem(LibraryItem* b)
{
	checkedOutItems.push_back(b);
}

/*********************************************************************************************
** Description: Removes a pointer to a LibraryItem to its member vector.
**
**********************************************************************************************/
void Patron::removeLibraryItem(LibraryItem* b)
{
	for (int index = 0; index < checkedOutItems.size(); index++)
	{
		if (checkedOutItems.at(index) == b)
			checkedOutItems.erase(checkedOutItems.begin()+index);
	}
}


double Patron::getFineAmount()
{
	return fineAmount;
}

/*********************************************************************************************
** Description: Function that either adds to or subtracts from the Patron's member variable that
**				represents fines owed to the library.
**********************************************************************************************/
void Patron::amendFine(double amount)
{
	this->fineAmount += amount;
}