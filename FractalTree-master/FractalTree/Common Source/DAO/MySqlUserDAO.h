#pragma once
#include "UserDAO.h"
#include <jdbc/cppconn/driver.h>
class MySqlUserDAO : public UserDAO
{
private:
	sql::Driver *driver;
	sql::Connection *con;

public:
	MySqlUserDAO(const std::string& hostname, const std::string& username, const std::string& password);
	virtual bool validateAccount(const std::string& username, const std::string& password) const override;
	virtual int getAccountId(const std::string& username) const override;
	virtual int getOrCreateAccountId(const std::string& username, const std::string& password, bool& created) const override;
	virtual std::vector<int> getUserIds() const override;
	virtual std::string getAccountName(const unsigned int userId) const override;
	~MySqlUserDAO();
};