#pragma once
#include<iostream>
#include<map>
#include<vector>
#include<string>
#include<fstream>
#include<list>
#include<ctime>
using namespace std;

static string getTime()
{
	char str[26];
	time_t result = time(NULL);
	ctime_s(str, sizeof str, &result);
	return str;
}

class Facebook
{
	struct timeline {
		string time;
		string text;
		int id;
	};
	struct User
	{
		bool Access_check;
		string name, password;
		int Id;
		map<int, User*> friends;
		map<int, string> status;
		map<string, timeline, greater<string>> feed;
		map<string, string, greater<string>> wall;
		friend class Facebook;

	public:
		User(int I, string n, string p)
		{
			Id = I;
			name = n;
			password = p;
			Access_check = true;
		}
	};

	int active_user_ID;
	int current_user_number;
	vector<User*> Users;

	User* search_user(string name)
	{
		for (User* temp : Users)
		{
			if (temp->name == name)
				return temp;
		}
		return nullptr;
	}
	bool search_name(vector<string>& n, string name)
	{
		if (n.size() == 0)
			return false;
		for (int i = 0; i < n.size(); i++)
		{
			if (name == n[i])
				return true;
		}
		return false;
	}

public:
	Facebook()
	{
		current_user_number = 0;
		active_user_ID = 0;
		Read_Users();
		Read_Friends();
		Read_Posts();
		srand(time(NULL));
	}						//Reading users,friends,post from Files for convinience 

	void CreateAccount(string name, string password)
	{
		auto itr = search_user(name);
		if (itr != nullptr)
		{
			cout << "Account already exists" << endl;
			return;
		}
		User* New_User = new User(current_user_number, name, password);
		Users.push_back(New_User);
		current_user_number++;
	}

	bool RemoveAccount()
	{
		char input;
		cout << endl << "ARE YOU SURE: ";
		cin >> input;
		if (input != 'Y' && input != 'y')
			return false;
		User* remove = Users[active_user_ID];														//Remove account from users vector after required checks
		for (auto temp = remove->friends.begin(); temp != remove->friends.end(); temp++)
		{
			auto itr = temp->second->friends.find(active_user_ID);
			temp->second->friends.erase(itr);												//Remove myself from my friends friendlist
		}

		swap(Users[active_user_ID], Users[Users.size() - 1]);
		Users[active_user_ID]->Id = Users[Users.size() - 1]->Id;
		Users.pop_back();

		return true;
	}

	void AddFriend()
	{
		vector<int> myvector;											//Add friend in friends map along with time
		string name, time;
		cout << "Enter the name to add: " << endl;
		getline(cin, name);

		int input;
		cout << endl << "ID" << "\t" << "Name" << endl;
		for (User* temp : Users)
		{
			if (temp->name.find(name) != std::string::npos)
			{
				cout << temp->Id << "\t" << temp->name << endl;
				myvector.push_back(temp->Id);
			}
		}

		if (myvector.size() == 0)
		{
			cout << "No such user exists" << endl;
			return;
		}
		cout << endl << "Enter ID number: ";
		cin >> input;
		cout << endl;

		auto it = find(myvector.begin(), myvector.end(), input);
		if (it == myvector.end())
		{
			cout << "Invalid ID" << endl << endl;
			return;
		}

		auto insert = Users[input];
		auto check = Users[active_user_ID]->friends.find(input);
		if (check != Users[active_user_ID]->friends.end())
		{
			cout << "Friend already exists" << endl << endl;
			return;
		}

		time = getTime();
		time.pop_back();

		Users[active_user_ID]->friends.insert(pair<int, User*>(input, insert));
		Users[active_user_ID]->status.insert(pair<int, string>(input, time));
		Users[input]->friends.insert(pair<int, User*>(active_user_ID, Users[active_user_ID]));
		Users[input]->status.insert(pair<int, string>(active_user_ID, time));
		cout << "Friend Successfully Added" << endl;

	}

	void AddPost()
	{
		string post;																	//Add text in mywall map
		string time;
		cout << "What is on your mind..." << endl;
		getline(cin, post);
		cout << endl;

		time = getTime();
		time.pop_back();
		Users[active_user_ID]->wall.insert(pair<string, string>(time, post));
	}

	void AddPostonFriendWall()
	{
		bool inputcheck = false;
		int input;
		string text;														//add text on my friends wall by accessing his wall map 
		string time = getTime();
		time.pop_back();
		Print_Friends();

		cout << "Enter friend id: ";
		cin >> input;

		for (auto temp : Users[active_user_ID]->friends)
		{
			if (input == temp.first)
			{
				inputcheck = true;
				break;
			}

		}

		if (inputcheck == false)
		{
			cout << "Invalid Id" << endl;
			return;
		}

		cin.ignore();
		cout << "Enter text: ";
		getline(cin, text);


		auto frnd = Users[active_user_ID]->friends.find(input);
		if (frnd->second->Access_check == true)
		{

			text = Users[active_user_ID]->name + " said on my wall: " + text;
			frnd->second->wall.insert(pair<string, string>(time, text));
		}
		else
		{
			cout << "Access not provided" << endl;
		}

	}

