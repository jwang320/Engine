#pragma once
#include <jdbc/cppconn/driver.h>
#include "EntityStateLoader.h"

class MySqlEntityStateLoader : public EntityStateLoader
{
private:
	sql::Driver* driver;
	sql::Connection* con;

public:
	MySqlEntityStateLoader(const std::string& hostname, const std::string& username, const std::string& password);
	virtual unsigned int getObjectId(const unsigned int owner, const unsigned int objectType, const bool persist) override;
	virtual std::vector<EntityState> getPlayerEntityStates(const unsigned int owner) override;
	virtual void savePlayerEntityStates(const std::vector<EntityState>& entityStates, const unsigned int owner) override;

	~MySqlEntityStateLoader();
};