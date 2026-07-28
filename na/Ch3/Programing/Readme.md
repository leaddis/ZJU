# Ch3 project 

## Structure
```
project ├─ src │ 
            ├─ failure│ 
            ├─ testprogram|
        ├─ pics │ 
        ├─ report │ 
        ├─ makefile 
        └─ README.md
```
To simplify the structure, design and report have been combined, with the design section at the beginning of the report.
## Prerequisites

- **C++ Compiler**: Ensure you have `g++` installed.
- **Eigen**: Ensure you have `Eigen` installed.
- **Python**: Ensure you have `python3` installed.
- **LaTeX**: Ensure you have a LaTeX distribution installed (e.g., TeX Live, MiKTeX).
- **Matplotlib**: Install Matplotlib for Python using `pip install matplotlib`.
- **numpy**:Install Matplotlib for Python using `pip install numpy`.
- **scipy**:Install Matplotlib for Python using `pip install scipy`.

## Usage

### Compiling and Running all programs and compiling the report

```bash
make 
```

### Compiling and Running single Programs for specific problem

To compile and run the single C++ programs and generate picture with Python programs, use the following commands:

```bash
make A
make C
make D
make E1
make E2
make E3
make F1
make F2
```
### Compiling the Report
Before doing this, you should make sure that you have compiled all the programs.
My report.pdf contains the design document and report.

```bash
make doc
```
### Clean
```bash
make clean
```