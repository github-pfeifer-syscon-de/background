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
#include <cstdint>
#include <fstream>
#include <filesystem>
#include <bit>
#include <vector>
#include <format>

#include "ShapeFile.hpp"

// according to
// https://www.esri.com/content/dam/esrisites/sitecore-archive/Files/Pdfs/library/whitepapers/pdfs/shapefile.pdf
template<typename T>
static inline T fromBigEndian(T val) {
    if constexpr (std::endian::native == std::endian::little) {
        if (sizeof(T) == 4) {
            return static_cast<T>(__builtin_bswap32(val));      // gcc function
        }
        if (sizeof(T) == 8) {
            return static_cast<T>(__builtin_bswap64(val));
        }
        static_assert("Not implemented size fromBigEndian");
    }
    return val;
}
template<typename T>
static inline T fromLittleEndian(T val) {
    if constexpr (std::endian::native == std::endian::big) {
        if (sizeof(T) == 4) {
            return static_cast<T>(__builtin_bswap32(val));      // gcc function
        }
        if(sizeof(T) == 8) {
            return static_cast<T>(__builtin_bswap64(val));
        }
        static_assert("Not implemented size fromLittleEndian");
    }
    return val;
}

#pragma pack(push, 4)   // this is the most portable way to do this, 4 as the fundamental datatype is int32
struct ShapeHeader {
                                //Position Field Value Type Order
    int32_t Code;               // Byte 0 File Code  9994 Integer Big
    int32_t unused0;            // Byte 4 Unused 0 Integer Big
    int32_t unused1;            // Byte 8 Unused 0 Integer Big
    int32_t unused2;            // Byte 12 Unused 0 Integer Big
    int32_t unused3;            // Byte 16 Unused 0 Integer Big
    int32_t unused4;            // Byte 20 Unused 0 Integer Big
    int32_t FileLength;         // Byte 24 File Length File Length Integer Big
    int32_t Version;            // Byte 28 Version 1000 Integer Little
    int32_t ShapeType;          // Byte 32 Shape Type Shape Type Integer Little
    double BoundingBoxXmin;     //Byte 36 Bounding Box Xmin Double Little
    double BoundingBoxYmin;     // Byte 44 Bounding Box Ymin Double Little
    double BoundingBoxXmax;     // Byte 52 Bounding Box Xmax Double Little
    double BoundingBoxYmax;     // Byte 60 Bounding Box Ymax Double Little
    double BoundingBoxZmin;     // Byte 68* Bounding Box Zmin Double Little
    double BoundingBoxZmax;     // Byte 76* Bounding Box Zmax Double Little
    double BoundingBoxMmin;     // Byte 84* Bounding Box Mmin Double Little
    double BoundingBoxMmax;     // Byte 92* Bounding Box Mmax Double Little
    int32_t getCode() {
        return fromBigEndian(Code);
    }
    int32_t getFileLength() {
        return fromBigEndian(FileLength);
    }
    int32_t getVersion() {
        return fromLittleEndian(Version);
    }
    int32_t getShapeType() {
        return fromLittleEndian(ShapeType);
    }
    double getBoundingBoxXmin() {
        return fromLittleEndian(BoundingBoxXmin);
    }
    double getBoundingBoxXmax() {
        return fromLittleEndian(BoundingBoxXmax);
    }
    double getBoundingBoxYmin() {
        return fromLittleEndian(BoundingBoxYmin);
    }
    double getBoundingBoxYmax() {
        return fromLittleEndian(BoundingBoxYmax);
    }
    static constexpr int64_t HEADER_SIZE_BYTES{100};
    int64_t read(std::ifstream& file) {
        file.read(reinterpret_cast<char*>(this), sizeof(ShapeHeader));
        auto pos = file.tellg();
        if (pos != HEADER_SIZE_BYTES) {
            auto msg = std::format("Expected position {} found {} ShapeHeader check alignment!", HEADER_SIZE_BYTES, static_cast<int64_t>(pos));
            throw std::runtime_error(msg);
        }
        return HEADER_SIZE_BYTES;
    }
};

struct ShapeType {
    int32_t ShapeType;      // Byte 0 Shape Type 0 Integer 1 Little
    int32_t getShapeType() {
        return fromLittleEndian(ShapeType);
    }
    static constexpr int64_t SHAPRETYPE_SIZE_BYTES{4};
    int64_t read(std::ifstream& file, int64_t posIn) {
        file.read(reinterpret_cast<char*>(this), sizeof(ShapeType));
        posIn += SHAPRETYPE_SIZE_BYTES;
        auto posRead = file.tellg();
        if (posIn!= posRead) {
            auto msg = std::format("Expected position {} found {} ShapeType check alignment!", posIn, static_cast<int64_t>(posRead));
            throw std::runtime_error(msg);
        }
        return posIn;
    }

};

