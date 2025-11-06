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
#include <core/scoring/ScoreFunctionFactory.hh>
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

	void test_setters_getters() {
        

        // 1. Setup the Mover (starts with default 100 iterations)
        BootCampMover mover;
        
        // --- Test Iterations Setter/Getter ---
        const core::Size new_iterations = 10;
        
        // Set the new value
        mover.set_num_iterations(new_iterations);
        
        // Retrieve and check using TS_ASSERT_EQUALS
        core::Size retrieved_iterations = mover.get_num_iterations();
        TS_ASSERT_EQUALS(new_iterations, retrieved_iterations);
        


        // --- Test ScoreFunction Setter/Getter ---
        
        // Create the test ScoreFunction object
        ScoreFunctionOP test_sfxn = ScoreFunctionFactory::create_score_function( "score13" );

        // Set the new ScoreFunction
        mover.set_sfxn(test_sfxn);
        
        // Retrieve the ScoreFunction
        ScoreFunctionOP retrieved_sfxn = mover.get_sfxn();

        // Check if the retrieved pointer is valid using TS_ASSERT
        // This is equivalent to checking if (!retrieved_sfxn)
        TS_ASSERT(retrieved_sfxn);
        
        // Check if the retrieved object is the correct one by comparing its unique name
        // TS_ASSERT_EQUALS(test_sfxn.get_name(), retrieved_sfxn.get_name());
        

        // Optional: Show the apply method uses the new values
        
        // mover.apply();
    }


		
 
};
