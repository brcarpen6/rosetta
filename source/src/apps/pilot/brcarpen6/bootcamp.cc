// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

#include <iostream>
#include <numeric>
// devel headers
#include <devel/init.hh>

// protocol headers
#include <protocols/jd2/JobDistributor.hh>
// #include <protocols/bootcamp/BootcampMover.hh>

// utility headers
#include <utility/excn/Exceptions.hh>

// basic headers
#include <basic/Tracer.hh>
#include <basic/options/keys/in.OptionKeys.gen.hh>
#include <basic/options/option.hh>
#include <basic/options/keys/OptionKeys.hh>
#include <utility/options/OptionCollection.hh>
#include <basic/options/option_macros.hh>
// Rosetta headers
#include <core/pose/Pose.hh>
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
#include <protocols/jd2/JobDistributor.hh> 


using namespace std;
using namespace core::import_pose;
using namespace core::pose;
using namespace core::scoring;
using namespace numeric::random;
using namespace protocols::moves;
using namespace core::pack::task;

static basic::Tracer TR( "bootcamp" );



int main(int argc, char ** argv) {
    cout << "Hello world!!" << endl;

    devel::init (argc, argv);

    utility::vector1<string> filenames = basic::options::option[
    basic::options::OptionKeys::in::file::s ].value();

    if (filenames.size() > 0) {
        cout << "You entered: " << filenames[ 1 ] << "as the PDB file to be read" << endl;   
    } else{
        cout << "You didn't provide a PDB file with the -in::file::s option" << endl;
        return 1;
    }

    PoseOP mypose = pose_from_file(filenames[1]);
    mypose->fold_tree(protocols::bootcamp::fold_tree_from_ss(*mypose));


    ScoreFunctionOP sfxn = get_score_function();
    sfxn->set_weight( linear_chainbreak, 1);
    correctly_add_cutpoint_variants(*mypose);

    core::kinematics::MoveMap mm;
    mm.set_bb(true);
    mm.set_chi(true);

    core::optimization::MinimizerOptions min_opts( "lbfgs_armijo_atol", 0.01, true );
    core::optimization::AtomTreeMinimizer atm;

    //set weight in score function before here linear_chainbreak term
 
    MonteCarlo mc(*mypose, *sfxn, 1.0);

    PyMOLObserverOP the_observer = AddPyMOLObserver( *mypose, true, 0);
    the_observer -> pymol().apply(*mypose);

    Pose copy_pose = *mypose;
    int accepted_count = 0;
    int rejected_count = 0;
    vector<int> energies;
    for(int i = 1; i <= 10; ++i){
        

        double uniform_random_number = uniform();
        core::Size N = mypose->size();
        core::Size randres = static_cast<core::Size> (uniform_random_number * N + 1);
        core::Real pert1 = gaussian();
        core::Real pert2 = gaussian();
        core::Real orig_phi = mypose->phi( randres );
        core::Real orig_psi = mypose->psi( randres );

        mypose->set_phi( randres, orig_phi + pert1);
        mypose->set_psi( randres, orig_psi + pert1);

        PackerTaskOP repack_task = TaskFactory::create_packer_task(*mypose);
        repack_task-> restrict_to_repacking();
        core::pack::pack_rotamers(*mypose, *sfxn, repack_task);

        copy_pose = *mypose;
        atm.run( copy_pose, mm, *sfxn, min_opts );
        *mypose = copy_pose;

        core::Real score = sfxn -> score ( *mypose );

        energies.push_back(score) ;


        bool accept = mc.boltzmann(*mypose);

        if(accept){
            accepted_count++;
        } else{
            rejected_count++;
        }

        TR << "Cycle: " << i << " score: " << score << " best: " << mc.lowest_score() << endl;
    }
    
    double average = static_cast<double>(std::accumulate(energies.begin(), energies.end(), 0)) / energies.size();
    double accepted_rate = accepted_count / 10.0; 
    double rejected_rate = rejected_count / 10.0; 

    TR << "Accepted Rate: " << accepted_rate << endl;
    TR << "Recjected Rate: " << rejected_rate << endl;
    TR << "Average Energy: " << average << endl;

    




        

    

}