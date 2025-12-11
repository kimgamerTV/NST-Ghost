#include <pybind11/embed.h>
#include <iostream>
#include <vector>
#include <string>

namespace py = pybind11;

#include <fstream>

int main() {
    std::ofstream out("verify_result.txt");
    if (!out) return 1;

    try {
        py::scoped_interpreter guard{};
        
        py::module_ sys = py::module_::import("sys");
        sys.attr("path").attr("append")(".");
        
        // Test AI Smart Filter
        py::module_ ai_mod = py::module_::import("scripts.ai_smart_filter");
        py::object filter = ai_mod.attr("AISmartFilter")();
        
        filter.attr("add_example")("Do Not Translate This");
        
        bool result = filter.attr("predict")("Do Not Translate This").cast<bool>();
        bool result2 = filter.attr("predict")("Apple is a fruit").cast<bool>();
        
        out << "Prediction for 'Do Not Translate This': " << (result ? "SKIP" : "KEEP") << std::endl;
        out << "Prediction for 'Apple is a fruit': " << (result2 ? "SKIP" : "KEEP") << std::endl;

        // Test Batch
        py::list batchInputs;
        batchInputs.append("Do Not Translate This");
        batchInputs.append("Apple is a fruit");
        batchInputs.append("Another Skip Phrase");
        
        filter.attr("add_example")("Another Skip Phrase");
        
        py::list batchResults = filter.attr("predict_batch")(batchInputs).cast<py::list>();
        bool b1 = batchResults[0].cast<bool>();
        bool b2 = batchResults[1].cast<bool>();
        bool b3 = batchResults[2].cast<bool>();
        
        out << "Batch Result 1: " << (b1 ? "SKIP" : "KEEP") << std::endl;
        out << "Batch Result 2: " << (b2 ? "SKIP" : "KEEP") << std::endl;
        out << "Batch Result 3: " << (b3 ? "SKIP" : "KEEP") << std::endl;

        if (result && !result2 && b1 && !b2 && b3) {
            out << "SUCCESS: AI Filter behaved as expected." << std::endl;
        } else {
            out << "FAILURE: AI Filter logic failed." << std::endl;
        }

    } catch (const std::exception &e) {
        out << "Exception: " << e.what() << std::endl;
        return 1;
    }
}
