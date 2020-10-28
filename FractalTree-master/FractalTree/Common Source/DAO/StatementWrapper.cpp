#include "pch.h"
#include <jdbc\cppconn\prepared_statement.h>
#include "StatementWrapper.h"

StatementWrapper::~StatementWrapper()
{
	if (this->prep_stmt != nullptr)
	{
		delete this->prep_stmt;
	}
	if (this->statement != nullptr)
	{
		delete this->statement;
	}
	if (this->result != nullptr)
	{
		delete this->result;
	}
}
