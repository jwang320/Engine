#pragma once

namespace sql
{
	class PreparedStatement;
	class ResultSet;
	class Statement;
}

class StatementWrapper
{
public:
	sql::PreparedStatement* prep_stmt = nullptr;
	sql::ResultSet* result = nullptr;
	sql::Statement* statement = nullptr;

	~StatementWrapper();
};

