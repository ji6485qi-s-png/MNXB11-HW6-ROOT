#include "MovingObject.h"
#include <TCanvas.h>
#include <TFile.h>
#include <TH2D.h>
#include <TTree.h>
#include <iostream>
#include <stdexcept>

void read() {
    TFile file("tree_file.root", "READ");
    if (file.IsZombie()) throw std::runtime_error("Could not open tree_file.root");

    TTree* tree = nullptr;
    file.GetObject("tree", tree);
    if (!tree) throw std::runtime_error("Missing tree");
    tree->Print();

    MovingObject measurement;
    MovingObject* object = &measurement;
    if (tree->SetBranchAddress("object", &object) < 0)
        throw std::runtime_error("Could not read the object branch");

    TH2D components("components", "Velocity in the xy plane;v_{x} (m/s);v_{y} (m/s);Measurements",
                    100, -1.5, 1.5, 100, -1.5, 1.5);
    components.SetStats(false);
    const Long64_t entries = tree->GetEntries();
    for (Long64_t i = 0; i < entries; ++i) {
        if (tree->GetEntry(i) <= 0) throw std::runtime_error("Could not read a measurement");
        components.Fill(object->Vx(), object->Vy());
    }
    tree->ResetBranchAddresses();

    TCanvas density("density", "Velocity components", 850, 650);
    density.SetRightMargin(0.16);
    components.Draw("COLZ");
    density.SaveAs("velocity_components.png");

    TCanvas scatter("scatter", "Selected velocities", 850, 650);
    tree->SetMarkerStyle(7);
    const Long64_t selected = tree->Draw("sqrt(fVx*fVx + fVy*fVy):fVz",
                                         "sqrt(fVx*fVx + fVy*fVy + fVz*fVz) > 0.50");
    if (selected <= 0) throw std::runtime_error("No points drawn for the speed cut");
    auto* frame = tree->GetHistogram();
    frame->SetTitle("Measurements with speed above 0.50 m/s");
    frame->GetXaxis()->SetTitle("v_{z} (m/s)");
    frame->GetYaxis()->SetTitle("Horizontal speed (m/s)");
    scatter.Modified();
    scatter.Update();
    scatter.SaveAs("selected_velocities.png");
    std::cout << "Read " << entries << " measurements; selected " << selected << ".\n";
}
