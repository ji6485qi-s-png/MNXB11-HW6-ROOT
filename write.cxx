#include "MovingObject.h"
#include <TFile.h>
#include <TRandom.h>
#include <TTree.h>
#include <iostream>
#include <stdexcept>

void write() {
    TFile file("tree_file.root", "RECREATE");
    if (file.IsZombie()) throw std::runtime_error("Could not create tree_file.root");

    MovingObject measurement;
    MovingObject* object = &measurement;
    TTree tree("tree", "Velocity measurements");
    tree.Branch("object", "MovingObject", &object);

    gRandom->SetSeed(12345);
    for (Int_t i = 0; i < 10000; ++i) {
        Double_t vx = gRandom->Gaus(0.0, 0.30);
        Double_t vy = gRandom->Gaus(0.0, 0.30);
        Double_t vz = gRandom->Gaus(0.0, 0.30);
        measurement.SetVelocity(vx, vy, vz);
        if (tree.Fill() < 0) throw std::runtime_error("Could not fill the tree");
    }

    if (tree.Write() <= 0) throw std::runtime_error("Could not write the tree");
    std::cout << "Wrote " << tree.GetEntries() << " measurements.\n";
    // The local tree is destroyed before the file closes.
}
