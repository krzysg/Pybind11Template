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
class MyVectorClass {
    std::unique_ptr<T[]> iData;
    size_t iSize = 0;

    bool releaseMemory = false;
    pybind11::handle handle;

public:

    MyVectorClass() : MyVectorClass(0) {}

    explicit MyVectorClass(size_t aSize) {
        iSize = aSize;
        iData.reset( new T[aSize] );
        releaseMemory = true;

        // init with a 1..size sequence
        for (size_t i = 0; i < aSize; ++i) iData[i] = i + 1;
    }

    ~MyVectorClass() {
        releasePython();
    }

    size_t size() const { return iSize; }

    T *data() { return iData.get(); }
    const T *data() const { return iData.get(); }
    T& operator[](size_t idx) { return iData[idx]; }
    const T& operator[](size_t idx) const { return iData[idx]; }

    void releasePython() {
        if (!releaseMemory) {
            if (handle.ptr() != nullptr) {
                handle.dec_ref(); // decrease reference counter - it will allow python to free object when not in use
                handle.ptr() = nullptr;
            }
            iData.release(); // release pointer from unique_ptr without freeing
        }
    }

    void setFromPython(T *aDataPtr, size_t aDataSize, const pybind11::handle &aPythonObjHandle) {
        releasePython();
        handle.ptr() = aPythonObjHandle.ptr();
        iData.reset(aDataPtr);
        iSize = aDataSize;
        releaseMemory = false;
    }

    std::string toString() {
        std::ostringstream s;
        s << "CPP: [";
        if (iSize > 0) {
            for (size_t i = 0; i < iSize - 1; ++i) { s << iData[i] << ", "; }
            s << iData[iSize - 1];
        }
        s << "]";
        return s.str();
    }
};

// -------- Templated wrapper -------------------------------------------------
template <typename DataType>
void AddMyClass(pybind11::module &aPythonModule, const std::string &aTypeString) {
    using MyType = MyVectorClass<DataType>;
    std::string typeStr = "MyVectorClass" + aTypeString;
    py::class_<MyType>(aPythonModule, typeStr.c_str(), py::buffer_protocol())
        .def(py::init([=](py::buffer b) {
            py::buffer_info info = b.request();

            if (info.ndim != 1) throw std::runtime_error("Incompatible buffer dimension!");
            if (info.format != py::format_descriptor<DataType>::format()) {
                std::string errMsg = "Incompatible format: expected a <";
                errMsg += aTypeString;
                errMsg += "> type vector!";
                throw std::runtime_error(errMsg);
            }
            auto *obj = new MyType();
            obj->setFromPython(static_cast<DataType*>(info.ptr), static_cast<size_t>(info.size), b.release());
            return obj;
        }))
        .def(py::init<size_t>())
        .def("__repr__", &MyType::toString)
        .def("__getitem__", [](const MyType &obj, size_t idx) {
            if (idx >= obj.size()) throw py::index_error();
            return obj[idx];
        })
        .def("__setitem__", [](MyType &obj, size_t idx, DataType value) {
            if (idx >= obj.size()) throw py::index_error();
            obj[idx] = value;
        })
        .def("__len__", &MyType::size)
        .def_buffer([](MyType &obj) -> py::buffer_info {
            return py::buffer_info(
                    obj.data(),
                    sizeof(DataType),
                    py::format_descriptor<DataType>::format(),
                    1,
                    {obj.size()},
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
