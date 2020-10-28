#pragma once
#include "UserDAO.h"

class NullUserDAO : public UserDAO
{
public:
	virtual bool validateAccount(const std::string& username, const std::string& password) const override;
	virtual int getAccountId(const std::string& username) const override;
	virtual std::vector<int> getUserIds() const override;

	// Inherited via UserDAO
	virtual int getOrCreateAccountId(const std::string& username, const std::string& password, bool& created) const override;

	// Inherited via UserDAO
	virtual std::string getAccountName(const unsigned int userId) const override;
};