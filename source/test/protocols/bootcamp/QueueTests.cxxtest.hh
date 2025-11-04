// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file  protocols/bootcamp/QueueTests.cxxtest.hh
/// @brief  Tests for the Queue class
/// @author brcarpen6 (bcarpen6@jhu.edu)


// Test headers
#include <test/UMoverTest.hh>
#include <test/UTracer.hh>
#include <cxxtest/TestSuite.h>
#include <test/util/pose_funcs.hh>
#include <test/core/init_util.hh>

// Project Headers
#include <protocols/bootcamp/Queue.hh>

// Core Headers
#include <core/pose/Pose.hh>
#include <core/import_pose/import_pose.hh>

// Utility, etc Headers
#include <basic/Tracer.hh>

using namespace std;
using namespace protocols::bootcamp;

static basic::Tracer TR("QueueTests");


class QueueTests : public CxxTest::TestSuite {
	//Define Variables
private:
	Queue test_queue;

public:

	void setUp() {
		core_init();

		test_queue = Queue();

	}

	void tearDown() {

	}



	void test_first() {
		TS_TRACE("Running my first unit test!");
		TS_ASSERT( true );


	}

	void test_enqueue(){
		TS_TRACE("Start Empty");

		TS_ASSERT_EQUALS(test_queue.is_empty(), true);

		TS_TRACE("Correct number of elements added");
		test_queue.enqueue("1");
		test_queue.enqueue("2");
		test_queue.enqueue("3");

		TS_ASSERT_EQUALS(test_queue.size(), 3);

		
	}

	void test_dequeue(){
		TS_TRACE("Start full");
		test_queue.enqueue("1");
		test_queue.enqueue("2");
		test_queue.enqueue("3");

		TS_ASSERT_EQUALS(test_queue.size(), 3);

		TS_TRACE("dequeue in correct order");


		TS_ASSERT_EQUALS(test_queue.dequeue(), "1");
		TS_ASSERT_EQUALS(test_queue.dequeue(), "2");
		TS_ASSERT_EQUALS(test_queue.dequeue(), "3");

		
		TS_TRACE("end empty");

		TS_ASSERT_EQUALS(test_queue.is_empty(), true);

	

		
	}


};
