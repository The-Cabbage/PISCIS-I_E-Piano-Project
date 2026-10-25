MDA.LV2
=======

This is a port of the MDA VST plugins to LV2.

The plugins are originally by Paul Kellett and were released under the GPL v2+
or MIT license.  Thanks, Paul!

This direct port by David Robillard is released under the same license.  Based
on the original SVN repository <https://svn.code.sf.net/p/mda-vst/code> as of
revision 12, the LV2 plugins are very similar to, but not 100% compatible with,
the VST originals.  They should sound more or less the same, but some controls
have been changed to be more appropriate for LV2, and a few internal things
have been fixed or improved.

Approach
--------

The missing interface code was written blindly to make the code compile and
work as LV2 plugins.  This internal porting layer, "LVZ", isn't intended for
general use, but might be useful as a starting point for porting similar
plugins to LV2.

The plugin implementations are largely unchanged, aside from cosmetic code
changes and fixes for minor issues.  They should sound more or less the same as
the originals.

Most controls are exposed as VST-style normal floats, exactly like the
originals.  A future major version may rework controls to use "natural" ranges
and units, as is generally expected in LV2.
