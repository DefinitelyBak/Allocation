#pragma once

#include "DbTables.h"


namespace Allocation::Adapters::Database
{

    void InitDatabase(Poco::Data::Session& session, const std::string& dbType)
    {
        std::string idColumn;

        if (dbType == "postgresql")
        {
            session << "SET search_path TO public", Poco::Data::Keywords::now;
            idColumn = "id SERIAL PRIMARY KEY";
        }
        else if (dbType == "SQLite")
        {
            session << "PRAGMA foreign_keys = ON", Poco::Data::Keywords::now;
            idColumn = "id INTEGER PRIMARY KEY AUTOINCREMENT";
        }
        else
            idColumn = "id INTEGER PRIMARY KEY";

        // order_lines
        std::string sqlOrderLines =
            "CREATE TABLE IF NOT EXISTS order_lines ("
            + idColumn + ", "
            "sku VARCHAR(255), "
            "qty INTEGER NOT NULL, "
            "orderid VARCHAR(255))";
        session << sqlOrderLines, Poco::Data::Keywords::now;

        // products
        session << R"(
            CREATE TABLE IF NOT EXISTS products (
                sku VARCHAR(255) PRIMARY KEY NOT NULL,
                version_number INTEGER NOT NULL DEFAULT 0
            )
        )", Poco::Data::Keywords::now;

        // batches
        std::string sqlBatches =
            "CREATE TABLE IF NOT EXISTS batches ("
            + idColumn + ", "
            "reference VARCHAR(255), "
            "sku VARCHAR(255) NOT NULL, "
            "purchased_quantity INTEGER NOT NULL, "
            "eta DATE, "
            "FOREIGN KEY(sku) REFERENCES products(sku))";
        session << sqlBatches, Poco::Data::Keywords::now;

        // allocations
        std::string sqlAllocations =
            "CREATE TABLE IF NOT EXISTS allocations ("
            + idColumn + ", "
            "orderline_id INTEGER NOT NULL, "
            "batch_id INTEGER NOT NULL, "
            "FOREIGN KEY(orderline_id) REFERENCES order_lines(id), "
            "FOREIGN KEY(batch_id) REFERENCES batches(id))";
        session << sqlAllocations, Poco::Data::Keywords::now;
    }

    void DropDatabase(Poco::Data::Session& session)
    {
        session << "DROP TABLE IF EXISTS allocations", Poco::Data::Keywords::now;
        session << "DROP TABLE IF EXISTS batches", Poco::Data::Keywords::now;
        session << "DROP TABLE IF EXISTS order_lines", Poco::Data::Keywords::now;
        session << "DROP TABLE IF EXISTS products", Poco::Data::Keywords::now;
    }
}