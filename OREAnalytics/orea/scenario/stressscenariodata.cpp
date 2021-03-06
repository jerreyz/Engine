/*
 Copyright (C) 2017 Quaternion Risk Management Ltd
 All rights reserved.

 This file is part of ORE, a free-software/open-source library
 for transparent pricing and risk analysis - http://opensourcerisk.org

 ORE is free software: you can redistribute it and/or modify it
 under the terms of the Modified BSD License.  You should have received a
 copy of the license along with this program.
 The license is also available online at <http://opensourcerisk.org>

 This program is distributed on the basis that it will form a useful
 contribution to risk analytics and model standardisation, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. See the license for more details.
*/

#include <orea/scenario/stressscenariodata.hpp>
#include <ored/utilities/xmlutils.hpp>
#include <ored/utilities/log.hpp>

using namespace QuantLib;

namespace ore {
namespace analytics {

void StressTestScenarioData::fromXML(XMLNode* root) {
    data_.clear();

    XMLNode* node = XMLUtils::locateNode(root, "StressTesting");
    XMLUtils::checkNode(node, "StressTesting");

    for (XMLNode* testCase = XMLUtils::getChildNode(node, "StressTest"); testCase;
         testCase = XMLUtils::getNextSibling(testCase)) {

        StressTestData test;
        test.label = XMLUtils::getAttribute(testCase, "id");
        // XMLUtils::getChildValue(testCase, "Label", true);

        LOG("Load stress test label " << test.label);

        LOG("Get discount curve shift parameters");
        XMLNode* discountCurves = XMLUtils::getChildNode(testCase, "DiscountCurves");
        QL_REQUIRE(discountCurves, "DiscountCurves node not found");
        test.discountCurveShifts.clear();
        for (XMLNode* child = XMLUtils::getChildNode(discountCurves, "DiscountCurve"); child;
             child = XMLUtils::getNextSibling(child)) {
            string ccy = XMLUtils::getAttribute(child, "ccy");
            LOG("Loading stress parameters for discount curve for ccy " << ccy);
            CurveShiftData data;
            data.shiftType = XMLUtils::getChildValue(child, "ShiftType", true);
            data.shifts = XMLUtils::getChildrenValuesAsDoublesCompact(child, "Shifts", true);
            data.shiftTenors = XMLUtils::getChildrenValuesAsPeriods(child, "ShiftTenors", true);
            QL_REQUIRE(data.shifts.size() == data.shiftTenors.size(),
                       "number of tenors and shifts does not match in discount curve stress data");
            QL_REQUIRE(data.shifts.size() > 0, "no shifts provided in discount curve stress data");
            test.discountCurveShifts[ccy] = data;
        }

        LOG("Get index curve stress parameters");
        XMLNode* indexCurves = XMLUtils::getChildNode(testCase, "IndexCurves");
        QL_REQUIRE(indexCurves, "IndexCurves node not found");
        test.indexCurveShifts.clear();
        for (XMLNode* child = XMLUtils::getChildNode(indexCurves, "IndexCurve"); child;
             child = XMLUtils::getNextSibling(child)) {
            string index = XMLUtils::getAttribute(child, "index");
            LOG("Loading stress parameters for index " << index);
            // same as discount curve sensitivity loading from here ...
            CurveShiftData data;
            data.shiftType = XMLUtils::getChildValue(child, "ShiftType", true);
            data.shifts = XMLUtils::getChildrenValuesAsDoublesCompact(child, "Shifts", true);
            data.shiftTenors = XMLUtils::getChildrenValuesAsPeriods(child, "ShiftTenors", true);
            QL_REQUIRE(data.shifts.size() == data.shiftTenors.size(),
                       "number of tenors and shifts does not match in index curve stress data");
            QL_REQUIRE(data.shifts.size() > 0, "no shifts provided in index curve stress data");
            test.indexCurveShifts[index] = data;
        }

        LOG("Get yield curve stress parameters");
        XMLNode* yieldCurves = XMLUtils::getChildNode(testCase, "YieldCurves");
        QL_REQUIRE(yieldCurves, "YieldCurves node not found");
        test.yieldCurveShifts.clear();
        for (XMLNode* child = XMLUtils::getChildNode(yieldCurves, "YieldCurve"); child;
             child = XMLUtils::getNextSibling(child)) {
            string name = XMLUtils::getAttribute(child, "name");
            LOG("Loading stress parameters for yield curve " << name);
            // same as discount curve sensitivity loading from here ...
            CurveShiftData data;
            data.shiftType = XMLUtils::getChildValue(child, "ShiftType", true);
            data.shifts = XMLUtils::getChildrenValuesAsDoublesCompact(child, "Shifts", true);
            data.shiftTenors = XMLUtils::getChildrenValuesAsPeriods(child, "ShiftTenors", true);
            QL_REQUIRE(data.shifts.size() == data.shiftTenors.size(),
                       "number of tenors and shifts does not match in yield curve stress data");
            QL_REQUIRE(data.shifts.size() > 0, "no shifts provided in yield curve stress data");
            test.yieldCurveShifts[name] = data;
        }

        LOG("Get FX spot stress parameters");
        XMLNode* fxSpots = XMLUtils::getChildNode(testCase, "FxSpots");
        QL_REQUIRE(fxSpots, "FxSpots node not found");
        test.fxShifts.clear();
        for (XMLNode* child = XMLUtils::getChildNode(fxSpots, "FxSpot"); child;
             child = XMLUtils::getNextSibling(child)) {
            string ccypair = XMLUtils::getAttribute(child, "ccypair");
            LOG("Loading stress parameters for FX " << ccypair);
            FxShiftData data;
            data.shiftType = XMLUtils::getChildValue(child, "ShiftType", true);
            data.shiftSize = XMLUtils::getChildValueAsDouble(child, "ShiftSize", true);
            test.fxShifts[ccypair] = data;
        }

        LOG("Get fx vol stress parameters");
        XMLNode* fxVols = XMLUtils::getChildNode(testCase, "FxVolatilities");
        QL_REQUIRE(fxVols, "FxVols node not found");
        test.fxVolShifts.clear();
        for (XMLNode* child = XMLUtils::getChildNode(fxVols, "FxVolatility"); child;
             child = XMLUtils::getNextSibling(child)) {
            string ccypair = XMLUtils::getAttribute(child, "ccypair");
            LOG("Loading stress parameters for FX vols " << ccypair);
            FxVolShiftData data;
            data.shiftType = XMLUtils::getChildValue(child, "ShiftType");
            data.shifts = XMLUtils::getChildrenValuesAsDoublesCompact(child, "Shifts", true);
            data.shiftExpiries = XMLUtils::getChildrenValuesAsPeriods(child, "ShiftExpiries", true);
            test.fxVolShifts[ccypair] = data;
        }

        LOG("Get swaption vol stress parameters");
        XMLNode* swaptionVols = XMLUtils::getChildNode(testCase, "SwaptionVolatilities");
        QL_REQUIRE(swaptionVols, "SwaptionVols node not found");
        test.swaptionVolShifts.clear();
        for (XMLNode* child = XMLUtils::getChildNode(swaptionVols, "SwaptionVolatility"); child;
             child = XMLUtils::getNextSibling(child)) {
            string ccy = XMLUtils::getAttribute(child, "ccy");
            LOG("Loading stress parameters for swaption vols " << ccy);
            SwaptionVolShiftData data;
            data.shiftType = XMLUtils::getChildValue(child, "ShiftType", true);
            data.shiftTerms = XMLUtils::getChildrenValuesAsPeriods(child, "ShiftTerms", true);
            data.shiftExpiries = XMLUtils::getChildrenValuesAsPeriods(child, "ShiftExpiries", true);
            XMLNode* shiftSizes = XMLUtils::getChildNode(child, "Shifts");
            data.parallelShiftSize = 0.0;
            for (XMLNode* child2 = XMLUtils::getChildNode(shiftSizes, "Shift"); child2;
                 child2 = XMLUtils::getNextSibling(child2)) {
                string expiry = XMLUtils::getAttribute(child2, "expiry");
                string term = XMLUtils::getAttribute(child2, "term");
                if (expiry == "" && term == "")
                    data.parallelShiftSize = ore::data::parseReal(XMLUtils::getNodeValue(child2));
                else {
                    QL_REQUIRE(expiry != "" && term != "", "expiry and term attributes required on shift size nodes");
                    Period e = ore::data::parsePeriod(expiry);
                    Period t = ore::data::parsePeriod(term);
                    Real value = ore::data::parseReal(XMLUtils::getNodeValue(child2));
                    pair<Period, Period> key(e, t);
                    data.shifts[key] = value;
                }
            }
            test.swaptionVolShifts[ccy] = data;
        }

        LOG("Get cap/floor vol stress parameters");
        XMLNode* capVols = XMLUtils::getChildNode(testCase, "CapFloorVolatilities");
        QL_REQUIRE(capVols, "CapVols node not found");
        for (XMLNode* child = XMLUtils::getChildNode(capVols, "CapFloorVolatility"); child;
             child = XMLUtils::getNextSibling(child)) {
            string ccy = XMLUtils::getAttribute(child, "ccy");
            CapFloorVolShiftData data;
            data.shiftType = XMLUtils::getChildValue(child, "ShiftType", true);
            data.shiftExpiries = XMLUtils::getChildrenValuesAsPeriods(child, "ShiftExpiries", true);
            data.shifts = XMLUtils::getChildrenValuesAsDoublesCompact(child, "Shifts", true);
            test.capVolShifts[ccy] = data;
        }

        data_.push_back(test);

        LOG("Loading stress test label " << test.label << " done");
    }

    LOG("Loading stress tests done");
}

XMLNode* StressTestScenarioData::toXML(XMLDocument& doc) {
    XMLNode* node = doc.allocNode("StressTesting");
    QL_FAIL("toXML not implemented for stress testing data");
    return node;
}
}
}
