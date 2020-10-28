#include "pch.h"
#include "NullUserDAO.h"

using namespace std;

bool NullUserDAO::validateAccount(const string& username, const string& password) const
{
	return true;
}

int NullUserDAO::getAccountId(const string& username) const
{
	return 1;
}

vector<int> NullUserDAO::getUserIds() const
{
	vector<int> ids = { 1 };
	return ids;
}

int NullUserDAO::getOrCreateAccountId(const std::string& username, const std::string& password, bool& created) const
{
	return 0;
}

std::string NullUserDAO::getAccountName(const unsigned int userId) const
{
	return "";
}
