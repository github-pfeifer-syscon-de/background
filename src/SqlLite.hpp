/* -*- Mode: c++; c-basic-offset: 4; tab-width: 4; coding: utf-8; -*-  */
/*
 * Copyright (C) 2026 RPf
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#pragma once

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <sqlite3.h>

struct sqlite3;
struct sqlite3_stmt;

namespace psc::sql {

class SqlLite;
class SqlLiteStmt;

class SqlException
: public std::runtime_error
{
public:
    SqlException(const std::string& msg);
    virtual ~SqlException() = default;

};

class SqlColumn
{
public:
    SqlColumn(const std::string& name, int type, int size = 0, bool nullable = true, bool primaryKey = false)
    : m_name{name}
    , m_type{type}
    , m_size{size}
    , m_nullable{nullable}
    , m_primaryKey{primaryKey}
    {
    }
    explicit SqlColumn(const SqlColumn& other) = delete;
    virtual ~SqlColumn() = default;

    std::string getName() {
        return m_name;
    }
    int getType() {
        return m_type;
    }
    int getSize() {
        return m_size;
    }
    bool isPrimaryKey() {
        return m_primaryKey;
    }
    virtual void addCreateSize(std::string& def) = 0;
    std::string getCreate() {
        std::string def;
        def.reserve(64);
        def += m_name;
        def += ' ';
        switch (m_type) {
        case SQLITE_INTEGER:
            def += "INTEGER";
            break;
        case SQLITE_FLOAT:
            def += "NUMERIC";
            addCreateSize(def);
            break;
        case SQLITE_BLOB:
            def += "BLOB";
            break;
        case SQLITE3_TEXT:
            if (m_size > 255) {
                def += "TEXT";
            }
            else {
                def += "VARCHAR";
                addCreateSize(def);
            }
            break;
        }
        if (m_primaryKey) {
            def += " PRIMARY KEY";
        }
        else if (!m_nullable) {
            def += " NOT NULL";
        }
        return def;
    }
protected:
    std::string m_name;
    int m_type;
    int m_size;
    bool m_nullable;
    bool m_primaryKey;
};

using PtrSqlColumn = std::shared_ptr<SqlColumn>;

class SqlVarcharColumn
: public SqlColumn
{
public:
    SqlVarcharColumn(const std::string& name, int size = 0, bool nullable = true, bool primaryKey = false)
    : SqlColumn(name, SQLITE3_TEXT, size, nullable, primaryKey)
    {
    }
    explicit SqlVarcharColumn(const SqlVarcharColumn& other) = delete;
    virtual ~SqlVarcharColumn() = default;
     void addCreateSize(std::string& def) override {
        if (m_size > 0) {
            def += '(';
            def += std::to_string(m_size);
            def += ')';
        }
    }

};


class SqlNumColumn
: public SqlColumn
{
public:
    SqlNumColumn(const std::string& name, int size, int decimals = 0, bool nullable = true, bool primaryKey = false)
    : SqlColumn(name, SQLITE_FLOAT, size, nullable, primaryKey)
    , m_decimals{decimals}
    {
    }
    explicit SqlNumColumn(const SqlNumColumn& other) = delete;
    virtual ~SqlNumColumn() = default;

    int getDecimals() {
        return m_decimals;
    }
    void addCreateSize(std::string& def) override {
        if (m_size > 0) {
            def += '(';
            def += std::to_string(m_size);
            if (m_decimals > 0) {
                def += ',';
                def += std::to_string(m_decimals);
            }
            def += ')';
        }
    }

private:
    int m_decimals;
};

class SqlTable
;
class SqlIndex
{
public:
    SqlIndex(bool unique, const std::string& name, const std::vector<PtrSqlColumn>& columns)
    : m_unique{unique}
    , m_name{name}
    , m_columns{columns}
    {
    }
    SqlIndex(bool unique, const std::string& name, const PtrSqlColumn& column)
    : m_unique{unique}
    , m_name{name}
    {
        m_columns.push_back(column);
    }
    explicit SqlIndex(const SqlIndex& other) = delete;
    virtual ~SqlIndex() = default;

    std::string getCreate(SqlTable* table);
    std::string getName() {
        return m_name;
    }
private:
    bool m_unique;
    std::string m_name;
    std::vector<PtrSqlColumn> m_columns;
};

using PtrSqlIndex = std::shared_ptr<SqlIndex>;

class SqlTable
{
public:
    SqlTable(const std::string& name)
    : m_name{name}
    {
    }
    explicit SqlTable(const SqlTable& other) = delete;
    virtual ~SqlTable() = default;

    // returns bind index
    size_t addColumns(const PtrSqlColumn& column) {
        m_columns.push_back(column);
        return m_columns.size();
    }
    size_t addIndex(const PtrSqlIndex& index) {
        m_indexes.push_back(index);
        return m_indexes.size();
    }
    std::string getName() {
        return m_name;
    }
    std::string getCreate();
    std::string getInsert();
    std::string getSelect();
private:
    std::string m_name;
    std::vector<PtrSqlColumn> m_columns;
    std::vector<PtrSqlIndex> m_indexes;
};

using PtrSqlTable = std::shared_ptr<SqlTable>;

class SqlLiteStmt
{
public:
    SqlLiteStmt(SqlLite* sqlLite, const std::string& sql);
    explicit SqlLiteStmt(const SqlLiteStmt& other) = delete;
    virtual ~SqlLiteStmt();

    // this is intended for selects
    bool next();
    // this is intended for dml e.g. insert
    //   @return same as sqlite e.g. constrain violated for existing data e.g. SQLITE_CONSTRAINT
    //   @Exception for "unfixable" errors from client side
    int exec();
    int64_t getRowCount();
    std::vector<std::string> getColumnNames();
    std::vector<int> getResultTypes();
    std::string getString(int col);
    double getDouble(int col);
    int64_t getLong(int col);
    std::string getString(const PtrSqlColumn& col);
    double getDouble(const PtrSqlColumn& col);
    int64_t getLong(const PtrSqlColumn& col);
    void setString(int bindIdx, const std::string& value);
    void setDouble(int bindIdx, double value);
protected:
    int getColumnIdx(const std::string& name);

private:
    SqlLite* m_sqlLite;
    std::string m_sql;
    sqlite3_stmt* m_stmt{};
    std::unordered_map<std::string, int32_t> m_nameCols;
};

using PtrSqlLiteStmt = std::unique_ptr<SqlLiteStmt>;

class SqlLite
{
public:
    SqlLite(const std::string& name);
    explicit SqlLite(const SqlLite& other) = delete;
    virtual ~SqlLite();

    // execute immediate e.g. create table goes here
    void execute(const std::string& stmt);
    // execute where a result is expected
    PtrSqlLiteStmt prepareStatement(const std::string& stmt);
    sqlite3* getDb() {
        return m_db;
    }
    void beginTransaction();
    void commit();
    void rollback();
    void create(const PtrSqlTable& table);
    PtrSqlLiteStmt insert(const PtrSqlTable& table);
    PtrSqlLiteStmt select(const PtrSqlTable& table, const std::string& where);
private:
    sqlite3* m_db{};
};

} // namespace psc::sql
