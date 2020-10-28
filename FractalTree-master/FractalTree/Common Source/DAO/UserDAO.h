#pragma once
#include <string>

class UserDAO
{
protected:
	bool initialized = false;

public:
	virtual bool validateAccount(const std::string& username, const std::string& password) const = 0;
	virtual int getAccountId(const std::string& username) const = 0;
	virtual int getOrCreateAccountId(const std::string& username, const std::string& password, bool& created) const = 0;
	virtual std::string getAccountName(const unsigned int userId) const = 0;
	virtual std::vector<int> getUserIds() const = 0;
	bool Initialized() const { return this->initialized; }
	virtual ~UserDAO() {}
};