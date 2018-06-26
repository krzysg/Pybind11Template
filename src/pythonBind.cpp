#include <pybind11/pybind11.h>
#include <iostream>

namespace py = pybind11;

// -------- Check if properly configured in CMAKE -----------------------------
#ifndef TEMPLATE_PYTHON_MODULE_NAME
#error "Name of module (python binding) is not defined!"
#endif

// -------- Class to be wrapped in python ----------------------------
template <typename T>
class MyClass {
    std::unique_ptr<T[]> iData;

public:
    MyClass () {
        iData.reset( new T[size()] );
        for (int i = 0; i < size(); ++i) iData[i] = 2 * (i + 1);
    }

    T *data() {return iData.get();}
    int size() const {return 10;}
    void showData() const {
        std::cout << "CPP: [";
        for (int i = 0; i < size() - 1; ++i) {
            std::cout << iData[i] << ", ";
        }
        std::cout << iData[size() - 1];
        std::cout << "]" << std::endl;
    }
};

// -------- Templated wrapper -------------------------------------------------
template <typename DataType>
void AddMyClass(pybind11::module &m, const std::string &aTypeString) {
    using MyType = MyClass<DataType>;
    std::string typeStr = "MyClass" + aTypeString;
    py::class_<MyType>(m, typeStr.c_str(), py::buffer_protocol())
            .def(py::init())
            .def("size", &MyType::size, "Returns size")
            .def("showData", &MyType::showData, "Shows data in CPP side")
            .def_buffer([](MyType &a) -> py::buffer_info{
                return py::buffer_info(
                        a.data(),
                        sizeof(DataType),
                        py::format_descriptor<DataType>::format(),
                        1,
                        {a.size()},
                        {sizeof(DataType)}
                );
            });
}


// -------- Definition of python module ---------------------------------------
PYBIND11_MODULE(TEMPLATE_PYTHON_MODULE_NAME, m) {
    m.doc() = "python binding for MyCLass library";
    m.attr("__version__") = "1.0.0";

    AddMyClass<uint8_t>(m, "Byte");
    AddMyClass<uint16_t>(m, "Short");
    AddMyClass<float>(m, "Float");
}
