#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <iostream>
#include <sstream>

namespace py = pybind11;

// -------- Check if properly configured in CMAKE -----------------------------
#ifndef TEMPLATE_PYTHON_MODULE_NAME
#error "Name of module (python binding) is not defined!"
#endif

// -------- Class to be wrapped in python ----------------------------
template <typename T>
class MyClass {
    std::unique_ptr<T[]> iData;
    size_t iSize;
    bool releaseMemory;

public:

    MyClass (size_t size) : iSize(size) {
        iData.reset( new T[size] );
        releaseMemory = true;
        for (size_t i = 0; i < size; ++i) iData[i] = 2 * (i + 1);
    }
    ~MyClass() {
        std::cout << "~MyClass[" << iSize << "]" << std::endl;
        if (!releaseMemory) iData.release();
    }

    size_t size() const { return iSize; }
    T *data() { return iData.get(); }

    T& operator[](size_t idx) { return iData[idx]; }
    const T& operator[](size_t idx) const { return iData[idx]; }

    void setNew(T *newData, size_t newSize) {
        if (releaseMemory) {
            std::cout << "released MyClass[" << iSize << "]" << std::endl;
            iData.reset();
        }
        else iData.release();
        iData.reset(newData);
        iSize = newSize;
        releaseMemory = false;
    }

    std::string toString() {
        std::ostringstream s;
        s << "CPP: [";
        for (size_t i = 0; i < size() - 1; ++i) {
            s << iData[i] << ", ";
        }
        s << iData[size() - 1];
        s << "]";
        return s.str();
    }
};

// -------- Templated wrapper -------------------------------------------------
template <typename DataType>
void AddMyClass(pybind11::module &m, const std::string &aTypeString) {
    using MyType = MyClass<DataType>;
    std::string typeStr = "MyClass" + aTypeString;
    py::class_<MyType>(m, typeStr.c_str(), py::buffer_protocol())
            .def(py::init<size_t>())
            .def("__repr__", &MyType::toString, "Default repr function")
            .def("setNew", [](MyType &s, py::array_t<DataType, py::array::c_style> &a) {
                s.setNew(a.mutable_data(0), a.size());
                std::cout << "PYTHON OWNS DATA: " << a.owndata() << std::endl;
                std::cout << "PYTHON REFS: " << a.ref_count() << std::endl;
                a.inc_ref();
                std::cout << "PYTHON REFS: " << a.ref_count() << std::endl;
            })
            .def("__getitem__", [](const MyType &s, size_t i) {
                if (i >= s.size()) throw py::index_error();
                return s[i];
            })
            .def("__setitem__", [](MyType &s, size_t i, DataType v) {
                if (i >= s.size()) throw py::index_error();
                s[i] = v;
            })
            .def("__len__", &MyType::size)
            .def_buffer([](MyType &a) -> py::buffer_info {
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
