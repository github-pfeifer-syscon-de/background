/*
 * Copyright (C) 2024 RPf 
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
#include <memory>
#include <cstdlib>
#include <cmath>
#include <StringUtils.hpp>

#include "JulianDate.hpp"
#include "GeoPosition.hpp"
#include "RaDec.hpp"
#include "Math.hpp"
#include "Planets.hpp"
#include "MessierLoader.hpp"
#include "FileLoader.hpp"
//#include "Phase.hpp"
//#include "HaruRenderer.hpp"

static constexpr auto expAz = 155.96;
static constexpr auto expAlt = 69.0103;
static constexpr auto raExp = 2.09998;
static constexpr auto decExp = 11.4938;
static constexpr auto distExp = 1.70358;
static constexpr auto numLowError = 0.00001;
static constexpr auto numHighError = 0.001;
static constexpr auto numHighestError = 0.1;

static bool
test_azAlt()
{
    JulianDate jd{2459349.210248739};
    GeoPosition geoPos{274.236400, 38.2464000};
    auto raDec = std::make_shared<RaDec>();
    raDec->setRaHours(3.4299545354);
    raDec->setDecDegrees(18.726708585);
    auto azAlt = geoPos.toAzimutAltitude(raDec, jd);
    std::cout << "is  az " << azAlt->getAzimutDegrees() << " alt " << azAlt->getAltitudeDegrees() << std::endl;
    std::cout << "exp az " << expAz << " alt " << expAlt << std::endl;
    if (std::abs(azAlt->getAzimutDegrees()  - expAz) > numHighestError ||
        std::abs(azAlt->getAltitudeDegrees()  - expAlt) > numHighestError) {
        return false;
    }
    return true;
}


static bool
test_deg2rad()
{
    auto rad = Math::toRadians(10.0);
    std::cout << "   deg rad " << rad << std::endl;
    if (std::abs(rad - 0.174533) > numLowError) {
        return false;
    }
    auto deg = Math::toDegrees(rad);
    std::cout << "is  deg " << deg << std::endl;
    std::cout << "exp deg " << 10.0 << std::endl;
    if (std::abs(deg - 10.0) > numHighError) {
        return false;
    }
    return true;
}

static bool
test_hour2rad()
{
    auto rad = Math::toRadianHours(10.0);
    std::cout << "   hour rad " << rad << std::endl;
    if (std::abs(rad - 2.61799) > numLowError) {
        return false;
    }
    auto hour = Math::toHoursRadian(rad);
    std::cout << "is  hour " << hour << std::endl;
    std::cout << "exp hour " << 10.0 << std::endl;
    if (std::abs(hour - 10.0) > numHighError) {
        return false;
    }
    return true;
}

static bool
test_planet()
{
    // see https://celestialprogramming.com/planets_with_keplers_equation.html
    Glib::DateTime date = Glib::DateTime::create_utc(2024, 5, 2, 10, 21, 0);
    JulianDate jd{date};
    Planets planets;
    auto venus = planets.find("Venus");
    auto raDec = venus->getRaDecPositon(jd);
    std::cout << "Venus ra " << raDec->getRaHours()
              << " dec " << raDec->getDecDegrees()
              << " dist " << raDec->getDistanceAU() << std::endl;
    if (std::abs(raDec->getRaHours() - raExp) > numHighError ||
        std::abs(raDec->getDecDegrees() - decExp) > numHighError ||
        std::abs(raDec->getDistanceAU() - distExp) > numHighestError) {
        return false;
    }
    return true;
}

static bool
test_dms()
{
    double dec = MessierLoader::toDecimal("17:40:20.75");
    std::cout << "toDecimal val " << dec << std::endl;
    if (std::abs(dec - 17.672431) > numLowError) {
        return false;
    }
    dec = MessierLoader::toDecimal("-13:03:13.1");
    std::cout << "toDecimal val " << dec << std::endl;
    if (std::abs(dec - -13.053639) > numLowError) {
        return false;
    }
    return true;
}



static bool
test_mix()
{
    auto r = Math::mix(3.0, 7.0, 0.5);
    std::cout << "mix " << r << std::endl;
    if (std::abs(r - 5.0) > numLowError) {
        return false;
    }
    r = Math::mix(7.0, 3.0, 0.5);
    std::cout << "mix " << r << std::endl;
    if (std::abs(r - 5.0) > numLowError) {
        return false;
    }
    r = Math::mix(7.0, 3.0, 1.5);
    std::cout << "mix " << r << std::endl;
    if (std::abs(r - 3.0) > numLowError) {
        return false;
    }

    return true;
}

static bool
test_read()
{
    std::vector<std::string> lines;
    try {
        auto dir = Glib::get_current_dir();
        auto fileName = Glib::canonicalize_filename("data_test.txt", dir);
        auto file = Gio::File::create_for_path(fileName);
        Glib::RefPtr<Gio::FileOutputStream> fileStrm;
        if (!file->query_exists()) {
            fileStrm = file->create_file(Gio::FileCreateFlags::FILE_CREATE_NONE);
        }
        else {
            fileStrm = file->append_to(Gio::FileCreateFlags::FILE_CREATE_REPLACE_DESTINATION);
            fileStrm->truncate(0);
        }
        std::string str; // {"\xd6\xdc\xdf\xc4\xe4\xf6"};
        str += (unsigned char)0xd6;//Ö write some iso-8859-15 compatible data
        str += (unsigned char)0xdc;//Ü
        str += (unsigned char)0xdf;//ß
        str += (unsigned char)0xc4;//Ä
        str += (unsigned char)0xe4;//ä
        str += (unsigned char)0xf6;//ö
        fileStrm->write(str);
        fileStrm->close();

        LineReaderEnc lineReaderEnc(file, "ISO-8859-15", true);
        Glib::ustring data;
        bool first = lineReaderEnc.next(data);
        std::cout << "data " << data << std::endl;
        file->remove();
        if (!first) {
            std::cout << "Read not the expected start " << std::endl;
            return false;
        }
        Glib::ustring exp = StringUtils::u8str(u8"ÖÜßÄäö");
        if (data != exp) {
            std::cout << "Read not the expected data " << data << std::endl;
            return false;
        }
        bool next = lineReaderEnc.next(data);
        if (next) {
            std::cout << "Read not the expected end " << std::endl;
            return false;
        }

        return true;
    }
    catch (const Glib::Error& err) {
        std::cout << "Error " << err.what() << std::endl;
        return false;
    }

    return true;
}

// if you want to debug the phase display use this
//static bool
//test_pdf()
//{
//    auto pdfExport = std::make_shared<PdfExport>();
//    auto page = std::make_shared<PdfPage>(pdfExport);
////
//    auto width = page->getWidth();
//    auto height = page->getHeight();
//    auto radius{(width - 40.0f) / 2.0f};
//    Phase phase(0.5);
//
//    HaruRenderer haruRenderer;
//    haruRenderer.translate(width / 2.0f, height / 2.0f);
//    haruRenderer.showPhase(phase, 0.0, 0.0, radius);
//
//    haruRenderer.save("moon_phase.pdf");
//
//    return true;
//}

// activate if function is getting used
//static bool
//test_read2()
//{
//    std::vector<Glib::ustring> lines;
//    try {
//        auto dir = Glib::get_current_dir();
//        auto fileName = Glib::canonicalize_filename("data_test2.txt", dir);
//        auto file = Gio::File::create_for_path(fileName);
//        Glib::RefPtr<Gio::FileOutputStream> fileStrm;
//        if (!file->query_exists()) {
//            fileStrm = file->create_file(Gio::FileCreateFlags::FILE_CREATE_NONE);
//        }
//        else {
//            fileStrm = file->append_to(Gio::FileCreateFlags::FILE_CREATE_REPLACE_DESTINATION);
//            fileStrm->truncate(0);
//        }
//        std::string str; // {"\xd6\xdc\xdf\xc4\xe4\xf6"};
//        str += (unsigned char)0xd6;//Ö write some iso-8859-15 compatible data
//        str += (unsigned char)0xdc;//Ü
//        str += (unsigned char)0xdf;//ß
//        str += (unsigned char)0xc4;//Ä
//        str += (unsigned char)0xe4;//ä
//        str += (unsigned char)0xf6;//ö
//        fileStrm->write(str);
//        fileStrm->close();
//
//        lines = FileLoader::readLines(file, 8, "ISO-8859-15", true);
//        std::cout << "lines " << lines.size() << std::endl;
//        file->remove();
//        if (lines.size() != 1) {
//            std::cout << "Read not the expected count " << lines.size() << std::endl;
//            return false;
//        }
//        Glib::ustring exp = StringUtils::u8str(u8"ÖÜßÄäö");
//        if (lines[0] != exp) {
//            std::cout << "Read not the expected data " << lines[0] << std::endl;
//            return false;
//        }
//
//        return true;
//    }
//    catch (const Glib::Error& err) {
//        std::cout << "Error " << err.what() << std::endl;
//        return false;
//    }
//
//    return true;
//}

/*
 *
 */
int main(int argc, char** argv)
{
    //std::locale::global(std::locale("de_DE.ISO-8859-15@euro"));
    //std::cout << "Loc " << std::setlocale(LC_ALL, nullptr) << std::endl;
    std::setlocale(LC_ALL, "");      // make locale dependent, and make glib accept u8 const !!!
    Glib::init();
    Gio::init();
    if (!test_azAlt()) {
        return 1;
    }
    if (!test_deg2rad()) {
        return 2;
    }
    if (!test_hour2rad()) {
        return 3;
    }
    if (!test_planet()) {
        return 4;
    }
    if (!test_dms()) {
        return 5;
    }
    if (!test_mix()) {
        return 6;
    }
    if (!test_read()) {
        return 7;
    }
    //if (!test_pdf()) {
    //    return 8;
    //}
    return 0;
}
