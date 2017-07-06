/********************************************************************************************
** Author: Aaron Boutin
** Date: 01/JUN/16
** Description: Library Class Implementation file.
********************************************************************************************/

#include <string>
#include <vector>
#include <iostream>
#include "Book.hpp"
#include "Album.hpp"
#include "Movie.hpp"
#include "Patron.hpp"
#include "Library.hpp"

/*********************************************************************************************
** Description: Class constructor that initalizes the start date.
**
**********************************************************************************************/
Library::Library()
{
	currentDate = 0;
}

/*********************************************************************************************
** Description: Uses vector function to add pointers to LibraryItems to its member vector that 
**				represents its stock.
**********************************************************************************************/
void Library::addLibraryItem(LibraryItem* libItem)
{
	holdings.push_back(libItem);
}

/*********************************************************************************************
** Description: Function that adds members to its vector that represents whom of which patronages
**				the library, as though they have an account there.
**********************************************************************************************/
void Library::addPatron(Patron* member)
{
	members.push_back(member);
}

/*********************************************************************************************
** Description: Function that allows Patrons to check out items from the library, altering the 
**				status of the item to reflect its status.
**********************************************************************************************/
std::string Library::checkOutLibraryItem(std::string patronID, std::string ItemID)
{
	Patron *memberTemp = NULL;

	bool isPresent = false;									
	for (int index = 0; index < members.size();index++)		// checks to see if Patron is present
	{
		if (members.at(index)->getIdNum() == patronID)
		{
			memberTemp = members.at(index);
			isPresent = true;
		}
	}
	if (!isPresent)
		return "patron not found";

	LibraryItem *itemTemp = NULL;
	bool isThere = false;
	for (int index = 0; index < holdings.size();index++)	// checks to see if item is present
	{
		if (holdings.at(index)->getIdCode() == ItemID)
		{
			itemTemp = holdings.at(index);
			isThere = true;
		}
	}
	if (!isThere)
		return "item not found";

	if (itemTemp->getLocation() == CHECKED_OUT)				// These if statements alter item location appropriately
		return "item already checked out";					
	else if (itemTemp->getLocation() == ON_HOLD_SHELF)
	{
		if (itemTemp->getRequestedBy()->getIdNum() == memberTemp->getIdNum())
		{
			itemTemp->setCheckedOutBy(memberTemp);
			itemTemp->setDateCheckedOut(currentDate);
			std::cout << currentDate << std::endl;
			itemTemp->setLocation(CHECKED_OUT);
			memberTemp->addLibraryItem(itemTemp);
			if (itemTemp->getRequestedBy() == memberTemp)
				itemTemp->setRequestedBy(NULL);
		}
		else	
			return "item on hold by another patron";
	}
	else if (itemTemp->getLocation() == ON_SHELF)
	{
		itemTemp->setCheckedOutBy(memberTemp);
		itemTemp->setDateCheckedOut(currentDate);
		std::cout << currentDate << std::endl;
		itemTemp->setLocation(CHECKED_OUT);
		memberTemp->addLibraryItem(itemTemp);
		if(itemTemp->getRequestedBy() == memberTemp)
			itemTemp->setRequestedBy(NULL);
		
	}
	return "check out successful";
}

/*********************************************************************************************
** Description: Function that returns the item from the Patron back to the library, and checks to 
**				see which shelf it should go on.
**********************************************************************************************/
std::string Library::returnLibraryItem(std::string ItemID)
{
	Patron *tempPatron;
	for (int indexOuter = 0; indexOuter < holdings.size();indexOuter++)
	{
		if (holdings.at(indexOuter)->getIdCode() == ItemID)
		{
			if (holdings.at(indexOuter)->getLocation() == ON_SHELF)
			{
				return "item already in library";
			}
			else
			{
				tempPatron = holdings.at(indexOuter)->getCheckedOutBy();
				for (int indexInner = 0; indexInner < tempPatron->getCheckedOutItems().size(); indexInner++)
				{
					if (tempPatron->getCheckedOutItems().at(indexInner)->getIdCode() == ItemID)
					{
						tempPatron->removeLibraryItem(tempPatron->getCheckedOutItems().at(indexInner));
					}
				}
				if (holdings.at(indexOuter)->getRequestedBy() != NULL)
				{
					holdings.at(indexOuter)->setLocation(ON_HOLD_SHELF);
				}
				else
				{
					holdings.at(indexOuter)->setLocation(ON_SHELF);
				}

				holdings.at(indexOuter)->setCheckedOutBy(NULL);
				return "return successful";
			}
		}
	}
	return "item not found";
}

/*********************************************************************************************
** Description: Function that allows Patrons to put a book on hold if it is available.
**
**********************************************************************************************/
std::string Library::requestLibraryItem(std::string patronID, std::string ItemID)
{
	for (int index = 0; index < members.size(); index++)
	{
		if (members.at(index)->getIdNum() == patronID)
		{
			for (int index = 0; index < holdings.size(); index++)
			{
				if (holdings.at(index)->getIdCode() == ItemID)
				{
					if (holdings.at(index)->getLocation() == ON_HOLD_SHELF)
					{
						return "item already on hold";
					}
					else
					{
						holdings.at(index)->setRequestedBy(members.at(index));
					}
				}

			}
			return "item not found";
		}
	}
	return "patron not found";
}

/*********************************************************************************************
** Description: Fucntion that allows a Patron to pay and fines they owe. Uses Patron member function
**				to update its member variable.
**********************************************************************************************/
std::string Library::payFine(std::string patronID, double payment)
{
	for (int index = 0; index < members.size();index++)
	{
		if (members.at(index)->getIdNum() == patronID)
		{
			members.at(index)->amendFine(payment);
			return "payment successful";
		}
	}
	return "patron not found";
}

/*********************************************************************************************
** Description: Fucntion increases the date to the "next day" when called. If any Patron has overdue
**				books, it accrues a daily fine until returned.
**********************************************************************************************/
void Library::incrementCurrentDate()
{
	this->currentDate++;
	std::vector<LibraryItem*> tempVec;
	for (int indexOuter = 0; indexOuter < members.size(); indexOuter++)
	{
		tempVec = members.at(indexOuter)->getCheckedOutItems();
		for (int index = 0; index < tempVec.size(); index++)
		{
			if ((currentDate - tempVec.at(index)->getDateCheckedOut()) > tempVec.at(index)->getCheckOutLength())
			{
				double temp = -0.10;
				members.at(indexOuter)->amendFine(temp);
			} 
		}
	}
}

Patron* Library::getPatron(std::string patronID)
{
	for (int index = 0; index < members.size();index++)
	{
		if (members.at(index)->getIdNum() == patronID)
			return members.at(index);
	}
	return NULL;
}

LibraryItem* Library::getLibraryItem(std::string ItemID)
{
	for (int index = 0; index < holdings.size();index++)
	{
		if ((holdings.at(index))->getIdCode() == ItemID)
			return holdings.at(index);
	}
	return NULL;
}