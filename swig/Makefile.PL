#!/usr/bin/perl -w
use ExtUtils::MakeMaker;

WriteMakefile(
    NAME => 'cueify',
    AUTHOR => 'Ian Jacobi <pipian@pipian.com>',
    ABSTRACT => 'Perl bindings for libcueify, a CD-metadata library',
    OBJECT => 'cueifyPERL_wrap.o',
    LIBS => ['-lcueify'],
    VERSION => '0.5.0'
);