struct PointShape {
    double x;
    double y;
    static constexpr int32_t TYPE{1};
    static constexpr int64_t POINTSHAPE_BYTE_SIZE{16};
    int64_t read(std::ifstream& file, int64_t posIn) {
        file.read(reinterpret_cast<char*>(this), sizeof(PointShape));
        std::cout << "x " << getX()
                  << "\ny " << getY() << std::endl;
        posIn += POINTSHAPE_BYTE_SIZE;
        auto posRead = file.tellg();
        if (posIn!= posRead) {
            auto msg = std::format("Expected position {} found {} ShapeRecordHeader check alignment!", posIn, static_cast<int64_t>(posRead));
            throw std::runtime_error(msg);
        }
        return posIn;
    }
    double getX() {
        return fromLittleEndian(x);
    }
    double getY() {
        return fromLittleEndian(y);
    }
};

struct PolyLine {
double xMin;
double yMin;
double xMax;
double yMax;
static constexpr int32_t TYPE{3};
};

struct PolygonShape {
    double xMin;
    double yMin;
    double xMax;
    double yMax;
    int32_t parts;
    int32_t points;
    static constexpr int32_t TYPE{5};
    static constexpr int64_t POLYGOON_FIX_BYTE_SIZE{40};
    int64_t read(std::ifstream& file, int64_t posIn)
    {
        file.read(reinterpret_cast<char*>(this), sizeof(PolygonShape));
        std::cout << "x " << getXMin()
                  << "\ny " << getYMin() << std::endl;
        posIn += POLYGOON_FIX_BYTE_SIZE;
        auto posRead = file.tellg();
        if (posIn!= posRead) {
            auto msg = std::format("Expected position {} found {} PolygonShape check alignment!", posIn, static_cast<int64_t>(posRead));
            throw std::runtime_error(msg);
        }
        size_t localParts = getParts();
        std::vector<int32_t> partIdx;
        partIdx.resize(localParts);
        if (localParts > 1) {
            std::cout << "Offset " << static_cast<int32_t>(&partIdx[1] - &partIdx[0]) << std::endl;
        }
        auto readPartsBytes = sizeof(int32_t) * localParts;
        posIn += readPartsBytes;
        file.read(reinterpret_cast<char*>(partIdx.data()), readPartsBytes);
        size_t localPoints = getPoints();
        std::vector<double> points;
        points.resize(localPoints * 2u);
        auto readPointsBytes = sizeof(double) * localPoints * 2u;
        posIn += readPointsBytes;
        file.read(reinterpret_cast<char*>(points.data()), readPointsBytes);
        return posIn;
    }
    double getXMin() {
        return fromLittleEndian(xMin);
    }
    double getYMin() {
        return fromLittleEndian(yMin);
    }
    int32_t getParts() {
        return fromLittleEndian(parts);
    }
    int32_t getPoints() {
        return fromLittleEndian(points);
    }
};

struct ShapeIndexRecordHeader
{
    int32_t Offset;      // Byte 0 Offset Integer Big
    int32_t ContentLength;     // Byte 4 Content Length Integer Big  /in 16-bit words
    int32_t getOffset() {
        return fromBigEndian(Offset);
    }
    int32_t getContentLength() {
        return fromBigEndian(ContentLength);
    }
    static constexpr int64_t RECORDHEADER_SIZE_BYTES{8};
    int64_t read(std::ifstream& file, int64_t posIn) {
        file.read(reinterpret_cast<char*>(this), sizeof(ShapeIndexRecordHeader));
        std::cout << "Offset " << getOffset()
                  << "\nLength " << getContentLength() << std::endl;
        posIn += RECORDHEADER_SIZE_BYTES;
        auto posRead = file.tellg();
        if (posIn!= posRead) {
            auto msg = std::format("Expected position {} found {} ShapeRecordHeader check alignment!", posIn, static_cast<int64_t>(posRead));
            throw std::runtime_error(msg);
        }
        return posIn;
    }
};