	void CheckFriendShipStatus()
	{
		if (Users[active_user_ID]->friends.size() == 0)																		//Print status map
		{
			cout << "You have no friends" << endl;
			return;
		}

		cout << "ID" << "\t" << "Name\t\tFriends Since" << endl;
		map<int, User*>::iterator temp;
		map<int, string>::iterator status;
		for (temp = Users[active_user_ID]->friends.begin(), status = Users[active_user_ID]->status.begin(); temp != Users[active_user_ID]->friends.end(); ++temp)
		{
			cout << temp->first << "\t" << temp->second->name << "\t\t" << status->second << endl;
		}
		cout << endl;
	}

	void LoadTimeline()
	{
		timeline t;
		auto p = Users[active_user_ID];													//Add post from friends wall in my timelime map

		for (auto temp : Users[active_user_ID]->friends)
		{
			if (temp.second->wall.size() == 0)
				continue;
			for (auto itr = temp.second->wall.begin(); itr != temp.second->wall.end(); itr++)
			{
				t.time = (*itr).first;
				t.text = (*itr).second;
				t.id = temp.first;
				p->feed.insert(pair<string, timeline>((*itr).first, t));
			}
		}
	}

	void PrintTimeline()
	{
		for (auto p : Users[active_user_ID]->feed)
		{
			cout << Users[p.second.id]->name << " posted on " << p.second.time << endl << p.second.text << endl << endl;
		}

		cout << endl;
	}

	void RemoveFriend()
	{
		if (Users[active_user_ID]->friends.size() == 0)
		{
			cout << "You have no friends" << endl;										//remove friend from friends list
			return;
		}
		int input;
		Print_Friends();
		cout << "Enter ID number: ";
		cin >> input;
		auto itr = Users[active_user_ID]->friends.find(input);
		Users[active_user_ID]->friends.erase(itr);

		itr = Users[input]->friends.find(active_user_ID);
		Users[input]->friends.erase(active_user_ID);
	}

	void Print_Friends()
	{
		if (Users[active_user_ID]->friends.size() == 0)
		{
			cout << "You have no friends" << endl;
			return;
		}

		cout << "ID" << "\t" << "Name" << endl;
		map<int, User*>::iterator temp;
		for (temp = Users[active_user_ID]->friends.begin(); temp != Users[active_user_ID]->friends.end(); ++temp)
		{
			cout << temp->first << "\t" << temp->second->name << endl;
		}
		cout << endl;
	}

	void Print_Wall()
	{

		if (Users[active_user_ID]->wall.size() == 0)
			return;

		for (auto temp : Users[active_user_ID]->wall)
		{
			cout << temp.first << "\t" << temp.second << endl;;
		}

		cout << endl;
	}

	void Print_Users()
	{
		cout << "ID" << "\t" << "Name" << endl;
		for (User* temp : Users)
		{
			cout << temp->Id << "\t" << temp->name << endl;
		}
		cout << endl;
	}

	void UpdateTimeLine()
	{
		int Limit = 20;
		timeline t;
		auto p = Users[active_user_ID];


		while (p->feed.size() > Limit)
		{
			auto del = p->feed.end();
			p->feed.erase(--del);
		}
		//Update timeline by checking the time of friends new wall post and if its newer as compared to old,inser it in timeline map
		for (auto temp : Users[active_user_ID]->friends)
		{
			if (temp.second->wall.size() == 0)
				continue;
			for (auto itr = temp.second->wall.begin(); itr != temp.second->wall.end(); itr++)
			{
				t.time = (*itr).first;
				t.text = (*itr).second;
				t.id = temp.first;
				if (t.time == p->feed.begin()->second.time)
					continue;
				p->feed.insert(pair<string, timeline>((*itr).first, t));
			}
		}

	}

	void Read_Users()
	{
		ifstream fin("Users.txt");
		int id;
		string name;
		string pass;
		while (fin.good())
		{
			fin >> id;
			fin >> name;
			fin >> pass;

			CreateAccount(name, pass);

			if (fin.eof())
				break;
		}

		active_user_ID = current_user_number;
	}

