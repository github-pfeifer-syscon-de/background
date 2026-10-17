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

#include <iostream>
#include <algorithm>
#include <format>

#include "DbfFile.hpp"
#include "libdbf.h"         // better keep this file local as it redefines some common pieces

namespace psc::dbf {
// Infos about dbf structure
// http://www.manmrk.net/tutorials/database/xbase/dbf.html
DbfFile::DbfFile(const std::string& name)
{
    m_dbf = dbf_Open(name.c_str());
    if (m_dbf == nullptr) {
        throw DbfException(std::format("The dbf file {} was not found!", name));
    }
    auto cols = dbf_NumCols(m_dbf);
    m_columns.reserve(cols);
    size_t offs{1};
    for (int col = 0; col < cols; ++col) {
        m_columns.emplace_back(
            std::move(
                std::make_shared<DbfColumn>(this, col, offs)));
        auto size = dbf_ColumnSize(m_dbf, col);
        offs += static_cast<size_t>(size);
    }
}

DbfFile::~DbfFile()
{
    dbf_Close(m_dbf);
}

std::string
DbfFile::getStringVersion()
{
    return dbf_GetStringVersion(m_dbf);
}

int
DbfFile::getNumRows()
{
    return dbf_NumRows(m_dbf);
}

size_t
DbfFile::getNumCols()
{
    return m_columns.size();
}

PtrDbfColumn
DbfFile::getColumn(size_t col)
{
    return m_columns[col];
}

PtrDbfRecord
DbfFile::getRecord()
{
    return std::make_shared<DbfRecord>(this);
}

P_DBF*
DbfFile::getDbf()
{
    return m_dbf;
}

std::string
DbfFile::info()
{
    std::string info;
    info.reserve(1024);
    for (size_t col = 0; col < getNumCols(); ++col) {
        auto column = getColumn(col);
        info += std::format("{}[{}]{}\n", column->getName(), column->getSize(), column->getType());
    }
    return info;
}

DbfColumn::DbfColumn(DbfFile* dbfFile, int col, size_t offs)
: m_dbfFile{dbfFile}
, m_col{col}
, m_offs{offs}
{
    P_DBF* dbf = dbfFile->getDbf();
    //auto addr = dbf_ColumnAddress(dbf, m_col); for clobs/blobs?
    m_name = dbf_ColumnName(dbf, m_col);
    m_type = dbf_ColumnType(dbf, m_col);
    m_size = dbf_ColumnSize(dbf, m_col);
    m_decimals = dbf_ColumnDecimals(dbf, m_col);
    //std::printf("Column %d %16s[%d]%c\n", m_col, m_name.c_str(), m_size, m_type);
}

std::string
DbfColumn::getName()
{
    return m_name;
}

char
DbfColumn::getType()
{
    return m_type;
}

int
DbfColumn::getSize()
{
    return m_size;
}

int
DbfColumn::getDecimals()
{
    return m_decimals;
}

int
DbfColumn::getColumn()
{
    return m_col;
}

size_t
DbfColumn::getOffs()
{
    return m_offs;
}

DbfRecord::DbfRecord(DbfFile* dbfFile)
: m_dbfFile{dbfFile}
{
    P_DBF* dbf = dbfFile->getDbf();
    m_recLen = dbf_RecordLength(dbf);
    m_rec.resize(static_cast<size_t>(m_recLen));
}

// returns the read record or a negative value if this was not possible
int
DbfRecord::read(int rec)
{
    P_DBF* dbf = m_dbfFile->getDbf();
    int r = dbf_SetRecordOffset(dbf, rec + 1);
    if (r < 0) {
        return r;
    }
    //std::printf("setRecordOffs %d %d\n", rec, r);
    return dbf_ReadRecord(dbf, m_rec.data(), static_cast<int>(m_rec.size()));
}

std::string
DbfRecord::getValue(const PtrDbfColumn& column, bool trim)
{
    auto value = m_rec.data();
    std::advance(value, column->getOffs());
    if (trim) { //column->getType() == 'C' && char type, only return usable length
        auto size = dbf_length(value, column->getSize());
        return std::string(value, size);
    }
    return std::string(value, static_cast<size_t>(column->getSize()));
}

double
DbfRecord::getDouble(const PtrDbfColumn& column)
{
    auto val = getValue(column);
    if (val.empty()) {
        return 0.0;
    }
    return std::stod(val);
}

int
DbfRecord::dbf_length(const char* value, const int size)
{
    if (size > 0) {
        // blame it on my incompatibility, but i did not find a range for this...
        auto end = value;
        std::advance(end,size - 1);
        for (int i = size; i > 0; --i,--end) {
            auto c = static_cast<unsigned char>(*end);
            if (c != ' ') {
                return i;
            }
        }
    }
    return 0;
}

} /* namespace psc::dbf */
