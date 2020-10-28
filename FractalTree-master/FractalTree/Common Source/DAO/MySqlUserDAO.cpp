#include "pch.h"
#include "MySqlUserDAO.h"
#include <jdbc\cppconn\connection.h>
#include <jdbc\cppconn\prepared_statement.h>
#include <iostream>
#include "StatementWrapper.h"
using namespace sql;
using namespace std;

MySqlUserDAO::MySqlUserDAO(const string& hostname, const string& username, const string& password)
{
	try
	{
		this->driver = get_driver_instance();
		const auto s = this->driver->getName();
		//cout << s;
		this->con = this->driver->connect(sql::SQLString("tcp://127.0.0.1:3306"), sql::SQLString("root"), sql::SQLString("root"));
		Statement* const stmt = con->createStatement();
		stmt->execute("USE vexal");
		delete stmt;
		this->initialized = true;
	}
	catch (...)
	{
		auto e = current_exception();
		this->initialized = false;
	}
}

bool MySqlUserDAO::validateAccount(const string& username, const string& password) const
{
	StatementWrapper statement;
	statement.prep_stmt = this->con->prepareStatement("SELECT id FROM users WHERE LOWER(username) = LOWER(?)");
	statement.prep_stmt->setString(1, username.c_str());
	//prep_stmt->setString(2, password);

	statement.result = statement.prep_stmt->executeQuery();
	const int resultCount = statement.result->rowsCount();

	return resultCount == 1;
}

int MySqlUserDAO::getAccountId(const string& username) const
{
	PreparedStatement* const prep_stmt = this->con->prepareStatement("SELECT id FROM users WHERE LOWER(username) = LOWER(?)");
	prep_stmt->setString(1, username.c_str());

	ResultSet* const result = prep_stmt->executeQuery();
	int id = -1;
	if (result->rowsCount() == 1)
	{
		result->next();
		id = result->getInt("id");
	}

	delete prep_stmt;
	delete result;
	return id;
}

int MySqlUserDAO::getOrCreateAccountId(const std::string& username, const std::string& password, bool& created) const
{
	const int possibleAccountId = this->getAccountId(username);
	if (possibleAccountId >= 0)
	{
		if (!this->validateAccount(username, password)) {
			return -1;
		}
		return possibleAccountId;
	}
	else
	{
		created = true;
		cout << "Creating account for " << username;
		PreparedStatement* const prep_stmt = this->con->prepareStatement("INSERT INTO users(username) values(LOWER(?))");
		prep_stmt->setString(1, username.c_str());
		//prep_stmt->setString(2, password);

		prep_stmt->executeUpdate();
		delete prep_stmt;

		return this->getAccountId(username);
	}

}

vector<int> MySqlUserDAO::getUserIds() const
{
	Statement* const statement = this->con->createStatement();
	ResultSet* const result = statement->executeQuery("SELECT id FROM users");
	vector<int> userIds;

	while (result->next())
	{
		userIds.push_back(result->getInt("id"));
	}

	return userIds;
}

string MySqlUserDAO::getAccountName(const unsigned int userId) const
{
	PreparedStatement* const prep_stmt = this->con->prepareStatement("SELECT username FROM users WHERE id = ?");
	prep_stmt->setUInt(1, userId);

	ResultSet* const result = prep_stmt->executeQuery();
	string username = "";
	if (result->rowsCount() == 1)
	{
		if (result->next())
		{
			const sql::SQLString res = result->getString(sql::SQLString("username"));
			username = string(res.c_str());
		}
	}

	delete prep_stmt;
	delete result;
	return username;
}

MySqlUserDAO::~MySqlUserDAO()
{
	delete this->con;
}
