---
title: 'GFAKluge: A C++ library and command line utilities for the Graphical Fragment Assembly formats.'
tags:
  - GFA
  - C++
  - genome assembly
  - bioinformatics
authors:
 - name: Eric T. Dawson
   orcid:  0000-0001-5448-1653
   affiliation: "1, 2"
 - name: Richard Durbin
   orcid: 0000-0002-9130-1006
   affiliation: "2, 3"
affiliations:
 - name: Division of Cancer Epidemiology and Genetics, National Cancer Institute, Rockville, MD, USA
   index: 1
 - name: Department of Genetics, University of Cambridge, Cambridge, UK
   index: 2
 - name: Wellcome Trust Sanger Institute, Hinxton, UK
 - index: 3
date: 19 March 2018
bibliography: paper.bib
---

# Summary
GFAKluge [@GFAKluge] is a set of command line utilities and a C++ library for parsing and
manipulating the Graphical Fragment Assembly format. 
Genome assembly algorithms often uses graph structures
to represent relationships between reads during the assembly process. Such information
is typically thrown away when assemblies are converted to FASTA; previous attempts to integrate 
graph information did not gain widespread acceptance largely because
they were difficult for humans to parse. The Graphical Fragment Assembly
(GFA) format was proposed as a way to encode the graph structure of an assembly in a human-readable
text format [@GFAOriginal]. 
GFA also aims to provide a single format for interchange between software for assembly,
visualization, read mapping and variant calling. Such programs are often written in high-performance
programming languages such as C or C++. GFAKluge facilitates interprogram exchange by providing
a high-level C++ API for developers and a set of command line tools for users. We hope the availability of an open-source,
easily extensible API will encourage software developers to consider adding support for GFA to their
bioinformatics programs.

# Command Line Utilities
GFAKluge provides a command line interface for working with GFA. This includes replacements for
common tasks on FASTA assemblies such as calculating assembly N50 or graph statistics. There are also methods for merging
assemblies, reformating files for readability, and converting between legacy GFA formats. A tool for constructing basic variation graphs
from a FASTA file and a VCF file is also included. To our knowledge,
GFAKluge is the only publically-available software package that can consume and produce both GFA1 and GFA2, though many other tools
exist for manipulating one of the GFA formats [@GFA-SPEC]. By allowing interconversion
between the compatible subsets of the formats, the gfak convert tool allows programs that usually can't communicate to share data
without changes to their code. We have used GFAKluge to convert GFA from TwoPaCo [@TwoPaCo] for visualization in Bandage [@Bandage], to calculate assembly
statistics from the Falcon assembler [@Falcon], and to extract FASTA from a vg msga assembly [@vg]. We see the command line utilities as being very useful
to the development community in the short term.

# Integrating GFAKluge into an existing program
As an example of how to use the GFAKluge API, we examine its use in the variation graph toolkit [vg](https://github.com/vgteam/vg) [@vg].
vg creates string graphs from assemblies and population variation that are then used for read mapping and variant calling. We incorporated
GFAKluge for input and output of GFA in vg, replacing an existing parser. Reading in a GFA file requires one line of code and is agnostic to
the GFA version used. Converting from GFA to vg's internal structures and vice versa requires forty source lines of code. Changing output from
GFA v1.0 to GFA v2.0 requires a single API call. This allows vg to take assemblies in GFA format from TwoPaCo and other assembly algorithms.
The gfak command line tools can be used to calculate assembly statistics on assemblies produced by vg.

