#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <random>
#include <sstream>

using namespace std;

struct Book
{
    string title;
    string author;
    string id;
};

struct User
{
    double id;
    string firstName;
    string lastName;
    string password;
    vector<Book> borrowedBooks;
    bool isAdmin;
};

vector<User> users;
vector<Book> books;

int generateRandomInt(int min, int max)
{
    std::random_device rd;  // Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<> dis(min, max);
    return dis(gen);
}

string generateRandomString(size_t size)
{
    string result;
    std::random_device rd;                        // Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd());                       // Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<> dis(97, 122); // ASCII value of 'a' is 97 and 'z' is 122
    for (size_t i = 0; i < size; i++)
    {
        result += static_cast<char>(dis(gen));
    }
    return result;
}

void saveData()
{
    ofstream dataFile("data.csv");
    dataFile << "ID,Title,Author" << endl;
    for (int i = 0; i < books.size(); i++)
    {
        dataFile << books[i].id << "," << books[i].title << "," << books[i].author << endl;
    }
    dataFile << endl;
    dataFile << "ID,First Name,Last Name,Is Admin, Password,Borrowed Books" << endl;
    for (int i = 0; i < users.size(); i++)
    {
        dataFile << users[i].id << "," << users[i].firstName << "," << users[i].lastName << "," << users[i].isAdmin << "," << users[i].password << ",";

        // debug
        std::cout << users[i].firstName << " String address = " << static_cast<void *>(&users[i].borrowedBooks) << std::endl;

        for (int j = 0; j < users[i].borrowedBooks.size(); j++)
        {

            dataFile << users[i].borrowedBooks[j].id;
            if (j < users[i].borrowedBooks.size() - 1)
            {
                dataFile << ",";
            }
        }
        dataFile << endl;
    }
    dataFile.close();
}

void readData()
{
    ifstream dataFile("data.csv");
    string line;
    getline(dataFile, line);
    while (getline(dataFile, line))
    {
        stringstream lineStream(line);
        string cell;
        vector<string> row;
        while (getline(lineStream, cell, ','))
        {
            row.push_back(cell);
        }
        if (row.size() == 3)
        {
            Book book;
            book.id = row[0];
            book.title = row[1];
            book.author = row[2];
            books.push_back(book);
        }
        else if (row.size() >= 4)
        {
            try
            {
                User user;
                user.id = stod(row[0]);
                user.firstName = row[1];
                user.lastName = row[2];
                user.isAdmin = (row[3] == "1");
                user.password = row[4];
                for (int i = 5; i < row.size(); i++)
                {
                    for (int j = 0; j < books.size(); j++)
                    {
                        if (books[j].title == row[i])
                        {
                            user.borrowedBooks.push_back(books[j]);
                            break;
                        }
                    }
                }
                users.push_back(user);
            }
            catch (const std::invalid_argument &e)
            {
                cout << "Invalid number input : " << e.what() << endl;
            }
            catch (const std::out_of_range &e)
            {
                cout << "Number out of range : " << e.what() << endl;
            }
        }
    }
    dataFile.close();
}

void borrowBook(User &user)
{
    string id;

    cout << "Enter book ID: ";
    cin >> id;
    for (int i = 0; i < books.size(); i++)
    {
        if (books[i].id == id)
        {
            user.borrowedBooks.push_back(books[i]);
            cout << "Book borrowed successfully." << endl;

            // debug
            // cout << "user => " << users[0].firstName;
            // std::cout << "String address = " << static_cast<void *>(&users[0].borrowedBooks) << std::endl;

            saveData();
            return;
        }
    }
    cout << "Book not found." << endl;
}

void viewBorrowedBooks(User &user)
{
    if (user.borrowedBooks.size() == 0)
    {
        cout << "You haven't borrowed any books yet." << endl;
        return;
    }
    cout << "List of borrowed books:" << endl;
    for (int i = 0; i < user.borrowedBooks.size(); i++)
    {
        cout << user.borrowedBooks[i].title << " by " << user.borrowedBooks[i].author << " (ID: " << user.borrowedBooks[i].id << ")" << endl;
    }
}

void viewBooks()
{
    // bubble sort
    for (int i = 0; i < books.size(); i++)
    {
        for (int j = 0; j < books.size() - i - 1; j++)
        {
            if (books[j].id > books[j + 1].id)
            {
                swap(books[j], books[j + 1]);
            }
        }
    }
    cout << "List of Books:" << endl;
    for (int i = 0; i < books.size(); i++)
    {
        cout << books[i].title << " by " << books[i].author << " (ID: " << books[i].id << ")" << endl;
    }
}

