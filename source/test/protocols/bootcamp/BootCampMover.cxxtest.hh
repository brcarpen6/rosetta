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
#include <test/protocols/init_util.hh>
#include <core/scoring/dssp/Dssp.hh>

// Utility headers
#include <basic/Tracer.hh>

/// Project headers
#include <core/types.hh>
#include <core/kinematics/FoldTree.hh>
#include <core/pose/Pose.hh>
#include <core/kinematics/Edge.hh>
#include <protocols/bootcamp/fold_tree_from_ss.hh>
#include <protocols/moves/Mover.hh>
#include <protocols/moves/MoverFactory.hh>
#include <protocols/moves/mover_schemas.hh>
#include <protocols/bootcamp/BootCampMover.hh>
#include <protocols/bootcamp/BootCampMoverCreator.hh>
// C++ headers

//Auto Headers
#include <core/pack/dunbrack/DunbrackRotamer.hh>


using namespace std;
using namespace protocols::match;
using namespace protocols::match::upstream;
using namespace core::kinematics;
using namespace core::scoring;
using namespace core::pose;
using namespace protocols::moves;
using namespace protocols::bootcamp;

// --------------- Test Class --------------- //

class BootCampMoverTests : public CxxTest::TestSuite {

public:


	// --------------- Fixtures --------------- //

	// Define a test fixture (some initial state that several tests share)
	// In CxxTest, setUp()/tearDown() are executed around each test case. If you need a fixture on the test
	// suite level, i.e. something that gets constructed once before all the tests in the test suite are run,
	// suites have to be dynamically created. See CxxTest sample directory for example.


	// Shared initialization goes here.
	void setUp() {
		protocols_init();
	}

	

	void test_factory_builds_bootcamp_mover() {
    
		protocols::moves::MoverOP base_mover_op =
		protocols::moves::MoverFactory::get_instance()->newMover( "BootCampMover" );

		
		protocols::bootcamp::BootCampMoverOP bcm_op =
		utility::pointer::dynamic_pointer_cast< protocols::bootcamp::BootCampMover >( base_mover_op );

	
		TS_ASSERT( bcm_op );               
		TS_ASSERT_DIFFERS( bcm_op, nullptr );
  	}


		
 
};
