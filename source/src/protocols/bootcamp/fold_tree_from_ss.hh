#ifndef PROTOCOLS_BOOTCAMP_FOLDTREESS_HH
#define PROTOCOLS_BOOTCAMP_FOLDTREESS_HH

// --- Dependencies ---
// Forward Declarations: Use these to avoid including large header files 
// if possible. If the function *returns* the type, you often need the full include.
// If the function only uses a *pointer* or *reference* to a type, a forward declaration is enough.

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

// C++ headers

//Auto Headers
#include <core/pack/dunbrack/DunbrackRotamer.hh>


using namespace std;
using namespace protocols::match;
using namespace protocols::match::upstream;
using namespace core::kinematics;
using namespace core::scoring;
using namespace core::pose;

// --- Namespace declarations (Crucial!) ---
// Declare the full namespace path where these functions are defined.
namespace protocols {
namespace bootcamp {

// 1. Function to identify secondary structure spans
// This function takes a const reference to a std::string and returns a vector1 of pairs.
utility::vector1< std::pair< core::Size, core::Size > >
identify_secondary_structure_spans( std::string const & ss_string );


// 2. Function to generate FoldTree from a Pose (it uses DSSP)
// This function takes a const reference to a Pose and returns a FoldTree object.
core::kinematics::FoldTree
fold_tree_from_ss( core::pose::Pose const & pose );


// 3. Function to generate FoldTree from a DSSP string
// This function takes a const reference to a string and returns a FoldTree object.
core::kinematics::FoldTree
fold_tree_from_dssp_string( std::string const & ss );


} // namespace bootcamp
} // namespace protocols

#endif // PROTOCOLS_BOOTCAMP_FOLDTREESS_HH