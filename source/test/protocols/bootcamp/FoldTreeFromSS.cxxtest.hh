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

	utility::vector1< std::pair< core::Size, core::Size > >
	identify_secondary_structure_spans( std::string const & ss_string )
	{
	utility::vector1< std::pair< core::Size, core::Size > > ss_boundaries;
	core::Size strand_start = -1;
	for ( core::Size ii = 0; ii < ss_string.size(); ++ii ) {
		if ( ss_string[ ii ] == 'E' || ss_string[ ii ] == 'H'  ) {
		if ( int( strand_start ) == -1 ) {
			strand_start = ii;
		} else if ( ss_string[ii] != ss_string[strand_start] ) {
			ss_boundaries.push_back( std::make_pair( strand_start+1, ii ) );
			strand_start = ii;
		}
		} else {
		if ( int( strand_start ) != -1 ) {
			ss_boundaries.push_back( std::make_pair( strand_start+1, ii ) );
			strand_start = -1;
		}
		}
	}
	if ( int( strand_start ) != -1 ) {
		// last residue was part of a ss-eleemnt                                                                                                                                
		ss_boundaries.push_back( std::make_pair( strand_start+1, ss_string.size() ));
	}
	for ( core::Size ii = 1; ii <= ss_boundaries.size(); ++ii ) {
		std::cout << "SS Element " << ii << " from residue "
		<< ss_boundaries[ ii ].first << " to "
		<< ss_boundaries[ ii ].second << std::endl;
	}
	return ss_boundaries;
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

		utility::vector1< std::pair< core::Size, core::Size > > spans_1 = identify_secondary_structure_spans(test_string1);

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

		utility::vector1< std::pair< core::Size, core::Size > > spans_2 = identify_secondary_structure_spans(test_string2);

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

		utility::vector1< std::pair< core::Size, core::Size > > spans_3 = identify_secondary_structure_spans(test_string3);

		TS_ASSERT_EQUALS(spans_3,expected_output3)


	}

	FoldTree fold_tree_from_ss(Pose const & pose){
		core::scoring::dssp::Dssp dssp( pose );
		string ss = dssp.get_dssp_secstruct();
		return fold_tree_from_dssp_string( ss );
	} 

	FoldTree fold_tree_from_dssp_string(string const & ss){

		FoldTree ft;
		core::Size const N = ss.size();

		TR << N << endl;

		// ft.simple_tree( N );
		
		utility::vector1< std::pair< core::Size, core::Size > > spans = identify_secondary_structure_spans(ss);
		
		// // First span and its midpoint (our root res)
		core::Size const sse1_start = spans[1].first;
		core::Size const sse1_end = spans[1].second;
		core::Size const rootpoint = (sse1_start + sse1_end)/2;

		TR << "first sse begin " << sse1_start << endl;
		TR << "first sse end " << sse1_end << endl;
		TR << "rootpoint " <<rootpoint << endl;


		ft.add_edge(rootpoint, 1, Edge::PEPTIDE);
		ft.add_edge(rootpoint, sse1_end , Edge::PEPTIDE);

		TR << "start ft " << ft << endl;
		
		int num_jump=1;
		for ( core::Size i = 2; i <= spans.size(); ++i ) {

			// TR << "start loop " << i << endl;


			// Define all points
			// each consecutive sse with cutpoint and jump edge to midpoint of sse and gaps
			core::Size const sse_i_start = spans[i].first;
			core::Size const sse_i_end = spans[i].second;
			core::Size const midpoint_i_sse = (sse_i_start + sse_i_end)/2;

			//loops
			// between sse
			core::Size const gap_start = spans[i-1].second;

			
			core::Size const gap_end = spans[i].first;
			core::Size const midpoint_gap = (gap_start + gap_end)/2;
			


			core::Size const loop_start = gap_start + 1;
			core::Size const loop_end = gap_end - 1;

	
			// TR << ft.cutpoint( gap_start ) << endl;
			TR << "Set up all points" << endl;

			ft.add_edge(rootpoint, midpoint_gap , num_jump++);

			ft.add_edge(midpoint_gap, loop_start, Edge::PEPTIDE);

		
			ft.add_edge(midpoint_gap, loop_end, Edge::PEPTIDE);

			ft.add_edge(rootpoint, midpoint_i_sse, num_jump++);

			ft.add_edge(midpoint_i_sse, sse_i_start, Edge::PEPTIDE);

			if(i == spans.size()){
				ft.add_edge(midpoint_i_sse, N, Edge::PEPTIDE);
				TR << "add peptide 6" << endl;

			}else{
				ft.add_edge(midpoint_i_sse, sse_i_end, Edge::PEPTIDE);
				TR << "add peptide 6" << endl;
			}

			

			// TR << "end loop" << i << endl;
			// TR << "current ft " << ft << endl;
			
		}

		return ft;
	}	

	


		
	
	void test_fold_tree_from_dssp_string(){
		
		string test_string_ft = "   EEEEEEE    EEEEEEE         EEEEEEEEE    EEEEEEEEEE   HHHHHH         EEEEEEEEE         EEEEE     ";

		FoldTree ft = fold_tree_from_dssp_string(test_string_ft);
		TS_ASSERT( ft.check_fold_tree() );

		TR << ft << endl;
	
		
		TS_ASSERT_EQUALS(ft.size(),38);
		TS_ASSERT_EQUALS(ft.num_jump(),12);
		TS_ASSERT_EQUALS(ft.nres(),test_string_ft.length());



	}
 

};
