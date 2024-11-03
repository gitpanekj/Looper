#include <iostream>
#include <fstream>
#include "directed_labeled_graph.cpp"
#include "labeled_transition_system.cpp"

#define TEST_OUTPUT_PATH "/home/panekj/programming/Looper/tests/"

int main()
{
    std::ofstream outFile;

    for (const auto &test : directed_labeled_graph_dot_tests::tests)
    {
        outFile.open(TEST_OUTPUT_PATH + test.name + ".dlg" + ".dot");
        if (!outFile)
        {
            std::cerr << "Error opening file: " << test.name << std::endl;
            return 1; // Exit with an error code
        }
        outFile << test.run() << std::endl;
        outFile.close();
    }

    for (const auto &test : labeled_transition_system_tests::tests)
    {
        outFile.open(TEST_OUTPUT_PATH + test.name + ".lts" + ".dot");
        if (!outFile)
        {
            std::cerr << "Error opening file: " << test.name << std::endl;
            return 1; // Exit with an error code
        }
        outFile << test.run() << std::endl;
        outFile.close();
    }

    return 0;
}