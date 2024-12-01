#include <chrono>
#include <ogdf/fileformats/GraphIO.h>
#include <ogdf/basic/LayoutStatistics.h>

#include <ogdf/orthogonal/OrthoLayout.h>
#include <ogdf/planarity/EmbedderMinDepthMaxFaceLayers.h>
#include <ogdf/planarity/PlanarSubgraphFast.h>
#include <ogdf/planarity/PlanarizationLayout.h>
#include <ogdf/planarity/SubgraphPlanarizer.h>
#include <ogdf/planarity/VariableEmbeddingInserter.h>

using namespace ogdf;

int calculateBends(const GraphAttributes &GA){
    LayoutStatistics stats;
    ArrayBuffer<int> bends = stats.numberOfBends(GA, false);

    int total = 0;
    for(int c : bends) {
        total+= c;
    }
    return total;
}
int calculateTotalCrossings(const GraphAttributes &GA) {
    LayoutStatistics stats;
    ArrayBuffer<int> crossings = stats.numberOfCrossings(GA);

    int totalCrossings = 0;
    for (int c : crossings) {
        totalCrossings += c;
    }

    return totalCrossings / 2;
}

double calculateMaxAngles(const GraphAttributes &GA) {
    LayoutStatistics stats;
    ArrayBuffer<double> angles = stats.angles(GA, false);

    double maxAngle = 0;
    for (double c : angles) {
        maxAngle = std::max(maxAngle, c);
    }

    return maxAngle;
}

double calculateMinAngles(const GraphAttributes &GA) {
    LayoutStatistics stats;
    ArrayBuffer<double> angles = stats.angles(GA, false);

    double minAngle = 1000;
    for (double c : angles) {
        minAngle = std::min(minAngle, c);
    }

    return minAngle;
}

int main()
{
    Graph G;
    GraphAttributes GA(G,
      GraphAttributes::nodeGraphics | GraphAttributes::nodeType |
      GraphAttributes::edgeGraphics | GraphAttributes::edgeType);

    if (!GraphIO::read(GA, G, "cylinder_rnd_010_010.gml", GraphIO::readGML)) {
        std::cerr << "Could not read ERDiagram.gml" << std::endl;
        return 1;
    }


    auto startt = std::chrono::steady_clock::now();
    pl.call(GA);
    auto endt = std::chrono::steady_clock::now();

    std::cout << "Layout call time " << std::chrono::duration_cast<std::chrono::milliseconds>(endt - startt).count() << " ms\n";

    GraphIO::write(GA, "output.svg", GraphIO::drawSVG);

    return 0;
}