User &login(string username, string password)
{
    for (int i = 0; i < users.size(); i++)
    {
        if (users[i].firstName + " " + users[i].lastName == username && users[i].password == password)
        {
            std::cout << "String address = " << static_cast<void *>(&users[i]) << std::endl;
            return users[i];
        }
    }
    User emptyUser = {-1, "", "", "", {}, false};
    return emptyUser;
}

void addBook(string title, string author)
{

    string id = generateRandomString(10);
    books.push_back({title, author, id});
    saveData();
}

void deleteBook(string id)
{
    for (int i = 0; i < books.size(); i++)
    {
        if (books[i].id == id)
        {
            for (int j = 0; j < users.size(); j++)
            {
                for (int k = 0; k < users[j].borrowedBooks.size(); k++)
                {
                    if (users[j].borrowedBooks[k].id == id)
                    {
                        cout << "Book cannot be deleted as it is borrowed by " << users[j].firstName << " " << users[j].lastName << endl;
                        return;
                    }
                }
            }
            books.erase(books.begin() + i);
            cout << "Book deleted successfully." << endl;
            saveData();
            return;
        }
    }
    cout << "Book not found." << endl;
}

vector<Book> searchBooks(string title)
{
    vector<Book> foundBooks;
    for (int i = 0; i < books.size(); i++)
    {
        if (books[i].title.find(title) != string::npos)
        {
            foundBooks.push_back(books[i]);
        }
    }
    return foundBooks;
}

int main()
{

    fstream dataFile("data.txt");

    if (!dataFile.good())
    {
        cout << "Error: data.txt file not found. Exiting program." << endl;
        return 0;
    }
    dataFile.close();

    readData();

    cout << "Hi there!\n";

    string username;
    cout << "Enter username: ";
    getline(cin, username);
    string password;
    cout << "Enter password: ";
    getline(cin, password);
    User currentUser = login(username, password);

    if (currentUser.id == -1)
    {
        cout << "Invalid username or password." << endl;
        return 0;
    }
    while (true)
    {

        if (currentUser.isAdmin)
        {
            cout << "\nWelcome, admin." << endl;
            cout << "1. Add a book" << endl;
            cout << "2. Delete a book" << endl;
            cout << "3. View all books" << endl;
            cout << "4. Exit application\n"
                 << endl;

            int choice;
            cin >> choice;
            if (choice == 1)
            {
                string title;
                cout << "Enter book title: ";
                cin.ignore();
                getline(cin, title);

                string author;
                cout << "Enter book author: ";
                getline(cin, author);

                addBook(title, author);
                cout << "Book added successfully." << endl;
            }
            else if (choice == 2)
            {
                string id;
                cout << "Enter book ID: ";
                cin >> id;
                deleteBook(id);
                cout << "Book deleted successfully." << endl;
            }
            else if (choice == 3)
            {
                viewBooks();
            }
            else if (choice == 4)
            {
                break;
            }
        }
        else
        {
            cout << "\nWelcome, " << currentUser.firstName << "." << endl;
            string title;

            cout << "1. Search for a book" << endl;
            cout << "2. Borrow a book" << endl;
            cout << "3. View borrowed books" << endl;
            cout << "4. Exit application\n"
                 << endl;
            int choice;
            cin >> choice;

            if (choice == 1)
            {
                // search for a book code here
                cout << "Enter the name of the book\n";
                cin.ignore();
                getline(cin, title);
                vector<Book> foundBooks = searchBooks(title);
                if (foundBooks.size() > 0)
                {
                    cout << "Found " << foundBooks.size() << " book(s)." << endl;
                    for (int i = 0; i < foundBooks.size(); i++)
                    {
                        cout << foundBooks[i].title << " by " << foundBooks[i].author << endl;
                    }
                }
                else
                {
                    cout << "No books found." << endl;
                }
            }
            else if (choice == 2)
            {
                borrowBook(currentUser);
            }
            else if (choice == 3)
            {
                viewBorrowedBooks(currentUser);
            }
            else if (choice == 4)
            {
                break;
            }
        }
    }
    cout << "see you sooon :)";
    return 0;
}