struct ShapeRecordHeader {
    int32_t RecordNumber;      // Byte 0 Record Number  Integer Big
    int32_t ContentLength;     // Byte 4 Content Length Integer Big   /in 16-bit words
    int32_t getRecordNumber() {
        return fromBigEndian(RecordNumber);
    }
    int32_t getContentLength() {
        return fromBigEndian(ContentLength);
    }
    static constexpr int64_t RECORDHEADER_SIZE_BYTES{8};
    static constexpr int32_t NULL_SHAPE{0};
    int64_t read(std::ifstream& file, int64_t posIn) {
        file.read(reinterpret_cast<char*>(this), sizeof(ShapeRecordHeader));
        std::cout << "RecordNo " << getRecordNumber()
                  << "\nLength " << getContentLength() << std::endl;
        posIn += RECORDHEADER_SIZE_BYTES;
        auto expectdRecEnd = posIn + getContentLength() * 2u;
        auto posRead = file.tellg();
        if (posIn!= posRead) {
            auto msg = std::format("Expected position {} found {} ShapeRecordHeader check alignment!", posIn, static_cast<int64_t>(posRead));
            throw std::runtime_error(msg);
        }
        ShapeType shapeType;
        posIn = shapeType.read(file, posIn);
        std::cout << "Shapetype " << shapeType.getShapeType() << std::endl;
        switch (shapeType.getShapeType()) {
        case NULL_SHAPE:
            std::cout << "NullShape" << std::endl;
            break;
        case PointShape::TYPE: {
            PointShape pointShape;
            posIn = pointShape.read(file, posIn);
            }
            break;
        case PolygonShape::TYPE: {
            PolygonShape polyShape;
            posIn = polyShape.read(file, posIn);
            }
            break;
        default: {
            auto msg = std::format("Shape type {} not implemented ", shapeType.getShapeType());
            throw std::runtime_error(msg);
            }
        }
        auto readPos = file.tellg();
        if (posIn != readPos || expectdRecEnd != readPos) {
            auto msg = std::format("Expected position {} recordEnd {} found {} Shape check alignment!", posIn, expectdRecEnd, static_cast<int64_t>(posRead));
            throw std::runtime_error(msg);
        }
        // is there some padding ???  there might be a mismatch of record size and content???
        return posIn;
    }
};
#pragma pack(pop)

static void
readShp(std::filesystem::path shpPath)
{
    shpPath += ".shp";
    std::ifstream shpFile(shpPath.string());
    if (!shpFile.good()) {
        std::cout << "Error opening " << shpPath.string() << std::endl;
    }
    struct ShapeHeader shapeHeader{};
    auto pos = shapeHeader.read(shpFile);
    std::cout << "Code " << shapeHeader.getCode()
              << "\nVersion " << shapeHeader.getVersion()
              << "\nFileLen(16bit) " << shapeHeader.getFileLength()
              << "\nShape " << shapeHeader.getShapeType()
              << "\nxMin" << shapeHeader.getBoundingBoxXmin()
              << "\nxMax" << shapeHeader.getBoundingBoxXmax()
              << "\nyMin" << shapeHeader.getBoundingBoxYmin()
              << "\nyMax" << shapeHeader.getBoundingBoxYmax() << std::endl;
    auto fileEndBytes = shapeHeader.getFileLength() * 2u;
    while (pos < fileEndBytes) {
        struct ShapeRecordHeader shapeRecordHeader;
        pos = shapeRecordHeader.read(shpFile, pos);
    }
    shpFile.close();
}

static void
readShx(std::filesystem::path shxPath)
{
    shxPath += ".shx";
    std::ifstream shxFile(shxPath.string());
    if (!shxFile.good()) {
        std::cout << "Error opening " << shxPath.string() << std::endl;
    }
    struct ShapeHeader shapeHeader{};
    auto pos = shapeHeader.read(shxFile);
    std::cout << "Code " << shapeHeader.getCode()
              << "\nVersion " << shapeHeader.getVersion()
              << "\nFileLen(16bit) " << shapeHeader.getFileLength()
              << "\nShape " << shapeHeader.getShapeType()
              << "\nxMin" << shapeHeader.getBoundingBoxXmin()
              << "\nxMax" << shapeHeader.getBoundingBoxXmax()
              << "\nyMin" << shapeHeader.getBoundingBoxYmin()
              << "\nyMax" << shapeHeader.getBoundingBoxYmax() << std::endl;
    auto fileEndBytes = shapeHeader.getFileLength() * 2u;
    while (pos < fileEndBytes) {
        struct ShapeIndexRecordHeader shapeIndexRecordHeader;
        pos = shapeIndexRecordHeader.read(shxFile, pos);
    }
    shxFile.close();
}

ShapeFile::ShapeFile(const std::string& dir)
{
    std::filesystem::path dirPath(dir);
    dirPath /= "GN250_p";
    // the record index of .dbf .shp .shx match
    readShp(dirPath);
    readShx(dirPath);
}
