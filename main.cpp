#include <ogdf/basic/PreprocessorLayout.h>
#include <ogdf/energybased/StressMinimization.h>
#include <ogdf/energybased/multilevel_mixer/RandomMerger.h>
#include <ogdf/energybased/multilevel_mixer/SolarPlacer.h>
#include <ogdf/energybased/multilevel_mixer/ModularMultilevelMixer.h>
#include <ogdf/fileformats/GraphIO.h>
#include <ogdf/packing/ComponentSplitterLayout.h>
#include <ogdf/packing/TileToRowsCCPacker.h>
#include <ogdf/energybased/multilevel_mixer/ScalingLayout.h>
using namespace ogdf;

static void configureRandomSolarStressLayout(ScalingLayout *sl, MultilevelBuilder *&merger, InitialPlacer *&placer)
{
    // Use RandomMerger for coarsening phase.
    merger = new RandomMerger();

    // Use SolarPlacer for placement.
    placer = new SolarPlacer();

    // Configure ScalingLayout.
    sl->setExtraScalingSteps(0); // No postprocessing at each level.
    sl->setScalingType(ScalingLayout::ScalingType::RelativeToDrawing);
    sl->setScaling(1.5, 1.5); // Example scaling factors.
}

int main(int argc, const char *argv[])
{

    // Load the graph.
    Graph g;
    GraphAttributes ga(g, GraphAttributes::nodeGraphics |
                              GraphAttributes::edgeGraphics |
                              GraphAttributes::nodeLabel |
                              GraphAttributes::edgeStyle |
                              GraphAttributes::nodeStyle);
    if (!GraphIO::read(ga, g, "rna.gml", GraphIO::readGML)) {
        std::cerr << "Could not load graph!" << std::endl;
        return 1;
    }

    // Set node dimensions.
    for (node v : g.nodes) {
        ga.width(v) = ga.height(v) = 10.0;
    }

    // Create a MultilevelGraph from the GraphAttributes.
    MultilevelGraph mlg(ga);

    // Use StressMinimization as the single-level layout algorithm.
    StressMinimization *stressMinimizer = new StressMinimization();

    // Wrap StressMinimization into a ScalingLayout.
    ScalingLayout *sl = new ScalingLayout();
    sl->setLayoutRepeats(1);
    sl->setSecondaryLayout(stressMinimizer);

    // Configure merger and placer.
    MultilevelBuilder *merger;
    InitialPlacer *placer;
    configureRandomSolarStressLayout(sl, merger, placer);

    // Create ModularMultilevelMixer.
    ModularMultilevelMixer *mmm = new ModularMultilevelMixer;
    mmm->setLayoutRepeats(1);
    mmm->setLevelLayoutModule(sl);
    mmm->setInitialPlacer(placer);
    mmm->setMultilevelBuilder(merger);

    // Use ComponentSplitterLayout to handle disconnected components.
    ComponentSplitterLayout *csl = new ComponentSplitterLayout;
    TileToRowsCCPacker *packer = new TileToRowsCCPacker;
    csl->setPacker(packer);
    csl->setLayoutModule(mmm);

    // Use PreprocessorLayout to clean up the graph.
    PreprocessorLayout ppl;
    ppl.setLayoutModule(csl);
    ppl.setRandomizePositions(true);

    auto startt = std::chrono::steady_clock::now();
    ppl.call(mlg);
    auto endt = std::chrono::steady_clock::now();

    std::cout << "Layout call time " << std::chrono::duration_cast<std::chrono::milliseconds>(endt - startt).count() << " ms\n";
    // Export the graph layout.
    mlg.exportAttributes(ga);
    std::string baseName = "renname";

    GraphIO::write(ga, baseName + ".svg", GraphIO::drawSVG);

    return 0;
}
