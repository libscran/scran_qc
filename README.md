# Quality control on single-cell data

![Unit tests](https://github.com/libscran/quality_control/actions/workflows/run-tests.yaml/badge.svg)
![Documentation](https://github.com/libscran/quality_control/actions/workflows/doxygenate.yaml/badge.svg)
[![Codecov](https://codecov.io/gh/libscran/quality_control/graph/badge.svg?token=JWV0I4WJX2)](https://codecov.io/gh/libscran/quality_control)

## Overview

This repository contains functions to perform quality control on cells, using metrics computed from a gene-by-cell matrix of expression values.
Cells with "unusual" values for these metrics are considered to be of low quality and are filtered out prior to downstream analysis.
The code itself was originally derived from the [**scran** R package](https://bioconductor.org/packages/scran),
factored out into a separate C++ library for easier re-use.

## Quick start

Given a [`tatami::Matrix`](https://github.com/tatami-inc/tatami) containing RNA data,
we can compute some common statistics like the sum of counts, number of detected genes, and the mitochondrial proportion:

```cpp
#include "scran_qc/scran_qc.hpp"

std::shared_ptr<tatami::Matrix<double, int> > mat = some_data_source();
std::vector<std::vector<int> > subsets;
subsets.push_back(some_mito_subsets()); // vector of row indices for mitochondrial genes.

scran_qc::ComputeRnaQcMetricsOptions mopt;
auto metrics = scran_qc::compute_rna_qc_metrics(*mat, subsets, mopt);

metrics.sum; // vector of count sums across cells. 
metrics.detected; // vector of detected genes across cells.
metrics.subset_proportion[0]; // vector of mitochondrial proportions.
```

We can then use this to identify high-quality cells:

```cpp
scran_qc::ComputeRnaQcFiltersOptions fopt;
auto filters = scran_qc::compute_rna_qc_filters(metrics, fopt);

filters.get_sum(); // lower threshold on the sum.
filters.get_detected(); // lower threshold on the number of detected genes.
filters.get_subset_proportion()[0]; // upper threshold on the mitochondrial proportion.

auto keep = filters.filter(metrics); // vector where 1 = high-quality, 0 = low-quality.
```

Users can also manually adjust the thresholds before filtering:

```cpp
filters.get_sum() = 500;
filters.get_subset_proportion()[0] = 0.1;
```

The same general approach applies to ADT and CRISPR data, albeit with different metrics that are most relevant to each modality. 
For example, we use the sum of counts for the IgG isotype control when filtering ADT metrics:

```cpp
std::shared_ptr<tatami::Matrix<double, int> > adt_mat = some_adt_data_source();
std::vector<std::vector<int> > asubsets;
asubsets.push_back(some_IgG_subsets()); // vector of row indices for IgG controls.

scran_qc::ComputeAdtQcMetricsOptions amopt;
auto ametrics = scran_qc::compute_adt_qc_metrics(*adt_mat, asubsets, amopt);
scran_qc::ComputeAdtQcFiltersOptions afopt;
auto afilters = scran_qc::compute_adt_qc_filters(ametrics, afopt);
auto akeep = filters.filter(ametrics);
```

Once we have our filter(s), we can subset our dataset so that only the columns corresponding to high-quality cells are used for downstream analysis:

```cpp
auto submatrix = tatami::make_DelayedSubset(
    mat, 
    scran_qc::filter_index<int>(keep.size(), keep.data()),
    /* by_row = */ false
);

// Combine filters from multiple modalities:
auto submatrix2 = tatami::make_DelayedSubset(
    mat, 
    scran_qc::combine_filters_index<int>(keep.size(), { keep.data(), akeep.data() }),
    /* by_row = */ false
);
```

Check out the [reference documentation](https://libscran.github.io/scran_qc) for more details.

## Building projects

### CMake with `FetchContent`

If you're using CMake, you just need to add something like this to your `CMakeLists.txt`:

```cmake
include(FetchContent)

FetchContent_Declare(
  scran_qc
  GIT_REPOSITORY https://github.com/libscran/scran_qc
  GIT_TAG master # or any version of interest
)

FetchContent_MakeAvailable(scran_qc)
```

Then you can link to **scran_qc** to make the headers available during compilation:

```cmake
# For executables:
target_link_libraries(myexe libscran::scran_qc)

# For libaries
target_link_libraries(mylib INTERFACE libscran::scran_qc)
```

### CMake with `find_package()`

```cmake
find_package(libscran_scran_qc CONFIG REQUIRED)
target_link_libraries(mylib INTERFACE libscran::scran_qc)
```

To install the library, use:

```sh
mkdir build && cd build
cmake .. -DSCRAN_QC_TESTS=OFF
cmake --build . --target install
```

By default, this will use `FetchContent` to fetch all external dependencies.
If you want to install them manually, use `-DSCRAN_QC_FETCH_EXTERN=OFF`.
See the tags in [`extern/CMakeLists.txt`](extern/CMakeLists.txt) to find compatible versions of each dependency.

### Manual

If you're not using CMake, the simple approach is to just copy the files in `include/` - either directly or with Git submodules - and include their path during compilation with, e.g., GCC's `-I`.
This requires the external dependencies listed in [`extern/CMakeLists.txt`](extern/CMakeLists.txt), which also need to be made available during compilation.
