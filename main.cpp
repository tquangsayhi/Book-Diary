//------------------------------------------------
//  COMP.CS.110 Programming 2: Techniques
//  Copyright © 2025
//------------------------------------------------

#include <iostream>
#include <vector>
#include <fstream>
#include <map>
#include <list>
#include <set>

using namespace std;

/*********************/
/* Utility functions */
/*********************/

// Split the given string based on the given delimiter.
// Consider text enclosed with quatation marks as a single word.
vector<string> split(const string& str, char delim) {
    vector<string> result = {""};
    bool inside_quotation = false;
    for ( char current_char : str ) {
        if ( current_char == '"' ) {
            inside_quotation = not inside_quotation;
        } else if ( current_char == delim and not inside_quotation ) {
            result.push_back("");
        } else {
            result.back().push_back(current_char);
        }
    }
    return result;
}

// Return true, if and only if the given string consists only of digits.
bool is_numeric(const string& str) {
    for( unsigned int i = 0; i < str.size(); ++i ) {
        if ( not isdigit(str.at(i)) ) {
            return false;
        }
    }
    return true;
}

// Return true, if and only if the given string consists only of empty spaces.
bool is_space(const string& str) {
    for ( char ch : str ) {
        if ( ch != ' ' ) {
            return false;
        }
    }
    return true;
}

// Print the given error message.
void error_message(const string& text) {
    cout << "Error: " << text << endl;
}

/****************************/
/* End of utility functions */
/****************************/

// Data structure to store information about a book.
struct Book {
    string author;
    string title;
    int number_of_pages;
    string date;
    // Ordering operator: compares by author, then title, then pages. This will be useful for the printing functions.
    bool operator<(const Book& other) const {
    if (author != other.author) return author < other.author;
    if (title != other.title) return title < other.title;
    return number_of_pages < other.number_of_pages;
    }
};

//Prints all authors in the database, along with the titles of the books they have written.
// The data is automatically sorted in alphabetical order when we collect data (because of <set> and <map>)
//If an author has multiple books, they are shown in alphabetical order (because set<Book> is ordered).
void command_authors (const map<string,set<Book>>& books_by_author)
{
    for(const auto& [author,books]: books_by_author)
            {
                cout << author << endl;
                for (const auto& b: books)
                {
                    cout << " * " << b.title << endl;
                }
            }
}

// Prints all book titles written by the given author. If the author does not exist in the database,print an error message.
void command_books_by(const map<string,set<Book>>& books_by_author, const string& author)
{
    if (books_by_author.find(author) == books_by_author.end())
    {
        error_message("unknown author");
    } else {
        for (const auto& b: books_by_author.find(author)->second)
        {
            cout << b.title << endl;
        }
    }
}

//Prints all authors who have written a book with the given title. If no such title exists in the database, print an error message
void command_authors_of(const map<string,set<Book>>& books_by_author, const string& title)
{
    // create a bool to check if the given title is valid from the data.
    bool exist = false;
    for(const auto& [author,books]: books_by_author)
            {
                for (const auto& b: books)
                {
                    if (b.title == title)
                    {
                        cout << author << endl;
                        exist = true;
                    }

                }
            }
    if (!exist) error_message("unknown title");
}

// Finds the book(s) with the largest number of pages among all authors and prints them.
//If multiple books share the same highest page count, all of them are printed.
void command_thickest(const map<string,set<Book>>& books_by_author) {
    // Firstly I set the highest page to 0, which will change after the loops
    int highest_page = 0;
    //Also, I create another map to store data of thickest books by author.
    // This is because there might be sevaral thickest books and some might come from the same author.
    map<string,set<string>> thickest_books;
    for(const auto& [author,books]: books_by_author)
            {
                for (const auto& b: books)
                {
                  if (b.number_of_pages > highest_page)
                  {
                    // Found a thicker book → reset and store only this one
                    highest_page = b.number_of_pages;
                    thickest_books.clear();
                    thickest_books[author].insert(b.title);
                } else if (b.number_of_pages == highest_page)
                {
                    // Same thickness → add to the map
                    thickest_books[author].insert(b.title);
                }}
            }
    cout << "Thickest book(s) has " << highest_page << " pages:" << endl;
    for(const auto& [author,titles]:thickest_books)
    {
        for (const auto& title: titles)
        {
            cout << " * " << author << " : " << title << endl;
        }
    }
}

