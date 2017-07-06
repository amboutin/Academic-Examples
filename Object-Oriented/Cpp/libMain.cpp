#include "Library.hpp"
#include <iostream>
#include <string>


int main()
{
	Book b1("123", "War and Peace", "Tolstoy");
	Book b2("234", "Moby Dick", "Melville");
	Book b3("345", "Phantom Tollbooth", "Juster");
	Album a1("1234", "Fancy Name", "Aaron");
	Album a2("1235", "Generic Name", "Daron");
	Album a3("1236", "Lame Name", "Laron");
	Movie m1("4123", "Serenity", "Joss Whedon");
	Movie m2("4124", "Avengers", "Joss Whedon");
	Movie m3("4125", "Captain Horrible's Singalong Blog", "Joss Whedon");
	Patron p1("abc", "Felicity");
	Patron p2("bcd", "Waldo");
	Patron p3("cde", "Mac");
	Library lib;

	std::cout << m2.getDirector() << std::endl;
	std::cout << m2.getTitle() << std::endl;

	lib.addLibraryItem(&b1);
	lib.addLibraryItem(&b2);
	lib.addLibraryItem(&b3);
	lib.addLibraryItem(&a1);
	lib.addLibraryItem(&a2);
	lib.addLibraryItem(&a3);
	lib.addLibraryItem(&m1);
	lib.addLibraryItem(&m2);
	lib.addLibraryItem(&m3);
	lib.addPatron(&p1);
	lib.addPatron(&p2);
	lib.addPatron(&p3);

	std::cout << lib.getPatron("bcd")->getName() << std::endl;


	std::cout << lib.checkOutLibraryItem("bcd", "234") << std::endl; // p2 has checked it out Moby Dick

	std::vector<LibraryItem*> temp = p2.getCheckedOutItems();
	for (int index = 0; index < temp.size(); index++)
		std::cout << temp.at(index)->getIdCode() << std::endl;

	for (int i = 0; i<7; i++)
		lib.incrementCurrentDate();		// date at 7
	std::cout << "" << std::endl;
	std::cout << lib.checkOutLibraryItem("bcd", "123") << std::endl; // p2 has checked it out Moby Dick
	std::cout << lib.checkOutLibraryItem("bcd", "123") << std::endl;// repeat checkout
	std::cout << lib.checkOutLibraryItem("abc", "345") << std::endl; // p1 checked Phanton Tollbooth
	std::cout << lib.requestLibraryItem("cde", "345") << std::endl;
	std::cout << lib.returnLibraryItem("345") << std::endl;
	std::cout << lib.requestLibraryItem("cde", "345") << std::endl;
	std::cout << lib.checkOutLibraryItem("bcd", "345") << std::endl;
	std::cout << lib.checkOutLibraryItem("cde", "345") << std::endl;
	for (int i = 0; i<24; i++)
		lib.incrementCurrentDate();		// date at 24
	lib.payFine("bcd", 0.4);
	double p1Fine = p1.getFineAmount();
	double p2Fine = p2.getFineAmount();
	std::cout << p1Fine << std::endl;
	std::cout << p2Fine << std::endl;
	std::cout << " " << std::endl;
	std::cout << " " << std::endl;
	std::cout << " " << std::endl;
	std::cout << " " << std::endl;
	std::cout << " " << std::endl;
	std::cout << " " << std::endl;
	std::cout << " " << std::endl;
	std::cout << " " << std::endl;
	std::cout << " " << std::endl;

	std::cin.get();

	return 0;
}