	void Read_Friends()
	{
		ifstream fin("Friends.txt");
		char c;
		int index = 0, id;
		while (fin.good())
		{
			c = fin.get();
			if (c == fin.eof())
				break;
			if (c == '\n')
				continue;
			if (isdigit(c))
			{
				index = c - 48;
			}

			if (c == '{')
			{
				while (c != '}')
				{
					c = fin.get();
					if (isdigit(c))
					{
						id = c - 48;
						Users[index]->friends.insert(pair<int, User*>(id, Users[id]));
						Users[index]->status.insert(pair<int, string>(id, getTime()));
					}

				}
			}
		}
	}

	void Read_Posts()
	{
		ifstream fin("Post.txt");
		char c;
		int i = 0;
		string text, time;
		while (fin.good())
		{
			c = fin.get();
			if (c == fin.eof())
				break;
			if (c == '\n')
				continue;
			if (isdigit(c))
			{
				i = c - 48;
			}
			if (c == '{')
			{
				while (c != ',')
				{
					c = fin.get();
					text.push_back(c);
				}

				text.pop_back();

				while (c != '}')
				{
					c = fin.get();
					time.push_back(c);
				}
				time.pop_back();
				Users[i]->wall.insert(pair<string, string>(time, text));
				text.erase();
				time.erase();
			}
		}
	}

	void Suggested_Friends()
	{
		vector<string> names;
		auto p = Users[active_user_ID];
		cout << "Suggested Friends: " << endl;													//Displaying a random person from my friends friendlist
		for (auto temp : p->friends)
		{
			int n = rand() % (temp.second->friends.size());
			auto itr = temp.second->friends.begin();

			for (int i = 0; i < n; i++)
			{
				itr++;
			}

			if ((*itr).second->name != p->name)
			{
				if (!search_name(names, (*itr).second->name))
				{
					names.push_back((*itr).second->name);
					cout << (*itr).second->name << endl;
				}
			}
		}
	}

	void Search_Friend()
	{
		string name, check;
		cout << "Enter friend name: ";
		cin >> name;


		cout << "Names" << endl;
		for (auto temp : Users[active_user_ID]->friends)
		{
			check = temp.second->name;
			if (check.find(name) != std::string::npos)
			{
				cout << check << endl;
			}
		}

	}

	bool Start_menu()
	{

		while (true)
		{
			int input;
			cout << "Welcome" << endl;
			cout << "1. Create an Account" << endl;
			cout << "2. Login" << endl;
			cout << "3. Exit" << endl;
			cin >> input;
			cin.ignore();

			if (input == 1)
			{
				string name, password;

				cout << "Enter your name: ";
				getline(cin, name);

				cout << endl << "Enter your password: ";
				cin >> password;
				CreateAccount(name, password);
				cout << "Account created Successfully" << endl;
			}
			else if (input == 2)
			{
				string name, password;
				cout << "Enter your name: ";
				getline(cin, name);

				auto user = search_user(name);
				if (user == nullptr)
				{
					cout << "Username does not exist" << endl;
					continue;
				}
				cout << endl << "Enter your password: ";
				cin >> password;

				if (password != user->password)
				{
					cout << "Wrong password" << endl;
					continue;
				}

				cout << "Welcome " << user->name << endl;
				active_user_ID = user->Id;
			}
			else if (input == 3)
			{
				return false;
			}
			return true;
		}
	}

	void Menu()
	{
		cout << "1. Add friend" << endl
			<< "2. Remove friend" << endl
			<< "3. Delete account" << endl
			<< "4. Post on your Wall" << endl
			<< "5. Show Friends" << endl
			<< "6. Show Feed" << endl
			<< "7. Show my wall" << endl
			<< "8. Suggested Friends" << endl
			<< "9. Search Friends" << endl
			<< "10. Post on Friends Wall" << endl
			<< "11. Check Friendship Status" << endl
			<< "12. Logout" << endl;


	}

	void Run()
	{
		int input;

		while (true)
		{

			if (Start_menu() == false)
				return;
			while (true)
			{

				cout << endl;
				Menu();
				cout << "-> ";
				cin >> input;
				cout << endl << endl;
				cin.ignore();
				switch (input)
				{
				case 1:
					AddFriend();
					break;
				case 2:
					RemoveFriend();
					break;
				case 3:
					if (RemoveAccount())
						cout << endl << "Account deleted successfully" << endl;
					goto end_loop;
					break;
				case 4:
					AddPost();
					Print_Wall();
					break;
				case 5:
					Print_Friends();
					break;
				case 6:
					LoadTimeline();
					PrintTimeline();
					UpdateTimeLine();
					break;
				case 7:
					Print_Wall();
					break;
				case 8:
					Suggested_Friends();
					break;
				case 9:
					Search_Friend();
					break;
				case 10:
					AddPostonFriendWall();
					break;
				case 11:
					CheckFriendShipStatus();
					break;
				case 12:
					goto end_loop;
				}
				cout << endl;

			}

		end_loop:;
		}
	}
};
