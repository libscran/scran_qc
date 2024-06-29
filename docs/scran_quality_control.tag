<?xml version='1.0' encoding='UTF-8' standalone='yes' ?>
<tagfile doxygen_version="1.9.8">
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
    <namespace>scran::choose_filter_thresholds</namespace>
    <namespace>scran::find_median_mad</namespace>
    <namespace>scran::per_cell_qc_metrics</namespace>
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
</tagfile>
