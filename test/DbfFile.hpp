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
#include <cstdio>
#include <algorithm>
#include <vector>
#include <memory>

#include "background_config.h"

struct _P_DBF;
typedef struct _P_DBF P_DBF;
namespace psc::dbf {

class DbfColumn;
using PtrDbfColumn = std::shared_ptr<DbfColumn>;
class DbfRecord;
using PtrDbfRecord = std::shared_ptr<DbfRecord>;

class DbfException
: public std::invalid_argument
{
public:
    DbfException(const std::string& msg)
    : std::invalid_argument{msg.c_str()}
    {
    }
    virtual ~DbfException() = default;
};

class DbfFile
{
public:
    DbfFile(const std::string& name);
    virtual ~DbfFile();
    std::string getStringVersion();
    int getNumRows();
    size_t getNumCols();
    PtrDbfColumn getColumn(size_t col);
    PtrDbfRecord getRecord();
    P_DBF* getDbf();
    std::string info();
private:
    P_DBF* m_dbf;
    std::vector<PtrDbfColumn> m_columns;
};


class DbfColumn {
public:
    DbfColumn(DbfFile* dbfFile, int col, size_t offs);
    virtual ~DbfColumn() = default;
    std::string getName();
    char getType();
    int getSize();
    int getDecimals();
    int getColumn();
    size_t getOffs();
private:
    DbfFile* m_dbfFile;
    int m_col;
    size_t m_offs;
    std::string m_name;
    char m_type;
    int m_size;
    int m_decimals;
};

using PtrDbfColumn = std::shared_ptr<DbfColumn>;

class DbfRecord {
public:
    DbfRecord(DbfFile* dbfFile);
    virtual ~DbfRecord() = default;
    // read record rec starts from 0
    
    int read(int rec);

    // by default fields are padded with blanks
    //   to remove these use the position returned by this function
    static int dbf_length(const char* value, int size);
    // it would be nice to use u8string, but that breaks various places like output by format
    std::string getValue(const PtrDbfColumn& column, bool trim = true);
    double getDouble(const PtrDbfColumn& column);
private:
    DbfFile* m_dbfFile;
    int m_recLen;
    std::vector<char> m_rec;

};



} /* namespace psc::dbf */

