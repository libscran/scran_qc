<?xml version='1.0' encoding='UTF-8' standalone='yes' ?>
<tagfile doxygen_version="1.9.8">
  <compound kind="file">
    <name>find_median_mad.hpp</name>
    <path>scran/</path>
    <filename>find__median__mad_8hpp.html</filename>
    <class kind="struct">scran::find_median_mad::Options</class>
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
    <name>scran.hpp</name>
    <path>scran/</path>
    <filename>scran_8hpp.html</filename>
    <namespace>scran</namespace>
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
    <templarg>typename Value_</templarg>
    <templarg>typename Index_</templarg>
    <member kind="function">
      <type></type>
      <name>Workspace</name>
      <anchorfile>classscran_1_1find__median__mad_1_1Workspace.html</anchorfile>
      <anchor>afdbea56a9d375b9bbe2630606d07237e</anchor>
      <arglist>(Index_ num, const Block_ *block)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Workspace</name>
      <anchorfile>classscran_1_1find__median__mad_1_1Workspace.html</anchorfile>
      <anchor>ab7d5b7dcf673cd5cc9f298274f1bc1a6</anchor>
      <arglist>()=default</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>set</name>
      <anchorfile>classscran_1_1find__median__mad_1_1Workspace.html</anchorfile>
      <anchor>af2756170b6b28f2711a35283d9a37e77</anchor>
      <arglist>(Index_ num, const Block_ *block)</arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>scran</name>
    <filename>namespacescran.html</filename>
    <namespace>scran::find_median_mad</namespace>
    <namespace>scran::per_cell_qc_metrics</namespace>
  </compound>
  <compound kind="namespace">
    <name>scran::find_median_mad</name>
    <filename>namespacescran_1_1find__median__mad.html</filename>
    <class kind="struct">scran::find_median_mad::Options</class>
    <class kind="class">scran::find_median_mad::Workspace</class>
    <member kind="function">
      <type>std::pair&lt; Value_, Value_ &gt;</type>
      <name>compute</name>
      <anchorfile>namespacescran_1_1find__median__mad.html</anchorfile>
      <anchor>ac8795940e464a71c9be6969b9919b037</anchor>
      <arglist>(Index_ num, Value_ *metrics, const Options &amp;options)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; std::pair&lt; Value_, Value_ &gt; &gt;</type>
      <name>compute_blocked</name>
      <anchorfile>namespacescran_1_1find__median__mad.html</anchorfile>
      <anchor>a559ce304323c302de8853c21cb620204</anchor>
      <arglist>(Index_ num, const Block_ *block, const Value_ *metrics, Workspace&lt; Value_, Index_ &gt; &amp;workspace, const Options &amp;options)</arglist>
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
</tagfile>
