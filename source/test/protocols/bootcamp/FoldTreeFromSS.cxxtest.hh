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

	// FoldTree fold_tree_from_dssp_string(string const & ss){

	// 	FoldTree ft;

	// 	utility::vector1< std::pair< core::Size, core::Size > > spans = identify_secondary_structure_spans(ss);
	// 	for ( core::Size i = 1; i + 1 <= spans.size(); ++i ) {
	// 		for (core::Size j = 1; j + 1 <= spans.size(); ++j ){
	// 			if (i < j && i!=j){
	// 				core::Size const firstspan_start = spans[i].first;
	// 				core::Size const firstspan_end = spans[i].second;
	// 				core::Size const secondspan_start = spans[j].first;
	// 				core::Size const secondspan_end = spans[j].second;

	// 				core::Size const mid_span_i = (firstspan_start + firstspan_end)/2;
	// 				core::Size const mid_span_j = (secondspan_start + secondspan_end)/2;

	// 				core::Size const cutpoint_ij_midspans = (firstspan_end + secondspan_start)/2;
	// 				core::Size const cutpoint_ij_midloops = (firstspan_end + cutpoint_ij_midspans)/2;

	// 				ft.new_jump(mid_span_i, mid_span_j, cutpoint_ij_midspans);
	// 				ft.new_jump(mid_span_i, cutpoint_ij_midspans, cutpoint_ij_midloops);

	// 				ft.add_edge(mid_span_i, firstspan_end, Edge::PEPTIDE);
	// 				ft.add_edge(mid_span_i, firstspan_start, Edge::PEPTIDE);



	// 			}
	// 		}

	// 	}

	FoldTree fold_tree_from_dssp_string(string const & ss) {

		FoldTree ft;
		core::Size const N = ss.size();

		// 1. Initial Setup: Create a linear FoldTree spanning all residues.
		ft.simple_tree(N);

		utility::vector1< std::pair< core::Size, core::Size > > sse_spans = identify_secondary_structure_spans(ss);
		
		// Handle the trivial case
		if ( sse_spans.empty() ) return ft;
		
		
		// --- Anchor Definition (SSE 1) ---
		core::Size const anchor_start = sse_spans[1].first;
		core::Size const anchor_end = sse_spans[1].second;
		core::Size const anchor_center = (anchor_start + anchor_end) / 2;
		
		// --- Helper function for adding an element to the tree (Jump + Edges) ---
		auto add_element_to_tree = [&](core::Size current_start, core::Size current_end) {
			
			core::Size const current_center = (current_start + current_end) / 2;
			
			// Skip the anchor element
			if ( current_center == anchor_center ) return; 

			// --- Cutpoint Calculation (Simplified & Robust) ---
			// Choose the cutpoint between anchor_end and current_start.
			// This is safe for non-overlapping sequential elements.
			core::Size cutpoint = ( anchor_end + current_start ) / 2;

			// **CRITICAL ROBUSTNESS CHECK**
			// If the calculated cutpoint is invalid (<= 0 or >= N), fix it.
			// This occurs for loops at the C-terminus or overlapping segments.
			if ( cutpoint <= 0 || cutpoint >= N ) { 
				// Choose the next residue after the segment furthest down the chain, 
				// then cap at the highest valid cutpoint: N - 1.
				cutpoint = std::max( anchor_end, current_end ) + 1;

				if ( cutpoint >= N ) {
					cutpoint = N - 1; 
				}
			}
			// ----------------------------------------------------
			
			
			// 1. Add the Jump (This is what triggers the reorder/check)
			// It removes the peptide bond (cutpoint <-> cutpoint + 1)
			ft.new_jump( current_center, anchor_center, cutpoint );

			// 2. REQUIRED FIX: Re-establish peptide edges for the new segment.
			// A segment defined by a jump must be spanned by edges.
			ft.add_edge(current_center, current_end, core::kinematics::Edge::PEPTIDE);
			ft.add_edge(current_center, current_start, core::kinematics::Edge::PEPTIDE);
		};


		// --- 2. Add all subsequent SSEs (i=2 to end) ---
		for ( core::Size i = 2; i <= sse_spans.size(); ++i ) {
			core::Size const start = sse_spans[i].first;
			core::Size const end = sse_spans[i].second;
			add_element_to_tree(start, end);
		}
		
		
		// --- 3. Add all Loops (connecting segments) ---
		for ( core::Size i = 1; i < sse_spans.size(); ++i ) {
			
			core::Size const loop_start = sse_spans[i].second + 1;
			core::Size const loop_end = sse_spans[i+1].first - 1;

			// Only add the loop if it has at least one residue (start <= end)
			if ( loop_start <= loop_end ) {
				add_element_to_tree(loop_start, loop_end);
			}
		}
		
		// --- 4. Add the final loop to the C-terminus (if any) ---
		// If the last SSE is not N, the remaining tail is a loop.
		core::Size const last_sse_end = sse_spans.back().second;
		if ( last_sse_end < N ) {
			add_element_to_tree(last_sse_end + 1, N);
		}


		// --- 5. REQUIRED FIX: Add Peptide Edges for the Anchor (SSE 1) ---
		// The anchor segment must also be spanned by edges relative to its center.
		ft.add_edge(anchor_center, anchor_end, core::kinematics::Edge::PEPTIDE);
		ft.add_edge(anchor_center, anchor_start, core::kinematics::Edge::PEPTIDE);

		return ft;
	}
		
	
	void test_fold_tree_from_dssp_string(){
		
		string test_string_ft = "   EEEEEEE    EEEEEEE         EEEEEEEEE    EEEEEEEEEE   HHHHHH         EEEEEEEEE         EEEEE     ";

		FoldTree ft = fold_tree_from_dssp_string(test_string_ft);
		TS_ASSERT( ft.check_fold_tree() );

		// TR << ft << endl;
		


		// int res = 18;

		// core::Size const start_res = ft.get_residue_edge(res).start();
		// core::Size const end_res = ft.get_residue_edge(res).stop();


		
		// TS_ASSERT_EQUALS(ft.size(),38);
		// TS_ASSERT_EQUALS(ft.num_jump(),12);
		// TS_ASSERT_EQUALS(ft.nres(),test_string_ft.length());

		// TS_ASSERT_EQUALS(start_res,7);
		// TS_ASSERT_EQUALS(end_res,18);

	}
 

};