// Prints the given N most recently read books, based on their 'date' field.
// Also prints a list of books that are planned to be read (books with an empty 'date' field).
// If there are no read books or planned books, there will be a notice message.
void command_recent_books(const map<string,set<Book>>& books_by_author, int n)
{
    // Create a map to store data of read Books by date. By this way, all books will be automatically sorted by date, we only need to print out.
    map<string,set<Book>> read_books;
    // also create a map to store the data of planned books by author.
    map<string, set<Book>> planned_books;

    //Go through the collected data and add books with the 'date' field to read_books, the rest will go to planned_books
    for(const auto& [author,books]: books_by_author)
            {
                for (const auto& b: books)
                {
                    if (is_space(b.date))
                    {
                        planned_books[author].insert(b);
                    } else {
                        read_books[b.date].insert(b);
                    }
                }
            }
    if (read_books.empty())
    {
        cout << "No books read yet" <<endl;
    } else {
        cout << "Books read most recently:" << endl;
        // add a counter to print out correct numbers of books
        int num = 0;
        // Go through from the back of read_books to print out most recent books
        for(auto it = read_books.rbegin(); it != read_books.rend(); ++it) {
            const string& date = it->first;
            const auto& books = it->second;
            for (const auto& b:books) {
                num ++;
                if (num <= n)
                {cout << " * " << date << " : " << b.author << " : " << b.title << endl;}
                else {
                    break;
                }
            }
        }
    }
    if (planned_books.empty())
    {
        cout << "No books planned to be read" << endl;
    } else {
        cout << "Books planned to be read:" << endl;
        for (const auto& [author, books]:planned_books)
        {
            for (const auto& b:books) {
                cout << " * " << author << " : " << b.title << endl;
        }}}
}

int main() {
    string filename_input = "";
    cout << "Input file: ";
    getline(cin, filename_input);

    // Try to open the input file given by the user
    ifstream file_object(filename_input);
    if ( not file_object ) {
        error_message("the input file cannot be opened");
        return EXIT_FAILURE;}

    string line;
     // Main data structure: the map stores data by author
    map<string,set<Book>> books_by_author ;


    //Read and parse the input file. Each line is expected to be in format: author ; title ; pages ; date
    // The program wil end and return EXIT_FAILURE if there is any error regarding the given input file
    while (std::getline(file_object, line)) {
        if (line.empty()) continue;
        vector<string> fields = split(line, ';');
        if (fields.size() != 4) {
            error_message("the input file has an erroneous line");
            return EXIT_FAILURE;
        }
        if (is_space(fields[0]) || is_space(fields[1]) || is_space(fields[2]))
        {
            error_message("a line has an empty field");
            return EXIT_FAILURE;
        }
        if (!is_numeric(fields[2]))
        {
            error_message("amount of pages is not numeric");
            return EXIT_FAILURE;
        }
        //create Book object
        string author = fields[0];
        Book b;
        b.author = author;
        b.title = fields[1];
        b.number_of_pages = stoi(fields[2]);
        b.date = fields[3];

        // Insert into map: If the same book (author+title+pages) already exists, keep the one with the more recent date.
        auto& books = books_by_author[author];
        auto it = books.find(b);
        if (it == books.end())
        {
            books.insert(b);
        }  else {
            if (it->date < b.date) {
            books.erase(it);
            books.insert(b);
        }}}
    file_object.close();

    // Program waits for user input and responds to commands:
        string command;
        while (true)
        {
            cout << "> ";
            getline(cin,command);
            //split the input into words using the given split function
            vector<string> split_command = split(command, ' ');

            if (command == "quit")
            {
                return EXIT_SUCCESS;
            }
            else if (split_command[0] == "authors")
                    {
                        if(split_command.size() == 1)
                        {command_authors(books_by_author);}
                        //return error message if input is in wrong format
                        else{
                            error_message("error in command " + split_command[0]);
                        }
                    }
            else if (split_command[0] == "books_by" )
                    {
                        if(split_command.size() == 2)
                        {command_books_by(books_by_author, split_command[1]);}
                        //return error message if input is in wrong format
                        else{
                            error_message("error in command " + split_command[0]);
                        }
                    }
            else if (split_command[0] == "authors_of" )
                    {
                        if(split_command.size() == 2)
                        {command_authors_of(books_by_author, split_command[1]);}
                        //return error message if input is in wrong format
                        else{
                            error_message("error in command " + split_command[0]);
                        }
                    }
            else if (command == "thickest")
                    {
                        if(split_command.size() == 1)
                        {command_thickest(books_by_author);}
                        //return error message if input is in wrong format
                        else{
                            error_message("error in command " + split_command[0]);
                        }
                    }
            else if (split_command[0] == "recent_books" )
                    {
                        //return error message if input is in wrong format
                        if (split_command.size() != 2) {
                            error_message("error in command " + split_command[0]);
                        }
                        else {
                            //return error message if given parameter is not a number
                            if (!is_numeric(split_command[1]) )
                        {
                            error_message("parameter not numeric");
                        } else {
                            command_recent_books(books_by_author, stoi(split_command[1]));
                        }
                        }
                    }
            // unknown command
            else {
                error_message("unknown command: " + split_command[0]);
            }
        }
        return EXIT_SUCCESS;
}
