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
#include "scran/rna_quality_control.hpp"

tatami::Matrix<double, int>* ptr = some_data_source();
std::vector<std::vector<int> > subsets;
subsets.push_back(some_mito_subsets()); // vector of row indices for mitochondrial genes.

scran::rna_quality_control::MetricsOptions mopt;
auto metrics = scran::rna_quality_control::compute_metrics(ptr, subsets, mopt);

metrics.sum; // vector of count sums across cells. 
metrics.detected; // vector of detected genes across cells.
metrics.subset_proportion[0]; // vector of mitochondrial proportions.
```

We can then use this to identify high-quality cells:

```cpp
scran::rna_quality_control::FiltersOptions fopt;
auto filters = scran::rna_quality_control::compute_filters(metrics, fopt);

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
tatami::Matrix<double, int>* ptr = some_data_source();
std::vector<std::vector<int> > subsets;
subsets.push_back(some_IgG_subsets()); // vector of row indices for IgG controls.

scran::adt_quality_control::MetricsOptions mopt;
auto metrics = scran::adt_quality_control::compute_metrics(ptr, subsets, mopt);
scran::adt_quality_control::FiltersOptions fopt;
auto filters = scran::adt_quality_control::compute_filters(metrics, fopt);
auto keep = filters.filter(metrics);
```

Check out the [reference documentation](https://libscran.github.io/quality_control) for more details.

## Building projects

This repository is part of the broader [**libscran**](https://github.com/libscran/libscran) library,
so users are recommended to use the latter in their projects.
**libscran** developers should just use CMake with `FetchContent`:

```cmake
include(FetchContent)

FetchContent_Declare(
  scran_quality_control 
  GIT_REPOSITORY https://github.com/libscran/quality_control
  GIT_TAG master # or any version of interest
)

FetchContent_MakeAvailable(scran_quality_control)

# For executables:
target_link_libraries(myexe scran_quality_control)

# For libaries
target_link_libraries(mylib INTERFACE scran_quality_control)
```
