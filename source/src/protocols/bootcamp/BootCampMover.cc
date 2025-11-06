// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file protocols/bootcamp/BootCampMover.cc
/// @brief 2025_bootcamp_mover_subclass
/// @author brcarpen6 (bcarpen6@jhu.edu)

// Unit headers 
#include <protocols/bootcamp/BootCampMover.hh>
#include <protocols/bootcamp/BootCampMoverCreator.hh>

// Core headers
#include <core/pose/Pose.hh>

// Basic/Utility headers
#include <basic/Tracer.hh>
#include <utility/tag/Tag.hh>
#include <utility/pointer/memory.hh>

// XSD Includes
#include <utility/tag/XMLSchemaGeneration.hh>
#include <protocols/moves/mover_schemas.hh> 


#include <core/import_pose/import_pose.hh>
#include <core/types.hh>
#include <utility/pointer/owning_ptr.hh>
#include <core/scoring/ScoreFunction.hh>
#include <core/scoring/ScoreFunctionFactory.hh>
#include <numeric/random/random.hh>
#include <protocols/moves/MonteCarlo.hh>
#include <protocols/moves/PyMOLMover.hh>
#include <core/pack/task/PackerTask.hh>
#include <core/pack/task/TaskFactory.hh>
#include <core/pack/pack_rotamers.hh>
#include <core/pack/task/operation/TaskOperations.hh>
#include <core/kinematics/MoveMap.hh>
#include <core/optimization/AtomTreeMinimizer.hh>
#include <core/optimization/MinimizerOptions.hh>
#include <core/pose/variant_util.hh>
#include <protocols/bootcamp/fold_tree_from_ss.hh>


// Citation Manager
#include <utility/vector1.hh>
#include <basic/citation_manager/UnpublishedModuleInfo.hh>

static basic::Tracer TR( "protocols/bootcamp.BootCampMover" );
using namespace std;
using namespace core::import_pose;
using namespace core::pose;
using namespace core::scoring;
using namespace numeric::random;
using namespace protocols::moves;
using namespace core::pack::task;

namespace protocols{
namespace bootcamp {

	/////////////////////
	/// Constructors  ///
	/////////////////////

/// @brief Default constructor
BootCampMover::BootCampMover(
	core::Size default_iterations,
	core::scoring::ScoreFunctionOP sfxn
)
: protocols::moves::Mover( BootCampMover::mover_name() ),
  sfxn_( sfxn ),
  num_iterations_( default_iterations )
{
	// If no scorefunction was provided, use Rosetta default
	if ( !sfxn_ ) {
		sfxn_ = core::scoring::get_score_function();
	}
}

////////////////////////////////////////////////////////////////////////////////
/// @brief Destructor (important for properly forward-declaring smart-pointer members)
BootCampMover::~BootCampMover(){}

////////////////////////////////////////////////////////////////////////////////
	/// Mover Methods ///
	/////////////////////

/// @brief Apply the mover
void
BootCampMover::apply( core::pose::Pose & pose ){

	pose.fold_tree(protocols::bootcamp::fold_tree_from_ss(pose));


    ScoreFunctionOP sfxn = sfxn_;
    sfxn->set_weight( linear_chainbreak, 1);
    correctly_add_cutpoint_variants(pose);

    core::kinematics::MoveMap mm; 
    mm.set_bb(true);
    mm.set_chi(true);

    core::optimization::MinimizerOptions min_opts( "lbfgs_armijo_atol", 0.01, true );
    core::optimization::AtomTreeMinimizer atm;

    //set weight in score function before here linear_chainbreak term
 
    moves::MonteCarlo mc(pose, *sfxn, 1.0);

    moves::PyMOLObserverOP the_observer = moves::AddPyMOLObserver( pose, true, 0);
    the_observer -> pymol().apply(pose);

    Pose copy_pose = pose;

    vector<int> energies;
    for(core::Size i = 1; i <= num_iterations_; ++i){
        

        double uniform_random_number = uniform();
        core::Size N = pose.size();
        core::Size randres = static_cast<core::Size> (uniform_random_number * N + 1);
        core::Real pert1 = gaussian();
        core::Real pert2 = gaussian();
        core::Real orig_phi = pose.phi( randres );
        core::Real orig_psi = pose.psi( randres );

        pose.set_phi( randres, orig_phi + pert1);
        pose.set_psi( randres, orig_psi + pert1);

        PackerTaskOP repack_task = TaskFactory::create_packer_task(pose);
        repack_task-> restrict_to_repacking();
        core::pack::pack_rotamers(pose, *sfxn, repack_task);

        copy_pose = pose;
        atm.run( copy_pose, mm, *sfxn, min_opts );
        pose = copy_pose;

        core::Real score = sfxn -> score ( pose );

        energies.push_back(score) ;


        bool accept = mc.boltzmann(pose);

       

        TR << "Cycle: " << i << " score: " << score << " best: " << mc.lowest_score() << endl;
    }

}

////////////////////////////////////////////////////////////////////////////////
/// @brief Show the contents of the Mover
void
BootCampMover::show(std::ostream & output) const
{
	protocols::moves::Mover::show(output);
}

////////////////////////////////////////////////////////////////////////////////
	/// Rosetta Scripts Support ///
	///////////////////////////////

/// @brief parse XML tag (to use this Mover in Rosetta Scripts)
void
BootCampMover::parse_my_tag(
	utility::tag::TagCOP ,
	basic::datacache::DataMap&
) {

}
void BootCampMover::provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd )
{

	using namespace utility::tag;
	AttributeList attlist;

	//here you should write code to describe the XML Schema for the class.  If it has only attributes, simply fill the probided AttributeList.

	protocols::moves::xsd_type_definition_w_attributes( xsd, mover_name(), "2025_bootcamp_mover_subclass", attlist );
}


////////////////////////////////////////////////////////////////////////////////
/// @brief required in the context of the parser/scripting scheme
protocols::moves::MoverOP
BootCampMover::fresh_instance() const
{
	return utility::pointer::make_shared< BootCampMover >();
}

/// @brief required in the context of the parser/scripting scheme
protocols::moves::MoverOP
BootCampMover::clone() const
{
	return utility::pointer::make_shared< BootCampMover >( *this );
}

std::string BootCampMover::get_name() const {
	return mover_name();
}

std::string BootCampMover::mover_name() {
	return "BootCampMover";
}



/////////////// Creator ///////////////

protocols::moves::MoverOP
BootCampMoverCreator::create_mover() const
{
	return utility::pointer::make_shared< BootCampMover >();
}

std::string
BootCampMoverCreator::keyname() const
{
	return BootCampMover::mover_name();
}

void BootCampMoverCreator::provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd ) const
{
	BootCampMover::provide_xml_schema( xsd );
}

/// @brief This mover is unpublished.  It returns brcarpen6 as its author.
void
BootCampMover::provide_citation_info(basic::citation_manager::CitationCollectionList & citations ) const {
	citations.add(
		utility::pointer::make_shared< basic::citation_manager::UnpublishedModuleInfo >(
		"BootCampMover", basic::citation_manager::CitedModuleType::Mover,
		"brcarpen6",
		"TODO: institution",
		"bcarpen6@jhu.edu",
		"Wrote the BootCampMover."
		)
	);
}


////////////////////////////////////////////////////////////////////////////////
	/// private methods ///
	///////////////////////


std::ostream &
operator<<( std::ostream & os, BootCampMover const & mover )
{
	mover.show(os);
	return os;
}


} //protocols/bootcamp
}
