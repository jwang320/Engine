#include "pch.h"
#include <jdbc\cppconn\connection.h>
#include <jdbc\cppconn\prepared_statement.h>
#include "MySqlEntityStateLoader.h"
#include "StatementWrapper.h"

using namespace sql;
using namespace std;

MySqlEntityStateLoader::MySqlEntityStateLoader(const std::string& hostname, const std::string& username, const std::string& password)
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

unsigned int MySqlEntityStateLoader::getObjectId(const unsigned int owner, const unsigned int objectType, const bool persist)
{
	StatementWrapper statement;

	//sql::SQLString s("INSERT INTO entities(object_type, owner, persist) values(?,?,?)", strlen("INSERT INTO entities(object_type, owner, persist) values(?,?,?)"));
	//statement.prep_stmt = this->con->prepareStatement(sql::SQLString("INSERT INTO entities(object_type, owner, persist) values(?,?,?)"));
	try {
		statement.prep_stmt = this->con->prepareStatement("insert into entities(`object_type`, `owner`, `persist`, `loc_x`, `loc_y`, `loc_z`) values(?,?,?,0,0,0)");
	}
	catch (sql::SQLException& e)
	{
		auto f = e.getErrorCode();
		auto ff = e.what();
		cout << f << ": " << ff;
	}
	//statement.prep_stmt = this->con->prepareStatement(s);
	statement.prep_stmt->setUInt(1, objectType);
	statement.prep_stmt->setUInt(2, owner);
	statement.prep_stmt->setBoolean(3, persist);

	statement.prep_stmt->executeUpdate();

	StatementWrapper getIdStatement;
	getIdStatement.statement = this->con->createStatement();
	getIdStatement.result = getIdStatement.statement->executeQuery("SELECT LAST_INSERT_ID()");

	getIdStatement.result->next();
	return getIdStatement.result->getInt(1);
}

vector<EntityState> MySqlEntityStateLoader::getPlayerEntityStates(const unsigned int owner)
{
	vector<EntityState> results;
	StatementWrapper statement;
	statement.prep_stmt = this->con->prepareStatement("SELECT `id`, `object_type`, `loc_x`, `loc_y`, `loc_z` FROM entities WHERE `owner` = ? AND `persist` = FALSE");
	statement.prep_stmt->setInt(1, owner);

	statement.result = statement.prep_stmt->executeQuery();

	while (statement.result->next())
	{
		EntityState e;
		using namespace SimulationEngine;
		using namespace DirectX;
		e.objectParamers = ObjectParameters(nullptr, -1, -1, statement.result->getInt(1));
		e.objectType = static_cast<ObjectTypes>(statement.result->getInt(2));
		e.moveableParameters = MoveableParameters(XMVectorSet(statement.result->getDouble(3), statement.result->getDouble(4), statement.result->getDouble(5), 1));
		e.persist = false;
		results.push_back(e);
	}
	return results;
}

void MySqlEntityStateLoader::savePlayerEntityStates(const vector<EntityState>& entityStates, const unsigned int owner)
{
	for (const EntityState& state : entityStates)
	{
		using namespace DirectX;
		XMFLOAT4 loc;
		XMStoreFloat4(&loc, state.moveableParameters.location);

		try {
			StatementWrapper statement;
			statement.prep_stmt = this->con->prepareStatement(R"(insert into entities(`id`, `object_type`, `owner`, `persist`, `loc_x`, `loc_y`, `loc_z`) values(?,?,?,?,?,?,?) 
		ON DUPLICATE KEY UPDATE `object_type` = ?, `owner` = ?, `persist` = ?, `loc_x` = ?, `loc_y` = ?, `loc_z` = ?)");
			statement.prep_stmt->setInt(1, state.objectParamers.persistentId);
			statement.prep_stmt->setUInt(2, static_cast<unsigned int>(state.objectType));
			statement.prep_stmt->setUInt(3, owner);
			statement.prep_stmt->setBoolean(4, false);
			statement.prep_stmt->setDouble(5, loc.x);
			statement.prep_stmt->setDouble(6, loc.y);
			statement.prep_stmt->setDouble(7, loc.z);

			statement.prep_stmt->setUInt(8, static_cast<unsigned int>(state.objectType));
			statement.prep_stmt->setUInt(9, owner);
			statement.prep_stmt->setBoolean(10, false);
			statement.prep_stmt->setDouble(11, loc.x);
			statement.prep_stmt->setDouble(12, loc.y);
			statement.prep_stmt->setDouble(13, loc.z);

			statement.prep_stmt->executeUpdate();

		} catch (sql::SQLException e) {
			cout << e.what() << endl;
			throw e;
		}
	}
}

MySqlEntityStateLoader::~MySqlEntityStateLoader()
{
	delete this->con;
}
