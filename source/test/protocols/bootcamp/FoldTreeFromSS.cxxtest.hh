// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file   test/protocols/match/ProteinSCSampler.cxxtest.hh
/// @brief
/// @author Andrew Leaver-Fay (aleaverfay@gmail.com)


// Test headers
#include <cxxtest/TestSuite.h>
#include <unordered_set>

#include <protocols/match/upstream/ProteinSCSampler.hh>
#include <protocols/match/upstream/OriginalScaffoldBuildPoint.hh>

#include <test/util/pose_funcs.hh>
#include <test/core/init_util.hh>
#include <core/scoring/dssp/Dssp.hh>

// Utility headers
#include <basic/Tracer.hh>

/// Project headers
#include <core/types.hh>
#include <core/kinematics/FoldTree.hh>
#include <core/pose/Pose.hh>
#include <core/kinematics/Edge.hh>
#include <protocols/bootcamp/fold_tree_from_ss.hh>

// C++ headers

//Auto Headers
#include <core/pack/dunbrack/DunbrackRotamer.hh>


using namespace std;
using namespace protocols::match;
using namespace protocols::match::upstream;
using namespace core::kinematics;
using namespace core::scoring;
using namespace core::pose;

static basic::Tracer TR( "protocols.bootcamp.FoldTreeFromSS.cxxtest");
// --------------- Test Class --------------- //

class FoldTreeFromSSTests : public CxxTest::TestSuite {

public:


	// --------------- Fixtures --------------- //

	// Define a test fixture (some initial state that several tests share)
	// In CxxTest, setUp()/tearDown() are executed around each test case. If you need a fixture on the test
	// suite level, i.e. something that gets constructed once before all the tests in the test suite are run,
	// suites have to be dynamically created. See CxxTest sample directory for example.


	// Shared initialization goes here.
	void setUp() {
		core_init();
	}

	void test_hello_world(){
		TS_ASSERT(true);
	}


	void test_identify_secondary_structure_spans(){

		string test_string1 = "   EEEEE   HHHHHHHH  EEEEE   IGNOR EEEEEE   HHHHHHHHHHH  EEEEE  HHHH   ";
		utility::vector1< std::pair< core::Size, core::Size > > expected_output1;
		expected_output1.push_back( std::make_pair( 4, 8 ) );
		expected_output1.push_back( std::make_pair( 12, 19 ) );
		expected_output1.push_back( std::make_pair( 22, 26 ) );
		expected_output1.push_back( std::make_pair( 36, 41 ) );
		expected_output1.push_back( std::make_pair( 45, 55 ) );
		expected_output1.push_back( std::make_pair( 58, 62 ) );
		expected_output1.push_back( std::make_pair( 65, 68 ) );

		utility::vector1< std::pair< core::Size, core::Size > > spans_1 = protocols::bootcamp::identify_secondary_structure_spans(test_string1);

		TS_ASSERT_EQUALS(spans_1,expected_output1)

		string test_string2 = "HHHHHHH   HHHHHHHHHHHH      HHHHHHHHHHHHEEEEEEEEEEHHHHHHH EEEEHHH ";
		utility::vector1< std::pair< core::Size, core::Size > > expected_output2;
		expected_output2.push_back( std::make_pair( 1, 7 ) );
		expected_output2.push_back( std::make_pair( 11, 22 ) );
		expected_output2.push_back( std::make_pair( 29, 40 ) );
		expected_output2.push_back( std::make_pair( 41, 50 ) );
		expected_output2.push_back( std::make_pair( 51, 57 ) );
		expected_output2.push_back( std::make_pair( 59, 62 ) );
		expected_output2.push_back( std::make_pair( 63, 65 ) );

		utility::vector1< std::pair< core::Size, core::Size > > spans_2 = protocols::bootcamp::identify_secondary_structure_spans(test_string2);

		TS_ASSERT_EQUALS(spans_2,expected_output2)

		string test_string3 = "EEEEEEEEE EEEEEEEE EEEEEEEEE H EEEEE H H H EEEEEEEE";
		utility::vector1< std::pair< core::Size, core::Size > > expected_output3;
		expected_output3.push_back( std::make_pair( 1, 9 ) );
		expected_output3.push_back( std::make_pair( 11, 18 ) );
		expected_output3.push_back( std::make_pair( 20, 28 ) );
		expected_output3.push_back( std::make_pair( 30, 30 ) );
		expected_output3.push_back( std::make_pair( 32, 36 ) );
		expected_output3.push_back( std::make_pair( 38, 38 ) );
		expected_output3.push_back( std::make_pair( 40, 40 ) );
		expected_output3.push_back( std::make_pair( 42, 42 ) );
		expected_output3.push_back( std::make_pair( 44, 51 ) );

		utility::vector1< std::pair< core::Size, core::Size > > spans_3 = protocols::bootcamp::identify_secondary_structure_spans(test_string3);

		TS_ASSERT_EQUALS(spans_3,expected_output3)


	}

		
	
	void test_fold_tree_from_dssp_string(){
		
		string test_string_ft = "   EEEEEEE    EEEEEEE         EEEEEEEEE    EEEEEEEEEE   HHHHHH         EEEEEEEEE         EEEEE     ";

		FoldTree ft = protocols::bootcamp::fold_tree_from_dssp_string(test_string_ft);
		TS_ASSERT( ft.check_fold_tree() );

		TR << ft << endl;
	
		
		TS_ASSERT_EQUALS(ft.size(),38);
		TS_ASSERT_EQUALS(ft.num_jump(),12);
		TS_ASSERT_EQUALS(ft.nres(),test_string_ft.length());



	}
 

};
