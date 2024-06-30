<?xml version='1.0' encoding='UTF-8' standalone='yes' ?>
<tagfile doxygen_version="1.9.8">
  <compound kind="file">
    <name>adt_quality_control.hpp</name>
    <path>scran/</path>
    <filename>adt__quality__control_8hpp.html</filename>
    <includes id="find__median__mad_8hpp" name="find_median_mad.hpp" local="yes" import="no" module="no" objc="no">find_median_mad.hpp</includes>
    <includes id="per__cell__qc__metrics_8hpp" name="per_cell_qc_metrics.hpp" local="yes" import="no" module="no" objc="no">per_cell_qc_metrics.hpp</includes>
    <includes id="choose__filter__thresholds_8hpp" name="choose_filter_thresholds.hpp" local="yes" import="no" module="no" objc="no">choose_filter_thresholds.hpp</includes>
    <class kind="struct">scran::adt_quality_control::MetricsOptions</class>
    <class kind="struct">scran::adt_quality_control::MetricsBuffers</class>
    <class kind="struct">scran::adt_quality_control::MetricsResults</class>
    <class kind="struct">scran::adt_quality_control::FiltersOptions</class>
    <class kind="class">scran::adt_quality_control::Filters</class>
    <class kind="class">scran::adt_quality_control::BlockedFilters</class>
    <namespace>scran</namespace>
    <namespace>scran::adt_quality_control</namespace>
  </compound>
  <compound kind="file">
    <name>choose_filter_thresholds.hpp</name>
    <path>scran/</path>
    <filename>choose__filter__thresholds_8hpp.html</filename>
    <includes id="find__median__mad_8hpp" name="find_median_mad.hpp" local="yes" import="no" module="no" objc="no">find_median_mad.hpp</includes>
    <class kind="struct">scran::choose_filter_thresholds::Options</class>
    <class kind="struct">scran::choose_filter_thresholds::Results</class>
    <namespace>scran</namespace>
    <namespace>scran::choose_filter_thresholds</namespace>
  </compound>
  <compound kind="file">
    <name>crispr_quality_control.hpp</name>
    <path>scran/</path>
    <filename>crispr__quality__control_8hpp.html</filename>
    <includes id="find__median__mad_8hpp" name="find_median_mad.hpp" local="yes" import="no" module="no" objc="no">find_median_mad.hpp</includes>
    <includes id="per__cell__qc__metrics_8hpp" name="per_cell_qc_metrics.hpp" local="yes" import="no" module="no" objc="no">per_cell_qc_metrics.hpp</includes>
    <includes id="choose__filter__thresholds_8hpp" name="choose_filter_thresholds.hpp" local="yes" import="no" module="no" objc="no">choose_filter_thresholds.hpp</includes>
    <class kind="struct">scran::crispr_quality_control::MetricsOptions</class>
    <class kind="struct">scran::crispr_quality_control::MetricsBuffers</class>
    <class kind="struct">scran::crispr_quality_control::MetricsResults</class>
    <class kind="struct">scran::crispr_quality_control::FiltersOptions</class>
    <class kind="class">scran::crispr_quality_control::Filters</class>
    <class kind="class">scran::crispr_quality_control::BlockedFilters</class>
    <namespace>scran</namespace>
    <namespace>scran::crispr_quality_control</namespace>
  </compound>
  <compound kind="file">
    <name>find_median_mad.hpp</name>
    <path>scran/</path>
    <filename>find__median__mad_8hpp.html</filename>
    <class kind="struct">scran::find_median_mad::Options</class>
    <class kind="struct">scran::find_median_mad::Results</class>
    <class kind="class">scran::find_median_mad::Workspace</class>
    <namespace>scran</namespace>
    <namespace>scran::find_median_mad</namespace>
  </compound>
  <compound kind="file">
    <name>per_cell_qc_metrics.hpp</name>
    <path>scran/</path>
    <filename>per__cell__qc__metrics_8hpp.html</filename>
    <class kind="struct">scran::per_cell_qc_metrics::Options</class>
    <class kind="struct">scran::per_cell_qc_metrics::Buffers</class>
    <class kind="struct">scran::per_cell_qc_metrics::Results</class>
    <namespace>scran</namespace>
    <namespace>scran::per_cell_qc_metrics</namespace>
  </compound>
  <compound kind="file">
    <name>rna_quality_control.hpp</name>
    <path>scran/</path>
    <filename>rna__quality__control_8hpp.html</filename>
    <includes id="find__median__mad_8hpp" name="find_median_mad.hpp" local="yes" import="no" module="no" objc="no">find_median_mad.hpp</includes>
    <includes id="per__cell__qc__metrics_8hpp" name="per_cell_qc_metrics.hpp" local="yes" import="no" module="no" objc="no">per_cell_qc_metrics.hpp</includes>
    <includes id="choose__filter__thresholds_8hpp" name="choose_filter_thresholds.hpp" local="yes" import="no" module="no" objc="no">choose_filter_thresholds.hpp</includes>
    <class kind="struct">scran::rna_quality_control::MetricsOptions</class>
    <class kind="struct">scran::rna_quality_control::MetricsBuffers</class>
    <class kind="struct">scran::rna_quality_control::MetricsResults</class>
    <class kind="struct">scran::rna_quality_control::FiltersOptions</class>
    <class kind="class">scran::rna_quality_control::Filters</class>
    <class kind="class">scran::rna_quality_control::BlockedFilters</class>
    <namespace>scran</namespace>
    <namespace>scran::rna_quality_control</namespace>
  </compound>
  <compound kind="file">
    <name>scran.hpp</name>
    <path>scran/</path>
    <filename>scran_8hpp.html</filename>
    <namespace>scran</namespace>
  </compound>
  <compound kind="class">
    <name>scran::adt_quality_control::BlockedFilters</name>
    <filename>classscran_1_1adt__quality__control_1_1BlockedFilters.html</filename>
    <templarg>typename Float_</templarg>
    <member kind="function">
      <type></type>
      <name>BlockedFilters</name>
      <anchorfile>classscran_1_1adt__quality__control_1_1BlockedFilters.html</anchorfile>
      <anchor>a5eac681c579923c82b4cabf71bab6820</anchor>
      <arglist>()=default</arglist>
    </member>
    <member kind="function">
      <type>const std::vector&lt; Float_ &gt; &amp;</type>
      <name>get_detected</name>
      <anchorfile>classscran_1_1adt__quality__control_1_1BlockedFilters.html</anchorfile>
      <anchor>a3def6eab5bf5adc4ec76ec731b7a513b</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>const std::vector&lt; std::vector&lt; Float_ &gt; &gt; &amp;</type>
      <name>get_subset_sum</name>
      <anchorfile>classscran_1_1adt__quality__control_1_1BlockedFilters.html</anchorfile>
      <anchor>a5b90e3a0d4bec366964dce033d62ca08</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; Float_ &gt; &amp;</type>
      <name>get_detected</name>
      <anchorfile>classscran_1_1adt__quality__control_1_1BlockedFilters.html</anchorfile>
      <anchor>aab5bbaa3fa932811a042d086e7e55348</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; std::vector&lt; Float_ &gt; &gt; &amp;</type>
      <name>get_subset_sum</name>
      <anchorfile>classscran_1_1adt__quality__control_1_1BlockedFilters.html</anchorfile>
      <anchor>a73207969697a5295f0a2cbe21fdd5234</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>filter</name>
      <anchorfile>classscran_1_1adt__quality__control_1_1BlockedFilters.html</anchorfile>
      <anchor>abbf03b12aead92a7a4a58d25a32e24dc</anchor>
      <arglist>(Index_ num, const MetricsBuffers&lt; Sum_, Detected_ &gt; &amp;metrics, const Block_ *block, Output_ *output) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>filter</name>
      <anchorfile>classscran_1_1adt__quality__control_1_1BlockedFilters.html</anchorfile>
      <anchor>ac7a2de62de2b1d5f2a527abed3e800df</anchor>
      <arglist>(const MetricsResults&lt; Sum_, Detected_ &gt; &amp;metrics, const Block_ *block, Output_ *output) const</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; Output_ &gt;</type>
      <name>filter</name>
      <anchorfile>classscran_1_1adt__quality__control_1_1BlockedFilters.html</anchorfile>
      <anchor>a5853624608db126e2d99060cad4a667d</anchor>
      <arglist>(const MetricsResults&lt; Sum_, Detected_ &gt; &amp;metrics, const Block_ *block) const</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>scran::crispr_quality_control::BlockedFilters</name>
    <filename>classscran_1_1crispr__quality__control_1_1BlockedFilters.html</filename>
    <templarg>typename Float_</templarg>
    <member kind="function">
      <type></type>
      <name>BlockedFilters</name>
      <anchorfile>classscran_1_1crispr__quality__control_1_1BlockedFilters.html</anchorfile>
      <anchor>a402004aa5474729739a234c4e53aac95</anchor>
      <arglist>()=default</arglist>
    </member>
    <member kind="function">
      <type>const std::vector&lt; Float_ &gt; &amp;</type>
      <name>get_max_value</name>
      <anchorfile>classscran_1_1crispr__quality__control_1_1BlockedFilters.html</anchorfile>
      <anchor>a738a223db413ed6cf69ef8d7899a0935</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; Float_ &gt; &amp;</type>
      <name>get_max_value</name>
      <anchorfile>classscran_1_1crispr__quality__control_1_1BlockedFilters.html</anchorfile>
      <anchor>a0ebca01693da9084577412ae3e32216a</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>filter</name>
      <anchorfile>classscran_1_1crispr__quality__control_1_1BlockedFilters.html</anchorfile>
      <anchor>af98d5546e74c7500c373a97626411514</anchor>
      <arglist>(size_t num, const MetricsBuffers&lt; Sum_, Detected_, Value_, Index_ &gt; &amp;metrics, const Block_ *block, Output_ *output) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>filter</name>
      <anchorfile>classscran_1_1crispr__quality__control_1_1BlockedFilters.html</anchorfile>
      <anchor>acf4ed3bf581e289236521bf7ddd084b1</anchor>
      <arglist>(const MetricsResults&lt; Sum_, Detected_, Value_, Index_ &gt; &amp;metrics, const Block_ *block, Output_ *output) const</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; Output_ &gt;</type>
      <name>filter</name>
      <anchorfile>classscran_1_1crispr__quality__control_1_1BlockedFilters.html</anchorfile>
      <anchor>aa05dbd507a7b01d75c221336d123826e</anchor>
      <arglist>(const MetricsResults&lt; Sum_, Detected_, Value_, Index_ &gt; &amp;metrics, const Block_ *block) const</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>scran::rna_quality_control::BlockedFilters</name>
    <filename>classscran_1_1rna__quality__control_1_1BlockedFilters.html</filename>
    <templarg>typename Float_</templarg>
    <member kind="function">
      <type></type>
      <name>BlockedFilters</name>
      <anchorfile>classscran_1_1rna__quality__control_1_1BlockedFilters.html</anchorfile>
      <anchor>a8cb9f3b425e8bc82adf24ccbf24fec47</anchor>
      <arglist>()=default</arglist>
    </member>
    <member kind="function">
      <type>const std::vector&lt; Float_ &gt; &amp;</type>
      <name>get_sum</name>
      <anchorfile>classscran_1_1rna__quality__control_1_1BlockedFilters.html</anchorfile>
      <anchor>a0e53c97f301448f5c8ca64a641aed74b</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>const std::vector&lt; Float_ &gt; &amp;</type>
      <name>get_detected</name>
      <anchorfile>classscran_1_1rna__quality__control_1_1BlockedFilters.html</anchorfile>
      <anchor>abca2ae9ce6ef91f8f87ee411dceaf23c</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>const std::vector&lt; std::vector&lt; Float_ &gt; &gt; &amp;</type>
      <name>get_subset_proportion</name>
      <anchorfile>classscran_1_1rna__quality__control_1_1BlockedFilters.html</anchorfile>
      <anchor>a5b6b7457fa31a49f3d2894a21098dd2b</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; Float_ &gt; &amp;</type>
      <name>get_sum</name>
      <anchorfile>classscran_1_1rna__quality__control_1_1BlockedFilters.html</anchorfile>
      <anchor>af1bf77f244d02e1335c1b87f27cd6360</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; Float_ &gt; &amp;</type>
      <name>get_detected</name>
      <anchorfile>classscran_1_1rna__quality__control_1_1BlockedFilters.html</anchorfile>
      <anchor>a9a6502aa1d2451c14051ca4c87005931</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; std::vector&lt; Float_ &gt; &gt; &amp;</type>
      <name>get_subset_proportion</name>
      <anchorfile>classscran_1_1rna__quality__control_1_1BlockedFilters.html</anchorfile>
      <anchor>a7e1010303176c36a122f88a77dbc4595</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>filter</name>
      <anchorfile>classscran_1_1rna__quality__control_1_1BlockedFilters.html</anchorfile>
      <anchor>a5a86beff22297abe4fa54976f1bbc0b9</anchor>
      <arglist>(Index_ num, const MetricsBuffers&lt; Sum_, Detected_, Proportion_ &gt; &amp;metrics, const Block_ *block, Output_ *output) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>filter</name>
      <anchorfile>classscran_1_1rna__quality__control_1_1BlockedFilters.html</anchorfile>
      <anchor>a4870fd40911f71133f4f3289264aaaaf</anchor>
      <arglist>(const MetricsResults&lt; Sum_, Detected_, Proportion_ &gt; &amp;metrics, const Block_ *block, Output_ *output) const</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; Output_ &gt;</type>
      <name>filter</name>
      <anchorfile>classscran_1_1rna__quality__control_1_1BlockedFilters.html</anchorfile>
      <anchor>a58a34d66a4b2b1acdfe3989a28263bdd</anchor>
      <arglist>(const MetricsResults&lt; Sum_, Detected_, Proportion_ &gt; &amp;metrics, const Block_ *block) const</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>scran::per_cell_qc_metrics::Buffers</name>
    <filename>structscran_1_1per__cell__qc__metrics_1_1Buffers.html</filename>
    <templarg>typename Sum_</templarg>
    <templarg>typename Detected_</templarg>
    <templarg>typename Value_</templarg>
    <templarg>typename Index_</templarg>
    <member kind="variable">
      <type>Sum_ *</type>
      <name>sum</name>
      <anchorfile>structscran_1_1per__cell__qc__metrics_1_1Buffers.html</anchorfile>
      <anchor>a7df53935bc006d1a9ca45f52dc70fc55</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>Detected_ *</type>
      <name>detected</name>
      <anchorfile>structscran_1_1per__cell__qc__metrics_1_1Buffers.html</anchorfile>
      <anchor>a09314a61b5355f1f06b3bc24e2460978</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>Index_ *</type>
      <name>max_index</name>
      <anchorfile>structscran_1_1per__cell__qc__metrics_1_1Buffers.html</anchorfile>
      <anchor>a4355495b679694522fd6343c9d1152f3</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>Value_ *</type>
      <name>max_value</name>
      <anchorfile>structscran_1_1per__cell__qc__metrics_1_1Buffers.html</anchorfile>
      <anchor>adc62170882eb23ce3e1ad84a5b5663fc</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::vector&lt; Sum_ * &gt;</type>
      <name>subset_sum</name>
      <anchorfile>structscran_1_1per__cell__qc__metrics_1_1Buffers.html</anchorfile>
      <anchor>a1b8652017957ca65e91722a9c61eb5c4</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::vector&lt; Detected_ * &gt;</type>
      <name>subset_detected</name>
      <anchorfile>structscran_1_1per__cell__qc__metrics_1_1Buffers.html</anchorfile>
      <anchor>a72b0bd35fcc484bc122be556e0e0aeee</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>scran::adt_quality_control::Filters</name>
    <filename>classscran_1_1adt__quality__control_1_1Filters.html</filename>
    <templarg>typename Float_</templarg>
    <member kind="function">
      <type></type>
      <name>Filters</name>
      <anchorfile>classscran_1_1adt__quality__control_1_1Filters.html</anchorfile>
      <anchor>a0143dfc5db87663b47bd1c4ff1eaa127</anchor>
      <arglist>()=default</arglist>
    </member>
    <member kind="function">
      <type>Float_</type>
      <name>get_detected</name>
      <anchorfile>classscran_1_1adt__quality__control_1_1Filters.html</anchorfile>
      <anchor>ae006769b74ba85b4c70cbe7e4c12f3cb</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>const std::vector&lt; Float_ &gt; &amp;</type>
      <name>get_subset_sum</name>
      <anchorfile>classscran_1_1adt__quality__control_1_1Filters.html</anchorfile>
      <anchor>a90e26131d503e5682d03cac8714e0024</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>Float_ &amp;</type>
      <name>get_detected</name>
      <anchorfile>classscran_1_1adt__quality__control_1_1Filters.html</anchorfile>
      <anchor>aae164bfead594118c99b4f6e097e8dd8</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; Float_ &gt; &amp;</type>
      <name>get_subset_sum</name>
      <anchorfile>classscran_1_1adt__quality__control_1_1Filters.html</anchorfile>
      <anchor>aa80cc033cfb028b56cb113addd96ede9</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>filter</name>
      <anchorfile>classscran_1_1adt__quality__control_1_1Filters.html</anchorfile>
      <anchor>aca1a528ec6925618e03e350019bf8707</anchor>
      <arglist>(size_t num, const MetricsBuffers&lt; Sum_, Detected_ &gt; &amp;metrics, Output_ *output) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>filter</name>
      <anchorfile>classscran_1_1adt__quality__control_1_1Filters.html</anchorfile>
      <anchor>ab6f0b5bae60a75ac9745ea774bdf087a</anchor>
      <arglist>(const MetricsResults&lt; Sum_, Detected_ &gt; &amp;metrics, Output_ *output) const</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; Output_ &gt;</type>
      <name>filter</name>
      <anchorfile>classscran_1_1adt__quality__control_1_1Filters.html</anchorfile>
      <anchor>a596712451b6f823d80c849805db519af</anchor>
      <arglist>(const MetricsResults&lt; Sum_, Detected_ &gt; &amp;metrics) const</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>scran::crispr_quality_control::Filters</name>
    <filename>classscran_1_1crispr__quality__control_1_1Filters.html</filename>
    <templarg>typename Float_</templarg>
    <member kind="function">
      <type></type>
      <name>Filters</name>
      <anchorfile>classscran_1_1crispr__quality__control_1_1Filters.html</anchorfile>
      <anchor>ae67b5c6cf4bd2c066937be1873f174e8</anchor>
      <arglist>()=default</arglist>
    </member>
    <member kind="function">
      <type>Float_</type>
      <name>get_max_value</name>
      <anchorfile>classscran_1_1crispr__quality__control_1_1Filters.html</anchorfile>
      <anchor>a05128c13ad180f0da43444ce14203ffe</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>Float_ &amp;</type>
      <name>get_max_value</name>
      <anchorfile>classscran_1_1crispr__quality__control_1_1Filters.html</anchorfile>
      <anchor>accd99968f2487317178c7f52dd5fede2</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>filter</name>
      <anchorfile>classscran_1_1crispr__quality__control_1_1Filters.html</anchorfile>
      <anchor>af819567873395d1befb493a542ca586e</anchor>
      <arglist>(size_t num, const MetricsBuffers&lt; Sum_, Detected_, Value_, Index_ &gt; &amp;metrics, Output_ *output) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>filter</name>
      <anchorfile>classscran_1_1crispr__quality__control_1_1Filters.html</anchorfile>
      <anchor>a2831a85ee73edd21326c7c9d7fd81de7</anchor>
      <arglist>(const MetricsResults&lt; Sum_, Detected_, Value_, Index_ &gt; &amp;metrics, Output_ *output) const</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; Output_ &gt;</type>
      <name>filter</name>
      <anchorfile>classscran_1_1crispr__quality__control_1_1Filters.html</anchorfile>
      <anchor>aed0aff71c39a3f783b8a8c553fb930d1</anchor>
      <arglist>(const MetricsResults&lt; Sum_, Detected_, Value_, Index_ &gt; &amp;metrics) const</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>scran::rna_quality_control::Filters</name>
    <filename>classscran_1_1rna__quality__control_1_1Filters.html</filename>
    <templarg>typename Float_</templarg>
    <member kind="function">
      <type></type>
      <name>Filters</name>
      <anchorfile>classscran_1_1rna__quality__control_1_1Filters.html</anchorfile>
      <anchor>aa3adcab2d39bae88a0c22e1cf0728b24</anchor>
      <arglist>()=default</arglist>
    </member>
    <member kind="function">
      <type>Float_</type>
      <name>get_sum</name>
      <anchorfile>classscran_1_1rna__quality__control_1_1Filters.html</anchorfile>
      <anchor>a812babea7b69476ab116bbdf3791472f</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>Float_</type>
      <name>get_detected</name>
      <anchorfile>classscran_1_1rna__quality__control_1_1Filters.html</anchorfile>
      <anchor>aa42537d9f4787c90dfa3ea84f5eecf09</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>const std::vector&lt; Float_ &gt; &amp;</type>
      <name>get_subset_proportion</name>
      <anchorfile>classscran_1_1rna__quality__control_1_1Filters.html</anchorfile>
      <anchor>a61f4eb22f81c8059a1104e19cd744d18</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>Float_ &amp;</type>
      <name>get_sum</name>
      <anchorfile>classscran_1_1rna__quality__control_1_1Filters.html</anchorfile>
      <anchor>ac4849304286a619c2f62aadf4bf15518</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>Float_ &amp;</type>
      <name>get_detected</name>
      <anchorfile>classscran_1_1rna__quality__control_1_1Filters.html</anchorfile>
      <anchor>a1c64a9a79a10747eaa1e3f28c38011a5</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; Float_ &gt; &amp;</type>
      <name>get_subset_proportion</name>
      <anchorfile>classscran_1_1rna__quality__control_1_1Filters.html</anchorfile>
      <anchor>aad69475f6440fbbddf1ca2c08f7f0f17</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>filter</name>
      <anchorfile>classscran_1_1rna__quality__control_1_1Filters.html</anchorfile>
      <anchor>a5f15147ad61823b604e03fc25fcc7934</anchor>
      <arglist>(size_t num, const MetricsBuffers&lt; Sum_, Detected_, Proportion_ &gt; &amp;metrics, Output_ *output) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>filter</name>
      <anchorfile>classscran_1_1rna__quality__control_1_1Filters.html</anchorfile>
      <anchor>a88ca11272e6020be6c9a19ae733077a8</anchor>
      <arglist>(const MetricsResults&lt; Sum_, Detected_, Proportion_ &gt; &amp;metrics, Output_ *output) const</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; Output_ &gt;</type>
      <name>filter</name>
      <anchorfile>classscran_1_1rna__quality__control_1_1Filters.html</anchorfile>
      <anchor>a3a7f21353957e5b1e23148b389ef2a19</anchor>
      <arglist>(const MetricsResults&lt; Sum_, Detected_, Proportion_ &gt; &amp;metrics) const</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>scran::adt_quality_control::FiltersOptions</name>
    <filename>structscran_1_1adt__quality__control_1_1FiltersOptions.html</filename>
    <member kind="variable">
      <type>double</type>
      <name>detected_num_mads</name>
      <anchorfile>structscran_1_1adt__quality__control_1_1FiltersOptions.html</anchorfile>
      <anchor>a8b5a6d3eb5c03f1570c7cef066c3fe69</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>detected_min_drop</name>
      <anchorfile>structscran_1_1adt__quality__control_1_1FiltersOptions.html</anchorfile>
      <anchor>a66c9d36cabca100bf03f87b8800ec896</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>subset_sum_num_mads</name>
      <anchorfile>structscran_1_1adt__quality__control_1_1FiltersOptions.html</anchorfile>
      <anchor>a8ee277de412baee7b8015fb9a59a1e68</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>scran::crispr_quality_control::FiltersOptions</name>
    <filename>structscran_1_1crispr__quality__control_1_1FiltersOptions.html</filename>
    <member kind="variable">
      <type>double</type>
      <name>max_value_num_mads</name>
      <anchorfile>structscran_1_1crispr__quality__control_1_1FiltersOptions.html</anchorfile>
      <anchor>a190ecbb3c51fe7d9b389f93eb64bca94</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>scran::rna_quality_control::FiltersOptions</name>
    <filename>structscran_1_1rna__quality__control_1_1FiltersOptions.html</filename>
    <member kind="variable">
      <type>double</type>
      <name>detected_num_mads</name>
      <anchorfile>structscran_1_1rna__quality__control_1_1FiltersOptions.html</anchorfile>
      <anchor>a4da99582d858c72167ede987b2f45290</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>sum_num_mads</name>
      <anchorfile>structscran_1_1rna__quality__control_1_1FiltersOptions.html</anchorfile>
      <anchor>a480b86353d40466e6a4dc133379f2998</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>subset_proportion_num_mads</name>
      <anchorfile>structscran_1_1rna__quality__control_1_1FiltersOptions.html</anchorfile>
      <anchor>a163a0b9c2e9aebf5f685ed854ed9df5e</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>scran::adt_quality_control::MetricsBuffers</name>
    <filename>structscran_1_1adt__quality__control_1_1MetricsBuffers.html</filename>
    <templarg>typename Sum_</templarg>
    <templarg>typename Detected_</templarg>
    <member kind="variable">
      <type>Sum_ *</type>
      <name>sum</name>
      <anchorfile>structscran_1_1adt__quality__control_1_1MetricsBuffers.html</anchorfile>
      <anchor>a57ccfc1f47a4443767fec76384a6f406</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>Detected_ *</type>
      <name>detected</name>
      <anchorfile>structscran_1_1adt__quality__control_1_1MetricsBuffers.html</anchorfile>
      <anchor>ab4cf91581e146da2be54139b6ee90953</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::vector&lt; Sum_ * &gt;</type>
      <name>subset_sum</name>
      <anchorfile>structscran_1_1adt__quality__control_1_1MetricsBuffers.html</anchorfile>
      <anchor>a541c0a789650dc135e5c759041c4a1c3</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>scran::crispr_quality_control::MetricsBuffers</name>
    <filename>structscran_1_1crispr__quality__control_1_1MetricsBuffers.html</filename>
    <templarg>typename Sum_</templarg>
    <templarg>typename Detected_</templarg>
    <templarg>typename Value_</templarg>
    <templarg>typename Index_</templarg>
    <member kind="variable">
      <type>Sum_ *</type>
      <name>sum</name>
      <anchorfile>structscran_1_1crispr__quality__control_1_1MetricsBuffers.html</anchorfile>
      <anchor>abc73760fe659f64ad87b5ec73c37f587</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>Detected_ *</type>
      <name>detected</name>
      <anchorfile>structscran_1_1crispr__quality__control_1_1MetricsBuffers.html</anchorfile>
      <anchor>adc50f314b70791110d0160ff650433e2</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>Value_ *</type>
      <name>max_value</name>
      <anchorfile>structscran_1_1crispr__quality__control_1_1MetricsBuffers.html</anchorfile>
      <anchor>ad0e2d7df3f528fa07759d232b226e9bc</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>Index_ *</type>
      <name>max_index</name>
      <anchorfile>structscran_1_1crispr__quality__control_1_1MetricsBuffers.html</anchorfile>
      <anchor>a88f7021de51b618ebaf26b3c6725213a</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>scran::rna_quality_control::MetricsBuffers</name>
    <filename>structscran_1_1rna__quality__control_1_1MetricsBuffers.html</filename>
    <templarg>typename Sum_</templarg>
    <templarg>typename Detected_</templarg>
    <templarg>typename Proportion_</templarg>
    <member kind="variable">
      <type>Sum_ *</type>
      <name>sum</name>
      <anchorfile>structscran_1_1rna__quality__control_1_1MetricsBuffers.html</anchorfile>
      <anchor>a4ed06dee15efb5cc2fa9c8956ea67423</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>Detected_ *</type>
      <name>detected</name>
      <anchorfile>structscran_1_1rna__quality__control_1_1MetricsBuffers.html</anchorfile>
      <anchor>aa1e96bff07ad427e8dcd261be8234baf</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::vector&lt; Proportion_ * &gt;</type>
      <name>subset_proportion</name>
      <anchorfile>structscran_1_1rna__quality__control_1_1MetricsBuffers.html</anchorfile>
      <anchor>a6efba9276b83dfa777da8a60aa2bd7b7</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>scran::adt_quality_control::MetricsOptions</name>
    <filename>structscran_1_1adt__quality__control_1_1MetricsOptions.html</filename>
    <member kind="variable">
      <type>int</type>
      <name>num_threads</name>
      <anchorfile>structscran_1_1adt__quality__control_1_1MetricsOptions.html</anchorfile>
      <anchor>a7fab45d26198c9c7c2186a7d6f876af3</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>scran::crispr_quality_control::MetricsOptions</name>
    <filename>structscran_1_1crispr__quality__control_1_1MetricsOptions.html</filename>
    <member kind="variable">
      <type>int</type>
      <name>num_threads</name>
      <anchorfile>structscran_1_1crispr__quality__control_1_1MetricsOptions.html</anchorfile>
      <anchor>a64fcc3fc16105fe2c18b6cb1b54b5e09</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>scran::rna_quality_control::MetricsOptions</name>
    <filename>structscran_1_1rna__quality__control_1_1MetricsOptions.html</filename>
    <member kind="variable">
      <type>int</type>
      <name>num_threads</name>
      <anchorfile>structscran_1_1rna__quality__control_1_1MetricsOptions.html</anchorfile>
      <anchor>ad6706ba6127762f0894342d70237b8fd</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>scran::adt_quality_control::MetricsResults</name>
    <filename>structscran_1_1adt__quality__control_1_1MetricsResults.html</filename>
    <templarg>typename Sum_</templarg>
    <templarg>typename Detected_</templarg>
    <member kind="variable">
      <type>std::vector&lt; Sum_ &gt;</type>
      <name>sum</name>
      <anchorfile>structscran_1_1adt__quality__control_1_1MetricsResults.html</anchorfile>
      <anchor>a0f9d001a5c1e2e7908b1e62ec87acce8</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::vector&lt; Detected_ &gt;</type>
      <name>detected</name>
      <anchorfile>structscran_1_1adt__quality__control_1_1MetricsResults.html</anchorfile>
      <anchor>a8802d821de49cd05090e1b98423fb4da</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::vector&lt; std::vector&lt; Sum_ &gt; &gt;</type>
      <name>subset_sum</name>
      <anchorfile>structscran_1_1adt__quality__control_1_1MetricsResults.html</anchorfile>
      <anchor>a786ea3b7e1bc317d031e6f29b74ff7c8</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>scran::crispr_quality_control::MetricsResults</name>
    <filename>structscran_1_1crispr__quality__control_1_1MetricsResults.html</filename>
    <templarg>typename Sum_</templarg>
    <templarg>typename Detected_</templarg>
    <templarg>typename Value_</templarg>
    <templarg>typename Index_</templarg>
    <member kind="variable">
      <type>std::vector&lt; Sum_ &gt;</type>
      <name>sum</name>
      <anchorfile>structscran_1_1crispr__quality__control_1_1MetricsResults.html</anchorfile>
      <anchor>ae690960c50586e767de0a36f65dac802</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::vector&lt; Detected_ &gt;</type>
      <name>detected</name>
      <anchorfile>structscran_1_1crispr__quality__control_1_1MetricsResults.html</anchorfile>
      <anchor>a5e8a29d574034718c109c49185dcea85</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::vector&lt; Value_ &gt;</type>
      <name>max_value</name>
      <anchorfile>structscran_1_1crispr__quality__control_1_1MetricsResults.html</anchorfile>
      <anchor>adbd58b52aef9b893b72499323c07fdfc</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::vector&lt; Index_ &gt;</type>
      <name>max_index</name>
      <anchorfile>structscran_1_1crispr__quality__control_1_1MetricsResults.html</anchorfile>
      <anchor>a0779ecebf71e3fe6df1a927ad3017af1</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>scran::rna_quality_control::MetricsResults</name>
    <filename>structscran_1_1rna__quality__control_1_1MetricsResults.html</filename>
    <templarg>typename Sum_</templarg>
    <templarg>typename Detected_</templarg>
    <templarg>typename Proportion_</templarg>
    <member kind="variable">
      <type>std::vector&lt; Sum_ &gt;</type>
      <name>sum</name>
      <anchorfile>structscran_1_1rna__quality__control_1_1MetricsResults.html</anchorfile>
      <anchor>a8970e6008c65965f0b852e7ae0f0e32e</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::vector&lt; Detected_ &gt;</type>
      <name>detected</name>
      <anchorfile>structscran_1_1rna__quality__control_1_1MetricsResults.html</anchorfile>
      <anchor>a5a2c8a5976c7f651e7bd3b6979c4cd5b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::vector&lt; std::vector&lt; Proportion_ &gt; &gt;</type>
      <name>subset_proportion</name>
      <anchorfile>structscran_1_1rna__quality__control_1_1MetricsResults.html</anchorfile>
      <anchor>ad8fc92dd666c24bd0d5b4cd66b16d745</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>scran::choose_filter_thresholds::Options</name>
    <filename>structscran_1_1choose__filter__thresholds_1_1Options.html</filename>
    <member kind="variable">
      <type>bool</type>
      <name>lower</name>
      <anchorfile>structscran_1_1choose__filter__thresholds_1_1Options.html</anchorfile>
      <anchor>a42b53952746e5cd3edb080e53c7e4e8f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>upper</name>
      <anchorfile>structscran_1_1choose__filter__thresholds_1_1Options.html</anchorfile>
      <anchor>a7e1b18a563d2706c3b4b0761d6805af9</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>num_mads</name>
      <anchorfile>structscran_1_1choose__filter__thresholds_1_1Options.html</anchorfile>
      <anchor>aa7b2d771468088cb63e949f430644dc6</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>min_diff</name>
      <anchorfile>structscran_1_1choose__filter__thresholds_1_1Options.html</anchorfile>
      <anchor>aefa36f1a53ef0ba54b0694e3c36c97d6</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>log</name>
      <anchorfile>structscran_1_1choose__filter__thresholds_1_1Options.html</anchorfile>
      <anchor>a61296fc2d9c9695879fb36bd63e1d49c</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>scran::find_median_mad::Options</name>
    <filename>structscran_1_1find__median__mad_1_1Options.html</filename>
    <member kind="variable">
      <type>bool</type>
      <name>log</name>
      <anchorfile>structscran_1_1find__median__mad_1_1Options.html</anchorfile>
      <anchor>a1af43eeea4b45f4f6b7847affbbeefc5</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>median_only</name>
      <anchorfile>structscran_1_1find__median__mad_1_1Options.html</anchorfile>
      <anchor>aafc3f5f04ebf8ca926c1c4cc65e05175</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>scran::per_cell_qc_metrics::Options</name>
    <filename>structscran_1_1per__cell__qc__metrics_1_1Options.html</filename>
    <member kind="variable">
      <type>bool</type>
      <name>compute_sum</name>
      <anchorfile>structscran_1_1per__cell__qc__metrics_1_1Options.html</anchorfile>
      <anchor>a16b45c4456b2e3c3224ed30b4ae32b5e</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>compute_detected</name>
      <anchorfile>structscran_1_1per__cell__qc__metrics_1_1Options.html</anchorfile>
      <anchor>a36e17bfef30d6f7327bfeb15fe6411d9</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>compute_max_value</name>
      <anchorfile>structscran_1_1per__cell__qc__metrics_1_1Options.html</anchorfile>
      <anchor>a8eeee4124747f4b03a3bce1e12722a04</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>compute_max_index</name>
      <anchorfile>structscran_1_1per__cell__qc__metrics_1_1Options.html</anchorfile>
      <anchor>aa910e71447b5e681b3fe2acda9ab85dc</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>compute_subset_sum</name>
      <anchorfile>structscran_1_1per__cell__qc__metrics_1_1Options.html</anchorfile>
      <anchor>a2399db3966e6bf1e31e9e2cf903a23ae</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>compute_subset_detected</name>
      <anchorfile>structscran_1_1per__cell__qc__metrics_1_1Options.html</anchorfile>
      <anchor>a680e3102896abe8b77c53afa149e84cc</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>num_threads</name>
      <anchorfile>structscran_1_1per__cell__qc__metrics_1_1Options.html</anchorfile>
      <anchor>ae942b32d5f22b062645e3f981cf3ab07</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>scran::choose_filter_thresholds::Results</name>
    <filename>structscran_1_1choose__filter__thresholds_1_1Results.html</filename>
    <templarg>typename Float_</templarg>
    <member kind="variable">
      <type>Float_</type>
      <name>lower</name>
      <anchorfile>structscran_1_1choose__filter__thresholds_1_1Results.html</anchorfile>
      <anchor>a6f1ca129602cbce09257573c24ec676b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>Float_</type>
      <name>upper</name>
      <anchorfile>structscran_1_1choose__filter__thresholds_1_1Results.html</anchorfile>
      <anchor>a93dff2f562ce66cc5b53410c82935002</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>scran::find_median_mad::Results</name>
    <filename>structscran_1_1find__median__mad_1_1Results.html</filename>
    <templarg>typename Float_</templarg>
    <member kind="variable">
      <type>Float_</type>
      <name>median</name>
      <anchorfile>structscran_1_1find__median__mad_1_1Results.html</anchorfile>
      <anchor>a9f925639f3b8fd1adaa018647bea075b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>Float_</type>
      <name>mad</name>
      <anchorfile>structscran_1_1find__median__mad_1_1Results.html</anchorfile>
      <anchor>af781189174d0843fe5fccd75c50029da</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>scran::per_cell_qc_metrics::Results</name>
    <filename>structscran_1_1per__cell__qc__metrics_1_1Results.html</filename>
    <templarg>typename Sum_</templarg>
    <templarg>typename Detected_</templarg>
    <templarg>typename Value_</templarg>
    <templarg>typename Index_</templarg>
    <member kind="variable">
      <type>std::vector&lt; Sum_ &gt;</type>
      <name>sum</name>
      <anchorfile>structscran_1_1per__cell__qc__metrics_1_1Results.html</anchorfile>
      <anchor>a563dd556e937bf92845333728b330c0c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::vector&lt; Detected_ &gt;</type>
      <name>detected</name>
      <anchorfile>structscran_1_1per__cell__qc__metrics_1_1Results.html</anchorfile>
      <anchor>aba7b4936bef4fb9a77c2d5f96ec21f6c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::vector&lt; Index_ &gt;</type>
      <name>max_index</name>
      <anchorfile>structscran_1_1per__cell__qc__metrics_1_1Results.html</anchorfile>
      <anchor>a45ffcd28424d9999fe8f3b7ee84e60c8</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::vector&lt; Value_ &gt;</type>
      <name>max_value</name>
      <anchorfile>structscran_1_1per__cell__qc__metrics_1_1Results.html</anchorfile>
      <anchor>a29c956a7991972a25bba0659febb104b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::vector&lt; std::vector&lt; Sum_ &gt; &gt;</type>
      <name>subset_sum</name>
      <anchorfile>structscran_1_1per__cell__qc__metrics_1_1Results.html</anchorfile>
      <anchor>ae40fa6e5f7853ee46bf3aed55bc5f4c8</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::vector&lt; std::vector&lt; Detected_ &gt; &gt;</type>
      <name>subset_detected</name>
      <anchorfile>structscran_1_1per__cell__qc__metrics_1_1Results.html</anchorfile>
      <anchor>abcd51c413a4e4bd093cf9416f0c6aa5b</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>scran::find_median_mad::Workspace</name>
    <filename>classscran_1_1find__median__mad_1_1Workspace.html</filename>
    <templarg>typename Float_</templarg>
    <templarg>typename Index_</templarg>
    <member kind="function">
      <type></type>
      <name>Workspace</name>
      <anchorfile>classscran_1_1find__median__mad_1_1Workspace.html</anchorfile>
      <anchor>a45a397c249671eb70e250b5dbb40dbf7</anchor>
      <arglist>(Index_ num, const Block_ *block)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Workspace</name>
      <anchorfile>classscran_1_1find__median__mad_1_1Workspace.html</anchorfile>
      <anchor>a8a74c27c9e317651f82b82a92dcca831</anchor>
      <arglist>()=default</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>set</name>
      <anchorfile>classscran_1_1find__median__mad_1_1Workspace.html</anchorfile>
      <anchor>a25d259a276111845207b37236569d346</anchor>
      <arglist>(Index_ num, const Block_ *block)</arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>scran</name>
    <filename>namespacescran.html</filename>
    <namespace>scran::adt_quality_control</namespace>
    <namespace>scran::choose_filter_thresholds</namespace>
    <namespace>scran::crispr_quality_control</namespace>
    <namespace>scran::find_median_mad</namespace>
    <namespace>scran::per_cell_qc_metrics</namespace>
    <namespace>scran::rna_quality_control</namespace>
  </compound>
  <compound kind="namespace">
    <name>scran::adt_quality_control</name>
    <filename>namespacescran_1_1adt__quality__control.html</filename>
    <class kind="class">scran::adt_quality_control::BlockedFilters</class>
    <class kind="class">scran::adt_quality_control::Filters</class>
    <class kind="struct">scran::adt_quality_control::FiltersOptions</class>
    <class kind="struct">scran::adt_quality_control::MetricsBuffers</class>
    <class kind="struct">scran::adt_quality_control::MetricsOptions</class>
    <class kind="struct">scran::adt_quality_control::MetricsResults</class>
    <member kind="function">
      <type>void</type>
      <name>compute_metrics</name>
      <anchorfile>namespacescran_1_1adt__quality__control.html</anchorfile>
      <anchor>a16d8e4add23d4a08431fde30264ea869</anchor>
      <arglist>(const tatami::Matrix&lt; Value_, Index_ &gt; *mat, const std::vector&lt; Subset_ &gt; &amp;subsets, MetricsBuffers&lt; Sum_, Detected_ &gt; &amp;output, const MetricsOptions &amp;options)</arglist>
    </member>
    <member kind="function">
      <type>MetricsResults&lt; Sum_, Detected_ &gt;</type>
      <name>compute_metrics</name>
      <anchorfile>namespacescran_1_1adt__quality__control.html</anchorfile>
      <anchor>a217e0138960775b917c05731ef18d246</anchor>
      <arglist>(const tatami::Matrix&lt; Value_, Index_ &gt; *mat, const std::vector&lt; Subset_ &gt; &amp;subsets, const MetricsOptions &amp;options)</arglist>
    </member>
    <member kind="function">
      <type>Filters&lt; Float_ &gt;</type>
      <name>compute_filters</name>
      <anchorfile>namespacescran_1_1adt__quality__control.html</anchorfile>
      <anchor>ac2ed5bbd683cbf8731f6d2557924994d</anchor>
      <arglist>(size_t num, const MetricsBuffers&lt; Sum_, Detected_ &gt; &amp;metrics, const FiltersOptions &amp;options)</arglist>
    </member>
    <member kind="function">
      <type>Filters&lt; Float_ &gt;</type>
      <name>compute_filters</name>
      <anchorfile>namespacescran_1_1adt__quality__control.html</anchorfile>
      <anchor>a2934150ffb0ed08bfda09e95b48196bb</anchor>
      <arglist>(const MetricsResults&lt; Sum_, Detected_ &gt; &amp;metrics, const FiltersOptions &amp;options)</arglist>
    </member>
    <member kind="function">
      <type>BlockedFilters&lt; Float_ &gt;</type>
      <name>compute_filters_blocked</name>
      <anchorfile>namespacescran_1_1adt__quality__control.html</anchorfile>
      <anchor>a61e264eff3efa03a66b8ee39df5d510c</anchor>
      <arglist>(size_t num, const MetricsBuffers&lt; Sum_, Detected_ &gt; &amp;metrics, const Block_ *block, const FiltersOptions &amp;options)</arglist>
    </member>
    <member kind="function">
      <type>BlockedFilters&lt; Float_ &gt;</type>
      <name>compute_filters_blocked</name>
      <anchorfile>namespacescran_1_1adt__quality__control.html</anchorfile>
      <anchor>aded7e02913640f70dd8e8fb25959f72d</anchor>
      <arglist>(const MetricsResults&lt; Sum_, Detected_ &gt; &amp;metrics, const Block_ *block, const FiltersOptions &amp;options)</arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>scran::choose_filter_thresholds</name>
    <filename>namespacescran_1_1choose__filter__thresholds.html</filename>
    <class kind="struct">scran::choose_filter_thresholds::Options</class>
    <class kind="struct">scran::choose_filter_thresholds::Results</class>
    <member kind="function">
      <type>Results&lt; Float_ &gt;</type>
      <name>compute</name>
      <anchorfile>namespacescran_1_1choose__filter__thresholds.html</anchorfile>
      <anchor>ae991983721dc1e71443f6ae641f67205</anchor>
      <arglist>(const find_median_mad::Results&lt; Float_ &gt; &amp;mm, const Options &amp;options)</arglist>
    </member>
    <member kind="function">
      <type>Results&lt; Float_ &gt;</type>
      <name>compute</name>
      <anchorfile>namespacescran_1_1choose__filter__thresholds.html</anchorfile>
      <anchor>a56e2b8ed305d4f58b3a0e929db49afaa</anchor>
      <arglist>(Index_ num, Float_ *metrics, const Options &amp;options)</arglist>
    </member>
    <member kind="function">
      <type>Results&lt; Float_ &gt;</type>
      <name>compute</name>
      <anchorfile>namespacescran_1_1choose__filter__thresholds.html</anchorfile>
      <anchor>ae52b9902520938f7de10d808caabe2f4</anchor>
      <arglist>(Index_ num, const Value_ *metrics, Float_ *buffer, const Options &amp;options)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; Results&lt; Float_ &gt; &gt;</type>
      <name>compute_blocked</name>
      <anchorfile>namespacescran_1_1choose__filter__thresholds.html</anchorfile>
      <anchor>adc92a1d34e7ca93a0dd82cdc156d6b12</anchor>
      <arglist>(const std::vector&lt; find_median_mad::Results&lt; Float_ &gt; &gt; mms, const Options &amp;options)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; Results&lt; Float_ &gt; &gt;</type>
      <name>compute_blocked</name>
      <anchorfile>namespacescran_1_1choose__filter__thresholds.html</anchorfile>
      <anchor>ac23c9e27e19730fda2ca0d7d70339c1f</anchor>
      <arglist>(Index_ num, const Value_ *metrics, const Block_ *block, find_median_mad::Workspace&lt; Float_, Index_ &gt; *workspace, const Options &amp;options)</arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>scran::crispr_quality_control</name>
    <filename>namespacescran_1_1crispr__quality__control.html</filename>
    <class kind="class">scran::crispr_quality_control::BlockedFilters</class>
    <class kind="class">scran::crispr_quality_control::Filters</class>
    <class kind="struct">scran::crispr_quality_control::FiltersOptions</class>
    <class kind="struct">scran::crispr_quality_control::MetricsBuffers</class>
    <class kind="struct">scran::crispr_quality_control::MetricsOptions</class>
    <class kind="struct">scran::crispr_quality_control::MetricsResults</class>
    <member kind="function">
      <type>void</type>
      <name>compute_metrics</name>
      <anchorfile>namespacescran_1_1crispr__quality__control.html</anchorfile>
      <anchor>a5e0a41f185fd86479db5ef8a7e5d4116</anchor>
      <arglist>(const tatami::Matrix&lt; Value_, Index_ &gt; *mat, MetricsBuffers&lt; Sum_, Detected_, Value_, Index_ &gt; &amp;output, const MetricsOptions &amp;options)</arglist>
    </member>
    <member kind="function">
      <type>MetricsResults&lt; Sum_, Detected_, Value_, Index_ &gt;</type>
      <name>compute_metrics</name>
      <anchorfile>namespacescran_1_1crispr__quality__control.html</anchorfile>
      <anchor>a5456326ad2cfe939762f0f79d2e9489b</anchor>
      <arglist>(const tatami::Matrix&lt; Value_, Index_ &gt; *mat, const MetricsOptions &amp;options)</arglist>
    </member>
    <member kind="function">
      <type>Filters&lt; Float_ &gt;</type>
      <name>compute_filters</name>
      <anchorfile>namespacescran_1_1crispr__quality__control.html</anchorfile>
      <anchor>a376b9b86602e1beef8b58270ec616063</anchor>
      <arglist>(size_t num, const MetricsBuffers&lt; Sum_, Detected_, Value_, Index_ &gt; &amp;metrics, const FiltersOptions &amp;options)</arglist>
    </member>
    <member kind="function">
      <type>Filters&lt; Float_ &gt;</type>
      <name>compute_filters</name>
      <anchorfile>namespacescran_1_1crispr__quality__control.html</anchorfile>
      <anchor>abc592b0d390ca248c7796ff4333d7aff</anchor>
      <arglist>(const MetricsResults&lt; Sum_, Detected_, Value_, Index_ &gt; &amp;metrics, const FiltersOptions &amp;options)</arglist>
    </member>
    <member kind="function">
      <type>BlockedFilters&lt; Float_ &gt;</type>
      <name>compute_filters_blocked</name>
      <anchorfile>namespacescran_1_1crispr__quality__control.html</anchorfile>
      <anchor>a3c7e3955d25ac8111974ea44fea05ed9</anchor>
      <arglist>(size_t num, const MetricsBuffers&lt; Sum_, Detected_, Value_, Index_ &gt; &amp;metrics, const Block_ *block, const FiltersOptions &amp;options)</arglist>
    </member>
    <member kind="function">
      <type>BlockedFilters&lt; Float_ &gt;</type>
      <name>compute_filters_blocked</name>
      <anchorfile>namespacescran_1_1crispr__quality__control.html</anchorfile>
      <anchor>a41e3134f0f14330ec702ce2421e5a96b</anchor>
      <arglist>(const MetricsResults&lt; Sum_, Detected_, Value_, Index_ &gt; &amp;metrics, const Block_ *block, const FiltersOptions &amp;options)</arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>scran::find_median_mad</name>
    <filename>namespacescran_1_1find__median__mad.html</filename>
    <class kind="struct">scran::find_median_mad::Options</class>
    <class kind="struct">scran::find_median_mad::Results</class>
    <class kind="class">scran::find_median_mad::Workspace</class>
    <member kind="function">
      <type>Results&lt; Float_ &gt;</type>
      <name>compute</name>
      <anchorfile>namespacescran_1_1find__median__mad.html</anchorfile>
      <anchor>a71c945f9e24df90abf065cc20e4067ae</anchor>
      <arglist>(Index_ num, Float_ *metrics, const Options &amp;options)</arglist>
    </member>
    <member kind="function">
      <type>Results&lt; Float_ &gt;</type>
      <name>compute</name>
      <anchorfile>namespacescran_1_1find__median__mad.html</anchorfile>
      <anchor>a2a802ecd5b415d4a3e7219e10b7594e5</anchor>
      <arglist>(Index_ num, const Value_ *metrics, Float_ *buffer, const Options &amp;options)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; Results&lt; Output_ &gt; &gt;</type>
      <name>compute_blocked</name>
      <anchorfile>namespacescran_1_1find__median__mad.html</anchorfile>
      <anchor>a31771db5590e3de43e2ffed92f03fc93</anchor>
      <arglist>(Index_ num, const Value_ *metrics, const Block_ *block, Workspace&lt; Output_, Index_ &gt; *workspace, const Options &amp;options)</arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>scran::per_cell_qc_metrics</name>
    <filename>namespacescran_1_1per__cell__qc__metrics.html</filename>
    <class kind="struct">scran::per_cell_qc_metrics::Buffers</class>
    <class kind="struct">scran::per_cell_qc_metrics::Options</class>
    <class kind="struct">scran::per_cell_qc_metrics::Results</class>
    <member kind="function">
      <type>void</type>
      <name>compute</name>
      <anchorfile>namespacescran_1_1per__cell__qc__metrics.html</anchorfile>
      <anchor>a442874c635f37e35a72d7048246615e2</anchor>
      <arglist>(const tatami::Matrix&lt; Value_, Index_ &gt; *mat, const std::vector&lt; Subset_ &gt; &amp;subsets, Buffers&lt; Sum_, Detected_, Value_, Index_ &gt; &amp;output, const Options &amp;options)</arglist>
    </member>
    <member kind="function">
      <type>Results&lt; Sum_, Detected_, Value_, Index_ &gt;</type>
      <name>compute</name>
      <anchorfile>namespacescran_1_1per__cell__qc__metrics.html</anchorfile>
      <anchor>aee1bf82509fd11171f94679e7eeb096d</anchor>
      <arglist>(const tatami::Matrix&lt; Value_, Index_ &gt; *mat, const std::vector&lt; Subset_ &gt; &amp;subsets, const Options &amp;options)</arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>scran::rna_quality_control</name>
    <filename>namespacescran_1_1rna__quality__control.html</filename>
    <class kind="class">scran::rna_quality_control::BlockedFilters</class>
    <class kind="class">scran::rna_quality_control::Filters</class>
    <class kind="struct">scran::rna_quality_control::FiltersOptions</class>
    <class kind="struct">scran::rna_quality_control::MetricsBuffers</class>
    <class kind="struct">scran::rna_quality_control::MetricsOptions</class>
    <class kind="struct">scran::rna_quality_control::MetricsResults</class>
    <member kind="function">
      <type>void</type>
      <name>compute_metrics</name>
      <anchorfile>namespacescran_1_1rna__quality__control.html</anchorfile>
      <anchor>af46c0710c8d28bd27cae80f79d482ae1</anchor>
      <arglist>(const tatami::Matrix&lt; Value_, Index_ &gt; *mat, const std::vector&lt; Subset_ &gt; &amp;subsets, MetricsBuffers&lt; Sum_, Detected_, Proportion_ &gt; &amp;output, const MetricsOptions &amp;options)</arglist>
    </member>
    <member kind="function">
      <type>MetricsResults&lt; Sum_, Detected_, Proportion_ &gt;</type>
      <name>compute_metrics</name>
      <anchorfile>namespacescran_1_1rna__quality__control.html</anchorfile>
      <anchor>ab533f23fa45aa86c93b4a32fc85be106</anchor>
      <arglist>(const tatami::Matrix&lt; Value_, Index_ &gt; *mat, const std::vector&lt; Subset_ &gt; &amp;subsets, const MetricsOptions &amp;options)</arglist>
    </member>
    <member kind="function">
      <type>Filters&lt; Float_ &gt;</type>
      <name>compute_filters</name>
      <anchorfile>namespacescran_1_1rna__quality__control.html</anchorfile>
      <anchor>a5167e6827b051b450cd7f381f46835ca</anchor>
      <arglist>(size_t num, const MetricsBuffers&lt; Sum_, Detected_, Proportion_ &gt; &amp;metrics, const FiltersOptions &amp;options)</arglist>
    </member>
    <member kind="function">
      <type>Filters&lt; Float_ &gt;</type>
      <name>compute_filters</name>
      <anchorfile>namespacescran_1_1rna__quality__control.html</anchorfile>
      <anchor>a160dc977011c3cf639e39568d0cca6aa</anchor>
      <arglist>(const MetricsResults&lt; Sum_, Detected_, Proportion_ &gt; &amp;metrics, const FiltersOptions &amp;options)</arglist>
    </member>
    <member kind="function">
      <type>BlockedFilters&lt; Float_ &gt;</type>
      <name>compute_filters_blocked</name>
      <anchorfile>namespacescran_1_1rna__quality__control.html</anchorfile>
      <anchor>acd74c87f5984c6d4eb50d3370373ff13</anchor>
      <arglist>(size_t num, const MetricsBuffers&lt; Sum_, Detected_, Proportion_ &gt; &amp;metrics, const Block_ *block, const FiltersOptions &amp;options)</arglist>
    </member>
    <member kind="function">
      <type>BlockedFilters&lt; Float_ &gt;</type>
      <name>compute_filters_blocked</name>
      <anchorfile>namespacescran_1_1rna__quality__control.html</anchorfile>
      <anchor>ab59c03c5dd92fede69594411f0fc7b93</anchor>
      <arglist>(const MetricsResults&lt; Sum_, Detected_, Proportion_ &gt; &amp;metrics, const Block_ *block, const FiltersOptions &amp;options)</arglist>
    </member>
  </compound>
</tagfile>
