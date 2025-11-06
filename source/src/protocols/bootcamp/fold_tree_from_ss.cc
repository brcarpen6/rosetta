
// FoldTreeSS.cc (Implementation File)

// 1. Include your new header file
#include <unordered_set>

#include <protocols/match/upstream/ProteinSCSampler.hh>
#include <protocols/match/upstream/OriginalScaffoldBuildPoint.hh>

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
// ... other includes needed for the function bodies (e.g., TR, Edge)

// 3. Wrap all function definitions in the correct namespace
namespace protocols {
namespace bootcamp {

utility::vector1< pair< core::Size, core::Size > >
	identify_secondary_structure_spans( string const & ss_string )
	{
	utility::vector1< pair< core::Size, core::Size > > ss_boundaries;
	core::Size strand_start = -1;
	for ( core::Size ii = 0; ii < ss_string.size(); ++ii ) {
		if ( ss_string[ ii ] == 'E' || ss_string[ ii ] == 'H'  ) {
		if ( int( strand_start ) == -1 ) {
			strand_start = ii;
		} else if ( ss_string[ii] != ss_string[strand_start] ) {
			ss_boundaries.push_back( make_pair( strand_start+1, ii ) );
			strand_start = ii;
		}
		} else {
		if ( int( strand_start ) != -1 ) {
			ss_boundaries.push_back( make_pair( strand_start+1, ii ) );
			strand_start = -1;
		}
		}
	}
	if ( int( strand_start ) != -1 ) {
		// last residue was part of a ss-eleemnt                                                                                                                                
		ss_boundaries.push_back( make_pair( strand_start+1, ss_string.size() ));
	}
	// for ( core::Size ii = 1; ii <= ss_boundaries.size(); ++ii ) {
	// 	std::cout << "SS Element " << ii << " from residue "
	// 	<< ss_boundaries[ ii ].first << " to "
	// 	<< ss_boundaries[ ii ].second << endl;
	// }
	return ss_boundaries;
	}

FoldTree fold_tree_from_ss(Pose const & pose){
		core::scoring::dssp::Dssp dssp( pose );
		string ss = dssp.get_dssp_secstruct();
		return fold_tree_from_dssp_string( ss );
	} 

FoldTree fold_tree_from_dssp_string(string const & ss){

    FoldTree ft;
    core::Size const N = ss.size();

    // TR << N << endl;

    // ft.simple_tree( N );
    
    utility::vector1< pair< core::Size, core::Size > > spans = identify_secondary_structure_spans(ss);
    
    // // First span and its midpoint (our root res)
    core::Size const sse1_start = spans[1].first;
    core::Size const sse1_end = spans[1].second;
    core::Size const rootpoint = (sse1_start + sse1_end)/2;


    ft.add_edge(rootpoint, 1, Edge::PEPTIDE);
    ft.add_edge(rootpoint, sse1_end , Edge::PEPTIDE);
    
    int num_jump=1;
    for ( core::Size i = 2; i <= spans.size(); ++i ) {

        


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


        ft.add_edge(rootpoint, midpoint_gap , num_jump++);

        ft.add_edge(midpoint_gap, loop_start, Edge::PEPTIDE);

    
        ft.add_edge(midpoint_gap, loop_end, Edge::PEPTIDE);

        ft.add_edge(rootpoint, midpoint_i_sse, num_jump++);

        ft.add_edge(midpoint_i_sse, sse_i_start, Edge::PEPTIDE);

        if(i == spans.size()){
            ft.add_edge(midpoint_i_sse, N, Edge::PEPTIDE);
            
        }else{
            ft.add_edge(midpoint_i_sse, sse_i_end, Edge::PEPTIDE);
            
        }

        

        // TR << "end loop" << i << endl;
        // TR << "current ft " << ft << endl;
        
    }

    return ft;
}
	
}

}